#pragma once

#include "agaliareptImpl.h"
#include "analyze_M2P_item_Base.h"

namespace analyze_M2P
{
	class item_ProgramEnd : public M2P_item_Base
	{
	public:
		item_ProgramEnd(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_ProgramEnd();

		virtual HRESULT getName(agaliaString** str) const override;

		virtual HRESULT getElementInfoCount(uint32_t* row) const override;
		virtual HRESULT getElementInfoValue(uint32_t row, uint32_t column, agaliaString** str) const override;
	};
}

