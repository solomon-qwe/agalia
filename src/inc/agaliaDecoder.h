#pragma once

#include "agaliarept.h"
#include "../inc/agaliaUtil.h"
#include <objidl.h>

interface IWICBitmap;
interface IWICColorContext;

class agaliaDecoder
{
public:
    virtual ULONG AddRef(void) = 0;
    virtual ULONG Release(void) = 0;

	virtual uint32_t getPriority(void) const = 0;

    virtual HRESULT decode(agaliaBitmap** ppBitmap, const agaliaContainer* image) = 0;
    virtual HRESULT decode(IWICBitmap** ppBitmap, IWICColorContext** ppColorContext, const agaliaContainer* image) = 0;
	virtual HRESULT decodeThumbnail(agaliaBitmap** ppBitmap, const agaliaContainer* image, uint32_t maxW, uint32_t maxH) = 0;
};
