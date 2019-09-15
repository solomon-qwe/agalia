#pragma once

#include "agaliareptImpl.h"
#include "analyze_M2P_item_Base.h"

namespace analyze_M2P
{
	class item_PES_packet : public item_Base
	{
	public:
		item_PES_packet(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_PES_packet();

		virtual HRESULT getItemName(agaliaString** str) const override;
		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getNextItem(agaliaItem** next) const override;

		virtual HRESULT getGridRowCount(uint32_t* row) const override;
		virtual HRESULT getGridValue(uint32_t row, uint32_t column, agaliaString** str) const override;

		enum
		{
			prop_packet_start_code_prefix = item_Base::prop_last,
			prop_stream_id,
			prop_PES_packet_length,
			prop_last
		};

		static uint64_t calc_item_size(const agaliaContainer* image, uint64_t offset);
	};
}
