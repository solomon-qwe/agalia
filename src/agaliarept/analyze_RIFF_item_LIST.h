#pragma once

#include "analyze_RIFF_item_Base.h"

namespace analyze_RIFF
{
	class item_LIST : public RIFF_item_Base
	{
	public:
		item_LIST(const container_RIFF* _image, uint64_t offset, uint64_t size);
		virtual ~item_LIST();

		virtual HRESULT getName(agaliaString** str) const override;

		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const;

		enum {
			prop_fccListType = RIFF_item_Base::prop_last,
			prop_last
		};
	};
}
