#pragma once

class agaliaBitmap;
struct IWICBitmap;
struct IWICColorContext;

HRESULT loadWICBitmap(IWICBitmap** ppBitmap, IWICColorContext** ppColorContext, const agaliaContainer* image);
HRESULT loadBitmap(agaliaBitmap** ppBitmap, const agaliaContainer* image);
HRESULT loadThumbnail(agaliaBitmap** ppBitmap, const agaliaContainer* image, uint32_t maxW, uint32_t maxH);
