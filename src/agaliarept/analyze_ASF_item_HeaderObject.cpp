#include "pch.h"
#include "analyze_ASF_item_HeaderObject.h"

#include "analyze_ASF_util.h"

using namespace analyze_ASF;

item_HeaderObject::item_HeaderObject(const agaliaContainer* image, uint64_t offset, uint64_t size, uint64_t endpos)
	:ASF_item_Base(image, offset, size, endpos)
{

}

item_HeaderObject::~item_HeaderObject()
{

}

HRESULT item_HeaderObject::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT item_HeaderObject::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_NumberOfHeaderObject: name = L"Number Of Header Objects"; break;
	case prop_Reserved1: name = L"Reserved1"; break;
	case prop_Reserved2: name = L"Reserved2"; break;
	default:
		return __super::getPropName(index, str);
	}

	*str = agaliaString::create(name);
	return S_OK;
}

HRESULT item_HeaderObject::getPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	std::wstringstream temp;
	if (index == prop_NumberOfHeaderObject)
	{
		HeaderObject h = {};
		auto hr = image->ReadData(&h.NumberOfHeaderObjects, getOffset() + offsetof(HeaderObject, NumberOfHeaderObjects), sizeof(HeaderObject::NumberOfHeaderObjects));
		if (FAILED(hr)) return hr;

		format_dec(temp, h.NumberOfHeaderObjects);
	}
	else if (index == prop_Reserved1)
	{
		HeaderObject h = {};
		auto hr = image->ReadData(&h.Reserved1, getOffset() + offsetof(HeaderObject, Reserved1), sizeof(HeaderObject::Reserved1));
		if (FAILED(hr)) return hr;

		format_dec(temp, h.Reserved1);
	}
	else if (index == prop_Reserved2)
	{
		HeaderObject h = {};
		auto hr = image->ReadData(&h.Reserved2, getOffset() + offsetof(HeaderObject, Reserved2), sizeof(HeaderObject::Reserved2));
		if (FAILED(hr)) return hr;

		format_dec(temp, h.Reserved2);
	}
	else
	{
		return __super::getPropValue(index, str);
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}



HRESULT item_HeaderObject::getChild(uint32_t sibling, agaliaElement** child) const
{
	if (sibling != 0) return E_FAIL;
	if (child == nullptr) return E_POINTER;

	HeaderObject h = {};
	auto hr = image->ReadData(&h, getOffset(), sizeof(h));
	if (FAILED(hr)) return hr;

	if (h.NumberOfHeaderObjects == 0)
		return E_FAIL;

	auto p = createItem(image, getOffset() + sizeof(HeaderObject), getOffset() + h.ObjectSize);
	if (p)
	{
		p->parent.reset(new uint64_t(getOffset()));
		*child = p;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT item_HeaderObject::getElementInfoCount(uint32_t* row) const
{
	if (row == nullptr) return E_POINTER;
	*row = 3;
	return S_OK;
}

HRESULT item_HeaderObject::getElementInfoValue(uint32_t row, uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	
	auto hr = __super::getElementInfoValue(row, column, str);
	if (SUCCEEDED(hr)) return hr;

	std::wstringstream temp;
	if (column == column_offset)
	{
	}
	else if (column == column_value)
	{
		if (row == 0)
		{
			temp << L"Number Of Header Objects=";
			HeaderObject h = {};
			hr = image->ReadData(&h.NumberOfHeaderObjects, getOffset() + offsetof(HeaderObject, NumberOfHeaderObjects), sizeof(HeaderObject::NumberOfHeaderObjects));
			if (FAILED(hr)) return hr;
			format_dec(temp, h.NumberOfHeaderObjects);
		}
		else if (row == 1)
		{
			temp << L"Reserved1=";
			HeaderObject h = {};
			hr = image->ReadData(&h.Reserved1, getOffset() + offsetof(HeaderObject, Reserved1), sizeof(HeaderObject::Reserved1));
			if (FAILED(hr)) return hr;
			format_dec(temp, h.Reserved1);
		}
		else if (row == 2)
		{
			temp << L"Reserved2=";
			HeaderObject h = {};
			hr = image->ReadData(&h.Reserved2, getOffset() + offsetof(HeaderObject, Reserved2), sizeof(HeaderObject::Reserved2));
			if (FAILED(hr)) return hr;
			format_dec(temp, h.Reserved2);
		}
	}
	else
	{
		return E_INVALIDARG;
	}
	
	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
