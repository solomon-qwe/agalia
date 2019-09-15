#pragma once
#include "analyze_RIFF_item_Base.h"

namespace analyze_RIFF
{
	class item_strf_auds : public item_Base
	{
	public:
		item_strf_auds(const container_RIFF* _image, uint64_t offset, uint64_t size);
		virtual ~item_strf_auds();

		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;
		virtual HRESULT getAdditionalInfoCount(uint32_t* row) const override;
		virtual HRESULT getAdditionalInfoValue(uint32_t row, agaliaString** str) const override;

		enum {
			prop_wFormatTag = item_Base::prop_last,
			prop_nChannels,
			prop_nSamplesPerSec,
			prop_nAvgBytesPerSec,
			prop_nBlockAlign,
			prop_wBitsPerSample,
			prop_cbSize,
			prop_last
		};

		enum {
			addinf_nChannels,
			addinf_nSamplesPerSec,
			addinf_nAvgBytesPerSec,
			addinf_nBlockAlign,
			addinf_wBitsPerSample,
			addinf_cbSize,
			addinf_last
		};
	};
}
