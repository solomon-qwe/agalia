// OpenFileDialog.cpp : implementation file
//

#include "pch.h"
#include "agalia.h"
#include "OpenFileDialog.h"

#include "../inc/agaliarept.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// OpenFileDialog

IMPLEMENT_DYNAMIC(OpenFileDialog, CFileDialog)

OpenFileDialog::OpenFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{

}

OpenFileDialog::~OpenFileDialog()
{
}


BEGIN_MESSAGE_MAP(OpenFileDialog, CFileDialog)
END_MESSAGE_MAP()



// OpenFileDialog message handlers


enum OpenFileDialog_ResouceID {
	ID_OFFSET_LABEL = 1001,
	ID_OFFSET_VALUE,
	ID_SIZE_LABEL,
	ID_SIZE_VALUE,
	ID_FORMAT_LABEL,
	ID_FORMAT_VALUE
};

INT_PTR OpenFileDialog::DoModal()
{
	AddText(ID_OFFSET_LABEL, _T("O&ffset:"));
	AddEditBox(ID_OFFSET_VALUE, _T("0"));

	AddText(ID_SIZE_LABEL, _T("&Size:"));
	AddEditBox(ID_SIZE_VALUE, _T("0"));

	AddText(ID_FORMAT_LABEL, _T("Fo&rmat:"));
	AddComboBox(ID_FORMAT_VALUE);
	AddControlItem(ID_FORMAT_VALUE, 0, _T("Auto"));

	int counts = 0;
	getAgaliaSupportTypeCount(&counts);
	for (int i = 0; i < counts; i++)
	{
		agaliaStringPtr name;
		auto hr = getAgaliaSupportTypeName(i, &name);
		if (SUCCEEDED(hr)) {
			AddControlItem(ID_FORMAT_VALUE, i + 1, name->GetData());
		}
	}

	SetSelectedControlItem(ID_FORMAT_VALUE, 0);

	return CFileDialog::DoModal();
}


BOOL OpenFileDialog::OnFileNameOK()
{
	GetEditBoxText(ID_OFFSET_VALUE, strOffset);
	GetEditBoxText(ID_SIZE_VALUE, strSize);
	DWORD dwFormat = 0;
	GetSelectedControlItem(ID_FORMAT_VALUE, dwFormat);
	nFormat = static_cast<int>(dwFormat) - 1;

	return CFileDialog::OnFileNameOK();
}
