#include "stdafx.h"
#include "CppUnitTest.h"

#include "../inc/agaliarept.h"

#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include <codecvt>
#include <iostream>
#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


#define ASSET_FOLDER_PATH L"..\\..\\TestCase\\TestData\\"

const wchar_t* GetErrorMessage(DWORD err, std::wstring& str)
{
	str.clear();

	switch (err)
	{
	case AGALIA_ERR_UNSUPPORTED:
		str = L"AGALIA_ERR_UNSUPPORTED";
		break;

	case AGALIA_ERR_PATH_NOT_FOUND:
		str = L"AGALIA_ERR_PATH_NOT_FOUND";
		break;

	default:
		if (HRESULT_FACILITY(err) == FACILITY_WIN32)
		{
			LPTSTR lpMsgBuf = nullptr;
			::FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				err,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Šù’è‚ÌŒ¾Œê
				(LPTSTR)&lpMsgBuf,
				0,
				NULL
			);

			if (lpMsgBuf)
			{
				str = lpMsgBuf;
				::LocalFree(lpMsgBuf);
				lpMsgBuf = nullptr;
			}
		}
	}

	return str.c_str();
}

namespace unit_test
{
	TEST_CLASS(UnitTest1)
	{
	public:

		void stub(agalia::config* config, HRESULT expected = S_OK)
		{
			HRESULT result = AgaliaMain(config);
			std::wstring str;
			GetErrorMessage(result, str);
			if (config->get_input_file_path())
			{
				str += L" / ";
				str += config->get_input_file_path();
			}
			Assert::AreEqual(expected, result, str.c_str());
		}

		void driver(agalia::config* config, const wchar_t* index_file_name)
		{
			const wchar_t* base_folder_path = L"..\\..\\TestCase\\";

			std::wstring index_file_path;
			index_file_path = base_folder_path;
			index_file_path += index_file_name;

			std::ifstream ifs(index_file_path);
			Assert::AreEqual(false, ifs.fail());

			std::wstring_convert<std::codecvt_utf8<wchar_t, 0x10ffff, std::consume_header>, wchar_t> cv;

			std::string line;
			while (std::getline(ifs, line))
			{
				if (line[0] == ';') {
					continue;
				}
				std::wstring path;
				path = base_folder_path;
				path += cv.from_bytes(line);

				config->set_input_file_path(path.c_str());
				stub(config);
			}
		}

		TEST_METHOD(Open_Null)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);

			stub(config, AGALIA_ERR_PATH_NOT_FOUND);
		}

		TEST_METHOD(Open_Empty)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);

			config->set_input_file_path(L"");
			stub(config, AGALIA_ERR_PATH_NOT_FOUND);
		}

		TEST_METHOD(Open_Exe)
		{
			TCHAR szPath[_MAX_PATH];
			::GetModuleFileName(NULL, szPath, _countof(szPath));

			agalia::config_ptr config;
			::AgaliaGetConfig(&config);

			config->set_input_file_path(szPath);
			stub(config, AGALIA_ERR_UNSUPPORTED);
		}

		TEST_METHOD(OpenApple)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"Apple.txt");
		}

		TEST_METHOD(OpenJPEG_ITUT_T083)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"ITU-T.083.txt");
		}

		TEST_METHOD(OpenTIFF_BigTIFF)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"BigTIFF.txt");
		}

		TEST_METHOD(OpenTIFF_ITUT_T024)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"ITU-T.024.txt");
		}

		TEST_METHOD(OpenDICOM_JIRA_2011)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"JIRA_2011.txt");
		};

		TEST_METHOD(OpenDICOM_JIRA_2012)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"JIRA_2012.txt");
		}

		TEST_METHOD(OpenDICOM_NEMA_WG12_Acuson)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"nema_wg12_acuson.txt");
		};

		TEST_METHOD(OpenDICOM_NEMA_WG12_ALI)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"nema_wg12_ALI.txt");
		};

		TEST_METHOD(OpenDICOM_NEMA_WG12_GE_EnhancedVolume_3D)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			config->set_input_file_path(ASSET_FOLDER_PATH L"DICOM\\NEMA\\WG12\\EnhancedVolume\\GE\\3D.dcm");
			stub(config);
		};

		TEST_METHOD(OpenDICOM_NEMA_WG12_GE_EnhancedVolume_3DwithColor)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			config->set_input_file_path(ASSET_FOLDER_PATH L"DICOM\\NEMA\\WG12\\EnhancedVolume\\GE\\3DwithColor.dcm");
			stub(config);
		};

		TEST_METHOD(OpenDICOM_NEMA_WG12_GE_EnhancedVolume_3DwithPDI)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			config->set_input_file_path(ASSET_FOLDER_PATH L"DICOM\\NEMA\\WG12\\EnhancedVolume\\GE\\3DwithPDI.dcm");
			stub(config);
		};

		TEST_METHOD(OpenDICOM_NEMA_WG12_GE_EnhancedVolume_4D)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			config->set_input_file_path(ASSET_FOLDER_PATH L"DICOM\\NEMA\\WG12\\EnhancedVolume\\GE\\4D.dcm");
			stub(config);
		};

		TEST_METHOD(OpenDICOM_NEMA_WG12_GE_Ving)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"nema_wg12_GE.txt");
		};

		TEST_METHOD(OpenDICOM_NEMA_WG12_HP)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"nema_wg12_HP.txt");
		};

		TEST_METHOD(OpenDICOM_NEMA_WG12_Philips)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"nema_wg12_philips.txt");
		};

		TEST_METHOD(OpenDICOM_NEMA_WG12_SuperSonic)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"nema_wg12_supersonic.txt");
		};

		TEST_METHOD(OpenDICOM_NEMA_97CD)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"nema97cd.txt");
		};

		TEST_METHOD(OpenDICOM_OsiriX_01)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"osirix_01.txt");
		}

		TEST_METHOD(OpenDICOM_OsiriX_02)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"osirix_02.txt");
		}

		TEST_METHOD(OpenDICOM_OsiriX_03)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"osirix_03.txt");
		}

		TEST_METHOD(OpenDICOM_OsiriX_04)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"osirix_04.txt");
		}

		TEST_METHOD(OpenDICOM_OsiriX_05)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"osirix_05.txt");
		}

		TEST_METHOD(OpenDICOM_OsiriX_06)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"osirix_06.txt");
		}

		TEST_METHOD(OpenDICOM_OsiriX_07)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"osirix_07.txt");
		}

		TEST_METHOD(OpenDICOM_OsiriX_08)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"osirix_08.txt");
		}

		TEST_METHOD(OpenDICOM_OsiriX_09)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"osirix_09.txt");
		}

		TEST_METHOD(OpenDICOM_OsiriX_10)
		{
			agalia::config_ptr config;
			::AgaliaGetConfig(&config);
			driver(config, L"osirix_10.txt");
		}
	};
}