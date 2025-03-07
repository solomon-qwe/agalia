// GraphicView.cpp : implementation file
//

#include "pch.h"
#include "GraphicView.h"
#include "ChildGraphicView.h"
#include "ChildGraphicViewD2D1_3.h"

#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// GraphicView

IMPLEMENT_DYNAMIC(GraphicView, CWnd)

GraphicView::GraphicView()
{
	auto p = new std::vector<CWnd*>;
	p->assign(3, nullptr);
	children = p;
}


GraphicView::~GraphicView()
{
	if (children)
	{
		auto temp = static_cast<std::vector<CWnd*>*>(children);
		children = nullptr;
		for (auto p : *temp)
			delete p;
		delete temp;
	}
}

BEGIN_MESSAGE_MAP(GraphicView, CWnd)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// GraphicView message handlers

void GraphicView::create(CWnd* pParent)
{
	Create(
		::AfxRegisterWndClass(0),
		_T(""),
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		CRect(0, 0, 0, 0), pParent, (UINT)-1);

	auto p = static_cast<std::vector<CWnd*>*>(children);

	auto child1 = new ChildGraphicView;
	child1->init_window(this, 0);

	auto child2 = new ChildGraphicView;
	child2->init_window(this, 1);

	auto child3 = new ChildGraphicViewD2D1_3;
	child3->init_window(this);

	p->at(0) = child1;
	p->at(1) = child2;
	p->at(2) = child3;
}

void GraphicView::update_setting(int rendering_engine, int monitor_color_profile)
{
	int old_renderingMode = renderingMode;
	renderingMode = rendering_engine;
	colorManagementMode = monitor_color_profile;

	auto p = static_cast<std::vector<CWnd*>*>(children);
	if (p == nullptr)
		return;

	if (old_renderingMode != renderingMode)
	{
		auto child = p->at(renderingMode);
		if (child) {
			switch (renderingMode)
			{
			case 0:
			case 1:
				static_cast<ChildGraphicView*>(child)->reset_color_profile(monitor_color_profile);
				break;
			case 2:
				static_cast<ChildGraphicViewD2D1_3*>(child)->update_setting(rendering_engine, monitor_color_profile);
				break;
			}
			CRect rc;
			GetClientRect(&rc);
			child->SetWindowPos(nullptr, 0, 0, rc.Width(), rc.Height(), SWP_SHOWWINDOW | SWP_NOZORDER);
		}

		if (old_renderingMode != -1)
		{
			child = p->at(old_renderingMode);
			if (child) {
				child->ShowWindow(SW_HIDE);
			}
		}
	}
	else
	{
		auto child = p->at(renderingMode);
		if (child) {
			switch (renderingMode)
			{
			case 0:
			case 1:
				static_cast<ChildGraphicView*>(child)->reset_color_profile(monitor_color_profile);
				break;
			case 2:
				static_cast<ChildGraphicViewD2D1_3*>(child)->update_setting(rendering_engine, monitor_color_profile);
				break;
			}
		}
	}
}


void GraphicView::reset_color_profile(void)
{
	auto p = static_cast<std::vector<CWnd*>*>(children);
	switch (renderingMode)
	{
	case 0:
	case 1:
		static_cast<ChildGraphicView*>(p->at(renderingMode))->reset_color_profile(-1);
		break;
	case 2:
		static_cast<ChildGraphicViewD2D1_3*>(p->at(renderingMode))->reset_color_profile();
		break;
	}
}

void GraphicView::reset_content(agaliaContainer* image)
{
	auto p = static_cast<std::vector<CWnd*>*>(children);
	switch (renderingMode)
	{
	case 0:
	case 1:
		static_cast<ChildGraphicView*>(p->at(renderingMode))->reset_content(image);
		break;
	case 2:
		static_cast<ChildGraphicViewD2D1_3*>(p->at(renderingMode))->reset_content(image);
		break;
	}
}


void GraphicView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (0 <= renderingMode && renderingMode < 3)
	{
		auto p = static_cast<std::vector<CWnd*>*>(children);
		auto child = p->at(renderingMode);
		if (child)
		{
			child->SetWindowPos(nullptr, 0, 0, cx, cy, SWP_NOZORDER);
		}
	}
}
