#include "pch.h"
#include "thumbnail.h"
#include "../inc/decode.h"


HRESULT loadWICBitmap(IWICBitmap** ppBitmap, IWICColorContext** ppColorContext, const agaliaContainer* image)
{
	if (!image || !ppBitmap || !ppColorContext) return E_POINTER;

	agaliaPtr<agaliaDecoder> decoder;
	auto hr = getAgaliaDecoder(&decoder);
	if (FAILED(hr)) return hr;

	return decoder->decode(ppBitmap, ppColorContext, image);
}


HRESULT loadBitmap(agaliaBitmap** ppBitmap, const agaliaContainer* image)
{
	if (!image || !ppBitmap) return E_POINTER;

	agaliaPtr<agaliaDecoder> decoder;
	auto hr = getAgaliaDecoder(&decoder);
	if (FAILED(hr)) return hr;

	return decoder->decode(ppBitmap, image);
}

HRESULT loadThumbnail(agaliaBitmap** ppBitmap, const agaliaContainer* image, uint32_t maxW, uint32_t maxH)
{
	if (!image || !ppBitmap) return E_POINTER;

	agaliaPtr<agaliaDecoder> decoder;
	auto hr = getAgaliaDecoder(&decoder);
	if (FAILED(hr)) return hr;

	return decoder->decodeThumbnail(ppBitmap, image, maxW, maxH);
}
