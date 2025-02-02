#pragma once

#include "analyze_JPEG_item_Base.h"

namespace analyze_JPEG
{
	class item_ExifJPEG : public JPEG_item_Base
	{
	public:
		item_ExifJPEG(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type);
		virtual ~item_ExifJPEG();

		virtual HRESULT getName(agaliaString** str) const override;
		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;
		virtual HRESULT getNext(agaliaElement** next) const override;

		virtual HRESULT getAsocImage(const agaliaContainer** imageAsoc) const override;
		virtual HRESULT getValueAreaOffset(uint64_t* offset) const override;
		virtual HRESULT getValueAreaSize(uint64_t* size) const override;

		agaliaContainer* tiff_image = nullptr;
	};
}

