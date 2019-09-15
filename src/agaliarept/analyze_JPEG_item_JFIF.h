#pragma once

#include "analyze_JPEG_item_Marker.h"

namespace analyze_JPEG
{
	class item_JFIF : public item_Base
	{
	public:
		item_JFIF(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type);
		virtual ~item_JFIF();

		virtual HRESULT getItemName(agaliaString** str) const override;

		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getNextItem(agaliaItem** next) const override;

		static HRESULT getColumnValueForAPP0(uint32_t column, const container_JPEG* image, const item_Base* item, agaliaString** str);
	};
};

