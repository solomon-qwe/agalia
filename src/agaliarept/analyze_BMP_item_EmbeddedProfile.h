#pragma once

#include "agaliareptImpl.h"
#include "analyze_BMP_item_Base.h"

namespace analyze_BMP
{
	class item_EmbeddedProfile : public item_Base
	{
	public:
		item_EmbeddedProfile(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_EmbeddedProfile();

		virtual HRESULT getItemName(agaliaString** str) const override;
	};
}
