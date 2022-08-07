#include "pch.h"
#include "analyze_ICC_item_Base.h"

#include "container_ICC.h"

using namespace analyze_ICC;

// {D95D1593-E9CF-4659-88D9-ADB253026CD0}
const GUID item_Base::guid_icc = 
{ 0xd95d1593, 0xe9cf, 0x4659, { 0x88, 0xd9, 0xad, 0xb2, 0x53, 0x2, 0x6c, 0xd0 } };


item_Base::item_Base(const container_ICC* image, uint64_t offset, uint64_t size, uint64_t endpos)
	: _agaliaItemBase(guid_icc, offset, size)
{
	this->image = image;
	this->endpos = endpos;
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
	UNREFERENCED_PARAMETER(index);
	UNREFERENCED_PARAMETER(str);
	return E_FAIL;
}


HRESULT item_Base::getItemPropValue(uint32_t index, agaliaString** str) const
{
	UNREFERENCED_PARAMETER(index);
	UNREFERENCED_PARAMETER(str);
	return E_FAIL;
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
	*imageAsoc = this->image;
	return S_OK;
}

HRESULT item_Base::getColumnValue(uint32_t column, agaliaString** str) const
{
	UNREFERENCED_PARAMETER(column);
	UNREFERENCED_PARAMETER(str);
	return E_FAIL;
}
