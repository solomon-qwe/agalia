#include "pch.h"
#include "container_ASF.h"

#include "asf_def.h"
#include "agaliareptImpl.h"
#include "analyze_ASF_util.h"
#include "analyze_ASF_item_HeaderObject.h"
#include "thumbnail.h"

using namespace analyze_ASF;

container_ASF::container_ASF(const wchar_t* path, IStream* stream)
	: _agaliaContainerBase(path, stream)
{
}

container_ASF::~container_ASF()
{
}


HRESULT container_ASF::getColumnCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = column_last;
	return S_OK;
}

HRESULT container_ASF::getColumnWidth(uint32_t column, int32_t* length) const
{
	if (length == nullptr) return E_POINTER;
	switch (column)
	{
	case column_offset: *length = 8; break;
	case column_parent: *length = 8; break;
	case column_objectID: *length = 24; break;
	case column_size: *length = -8; break;
	case column_value: *length = 8; break;
	default:
		return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT container_ASF::getColumnName(uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (column)
	{
	case column_offset: name = L"offset"; break;
	case column_parent: name = L"parent"; break;
	case column_objectID: name = L"objectID"; break;
	case column_size: name = L"size"; break;
	case column_value: name = L"value"; break;
	default:
		return E_INVALIDARG;
	}
	*str = agaliaString::create(name);
	return S_OK;
}

HRESULT container_ASF::getElementInfoCount(const agaliaElement* item, uint32_t* row) const
{
	if (item == nullptr) return E_POINTER;
	if (item->getGUID() == ASF_item_Base::guid_asf)
		return static_cast<const ASF_item_Base*>(item)->getElementInfoCount(row);
	return E_FAIL;
}



HRESULT container_ASF::getElementInfoValue(const agaliaElement* item, uint32_t row, uint32_t column, agaliaString** str) const
{
	if (item == nullptr) return E_POINTER;
	if (item->getGUID() == ASF_item_Base::guid_asf)
		return static_cast<const ASF_item_Base*>(item)->getElementInfoValue(row, column, str);
	return E_FAIL;
}



HRESULT container_ASF::getRootElement(agaliaElement** root) const
{
	if (root == nullptr) return E_POINTER;

	ULARGE_INTEGER endpos = {};
	LockStream();
	auto hr = data_stream->Seek(uint64_to_li(0), STREAM_SEEK_END, &endpos);
	UnlockStream();
	if (FAILED(hr)) return hr;

	auto p = createItem(this, 0, endpos.QuadPart);
	if (p)
	{
		*root = p;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT container_ASF::getPropertyValue(PropertyType type, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	if (type == ContainerType)
	{
		return GetContainerName(str);
	}
	return E_FAIL;
}

HRESULT container_ASF::getThumbnailImage(HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH) const
{
	return loadThumbnailBitmap(phBitmap, maxW, maxH, data_stream);
}

_agaliaContainerBase* container_ASF::CreateInstance(const wchar_t* path, IStream* stream)
{
	return new container_ASF(path, stream);
}

bool container_ASF::IsSupported(IStream* stream)
{
	if (!stream) return false;

	if (FAILED(stream->Seek(uint64_to_li(0), STREAM_SEEK_SET, nullptr)))
		return false;

	ULONG cbRead = 0;
	GUID ObjectID = {};
	auto hr = stream->Read(&ObjectID, sizeof(ObjectID), &cbRead);
	if (hr != S_OK || cbRead != sizeof(ObjectID))
		return false;

	return IsEqualGUID(ObjectID, ASF_Header_Object);
}



HRESULT container_ASF::GetContainerName(agaliaString** name)
{
	if (name == nullptr) return E_POINTER;
	*name = agaliaString::create(L"ASF");
	return S_OK;
}
