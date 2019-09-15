#pragma once

#include <string>

// CTextViewer

class CTextViewer : public CWnd
{
	DECLARE_DYNAMIC(CTextViewer)

public:
	CTextViewer();
	virtual ~CTextViewer();

	typedef enum type
	{
		ascii,
		latin1,
		utf16,
		utf8,
		jis
	} type_t;

	static void CreateViewer(LPARAM lParam, type_t type);

public:
	std::wstring path;
	uint64_t offset;
	uint32_t size;
	type type;
	bool replace_terminator;

protected:
	CEdit m_child;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void PostNcDestroy();
	afx_msg void OnWrap();
	afx_msg void OnFileClose();
	afx_msg void OnReplaceTerminator();
	afx_msg void OnEditCopy();
	afx_msg void OnSelectAll();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnUpdateReplaceTerminator(CCmdUI *pCmdUI);
	afx_msg void OnUpdateWrap(CCmdUI *pCmdUI);
};


