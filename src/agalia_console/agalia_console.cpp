// agalia_console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../inc/agaliarept.h"
#include "../inc/agalia_version.h"

#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include <iostream>

void usage(agalia::config* config)
{
	using namespace std;
	cout << AGALIA_ABOUT_STRING << endl;
	cout << AGALIA_COPYRIGHT_STRING << endl;
	cout << endl;
	config->usage();
}


int run_gui(void)
{
	int ret = 0;

	TCHAR szFilePath[_MAX_PATH] = {};
	::GetModuleFileName(NULL, szFilePath, _countof(szFilePath));
	::PathRemoveFileSpec(szFilePath);
	::PathAppend(szFilePath, _T("agalia.exe"));

	PROCESS_INFORMATION pi = {};
	STARTUPINFO si = {};
	si.cb = sizeof(si);

	BOOL succeeded = ::CreateProcess(NULL, szFilePath, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if (!succeeded)
	{
		ret = -1;
	}
	else
	{
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
	}

	return ret;
}


int run_console(void)
{
	using namespace agalia;

	int ret = -1;

	config_ptr config;
	::AgaliaGetConfig(&config);

	HRESULT result = config->parse_command_line();
	if (FAILED(result))
	{
		usage(config);
		return ret;
	}

	config::out_stream mode = config->get_output_file_path() ? config::file : config::console;
	result = config->open_out_stream(mode);
	if (FAILED(result)) {
		return ret;
	}

	result = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (SUCCEEDED(result))
	{
		result = AgaliaMain(config);
		if (SUCCEEDED(result))
		{
			ret = 0;
		}
		::CoUninitialize();
	}

	config->close_out_stream();

	return ret;
}


int wmain(int argc)
{
	int ret = 0;

	// ˆø”‚ªw’è‚³‚ê‚Ä‚¢‚È‚¢‚Æ‚«‚ÍAGUI”Å‚ğ‹N“®‚·‚é 
	if (argc == 1) {
		ret = run_gui();
	}
	else {
		ret = run_console();
	}

	return ret;
}
