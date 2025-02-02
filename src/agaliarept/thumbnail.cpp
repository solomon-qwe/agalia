#include "pch.h"
#include "thumbnail.h"
#include "../inc/agaliarept.h"

#include <atlbase.h>

// for Media Foundation 
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

// for Windows Imaging Component
#include <wincodec.h>

// for Direct2D
#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")


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


static HRESULT initializeMFSourceReader(IStream* stream, IMFSourceReader** ppSourceReader)
{
	if (!stream || !ppSourceReader) return E_POINTER;
	if (*ppSourceReader) return E_INVALIDARG;

	CComPtr<IMFAttributes> spAttributes;
	HRESULT hr = ::MFCreateAttributes(&spAttributes, 1);
	if (FAILED(hr)) return hr;

	hr = spAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
	if (FAILED(hr)) return hr;

	CComPtr<IMFByteStream> spByteStream;
	hr = ::MFCreateMFByteStreamOnStream(stream, &spByteStream);
	if (FAILED(hr)) return hr;

	hr = ::MFCreateSourceReaderFromByteStream(spByteStream, spAttributes, ppSourceReader);
	return hr;
}


static HRESULT getFirstVideoSample(IMFSourceReader* pSourceReader, IMFSample** ppSample)
{
	if (!pSourceReader || !ppSample) return E_POINTER;
	if (*ppSample) return E_INVALIDARG;

	CComPtr<IMFMediaType> spMediaType;
	HRESULT hr = ::MFCreateMediaType(&spMediaType);
	if (FAILED(hr)) return hr;

	hr = spMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	if (FAILED(hr)) return hr;

	hr = spMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
	if (FAILED(hr)) return hr;

	hr = pSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, spMediaType);
	if (FAILED(hr)) return hr;

	DWORD flags = 0;
	hr = pSourceReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, &flags, NULL, ppSample);
	return S_OK;
}


static HRESULT setupVideoProcessor(IMFSourceReader* pSourceReader, UINT32 thumbWidth, UINT32 thumbHeight, IMFSample* pSample, IMFTransform** ppVideoProcessor)
{
	if (!pSourceReader || !pSample || !ppVideoProcessor) return E_POINTER;
	if (*ppVideoProcessor) return E_INVALIDARG;

	CComPtr<IMFMediaType> spMediaType;
	HRESULT hr = pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &spMediaType);
	if (FAILED(hr)) return hr;

	CComPtr<IMFTransform> spVideoProcessor;
	hr = spVideoProcessor.CoCreateInstance(CLSID_VideoProcessorMFT, nullptr, CLSCTX_INPROC_SERVER);
	if (FAILED(hr)) return hr;

	hr = spVideoProcessor->SetInputType(0, spMediaType, 0);
	if (FAILED(hr)) return hr;

	CComPtr<IMFMediaType> spOutputType;
	hr = ::MFCreateMediaType(&spOutputType);
	if (FAILED(hr)) return hr;

	hr = spOutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	if (FAILED(hr)) return hr;

	hr = spOutputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
	if (FAILED(hr)) return hr;

	hr = ::MFSetAttributeSize(spOutputType, MF_MT_FRAME_SIZE, thumbWidth, thumbHeight);
	if (FAILED(hr)) return hr;

	hr = spVideoProcessor->SetOutputType(0, spOutputType, 0);
	if (FAILED(hr)) return hr;

	hr = spVideoProcessor->ProcessInput(0, pSample, 0);
	if (FAILED(hr)) return hr;

	*ppVideoProcessor = spVideoProcessor.Detach();
	return S_OK;
}


static HRESULT processVideoFrame(IMFTransform* pVideoProcessor, UINT32 thumbWidth, UINT32 thumbHeight, IMFMediaBuffer** ppOutputBuffer)
{
	if (!pVideoProcessor || !ppOutputBuffer) return E_POINTER;
	if (*ppOutputBuffer) return E_INVALIDARG;

	CComPtr<IMFSample> spOutputSample;
	HRESULT hr = ::MFCreateSample(&spOutputSample);
	if (FAILED(hr)) return hr;

	CComPtr<IMFMediaBuffer> spOutputBuffer;
	hr = ::MFCreateMemoryBuffer(thumbWidth * thumbHeight * 4, &spOutputBuffer);
	if (FAILED(hr)) return hr;

	hr = spOutputSample->AddBuffer(spOutputBuffer);
	if (FAILED(hr)) return hr;

	MFT_OUTPUT_DATA_BUFFER outputDataBuffer = { 0 };
	outputDataBuffer.pSample = spOutputSample;

	DWORD processOutputStatus = 0;
	hr = pVideoProcessor->ProcessOutput(0, 1, &outputDataBuffer, &processOutputStatus);
	if (FAILED(hr)) return hr;

	// WMVやAVIをロードするとアルファチャンネルが0クリア（透過）されてしまうので0xFF（不透過）に設定する 
	BYTE* p = nullptr;
	DWORD cb = 0;
	spOutputBuffer->Lock(&p, nullptr, &cb);
	for (DWORD i = 3; i < cb; i += 4)
		p[i] = 0xFF;
	spOutputBuffer->Unlock();

	*ppOutputBuffer = spOutputBuffer.Detach();
	return S_OK;
}


static HRESULT generateThumbnailMF(HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH, IStream* stream)
{
	if (!stream || !phBitmap) return E_POINTER;

	CComPtr<IMFSourceReader> spSourceReader;
	HRESULT hr = initializeMFSourceReader(stream, &spSourceReader);
	if (FAILED(hr)) return hr;

	CComPtr<IMFSample> spSample;
	hr = getFirstVideoSample(spSourceReader, &spSample);
	if (FAILED(hr)) return hr;

	CComPtr<IMFMediaType> spMediaType;
	hr = spSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &spMediaType);
	if (FAILED(hr)) return hr;

	UINT32 width = 0, height = 0;
	hr = ::MFGetAttributeSize(spMediaType, MF_MT_FRAME_SIZE, &width, &height);
	if (FAILED(hr)) return hr;

	UINT32 thumbWidth = 0, thumbHeight = 0;
	hr = calculateThumbnailSize(width, height, maxW, maxH, &thumbWidth, &thumbHeight);
	if (FAILED(hr)) return hr;

	CComPtr<IMFTransform> spVideoProcessor;
	hr = setupVideoProcessor(spSourceReader, thumbWidth, thumbHeight, spSample, &spVideoProcessor);
	if (FAILED(hr)) return hr;

	CComPtr<IMFMediaBuffer> spMediaBuffer;
	hr = processVideoFrame(spVideoProcessor, thumbWidth, thumbHeight, &spMediaBuffer);
	if (FAILED(hr)) return hr;

	BYTE* pBuffer = nullptr;
	DWORD cbBuffer = 0;
	hr = spMediaBuffer->Lock(&pBuffer, nullptr, &cbBuffer);
	if (FAILED(hr)) return hr;

	void* pBits = nullptr;
	hr = createHBitmap(thumbWidth, thumbHeight, phBitmap, &pBits);
	if (SUCCEEDED(hr)) {
		memcpy(pBits, pBuffer, cbBuffer);
	}

	spMediaBuffer->Unlock();

	return hr;
}





HRESULT loadThumbnailBitmap(HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH, IStream* stream)
{
	if (!stream || !phBitmap) return E_POINTER;

	HRESULT hr = ::MFStartup(MF_VERSION);
	if (SUCCEEDED(hr))
	{
		hr = generateThumbnailMF(phBitmap, maxW, maxH, stream);
		::MFShutdown();
	}

	if (FAILED(hr))
	{
		hr = generateThumbnailWIC(phBitmap, maxW, maxH, stream);
	}

	return hr;
}
