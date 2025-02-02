#include "pch.h"
#include "analyze_ISO_item_Box.h"

#include "analyze_ISO_util.h"

using namespace analyze_ISO;

uint64_t analyze_ISO::Box::getBoxSize(const agaliaContainer* image, uint64_t offset, uint64_t endpos)
{
	Box temp = {};
	auto hr = image->ReadData(&temp.size, offset + offsetof(Box, size), sizeof(Box::size));
	if (FAILED(hr)) return 0;

	if (temp.getSize() == 1)
	{
		uint64_t largesize = 0;
		hr = image->ReadData(&largesize, offset + sizeof(Box), sizeof(largesize));
		if (FAILED(hr)) return 0;

		return agalia_byteswap(largesize);
	}
	else if (temp.getSize() == 0)
	{
		return endpos - offset;
	}
	return temp.getSize();
}

uint64_t analyze_ISO::Box::getDataOffset(const agaliaContainer* image, uint64_t offset)
{
	Box temp = {};
	auto hr = image->ReadData(&temp, offset, sizeof(temp));
	if (FAILED(hr)) return 0;

	uint64_t ret = sizeof(Box);

	if (temp.getSize() == 1)
	{
		ret += sizeof(uint64_t);
	}

	return ret;
}



// {554A3BF5-DD08-478B-9E09-D5436450BE09}
const GUID ISO_item_Box::guid_iso =
{ 0x554a3bf5, 0xdd08, 0x478b, { 0x9e, 0x9, 0xd5, 0x43, 0x64, 0x50, 0xbe, 0x9 } };

ISO_item_Box::ISO_item_Box(const agaliaContainer* image, uint64_t offset, uint64_t size, uint64_t endpos, uint32_t parent)
	:_agaliaItemBase(guid_iso, offset, size)
{
	this->image = image;
	this->endpos = endpos;
	this->parent = parent;
}

ISO_item_Box::~ISO_item_Box()
{

}

HRESULT ISO_item_Box::getName(agaliaString** str) const
{
	Box box = {};
	auto hr = image->ReadData(&box, getOffset(), sizeof(box));
	if (FAILED(hr)) return hr;

	return multibyte_to_widechar(reinterpret_cast<char*>(&box.type), sizeof(box.type), CP_US_ASCII, str);
}

HRESULT ISO_item_Box::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT ISO_item_Box::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	if (index == prop_largesize)
	{
		Box box = {};
		auto hr = image->ReadData(&box, getOffset(), sizeof(box));
		if (FAILED(hr)) return hr;

		if (box.getSize() != 1)
			return E_FAIL;
	}

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_offset: name = L"offset"; break;
	case prop_size: name = L"size"; break;
	case prop_type: name = L"type"; break;
	case prop_largesize: name = L"largesize"; break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(name);
	return S_OK;
}

HRESULT ISO_item_Box::getPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	if (index == prop_offset)
	{
		std::wstringstream temp;
		format_dec(temp, getOffset());
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_size)
	{
		Box box = {};
		auto hr = image->ReadData(&box, getOffset(), sizeof(box));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		format_dec(temp, box.getSize());
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_type)
	{
		return getName(str);
	}
	else if (index == prop_largesize)
	{
		Box box = {};
		auto hr = image->ReadData(&box, getOffset(), sizeof(box));
		if (FAILED(hr)) return hr;

		if (box.getSize() != 1)
			return E_FAIL;

		std::wstringstream temp;
		format_dec(temp, Box::getBoxSize(image, getOffset(), endpos));
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	return E_INVALIDARG;
}


HRESULT ISO_item_Box::getChild(uint32_t sibling, agaliaElement** child) const
{
	if (sibling != 0) return E_FAIL;

	Box curBox = {};
	auto hr = image->ReadData(&curBox, getOffset(), sizeof(curBox));
	if (FAILED(hr)) return hr;

	switch (curBox.getType())
	{
	case 'moov':
	case 'trak':
	case 'edts':
	case 'mdia':
	case 'minf':
	case 'dinf':
	case 'stbl':
	case 'mvex':
	case 'moof':
	case 'traf':
	case 'mfra':
	case 'skip':
	case 'meta':
	case 'ipro':
	case 'sinf':
		break;
	default:
		return E_FAIL;
	}

	auto p = createItem(image, getOffset() + sizeof(Box), getOffset() + getSize(), curBox.type);
	if (p)
	{
		*child = p;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT ISO_item_Box::getNext(agaliaElement** next) const
{
	auto p = createItem(image, getOffset() + getSize(), endpos, parent);
	if (p)
	{
		*next = p;
		return S_OK;
	}
	return E_FAIL;
}


HRESULT ISO_item_Box::getAsocImage(const agaliaContainer** imageAsoc) const
{
	if (imageAsoc == nullptr) return E_POINTER;
	*imageAsoc = image;
	return S_OK;
}


HRESULT ISO_item_Box::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	if (column == column_offset)
	{
		std::wstringstream temp;
		format_hex(temp, getOffset(), 8);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (column == column_parent)
	{
		std::wstringstream temp;
		multibyte_to_widechar(const_cast<char*>(reinterpret_cast<const char*>(&parent)), sizeof(parent), CP_US_ASCII, temp);

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (column == column_type)
	{
		Box box = {};
		auto hr = image->ReadData(&box, getOffset(), sizeof(box));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		multibyte_to_widechar(reinterpret_cast<char*>(&box.type), sizeof(Box::type), CP_US_ASCII, temp);
		temp << L" (";
		format_hex(temp, box.type);
		temp << L")";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (column == column_size)
	{
		Box box = {};
		auto hr = image->ReadData(&box, getOffset(), sizeof(box));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		format_dec(temp, box.getSize());
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (column == column_value)
	{

	}

	return E_INVALIDARG;
}
