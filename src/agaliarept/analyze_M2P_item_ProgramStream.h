#pragma once

#include "agaliareptImpl.h"
#include "analyze_M2P_item_Base.h"

namespace analyze_M2P
{
	class item_ProgramStream : public item_Base
	{
	public:
		item_ProgramStream(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_ProgramStream();

		virtual HRESULT getItemName(agaliaString** str) const override;

		virtual HRESULT getChildItem(uint32_t sibling, agaliaItem** child) const override;
		virtual HRESULT getNextItem(agaliaItem** next) const override;
	};
}

