#pragma once

class agaliaContainer;

enum colorManagementMode
{
	color_management_disable,
	color_management_sRGB,
	color_management_system
};

enum renderingMode
{
	rendering_GDI,
	rendering_D2D_1_1,
	rendering_D2D_1_3
};

class ImageDraw
{
public:
	virtual ~ImageDraw() {}

	virtual void attach(HWND hwnd) = 0;
	virtual void detach(void) = 0;

	virtual HRESULT reset_content(agaliaContainer* image, int colorManagementMode) = 0;
	virtual HRESULT reset_color_profile(int colorManagementMode) = 0;
	virtual HRESULT update_for_window_size_change(void) = 0;
	virtual HRESULT render(DWORD bkcolor) = 0;
	virtual HRESULT offset(int x, int y) = 0;
	virtual HRESULT set_scale(FLOAT scale) = 0;
};
