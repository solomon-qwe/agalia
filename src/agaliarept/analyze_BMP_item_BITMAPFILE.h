#pragma once

#include "agaliareptImpl.h"
#include "analyze_BMP_item_Base.h"

namespace analyze_BMP
{
	class item_BITMAPFILE : public item_Base
	{
	public:
		item_BITMAPFILE(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_BITMAPFILE();

		virtual HRESULT getItemName(agaliaString** str) const override;
		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getNextItem(agaliaItem** next) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;
		virtual HRESULT getAdditionalInfoCount(uint32_t* row) const override;
		virtual HRESULT getAdditionalInfoValue(uint32_t row, agaliaString** str) const override;

		enum {
			prop_offset = item_Base::prop_last,
			prop_bfType,
			prop_bfSize,
			prop_bfReserved1,
			prop_bfReserved2,
			prop_bfOffBits,
			prop_last
		};

		enum {
			addinf_bfSize,
			addinf_bfReserved1,
			addinf_bfReserved2,
			addinf_bfOffBits,
			addinf_last
		};
	};
};

