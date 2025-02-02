#include "pch.h"
#include "analyze_ICC_item_TagTable.h"

#include "analyze_ICC_item_Element.h"
#include "byteswap.h"

using namespace analyze_ICC;

item_TagTable::item_TagTable(const container_ICC* image, uint64_t offset, uint64_t size, uint64_t endpos)
	: ICC_item_Base(image, offset, size, endpos)
{
}

item_TagTable::~item_TagTable()
{
}

HRESULT item_TagTable::getName(agaliaString** str) const
{
	*str = agaliaString::create(L"Tag Table");
	return S_OK;
}

HRESULT item_TagTable::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT item_TagTable::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_tag_count: name = L"Tag count"; break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(name);
	return S_OK;
}


HRESULT item_TagTable::getPropValue(uint32_t index, agaliaString** str) const
{
	if (index == prop_tag_count)
	{
		uint32_t val;
		auto hr = image->ReadData(&val, getOffset(), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		format_dec(temp, agalia_byteswap(val));

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}

	return E_FAIL;
}

HRESULT item_TagTable::getChild(uint32_t sibling, agaliaElement** child) const
{
	if (sibling != 0)
		return E_FAIL;

	uint32_t count;
	auto hr = image->ReadData(&count, getOffset(), sizeof(count));
	if (FAILED(hr)) return hr;
	count = agalia_byteswap(count);

	*child = new item_Element(image, getOffset() + getSize(), sizeof(Element), endpos, count, 0);
	return S_OK;
}

HRESULT item_TagTable::getNext(agaliaElement** next) const
{
	UNREFERENCED_PARAMETER(next);
	return E_FAIL;
}
