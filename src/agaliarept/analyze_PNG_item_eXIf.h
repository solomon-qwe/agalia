#pragma once

#include "analyze_PNG_item_Base.h"

namespace analyze_PNG
{
	class item_eXIf : public PNG_item_Base
	{
	public:
		item_eXIf(const container_PNG* image, uint64_t offset, uint64_t size, uint64_t endpos);
		virtual ~item_eXIf();

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;

	protected:
		agaliaContainer* tiff_image = nullptr;
	};
}
