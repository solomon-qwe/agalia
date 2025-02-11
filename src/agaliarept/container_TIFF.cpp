#include "pch.h"
#include "container_TIFF.h"

#include "analyze_TIFF_item_Header.h"
#include "analyze_TIFF_item_Header8.h"

#include "tiff_common.h"
#include "thumbnail.h"

using namespace analyze_TIFF;



container_TIFF::container_TIFF(const wchar_t* path, IStream* stream)
	:_agaliaContainerBase(path, stream)
{
	ReadData(&byte_order, 0, sizeof(byte_order));

	init_tif_dictionary(agalia_pref_dic_lang == Japanese ? L"jpn" : L"enu");
}



container_TIFF::~container_TIFF()
{
	clear_tif_dictionary();
}



HRESULT container_TIFF::ReadData(void* buf, uint64_t pos, uint64_t size) const
{
	return __super::ReadData(buf, pos + (is_exif ? 6 : 0), size);
}



HRESULT container_TIFF::getColumnCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = column_last;
	return S_OK;
}



HRESULT container_TIFF::getColumnWidth(uint32_t column, int32_t* length) const
{
	if (length == nullptr) return E_POINTER;

	switch (column)
	{
	case column_offset:			*length = 8;	break;
	case column_ifd:			*length = 8;	break;
	case column_tag:			*length = 4;	break;
	case column_name:			*length = 20;	break;
	case column_type:			*length = 8;	break;
	case column_count:			*length = -6;	break;
	case column_value_offset:	*length = 12;	break;
	case column_value:			*length = 0;	break;
	default:
		return E_FAIL;
	}

	return S_OK;
}



HRESULT container_TIFF::getColumnName(uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (column)
	{
	case column_offset:			name = L"offset"; break;
	case column_ifd:			name = L"IFD"; break;
	case column_tag:			name = L"Tag"; break;
	case column_name:			name = L"Name"; break;
	case column_type:			name = L"Type"; break;
	case column_count:			name = L"Count"; break;
	case column_value_offset:	name = L"Value Offset"; break;
	case column_value:			name = L"value"; break;
	default:
		return E_FAIL;
	}

	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT container_TIFF::getElementInfoCount(const agaliaElement* item, uint32_t* row) const
{
	if (item == nullptr) return E_POINTER;
	if (row == nullptr) return E_POINTER;

	if (item->getGUID() != TIFF_item_Base::guid_tiff)
		return E_FAIL;

	*row = 1;
	return S_OK;
}



HRESULT container_TIFF::getElementInfoValue(const agaliaElement* item, uint32_t row, uint32_t column, agaliaString** str) const
{
	if (item == nullptr) return E_POINTER;
	if (str == nullptr) return E_POINTER;

	if (item->getGUID() != TIFF_item_Base::guid_tiff)
		return E_FAIL;

	if (row != 0) return E_INVALIDARG;

	return static_cast<const TIFF_item_Base*>(item)->getColumnValue(column, str);
}



HRESULT container_TIFF::getRootElement(agaliaElement** root) const
{
	if (root == nullptr) return E_POINTER;

	TIFFHEADER header = {};
	auto hr = ReadData(&header, 0, sizeof(header));
	if (FAILED(hr)) return hr;

	switch (header.get_version())
	{
	case 0x002A:
	case 0x01BC:
	case 'OR':
		*root = new item_TIFFHeader(this, 0, sizeof(TIFFHEADER));
		return S_OK;

	case 0x002B:
		*root = new item_TIFFHeader8(this, 0, sizeof(TIFFHEADER8));
		return S_OK;
	}

	return E_FAIL;
}



HRESULT container_TIFF::getPropertyValue(PropertyType type, agaliaString** str) const
{
	if (type == ContainerType)
	{
		return GetContainerName(str);
	}
	return E_FAIL;
}



HRESULT container_TIFF::getThumbnailImage(HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH) const
{
	return loadThumbnailBitmap(phBitmap, maxW, maxH, data_stream);
}



bool container_TIFF::IsSupported(IStream* stream)
{
	if (stream == nullptr) return false;

	if (FAILED(stream->Seek(uint64_to_li(0), STREAM_SEEK_SET, nullptr)))
		return false;

	ULONG cbRead = 0;

	char exif_id[6] = {};
	auto hr = stream->Read(&exif_id, sizeof(exif_id), &cbRead);
	if (hr != S_OK || cbRead != sizeof(exif_id))
		return false;

	bool isExif = (memcmp(exif_id, "Exif\0\0", 6) == 0) ? true : false;
	if (!isExif) {
		if (FAILED(stream->Seek(uint64_to_li(0), STREAM_SEEK_SET, nullptr)))
			return false;
	}

	TIFFHEADER header = {};
	hr = stream->Read(&header, sizeof(header), &cbRead);
	if (hr != S_OK || cbRead != sizeof(header))
		return false;

	switch (header.get_order())
	{
	case 'II':
	case 'MM':
		break;
	default:
		return false;
	}

	switch (header.get_version())
	{
	case 0x002A:	// TIFF 
	case 0x002B:	// BigTIFF 
	case 0x01BC:	// JPEG XR 
	case 'OR':		// OLYMPUS RAW 
		break;
	default:
		return false;
	}

	return true;
}



_agaliaContainerBase* container_TIFF::CreateInstance(const wchar_t* path, IStream* stream)
{
	if (FAILED(stream->Seek(uint64_to_li(0), STREAM_SEEK_SET, nullptr)))
		return nullptr;

	ULONG cbRead = 0;

	char exif_id[6] = {};
	auto hr = stream->Read(&exif_id, sizeof(exif_id), &cbRead);
	if (hr != S_OK || cbRead != sizeof(exif_id))
		return nullptr;

	if (FAILED(stream->Seek(uint64_to_li(0), STREAM_SEEK_SET, nullptr)))
		return nullptr;

	auto p = new container_TIFF(path, stream);
	p->is_exif = (memcmp(exif_id, "Exif\0\0", 6) == 0) ? true : false;
	return p;
}



HRESULT container_TIFF::GetContainerName(agaliaString** name)
{
	if (name == nullptr) return E_POINTER;
	*name = agaliaString::create(L"TIFF");
	return S_OK;
}
