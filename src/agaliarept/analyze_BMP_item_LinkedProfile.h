#pragma once

#include "agaliareptImpl.h"
#include "analyze_BMP_item_Base.h"

namespace analyze_BMP
{
	class item_LinkedProfile : public item_Base
	{
	public:
		item_LinkedProfile(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_LinkedProfile();

		virtual HRESULT getItemName(agaliaString** str) const override;
		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		enum {
			prop_filename = item_Base::prop_last,
			prop_last
		};
	};
}
