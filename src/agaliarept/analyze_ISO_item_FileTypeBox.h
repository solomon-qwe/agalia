#pragma once

#include "analyze_ISO_item_Box.h"

namespace analyze_ISO
{
	class item_FileTypeBox : public ISO_item_Box
	{
	public:
		item_FileTypeBox(const agaliaContainer* image, uint64_t offset, uint64_t size, uint64_t endpos, uint32_t parent);
		virtual ~item_FileTypeBox();

		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		enum {
			prop_major_brand = ISO_item_Box::prop_last,
			prop_minor_version,
			prop_compatible_brands,
			prop_last
		};
	};


#pragma pack(push, 1)
	struct FileTypeBox
	{
		uint32_t major_brand;
		uint32_t minor_version;
		//uint32_t compatible_brands[];

		uint32_t getMajorBrand(void) const { return agalia_byteswap(major_brand); }
		uint32_t getMinorVersion(void) const { return agalia_byteswap(minor_version); }
	};
#pragma pack(pop)

}
