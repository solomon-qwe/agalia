// GraphicView.cpp : implementation file
//

#include "pch.h"
#include "agalia.h"
#include "GraphicView.h"

#include "ImageDrawD2D.h"
#include "ImageDrawGDI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// GraphicView

IMPLEMENT_DYNAMIC(GraphicView, CWnd)

GraphicView::GraphicView()
{
}


GraphicView::~GraphicView()
{
	if (v)
	{
		auto temp = v;
		v = nullptr;
		delete temp;
	}
}

void GraphicView::init_window(void)
{
	if (::IsWindow(GetSafeHwnd())) {
		DestroyWindow();
	}

	Create(
		::AfxRegisterWndClass(CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW)),
		_T(""),
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		CRect(0, 0, 0, 0), ::AfxGetMainWnd(), (UINT)-1);
}

void GraphicView::reset_renderer(int mode)
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

		init_window();
	}

	if (renderingMode == 0) {
		v = new ImageDrawGDI;
	}
	else {
		v = new ImageDrawD2D;
	}
}

void GraphicView::reset_content(agaliaContainer* image)
{
	if (!v) return;

	v->attach(GetSafeHwnd());
	v->reset_content(image, colorManagementMode);
	render();

	if (::IsWindow(GetSafeHwnd()) && IsWindowVisible())
		Invalidate(FALSE);
}


void GraphicView::reset_color_profile(int mode)
{
	if (0 <= mode) {
		colorManagementMode = mode;
	}

	if (!v) return;
	v->reset_color_profile(colorManagementMode);

	if (::IsWindow(GetSafeHwnd()) && IsWindowVisible())
		Invalidate(FALSE);
}


void GraphicView::render(void)
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


BEGIN_MESSAGE_MAP(GraphicView, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// GraphicView message handlers


int GraphicView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


void GraphicView::OnDestroy()
{
	if (v)
		v->detach();

	CWnd::OnDestroy();
}


void GraphicView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (v) {
		v->update_for_window_size_change();
		render();
	}
}


void GraphicView::OnPaint()
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
