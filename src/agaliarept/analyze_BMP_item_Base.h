#pragma once

#include "agaliareptImpl.h"

namespace analyze_BMP
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

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const;
		virtual HRESULT getAdditionalInfoCount(uint32_t* row) const;
		virtual HRESULT getAdditionalInfoValue(uint32_t row, agaliaString** str) const;

		enum {
			prop_offset,
			prop_last
		};

		uint64_t bfOffBits = 0;
		LONG biSizeImage = 0;
		const agaliaContainer* image = nullptr;
		static const GUID guid_bmp;
	};

	enum
	{
		column_offset,
		column_structure,
		column_value,
		column_last
	};
}

