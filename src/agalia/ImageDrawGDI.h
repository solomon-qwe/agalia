#pragma once

#include "ImageDraw.h"
#include "../inc/agaliarept.h"

class ImageDrawGDI : public ImageDraw
{
public:
	ImageDrawGDI();
	virtual ~ImageDrawGDI();

	void attach(HWND target);
	void detach(void);

	virtual HRESULT reset_content(agaliaContainer* image, int colorManagementMode);
	virtual HRESULT reset_color_profile(int colorManagementMode);
	virtual HRESULT update_for_window_size_change(void);
	virtual HRESULT render(DWORD bkcolor);
	virtual HRESULT offset(int x, int y);
	virtual HRESULT set_scale(FLOAT scale);

protected:
	void UpdateImagePosition(void);

protected:
	HWND hwnd = NULL;
	CHeapPtr<BITMAPV5HEADER> bmpInfoWithProfile;
	agaliaPtr<agaliaBitmap> source_bmp;
	agaliaPtr<agaliaBitmap> offscreen_bmp;
	CPoint ptOffset;
	FLOAT fScale = 1.f;

	CRect rcSrc;
	CRect rcDst;
};

