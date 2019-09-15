#pragma once

#include "agaliareptImpl.h"
#include "analyze_BMP_item_Base.h"

namespace analyze_BMP
{
	class item_RGBQUADParent : public item_Base
	{
	public:
		item_RGBQUADParent(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_RGBQUADParent();

		virtual HRESULT getItemName(agaliaString** str) const override;

		virtual HRESULT getChildItem(uint32_t sibling, agaliaItem** child) const override;
		virtual HRESULT getNextItem(agaliaItem** next) const override;

		LONG scans = 0;
		LONG bytes_of_line = 0;
		LONG biCompression = 0;
		LONG colors = 0;
	};
}

