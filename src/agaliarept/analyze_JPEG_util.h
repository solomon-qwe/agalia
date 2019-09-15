#pragma once

#include <stdint.h>

#include "container_JPEG.h"
#include "jpeg_def.h"
#include "byteswap.h"

namespace analyze_JPEG
{
	inline uint16_t getMarker(const container_JPEG* image, const agaliaItem* item)
	{
		JPEGSEGMENT segment = {};
		auto hr = image->ReadData(&segment, item->getOffset(), sizeof(segment));
		if (FAILED(hr)) return 0;

		return agalia_byteswap(segment.marker);
	}



	inline const wchar_t int_to_subscript_wchar(int i)
	{
		static const wchar_t s[] = L"₀₁₂₃₄₅₆₇₈₉";
		return (i <= 9) ? s[i] : L'\0';
	}



	template<class T>
	HRESULT ReadSegment(CHeapPtr<T>& buf, rsize_t* bufsize, const container_JPEG* image, const agaliaItem* item)
	{
		if (bufsize == nullptr) return E_POINTER;
		if (image == nullptr) return E_POINTER;
		if (item == nullptr) return E_POINTER;

		rsize_t l_size = 0;
		auto hr = UInt64ToSizeT(item->getSize(), &l_size);
		if (FAILED(hr)) return hr;

		CHeapPtr<T> l_buf;
		if (!l_buf.AllocateBytes(l_size)) return E_OUTOFMEMORY;
		hr = image->ReadData(l_buf, item->getOffset(), l_size);
		if (FAILED(hr)) return hr;

		buf.Attach(l_buf.Detach());
		*bufsize = l_size;
		return S_OK;
	}


	bool app_identify(const char* name, rsize_t name_size, JPEGSEGMENT_APPX* app, rsize_t bufsize);

	enum {
		column_offset,
		column_marker,
		column_name,
		column_length,
		column_value,
		column_last
	};
}
