#include "pch.h"
#include "analyze_M2P_item_PESPacket.h"

#include "analyze_M2P_util.h"

using namespace analyze_M2P;



item_PES_packet::item_PES_packet(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:item_Base(image, offset, size)
{
}



item_PES_packet::~item_PES_packet()
{
}



HRESULT item_PES_packet::getItemName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	*str = agaliaString::create(L"PES_packet");
	return S_OK;
}



HRESULT item_PES_packet::getItemPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}



HRESULT item_PES_packet::getItemPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_packet_start_code_prefix: name = L"packet_start_code_prefix"; break;
	case prop_stream_id: name = L"stream_id"; break;
	case prop_PES_packet_length: name = L"PES_packet_length"; break;
	default:
		return __super::getItemPropName(index, str);
	}

	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT item_PES_packet::getItemPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	auto hr = __super::getItemPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	uint8_t buf[6 + 8] = {};
	hr = image->ReadData(buf, getOffset(), 6);
	if (FAILED(hr)) return hr;

	std::wstringstream temp;
	if (index == prop_packet_start_code_prefix)
	{
		temp << L"0x";
		format_hex(temp, get_value_32(buf, 0, 24), 9);
	}
	else if (index == prop_stream_id)
	{
		temp << L"0x";
		format_hex(temp, get_value_8(buf, 24, 8));
	}
	else if (index == prop_PES_packet_length)
	{
		format_dec(temp, get_value_16(buf, 24 + 8, 16));
	}
	else
	{
		return E_INVALIDARG;
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}



HRESULT item_PES_packet::getNextItem(agaliaItem** next) const
{
	if (next == nullptr) return E_POINTER;

	uint64_t PES_packet_size = item_PES_packet::calc_item_size(image, getOffset() + getSize());
	if (PES_packet_size == 0) return E_FAIL;

	auto p = new item_PES_packet(image, getOffset() + getSize(), PES_packet_size);
	*next = p;
	return S_OK;
}



HRESULT item_PES_packet::getGridRowCount(uint32_t* row) const
{
	if (row == nullptr) return E_POINTER;
	*row = 3;
	return S_OK;
}



HRESULT item_PES_packet::getGridValue(uint32_t row, uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	std::wstringstream temp;
	if (column == column_offset)
	{
		if (row == 0) {
			format_hex(temp, getOffset(), 8);
		}
	}
	else if (column == column_field)
	{
		switch (row)
		{
		case 0: temp << L"packet_start_code_prefix"; break;
		case 1: temp << L"stream_id"; break;
		case 2: temp << L"PES_packet_length"; break;
		}
	}
	else if (column == column_bits)
	{
		switch (row)
		{
		case 0: format_dec(temp, 24); break;
		case 1: format_dec(temp, 8); break;
		case 2: format_dec(temp, 16); break;
		}
		
	}
	else if (column == column_value)
	{
		rsize_t pos = 0;
		rsize_t size = 0;
		rsize_t read_size = 0;
		if (0 <= row) {
			pos += size;
			size = 24;
			read_size = (pos + size + 7) / 8;
		}
		if (1 <= row) {
			pos += size;
			size = 8;
			read_size = (pos + size + 7) / 8;
		}
		if (2 <= row) {
			pos += size;
			size = 16;
			read_size = (pos + size + 7) / 8;
		}

		CHeapPtr<uint8_t> buf;
		if (!buf.AllocateBytes(read_size + 8)) return E_POINTER;
		image->ReadData(buf, getOffset(), read_size);

		temp << L"0x";
		if (size <= 8)
		{
			uint8_t val = get_value_8(buf, pos, size);
			format_hex(temp, val);
		}
		else if (size <= 16)
		{
			uint16_t val = get_value_16(buf, pos, size);
			format_hex(temp, val);
		}
		else if (size <= 32)
		{
			uint32_t val = get_value_32(buf, pos, size);
			format_hex(temp, val);
		}
		else
		{
			return E_FAIL;
		}
	}
	else
	{
		return E_INVALIDARG;
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}



uint64_t item_PES_packet::calc_item_size(const agaliaContainer* image, uint64_t offset)
{
	CHeapPtr<uint8_t> buf;
	if (!buf.AllocateBytes(8 + 8)) return 0;
	auto hr = image->ReadData(buf, offset, 6);
	if (FAILED(hr)) return 0;

	uint32_t code = get_value_32(buf, 0, 32);
	if (code == 0x000001BA)	// pack_start_code
		return 0;
	if (code == 0x000001B9)	// MPEG_program_end_code
		return 0;

	uint32_t packet_start_code_prefix = get_value_32(buf, 0, 24);
	if (packet_start_code_prefix != 0x000001)
		return 0;

	uint16_t PES_packet_length = get_value_16(buf, 24 + 8, 16);
	return 6ULL + PES_packet_length;
}
