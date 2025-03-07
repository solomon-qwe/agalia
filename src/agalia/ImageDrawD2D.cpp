#include "pch.h"
#include "ImageDrawD2D.h"
#include "util.h"
#include "../inc/agaliaUtil.h"

#include <io.h>

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else 
#include <guiddef.h>
#endif

#include <wrl.h>

#include <d2d1_1.h>
#include <d3d11_1.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "d3d11")

using namespace Microsoft::WRL;
using namespace D2D1;


HRESULT CreateDeviceIndependentResources_DXFactory(ID2D1Factory1** d2dFactory, IWICImagingFactory2** wicFactory)
{
	// ID2D1Factory1 
	D2D1_FACTORY_OPTIONS options = {};
#if defined(_DEBUG)
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
	auto hr = ::D2D1CreateFactory<ID2D1Factory1>(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		options,
		d2dFactory);
	if (FAILED(hr)) return hr;

	// IWICImagingFactory2 
	hr = ::CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(wicFactory));
	if (FAILED(hr)) return hr;

	return S_OK;
}

HRESULT CreateDeviceIndependentResources_HBITMAP(IWICImagingFactory2* wicFactory, IWICFormatConverter** wicFormatConverter, IWICColorContext** wicColorContext, HBITMAP hBitmap)
{
	ComPtr<IWICBitmap> wicBitmap;
	auto hr = wicFactory->CreateBitmapFromHBITMAP(hBitmap, nullptr, WICBitmapIgnoreAlpha, &wicBitmap);
	if (FAILED(hr)) return hr;

	hr = wicFactory->CreateColorContext(wicColorContext);
	if (FAILED(hr)) return hr;

	// use default color space if color profile is empty 
	const unsigned int ExifColorSpaceSRGB = 1;
	hr = (*wicColorContext)->InitializeFromExifColorSpace(ExifColorSpaceSRGB);
	if (FAILED(hr)) return hr;

	hr = wicFactory->CreateFormatConverter(wicFormatConverter);
	if (FAILED(hr)) return hr;

	hr = (*wicFormatConverter)->Initialize(
		wicBitmap.Get(),
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0f,
		WICBitmapPaletteTypeCustom
	);
	if (FAILED(hr)) return hr;

	return S_OK;
}



HRESULT CreateDeviceIndependentResources_Image(IWICImagingFactory2* wicFactory, IWICFormatConverter** wicFormatConverter, IWICColorContext** wicColorContext, IStream* stream)
{
	ComPtr<IWICBitmapDecoder> decoder;
	auto hr = wicFactory->CreateDecoderFromStream(
		stream,
		nullptr,
		WICDecodeMetadataCacheOnDemand,
		&decoder);
	if (FAILED(hr)) return hr;

	ComPtr<IWICBitmapFrameDecode> frame;
	hr = decoder->GetFrame(0, &frame);
	if (FAILED(hr)) return hr;

	hr = wicFactory->CreateColorContext(wicColorContext);
	if (FAILED(hr)) return hr;

	unsigned int actualCount = 0;
	hr = frame->GetColorContexts(1, wicColorContext, &actualCount);
	if (FAILED(hr)) return hr;
	if (actualCount == 0)
	{
		// use default color space if color profile is empty 
		const unsigned int ExifColorSpaceSRGB = 1;
		hr = (*wicColorContext)->InitializeFromExifColorSpace(ExifColorSpaceSRGB);
		if (FAILED(hr)) return hr;
	}

	hr = wicFactory->CreateFormatConverter(wicFormatConverter);
	if (FAILED(hr)) return hr;

	hr = (*wicFormatConverter)->Initialize(
		frame.Get(),
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0f,
		WICBitmapPaletteTypeCustom
	);
	if (FAILED(hr)) return hr;

	return S_OK;
}


inline bool SdkLayersAvailable()
{
	auto hr = ::D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_NULL,
		0,
		D3D11_CREATE_DEVICE_DEBUG,
		nullptr, 0,
		D3D11_SDK_VERSION,
		nullptr, nullptr, nullptr);

	return SUCCEEDED(hr);
}


HRESULT CreateD3DDevice(D3D_DRIVER_TYPE type, ID3D11Device** device, ID3D11DeviceContext** context)
{
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
	if (SdkLayersAvailable())
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	auto hr = ::D3D11CreateDevice(
		nullptr,
		type,
		0,
		creationFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		device,
		nullptr,
		context);
	return hr;
}


HRESULT CreateDeviceResources_DX(ID2D1Factory1* d2dFactory, ComPtr<ID2D1Device>& d2dDevice, ComPtr<ID2D1DeviceContext>& d2dContext, ComPtr<ID3D11Device1>& d3dDevice, ComPtr<ID3D11DeviceContext>& d3dContext)
{
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	auto hr = CreateD3DDevice(D3D_DRIVER_TYPE_HARDWARE, &device, &context);
	if (FAILED(hr))
		hr = CreateD3DDevice(D3D_DRIVER_TYPE_WARP, &device, &context);
	if (FAILED(hr)) return hr;

	hr = device.As(&d3dDevice);
	if (FAILED(hr)) return hr;
	hr = context.As(&d3dContext);
	if (FAILED(hr)) return hr;

	ComPtr<IDXGIDevice> dxgiDevice;
	hr = d3dDevice.As(&dxgiDevice);
	if (FAILED(hr)) return hr;

	hr = d2dFactory->CreateDevice(dxgiDevice.Get(), &d2dDevice);
	if (FAILED(hr)) return hr;

	hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);
	if (FAILED(hr)) return hr;

	return S_OK;
}


HRESULT CreateDeviceResources_Image(ID2D1Effect** displayEffect, ID2D1Effect** sourceEffect, ID2D1DeviceContext* d2dContext, IWICFormatConverter* wicFormatConverter, IWICColorContext* wicColorContext, int colorManagementMode)
{
	if (d2dContext == nullptr) return E_POINTER;
	if (wicColorContext == nullptr) return E_POINTER;

	ComPtr<ID2D1ColorContext> imageColorContext;
	auto hr = d2dContext->CreateColorContextFromWicColorContext(wicColorContext, &imageColorContext);
	if (FAILED(hr)) return hr;

	// source effect 
	hr = d2dContext->CreateEffect(CLSID_D2D1BitmapSource, sourceEffect);
	if (FAILED(hr)) return hr;
	hr = (*sourceEffect)->SetValue(D2D1_BITMAPSOURCE_PROP_WIC_BITMAP_SOURCE, wicFormatConverter);
	if (FAILED(hr)) return hr;
	hr = (*sourceEffect)->SetValue(D2D1_BITMAPSOURCE_PROP_INTERPOLATION_MODE, D2D1_BITMAPSOURCE_INTERPOLATION_MODE_LINEAR);
	if (FAILED(hr)) return hr;

	// destination effect 
	hr = d2dContext->CreateEffect(CLSID_D2D1ColorManagement, displayEffect);
	if (FAILED(hr)) return hr;
	if (colorManagementMode) {
		hr = (*displayEffect)->SetValue(D2D1_COLORMANAGEMENT_PROP_SOURCE_COLOR_CONTEXT, imageColorContext.Get());
		if (FAILED(hr)) return hr;
	}
	hr = (*displayEffect)->SetValue(D2D1_COLORMANAGEMENT_PROP_ALPHA_MODE, D2D1_COLORMANAGEMENT_ALPHA_MODE_STRAIGHT);
	if (FAILED(hr)) return hr;
	(*displayEffect)->SetInputEffect(0, *sourceEffect);

	return S_OK;
}


HRESULT CreateSwapChain(const ComPtr<ID3D11Device1>& d3dDevice, IDXGISwapChain1** ppSwapChain, HWND hwnd, DXGI_SCALING scaling)
{
	ComPtr<IDXGIDevice1> dxgiDevice;
	auto hr = d3dDevice.As(&dxgiDevice);
	if (FAILED(hr)) return hr;

	ComPtr<IDXGIAdapter> dxgiAdapter;
	hr = dxgiDevice->GetAdapter(&dxgiAdapter);
	if (FAILED(hr)) return hr;

	ComPtr<IDXGIFactory2> dxgiFactory;
	hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr)) return hr;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = scaling;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

	hr = dxgiFactory->CreateSwapChainForHwnd(
		d3dDevice.Get(),
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		ppSwapChain);

	return hr;
}


HRESULT CreateWindowSizeDependentResources(IDXGISwapChain1** swapChain, ID2D1Bitmap1** d2dTarget, ID3D11Device1* d3dDevice, ID2D1DeviceContext* d2dContext, HWND hwnd)
{
	if (*swapChain != nullptr)
	{
		auto hr = (*swapChain)->ResizeBuffers(2, 0, 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
		if (FAILED(hr)) return hr;
	}
	else
	{
		auto hr = CreateSwapChain(d3dDevice, swapChain, hwnd, DXGI_SCALING_NONE);	// for Win8 or Later 
		if (hr == DXGI_ERROR_INVALID_CALL) {
			hr = CreateSwapChain(d3dDevice, swapChain, hwnd, DXGI_SCALING_STRETCH);	// for Win7 
		}
		if (FAILED(hr)) return hr;
	}

	D2D1_BITMAP_PROPERTIES1 bitmapProperties = BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

	ComPtr<IDXGISurface> dxgiBackBuffer;
	auto hr = (*swapChain)->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
	if (FAILED(hr)) return hr;

	hr = d2dContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer.Get(), &bitmapProperties, d2dTarget);
	if (FAILED(hr)) return hr;
	d2dContext->SetTarget(*d2dTarget);

	return S_OK;
}


HRESULT UpdateForWindowSizeChange(IDXGISwapChain1** swapChain, ID2D1Bitmap1** d2dTarget, ID3D11Device1* d3dDevice, ID2D1DeviceContext* d2dContext, HWND hwnd)
{
	d2dContext->SetTarget(nullptr);
	if (*d2dTarget) {
		(*d2dTarget)->Release();
		*d2dTarget = nullptr;
	}
	return CreateWindowSizeDependentResources(swapChain, d2dTarget, d3dDevice, d2dContext, hwnd);
}


HRESULT Render(IDXGISwapChain1* swapChain, ID2D1DeviceContext* d2dContext, ID2D1Effect* displayEffect, DWORD bkcolor, const D2D1_POINT_2F& target_offset, const D2D1_RECT_F& image_rect, FLOAT scale)
{
	d2dContext->BeginDraw();

	// fill with background color 
	D2D1_COLOR_F oBKColor = D2D1::ColorF(bkcolor);
	d2dContext->Clear(oBKColor);

	if (displayEffect)
	{
		ComPtr<ID2D1Effect> scaleEffect;
		d2dContext->CreateEffect(CLSID_D2D1Scale, &scaleEffect);
		scaleEffect->SetInputEffect(0, displayEffect);
		scaleEffect->SetValue(D2D1_SCALE_PROP_CENTER_POINT, D2D1::Vector2F(0, 0));
		scaleEffect->SetValue(D2D1_SCALE_PROP_SCALE, D2D1::Vector2F(scale, scale));

		d2dContext->DrawImage(scaleEffect.Get(), target_offset, image_rect);
	}

	auto hr = d2dContext->EndDraw();
	if (FAILED(hr))
		if (hr != D2DERR_RECREATE_TARGET)
			return hr;

	DXGI_PRESENT_PARAMETERS parameters = {};
	hr = swapChain->Present1(1, 0, &parameters);
	if (FAILED(hr)) return hr;

	return S_OK;
}


HRESULT UpdateDisplayColorContext(ID2D1Effect* displayEffect, ID2D1DeviceContext* d2dContext, HWND hwnd, int mode)
{
	if (!displayEffect)
		return S_FALSE;

	HRESULT hr;
	ComPtr<ID2D1ColorContext> displayColorContext;

	if (mode == 2)	// mode: system
	{
		DWORD size = MAX_PATH;
		TCHAR path[MAX_PATH] = {};
		hr = GetMonitorColorProfilePath(hwnd, &size, path);
		if (SUCCEEDED(hr))
		{
			CHeapPtr<BYTE> buf;
			long file_size = 0;
			hr = LoadFile(buf, &file_size, path);
			if (SUCCEEDED(hr))
			{
				// specified color space 
				hr = d2dContext->CreateColorContext(D2D1_COLOR_SPACE_CUSTOM, buf, file_size, &displayColorContext);
			}
		}

		if (!displayColorContext)
		{
			// default color space 
			hr = d2dContext->CreateColorContext(D2D1_COLOR_SPACE_SRGB, nullptr, 0, &displayColorContext);
			if (FAILED(hr)) return hr;
		}
	}
	else if (mode == 1)	// sRGB 
	{
		hr = d2dContext->CreateColorContext(D2D1_COLOR_SPACE_SRGB, nullptr, 0, &displayColorContext);
		if (FAILED(hr)) return hr;
	}

	// set destination color space 
	hr = displayEffect->SetValue(D2D1_COLORMANAGEMENT_PROP_DESTINATION_COLOR_CONTEXT, displayColorContext.Get());
	if (FAILED(hr)) return hr;

	return S_OK;
}

HRESULT UpdateImagePosition(IWICFormatConverter* wicFormatConverter, ID2D1DeviceContext* d2dContext, D2D1_POINT_2F* offset, D2D1_RECT_F* image_rect, D2D1_POINT_2F* target_offset, FLOAT scale)
{
	if (!wicFormatConverter) return E_POINTER;
	if (!d2dContext) return E_POINTER;
		
	// get souce image size 
	UINT w = 0, h = 0;
	wicFormatConverter->GetSize(&w, &h);
	D2D1_SIZE_F image_size = { (FLOAT)w * scale, (FLOAT)h * scale };

	D2D1_SIZE_F target_size = d2dContext->GetSize();

	FLOAT dstW = min(image_size.width, target_size.width);
	FLOAT dstH = min(image_size.height, target_size.height);
	FLOAT dstX = max(0, (target_size.width - image_size.width) / 2);
	FLOAT dstY = max(0, (target_size.height - image_size.height) / 2);

	FLOAT srcX = max(0, min(-offset->x, image_size.width - dstW));
	FLOAT srcY = max(0, min(-offset->y, image_size.height - dstH));

	*image_rect = D2D1::RectF(srcX, srcY, image_size.width, image_size.height);
	*target_offset = D2D1::Point2F(dstX, dstY);
	offset->x = -srcX;
	offset->y = -srcY;

	return S_OK;
}


// ImageDrawD2D class implementation 

struct InternalImageDrawD2DParam
{
	// Image 
	ComPtr<IWICColorContext>    wicColorContext;
	ComPtr<IWICFormatConverter> wicFormatConverter;
	ComPtr<ID2D1Effect>         sourceEffect;
	ComPtr<ID2D1Effect>         displayEffect;

	// DX
	ComPtr<ID2D1Factory1>       d2dFactory;
	ComPtr<IWICImagingFactory2> wicFactory;

	ComPtr<ID2D1Device>         d2dDevice;
	ComPtr<ID2D1DeviceContext>  d2dContext;
	ComPtr<ID3D11Device1>       d3dDevice;
	ComPtr<ID3D11DeviceContext> d3dContext;

	ComPtr<ID2D1Bitmap1>        d2dTarget;
	ComPtr<IDXGISwapChain1>     swapChain;

	D2D1_POINT_2F offset = {};
	FLOAT scale = 1.f;

	D2D1_POINT_2F target_offset = {};
	D2D1_RECT_F image_rect = {};

	void DeleteFactory(void)
	{
		d2dFactory = nullptr;
		wicFactory = nullptr;
	}

	void DeleteContents(void)
	{
		wicColorContext = nullptr;
		wicFormatConverter = nullptr;
		sourceEffect = nullptr;
		displayEffect = nullptr;
		d2dDevice = nullptr;
		d2dContext = nullptr;
		d2dTarget = nullptr;
		d3dDevice = nullptr;
		d3dContext = nullptr;
		swapChain = nullptr;
	}

	void DeleteAll(void)
	{
		DeleteContents();
		DeleteFactory();
	}
};


void ImageDrawD2D::attach(HWND hwndOuter)
{
	if (!_p)
	{
		auto hr = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (FAILED(hr)) return;

		_p = new InternalImageDrawD2DParam;
	}

	if (this->hwnd)
	{
		if (this->hwnd == hwndOuter) return;
		_p->DeleteAll();
	}
	this->hwnd = hwndOuter;

	auto hr = ::CreateDeviceIndependentResources_DXFactory(_p->d2dFactory.GetAddressOf(), _p->wicFactory.GetAddressOf());
	if (FAILED(hr)) return;
}


void ImageDrawD2D::detach(void)
{
	if (!_p) return;

	_p->DeleteAll();
	delete _p;
	_p = nullptr;

	hwnd = NULL;

	::CoUninitialize();
}


#include "../inc/agaliarept.h"

HRESULT ImageDrawD2D::reset_content(agaliaContainer* image, int colorManagementMode)
{
	if (!_p) return E_FAIL;

	_p->DeleteContents();

	auto hr = ::CreateDeviceResources_DX(_p->d2dFactory.Get(), _p->d2dDevice, _p->d2dContext, _p->d3dDevice, _p->d3dContext);
	if (FAILED(hr)) return hr;

	hr = update_for_window_size_change();
	if (FAILED(hr)) return hr;

	CComPtr<IStream> stream;
	if (image) {
		image->getAsocStream(&stream);
		LARGE_INTEGER li = {};
		hr = image->LockStream();
		if (FAILED(hr)) return hr;
		stream->Seek(li, STREAM_SEEK_SET, nullptr);
		image->UnlockStream();
	}
	if (stream == nullptr) return S_OK;

	hr = ::CreateDeviceIndependentResources_Image(_p->wicFactory.Get(), _p->wicFormatConverter.GetAddressOf(), _p->wicColorContext.GetAddressOf(), stream);
	if (FAILED(hr))
	{
		if (!image) return hr;

		agaliaPtr<agaliaBitmap> bitmap;
		hr = image->loadBitmap(&bitmap);
		if (FAILED(hr)) return hr;

		HBITMAP hBitmap = NULL;
		hr = bitmap->getHBitmap(&hBitmap);
		if (FAILED(hr)) return hr;

		hr = ::CreateDeviceIndependentResources_HBITMAP(_p->wicFactory.Get(), _p->wicFormatConverter.GetAddressOf(), _p->wicColorContext.GetAddressOf(), hBitmap);
		if (FAILED(hr)) return hr;
	}

	hr = ::CreateDeviceResources_Image(_p->displayEffect.GetAddressOf(), _p->sourceEffect.GetAddressOf(), _p->d2dContext.Get(), _p->wicFormatConverter.Get(), _p->wicColorContext.Get(), colorManagementMode);
	if (FAILED(hr)) return hr;

	hr = ::UpdateDisplayColorContext(_p->displayEffect.Get(), _p->d2dContext.Get(), hwnd, colorManagementMode);
	if (FAILED(hr)) return hr;

	hr = ::UpdateImagePosition(_p->wicFormatConverter.Get(), _p->d2dContext.Get(), &_p->offset, &_p->image_rect, &_p->target_offset, _p->scale);
	if (FAILED(hr)) return hr;

	return S_OK;
}


HRESULT ImageDrawD2D::reset_color_profile(int colorManagementMode)
{
	if (!_p) return E_FAIL;

	_p->displayEffect = nullptr;
	_p->sourceEffect = nullptr;

	auto hr = ::CreateDeviceResources_Image(_p->displayEffect.GetAddressOf(), _p->sourceEffect.GetAddressOf(), _p->d2dContext.Get(), _p->wicFormatConverter.Get(), _p->wicColorContext.Get(), colorManagementMode);
	if (FAILED(hr)) return hr;

	hr = ::UpdateDisplayColorContext(_p->displayEffect.Get(), _p->d2dContext.Get(), hwnd, colorManagementMode);
	if (FAILED(hr)) return hr;

	return S_OK;
}


HRESULT ImageDrawD2D::update_for_window_size_change(void)
{
	if (!_p) return E_FAIL;

	auto hr = ::UpdateForWindowSizeChange(_p->swapChain.GetAddressOf(), _p->d2dTarget.GetAddressOf(), _p->d3dDevice.Get(), _p->d2dContext.Get(), hwnd);
	if (FAILED(hr)) return hr;

	if (_p->wicFormatConverter.Get() && _p->d2dContext.Get())
	{
		hr = ::UpdateImagePosition(_p->wicFormatConverter.Get(), _p->d2dContext.Get(), &_p->offset, &_p->image_rect, &_p->target_offset, _p->scale);
		if (FAILED(hr)) return E_FAIL;
	}

	return hr;
}


HRESULT ImageDrawD2D::render(DWORD bkcolor)
{
	if (!_p) return E_FAIL;

	return ::Render(_p->swapChain.Get(), _p->d2dContext.Get(), _p->displayEffect.Get(), bkcolor, _p->target_offset, _p->image_rect, _p->scale);
}


HRESULT ImageDrawD2D::offset(int x, int y)
{
	if (!_p) return E_FAIL;

	_p->offset.x += x;
	_p->offset.y += y;

	if (_p->wicFormatConverter.Get() && _p->d2dContext.Get())
	{
		auto hr = ::UpdateImagePosition(_p->wicFormatConverter.Get(), _p->d2dContext.Get(), &_p->offset, &_p->image_rect, &_p->target_offset, _p->scale);
		if (FAILED(hr)) return E_FAIL;
	}

	return S_OK;
}


HRESULT ImageDrawD2D::set_scale(float scale)
{
	if (!_p) return E_FAIL;
	_p->scale = scale;
	return S_OK;
}
