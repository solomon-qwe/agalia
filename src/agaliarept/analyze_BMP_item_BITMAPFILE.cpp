#include "pch.h"
#include "analyze_BMP_item_BITMAPFILE.h"

#include "analyze_BMP_item_BITMAPCORE.h"
#include "analyze_BMP_item_BITMAPV5.h"

using namespace analyze_BMP;



item_BITMAPFILE::item_BITMAPFILE(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:item_Base(image, offset, size)
{
}



item_BITMAPFILE::~item_BITMAPFILE()
{
}



HRESULT item_BITMAPFILE::getItemName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	*str = agaliaString::create(L"BITMAPFILEHEADER");
	return S_OK;
}



HRESULT item_BITMAPFILE::getItemPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}



HRESULT item_BITMAPFILE::getItemPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_bfType: name = L"bfType"; break;
	case prop_bfSize: name = L"bfSize"; break;
	case prop_bfReserved1: name = L"bfReserved1"; break;
	case prop_bfReserved2: name = L"bfReserved2"; break;
	case prop_bfOffBits: name = L"bfOffBits"; break;
	default:
		return __super::getItemPropName(index, str);
	};

	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT item_BITMAPFILE::getItemPropValue(uint32_t index, agaliaString** str) const
{
	BITMAPFILEHEADER bfh = {};
	std::wstringstream temp;

	if (index == prop_bfType)
	{
		auto hr = image->ReadData(&bfh.bfType, getOffset() + offsetof(BITMAPFILEHEADER, bfType), sizeof(bfh.bfType));
		if (FAILED(hr)) return hr;
		hr = multibyte_to_widechar(reinterpret_cast<char*>(&bfh.bfType), sizeof(bfh.bfType), CP_ACP, temp);
		if (FAILED(hr)) return hr;
	}
	else if (index == prop_bfSize)
	{
		auto hr = image->ReadData(&bfh.bfSize, getOffset() + offsetof(BITMAPFILEHEADER, bfSize), sizeof(bfh.bfSize));
		if (FAILED(hr)) return hr;
		format_dec(temp, bfh.bfSize);
	}
	else if (index == prop_bfReserved1)
	{
		auto hr = image->ReadData(&bfh.bfReserved1, getOffset() + offsetof(BITMAPFILEHEADER, bfReserved1), sizeof(bfh.bfReserved1));
		if (FAILED(hr)) return hr;
		format_dec(temp, bfh.bfReserved1);
	}
	else if (index == prop_bfReserved2)
	{
		auto hr = image->ReadData(&bfh.bfReserved2, getOffset() + offsetof(BITMAPFILEHEADER, bfReserved2), sizeof(bfh.bfReserved2));
		if (FAILED(hr)) return hr;
		format_dec(temp, bfh.bfReserved2);
	}
	else if (index == prop_bfOffBits)
	{
		auto hr = image->ReadData(&bfh.bfOffBits, getOffset() + offsetof(BITMAPFILEHEADER, bfOffBits), sizeof(bfh.bfOffBits));
		if (FAILED(hr)) return hr;
		format_dec(temp, bfh.bfOffBits);
	}
	else
	{
		return __super::getItemPropValue(index, str);
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}



HRESULT item_BITMAPFILE::getNextItem(agaliaItem** next) const
{
	if (next == nullptr) return E_POINTER;

	BITMAPFILEHEADER bfh = {};
	auto hr = image->ReadData(&bfh.bfOffBits, getOffset() + offsetof(BITMAPFILEHEADER, bfOffBits), sizeof(bfh.bfOffBits));
	if (FAILED(hr)) return hr;

	DWORD biSize = 0;
	hr = image->ReadData(&biSize, getOffset() + getSize(), sizeof(biSize));
	if (FAILED(hr)) return hr;

	if (sizeof(BITMAPCOREHEADER) == biSize)
	{
		auto p = new item_BITMAPCORE(image, getOffset() + getSize(), biSize);
		p->bfOffBits = getOffset() + bfh.bfOffBits;
		*next = p;
		return S_OK;

	}
	else if (sizeof(BITMAPINFOHEADER) <= biSize)
	{
		auto p = new item_BITMAPV5(image, getOffset() + getSize(), biSize);
		p->bfOffBits = getOffset() + bfh.bfOffBits;
		*next = p;
		return S_OK;
	}

	return E_FAIL;
}



HRESULT item_BITMAPFILE::getColumnValue(uint32_t column, agaliaString** str) const
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
		BITMAPFILEHEADER bfh = {};
		auto hr = image->ReadData(&bfh.bfType, getOffset() + offsetof(BITMAPFILEHEADER, bfType), sizeof(bfh.bfType));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"bfType : ";
		hr = multibyte_to_widechar(reinterpret_cast<char*>(&bfh.bfType), sizeof(bfh.bfType), CP_ACP, temp);
		if (FAILED(hr)) return hr;

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	return E_INVALIDARG;
}



HRESULT item_BITMAPFILE::getAdditionalInfoCount(uint32_t* row) const
{
	*row = addinf_last;
	return S_OK;
}



HRESULT item_BITMAPFILE::getAdditionalInfoValue(uint32_t row, agaliaString** str) const
{
	BITMAPFILEHEADER bfh = {};
	std::wstringstream temp;

	if (row == addinf_bfSize)
	{
		auto hr = image->ReadData(&bfh.bfSize, getOffset() + offsetof(BITMAPFILEHEADER, bfSize), sizeof(bfh.bfSize));
		if (FAILED(hr)) return hr;
		temp << L"bfSize : ";
		format_dec(temp, bfh.bfSize);
	}
	else if (row == addinf_bfReserved1)
	{
		auto hr = image->ReadData(&bfh.bfReserved1, getOffset() + offsetof(BITMAPFILEHEADER, bfReserved1), sizeof(bfh.bfReserved1));
		if (FAILED(hr)) return hr;
		temp << L"bfReserved1 : ";
		format_dec(temp, bfh.bfReserved1);
	}
	else if (row == addinf_bfReserved2)
	{
		auto hr = image->ReadData(&bfh.bfReserved2, getOffset() + offsetof(BITMAPFILEHEADER, bfReserved2), sizeof(bfh.bfReserved2));
		if (FAILED(hr)) return hr;
		temp << L"bfReserved2 : ";
		format_dec(temp, bfh.bfReserved2);
	}
	else if (row == addinf_bfOffBits)
	{
		auto hr = image->ReadData(&bfh.bfOffBits, getOffset() + offsetof(BITMAPFILEHEADER, bfOffBits), sizeof(bfh.bfOffBits));
		if (FAILED(hr)) return hr;
		temp << L"bfOffBits : ";
		format_dec(temp, bfh.bfOffBits);
	}
	else
	{
		return E_INVALIDARG;
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
