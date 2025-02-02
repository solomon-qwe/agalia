#pragma once
#include "analyze_RIFF_item_Base.h"

namespace analyze_RIFF
{
	class item_idx1 : public RIFF_item_Base
	{
	public:
		item_idx1(const container_RIFF* _image, uint64_t offset, uint64_t size);
		virtual ~item_idx1();

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;

		virtual HRESULT getValueAreaOffset(uint64_t* offset) const override;
		virtual HRESULT getValueAreaSize(uint64_t* size) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;
	};


	class item_idx1_entry : public RIFF_item_Base
	{
	public:
		item_idx1_entry(const container_RIFF* _image, uint64_t offset, uint64_t size);
		virtual ~item_idx1_entry();

		virtual HRESULT getName(agaliaString** str) const override;

		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;
		virtual HRESULT getNext(agaliaElement** next) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;

		enum {
			prop_dwChunkId = 0,
			prop_dwFlags,
			prop_dwOffset,
			prop_dwSize,
			prop_last,
		};

		int _index = 0;
	};
}
