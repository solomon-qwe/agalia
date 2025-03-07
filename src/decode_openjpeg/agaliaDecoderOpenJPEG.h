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
	virtual HRESULT decode(agaliaBitmap** ppBitmap, const agaliaContainer* image) override;
	virtual HRESULT decode(IWICBitmap** ppBitmap, IWICColorContext** ppColorContext, const agaliaContainer* image) override;
	virtual HRESULT decodeThumbnail(agaliaBitmap** ppBitmap, const agaliaContainer* image, uint32_t maxW, uint32_t maxH) override;

protected:
	ULONG refCount = 1;
};

