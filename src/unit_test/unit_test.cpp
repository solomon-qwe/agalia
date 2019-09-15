#include "pch.h"
#include "CppUnitTest.h"

#include <Windows.h>
#include "../inc/agaliarept.h"
#include "util.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	const wchar_t* script_dir = L"..\\..\\TestCase\\";


	TEST_CLASS(unittest)
	{
	public:
		err_mgr em;

		TEST_METHOD_INITIALIZE(MethodInitialize)
		{
			em.crt_memchk_init();
		}

		TEST_METHOD_CLEANUP(MethodCleanup)
		{
			em.crt_memchk();
		}

		TEST_METHOD(Open_Abnormal)
		{
			std::wstring err_str;
			agaliaPtr<agaliaContainer> image;
			HRESULT hr;

			hr = getAgaliaImage(nullptr, L"", 0, 0);
			Assert::AreEqual(E_POINTER, hr, GetErrorMessage(hr, err_str));

			hr = getAgaliaImage(&image, nullptr, 0, 0);
			Assert::AreEqual(E_POINTER, hr, GetErrorMessage(hr, err_str));

			hr = getAgaliaImage(&image, L"", 0, 0);
			Assert::AreEqual(HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND), hr, GetErrorMessage(hr, err_str));

			hr = getAgaliaImage(&image, L"\\\\\\\\\\\\\\\\", 0, 0);
			Assert::AreEqual(HRESULT_FROM_WIN32(ERROR_BAD_PATHNAME), hr, GetErrorMessage(hr, err_str));
		}
	};



	const wchar_t* GetErrorMessage(DWORD err, std::wstring& str)
	{
		str.clear();

		LPTSTR lpMsgBuf = nullptr;
		::FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)& lpMsgBuf,
			0,
			NULL
		);

		if (lpMsgBuf)
		{
			str = lpMsgBuf;
			::LocalFree(lpMsgBuf);
			lpMsgBuf = nullptr;
		}

		return str.c_str();
	}



	void parse_to_log_stub(const agaliaContainer* image, agaliaItem* parent, uint32_t column)
	{
		HRESULT hr;
		std::wstring err_str;

		agaliaStringPtr temp;
		agaliaPtr<agaliaItem> item(parent);

		do
		{
			// 現在のアイテムの情報をログ出力 
			uint32_t row = 0;
			image->getGridRowCount(item, &row);
			for (uint32_t i = 0; i < row; i++)
			{
				hr = image->getGridValue(item, i, 0, &temp);
				if (FAILED(hr))
					break;
				Logger::WriteMessage(temp);

				for (uint32_t j = 1; j < column; j++)
				{
					Logger::WriteMessage(L"\t");
					hr = image->getGridValue(item, i, j, &temp);
					if (SUCCEEDED(hr))
					{
						Logger::WriteMessage(temp);
						temp.detach()->Release();
					}
				}
				Logger::WriteMessage(L"\n");
			}

			// 子アイテムがあれば再帰 
			uint32_t sibling = 0;
			do
			{
				agaliaPtr<agaliaItem> child;
				hr = item->getChildItem(sibling++, &child);
				if (SUCCEEDED(hr)) {
					parse_to_log_stub(image, child.detach(), column);
				}
			} while (SUCCEEDED(hr));

			// 次のアイテムへ遷移 
			agaliaPtr<agaliaItem> next;
			hr = item->getNextItem(&next);
			if (SUCCEEDED(hr)) {
				item = next.detach();
			}

		} while (SUCCEEDED(hr));
	}



	void parse_to_log(const wchar_t* path)
	{
		HRESULT hr;
		std::wstring err_str;

		// ファイルを開く 
		agaliaPtr<agaliaContainer> image;
		hr = getAgaliaImage(&image, path, 0, 0);
		Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));

		// ヘッダをログに出力 
		uint32_t column = 0;
		hr = image->getColumnCount(&column);
		Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
		for (uint32_t i = 0; i < column; i++)
		{
			agaliaStringPtr temp;
			hr = image->getColumnName(i, &temp);
			if (SUCCEEDED(hr))
			{
				if (i != 0) Logger::WriteMessage(L"\t");
				Logger::WriteMessage(temp);
			}
		}
		Logger::WriteMessage(L"\n");

		// ルートアイテムを取得 
		agaliaPtr<agaliaItem> root;
		hr = image->getRootItem(&root);
		Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));

		// 再帰的に解析・出力 
		parse_to_log_stub(image, root.detach(), column);
	}



	void get_asset_dir(std::wstring* target_dir)
	{
		std::wstring target_desc_file;
		target_desc_file = script_dir;
		target_desc_file += L"_asset_data_dir.dat";

		wchar_t path_temp[_MAX_PATH];
		FILE* fp = nullptr;
		_wfopen_s(&fp, target_desc_file.c_str(), L"rt,ccs=UTF-8");
		if (fp)
		{
			if (fgetws(path_temp, _countof(path_temp), fp)) {
				wchar_t* p = wcschr(path_temp, L'\n');
				if (p) {
					*p = L'\0';
				}
				if (wcslen(path_temp) != 0) {
					*target_dir = path_temp;
				}
			}
			fclose(fp);
			fp = nullptr;
		}
	}
}
