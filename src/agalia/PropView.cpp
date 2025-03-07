// PropView.cpp : implementation file
//

#include "pch.h"
#include "agalia.h"
#include "PropView.h"

#include "../inc/agaliaUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// PropView

IMPLEMENT_DYNAMIC(PropView, CWnd)

PropView::PropView()
{

}

PropView::~PropView()
{
}


BEGIN_MESSAGE_MAP(PropView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()



// PropView message handlers




void PropView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CWnd::OnPaint() for painting messages

	dc.FillSolidRect(&dc.m_ps.rcPaint, ::GetSysColor(COLOR_WINDOW));

	BITMAPV5HEADER bi = {};
	HBITMAP hBitmap = NULL;
	if (m_pBitmap && SUCCEEDED(m_pBitmap->getBitmapInfo(&bi)) && SUCCEEDED(m_pBitmap->getHBitmap(&hBitmap)))
	{
		CRect rcClient;
		GetClientRect(&rcClient);

		int dstAreaW = rcClient.Width();
		int dstAreaH = dstAreaW;

		auto mx = static_cast<double>(dstAreaW) / bi.bV5Width;
		auto my = static_cast<double>(dstAreaH) / bi.bV5Height;
		double m = min(mx, my);

		auto dstImgW = static_cast<int>(bi.bV5Width * m);
		auto dstImgH = static_cast<int>(bi.bV5Height * m);

		int dstImgX = (dstAreaW - dstImgW) / 2;
		int dstImgY = (dstAreaH - dstImgH) / 2;

		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		HGDIOBJ hOldBmp = memDC.SelectObject(hBitmap);

		if (bi.bV5AlphaMask == 0)
		{
			dc.SetStretchBltMode(HALFTONE);
			dc.SetBrushOrg(0, 0);
			dc.StretchBlt(dstImgX, dstImgY, dstImgW, dstImgH, &memDC, 0, 0, bi.bV5Width, bi.bV5Height, SRCCOPY);
		}
		else
		{
			dc.AlphaBlend(dstImgX, dstImgY, dstImgW, dstImgH, &memDC, 0, 0, bi.bV5Width, bi.bV5Height, BLENDFUNCTION{ AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
		}

		memDC.SelectObject(hOldBmp);
		memDC.DeleteDC();
	}
}


int PropView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_listCtrl.Create(LVS_REPORT | WS_VISIBLE | WS_CHILD | WS_BORDER, CRect(0, 0, 0, 0), this, (UINT)-1);
	DWORD dwExStyle = m_listCtrl.GetExtendedStyle();
	m_listCtrl.SetExtendedStyle(dwExStyle | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	m_listCtrl.InsertColumn(0, L"Property");
	m_listCtrl.InsertColumn(1, L"Value");

	return 0;
}


void PropView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_listCtrl.MoveWindow(0, cx, cx, cy - cx);
}


void PropView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		if (!pos_initialized)
		{
			CRect rcClient;
			GetClientRect(&rcClient);
			int cx = rcClient.Width();
			int cy = rcClient.Height();
			m_listCtrl.MoveWindow(0, cx, cx, cy - cx);

			m_listCtrl.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
			m_listCtrl.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

			pos_initialized = true;
		}
	}
}
