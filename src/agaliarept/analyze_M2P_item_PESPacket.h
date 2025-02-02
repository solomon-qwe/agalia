#pragma once

#include "agaliareptImpl.h"
#include "analyze_M2P_item_Base.h"

namespace analyze_M2P
{
	class item_PES_packet : public M2P_item_Base
	{
	public:
		item_PES_packet(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_PES_packet();

		virtual HRESULT getName(agaliaString** str) const override;
		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getNext(agaliaElement** next) const override;

		virtual HRESULT getElementInfoCount(uint32_t* row) const override;
		virtual HRESULT getElementInfoValue(uint32_t row, uint32_t column, agaliaString** str) const override;

		enum
		{
			prop_packet_start_code_prefix = M2P_item_Base::prop_last,
			prop_stream_id,
			prop_PES_packet_length,
			prop_last
		};

		static uint64_t calc_item_size(const agaliaContainer* image, uint64_t offset);
	};
}
