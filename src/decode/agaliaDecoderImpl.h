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

	virtual HRESULT decode(agaliaBitmap** ppBitmap, const agaliaContainer* image) override;
	virtual HRESULT decode(IWICBitmap** ppBitmap, IWICColorContext** ppColorContext, const agaliaContainer* image) override;
	virtual HRESULT decodeThumbnail(agaliaBitmap** ppBitmap, const agaliaContainer* image, uint32_t maxW, uint32_t maxH) override;

	HRESULT initialize(void);
	HRESULT finalize(void);
	HRESULT add(agaliaDecoder* decoder);

protected:
	ULONG refCount = 1;

	struct item
	{
		HMODULE hModule;
		agaliaDecoder* decoder;
	};

	std::list<item>* decoders = nullptr;
};

