#include "pch.h"

#include "dcm_common.h"

#include <atlbase.h>
#include <atlstr.h>

#include <xmllite.h>
#pragma comment(lib, "xmllite.lib")

#include <sstream>
#include <iomanip>

std::unordered_map<std::wstring, dcm_dic_elem>* g_dcm_dic_elem = nullptr;
std::unordered_map<std::string, dcm_dic_uid>* g_dcm_dic_uid = nullptr;

bool findVR(dcm_dic_elem_iterator& it, uint16_t group, uint16_t element)
{
	wchar_t sz[16];
	swprintf_s(sz, L"(%.4X,%.4X)", group, element);
	it = g_dcm_dic_elem->find(sz);
	return (it != g_dcm_dic_elem->end());
}


bool findUID(dcm_dic_uid_iterator& it, const char* uid)
{
	it = g_dcm_dic_uid->find(uid);
	return (it != g_dcm_dic_uid->end());
}


void load_dcm_dictionary(LPCTSTR pszFilePath, const wchar_t* lang)
{
	CComPtr<IStream> pStream;
	if (FAILED(::SHCreateStreamOnFile(pszFilePath, STGM_READ, &pStream))) {
		return;
	}

	CComPtr<IXmlReader> pReader;
	if (FAILED(::CreateXmlReader(__uuidof(IXmlReader), reinterpret_cast<void**>(&pReader), 0))) {
		return;
	}
	if (FAILED(pReader->SetInput(pStream))) {
		return;
	}

	XmlNodeType nodeType = XmlNodeType_None;
	while (pReader->Read(&nodeType) == S_OK)
	{
		if (nodeType != XmlNodeType_Element) {
			continue;
		}

		LPCWSTR pszLocalName = nullptr;
		HRESULT result = pReader->GetLocalName(&pszLocalName, NULL);
		if (wcscmp(pszLocalName, L"elem") == 0)
		{
			std::wstring tag;
			dcm_dic_elem val;

			result = pReader->MoveToAttributeByName(L"tag", NULL);
			if (result == S_OK)
			{
				LPCWSTR pszValue = nullptr;
				pReader->GetValue(&pszValue, NULL);
				tag = pszValue;
			}

			result = pReader->MoveToAttributeByName(L"vr", NULL);
			if (result == S_OK)
			{
				LPCWSTR pszValue = nullptr;
				pReader->GetValue(&pszValue, NULL);
				CStringA str(pszValue);
				val.vr = str;
			}

			result = pReader->MoveToAttributeByName(lang, NULL);
			if (result == S_OK)
			{
				LPCWSTR pszValue = nullptr;
				pReader->GetValue(&pszValue, NULL);
				if (pszValue != nullptr)
					val.name = pszValue;
			}
			else if (wcscmp(lang, L"enu") != 0)
			{
				result = pReader->MoveToAttributeByName(L"enu", NULL);
				if (result == S_OK)
				{
					LPCWSTR pszValue = nullptr;
					pReader->GetValue(&pszValue, NULL);
					if (pszValue != nullptr)
						val.name = pszValue;
				}
			}

			// 上書き（辞書に二重登録されていた場合は後から読み込まれたものが有効） 
			(*g_dcm_dic_elem)[tag] = val;
		}
		else if (wcscmp(pszLocalName, L"uid") == 0)
		{
			std::string uid;
			dcm_dic_uid val;

			result = pReader->MoveToAttributeByName(L"value", NULL);
			if (result == S_OK)
			{
				LPCWSTR pszValue = nullptr;
				pReader->GetValue(&pszValue, NULL);
				CStringA str(pszValue);
				uid = str;
			}

			result = pReader->MoveToAttributeByName(lang, NULL);
			if (result == S_OK)
			{
				LPCWSTR pszValue = nullptr;
				pReader->GetValue(&pszValue, NULL);
				if (pszValue != nullptr)
					val.name = pszValue;
			}
			else if (wcscmp(lang, L"enu") != 0)
			{
				result = pReader->MoveToAttributeByName(L"enu", NULL);
				if (result == S_OK)
				{
					LPCWSTR pszValue = nullptr;
					pReader->GetValue(&pszValue, NULL);
					if (pszValue != nullptr)
						val.name = pszValue;
				}
			}

			// 上書き（辞書に二重登録されていた場合は後から読み込まれたものが有効） 
			(*g_dcm_dic_uid)[uid] = val;
		}
	}
}


void clear_dcm_dictionary(void)
{
	if (g_dcm_dic_elem)
	{
		g_dcm_dic_elem->clear();
		delete g_dcm_dic_elem;
		g_dcm_dic_elem = nullptr;
	}

	if (g_dcm_dic_uid)
	{
		g_dcm_dic_uid->clear();
		delete g_dcm_dic_uid;
		g_dcm_dic_uid = nullptr;
	}
}


void init_dcm_dictionary(_In_opt_ const wchar_t* lang)
{
	// 指定されていなければ英語 
	if (lang == nullptr) {
		lang = L"enu";
	}

	// 空の辞書を用意  
	clear_dcm_dictionary();
	g_dcm_dic_elem = new std::unordered_map<std::wstring, dcm_dic_elem>;
	g_dcm_dic_uid = new std::unordered_map<std::string, dcm_dic_uid>;

	TCHAR szFilePath[_MAX_PATH] = {};
	extern HMODULE g_hModule;
	::GetModuleFileName(g_hModule, szFilePath, _countof(szFilePath));

	// 標準タグの辞書を読み込む 
	::PathRemoveFileSpec(szFilePath);
	::PathAppend(szFilePath, _T("dcm_standard.xml"));
	load_dcm_dictionary(szFilePath, lang);

	// プライベートタグの辞書を読み込む 
	::PathRemoveFileSpec(szFilePath);
	::PathAppend(szFilePath, _T("dcm_private.xml"));
	load_dcm_dictionary(szFilePath, lang);
}
