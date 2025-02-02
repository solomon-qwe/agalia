#include "pch.h"
#include "container_PNG.h"

#include "agaliareptImpl.h"
#include "byteswap.h"

#include "analyze_PNG_util.h"
#include "analyze_PNG_item_Base.h"
#include "analyze_PNG_item_IHDR.h"
#include "thumbnail.h"

using namespace analyze_PNG;

container_PNG::container_PNG(const wchar_t* path, IStream* stream)
	: _agaliaContainerBase(path, stream)
{

}

container_PNG::~container_PNG()
{

}

HRESULT container_PNG::getColumnCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = column_last;
	return S_OK;
}

HRESULT container_PNG::getColumnWidth(uint32_t column, int32_t* length) const
{
	if (length == nullptr) return E_POINTER;

	switch (column)
	{
	case column_offset: *length = 8; break;
	case column_length: *length = 8; break;
	case column_chunk_type: *length = 8; break;
	case column_crc: *length = 8; break;
	case column_value: *length = 8; break;
	default:
		return E_INVALIDARG;
	}

	return S_OK;
}

HRESULT container_PNG::getColumnName(uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (column)
	{
	case column_offset: name = L"offset"; break;
	case column_length: name = L"Length"; break;
	case column_chunk_type: name = L"Chunk Type"; break;
	case column_crc: name = L"CRC"; break;
	case column_value: name = L"value"; break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT container_PNG::getElementInfoCount(const agaliaElement* item, uint32_t* row) const
{
	if (item == nullptr) return E_POINTER;
	if (row == nullptr) return E_POINTER;

	if (item->getGUID() != PNG_item_Base::guid_png)
		return E_FAIL;

	*row = 1;
	return S_OK;
}



HRESULT container_PNG::getElementInfoValue(const agaliaElement* item, uint32_t row, uint32_t column, agaliaString** str) const
{
	if (item == nullptr) return E_POINTER;
	if (str == nullptr) return E_POINTER;

	if (item->getGUID() != PNG_item_Base::guid_png)
		return E_FAIL;

	if (row != 0) return E_INVALIDARG;

	return static_cast<const PNG_item_Base*>(item)->getColumnValue(column, str);
}




HRESULT container_PNG::getRootElement(agaliaElement** root) const
{
	if (root == nullptr) return E_POINTER;

	ULARGE_INTEGER endpos = {};
	LockStream();
	auto hr = data_stream->Seek(uint64_to_li(0), STREAM_SEEK_END, &endpos);
	UnlockStream();
	if (FAILED(hr)) return hr;

	*root = createItem(this, 8, endpos.QuadPart);

	return S_OK;
}


HRESULT container_PNG::getPropertyValue(PropertyType type, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	if (type == ContainerType)
	{
		return GetContainerName(str);
	}
	else if (type == ImageWidth)
	{
		agaliaPtr<agaliaElement> root;
		auto hr = this->getRootElement(&root);
		if (FAILED(hr)) return hr;

		Chunk chunk = {};
		hr = ReadData(&chunk, root->getOffset(), sizeof(chunk));
		if (FAILED(hr)) return hr;

		if (chunk.getChunkType() != 'IHDR')
			return E_FAIL;

		IHDR ihdr = {};
		hr = ReadData(&ihdr, root->getOffset() + offsetof(Chunk, ChunkData), sizeof(ihdr));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		format_dec(temp, ihdr.getWidth());
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (type == ImageHeight)
	{
		agaliaPtr<agaliaElement> root;
		auto hr = this->getRootElement(&root);
		if (FAILED(hr)) return hr;

		Chunk chunk = {};
		hr = ReadData(&chunk, root->getOffset(), sizeof(chunk));
		if (FAILED(hr)) return hr;

		if (chunk.getChunkType() != 'IHDR')
			return E_FAIL;

		IHDR ihdr = {};
		hr = ReadData(&ihdr, root->getOffset() + offsetof(Chunk, ChunkData), sizeof(ihdr));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		format_dec(temp, ihdr.getHeight());
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	return E_FAIL;
}

HRESULT container_PNG::getThumbnailImage(HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH) const
{
	return loadThumbnailBitmap(phBitmap, maxW, maxH, data_stream);
}


_agaliaContainerBase* container_PNG::CreateInstance(const wchar_t* path, IStream* stream)
{
	return new container_PNG(path, stream);
}

bool container_PNG::IsSupported(IStream* stream)
{
	if (!stream) return false;

	if (FAILED(stream->Seek(uint64_to_li(0), STREAM_SEEK_SET, nullptr)))
		return false;

	const uint8_t PNGsignature[] = {137, 80, 78, 71, 13, 10, 26, 10};

	ULONG cbRead = 0;

	uint8_t buf[_countof(PNGsignature)] = {};
	auto hr = stream->Read(buf, sizeof(buf), &cbRead);
	if (hr != S_OK || cbRead != sizeof(buf))
		return false;

	if (memcmp(buf, PNGsignature, sizeof(PNGsignature)) == 0)
		return true;

	return false;
}



HRESULT container_PNG::GetContainerName(agaliaString** name)
{
	if (name == nullptr) return E_POINTER;
	*name = agaliaString::create(L"PNG");
	return S_OK;
}
