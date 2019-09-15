#pragma once
#include "agaliareptImpl.h"
#include "container_RIFF.h"

#include "byteswap.h"

#include <mmiscapi.h>
#include <AviFmt.h>
#include <Aviriff.h>


namespace analyze_RIFF
{
	class item_Base : public _agaliaItemBase
	{
	public:
		item_Base(const container_RIFF* _image, uint64_t offset, uint64_t size);
		virtual ~item_Base();

		virtual HRESULT getItemName(agaliaString** str) const override;

		virtual HRESULT getItemPropCount(uint32_t* count) const override;
		virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChildItem(uint32_t sibling, agaliaItem** child) const override;
		virtual HRESULT getNextItem(agaliaItem** next) const override;

		virtual HRESULT getAsocImage(const agaliaContainer** imageAsoc) const override;
		virtual HRESULT getValueAreaOffset(uint64_t* offset) const override;
		virtual HRESULT getValueAreaSize(uint64_t* size) const override;


		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const;

		virtual HRESULT getAdditionalInfoCount(uint32_t* row) const
		{
			*row = 0;
			return S_OK;
		}

		virtual HRESULT getAdditionalInfoValue(uint32_t row, agaliaString** str) const
		{
			row; str;
			return E_NOTIMPL;
		}

		enum {
			prop_offset,
			prop_fcc,
			prop_cb,
			prop_last
		};

		const container_RIFF* image = nullptr;
		static const GUID guid_riff;
		uint64_t endPos = 0;
		FOURCC fccType = 0;
		FOURCC fccParent = 0;
	};



	enum {
		column_offset,
		column_parent,
		column_id,
		column_size,
		column_value,
		column_last
	};



	item_Base* create_item(const container_RIFF* image, uint64_t offset, FOURCC fcc, DWORD cb, FOURCC fccType);



	inline HRESULT format_fcc(std::wstringstream& dst, FOURCC fcc)
	{
		return multibyte_to_widechar(reinterpret_cast<char*>(&fcc), sizeof(fcc), CP_US_ASCII, dst);
	}



	template <typename T>
	HRESULT format_chunk_dec(std::wstringstream& dst, rsize_t value_offset, const agaliaItem* item)
	{
		const agaliaContainer* image = nullptr;
		auto hr = item->getAsocImage(&image);
		if (FAILED(hr)) return hr;

		T val = 0;
		hr = image->ReadData(&val, item->getOffset() + value_offset, sizeof(val));
		if (FAILED(hr)) return hr;

		format_dec(dst, val);
		return S_OK;
	}



	template <typename T>
	HRESULT format_chunk_hex(std::wstringstream& dst, rsize_t value_offset, const agaliaItem* item)
	{
		const agaliaContainer* image = nullptr;
		auto hr = item->getAsocImage(&image);
		if (FAILED(hr)) return hr;

		T val = 0;
		hr = image->ReadData(&val, item->getOffset() + value_offset, sizeof(val));
		if (FAILED(hr)) return hr;

		format_hex(dst, val);
		return S_OK;
	}



	template <typename T>
	HRESULT format_chunk_fcc(std::wstringstream& dst, rsize_t value_offset, const agaliaItem* item)
	{
		const agaliaContainer* image = nullptr;
		auto hr = item->getAsocImage(&image);
		if (FAILED(hr)) return hr;

		T val = 0;
		hr = image->ReadData(&val, item->getOffset() + value_offset, sizeof(val));
		if (FAILED(hr)) return hr;

		format_fcc(dst, val);
		return S_OK;
	}



	template <typename T>
	HRESULT format_chunksub_dec(std::wstringstream& dst, rsize_t value_offset, const agaliaItem* item)
	{
		const agaliaContainer* image = nullptr;
		auto hr = item->getAsocImage(&image);
		if (FAILED(hr)) return hr;

		T val = 0;
		hr = image->ReadData(&val, item->getOffset() + sizeof(RIFFCHUNK) + value_offset, sizeof(val));
		if (FAILED(hr)) return hr;

		format_dec(dst, val);
		return S_OK;
	}



	template <typename T>
	HRESULT format_chunksub_fcc(std::wstringstream& dst, rsize_t value_offset, const agaliaItem* item)
	{
		const agaliaContainer* image = nullptr;
		auto hr = item->getAsocImage(&image);
		if (FAILED(hr)) return hr;

		T val = 0;
		hr = image->ReadData(&val, item->getOffset() + sizeof(RIFFCHUNK) + value_offset, sizeof(val));
		if (FAILED(hr)) return hr;

		format_fcc(dst, val);
		return S_OK;
	}
}