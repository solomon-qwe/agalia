#pragma once

#include "analyze_ICC_item_Base.h"

namespace analyze_ICC
{
	class item_Header : public item_Base
	{
	public:
		item_Header(const container_ICC* image, uint64_t offset, uint64_t size, uint64_t endpos);
		virtual ~item_Header();

		virtual HRESULT getItemName(agaliaString** str) const override;
		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChildItem(uint32_t sibling, agaliaItem** child) const override;
		virtual HRESULT getNextItem(agaliaItem** next) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const;

		enum {
			prop_profile_size = item_Base::prop_last,
			prop_preferred_cmm_type,
			prop_profile_version,
			prop_profile_device_class,
			prop_data_colour_space,
			prop_pcs,
			prop_date_and_time,
			prop_profile_file_signature,
			prop_primary_platform,
			prop_profile_flags,
			prop_device_manufacturer,
			prop_device_model,
			prop_device_attributes,
			prop_rendering_intent,
			prop_pcs_illuminant,
			prop_profile_creator,
			prop_profile_id,
			prop_last
		};
	};

#pragma pack(push, 1)
	struct Header
	{
		uint32_t ProfileSize;
		uint32_t PreferredCMMType;
		uint32_t ProfileVersion;
		uint32_t ProfileDeviceClass;
		uint32_t DataColourSpace;
		uint32_t PCS;
		uint16_t DateAndTime[6];
		uint32_t ProfileFileSignature;
		uint32_t PrimaryPlatform;
		uint32_t ProfileFlags;
		uint32_t DeviceManufacturer;
		uint32_t DeviceModel;
		uint64_t DeviceAttributes;
		uint32_t RenderingIntent;
		uint32_t PCSIlluminant[3];
		uint32_t ProfileCreator;
		uint8_t ProfileID[16];
		uint8_t Reserved[28];
	};
#pragma pack(pop)
}
