// ChildTreeView.cpp : implementation file
//

#include "pch.h"
#include "agalia.h"
#include "ChildTreeView.h"

#include "../inc/agaliarept.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ChildTreeView

IMPLEMENT_DYNCREATE(ChildTreeView, CTreeView)

ChildTreeView::ChildTreeView()
{

}

ChildTreeView::~ChildTreeView()
{
}

BEGIN_MESSAGE_MAP(ChildTreeView, CTreeView)
#pragma warning(suppress: 26454)	// warning C26454: Arithmetic overflow: '-' operation produces a negative unsigned result at compile time (io.5). 
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, &ChildTreeView::OnTvnDeleteitem)
#pragma warning(suppress: 26454)
	ON_NOTIFY_REFLECT(TVN_ITEMCHANGED, &ChildTreeView::OnTvnItemChanged)
END_MESSAGE_MAP()


// ChildTreeView diagnostics

#ifdef _DEBUG
void ChildTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

#ifndef _WIN32_WCE
void ChildTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif
#endif //_DEBUG


// ChildTreeView message handlers

void ChildTreeView::OnTvnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if ((pNMTreeView->itemOld.mask & TVIF_PARAM) && pNMTreeView->itemOld.lParam)
	{
		agaliaItem* p = reinterpret_cast<agaliaItem*>(pNMTreeView->itemOld.lParam);
		p->Release();
	}

	*pResult = 0;
}


void ChildTreeView::OnTvnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTVITEMCHANGE* pNMTVItemChange = reinterpret_cast<NMTVITEMCHANGE*>(pNMHDR);

	if ((pNMTVItemChange->uStateNew & TVIS_SELECTED) && pNMTVItemChange->lParam)
	{
		::AfxGetMainWnd()->PostMessageW(WM_APP, 0, pNMTVItemChange->lParam);
	}

	*pResult = 0;
}


BOOL ChildTreeView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_FULLROWSELECT;

	return CTreeView::PreCreateWindow(cs);
}
