#pragma once

#include "analyze_ISO_item_Box.h"

namespace analyze_ISO
{
    class item_MovieHeaderBox : public item_Box
    {
	public:
		item_MovieHeaderBox(const agaliaContainer* image, uint64_t offset, uint64_t size, uint64_t endpos, uint32_t parent);
		virtual ~item_MovieHeaderBox();

		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		enum {
			prop_version = item_Box::prop_last,
			prop_creation_time,
			prop_modification_time,
			prop_timescale,
			prop_duration,
			prop_rate,
			prop_volume,
			prop_reserved1,
			prop_reserved2,
			prop_matrix,
			prop_pre_defined,
			prop_next_track_ID,
			prop_last
		};
	};

#pragma pack(push, 1)
	struct MovieHeaderBox : FullBox
	{
		union
		{
			struct
			{
				uint32_t creation_time;
				uint32_t modification_time;
				uint32_t timescale;
				uint32_t duration;

				int32_t rate;
				int16_t volume;
				uint16_t reserved1;
				uint32_t reserved2[2];
				int32_t matrix[9];
				uint32_t pre_defined[6];
				uint32_t next_track_ID;
			} version0;

			struct
			{
				uint64_t creation_time;
				uint64_t modification_time;
				uint32_t timescale;
				uint64_t duration;

				int32_t rate;
				int16_t volume;
				uint16_t reserved1;
				uint32_t reserved2[2];
				int32_t matrix[9];
				uint32_t pre_defined[6];
				uint32_t next_track_ID;
			} version1;
		};
	};
#pragma pack(pop)
}
