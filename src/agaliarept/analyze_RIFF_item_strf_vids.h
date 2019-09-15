#pragma once
#include "analyze_RIFF_item_Base.h"

namespace analyze_RIFF
{
	class item_strf_vids : public item_Base
	{
	public:
		item_strf_vids(const container_RIFF* _image, uint64_t offset, uint64_t size);
		virtual ~item_strf_vids();

		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;
		virtual HRESULT getAdditionalInfoCount(uint32_t* row) const override;
		virtual HRESULT getAdditionalInfoValue(uint32_t row, agaliaString** str) const override;

		enum {
			prop_biSize = item_Base::prop_last,
			prop_biWidth,
			prop_biHeight,
			prop_biPlanes,
			prop_biBitCount,
			prop_biCompression,
			prop_biSizeImage,
			prop_biXPelsPerMeter,
			prop_biYPelsPerMeter,
			prop_biClrUsed,
			prop_biClrImportant,
			prop_biExtDataOffset,
			prop_JPEGSize,
			prop_JPEGProcess,
			prop_JPEGColorSpaceID,
			prop_JPEGBitsPerSample,
			prop_JPEGHSubSampling,
			prop_JPEGVSubSampling,
			prop_last
		};

		enum {
			addinf_biWidth,
			addinf_biHeight,
			addinf_biPlanes,
			addinf_biBitCount,
			addinf_biCompression,
			addinf_biSizeImage,
			addinf_biXPelsPerMeter,
			addinf_biYPelsPerMeter,
			addinf_biClrUsed,
			addinf_biClrImportant,
			addinf_biExtDataOffset,
			addinf_JPEGSize,
			addinf_JPEGProcess,
			addinf_JPEGColorSpaceID,
			addinf_JPEGBitsPerSample,
			addinf_JPEGHSubSampling,
			addinf_JPEGVSubSampling,
			addinf_last
		};
	};
}
