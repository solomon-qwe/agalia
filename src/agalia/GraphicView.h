#pragma once

#include <stdint.h>

#include "ImageDraw.h"

class agaliaContainer;

// GraphicView

class GraphicView : public CWnd
{
	DECLARE_DYNAMIC(GraphicView)

public:
	GraphicView();
	virtual ~GraphicView();

public:
	void init_window(void);
	void reset_renderer(int mode);
	void reset_content(agaliaContainer* image);
	void reset_color_profile(int mode);
	void render(void);
	void reset_scale(void);

protected:
	ImageDraw* v = nullptr;
	int renderingMode = -1;
	int colorManagementMode = -1;
	CPoint prePosition;
	int scaleIndex = 0;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
};


