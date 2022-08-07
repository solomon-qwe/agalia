#pragma once

#include "agaliareptImpl.h"
#include "container_ICC.h"

namespace analyze_ICC
{
	class item_Base : public _agaliaItemBase
	{
	public:
		item_Base(const container_ICC* image, uint64_t offset, uint64_t size, uint64_t endpos);
		virtual ~item_Base();

		virtual HRESULT getItemName(agaliaString** str) const override;
		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChildItem(uint32_t sibling, agaliaItem** child) const override;
		virtual HRESULT getNextItem(agaliaItem** next) const override;

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
