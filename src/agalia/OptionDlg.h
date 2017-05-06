#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// COptionDlg dialog

class COptionDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionDlg)

public:
	COptionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptionDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPTION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_cmb_pref_lang;
	CEdit m_edt_limit_length;
	CButton m_chk_vr;
	CSpinButtonCtrl m_spin_limit_length;
	virtual void OnOK();
};
