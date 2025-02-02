#include "pch.h"
#include "analyze_TIFF_item_Header.h"

#include "container_TIFF.h"
#include "analyze_TIFF_item_IFD.h"

#include "tiff_common.h"

using namespace analyze_TIFF;



item_TIFFHeader::item_TIFFHeader(const container_TIFF* image, uint64_t offset, uint64_t size)
	: TIFF_item_Base(image, offset, size)
{
}



item_TIFFHeader::~item_TIFFHeader()
{
}



HRESULT item_TIFFHeader::getName(agaliaString** str) const
{
	*str = agaliaString::create(L"TIFF Header");
	return S_OK;
}



HRESULT item_TIFFHeader::getPropCount(uint32_t* count) const
{
	*count = prop_last;
	return S_OK;
}



HRESULT item_TIFFHeader::getPropName(uint32_t index, agaliaString** str) const
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



HRESULT item_TIFFHeader::getPropValue(uint32_t index, agaliaString** str) const
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



HRESULT item_TIFFHeader::getChild(uint32_t, agaliaElement**) const
{
	return E_FAIL;
}



HRESULT item_TIFFHeader::getNext(agaliaElement** next) const
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
