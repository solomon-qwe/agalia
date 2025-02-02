#include "pch.h"
#include "CppUnitTest.h"

#include <Windows.h>
#include "../inc/agaliarept.h"
#include "util.h"

#include <io.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	const int max_parse_line = 20 * 1024;

	void asset_test_main(const wchar_t* file_name);

	TEST_CLASS(Asset)
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

		void main(const wchar_t* file_name)
		{
			asset_test_main(file_name);
		}


		TEST_METHOD(Apple)
		{
			main(L"Apple.txt");
		}

		TEST_METHOD(BigTIFF)
		{
			main(L"BigTIFF.txt");
		}

		TEST_METHOD(ITU_T_024)
		{
			main(L"ITU-T.024.txt");
		}

		TEST_METHOD(ITU_T_083)
		{
			main(L"ITU-T.083.txt");
		}

		TEST_METHOD(JIRA_2011)
		{
			main(L"JIRA_2011.txt");
		}

		TEST_METHOD(JIRA_2012)
		{
			main(L"JIRA_2012.txt");
		}

		TEST_METHOD(nema97cd)
		{
			main(L"nema97cd.txt");
		}

		TEST_METHOD(nema_wg12_acuson)
		{
			main(L"nema_wg12_acuson.txt");
		}

		TEST_METHOD(nema_wg12_ALI)
		{
			main(L"nema_wg12_ALI.txt");
		}

		TEST_METHOD(nema_wg12_GE)
		{
			main(L"nema_wg12_GE.txt");
		}

		TEST_METHOD(nema_wg12_HP)
		{
			main(L"nema_wg12_HP.txt");
		}

		TEST_METHOD(nema_wg12_philips)
		{
			main(L"nema_wg12_philips.txt");
		}

		TEST_METHOD(nema_wg12_supersonic)
		{
			main(L"nema_wg12_supersonic.txt");
		}

		TEST_METHOD(osirix_01)
		{
			main(L"osirix_01.txt");
		}

		TEST_METHOD(osirix_02)
		{
			main(L"osirix_02.txt");
		}

		TEST_METHOD(osirix_03)
		{
			main(L"osirix_03.txt");
		}

		TEST_METHOD(osirix_04)
		{
			main(L"osirix_04.txt");
		}

		TEST_METHOD(osirix_05)
		{
			main(L"osirix_05.txt");
		}

		TEST_METHOD(osirix_06)
		{
			main(L"osirix_06.txt");
		}

		TEST_METHOD(osirix_07)
		{
			main(L"osirix_07.txt");
		}

		TEST_METHOD(osirix_08)
		{
			main(L"osirix_08.txt");
		}

		TEST_METHOD(osirix_09)
		{
			main(L"osirix_09.txt");
		}

		TEST_METHOD(osirix_10)
		{
			main(L"osirix_10.txt");
		}
	};



	int parse_to_nul_stub(const agaliaContainer* image, agaliaElement* parent, uint32_t column)
	{
		int ret = 0;

		HRESULT hr;
		std::wstring err_str;

		agaliaStringPtr temp;
		agaliaPtr<agaliaElement> item(parent);

		do
		{
			if (max_parse_line < ++ret)
				return ret;

			// 現在のアイテムの情報をログ出力 
			uint32_t row = 0;
			image->getElementInfoCount(item, &row);
			for (uint32_t i = 0; i < row; i++)
			{
				hr = image->getElementInfoValue(item, i, 0, &temp);
				if (FAILED(hr))
					break;

				for (uint32_t j = 1; j < column; j++)
				{
					hr = image->getElementInfoValue(item, i, j, &temp);
				}
			}

			// 子アイテムがあれば再帰 
			uint32_t sibling = 0;
			do
			{
				agaliaPtr<agaliaElement> child;
				hr = item->getChild(sibling++, &child);
				if (SUCCEEDED(hr)) {
					ret += parse_to_nul_stub(image, child.detach(), column);
				}
			} while (SUCCEEDED(hr));

			// 次のアイテムへ遷移 
			agaliaPtr<agaliaElement> next;
			hr = item->getNext(&next);
			if (SUCCEEDED(hr)) {
				item = next.detach();
			}

		} while (SUCCEEDED(hr));

		return ret;
	}


	void parse_to_nul(const wchar_t* path)
	{
		HRESULT hr;
		std::wstring err_str;

		// ファイルを開く 
		agaliaPtr<agaliaContainer> image;
		hr = getAgaliaImage(&image, path, 0, 0);
		Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));

		// カラム数を取得 
		uint32_t column = 0;
		hr = image->getColumnCount(&column);
		Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));

		// ルートアイテムを取得 
		agaliaPtr<agaliaElement> root;
		hr = image->getRootElement(&root);
		Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));

		// 再帰的に解析 
		int ret = parse_to_nul_stub(image, root.detach(), column);
		if (max_parse_line < ret) {
			Logger::WriteMessage(L"Too many items.\n");
		}
	}


	void asset_test_main(const wchar_t* file_name)
	{
		wchar_t path_temp[_MAX_PATH];
		FILE* fp = nullptr;

		std::wstring target_dir;
		get_asset_dir(&target_dir);

		std::wstring index_file_path;
		index_file_path = script_dir;
		index_file_path += file_name;

		_wfopen_s(&fp, index_file_path.c_str(), L"rt,ccs=UTF-8");
		if (fp)
		{
			while (fgetws(path_temp, _countof(path_temp), fp))
			{
				if (path_temp[0] == L';') {
					continue;
				}
				wchar_t* p = wcschr(path_temp, L'\n');
				if (p) {
					*p = L'\0';
				}
				if (wcslen(path_temp) != 0)
				{
					std::wstring target_file_path;
					target_file_path = target_dir;
					target_file_path += path_temp;

					Logger::WriteMessage(target_file_path.c_str());
					Logger::WriteMessage(L"\n");
					parse_to_nul(target_file_path.c_str());
				}
			}

			fclose(fp);
			fp = nullptr;
		}
	}
}