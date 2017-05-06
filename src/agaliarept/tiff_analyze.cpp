#include "stdafx.h"
#include "tiff_common.h"
using namespace agalia_tiff;

#include <stdio.h>
#include <stdio.h>
#include <assert.h>
#include <sstream>

#include "callback.h"
#include "util.h"


// プリミティブ型を文字列化 
// type : IFD Entryの値の型 
// size : 中間バッファの文字数 
// format : 文字列化フォーマット 
// str : 出力先 
// addr : 値の配列の先頭アドレス 
// index : 取得する値の要素番号 
template<typename type, class F, size_t size>
void print_value_primitive(const wchar_t* format, std::wstring& str, const void* addr, unsigned int index)
{
	wchar_t buf[size] = {};
	swprintf_s(buf, format, get_ifd_entry_value<type, F>()(addr, index));
	str += buf;
}

// 分数型を文字列化 
template<typename type, class F, size_t size>
void print_value_rational(const wchar_t* format, std::wstring& str, const void* addr, unsigned int index)
{
	wchar_t buf[size] = {};
	type val = get_ifd_entry_value<type, F>()(addr, index);
	swprintf_s(buf, format, val.fraction, val.denominator);
	str += buf;
}

namespace agalia_tiff
{
	template <class F> class tiff_type
	{
	public:
		virtual size_t getTypeSize(void) const = 0;
		virtual uint16_t getTypeValue(void) const = 0;
		virtual const wchar_t* getTypeName(void) const = 0;
		virtual void printValue(std::wstring& str, const void* addr, unsigned int index) const = 0;
	};

	template <class F> class tiff_type_other : public tiff_type<F>
	{
	public:
		tiff_type_other(uint16_t type_value) { typeval = type_value; }
		virtual size_t getTypeSize(void) const { return 0; }
		virtual uint16_t getTypeValue(void) const { return typeval; }
		virtual const wchar_t* getTypeName(void) const { return nullptr; }
		virtual void printValue(std::wstring& /*str*/, const void* /*addr*/, unsigned int /*index*/) const {}

	private:
		uint16_t typeval;
	};

	template<typename T, uint16_t type_value, class F> class tiff_type_templ : public tiff_type<F>
	{
	public:
		using type = T;	// 派生クラス側でも型情報を使うので using しておく 

		virtual size_t getTypeSize(void) const { return sizeof(type); }
		virtual uint16_t getTypeValue(void) const { return type_value; }

	protected:
		get_ifd_entry_value<type, F> fanctor;
	};

	template <class F> class tiff_type_ascii : public tiff_type_templ<int8_t, TYPE_ASCII, F>
	{
	public:
		virtual const wchar_t* getTypeName(void) const { return L"ASCII"; }
		virtual void printValue(std::wstring& str, const void* addr, unsigned int index) const { print_value_primitive<type, F, 16>(L"%C", str, addr, index); }
	};

	template <class F> class tiff_type_undefined : public tiff_type_templ<uint8_t, TYPE_UNDEFINED, F>
	{
	public:
		virtual const wchar_t* getTypeName(void) const { return L"UNDEFINED"; }
		virtual void printValue(std::wstring& str, const void* addr, unsigned int index) const { print_value_primitive<type, F, 8>(L"%.2X", str, addr, index); }
	};

	template <class F> class tiff_type_sbyte : public tiff_type_templ<int8_t, TYPE_SBYTE, F>
	{
	public:
		virtual const wchar_t* getTypeName(void) const { return L"SBYTE"; }
		virtual void printValue(std::wstring& str, const void* addr, unsigned int index) const { print_value_primitive<type, F, 16>(L"%d", str, addr, index); }
	};

	template <class F> class tiff_type_byte : public tiff_type_templ<uint8_t, TYPE_BYTE, F>
	{
	public:
		virtual const wchar_t* getTypeName(void) const { return L"BYTE"; }
		virtual void printValue(std::wstring& str, const void* addr, unsigned int index) const { print_value_primitive<type, F, 16>(L"%u", str, addr, index); }
	};

	template <class F> class tiff_type_sshort : public tiff_type_templ<int16_t, TYPE_SSHORT, F>
	{
	public:
		virtual const wchar_t* getTypeName(void) const { return L"SSHORT"; }
		virtual void printValue(std::wstring& str, const void* addr, unsigned int index) const { print_value_primitive<type, F, 16>(L"%d", str, addr, index); }
	};

	template <class F> class tiff_type_short : public tiff_type_templ<uint16_t, TYPE_SHORT, F>
	{
	public:
		virtual const wchar_t* getTypeName(void) const { return L"SHORT"; }
		virtual void printValue(std::wstring& str, const void* addr, unsigned int index) const { print_value_primitive<type, F, 16>(L"%u", str, addr, index); }
	};

	template <class F> class tiff_type_slong : public tiff_type_templ<int32_t, TYPE_SLONG, F>
	{
	public:
		virtual const wchar_t* getTypeName(void) const { return L"SLONG"; }
		virtual void printValue(std::wstring& str, const void* addr, unsigned int index) const { print_value_primitive<type, F, 16>(L"%d", str, addr, index); }
	};

	template <class F> class tiff_type_long : public tiff_type_templ<uint32_t, TYPE_LONG, F>
	{
	public:
		virtual const wchar_t* getTypeName(void) const { return L"LONG"; }
		virtual void printValue(std::wstring& str, const void* addr, unsigned int index) const { print_value_primitive<type, F, 16>(L"%u", str, addr, index); }
	};

	template <class F> class tiff_type_slong8 : public tiff_type_templ<int64_t, TYPE_SLONG8, F>
	{
	public:
		virtual const wchar_t* getTypeName(void) const { return L"SLONG8"; }
		virtual void printValue(std::wstring& str, const void* addr, unsigned int index) const { print_value_primitive<type, F, 128>(L"%I64d", str, addr, index); }
	};

	template <class F> class tiff_type_long8 : public tiff_type_templ<uint64_t, TYPE_LONG8, F>
	{
	public:
		virtual const wchar_t* getTypeName(void) const { return L"LONG8"; }
		virtual void printValue(std::wstring& str, const void* addr, unsigned int index) const { print_value_primitive<type, F, 128>(L"%I64u", str, addr, index); }
	};

	template <class F> class tiff_type_float : public tiff_type_templ<float, TYPE_FLOAT, F>
	{
	public:
		virtual const wchar_t* getTypeName(void) const { return L"FLOAT"; }
		virtual void printValue(std::wstring& str, const void* addr, unsigned int index) const { print_value_primitive<type, F, 16>(L"%.7e", str, addr, index); }
	};

	template <class F> class tiff_type_double : public tiff_type_templ<double, TYPE_DOUBLE, F>
	{
	public:
		virtual const wchar_t* getTypeName(void) const { return L"DOUBLE"; }
		virtual void printValue(std::wstring& str, const void* addr, unsigned int index) const { print_value_primitive<type, F, 32>(L"%.14e", str, addr, index); }
	};

	template <class F> class tiff_type_srational : public tiff_type_templ<TIFFSRATIONAL, TYPE_SRATIONAL, F>
	{
	public:
		virtual const wchar_t* getTypeName(void) const { return L"SRATIONAL"; }
		virtual void printValue(std::wstring& str, const void* addr, unsigned int index) const { print_value_rational<type, F, 64>(L"%d/%d", str, addr, index); }
	};

	template <class F> class tiff_type_rational : public tiff_type_templ<TIFFRATIONAL, TYPE_RATIONAL, F>
	{
	public:
		virtual const wchar_t* getTypeName(void) const { return L"RATIONAL"; }
		virtual void printValue(std::wstring& str, const void* addr, unsigned int index) const { print_value_rational<type, F, 64>(L"%u/%u", str, addr, index); }
	};

	template <class F> class tiff_type_ifd : public tiff_type_templ<uint32_t, TYPE_IFD, F>
	{
	public:
		virtual const wchar_t* getTypeName(void) const { return L"IFD"; }
		virtual void printValue(std::wstring& str, const void* addr, unsigned int index) const { print_value_primitive<type, F, 16>(L"%u", str, addr, index); }
	};

	template <class F> class tiff_type_ifd8 : public tiff_type_templ<uint64_t, TYPE_IFD8, F>
	{
	public:
		virtual const wchar_t* getTypeName(void) const { return L"IFD8"; }
		virtual void printValue(std::wstring& str, const void* addr, unsigned int index) const { print_value_primitive<type, F, 128>(L"%I64u", str, addr, index); }
	};
}

template <class F>
tiff_type<F>* get_tiff_type_obj(uint16_t type)
{
	switch (type)
	{
	case TYPE_BYTE:		return new tiff_type_byte<F>;
	case TYPE_ASCII:	return new tiff_type_ascii<F>;
	case TYPE_SHORT:	return new tiff_type_short<F>;
	case TYPE_LONG:		return new tiff_type_long<F>;
	case TYPE_RATIONAL:	return new tiff_type_rational<F>;
	case TYPE_SBYTE:	return new tiff_type_sbyte<F>;
	case TYPE_UNDEFINED:return new tiff_type_undefined<F>;
	case TYPE_SSHORT:	return new tiff_type_sshort<F>;
	case TYPE_SLONG:	return new tiff_type_slong<F>;
	case TYPE_SRATIONAL:return new tiff_type_srational<F>;
	case TYPE_FLOAT:	return new tiff_type_float<F>;
	case TYPE_DOUBLE:	return new tiff_type_double<F>;
	case TYPE_IFD:		return new tiff_type_ifd<F>;
	case TYPE_LONG8:	return new tiff_type_long8<F>;
	case TYPE_SLONG8:	return new tiff_type_slong8<F>;
	case TYPE_IFD8:		return new tiff_type_ifd8<F>;
	}
	return new tiff_type_other<F>(type);
}

template <size_t size> void local_wprintf_d(wchar_t(&sz)[size], uint32_t val) { swprintf_s(sz, L"%u", val); }
template <size_t size> void local_wprintf_d(wchar_t(&sz)[size], uint64_t val) { swprintf_s(sz, L"%I64u", val); }
template <size_t size> void local_wprintf_x(wchar_t(&sz)[size], uint32_t val) { swprintf_s(sz, L"%.8X", val); }
template <size_t size> void local_wprintf_x(wchar_t(&sz)[size], uint64_t val) { swprintf_s(sz, L"%.16I64X", val); }

static void print_tiff_offset(int subItem, uint64_t offset, callback_cls* callback)
{
	wchar_t sz[16] = {};
	swprintf_s(sz, L"%.8I64X", offset);
	callback->print(subItem, sz);
}

void print_tiff_tag(int subItem, uint16_t tag, callback_cls* callback)
{
	wchar_t sz[16] = {};
	swprintf_s(sz, L"%.4X", tag);
	callback->print(subItem, sz);
}

void print_tiff_tag_name(int subItem, uint16_t tag, uint16_t parent, callback_cls* callback)
{
	wchar_t sz[16] = {};
	swprintf_s(sz, L"%.4X", tag);
	std::unordered_map<std::wstring, tif_dic_elem>::iterator it = g_tif_dic->find(sz);

	while (it != g_tif_dic->end())
	{
		if (it->second.parent == parent) {
			callback->print(subItem, it->second.name.c_str());
			break;
		}
		it++;
	}
}

template<class F>
void print_tiff_type(int subItem, const tiff_type<F>* tiff_type_obj, callback_cls* callback)
{
	wchar_t sz[16] = {};
	const wchar_t* pType = tiff_type_obj->getTypeName();
	if (pType == nullptr)
	{
		swprintf_s(sz, L"%.4X", tiff_type_obj->getTypeValue());
		pType = sz;
	}

	callback->print(subItem, pType);
}

template<typename T>
void print_tiff_count(int subItem, T val, callback_cls* callback)
{
	wchar_t sz[128] = {};
	local_wprintf_d<_countof(sz)>(sz, val);

	callback->print(subItem, sz);
}

template<typename T>
void print_tiff_data(int subItem, T val, callback_cls* callback)
{
	wchar_t sz[128] = {};
	local_wprintf_x<_countof(sz)>(sz, val);

	callback->print(subItem, sz);
}

template <class F>
void print_tiff_value(int subItem, uint64_t counts, const void* addr, tiff_type<F>* tiff_type_obj, callback_cls* callback, const agalia::config* config)
{
	std::wstring str;
	const size_t limit = config->byte_stream_limit_length;
	str.reserve(limit * 3 + 8);

	if (counts != 0)
	{
		if (tiff_type_obj->getTypeValue() == TYPE_ASCII)
		{
			str += L"\"";
			tiff_type_obj->printValue(str, addr, 0);

			for (unsigned int i = 1; i < min(counts, limit); i++)
			{
				tiff_type_obj->printValue(str, addr, i);
			}

			if (limit < counts) {
				str += L"...";
			}
			str += L"\"";
		}
		else if (tiff_type_obj->getTypeSize() != 0)
		{
			tiff_type_obj->printValue(str, addr, 0);

			for (unsigned int i = 1; i < min(counts, limit); i++)
			{
				str += L", ";
				tiff_type_obj->printValue(str, addr, i);
			}

			if (limit < counts) {
				str += L",...";
			}
		}
	}

	callback->print(subItem, str);
}

template<class ifdtype>
const void* getDataAddr(const void* base, const ifdtype& entry, size_t value_size)
{
	const void* p = nullptr;
	if (value_size * entry.get_count() <= sizeof(ifdtype::offset))
	{	// 値の総サイズが IFD Entry に収まる場合 
		p = &entry.get_raw_obj().offset;
	}
	else
	{	// IFD Entry の外に値がある場合 
		p = reinterpret_cast<const char*>(base) + entry.get_offset();
	}
	return p;
}

template<typename directory, class F>
void parseNextIFD(const uint8_t* base_addr, uint64_t base_offset, uint64_t endPos, const wchar_t* ifdname, uint16_t parent, const directory* pDir, callback_cls* callback, const agalia::config* config)
{
	const directory::next_offset_type* next_offset_ptr = pDir->get_next_offset_ptr();
	uint64_t offset = reinterpret_cast<const uint8_t*>(next_offset_ptr) - base_addr + base_offset;

	// offset 
	int subItem = 0;
	print_tiff_offset(subItem, offset, callback);

	// IFD 
	callback->print(++subItem, ifdname);

	// Tag 
	wchar_t sz[128] = {};
	swprintf_s(sz, pDir->get_next_offset_size() == 8 ? L"NEXTIFD8" : L"NEXTIFD");
	callback->print(++subItem, sz);

	// Name 
	callback->print(++subItem, L"NextIFD");

	// Type, Count 
	++subItem;
	++subItem;

	// Value Offset 
	directory::next_offset_type next_offset = F()(*next_offset_ptr);
	print_tiff_data(++subItem, next_offset, callback);

	// data 
	++subItem;

	// value 
	local_wprintf_d(sz, next_offset);
	callback->print(++subItem, sz);

	callback->next();

	if (next_offset == 0) {
	}
	else if (endPos <= next_offset) {
		// ★ ファイルが壊れている 
	}
	else
	{
		std::wstring nextIFDName = ifdname;
		if (nextIFDName == L"0th IFD") { nextIFDName = L"1st IFD"; }
		else if (nextIFDName == L"1st IFD") { nextIFDName = L"2nd IFD"; }
		else if (nextIFDName == L"2nd IFD") { nextIFDName = L"3rd IFD"; }
		else if (nextIFDName == L"3rd IFD") { nextIFDName = L"4th IFD"; }
		else { nextIFDName = L"Xth IFD"; }

		const directory* pNextIFD = reinterpret_cast<const directory*>(base_addr + base_offset + next_offset);
		parseTiffDirectory<directory, F>(base_addr, base_offset, endPos, nextIFDName.c_str(), parent, pNextIFD, callback, config);
	}
}

template<typename directory, class F, class tiff_type_ifd_cls>
void parseSubIFD(const uint8_t* base_addr, uint64_t base_offset, uint64_t endPos, const wchar_t* ifdname, uint16_t parent, const void* value_addr, callback_cls* callback, const agalia::config* config)
{
	if (reinterpret_cast<const uint8_t*>(value_addr) <= base_addr + base_offset ||
		endPos <= static_cast<size_t>(reinterpret_cast<const uint8_t*>(value_addr) - base_addr + base_offset))
	{
		return;
	}

	tiff_type_ifd_cls::type sub_ifd_offset = get_ifd_entry_value<tiff_type_ifd_cls::type, F>()(value_addr, 0);
	if (sub_ifd_offset != 0)
	{
		const directory* pSubIFD = reinterpret_cast<const directory*>(base_addr + base_offset + sub_ifd_offset);
		parseTiffDirectory<directory, F>(base_addr, base_offset, endPos, ifdname, parent, pSubIFD, callback, config);
	}
}

template<typename directory, class F>
void parseTiffDirectory(const uint8_t* base_addr, uint64_t base_offset, uint64_t endPos, const wchar_t* ifdname, uint16_t parent, const directory* pDir, callback_cls* callback, const agalia::config* config)
{
	if (reinterpret_cast<const uint8_t*>(pDir) <= base_addr + base_offset ||
		endPos <= static_cast<size_t>(reinterpret_cast<const uint8_t*>(pDir) - base_addr + base_offset))
	{
		return;
	}

	for (unsigned int i = 0; i < pDir->get_count(); i++)
	{
		config->is_abort(agalia::config::throw_on_abort);

		// IFD Entry を取得 
		const directory::entry_wrapper& entry = pDir->get_entry(i);
		// 先頭からのオフセット 
		uint64_t offset = reinterpret_cast<const uint8_t*>(&entry) - base_addr - base_offset;
		// TIFFの型情報クラス 
		agalia_ptr<tiff_type<F>> tiff_type_obj;
		tiff_type_obj.p = get_tiff_type_obj<F>(entry.get_type());
		// 値の格納されているアドレス 
		const void* value_addr = getDataAddr(base_addr + base_offset, entry, tiff_type_obj.p->getTypeSize());

		// offset 
		int subItem = 0;
		print_tiff_offset(subItem, offset, callback);

		// IFD 
		callback->print(++subItem, ifdname);

		// Tag 
		print_tiff_tag(++subItem, entry.get_tag(), callback);

		// Name 
		print_tiff_tag_name(++subItem, entry.get_tag(), parent, callback);

		// Type 
		print_tiff_type(++subItem, tiff_type_obj.p, callback);

		// Count 
		print_tiff_count(++subItem, entry.get_count(), callback);

		// Value Offset
		print_tiff_data(++subItem, entry.get_offset(), callback);

		if (reinterpret_cast<const uint8_t*>(value_addr) < base_addr) {
			continue;
		}

		uint64_t pos = reinterpret_cast<const uint8_t*>(value_addr) - base_addr;
		uint64_t size = tiff_type_obj.p->getTypeSize() * (size_t)entry.get_count();

		if (pos < base_offset || endPos < pos + size || _UI64_MAX - endPos < size) {
			// IFD の OFFSET の指しているデータが、ファイルの範囲を超えている 
			continue;
		}

		// data 
		void putDataAsByteArray(int subItem, const void* srcBuf, uint64_t srcLength, callback_cls* callback, const agalia::config* config);
		putDataAsByteArray(++subItem, value_addr, size, callback, config);

		// value 
		++subItem;
		if (entry.get_count() <= 0xFFFFFFFF)
		{
			print_tiff_value(subItem, entry.get_count(), value_addr, tiff_type_obj.p, callback, config);
		}

		callback->set_extra_data(pos, size);

		callback->next();

		tiff_type_obj.cleanup();

		if (entry.get_tag() == 0x8769)
		{
			const wchar_t subIFDName[] = L"Exif IFD";
			parseSubIFD<directory, F, tiff_type_ifd<F>>(base_addr, base_offset, endPos, subIFDName, entry.get_tag(), value_addr, callback, config);
		}
		else if (entry.get_tag() == 0x8825)
		{
			const wchar_t subIFDName[] = L"GPSInfo IFD";
			parseSubIFD<directory, F, tiff_type_ifd<F>>(base_addr, base_offset, endPos, subIFDName, entry.get_tag(), value_addr, callback, config);
		}
		else if (entry.get_tag() == 0xA005)
		{
			const wchar_t subIFDName[] = L"Interoperability IFD";
			parseSubIFD<directory, F, tiff_type_ifd<F>>(base_addr, base_offset, endPos, subIFDName, entry.get_tag(), value_addr, callback, config);
		}
		else
		{
			const wchar_t subIFDName[] = L"Sub IFD";
			switch (entry.get_type())
			{
			case TIFFTYPE::TYPE_IFD:
				parseSubIFD<directory, F, tiff_type_ifd<F>>(base_addr, base_offset, endPos, subIFDName, entry.get_tag(), value_addr, callback, config);
				break;
			case TIFFTYPE::TYPE_IFD8:
				parseSubIFD<directory, F, tiff_type_ifd8<F>>(base_addr, base_offset, endPos, subIFDName, entry.get_tag(), value_addr, callback, config);
				break;
			}
		}
	}

	parseNextIFD<directory, F>(base_addr, base_offset, endPos, ifdname, parent, pDir, callback, config);
}

void analyzeTiffFile(_In_ const uint8_t* base_addr, _In_ uint64_t base_offset, _In_ uint64_t data_size, callback_cls* callback, const agalia::config* config)
{
	bool isExif = (memcmp(base_addr + base_offset, "Exif\0\0", 6) == 0) ? true : false;
	uint64_t offset = base_offset + (isExif ? 6 : 0);
	uint64_t endPos = base_offset + data_size;

	const uint8_t* base = base_addr + offset;
	const TIFFHEADER* pTiff = reinterpret_cast<const TIFFHEADER*>(base);

	const wchar_t ifdname[] = L"0th IFD";
	uint16_t parent = 0;
	switch (pTiff->get_version())
	{
	case 0x2A:	// TIFF 
	case 'OR':	// OLYMPUS RAW 
		//wprintf_s(L"%.4x, %.4x, %.8x\n\n", pTiff->get_order(), pTiff->get_version(), pTiff->get_offset());
		if (pTiff->get_order() == 'MM')
		{
			using F = byte_coodinate_MM;
			using IFD = ifd_4<F>;
			parseTiffDirectory<IFD, F>(base_addr, offset, endPos, ifdname, parent, reinterpret_cast<const IFD*>(base + pTiff->get_offset()), callback, config);
		}
		else
		{
			using F = byte_coodinate_II;
			using IFD = ifd_4<F>;
			parseTiffDirectory<IFD, F>(base_addr, offset, endPos, ifdname, parent, reinterpret_cast<const IFD*>(base + pTiff->get_offset()), callback, config);
		}
		break;

	case 0x2B:	// BigTIFF 
		const TIFFHEADER8* pTiff8 = reinterpret_cast<const TIFFHEADER8*>(pTiff);
		//wprintf_s(L"%.4x, %.4x, %.4x, %.4x, %.16I64x\n\n", pTiff8->get_order(), pTiff8->get_version(), pTiff8->get_offset(), pTiff8->get_reserved(), pTiff8->get_offset8());
		if (pTiff->get_order() == 'MM')
		{
			using F = byte_coodinate_MM;
			using IFD = ifd_8<F>;
			parseTiffDirectory<IFD, F>(base_addr, offset, endPos, ifdname, parent, reinterpret_cast<const IFD*>(base + pTiff8->get_offset8()), callback, config);
		}
		else
		{
			using F = byte_coodinate_II;
			using IFD = ifd_8<F>;
			parseTiffDirectory<IFD, F>(base_addr, offset, endPos, ifdname, parent, reinterpret_cast<const IFD*>(base + pTiff8->get_offset8()), callback, config);
		}
		break;
	}
}

#include "image_file.h"

bool image_file_tiff::is_supported(const void* buf, uint64_t size)
{
	if (buf == nullptr) return false;

	if (size < 6) return false;
	bool isExif = (memcmp(buf, "Exif\0\0", 6) == 0) ? true : false;
	if (size < sizeof(TIFFHEADER) + (isExif ? 6 : 0)) return false;

	const uint8_t* base = reinterpret_cast<const uint8_t*>(buf);
	const TIFFHEADER* p = reinterpret_cast<const TIFFHEADER*>(base + (isExif ? 6 : 0));

	switch (p->get_order())
	{
	case 'II':
	case 'MM':
		break;
	default:
		return false;
	}

	switch (p->get_version())
	{
	case 0x002A:	// TIFF 
	case 0x002B:	// BigTIFF 
	case 'OR':		// OLYMPUS RAW 
		break;
	default:
		return false;
	}

	return true;
}


class tiff_dictionary_wrapper
{
public:
	tiff_dictionary_wrapper(const wchar_t* lang)
	{
		init_tif_dictionary(lang);
	}

	virtual ~tiff_dictionary_wrapper()
	{
		clear_tif_dictionary();
	}
};


void image_file_tiff::parse(callback_cls* callback, const agalia::config* config)
{
	callback->insert_column(96, L"offset");
	callback->insert_column(96, L"IFD");
	callback->insert_column(96, L"Tag");
	callback->insert_column(265, L"Name");
	callback->insert_column(96, L"Type");
	int count_column_number = callback->insert_column(96, L"Count");
	callback->set_right_align(count_column_number, true);
	callback->insert_column(192, L"Value Offset");
	callback->insert_column(96, L"data");
	callback->insert_column(320, L"value");
	callback->next();

	tiff_dictionary_wrapper dic((config->preferred_language == agalia::config::jpn) ? L"jpn" : L"enu");
	analyzeTiffFile(this->base_addr, this->base_offset, this->data_size, callback, config);
}
