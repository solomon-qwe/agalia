#include "pch.h"
#include "analyze_ICC_item_Element.h"

#include "byteswap.h"

using namespace analyze_ICC;

HRESULT decode_textDescriptionType(std::wstringstream& dest, uint8_t* buf, uint32_t size, bool add_quot)
{
	struct desc
	{
		uint32_t descriptor;
		uint32_t reserved;
		uint32_t count;
	};

	if (size < sizeof(desc))
		return E_FAIL;

	desc* p = reinterpret_cast<desc*>(buf);

	if (agalia_byteswap(p->descriptor) != 'desc')
		return E_FAIL;

	if (size < sizeof(desc) + agalia_byteswap(p->count))
		return E_FAIL;

	// Photoshopで保存したファイルにShift-JISで格納されていたので、CP_US_ASCIIではなくCP_ACPを指定することにする 
	std::wstringstream temp;
	if (add_quot) temp << L"\"";
	auto hr = multibyte_to_widechar((char*)buf + sizeof(desc), agalia_byteswap(p->count), CP_ACP, temp);
	if (FAILED(hr)) return hr;
	if (add_quot) temp << L"\"";

	dest << temp.str();
	return S_OK;
}

HRESULT decode_textType(std::wstringstream& dest, uint8_t* buf, uint32_t size, bool add_quot)
{
	struct text
	{
		uint32_t descriptor;
		uint32_t reserved;
	};

	if (size < sizeof(text))
		return E_FAIL;

	text* p = reinterpret_cast<text*>(buf);

	if (agalia_byteswap(p->descriptor) != 'text')
		return E_FAIL;

	std::wstringstream temp;
	if (add_quot) temp << L"\"";
	auto hr = multibyte_to_widechar((char*)buf + sizeof(text), size - sizeof(text), CP_US_ASCII, temp);
	if (FAILED(hr)) return hr;
	if (add_quot) temp << L"\"";

	dest << temp.str();
	return S_OK;
}

HRESULT decode_multiLocalizedUnicodeType(std::wstringstream& dest, uint8_t* buf, uint32_t size, bool add_quot, bool only_first)
{
#pragma pack(push, 1)
	struct mluc
	{
		uint32_t signature;
		uint32_t reserved;
		uint32_t records;
		uint32_t size;
	};

	struct record
	{
		uint16_t language;
		uint16_t country;
		uint32_t length;
		uint32_t offset;
	};
#pragma pack(pop)

	if (size < sizeof(mluc))
		return E_FAIL;

	mluc* p = reinterpret_cast<mluc*>(buf);

	if (agalia_byteswap(p->signature) != 'mluc')
		return E_FAIL;

	std::wstringstream temp;
	size_t count = only_first ? 1 : agalia_byteswap(p->records);
	for (size_t i = 0; i < count; i++)
	{
		record* q = reinterpret_cast<record*>(buf + sizeof(mluc) + sizeof(record) * i);

		char code[5] = {};
		memcpy_s(code, sizeof(code), q, sizeof(uint16_t) * 2);

		multibyte_to_widechar(code, _countof(code), CP_US_ASCII, temp);

		CHeapPtr<wchar_t> utf16le;
		if (!utf16le.AllocateBytes(agalia_byteswap(q->length) + 2))
			return E_OUTOFMEMORY;
		memset(utf16le, 0, agalia_byteswap(q->length) + 2);
		for (size_t j = 0; j < agalia_byteswap(q->length) / 2; j++)
		{
			uint16_t c = ((uint16_t*)(buf + agalia_byteswap(q->offset)))[j];
			utf16le.m_pData[j] = (wchar_t)agalia_byteswap(c);
		}

		if (add_quot) temp << L":\"";
		temp << utf16le.m_pData;
		if (add_quot) temp << L":\"";
		if (i - 1 != count)
			temp << L", ";
	}

	dest << temp.str();
	return S_OK;

}

HRESULT item_Element::getProfileName(agaliaString** str)
{
	Element elem = {};
	auto hr = image->ReadData(&elem, getOffset(), sizeof(elem));
	if (FAILED(hr)) return hr;

	CHeapPtr<uint8_t> buf;
	if (!buf.AllocateBytes(elem.getSize()))
		return E_OUTOFMEMORY;

	hr = image->ReadData(buf, elem.getOffset(), elem.getSize());
	if (FAILED(hr)) return hr;

	std::wstringstream temp;
	if (SUCCEEDED(decode_textDescriptionType(temp, buf, elem.getSize(), false)) ||
		SUCCEEDED(decode_textType(temp, buf, elem.getSize(), false)) ||
		SUCCEEDED(decode_multiLocalizedUnicodeType(temp, buf, elem.getSize(), false, true)))
	{
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}

	return E_FAIL;
}


item_Element::item_Element(const container_ICC* image, uint64_t offset, uint64_t size, uint64_t endpos, uint32_t count, uint32_t index)
	: ICC_item_Base(image, offset, size, endpos), tag_count(count), tag_index(index)
{
}

item_Element::~item_Element()
{
}

HRESULT item_Element::getName(agaliaString** str) const
{
	return getPropValue(prop_signature, str);
}

HRESULT item_Element::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT item_Element::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_signature: name = L"Signature"; break;
	case prop_offset: name = L"Offset"; break;
	case prop_size: name = L"Size"; break;
	case prop_value: name = L"Value"; break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(name);
	return S_OK;
}


HRESULT item_Element::getPropValue(uint32_t index, agaliaString** str) const
{
	if (index == prop_signature)
	{
		decltype(Element::signature) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Element, signature), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		multibyte_to_widechar((char*)&val, sizeof(val), CP_US_ASCII, temp);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_offset)
	{
		decltype(Element::offset) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Element, offset), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		format_dec(temp, agalia_byteswap(val));

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_size)
	{
		decltype(Element::size) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Element, size), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		format_dec(temp, agalia_byteswap(val));

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_value)
	{
		Element elem = {};
		auto hr = image->ReadData(&elem, getOffset(), sizeof(elem));
		if (FAILED(hr)) return hr;

		CHeapPtr<uint8_t> buf;
		if (!buf.AllocateBytes(elem.getSize()))
			return E_OUTOFMEMORY;

		hr = image->ReadData(buf, elem.getOffset(), elem.getSize());
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		if (SUCCEEDED(decode_textDescriptionType(temp, buf, elem.getSize(), true)) ||
			SUCCEEDED(decode_textType(temp, buf, elem.getSize(), true)) ||
			SUCCEEDED(decode_multiLocalizedUnicodeType(temp, buf, elem.getSize(), true, false)))
		{
			*str = agaliaString::create(temp.str().c_str());
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT item_Element::getChild(uint32_t sibling, agaliaElement** child) const
{
	UNREFERENCED_PARAMETER(sibling);
	UNREFERENCED_PARAMETER(child);
	return E_FAIL;
}

HRESULT item_Element::getNext(agaliaElement** next) const
{
	if (tag_count <= tag_index + 1)
		return E_FAIL;

	if (endpos < getOffset() + getSize() + sizeof(Element))
		return E_FAIL;

	*next = new item_Element(image, getOffset() + getSize(), sizeof(Element), endpos, tag_count, tag_index + 1);
	return S_OK;
}


HRESULT item_Element::getValueAreaOffset(uint64_t* offset) const
{
	if (offset == nullptr) return E_POINTER;

	decltype(Element::offset) val;
	auto hr = image->ReadData(&val, getOffset() + offsetof(Element, offset), sizeof(val));
	if (FAILED(hr)) return hr;

	*offset = agalia_byteswap(val);
	return S_OK;
}

HRESULT item_Element::getValueAreaSize(uint64_t* size) const
{
	if (size == nullptr) return E_POINTER;

	decltype(Element::size) val;
	auto hr = image->ReadData(&val, getOffset() + offsetof(Element, size), sizeof(val));
	if (FAILED(hr)) return hr;

	*size = agalia_byteswap(val);
	return S_OK;
}

HRESULT item_Element::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	if (column == column_offset)
	{
		std::wstringstream temp;
		format_dec(temp, getOffset());
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (column == column_signature)
	{
		return getPropValue(prop_signature, str);
	}
	else if (column == column_value_offset)
	{
		return getPropValue(prop_offset, str);
	}
	else if (column == column_element_size)
	{
		return getPropValue(prop_size, str);
	}
	else if (column == column_value)
	{
		return getPropValue(prop_value, str);
	}
	return E_INVALIDARG;
}
