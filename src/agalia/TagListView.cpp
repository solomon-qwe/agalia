// TagListView.cpp : implementation file
//

#include "pch.h"
#include "agalia.h"
#include "TagListView.h"

#include "TextViewer.h"
#include "CSVString.h"
#include "util.h"
#include <vector>

#include "../inc/agaliarept.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

template <class T>
void copy_to_clipboard(CListCtrl* pCtrl, T& dst)
{
	CWaitCursor wait;

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


void dump(LPARAM lParam)
{
	auto param = reinterpret_cast<agaliaItem*>(lParam);

	CFileDialog dlg(FALSE);
	if (dlg.DoModal() == IDOK)
	{
		const agaliaContainer* image = nullptr;
		auto hr = param->getAsocImage(&image);
		if (FAILED(hr)) return;
		agaliaStringPtr path;
		hr = image->getFilePath(&path);
		if (FAILED(hr)) return;

		HANDLE hFile = ::CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			::AfxGetMainWnd()->MessageBox(_T("File Open Error."), nullptr, MB_ICONWARNING);
			return;
		}

		HANDLE hMap = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		if (hMap == NULL) {
			::CloseHandle(hFile);
			::AfxGetMainWnd()->MessageBox(_T("File Mapping Error."), nullptr, MB_ICONWARNING);
			return;
		}

		LPVOID pFile = ::MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
		if (pFile == NULL) {
			::CloseHandle(hMap);
			::CloseHandle(hFile);
			::AfxGetMainWnd()->MessageBox(_T("File Mapping Error."), nullptr, MB_ICONWARNING);
			return;
		}

		uint64_t offset = 0;
		uint64_t size = 0;
		param->getValueAreaOffset(&offset);
		param->getValueAreaSize(&size);

		HANDLE h = ::CreateFile(dlg.GetOFN().lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD dwNumberOfBytesWritten = 0;
		::WriteFile(h, reinterpret_cast<BYTE*>(pFile) + offset, (DWORD)size, &dwNumberOfBytesWritten, NULL);
		::CloseHandle(h);

		::UnmapViewOfFile(pFile);
		::CloseHandle(hMap);
		::CloseHandle(hFile);
	}
}

void analyze(LPARAM lParam)
{
	auto param = reinterpret_cast<agaliaItem*>(lParam);

	const agaliaContainer* image;
	auto hr = param->getAsocImage(&image);
	if (FAILED(hr)) return;
	agaliaStringPtr path;
	image->getFilePath(&path);
	if (FAILED(hr)) return;

	std::wstring str;
	wchar_t temp[1024] = {};

	::GetModuleFileName(NULL, temp, _countof(temp));
	::PathQuoteSpaces(temp);
	str += temp;
	str += L" ";

	wcscpy_s(temp, path);
	::PathQuoteSpaces(temp);
	str += temp;
	str += L" ";

	uint64_t offset = 0;
	param->getValueAreaOffset(&offset);
	swprintf_s(temp, L"/offset:%llu ", offset);
	str += temp;

	uint64_t size = 0;
	param->getValueAreaSize(&size);
	swprintf_s(temp, L"/size:%llu ", size);
	str += temp;

	PROCESS_INFORMATION pi = {};
	STARTUPINFO si = {};
	si.cb = sizeof(si);

	wchar_t* cmdline = _wcsdup(str.c_str());
	BOOL ret = ::CreateProcess(NULL, cmdline, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if (ret)
	{
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
	}
	free(cmdline);
}


// TagListView

static UINT WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);

IMPLEMENT_DYNAMIC(TagListView, CListCtrl)

TagListView::TagListView()
{

}

TagListView::~TagListView()
{
}

// ON_UPDATE_COMMAND_UI_RANGE() はバグがあるので自前で定義 
#define ON_MY_UPDATE_COMMAND_UI_RANGE(id, idLast, memberFxn) \
	{ WM_COMMAND, CN_UPDATE_COMMAND_UI, (WORD)id, (WORD)idLast, AfxSigCmdUI_RANGE, \
		(AFX_PMSG) \
		(static_cast< void (AFX_MSG_CALL CCmdTarget::*)(CCmdUI*,UINT) > \
		(memberFxn)) },

BEGIN_MESSAGE_MAP(TagListView, CListCtrl)
	ON_WM_INITMENUPOPUP()
	ON_WM_CONTEXTMENU()
	ON_REGISTERED_MESSAGE(WM_FINDREPLACE, &TagListView::OnFindReplace)
	ON_MY_UPDATE_COMMAND_UI_RANGE(IDM_COPY_TO_CLIPBOARD_TAB, IDM_ANALYZE, &TagListView::OnUpdateCommand)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &TagListView::OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, &TagListView::OnUpdateEditFind)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, &TagListView::OnUpdateEditSelectAll)
	ON_COMMAND(IDM_COPY_TO_CLIPBOARD_TAB, &TagListView::OnCopyToClipboardTab)
	ON_COMMAND(IDM_COPY_TO_CLIPBOARD_CSV, &TagListView::OnCopyToClipboardCSV)
	ON_COMMAND(IDM_SHOW_TEXT_ASCII, &TagListView::OnShowTextAscii)
	ON_COMMAND(IDM_SHOW_TEXT_LATIN1, &TagListView::OnShowTextLatin1)
	ON_COMMAND(IDM_SHOW_TEXT_UTF16, &TagListView::OnShowTextUtf16)
	ON_COMMAND(IDM_SHOW_TEXT_UTF8, &TagListView::OnShowTextUtf8)
	ON_COMMAND(IDM_SHOW_TEXT_JIS, &TagListView::OnShowTextJis)
	ON_COMMAND(IDM_SAVE_DUMP, &TagListView::OnSaveDump)
	ON_COMMAND(IDM_ANALYZE, &TagListView::OnAnalyze)
	ON_COMMAND(ID_EDIT_COPY, &TagListView::OnEditCopy)
	ON_COMMAND(ID_EDIT_FIND, &TagListView::OnEditFind)
	ON_COMMAND(ID_EDIT_SELECT_ALL, &TagListView::OnSelectAll)
#pragma warning(suppress: 26454)	// warning C26454: Arithmetic overflow: '-' operation produces a negative unsigned result at compile time (io.5). 
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, &TagListView::OnLvnDeleteitem)
END_MESSAGE_MAP()



// TagListView message handlers




// WM_INIT_MENU_POPUP message handler 
void TagListView::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	::OnInitMenuPopup(this, pPopupMenu, nIndex, bSysMenu);
}


// WM_CONTEXT_MENU message handler 
void TagListView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
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
void TagListView::OnCopyToClipboardTab()
{
	CTSVString dst;
	copy_to_clipboard(this, dst);
}

// IDM_COPY_TO_CLIPBOARD_CSV command message handler 
void TagListView::OnCopyToClipboardCSV()
{
	CCSVString dst;
	copy_to_clipboard(this, dst);
}

// IDM_SHOW_TEXT_ASCII command message handler 
void TagListView::OnShowTextAscii()
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

	CTextViewer::CreateViewer(item.lParam, CTextViewer::ascii);
}

// IDM_SHOW_TEXT_LATIN1 command message handler 
void TagListView::OnShowTextLatin1()
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

	CTextViewer::CreateViewer(item.lParam, CTextViewer::latin1);
}

// IDM_SHOW_TEXT_UTF16 command message handler 
void TagListView::OnShowTextUtf16()
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

	CTextViewer::CreateViewer(item.lParam, CTextViewer::utf16);
}

// IDM_SHOW_TEXT_UTF8 command message handler 
void TagListView::OnShowTextUtf8()
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

	CTextViewer::CreateViewer(item.lParam, CTextViewer::utf8);
}

// IDM_SHOW_TEXT_JIS command message handler 
void TagListView::OnShowTextJis()
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

	CTextViewer::CreateViewer(item.lParam, CTextViewer::jis);
}

// IDM_SAVE_DUMP command message handler 
void TagListView::OnSaveDump()
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

	dump(item.lParam);
}

// IDM_ANALYZE command message handler 
void TagListView::OnAnalyze()
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

	analyze(item.lParam);
}

// ID_SELECT_ALL command message handler 
void TagListView::OnSelectAll()
{
	CWaitCursor wait;

	int counts = GetItemCount();
	for (int i = 0; i < counts; i++)
	{
		SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}
}

// ID_EDIT_COPY command message handler 
void TagListView::OnEditCopy()
{
	OnCopyToClipboardTab();
}

// ID_EDIT_COPY update command UI 
void TagListView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetFirstSelectedItemPosition() != NULL);
}

// ID_EDIT_SELECT_ALL update command UI 
void TagListView::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetItemCount() != 0);
}

void TagListView::OnUpdateCommand(CCmdUI* pCmdUI, UINT id)
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


void TagListView::OpenPopup(POINT& pos)
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
	menu.AppendMenu(MF_STRING, IDM_SAVE_DUMP, _T("Save Dump As..."));
	menu.AppendMenu(MF_STRING, IDM_ANALYZE, _T("Analyze"));

	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this);
}


void TagListView::OnEditFind()
{
	if (m_pFindDlg != nullptr) {
		m_pFindDlg->SetFocus();
		return;
	}

	m_pFindDlg = new CFindReplaceDialog;
	m_pFindDlg->Create(TRUE, NULL, NULL, FR_DOWN, this);

}


void TagListView::OnUpdateEditFind(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetItemCount() != 0);
}

// WM_FINDREPLACE registered message handler 
LRESULT TagListView::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	CFindReplaceDialog* pDlg = CFindReplaceDialog::GetNotifier(lParam);
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


void TagListView::OnLvnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (pNMLV->lParam)
	{
		agaliaItem* p = reinterpret_cast<agaliaItem*>(pNMLV->lParam);
		p->Release();
	}

	*pResult = 0;
}
