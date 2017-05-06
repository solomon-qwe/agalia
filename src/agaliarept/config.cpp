#include "stdafx.h"
#include "../inc/agaliarept.h"

#include <shellapi.h>	// for CommandLineToArgvW() 
#include <iostream>		// std::ostream 

agalia::config* agalia::config::create_instance(void)
{
	return new agalia::config;
}


agalia::config::~config()
{
	if (input_file_path) {
		free(input_file_path);
	}

	if (output_file_path) {
		free(output_file_path);
	}
}


void agalia::config::release(void)
{
	delete this;
}


void agalia::config::set_file_path(wchar_t** pp, const wchar_t* path)
{
	if (*pp)
	{
		wchar_t* p = *pp;
		*pp = nullptr;
		free(p);
	}
	*pp = _wcsdup(path);
}


void agalia::config::set_output_file_path(const wchar_t* path)
{
	set_file_path(&output_file_path, path);
}


void agalia::config::set_input_file_path(const wchar_t* path)
{
	set_file_path(&input_file_path, path);
}


HRESULT agalia::config::open_out_stream(int mode)
{
	switch (mode)
	{
	case agalia::config::console:
		this->stream = stdout;
		return S_OK;

	case agalia::config::file:
		errno_t err = _wfopen_s(&this->stream, this->get_output_file_path(), L"wt,ccs=UTF-8");
		return (err == 0) ? S_OK : E_FAIL;
	}
	return E_INVALIDARG;
}


void agalia::config::close_out_stream(void)
{
	if (this->stream == nullptr) {
		return;
	}

	if (this->stream == stdout) {
		this->stream = nullptr;
	}
	else {
		FILE* fp = this->stream;
		this->stream = nullptr;
		fclose(fp);
	}
}


void agalia::config::usage(void)
{
	using namespace std;
	cout << "usage:" << endl;
	cout << "agalia filepath [switches]" << endl;
	cout << endl;
	cout << "/out:" << endl;
	cout << "/offset:" << endl;
	cout << "/size:" << endl;
	cout << "/byte_stream_limit_length:" << endl;
	cout << "/force_dictionary_vr" << endl;
}

HRESULT agalia::config::parse_command_line(void)
{
	int nNumArg = 0;
	LPWSTR* ppsz = ::CommandLineToArgvW(::GetCommandLine(), &nNumArg);

	if (nNumArg == 1) {
		return E_FAIL;
	}

	HRESULT result = E_INVALIDARG;

	for (int i = 1; i < nNumArg; i++)
	{
		// 解析対象ファイル 
		if (ppsz[i][0] != L'/') {
			set_input_file_path(ppsz[i]);
			result = S_OK;
			continue;
		}

		// バイト列を出力する際の最大数 
		const wchar_t* pParam = L"/byte_stream_limit_length:";
		if (_wcsnicmp(ppsz[i], pParam, wcslen(pParam)) == 0)
		{
			this->byte_stream_limit_length = _wtoi(ppsz[i] + wcslen(pParam));
			continue;
		}

		// 入力データが DICOM の Implicit VR であっても、dictionary で指定された VR を優先する 
		pParam = L"/force_dictionary_vr";
		if (_wcsnicmp(ppsz[i], pParam, wcslen(pParam)) == 0)
		{
			this->force_dictionary_vr = true;
			continue;
		}

		// 入力データの読出し開始オフセット 
		pParam = L"/offset:";
		if (_wcsnicmp(ppsz[i], pParam, wcslen(pParam)) == 0)
		{
			this->offset = _wcstoui64(ppsz[i] + wcslen(pParam), nullptr, 10);
			continue;
		}

		// 入力データの読出し最大サイズ 
		pParam = L"/size:";
		if (_wcsnicmp(ppsz[i], pParam, wcslen(pParam)) == 0)
		{
			this->data_size = _wcstoui64(ppsz[i] + wcslen(pParam), nullptr, 10);
			continue;
		}

		// 出力先ファイル 
		pParam = L"/out:";
		if (_wcsnicmp(ppsz[i], pParam, wcslen(pParam)) == 0)
		{
			set_output_file_path(ppsz[i] + wcslen(pParam));
			continue;
		}

		// 未知の引数は無視 
	}

	return result;
}


bool agalia::config::is_abort(agalia::config::abort_flag flag) const
{
	if (abort_event)
	{
		if (::WaitForSingleObject(abort_event, 0) != WAIT_TIMEOUT)
		{
			if (flag == agalia::config::throw_on_abort) {
				throw exception(E_ABORT);
			}
			else {
				return true;
			}
		}
	}
	return false;
}


extern "C" AGALIAREPT_API
HRESULT AgaliaGetConfig(agalia::config** config)
{
	if (config == nullptr) return E_POINTER;
	agalia::config* p = agalia::config::create_instance();
	*config = p;
	return S_OK;
}
