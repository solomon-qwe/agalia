
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "agalia.h"

#include "MainFrm.h"

#include "../inc/agaliarept.h"

#include "ChildTreeView.h"
#include "ChildListView.h"
#include "ChildTextView.h"

#include "OpenFileDialog.h"
#include "ImageDraw.h"

#include "OptionDlg.h"

#include <string>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void ShowErrMsg(DWORD dwErr)
{
	LPVOID lpMsgBuf = nullptr;
	::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<TCHAR*>(&lpMsgBuf),
		0, NULL);

	::AfxMessageBox(reinterpret_cast<TCHAR*>(lpMsgBuf), MB_ICONSTOP);

	LocalFree(lpMsgBuf);
}


// CMainFrame



void CMainFrame::DeleteAllContents(void)
{
	auto tempHandle = ctrl.terminate_thread_request;
	ctrl.terminate_thread_request = NULL;
	if (tempHandle) {
		::SetEvent(tempHandle);
	}
	if (ctrl.parse_thread) {
		auto temp = ctrl.parse_thread;
		ctrl.parse_thread = nullptr;
		while (::WaitForSingleObject(temp->m_hThread, 1) == WAIT_TIMEOUT) {
			MSG msg = {};
			if (::PeekMessage(&msg, GetSafeHwnd(), 0, 0, PM_REMOVE)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		temp->m_bAutoDelete = TRUE;
		temp->Delete();
	}
	if (tempHandle) {
		::CloseHandle(tempHandle);
	}

	// Flat View 
	m_wndTagListView.DeleteAllItems();
	int count = m_wndTagListView.GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < count; i++)
		m_wndTagListView.DeleteColumn(0);

	// Hierarchy View 
	auto treeView = dynamic_cast<ChildTreeView*>(m_wndVSplitter.GetPane(0, 0));
	auto listView = dynamic_cast<ChildListView*>(m_wndHSplitter.GetPane(0, 0));
	auto textView = dynamic_cast<ChildTextView*>(m_wndHSplitter.GetPane(1, 0));

	if (listView) {
		listView->GetListCtrl().DeleteAllItems();
	}
	if (textView) {
		textView->GetEditCtrl().SetWindowTextW(L"");
	}
	if (treeView) {
		treeView->GetTreeCtrl().DeleteAllItems();
	}

	// Graphic View 
	m_wndGraphicView.reset_content(nullptr);

	// Property View 
	m_wndPropView.m_listCtrl.DeleteAllItems();
	if (m_wndPropView.m_hBitmap)
	{
		auto temp = m_wndPropView.m_hBitmap;
		m_wndPropView.m_hBitmap = NULL;
		::DeleteObject(temp);
	}

	// clear cache data 
	if (ctrl.image)
	{
		auto temp = ctrl.image;
		ctrl.image = nullptr;
		temp->Release();
	}

	Invalidate();
}


void CMainFrame::ResetContents(const wchar_t* path, uint64_t offset, uint64_t size, int format)
{
	CMenu* menu = GetMenu();
	if (!menu) return;

	// ビュー設定を取得 
	UINT cur = 0;
	if (menu->GetMenuState(ID_VIEW_FLAT, MF_BYCOMMAND) & MF_CHECKED)
		cur = ID_VIEW_FLAT;
	else if (menu->GetMenuState(ID_VIEW_HIERARCHY, MF_BYCOMMAND) & MF_CHECKED)
		cur = ID_VIEW_HIERARCHY;
	else if (menu->GetMenuState(ID_VIEW_GRAPHIC, MF_BYCOMMAND) & MF_CHECKED)
		cur = ID_VIEW_GRAPHIC;

	// キャッシュを削除 
	DeleteAllContents();

	if (path[0] == L'\0') return;
	// ウィンドウタイトル更新 
	UpdateFrameTitleForDocument(path);

	// ファイルを読み込む 
	agaliaPtr<agaliaContainer> sp;
	auto hr = getAgaliaImage(&sp, path, offset, size, format);
	if (FAILED(hr))
	{
		ShowErrMsg(hr);
		return;
	}
	ctrl.image = sp.detach();

	ctrl.data_path = path;
	ctrl.data_offset = offset;
	ctrl.data_size = size;
	ctrl.data_format = format;

	// ビューをリセット 
	switch (cur)
	{
	case ID_VIEW_FLAT:
		ResetFlatView();
		break;
	case ID_VIEW_HIERARCHY:
		ResetHierarchyView();
		break;
	case ID_VIEW_GRAPHIC:
		ResetGraphicView();
		break;
	}

	// プロパティの情報 
	if (ctrl.image)
	{
		int nItem = 0;
		agaliaStringPtr str;

		if (SUCCEEDED(ctrl.image->getPropertyValue(agaliaContainer::ContainerType, &str)))
		{
			m_wndPropView.m_listCtrl.InsertItem(nItem, L"Container");
			m_wndPropView.m_listCtrl.SetItemText(nItem, 1, str);
			str.detach()->Release();
			nItem++;
		}

		if (SUCCEEDED(ctrl.image->getPropertyValue(agaliaContainer::FormatType, &str)))
		{
			m_wndPropView.m_listCtrl.InsertItem(nItem, L"Format");
			m_wndPropView.m_listCtrl.SetItemText(nItem, 1, str);
			str.detach()->Release();
			nItem++;
		}

		if (SUCCEEDED(ctrl.image->getPropertyValue(agaliaContainer::ImageWidth, &str)))
		{
			m_wndPropView.m_listCtrl.InsertItem(nItem, L"Width");
			m_wndPropView.m_listCtrl.SetItemText(nItem, 1, str);
			str.detach()->Release();
			nItem++;
		}

		if (SUCCEEDED(ctrl.image->getPropertyValue(agaliaContainer::ImageHeight, &str)))
		{
			m_wndPropView.m_listCtrl.InsertItem(nItem, L"Height");
			m_wndPropView.m_listCtrl.SetItemText(nItem, 1, str);
			str.detach()->Release();
			nItem++;
		}

		if (SUCCEEDED(ctrl.image->getPropertyValue(agaliaContainer::ICCProfile, &str)))
		{
			m_wndPropView.m_listCtrl.InsertItem(nItem, L"ICC Profile");
			m_wndPropView.m_listCtrl.SetItemText(nItem, 1, str);
			str.detach()->Release();
			nItem++;
		}

		if (nItem != 0)
		{
			m_wndPropView.m_listCtrl.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
			m_wndPropView.m_listCtrl.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
		}

		HBITMAP hBitmap = NULL;
		if (SUCCEEDED(ctrl.image->getThumbnailImage(&hBitmap, thumbnail_area_size, thumbnail_area_size)))
		{
			m_wndPropView.m_hBitmap = hBitmap;
			m_wndPropView.Invalidate();
		}
	}
}


int CMainFrame::ResetViewLayout()
{
	CMenu* menu = GetMenu();
	if (!menu) return -1;

	// ビュー設定を取得 
	UINT cur = 0;
	if (menu->GetMenuState(ID_VIEW_FLAT, MF_BYCOMMAND) & MF_CHECKED)
		cur = ID_VIEW_FLAT;
	else if (menu->GetMenuState(ID_VIEW_HIERARCHY, MF_BYCOMMAND) & MF_CHECKED)
		cur = ID_VIEW_HIERARCHY;
	else if (menu->GetMenuState(ID_VIEW_GRAPHIC, MF_BYCOMMAND) & MF_CHECKED)
		cur = ID_VIEW_GRAPHIC;

	// プロパティ表示設定を取得 
	bool prop = (menu->GetMenuState(ID_VIEW_PROPERTY, MF_BYCOMMAND) & MF_CHECKED);

	// 現在のビューのポインタを設定 
	m_pCurWnd = nullptr;
	if (cur == ID_VIEW_FLAT)
		m_pCurWnd = &m_wndTagListView;
	else if (cur == ID_VIEW_HIERARCHY)
		m_pCurWnd = &m_wndVSplitter;
	else if (cur == ID_VIEW_GRAPHIC)
		m_pCurWnd = &m_wndGraphicView;

	// レイアウト変更 
	RecalcLayout();

	// ビューを表示 
	if (prop)
	{
		m_wndPropView.EnableWindow();
		m_wndPropView.ShowWindow(SW_SHOW);
	}
	if (m_pCurWnd)
	{
		m_pCurWnd->EnableWindow();
		m_pCurWnd->SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		m_pCurWnd->Invalidate();
		m_pCurWnd->UpdateWindow();
	}

	// 不要なビューを非表示にする 
	if (cur != ID_VIEW_FLAT)
	{
		m_wndTagListView.EnableWindow(FALSE);
		m_wndTagListView.ShowWindow(SW_HIDE);
	}
	if (cur != ID_VIEW_HIERARCHY)
	{
		m_wndVSplitter.EnableWindow(FALSE);
		m_wndVSplitter.ShowWindow(SW_HIDE);
	}
	if (cur != ID_VIEW_GRAPHIC)
	{
		m_wndGraphicView.EnableWindow(FALSE);
		m_wndGraphicView.ShowWindow(SW_HIDE);
	}
	if (!prop)
	{
		m_wndPropView.EnableWindow(FALSE);
		m_wndPropView.ShowWindow(SW_HIDE);
	}

	return 0;
}




parser_thread_param* prepare_parser(watch_progress_param* wa, controller* ctrl, HWND hwndTarget)
{
	auto param2 = new parser_thread_param;
	param2->hwndTarget = hwndTarget;
	param2->ctrl = ctrl;
	param2->abort_request_event = wa->abort_request_event;
	param2->parse_finished_event = wa->parse_finished_event;
	param2->terminate_thread_request = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	ctrl->terminate_thread_request = param2->terminate_thread_request;

	return param2;
}





// Flat View 
void CMainFrame::ResetFlatView(void)
{
	// リストビューのヘッダカラムを初期化 
	int col = 0;
	uint32_t count = 0;
	auto hr = ctrl.image->getColumnCount(&count);
	if (FAILED(hr)) return;
	for (uint32_t i = 0; i < count; i++)
	{
		// カラム名 
		agaliaStringPtr str;
		if (FAILED(ctrl.image->getColumnName(i, &str)))
			continue;

		// カラムの幅 
		int column_width = 0;
		bool align_right = false;
		int32_t length = 0;
		if (SUCCEEDED(ctrl.image->getColumnWidth(i, &length)))
		{
			align_right = (length < 0);

			if (length == 0)
			{
				column_width = LVSCW_AUTOSIZE;
			}
			else
			{
				std::wstring u;
				u.assign(abs(length), L'W');
				column_width = m_wndTagListView.GetStringWidth(u.c_str());
			}
		}

		if (0 <= m_wndTagListView.InsertColumn(col, str, align_right ? LVCFMT_RIGHT : LVCFMT_LEFT, column_width))
		{
			col++;
		}
	}
	if (col == 0) return;

	// 末尾のヘッダーの幅を自動調整 
	m_wndTagListView.SetColumnWidth(col - 1, LVSCW_AUTOSIZE_USEHEADER);
	// 自動調整後の幅からスクロールバーの幅を引く 
	int cx = m_wndTagListView.GetColumnWidth(col - 1) - ::GetSystemMetrics(SM_CXVSCROLL);
	m_wndTagListView.SetColumnWidth(col - 1, cx);

	// プログレスダイアログを準備 
	auto param1 = prepare_progress(GetSafeHwnd(), ctrl.data_path);
	auto param2 = prepare_parser(param1, &ctrl, m_wndTagListView.GetSafeHwnd());
	// 解析スレッドを起動 
	launch_parser(&ctrl, ResetFlatViewThreadProc, param1, param2);
}

// Hierarchy View 
void CMainFrame::ResetHierarchyView(void)
{
	// プログレスダイアログを準備 
	auto param1 = prepare_progress(GetSafeHwnd(), ctrl.data_path);
	auto param2 = prepare_parser(param1, &ctrl, static_cast<ChildTreeView*>(m_wndVSplitter.GetPane(0, 0))->GetTreeCtrl().GetSafeHwnd());
	// 解析スレッドを起動 
	launch_parser(&ctrl, ResetHierarchyViewThreadProc, param1, param2);
}

// Graphic View 
void CMainFrame::ResetGraphicView(void)
{
	Invalidate();
	UpdateWindow();

	m_wndGraphicView.reset_content(ctrl.image);
}



// Hierarchy View / List View Pane 
void CMainFrame::UpdateListViewPane(const agaliaItem* item)
{
	if (ctrl.image == nullptr) return;

	ChildListView* pListView = dynamic_cast<ChildListView*>(m_wndHSplitter.GetPane(0, 0));
	if (!pListView) return;

	uint32_t prop_count = 0;
	if (FAILED(item->getItemPropCount(&prop_count)))
		return;

	int list_index = 0;
	for (uint32_t i = 0; i < prop_count; i++)
	{
		// １列目 
		agaliaStringPtr str;
		if (FAILED(item->getItemPropName(i, &str)))
			continue;
		if (pListView->GetListCtrl().InsertItem(list_index, str) < 0)
			continue;
		str.detach()->Release();

		// ２列目 
		if (SUCCEEDED(item->getItemPropValue(i, &str))) {
			pListView->GetListCtrl().SetItemText(list_index, 1, str);
		}
		list_index++;
	}
}

// Hierarchy View / Text View Pane 
void CMainFrame::UpdateTextViewPane(const agaliaItem* item)
{
	ChildTextView* pTextView = dynamic_cast<ChildTextView*>(m_wndHSplitter.GetPane(1, 0));
	if (!pTextView) return;

	uint64_t datasize = 0;
	uint64_t dataoffset = 0;
	item->getValueAreaSize(&datasize);
	item->getValueAreaOffset(&dataoffset);
	size_t bufsize = static_cast<size_t>(min(datasize, 1024 * 4));	// 全部読込むと表示に時間がかかるので制限をかける 

	// データを読み込む 
	CHeapPtr<uint8_t> buf;
	if (!buf.AllocateBytes(bufsize))
		return;
	const agaliaContainer* img = nullptr;
	auto hr = item->getAsocImage(&img);
	if (FAILED(hr)) return;
	hr = img->ReadData(buf, dataoffset, bufsize);
	if (FAILED(hr)) return;


	// 更新中の画面更新抑制のためロック 
	pTextView->GetEditCtrl().LockWindowUpdate();

	// ヘッダ出力  
	pTextView->GetEditCtrl().ReplaceSel(L" offset  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  ----------------\r\n");

	size_t pos = 0;
	while (pos < bufsize)
	{
		const size_t line_count = 16;

		const size_t tempbuf_size = 256;
		wchar_t tempbuf[tempbuf_size];
		size_t written = 0;

		// オフセット位置を出力 
		int ret = swprintf_s(tempbuf + written, tempbuf_size - written, L"%.8zX ", pos);
		if (ret < 0) break;
		written += ret;

		uint8_t* q = buf + pos;

		size_t c = min(line_count, bufsize - pos);
		// バイナリを16進出力 
		for (size_t i = 0; i < c; i++)
		{
			ret = swprintf_s(tempbuf + written, tempbuf_size - written, L"%.2X ", q[i]);
			if (ret < 0) break;
			written += ret;
		}
		if (c < line_count)
		{
			// 桁合わせのため空白を出力 
			size_t pad = 3 * (line_count - c);
			wmemset(tempbuf + written, L' ', pad);
			written += pad;
		}

		// バイナリをASCII出力 
		for (size_t i = 0; i < c; i++)
		{
			ret = swprintf_s(tempbuf + written, tempbuf_size - written, L"%C", isprint(q[i]) ? q[i] : '.');
			if (ret < 0) break;
			written += ret;
		}

		// 行末で改行 
		errno_t err = wcscpy_s(tempbuf + written, tempbuf_size - written, L"\r\n");
		if (err) break;

		// エディットボックスに追加出力 
		pTextView->GetEditCtrl().ReplaceSel(tempbuf);

		// 次の行へ 
		pos += line_count;
	}

	// 元データの一部しか読んでいなければ、最後に省略記号を出力 
	if (datasize != bufsize)
		pTextView->GetEditCtrl().ReplaceSel(_T("..."));

	// カーソルを先頭に戻して描画ロック解除 
	pTextView->GetEditCtrl().SetSel(0);
	pTextView->GetEditCtrl().UnlockWindowUpdate();
}







IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_DROPFILES()
	ON_WM_EXITSIZEMOVE()
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
	ON_COMMAND(ID_VIEW_PROPERTY, &CMainFrame::OnViewProperty)
	ON_COMMAND_RANGE(ID_VIEW_FLAT, ID_VIEW_GRAPHIC, &CMainFrame::OnViewRange)
	ON_COMMAND(ID_EDIT_OPTIONS, &CMainFrame::OnEditOptions)
	ON_MESSAGE(WM_APP, &CMainFrame::OnApp)
	ON_WM_NCCREATE()
END_MESSAGE_MAP()



// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	HMODULE hModUser32 = LoadLibrary(L"User32.dll");
	typedef DPI_AWARENESS_CONTEXT(_stdcall* SETTHREADDPIAWARENESSCONTEXTPROC)(DPI_AWARENESS_CONTEXT);
	SETTHREADDPIAWARENESSCONTEXTPROC SetThreadDpiAwarenessContextProc;
	SetThreadDpiAwarenessContextProc = (SETTHREADDPIAWARENESSCONTEXTPROC)GetProcAddress(hModUser32, "SetThreadDpiAwarenessContext");
	if (SetThreadDpiAwarenessContextProc) {
		SetThreadDpiAwarenessContextProc(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	}
}

CMainFrame::~CMainFrame()
{
	if (ctrl.image)
	{
		auto temp = ctrl.image;
		ctrl.image = nullptr;
		temp->Release();
	}
}



// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG



// CMainFrame message handlers

UINT uDPI = 0;
// WM_CREATE message handler 
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	HMODULE hModUser32 = ::LoadLibrary(L"User32.dll");
	typedef UINT (WINAPI* GETDPIFORWINDOWPROC)(HWND);
	GETDPIFORWINDOWPROC GetDpiForWindowProc;
	GetDpiForWindowProc = (GETDPIFORWINDOWPROC)::GetProcAddress(hModUser32, "GetDpiForWindow");
	if (GetDpiForWindowProc) {
		uDPI = GetDpiForWindowProc(GetSafeHwnd());
	}

	ctrl.load_registry();

	// set default view sytle 
	CMenu* menu = GetMenu();
	if (menu)
	{
		int nDefaultItem = ID_VIEW_FLAT + ctrl.startup_view;
		menu->CheckMenuRadioItem(ID_VIEW_FLAT, ID_VIEW_GRAPHIC, nDefaultItem, MF_BYCOMMAND);

		int nDefaultCheck = ctrl.startup_properties_show ? MF_CHECKED : MF_UNCHECKED;
		menu->CheckMenuItem(ID_VIEW_PROPERTY, nDefaultCheck);
	}
	m_wndGraphicView.reset_color_profile(ctrl.monitor_color_profile);
	m_wndGraphicView.reset_renderer(ctrl.rendering_engine);

	// initialize flat sytle 
	if (!m_wndTagListView.Create(
		AFX_WS_DEFAULT_VIEW | LVS_REPORT | LVS_SHOWSELALWAYS,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST))
	{
		return -1;
	}
	DWORD dwStyle = m_wndTagListView.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_HEADERDRAGDROP;
	m_wndTagListView.SetExtendedStyle(dwStyle);

	// initialize graphic sytle 
	m_wndGraphicView.init_window();

	// initialize hierarchy style 
	CListView* pListView = dynamic_cast<CListView*>(m_wndHSplitter.GetPane(0, 0));
	CListCtrl* pList = &pListView->GetListCtrl();
	dwStyle = pList->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_HEADERDRAGDROP;
	pList->SetExtendedStyle(dwStyle);
	pList->InsertColumn(0, _T("item"), 0, 400);
	pList->InsertColumn(1, _T("value"), 0, 800);

	// initialize property view 
	m_wndPropView.Create(
		::AfxRegisterWndClass(0, ::LoadCursor(NULL, IDC_ARROW)),
		_T(""),
		WS_CHILD | WS_BORDER,
		CRect(0, 0, 0, 0), this, (UINT)-1);

	return ResetViewLayout();
}



BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_ACCEPTFILES;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	return TRUE;
}



BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	m_wndVSplitter.CreateStatic(this, 1, 2);
	m_wndHSplitter.CreateStatic(&m_wndVSplitter, 2, 1, WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndVSplitter.IdFromRowCol(0, 1));

	m_wndVSplitter.CreateView(0, 0, RUNTIME_CLASS(ChildTreeView), CSize(300, 0), pContext);
	m_wndHSplitter.CreateView(0, 0, RUNTIME_CLASS(ChildListView), CSize(0, 800), pContext);
	m_wndHSplitter.CreateView(1, 0, RUNTIME_CLASS(ChildTextView), CSize(0, 0), pContext);

	return CFrameWnd::OnCreateClient(lpcs, pContext);
}



void CMainFrame::RecalcLayout(BOOL bNotify)
{
	//CFrameWnd::RecalcLayout(bNotify);
	UNREFERENCED_PARAMETER(bNotify);

	if (!m_pCurWnd) return;

	CMenu* menu = GetMenu();
	if (!menu) return;

	CRect rect = {};
	GetClientRect(&rect);
	int cx = rect.Width();

	UINT dpi = 0;

	HMODULE user32dll = GetModuleHandle(TEXT("user32.dll"));
	if (user32dll)
	{
		typedef UINT(WINAPI* GETDPIFORWINDOW_FUNC)(HWND);
		GETDPIFORWINDOW_FUNC getDpiForWindow = reinterpret_cast<GETDPIFORWINDOW_FUNC>(GetProcAddress(user32dll, "GetDpiForWindow"));
		if (getDpiForWindow)
		{
			dpi = getDpiForWindow(m_wndPropView.GetSafeHwnd());
		}
	}

	if (dpi == 0)
	{
		CWindowDC dc(&m_wndPropView);
		dpi = dc.GetDeviceCaps(LOGPIXELSX);
	}

	const int defaultWidth = 200;
	int propW = defaultWidth * dpi / USER_DEFAULT_SCREEN_DPI;
	thumbnail_area_size = propW;

	// プロパティ ビュー の幅を調整 
	bool prop = (menu->GetMenuState(ID_VIEW_PROPERTY, MF_BYCOMMAND) & MF_CHECKED);
	if (prop)
	{
		cx = max(0, cx - propW);
		m_wndPropView.SetWindowPos(NULL, rect.left + cx, rect.top, propW, rect.Height(), SWP_NOZORDER);
	}

	// プロパティ ビュー を除いた領域に移動 
	m_pCurWnd->SetWindowPos(NULL, rect.left, rect.top, cx, rect.Height(), SWP_NOZORDER);
}



BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	CWnd* pWndFocus = GetFocus();
	if (pWndFocus)
	{
		CWnd* pWndRoot = pWndFocus->GetAncestor(GA_ROOT);
		if (pWndRoot)
		{
			if (pWndRoot->GetSafeHwnd() != GetSafeHwnd())
			{
				// pWndFocus が CEditCtrl、pWndRoot が CTextViewer のときにここに来る 
				if (pWndRoot->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) {
					return TRUE;
				}
				else {
					return FALSE;
				}
			}
		}
	}

	// let the view have first crack at the command
	if (m_pCurWnd) {
		if (m_pCurWnd->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;
	}

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



// WM_DESTROY message handler 
void CMainFrame::OnDestroy()
{
	DeleteAllContents();

	CFrameWnd::OnDestroy();
}



// WM_SETFOCUS message handler 
void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	if (m_pCurWnd)
		m_pCurWnd->SetFocus();
}



// WM_DROPFILES message handler 
void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	CWaitCursor wait;

	// ドロップされたファイルのパスを取得 
	CString path;
	UINT count = ::DragQueryFile(hDropInfo, 0, NULL, 0);
	count += 1;
	if (!::DragQueryFile(hDropInfo, 0, path.GetBuffer(count), count))
		return;
	path.ReleaseBuffer();

	// ファイルを開く 
	ctrl.data_path = path;
	ctrl.data_offset = 0;
	ctrl.data_size = 0;
	ctrl.data_format = agalia_format_auto;
	ResetContents(ctrl.data_path, ctrl.data_offset, ctrl.data_size, ctrl.data_format);
}



// WM_EXITSIZEMOVE message handler 
void CMainFrame::OnExitSizeMove()
{
	// Graphic View のカラープロファイルを移動先のモニタに合わせる 
	m_wndGraphicView.reset_color_profile(-1);

	CFrameWnd::OnExitSizeMove();
}



afx_msg LRESULT CMainFrame::OnApp(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	CWaitCursor wait;

	auto pListView = dynamic_cast<ChildListView*>(m_wndHSplitter.GetPane(0, 0));
	auto pTextView = dynamic_cast<ChildTextView*>(m_wndHSplitter.GetPane(1, 0));
	auto pTreeView = dynamic_cast<ChildTreeView*>(m_wndVSplitter.GetPane(0, 0));

	if (pListView == nullptr) return 0;
	if (pTextView == nullptr) return 0;
	if (pTreeView == nullptr) return 0;

	// 右上のリストをクリア 
	if (pListView) pListView->GetListCtrl().DeleteAllItems();

	// 右下のテキストをクリア 
	if (pTextView) pTextView->GetEditCtrl().SetWindowTextW(L"");

	// 左のツリーで選択されているアイテムを取得 
	HTREEITEM hTI = pTreeView->GetTreeCtrl().GetSelectedItem();
	if (hTI == NULL) return 0;
	DWORD_PTR data = pTreeView->GetTreeCtrl().GetItemData(hTI);
	if (data == NULL) return 0;
	agaliaItem* item = reinterpret_cast<agaliaItem*>(data);

	// 右側のビューを更新 
	if (pListView->IsWindowVisible())
		UpdateListViewPane(item);
	if (pTextView->IsWindowVisible())
		UpdateTextViewPane(item);

	return 0;
}


// [File]-[Open] 
void CMainFrame::OnFileOpen()
{
	OpenFileDialog dlg(TRUE);

	// [開く]ダイアログ 
	dlg.m_ofn.nMaxFile = 32767 + 1;
	dlg.m_ofn.lpstrFile = new TCHAR[dlg.m_ofn.nMaxFile];
	if (dlg.m_ofn.lpstrFile)
	{
		dlg.m_ofn.lpstrFile[0] = _T('\0');
		if (dlg.DoModal() == IDOK)
		{
			CWaitCursor wait;

			// ファイルを開く 
			ResetContents(
				dlg.GetOFN().lpstrFile,
				_wtoi64(dlg.strOffset),
				_wtoi64(dlg.strSize),
				dlg.nFormat);
		}
		delete[]dlg.m_ofn.lpstrFile;
	}
}



// [Edit]-[Options] 
void CMainFrame::OnEditOptions()
{
	OptionDlg dlg(this);

	// 初期値を設定 
	getAgaliaPreference(dictionary_lang, &dlg.dictionary_lang);
	getAgaliaPreference(dicom_force_dictionary_vr, &dlg.dicom_force_dictionary_vr);
	dlg.startup_view = ctrl.startup_view;
	dlg.startup_properties_show = ctrl.startup_properties_show;
	dlg.rendering_engine = ctrl.rendering_engine;
	dlg.monitor_color_profile = ctrl.monitor_color_profile;

	if (dlg.DoModal() == IDOK)
	{
		// 設定値を反映させる 
		setAgaliaPreference(dictionary_lang, dlg.dictionary_lang);
		setAgaliaPreference(dicom_force_dictionary_vr, dlg.dicom_force_dictionary_vr);
		ctrl.startup_view = dlg.startup_view;
		ctrl.startup_properties_show = dlg.startup_properties_show;
		ctrl.rendering_engine = dlg.rendering_engine;
		ctrl.monitor_color_profile = dlg.monitor_color_profile;

		ctrl.save_registry();

		// 画面更新 
		CWaitCursor wait;
		m_wndGraphicView.reset_renderer(ctrl.rendering_engine);
		m_wndGraphicView.reset_color_profile(ctrl.monitor_color_profile);
		ResetContents(ctrl.data_path, ctrl.data_offset, ctrl.data_size, ctrl.data_format);
		RecalcLayout();
	}
}



// [View]-[Flat]~[Graphic]
void CMainFrame::OnViewRange(UINT id)
{
	CMenu* menu = GetMenu();
	if (!menu) return;

	menu->CheckMenuRadioItem(ID_VIEW_FLAT, ID_VIEW_GRAPHIC, id, MF_BYCOMMAND);

	CWaitCursor wait;
	DeleteAllContents();
	ResetViewLayout();
	ResetContents(ctrl.data_path, ctrl.data_offset, ctrl.data_size, ctrl.data_format);
}



// [View]-[Property] 
void CMainFrame::OnViewProperty()
{
	CMenu* menu = GetMenu();
	if (!menu) return;

	UINT stat = menu->GetMenuState(ID_VIEW_PROPERTY, MF_BYCOMMAND);
	menu->CheckMenuItem(ID_VIEW_PROPERTY, (stat & MF_CHECKED) ? MF_UNCHECKED : MF_CHECKED);

	ResetViewLayout();
}


BOOL CMainFrame::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
	HMODULE hModUser32 = LoadLibrary(L"User32.dll");
	typedef BOOL(WINAPI* ENABLENONCLIENTDPISCALINGPROC)(HWND);
	ENABLENONCLIENTDPISCALINGPROC EnableNonClientDpiScalingProc;
	EnableNonClientDpiScalingProc = (ENABLENONCLIENTDPISCALINGPROC)GetProcAddress(hModUser32, "EnableNonClientDpiScaling");
	if (EnableNonClientDpiScalingProc) {
		EnableNonClientDpiScalingProc(GetSafeHwnd());
	}

	if (!CFrameWnd::OnNcCreate(lpCreateStruct))
		return FALSE;
	return TRUE;
}


void UpdateFont(HWND hwnd, int dpi)
{
	HFONT hCurFont = (HFONT)::SendMessage(hwnd, WM_GETFONT, 0, 0);
	if (hCurFont == NULL)
		return;

	LOGFONT lf = {};
	if (!::GetObject(hCurFont, sizeof(lf), &lf))
		return;

	lf.lfHeight = lf.lfHeight * dpi / (int)uDPI;
	HFONT hNewFont = ::CreateFontIndirect(&lf);
	if (hNewFont == NULL)
		return;

	::SendMessage(hwnd, WM_SETFONT, (WPARAM)hNewFont, TRUE);
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_DPICHANGED)
	{
		WORD dpi = HIWORD(wParam);
		RECT* rcNew = (RECT*)lParam;

		auto textView = dynamic_cast<ChildTextView*>(m_wndHSplitter.GetPane(1, 0));
		UpdateFont(textView->GetEditCtrl(), dpi);
		uDPI = dpi;

		SetWindowPos(
			nullptr,
			rcNew->left, rcNew->top,
			rcNew->right - rcNew->left, rcNew->bottom - rcNew->top,
			SWP_NOZORDER | SWP_NOACTIVATE);
	}

	return CFrameWnd::WindowProc(message, wParam, lParam);
}
