#include "pch.h"

#include "tiff_common.h"

#include <atlbase.h>
#include <atlstr.h>

#include <xmllite.h>
#pragma comment(lib, "xmllite.lib")

#include <sstream>
#include <iomanip>

std::unordered_map<std::wstring, std::wstring>* g_tif_dic = nullptr;


void load_tif_dictionary(LPCTSTR pszFilePath, const wchar_t* lang)
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

	std::wstring parent;
	XmlNodeType nodeType = XmlNodeType_None;
	while (pReader->Read(&nodeType) == S_OK)
	{
		if (nodeType == XmlNodeType_EndElement)
		{
			LPCWSTR pszLocalName = nullptr;
			pReader->GetLocalName(&pszLocalName, NULL);
			std::wstring local_name(pszLocalName);

			if (local_name == L"group")
			{
				parent = L"";
			}
		}
		else if (nodeType == XmlNodeType_Element)
		{
			LPCWSTR pszLocalName = nullptr;
			pReader->GetLocalName(&pszLocalName, NULL);
			std::wstring local_name(pszLocalName);

			if (local_name == L"group")
			{
				if (pReader->MoveToAttributeByName(L"tag", NULL) == S_OK)
				{
					LPCWSTR pszValue = nullptr;
					pReader->GetValue(&pszValue, NULL);
					if (pszValue) {
						parent = pszValue;
					}
				}
			}
			else if (local_name == L"elem")
			{
				std::wstring key;
				std::wstring name;

				HRESULT result = pReader->MoveToAttributeByName(L"tag", NULL);
				if (result == S_OK)
				{
					LPCWSTR pszValue = nullptr;
					pReader->GetValue(&pszValue, NULL);
					if (pszValue) {
						key = parent + pszValue;
					}
				}

				result = pReader->MoveToAttributeByName(lang, NULL);
				if (result == S_OK)
				{
					LPCWSTR pszValue = nullptr;
					pReader->GetValue(&pszValue, NULL);
					if (pszValue) {
						name = pszValue;
					}
				}
				else if (wcscmp(lang, L"enu") != 0)
				{
					result = pReader->MoveToAttributeByName(L"enu", NULL);
					if (result == S_OK)
					{
						LPCWSTR pszValue = nullptr;
						pReader->GetValue(&pszValue, NULL);
						if (pszValue) {
							name = pszValue;
						}
					}
				}

				// 上書き（辞書に二重登録されていた場合は後から読み込まれたものが有効） 
				(*g_tif_dic)[key] = name;
			}
		}
	}
}


void clear_tif_dictionary(void)
{
	if (g_tif_dic)
	{
		g_tif_dic->clear();
		delete g_tif_dic;
		g_tif_dic = nullptr;
	}
}


void init_tif_dictionary(_In_opt_ const wchar_t* lang)
{
	// 指定されていなければ英語 
	if (lang == nullptr) {
		lang = L"enu";
	}

	// 空の辞書を用意  
	clear_tif_dictionary();
	g_tif_dic = new std::unordered_map<std::wstring, std::wstring>;

	// 辞書のパスを生成 
	TCHAR szFilePath[_MAX_PATH] = {};
	extern HMODULE g_hModule;
	::GetModuleFileName(g_hModule, szFilePath, _countof(szFilePath));
	::PathRemoveFileSpec(szFilePath);
	::PathAppend(szFilePath, _T("tif_standard.xml"));

	// 辞書を読み込む 
	load_tif_dictionary(szFilePath, lang);
}
