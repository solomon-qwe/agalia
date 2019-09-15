#pragma once

#include <vector>
#include "byteswap.h"

typedef std::pair<const wchar_t*, rsize_t> bitpair;

namespace analyze_M2P
{
	class bitfield
	{
	public:
		bitfield() {}
		virtual ~bitfield() {}

		rsize_t bit_size(void)
		{
			rsize_t ret = 0;
			for (const auto& e : v)
				ret += e.second;
			return ret;
		}

		rsize_t byte_size(void)
		{
			return (bit_size() + 7) / 8;
		}

		rsize_t bit_offset_of(rsize_t index)
		{
			if (v.size() <= index) return 0;
			rsize_t ret = 0;
			for (rsize_t i = 0; i < index; i++)
				ret += v[i].second;
			return ret;
		}

		rsize_t bit_size_of(rsize_t index)
		{
			return v[index].second;
		}

		std::vector<bitpair> v;
	};

	inline uint8_t get_value_8(uint8_t* buf, rsize_t pos_bits, rsize_t leng_bits)
	{
		uint16_t temp = agalia_byteswap(*reinterpret_cast<decltype(temp)*>(buf + (pos_bits / 8)));
		uint8_t ret = (temp >> (16 - (pos_bits % 8) - leng_bits)) & ((1UL << leng_bits) - 1);
		return ret;
	}

	inline uint16_t get_value_16(uint8_t* buf, rsize_t pos_bits, rsize_t leng_bits)
	{
		uint32_t temp = agalia_byteswap(*reinterpret_cast<decltype(temp)*>(buf + (pos_bits / 8)));
		uint16_t ret = (temp >> (32 - (pos_bits % 8) - leng_bits)) & ((1UL << leng_bits) - 1);
		return ret;
	}

	inline uint32_t get_value_32(uint8_t* buf, rsize_t pos_bits, rsize_t leng_bits)
	{
		uint64_t temp = agalia_byteswap(*reinterpret_cast<decltype(temp)*>(buf + (pos_bits / 8)));
		uint32_t ret = (temp >> (64 - (pos_bits % 8) - leng_bits)) & ((1ULL << leng_bits) - 1);
		return ret;
	}
}