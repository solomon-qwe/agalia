#include "pch.h"
#include "analyze_TIFF_item_Header.h"

#include "container_TIFF.h"
#include "analyze_TIFF_item_IFD.h"

#include "tiff_common.h"

using namespace analyze_TIFF;



item_TIFFHeader::item_TIFFHeader(const container_TIFF* image, uint64_t offset, uint64_t size)
	: item_tiff_Base(image, offset, size)
{
}



item_TIFFHeader::~item_TIFFHeader()
{
}



HRESULT item_TIFFHeader::getItemName(agaliaString** str) const
{
	*str = agaliaString::create(L"TIFF Header");
	return S_OK;
}



HRESULT item_TIFFHeader::getItemPropCount(uint32_t* count) const
{
	*count = prop_last;
	return S_OK;
}



HRESULT item_TIFFHeader::getItemPropName(uint32_t index, agaliaString** str) const
{
	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_order:	name = L"order";	break;
	case prop_version:	name = L"version";	break;
	case prop_offset:	name = L"offset";	break;
	default:
		return E_FAIL;
	}

	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT item_TIFFHeader::getItemPropValue(uint32_t index, agaliaString** str) const
{
	CHeapPtr<TIFFHEADER> tiff;
	if (!tiff.AllocateBytes(sizeof(TIFFHEADER))) return E_OUTOFMEMORY;
	auto hr = image->ReadData(tiff, 0, sizeof(TIFFHEADER));
	if (FAILED(hr)) return hr;

	std::wstringstream temp;
	switch (index)
	{
	case prop_order:	format_asc(temp, tiff->get_order());	break;
	case prop_version:	format_hex(temp, tiff->get_version());	break;
	case prop_offset:	format_dec(temp, tiff->get_offset());	break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}



HRESULT item_TIFFHeader::getChildItem(uint32_t, agaliaItem**) const
{
	return E_FAIL;
}



HRESULT item_TIFFHeader::getNextItem(agaliaItem** next) const
{
	CHeapPtr<TIFFHEADER> tiff;
	if (!tiff.AllocateBytes(sizeof(TIFFHEADER))) return E_POINTER;
	auto hr = image->ReadData(tiff, getOffset(), sizeof(TIFFHEADER));
	if (FAILED(hr)) return hr;

	uint64_t next_data_offset = tiff->get_offset();

	decltype(TIFFIFD::count) ifd_count = 0;
	hr = image->ReadData(&ifd_count, next_data_offset, sizeof(ifd_count));
	if (FAILED(hr)) return hr;

	uint64_t next_data_size =
		static_cast<uint64_t>(sizeof(TIFFIFD::count)) +
		static_cast<uint64_t>(sizeof(TIFFIFD::entry)) * get_tiff_value(image->byte_order, ifd_count) +
		sizeof(TIFFIFD::next_offset_type);

	*next = new item_IFD<TIFFIFD>(image, next_data_offset, next_data_size, 0);
	return S_OK;
}
