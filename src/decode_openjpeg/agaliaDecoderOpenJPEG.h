#pragma once

#include "../inc/agaliaDecoder.h"

class agaliaDecoderOpenJPEG :
    public agaliaDecoder
{
public:
	agaliaDecoderOpenJPEG();
	virtual ~agaliaDecoderOpenJPEG();
	virtual ULONG AddRef(void) override;
	virtual ULONG Release(void) override;
	virtual uint32_t getPriority(void) const override { return 3000; };
	virtual HRESULT decode(IStream* stream, HBITMAP* phBitmap) override;
	virtual HRESULT decode(IStream* stream, agaliaHeap** bmpInfo, void** ppBits) override;
	virtual HRESULT decode(IStream* stream, uint32_t maxW, uint32_t maxH, HBITMAP* phBitmap) override;
	virtual HRESULT decode(IStream* stream, uint32_t maxW, uint32_t maxH, agaliaHeap** bmpInfo, void** ppBits) override;

protected:
	ULONG refCount = 1;
};

