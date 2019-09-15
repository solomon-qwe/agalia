#pragma once

#include "agaliareptImpl.h"
#include "analyze_ASF_item_Base.h"

namespace analyze_ASF
{
	class item_HeaderObject : public item_Base
	{
	public:
		item_HeaderObject(const agaliaContainer* image, uint64_t offset, uint64_t size, uint64_t endpos);
		virtual ~item_HeaderObject();

		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChildItem(uint32_t sibling, agaliaItem** child) const override;

		virtual HRESULT getGridRowCount(uint32_t* row) const override;
		virtual HRESULT getGridValue(uint32_t row, uint32_t column, agaliaString** str) const override;

		enum {
			prop_offset = item_Base::prop_last,
			prop_NumberOfHeaderObject,
			prop_Reserved1,
			prop_Reserved2,
			prop_last
		};
	};

#pragma pack(push, 1)
	struct HeaderObject : ASFObject
	{
		uint32_t NumberOfHeaderObjects;
		uint8_t Reserved1;
		uint8_t Reserved2;
	};
#pragma pack(pop)
}

