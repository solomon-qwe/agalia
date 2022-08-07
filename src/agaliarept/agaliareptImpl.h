#pragma once

#include <intsafe.h>
#include <atlbase.h>
#include <sstream>
#include <iomanip>

#include "../inc/agaliarept.h"
#include "agaliaImageBase.h"
using namespace agalia;

class _agaliaHeapImpl : public agaliaHeap
{
public:
	_agaliaHeapImpl();
	virtual ~_agaliaHeapImpl();
	virtual void Release(void) override;
	virtual HRESULT Free(void) override;
	virtual HRESULT AllocateBytes(rsize_t size, bool init = false) override;
	virtual rsize_t GetSize(void) const override;
	virtual void* GetData(void) const override;
protected:
	rsize_t _size = 0;
	void* _p = nullptr;
};



class _agaliaItemBase : public agaliaItem
{
public:
	_agaliaItemBase(const GUID& guid, uint64_t offset, uint64_t size);
	virtual ~_agaliaItemBase();

	virtual void Release(void) override;

	virtual const GUID& getGUID(void) const override { return guid; }
	virtual uint64_t getOffset(void) const override { return item_data_offset; }
	virtual uint64_t getSize(void) const override { return item_data_size; }

	virtual HRESULT getValueAreaOffset(uint64_t* offset) const override { *offset = getOffset(); return S_OK; }
	virtual HRESULT getValueAreaSize(uint64_t* size) const override { *size = getSize(); return S_OK; }

protected:
	GUID guid = {};
	uint64_t item_data_offset = 0;
	uint64_t item_data_size = 0;
};





inline LARGE_INTEGER uint64_to_li(uint64_t u)
{
	LARGE_INTEGER li;
	li.QuadPart = u;
	return li;
}

inline LARGE_INTEGER int64_to_li(int64_t i)
{
	LARGE_INTEGER li;
	li.QuadPart = i;
	return li;
}

HRESULT multibyte_to_widechar(char* srcBuf, int size, uint32_t codepage, agaliaString** dst);
HRESULT multibyte_to_widechar(char* srcBuf, int size, uint32_t codepage, std::wstringstream& dst);




// 値を16進数の文字列に変換し、型のサイズに合わせてゼロパディングする 
template <typename T>
void format_hex(std::wstringstream& dst, T value, rsize_t def_w = 0)
{
	dst << std::hex << std::uppercase << std::setw(def_w ? def_w : (sizeof(T) * 2)) << std::setfill(L'0') << value;
}


// 値を10進数の文字列に変換 
template <typename T>
void format_dec(std::wstringstream& dst, T value)
{
	dst << std::dec << value;
}


// 値を浮動小数点数の文字列に変換 
template <typename T>
void format_fixed(std::wstringstream& dst, T value)
{
	dst << std::fixed << value;
}


// 値を浮動小数点数の指数表記文字列に変換 
template <typename T>
void format_scientific(std::wstringstream& dst, T value)
{
	dst << std::scientific << std::setprecision(sizeof(T) * 2) << value;
}

template <typename T>
void format_asc(std::wstringstream& dst, T value)
{
	agaliaStringPtr s;
	auto hr = multibyte_to_widechar(reinterpret_cast<char*>(&value), sizeof(T), CP_US_ASCII, &s);
	if (SUCCEEDED(hr))
	{
		dst << s->GetData();
	}
}

inline void format_time32(std::wstringstream& dst, __time32_t value)
{
	wchar_t buf[64] = {};
	if (_wctime32_s(buf, &value) == 0) {
		dst << buf;
	}
}

inline void format_time64(std::wstringstream& dst, __time64_t value)
{
	wchar_t buf[64] = {};
	if (_wctime64_s(buf, &value) == 0) {
		dst << buf;
	}
}



inline void fast_itow16(int v, wchar_t* p)
{
	static const wchar_t s[] = L"0123456789ABCDEF";
	p[0] = s[(v >> 4) & 0xF];
	p[1] = s[(v >> 0) & 0xF];
	p[2] = '\0';
}



extern int agalia_pref_dic_lang;
extern int agalia_pref_dicom_vr;
