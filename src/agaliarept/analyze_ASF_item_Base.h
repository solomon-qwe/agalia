#pragma once

#include "agaliareptImpl.h"

namespace analyze_ASF
{
	class ASF_item_Base : public _agaliaItemBase
	{
	public:
		ASF_item_Base(const agaliaContainer* image, uint64_t offset, uint64_t size, uint64_t endpos);
		virtual ~ASF_item_Base();

		virtual HRESULT getName(agaliaString** str) const override;
		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;
		virtual HRESULT getNext(agaliaElement** next) const override;

		virtual HRESULT getAsocImage(const agaliaContainer** imageAsoc) const override;

		virtual HRESULT getElementInfoCount(uint32_t* row) const;
		virtual HRESULT getElementInfoValue(uint32_t row, uint32_t column, agaliaString** str) const;

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
