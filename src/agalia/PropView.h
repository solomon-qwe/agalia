#pragma once


// PropView

class PropView : public CWnd
{
	DECLARE_DYNAMIC(PropView)

public:
	PropView();
	virtual ~PropView();

public:
	CListCtrl m_listCtrl;
	HBITMAP m_hBitmap = NULL;
	bool pos_initialized = false;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};


