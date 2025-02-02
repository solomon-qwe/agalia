#pragma once

#include "container_DCM_Impl.h"
#include "analyze_DCM_item_Base.h"

namespace analyze_DCM
{
	const int dicom_preamble_size = 128;

	class _agaliaItemDICOMPreamble : public _agaliaItemDICOMBase
	{
	public:
		_agaliaItemDICOMPreamble(const container_DCM_Impl* image, uint64_t offset, uint64_t size);
		virtual ~_agaliaItemDICOMPreamble();

		virtual HRESULT getName(agaliaString** str) const override;
		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;
		virtual HRESULT getNext(agaliaElement** next) const override;

		virtual HRESULT getColumnValue(uint32_t, agaliaString**) const override { return E_FAIL; }

		enum {
			prop_offset,
			prop_preamble,
			prop_prefix,
			prop_max
		};
	};
}
