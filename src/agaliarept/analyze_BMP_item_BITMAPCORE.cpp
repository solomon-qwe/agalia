#include "pch.h"
#include "analyze_BMP_item_BITMAPCORE.h"

#include "analyze_BMP_item_ColorIndexParent.h"

using namespace analyze_BMP;

item_BITMAPCORE::item_BITMAPCORE(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:BMP_item_Base(image, offset, size)
{
}

item_BITMAPCORE::~item_BITMAPCORE()
{
}

HRESULT item_BITMAPCORE::getName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	*str = agaliaString::create(L"BITMAPCOREHEADER");
	return S_OK;
}

HRESULT item_BITMAPCORE::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT item_BITMAPCORE::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_bcSize: name = L"bcSize"; break;
	case prop_bcWidth: name = L"bcWidth"; break;
	case prop_bcHeight: name = L"bcHeight"; break;
	case prop_bcPlanes: name = L"bcPlanes"; break;
	case prop_bcBitCount: name = L"bcBitCount"; break;
	default:
		return __super::getPropName(index, str);
	};

	*str = agaliaString::create(name);
	return S_OK;
}

HRESULT item_BITMAPCORE::getPropValue(uint32_t index, agaliaString** str) const
{
	BITMAPCOREHEADER bch = {};
	std::wstringstream temp;
	if (index == prop_bcSize)
	{
		auto hr = image->ReadData(&bch.bcSize, getOffset() + offsetof(BITMAPCOREHEADER, bcSize), sizeof(bch.bcSize));
		if (FAILED(hr)) return hr;
		format_dec(temp, bch.bcSize);
	}
	else if (index == prop_bcWidth)
	{
		auto hr = image->ReadData(&bch.bcWidth, getOffset() + offsetof(BITMAPCOREHEADER, bcWidth), sizeof(bch.bcWidth));
		if (FAILED(hr)) return hr;
		format_dec(temp, bch.bcWidth);
	}
	else if (index == prop_bcHeight)
	{
		auto hr = image->ReadData(&bch.bcHeight, getOffset() + offsetof(BITMAPCOREHEADER, bcHeight), sizeof(bch.bcHeight));
		if (FAILED(hr)) return hr;
		format_dec(temp, bch.bcHeight);
	}
	else if (index == prop_bcPlanes)
	{
		auto hr = image->ReadData(&bch.bcPlanes, getOffset() + offsetof(BITMAPCOREHEADER, bcPlanes), sizeof(bch.bcPlanes));
		if (FAILED(hr)) return hr;
		format_dec(temp, bch.bcPlanes);
	}
	else if (index == prop_bcBitCount)
	{
		auto hr = image->ReadData(&bch.bcBitCount, getOffset() + offsetof(BITMAPCOREHEADER, bcBitCount), sizeof(bch.bcBitCount));
		if (FAILED(hr)) return hr;
		format_dec(temp, bch.bcBitCount);
	}
	else
	{
		return __super::getPropValue(index, str);
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}


HRESULT item_BITMAPCORE::getNext(agaliaElement** next) const
{
	BITMAPCOREHEADER bch = {};
	auto hr = image->ReadData(&bch, getOffset(), sizeof(bch));
	if (FAILED(hr)) return hr;

	LONG scans = abs(bch.bcHeight);
	LONG bytes_of_line = (bch.bcBitCount * bch.bcWidth / 8 + 3) & ~3;
	LONG size_image = bytes_of_line * scans;

	auto p = new item_ColorIndexParent(image, bfOffBits, size_image);
	p->bfOffBits = bfOffBits;
	p->scans = scans;
	p->bytes_of_line = bytes_of_line;
	p->biSizeImage = size_image;
	*next = p;
	return S_OK;
}

HRESULT item_BITMAPCORE::getColumnValue(uint32_t column, agaliaString** str) const
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
		return getName(str);
	}
	else if (column == column_value)
	{
		BITMAPCOREHEADER bch = {};
		auto hr = image->ReadData(&bch.bcSize, getOffset() + offsetof(BITMAPCOREHEADER, bcSize), sizeof(bch.bcSize));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"bcSize : " << bch.bcSize;
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	return E_NOTIMPL;
}

HRESULT item_BITMAPCORE::getAdditionalInfoCount(uint32_t* row) const
{
	*row = addinf_last;
	return S_OK;
}

HRESULT item_BITMAPCORE::getAdditionalInfoValue(uint32_t row, agaliaString** str) const
{
	BITMAPCOREHEADER bch = {};
	std::wstringstream temp;
	if (row == addinf_bcWidth)
	{
		auto hr = image->ReadData(&bch.bcWidth, getOffset() + offsetof(BITMAPCOREHEADER, bcWidth), sizeof(bch.bcWidth));
		if (FAILED(hr)) return hr;

		temp << L"bcWidth : " << bch.bcWidth;
	}
	else if (row == addinf_bcHeight)
	{
		auto hr = image->ReadData(&bch.bcHeight, getOffset() + offsetof(BITMAPCOREHEADER, bcHeight), sizeof(bch.bcHeight));
		if (FAILED(hr)) return hr;

		temp << L"bcHeight : " << bch.bcHeight;
	}
	else if (row == addinf_bcPlanes)
	{
		auto hr = image->ReadData(&bch.bcPlanes, getOffset() + offsetof(BITMAPCOREHEADER, bcPlanes), sizeof(bch.bcPlanes));
		if (FAILED(hr)) return hr;

		temp << L"bcPlanes : " << bch.bcPlanes;
	}
	else if (row == addinf_bcBitCount)
	{
		auto hr = image->ReadData(&bch.bcBitCount, getOffset() + offsetof(BITMAPCOREHEADER, bcBitCount), sizeof(bch.bcBitCount));
		if (FAILED(hr)) return hr;

		temp << L"bcBitCount : " << bch.bcBitCount;
	}
	else
	{
		return E_INVALIDARG;
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
