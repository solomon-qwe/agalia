#pragma once

#include "analyze_JPEG_item_Marker.h"

namespace analyze_JPEG
{
	class item_JFIF : public JPEG_item_Base
	{
	public:
		item_JFIF(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type);
		virtual ~item_JFIF();

		virtual HRESULT getName(agaliaString** str) const override;

		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getNext(agaliaElement** next) const override;

		static HRESULT getColumnValueForAPP0(uint32_t column, const container_JPEG* image, const JPEG_item_Base* item, agaliaString** str);
	};
};

