#pragma once

#include "analyze_RIFF_item_Base.h"

namespace analyze_RIFF
{
	class item_LIST : public item_Base
	{
	public:
		item_LIST(const container_RIFF* _image, uint64_t offset, uint64_t size);
		virtual ~item_LIST();

		virtual HRESULT getItemName(agaliaString** str) const override;

		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChildItem(uint32_t sibling, agaliaItem** child) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const;

		enum {
			prop_fccListType = item_Base::prop_last,
			prop_last
		};
	};
}
