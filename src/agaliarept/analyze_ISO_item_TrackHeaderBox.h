#pragma once

#include "analyze_ISO_item_Box.h"

namespace analyze_ISO
{
    class item_TrackHeaderBox : public item_Box
    {
	public:
		item_TrackHeaderBox(const agaliaContainer* image, uint64_t offset, uint64_t size, uint64_t endpos, uint32_t parent);
		virtual ~item_TrackHeaderBox();

		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		enum {
			prop_version = item_Box::prop_last,
			prop_creation_time,
			prop_modification_time,
			prop_track_ID,
			prop_reserved1,
			prop_duration,
			prop_reserved2,
			prop_layer,
			prop_alternate_group,
			prop_volume,
			prop_reserved3,
			prop_matrix,
			prop_width,
			prop_height,
			prop_last
		};
	};

#pragma pack(push, 1)
	struct TrackHeaderBox : FullBox
	{
		union
		{
			struct
			{
				uint32_t creation_time;
				uint32_t modification_time;
				uint32_t track_ID;
				uint32_t reserved1;
				uint32_t duration;

				uint32_t reserved2[2];
				int16_t layer;
				int16_t alternate_group;
				int16_t volume;
				uint16_t reserved3;
				int32_t matrix[9];
				uint32_t width;
				uint32_t height;
			} version0;

			struct
			{
				uint64_t creation_time;
				uint64_t modification_time;
				uint32_t track_ID;
				uint32_t reserved1;
				uint64_t duration;

				uint32_t reserved2[2];
				int16_t layer;
				int16_t alternate_group;
				int16_t volume;
				uint16_t reserved3;
				int32_t matrix[9];
				uint32_t width;
				uint32_t height;
			} version1;
		};
	};
#pragma pack(pop)
}
