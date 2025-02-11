#pragma once

#include "agaliarept.h"
#include <objidl.h>

class agaliaDecoder
{
public:
    virtual ULONG AddRef(void) = 0;
    virtual ULONG Release(void) = 0;

	virtual uint32_t getPriority(void) const = 0;

    virtual HRESULT decode(IStream* stream, HBITMAP* phBitmap) = 0;
    virtual HRESULT decode(IStream* stream, agaliaHeap** bmpInfo, void** ppBits) = 0;
    virtual HRESULT decode(IStream* stream, uint32_t maxW, uint32_t maxH, HBITMAP* phBitmap) = 0;
    virtual HRESULT decode(IStream* stream, uint32_t maxW, uint32_t maxH, agaliaHeap** bmpInfo, void** ppBits) = 0;
};
