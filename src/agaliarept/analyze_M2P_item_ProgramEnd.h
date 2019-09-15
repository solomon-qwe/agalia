#pragma once

#include "agaliareptImpl.h"
#include "analyze_M2P_item_Base.h"

namespace analyze_M2P
{
	class item_ProgramEnd : public item_Base
	{
	public:
		item_ProgramEnd(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_ProgramEnd();

		virtual HRESULT getItemName(agaliaString** str) const override;

		virtual HRESULT getGridRowCount(uint32_t* row) const override;
		virtual HRESULT getGridValue(uint32_t row, uint32_t column, agaliaString** str) const override;
	};
}

