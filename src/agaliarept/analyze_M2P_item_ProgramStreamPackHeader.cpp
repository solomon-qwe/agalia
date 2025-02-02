#include "pch.h"
#include "analyze_M2P_item_ProgramStreamPackHeader.h"

#include "analyze_M2P_util.h"
#include "analyze_M2P_item_ProgramStreamSystemHeader.h"
#include "analyze_M2P_item_PESPacket.h"

using namespace analyze_M2P;

class analyze_M2P::program_stream_pack_header : public bitfield
{
public:
	program_stream_pack_header()
	{
		v.push_back(bitpair(L"pack_start_code", 32));
		v.push_back(bitpair(L"'01'", 2));
		v.push_back(bitpair(L"system_clock_reference_base [32..30]", 3));
		v.push_back(bitpair(L"marker_bit", 1));
		v.push_back(bitpair(L"system_clock_reference_base [29..15]", 15));
		v.push_back(bitpair(L"marker_bit", 1));
		v.push_back(bitpair(L"system_clock_reference_base [14..0]", 15));
		v.push_back(bitpair(L"marker_bit", 1));
		v.push_back(bitpair(L"system_clock_reference_extension", 9));
		v.push_back(bitpair(L"marker_bit", 1));
		v.push_back(bitpair(L"program_mux_rate", 22));
		v.push_back(bitpair(L"marker_bit", 1));
		v.push_back(bitpair(L"marker_bit", 1));
		v.push_back(bitpair(L"reserved", 5));
		v.push_back(bitpair(L"pack_stuffing_length", 3));
	}

	virtual ~program_stream_pack_header()
	{
	}

	enum
	{
		pack_start_code,
		zero_one,
		system_clock_reference_base_1,
		marker_bit_1,
		system_clock_reference_base_2,
		marker_bit_2,
		system_clock_reference_base_3,
		marker_bit_3,
		system_clock_reference_extension,
		marker_bit_4,
		program_mux_rate,
		marker_bit_5,
		marker_bit_6,
		reserved,
		pack_stuffing_length,
		last
	};
};







item_ProgramStreamPackHeader::item_ProgramStreamPackHeader(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:M2P_item_Base(image, offset, size)
{
	bitfield = new program_stream_pack_header;
}

item_ProgramStreamPackHeader::~item_ProgramStreamPackHeader()
{
	delete bitfield;
}

HRESULT item_ProgramStreamPackHeader::getName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	*str = agaliaString::create(L"pack_header");
	return S_OK;
}

HRESULT item_ProgramStreamPackHeader::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT item_ProgramStreamPackHeader::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	program_stream_pack_header* b = reinterpret_cast<program_stream_pack_header*>(bitfield);

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_pack_start_code:					name = b->v[b->pack_start_code].first; break;
	case prop_01:								name = b->v[b->zero_one].first; break;
	case prop_system_clock_reference_base:		name = L"system_clock_reference_base";	break;
	case prop_system_clock_reference_extension:	name = b->v[b->system_clock_reference_extension].first; break;
	case prop_program_mux_rate:					name = b->v[b->program_mux_rate].first; break;
	case prop_reserved:							name = b->v[b->reserved].first; break;
	case prop_pack_stuffing_length:				name = b->v[b->pack_stuffing_length].first; break;
	case prop_stuffing_byte:
		if (1)
		{
			const rsize_t read_size = b->byte_size();
			CHeapPtr<uint8_t> buf;
			if (!buf.AllocateBytes(read_size + 8)) return E_OUTOFMEMORY;
			auto hr = image->ReadData(buf, getOffset(), read_size);
			if (FAILED(hr)) return hr;

			uint8_t val = get_value_8(buf, b->bit_offset_of(b->pack_stuffing_length), b->bit_size_of(b->pack_stuffing_length));
			if (val == 0) return E_FAIL;
			name = b->v[b->pack_stuffing_length].first;
		}
		break;
	default:
		return __super::getPropName(index, str);
	}

	*str = agaliaString::create(name);
	return S_OK;
}


HRESULT item_ProgramStreamPackHeader::getPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	auto hr = __super::getPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	program_stream_pack_header* b = bitfield;

	const rsize_t read_size = b->byte_size();
	CHeapPtr<uint8_t> buf;
	if (!buf.AllocateBytes(read_size + 8)) return E_OUTOFMEMORY;
	hr = image->ReadData(buf, getOffset(), read_size);
	if (FAILED(hr)) return hr;

	std::wstringstream temp;
	if (index == prop_pack_start_code)
	{
		int idx = b->pack_start_code;
		rsize_t offset = b->bit_offset_of(idx);
		rsize_t leng = b->bit_size_of(idx);
		uint32_t pack_start_code = get_value_32(buf, offset, leng);

		temp << L"0x";
		format_hex(temp, pack_start_code);
	}
	else if (index == prop_01)
	{
		int idx = b->zero_one;
		rsize_t offset = b->bit_offset_of(idx);
		rsize_t leng = b->bit_size_of(idx);
		uint8_t val_01 = get_value_8(buf, offset, leng);

		format_hex(temp, val_01, 1);
	}
	else if (index == prop_system_clock_reference_base)
	{
		uint64_t system_clock_reference_base = 0;

		int idx1 = b->system_clock_reference_base_1;
		int idx2 = b->system_clock_reference_base_2;
		int idx3 = b->system_clock_reference_base_3;

		rsize_t offset = b->bit_offset_of(idx1);
		rsize_t leng = 0;
		rsize_t shift = b->bit_size_of(idx1) + b->bit_size_of(idx2) + b->bit_size_of(idx3);

		offset = b->bit_offset_of(idx1); leng = b->bit_size_of(idx1); shift -= leng;
		system_clock_reference_base |= static_cast<uint64_t>(get_value_16(buf, offset, leng)) << shift;

		offset = b->bit_offset_of(idx2); leng = b->bit_size_of(idx2); shift -= leng;
		system_clock_reference_base |= static_cast<uint64_t>(get_value_16(buf, offset, leng)) << shift;

		offset += leng + 1; leng = b->bit_size_of(idx3); shift -= leng;
		system_clock_reference_base |= static_cast<uint64_t>(get_value_16(buf, offset, leng)) << shift;

		temp << L"0x";
		format_hex(temp, system_clock_reference_base, 5);
	}
	else if (index == prop_system_clock_reference_extension)
	{
		int idx = b->system_clock_reference_extension;
		rsize_t offset = b->bit_offset_of(idx);
		rsize_t leng = b->bit_size_of(idx);
		uint16_t system_clock_reference_extension = get_value_16(buf, offset, leng);

		temp << L"0x";
		format_hex(temp, system_clock_reference_extension, 2);
	}
	else if (index == prop_program_mux_rate)
	{
		int idx = b->program_mux_rate;
		rsize_t offset = b->bit_offset_of(idx);
		rsize_t leng = b->bit_size_of(idx);
		uint32_t program_mux_rate = get_value_32(buf, offset, leng);

		temp << L"0x";
		format_hex(temp, program_mux_rate, 3);
	}
	else if (index == prop_reserved)
	{
		int idx = b->reserved;
		rsize_t offset = b->bit_offset_of(idx);
		rsize_t leng = b->bit_size_of(idx);
		uint8_t reserved = get_value_8(buf, offset, leng);

		temp << L"0x";
		format_hex(temp, reserved, 1);
	}
	else if (index == prop_pack_stuffing_length)
	{
		int idx = b->pack_stuffing_length;
		rsize_t offset = b->bit_offset_of(idx);
		rsize_t leng = b->bit_size_of(idx);
		uint8_t pack_stuffing_length = get_value_8(buf, offset, leng);

		temp << L"0x";
		format_hex(temp, pack_stuffing_length, 1);
	}
	else
	{
		return E_FAIL;
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}




HRESULT item_ProgramStreamPackHeader::getChild(uint32_t sibling, agaliaElement** child) const
{
	if (sibling != 0) return E_FAIL;
	if (child == nullptr) return E_POINTER;

	const rsize_t read_size = bitfield->byte_size();
	CHeapPtr<uint8_t> buf;
	if (!buf.AllocateBytes(read_size + 8)) return E_OUTOFMEMORY;
	auto hr = image->ReadData(buf, getOffset(), read_size);
	if (FAILED(hr)) return E_FAIL;

	int idx = bitfield->pack_start_code;
	rsize_t pos = bitfield->bit_offset_of(idx);
	rsize_t leng = bitfield->bit_size_of(idx);
	uint32_t pack_start_code = get_value_32(buf, pos, leng);
	if (pack_start_code != 0x000001BA)
		return E_FAIL;

	idx = bitfield->pack_stuffing_length;
	pos = bitfield->bit_offset_of(idx);
	leng = bitfield->bit_size_of(idx);
	uint8_t pack_stuffing_length = get_value_8(buf, pos, leng);

	uint64_t system_header_pos = static_cast<uint64_t>(bitfield->byte_size()) + pack_stuffing_length;
	uint64_t system_header_size = item_ProgramStreamSystemHeader::calc_item_size(image, system_header_pos);
	if (system_header_size == 0)
		return E_FAIL;

	auto p = new item_ProgramStreamSystemHeader(image, getOffset() + system_header_pos, system_header_size);
	*child = p;
	return S_OK;
}


HRESULT item_ProgramStreamPackHeader::getNext(agaliaElement** next) const
{
	if (next == nullptr) return E_POINTER;

	uint64_t PES_packet_size = item_PES_packet::calc_item_size(image, getOffset() + getSize());
	if (PES_packet_size == 0) return E_FAIL;

	auto p = new item_PES_packet(image, getOffset() + getSize(), PES_packet_size);
	*next = p;
	return S_OK;
}



HRESULT item_ProgramStreamPackHeader::getElementInfoCount(uint32_t* row) const
{
	if (row == nullptr) return E_POINTER;
	*row = bitfield->last;
	return S_OK;
}



HRESULT item_ProgramStreamPackHeader::getElementInfoValue(uint32_t row, uint32_t column, agaliaString** str) const
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
		temp << bitfield->v[row].first;
	}
	else if (column == column_bits)
	{
		format_dec(temp, bitfield->v[row].second);
	}
	else if (column == column_value)
	{
		rsize_t pos = bitfield->bit_offset_of(row);
		rsize_t size = bitfield->bit_size_of(row);

		rsize_t read_size = bitfield->byte_size();
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



uint64_t item_ProgramStreamPackHeader::calc_item_size(const agaliaContainer* image, uint64_t offset)
{
	if (image == nullptr) return 0;

	program_stream_pack_header b;

	const rsize_t read_size = b.byte_size();
	CHeapPtr<uint8_t> buf;
	if (!buf.AllocateBytes(read_size + 8)) return 0;
	auto hr = image->ReadData(buf, offset, read_size);
	if (FAILED(hr)) return 0;

	int idx = b.pack_start_code;
	rsize_t pos = b.bit_offset_of(idx);
	rsize_t leng = b.bit_size_of(idx);
	uint32_t pack_start_code = get_value_32(buf, pos, leng);
	if (pack_start_code != 0x000001BA)
		return 0;

	idx = b.pack_stuffing_length;
	pos = b.bit_offset_of(idx);
	leng = b.bit_size_of(idx);
	uint8_t pack_stuffing_length = get_value_8(buf, pos, leng);

	uint64_t system_header_pos = static_cast<uint64_t>(b.byte_size()) + pack_stuffing_length;
	uint64_t system_header_size = item_ProgramStreamSystemHeader::calc_item_size(image, system_header_pos);
	return system_header_pos + system_header_size;
}
