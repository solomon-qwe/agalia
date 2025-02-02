#pragma once

#include "agaliareptImpl.h"

namespace analyze_BMP
{
	class BMP_item_Base : public _agaliaItemBase
	{
	public:
		BMP_item_Base(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~BMP_item_Base();

		virtual HRESULT getName(agaliaString** str) const override;
		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;
		virtual HRESULT getNext(agaliaElement** next) const override;

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

