#pragma once

#include "analyze_ICC_item_Base.h"

namespace analyze_ICC
{
	class item_TagTable : public item_Base
	{
	public:
		item_TagTable(const container_ICC* image, uint64_t offset, uint64_t size, uint64_t endpos);
		virtual ~item_TagTable();

		virtual HRESULT getItemName(agaliaString** str) const override;
		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChildItem(uint32_t sibling, agaliaItem** child) const override;
		virtual HRESULT getNextItem(agaliaItem** next) const override;

		enum {
			prop_tag_count = item_Base::prop_last,
			prop_last
		};
	};
}
