#pragma once

#include "analyze_PNG_item_Base.h"

namespace analyze_PNG
{
	class item_iCCP : public PNG_item_Base
	{
	public:
		item_iCCP(const container_PNG* image, uint64_t offset, uint64_t size, uint64_t endpos);
		virtual ~item_iCCP();

		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		enum {
			prop_profile_name = PNG_item_Base::prop_last,
			prop_compression_method,
			prop_last
		};
	};
}
