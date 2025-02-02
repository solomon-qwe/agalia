#pragma once

#include "analyze_TIFF_item_Entry.h"

namespace analyze_TIFF
{
	class item_IFDBase : public TIFF_item_Base
	{
	public:
		item_IFDBase(const container_TIFF* image, uint64_t offset, uint64_t size, int ifdtype);
		virtual ~item_IFDBase();

		enum {
			prop_offset,
			prop_count,
			prop_next_offset,
			prop_last
		};

		virtual HRESULT getName(agaliaString** str) const override;
		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;

	protected:
		int ifd_type_id = ifd_type::ifd_unknown;

		friend container_TIFF;
	};


	template <typename IFD_TYPE>
	class item_IFD : public item_IFDBase
	{
	public:
		item_IFD(const container_TIFF* image, uint64_t offset, uint64_t size, int ifdtype = ifd_unknown)
			: item_IFDBase(image, offset, size, ifdtype)
		{
		}

		virtual ~item_IFD()
		{
		}


		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override
		{
			rsize_t bufsize = 0;
			auto hr = UInt64ToSizeT(getSize(), &bufsize);
			if (FAILED(hr)) return hr;

			CHeapPtr<uint8_t> buf;
			if (!buf.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
			hr = image->ReadData(buf, getOffset(), getSize());
			if (FAILED(hr)) return hr;

			auto ifd = reinterpret_cast<IFD_TYPE*>(buf.m_pData);

			std::wstringstream temp;
			if (index == prop_offset)
			{
				format_dec(temp, getOffset());
			}
			else if (index == prop_count)
			{
				format_dec(temp, get_tiff_value(image->byte_order, ifd->count));
			}
			else if (index == prop_next_offset)
			{
				format_dec(temp,
					get_tiff_value(image->byte_order,
						*reinterpret_cast<const IFD_TYPE::next_offset_type*>(
							buf.m_pData +
							sizeof(ifd->count) +
							sizeof(ifd->entry) * get_tiff_value(image->byte_order, ifd->count))));
			}
			else 
			{
				return E_FAIL;
			}

			*str = agaliaString::create(temp.str().c_str());
			return S_OK;
		}



		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override
		{
			if (sibling != 0) return E_FAIL;

			rsize_t bufsize = 0;
			auto hr = UInt64ToSizeT(getSize(), &bufsize);
			if (FAILED(hr)) return hr;

			CHeapPtr<IFD_TYPE> ifd;
			if (!ifd.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
			hr = image->ReadData(ifd, getOffset(), getSize());
			if (FAILED(hr)) return hr;

			if (ifd->count == 0) return E_FAIL;

			uint64_t child_offset = getOffset() + offsetof(IFD_TYPE, entry);
			uint64_t child_size = sizeof(IFD_TYPE::entry);

			uint16_t parent_tag = 0;
			switch (ifd_type_id)
			{
			case ifd_exif:
			case ifd_gps:
			case ifd_interoperability:
			case ifd_subifds:
				parent_tag = static_cast<uint16_t>(ifd_type_id);
				break;
			}

			*child = new item_IFDEntry<IFD_TYPE>(image, child_offset, child_size, ifd_type_id, parent_tag, getOffset(), 0, 0);
			return S_OK;
		}



		virtual HRESULT getNext(agaliaElement** next) const override
		{
			// 現在の IFD 構造体を読み込む
			size_t bufsize = 0;
			auto hr = UInt64ToSizeT(getSize(), &bufsize);
			if (FAILED(hr)) return hr;

			CHeapPtr<uint8_t> buf;
			if (!buf.AllocateBytes(bufsize)) return E_POINTER;
			hr = image->ReadData(buf, getOffset(), getSize());
			if (FAILED(hr)) return hr;
			
			auto ifd = reinterpret_cast<IFD_TYPE*>(buf.m_pData);

			// IFD 構造体末尾の next_offset を読み込む
			uint64_t next_offset_pos =
				static_cast<uint64_t>(sizeof(IFD_TYPE::count)) +
				static_cast<uint64_t>(sizeof(IFD_TYPE::entry)) * get_tiff_value(image->byte_order, ifd->count);
			rsize_t pos = 0;
			hr = UInt64ToSizeT(next_offset_pos, &pos);
			if (FAILED(hr)) return hr;

			auto next_offset = get_tiff_value(image->byte_order, *reinterpret_cast<typename IFD_TYPE::next_offset_type*>(buf.m_pData + pos));

			if (next_offset == 0)
				return E_FAIL;

			// 次の IFD の位置とサイズを取得 
			decltype(IFD_TYPE::count) next_ifd_count = 0;
			hr = image->ReadData(&next_ifd_count, next_offset, sizeof(next_ifd_count));
			if (FAILED(hr)) return hr;

			uint64_t next_ifd_offset = next_offset;
			uint64_t next_ifd_size =
				static_cast<uint64_t>(sizeof(IFD_TYPE::count)) +
				static_cast<uint64_t>(sizeof(IFD_TYPE::entry)) * get_tiff_value(image->byte_order, next_ifd_count) +
				sizeof(IFD_TYPE::next_offset_type);
			int next_ifd_type = 0;
			if (ifd_type_id < 0) {
				next_ifd_type = ifd_unknown;
			}
			else if (ifd_type_id == ifd_subifds) {
				next_ifd_type = ifd_subifds;
			}
			else {
				next_ifd_type = ifd_type_id + 1;
			}

			// 次の IFD を生成 
			*next = new item_IFD(image, next_ifd_offset, next_ifd_size, next_ifd_type);
			return S_OK;
		}
	};
}

