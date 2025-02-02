#include "pch.h"
#include "analyze_RIFF_item_idx1.h"



using namespace analyze_RIFF;



item_idx1::item_idx1(const container_RIFF* _image, uint64_t offset, uint64_t size)
	: RIFF_item_Base(_image, offset, size)
{
}



item_idx1::~item_idx1()
{
}



HRESULT item_idx1::getChild(uint32_t sibling, agaliaElement** child) const
{
	if (sibling != 0) return E_FAIL;
	if (child == nullptr) return E_POINTER;

	uint64_t child_item_offset = getOffset() + sizeof(RIFFCHUNK);

	RIFFCHUNK cur_chunk = {};
	auto hr = image->ReadData(&cur_chunk, getOffset(), sizeof(cur_chunk));
	if (FAILED(hr)) return hr;

	auto item = new item_idx1_entry(image, child_item_offset, sizeof(AVIOLDINDEX::_avioldindex_entry));
	item->endPos = getOffset() + sizeof(RIFFCHUNK) + cur_chunk.cb;
	item->fccParent = cur_chunk.fcc;
	*child = item;
	return S_OK;
}



HRESULT item_idx1::getValueAreaOffset(uint64_t* offset) const
{
	if (offset == nullptr) return E_POINTER;
	*offset = getOffset();
	return S_OK;
}



HRESULT item_idx1::getValueAreaSize(uint64_t* size) const
{
	if (size == nullptr) return E_POINTER;
	*size = offsetof(AVIOLDINDEX, aIndex);
	return S_OK;
}



HRESULT item_idx1::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_value)
	{
		RIFFCHUNK avioldindex = {};
		auto hr = image->ReadData(&avioldindex, getOffset(), sizeof(avioldindex));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"fcc:";
		format_fcc(temp, avioldindex.fcc);
		temp << L", cb:";
		format_dec(temp, avioldindex.cb);

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}

	return __super::getColumnValue(column, str);
}





item_idx1_entry::item_idx1_entry(const container_RIFF* _image, uint64_t offset, uint64_t size)
	: RIFF_item_Base(_image, offset, size)
{
}



item_idx1_entry::~item_idx1_entry()
{
}



HRESULT item_idx1_entry::getName(agaliaString** str) const
{
	std::wstringstream temp;
	temp << L"aIndex[" << _index << L"]";
	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}



HRESULT item_idx1_entry::getPropCount(uint32_t* count) const
{
	*count = prop_last;
	return S_OK;
}



HRESULT item_idx1_entry::getPropName(uint32_t index, agaliaString** str) const
{
	switch (index)
	{
	case prop_dwChunkId:	*str = agaliaString::create(L"dwChunkId"); break;
	case prop_dwFlags:		*str = agaliaString::create(L"dwFlags"); break;
	case prop_dwOffset:		*str = agaliaString::create(L"dwOffset"); break;
	case prop_dwSize:		*str = agaliaString::create(L"dwSize"); break;
	default:
		return E_INVALIDARG;
	}
	return S_OK;
}



HRESULT item_idx1_entry::getPropValue(uint32_t index, agaliaString** str) const
{
	AVIOLDINDEX::_avioldindex_entry entry = {};
	auto hr = image->ReadData(&entry, getOffset(), sizeof(entry));
	if (FAILED(hr)) return hr;

	std::wstringstream temp;
	if (index == prop_dwChunkId)
	{
		format_fcc(temp, entry.dwChunkId);
	}
	else if (index == prop_dwFlags)
	{
		format_hex(temp, entry.dwFlags);
	}
	else if (index == prop_dwOffset)
	{
		format_dec(temp, entry.dwOffset);
	}
	else if (index == prop_dwSize)
	{
		format_dec(temp, entry.dwSize);
	}
	else
	{
		return E_INVALIDARG;
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}



HRESULT item_idx1_entry::getChild(uint32_t sibling, agaliaElement** child) const
{
	UNREFERENCED_PARAMETER(sibling);
	UNREFERENCED_PARAMETER(child);
	return E_FAIL;
}



HRESULT item_idx1_entry::getNext(agaliaElement** next) const
{
	uint64_t next_item_offset = getOffset() + sizeof(AVIOLDINDEX::_avioldindex_entry);
	if (endPos <= next_item_offset)
		return E_FAIL;

	auto item = new item_idx1_entry(image, next_item_offset, sizeof(AVIOLDINDEX::_avioldindex_entry));
	item->_index = _index + 1;
	item->endPos = endPos;
	item->fccParent = fccParent;
	*next = item;

	return S_OK;
}



HRESULT item_idx1_entry::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_offset)
	{
		*str = agaliaString::create(L"");
		return S_OK;
	}
	if (column == column_value)
	{
		AVIOLDINDEX::_avioldindex_entry entry = {};
		auto hr = image->ReadData(&entry, getOffset(), sizeof(entry));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"[" << _index << L"].dwChunkId=";
		format_fcc(temp, entry.dwChunkId);

		temp << L", dwFlags=";
		format_hex(temp, entry.dwFlags);

		temp << L", dwOffset=";
		format_dec(temp, entry.dwOffset);

		temp << L", dwSize=";
		format_dec(temp, entry.dwSize);

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}

	return E_FAIL;
}
