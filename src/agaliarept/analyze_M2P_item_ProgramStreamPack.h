#pragma once

#include "agaliareptImpl.h"
#include "analyze_M2P_item_Base.h"

namespace analyze_M2P
{
	class item_ProgramStreamPack : public item_Base
	{
	public:
		item_ProgramStreamPack(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_ProgramStreamPack();

		virtual HRESULT getItemName(agaliaString** str) const override;

		virtual HRESULT getChildItem(uint32_t sibling, agaliaItem** child) const override;
		virtual HRESULT getNextItem(agaliaItem** next) const override;

		static uint64_t calc_item_size(const agaliaContainer* image, uint64_t offset);
	};
}

