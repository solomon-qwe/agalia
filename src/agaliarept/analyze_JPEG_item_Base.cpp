#include "pch.h"
#include "analyze_JPEG_item_Base.h"

#include "jpeg_def.h"
#include "byteswap.h"

using namespace analyze_JPEG;



// {C04354BB-B93C-4907-8ACC-0F840B86EA6B}
const GUID item_Base::guid_jpeg =
{ 0xc04354bb, 0xb93c, 0x4907, { 0x8a, 0xcc, 0xf, 0x84, 0xb, 0x86, 0xea, 0x6b } };


item_Base::item_Base(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type)
	: _agaliaItemBase(guid_jpeg, offset, size)
{
	data_type = type;
	this->image = image;
}



item_Base::~item_Base()
{
}



HRESULT item_Base::getChildItem(uint32_t, agaliaItem**) const
{
	return E_FAIL;
}



HRESULT item_Base::getColumnValue(uint32_t, agaliaString**) const
{
	return E_FAIL;
}



HRESULT item_Base::getAdditionalInfoCount(uint32_t* row) const
{
	*row = 0;
	return S_OK;
}



HRESULT item_Base::getAdditionalInfoValue(uint32_t, agaliaString**) const
{
	return E_FAIL;
}
