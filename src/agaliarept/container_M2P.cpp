#include "pch.h"
#include "container_M2P.h"

#include "agaliareptImpl.h"
#include "analyze_M2P_item_ProgramStream.h"
#include "thumbnail.h"

using namespace analyze_M2P;



container_M2P::container_M2P(const wchar_t* path, IStream* stream)
	: _agaliaContainerBase(path, stream)
{
}



container_M2P::~container_M2P()
{
}



HRESULT container_M2P::getColumnCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = column_last;
	return S_OK;
}



HRESULT container_M2P::getColumnWidth(uint32_t column, int32_t* length) const
{
	if (length == nullptr) return E_POINTER;
	switch (column)
	{
	case column_offset: *length = 8; break;
	case column_field: *length = 16; break;
	case column_bits: *length = -4; break;
	case column_value: *length = 8; break;
	default:
		return E_INVALIDARG;
	}
	return S_OK;
}



HRESULT container_M2P::getColumnName(uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (column)
	{
	case column_offset: name = L"offset"; break;
	case column_field: name = L"field"; break;
	case column_bits: name = L"bits"; break;
	case column_value: name = L"value"; break;
	default:
		return E_INVALIDARG;
	}
	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT container_M2P::getElementInfoCount(const agaliaElement* item, uint32_t* row) const
{
	if (item == nullptr) return E_POINTER;
	if (item->getGUID() == M2P_item_Base::guid_m2p)
		return static_cast<const M2P_item_Base*>(item)->getElementInfoCount(row);
	return E_FAIL;
}



HRESULT container_M2P::getElementInfoValue(const agaliaElement* item, uint32_t row, uint32_t column, agaliaString** str) const
{
	if (item == nullptr) return E_POINTER;
	if (item->getGUID() == M2P_item_Base::guid_m2p)
		return static_cast<const M2P_item_Base*>(item)->getElementInfoValue(row, column, str);
	return E_FAIL;
}



HRESULT container_M2P::getRootElement(agaliaElement** root) const
{
	if (root == nullptr) return E_POINTER;
	auto p = new item_ProgramStream(this, 0, 0);
	*root = p;
	return S_OK;
}



HRESULT container_M2P::getPropertyValue(PropertyType type, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	if (type == FormatType)
	{
		return GetContainerName(str);
	}
	return E_FAIL;
}



HRESULT container_M2P::getThumbnailImage(HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH) const
{
	return loadThumbnailBitmap(phBitmap, maxW, maxH, data_stream);
}



_agaliaContainerBase* container_M2P::CreateInstance(const wchar_t* path, IStream* stream)
{
	return new container_M2P(path, stream);
}



bool container_M2P::IsSupported(IStream* stream)
{
	if (!stream) return false;

	if (FAILED(stream->Seek(uint64_to_li(0), STREAM_SEEK_SET, nullptr)))
		return false;

	ULONG cbRead = 0;
	uint32_t pack_start_code = 0;
	auto hr = stream->Read(&pack_start_code, sizeof(pack_start_code), &cbRead);
	if (hr != S_OK || cbRead != sizeof(pack_start_code))
		return false;

	if (_byteswap_ulong(pack_start_code) == 0x000001BA)
	{
		return true;
	}
	return false;
}



HRESULT container_M2P::GetContainerName(agaliaString** name)
{
	if (name == nullptr) return E_POINTER;
	*name = agaliaString::create(L"MPEG-2");
	return S_OK;
}
