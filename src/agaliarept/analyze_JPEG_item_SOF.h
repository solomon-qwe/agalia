#pragma once

#include "analyze_JPEG_item_Marker.h"

namespace analyze_JPEG
{
	class item_SOF : public item_Marker
	{
	public:
		item_SOF(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type);
		virtual ~item_SOF();

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;
		virtual HRESULT getAdditionalInfoCount(uint32_t* row) const override;
		virtual HRESULT getAdditionalInfoValue(uint32_t row, agaliaString** str) const override;
		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;
	};
}
