#pragma once

#include "analyze_DCM_item_Base.h"

#include <list>

namespace analyze_DCM
{
	class _agaliaItemDICOMElement : public _agaliaItemDICOMBase
	{
	public:
		_agaliaItemDICOMElement(const container_DCM_Impl* image, uint64_t offset, uint64_t size);
		virtual ~_agaliaItemDICOMElement();

		virtual HRESULT getName(agaliaString** str) const override;
		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;
		virtual HRESULT getNext(agaliaElement** next) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;

		virtual HRESULT getValueAreaOffset(uint64_t* offset) const override;
		virtual HRESULT getValueAreaSize(uint64_t* size) const override;

		enum {
			prop_offset,
			prop_tag,
			prop_name,
			prop_vr,
			prop_length,
			prop_value_offset,
			prop_value,
			prop_max
		};

		uint16_t owner = 0;
		int layer = 0;
		uint64_t endPos = 0;
		std::list<int> codepage;
		bool is_undefine_length_sequence = false;
	};
}
