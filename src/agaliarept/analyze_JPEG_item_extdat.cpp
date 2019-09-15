#include "pch.h"
#include "analyze_JPEG_item_extdat.h"

using namespace analyze_JPEG;



item_extdat::item_extdat(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type)
	:item_Base(image, offset, size, type)
{
}



item_extdat::~item_extdat()
{
}



HRESULT item_extdat::getItemName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	switch (data_type)
	{
	case item_Base::xmp:
		*str = agaliaString::create(L"XMP");
		return S_OK;

	case item_Base::icc_profile:
		*str = agaliaString::create(L"ICC profile");
		return S_OK;

	case item_Base::ducky:
		*str = agaliaString::create(L"Ducky");
		return S_OK;

	case item_Base::photoshop:
		*str = agaliaString::create(L"Photoshop");
		return S_OK;

	case item_Base::adobe:
		*str = agaliaString::create(L"Adobe");
		return S_OK;
	}

	return E_FAIL;
}



HRESULT item_extdat::getValueAreaOffset(uint64_t* ) const
{
	return E_FAIL;
}



HRESULT item_extdat::getValueAreaSize(uint64_t* ) const
{
	return E_FAIL;
}



HRESULT item_extdat::getColumnValue(uint32_t , agaliaString** ) const
{
	return E_FAIL;
}



HRESULT item_extdat::getNextItem(agaliaItem** ) const
{
	return E_FAIL;
}



HRESULT item_extdat::getItemPropCount(uint32_t* ) const
{
	return E_FAIL;
}



HRESULT item_extdat::getItemPropName(uint32_t , agaliaString** ) const
{
	return E_FAIL;
}



HRESULT item_extdat::getItemPropValue(uint32_t , agaliaString** ) const
{
	return E_FAIL;
}
