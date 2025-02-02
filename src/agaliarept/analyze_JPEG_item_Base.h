#pragma once

#include "container_JPEG.h"
#include "jpeg_def.h"
#include "byteswap.h"

namespace analyze_JPEG
{
	class JPEG_item_Base : public _agaliaItemBase
	{
	public:
		enum item_type
		{
			unknown,
			marker_segment,
			jfif,			// APP0 
			exif,			// APP1 
			xmp,			// APP1
			icc_profile,	// APP2 
			ducky,			// APP12 
			photoshop,		// APP13
			adobe,			// APP14
		};

		JPEG_item_Base(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type);
		virtual ~JPEG_item_Base();

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;

		virtual HRESULT getAsocImage(const agaliaContainer** imageAsoc) const override {
			*imageAsoc = this->image;
			return S_OK;
		}

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const;
		virtual HRESULT getAdditionalInfoCount(uint32_t* row) const;
		virtual HRESULT getAdditionalInfoValue(uint32_t row, agaliaString** str) const;

		static const GUID guid_jpeg;

	protected:
		item_type data_type = unknown;
		const container_JPEG* image = nullptr;
	};
}
