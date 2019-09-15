#include "pch.h"
#include "analyze_BMP_item_ColorMask.h"

#include "analyze_BMP_item_ColorIndexParent.h"

using namespace analyze_BMP;

item_ColorMask::item_ColorMask(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:item_Base(image, offset, size)
{
}

item_ColorMask::~item_ColorMask()
{
}

HRESULT item_ColorMask::getItemName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	*str = agaliaString::create(L"Color-mask");
	return S_OK;
}

HRESULT item_ColorMask::getNextItem(agaliaItem** next) const
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

HRESULT item_ColorMask::getItemPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT item_ColorMask::getItemPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_red: name = L"red"; break;
	case prop_green: name = L"green"; break;
	case prop_blue: name = L"blue"; break;
	default:
		return __super::getItemPropName(index, str);
	}

	*str = agaliaString::create(name);
	return S_OK;
}

HRESULT item_ColorMask::getItemPropValue(uint32_t index, agaliaString** str) const
{
	DWORD mask = 0;
	if (index == prop_red)
	{
		auto hr = image->ReadData(&mask, getOffset() + sizeof(mask) * 0, sizeof(mask));
		if (FAILED(hr)) return hr;
	}
	else if (index == prop_green)
	{
		auto hr = image->ReadData(&mask, getOffset() + sizeof(mask) * 1, sizeof(mask));
		if (FAILED(hr)) return hr;
	}
	else if (index == prop_blue)
	{
		auto hr = image->ReadData(&mask, getOffset() + sizeof(mask) * 2, sizeof(mask));
		if (FAILED(hr)) return hr;
	}
	else
	{
		return __super::getItemPropValue(index, str);
	}

	std::wstringstream temp;
	format_hex(temp, mask);
	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}


HRESULT item_ColorMask::getColumnValue(uint32_t column, agaliaString** str) const
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
		DWORD mask = 0;
		auto hr = image->ReadData(&mask, getOffset(), sizeof(mask));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"red : 0x";
		format_hex(temp, mask);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	return E_INVALIDARG;
}

HRESULT item_ColorMask::getAdditionalInfoCount(uint32_t* row) const
{
	*row = 2;
	return S_OK;
}

HRESULT item_ColorMask::getAdditionalInfoValue(uint32_t row, agaliaString** str) const
{
	if (2 <= row) return E_FAIL;

	DWORD mask = 0;
	auto hr = image->ReadData(&mask, getOffset() + sizeof(mask) * (static_cast<uint64_t>(row) + 1), sizeof(mask));
	if (FAILED(hr)) return hr;

	std::wstringstream temp;
	if (row == 0) {
		temp << L"green : 0x";
	}
	else if (row == 1) {
		temp << L"blue : 0x";
	}
	format_hex(temp, mask);
	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
