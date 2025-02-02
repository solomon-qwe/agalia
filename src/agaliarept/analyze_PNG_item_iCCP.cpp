#include "pch.h"
#include "analyze_PNG_item_iCCP.h"

#include "container_PNG.h"
#include "byteswap.h"

using namespace analyze_PNG;

item_iCCP::item_iCCP(const container_PNG* image, uint64_t offset, uint64_t size, uint64_t endpos)
	: PNG_item_Base(image, offset, size, endpos)
{
}

item_iCCP::~item_iCCP()
{
}

HRESULT item_iCCP::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT item_iCCP::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_profile_name: name = L"Profile name"; break;
	case prop_compression_method: name = L"Compression method"; break;
	default:
		return __super::getPropName(index, str);
	}

	*str = agaliaString::create(name);
	return S_OK;
}


HRESULT item_iCCP::getPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	std::wstringstream temp;
	if (index == prop_profile_name)
	{
		uint32_t length = 0;
		auto hr = image->ReadData(&length, getOffset(), sizeof(length));
		if (FAILED(hr)) return hr;
		length = agalia_byteswap(length);

		CHeapPtr<uint8_t> buf;
		if (!buf.AllocateBytes(length + 1))
			return E_OUTOFMEMORY;

		hr = image->ReadData(buf, getOffset() + offsetof(Chunk, ChunkData), length);
		if (FAILED(hr)) return hr;
		buf[length] = '\0';

		char* profile_name = (char*)buf.m_pData;

		int profile_name_length = 0;
		hr = SIZETToInt(strnlen_s(profile_name, length), &profile_name_length);
		if (FAILED(hr)) return hr;

		temp << L"\"";
		multibyte_to_widechar(profile_name, profile_name_length, CP_LATIN1, temp);
		temp << L"\"";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_compression_method)
	{
		uint32_t length = 0;
		auto hr = image->ReadData(&length, getOffset(), sizeof(length));
		if (FAILED(hr)) return hr;
		length = agalia_byteswap(length);

		CHeapPtr<uint8_t> buf;
		if (!buf.AllocateBytes(length + 1))
			return E_OUTOFMEMORY;

		hr = image->ReadData(buf, getOffset() + offsetof(Chunk, ChunkData), length);
		if (FAILED(hr)) return hr;
		buf[length] = '\0';

		char* profile_name = (char*)buf.m_pData;

		size_t profile_name_length = strnlen_s(profile_name, length);
		size_t pos = profile_name_length + 1;
		if (length < pos)
			return E_FAIL;

		format_dec(temp, buf[pos]);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else
	{
		return __super::getPropValue(index, str);
	}

	return E_FAIL;
}
