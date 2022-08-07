#include "pch.h"
#include "analyze_DCM_util.h"

using namespace analyze_DCM;

HRESULT format_dicm_value_ascii_string(std::wstringstream& dst, const container_DCM_Impl* image, uint64_t value_offset, uint32_t value_size)
{
	CHeapPtr<char> buf;
	if (!buf.AllocateBytes(static_cast<size_t>(value_size) + 1)) return E_OUTOFMEMORY;
	buf[value_size] = '\0';
	auto hr = image->ReadData(buf, value_offset, value_size);
	if (FAILED(hr)) return hr;

	dst << L"\"";
	hr = multibyte_to_widechar(buf, value_size, CP_US_ASCII, dst);
	dst << L"\"";

	return hr;
}


HRESULT format_dicm_value_localize_string(std::wstringstream& dst, const container_DCM_Impl* image, uint64_t value_offset, uint32_t value_size, const std::list<int>& codepage)
{
	CHeapPtr<char> buf;
	if (!buf.AllocateBytes(static_cast<size_t>(value_size) + 1)) return E_OUTOFMEMORY;
	buf[value_size] = '\0';
	auto hr = image->ReadData(buf, value_offset, value_size);
	if (FAILED(hr)) return hr;

	// 注：SpecificCharacterSet に複数指定されていた場合、最後の指定を用いて一括変換 
	// 　　本来はエスケープ処理すべき 
	dst << L"\"";
	hr = multibyte_to_widechar(buf, value_size, *codepage.rbegin(), dst);
	dst << L"\"";

	return hr;
}


HRESULT format_dicm_value_uid(std::wstringstream& dst, const container_DCM_Impl* image, uint64_t value_offset, uint32_t value_size)
{
	CHeapPtr<char> buf;
	if (!buf.AllocateBytes(static_cast<size_t>(value_size) + 1)) return E_OUTOFMEMORY;
	buf[value_size] = '\0';
	auto hr = image->ReadData(buf, value_offset, value_size);
	if (FAILED(hr)) return hr;

	dcm_dic_uid_iterator it;
	if (findUID(it, buf.m_pData))
	{
		dst << it->second.name.c_str() << L" (";
		multibyte_to_widechar(buf, value_size, CP_US_ASCII, dst);
		dst << " )";
		return S_OK;
	}
	return format_dicm_value_ascii_string(dst, image, value_offset, value_size);
}


template <typename T>
HRESULT format_dicm_value_dec(std::wstringstream& str, const container_DCM_Impl* image, uint64_t value_offset, uint32_t value_size)
{
	if (value_size == 0xFFFFFFFF) {
		str.str(L"");
		return S_OK;
	}

	rsize_t raw_counts = value_size / sizeof(T);
	if (raw_counts == 0) {
		str.str(L"");
		return S_OK;
	}
	rsize_t counts = min(128, raw_counts);	// 大きすぎる場合は省略 

	CHeapPtr<T> buf;
	if (!buf.AllocateBytes(value_size)) return E_OUTOFMEMORY;
	auto hr = image->ReadData(buf, value_offset, value_size);
	if (FAILED(hr)) return hr;

	if (1 < counts) { str << "["; }
	for (rsize_t i = 0; i < counts; i++) {
		if (i != 0) str << ", ";
		format_dec(str, buf[i]);
	}
	if (1 < counts) { str << "]"; }
	return S_OK;
}


template <typename T>
HRESULT format_dicm_value_hex(std::wstringstream& str, const container_DCM_Impl* image, uint64_t value_offset, uint32_t value_size)
{
	if (value_size == 0xFFFFFFFF) {
		str.str(L"");
		return S_OK;
	}

	rsize_t raw_counts = value_size / sizeof(T);
	if (raw_counts == 0) {
		str.str(L"");
		return S_OK;
	}
	rsize_t counts = min(128, raw_counts);	// 大きすぎる場合は省略 

	CHeapPtr<T> buf;
	if (!buf.AllocateBytes(value_size)) return E_OUTOFMEMORY;
	auto hr = image->ReadData(buf, value_offset, value_size);
	if (FAILED(hr)) return hr;

	if (1 < counts) { str << "["; }
	for (rsize_t i = 0; i < counts; i++) {
		if (i != 0) str << ",";
		format_hex(str, buf[i]);
	}
	if (counts < raw_counts) { str << ", ..."; }
	if (1 < counts) { str << "]"; }
	return S_OK;
}


template <typename T>
HRESULT format_dicm_value_float(std::wstringstream& str, const container_DCM_Impl* image, uint64_t value_offset, uint32_t value_size)
{
	if (value_size == 0xFFFFFFFF) {
		str.str(L"");
		return S_OK;
	}

	rsize_t raw_counts = value_size / sizeof(T);
	if (raw_counts == 0) {
		str.str(L"");
		return S_OK;
	}
	rsize_t counts = min(128, raw_counts);	// 大きすぎる場合は省略 

	CHeapPtr<T> buf;
	if (!buf.AllocateBytes(value_size)) return E_OUTOFMEMORY;
	auto hr = image->ReadData(buf, value_offset, value_size);
	if (FAILED(hr)) return hr;

	if (1 < counts) { str << "["; }
	for (rsize_t i = 0; i < counts; i++) {
		if (i != 0) str << ", ";
		format_scientific(str, buf[i]);
	}
	if (counts < raw_counts) { str << ", ..."; }
	if (1 < counts) { str << "]"; }
	return S_OK;
}


HRESULT analyze_DCM::format_dicm_value(std::wstringstream& dst, const container_DCM_Impl* image, uint64_t value_offset, uint32_t value_size, uint16_t vr, const std::list<int>& codepage)
{
	switch (agalia_byteswap(vr))
	{
	case 'AE':
		return format_dicm_value_ascii_string(dst, image, value_offset, value_size);
	case 'AS':
		return format_dicm_value_ascii_string(dst, image, value_offset, value_size);
	case 'AT':
		return format_dicm_value_hex<uint16_t>(dst, image, value_offset, value_size);
	case 'CS':
		return format_dicm_value_ascii_string(dst, image, value_offset, value_size);
	case 'DA':
		return format_dicm_value_ascii_string(dst, image, value_offset, value_size);
	case 'DS':
		return format_dicm_value_ascii_string(dst, image, value_offset, value_size);
	case 'DT':
		return format_dicm_value_ascii_string(dst, image, value_offset, value_size);
	case 'FD':
		return format_dicm_value_float<double>(dst, image, value_offset, value_size);
	case 'FL':
		return format_dicm_value_float<float>(dst, image, value_offset, value_size);
	case 'IS':
		return format_dicm_value_ascii_string(dst, image, value_offset, value_size);
	case 'LO':
		return format_dicm_value_localize_string(dst, image, value_offset, value_size, codepage);
	case 'LT':
		return format_dicm_value_localize_string(dst, image, value_offset, value_size, codepage);
	case 'OB':
		return format_dicm_value_hex<uint8_t>(dst, image, value_offset, value_size);
	case 'OF':
		return format_dicm_value_float<float>(dst, image, value_offset, value_size);
	case 'OW':
		return format_dicm_value_hex<uint16_t>(dst, image, value_offset, value_size);
	case 'PN':
		return format_dicm_value_localize_string(dst, image, value_offset, value_size, codepage);
	case 'SH':
		return format_dicm_value_localize_string(dst, image, value_offset, value_size, codepage);
	case 'SL':
		return format_dicm_value_dec<int32_t>(dst, image, value_offset, value_size);
	case 'SQ':
		break; // シーケンスなので値解釈しない 
	case 'SS':
		return format_dicm_value_dec<int16_t>(dst, image, value_offset, value_size);
	case 'ST':
		return format_dicm_value_localize_string(dst, image, value_offset, value_size, codepage);
	case 'TM':
		return format_dicm_value_ascii_string(dst, image, value_offset, value_size);
	case 'UI':
		return format_dicm_value_uid(dst, image, value_offset, value_size);
	case 'UL':
		return format_dicm_value_dec<uint32_t>(dst, image, value_offset, value_size);
	case 'UN':
		return format_dicm_value_hex<uint8_t>(dst, image, value_offset, value_size);
	case 'US':
		return format_dicm_value_dec<uint16_t>(dst, image, value_offset, value_size);
	case 'UT':
		return format_dicm_value_localize_string(dst, image, value_offset, value_size, codepage);
	case 0:
		return format_dicm_value_hex<uint8_t>(dst, image, value_offset, value_size);
	}
	return E_FAIL;
}
