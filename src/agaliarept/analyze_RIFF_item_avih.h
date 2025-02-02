#pragma once

#include "analyze_RIFF_item_Base.h"

namespace analyze_RIFF
{
	class item_avih : public RIFF_item_Base
	{
	public:
		item_avih(const container_RIFF* _image, uint64_t offset, uint64_t size);
		virtual ~item_avih();

		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;
		virtual HRESULT getAdditionalInfoCount(uint32_t* row) const override;
		virtual HRESULT getAdditionalInfoValue(uint32_t row, agaliaString** str) const override;

		enum {
			prop_dwMicroSecPerFrame = RIFF_item_Base::prop_last,
			prop_dwMaxBytesPerSec,
			prop_dwPaddingGranularity,
			prop_dwFlags,
			prop_dwTotalFrames,
			prop_dwInitialFrames,
			prop_dwStreams,
			prop_dwSuggestedBufferSize,
			prop_dwWidth,
			prop_dwHeight,
			prop_dwReserved,
			prop_last
		};

		enum {
			addinf_dwMicroSecPerFrame = 0,
			addinf_dwMaxBytesPerSec,
			addinf_dwPaddingGranularity,
			addinf_dwFlags,
			addinf_dwTotalFrames,
			addinf_dwInitialFrames,
			addinf_dwStreams,
			addinf_dwSuggestedBufferSize,
			addinf_dwWidth,
			addinf_dwHeight,
			addinf_dwReserved,
			addinf_last
		};
	};
}
