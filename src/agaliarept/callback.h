#pragma once

#include <sstream>

#include "../inc/agaliarept.h"

// テキスト出力の純粋仮想クラス 
class callback_cls
{
public:
	virtual int insert_column(int width, wchar_t* name) = 0;

	virtual void next(void) = 0;
	virtual void print(int subItem, const char* sz) = 0;
	virtual void print(int subItem, wchar_t* sz) = 0;
	virtual void print(int subItem, const std::string& str) = 0;
	virtual void print(int subItem, const std::wstring& str) = 0;
	virtual void set_extra_data(uint64_t offset, uint64_t size) = 0;
	virtual void set_right_align(int col, bool enable) = 0;
	virtual void set_codepage(int codepage) = 0;
};

// NULL出力（デバッグ用） 
class callback_obj_null : public callback_cls
{
public:
	virtual int insert_column(int /*width*/, wchar_t* /*name*/) { return -1; }

	virtual void next(void) {}
	virtual void print(int /*subItem*/, const char* /*sz*/) {}
	virtual void print(int /*subItem*/, wchar_t* /*sz*/) {}
	virtual void print(int /*subItem*/, const std::string& /*str*/) {}
	virtual void print(int /*subItem*/, const std::wstring& /*str*/) {}
	virtual void set_extra_data(uint64_t /*offset*/, uint64_t /*size*/) {}
	virtual void set_right_align(int /*col*/, bool /*enable*/) {}
	virtual void set_codepage(int /*codepage*/) {}
};

// リストビューコントロール出力 
class callback_obj_ListCtrl : public callback_cls
{
public:
	callback_obj_ListCtrl(HWND hwnd, HANDLE abort)
	{
		hwnd_listctrl = hwnd;
		handle_abort = abort;
		cur_index = -1;
		new_index = -1;
		subitem = 0;
	}

	virtual int insert_column(int width, wchar_t* name);

	virtual void next(void)
	{
		new_index++;
	}

	virtual void print(int subItem, const char* sz);
	virtual void print(int subItem, wchar_t* sz);

	virtual void print(int subItem, const std::string& str)
	{
		print(subItem, str.c_str());
	}

	virtual void print(int subItem, const std::wstring& str)
	{
		wchar_t* p = _wcsdup(str.c_str());
		print(subItem, p);
		free(p);
	}

	virtual void set_extra_data(uint64_t offset, uint64_t size);

	virtual void set_right_align(int col, bool enable);

	virtual void set_codepage(int codepage)
	{
		UNREFERENCED_PARAMETER(codepage);
	}

protected:
	HWND hwnd_listctrl;
	HANDLE handle_abort;
	int cur_index;
	int new_index;
	int subitem;
};

// ストリーム出力 
class callback_obj_stream : public callback_cls
{
public:
	callback_obj_stream(FILE* fp)
	{
		_stream = fp;
		column = 0;
	}

	callback_obj_stream(FILE* fp, const wchar_t* delimit)
	{
		_stream = fp;
		_delimit = delimit;
		column = 0;
	}

	virtual int insert_column(int width, wchar_t* name)
	{
		UNREFERENCED_PARAMETER(width);
		if (column != 0) {
			fputws(_delimit.c_str(), _stream);
		}
		fwprintf_s(_stream, L"%s", name);
		column++;
		return -1;
	}

	virtual void next(void)
	{
		fputws(L"\n", _stream);
		column = 0;
	}

	virtual void print(int subItem, const char* sz);

	virtual void print(int subItem, wchar_t* sz)
	{
		UNREFERENCED_PARAMETER(subItem);
		if (column != 0) {
			fputws(_delimit.c_str(), _stream);
		}
		fwprintf_s(_stream, L"%s", sz);
		column++;
	}

	virtual void print(int subItem, const std::string& str)
	{
		print(subItem, str.c_str());
	}

	virtual void print(int subItem, const std::wstring& str)
	{
		wchar_t* p = _wcsdup(str.c_str());
		print(subItem, p);
		free(p);
	}

	virtual void set_extra_data(uint64_t offset, uint64_t size)
	{
		UNREFERENCED_PARAMETER(offset);
		UNREFERENCED_PARAMETER(size);
	}

	virtual void set_right_align(int col, bool enable)
	{
		UNREFERENCED_PARAMETER(col);
		UNREFERENCED_PARAMETER(enable);
	}

	virtual void set_codepage(int codepage)
	{
		char buf[64] = {};
		switch (codepage)
		{
		case 1252:
			sprintf_s(buf, ".%d", codepage);
			break;
		default:
			sprintf_s(buf, ".%d", 932);
		}
		setlocale(LC_ALL, buf);
	}


protected:
	FILE* _stream;
	std::wstring _delimit;
	int column;
};

template<class T>
void print_value(const wchar_t* label, T value, int subItem, callback_cls* callback, bool enable_next = false)
{
	std::wstringbuf buf;
	std::wostream str(&buf, true);
	str << label << L" : " << value;
	callback->print(subItem, buf.str());
	if (enable_next) callback->next();
}

template<class T>
void print_value_hex(const wchar_t* label, T value, int subItem, callback_cls* callback, bool enable_next = false)
{
	std::wstringbuf buf;
	std::wostream str(&buf, true);
	str.setf(std::ios::hex, std::ios::basefield);
	str << label << L" : 0x" << value;
	callback->print(subItem, buf.str());
	if (enable_next) callback->next();
}
