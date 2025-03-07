// ChildGraphicViewD2D1_3.cpp : implementation file
//

#include "pch.h"
#include "agalia.h"
#include "ChildGraphicViewD2D1_3.h"
#include "util.h"

#include "../inc/agaliarept.h"

#include <atlbase.h>	// for CHeapPtr 

#define INITGUID
#include <guiddef.h>
#include <d2d1_3.h>
#undef INITGUID

#include <shellapi.h>

#include <d3d11.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d2d1.lib")

using Microsoft::WRL::ComPtr;

#include <vector>
#include <io.h>


static HRESULT GetMonitorColorContext(ID2D1ColorContext** monitorColorContext, ID2D1DeviceContext* d2dContext, HWND hwnd, int mode)
{
    HRESULT hr;

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
                hr = d2dContext->CreateColorContext(D2D1_COLOR_SPACE_CUSTOM, buf, file_size, monitorColorContext);
            }
        }

        if (!*monitorColorContext)
        {
            // default color space 
            hr = d2dContext->CreateColorContext(D2D1_COLOR_SPACE_SRGB, nullptr, 0, monitorColorContext);
            if (FAILED(hr)) return hr;
        }
    }
    else if (mode == 1)	// sRGB 
    {
        hr = d2dContext->CreateColorContext(D2D1_COLOR_SPACE_SRGB, nullptr, 0, monitorColorContext);
        if (FAILED(hr)) return hr;
    }

    return S_OK;
}


struct d2d_resouce
{
	ComPtr<ID3D11Device> d3dDevice;
	ComPtr<IDXGISwapChain4> swapChain;
	ComPtr<ID2D1Factory3> d2dFactory;
	ComPtr<ID2D1Device> d2dDevice;
	ComPtr<ID2D1DeviceContext5> d2dContext;
	ComPtr<ID2D1Bitmap1> targetBitmap;
	ComPtr<ID2D1Effect> colorManageEffect;
	ComPtr<ID2D1Effect> displayEffect;
	ComPtr<ID2D1ColorContext> imageColorContext;
};

class D2D1_3_Renderer
{
public:

    explicit D2D1_3_Renderer(HWND hwnd)
        : m_hwnd(hwnd)
    {
		res = new d2d_resouce;
    }

    virtual ~D2D1_3_Renderer()
    {
        delete res;
		res = nullptr;
    }

    HRESULT init(void);
    HRESULT resize(void);
    HRESULT render(void);
    HRESULT reset_color_profile(int colorManagementMode);
    HRESULT update_white_level(void);
    HRESULT set_source_image(IWICBitmap* pBitmap, IWICColorContext* pColorContext);
	HRESULT set_offset(int x, int y);
	HRESULT set_scale(float s);
	HRESULT update_image_position(void);

private:
    HWND m_hwnd;
    D2D1_SIZE_F image_size = {};
    float color_scale = 1.0F;
    ULONG SDRWhiteLevel = 1000;

    D2D1_POINT_2F offset = {};
    FLOAT scale = 1.f;
    D2D1_POINT_2F target_offset = {};
    D2D1_RECT_F display_rect = {};

	d2d_resouce* res = nullptr;

    const DXGI_FORMAT surfacePixelFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
};

HRESULT D2D1_3_Renderer::init(void)
{
    HRESULT hr = S_OK;

    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    hr = ::D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &res->d3dDevice,
        nullptr,
        nullptr);
    if (FAILED(hr)) return hr;

    ComPtr<IDXGIDevice1> dxgiDevice;
    hr = res->d3dDevice.As(&dxgiDevice);
    if (FAILED(hr)) return hr;

    dxgiDevice->SetMaximumFrameLatency(1);

    D2D1_FACTORY_OPTIONS options = {};
#if defined(_DEBUG)
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
    hr = ::D2D1CreateFactory<ID2D1Factory3>(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &res->d2dFactory);
    if (FAILED(hr)) return hr;

    hr = res->d2dFactory->CreateDevice(dxgiDevice.Get(), &res->d2dDevice);
    if (FAILED(hr)) return hr;

    ComPtr<ID2D1DeviceContext> d2dDeviceContext;
    hr = res->d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dDeviceContext);
    if (FAILED(hr)) return hr;

    hr = d2dDeviceContext.As(&res->d2dContext);
    if (FAILED(hr)) return hr;

    ComPtr<IDXGIFactory2> dxgiFactory;
    {
        ComPtr<IDXGIAdapter> adapter;
        hr = dxgiDevice->GetAdapter(&adapter);
        if (FAILED(hr)) return hr;

        hr = adapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(dxgiFactory.GetAddressOf()));
        if (FAILED(hr)) return hr;
    }

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Format = surfacePixelFormat;
    swapChainDesc.Stereo = FALSE;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Scaling = DXGI_SCALING_NONE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

    ComPtr<IDXGISwapChain1> swapChain1;
    hr = dxgiFactory->CreateSwapChainForHwnd(
        res->d3dDevice.Get(),
        m_hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1
    );
    if (FAILED(hr)) return hr;

    hr = swapChain1.As(&res->swapChain);
    if (FAILED(hr)) return hr;

    hr = res->swapChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709);
    if (FAILED(hr)) return hr;

    {
        hr = res->d2dContext->CreateEffect(CLSID_D2D1ColorManagement, &res->colorManageEffect);
        if (FAILED(hr)) return hr;
        hr = res->colorManageEffect->SetValue(D2D1_COLORMANAGEMENT_PROP_QUALITY, D2D1_COLORMANAGEMENT_QUALITY_BEST);
        if (FAILED(hr)) return hr;
        hr = res->colorManageEffect->SetValue(D2D1_COLORMANAGEMENT_PROP_ALPHA_MODE, D2D1_COLORMANAGEMENT_ALPHA_MODE_STRAIGHT);
        if (FAILED(hr)) return hr;
    }

    {
        hr = res->d2dContext->CreateEffect(CLSID_D2D1ColorManagement, &res->displayEffect);
        if (FAILED(hr)) return hr;
        hr = res->displayEffect->SetValue(D2D1_COLORMANAGEMENT_PROP_QUALITY, D2D1_COLORMANAGEMENT_QUALITY_BEST);
        if (FAILED(hr)) return hr;
        hr = res->displayEffect->SetValue(D2D1_COLORMANAGEMENT_PROP_ALPHA_MODE, D2D1_COLORMANAGEMENT_ALPHA_MODE_STRAIGHT);
        if (FAILED(hr)) return hr;
    }

    resize();

    return S_OK;
}

HRESULT D2D1_3_Renderer::resize(void)
{
    if (res->d2dContext == nullptr) return E_FAIL;
    if (res->swapChain == nullptr) return E_FAIL;

    HRESULT hr = S_OK;

    D2D1_BITMAP_PROPERTIES1 bitmapProperties =
        D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(surfacePixelFormat, D2D1_ALPHA_MODE_IGNORE));

    RECT rc = {};
    GetClientRect(m_hwnd, &rc);
    UINT cx = rc.right - rc.left;
    UINT cy = rc.bottom - rc.top;

    D2D1_SIZE_F target_size = res->d2dContext->GetSize();
    if (target_size.width == cx && target_size.height == cy)
        return S_FALSE;

    res->d2dContext->SetTarget(nullptr);
    res->targetBitmap.Reset();

    hr = res->swapChain->ResizeBuffers(0, cx, cy, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) return hr;

    ComPtr<IDXGISurface> dxgiBackBuffer;
    hr = res->swapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(dxgiBackBuffer.GetAddressOf()));
    if (FAILED(hr)) return hr;

    hr = res->d2dContext->CreateBitmapFromDxgiSurface(
        dxgiBackBuffer.Get(),
        &bitmapProperties,
        &res->targetBitmap);
    if (FAILED(hr)) return hr;

    res->d2dContext->SetTarget(res->targetBitmap.Get());

    return S_OK;
}


HRESULT D2D1_3_Renderer::set_source_image(IWICBitmap* pBitmap, IWICColorContext* pColorContext)
{
    image_size.width = 0;
    image_size.height = 0;

    if (res->colorManageEffect == nullptr) return E_FAIL;
    res->colorManageEffect->SetInputEffect(0, nullptr);

    if (pBitmap == nullptr) return E_POINTER;
	if (res->d2dContext == nullptr) return E_FAIL;

    HRESULT hr = S_OK;

    UINT w = 0, h = 0;
	hr = pBitmap->GetSize(&w, &h);
    if (FAILED(hr)) return hr;
    image_size.width = static_cast<FLOAT>(w);
    image_size.height = static_cast<FLOAT>(h);

    if (pColorContext)
    {
        hr = res->d2dContext->CreateColorContextFromWicColorContext(pColorContext, &res->imageColorContext);
        if (FAILED(hr)) return hr;
    }
    else
    {
		WICPixelFormatGUID pixelFormatGUID;
		hr = pBitmap->GetPixelFormat(&pixelFormatGUID);
		if (FAILED(hr)) return hr;

        ComPtr<IWICImagingFactory> wicFactory;
        hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&wicFactory));
        if (FAILED(hr)) return hr;

		ComPtr<IWICComponentInfo> spWicComponentInfo;
        hr = wicFactory->CreateComponentInfo(pixelFormatGUID, &spWicComponentInfo);
		if (FAILED(hr)) return hr;

		ComPtr<IWICPixelFormatInfo2> spWicPixelFormatInfo2;
		hr = spWicComponentInfo.As(&spWicPixelFormatInfo2);
		if (FAILED(hr)) return hr;

        WICPixelFormatNumericRepresentation pixelFormat = WICPixelFormatNumericRepresentationUnspecified;
        hr = spWicPixelFormatInfo2->GetNumericRepresentation(&pixelFormat);
		if (FAILED(hr)) return hr;

        D2D1_COLOR_SPACE colorSpace = 
            (pixelFormat == WICPixelFormatNumericRepresentationFloat) ? 
            D2D1_COLOR_SPACE_SCRGB : 
            D2D1_COLOR_SPACE_SRGB;
        hr = res->d2dContext->CreateColorContext(colorSpace, nullptr, 0, &res->imageColorContext);
        if (FAILED(hr)) return hr;
    }

    ComPtr<ID2D1Effect> sourceEffect;
    {
        hr = res->d2dContext->CreateEffect(CLSID_D2D1BitmapSource, &sourceEffect);
        if (FAILED(hr)) return hr;

        hr = sourceEffect->SetValue(D2D1_BITMAPSOURCE_PROP_WIC_BITMAP_SOURCE, pBitmap);
        if (FAILED(hr)) return hr;

        hr = sourceEffect->SetValue(D2D1_BITMAPSOURCE_PROP_INTERPOLATION_MODE, D2D1_BITMAPSOURCE_INTERPOLATION_MODE_LINEAR);
        if (FAILED(hr)) return hr;
    }

    res->colorManageEffect->SetInputEffect(0, sourceEffect.Get());

    return hr;
}

HRESULT D2D1_3_Renderer::reset_color_profile(int colorManagementMode)
{
    if (res->swapChain == nullptr) return E_FAIL;
	if (res->d2dContext == nullptr) return E_FAIL;

    HRESULT hr = S_OK;

    color_scale = 1.0F;

    ComPtr<ID2D1ColorContext1> displayColorContext;
    hr = res->d2dContext->CreateColorContextFromDxgiColorSpace(DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709, &displayColorContext);
    if (FAILED(hr)) return hr;

	if (colorManagementMode != 0)
    {
        DXGI_OUTPUT_DESC1 outputDesc = {};
        {
            ComPtr<IDXGIOutput> output;
            hr = res->swapChain->GetContainingOutput(&output);
            if (SUCCEEDED(hr))
            {
                ComPtr<IDXGIOutput6> output6;
                hr = output.As(&output6);
                if (SUCCEEDED(hr)) {
                    output6->GetDesc1(&outputDesc);
                }
            }
        }

        bool isHDR = (outputDesc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
        if (isHDR && colorManagementMode == 2)
        {
            hr = res->colorManageEffect->SetValue(D2D1_COLORMANAGEMENT_PROP_SOURCE_COLOR_CONTEXT, res->imageColorContext.Get());
            if (FAILED(hr)) return hr;

            hr = res->colorManageEffect->SetValue(D2D1_COLORMANAGEMENT_PROP_DESTINATION_COLOR_CONTEXT, displayColorContext.Get());
            if (FAILED(hr)) return hr;

            hr = res->displayEffect->SetValue(D2D1_COLORMANAGEMENT_PROP_DESTINATION_COLOR_CONTEXT, nullptr);
            if (FAILED(hr)) return hr;

            ComPtr<ID2D1Effect> colorMatrixEffect;
            hr = res->d2dContext->CreateEffect(CLSID_D2D1ColorMatrix, &colorMatrixEffect);
            if (FAILED(hr)) return hr;

            color_scale = outputDesc.MaxFullFrameLuminance / D2D1_SCENE_REFERRED_SDR_WHITE_LEVEL;
            D2D1_MATRIX_5X4_F matrix = D2D1::Matrix5x4F(
                color_scale, 0, 0, 0,
                0, color_scale, 0, 0,
                0, 0, color_scale, 0,
                0, 0, 0, 1,
                0, 0, 0, 0);

            hr = colorMatrixEffect->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, matrix);
            if (FAILED(hr)) return hr;

            colorMatrixEffect->SetInputEffect(0, res->colorManageEffect.Get());
            res->displayEffect->SetInputEffect(0, colorMatrixEffect.Get());
        }
        else
        {
            ComPtr<ID2D1ColorContext> monitorColorContext;
            hr = GetMonitorColorContext(&monitorColorContext, res->d2dContext.Get(), m_hwnd, colorManagementMode);
            if (FAILED(hr)) return hr;

            hr = res->colorManageEffect->SetValue(D2D1_COLORMANAGEMENT_PROP_SOURCE_COLOR_CONTEXT, res->imageColorContext.Get());
            if (FAILED(hr)) return hr;
            hr = res->colorManageEffect->SetValue(D2D1_COLORMANAGEMENT_PROP_DESTINATION_COLOR_CONTEXT, monitorColorContext.Get());
            if (FAILED(hr)) return hr;

            hr = res->displayEffect->SetValue(D2D1_COLORMANAGEMENT_PROP_DESTINATION_COLOR_CONTEXT, displayColorContext.Get());
            if (FAILED(hr)) return hr;

            res->displayEffect->SetInputEffect(0, res->colorManageEffect.Get());
        }
    }
    else
    {
        hr = res->colorManageEffect->SetValue(D2D1_COLORMANAGEMENT_PROP_SOURCE_COLOR_CONTEXT, nullptr);
        if (FAILED(hr)) return hr;
        hr = res->colorManageEffect->SetValue(D2D1_COLORMANAGEMENT_PROP_DESTINATION_COLOR_CONTEXT, nullptr);
        if (FAILED(hr)) return hr;

        hr = res->displayEffect->SetValue(D2D1_COLORMANAGEMENT_PROP_DESTINATION_COLOR_CONTEXT, displayColorContext.Get());
        if (FAILED(hr)) return hr;

        res->displayEffect->SetInputEffect(0, res->colorManageEffect.Get());
    }

    return S_OK;
}

HRESULT D2D1_3_Renderer::update_white_level(void)
{
    HMONITOR hMonitor = ::MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
    if (!hMonitor) return HRESULT_FROM_WIN32(::GetLastError());

    MONITORINFOEX mi = {};
    mi.cbSize = sizeof(mi);
    BOOL ret = ::GetMonitorInfo(hMonitor, &mi);
    if (!ret) return HRESULT_FROM_WIN32(::GetLastError());

    using namespace std;
    vector<DISPLAYCONFIG_PATH_INFO> paths;
    vector<DISPLAYCONFIG_MODE_INFO> modes;
    UINT32 flags = QDC_ONLY_ACTIVE_PATHS | QDC_VIRTUAL_MODE_AWARE;
    LONG result = ERROR_SUCCESS;

    do
    {
        UINT32 pathCount, modeCount;
        result = GetDisplayConfigBufferSizes(flags, &pathCount, &modeCount);
        if (result != ERROR_SUCCESS)
            return HRESULT_FROM_WIN32(result);
        paths.resize(pathCount);
        modes.resize(modeCount);

        result = QueryDisplayConfig(flags, &pathCount, paths.data(), &modeCount, modes.data(), nullptr);
        paths.resize(pathCount);
        modes.resize(modeCount);

    } while (result == ERROR_INSUFFICIENT_BUFFER);

    if (result != ERROR_SUCCESS)
        return HRESULT_FROM_WIN32(result);

    for (auto& path : paths)
    {
        DISPLAYCONFIG_SOURCE_DEVICE_NAME sourceName = {};
        sourceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        sourceName.header.size = sizeof(sourceName);
        sourceName.header.adapterId = path.sourceInfo.adapterId;
        sourceName.header.id = path.sourceInfo.id;
        result = DisplayConfigGetDeviceInfo(&sourceName.header);
        if (result != ERROR_SUCCESS)
            continue;

		if (wcscmp(mi.szDevice, sourceName.viewGdiDeviceName) != 0)
			continue;

        DISPLAYCONFIG_SDR_WHITE_LEVEL whiteLevel = {};
        whiteLevel.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SDR_WHITE_LEVEL;
        whiteLevel.header.size = sizeof(whiteLevel);
        whiteLevel.header.adapterId = path.targetInfo.adapterId;
        whiteLevel.header.id = path.targetInfo.id;
        result = DisplayConfigGetDeviceInfo(&whiteLevel.header);
        if (result != ERROR_SUCCESS)
            return HRESULT_FROM_WIN32(result);

        SDRWhiteLevel = whiteLevel.SDRWhiteLevel;
        return S_OK;
    }

    return S_FALSE;
}

HRESULT D2D1_3_Renderer::render(void)
{
    if (!res->d2dContext)
        return E_FAIL;

    HRESULT hr = S_OK;

    DWORD bkcolor = ::GetSysColor(COLOR_APPWORKSPACE);
	float r = GetRValue(bkcolor) * SDRWhiteLevel / (255.0F * 1000);
	float g = GetGValue(bkcolor) * SDRWhiteLevel / (255.0F * 1000);
	float b = GetBValue(bkcolor) * SDRWhiteLevel / (255.0F * 1000);

    res->d2dContext->BeginDraw();
    res->d2dContext->Clear(D2D1::ColorF(r, g, b));

    if (image_size.height > 0 && image_size.width > 0)
    {
        ComPtr<ID2D1Effect> scaleEffect;
        res->d2dContext->CreateEffect(CLSID_D2D1Scale, &scaleEffect);
        scaleEffect->SetInputEffect(0, res->displayEffect.Get());
        scaleEffect->SetValue(D2D1_SCALE_PROP_CENTER_POINT, D2D1::Vector2F(0, 0));
        scaleEffect->SetValue(D2D1_SCALE_PROP_SCALE, D2D1::Vector2F(scale, scale));

        res->d2dContext->DrawImage(
            scaleEffect.Get(),
            target_offset,
            display_rect,
            D2D1_INTERPOLATION_MODE_LINEAR,
            D2D1_COMPOSITE_MODE_SOURCE_OVER);
    }

    hr = res->d2dContext->EndDraw();
    hr = res->swapChain->Present(1, 0);
    return hr;
}

HRESULT D2D1_3_Renderer::set_offset(int x, int y)
{
	offset.x += x;
	offset.y += y;
	return update_image_position();
}

HRESULT D2D1_3_Renderer::set_scale(float s)
{
	scale = s;
    return update_image_position();
}

HRESULT D2D1_3_Renderer::update_image_position(void)
{
	if (res->d2dContext == nullptr) return E_FAIL;

    D2D1_SIZE_F display_size = { image_size.width * scale, image_size.height * scale };
	D2D1_SIZE_F target_size = res->d2dContext->GetSize();

    FLOAT dstW = min(display_size.width, target_size.width);
    FLOAT dstH = min(display_size.height, target_size.height);
    FLOAT dstX = max(0, (target_size.width - display_size.width) / 2);
    FLOAT dstY = max(0, (target_size.height - display_size.height) / 2);

    FLOAT srcX = max(0, min(-offset.x, display_size.width - dstW));
    FLOAT srcY = max(0, min(-offset.y, display_size.height - dstH));

    display_rect = D2D1::RectF(srcX, srcY, display_size.width, display_size.height);
    target_offset = D2D1::Point2F(dstX, dstY);
    offset.x = -srcX;
    offset.y = -srcY;

    return S_OK;
}


// ChildGraphicViewD2D1_3

IMPLEMENT_DYNAMIC(ChildGraphicViewD2D1_3, CWnd)

ChildGraphicViewD2D1_3::ChildGraphicViewD2D1_3()
{

}

ChildGraphicViewD2D1_3::~ChildGraphicViewD2D1_3()
{
}

void ChildGraphicViewD2D1_3::init_window(CWnd* pParent)
{
    Create(
        ::AfxRegisterWndClass(CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW)),
        _T(""),
        WS_CHILD,
        CRect(0, 0, 0, 0), pParent, (UINT)-1);
}


void ChildGraphicViewD2D1_3::update_setting(int /*rendering_engine*/, int monitor_color_profile)
{
	colorManagementMode = monitor_color_profile;
	if (renderer == nullptr) return;
	renderer->reset_color_profile(colorManagementMode);
}

void ChildGraphicViewD2D1_3::reset_color_profile(void)
{
    if (renderer == nullptr) return;
    renderer->reset_color_profile(colorManagementMode);
}

void ChildGraphicViewD2D1_3::reset_content(agaliaContainer* image)
{
    if (renderer == nullptr) return;

	bool success = false;

	CComPtr<IWICBitmap> bitmap;
	CComPtr<IWICColorContext> colorContext;
    if (image)
    {
        auto hr = image->loadBitmap(&bitmap, &colorContext);
        if (SUCCEEDED(hr))
        {
            renderer->set_source_image(bitmap, colorContext);
            renderer->reset_color_profile(colorManagementMode);
			renderer->update_image_position();
            renderer->render();
			success = true;
        }
    }

	if (!success)
	{
		renderer->set_source_image(nullptr, nullptr);
    }
}

void ChildGraphicViewD2D1_3::reset_scale(void)
{
	if (renderer == nullptr) return;

    const FLOAT scales[] = { 0.125f, 0.25f, 0.50f, 0.75f, 1.00f, 1.25f, 1.50f, 2.00f, 3.00f, 4.00f, 5.00f };
    const int idx_org = 4;

    int idx = min(max(0, scaleIndex + idx_org), _countof(scales) - 1);
    scaleIndex = idx - idx_org;
    renderer->set_scale(scales[idx]);
}


BEGIN_MESSAGE_MAP(ChildGraphicViewD2D1_3, CWnd)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_SIZE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEHWHEEL()
    ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()



// ChildGraphicViewD2D1_3 message handlers




int ChildGraphicViewD2D1_3::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    renderer = new D2D1_3_Renderer(GetSafeHwnd());
    renderer->init();

    return 0;
}


void ChildGraphicViewD2D1_3::OnDestroy()
{
    CWnd::OnDestroy();

    delete renderer;
    renderer = nullptr;
}


void ChildGraphicViewD2D1_3::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    renderer->render();
}


BOOL ChildGraphicViewD2D1_3::OnEraseBkgnd(CDC* pDC)
{
    renderer->update_white_level();

    return CWnd::OnEraseBkgnd(pDC);
}


void ChildGraphicViewD2D1_3::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    renderer->resize();
    renderer->update_image_position();
}


void ChildGraphicViewD2D1_3::OnLButtonDown(UINT nFlags, CPoint point)
{
    ::SetCapture(GetSafeHwnd());
    prePosition = point;

    CWnd::OnLButtonDown(nFlags, point);
}


void ChildGraphicViewD2D1_3::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (::GetCapture() == GetSafeHwnd())
        ::ReleaseCapture();

    CWnd::OnLButtonUp(nFlags, point);
}


void ChildGraphicViewD2D1_3::OnMouseMove(UINT nFlags, CPoint point)
{
    if (::GetCapture() == GetSafeHwnd())
    {
        int x = point.x - prePosition.x;
        int y = point.y - prePosition.y;
        prePosition.x = point.x;
        prePosition.y = point.y;

        HRESULT hr = renderer->set_offset(x, y);
        if (SUCCEEDED(hr))
        {
            renderer->render();
        }
    }

    CWnd::OnMouseMove(nFlags, point);
}


BOOL ChildGraphicViewD2D1_3::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    if (zDelta)
    {
        if (::GetKeyState(VK_CONTROL) < 0)
        {
            int shift = (zDelta < 0) ? -1 : 1;
            scaleIndex += shift;

            reset_scale();
            renderer->render();
        }
        else if (::GetKeyState(VK_SHIFT) < 0)
        {
            HRESULT hr = renderer->set_offset(zDelta, 0);
            if (SUCCEEDED(hr))
            {
                renderer->render();
            }
        }
        else
        {
            HRESULT hr = renderer->set_offset(0, zDelta);
            if (SUCCEEDED(hr))
            {
                renderer->render();
            }
        }
    }

    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


void ChildGraphicViewD2D1_3::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
    if (zDelta)
    {
        if (::GetKeyState(VK_CONTROL) < 0)
        {
        }
        else
        {
            HRESULT hr = renderer->set_offset(-zDelta, 0);
            if (SUCCEEDED(hr))
            {
                renderer->render();
            }
        }
    }

    CWnd::OnMouseHWheel(nFlags, zDelta, pt);
}
