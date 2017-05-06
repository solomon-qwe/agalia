#include "stdafx.h"
#include "../inc/agaliarept.h"
#include "callback.h"
#include "util.h"

template <class T>
T print_be_value(uint64_t* pos, int subItem, const uint8_t* base, uint64_t offset, uint64_t size, const wchar_t* item_name, bool need_linefeed, callback_cls* callback)
{
	uint32_t item_size = max(sizeof(uint32_t), sizeof(T));
	if (size < *pos + item_size) {
		throw - 1;
	}

	std::wstring str(item_name);
	T val = be_itostr<T>(str, base, offset + *pos);

	if (need_linefeed) {
		callback->next();
	}
	callback->print(subItem, str);

	*pos += item_size;
	return val;
}

void print_ftyp(int subItem, const uint8_t* base, uint64_t offset, uint64_t size, callback_cls* callback)
{
	uint64_t pos = 0;
	uint32_t item_size = 0;

	// Size 
	item_size = 4;
	if (size < pos + item_size) return;
	pos += item_size;

	// Type 
	item_size = 4;
	if (size < pos + item_size) return;
	pos += item_size;

	char fcc[5] = {};
	wchar_t temp[128] = {};

	std::wstring str;

	// Major_Brand 
	item_size = 4;
	if (size < pos + item_size) return;
	*reinterpret_cast<uint32_t*>(fcc) = *reinterpret_cast<const uint32_t*>(base + offset + pos);
	swprintf_s(temp, L"'%S'", fcc);
	str += L"Major_Brand:";
	str += temp;
	pos += item_size;

	// Minor_Version 
	item_size = 4;
	if (size < pos + item_size) return;
	swprintf_s(temp, L"'%.8X'", _byteswap_ulong(*reinterpret_cast<const uint32_t*>(base + offset + pos)));
	str += L", Minor_Version:";
	str += temp;
	pos += item_size;

	callback->print(subItem, str);

	// Compatible_Brands[] 
	for (size_t i = 0; i < (size - 8) / sizeof(uint32_t); i++)
	{
		callback->next();
		swprintf_s(temp, L"Compatible_Brands[%zu]:'%.8X'", i, _byteswap_ulong(*reinterpret_cast<const uint32_t*>(base + offset + pos)));
		callback->print(subItem, temp);
		pos += 4;
	}
}

void print_mvhd(int subItem, const uint8_t* base, uint64_t offset, uint64_t size, callback_cls* callback)
{
	uint64_t pos = 0;
	uint32_t item_size = 0;

	// Size 
	item_size = 4;
	if (size < pos + item_size) return;
	pos += item_size;

	// Type 
	item_size = 4;
	if (size < pos + item_size) return;
	pos += item_size;

	try
	{
		uint8_t version = print_be_value<uint8_t>(&pos, subItem, base, offset, size, L"Version:", false, callback);

		if (version == 0)
		{
			print_be_value<__time32_t>(&pos, subItem, base, offset, size, L"creation_time:", true, callback);
			print_be_value<__time32_t>(&pos, subItem, base, offset, size, L"modification_time:", true, callback);
			print_be_value<uint32_t>(&pos, subItem, base, offset, size, L"timescale:", true, callback);
			print_be_value<uint32_t>(&pos, subItem, base, offset, size, L"duration:", true, callback);
		}
		else if (version == 1)
		{
			print_be_value<__time64_t>(&pos, subItem, base, offset, size, L"creation_time:", true, callback);
			print_be_value<__time64_t>(&pos, subItem, base, offset, size, L"modification_time:", true, callback);
			print_be_value<uint32_t>(&pos, subItem, base, offset, size, L"timescale:", true, callback);
			print_be_value<uint64_t>(&pos, subItem, base, offset, size, L"duration:", true, callback);
		}
	}
	catch (int)
	{
		return;
	}
}

void print_tkhd(int subItem, const uint8_t* base, uint64_t offset, uint64_t size, callback_cls* callback)
{
	uint64_t pos = 0;
	uint32_t item_size = 0;

	// Size 
	item_size = 4;
	if (size < pos + item_size) return;
	pos += item_size;

	// Type 
	item_size = 4;
	if (size < pos + item_size) return;
	pos += item_size;

	try
	{
		uint8_t version = print_be_value<uint8_t>(&pos, subItem, base, offset, size, L"Version:", false, callback);

		if (version == 0)
		{
			print_be_value<__time32_t>(&pos, subItem, base, offset, size, L"creation_time:", true, callback);
			print_be_value<__time32_t>(&pos, subItem, base, offset, size, L"modification_time:", true, callback);
			print_be_value<uint32_t>(&pos, subItem, base, offset, size, L"track_ID:", true, callback);
			print_be_value<uint32_t>(&pos, subItem, base, offset, size, L"reserved:", true, callback);
			print_be_value<uint32_t>(&pos, subItem, base, offset, size, L"duration:", true, callback);
		}
		else if (version == 1)
		{
			print_be_value<__time64_t>(&pos, subItem, base, offset, size, L"creation_time:", true, callback);
			print_be_value<__time64_t>(&pos, subItem, base, offset, size, L"modification_time:", true, callback);
			print_be_value<uint32_t>(&pos, subItem, base, offset, size, L"track_ID:", true, callback);
			print_be_value<uint32_t>(&pos, subItem, base, offset, size, L"reserved:", true, callback);
			print_be_value<uint64_t>(&pos, subItem, base, offset, size, L"duration:", true, callback);
		}
	}
	catch (int)
	{
		return;
	}
}

void parseBox(const uint8_t* base, uint64_t offset, uint64_t endPos, uint32_t parent, callback_cls* callback, const agalia::config* config)
{
	uint64_t size = 0;

	while (offset < endPos)
	{
		// offset 
		int subItem = 0;
		{
			wchar_t sz[16] = {};
			swprintf_s(sz, L"%.8I64X", offset);
			callback->print(subItem, sz);
		}

		// parent  
		subItem++;
		{
			char sz[5] = {};
			*reinterpret_cast<uint32_t*>(sz) = parent;
			callback->print(subItem, sz);
		}

		uint32_t item_size = 0;
		uint64_t local_offset = 0;

		item_size = sizeof(uint32_t);
		if (endPos <= offset + local_offset + item_size) break;
		size = _byteswap_ulong(*reinterpret_cast<const uint32_t*>(base + offset + local_offset));
		local_offset += item_size;

		item_size = sizeof(uint32_t);
		if (endPos <= offset + local_offset + item_size) break;
		uint32_t type = *reinterpret_cast<const uint32_t*>(base + offset + local_offset);
		local_offset += item_size;

		if (size == 1)
		{
			item_size = sizeof(uint64_t);
			if (endPos <= offset + local_offset + item_size) break;
			size = _byteswap_uint64(*reinterpret_cast<const uint64_t*>(base + offset + local_offset));
			local_offset += item_size;
		}

		// type 
		subItem++;
		{
			char sz[5] = {};
			*reinterpret_cast<uint32_t*>(sz) = type;
			wchar_t temp[64] = {};
			swprintf_s(temp, L"%S (%.4X)", sz, type);
			callback->print(subItem, temp);
		}

		// Size 
		subItem++;
		{
			wchar_t temp[16] = {};
			_ui64tow_s(size, temp, _countof(temp), 10);
			callback->print(subItem, temp);
		}

		callback->set_extra_data(offset + local_offset, size);

		// data 
		subItem++;
		void putDataAsByteArray(int subItem, const uint8_t* base_addr, uint64_t offset, uint64_t size, callback_cls* callback, const agalia::config* config);
		putDataAsByteArray(subItem, base, offset + local_offset, size, callback, config);

		// value 
		subItem++;
		switch (_byteswap_ulong(type))
		{
		case 'ftyp':
			print_ftyp(subItem, base, offset, size, callback);
			break;
		case 'mvhd':
			print_mvhd(subItem, base, offset, size, callback);
			break;
		case 'tkhd':
			print_tkhd(subItem, base, offset, size, callback);
			break;
		}

		callback->next();

		switch (_byteswap_ulong(type))
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
			parseBox(base, offset + local_offset, min(endPos, offset + size), type, callback, config);
			break;
		}

		offset += size;

		config->is_abort(agalia::config::throw_on_abort);
	}
}

#include "image_file.h"

bool image_file_iso::is_supported(const void* buf, uint64_t size)
{
	if (buf == nullptr) return false;
	if (size < 4 + 4) return false;

	uint32_t prefix = *reinterpret_cast<const uint32_t*>(reinterpret_cast<const unsigned char*>(buf) + 4);

	bool ret = false;
	switch (_byteswap_ulong(prefix))
	{
	case 'ftyp':
	case 'moov':
	case 'wide':
	case 'pnot':
		ret = true;
		break;
	}

	return ret;
}

void image_file_iso::parse(callback_cls* callback, const agalia::config* config)
{
	callback->insert_column(96, L"offset");
	callback->insert_column(96, L"parent");
	callback->insert_column(128, L"type");
	int size_column_number = callback->insert_column(128, L"size");
	callback->set_right_align(size_column_number, true);
	callback->insert_column(128, L"data");
	callback->insert_column(320, L"value");
	callback->next();

	parseBox(this->base_addr, this->base_offset, this->base_offset + this->data_size, 0, callback, config);
}


