// TagListView.cpp : 
//

#include "stdafx.h"
#include "agalia.h"
#include "TagListView.h"
#include "ImageViewer.h"
#include "TextViewer.h"

#include "AgaliaController.h"

#include "CSVString.h"
#include "util.h"
#include <vector>


template <class T>
void copy_to_clipboard(CListCtrl* pCtrl, T& dst)
{
	int columns = pCtrl->GetHeaderCtrl()->GetItemCount();

	std::vector<int> v(columns);
	pCtrl->GetColumnOrderArray(v.data(), columns);

	// タイトル行 
	for (int i = 0; i < columns; i++)
	{
		wchar_t buf[128] = {};
		HDITEM hdi = {};
		hdi.mask = HDI_TEXT;
		hdi.pszText = buf;
		hdi.cchTextMax = _countof(buf);

		pCtrl->GetHeaderCtrl()->GetItem(v[i], &hdi);
		dst.Add(hdi.pszText);
	}

	// データ行 
	POSITION pos = pCtrl->GetFirstSelectedItemPosition();
	while (pos)
	{
		dst.Next();

		int index = pCtrl->GetNextSelectedItem(pos);
		for (int i = 0; i < columns; i++)
		{
			dst.Add(pCtrl->GetItemText(index, v[i]));
		}
	}

	// クリップボードにコピー 
	if (::OpenClipboard(pCtrl->GetSafeHwnd()))
	{
		int length = dst.GetLength() + 1;
		HGLOBAL hMem = ::GlobalAlloc(GHND, sizeof(wchar_t) * length);
		if (hMem)
		{
			LPVOID p = ::GlobalLock(hMem);
			if (p)
			{
				memcpy(p, dst.GetBuffer(length), sizeof(wchar_t) * length);
				::GlobalUnlock(hMem);
				::EmptyClipboard();
				::SetClipboardData(CF_UNICODETEXT, hMem);
			}
		}
		::CloseClipboard();
	}
}


// CTagListView

static UINT WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);

IMPLEMENT_DYNAMIC(CTagListView, CListCtrl)

CTagListView::CTagListView()
{
	m_pFindDlg = nullptr;
}

CTagListView::~CTagListView()
{
}

// ON_UPDATE_COMMAND_UI_RANGE() はバグがあるので自前で定義 
#define ON_MY_UPDATE_COMMAND_UI_RANGE(id, idLast, memberFxn) \
	{ WM_COMMAND, CN_UPDATE_COMMAND_UI, (WORD)id, (WORD)idLast, AfxSigCmdUI_RANGE, \
		(AFX_PMSG) \
		(static_cast< void (AFX_MSG_CALL CCmdTarget::*)(CCmdUI*,UINT) > \
		(memberFxn)) },

BEGIN_MESSAGE_MAP(CTagListView, CListCtrl)
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
	ON_WM_INITMENUPOPUP()
	ON_WM_CONTEXTMENU()
	ON_REGISTERED_MESSAGE(WM_FINDREPLACE, &CTagListView::OnFindReplace)
	ON_MY_UPDATE_COMMAND_UI_RANGE(IDM_COPY_TO_CLIPBOARD_TAB, IDM_ANALYZE, &CTagListView::OnUpdateCommand)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CTagListView::OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, &CTagListView::OnUpdateEditFind)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, &CTagListView::OnUpdateEditSelectAll)
	ON_COMMAND(IDM_COPY_TO_CLIPBOARD_TAB, &CTagListView::OnCopyToClipboardTab)
	ON_COMMAND(IDM_COPY_TO_CLIPBOARD_CSV, &CTagListView::OnCopyToClipboardCSV)
	ON_COMMAND(IDM_SHOW_TEXT_ASCII, &CTagListView::OnShowTextAscii)
	ON_COMMAND(IDM_SHOW_TEXT_LATIN1, &CTagListView::OnShowTextLatin1)
	ON_COMMAND(IDM_SHOW_TEXT_UTF16, &CTagListView::OnShowTextUtf16)
	ON_COMMAND(IDM_SHOW_TEXT_UTF8, &CTagListView::OnShowTextUtf8)
	ON_COMMAND(IDM_SHOW_TEXT_JIS, &CTagListView::OnShowTextJis)
	ON_COMMAND(IDM_SHOW_IMAGE, &CTagListView::OnShowImage)
	ON_COMMAND(IDM_SAVE_DUMP, &CTagListView::OnSaveDump)
	ON_COMMAND(IDM_ANALYZE, &CTagListView::OnAnalyze)
	ON_COMMAND(ID_EDIT_COPY, &CTagListView::OnEditCopy)
	ON_COMMAND(ID_EDIT_FIND, &CTagListView::OnEditFind)
	ON_COMMAND(ID_EDIT_SELECT_ALL, &CTagListView::OnSelectAll)
END_MESSAGE_MAP()

// PreCreateWindow virtual function 
BOOL CTagListView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.dwExStyle |= WS_EX_CLIENTEDGE | WS_EX_ACCEPTFILES;
	cs.style &= ~WS_BORDER;

	return CListCtrl::PreCreateWindow(cs);
}

// WM_INIT_MENU_POPUP message handler 
void CTagListView::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	::OnInitMenuPopup(this, pPopupMenu, nIndex, bSysMenu);
}

// WM_DESTROY message handler 
void CTagListView::OnDestroy()
{
	LVITEM item = { 0 };
	item.mask = LVIF_PARAM;

	int counts = GetItemCount();
	for (int i = 0; i < counts; i++)
	{
		item.iItem = i;
		GetItem(&item);

		LPARAM p = item.lParam;
		item.lParam = NULL;
		if (p) {
			::CoTaskMemFree(reinterpret_cast<LPVOID>(p));
		}
	}

	CListCtrl::OnDestroy();
}

// WM_DROPFILES message handler 
void CTagListView::OnDropFiles(HDROP hDropInfo)
{
	TCHAR szFilePath[_MAX_PATH] = {};
	UINT ret = ::DragQueryFile(hDropInfo, 0, szFilePath, _countof(szFilePath));
	if (ret != 0) {
		theApp.pController->ResetContents(szFilePath);
	}

	CListCtrl::OnDropFiles(hDropInfo);
}

// WM_CONTEXT_MENU message handler 
void CTagListView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	int focused = GetNextItem(-1, LVNI_FOCUSED);
	if (focused < 0) {
		return;
	}

	CPoint pos = point;
	if (point.x < 0)
	{
		CRect rc;
		GetItemRect(focused, rc, LVIR_BOUNDS);
		pos = rc.CenterPoint();
		ClientToScreen(&pos);
	}

	OpenPopup(pos);
}

// IDM_COPY_TO_CLIPBOARD_TAB command message handler 
void CTagListView::OnCopyToClipboardTab()
{
	CTSVString dst;
	copy_to_clipboard(this, dst);
}

// IDM_COPY_TO_CLIPBOARD_CSV command message handler 
void CTagListView::OnCopyToClipboardCSV()
{
	CCSVString dst;
	copy_to_clipboard(this, dst);
}

// IDM_SHOW_TEXT_ASCII command message handler 
void CTagListView::OnShowTextAscii()
{
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL) {
		return;
	}

	LVITEM item = { 0 };
	item.mask = LVIF_PARAM;
	item.iItem = GetNextSelectedItem(pos);
	GetItem(&item);
	if (!item.lParam) {
		return;
	}

	CTextViewer::CreateViewer(m_strFilePath, item.lParam, CTextViewer::ascii);
}

// IDM_SHOW_TEXT_LATIN1 command message handler 
void CTagListView::OnShowTextLatin1()
{
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL) {
		return;
	}

	LVITEM item = { 0 };
	item.mask = LVIF_PARAM;
	item.iItem = GetNextSelectedItem(pos);
	GetItem(&item);
	if (!item.lParam) {
		return;
	}

	CTextViewer::CreateViewer(m_strFilePath, item.lParam, CTextViewer::latin1);
}

// IDM_SHOW_TEXT_UTF16 command message handler 
void CTagListView::OnShowTextUtf16()
{
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL) {
		return;
	}

	LVITEM item = { 0 };
	item.mask = LVIF_PARAM;
	item.iItem = GetNextSelectedItem(pos);
	GetItem(&item);
	if (!item.lParam) {
		return;
	}

	CTextViewer::CreateViewer(m_strFilePath, item.lParam, CTextViewer::utf16);
}

// IDM_SHOW_TEXT_UTF8 command message handler 
void CTagListView::OnShowTextUtf8()
{
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL) {
		return;
	}

	LVITEM item = { 0 };
	item.mask = LVIF_PARAM;
	item.iItem = GetNextSelectedItem(pos);
	GetItem(&item);
	if (!item.lParam) {
		return;
	}

	CTextViewer::CreateViewer(m_strFilePath, item.lParam, CTextViewer::utf8);
}

// IDM_SHOW_TEXT_JIS command message handler 
void CTagListView::OnShowTextJis()
{
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL) {
		return;
	}

	LVITEM item = { 0 };
	item.mask = LVIF_PARAM;
	item.iItem = GetNextSelectedItem(pos);
	GetItem(&item);
	if (!item.lParam) {
		return;
	}

	CTextViewer::CreateViewer(m_strFilePath, item.lParam, CTextViewer::jis);
}

// IDM_SHOW_IMAGE command message handler 
void CTagListView::OnShowImage()
{
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL) {
		return;
	}

	LVITEM item = { 0 };
	item.mask = LVIF_PARAM;
	item.iItem = GetNextSelectedItem(pos);
	GetItem(&item);
	if (!item.lParam) {
		return;
	}

	CImageViewer::CreateViewer(m_strFilePath, item.lParam);
}

// IDM_SAVE_DUMP command message handler 
void CTagListView::OnSaveDump()
{
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL) {
		return;
	}

	LVITEM item = { 0 };
	item.mask = LVIF_PARAM;
	item.iItem = GetNextSelectedItem(pos);
	GetItem(&item);
	if (!item.lParam) {
		return;
	}

	dump(m_strFilePath, item.lParam);
}

// IDM_ANALYZE command message handler 
void CTagListView::OnAnalyze()
{
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL) {
		return;
	}

	LVITEM item = { 0 };
	item.mask = LVIF_PARAM;
	item.iItem = GetNextSelectedItem(pos);
	GetItem(&item);
	if (!item.lParam) {
		return;
	}

	analyze(m_strFilePath, item.lParam);
}

// ID_SELECT_ALL command message handler 
void CTagListView::OnSelectAll()
{
	int counts = GetItemCount();
	for (int i = 0; i < counts; i++)
	{
		SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}
}

// ID_EDIT_COPY command message handler 
void CTagListView::OnEditCopy()
{
	OnCopyToClipboardTab();
}

// ID_EDIT_COPY update command UI 
void CTagListView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetFirstSelectedItemPosition() != NULL);
}

// ID_EDIT_SELECT_ALL update command UI 
void CTagListView::OnUpdateEditSelectAll(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetItemCount() != 0);
}

void CTagListView::OnUpdateCommand(CCmdUI* pCmdUI, UINT id)
{
	UINT selected_count = GetSelectedCount();

	BOOL enable = FALSE;
	switch (id)
	{
	case IDM_COPY_TO_CLIPBOARD_TAB:
	case IDM_COPY_TO_CLIPBOARD_CSV:
		enable = (0 < selected_count);
		break;

	default:
		if (selected_count == 1)
		{
			POSITION pos = GetFirstSelectedItemPosition();
			if (pos != NULL)
			{
				LVITEM item = { 0 };
				item.mask = LVIF_PARAM;
				item.iItem = GetNextSelectedItem(pos);
				GetItem(&item);
				enable = (item.lParam != NULL);
			}
		}
	}

	pCmdUI->Enable(enable);
}


void CTagListView::OpenPopup(POINT& pos)
{
	CMenu menu;
	menu.CreatePopupMenu();

	menu.AppendMenu(MF_STRING, IDM_COPY_TO_CLIPBOARD_TAB, _T("Copy to Clipboard (TAB)	Ctrl+C"));
	menu.AppendMenu(MF_STRING, IDM_COPY_TO_CLIPBOARD_CSV, _T("Copy to Clipboard (CSV)"));
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, IDM_SHOW_TEXT_ASCII, _T("Show Text (ASCII)"));
	menu.AppendMenu(MF_STRING, IDM_SHOW_TEXT_LATIN1, _T("Show Text (Latin-1)"));
	menu.AppendMenu(MF_STRING, IDM_SHOW_TEXT_UTF8, _T("Show Text (UTF-8)"));
	menu.AppendMenu(MF_STRING, IDM_SHOW_TEXT_UTF16, _T("Show Text (UTF-16)"));
	menu.AppendMenu(MF_STRING, IDM_SHOW_TEXT_JIS, _T("Show Text (JIS)"));
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, IDM_SHOW_IMAGE, _T("Show Image"));
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, IDM_SAVE_DUMP, _T("Save Dump As..."));
	menu.AppendMenu(MF_STRING, IDM_ANALYZE, _T("Analyze"));

	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this);
}


void CTagListView::OnEditFind()
{
	if (m_pFindDlg != nullptr) {
		m_pFindDlg->SetFocus();
		return;
	}

	m_pFindDlg = new CFindReplaceDialog;
	m_pFindDlg->Create(TRUE, NULL, NULL, FR_DOWN, this);

}


void CTagListView::OnUpdateEditFind(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetItemCount() != 0);
}

// WM_FINDREPLACE registered message handler 
LRESULT CTagListView::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	CFindReplaceDialog *pDlg = CFindReplaceDialog::GetNotifier(lParam);
	if (pDlg == nullptr) {
		// オブジェクト取得失敗 
		return 0;
	}

	if (pDlg->IsTerminating())
	{
		// 検索ダイアログのクローズ 
		m_pFindDlg = nullptr;
		return 0;
	}

	if (!pDlg->FindNext())
	{
		// 検索以外のコマンドは無視 
		return 0;
	}

	CWaitCursor wait;

	// 大文字・小文字の区別をしないときは検索条件を小文字化 
	CString strFind = pDlg->GetFindString();
	if (!pDlg->MatchCase()) {
		strFind.MakeLower();
	}

	int focused = GetNextItem(-1, LVNI_FOCUSED);
	int raws = GetItemCount();
	for (int i = 1; i < raws; i++)
	{
		int item = 0;
		if (pDlg->SearchDown()) {
			// 下方向 
			item = (focused + i) % raws;
		}
		else {
			// 上方向 
			item = (raws + focused - i) % raws;
		}

		int cols = GetHeaderCtrl()->GetItemCount();
		for (int j = 0; j < cols; j++)
		{
			// 大文字小文字の区別をしないときは検索対象を小文字化 
			CString strItem = GetItemText(item, j);
			if (!pDlg->MatchCase()) {
				strItem.MakeLower();
			}

			bool is_find = false;
			if (pDlg->MatchWholeWord()) {
				// 完全一致 
				is_find = (strFind == strItem);
			}
			else {
				// 部分一致 
				is_find = (0 <= strItem.Find(strFind));
			}

			if (is_find)
			{
				// 現在の選択をすべて解除 
				POSITION pos = GetFirstSelectedItemPosition();
				while (pos)
				{
					int sel = GetNextSelectedItem(pos);
					if (item != sel)
					{
						SetItemState(sel, 0, LVIS_SELECTED);
					}
				}
				// 見つかった行を選択してフォーカスを移す 
				SetItemState(item, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
				EnsureVisible(item, FALSE);
				return 0;
			}
		}
	}

	// 見つからなかったらメッセージ表示 
	CString strMessage;
	strMessage.Format(IDS_CANNOT_FIND, (LPCTSTR)pDlg->GetFindString());
	::AfxMessageBox(strMessage, MB_ICONINFORMATION);

	return 0;
}
