#include "pch.h"
#include "analyze_BMP_item_RGBQUADParent.h"

#include "analyze_BMP_item_RGBQUAD.h"
#include "analyze_BMP_item_ColorIndexParent.h"

using namespace analyze_BMP;

item_RGBQUADParent::item_RGBQUADParent(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:item_Base(image, offset, size)
{
}

item_RGBQUADParent::~item_RGBQUADParent()
{
}

HRESULT item_RGBQUADParent::getItemName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	*str = agaliaString::create(L"RGBQUAD");
	return S_OK;
}

HRESULT item_RGBQUADParent::getChildItem(uint32_t sibling, agaliaItem** child) const
{
	if (sibling != 0) return E_FAIL;

	auto p = new item_RGBQUAD(image, getOffset(), sizeof(RGBQUAD));
	p->scans = scans;
	p->bytes_of_line = bytes_of_line;
	p->biCompression = biCompression;
	p->biSizeImage = biSizeImage;
	p->colors = colors;
	p->_index = 0;
	*child = p;
	return S_OK;
}

HRESULT item_RGBQUADParent::getNextItem(agaliaItem** next) const
{
	auto p = new item_ColorIndexParent(image, bfOffBits, biSizeImage);
	p->bfOffBits = bfOffBits;
	p->scans = scans;
	p->bytes_of_line = bytes_of_line;
	p->biCompression = biCompression;
	p->biSizeImage = biSizeImage;
	*next = p;
	return S_OK;
}
