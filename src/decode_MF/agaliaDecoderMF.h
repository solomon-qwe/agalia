#pragma once

#include "../inc/agaliaDecoder.h"

class agaliaDecoderMF :
	public agaliaDecoder
{
public:
	agaliaDecoderMF();
	virtual ~agaliaDecoderMF();
	virtual ULONG AddRef(void) override;
	virtual ULONG Release(void) override;
	virtual uint32_t getPriority(void) const override { return 2000; };
	virtual HRESULT decode(IStream* stream, HBITMAP* phBitmap) override;
	virtual HRESULT decode(IStream* stream, agaliaHeap** bmpInfo, void** ppBits) override;
	virtual HRESULT decode(IStream* stream, uint32_t maxW, uint32_t maxH, HBITMAP* phBitmap) override;
	virtual HRESULT decode(IStream* stream, uint32_t maxW, uint32_t maxH, agaliaHeap** bmpInfo, void** ppBits) override;

protected:
	ULONG refCount = 1;
};
