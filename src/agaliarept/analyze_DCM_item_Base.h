#pragma once

#include "agaliareptImpl.h"
#include "container_DCM_Impl.h"

namespace analyze_DCM
{
	class _agaliaItemDICOMBase : public _agaliaItemBase
	{
	public:
		_agaliaItemDICOMBase(const container_DCM_Impl* image, uint64_t offset, uint64_t size);
		virtual ~_agaliaItemDICOMBase();

		virtual HRESULT getAsocImage(const agaliaContainer** imageAsoc) const override
		{
			if (imageAsoc == nullptr) return E_POINTER;
			*imageAsoc = image;
			return S_OK;
		}

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const = 0;

		const container_DCM_Impl* image = nullptr;

		static const GUID guid_dicom;
	};
}
