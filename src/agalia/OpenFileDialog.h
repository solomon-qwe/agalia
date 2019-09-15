#pragma once


// OpenFileDialog

class OpenFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(OpenFileDialog)

public:
	OpenFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = nullptr,
		LPCTSTR lpszFileName = nullptr,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = nullptr,
		CWnd* pParentWnd = nullptr);
	virtual ~OpenFileDialog();

public:
	CString strOffset;
	CString strSize;
	int nFormat = 0;

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual INT_PTR DoModal();
	virtual BOOL OnFileNameOK();
};


