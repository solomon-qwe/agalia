#pragma once


// TagListView

class TagListView : public CListCtrl
{
	DECLARE_DYNAMIC(TagListView)

public:
	TagListView();
	virtual ~TagListView();

	CFindReplaceDialog* m_pFindDlg = nullptr;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnCopyToClipboardCSV();
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
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCommand(CCmdUI* pCmdUI, UINT id);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	void OpenPopup(POINT& pos);
	afx_msg void OnEditFind();
	afx_msg void OnUpdateEditFind(CCmdUI* pCmdUI);
	afx_msg LRESULT OnFindReplace(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLvnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
};
