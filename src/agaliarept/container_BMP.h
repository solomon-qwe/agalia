#pragma once

#include "agaliaImageBase.h"

namespace analyze_BMP
{
	class container_BMP : public _agaliaContainerBase
	{
	protected:
		container_BMP(const wchar_t* path, IStream* stream);
		virtual ~container_BMP();

		virtual HRESULT getColumnCount(uint32_t* count) const override;
		virtual HRESULT getColumnWidth(uint32_t column, int32_t* length) const override;
		virtual HRESULT getColumnName(uint32_t column, agaliaString** str) const override;
		virtual HRESULT getGridRowCount(const agaliaItem* item, uint32_t* row) const override;
		virtual HRESULT getGridValue(const agaliaItem* item, uint32_t row, uint32_t column, agaliaString** str) const override;

		virtual HRESULT getRootItem(agaliaItem** root) const override;

		virtual HRESULT getPropertyValue(PropertyType type, agaliaString** str) const override;
		virtual HRESULT getThumbnailImage(HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH) const override;

	public:
		static _agaliaContainerBase* CreateInstance(const wchar_t* path, IStream* stream);
		static bool IsSupported(IStream* stream);
		static HRESULT GetContainerName(agaliaString** name);
	};
};

