
// agalia.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "agalia.h"
#include "MainFrm.h"

#include "../inc/agalia_version.h"
#include "../inc/agaliarept.h"
#include "../inc/decode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAgaliaApp

BEGIN_MESSAGE_MAP(CAgaliaApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CAgaliaApp::OnAppAbout)
END_MESSAGE_MAP()


// CAgaliaApp construction

CAgaliaApp::CAgaliaApp() noexcept
{
	SetAppID(_T(AGALIA_APPID));
}

// The one and only CAgaliaApp object

CAgaliaApp theApp;


// CAgaliaApp initialization

BOOL CAgaliaApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T(AGALIA_MANUFACTURER));


	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	//CFrameWnd* pFrame = new CMainFrame;
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, nullptr,
		nullptr);





	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	initializeAgaliaDecoder();

	// perse command line 
	agaliaPtr<agaliaCmdLineParam> param;
	auto hr = agaliaCmdLineParam::parseCmdLine(&param);
	if (hr == S_OK) {
		pFrame->ResetContents(param->getTargetFilePath(), param->getOffset(), param->getSize(), agalia_format_auto);
	}

	return TRUE;
}

int CAgaliaApp::ExitInstance()
{
	finalizeAgaliaDecoder();

	return CWinAppEx::ExitInstance();
}

// CAgaliaApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CAgaliaApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CAgaliaApp message handlers



