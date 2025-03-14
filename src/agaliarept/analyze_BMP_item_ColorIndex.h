#pragma once

#include "agaliareptImpl.h"
#include "analyze_BMP_item_Base.h"

namespace analyze_BMP
{
	class item_ColorIndex : public BMP_item_Base
	{
	public:
		item_ColorIndex(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_ColorIndex();

		virtual HRESULT getName(agaliaString** str) const override;

		virtual HRESULT getNext(agaliaElement** next) const override;
		
		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;

		LONG scans = 0;
		LONG bytes_of_line = 0;
		LONG index = 0;
	};
}
