#include "pch.h"
#include "container_BMP.h"

#include "agaliareptImpl.h"
#include "analyze_BMP_item_BITMAPFILE.h"

using namespace analyze_BMP;

container_BMP::container_BMP(const wchar_t* path, IStream* stream)
	: _agaliaContainerBase(path, stream)
{
}

container_BMP::~container_BMP()
{
}


HRESULT container_BMP::getColumnCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = column_last;
	return S_OK;
}

HRESULT container_BMP::getColumnWidth(uint32_t column, int32_t* length) const
{
	if (length == nullptr) return E_POINTER;
	switch (column)
	{
	case column_offset: *length = 8; break;
	case column_structure: *length = 16; break;
	case column_value: *length = 8; break;
	default:
		return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT container_BMP::getColumnName(uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (column)
	{
	case column_offset: name = L"offset"; break;
	case column_structure: name = L"structure"; break;
	case column_value: name = L"value"; break;
	default:
		return E_INVALIDARG;
	}
	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT container_BMP::getGridRowCount(const agaliaItem* item, uint32_t* row) const
{
	if (item == nullptr) return E_POINTER;
	if (row == nullptr) return E_POINTER;

	if (item->getGUID() != item_Base::guid_bmp)
		return E_FAIL;

	*row = 1;
	auto hr = static_cast<const item_Base*>(item)->getAdditionalInfoCount(row);
	if (SUCCEEDED(hr))
		(*row)++;
	return S_OK;
}



HRESULT container_BMP::getGridValue(const agaliaItem* item, uint32_t row, uint32_t column, agaliaString** str) const
{
	if (item == nullptr) return E_POINTER;
	if (str == nullptr) return E_POINTER;

	if (item->getGUID() != item_Base::guid_bmp)
		return E_FAIL;

	if (row == 0) {
		return static_cast<const item_Base*>(item)->getColumnValue(column, str);
	}
	else if (column == 0) {
		*str = agaliaString::create(L"");
		return S_OK;
	}
	else if (column == column_value) {
		return static_cast<const item_Base*>(item)->getAdditionalInfoValue(row - 1, str);
	}
	return E_FAIL;
}



HRESULT container_BMP::getRootItem(agaliaItem** root) const
{
	if (root == nullptr) return E_POINTER;
	auto p = new item_BITMAPFILE(this, 0, sizeof(BITMAPFILEHEADER));
	*root = p;
	return S_OK;
}


HRESULT container_BMP::getPropertyValue(PropertyType type, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	if (type == ContainerType)
	{
		return GetContainerName(str);
	}
	return E_FAIL;
}

HRESULT container_BMP::getThumbnailImage(HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH) const
{
	HRESULT loadThumbnailImageGDIP(IStream * stream, uint32_t maxW, uint32_t maxH, HBITMAP * phBitmap);

	return loadThumbnailImageGDIP(data_stream, maxW, maxH, phBitmap);
}


bool container_BMP::IsSupported(IStream* stream)
{
	if (!stream) return false;

	if (FAILED(stream->Seek(uint64_to_li(0), STREAM_SEEK_SET, nullptr)))
		return false;

	ULONG cbRead = 0;
	BITMAPFILEHEADER bmp = {};
	auto hr = stream->Read(&bmp.bfType, sizeof(bmp.bfType), &cbRead);
	if (hr != S_OK || cbRead != sizeof(bmp.bfType))
		return false;

	char t[] = { 'B', 'M' };
	return (bmp.bfType == *reinterpret_cast<const uint16_t*>(t));
}

_agaliaContainerBase* container_BMP::CreateInstance(const wchar_t* path, IStream* stream)
{
	return new container_BMP(path, stream);
}



HRESULT container_BMP::GetContainerName(agaliaString** name)
{
	if (name == nullptr) return E_POINTER;
	*name = agaliaString::create(L"Bitmap");
	return S_OK;
}
