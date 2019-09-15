#include "pch.h"
#include "analyze_M2P_item_ProgramStreamSystemHeader.h"

#include "byteswap.h"
#include "analyze_M2P_util.h"

using namespace analyze_M2P;

class analyze_M2P::program_stream_system_header : public bitfield
{
public:
	program_stream_system_header()
	{
		v.push_back(bitpair(L"system_header_start_code", 32));
		v.push_back(bitpair(L"header_length", 16));
		v.push_back(bitpair(L"marker_bit", 1));
		v.push_back(bitpair(L"rate_bound", 22));
		v.push_back(bitpair(L"marker_bit", 1));
		v.push_back(bitpair(L"audio_bound", 6));
		v.push_back(bitpair(L"fixed_flag", 1));
		v.push_back(bitpair(L"CSPS_flag", 1));
		v.push_back(bitpair(L"system_audio_lock_flag", 1));
		v.push_back(bitpair(L"system_video_lock_flag", 1));
		v.push_back(bitpair(L"marker_bit", 1));
		v.push_back(bitpair(L"video_bound", 5));
		v.push_back(bitpair(L"packet_rate_restriction_flag", 1));
		v.push_back(bitpair(L"reserved_bits", 7));
	}

	virtual ~program_stream_system_header()
	{
	}

	enum
	{
		system_header_start_code,
		header_length,
		marker_bit_1,
		rate_bound,
		marker_bit_2,
		audio_bound,
		fixed_flag,
		CSPS_flag,
		system_audio_lock_flag,
		system_video_lock_flag,
		marker_bit_3,
		video_bound,
		packet_rate_restriction_flag,
		reserved_bits,
		last
	};
};






item_ProgramStreamSystemHeader::item_ProgramStreamSystemHeader(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:item_Base(image, offset, size)
{
	bitfield = new program_stream_system_header;
}

item_ProgramStreamSystemHeader::~item_ProgramStreamSystemHeader()
{
	delete bitfield;
}

HRESULT item_ProgramStreamSystemHeader::getItemName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	*str = agaliaString::create(L"system_header");
	return S_OK;
}

HRESULT item_ProgramStreamSystemHeader::getItemPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	uint32_t ret = prop_last;

	uint64_t size = bitfield->byte_size();
	while (1)
	{
		CHeapPtr<uint8_t> buf;

		if (!buf.AllocateBytes(2)) return E_OUTOFMEMORY;
		auto hr = image->ReadData(buf, getOffset() + size, 1);
		if (FAILED(hr))
			break;

		if (get_value_8(buf, 0, 1) != 1)
			break;
		ret += 1;
		if (get_value_8(buf, 0, 8) == 0b1011'0111) {
			ret += 7;
			size += 5;
		}
		else {
			ret += 3;
			size += 2;
		}
	}

	*count = ret;
	return S_OK;
}

HRESULT item_ProgramStreamSystemHeader::getItemPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	auto hr = __super::getItemPropName(index, str);
	if (SUCCEEDED(hr)) return hr;

	if (index == prop_system_header_start_code) {
		*str = agaliaString::create(bitfield->v[bitfield->system_header_start_code].first);
		return S_OK;
	}
	else if (index == prop_header_length) {
		*str = agaliaString::create(bitfield->v[bitfield->header_length].first);
		return S_OK;
	}
	else if (index == prop_rate_bound) {
		*str = agaliaString::create(bitfield->v[bitfield->rate_bound].first);
		return S_OK;
	}
	else if (index == prop_audio_bound) {
		*str = agaliaString::create(bitfield->v[bitfield->audio_bound].first);
		return S_OK;
	}
	else if (index == prop_fixed_flag) {
		*str = agaliaString::create(bitfield->v[bitfield->fixed_flag].first);
		return S_OK;
	}
	else if (index == prop_CSPS_flag) {
		*str = agaliaString::create(bitfield->v[bitfield->CSPS_flag].first);
		return S_OK;
	}
	else if (index == prop_system_audio_lock_flag) {
		*str = agaliaString::create(bitfield->v[bitfield->system_audio_lock_flag].first);
		return S_OK;
	}
	else if (index == prop_system_video_lock_flag) {
		*str = agaliaString::create(bitfield->v[bitfield->system_video_lock_flag].first);
		return S_OK;
	}
	else if (index == prop_video_bound) {
		*str = agaliaString::create(bitfield->v[bitfield->video_bound].first);
		return S_OK;
	}
	else if (index == prop_packet_rate_restriction_flag) {
		*str = agaliaString::create(bitfield->v[bitfield->packet_rate_restriction_flag].first);
		return S_OK;
	}
	else if (index == prop_reserved_bits) {
		*str = agaliaString::create(bitfield->v[bitfield->reserved_bits].first);
		return S_OK;
	}
	else
	{
		//TODO stream_idˆÈ~
	}


	return E_FAIL;
}

HRESULT item_ProgramStreamSystemHeader::getItemPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	auto hr = __super::getItemPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	program_stream_system_header* b = bitfield;

	const rsize_t read_size = b->byte_size();
	CHeapPtr<uint8_t> buf;
	if (!buf.AllocateBytes(read_size + 8)) return E_OUTOFMEMORY;
	hr = image->ReadData(buf, getOffset(), read_size);
	if (FAILED(hr)) return hr;

	std::wstringstream temp;

	if (index == prop_system_header_start_code)
	{
		int idx = b->system_header_start_code;
		rsize_t offset = b->bit_offset_of(idx);
		rsize_t leng = b->bit_size_of(idx);
		uint32_t val = get_value_32(buf, offset, leng);

		temp << L"0x";
		format_hex(temp, val);
	}
	else if (index == prop_header_length)
	{
		int idx = b->header_length;
		rsize_t offset = b->bit_offset_of(idx);
		rsize_t leng = b->bit_size_of(idx);
		uint16_t val = get_value_16(buf, offset, leng);

		format_dec(temp, val);
	}
	else if (index == prop_rate_bound)
	{
		int idx = b->rate_bound;
		rsize_t offset = b->bit_offset_of(idx);
		rsize_t leng = b->bit_size_of(idx);
		uint32_t val = get_value_32(buf, offset, leng);

		format_dec(temp, val);
	}
	else if (index == prop_audio_bound)
	{
		int idx = b->audio_bound;
		rsize_t offset = b->bit_offset_of(idx);
		rsize_t leng = b->bit_size_of(idx);
		uint8_t val = get_value_8(buf, offset, leng);

		format_dec(temp, val);
	}
	else if (index == prop_fixed_flag)
	{
		int idx = b->fixed_flag;
		rsize_t offset = b->bit_offset_of(idx);
		rsize_t leng = b->bit_size_of(idx);
		uint8_t val = get_value_8(buf, offset, leng);

		format_dec(temp, val);
	}
	else if (index == prop_CSPS_flag)
	{
		int idx = b->CSPS_flag;
		rsize_t offset = b->bit_offset_of(idx);
		rsize_t leng = b->bit_size_of(idx);
		uint8_t val = get_value_8(buf, offset, leng);

		format_dec(temp, val);
	}
	else if (index == prop_system_audio_lock_flag)
	{
		int idx = b->system_audio_lock_flag;
		rsize_t offset = b->bit_offset_of(idx);
		rsize_t leng = b->bit_size_of(idx);
		uint8_t val = get_value_8(buf, offset, leng);

		format_dec(temp, val);
	}
	else if (index == prop_system_video_lock_flag)
	{
		int idx = b->system_video_lock_flag;
		rsize_t offset = b->bit_offset_of(idx);
		rsize_t leng = b->bit_size_of(idx);
		uint8_t val = get_value_8(buf, offset, leng);

		format_dec(temp, val);
	}
	else if (index == prop_video_bound)
	{
		int idx = b->video_bound;
		rsize_t offset = b->bit_offset_of(idx);
		rsize_t leng = b->bit_size_of(idx);
		uint8_t val = get_value_8(buf, offset, leng);

		format_dec(temp, val);
	}
	else if (index == prop_packet_rate_restriction_flag)
	{
		int idx = b->packet_rate_restriction_flag;
		rsize_t offset = b->bit_offset_of(idx);
		rsize_t leng = b->bit_size_of(idx);
		uint8_t val = get_value_8(buf, offset, leng);

		format_dec(temp, val);
	}
	else if (index == prop_reserved_bits)
	{
		int idx = b->reserved_bits;
		rsize_t offset = b->bit_offset_of(idx);
		rsize_t leng = b->bit_size_of(idx);
		uint8_t val = get_value_8(buf, offset, leng);

		temp << L"0x";
		format_hex(temp, val);
	}
	else
	{
		//TODO stream_idˆÈ~
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}



HRESULT item_ProgramStreamSystemHeader::getGridRowCount(uint32_t* row) const
{
	if (row == nullptr) return E_POINTER;
	*row = bitfield->last;
	return S_OK;
}



HRESULT item_ProgramStreamSystemHeader::getGridValue(uint32_t row, uint32_t column, agaliaString** str) const
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



uint64_t item_ProgramStreamSystemHeader::calc_item_size(const agaliaContainer* image, uint64_t offset)
{
	program_stream_system_header b;

	CHeapPtr<uint8_t> buf;
	if (!buf.AllocateBytes(8)) return 0;
	auto hr = image->ReadData(buf, offset, 4);
	if (FAILED(hr)) return 0;

	uint32_t system_header_start_code = get_value_32(buf, 0, 32);
	if (system_header_start_code != 0x000001BB)
		return 0;
	buf.Free();

	uint64_t size = b.byte_size();
	while (1)
	{
		if (!buf.AllocateBytes(2)) return 0;
		hr = image->ReadData(buf, offset + size, 1);
		if (FAILED(hr))
			break;

		if (get_value_8(buf, 0, 1) != 1)
			break;
		size += 1;

		if (get_value_8(buf, 0, 8) == 0b1011'0111) {
			size += 5;
		}
		else {
			size += 2;
		}

		buf.Free();
	}

	return size;
}
