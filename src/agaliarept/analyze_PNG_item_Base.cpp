#include "pch.h"
#include "analyze_PNG_item_Base.h"

#include "analyze_PNG_util.h"
#include "container_PNG.h"

using namespace analyze_PNG;

// {C9D40159-E5DA-40B2-96B6-99C29B99B71F}
const GUID item_Base::guid_png =
{ 0xc9d40159, 0xe5da, 0x40b2, { 0x96, 0xb6, 0x99, 0xc2, 0x9b, 0x99, 0xb7, 0x1f } };

item_Base::item_Base(const container_PNG* image, uint64_t offset, uint64_t size, uint64_t endpos)
	: _agaliaItemBase(guid_png, offset, size)
{
	this->image = image;
	this->endpos = endpos;
}

item_Base::~item_Base()
{
}

HRESULT item_Base::getItemName(agaliaString** str) const
{
	char name[sizeof(Chunk::ChunkType) + 1] = {};
	auto hr = image->ReadData(name, getOffset() + offsetof(Chunk, ChunkType), sizeof(Chunk::ChunkType));
	if (FAILED(hr)) return hr;

	return multibyte_to_widechar(name, sizeof(name), CP_US_ASCII, str);
}

HRESULT item_Base::getItemPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT item_Base::getItemPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_offset: name = L"offset"; break;
	case prop_length: name = L"Length"; break;
	case prop_chunk_type: name = L"Chunk Type"; break;
	case prop_crc: name = L"CRC"; break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(name);
	return S_OK;
}


HRESULT item_Base::getItemPropValue(uint32_t index, agaliaString** str) const
{
	if (index == prop_offset)
	{
		std::wstringstream temp;
		format_dec(temp, getOffset());
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_length)
	{
		uint32_t length = 0;
		auto hr = image->ReadData(&length, getOffset(), sizeof(length));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		format_dec(temp, agalia_byteswap(length));
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_chunk_type)
	{
		return getItemName(str);
	}
	else if (index == prop_crc)
	{
		uint32_t length = 0;
		auto hr = image->ReadData(&length, getOffset(), sizeof(length));
		if (FAILED(hr)) return hr;

		uint64_t pos = getOffset() + offsetof(Chunk, ChunkData) + agalia_byteswap(length);

		uint32_t crc = 0;
		hr = image->ReadData(&crc, pos, sizeof(crc));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"0x";
		format_hex(temp, agalia_byteswap(crc));
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}

	return E_FAIL;
}

HRESULT item_Base::getChildItem(uint32_t sibling, agaliaItem** child) const
{
	UNREFERENCED_PARAMETER(sibling);
	UNREFERENCED_PARAMETER(child);
	return E_FAIL;
}

HRESULT item_Base::getNextItem(agaliaItem** next) const
{
	uint32_t length = 0;
	auto hr = image->ReadData(&length, getOffset(), sizeof(length));
	if (FAILED(hr)) return hr;

	uint64_t chunk_size = offsetof(Chunk, ChunkData) + agalia_byteswap(length) + sizeof(uint32_t);

	agaliaItem* p = createItem(image, getOffset() + chunk_size, endpos);
	if (p)
	{
		*next = p;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT item_Base::getAsocImage(const agaliaContainer** imageAsoc) const
{
	*imageAsoc = this->image;
	return S_OK;
}

HRESULT item_Base::getValueAreaOffset(uint64_t* offset) const
{
	if (offset == nullptr) return E_POINTER;
	*offset = getOffset() + offsetof(Chunk, ChunkData);
	return S_OK;
}

HRESULT item_Base::getValueAreaSize(uint64_t* size) const
{
	if (size == nullptr) return E_POINTER;
	*size = getSize() - offsetof(Chunk, ChunkData) - sizeof(uint32_t);
	return S_OK;
}

HRESULT item_Base::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	if (column == column_offset)
	{
		return getItemPropValue(prop_offset, str);
	}
	else if (column == column_length)
	{
		return getItemPropValue(prop_length, str);
	}
	else if (column == column_chunk_type)
	{
		return getItemPropValue(prop_chunk_type, str);
	}
	else if (column == column_crc)
	{
		return getItemPropValue(prop_crc, str);
	}

	return E_INVALIDARG;
}
