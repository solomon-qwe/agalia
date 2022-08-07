#pragma once

#include "analyze_PNG_item_Base.h"

namespace analyze_PNG
{
	class item_iTXt : public item_Base
	{
	public:
		item_iTXt(const container_PNG* image, uint64_t offset, uint64_t size, uint64_t endpos);
		virtual ~item_iTXt();

		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		enum {
			prop_keyword = item_Base::prop_last,
			prop_compression_flag,
			prop_compression_method,
			prop_language_tag,
			prop_translated_keyword,
			prop_text,
			prop_last
		};
	};
}
