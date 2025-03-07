#include "pch.h"
#include "ImageDrawGDI.h"

#include "../inc/agaliarept.h"
#include "../inc/agaliaUtil.h"

#include <stdint.h>

#include <Icm.h>
#pragma comment(lib, "Mscms.lib")


inline LARGE_INTEGER uint64_to_li(uint64_t u)
{
	LARGE_INTEGER li;
	li.QuadPart = u;
	return li;
}



ImageDrawGDI::ImageDrawGDI()
{

}


ImageDrawGDI::~ImageDrawGDI()
{
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
	if (offscreen_bmp) {
		offscreen_bmp.detach()->Release();
	}

	if (bmpInfoWithProfile) {
		bmpInfoWithProfile.Free();
	}

	if (source_bmp) {
		source_bmp.detach()->Release();
	}

	if (!image)
		return E_FAIL;

	agaliaPtr<agaliaHeap> profile;
	if (colorManagementMode != color_management_disable) {
		image->getColorProfile(&profile);
	}

	agaliaPtr<agaliaBitmap> bitmap;
	HRESULT hr = image->loadBitmap(&bitmap);
	if (SUCCEEDED(hr))
	{
		BITMAPV5HEADER bmpInfo = {};
		hr = bitmap->getBitmapInfo(&bmpInfo);
		if (SUCCEEDED(hr))
		{
			source_bmp = bitmap.detach();

			rsize_t bmpInfoSize = sizeof(BITMAPV5HEADER) + (profile ? profile->GetSize() : 0);
			if (!bmpInfoWithProfile.AllocateBytes(bmpInfoSize)) return E_OUTOFMEMORY;
			memset(bmpInfoWithProfile.m_pData, 0, bmpInfoSize);
			memcpy(bmpInfoWithProfile.m_pData, &bmpInfo, sizeof(BITMAPV5HEADER));
			if (profile)
			{
				bmpInfoWithProfile->bV5CSType = PROFILE_EMBEDDED;
				bmpInfoWithProfile->bV5ProfileData = sizeof(BITMAPV5HEADER);
				hr = SizeTToDWord(profile->GetSize(), &bmpInfoWithProfile->bV5ProfileSize);
				if (FAILED(hr)) return hr;
				memcpy(reinterpret_cast<uint8_t*>(bmpInfoWithProfile.m_pData) + sizeof(BITMAPV5HEADER), profile->GetData(), profile->GetSize());
			}

			reset_color_profile(colorManagementMode);
			UpdateImagePosition();
		}
	}

	if (FAILED(hr))
	{
		::RedrawWindow(hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
		return hr;
	}

	return S_OK;
}


HRESULT ImageDrawGDI::reset_color_profile(int colorManagementMode)
{
	if (!bmpInfoWithProfile)
		return E_FAIL;

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

	auto hr = createAgaliaBitmap(&offscreen_bmp, nullptr);
	if (SUCCEEDED(hr))
	{
		BITMAPV5HEADER bmpInfo = *bmpInfoWithProfile;
		bmpInfo.bV5CSType = 0;
		bmpInfo.bV5ProfileData = 0;
		bmpInfo.bV5ProfileSize = 0;

		void* pBits = nullptr;
		HBITMAP hOffscreeBitmap = ::CreateDIBSection(hMemDC, reinterpret_cast<BITMAPINFO*>(&bmpInfo), DIB_RGB_COLORS, &pBits, NULL, 0);
		if (hOffscreeBitmap) {
			offscreen_bmp->init(hOffscreeBitmap, bmpInfo, pBits);
		}
		else {
			hr = HRESULT_FROM_WIN32(::GetLastError());
		}
	}

	if (SUCCEEDED(hr))
	{
		void* pSrcBits = nullptr;
		HBITMAP hOfscreenbmp = NULL;
		if (SUCCEEDED(source_bmp->getBits(&pSrcBits)) &&
			SUCCEEDED(offscreen_bmp->getHBitmap(&hOfscreenbmp)))
		{
			HGDIOBJ hOldBmp = ::SelectObject(hMemDC, hOfscreenbmp);
			if (colorManagementMode != color_management_disable) {
				::SetICMMode(hMemDC, ICM_ON);
			}
			::SetDIBitsToDevice(
				hMemDC,
				0, 0, bmpInfoWithProfile->bV5Width, bmpInfoWithProfile->bV5Height,
				0, 0, 0, bmpInfoWithProfile->bV5Height,
				pSrcBits,
				reinterpret_cast<BITMAPINFO*>(bmpInfoWithProfile.m_pData),
				DIB_RGB_COLORS);
			if (colorManagementMode != color_management_disable) {
				::SetICMMode(hMemDC, ICM_OFF);
			}
			::SelectObject(hMemDC, hOldBmp);
		}
	}

	::ReleaseDC(hwnd, hdc);
	::DeleteDC(hMemDC);

	return S_OK;
}


HRESULT ImageDrawGDI::update_for_window_size_change(void)
{
	UpdateImagePosition();
	return S_OK;
}


HRESULT ImageDrawGDI::render(DWORD bkcolor)
{
	if (hwnd == NULL) return E_FAIL;

	CRect rcClient(0, 0, 0, 0);
	::GetClientRect(hwnd, &rcClient);

	HDC hDC = ::GetDC(hwnd);
	HBITMAP hMemBitmap = ::CreateCompatibleBitmap(hDC, rcClient.Width(), rcClient.Height());
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HGDIOBJ hOldMemBmp = ::SelectObject(hMemDC, hMemBitmap);

	HBITMAP hOfscreenBmp = NULL;
	void* pBits = nullptr;
	BITMAPV5HEADER bmpInfo = {};
	if (offscreen_bmp) {
		offscreen_bmp->getBits(&pBits);
		offscreen_bmp->getBitmapInfo(&bmpInfo);
		offscreen_bmp->getHBitmap(&hOfscreenBmp);
	}

	HBRUSH br = ::CreateSolidBrush(bkcolor);
	::FillRect(hMemDC, &rcClient, br);
	::DeleteObject(br);

	if (hOfscreenBmp)
	{
		HDC hOffscreenDC = ::CreateCompatibleDC(hMemDC);
		HGDIOBJ hOldBmp = ::SelectObject(hOffscreenDC, hOfscreenBmp);
		if (bmpInfo.bV5AlphaMask == 0)
		{
			::SetStretchBltMode(hMemDC, HALFTONE);
			::SetBrushOrgEx(hMemDC, 0, 0, NULL);
			::StretchDIBits(
				hMemDC,
				rcDst.left, rcDst.top, rcDst.Width(), rcDst.Height(),
				rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(),
				pBits, reinterpret_cast<BITMAPINFO*>(&bmpInfo),
				DIB_RGB_COLORS,
				SRCCOPY);
		}
		else {
			::AlphaBlend(
				hMemDC,
				rcDst.left, rcDst.top, rcDst.Width(), rcDst.Height(),
				hOffscreenDC,
				rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(),
				BLENDFUNCTION{ AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
		}
		::SelectObject(hOffscreenDC, hOldBmp);
		::DeleteDC(hOffscreenDC);
	}

	::BitBlt(hDC, 0, 0, rcClient.Width(), rcClient.Height(), hMemDC, 0, 0, SRCCOPY);
	::SelectObject(hMemDC, hOldMemBmp);
	::DeleteObject(hMemBitmap);
	::DeleteDC(hMemDC);
	::ReleaseDC(hwnd, hDC);

	return S_OK;
}


HRESULT ImageDrawGDI::offset(int x, int y)
{
	ptOffset.x += x;
	ptOffset.y += y;
	UpdateImagePosition();
	return S_OK;
}


HRESULT ImageDrawGDI::set_scale(float scale)
{
	fScale = scale;
	UpdateImagePosition();
	return S_OK;
}


void ImageDrawGDI::UpdateImagePosition(void)
{
	if (hwnd == NULL) return;

	CRect rcClient(0, 0, 0, 0);
	::GetClientRect(hwnd, &rcClient);

	if (!bmpInfoWithProfile) return;

	double fWndW = rcClient.Width();
	double fWndH = rcClient.Height();

	double fImgW = bmpInfoWithProfile->bV5Width;
	double fImgH = bmpInfoWithProfile->bV5Height;

	double fDstW = min(fImgW * fScale, fWndW);
	double fDstH = min(fImgH * fScale, fWndH);

	double fSrcX = max(0, min(-ptOffset.x / fScale, fImgW - fWndW / fScale));
	double fSrcY = max(0, min(-ptOffset.y / fScale, fImgH - fWndH / fScale));

	ptOffset.x = -(int)(fSrcX * fScale);
	ptOffset.y = -(int)(fSrcY * fScale);

	rcDst.left   = (int)max(0, (fWndW - fImgW * fScale) / 2);
	rcDst.top    = (int)max(0, (fWndH - fImgH * fScale) / 2);
	rcDst.right  = rcDst.left + (int)fDstW;
	rcDst.bottom = rcDst.top  + (int)fDstH;

	bool isInvert = false;
	if (offscreen_bmp) {
		BITMAPV5HEADER bmpInfo = {};
		offscreen_bmp->getBitmapInfo(&bmpInfo);
		isInvert = (bmpInfo.bV5AlphaMask == 0);
	}

	rcSrc.left = (int)fSrcX;
	rcSrc.top = isInvert ? (int)(fImgH - (fDstH / fScale + fSrcY)) : (int)fSrcY;
	rcSrc.right = rcSrc.left + (int)(fDstW / fScale);
	rcSrc.bottom = rcSrc.top + (int)(fDstH / fScale);
}
