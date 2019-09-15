#pragma once

#include "agaliareptImpl.h"

namespace analyze_ASF
{
	class item_Base : public _agaliaItemBase
	{
	public:
		item_Base(const agaliaContainer* image, uint64_t offset, uint64_t size, uint64_t endpos);
		virtual ~item_Base();

		virtual HRESULT getItemName(agaliaString** str) const override;
		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChildItem(uint32_t sibling, agaliaItem** child) const override;
		virtual HRESULT getNextItem(agaliaItem** next) const override;

		virtual HRESULT getAsocImage(const agaliaContainer** imageAsoc) const override;

		virtual HRESULT getGridRowCount(uint32_t* row) const;
		virtual HRESULT getGridValue(uint32_t row, uint32_t column, agaliaString** str) const;

		enum {
			prop_offset,
			prop_object_guid,
			prop_object_size,
			prop_last
		};

		const agaliaContainer* image = nullptr;
		static const GUID guid_asf;

		uint64_t endpos = 0;
		std::shared_ptr<uint64_t> parent;
	};

	enum
	{
		column_offset,
		column_parent,
		column_objectID,
		column_size,
		column_value,
		column_last
	};


#pragma pack(push, 1)
	struct ASFObject
	{
		GUID ObjectID;
		uint64_t ObjectSize;
	};
#pragma pack(pop)
}
