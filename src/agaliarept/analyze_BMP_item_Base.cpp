#include "pch.h"
#include "analyze_BMP_item_Base.h"

using namespace analyze_BMP;

// {5CEFA4C6-B92F-44BE-9E98-53EAB454F2EA}
const GUID BMP_item_Base::guid_bmp =
{ 0x5cefa4c6, 0xb92f, 0x44be, { 0x9e, 0x98, 0x53, 0xea, 0xb4, 0x54, 0xf2, 0xea } };

BMP_item_Base::BMP_item_Base(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:_agaliaItemBase(guid_bmp, offset, size)
{
	this->image = image;
}

BMP_item_Base::~BMP_item_Base()
{

}

HRESULT BMP_item_Base::getName(agaliaString** str) const
{
	UNREFERENCED_PARAMETER(str);
	return E_FAIL;
}

HRESULT BMP_item_Base::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT BMP_item_Base::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_offset: name = L"offset"; break;
	default:
		return E_INVALIDARG;
	};

	*str = agaliaString::create(name);
	return S_OK;
}

HRESULT BMP_item_Base::getPropValue(uint32_t index, agaliaString** str) const
{
	if (index == prop_offset)
	{
		std::wstringstream temp;
		format_dec(temp, getOffset());
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	return E_INVALIDARG;
}


HRESULT BMP_item_Base::getChild(uint32_t, agaliaElement**) const
{
	return E_FAIL;
}

HRESULT BMP_item_Base::getNext(agaliaElement**) const
{
	return E_FAIL;
}


HRESULT BMP_item_Base::getAsocImage(const agaliaContainer** imageAsoc) const
{
	if (imageAsoc == nullptr) return E_POINTER;
	*imageAsoc = image;
	return S_OK;
}


HRESULT BMP_item_Base::getColumnValue(uint32_t, agaliaString**) const
{
	return E_FAIL;
}

HRESULT BMP_item_Base::getAdditionalInfoCount(uint32_t* row) const
{
	*row = 0;
	return S_OK;
}

HRESULT BMP_item_Base::getAdditionalInfoValue(uint32_t, agaliaString**) const
{
	return E_FAIL;
}
