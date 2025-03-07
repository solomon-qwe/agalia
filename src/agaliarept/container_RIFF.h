#pragma once

namespace analyze_RIFF
{
	class container_RIFF : public _agaliaContainerBase
	{
	public:
		container_RIFF(const wchar_t* path, IStream* stream);
		virtual ~container_RIFF();

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
	};
}
