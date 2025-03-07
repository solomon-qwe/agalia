#include "pch.h"
#include "controller.h"

#include "../inc/agaliarept.h"
#include "ImageDraw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



void controller::load_registry(void)
{
	CString temp = ::AfxGetApp()->GetProfileStringW(L"2.0", L"StartupView", L"Flat");
	if (temp == L"Hierarchy") {
		startup_view = 1;
	}
	else if (temp == L"Graphic") {
		startup_view = 2;
	}
	else {
		startup_view = 0;
	}

	startup_properties_show = ::AfxGetApp()->GetProfileIntW(L"2.0", L"StartupPropertiesShow", 1);

	temp = ::AfxGetApp()->GetProfileStringW(L"2.0", L"PreferredDictonaryLanguage", L"English");
	if (temp == L"Japanese") {
		setAgaliaPreference(dictionary_lang, PreferredDictonaryLanguage::Japanese);
	}
	else {
		setAgaliaPreference(dictionary_lang, PreferredDictonaryLanguage::English);
	}

	int force_dictionary_vr = ::AfxGetApp()->GetProfileIntW(L"2.0", L"ForceDictionaryVR", 0);
	setAgaliaPreference(dicom_force_dictionary_vr, force_dictionary_vr ? 1 : 0);

	temp = ::AfxGetApp()->GetProfileStringW(L"2.0", L"RenderingEngine", L"Direct2D_1_1");
	if (temp == L"GDI") {
		rendering_engine = rendering_GDI;
	}
	else if (temp == L"Direct2D_1_3") {
		rendering_engine = rendering_D2D_1_3;
	}
	else {
		rendering_engine = rendering_D2D_1_1;
	}

	temp = ::AfxGetApp()->GetProfileStringW(L"2.0", L"MonitorColorProfile", L"sRGB");
	if (temp == L"Disable") {
		monitor_color_profile = color_management_disable;
	}
	else if (temp == L"System") {
		monitor_color_profile = color_management_system;
	}
	else {
		monitor_color_profile = color_management_sRGB;
	}
}



void controller::save_registry(void)
{
	CString temp;

	switch (startup_view) {
	case 1:
		temp = L"Hierarchy";
		break;
	case 2:
		temp = L"Graphic";
		break;
	default:
		temp = L"Flat";
	}
	::AfxGetApp()->WriteProfileStringW(L"2.0", L"StartupView", temp);

	::AfxGetApp()->WriteProfileInt(L"2.0", L"StartupPropertiesShow", startup_properties_show ? 1 : 0);

	int lang = 0;
	getAgaliaPreference(dictionary_lang, &lang);
	if (lang == PreferredDictonaryLanguage::Japanese) {
		temp = L"Japanese";
	}
	else {
		temp = L"English";
	}
	::AfxGetApp()->WriteProfileStringW(L"2.0", L"PreferredDictonaryLanguage", temp);

	int force_dictionary_vr = 0;
	getAgaliaPreference(dicom_force_dictionary_vr, &force_dictionary_vr);
	::AfxGetApp()->WriteProfileInt(L"2.0", L"ForceDictionaryVR", force_dictionary_vr);

	switch(rendering_engine) {
	case rendering_GDI:
		temp = L"GDI";
		break;
	case rendering_D2D_1_1:
		temp = L"Direct2D_1_1";
		break;
	case rendering_D2D_1_3:
	default:
		temp = L"Direct2D_1_3";
		break;
	}
	::AfxGetApp()->WriteProfileStringW(L"2.0", L"RenderingEngine", temp);

	switch (monitor_color_profile) {
	case color_management_disable:
		temp = L"Disable";
		break;
	case color_management_system:
		temp = L"System";
		break;
	default:
		temp = L"sRGB";
	}
	::AfxGetApp()->WriteProfileStringW(L"2.0", L"MonitorColorProfile", temp);
}



// プログレスダイアログ監視スレッド プロシージャ 
UINT AFX_CDECL watch_progress_proc(LPVOID param)
{
	auto p = reinterpret_cast<watch_progress_param*>(param);

	IProgressDialog* pDlg = p->m_pProgressDlg;
	HANDLE parse_finished_event = p->parse_finished_event;
	HANDLE abort_request_event = p->abort_request_event;

	while (::WaitForSingleObject(parse_finished_event, 10) == WAIT_TIMEOUT)
	{
		if (pDlg->HasUserCancelled())
		{
			::SetEvent(abort_request_event);
			break;
		}
	}

	::CloseHandle(parse_finished_event);
	pDlg->StopProgressDialog();
	pDlg->Release();
	delete p;
	return 0;
}



watch_progress_param* prepare_progress(HWND hwdParent, const wchar_t* title)
{
	IProgressDialog* pDlg = nullptr;
	auto hr = ::CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_IProgressDialog, (void**)& pDlg);
	if (SUCCEEDED(hr))
	{
		TCHAR szAppName[256] = {};
		::AfxLoadString(AFX_IDS_APP_TITLE, szAppName);

		pDlg->SetTitle(szAppName);
		pDlg->StartProgressDialog(hwdParent, NULL, PROGDLG_MARQUEEPROGRESS | PROGDLG_NOTIME, NULL);
		pDlg->SetLine(0, title, TRUE, NULL);
	}

	auto param = new watch_progress_param;
	param->m_pProgressDlg = pDlg;
	param->abort_request_event = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	param->parse_finished_event = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	return param;
}



void ResetFlatView_Stub(parser_thread_param* param, int& pos, int count, agaliaElement* item)
{
	HRESULT hr = S_OK;
	while (SUCCEEDED(hr))
	{
		agaliaPtr<agaliaElement> sp;
		sp.attach(item);

		if (::WaitForSingleObject(param->abort_request_event, 0) != WAIT_TIMEOUT)
			return;
		if (::WaitForSingleObject(param->terminate_thread_request, 0) != WAIT_TIMEOUT)
			return;

		uint32_t row = 0;
		hr = param->ctrl->image->getElementInfoCount(item, &row);
		if (SUCCEEDED(hr))
		{
			for (uint32_t i = 0; i < row; i++)
			{
				agaliaStringPtr str;
				hr = param->ctrl->image->getElementInfoValue(item, i, 0, &str);
				if (FAILED(hr))
				{
					break;
				}

				if (i == 0)
				{
					LV_ITEM lvItem = {};
					lvItem.mask = LVIF_TEXT | LVIF_PARAM;
					lvItem.iItem = pos;
					lvItem.pszText = str->GetData();
					lvItem.cchTextMax = -1;
					lvItem.lParam = reinterpret_cast<LPARAM>(sp.detach());
					ListView_InsertItem(param->hwndTarget, &lvItem);
				}
				else
				{
					LV_ITEM lvItem = {};
					lvItem.mask = LVIF_TEXT;
					lvItem.iItem = pos;
					lvItem.pszText = str->GetData();
					lvItem.cchTextMax = -1;
					ListView_InsertItem(param->hwndTarget, &lvItem);
				}

				for (int j = 1; j < count; j++)
				{
					hr = param->ctrl->image->getElementInfoValue(item, i, j, &str);
					if (SUCCEEDED(hr))
					{
						ListView_SetItemText(param->hwndTarget, pos, j, str->GetData());
						str.detach()->Release();
					}
				}

				pos++;
			}
		}

		// 子アイテムを取得 
		uint32_t sibling = 0;
		do
		{
			agaliaPtr<agaliaElement> child;
			hr = item->getChild(sibling++, &child);
			if (SUCCEEDED(hr)) {
				ResetFlatView_Stub(param, pos, count, child.detach());
			}
		} while (SUCCEEDED(hr));

		// 次のアイテムを取得 
		agaliaPtr<agaliaElement> next;
		hr = item->getNext(&next);
		if (SUCCEEDED(hr)) {
			item = next.detach();
		}
	}
}



UINT ResetFlatViewThreadProc(LPVOID param)
{
	auto p = reinterpret_cast<parser_thread_param*>(param);

	agaliaPtr<agaliaElement> root;
	auto hr = p->ctrl->image->getRootElement(&root);
	if (SUCCEEDED(hr)) {
		uint32_t count = 0;
		hr = p->ctrl->image->getColumnCount(&count);
		if (SUCCEEDED(hr)) {
			int pos = 0;
			ResetFlatView_Stub(p, pos, count, root.detach());
		}
	}
	::SetEvent(p->parse_finished_event);

	::CloseHandle(p->abort_request_event);
	delete p;
	return 0;
}



void ResetHierarchyView_Stub(parser_thread_param* param, HTREEITEM hParent, agaliaElement* parent)
{
	agaliaPtr<agaliaElement> item;
	item.attach(parent);

	HRESULT hr = S_OK;

	bool is_first = true;
	while (item._p)
	{
		if (::WaitForSingleObject(param->abort_request_event, 0) != WAIT_TIMEOUT)
			return;
		if (::WaitForSingleObject(param->terminate_thread_request, 0) != WAIT_TIMEOUT)
			return;

		agaliaElement* current = item._p;

		agaliaStringPtr str;
		hr = item->getName(&str);
		if (FAILED(hr))
			break;

		agaliaPtr<agaliaElement> next;
		auto hrNext = item->getNext(&next);

		TVINSERTSTRUCTW s = {};
		s.hParent = hParent;
		s.hInsertAfter = TVI_LAST;
		s.itemex.mask = TVIF_TEXT | TVIF_PARAM;
		s.itemex.pszText = str->GetData();
		s.itemex.cchTextMax = -1;
		s.itemex.lParam = reinterpret_cast<LPARAM>(item.detach());
		HTREEITEM hItem = TreeView_InsertItem(param->hwndTarget, &s);
		::UpdateWindow(param->hwndTarget);

		if (hParent == TVI_ROOT && is_first)
		{
			TreeView_SelectItem(param->hwndTarget, hItem);
		}
		is_first = false;

		uint32_t sibling = 0;
		do
		{
			agaliaPtr<agaliaElement> child;
			hr = current->getChild(sibling++, &child);
			if (SUCCEEDED(hr))
			{
				ResetHierarchyView_Stub(param, hItem, child.detach());
			}
		} while (SUCCEEDED(hr));

		if (SUCCEEDED(hrNext))
		{
			item.attach(next.detach());
		}

		str->Free();
	}
}



UINT ResetHierarchyViewThreadProc(LPVOID param)
{
	auto p = reinterpret_cast<parser_thread_param*>(param);

	agaliaPtr<agaliaElement> root;
	auto hr = p->ctrl->image->getRootElement(&root);
	if (SUCCEEDED(hr)) {
		ResetHierarchyView_Stub(p, TVI_ROOT, root.detach());
	}
	::SetEvent(p->parse_finished_event);

	::CloseHandle(p->abort_request_event);
	delete p;
	return 0;
}



void launch_parser(controller* ctrl, AFX_THREADPROC pfnThreadProc, watch_progress_param* param1, parser_thread_param* param2)
{
	// プログレス監視スレッドを起動 
	::AfxBeginThread(watch_progress_proc, param1);

	// 解析スレッドを起動 
	ctrl->parse_thread = ::AfxBeginThread(pfnThreadProc, param2, 0, CREATE_SUSPENDED);
	ctrl->parse_thread->m_bAutoDelete = FALSE;
	ctrl->parse_thread->ResumeThread();
}
