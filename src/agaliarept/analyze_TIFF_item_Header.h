#pragma once

#include "analyze_TIFF_item_Base.h"

namespace analyze_TIFF
{
	class item_TIFFHeader : public item_tiff_Base
	{
	public:
		item_TIFFHeader(const container_TIFF* image, uint64_t offset, uint64_t size);
		virtual ~item_TIFFHeader();

		virtual HRESULT getItemName(agaliaString** str) const override;

		enum {
			prop_order,
			prop_version,
			prop_offset,
			prop_last
		};

		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChildItem(uint32_t sibling, agaliaItem** child) const override;
		virtual HRESULT getNextItem(agaliaItem** next) const override;
	};

}
