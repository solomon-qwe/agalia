// OptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agalia.h"
#include "OptionDlg.h"
#include "afxdialogex.h"


// COptionDlg dialog

IMPLEMENT_DYNAMIC(COptionDlg, CDialog)

COptionDlg::COptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_OPTION, pParent)
{

}

COptionDlg::~COptionDlg()
{
}

void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PREF_LANG, m_cmb_pref_lang);
	DDX_Control(pDX, IDC_EDIT_LIMIT_LENGTH, m_edt_limit_length);
	DDX_Control(pDX, IDC_CHECK_VR, m_chk_vr);
	DDX_Control(pDX, IDC_SPIN_LIMIT_LENGTH, m_spin_limit_length);
}


BEGIN_MESSAGE_MAP(COptionDlg, CDialog)
END_MESSAGE_MAP()


// COptionDlg message handlers


BOOL COptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_cmb_pref_lang.AddString(L"English");
	m_cmb_pref_lang.AddString(L"Japanese");

	m_edt_limit_length.SetLimitText(5);
	m_spin_limit_length.SetRange32(1, 99999);

	CString strLang = ::AfxGetApp()->GetProfileStringW(L"", L"PreferredLanguage", L"English");
	m_cmb_pref_lang.SelectString(0, strLang);

	UINT uLimitLeng = ::AfxGetApp()->GetProfileIntW(L"", L"ByteStreamLimitLength", 128);
	m_spin_limit_length.SetPos32(uLimitLeng);

	UINT uVR_Preferrentially = ::AfxGetApp()->GetProfileIntW(L"", L"VR_Preferrentially", 0);
	m_chk_vr.SetCheck(uVR_Preferrentially);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void COptionDlg::OnOK()
{
	CString strLang, strLimitLength;
	m_cmb_pref_lang.GetWindowTextW(strLang);
	m_spin_limit_length.GetWindowTextW(strLimitLength);

	::AfxGetApp()->WriteProfileStringW(L"", L"PreferredLanguage", strLang);
	::AfxGetApp()->WriteProfileInt(L"", L"ByteStreamLimitLength", m_spin_limit_length.GetPos32());
	::AfxGetApp()->WriteProfileInt(L"", L"VR_Preferrentially", m_chk_vr.GetCheck());

	CDialog::OnOK();
}
