#pragma once


#include "analyze_TIFF_util.h"

namespace analyze_TIFF
{
	template <typename TYPE_IFD>
	class item_IFDEntry : public TIFF_item_Base
	{
	public:
		item_IFDEntry(const container_TIFF* image, uint64_t offset, uint64_t size, int parent_ifd_type, uint16_t parent_tag, uint64_t parent_offset, uint64_t entry_index, uint32_t burst_err_count)
			: TIFF_item_Base(image, offset, size)
		{
			this->parent_tag = parent_tag;
			this->parent_ifd_type = parent_ifd_type;
			this->parent_offset = parent_offset;
			this->entry_index = entry_index;
			this->burst_err_count = burst_err_count;
		}



		virtual ~item_IFDEntry()
		{
		}



		virtual HRESULT getName(agaliaString** str) const override
		{
			rsize_t bufsize = 0;
			auto hr = UInt64ToSizeT(getSize(), &bufsize);
			if (FAILED(hr)) return hr;

			CHeapPtr<TYPE_IFD::entry_type> entry;
			if (!entry.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
			hr = image->ReadData(entry, getOffset(), getSize());
			if (FAILED(hr)) return hr;

			uint16_t tag = get_tiff_value(image->byte_order, entry->tag);

			std::wstringstream temp;
			format_hex(temp, tag);
			*str = agaliaString::create(temp.str().c_str());
			return S_OK;
		}



		enum {
			prop_offset,
			prop_tag,
			prop_name,
			prop_type,
			prop_count,
			prop_value_offset,
			prop_value,
			prop_last
		};



		virtual HRESULT getPropCount(uint32_t* count) const override
		{
			if (count == nullptr) return E_POINTER;
			*count = prop_last;
			return S_OK;
		}



		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override
		{
			const wchar_t* name = nullptr;
			switch (index)
			{
			case prop_offset:	name = L"offset";	break;
			case prop_tag:		name = L"tag";		break;
			case prop_name:		name = L"name";		break;
			case prop_type:		name = L"type";		break;
			case prop_count:	name = L"count";	break;
			case prop_value_offset:	name = L"value offset";	break;
			case prop_value:	name = L"value";	break;
			default:
				return E_FAIL;
			}

			*str = agaliaString::create(name);
			return S_OK;
		}



		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override
		{
			rsize_t bufsize = 0;
			auto hr = UInt64ToSizeT(getSize(), &bufsize);
			if (FAILED(hr)) return hr;

			CHeapPtr<TYPE_IFD::entry_type> entry;
			if (!entry.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
			hr = image->ReadData(entry, getOffset(), getSize());
			if (FAILED(hr)) return hr;

			std::wstringstream temp;
			if (index == prop_offset) {
				return getItemPropValue_offset(str);
			}
			else if (index == prop_tag) {
				return getItemPropValue_tag(str, entry);
			}
			else if (index == prop_name) {
				return getItemPropValue_name(str, entry);
			}
			else if (index == prop_type) {
				return getTiffTypeName(get_tiff_value(image->byte_order, entry->type), str);
			}
			else if (index == prop_count) {
				return getItemPropValue_count(str, entry);
			}
			else if (index == prop_value_offset) {
				return getItemPropValue_offset(str);
			}
			else if (index == prop_value) {
				return getItemPropValue_value(str, entry);
			}
			return E_FAIL;
		}



		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override
		{
			// エントリデータを読み込む 
			rsize_t bufsize = 0;
			auto hr = UInt64ToSizeT(getSize(), &bufsize);
			if (FAILED(hr)) return hr;

			CHeapPtr<TYPE_IFD::entry_type> entry;
			if (!entry.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
			hr = image->ReadData(entry, getOffset(), getSize());
			if (FAILED(hr)) return hr;

			// 値の位置を取得 
			uint64_t value_offset = 0;
			hr = getTiffValueOffset<TYPE_IFD, TYPE_IFD::entry_type>(&value_offset, image, this, entry);
			if (FAILED(hr)) return hr;

			if (get_tiff_value(image->byte_order, entry->count) <= sibling) {
				return E_FAIL;
			}

			// SubIFDを持つタグであれば子アイテムを生成 
			auto tag = get_tiff_value(image->byte_order, entry->tag);
			switch (tag)
			{
			case ifd_exif:		// ExifIFD
			case ifd_gps:		// GPS
			case ifd_interoperability:	// Interoperability
			case ifd_subifds:	// SubIFDs
				return getEntryChildItem<TYPE_IFD>(image, child, sibling, value_offset, tag);
			}

			// タグの型がSubIFDであれば子アイテムを生成 
			auto type = get_tiff_value(image->byte_order, entry->type);
			switch (type)
			{
			case TIFFTYPEID::TYPE_IFD:
				return getEntryChildItem<TYPE_IFD>(image, child, sibling, value_offset, ifd_subifd);
			case TIFFTYPEID::TYPE_IFD8:
				return getEntryChildItem<TYPE_IFD>(image, child, sibling, value_offset, ifd_subifd);
			}

			return E_FAIL;
		}



		virtual HRESULT getNext(agaliaElement** next) const override
		{
			decltype(TYPE_IFD::count) ifd_count = 0;
			auto hr = image->ReadData(&ifd_count, parent_offset, sizeof(ifd_count));
			if (FAILED(hr)) return hr;

			// 終端のエントリに達したか 
			if (get_tiff_value(image->byte_order, ifd_count) - 1 <= entry_index)
				return E_FAIL;

			// タイプの読み取りエラーが連続していないか 
			// （NikonのCapture NXで保存するとSubIFDのNextIFDが不正になる模様。連続してエラーが見つかったら処理中断することでロックを防ぐ） 
			uint32_t err_count = burst_err_count;
			{
				rsize_t bufsize = 0;
				hr = UInt64ToSizeT(getSize(), &bufsize);
				if (FAILED(hr)) return hr;

				CHeapPtr<TYPE_IFD::entry_type> entry;
				if (!entry.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
				hr = image->ReadData(entry, getOffset(), getSize());
				if (FAILED(hr)) return hr;

				agaliaStringPtr str;
				hr = getTiffTypeName(get_tiff_value(image->byte_order, entry->type), &str);
				if (FAILED(hr)) {
					err_count++;
				}
				else {
					err_count = 0;
				}
			}
			if (10 < err_count) {
				return AGALIA_ERR_DATA_CORRUPT;
			}

			// 次のエントリを生成 
			*next = new item_IFDEntry<TYPE_IFD>(
				image,
				getOffset() + sizeof(TYPE_IFD::entry_type),
				getSize(),
				parent_ifd_type,
				parent_tag,
				parent_offset,
				entry_index + 1,
				err_count);
			return S_OK;
		}



		virtual HRESULT getValueAreaOffset(uint64_t* offset) const override
		{
			rsize_t bufsize = 0;
			auto hr = UInt64ToSizeT(getSize(), &bufsize);
			if (FAILED(hr)) return hr;

			CHeapPtr<TYPE_IFD::entry_type> entry;
			if (!entry.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
			hr = image->ReadData(entry, getOffset(), getSize());
			if (FAILED(hr)) return hr;

			uint64_t value_offset = 0;
			hr = getTiffValueOffset<TYPE_IFD, TYPE_IFD::entry_type>(&value_offset, image, this, entry);
			if (FAILED(hr)) return hr;

			*offset = value_offset;
			return S_OK;
		}



		virtual HRESULT getValueAreaSize(uint64_t* size) const override
		{
			rsize_t bufsize = 0;
			auto hr = UInt64ToSizeT(getSize(), &bufsize);
			if (FAILED(hr)) return hr;

			CHeapPtr<TYPE_IFD::entry_type> entry;
			if (!entry.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
			hr = image->ReadData(entry, getOffset(), getSize());
			if (FAILED(hr)) return hr;

			rsize_t typesize = 0;
			hr = getTiffTypeSize(get_tiff_value(image->byte_order, entry->type), &typesize);
			if (FAILED(hr)) return hr;

			*size = static_cast<uint64_t>(typesize) * get_tiff_value(image->byte_order, entry->count);
			return S_OK;
		}



		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override
		{
			switch (column)
			{
			case column_offset:	return getColumnValue_offset(str);
			case column_ifd:	return getColumnValue_ifd(str);
			case column_tag:	return getColumnValue_tag(str);
			case column_name:	return getColumnValue_name(str);
			case column_type:	return getColumnValue_type(str);
			case column_count:	return getColumnValue_count(str);
			case column_value_offset: return getColumnValue_value_offset(str);
			case column_value:	return getColumnValue_value(str);
			}

			return E_FAIL;
		}



















		HRESULT getColumnValue_offset(agaliaString** str) const
		{
			std::wstringstream temp;
			format_hex(temp, getOffset(), 8);
			*str = agaliaString::create(temp.str().c_str());
			return S_OK;
		}



		HRESULT getColumnValue_ifd(agaliaString** str) const
		{
			if (parent_ifd_type == ifd_exif) {
				*str = agaliaString::create(L"Exif IFD");
			}
			else if (parent_ifd_type == ifd_gps) {
				*str = agaliaString::create(L"GPSInfo IFD");
			}
			else if (parent_ifd_type == ifd_interoperability) {
				*str = agaliaString::create(L"Interoperability IFD");
			}
			else if (parent_ifd_type == ifd_subifds) {
				*str = agaliaString::create(L"Sub IFD");
			}
			else if (parent_ifd_type == ifd_subifd) {
				*str = agaliaString::create(L"Sub IFD");
			}
			else if (parent_ifd_type == 1) {
				*str = agaliaString::create(L"1st IFD");
			}
			else if (parent_ifd_type == 2) {
				*str = agaliaString::create(L"2nd IFD");
			}
			else if (parent_ifd_type == 3) {
				*str = agaliaString::create(L"3rd IFD");
			}
			else if (0 <= parent_ifd_type) {
				std::wstringstream temp;
				temp << parent_ifd_type << L"th IFD";
				*str = agaliaString::create(temp.str().c_str());
			}
			else {
				return E_FAIL;
			}
			return S_OK;
		}



		HRESULT getColumnValue_tag(agaliaString** str) const
		{
			rsize_t bufsize = 0;
			auto hr = UInt64ToSizeT(getSize(), &bufsize);
			if (FAILED(hr)) return hr;

			CHeapPtr<TYPE_IFD::entry_type> entry;
			if (!entry.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
			hr = image->ReadData(entry, getOffset(), getSize());
			if (FAILED(hr)) return hr;

			std::wstringstream temp;
			format_hex(temp, get_tiff_value(image->byte_order, entry->tag));
			*str = agaliaString::create(temp.str().c_str());
			return S_OK;
		}



		HRESULT getColumnValue_name(agaliaString** str) const
		{
			rsize_t bufsize = 0;
			auto hr = UInt64ToSizeT(getSize(), &bufsize);
			if (FAILED(hr)) return hr;

			CHeapPtr<TYPE_IFD::entry_type> entry;
			if (!entry.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
			hr = image->ReadData(entry, getOffset(), getSize());
			if (FAILED(hr)) return hr;

			std::wstringstream parent;
			switch (parent_ifd_type)
			{
			case ifd_exif:
			case ifd_gps:
			case ifd_interoperability:
				format_hex(parent, parent_ifd_type, 4);
				break;
			}

			std::wstringstream tag;
			format_hex(tag, get_tiff_value(image->byte_order, entry->tag));

			std::wstring key;
			key = parent.str() + tag.str();

			std::unordered_map<std::wstring, std::wstring>::iterator it = g_tif_dic->find(key);
			if (it != g_tif_dic->end())
			{
				*str = agaliaString::create(it->second.c_str());
				return S_OK;
			}
			return E_FAIL;
		}



		HRESULT getColumnValue_type(agaliaString** str) const
		{
			rsize_t bufsize = 0;
			auto hr = UInt64ToSizeT(getSize(), &bufsize);
			if (FAILED(hr)) return hr;

			CHeapPtr<TYPE_IFD::entry_type> entry;
			if (!entry.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
			hr = image->ReadData(entry, getOffset(), getSize());
			if (FAILED(hr)) return hr;

			return getTiffTypeName(get_tiff_value(image->byte_order, entry->type), str);
		}



		HRESULT getColumnValue_count(agaliaString** str) const
		{
			rsize_t bufsize = 0;
			auto hr = UInt64ToSizeT(getSize(), &bufsize);
			if (FAILED(hr)) return hr;

			CHeapPtr<TYPE_IFD::entry_type> entry;
			if (!entry.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
			hr = image->ReadData(entry, getOffset(), getSize());
			if (FAILED(hr)) return hr;

			std::wstringstream temp;
			format_dec(temp, get_tiff_value(image->byte_order, entry->count));
			*str = agaliaString::create(temp.str().c_str());
			return S_OK;
		}



		HRESULT getColumnValue_value_offset(agaliaString** str) const
		{
			rsize_t bufsize = 0;
			auto hr = UInt64ToSizeT(getSize(), &bufsize);
			if (FAILED(hr)) return hr;

			CHeapPtr<TYPE_IFD::entry_type> entry;
			if (!entry.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
			hr = image->ReadData(entry, getOffset(), getSize());
			if (FAILED(hr)) return hr;

			std::wstringstream temp;
			format_hex(temp, get_tiff_value(image->byte_order, entry->offset));
			*str = agaliaString::create(temp.str().c_str());
			return S_OK;
		}



		HRESULT getColumnValue_value(agaliaString** str) const
		{
			rsize_t bufsize = 0;
			auto hr = UInt64ToSizeT(getSize(), &bufsize);
			if (FAILED(hr)) return hr;

			CHeapPtr<TYPE_IFD::entry_type> entry;
			if (!entry.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
			hr = image->ReadData(entry, getOffset(), getSize());
			if (FAILED(hr)) return hr;

			uint64_t value_offset = 0;
			hr = getTiffValueOffset<TYPE_IFD, TYPE_IFD::entry_type>(&value_offset, image, this, entry);
			if (FAILED(hr)) return hr;

			std::wstringstream temp;
			rsize_t count = static_cast<rsize_t>(min(128, get_tiff_value(image->byte_order, entry->count)));
			for (rsize_t i = 0; i < count; i++)
			{
				hr = format_tiff_value(temp, image, value_offset, get_tiff_value(image->byte_order, entry->type), i);
				if (hr != S_OK) {
					break;
				}
			}
			*str = agaliaString::create(temp.str().c_str());
			return S_OK;
		}










		HRESULT getItemPropValue_offset(agaliaString** str) const
		{
			std::wstringstream temp;
			format_dec(temp, getOffset());
			*str = agaliaString::create(temp.str().c_str());
			return S_OK;
		}



		HRESULT getItemPropValue_tag(agaliaString** str, typename TYPE_IFD::entry_type* entry) const
		{
			std::wstringstream temp;
			format_hex(temp, get_tiff_value(image->byte_order, entry->tag));
			*str = agaliaString::create(temp.str().c_str());
			return S_OK;
		}



		HRESULT getItemPropValue_name(agaliaString** str, typename TYPE_IFD::entry_type* entry) const
		{
			std::wstringstream parent;
			switch (parent_ifd_type)
			{
			case ifd_exif:
			case ifd_gps:
			case ifd_interoperability:
				format_hex(parent, parent_ifd_type, 4);
				break;
			}
			std::wstringstream tag;
			format_hex(tag, get_tiff_value(image->byte_order, entry->tag));

			std::wstring key;
			key = parent.str() + tag.str();

			std::unordered_map<std::wstring, std::wstring>::iterator it = g_tif_dic->find(key);
			if (it != g_tif_dic->end())
			{
				*str = agaliaString::create(it->second.c_str());
				return S_OK;
			}
			return E_FAIL;
		}



		HRESULT getItemPropValue_count(agaliaString** str, typename TYPE_IFD::entry_type* entry) const
		{
			std::wstringstream temp;
			format_dec(temp, get_tiff_value(image->byte_order, entry->count));
			*str = agaliaString::create(temp.str().c_str());
			return S_OK;
		}



		HRESULT getItemPropValue_offset(agaliaString** str, typename TYPE_IFD::entry_type* entry) const
		{
			std::wstringstream temp;
			format_hex(temp, get_tiff_value(image->byte_order, entry->offset));
			*str = agaliaString::create(temp.str().c_str());
			return S_OK;
		}



		HRESULT getItemPropValue_value(agaliaString** str, typename TYPE_IFD::entry_type* entry) const
		{
			// 値の位置を取得 
			uint64_t value_offset = 0;
			auto hr = getTiffValueOffset<TYPE_IFD, TYPE_IFD::entry_type>(&value_offset, image, this, entry);
			if (FAILED(hr)) return hr;

			if (parent_ifd_type == ifd_exif && get_tiff_value(image->byte_order, entry->tag) == 0x9286)
			{
				std::wstringstream temp;
				hr = format_exif_user_comment(temp, image, value_offset, get_tiff_value(image->byte_order, entry->count));
				if (hr == S_OK) {
					*str = agaliaString::create(temp.str().c_str());
					return S_OK;
				}
			}

			// 読み込む値の数を決める 
			rsize_t count = static_cast<rsize_t>(min(128, get_tiff_value(image->byte_order, entry->count)));

			// 値を文字列に変換 
			std::wstringstream temp;
			for (rsize_t i = 0; i < count; i++)
			{
				hr = format_tiff_value(temp, image, value_offset, get_tiff_value(image->byte_order, entry->type), i);
				if (hr != S_OK) {
					break;
				}
			}
			*str = agaliaString::create(temp.str().c_str());
			return S_OK;
		}





	protected:
		uint16_t parent_tag = 0;
		uint64_t parent_offset = 0;
		uint64_t entry_index = 0;
		uint32_t burst_err_count = 0;
		int parent_ifd_type = ifd_type::ifd_unknown;

		friend container_TIFF;
	};
}
