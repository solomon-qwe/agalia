#pragma once

#include "agaliareptImpl.h"
#include "analyze_BMP_item_Base.h"

namespace analyze_BMP
{
	class item_ColorMask : public BMP_item_Base
	{
	public:
		item_ColorMask(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_ColorMask();

		virtual HRESULT getName(agaliaString** str) const override;
		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getNext(agaliaElement** next) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;
		virtual HRESULT getAdditionalInfoCount(uint32_t* row) const override;
		virtual HRESULT getAdditionalInfoValue(uint32_t row, agaliaString** str) const override;
		
		enum {
			prop_red = BMP_item_Base::prop_last,
			prop_green,
			prop_blue,
			prop_last
		};


		LONG scans = 0;
		LONG bytes_of_line = 0;
		LONG biCompression = 0;
	};
}

