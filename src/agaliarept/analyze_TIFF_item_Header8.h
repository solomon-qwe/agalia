#pragma once

#include "analyze_TIFF_item_Base.h"

namespace analyze_TIFF
{
	class item_TIFFHeader8 : public TIFF_item_Base
	{
	public:
		item_TIFFHeader8(const container_TIFF* image, uint64_t offset, uint64_t size);
		virtual ~item_TIFFHeader8();

		virtual HRESULT getName(agaliaString** str) const override;

		enum {
			prop_order,
			prop_version,
			prop_offset_size,
			prop_reserved,
			prop_offset,
			prop_last
		};

		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;
		virtual HRESULT getNext(agaliaElement** next) const override;
	};
}
