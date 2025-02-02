#pragma once

#include "analyze_PNG_item_Base.h"
#include "byteswap.h"

namespace analyze_PNG
{
	class item_IHDR : public PNG_item_Base
	{
	public:
		item_IHDR(const container_PNG* image, uint64_t offset, uint64_t size, uint64_t endpos);
		virtual ~item_IHDR();

		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;

		enum {
			prop_width = PNG_item_Base::prop_last,
			prop_height,
			prop_bit_depth,
			prop_color_type,
			prop_compression_method,
			prop_filter_method,
			prop_interlace_method,
			prop_last
		};
	};

#pragma pack(push, 1)
	struct IHDR
	{
		uint32_t Width;
		uint32_t Height;
		uint8_t BitDepth;
		uint8_t ColorType;
		uint8_t CompressionMethod;
		uint8_t FilterMethod;
		uint8_t InterlaceMethod;

		uint32_t getWidth(void) const { return agalia_byteswap(Width); };
		uint32_t getHeight(void) const { return agalia_byteswap(Height); };
		uint8_t getBitDepth(void) const { return agalia_byteswap(BitDepth); };
		uint8_t getColorType(void) const { return agalia_byteswap(ColorType); };
		uint8_t getCompressionMethod(void) const { return agalia_byteswap(CompressionMethod); };
		uint8_t getFilterMethod(void) const { return agalia_byteswap(FilterMethod); };
		uint8_t getInterlaceMethod(void) const { return agalia_byteswap(InterlaceMethod); };
	};
#pragma pack(pop)
}
