#include "pch.h"
#include "analyze_JPEG_item_Exif.h"



using namespace analyze_JPEG;



item_ExifJPEG::item_ExifJPEG(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type)
	:JPEG_item_Base(image, offset, size, type)
{
}



item_ExifJPEG::~item_ExifJPEG()
{
	//if (stream)
	//{
	//	auto temp = stream;
	//	stream = nullptr;
	//	temp->Release();
	//}

	if (image)
	{
		auto temp = tiff_image;
		tiff_image = nullptr;
		temp->Release();
	}
}



HRESULT item_ExifJPEG::getName(agaliaString** str) const
{
	*str = agaliaString::create(L"Exif");
	return S_OK;
}



HRESULT item_ExifJPEG::getPropCount(uint32_t* count) const
{
	*count = 0;
	return S_OK;
}



HRESULT item_ExifJPEG::getPropName(uint32_t, agaliaString**) const
{
	return E_FAIL;
}



HRESULT item_ExifJPEG::getPropValue(uint32_t, agaliaString**) const
{
	return E_FAIL;
}



HRESULT item_ExifJPEG::getChild(uint32_t sibling, agaliaElement** child) const
{
	if (sibling != 0) return E_FAIL;

	if (tiff_image)
		return tiff_image->getRootElement(child);
	return E_FAIL;
}



HRESULT item_ExifJPEG::getNext(agaliaElement**) const
{
	return E_FAIL;
}



HRESULT item_ExifJPEG::getAsocImage(const agaliaContainer** imageAsoc) const
{
	*imageAsoc = tiff_image;
	return S_OK;
}



HRESULT item_ExifJPEG::getValueAreaOffset(uint64_t*) const
{
	return E_FAIL;
}



HRESULT item_ExifJPEG::getValueAreaSize(uint64_t*) const
{
	return E_FAIL;
}
