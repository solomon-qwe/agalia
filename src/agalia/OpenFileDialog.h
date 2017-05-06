#pragma once


// COpenFileDialog

class COpenFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(COpenFileDialog)

public:
	COpenFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);
	virtual ~COpenFileDialog();

public:
	CString strOffset;
	CString strSize;

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnFileNameOK();
	virtual INT_PTR DoModal();
};


