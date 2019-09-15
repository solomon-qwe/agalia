#include "pch.h"
#include "analyze_JPEG_item_SOI.h"

#include "analyze_JPEG_util.h"



using namespace analyze_JPEG;



item_SOI::item_SOI(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type)
	:item_Marker(image, offset, size, type)
{
}



item_SOI::~item_SOI()
{
}



HRESULT item_SOI::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_length)
	{
		return E_FAIL;
	}
	else if (column == column_value)
	{
		return E_FAIL;
	}

	return __super::getColumnValue(column, str);
}



HRESULT item_SOI::getValueAreaOffset(uint64_t* offset) const
{
	*offset = getOffset() + sizeof(JPEGSEGMENT);
	return S_OK;
}



HRESULT item_SOI::getValueAreaSize(uint64_t* size) const
{
	*size = 0;
	return S_OK;
}
