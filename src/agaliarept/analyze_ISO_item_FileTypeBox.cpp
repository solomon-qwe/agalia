#include "pch.h"
#include "analyze_ISO_item_FileTypeBox.h"

using namespace analyze_ISO;

item_FileTypeBox::item_FileTypeBox(const agaliaContainer* image, uint64_t offset, uint64_t size, uint64_t endpos, uint32_t parent)
	:item_Box(image, offset, size, endpos, parent)
{

}

item_FileTypeBox::~item_FileTypeBox()
{

}

HRESULT item_FileTypeBox::getItemPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT item_FileTypeBox::getItemPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_major_brand: name = L"major_brand"; break;
	case prop_minor_version: name = L"minor_version"; break;
	case prop_compatible_brands: name = L"comatible_brands"; break;
	default:
		return __super::getItemPropName(index, str);
	}

	*str = agaliaString::create(name);
	return S_OK;
}

HRESULT item_FileTypeBox::getItemPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	std::wstringstream temp;
	if (index == prop_major_brand)
	{
		uint64_t box_data_offset = Box::getDataOffset(image, getOffset());

		FileTypeBox filetype = {};
		auto hr = image->ReadData(&filetype, getOffset() + box_data_offset, sizeof(filetype));
		if (FAILED(hr)) return hr;

		return multibyte_to_widechar(reinterpret_cast<char*>(&filetype.major_brand), sizeof(filetype.major_brand), CP_US_ASCII, str);
	}
	else if (index == prop_minor_version)
	{
		uint64_t box_data_offset = Box::getDataOffset(image, getOffset());

		FileTypeBox filetype = {};
		auto hr = image->ReadData(&filetype, getOffset() + box_data_offset, sizeof(filetype));
		if (FAILED(hr)) return hr;

		format_hex(temp, filetype.getMinorVersion());
	}
	else if (index == prop_compatible_brands)
	{
		uint64_t box_size = Box::getBoxSize(image, getOffset(), endpos);
		uint64_t box_data_offset = Box::getDataOffset(image, getOffset());

		uint64_t value_offset = box_data_offset + sizeof(FileTypeBox);
		uint64_t value_size = box_size - value_offset;

		CHeapPtr<uint32_t> buf;
		rsize_t bufsize = 0;
		auto hr = UInt64ToSizeT(getSize(), &bufsize);
		if (FAILED(hr)) return hr;
		if (!buf.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
		hr = image->ReadData(buf, getOffset() + value_offset, value_size);
		if (FAILED(hr)) return hr;

		for (uint64_t i = 0; i < value_size / sizeof(uint32_t); i++)
		{
			if (i != 0) temp << L", ";
			multibyte_to_widechar(reinterpret_cast<char*>(&buf[i]), sizeof(uint32_t), CP_US_ASCII, temp);
		}
	}
	else
	{
		return __super::getItemPropValue(index, str);
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
