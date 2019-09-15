#pragma once


// ChildTreeView view

class ChildTreeView : public CTreeView
{
	DECLARE_DYNCREATE(ChildTreeView)

protected:
	ChildTreeView();           // protected constructor used by dynamic creation
	virtual ~ChildTreeView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTvnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


