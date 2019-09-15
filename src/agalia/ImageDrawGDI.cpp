#include "pch.h"
#include "ImageDrawGDI.h"

#include "../inc/agaliarept.h"

#include <stdint.h>

#include <Icm.h>
#pragma comment(lib, "Mscms.lib")


inline LARGE_INTEGER uint64_to_li(uint64_t u)
{
	LARGE_INTEGER li;
	li.QuadPart = u;
	return li;
}



HRESULT read_jpeg(IStream* stream, BITMAPV5HEADER* bmpInfo, CHeapPtr<char>& image_buf)
{
	using namespace Gdiplus;

	Image image(stream);
	if (image.GetLastStatus() != Ok)
		return E_FAIL;

	BITMAPINFOHEADER bmi = {};
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biWidth = image.GetWidth();
	bmi.biHeight = image.GetHeight();
	bmi.biPlanes = 1;
	bmi.biBitCount = 32;
	void* pBits = nullptr;

	bmpInfo->bV5Width = bmi.biWidth;
	bmpInfo->bV5Height = bmi.biHeight;

	HDC memDC = ::CreateCompatibleDC(NULL);
	HBITMAP hBitmap = ::CreateDIBSection(memDC, reinterpret_cast<BITMAPINFO*>(&bmi), DIB_RGB_COLORS, &pBits, NULL, 0);
	if (hBitmap)
	{
		HGDIOBJ hOldBmp = SelectObject(memDC, hBitmap);
		Graphics* graphics = new Graphics(memDC);
		if (graphics)
		{
			graphics->DrawImage(&image, 0, 0, image.GetWidth(), image.GetHeight());
			delete graphics;
		}
		SelectObject(memDC, hOldBmp);
	}
	DeleteDC(memDC);

	if (hBitmap)
	{
		rsize_t bufsize = size_t(4) * bmi.biWidth * bmi.biHeight;
		image_buf.AllocateBytes(bufsize);
		memcpy(image_buf.m_pData, pBits, bufsize);

		::DeleteObject(hBitmap);
		return S_OK;
	}
	return E_FAIL;
}



ImageDrawGDI::ImageDrawGDI()
{

}


ImageDrawGDI::~ImageDrawGDI()
{
	if (hOffscreenBmp)
	{
		auto temp = hOffscreenBmp;
		hOffscreenBmp = NULL;
		::DeleteObject(temp);
	}
}


void ImageDrawGDI::attach(HWND target)
{
	hwnd = target;
}


void ImageDrawGDI::detach(void)
{
	hwnd = NULL;
}


HRESULT ImageDrawGDI::reset_content(agaliaContainer* image, int colorManagementMode)
{
	if (!image)
		return E_FAIL;

	if (hOffscreenBmp)
	{
		auto temp = hOffscreenBmp;
		hOffscreenBmp = NULL;
		::DeleteObject(temp);
	}

	if (source_bmpInfo) {
		source_bmpInfo.Free();
	}

	if (image_buf) {
		image_buf.Free();
	}
	
	agaliaPtr<agaliaHeap> profile;
	if (colorManagementMode != color_management_disable) {
		image->getColorProfile(&profile);
	}

	rsize_t bufsize = sizeof(BITMAPV5HEADER) + (profile ? profile->GetSize() : 0);
	if (!source_bmpInfo.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
	memset(source_bmpInfo.m_pData, 0, bufsize);

	source_bmpInfo->bV5Size = sizeof(BITMAPV5HEADER);
	source_bmpInfo->bV5Width = 0;
	source_bmpInfo->bV5Height = 0;
	source_bmpInfo->bV5Planes = 1;
	source_bmpInfo->bV5BitCount = 32;
	if (profile)
	{
		source_bmpInfo->bV5CSType = PROFILE_EMBEDDED;
		source_bmpInfo->bV5ProfileData = sizeof(BITMAPV5HEADER);
		auto hr = SizeTToDWord(profile->GetSize(), &source_bmpInfo->bV5ProfileSize);
		if (FAILED(hr)) return hr;
		memcpy(reinterpret_cast<uint8_t*>(source_bmpInfo.m_pData) + sizeof(BITMAPV5HEADER), profile->GetData(), profile->GetSize());
	}

	CComPtr<IStream> stream;
	auto hr = image->getAsocStream(&stream);
	if (FAILED(hr)) return hr;

	using namespace Gdiplus;

	GdiplusStartupInput gdiplusStartupInput = {};
	ULONG_PTR gdiplusToken = 0;
	Status stat = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	if (stat != Ok) {
		return E_FAIL;
	}

	hr = read_jpeg(stream, source_bmpInfo, image_buf);

	GdiplusShutdown(gdiplusToken);

	if (FAILED(hr)) return hr;


	reset_color_profile(colorManagementMode);

	return S_OK;
}


HRESULT ImageDrawGDI::reset_color_profile(int colorManagementMode)
{
	if (!source_bmpInfo)
		return E_FAIL;

	// ウィンドウデバイスコンテキストの初期化して、それを元にメモリデバイス子テキストを作成 
	HDC hdc = ::GetDC(hwnd);
	if (colorManagementMode != color_management_disable) {
		::SetICMMode(hdc, ICM_ON);
		if (colorManagementMode == color_management_sRGB) {
			const wchar_t filename[] = L"sRGB Color Space Profile.icm";
			DWORD dwSize = 0;
			::GetColorDirectory(NULL, NULL, &dwSize);
			CHeapPtr<wchar_t> path;
			if (path.Allocate(size_t(dwSize) + 1 + _countof(filename))) {
				::GetColorDirectory(NULL, path, &dwSize);
				::PathAppend(path, filename);
			}
			::SetICMProfile(hdc, path);
		}
	}
	HDC hMemDC = ::CreateCompatibleDC(hdc);
	if (colorManagementMode != color_management_disable) {
		::SetICMMode(hdc, ICM_OFF);
	}

	// オフスクリーン用ビットマップを作成 
	BITMAPINFOHEADER bmpInfoHeader = {};
	memcpy(&bmpInfoHeader, source_bmpInfo.m_pData, sizeof(BITMAPINFOHEADER));
	bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	void* pBits = nullptr;
	HBITMAP hBitmap = ::CreateDIBSection(hMemDC, reinterpret_cast<BITMAPINFO*>(&bmpInfoHeader), DIB_RGB_COLORS, &pBits, NULL, 0);
	if (hBitmap)
	{
		// オフスクリーンビットマップに原画像を色変換して転送 
		HGDIOBJ hOldBmp = ::SelectObject(hMemDC, hBitmap);
		if (colorManagementMode != color_management_disable) {
			::SetICMMode(hMemDC, ICM_ON);
		}
		::SetDIBitsToDevice(
			hMemDC,
			0, 0, source_bmpInfo->bV5Width, source_bmpInfo->bV5Height,
			0, 0, 0, source_bmpInfo->bV5Height,
			image_buf.m_pData,
			reinterpret_cast<BITMAPINFO*>(source_bmpInfo.m_pData),
			DIB_RGB_COLORS);
		if (colorManagementMode != color_management_disable) {
			::SetICMMode(hMemDC, ICM_OFF);
		}
		::SelectObject(hMemDC, hOldBmp);
	}

	::ReleaseDC(hwnd, hdc);
	::DeleteDC(hMemDC);

	if (hBitmap == NULL)
		return E_FAIL;

	hOffscreenBmp = hBitmap;
	return S_OK;
}


HRESULT ImageDrawGDI::update_for_window_size_change(void)
{
	return E_NOTIMPL;
}


HRESULT ImageDrawGDI::render(DWORD bkcolor)
{
	HDC hDC = ::GetDC(hwnd);

	CRect rcClient(0, 0, 0, 0);
	::GetClientRect(hwnd, &rcClient);

	DIBSECTION dib = {};
	::GetObject(hOffscreenBmp, sizeof(dib), &dib);

	// 画面が広ければセンタリング、狭ければ左上座標固定 
	int w = min(dib.dsBmih.biWidth, rcClient.Width());
	int h = min(dib.dsBmih.biHeight, rcClient.Height());
	int x = max(0, (rcClient.Width() - dib.dsBmih.biWidth) / 2);
	int y = max(0, (rcClient.Height() - dib.dsBmih.biHeight) / 2);

	// 画像描画 
	::SetDIBitsToDevice(
		hDC,
		x, y, w, h,
		0, dib.dsBmih.biHeight - h, 0, dib.dsBmih.biHeight,
		dib.dsBm.bmBits, reinterpret_cast<BITMAPINFO*>(&dib.dsBmih),
		DIB_RGB_COLORS);

	// 背景描画 
	HBRUSH br = ::CreateSolidBrush(bkcolor);
	CRect rcFill;

	rcFill = rcClient;
	rcFill.bottom = y;
	::FillRect(hDC, &rcFill, br);	// upper 

	rcFill = rcClient;
	rcFill.top = y;
	rcFill.right = x;
	rcFill.bottom = y + h;
	::FillRect(hDC, &rcFill, br);	// left 

	rcFill = rcClient;
	rcFill.top = y;
	rcFill.left = x + w;
	rcFill.bottom = y + h;
	::FillRect(hDC, &rcFill, br);	// right 

	rcFill = rcClient;
	rcFill.top = y + h;
	::FillRect(hDC, &rcFill, br);	// lower 

	::DeleteObject(br);
	::ReleaseDC(hwnd, hDC);

	return S_OK;
}
