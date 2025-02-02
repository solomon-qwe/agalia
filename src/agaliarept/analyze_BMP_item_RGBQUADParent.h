#pragma once

#include "agaliareptImpl.h"
#include "analyze_BMP_item_Base.h"

namespace analyze_BMP
{
	class item_RGBQUADParent : public BMP_item_Base
	{
	public:
		item_RGBQUADParent(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_RGBQUADParent();

		virtual HRESULT getName(agaliaString** str) const override;

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;
		virtual HRESULT getNext(agaliaElement** next) const override;

		LONG scans = 0;
		LONG bytes_of_line = 0;
		LONG biCompression = 0;
		LONG colors = 0;
	};
}

