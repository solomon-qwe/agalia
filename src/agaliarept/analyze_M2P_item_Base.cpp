#include "pch.h"
#include "analyze_M2P_item_Base.h"

using namespace analyze_M2P;



// {4FD9FF5E-B56E-4C04-BF38-B5195206F519}
const GUID item_Base::guid_m2p =
{ 0x4fd9ff5e, 0xb56e, 0x4c04, { 0xbf, 0x38, 0xb5, 0x19, 0x52, 0x6, 0xf5, 0x19 } };



item_Base::item_Base(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:_agaliaItemBase(guid_m2p, offset, size)
{
	this->image = image;
}



item_Base::~item_Base()
{
}



HRESULT item_Base::getItemName(agaliaString** str) const
{
	UNREFERENCED_PARAMETER(str);
	return E_FAIL;
}



HRESULT item_Base::getItemPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}



HRESULT item_Base::getItemPropName(uint32_t index, agaliaString** str) const
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



HRESULT item_Base::getItemPropValue(uint32_t index, agaliaString** str) const
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



HRESULT item_Base::getChildItem(uint32_t sibling, agaliaItem** child) const
{
	UNREFERENCED_PARAMETER(sibling);
	UNREFERENCED_PARAMETER(child);
	return E_FAIL;
}



HRESULT item_Base::getNextItem(agaliaItem** next) const
{
	UNREFERENCED_PARAMETER(next);
	return E_FAIL;
}



HRESULT item_Base::getAsocImage(const agaliaContainer** imageAsoc) const
{
	if (imageAsoc == nullptr) return E_POINTER;
	*imageAsoc = image;
	return S_OK;
}



HRESULT item_Base::getGridRowCount(uint32_t* row) const
{
	UNREFERENCED_PARAMETER(row);
	return E_FAIL;
}



HRESULT item_Base::getGridValue(uint32_t row, uint32_t column, agaliaString** str) const
{
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(column);
	UNREFERENCED_PARAMETER(str);
	return E_FAIL;
}
