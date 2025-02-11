#include "pch.h"
#include "agaliaDecoderWIC.h"

// for Windows Imaging Component
#include <wincodec.h>

// for Direct2D
#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")

#include <atlbase.h>

static HRESULT calculateThumbnailSize(UINT orgW, UINT orgH, UINT maxW, UINT maxH, UINT* thumbWidth, UINT* thumbHeight)
{
	if (!thumbWidth || !thumbHeight) return E_POINTER;
	if (orgH == 0 || orgW == 0) return E_INVALIDARG;

	double scaleH = (maxH == 0) ? 1 : (double)maxH / orgH;
	double scaleW = (maxW == 0) ? 1 : (double)maxW / orgW;
	double scale = min(scaleH, scaleW);
	*thumbWidth = static_cast<UINT>(orgW * scale);
	*thumbHeight = static_cast<UINT>(orgH * scale);

	return S_OK;
}


static HRESULT createHBitmap(UINT width, UINT height, HBITMAP* phBitmap, void** ppBits)
{
	if (!phBitmap || !ppBits) return E_POINTER;

	BITMAPINFOHEADER bmi = {};
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biWidth = width;
	bmi.biHeight = height;
	bmi.biPlanes = 1;
	bmi.biBitCount = 32;
	bmi.biCompression = BI_RGB;

	HDC hMemDC = ::CreateCompatibleDC(NULL);
	if (hMemDC == NULL) return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ::GetLastError());

	void* pBits = nullptr;
	HBITMAP hBitmap = ::CreateDIBSection(hMemDC, reinterpret_cast<BITMAPINFO*>(&bmi), DIB_RGB_COLORS, &pBits, NULL, 0);
	DWORD dwErr = ::GetLastError();

	::DeleteDC(hMemDC);
	if (!hBitmap) return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, dwErr);

	*phBitmap = hBitmap;
	*ppBits = pBits;
	return S_OK;
}


static HRESULT initializeWIC(IStream* stream, IWICImagingFactory** ppFactory, IWICBitmapFrameDecode** ppFrame, IWICFormatConverter** ppConverter)
{
	if (!stream || !ppFactory || !ppFrame || !ppConverter) return E_POINTER;
	if (*ppFactory || *ppFrame || *ppConverter) return E_INVALIDARG;

	CComPtr<IWICImagingFactory> spFactory;
	HRESULT hr = spFactory.CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER);
	if (FAILED(hr)) return hr;

	CComPtr<IWICBitmapDecoder> spDecoder;
	hr = spFactory->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnLoad, &spDecoder);
	if (FAILED(hr)) return hr;

	CComPtr<IWICBitmapFrameDecode> spFrame;
	hr = spDecoder->GetFrame(0, &spFrame);
	if (FAILED(hr)) return hr;

	CComPtr<IWICFormatConverter> spConverter;
	hr = spFactory->CreateFormatConverter(&spConverter);
	if (FAILED(hr)) return hr;

	hr = spConverter->Initialize(spFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
	if (FAILED(hr)) return hr;

	*ppFactory = spFactory.Detach();
	*ppFrame = spFrame.Detach();
	*ppConverter = spConverter.Detach();
	return S_OK;
}


static HRESULT createScaledWICBitmap(IWICImagingFactory* pFactory, IWICFormatConverter* pConverter, UINT thumbWidth, UINT thumbHeight, IWICBitmap** ppScaledWICBitmap)
{
	if (!pFactory || !pConverter || !ppScaledWICBitmap) return E_POINTER;
	if (*ppScaledWICBitmap) return E_INVALIDARG;

	CComPtr<IWICBitmapScaler> spScaler;
	HRESULT hr = pFactory->CreateBitmapScaler(&spScaler);
	if (FAILED(hr)) return hr;

	hr = spScaler->Initialize(pConverter, thumbWidth, thumbHeight, WICBitmapInterpolationModeFant);
	if (FAILED(hr)) return hr;

	hr = pFactory->CreateBitmapFromSource(spScaler, WICBitmapCacheOnLoad, ppScaledWICBitmap);
	return hr;
}


static HRESULT initializeDirect2DResources(IWICBitmap* pWICBitmap, ID2D1RenderTarget** ppRenderTarget)
{
	if (!pWICBitmap || !ppRenderTarget) return E_POINTER;
	if (*ppRenderTarget) return E_INVALIDARG;

	CComPtr<ID2D1Factory> spFactory;
	HRESULT hr = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &spFactory);
	if (FAILED(hr)) return hr;

	D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
	rtProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);

	CComPtr<ID2D1RenderTarget> spRenderTarget;
	hr = spFactory->CreateWicBitmapRenderTarget(pWICBitmap, rtProps, &spRenderTarget);
	if (FAILED(hr)) return hr;

	*ppRenderTarget = spRenderTarget.Detach();
	return S_OK;
}


static HRESULT drawBitmapToRenderTarget(ID2D1RenderTarget* pRenderTarget, IWICBitmap* pWICBitmap, D2D1_COLOR_F color)
{
	if (!pRenderTarget || !pWICBitmap) return E_POINTER;

	CComPtr<ID2D1Bitmap> spD2DBitmap;
	HRESULT hr = pRenderTarget->CreateBitmapFromWicBitmap(pWICBitmap, nullptr, &spD2DBitmap);
	if (FAILED(hr)) return hr;

	D2D1_SIZE_F size = pRenderTarget->GetSize();

	// 上下反転 
	D2D1_MATRIX_3X2_F transform = D2D1::Matrix3x2F::Scale(1.0f, -1.0f, D2D1::Point2F(0.0f, 0.0f));
	transform = transform * D2D1::Matrix3x2F::Translation(0.0f, size.height);

	pRenderTarget->BeginDraw();
	pRenderTarget->SetTransform(transform);
	pRenderTarget->Clear(color);
	pRenderTarget->DrawBitmap(spD2DBitmap, D2D1::RectF(0, 0, size.width, size.height));
	hr = pRenderTarget->EndDraw();

	return hr;
}

static HRESULT generateThumbnailWIC(HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH, IStream* stream)
{
	if (!stream || !phBitmap) return E_POINTER;

	LARGE_INTEGER li = {};
	HRESULT hr = stream->Seek(li, STREAM_SEEK_SET, nullptr);
	if (FAILED(hr)) return hr;

	// WIC のリソースを初期化 
	CComPtr<IWICImagingFactory> spFactory;
	CComPtr<IWICBitmapFrameDecode> spFrameDecode;
	CComPtr<IWICFormatConverter> spConverter;
	hr = initializeWIC(stream, &spFactory, &spFrameDecode, &spConverter);
	if (FAILED(hr)) return hr;

	// 画像のサイズを取得 
	UINT imageWidth = 0, imageHeight = 0;
	hr = spFrameDecode->GetSize(&imageWidth, &imageHeight);
	if (FAILED(hr)) return hr;

	// サムネイルのサイズを計算 
	UINT thumbWidth = 0, thumbHeight = 0;
	hr = calculateThumbnailSize(imageWidth, imageHeight, maxW, maxH, &thumbWidth, &thumbHeight);
	if (FAILED(hr)) return hr;

	// サムネイルサイズにスケーリングした WICBitmap を作成 
	CComPtr<IWICBitmap> spScaledWICBitmap;
	hr = createScaledWICBitmap(spFactory, spConverter, thumbWidth, thumbHeight, &spScaledWICBitmap);
	if (FAILED(hr)) return hr;

	// 合成用の WICBitmap を作成 
	CComPtr<IWICBitmap> spTargetWICBitmap;
	hr = spFactory->CreateBitmap(thumbWidth, thumbHeight, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad, &spTargetWICBitmap);
	if (FAILED(hr)) return hr;

	// 合成用 WICBitmap を D2D の RenderTarget とする 
	CComPtr<ID2D1RenderTarget> spRenderTarget;
	hr = initializeDirect2DResources(spTargetWICBitmap, &spRenderTarget);
	if (FAILED(hr)) return hr;

	// 背景色で塗りつぶした RenderTarget に スケーリングした画像を合成 
	auto bkcolor = ::GetSysColor(COLOR_WINDOW);
	float r = GetRValue(bkcolor) / 255.0f;
	float g = GetGValue(bkcolor) / 255.0f;
	float b = GetBValue(bkcolor) / 255.0f;
	hr = drawBitmapToRenderTarget(spRenderTarget, spScaledWICBitmap, D2D1::ColorF(r, g, b));
	if (FAILED(hr)) return hr;

	// 合成した画像を HBITMAP に変換 
	void* pBits = nullptr;
	HBITMAP hBitmap = NULL;
	hr = createHBitmap(thumbWidth, thumbHeight, &hBitmap, &pBits);
	if (FAILED(hr)) return hr;

	WICRect rcLock = { 0, 0, static_cast<INT>(thumbWidth), static_cast<INT>(thumbHeight) };
	UINT cbStrideWIC = thumbWidth * 4;
	UINT cbBufferSizeWIC = cbStrideWIC * thumbHeight;

	hr = spTargetWICBitmap->CopyPixels(&rcLock, cbStrideWIC, cbBufferSizeWIC, (PBYTE)pBits);

	if (FAILED(hr))
		::DeleteObject(hBitmap);
	else
		*phBitmap = hBitmap;

	return hr;
}

extern "C"
__declspec(dllexport) agaliaDecoder* GetAgaliaDecoder(void)
{
	return new agaliaDecoderWIC();
}


agaliaDecoderWIC::agaliaDecoderWIC()
{
}

agaliaDecoderWIC::~agaliaDecoderWIC()
{
}

ULONG agaliaDecoderWIC::AddRef(void)
{
	return ++refCount;
}

ULONG agaliaDecoderWIC::Release(void)
{
	if (--refCount == 0)
	{
		delete this;
		return 0;
	}
	return refCount;
}

HRESULT agaliaDecoderWIC::decode(IStream* stream, HBITMAP* phBitmap)
{
	return generateThumbnailWIC(phBitmap, 0, 0, stream);
}

HRESULT agaliaDecoderWIC::decode(IStream* stream, agaliaHeap** bmpInfo, void** ppBits)
{
	UNREFERENCED_PARAMETER(stream);
	UNREFERENCED_PARAMETER(bmpInfo);
	UNREFERENCED_PARAMETER(ppBits);

	return E_NOTIMPL;
}

HRESULT agaliaDecoderWIC::decode(IStream* stream, uint32_t maxW, uint32_t maxH, HBITMAP* phBitmap)
{
	return generateThumbnailWIC(phBitmap, maxW, maxH, stream);
}

HRESULT agaliaDecoderWIC::decode(IStream* stream, uint32_t maxW, uint32_t maxH, agaliaHeap** bmpInfo, void** ppBits)
{
	UNREFERENCED_PARAMETER(stream);
	UNREFERENCED_PARAMETER(maxW);
	UNREFERENCED_PARAMETER(maxH);
	UNREFERENCED_PARAMETER(bmpInfo);
	UNREFERENCED_PARAMETER(ppBits);

	return E_NOTIMPL;
}
