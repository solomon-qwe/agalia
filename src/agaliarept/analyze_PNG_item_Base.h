#pragma once

#include "agaliareptImpl.h"
#include "container_PNG.h"
#include "byteswap.h"

namespace analyze_PNG
{
	class item_Base : public _agaliaItemBase
	{
	public:
		item_Base(const container_PNG* image, uint64_t offset, uint64_t size, uint64_t endpos);
		virtual ~item_Base();

		virtual HRESULT getItemName(agaliaString** str) const override;
		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChildItem(uint32_t sibling, agaliaItem** child) const override;
		virtual HRESULT getNextItem(agaliaItem** next) const override;

		virtual HRESULT getAsocImage(const agaliaContainer** imageAsoc) const override;
		
		virtual HRESULT getValueAreaOffset(uint64_t* offset) const override;
		virtual HRESULT getValueAreaSize(uint64_t* size) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const;

		enum {
			prop_offset,
			prop_length,
			prop_chunk_type,
			prop_crc,
			prop_last
		};

		const container_PNG* image = nullptr;
		static const GUID guid_png;
		uint64_t endpos;
	};

	enum {
		column_offset,
		column_length,
		column_chunk_type,
		column_crc,
		column_value,
		column_last
	};

#pragma pack(push, 1)
	struct Chunk
	{
		uint32_t Length;
		uint32_t ChunkType;
		uint8_t ChunkData[1];

		uint32_t getLength(void) const { return agalia_byteswap(Length); };
		uint32_t getChunkType(void) const { return agalia_byteswap(ChunkType); };
	};
#pragma pack(pop)
}
