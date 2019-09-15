#pragma once

#include "analyze_JPEG_item_Marker.h"

namespace analyze_JPEG
{
	class item_SOI : public item_Marker
	{
	public:
		item_SOI(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type);
		virtual ~item_SOI();

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;
		virtual HRESULT getValueAreaOffset(uint64_t* offset) const override;
		virtual HRESULT getValueAreaSize(uint64_t* size) const override;
	};
};

