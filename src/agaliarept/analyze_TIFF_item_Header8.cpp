#include "pch.h"
#include "analyze_TIFF_item_Header8.h"

#include "container_TIFF.h"
#include "analyze_TIFF_item_IFD.h"

#include "tiff_common.h"

using namespace analyze_TIFF;



item_TIFFHeader8::item_TIFFHeader8(const container_TIFF* image, uint64_t offset, uint64_t size)
	: TIFF_item_Base(image, offset, size)
{
}



item_TIFFHeader8::~item_TIFFHeader8()
{
}



HRESULT item_TIFFHeader8::getName(agaliaString** str) const
{
	*str = agaliaString::create(L"BigTIFF Header");
	return S_OK;
}



HRESULT item_TIFFHeader8::getPropCount(uint32_t* count) const
{
	*count = prop_last;
	return S_OK;
}



HRESULT item_TIFFHeader8::getPropName(uint32_t index, agaliaString** str) const
{
	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_order:		name = L"order"; break;
	case prop_version:		name = L"version"; break;
	case prop_offset_size:	name = L"offset size"; break;
	case prop_reserved:		name = L"reserved"; break;
	case prop_offset:		name = L"offset"; break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT item_TIFFHeader8::getPropValue(uint32_t index, agaliaString** str) const
{
	CHeapPtr<TIFFHEADER8> tiff;
	if (!tiff.AllocateBytes(sizeof(TIFFHEADER8))) return E_OUTOFMEMORY;
	auto hr = image->ReadData(tiff, 0, sizeof(TIFFHEADER8));
	if (FAILED(hr)) return hr;

	std::wstringstream temp;
	switch (index)
	{
	case prop_order:		format_asc(temp, tiff->get_order()); break;
	case prop_version:		format_hex(temp, tiff->get_version()); break;
	case prop_offset_size:	format_dec(temp, tiff->get_offset_size()); break;
	case prop_reserved:		format_hex(temp, tiff->get_reserved()); break;
	case prop_offset:		format_dec(temp, tiff->get_offset()); break;
	default:
		return E_FAIL;
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}



HRESULT item_TIFFHeader8::getChild(uint32_t, agaliaElement**) const
{
	return E_FAIL;
}



HRESULT item_TIFFHeader8::getNext(agaliaElement** next) const
{
	// BigTIFFヘッダを読み込む 
	CHeapPtr<TIFFHEADER8> tiff;
	if (!tiff.AllocateBytes(sizeof(TIFFHEADER8))) return E_POINTER;
	auto hr = image->ReadData(tiff, getOffset(), sizeof(TIFFHEADER8));
	if (FAILED(hr)) return hr;

	// オフセットサイズが8でなければ中断 
	if (tiff->get_offset_size() != 8) return E_FAIL;

	uint64_t next_data_offset = tiff->get_offset();

	decltype(TIFFIFD8::count) count = 0;
	hr = image->ReadData(&count, next_data_offset, sizeof(count));
	if (FAILED(hr)) return hr;

	uint64_t next_data_size =
		sizeof(TIFFIFD8::count) +
		sizeof(TIFFIFD8::entry) * get_tiff_value(image->byte_order, count) +
		sizeof(TIFFIFD8::next_offset_type);

	*next = new analyze_TIFF::item_IFD<TIFFIFD8>(image, next_data_offset, next_data_size, 0);
	return S_OK;
}
