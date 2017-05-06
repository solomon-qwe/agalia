// ImageViewer.cpp : 
//

#include "stdafx.h"
#include "agalia.h"
#include "ImageViewer.h"

#include <gdiplus.h>

// CImageViewer

IMPLEMENT_DYNAMIC(CImageViewer, CWnd)

CImageViewer::CImageViewer()
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

CImageViewer::~CImageViewer()
{
	if (gdi_image)
	{
		Gdiplus::Image* p = reinterpret_cast<Gdiplus::Image*>(gdi_image);
		gdi_image = nullptr;
		delete p;
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);
}


BEGIN_MESSAGE_MAP(CImageViewer, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

#include "../inc/agaliarept.h"
#include "../inc/handle_wrapper.h"

void CImageViewer::CreateViewer(LPCTSTR pszFilePath, LPARAM lParam)
{
	agalia::list_item_param* param = reinterpret_cast<agalia::list_item_param*>(lParam);
	uint64_t offset = param->offset;
	uint64_t size = param->size;

	if (size > 0xFFFFFFFF) {
		return;
	}

	TCHAR sz[1024] = {};
	_stprintf_s(sz, _T("%s %I64u-%I64u"), ::PathFindFileName(pszFilePath), offset, size);

	handle_wrapper hFile(INVALID_HANDLE_VALUE);
	hFile = ::CreateFile(pszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile.isErr()) {
		::AfxGetMainWnd()->MessageBox(_T("File Open Error."), nullptr, MB_ICONWARNING);
		return;
	}

	handle_wrapper hMap(NULL);
	hMap = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hMap.isErr()) {
		::AfxGetMainWnd()->MessageBox(_T("File Mapping Error."), nullptr, MB_ICONWARNING);
		return;
	}

	LPVOID pFile = ::MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	if (pFile == NULL) {
		::AfxGetMainWnd()->MessageBox(_T("File Mapping Error."), nullptr, MB_ICONWARNING);
		return;
	}

	IStream* pStream = SHCreateMemStream(reinterpret_cast<BYTE*>(pFile) + offset, (UINT)size);

	CImageViewer* p = new CImageViewer;
	Gdiplus::Image* image = Gdiplus::Image::FromStream(pStream);

	pStream->Release();
	::UnmapViewOfFile(pFile);
	hMap.close();
	hFile.close();

	Gdiplus::Status stat = image->GetLastStatus();
	if (stat != Gdiplus::Ok)
	{
		::AfxGetMainWnd()->MessageBox(_T("Image Load Error."), nullptr, MB_ICONWARNING);
		delete image;
		delete p;
		return;
	}

	p->gdi_image = image;
	p->CreateEx(
		0,
		::AfxRegisterWndClass(
			CS_VREDRAW | CS_HREDRAW,
			::LoadCursor(NULL, IDC_ARROW),
			0,
			::LoadIcon(::GetModuleHandle(NULL) , MAKEINTRESOURCE(IDR_MAINFRAME))),
		sz,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		::AfxGetMainWnd()->GetSafeHwnd(),
		NULL,
		0);
}


void CImageViewer::PostNcDestroy()
{
	CWnd::PostNcDestroy();

	delete this;
}


void CImageViewer::OnPaint()
{
	CPaintDC dc(this);

	if (gdi_image == nullptr)
	{
		dc.FillSolidRect(&dc.m_ps.rcPaint, ::GetSysColor(COLOR_WINDOW));
		return;
	}

	using namespace Gdiplus;

	Bitmap* image = reinterpret_cast<Bitmap*>(gdi_image);
	RectF boundsRect;
	Unit unit;
	image->GetBounds(&boundsRect, &unit);

	Graphics* graphics = new Graphics(dc.GetSafeHdc());
	graphics->DrawImage(image, boundsRect);
	delete graphics;

	CRect rc;
	rc = dc.m_ps.rcPaint;
	rc.left = (LONG)boundsRect.Width;
	dc.FillSolidRect(rc, ::GetSysColor(COLOR_WINDOW));

	rc = dc.m_ps.rcPaint;
	rc.top = (LONG)boundsRect.Height;
	rc.right = (LONG)boundsRect.Width;
	dc.FillSolidRect(rc, ::GetSysColor(COLOR_WINDOW));
}
