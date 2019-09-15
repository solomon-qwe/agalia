#include "pch.h"
#include "CppUnitTest.h"

#include <Windows.h>
#include "../inc/agaliarept.h"
#include "util.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(DICOM)
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

		TEST_METHOD(US_JPG_IR87_Parse)
		{
			std::wstring asset_dir;
			get_asset_dir(&asset_dir);

			parse_to_log((asset_dir + L"TestData\\DICOM\\JIRA\\2012\\US_JPG_IR87.dcm").c_str());
		}
	};
}