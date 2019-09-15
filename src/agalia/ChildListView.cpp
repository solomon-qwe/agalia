// ChildListView.cpp : implementation file
//

#include "pch.h"
#include "agalia.h"
#include "ChildListView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ChildListView

IMPLEMENT_DYNCREATE(ChildListView, CListView)

ChildListView::ChildListView()
{

}

ChildListView::~ChildListView()
{
}

BEGIN_MESSAGE_MAP(ChildListView, CListView)
END_MESSAGE_MAP()


// ChildListView diagnostics

#ifdef _DEBUG
void ChildListView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void ChildListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// ChildListView message handlers


BOOL ChildListView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= LVS_REPORT;

	return CListView::PreCreateWindow(cs);
}
