﻿#pragma once

#include "agaliareptImpl.h"
#include "byteswap.h"

namespace analyze_ISO
{
	class item_Box : public _agaliaItemBase
	{
	public:
		item_Box(const agaliaContainer* image, uint64_t offset, uint64_t size, uint64_t endpos, uint32_t parent);
		virtual ~item_Box();

		virtual HRESULT getItemName(agaliaString** str) const override;
		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChildItem(uint32_t sibling, agaliaItem** child) const override;
		virtual HRESULT getNextItem(agaliaItem** next) const override;

		virtual HRESULT getAsocImage(const agaliaContainer** imageAsoc) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const;

		enum {
			prop_offset,
			prop_size,
			prop_type,
			prop_largesize,
			prop_last
		};

		const agaliaContainer* image = nullptr;
		static const GUID guid_iso;

		uint64_t endpos = 0;
		uint32_t parent = 0;
	};

	enum
	{
		column_offset,
		column_parent,
		column_type,
		column_size,
		column_value,
		column_last
	};


#pragma pack(push, 1)
	struct Box
	{
		uint32_t size;
		uint32_t type;

		uint32_t getSize(void) const { return agalia_byteswap(size); }
		uint32_t getType(void) const { return agalia_byteswap(type); }
		static uint64_t getBoxSize(const agaliaContainer* image, uint64_t offset, uint64_t endpos);
		static uint64_t getDataOffset(const agaliaContainer* image, uint64_t offset);
	};

	struct FullBox
	{
		uint8_t version;
		uint8_t flags[3];
	};
#pragma pack(pop)
}
