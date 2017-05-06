#pragma once

#include <stdint.h>

namespace agalia_tiff
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
	};

	// バイトーオーダー調整 関数オブジェクト（order が 'MM' のデータの読み込み） 
	class byte_coodinate_MM
	{
	public:
		uint16_t get_order(void) const { return 'MM'; }
		int8_t  operator ()(int8_t v) const { return v; }
		int16_t operator ()(int16_t v) const { return _byteswap_ushort(v); }
		int32_t operator ()(int32_t v) const { return _byteswap_ulong(v); }
		int64_t operator ()(int64_t v) const { return _byteswap_uint64(v); }
		uint8_t  operator ()(uint8_t v) const { return v; }
		uint16_t operator ()(uint16_t v) const { return _byteswap_ushort(v); }
		uint32_t operator ()(uint32_t v) const { return _byteswap_ulong(v); }
		uint64_t operator ()(uint64_t v) const { return _byteswap_uint64(v); }
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
		TIFFIFDENTRY entry[1];
		using next_offset_type = uint32_t;
	};

	struct TIFFHEADER8
	{
		uint16_t order;
		uint16_t version;
		uint16_t offset;
		uint16_t reserved;
		uint64_t offset8;

		decltype(order) get_order(void) const { return order; }
		decltype(version) get_version(void) const { return get_tiff_value(get_order(), version); }
		decltype(offset) get_offset(void) const { return get_tiff_value(get_order(), offset); }
		decltype(reserved) get_reserved(void) const { return get_tiff_value(get_order(), reserved); }
		decltype(offset8) get_offset8(void) const { return get_tiff_value(get_order(), offset8); }
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
		TIFFIFDENTRY8 entry[1];
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

	enum TIFFTYPE
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

	// 
	// TIFF構造体 ラッパー 
	// 

	// IFD Entry のラッパー 
	template <class T, class F>
	class ifd_entry_templ : private T
	{
	public:
		decltype(tag) get_tag(void) const { return F()(tag); }
		decltype(type) get_type(void) const { return F()(type); }
		decltype(count) get_count(void) const { return F()(count); }
		decltype(offset) get_offset(void) const { return F()(offset); }

		const T& get_raw_obj(void) const { return *this; }
	};
	// ラッパーを特殊化 
	template <class F> class ifd_entry_4 : public ifd_entry_templ<TIFFIFDENTRY, F> {};
	template <class F> class ifd_entry_8 : public ifd_entry_templ<TIFFIFDENTRY8, F> {};

	// IFD のラッパー 
	template<class T, class l_entry_wrapper, class F>
	class ifd_templ : protected T
	{
	public:
		using entry_wrapper = l_entry_wrapper;	// IFD Entry のラッパーの型情報を呼び出し側からも参照できるようにする 
		typedef typename T::next_offset_type next_offset_type;	// 基底クラスのエイリアスを参照できるようにする 

		// IFD に含まれる Entry の数を返す 
		decltype(count) get_count(void) const { return F()(count); }

		// 指定された要素番号の Entry を、ラッパー付きで返す 
		const entry_wrapper& get_entry(size_t index) const { return *reinterpret_cast<const entry_wrapper*>(&entry[index]); }

		// IFD の終端のアドレス、サイズを返す 
		const next_offset_type* get_next_offset_ptr() const { return reinterpret_cast<const next_offset_type*>(reinterpret_cast<const char*>(this) + sizeof(count) + sizeof(entry[0]) * get_count()); }
		unsigned int get_next_offset_size(void) const { return sizeof(next_offset_type); }
	};
	// ラッパーを特殊化 
	template <class F> class ifd_4 : public ifd_templ<TIFFIFD, ifd_entry_4<F>, F> {};
	template <class F> class ifd_8 : public ifd_templ<TIFFIFD8, ifd_entry_8<F>, F> {};

	// IFD Entry から値を取得する関数オブジェクト 
	template <typename T, class F> class get_ifd_entry_value
	{
	public:
		// 指定された型にキャストして、指定された要素番号の値を取得し、関数オブジェクトでバイトオーダーを調整 
		T operator ()(const void* p, size_t i) const
		{
			return F()(reinterpret_cast<const T*>(p)[i]);
		}
	};

	// IFD Entry から値を取得する（FLOAT型の特殊化） 
	template <class F> class get_ifd_entry_value<float, F>
	{
	public:
		float operator ()(const void* p, size_t i) const
		{
			uint32_t val = F()(reinterpret_cast<const uint32_t*>(p)[i]);
			return *reinterpret_cast<float*>(&val);
		}
	};

	// IFD Entry から値を取得する（DOUBLE型の特殊化） 
	template <class F> class get_ifd_entry_value<double, F>
	{
	public:
		double operator ()(const void* p, size_t i) const
		{
			uint64_t val = F()(reinterpret_cast<const uint64_t*>(p)[i]);
			return *reinterpret_cast<double*>(&val);
		}
	};

	// IFD Entry から値を取得する（RATIONAL型の特殊化） 
	template <class F> class get_ifd_entry_value<TIFFRATIONAL, F>
	{
	public:
		TIFFRATIONAL operator ()(const void* p, size_t i) const
		{
			get_ifd_entry_value<TIFFRATIONAL::type, F> g;
			return TIFFRATIONAL(g(p, i), g(p, i + 1));
		}
	};

	// IFD Entry から値を取得する（SRATIONAL型の特殊化） 
	template <class F> class get_ifd_entry_value<TIFFSRATIONAL, F>
	{
	public:
		TIFFSRATIONAL operator ()(const void* p, size_t i) const
		{
			get_ifd_entry_value<TIFFSRATIONAL::type, F> g;
			return TIFFSRATIONAL(g(p, i), g(p, i + 1));
		}
	};
}

// その他 

#include <iostream>
#include <unordered_map>

struct tif_dic_elem
{
	uint16_t parent;
	std::wstring name;
};

extern std::unordered_map<std::wstring, tif_dic_elem>* g_tif_dic;
typedef std::unordered_map<std::wstring, tif_dic_elem>::iterator tif_dic_iterator;

void init_tif_dictionary(_In_opt_ const wchar_t* lang);
void clear_tif_dictionary(void);
