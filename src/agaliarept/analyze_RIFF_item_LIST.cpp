#include "pch.h"
#include "analyze_RIFF_item_LIST.h"



using namespace analyze_RIFF;



item_LIST::item_LIST(const container_RIFF* _image, uint64_t offset, uint64_t size)
	: RIFF_item_Base(_image, offset, size)
{
}



item_LIST::~item_LIST()
{
}



HRESULT item_LIST::getName(agaliaString** str) const
{
	std::wstringstream temp;

	RIFFLIST list = {};
	auto hr = image->ReadData(&list, getOffset(), sizeof(list));
	if (FAILED(hr)) return hr;

	format_fcc(temp, list.fcc);
	temp << L" (";
	format_fcc(temp, list.fccListType);
	temp << L")";

	*str = agaliaString::create(temp.str().c_str());

	return S_OK;
}



HRESULT item_LIST::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}


HRESULT item_LIST::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	auto hr = RIFF_item_Base::getPropName(index, str);
	if (SUCCEEDED(hr)) return hr;

	switch (index)
	{
	case prop_fccListType: *str = agaliaString::create(L"fccListType"); break;
	default:
		return E_INVALIDARG;
	}
	return S_OK;
}



HRESULT item_LIST::getPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	auto hr = RIFF_item_Base::getPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	if (index == prop_fccListType)
	{
		std::wstringstream temp;
		hr = format_chunk_fcc<decltype(RIFFLIST::fccListType)>(temp, offsetof(RIFFLIST, fccListType), this);
		if (FAILED(hr)) return hr;
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}

	return E_INVALIDARG;
}



HRESULT item_LIST::getChild(uint32_t sibling, agaliaElement** child) const
{
	if (sibling != 0) return E_FAIL;
	if (child == nullptr) return E_POINTER;

	RIFFCHUNK cur_chunk = {};
	auto hr = image->ReadData(&cur_chunk, getOffset(), sizeof(cur_chunk));
	if (FAILED(hr)) return hr;

	uint64_t child_item_offset = getOffset() + sizeof(RIFFLIST);

	RIFFCHUNK child_chunk = {};
	hr = image->ReadData(&child_chunk, child_item_offset, sizeof(child_chunk));
	if (FAILED(hr)) return hr;

	RIFF_item_Base* item = create_item(image, child_item_offset, child_chunk.fcc, child_chunk.cb, fccType);
	item->endPos = getOffset() + sizeof(RIFFCHUNK) + cur_chunk.cb;
	item->fccParent = cur_chunk.fcc;
	*child = item;
	return S_OK;
}



HRESULT item_LIST::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_value)
	{
		std::wstringstream temp;
		auto hr = format_chunk_fcc<decltype(RIFFLIST::fccListType)>(temp, offsetof(RIFFLIST, fccListType), this);
		if (FAILED(hr)) return hr;

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}

	return __super::getColumnValue(column, str);
}
