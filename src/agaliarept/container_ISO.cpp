#include "pch.h"
#include "container_ISO.h"

#include "agaliareptImpl.h"
#include "byteswap.h"

#include "analyze_ISO_item_Box.h"
#include "analyze_ISO_util.h"
#include "thumbnail.h"

using namespace analyze_ISO;

container_ISO::container_ISO(const wchar_t* path, IStream* stream)
	: _agaliaContainerBase(path, stream)
{

}

container_ISO::~container_ISO()
{

}

HRESULT container_ISO::getColumnCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = column_last;
	return S_OK;
}

HRESULT container_ISO::getColumnWidth(uint32_t column, int32_t* length) const
{
	if (length == nullptr) return E_POINTER;

	switch (column)
	{
	case column_offset: *length = 8; break;
	case column_parent: *length = 8; break;
	case column_type: *length = 16; break;
	case column_size: *length = -8; break;
	case column_value: *length = 8; break;
	default:
		return E_INVALIDARG;
	}

	return S_OK;
}

HRESULT container_ISO::getColumnName(uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (column)
	{
	case column_offset: name = L"offset"; break;
	case column_parent: name = L"parent"; break;
	case column_type: name = L"type"; break;
	case column_size: name = L"size"; break;
	case column_value: name = L"value"; break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT container_ISO::getElementInfoCount(const agaliaElement* item, uint32_t* row) const
{
	if (item == nullptr) return E_POINTER;
	if (row == nullptr) return E_POINTER;

	if (item->getGUID() != ISO_item_Box::guid_iso)
		return E_FAIL;

	*row = 1;
	return S_OK;
}



HRESULT container_ISO::getElementInfoValue(const agaliaElement* item, uint32_t row, uint32_t column, agaliaString** str) const
{
	if (item == nullptr) return E_POINTER;
	if (str == nullptr) return E_POINTER;

	if (item->getGUID() != ISO_item_Box::guid_iso)
		return E_FAIL;

	if (row != 0) return E_INVALIDARG;

	return static_cast<const ISO_item_Box*>(item)->getColumnValue(column, str);
}



HRESULT container_ISO::getRootElement(agaliaElement** root) const
{
	if (root == nullptr) return E_POINTER;

	ULARGE_INTEGER endpos = {};
	LockStream();
	auto hr = data_stream->Seek(uint64_to_li(0), STREAM_SEEK_END, &endpos);
	UnlockStream();
	if (FAILED(hr)) return hr;

	auto p = createItem(this, 0, endpos.QuadPart, 0);
	if (p)
	{
		*root = p;
		return S_OK;
	}
	return E_FAIL;
}


HRESULT container_ISO::getPropertyValue(PropertyType type, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	if (type == ContainerType)
	{
		return GetContainerName(str);
	}
	return E_FAIL;
}


HRESULT container_ISO::loadBitmap(agaliaBitmap** ppBitmap) const
{
	return ::loadBitmap(ppBitmap, this);
}


HRESULT container_ISO::loadThumbnail(agaliaBitmap** ppBitmap, uint32_t maxW, uint32_t maxH) const
{
	return ::loadThumbnail(ppBitmap, this, maxW, maxH);
}


_agaliaContainerBase* container_ISO::CreateInstance(const wchar_t* path, IStream* stream)
{
	return new container_ISO(path, stream);
}

bool container_ISO::IsSupported(IStream* stream)
{
	if (!stream) return false;

	if (FAILED(stream->Seek(uint64_to_li(0), STREAM_SEEK_SET, nullptr)))
		return false;

	Box box = {};
	ULONG cbRead = 0;
	auto hr = stream->Read(&box, sizeof(box), &cbRead);
	if (hr != S_OK || cbRead != sizeof(box))
		return false;

	switch (agalia_byteswap(box.type))
	{
	case 'ftyp':
	case 'moov':
	case 'mdat':
	case 'free':
	case 'skip':
	case 'wide':
	case 'pnot':
	case 'jP  ':
		return true;
	}
	return false;
}



HRESULT container_ISO::GetContainerName(agaliaString** name)
{
	if (name == nullptr) return E_POINTER;
	*name = agaliaString::create(L"ISO based media format");
	return S_OK;
}
