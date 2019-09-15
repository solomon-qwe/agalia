#include "pch.h"
#include "analyze_RIFF_item_Base.h"

#include "agaliareptImpl.h"
#include "container_RIFF.h"



using namespace analyze_RIFF;



// {F751F8B3-FC25-46A0-9956-7EF48E55BB6B}
const GUID item_Base::guid_riff =
{ 0xf751f8b3, 0xfc25, 0x46a0, { 0x99, 0x56, 0x7e, 0xf4, 0x8e, 0x55, 0xbb, 0x6b } };



item_Base::item_Base(const container_RIFF* _image, uint64_t offset, uint64_t size)
	: _agaliaItemBase(guid_riff, offset, size), image(_image)
{
}



item_Base::~item_Base()
{
}



HRESULT item_Base::getItemName(agaliaString** str) const
{
	std::wstringstream temp;
	auto hr = format_chunk_fcc<decltype(RIFFCHUNK::fcc)>(temp, offsetof(RIFFCHUNK, fcc), this);
	if (FAILED(hr)) return hr;
	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
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

	switch (index)
	{
	case prop_offset:	*str = agaliaString::create(L"offset"); break;
	case prop_fcc:		*str = agaliaString::create(L"fcc"); break;
	case prop_cb:		*str = agaliaString::create(L"cb"); break;
	default:
		return E_INVALIDARG;
	}

	return S_OK;
}



HRESULT item_Base::getItemPropValue(uint32_t index, agaliaString** str) const
{
	std::wstringstream temp;

	if (index == prop_offset)
	{
		format_dec(temp, getOffset());
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_fcc)
	{
		return item_Base::getItemName(str);
	}
	else if (index == prop_cb)
	{
		auto hr = format_chunk_dec<decltype(RIFFCHUNK::cb)>(temp, offsetof(RIFFCHUNK, cb), this);
		if (FAILED(hr)) return hr;

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
	if (next == nullptr) return E_POINTER;

	RIFFCHUNK cur_chunk = {};
	auto hr = image->ReadData(&cur_chunk, getOffset(), sizeof(cur_chunk));
	if (FAILED(hr)) return hr;

	uint64_t next_item_offset = getOffset() + sizeof(RIFFCHUNK) + RIFFROUND(uint64_t(cur_chunk.cb));

	if (endPos <= next_item_offset)
		return E_FAIL;	// termination  

	RIFFCHUNK next_chunk = {};
	hr = image->ReadData(&next_chunk, next_item_offset, sizeof(next_chunk));
	if (FAILED(hr)) return hr;

	if (next_chunk.fcc == 0)
		return E_FAIL;

	item_Base* item = create_item(image, next_item_offset, next_chunk.fcc, next_chunk.cb, fccType);
	item->endPos = endPos;
	item->fccParent = fccParent;
	*next = item;
	return S_OK;
}



HRESULT item_Base::getAsocImage(const agaliaContainer** imageAsoc) const
{
	if (imageAsoc == nullptr) return E_POINTER;
	*imageAsoc = image;
	return S_OK;
}



HRESULT item_Base::getValueAreaOffset(uint64_t* offset) const
{
	if (offset == nullptr) return E_POINTER;
	*offset = getOffset() + sizeof(RIFFCHUNK);
	return S_OK;
}



HRESULT item_Base::getValueAreaSize(uint64_t* size) const
{
	if (size == nullptr) return E_POINTER;
	*size = getSize() - sizeof(RIFFCHUNK);
	return S_OK;
}



HRESULT item_Base::getColumnValue(uint32_t column, agaliaString** str) const
{
	std::wstringstream temp;

	if (column == column_offset)
	{
		format_hex(temp, getOffset(), 8);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (column == column_parent)
	{
		if (fccParent) {
			format_fcc(temp, fccParent);
		}

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (column == column_id)
	{
		return item_Base::getItemName(str);
	}
	else if (column == column_size)
	{
		auto hr = format_chunk_dec<decltype(RIFFCHUNK::cb)>(temp, offsetof(RIFFCHUNK, cb), this);
		if (FAILED(hr)) return hr;

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (column == column_value)
	{
		uint64_t item_size = getSize() - sizeof(RIFFCHUNK);

		rsize_t bufsize = 0;
		auto hr = UInt64ToSizeT(min(64, item_size), &bufsize);
		if (FAILED(hr)) return hr;

		CHeapPtr<uint8_t> buf;
		if (!buf.AllocateBytes(bufsize)) return E_OUTOFMEMORY;

		hr = image->ReadData(buf, getOffset() + sizeof(RIFFCHUNK), bufsize);
		if (FAILED(hr)) return hr;
		for (rsize_t i = 0; i < bufsize; i++)
		{
			if (i != 0) temp << L",";
			format_hex(temp, buf[i]);
		}
		if (bufsize != item_size) {
			temp << L", ...";
		}

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}

	return E_FAIL;
}
