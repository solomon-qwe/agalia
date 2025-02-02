#pragma once

#include "agaliareptImpl.h"
#include "container_ICC.h"

namespace analyze_ICC
{
	class ICC_item_Base : public _agaliaItemBase
	{
	public:
		ICC_item_Base(const container_ICC* image, uint64_t offset, uint64_t size, uint64_t endpos);
		virtual ~ICC_item_Base();

		virtual HRESULT getName(agaliaString** str) const override;
		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;
		virtual HRESULT getNext(agaliaElement** next) const override;

		virtual HRESULT getAsocImage(const agaliaContainer** imageAsoc) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const;

		enum {
			prop_last
		};

		const container_ICC* image = nullptr;
		static const GUID guid_icc;
		uint64_t endpos;
	};

	enum {
		column_offset,
		column_signature,
		column_value_offset,
		column_element_size,
		column_value,
		column_last
	};
}
