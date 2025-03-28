#pragma once
#include "analyze_RIFF_item_Base.h"

namespace analyze_RIFF
{
	class item_text : public RIFF_item_Base
	{
	public:
		item_text(const container_RIFF* _image, uint64_t offset, uint64_t size);
		virtual ~item_text();


		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;

		enum {
			prop_text = RIFF_item_Base::prop_last,
			prop_last
		};
	};
}
