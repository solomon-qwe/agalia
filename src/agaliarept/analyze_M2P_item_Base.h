#pragma once

#include "agaliareptImpl.h"

namespace analyze_M2P
{
	class item_Base : public _agaliaItemBase
	{
	public:
		item_Base(const agaliaContainer* image, uint64_t offset, uint64_t size);
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
			prop_last
		};

		const agaliaContainer* image = nullptr;
		static const GUID guid_m2p;
	};

	enum
	{
		column_offset,
		column_field,
		column_bits,
		column_value,
		column_last
	};
}
