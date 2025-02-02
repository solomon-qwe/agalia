#pragma once

#include "analyze_JPEG_item_Base.h"

namespace analyze_JPEG
{
	class item_Marker : public JPEG_item_Base
	{
	public:
		item_Marker(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type);
		virtual ~item_Marker();

		virtual HRESULT getName(agaliaString** str) const override;
		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getNext(agaliaElement** next) const override;

		virtual HRESULT getValueAreaOffset(uint64_t* offset) const override;
		virtual HRESULT getValueAreaSize(uint64_t* size) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;
		virtual HRESULT getAdditionalInfoCount(uint32_t* row) const override;
		virtual HRESULT getAdditionalInfoValue(uint32_t row, agaliaString** str) const override;

	protected:
		friend container_JPEG;
	};


	HRESULT create_item(agaliaElement** next, uint64_t next_item_offset, const container_JPEG* image);
}

