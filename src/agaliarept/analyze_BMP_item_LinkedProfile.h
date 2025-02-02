#pragma once

#include "agaliareptImpl.h"
#include "analyze_BMP_item_Base.h"

namespace analyze_BMP
{
	class item_LinkedProfile : public BMP_item_Base
	{
	public:
		item_LinkedProfile(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_LinkedProfile();

		virtual HRESULT getName(agaliaString** str) const override;
		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		enum {
			prop_filename = BMP_item_Base::prop_last,
			prop_last
		};
	};
}
