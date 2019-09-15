#pragma once

#include <stdint.h>
#include <stdlib.h>

//template <typename T> T agalia_byteswap(T& v) { return T(); };

_Check_return_ inline int8_t agalia_byteswap(_In_ const int8_t& v) { return v; }
_Check_return_ inline int16_t agalia_byteswap(_In_ const int16_t& v) { return _byteswap_ushort(v); }
_Check_return_ inline int32_t agalia_byteswap(_In_ const int32_t& v) { return _byteswap_ulong(v); }
_Check_return_ inline int64_t agalia_byteswap(_In_ const int64_t& v) { return _byteswap_uint64(v); }
_Check_return_ inline uint8_t agalia_byteswap(_In_ const uint8_t& v) { return v; }
_Check_return_ inline uint16_t agalia_byteswap(_In_ const uint16_t& v) { return _byteswap_ushort(v); }
_Check_return_ inline uint32_t agalia_byteswap(_In_ const uint32_t& v) { return _byteswap_ulong(v); }
_Check_return_ inline uint64_t agalia_byteswap(_In_ const uint64_t& v) { return _byteswap_uint64(v); }
