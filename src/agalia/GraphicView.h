#pragma once

class agaliaContainer;

// GraphicView

class GraphicView : public CWnd
{
	DECLARE_DYNAMIC(GraphicView)

public:
	GraphicView();
	virtual ~GraphicView();

	void create(CWnd* pParent);
	void update_setting(int rendering_engine, int monitor_color_profile);
	void reset_color_profile(void);
	void reset_content(agaliaContainer* image);

protected:
	void* children = nullptr;
	int renderingMode = -1;
	int colorManagementMode = -1;


protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

