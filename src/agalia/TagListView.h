#pragma once


// CTagListView

class CTagListView : public CListCtrl
{
	DECLARE_DYNAMIC(CTagListView)

public:
	CTagListView();
	virtual ~CTagListView();

	CString m_strFilePath;
	CFindReplaceDialog* m_pFindDlg;

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnCopyToClipboardCSV();
	afx_msg void OnShowImage();
	afx_msg void OnDestroy();
	afx_msg void OnSaveDump();
	afx_msg void OnAnalyze();
	afx_msg void OnCopyToClipboardTab();
	afx_msg void OnSelectAll();
	afx_msg void OnEditCopy();
	afx_msg void OnShowTextAscii();
	afx_msg void OnShowTextLatin1();
	afx_msg void OnShowTextUtf16();
	afx_msg void OnShowTextUtf8();
	afx_msg void OnShowTextJis();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditSelectAll(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCommand(CCmdUI* pCmdUI, UINT id);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	void OpenPopup(POINT& pos);
	afx_msg void OnEditFind();
	afx_msg void OnUpdateEditFind(CCmdUI *pCmdUI);
	afx_msg LRESULT OnFindReplace(WPARAM wParam, LPARAM lParam);
};
