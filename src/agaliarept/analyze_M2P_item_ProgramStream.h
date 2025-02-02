#pragma once

#include "agaliareptImpl.h"
#include "analyze_M2P_item_Base.h"

namespace analyze_M2P
{
	class item_ProgramStream : public M2P_item_Base
	{
	public:
		item_ProgramStream(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_ProgramStream();

		virtual HRESULT getName(agaliaString** str) const override;

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;
		virtual HRESULT getNext(agaliaElement** next) const override;
	};
}

