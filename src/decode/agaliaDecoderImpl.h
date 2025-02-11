#pragma once
#include "../inc/agaliaDecoder.h"
#include <list>

class agaliaDecoderImpl :
    public agaliaDecoder
{
public:
	agaliaDecoderImpl();
	virtual ~agaliaDecoderImpl();

	virtual uint32_t getPriority(void) const override { return 0; };

	virtual ULONG AddRef(void) override;
	virtual ULONG Release(void) override;
	virtual HRESULT decode(IStream* stream, HBITMAP* phBitmap) override;
	virtual HRESULT decode(IStream* stream, agaliaHeap** bmpInfo, void** ppBits) override;
	virtual HRESULT decode(IStream* stream, uint32_t maxW, uint32_t maxH, HBITMAP* phBitmap) override;
	virtual HRESULT decode(IStream* stream, uint32_t maxW, uint32_t maxH, agaliaHeap** bmpInfo, void** ppBits) override;

	HRESULT initialize(void);
	HRESULT finalize(void);

protected:
	ULONG refCount = 1;

	struct item
	{
		HMODULE hModule;
		agaliaDecoder* decoder;
	};

	std::list<item>* decoders = nullptr;
};

