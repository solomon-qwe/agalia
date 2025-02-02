#include "pch.h"
#include "analyze_JPEG_item_ICC.h"



using namespace analyze_JPEG;



JPEG_item_ICC::JPEG_item_ICC(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type)
	:JPEG_item_Base(image, offset, size, type)
{
}



JPEG_item_ICC::~JPEG_item_ICC()
{
	if (image)
	{
		auto temp = container;
		container = nullptr;
		temp->Release();
	}
}



HRESULT JPEG_item_ICC::getName(agaliaString** str) const
{
	*str = agaliaString::create(L"ICC Profile");
	return S_OK;
}



HRESULT JPEG_item_ICC::getPropCount(uint32_t* count) const
{
	*count = 0;
	return S_OK;
}



HRESULT JPEG_item_ICC::getPropName(uint32_t, agaliaString**) const
{
	return E_FAIL;
}



HRESULT JPEG_item_ICC::getPropValue(uint32_t, agaliaString**) const
{
	return E_FAIL;
}



HRESULT JPEG_item_ICC::getChild(uint32_t sibling, agaliaElement** child) const
{
	if (sibling != 0) return E_FAIL;

	if (container)
		return container->getRootElement(child);
	return E_FAIL;
}



HRESULT JPEG_item_ICC::getNext(agaliaElement**) const
{
	return E_FAIL;
}



HRESULT JPEG_item_ICC::getAsocImage(const agaliaContainer** imageAsoc) const
{
	*imageAsoc = container;
	return S_OK;
}



HRESULT JPEG_item_ICC::getValueAreaOffset(uint64_t*) const
{
	return E_FAIL;
}



HRESULT JPEG_item_ICC::getValueAreaSize(uint64_t*) const
{
	return E_FAIL;
}
