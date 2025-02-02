#include "pch.h"
#include "analyze_ICC_item_Base.h"

#include "container_ICC.h"

using namespace analyze_ICC;

// {D95D1593-E9CF-4659-88D9-ADB253026CD0}
const GUID ICC_item_Base::guid_icc = 
{ 0xd95d1593, 0xe9cf, 0x4659, { 0x88, 0xd9, 0xad, 0xb2, 0x53, 0x2, 0x6c, 0xd0 } };


ICC_item_Base::ICC_item_Base(const container_ICC* image, uint64_t offset, uint64_t size, uint64_t endpos)
	: _agaliaItemBase(guid_icc, offset, size)
{
	this->image = image;
	this->endpos = endpos;
}

ICC_item_Base::~ICC_item_Base()
{
}

HRESULT ICC_item_Base::getName(agaliaString** str) const
{
	UNREFERENCED_PARAMETER(str);
	return E_FAIL;
}

HRESULT ICC_item_Base::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT ICC_item_Base::getPropName(uint32_t index, agaliaString** str) const
{
	UNREFERENCED_PARAMETER(index);
	UNREFERENCED_PARAMETER(str);
	return E_FAIL;
}


HRESULT ICC_item_Base::getPropValue(uint32_t index, agaliaString** str) const
{
	UNREFERENCED_PARAMETER(index);
	UNREFERENCED_PARAMETER(str);
	return E_FAIL;
}

HRESULT ICC_item_Base::getChild(uint32_t sibling, agaliaElement** child) const
{
	UNREFERENCED_PARAMETER(sibling);
	UNREFERENCED_PARAMETER(child);
	return E_FAIL;
}

HRESULT ICC_item_Base::getNext(agaliaElement** next) const
{
	UNREFERENCED_PARAMETER(next);
	return E_FAIL;
}

HRESULT ICC_item_Base::getAsocImage(const agaliaContainer** imageAsoc) const
{
	*imageAsoc = this->image;
	return S_OK;
}

HRESULT ICC_item_Base::getColumnValue(uint32_t column, agaliaString** str) const
{
	UNREFERENCED_PARAMETER(column);
	UNREFERENCED_PARAMETER(str);
	return E_FAIL;
}
