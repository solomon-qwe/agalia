// AgaliaController.cpp : implementation file
//

#include "stdafx.h"
#include "agalia.h"
#include "AgaliaController.h"

#include "MainFrm.h"
#include "../inc/agaliarept.h"

#include <sstream>	// for std::wstring 

#define WM_EXIT_AGALIA_MAIN	(WM_APP+1)

// CAgaliaController

IMPLEMENT_DYNAMIC(CAgaliaController, CWnd)

CAgaliaController::CAgaliaController()
{
	m_hAbortEvent = NULL;
	m_hWatchEvent = NULL;
	m_pWatchThread = nullptr;
	m_pAgaliaThread = nullptr;
	m_pAgaliaConfig = nullptr;
	m_pProgressDlg = nullptr;
}

CAgaliaController::~CAgaliaController()
{
}


BEGIN_MESSAGE_MAP(CAgaliaController, CWnd)
	ON_MESSAGE(WM_EXIT_AGALIA_MAIN, &CAgaliaController::OnExitAgaliaMain)
END_MESSAGE_MAP()

// CAgaliaController message handlers

void CAgaliaController::Init()
{
	m_hAbortEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hWatchEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	CreateEx(0, ::AfxRegisterWndClass(0), _T(""), WS_POPUP, CRect(0, 0, 0, 0), nullptr, 0);
}


void CAgaliaController::Exit()
{
	DestroyWindow();

	::SetEvent(m_hAbortEvent);
	::SetEvent(m_hWatchEvent);

	CWinThread* pThread = m_pWatchThread;
	if (pThread)
	{
		::WaitForSingleObject(pThread->m_hThread, INFINITE);
		m_pWatchThread = nullptr;
		pThread->m_bAutoDelete = TRUE;
		pThread->Delete();
	}

	pThread = m_pAgaliaThread;
	if (pThread)
	{
		::WaitForSingleObject(pThread->m_hThread, INFINITE);
		m_pAgaliaThread = nullptr;
		pThread->m_bAutoDelete = TRUE;
		pThread->Delete();
	}

	IProgressDialog* pDlg = m_pProgressDlg;
	if (pDlg)
	{
		pDlg = nullptr;
		pDlg->Release();
	}

	HANDLE h = m_hAbortEvent;
	m_hAbortEvent = NULL;
	::CloseHandle(h);

	h = m_hWatchEvent;
	m_hWatchEvent = NULL;
	::CloseHandle(h);
}

void CAgaliaController::ResetContents(LPCTSTR pszFilePath, uint64_t offset, uint64_t size)
{
	if (m_pAgaliaThread) return;

	CMainFrame* pFrame = dynamic_cast<CMainFrame*>(::AfxGetMainWnd());

	agalia::config* config;
	::AgaliaGetConfig(&config);

	// �I�v�V�����ݒ� 
	CString strLang = ::AfxGetApp()->GetProfileStringW(L"", L"PreferredLanguage", L"English");
	if (strLang == L"Japanese") {
		config->preferred_language = agalia::config::jpn;
	}
	config->byte_stream_limit_length = ::AfxGetApp()->GetProfileIntW(L"", L"ByteStreamLimitLength", 128);
	config->force_dictionary_vr = (::AfxGetApp()->GetProfileIntW(L"", L"VR_Preferrentially", 0) != 0);

	// �R�}���h���C���ݒ�ŏ㏑�� 
	HRESULT result = config->parse_command_line();
	if (result != S_OK && pszFilePath == nullptr) {
		config->release();
		return;
	}

	// �֐��̈����ŏ㏑�� 
	if (pszFilePath) {
		config->set_input_file_path(pszFilePath);
	}
	if (offset) {
		config->offset = offset;
	}
	if (size) {
		config->data_size = size;
	}

	config->hwndListCtrl = pFrame->m_wndView;
	config->abort_event = m_hAbortEvent;

	// ���X�g�r���[�R���g���[���̒��g���폜 
	pFrame->m_wndView.DeleteAllItems();
	int colmuns = pFrame->m_wndView.GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < colmuns; i++)
	{
		pFrame->m_wndView.DeleteColumn(0);
	}

	// ��͑Ώۂ̃t�@�C���p�X���E�B���h�E�^�C�g���ɐݒ� 
	TCHAR szAppName[256] = {};
	::AfxLoadString(AFX_IDS_APP_TITLE, szAppName);
	TCHAR szTitle[1024] = {};
	if (config->get_input_file_path()) {
		swprintf_s(szTitle, L"%s - %s", config->get_input_file_path(), szAppName);
		pFrame->m_wndView.m_strFilePath = config->get_input_file_path();
	}
	else {
		swprintf_s(szTitle, L"%s", szAppName);
	}
	pFrame->SetWindowTextW(szTitle);

	::ResetEvent(m_hAbortEvent);
	::ResetEvent(m_hWatchEvent);

	m_pAgaliaConfig = config;

	// �v���O���X�_�C�A���O��\�� 
	IProgressDialog* pDlg = nullptr;
	result = ::CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_IProgressDialog, (void**)&pDlg);
	if (SUCCEEDED(result))
	{
		pDlg->SetTitle(szAppName);
		pDlg->StartProgressDialog(pFrame->GetSafeHwnd(), NULL, PROGDLG_MARQUEEPROGRESS | PROGDLG_NOTIME, NULL);
		pDlg->SetLine(0, config->get_input_file_path(), TRUE, NULL);
	}
	m_pProgressDlg = pDlg;

	// �v���O���X�Ď��X���b�h������ 
	CWinThread* pThread = ::AfxBeginThread(WatchProgressProc, this, 0, 0, CREATE_SUSPENDED, NULL);
	pThread->m_bAutoDelete = FALSE;
	m_pWatchThread = pThread;

	// ��̓X���b�h������ 
	pThread = ::AfxBeginThread(AgaliaMainProc, this, 0, 0, CREATE_SUSPENDED, NULL);
	pThread->m_bAutoDelete = FALSE;
	m_pAgaliaThread = pThread;

	// �X���b�h���N�� 
	m_pWatchThread->ResumeThread();
	m_pAgaliaThread->ResumeThread();
}

// �f�[�^��̓X���b�h �v���V�[�W�� 
UINT AFX_CDECL CAgaliaController::AgaliaMainProc(LPVOID p)
{
	CAgaliaController* pApp = reinterpret_cast<CAgaliaController*>(p);

	HWND hwnd = pApp->GetSafeHwnd();
	agalia::config* config = reinterpret_cast<agalia::config*>(pApp->m_pAgaliaConfig);

	HRESULT result = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(result)) {
		return result;
	}

	result = AgaliaMain(config);

	::CoUninitialize();

	// ���C���X���b�h�ɏI���ʒm 
	::PostMessage(hwnd, WM_EXIT_AGALIA_MAIN, 0, result);

	return 0;
}

// �v���O���X�_�C�A���O�Ď��X���b�h �v���V�[�W�� 
UINT AFX_CDECL CAgaliaController::WatchProgressProc(LPVOID p)
{
	CAgaliaController* pApp = reinterpret_cast<CAgaliaController*>(p);

	IProgressDialog* pDlg = pApp->m_pProgressDlg;
	HANDLE hWatchEvent = pApp->m_hWatchEvent;
	HANDLE hAbortEvent = pApp->m_hAbortEvent;

	while (::WaitForSingleObject(hWatchEvent, 10) == WAIT_TIMEOUT)
	{
		if (pDlg->HasUserCancelled())
		{
			::SetEvent(hAbortEvent);
			break;
		}
	}

	return 0;
}

bool CAgaliaController::IsProcessing(void)
{
	return (m_pAgaliaThread != nullptr);
}


LRESULT CAgaliaController::OnExitAgaliaMain(WPARAM, LPARAM lParam)
{
	CAgaliaController* pApp = this;

									// �v���O���X�Ď��X���b�h���~�����邽�߂ɃV�O�i����  
	::SetEvent(pApp->m_hWatchEvent);

	// �v���O���X�_�C�A���O���~������ 
	if (!pApp->m_pProgressDlg->HasUserCancelled()) {
		pApp->m_pProgressDlg->SetProgress(1, 1);
	}
	pApp->m_pProgressDlg->StopProgressDialog();

	// �v���O���X�_�C�A���O�Ď��X���b�h�̏I����҂��Ă���X���b�h�I�u�W�F�N�g��j�� 
	CWinThread* pThread = pApp->m_pWatchThread;
	if (pThread)
	{
		::WaitForSingleObject(pThread->m_hThread, INFINITE);
		pApp->m_pWatchThread = nullptr;
		pThread->m_bAutoDelete = TRUE;
		pThread->Delete();
	}

	// �v���O���X�_�C�A���O��j�� 
	IProgressDialog* pDlg = pApp->m_pProgressDlg;
	pApp->m_pProgressDlg = nullptr;
	pDlg->Release();

	pThread = pApp->m_pAgaliaThread;
	if (pThread)
	{
		::WaitForSingleObject(pThread->m_hThread, INFINITE);
		pApp->m_pAgaliaThread = nullptr;
		pThread->m_bAutoDelete = TRUE;
		pThread->Delete();
	}

	agalia::config* config = reinterpret_cast<agalia::config*>(pApp->m_pAgaliaConfig);
	if (config)
	{
		pApp->m_pAgaliaConfig = nullptr;
		config->release();
	}

	HRESULT result = (HRESULT)lParam;
	switch (result)
	{
	case S_OK:
		break;

	case AGALIA_ERR_UNSUPPORTED:
		::AfxMessageBox(IDS_UNSUPPORTED_FILE_TYPE);
		break;

	case AGALIA_ERR_IN_PAGE_ERROR:
		::AfxMessageBox(IDS_IN_PAGE_ERROR, MB_ICONSTOP);
		break;

	case AGALIA_ERR_ABORT:
		::AfxMessageBox(IDS_USER_ABORTED, MB_ICONINFORMATION);
		break;

	default:
		::AfxMessageBox(IDS_UNKNOWN_ERROR);
	}
	return 0;
}


void dump(const wchar_t* szFilePath, LPARAM lParam)
{
	agalia::list_item_param* param = reinterpret_cast<agalia::list_item_param*>(lParam);

	CFileDialog dlg(FALSE);
	if (dlg.DoModal() == IDOK)
	{
		HANDLE hFile = ::CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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

		HANDLE h = ::CreateFile(dlg.GetOFN().lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD dwNumberOfBytesWritten = 0;
		::WriteFile(h, reinterpret_cast<BYTE*>(pFile) + param->offset, (DWORD)param->size, &dwNumberOfBytesWritten, NULL);
		::CloseHandle(h);

		::UnmapViewOfFile(pFile);
		::CloseHandle(hMap);
		::CloseHandle(hFile);
	}
}


void analyze(const wchar_t* szFilePath, LPARAM lParam)
{
	agalia::list_item_param* param = reinterpret_cast<agalia::list_item_param*>(lParam);

	agalia::config_ptr config;
	::AgaliaGetConfig(&config);
	config->parse_command_line();

	std::wstring str;
	wchar_t temp[1024] = {};

	::GetModuleFileName(NULL, temp, _countof(temp));
	::PathQuoteSpaces(temp);
	str += temp;
	str += L" ";

	wcscpy_s(temp, szFilePath);
	::PathQuoteSpaces(temp);
	str += temp;
	str += L" ";

	swprintf_s(temp, L"/byte_stream_limit_length:%u ", config->byte_stream_limit_length);
	str += temp;

	if (config->force_dictionary_vr) {
		str += L"/force_dictionary_vr ";
	}

	swprintf_s(temp, L"/offset:%llu ", param->offset);
	str += temp;

	swprintf_s(temp, L"/size:%llu ", param->size);
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

