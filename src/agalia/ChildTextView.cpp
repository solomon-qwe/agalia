// ChildTextView.cpp : implementation file
//

#include "pch.h"
#include "agalia.h"
#include "ChildTextView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ChildTextView

IMPLEMENT_DYNCREATE(ChildTextView, CWnd)

ChildTextView::ChildTextView()
{

}

ChildTextView::~ChildTextView()
{
}


BEGIN_MESSAGE_MAP(ChildTextView, CWnd)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// ChildTextView message handlers




void ChildTextView::PostNcDestroy()
{
	CWnd::PostNcDestroy();

	delete this;
}


void ChildTextView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_edit.MoveWindow(0, 0, cx, cy);
}


int ChildTextView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_edit.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY, CRect(0, 0, 0, 0), this, 0)) {
		return -1;
	}

	// フォントを Consolas に変更 
	HGDIOBJ hGUIFont = ::GetStockObject(DEFAULT_GUI_FONT);
	if (hGUIFont)
	{
		LOGFONT logfont = {};
		if (::GetObject(hGUIFont, sizeof(logfont), &logfont) == sizeof(logfont))
		{
			logfont.lfCharSet = ANSI_CHARSET;
			_tcscpy_s(logfont.lfFaceName, _T("Consolas"));
			CFont font;
			if (font.CreateFontIndirect(&logfont))
			{
				m_edit.SetFont(&font);
				font.Detach();
			}
		}
	}

	// エディットコントロールの文字数制限を解除 
	m_edit.SetLimitText(0);

	return 0;
}
