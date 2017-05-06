// OpenFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "agalia.h"
#include "OpenFileDialog.h"


// COpenFileDialog

IMPLEMENT_DYNAMIC(COpenFileDialog, CFileDialog)

COpenFileDialog::COpenFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{

}

COpenFileDialog::~COpenFileDialog()
{
}


BEGIN_MESSAGE_MAP(COpenFileDialog, CFileDialog)
END_MESSAGE_MAP()

INT_PTR COpenFileDialog::DoModal()
{
	AddText(1001, _T("offset:"));
	AddEditBox(1002, _T("0"));
	AddText(1003, _T("size:"));
	AddEditBox(1004, _T("0"));

	return CFileDialog::DoModal();
}


BOOL COpenFileDialog::OnFileNameOK()
{
	GetEditBoxText(1002, strOffset);
	GetEditBoxText(1004, strSize);

	return CFileDialog::OnFileNameOK();
}

