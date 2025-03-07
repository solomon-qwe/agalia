// ChildGraphicView.cpp : implementation file
//

#include "pch.h"
#include "agalia.h"
#include "ChildGraphicView.h"

#include "ImageDrawD2D.h"
#include "ImageDrawGDI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ChildGraphicView

IMPLEMENT_DYNAMIC(ChildGraphicView, CWnd)

ChildGraphicView::ChildGraphicView()
{
}


ChildGraphicView::~ChildGraphicView()
{
	if (v)
	{
		auto temp = v;
		v = nullptr;
		delete temp;
	}
}

void ChildGraphicView::init_window(CWnd* pParent, int mode)
{
	if (::IsWindow(GetSafeHwnd())) {
		DestroyWindow();
	}

	Create(
		::AfxRegisterWndClass(CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW)),
		_T(""),
		WS_CHILD,
		CRect(0, 0, 0, 0), pParent, (UINT)-1);

	reset_renderer(mode);
}

void ChildGraphicView::reset_renderer(int mode)
{
	if (0 <= mode) {
		renderingMode = mode;
	}

	if (v)
	{
		auto temp = v;
		v = nullptr;
		temp->detach();
		delete temp;
	}

	switch (renderingMode)
	{
	case rendering_GDI:
		v = new ImageDrawGDI;
		break;
	case rendering_D2D_1_1:
	default:
		v = new ImageDrawD2D;
		break;
	}

	scaleIndex = 0;
	reset_scale();
}

void ChildGraphicView::reset_content(agaliaContainer* image)
{
	if (!v) return;

	v->attach(GetSafeHwnd());
	v->reset_content(image, colorManagementMode);

	if (::IsWindow(GetSafeHwnd()) && IsWindowVisible())
		Invalidate(FALSE);
}


void ChildGraphicView::reset_color_profile(int mode)
{
	if (0 <= mode) {
		colorManagementMode = mode;
	}

	if (!v) return;
	v->reset_color_profile(colorManagementMode);

	if (::IsWindow(GetSafeHwnd()) && IsWindowVisible())
		Invalidate(FALSE);
}


void ChildGraphicView::render(void)
{
	DWORD bkcolor = ::GetSysColor(COLOR_APPWORKSPACE);

	HRESULT hr = E_FAIL;
	if (v) {
		hr = v->render(bkcolor);
	}

	if (FAILED(hr))
	{
		if (::IsWindow(GetSafeHwnd()) && IsWindowVisible())
		{
			CRect rc;
			GetClientRect(&rc);

			CClientDC dc(this);
			dc.FillSolidRect(&rc, bkcolor);
		}
	}
}


void ChildGraphicView::reset_scale(void)
{
	if (!v) return;

	const FLOAT scales[] = { 0.125f, 0.25f, 0.50f, 0.75f, 1.00f, 1.25f, 1.50f, 2.00f, 3.00f, 4.00f, 5.00f };
	const int idx_org = 4;

	int idx = min(max(0, scaleIndex + idx_org), _countof(scales) - 1);
	scaleIndex = idx - idx_org;
	v->set_scale(scales[idx]);
}



BEGIN_MESSAGE_MAP(ChildGraphicView, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEHWHEEL()
END_MESSAGE_MAP()


// ChildGraphicView message handlers


int ChildGraphicView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


void ChildGraphicView::OnDestroy()
{
	if (v)
		v->detach();

	CWnd::OnDestroy();
}


void ChildGraphicView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (v) {
		v->update_for_window_size_change();
		render();
	}
}


void ChildGraphicView::OnPaint()
{
	CPaintDC dc(this);

	DWORD bkcolor = ::GetSysColor(COLOR_APPWORKSPACE);

	HRESULT hr = E_FAIL;
	if (v) {
		hr = v->render(bkcolor);
	}

	if (FAILED(hr)) {
		dc.FillSolidRect(&dc.m_ps.rcPaint, bkcolor);
	}
}


void ChildGraphicView::OnLButtonDown(UINT nFlags, CPoint point)
{
	::SetCapture(GetSafeHwnd());
	prePosition = point;

	CWnd::OnLButtonDown(nFlags, point);
}


void ChildGraphicView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (::GetCapture() == GetSafeHwnd())
		::ReleaseCapture();

	CWnd::OnLButtonUp(nFlags, point);
}


void ChildGraphicView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (::GetCapture() == GetSafeHwnd())
	{
		if (v)
		{
			int x = point.x - prePosition.x;
			int y = point.y - prePosition.y;
			prePosition.x = point.x;
			prePosition.y = point.y;

			HRESULT hr = v->offset(x, y);
			if (SUCCEEDED(hr))
			{
				DWORD bkcolor = ::GetSysColor(COLOR_APPWORKSPACE);
				v->render(bkcolor);
			}
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}


BOOL ChildGraphicView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (v)
	{
		if (zDelta)
		{
			if (::GetKeyState(VK_CONTROL) < 0)
			{
				// Ctrlキーが押されていれば拡縮 
				int shift = (zDelta < 0) ? -1 : 1;
				scaleIndex += shift;

				reset_scale();
				v->update_for_window_size_change();
				render();
			}
			else if (::GetKeyState(VK_SHIFT) < 0)
			{
				// Shiftキーが押されていれば左右スクロール 
				HRESULT hr = v->offset(zDelta, 0);
				if (SUCCEEDED(hr))
				{
					DWORD bkcolor = ::GetSysColor(COLOR_APPWORKSPACE);
					v->render(bkcolor);
				}
			}
			else
			{
				// いずれでもなければ上下スクロール
				HRESULT hr = v->offset(0, zDelta);
				if (SUCCEEDED(hr))
				{
					DWORD bkcolor = ::GetSysColor(COLOR_APPWORKSPACE);
					v->render(bkcolor);
				}
			}
		}
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


void ChildGraphicView::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (v)
	{
		if (zDelta)
		{
			if (::GetKeyState(VK_CONTROL) < 0)
			{
				// 何もしない 
			}
			else
			{
				HRESULT hr = v->offset(-zDelta, 0);
				if (SUCCEEDED(hr))
				{
					DWORD bkcolor = ::GetSysColor(COLOR_APPWORKSPACE);
					v->render(bkcolor);
				}
			}
		}
	}

	CWnd::OnMouseHWheel(nFlags, zDelta, pt);
}
