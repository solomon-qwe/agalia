#pragma once

#include "ImageDraw.h"

//#include <stdint.h>

struct InternalImageDrawD2DParam;

class ImageDrawD2D : public ImageDraw
{
public:
	ImageDrawD2D() {}
	virtual ~ImageDrawD2D() {}

	virtual void attach(HWND hwnd);
	virtual void detach(void);

	virtual HRESULT reset_content(agaliaContainer* image, int colorManagementMode);
	virtual HRESULT reset_color_profile(int colorManagementMode);
	virtual HRESULT update_for_window_size_change(void);
	virtual HRESULT render(DWORD bkcolor);

protected:
	HWND hwnd = NULL;
	InternalImageDrawD2DParam* _p = nullptr;
};

