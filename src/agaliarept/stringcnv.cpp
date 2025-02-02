#include "pch.h"
#include "agaliareptImpl.h"
#include "stringcnv.h"


static HRESULT multibyte_to_widechar_internal(char* srcBuf, int size, uint32_t codepage, CHeapPtr<wchar_t>& dst)
{
	// 韓国語の場合は自前でエスケープを除去する 
	std::string kor;
	if (codepage == 949) {
		char esc[] = { 0x1B, 0x24, 0x29, 0x43, 0x00 };
		kor = srcBuf;
		size_t pos;
		while ((pos = kor.find_first_of(esc)) != std::string::npos) {
			kor.erase(pos, 4);
		}
		srcBuf = const_cast<char*>(kor.c_str());
		size = static_cast<int>(kor.size());
	}

	if (size == 0)
		return S_FALSE;

	int wleng = ::MultiByteToWideChar(codepage, 0, srcBuf, size, nullptr, 0);
	size_t bufsize = 0;
	auto hr = IntToSizeT(wleng + 1, &bufsize);
	if (FAILED(hr)) return hr;

	if (!dst.Allocate(bufsize)) return E_FAIL;
	dst[wleng] = L'\0';

	int ret = ::MultiByteToWideChar(codepage, 0, srcBuf, size, dst, wleng);
	if (ret == 0) return HRESULT_FROM_WIN32(::GetLastError());

	return S_OK;
}



HRESULT multibyte_to_widechar(char* srcBuf, int size, uint32_t codepage, agaliaString** dst)
{
	CHeapPtr<wchar_t> wbuf;
	auto hr = multibyte_to_widechar_internal(srcBuf, size, codepage, wbuf);
	if (hr != S_OK) return hr;
	*dst = agaliaString::create(wbuf);
	return S_OK;
}



HRESULT multibyte_to_widechar(char* srcBuf, int size, uint32_t codepage, std::wstringstream& dst)
{
	CHeapPtr<wchar_t> wbuf;
	auto hr = multibyte_to_widechar_internal(srcBuf, size, codepage, wbuf);
	if (hr != S_OK) return hr;
	dst << wbuf.m_pData;
	return S_OK;
}
