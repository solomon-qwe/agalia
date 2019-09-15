#pragma once

#include "analyze_JPEG_item_Marker.h"

namespace analyze_JPEG
{
	class item_APP : public item_Marker
	{
	public:
		item_APP(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type);
		virtual ~item_APP();

		virtual HRESULT getChildItem(uint32_t sibling, agaliaItem** child) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;
		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;


		HRESULT getColumnValueAPPx(agaliaString** str) const;
		HRESULT getColumnValueAPP0JFIF(agaliaString** str) const;
		HRESULT getColumnValueAPP0(agaliaString** str) const;
	};
}

