#pragma once

#include "agaliareptImpl.h"
#include "analyze_BMP_item_Base.h"

namespace analyze_BMP
{
	class item_ColorIndexParent : public BMP_item_Base
	{
	public:
		item_ColorIndexParent(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_ColorIndexParent();

		virtual HRESULT getName(agaliaString** str) const override;

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const;

		LONG scans = 0;
		LONG bytes_of_line = 0;
		LONG biCompression = 0;
	};
}
