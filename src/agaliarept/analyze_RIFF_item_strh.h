#pragma once
#include "analyze_RIFF_item_Base.h"

namespace analyze_RIFF
{
	class item_strh : public RIFF_item_Base
	{
	public:
		item_strh(const container_RIFF* _image, uint64_t offset, uint64_t size);
		virtual ~item_strh();

		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;
		virtual HRESULT getAdditionalInfoCount(uint32_t* row) const override;
		virtual HRESULT getAdditionalInfoValue(uint32_t row, agaliaString** str) const override;

		enum {
			prop_fccType = RIFF_item_Base::prop_last,
			prop_fccHandler,
			prop_dwFlags,
			prop_wPriority,
			prop_wLanguage,
			prop_dwInitialFrames,
			prop_dwScale,
			prop_dwRate,
			prop_dwStart,
			prop_dwLength,
			prop_dwSuggestedBufferSize,
			prop_dwQuality,
			prop_dwSampleSize,
			prop_rcFrame_left,
			prop_rcFrame_top,
			prop_rcFrame_right,
			prop_rcFrame_bottom,
			prop_last
		};

		enum {
			addinf_fccType,
			addinf_fccHandler,
			addinf_dwFlags,
			addinf_wPriority,
			addinf_wLanguage,
			addinf_dwInitialFrames,
			addinf_dwScale,
			addinf_dwRate,
			addinf_dwStart,
			addinf_dwLength,
			addinf_dwSuggestedBufferSize,
			addinf_dwQuality,
			addinf_dwSampleSize,
			addinf_rcFrame,
			addinf_last
		};
	};
}