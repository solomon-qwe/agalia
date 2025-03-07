#pragma once

#include "agaliareptImpl.h"

namespace analyze_TIFF
{
	class container_TIFF : public _agaliaContainerBase
	{
	public:
		container_TIFF(const wchar_t* path, IStream* stream);
		virtual ~container_TIFF();

		virtual HRESULT ReadData(void* buf, uint64_t pos, uint64_t size) const override;

		virtual HRESULT getColumnCount(uint32_t* count) const override;
		virtual HRESULT getColumnWidth(uint32_t column, int32_t* length) const override;
		virtual HRESULT getColumnName(uint32_t column, agaliaString** str) const override;
		virtual HRESULT getElementInfoCount(const agaliaElement* item, uint32_t* row) const override;
		virtual HRESULT getElementInfoValue(const agaliaElement* item, uint32_t row, uint32_t column, agaliaString** str) const override;

		virtual HRESULT getRootElement(agaliaElement** root) const override;

		virtual HRESULT getPropertyValue(PropertyType type, agaliaString** str) const override;
		virtual HRESULT loadBitmap(agaliaBitmap** ppBitmap) const override;
		virtual HRESULT loadThumbnail(agaliaBitmap** ppBitmap, uint32_t maxW, uint32_t maxH) const override;

		static _agaliaContainerBase* CreateInstance(const wchar_t* path, IStream* stream);
		static bool IsSupported(IStream* stream);
		static HRESULT GetContainerName(agaliaString** name);

	public:
		bool is_exif = false;
		uint16_t byte_order = 0;
	};
}