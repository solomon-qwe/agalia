#include "pch.h"
#include "thumbnail.h"
#include "../inc/decode.h"


HRESULT loadThumbnailBitmap(HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH, IStream* stream)
{
	if (!stream || !phBitmap) return E_POINTER;

	agaliaPtr<agaliaDecoder> decoder;
	auto hr = getAgaliaDecoder(&decoder);
	if (FAILED(hr)) return hr;

	return decoder->decode(stream, maxW, maxH, phBitmap);
}
