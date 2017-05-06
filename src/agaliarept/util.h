#pragma once

/* Seconds between 00:00 1904-01-01 and 00:00 1970-01-01 */
const unsigned long SecsUntil1970 = 2082844800;

template <class T>
T be_itostr(std::wstring& str, const uint8_t* addr, uint64_t offset);

template <>
inline uint8_t be_itostr<uint8_t>(std::wstring& str, const uint8_t* addr, uint64_t offset)
{
	uint8_t val = *reinterpret_cast<const uint8_t*>(addr + offset);
	wchar_t temp[8] = {};
	swprintf_s(temp, L"'%hu'", (unsigned short)val);
	str += temp;
	return val;
}

template <>
inline uint16_t be_itostr<uint16_t>(std::wstring& str, const uint8_t* addr, uint64_t offset)
{
	uint16_t val = _byteswap_ushort(*reinterpret_cast<const uint16_t*>(addr + offset));
	wchar_t temp[16] = {};
	swprintf_s(temp, L"'%hu'", val);
	str += temp;
	return val;
}

template <>
inline uint32_t be_itostr<uint32_t>(std::wstring& str, const uint8_t* addr, uint64_t offset)
{
	uint32_t val = _byteswap_ulong(*reinterpret_cast<const uint32_t*>(addr + offset));
	wchar_t temp[16] = {};
	swprintf_s(temp, L"'%I32u'", val);
	str += temp;
	return val;
}

template <>
inline uint64_t be_itostr<uint64_t>(std::wstring& str, const uint8_t* addr, uint64_t offset)
{
	uint64_t val = _byteswap_uint64(*reinterpret_cast<const uint64_t*>(addr + offset));
	wchar_t temp[32] = {};
	swprintf_s(temp, L"'%I64u'", val);
	str += temp;
	return val;
}

template <>
inline __time32_t be_itostr<__time32_t>(std::wstring& str, const uint8_t* addr, uint64_t offset)
{
	__time32_t val = _byteswap_ulong(*reinterpret_cast<const __time32_t*>(addr + offset)) - SecsUntil1970;
	wchar_t temp[64] = {};
	if (_wctime32_s(temp, &val) == 0) {
		str += temp;
	}
	return val;
}

template <>
inline __time64_t be_itostr<__time64_t>(std::wstring& str, const uint8_t* addr, uint64_t offset)
{
	__time64_t val = _byteswap_uint64(*reinterpret_cast<const __time64_t*>(addr + offset)) - SecsUntil1970;
	wchar_t temp[64] = {};
	if (_wctime64_s(temp, &val) == 0) {
		str += temp;
	}
	return val;
}


inline void fast_itow16(int v, wchar_t* p)
{
	static const wchar_t s[] = L"0123456789ABCDEF";
	p[0] = s[(v >> 4) & 0xF];
	p[1] = s[(v >> 0) & 0xF];
	p[2] = '\0';
}


template<class T>
class agalia_ptr
{
public:
	agalia_ptr()
	{
		p = nullptr;
	}

	virtual ~agalia_ptr()
	{
		cleanup();
	}

	void cleanup(void)
	{
		if (p)
		{
			T* temp = p;
			p = nullptr;
			delete temp;
		}
	}

	T* p;
};