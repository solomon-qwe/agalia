#pragma once

#include "agaliareptImpl.h"
#include "analyze_M2P_item_Base.h"

namespace analyze_M2P
{
	class program_stream_pack_header;

	class item_ProgramStreamPackHeader : public item_Base
	{
	public:
		item_ProgramStreamPackHeader(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_ProgramStreamPackHeader();

		virtual HRESULT getItemName(agaliaString** str) const override;
		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChildItem(uint32_t sibling, agaliaItem** child) const override;
		virtual HRESULT getNextItem(agaliaItem** next) const override;

		virtual HRESULT getGridRowCount(uint32_t* row) const override;
		virtual HRESULT getGridValue(uint32_t row, uint32_t column, agaliaString** str) const override;

		enum
		{
			prop_pack_start_code = item_Base::prop_last,
			prop_01,
			prop_system_clock_reference_base,
			prop_system_clock_reference_extension,
			prop_program_mux_rate,
			prop_reserved,
			prop_pack_stuffing_length,
			prop_stuffing_byte,
			prop_last
		};

		static uint64_t calc_item_size(const agaliaContainer* image, uint64_t offset);

		program_stream_pack_header* bitfield = nullptr;
	};
}
