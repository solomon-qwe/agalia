// OptionDlg.cpp : implementation file
//

#include "pch.h"
#include "agalia.h"
#include "OptionDlg.h"
#include "afxdialogex.h"

#include <vector>
#include <string>
#include "../inc/agaliarept.h"

struct myitemset {
	std::wstring name;
	std::wstring desc;
	std::vector<std::wstring> options;
};


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



void add_prop(CMFCPropertyGridProperty* propGroup, const myitemset& set, size_t default)
{
	auto prop = new CMFCPropertyGridProperty(set.name.c_str(), set.options[default].c_str(), set.desc.c_str());
	for (auto op : set.options) {
		prop->AddOption(op.c_str());
	}
	prop->AllowEdit(FALSE);
	propGroup->AddSubItem(prop);
}




// OptionDlg dialog

IMPLEMENT_DYNAMIC(OptionDlg, CDialogEx)

OptionDlg::OptionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OPTION, pParent)
{

}

OptionDlg::~OptionDlg()
{
}

void OptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID, grid);
}


BEGIN_MESSAGE_MAP(OptionDlg, CDialogEx)
END_MESSAGE_MAP()


// OptionDlg message handlers

BOOL OptionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
	grid.SetFont(GetFont());
	grid.SetLeftColumnWidth(200);

	{
		auto propGroup = new CMFCPropertyGridProperty(L"Startup");
		grid.AddProperty(propGroup);

		{
			myitemset item;
			item.name = L"View";
			item.desc = L"";
			item.options.push_back(L"Flat");
			item.options.push_back(L"Hierarchy");
			item.options.push_back(L"Graphic");
			int default = startup_view;

			add_prop(propGroup, item, default);
			set.insert(std::pair<std::wstring, myitemset>(item.name, item));
		}

		{
			myitemset item;
			item.name = L"Properties";
			item.desc = L"";
			item.options.push_back(L"Hidden");
			item.options.push_back(L"Show");
			int default = startup_properties_show;

			add_prop(propGroup, item, default);
			set.insert(std::pair<std::wstring, myitemset>(item.name, item));
		}
	}

	{
		auto propGroup = new CMFCPropertyGridProperty(L"Dictionary");
		grid.AddProperty(propGroup);

		{
			myitemset item;
			item.name = L"Language";
			item.desc = L"primary language";
			item.options.push_back(L"English");
			item.options.push_back(L"Japanese");
			int default = dictionary_lang;

			add_prop(propGroup, item, default);
			set.insert(std::pair<std::wstring, myitemset>(item.name, item));
		}
	}

	{
		auto propGroup = new CMFCPropertyGridProperty(L"DICOM");
		grid.AddProperty(propGroup);

		{
			myitemset item;
			item.name = L"Force dictonary VR";
			item.desc = L"";
			item.options.push_back(L"Disable");
			item.options.push_back(L"Enable");
			int default = dicom_force_dictionary_vr;

			add_prop(propGroup, item, default);
			set.insert(std::pair<std::wstring, myitemset>(item.name, item));
		}
	}

	{
		auto propGroup = new CMFCPropertyGridProperty(L"Graphic");
		grid.AddProperty(propGroup);

		{
			myitemset item;
			item.name = L"Monitor Color Profile";
			item.desc = L"";
			item.options.push_back(L"Disable");
			item.options.push_back(L"sRGB");
			item.options.push_back(L"System");
			int default = monitor_color_profile;

			add_prop(propGroup, item, default);
			set.insert(std::pair<std::wstring, myitemset>(item.name, item));
		}

		{
			myitemset item;
			item.name = L"Rendering";
			item.desc = L"";
			item.options.push_back(L"GDI");
			item.options.push_back(L"Direct2D 1.1 (Windows 7 SP1 and later)");
			item.options.push_back(L"Direct2D 1.3 (Windows 10 and later)");
			int default = rendering_engine;

			add_prop(propGroup, item, default);
			set.insert(std::pair<std::wstring, myitemset>(item.name, item));
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}



void OptionDlg::OnOK()
{
	int groups = grid.GetPropertyCount();
	for (int i = 0; i < groups; i++)
	{
		auto group = grid.GetProperty(i);

		int props = group->GetSubItemsCount();
		for (int j = 0; j < props; j++)
		{
			auto prop = group->GetSubItem(j);
			CString name = prop->GetName();
			const COleVariant& v = prop->GetValue();
			CString val = v.bstrVal;

			int index = 0;
			auto item = set.find(name.GetString());
			if (item != set.end())
			{
				auto x = item->second.options;
				auto e = std::find(x.begin(), x.end(), val.GetString());
				if (e != x.end())
				{
					index = int(std::distance(x.begin(), e));
				}
			}

			if (name == L"View")
			{
				startup_view = index;
			}
			else if (name == L"Properties")
			{
				startup_properties_show = index;
			}
			else if (name == L"Language")
			{
				dictionary_lang = index;
			}
			else if (name == L"Force dictonary VR")
			{
				dicom_force_dictionary_vr = index;
			}
			else if (name == L"Monitor Color Profile")
			{
				monitor_color_profile = index;
			}
			else if (name == L"Rendering")
			{
				rendering_engine = index;
			}
		}
	}

	CDialogEx::OnOK();
}
