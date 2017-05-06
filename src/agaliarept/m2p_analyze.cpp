#include "stdafx.h"
#include "../inc/agaliarept.h"
#include "image_file.h"
#include "callback.h"

#include <assert.h>

void putDataAsByteArray(int subItem, const uint8_t* base_addr, uint64_t offset, uint64_t size, callback_cls* callback, const agalia::config* config);

const uint32_t packet_start_code_prefix = 0x000001;
const uint32_t packet_start_code = 0x000001BA;
const uint32_t system_header_start_code = 0x000001BB;
const uint32_t MPEG_program_end_code = 0x000001B9;

const uint8_t program_stream_map = 0b10111100;
const uint8_t private_stream_1 = 0b10111101;
const uint8_t padding_stream = 0b10111110;
const uint8_t private_stream_2 = 0b10111111;
const uint8_t ECM_stream = 0b11100000;
const uint8_t EMM_stream = 0b11110001;
const uint8_t Rec_ITU_T_H_222_0 = 0b11110010;
const uint8_t DSMCC_stream = 0b11110010;
const uint8_t Rec_ITU_T_H_222_1_TypeE = 0b11111000;
const uint8_t program_stream_directory = 0b11111111;


class bitstream
{
public:
	bitstream(const uint8_t* base, uint64_t data_size) {
		this->base = cur = base;
		this->end = base + data_size;
	}

	enum mode {
		get,
		peek
	};

	uint8_t get_value_8(size_t bits, mode mode = get)
	{
		assert(bits <= 8);
		if (end <= cur) return 0;

		uint16_t temp = 0;
		memcpy(&temp, cur, min(sizeof(temp), end - cur));
		temp = _byteswap_ushort(temp);
		uint8_t ret = (temp >> (16 - bit_offset - bits)) & ((1 << bits) - 1);

		if (mode == get)
		{
			bit_offset += bits;
			cur += bit_offset / 8;
			bit_offset = bit_offset % 8;
		}
		return ret;
	}

	uint16_t get_value_16(size_t bits, mode mode = get)
	{
		assert(bits <= 16);
		if (end <= cur) return 0;

		uint32_t temp = 0;
		memcpy(&temp, cur, min(sizeof(temp), end - cur));
		temp = _byteswap_ulong(temp);
		uint16_t ret = (temp >> (32 - bit_offset - bits)) & ((1UL << bits) - 1);

		if (mode == get)
		{
			bit_offset += bits;
			cur += bit_offset / 8;
			bit_offset = bit_offset % 8;
		}

		return ret;
	}

	uint32_t get_value_32(size_t bits, mode mode = get)
	{
		assert(bits <= 32);
		if (end <= cur) return 0;

		uint64_t temp = 0;
		memcpy(&temp, cur, min(sizeof(temp), end - cur));
		temp = _byteswap_uint64(temp);
		uint32_t ret = (temp >> (64 - bit_offset - bits)) & ((1ULL << bits) - 1);

		if (mode == get)
		{
			bit_offset += bits;
			cur += bit_offset / 8;
			bit_offset = bit_offset % 8;
		}

		return ret;
	}

	const uint8_t* base;	// 基準アドレス 
	const uint8_t* cur;		// 現在のアドレス 
	const uint8_t* end;		// アクセス可能領域＋１のアドレス  
	size_t bit_offset;
};


template <class T>
void print_bitfield_stub(const wchar_t* label, T value, size_t bits, int subItem, callback_cls* callback, bool enable_next, int fmtflags)
{
	callback->print(subItem, label);

	subItem++;
	{
		std::wstringbuf buf;
		std::wostream str(&buf, true);
		str << bits;
		callback->print(subItem, buf.str());
	}

	subItem++;
	{
		std::wstringbuf buf;
		std::wostream str(&buf, true);
		str.setf(fmtflags, std::ios::basefield);
		if (fmtflags == std::ios::hex) {
			str.setf(std::ios::uppercase);
			str << L"0x" << value;
		}
		else {
			str << value;
		}
		callback->print(subItem, buf.str());
	}

	if (enable_next) callback->next();
}


enum fmt
{
	dec = std::ios::dec,
	hex = std::ios::hex
};


template <class T>
T print_bitfield(const wchar_t* label, bitstream& bs, size_t bits, fmt fmt, int subItem, callback_cls* callback, bool enable_next)
{
}


template <>
uint8_t print_bitfield<uint8_t>(const wchar_t* label, bitstream& bs, size_t bits, fmt fmt, int subItem, callback_cls* callback, bool enable_next)
{
	assert(bits <= 8);
	uint8_t val = bs.get_value_8(bits);
	print_bitfield_stub(label, val, bits, subItem, callback, enable_next, (fmt == hex) ? std::ios::hex : std::ios::dec);
	return val;
}


template <>
uint16_t print_bitfield<uint16_t>(const wchar_t* label, bitstream& bs, size_t bits, fmt fmt, int subItem, callback_cls* callback, bool enable_next)
{
	assert(bits <= 16);
	uint16_t val = bs.get_value_16(bits);
	print_bitfield_stub(label, val, bits, subItem, callback, enable_next, (fmt == hex) ? std::ios::hex : std::ios::dec);
	return val;
}


void print_bitfield(const wchar_t* label, bitstream& bs, size_t bits, int subItem, callback_cls* callback, bool enable_next)
{
	if (bits <= 8) {
		print_bitfield_stub(label, bs.get_value_8(bits), bits, subItem, callback, enable_next, std::ios::dec);
	}
	else if (bits <= 16) {
		print_bitfield_stub(label, bs.get_value_16(bits), bits, subItem, callback, enable_next, std::ios::dec);
	}
	else if (bits <= 32) {
		print_bitfield_stub(label, bs.get_value_32(bits), bits, subItem, callback, enable_next, std::ios::dec);
	}
	else {
		assert(0);
	}
}


void print_bitfield_hex(const wchar_t* label, bitstream& bs, size_t bits, int subItem, callback_cls* callback, bool enable_next = false)
{
	if (bits <= 8) {
		print_bitfield_stub(label, bs.get_value_8(bits), bits, subItem, callback, enable_next, std::ios::hex);
	}
	else if (bits <= 16) {
		print_bitfield_stub(label, bs.get_value_16(bits), bits, subItem, callback, enable_next, std::ios::hex);
	}
	else if (bits <= 32) {
		print_bitfield_stub(label, bs.get_value_32(bits), bits, subItem, callback, enable_next, std::ios::hex);
	}
	else {
		assert(0);
	}
}


void parse_system_header(bitstream& bs, callback_cls* callback, const agalia::config* config)
{
	// offset 
	int subItem = 0;
	{
		wchar_t sz[16] = {};
		swprintf_s(sz, L"%.8zX", bs.cur - bs.base);
		callback->print(subItem, sz);
	}

	subItem++;

	print_bitfield_hex(L"system_header_start_code", bs, 32, subItem, callback, true);
	print_bitfield(L"header_length", bs, 16, subItem, callback, true);
	print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
	print_bitfield_hex(L"rate_bound", bs, 22, subItem, callback, true);
	print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
	print_bitfield_hex(L"audio_bound", bs, 6, subItem, callback, true);
	print_bitfield(L"fixed_flag", bs, 1, subItem, callback, true);
	print_bitfield(L"CSPS_flag", bs, 1, subItem, callback, true);
	print_bitfield(L"system_audio_lock_flag", bs, 1, subItem, callback, true);
	print_bitfield(L"system_video_lock_flag", bs, 1, subItem, callback, true);
	print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
	print_bitfield_hex(L"video_bound", bs, 5, subItem, callback, true);
	print_bitfield(L"packet_rate_restriction_flag", bs, 1, subItem, callback, true);
	print_bitfield_hex(L"reserved_bits", bs, 7, subItem, callback, true);

	while (bs.get_value_8(1, bs.peek))
	{
		print_bitfield_hex(L"stream_id", bs, 8, subItem, callback, true);
		print_bitfield(L"'11'", bs, 2, subItem, callback, true);
		print_bitfield_hex(L"P-STD_buffer_bound_scale", bs, 1, subItem, callback, true);
		print_bitfield_hex(L"P-STD_buffer_size_bound", bs, 13, subItem, callback, true);

		config->is_abort(agalia::config::throw_on_abort);
	}
}


void parse_pack_header(bitstream& bs, callback_cls* callback, const agalia::config* config)
{
	// offset 
	int subItem = 0;
	{
		wchar_t sz[16] = {};
		swprintf_s(sz, L"%.8zX", bs.cur - bs.base);
		callback->print(subItem, sz);
	}

	subItem++;

	print_bitfield_hex(L"pack_start_code", bs, 32, subItem, callback, true);
	print_bitfield(L"'01'", bs, 2, subItem, callback, true);
	print_bitfield_hex(L"system_clock_reference_base[32..30]", bs, 3, subItem, callback, true);
	print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
	print_bitfield_hex(L"system_clock_reference_base[29..15]", bs, 15, subItem, callback, true);
	print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
	print_bitfield_hex(L"system_clock_reference_base[14..0]", bs, 15, subItem, callback, true);
	print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
	print_bitfield_hex(L"system_clock_reference_extension", bs, 9, subItem, callback, true);
	print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
	print_bitfield_hex(L"program_mux_rate", bs, 22, subItem, callback, true);
	print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
	print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
	print_bitfield(L"Reserved", bs, 5, subItem, callback, true);
	uint8_t pack_stuff_length = print_bitfield<uint8_t>(L"pack_stuff_length", bs, 3, hex, subItem, callback, true);

	for (size_t i = 0; i < pack_stuff_length; i++)
	{
		print_bitfield_hex(L"stuffing_byte", bs, 8, subItem, callback, true);
	}

	if (bs.get_value_32(32, bs.peek) == system_header_start_code)
	{
		parse_system_header(bs, callback, config);
	}
}


void parse_PES_packet(bitstream& bs, callback_cls* callback, const agalia::config* config)
{
	// offset 
	int subItem = 0;
	{
		wchar_t sz[16] = {};
		swprintf_s(sz, L"%.8zX", bs.cur - bs.base);
		callback->print(subItem, sz);
	}

	subItem++;

	print_bitfield_hex(L"packet_start_code_prefix", bs, 24, subItem, callback, true);
	uint8_t stream_id = print_bitfield<uint8_t>(L"stream_id", bs, 8, hex, subItem, callback, true);
	uint16_t PES_packet_length = print_bitfield<uint16_t>(L"PES_packet_length", bs, 16, dec, subItem, callback, true);
	const uint8_t* packet_base = bs.cur;

	if (stream_id != program_stream_map
		&& stream_id != padding_stream
		&& stream_id != private_stream_2
		&& stream_id != ECM_stream
		&& stream_id != EMM_stream
		&& stream_id != program_stream_directory
		&& stream_id != DSMCC_stream
		&& stream_id != Rec_ITU_T_H_222_1_TypeE)
	{
		print_bitfield(L"'10'", bs, 2, subItem, callback, true);
		print_bitfield(L"PES_scrambling_control", bs, 2, subItem, callback, true);

		print_bitfield(L"PES_priority", bs, 1, subItem, callback, true);
		print_bitfield(L"data_alignment_indicator", bs, 1, subItem, callback, true);
		print_bitfield(L"copyright", bs, 1, subItem, callback, true);
		print_bitfield(L"original_or_copy", bs, 1, subItem, callback, true);
		uint8_t PTS_DTS_flags = print_bitfield<uint8_t>(L"PTS_DTS_flags", bs, 2, dec, subItem, callback, true);
		uint8_t ESCR_flag = print_bitfield<uint8_t>(L"ESCR_flag", bs, 1, dec, subItem, callback, true);
		uint8_t ES_rate_flag = print_bitfield<uint8_t>(L"ES_rate_flag", bs, 1, dec, subItem, callback, true);
		uint8_t DSM_trick_mode_flag = print_bitfield<uint8_t>(L"DSM_trick_mode_flag", bs, 1, dec, subItem, callback, true);
		uint8_t additional_copy_info_flag = print_bitfield<uint8_t>(L"additional_copy_info_flag", bs, 1, dec, subItem, callback, true);
		uint8_t PES_CRC_flag = print_bitfield<uint8_t>(L"PES_CRC_flag", bs, 1, dec, subItem, callback, true);
		uint8_t PES_extension_flag = print_bitfield<uint8_t>(L"PES_extension_flag", bs, 1, dec, subItem, callback, true);
		print_bitfield_hex(L"PES_header_data_length", bs, 8, subItem, callback, true);

		if (PTS_DTS_flags == 0b10)
		{
			print_bitfield(L"'0010'", bs, 4, subItem, callback, true);
			print_bitfield_hex(L"PTS [32..30]", bs, 3, subItem, callback, true);
			print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
			print_bitfield_hex(L"PTS [29..15]", bs, 15, subItem, callback, true);
			print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
			print_bitfield_hex(L"PTS [14..0]", bs, 15, subItem, callback, true);
			print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
		}

		if (PTS_DTS_flags == 0b11)
		{
			print_bitfield(L"'0010'", bs, 4, subItem, callback, true);
			print_bitfield_hex(L"PTS [32..30]", bs, 3, subItem, callback, true);
			print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
			print_bitfield_hex(L"PTS [29..15]", bs, 15, subItem, callback, true);
			print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
			print_bitfield_hex(L"PTS [14..0]", bs, 15, subItem, callback, true);
			print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
			print_bitfield(L"'0001'", bs, 4, subItem, callback, true);
			print_bitfield_hex(L"DTS [32..30]", bs, 3, subItem, callback, true);
			print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
			print_bitfield_hex(L"DTS [29..15]", bs, 15, subItem, callback, true);
			print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
			print_bitfield_hex(L"DTS [14..0]", bs, 15, subItem, callback, true);
			print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
		}

		if (ESCR_flag == 1)
		{
			print_bitfield(L"reserved", bs, 2, subItem, callback, true);
			print_bitfield_hex(L"ESCR_base[32..30]", bs, 3, subItem, callback, true);
			print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
			print_bitfield_hex(L"ESCR_base[29..15]", bs, 15, subItem, callback, true);
			print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
			print_bitfield_hex(L"ESCR_base[14..0]", bs, 15, subItem, callback, true);
			print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
			print_bitfield_hex(L"ESCR_extension", bs, 9, subItem, callback, true);
			print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
		}

		if (ES_rate_flag == 1)
		{
			print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
			print_bitfield_hex(L"ES_rate", bs, 22, subItem, callback, true);
			print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
		}

		if (DSM_trick_mode_flag == 1)
		{
			enum {
				fast_forward = 0b000,
				slow_motion = 0b001,
				freeze_frame = 0b010,
				fast_reverse = 0b011,
				slow_reverse = 0b100,
			};

			uint8_t trick_mode_control = print_bitfield<uint8_t>(L"trick_mode_control", bs, 3, hex, subItem, callback, true);
			if (trick_mode_control == fast_forward)
			{
				print_bitfield(L"field_id", bs, 2, subItem, callback, true);
				print_bitfield(L"intra_slice_refresh", bs, 1, subItem, callback, true);
				print_bitfield(L"frequency_truncation", bs, 2, subItem, callback, true);
			}
			else if (trick_mode_control == slow_motion)
			{
				print_bitfield(L"rep_cntrl", bs, 5, subItem, callback, true);
			}
			else if (trick_mode_control == freeze_frame)
			{
				print_bitfield(L"field_id", bs, 2, subItem, callback, true);
				print_bitfield(L"reserved", bs, 3, subItem, callback, true);
			}
			else if (trick_mode_control == fast_reverse)
			{
				print_bitfield(L"field_id", bs, 2, subItem, callback, true);
				print_bitfield(L"intra_slice_refresh", bs, 1, subItem, callback, true);
				print_bitfield(L"frequency_truncation", bs, 2, subItem, callback, true);
			}
			else if (trick_mode_control == slow_reverse)
			{
				print_bitfield_hex(L"rep_cntrl", bs, 5, subItem, callback, true);
			}
			else
			{
				print_bitfield_hex(L"reserved", bs, 5, subItem, callback, true);
			}
		}

		if (additional_copy_info_flag == 1)
		{
			print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
			print_bitfield_hex(L"additional_copy_info", bs, 7, subItem, callback, true);
		}

		if (PES_CRC_flag == 1)
		{
			print_bitfield_hex(L"previous_PES_packet_CRC", bs, 16, subItem, callback, true);
		}

		if (PES_extension_flag == 1)
		{
			uint8_t PES_private_data_flag = print_bitfield<uint8_t>(L"PES_private_data_flag", bs, 1, dec, subItem, callback, true);
			uint8_t pack_header_field_flag = print_bitfield<uint8_t>(L"pack_header_field_flag", bs, 1, dec, subItem, callback, true);
			uint8_t program_packet_sequence_counter_flag = print_bitfield<uint8_t>(L"program_packet_sequence_counter_flag", bs, 1, dec, subItem, callback, true);
			uint8_t P_STD_buffer_flag = print_bitfield<uint8_t>(L"P-STD_buffer_flag", bs, 1, dec, subItem, callback, true);
			print_bitfield_hex(L"reserved", bs, 3, subItem, callback, true);
			uint8_t PES_extension_flag_2 = print_bitfield<uint8_t>(L"PES_extension_flag_2", bs, 1, dec, subItem, callback, true);
			if (PES_private_data_flag == 1) {
				print_bitfield_hex(L"PES_private_data", bs, 32, subItem, callback, true);
				print_bitfield_hex(L"PES_private_data", bs, 32, subItem, callback, true);
				print_bitfield_hex(L"PES_private_data", bs, 32, subItem, callback, true);
				print_bitfield_hex(L"PES_private_data", bs, 32, subItem, callback, true);
			}

			if (pack_header_field_flag == 1)
			{
				print_bitfield_hex(L"pack_field_length", bs, 8, subItem, callback, true);
				parse_pack_header(bs, callback, config);
			}

			if (program_packet_sequence_counter_flag == 1)
			{
				print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
				print_bitfield_hex(L"program_packet_sequence_counter", bs, 7, subItem, callback, true);
				print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
				print_bitfield(L"MPEG1_MPEG2_identifier", bs, 1, subItem, callback, true);
				print_bitfield_hex(L"original_stuff_length", bs, 6, subItem, callback, true);
			}

			if (P_STD_buffer_flag == 1)
			{
				print_bitfield(L"'01'", bs, 2, subItem, callback, true);
				print_bitfield_hex(L"'P-STD_buffer_scale'", bs, 1, subItem, callback, true);
				print_bitfield_hex(L"'P-STD_buffer_size'", bs, 13, subItem, callback, true);
			}

			if (PES_extension_flag_2 == 1)
			{
				print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
				uint8_t PES_extension_field_length = print_bitfield<uint8_t>(L"PES_extension_field_length", bs, 7, hex, subItem, callback, true);
				uint8_t stream_id_extension_flag = print_bitfield<uint8_t>(L"stream_id_extension_flag", bs, 1, dec, subItem, callback, true);
				if (stream_id_extension_flag == 0)
				{
					print_bitfield(L"stream_id_extension", bs, 7, subItem, callback, true);
				}
				else
				{
					print_bitfield(L"reserved", bs, 6, subItem, callback, true);
					uint8_t tref_extension_flag = print_bitfield<uint8_t>(L"tref_extension_flag", bs, 1, dec, subItem, callback, true);
					if (tref_extension_flag == 0)
					{
						print_bitfield(L"reserved", bs, 4, subItem, callback, true);
						print_bitfield_hex(L"TREF[32..30]", bs, 3, subItem, callback, true);
						print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
						print_bitfield_hex(L"TREF[29..15]", bs, 15, subItem, callback, true);
						print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
						print_bitfield_hex(L"TREF[14..0]", bs, 15, subItem, callback, true);
						print_bitfield(L"marker_bit", bs, 1, subItem, callback, true);
					}
					for (int i = 0; i < 3; i++)
					{
						print_bitfield(L"reserved", bs, 8, subItem, callback, true);
					}
				}

				for (int i = 0; i < PES_extension_field_length; i++)
				{
					print_bitfield(L"reserved", bs, 8, subItem, callback, true);
				}
			}
		}

		{
			size_t count = 0;
			size_t pos = bs.cur - bs.base;
			while (bs.base[pos++] == 0xFF)
			{
				count++;
			}

			callback->print(subItem, L"stuffing_byte");
			putDataAsByteArray(subItem + 2, bs.base, bs.cur - bs.base, count, callback, config);
			callback->next();
			bs.cur += count;
		}

		{
			callback->print(subItem, L"PES_packet_data_byte");
			putDataAsByteArray(subItem + 2, bs.base, bs.cur - bs.base, PES_packet_length - (bs.cur - packet_base), callback, config);
			callback->next();
			bs.cur += PES_packet_length - (bs.cur - packet_base);
		}
	}
	else if (stream_id == program_stream_map
		|| stream_id == private_stream_2
		|| stream_id == ECM_stream
		|| stream_id == EMM_stream
		|| stream_id == program_stream_directory
		|| stream_id == DSMCC_stream
		|| stream_id == Rec_ITU_T_H_222_1_TypeE)
	{
		callback->print(subItem, L"PES_packet_data_byte");
		putDataAsByteArray(subItem + 2, bs.base, bs.cur - bs.base, PES_packet_length, callback, config);
		callback->next();
		bs.cur += PES_packet_length;
	}
	else if (stream_id == padding_stream)
	{
		callback->print(subItem, L"padding_byte");
		putDataAsByteArray(subItem + 2, bs.base, bs.cur - bs.base, PES_packet_length, callback, config);
		callback->next();
		bs.cur += PES_packet_length;
	}
}


void parse_MPEG_program_end_code(bitstream& bs, callback_cls* callback)
{
	// offset 
	int subItem = 0;
	{
		wchar_t sz[16] = {};
		swprintf_s(sz, L"%.8zX", bs.cur - bs.base);
		callback->print(subItem, sz);
	}

	subItem++;

	print_bitfield_hex(L"MPEG2_program_end_code", bs, 32, subItem, callback, true);
}


void MPEG2_program_stream(bitstream& bs, callback_cls* callback, const agalia::config* config)
{
	while (bs.cur < bs.end && bs.get_value_32(24, bs.peek) == packet_start_code_prefix)
	{
		switch (bs.get_value_32(32, bs.peek))
		{
		case packet_start_code:
			parse_pack_header(bs, callback, config);
			break;

		case MPEG_program_end_code:
			parse_MPEG_program_end_code(bs, callback);
			break;

		default:
			parse_PES_packet(bs, callback, config);
		}

		config->is_abort(agalia::config::throw_on_abort);
	}
}


bool image_file_m2p::is_supported(const void* buf, uint64_t size)
{
	if (buf == nullptr) return false;
	if (size < 4) return false;

	uint32_t pack_start_code = *reinterpret_cast<const uint32_t*>(buf);

	if (_byteswap_ulong(pack_start_code) == 0x000001BA)
	{
		return true;
	}
	return false;
}


void image_file_m2p::parse(callback_cls* callback, const agalia::config* config)
{
	callback->insert_column(96, L"offset");
	callback->insert_column(320, L"field");
	callback->insert_column(64, L"bits");
	callback->insert_column(320, L"value");
	callback->next();

	bitstream bs(this->base_addr + this->base_offset, this->data_size);
	MPEG2_program_stream(bs, callback, config);
}
