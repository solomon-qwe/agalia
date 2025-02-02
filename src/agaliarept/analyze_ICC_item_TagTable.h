#pragma once

#include "analyze_ICC_item_Base.h"

namespace analyze_ICC
{
	class item_TagTable : public ICC_item_Base
	{
	public:
		item_TagTable(const container_ICC* image, uint64_t offset, uint64_t size, uint64_t endpos);
		virtual ~item_TagTable();

		virtual HRESULT getName(agaliaString** str) const override;
		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;
		virtual HRESULT getNext(agaliaElement** next) const override;

		enum {
			prop_tag_count = ICC_item_Base::prop_last,
			prop_last
		};
	};
}
