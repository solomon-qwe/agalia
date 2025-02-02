#include "pch.h"
#include "../inc/agaliarept.h"

#include <atlbase.h>

__pragma(warning(push)) __pragma(warning(disable: 4458))
#include <gdiplus.h>
__pragma(warning(pop))
#pragma comment (lib,"Gdiplus.lib")



HRESULT loadThumbnailImageGDIP_stub(IStream* stream, uint32_t maxW, uint32_t maxH, HBITMAP* phBitmap)
{
	using namespace Gdiplus;

	Image image(stream);
	if (image.GetLastStatus() != Ok)
		return E_FAIL;

	double scaleH = (double)maxH / image.GetHeight();
	double scaleW = (double)maxW / image.GetWidth();
	double scale = min(scaleH, scaleW);

	auto w = static_cast<UINT>(image.GetWidth() * scale);
	auto h = static_cast<UINT>(image.GetHeight() * scale);

	Image* thumb = image.GetThumbnailImage(w, h);
	if (!thumb)
		return E_FAIL;

	BITMAPINFOHEADER bmi = {};
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biWidth = thumb->GetWidth();
	bmi.biHeight = thumb->GetHeight();
	bmi.biPlanes = 1;
	bmi.biBitCount = 32;
	void* pBits = nullptr;

	HDC memDC = ::CreateCompatibleDC(NULL);
	HBITMAP hBitmap = ::CreateDIBSection(memDC, reinterpret_cast<BITMAPINFO*>(&bmi), DIB_RGB_COLORS, &pBits, NULL, 0);
	if (hBitmap)
	{
		HGDIOBJ hOldBmp = SelectObject(memDC, hBitmap);
		Graphics* graphics = new Graphics(memDC);
		if (graphics)
		{
			graphics->DrawImage(thumb, 0, 0, thumb->GetWidth(), thumb->GetHeight());
			delete graphics;
		}
		SelectObject(memDC, hOldBmp);
	}
	DeleteDC(memDC);
	delete thumb;

	if (hBitmap)
	{
		*phBitmap = hBitmap;
		return S_OK;
	}
	return E_FAIL;
}


HRESULT loadThumbnailBitmap(HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH, IStream* stream)
{
	using namespace Gdiplus;

	GdiplusStartupInput gdiplusStartupInput = {};
	ULONG_PTR gdiplusToken = 0;
	Status stat = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	if (stat != Ok) {
		return E_FAIL;
	}

	auto result = loadThumbnailImageGDIP_stub(stream, maxW, maxH, phBitmap);

	GdiplusShutdown(gdiplusToken);

	return result;
}
