#pragma once

#include <stdint.h>	// for uint**_t 


// CAgaliaController

class CAgaliaController : public CWnd
{
	DECLARE_DYNAMIC(CAgaliaController)

public:
	CAgaliaController();
	virtual ~CAgaliaController();

protected:
	afx_msg LRESULT OnExitAgaliaMain(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	HANDLE m_hAbortEvent;
	HANDLE m_hWatchEvent;
	CWinThread* m_pWatchThread;
	CWinThread* m_pAgaliaThread;
	void* m_pAgaliaConfig;
	IProgressDialog* m_pProgressDlg;

	void Init();
	void Exit();
	void ResetContents(LPCTSTR pszFilePath, uint64_t offset = 0, uint64_t size = 0);
	bool IsProcessing(void);

protected:
	static UINT AFX_CDECL AgaliaMainProc(LPVOID p);
	static UINT AFX_CDECL WatchProgressProc(LPVOID p);
};


void dump(const wchar_t* szFilePath, LPARAM lParam);
void analyze(const wchar_t* szFilePath, LPARAM lParam);
