#include "pch.h"
#include "analyze_JPEG_item_ICC.h"



using namespace analyze_JPEG;



item_ICC::item_ICC(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type)
	:item_Base(image, offset, size, type)
{
}



item_ICC::~item_ICC()
{
	if (image)
	{
		auto temp = container;
		container = nullptr;
		temp->Release();
	}
}



HRESULT item_ICC::getItemName(agaliaString** str) const
{
	*str = agaliaString::create(L"ICC Profile");
	return S_OK;
}



HRESULT item_ICC::getItemPropCount(uint32_t* count) const
{
	*count = 0;
	return S_OK;
}



HRESULT item_ICC::getItemPropName(uint32_t, agaliaString**) const
{
	return E_FAIL;
}



HRESULT item_ICC::getItemPropValue(uint32_t, agaliaString**) const
{
	return E_FAIL;
}



HRESULT item_ICC::getChildItem(uint32_t sibling, agaliaItem** child) const
{
	if (sibling != 0) return E_FAIL;

	if (container)
		return container->getRootItem(child);
	return E_FAIL;
}



HRESULT item_ICC::getNextItem(agaliaItem**) const
{
	return E_FAIL;
}



HRESULT item_ICC::getAsocImage(const agaliaContainer** imageAsoc) const
{
	*imageAsoc = container;
	return S_OK;
}



HRESULT item_ICC::getValueAreaOffset(uint64_t*) const
{
	return E_FAIL;
}



HRESULT item_ICC::getValueAreaSize(uint64_t*) const
{
	return E_FAIL;
}
