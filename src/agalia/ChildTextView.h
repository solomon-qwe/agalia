#pragma once


// ChildTextView

class ChildTextView : public CWnd
{
	DECLARE_DYNCREATE(ChildTextView)

public:
	ChildTextView();
	virtual ~ChildTextView();

protected:
	CEdit m_edit;
public:
	CEdit& GetEditCtrl(void) { return m_edit; }

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


