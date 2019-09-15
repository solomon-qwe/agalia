#pragma once


// ChildListView view

class ChildListView : public CListView
{
	DECLARE_DYNCREATE(ChildListView)

protected:
	ChildListView();           // protected constructor used by dynamic creation
	virtual ~ChildListView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


