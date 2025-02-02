#pragma once

#include "agaliareptImpl.h"
#include "container_TIFF.h"

namespace analyze_TIFF
{
	class TIFF_item_Base : public _agaliaItemBase
	{
	public:
		TIFF_item_Base(const container_TIFF* image, uint64_t offset, uint64_t size);
		virtual ~TIFF_item_Base();

		virtual HRESULT getAsocImage(const agaliaContainer** imageAsoc) const;
		
		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const;

		static const GUID guid_tiff;

		const container_TIFF* image = nullptr;
	};


	enum {
		column_offset,
		column_ifd,
		column_tag,
		column_name,
		column_type,
		column_count,
		column_value_offset,
		column_value,
		column_last
	};

	enum ifd_type
	{
		ifd_interoperability = 0xA005,
		ifd_gps = 0x8825,
		ifd_exif = 0x8769,
		ifd_subifds = 0x014A,
		ifd_subifd = -2,
		ifd_unknown = -1,
	};
}
