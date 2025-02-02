#include "pch.h"
#include "analyze_PNG_item_iTXt.h"

#include "container_PNG.h"
#include "byteswap.h"

using namespace analyze_PNG;

item_iTXt::item_iTXt(const container_PNG* image, uint64_t offset, uint64_t size, uint64_t endpos)
	: PNG_item_Base(image, offset, size, endpos)
{
}

item_iTXt::~item_iTXt()
{
}

HRESULT item_iTXt::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT item_iTXt::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_keyword: name = L"Keyword"; break;
	case prop_compression_flag: name = L"Compression flag"; break;
	case prop_compression_method: name = L"Compression method"; break;
	case prop_language_tag: name = L"Language tag"; break;
	case prop_translated_keyword: name = L"Translated keyword"; break;
	case prop_text: name = L"Text"; break;
	default:
		return __super::getPropName(index, str);
	}

	*str = agaliaString::create(name);
	return S_OK;
}


HRESULT item_iTXt::getPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	std::wstringstream temp;
	if (index == prop_keyword)
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

		char* keyword = (char*)buf.m_pData;

		int keyword_length = 0;
		hr = SIZETToInt(strnlen_s(keyword, length), &keyword_length);
		if (FAILED(hr)) return hr;

		temp << L"\"";
		multibyte_to_widechar(keyword, keyword_length, CP_US_ASCII, temp);
		temp << L"\"";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_compression_flag)
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

		char* keyword = (char*)buf.m_pData;

		size_t keyword_length = strnlen_s(keyword, length);
		size_t pos = keyword_length + 1;
		if (length < pos)
			return E_FAIL;

		format_dec(temp, buf[pos]);
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

		char* keyword = (char*)buf.m_pData;

		size_t keyword_length = strnlen_s(keyword, length);
		size_t pos = keyword_length + 1 + 1;
		if (length < pos)
			return E_FAIL;

		format_dec(temp, buf[pos]);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_language_tag)
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

		char* keyword = (char*)buf.m_pData;

		size_t keyword_length = strnlen_s(keyword, length);
		size_t pos = keyword_length + 1 + 1 + 1;
		if (length < pos)
			return E_FAIL;

		char* lang = (char*)buf.m_pData + pos;

		int lang_length = 0;
		hr = SIZETToInt(strnlen_s(lang, length - pos), &lang_length);
		if (FAILED(hr)) return hr;

		temp << L"\"";
		multibyte_to_widechar(lang, lang_length, CP_US_ASCII, temp);
		temp << L"\"";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_translated_keyword)
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

		char* keyword = (char*)buf.m_pData;

		size_t keyword_length = strnlen_s(keyword, length);
		size_t pos = keyword_length + 3;
		if (length < pos)
			return E_FAIL;

		char* lang = (char*)buf.m_pData + pos;

		size_t lang_length = strnlen_s(lang, length - pos);
		pos += lang_length + 1;
		if (length < pos)
			return E_FAIL;

		char* trans = (char*)buf.m_pData + pos;

		int trans_length = 0;
		hr = SIZETToInt(strnlen_s(trans, length - pos), &trans_length);
		if (FAILED(hr)) return hr;

		temp << L"\"";
		multibyte_to_widechar(trans, trans_length, CP_UTF8, temp);
		temp << L"\"";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_text)
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

		char* keyword = (char*)buf.m_pData;

		size_t keyword_length = strnlen_s(keyword, length);
		size_t pos = keyword_length + 3;
		if (length < pos)
			return E_FAIL;

		char* lang = (char*)buf.m_pData + pos;

		size_t lang_length = strnlen_s(lang, length - pos);
		pos += lang_length + 1;
		if (length < pos)
			return E_FAIL;

		char* trans = (char*)buf.m_pData + pos;

		size_t trans_length = strnlen_s(trans, length - pos);
		pos += trans_length + 1;
		if (length < pos)
			return E_FAIL;

		char* text = (char*)buf.m_pData + pos;

		int text_length = 0;
		hr = SIZETToInt(strnlen_s(text, length - pos), &text_length);
		if (FAILED(hr)) return hr;

		temp << L"\"";
		multibyte_to_widechar(text, text_length, CP_UTF8, temp);
		temp << L"\"";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else
	{
		return __super::getPropValue(index, str);
	}

	return E_FAIL;
}
