#include "pch.h"
#include "analyze_BMP_item_RGBQUAD.h"

#include "analyze_BMP_item_ColorIndexParent.h"

using namespace analyze_BMP;

item_RGBQUAD::item_RGBQUAD(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:item_Base(image, offset, size)
{
}

item_RGBQUAD::~item_RGBQUAD()
{
}

HRESULT item_RGBQUAD::getItemName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	std::wstringstream temp;
	temp << L"RGBQUAD[" << _index << L"]";
	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}

HRESULT item_RGBQUAD::getNextItem(agaliaItem** next) const
{
	if (_index + 1 < colors)
	{
		auto p = new item_RGBQUAD(image, getOffset() + sizeof(RGBQUAD), sizeof(RGBQUAD));
		p->scans = scans;
		p->bytes_of_line = bytes_of_line;
		p->biCompression = biCompression;
		p->biSizeImage = biSizeImage;
		p->colors = colors;
		p->_index = _index + 1;
		*next = p;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT item_RGBQUAD::getItemPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT item_RGBQUAD::getItemPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_blue: name = L"rgbBlue"; break;
	case prop_green: name = L"rgbGreen"; break;
	case prop_red: name = L"rgbRed"; break;
	case prop_reserved: name = L"rgbReserved"; break;
	default:
		return E_INVALIDARG;
	}
	*str = agaliaString::create(name);
	return S_OK;
}

HRESULT item_RGBQUAD::getItemPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	auto hr = __super::getItemPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	RGBQUAD quad = {};
	hr = image->ReadData(&quad, getOffset(), sizeof(quad));
	if (FAILED(hr)) return hr;

	std::wstringstream temp;
	switch (index)
	{
	case prop_blue: format_dec(temp, quad.rgbBlue); break;
	case prop_green: format_dec(temp, quad.rgbGreen); break;
	case prop_red: format_dec(temp, quad.rgbRed); break;
	case prop_reserved: format_dec(temp, quad.rgbReserved); break;
	default:
		return E_INVALIDARG;
	}
	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}


HRESULT item_RGBQUAD::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_offset)
	{
		std::wstringstream temp;
		format_hex(temp, getOffset(), 8);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (column == column_structure)
	{
		return getItemName(str);
	}
	else if (column == column_value)
	{
		RGBQUAD quad = {};
		auto hr = image->ReadData(&quad, getOffset(), sizeof(quad));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"rgbBlue=" << quad.rgbBlue << L", ";
		temp << L"rgbGreen=" << quad.rgbGreen << L", ";
		temp << L"rgbRed=" << quad.rgbRed << L", ";
		temp << L"rgbReserved=" << quad.rgbReserved;
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	return E_INVALIDARG;
}
