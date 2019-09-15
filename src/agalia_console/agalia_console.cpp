// agalia_console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <io.h>	// for _setmode 
#include <fcntl.h>	// for _O_U8TEXT 
#include <iostream>	// for wstring 

#include <Shlwapi.h>	// for Path~
#pragma comment(lib, "Shlwapi.lib")

#include "../inc/agaliarept.h"

/// <summary>
/// コマンドライン解析に失敗した際のエラー表示</summary>
void usage(void)
{
	std::cout << "usage:" << std::endl;
	std::cout << "agalia filepath [switched]" << std::endl;
	std::cout << std::endl;
	std::cout << "/out:" << std::endl;
	std::cout << "/offset:" << std::endl;
	std::cout << "/size:" << std::endl;
}

/// <summary>
/// エラーコードに応じたメッセージを表示</summary>
void err_msg(HRESULT hr)
{
	void* buf = nullptr;
	::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPTSTR>(&buf),
		0,
		NULL);

	if (buf)
	{
		setlocale(LC_ALL, ".65001");
		std::wcerr << reinterpret_cast<wchar_t*>(buf);
		::LocalFree(buf);
	}
}


void parse_stub(FILE* stream, const agaliaContainer* image, agaliaItem* parent, uint32_t column)
{
	HRESULT hr;

	agaliaStringPtr temp;
	agaliaPtr<agaliaItem> item(parent);

	do
	{
		// 現在のアイテムの情報を出力 
		uint32_t row = 0;
		image->getGridRowCount(item, &row);
		for (uint32_t i = 0; i < row; i++)
		{
			hr = image->getGridValue(item, i, 0, &temp);
			if (FAILED(hr))
				break;
			fputws(temp->GetData(), stream);

			for (uint32_t j = 1; j < column; j++)
			{
				fputws(L"\t", stream);
				hr = image->getGridValue(item, i, j, &temp);
				if (SUCCEEDED(hr))
				{
					fputws(temp->GetData(), stream);
					temp.detach()->Release();
				}
			}
			fputws(L"\n", stream);
		}

		// 子アイテムがあれば再帰 
		uint32_t sibling = 0;
		do
		{
			agaliaPtr<agaliaItem> child;
			hr = item->getChildItem(sibling++, &child);
			if (SUCCEEDED(hr)) {
				parse_stub(stream, image, child.detach(), column);
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


HRESULT parse(FILE* stream, const wchar_t* path, uint64_t offset, uint64_t size)
{
	HRESULT hr;

	// ファイルを開く 
	agaliaPtr<agaliaContainer> image;
	hr = getAgaliaImage(&image, path, offset, size);
	if (FAILED(hr)) return hr;

	// ヘッダを出力 
	uint32_t count = 0;
	hr = image->getColumnCount(&count);
	if (FAILED(hr)) return hr;
	for (uint32_t i = 0; i < count; i++)
	{
		agaliaStringPtr temp;
		hr = image->getColumnName(i, &temp);
		if (SUCCEEDED(hr))
		{
			if (i != 0) fputws(L"\t", stream);
			fputws(temp->GetData(), stream);
		}
	}
	fputws(L"\n", stream);

	// ルートアイテムを取得 
	agaliaPtr<agaliaItem> root;
	hr = image->getRootItem(&root);
	if (FAILED(hr)) return hr;

	// 再帰的に解析・出力 
	parse_stub(stream, image, root.detach(), count);

	return S_OK;
}


/// <summary>
/// 出力用ストリームを開く</summary>
HRESULT open_stream(agaliaCmdLineParam* param, FILE** stream)
{
	if (param->getOutputFilePath() == nullptr || *param->getOutputFilePath() == 0)
	{
		// 出力先ファイルパスが指定されていなければ、標準出力をUTF-8に設定して出力先とする 
		*stream = stdout;
		int result = _setmode(_fileno(stdout), _O_U8TEXT);
		if (result == -1) {
			return HRESULT_FROM_WIN32(::GetLastError());
		}
	}
	else
	{
		// 出力先ファイルパスが指定されていれば、ファイルをUTF-8でオープンする
		errno_t err = _wfopen_s(stream, param->getOutputFilePath(), L"wt,ccs=UTF-8");
		if (err != 0) {
			return HRESULT_FROM_WIN32(::GetLastError());
		}
	}

	return S_OK;
}


/// <summary>
/// 出力用ストリームを閉じる</summary>
void close_stream(FILE* stream)
{
	if (stream != stdout && stream != nullptr) {
		fclose(stream);
	}
}


/// <summary>
/// コンソール版を起動</summary>
HRESULT run_console(agaliaCmdLineParam* param)
{
	FILE* stream = nullptr;
	auto hr = open_stream(param, &stream);
	if (FAILED(hr)) return hr;

	hr = parse(stream, param->getTargetFilePath(), param->getOffset(), param->getSize());

	close_stream(stream);
	return hr;
}


/// <summary>
/// GUI版を起動</summary>
HRESULT run_gui(void)
{
	TCHAR szFilePath[_MAX_PATH] = {};
	::GetModuleFileName(NULL, szFilePath, _countof(szFilePath));
	::PathRemoveFileSpec(szFilePath);
	::PathAppend(szFilePath, L"agalia.exe");

	PROCESS_INFORMATION pi = {};
	STARTUPINFO si = {};
	si.cb = sizeof(si);

	BOOL succeeded = ::CreateProcess(NULL, szFilePath, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if (!succeeded)
	{
		return HRESULT_FROM_WIN32(::GetLastError());
	}
	else
	{
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
	}

	return S_OK;
}


int main()
{
	// コマンドライン解析 
	agaliaPtr< agaliaCmdLineParam> param;
	HRESULT hr = agaliaCmdLineParam::parseCmdLine(&param);

	// 処理実行 
	if (hr == S_FALSE)
	{
		hr = run_gui();
	}
	else if (hr == S_OK)
	{
		hr = run_console(param);
	}

	// 異常があった場合はエラー出力 
	if (hr == E_INVALIDARG)
	{
		usage();
	}
	else if (FAILED(hr))
	{
		err_msg(hr);
	}

	return 0;
}
