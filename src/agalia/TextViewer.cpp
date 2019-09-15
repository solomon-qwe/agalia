// TextViewer.cpp : implementation file
//

#include "pch.h"
#include "agalia.h"
#include "TextViewer.h"

#include "util.h"

#include "../inc/agaliarept.h"
#include "handle_wrapper.h"



wchar_t* get_string(const wchar_t* path, uint64_t offset, uint32_t size, CTextViewer::type_t type, bool replace_terminator)
{
	handle_wrapper hFile(INVALID_HANDLE_VALUE);
	hFile = ::CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile.isErr()) {
		::AfxGetMainWnd()->MessageBox(_T("File Open Error."), nullptr, MB_ICONWARNING);
		return nullptr;
	}

	handle_wrapper hMap(NULL);
	hMap = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hMap.isErr()) {
		::AfxGetMainWnd()->MessageBox(_T("File Mapping Error."), nullptr, MB_ICONWARNING);
		return nullptr;
	}

	LPVOID pFile = ::MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	if (pFile == NULL) {
		::AfxGetMainWnd()->MessageBox(_T("File Mapping Error."), nullptr, MB_ICONWARNING);
		return nullptr;
	}
	char* src = reinterpret_cast<char*>(pFile) + offset;

	wchar_t* buf = nullptr;
	rsize_t leng = 0;
	switch (type)
	{
	case CTextViewer::utf16:
		leng = size;
		buf = (wchar_t*)calloc(leng + 1, sizeof(wchar_t));
		if (buf)
		{
			memcpy(buf, src, leng);
		}
		break;

	default:
		UINT codepage = CP_ACP;
		switch (type)
		{
		case CTextViewer::ascii:
			codepage = CP_ACP;
			break;
		case CTextViewer::latin1:
			codepage = agalia::CP_LATIN1;
			break;
		case CTextViewer::utf8:
			codepage = CP_UTF8;
			break;
		case CTextViewer::jis:
			codepage = agalia::CP_JIS;
			break;
		}
		leng = ::MultiByteToWideChar(codepage, 0, (char*)src, size, nullptr, 0);
		buf = (wchar_t*)calloc(leng + 1, sizeof(wchar_t));
		if (buf)
		{
			::MultiByteToWideChar(codepage, 0, (char*)src, size, buf, (int)leng);
		}
	}

	if (buf)
	{
		if (replace_terminator) {
			for (rsize_t i = 0; i < (leng + 1) - 1; i++) {
				if (buf[i] == L'\0') {
					buf[i] = L' ';
				}
			}
		}
	}

	::UnmapViewOfFile(pFile);

	return buf;
}



void create_edit(CEdit& edit, CWnd* parent, bool wrap)
{
	// 読み込み済みであれば一度破棄する 
	if (::IsWindow(edit.GetSafeHwnd())) {
		edit.DestroyWindow();
	}

	// 親ウィンドウ（モードレスダイアログ）のクライアント領域のサイズを取得 
	CRect rc;
	parent->GetClientRect(&rc);

	// エディットボックスを構築 
	edit.Create(
		WS_CHILD | WS_VISIBLE |
		WS_BORDER |
		ES_MULTILINE |
		ES_READONLY |
		WS_VSCROLL | ES_AUTOVSCROLL |
		(wrap ? 0 : (WS_HSCROLL | ES_AUTOHSCROLL)),
		rc, parent, 0);

	// OSからフォント情報を取得 
	NONCLIENTMETRICS ncm = {};
	ncm.cbSize = sizeof(ncm);
	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);

	// フォントを作成してエディットボックスに適用 
	CFont font;
	font.CreateFontIndirect(&ncm.lfMessageFont);
	edit.SetFont(&font);
	font.Detach();

	edit.SetFocus();
}


// CTextViewer

IMPLEMENT_DYNAMIC(CTextViewer, CWnd)

CTextViewer::CTextViewer()
{
	offset = 0;
	size = 0;
	type = ascii;
	replace_terminator = false;
}

CTextViewer::~CTextViewer()
{
}


BEGIN_MESSAGE_MAP(CTextViewer, CWnd)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(IDM_WRAP, &CTextViewer::OnWrap)
	ON_COMMAND(ID_FILE_CLOSE, &CTextViewer::OnFileClose)
	ON_COMMAND(IDM_REPLACE_TERMINATOR, &CTextViewer::OnReplaceTerminator)
	ON_COMMAND(ID_EDIT_COPY, &CTextViewer::OnEditCopy)
	ON_COMMAND(ID_EDIT_SELECT_ALL, &CTextViewer::OnSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CTextViewer::OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(IDM_REPLACE_TERMINATOR, &CTextViewer::OnUpdateReplaceTerminator)
	ON_UPDATE_COMMAND_UI(IDM_WRAP, &CTextViewer::OnUpdateWrap)
END_MESSAGE_MAP()



// CTextViewer message handlers

void CTextViewer::CreateViewer(LPARAM lParam, type_t type)
{
	auto param = reinterpret_cast<agaliaItem*>(lParam);

	uint64_t offset = 0;
	param->getValueAreaOffset(&offset);

	uint64_t raw_size = 0;
	param->getValueAreaSize(&raw_size);
	uint32_t size = 0;
	auto hr = UInt64ToUInt32(raw_size, &size);
	if (FAILED(hr)) return;

	const agaliaContainer* image = nullptr;
	hr = param->getAsocImage(&image);
	if (FAILED(hr)) return;
	agaliaStringPtr path;
	hr = image->getFilePath(&path);
	if (FAILED(hr)) return;

	TCHAR sz[1024] = {};
	_stprintf_s(sz, _T("%s %I64u-%u"), ::PathFindFileName(path), offset, size);

	wchar_t* buf = get_string(path, offset, size, type, false);
	if (buf == nullptr)
	{
		return;
	}

	CRect rcParent;
	::AfxGetMainWnd()->GetWindowRect(&rcParent);

	CRect rcWindow(0, 0, 640, 480);
	rcWindow.OffsetRect(
		rcParent.left + (rcParent.Width() - rcWindow.Width()) / 2,
		rcParent.top + (rcParent.Height() - rcWindow.Height()) / 2);

	CTextViewer* pViewer = new CTextViewer;
	pViewer->path = path;
	pViewer->offset = offset;
	pViewer->size = size;
	pViewer->type = type;
	pViewer->CreateEx(
		0,
		::AfxRegisterWndClass(
			CS_VREDRAW | CS_HREDRAW,
			::LoadCursor(NULL, IDC_ARROW),
			0,
			::LoadIcon(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINFRAME))),
		sz,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		rcWindow,
		::AfxGetMainWnd(),
		0);
	pViewer->m_child.SetWindowText(buf);

	free(buf);
}

void CTextViewer::PostNcDestroy()
{
	CWnd::PostNcDestroy();
	delete this;
}

// WM_CREATE message handler 
int CTextViewer::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CMenu popupEdit;
	popupEdit.CreateMenu();
	popupEdit.AppendMenu(MF_STRING, ID_EDIT_SELECT_ALL, L"Select &All	Ctrl+A");
	popupEdit.AppendMenu(MF_STRING, ID_EDIT_COPY, L"&Copy	Ctrl+C");

	CMenu popupFormat;
	popupFormat.CreateMenu();
	popupFormat.AppendMenu(MF_STRING, IDM_WRAP, L"&Wrap");
	popupFormat.AppendMenu(MF_STRING, IDM_REPLACE_TERMINATOR, L"&Replace Terminator");

	CMenu menu;
	menu.CreateMenu();
	menu.AppendMenu(MF_STRING, ID_FILE_CLOSE, L"E&xit");
	menu.AppendMenu(MF_POPUP, (UINT_PTR)popupEdit.GetSafeHmenu(), L"&Edit");
	menu.AppendMenu(MF_POPUP, (UINT_PTR)popupFormat.GetSafeHmenu(), L"F&ormat");
	SetMenu(&menu);

	menu.Detach();
	popupFormat.Detach();
	popupEdit.Detach();

	create_edit(m_child, this, false);

	return 0;
}

// WM_SIZE message handler 
void CTextViewer::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if (m_child.GetSafeHwnd())
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		m_child.MoveWindow(rcClient);
	}
}

// WM_INITMENUPOPUP message handler 
void CTextViewer::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	::OnInitMenuPopup(this, pPopupMenu, nIndex, bSysMenu);
}


void CTextViewer::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	int nStartChar = 0;
	int nEndChar = 0;
	m_child.GetSel(nStartChar, nEndChar);

	pCmdUI->Enable(nStartChar != nEndChar);
}

void CTextViewer::OnUpdateWrap(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(!(m_child.GetStyle() & ES_AUTOHSCROLL));
}

void CTextViewer::OnUpdateReplaceTerminator(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(replace_terminator);
}

void CTextViewer::OnFileClose()
{
	DestroyWindow();
}

void CTextViewer::OnEditCopy()
{
	m_child.Copy();
}

void CTextViewer::OnSelectAll()
{
	m_child.SetSel(0, -1);
}

// IDM_WRAP command message handler 
void CTextViewer::OnWrap()
{
	DWORD style = m_child.GetStyle();
	bool wrap = (style & ES_AUTOHSCROLL) == 0;
	create_edit(m_child, this, !wrap);

	wchar_t* buf = get_string(path.c_str(), offset, size, type, replace_terminator);
	m_child.SetWindowText(buf);
	free(buf);
}

// IDM_REPLACE_TERMINATOR command message handler 
void CTextViewer::OnReplaceTerminator()
{
	replace_terminator = !replace_terminator;

	wchar_t* buf = get_string(path.c_str(), offset, size, type, replace_terminator);
	m_child.SetWindowText(buf);
	free(buf);
}
