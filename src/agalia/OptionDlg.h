#pragma once

#include <unordered_map>
#include <string>
struct myitemset;

// OptionDlg dialog

class CMyPropertyGridCtrl : public CMFCPropertyGridCtrl
{
public:
	CMyPropertyGridCtrl() {}
	virtual ~CMyPropertyGridCtrl() {}

	void SetLeftColumnWidth(int cx)
	{
		m_nLeftColumnWidth = cx;
	}
};

class OptionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(OptionDlg)

public:
	OptionDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~OptionDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPTION };
#endif

protected:
	std::unordered_map<std::wstring, myitemset> set;

public:
	int startup_view = 0;
	int startup_properties_show = 0;
	int dictionary_lang = 0;
	int dicom_force_dictionary_vr = 0;
	int monitor_color_profile = 0;
	int rendering_engine = 0;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CMyPropertyGridCtrl grid;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
