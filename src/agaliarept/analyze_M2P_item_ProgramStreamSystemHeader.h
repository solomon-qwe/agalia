#pragma once

#include "agaliareptImpl.h"
#include "analyze_M2P_item_Base.h"

namespace analyze_M2P
{
	class program_stream_system_header;

	class item_ProgramStreamSystemHeader : public M2P_item_Base
	{
	public:
		item_ProgramStreamSystemHeader(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_ProgramStreamSystemHeader();

		virtual HRESULT getName(agaliaString** str) const override;
		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getElementInfoCount(uint32_t* row) const override;
		virtual HRESULT getElementInfoValue(uint32_t row, uint32_t column, agaliaString** str) const override;

		enum
		{
			prop_system_header_start_code = M2P_item_Base::prop_last,
			prop_header_length,
			prop_rate_bound,
			prop_audio_bound,
			prop_fixed_flag,
			prop_CSPS_flag,
			prop_system_audio_lock_flag,
			prop_system_video_lock_flag,
			prop_video_bound,
			prop_packet_rate_restriction_flag,
			prop_reserved_bits,
			prop_last
		};

		static uint64_t calc_item_size(const agaliaContainer* image, uint64_t offset);

		program_stream_system_header* bitfield = nullptr;
	};
}
