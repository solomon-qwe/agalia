#pragma once

#include "container_DCM.h"

namespace analyze_DCM
{
	enum {
		column_offset,
		column_tag,
		column_name,
		column_vr,
		column_length,
		column_value,
		column_max
	};

	class container_DCM_Impl : public container_DCM
	{
	public:
		container_DCM_Impl(const wchar_t* path, IStream* stream);
		virtual ~container_DCM_Impl();

		virtual HRESULT getColumnCount(uint32_t* count) const override;
		virtual HRESULT getColumnWidth(uint32_t column, int32_t* length) const override;
		virtual HRESULT getColumnName(uint32_t column, agaliaString** str) const override;
		virtual HRESULT getGridRowCount(const agaliaItem* item, uint32_t* row) const override;
		virtual HRESULT getGridValue(const agaliaItem* item, uint32_t row, uint32_t column, agaliaString** str) const override;

		virtual HRESULT getRootItem(agaliaItem** root) const override;

		virtual HRESULT getPropertyValue(PropertyType type, agaliaString** str) const override;
		virtual HRESULT getThumbnailImage(HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH) const override;

		uint64_t getImageSize(void) const
		{
			ULARGE_INTEGER pos = {};
			LockStream();
			auto hr = data_stream->Seek(int64_to_li(0), STREAM_SEEK_END, &pos);
			UnlockStream();
			if (FAILED(hr)) return 0;
			return pos.QuadPart;
		}

		TransferSyntax _transferSyntax = ExplicitVR_LittleEndian;
	};
}
