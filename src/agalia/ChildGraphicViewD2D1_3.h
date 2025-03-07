#pragma once


// ChildGraphicViewD2D1_3

class D2D1_3_Renderer;
class agaliaContainer;

class ChildGraphicViewD2D1_3 : public CWnd
{
	DECLARE_DYNAMIC(ChildGraphicViewD2D1_3)

public:
	ChildGraphicViewD2D1_3();
	virtual ~ChildGraphicViewD2D1_3();

	void init_window(CWnd* pParent);
	void update_setting(int rendering_engine, int monitor_color_profile);
	void reset_color_profile(void);
	void reset_content(agaliaContainer* image);
	void reset_scale(void);

protected:
	D2D1_3_Renderer* renderer = nullptr;
	int colorManagementMode = -1;
	CPoint prePosition;
	int scaleIndex = 0;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
};
