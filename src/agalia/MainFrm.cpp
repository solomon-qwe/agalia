
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "agalia.h"

#include "MainFrm.h"

#include "AgaliaController.h"
#include "OptionDlg.h"
#include "OpenFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_EDIT_OPTIONS, &CMainFrame::OnEditOptions)
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create a view to occupy the client area of the frame
	//if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
	//	CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	if (!m_wndView.Create(
		AFX_WS_DEFAULT_VIEW | LVS_REPORT | LVS_SHOWSELALWAYS,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	DWORD dwStyle = m_wndView.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_HEADERDRAGDROP;
	m_wndView.SetExtendedStyle(dwStyle);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// viewer 
	CWnd* pWnd = GetFocus();
	if (pWnd &&
		pWnd->GetSafeHwnd() != m_wndView.GetSafeHwnd())
	{
		pWnd = pWnd->GetParent();
		if (pWnd &&
			pWnd->GetSafeHwnd() != GetSafeHwnd() &&
			pWnd->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		{
			return TRUE;
		}
		return FALSE;
	}

	// list 
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


BOOL CMainFrame::OnEraseBkgnd(CDC* /*pDC*/)
{
	//	return CFrameWnd::OnEraseBkgnd(pDC);
	return TRUE;
}


void CMainFrame::OnClose()
{
	CAgaliaApp* pApp = dynamic_cast<CAgaliaApp*>(::AfxGetApp());
	if (!pApp->pController->IsProcessing())
	{
		CFrameWnd::OnClose();
	}
}


void CMainFrame::OnEditOptions()
{
	COptionDlg dlg(this);
	dlg.DoModal();
}


void CMainFrame::OnFileOpen()
{
	COpenFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK)
	{
		uint64_t offset = _wtoi64(dlg.strOffset);
		uint64_t size = _wtoi64(dlg.strSize);
		theApp.pController->ResetContents(dlg.GetOFN().lpstrFile, offset, size);
	}
}
