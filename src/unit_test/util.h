#pragma once

class err_mgr
{
public:
	_CrtMemState s1 = {}, s2 = {}, s3 = {};

	void crt_memchk_init()
	{
		ZeroMemory(&s1, sizeof(s1));
		ZeroMemory(&s2, sizeof(s2));
		ZeroMemory(&s3, sizeof(s3));

		_CrtMemCheckpoint(&s1);
	}

	void crt_memchk()
	{
		_CrtMemCheckpoint(&s2);

		bool failed = false;
		OutputDebugString(L"===== memory leak =====\r\n");
		if (_CrtMemDifference(&s3, &s1, &s2))
		{
			_CrtMemDumpStatistics(&s3);
			failed = true;
		}
		OutputDebugString(L"=======================\r\n");

		if (failed)
		{
			using namespace Microsoft::VisualStudio::CppUnitTestFramework;
			Assert::Fail();
		}
	}
};


namespace unittest
{
	extern const wchar_t* script_dir;

	const wchar_t* GetErrorMessage(DWORD err, std::wstring& str);

	void get_asset_dir(std::wstring* target_dir);
	void parse_to_log(const wchar_t* path);
}
