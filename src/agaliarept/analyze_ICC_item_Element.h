#pragma once

#include "analyze_ICC_item_Base.h"
#include "byteswap.h"

namespace analyze_ICC
{
	class item_Element : public ICC_item_Base
	{
	public:
		item_Element(const container_ICC* image, uint64_t offset, uint64_t size, uint64_t endpos, uint32_t count, uint32_t index);
		virtual ~item_Element();

		virtual HRESULT getName(agaliaString** str) const override;
		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;
		virtual HRESULT getNext(agaliaElement** next) const override;

		virtual HRESULT getValueAreaOffset(uint64_t* offset) const override;
		virtual HRESULT getValueAreaSize(uint64_t* size) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;

		HRESULT getProfileName(agaliaString** str);

		enum {
			prop_signature = ICC_item_Base::prop_last,
			prop_offset,
			prop_size,
			prop_value,
			prop_last
		};

	protected:
		uint32_t tag_count = 0;
		uint32_t tag_index = 0;
	};
	
	struct Element
	{
		uint32_t signature;
		uint32_t offset;
		uint32_t size;

		uint32_t getSignature(void) const { return agalia_byteswap(signature); }
		uint32_t getOffset(void) const { return agalia_byteswap(offset); }
		uint32_t getSize(void) const { return agalia_byteswap(size); }
	};
}
