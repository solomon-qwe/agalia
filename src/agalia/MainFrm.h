
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "TagListView.h"
#include "GraphicView.h"
#include "PropView.h"
#include <stdint.h>

#include "controller.h"
class agaliaItem;

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame() noexcept;
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:
	controller ctrl;
	uint32_t thumbnail_area_size = 0;

// Operations
public:
	void ResetContents(const wchar_t* path, uint64_t offset, uint64_t size, int format);

protected:
	void DeleteAllContents(void);
	void ResetGraphicView(void);
	void ResetHierarchyView(void);
	void ResetFlatView(void);
	void UpdateTextViewPane(const agaliaItem* item);
	void UpdateListViewPane(const agaliaItem* item);

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CWnd* m_pCurWnd = nullptr;
	CSplitterWnd	m_wndVSplitter;
	CSplitterWnd	m_wndHSplitter;
	TagListView		m_wndTagListView;
	GraphicView		m_wndGraphicView;
	PropView		m_wndPropView;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()

protected:
	int ResetViewLayout();
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
public:
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	afx_msg void OnDestroy();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnExitSizeMove();
	afx_msg void OnFileOpen();
	afx_msg void OnEditOptions();
	afx_msg void OnViewProperty();
	afx_msg void OnViewRange(UINT id);
protected:
	afx_msg LRESULT OnApp(WPARAM wParam, LPARAM lParam);
public:
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


