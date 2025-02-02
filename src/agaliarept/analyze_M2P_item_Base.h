#pragma once

#include "agaliareptImpl.h"

namespace analyze_M2P
{
	class M2P_item_Base : public _agaliaItemBase
	{
	public:
		M2P_item_Base(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~M2P_item_Base();

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
