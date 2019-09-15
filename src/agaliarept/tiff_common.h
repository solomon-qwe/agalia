#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "byteswap.h"

namespace analyze_TIFF
{
	// バイトーオーダー調整用 関数オブジェクト（order が 'II' のデータの読み込み） 
	class byte_coodinate_II
	{
	public:
		uint16_t get_order(void) const { return 'II'; }
		int8_t  operator ()(int8_t v) const { return v; }
		int16_t operator ()(int16_t v) const { return v; }
		int32_t operator ()(int32_t v) const { return v; }
		int64_t operator ()(int64_t v) const { return v; }
		uint8_t  operator ()(uint8_t v) const { return v; }
		uint16_t operator ()(uint16_t v) const { return v; }
		uint32_t operator ()(uint32_t v) const { return v; }
		uint64_t operator ()(uint64_t v) const { return v; }
		float operator ()(float v) const { return v; }
		double operator()(double v) const { return v; }
	};

	// バイトーオーダー調整 関数オブジェクト（order が 'MM' のデータの読み込み） 
	class byte_coodinate_MM
	{
	public:
		uint16_t get_order(void) const { return 'MM'; }
		int8_t  operator ()(int8_t v) const { return agalia_byteswap(v); }
		int16_t operator ()(int16_t v) const { return agalia_byteswap(v); }
		int32_t operator ()(int32_t v) const { return agalia_byteswap(v); }
		int64_t operator ()(int64_t v) const { return agalia_byteswap(v); }
		uint8_t  operator ()(uint8_t v) const { return agalia_byteswap(v); }
		uint16_t operator ()(uint16_t v) const { return agalia_byteswap(v); }
		uint32_t operator ()(uint32_t v) const { return agalia_byteswap(v); }
		uint64_t operator ()(uint64_t v) const { return agalia_byteswap(v); }
		float operator ()(float v) const { uint32_t t = operator()(*reinterpret_cast<uint32_t*>(&v)); return *reinterpret_cast<float*>(&t); }
		double operator ()(double v) const { uint64_t t = operator()(*reinterpret_cast<uint64_t*>(&v)); return *reinterpret_cast<double*>(&t); }
	};

	template <typename type> type get_tiff_value(uint16_t order, type v) { return order == 'MM' ? byte_coodinate_MM()(v) : v; }

	// 
	// TIFF構造体
	// ref. TIFF6.pdf, etc. 
	// 
#pragma pack(push, 2)

	struct TIFFHEADER
	{
		uint16_t order;
		uint16_t version;
		uint32_t offset;

		decltype(order) get_order(void) const { return order; }
		decltype(version) get_version(void) const { return get_tiff_value(get_order(), version); }
		decltype(offset) get_offset(void) const { return get_tiff_value(get_order(), offset); }
	};

	struct TIFFIFDENTRY
	{
		uint16_t tag;
		uint16_t type;
		uint32_t count;
		uint32_t offset;
	};

	struct TIFFIFD
	{
		uint16_t count;
		using entry_type = TIFFIFDENTRY;
		entry_type entry[1];
		using next_offset_type = uint32_t;
	};

	struct TIFFHEADER8
	{
		uint16_t order;
		uint16_t version;
		uint16_t offset_size;
		uint16_t reserved;
		uint64_t offset;

		decltype(order) get_order(void) const { return order; }
		decltype(version) get_version(void) const { return get_tiff_value(get_order(), version); }
		decltype(offset_size) get_offset_size(void) const { return get_tiff_value(get_order(), offset_size); }
		decltype(reserved) get_reserved(void) const { return get_tiff_value(get_order(), reserved); }
		decltype(offset) get_offset(void) const { return get_tiff_value(get_order(), offset); }
	};

	struct TIFFIFDENTRY8
	{
		uint16_t tag;
		uint16_t type;
		uint64_t count;
		uint64_t offset;
	};

	struct TIFFIFD8
	{
		uint64_t count;
		using entry_type = TIFFIFDENTRY8;
		entry_type entry[1];
		using next_offset_type = uint64_t;
	};

	struct TIFFRATIONAL
	{
		using type = uint32_t;
		type fraction;
		type denominator;

		TIFFRATIONAL(type s1, type s2) { fraction = s1; denominator = s2; }
	};

	struct TIFFSRATIONAL
	{
		using type = int32_t;
		type fraction;
		type denominator;

		TIFFSRATIONAL(type s1, type s2) { fraction = s1; denominator = s2; }
	};

#pragma pack(pop)

	typedef uint8_t TIFFBYTE;
	typedef uint8_t TIFFASCII;
	typedef uint16_t TIFFSHORT;
	typedef uint32_t TIFFLONG;
	typedef int8_t TIFFSBYTE;
	typedef uint8_t TIFFUNDEFINED;
	typedef int16_t TIFFSSHORT;
	typedef int32_t TIFFSLONG;
	typedef float TIFFFLOAT;
	typedef double TIFFDOUBLE;
	typedef uint64_t TIFFLONG8;
	typedef int64_t TIFFSLONG8;

	enum TIFFTYPEID
	{
		TYPE_BYTE = 1,
		TYPE_ASCII,
		TYPE_SHORT,
		TYPE_LONG,
		TYPE_RATIONAL,
		TYPE_SBYTE,
		TYPE_UNDEFINED,
		TYPE_SSHORT,
		TYPE_SLONG,
		TYPE_SRATIONAL,
		TYPE_FLOAT,
		TYPE_DOUBLE,
		TYPE_IFD,
		TYPE_LONG8 = 16,
		TYPE_SLONG8,
		TYPE_IFD8
	};
}

//// その他 

#include <iostream>
#include <unordered_map>


extern std::unordered_map<std::wstring, std::wstring>* g_tif_dic;
typedef std::unordered_map<std::wstring, std::wstring>::iterator tif_dic_iterator;

void init_tif_dictionary(_In_opt_ const wchar_t* lang);
void clear_tif_dictionary(void);
