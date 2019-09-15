#include "pch.h"
#include "analyze_TIFF_item_Base.h"

#include "container_TIFF.h"



using namespace analyze_TIFF;



// {C7797837-FF01-40C9-9CCB-AD451F2B129A}
const GUID item_tiff_Base::guid_tiff =
{ 0xc7797837, 0xff01, 0x40c9, { 0x9c, 0xcb, 0xad, 0x45, 0x1f, 0x2b, 0x12, 0x9a } };



item_tiff_Base::item_tiff_Base(const container_TIFF* image, uint64_t offset, uint64_t size)
	: _agaliaItemBase(guid_tiff, offset, size)
{
	this->image = image;
}



item_tiff_Base::~item_tiff_Base()
{
}



HRESULT item_tiff_Base::getAsocImage(const agaliaContainer** imageAsoc) const
{
	*imageAsoc = this->image;
	return S_OK;
}



HRESULT item_tiff_Base::getColumnValue(uint32_t, agaliaString**) const
{
	return E_FAIL;
}
