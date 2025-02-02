#pragma once

#include <stdint.h>

#include "tiff_common.h"
#include "container_TIFF.h"
#include "analyze_TIFF_item_Base.h"

namespace analyze_TIFF
{
	template <typename IFD_TYPE> class item_IFD;

	HRESULT format_exif_user_comment(std::wstringstream& str, const container_TIFF* image, uint64_t value_offset, uint64_t value_size);
	HRESULT format_tiff_value(std::wstringstream& str, const container_TIFF* image, uint64_t value_offset, uint16_t type, rsize_t index);


	// TIFFの型の文字列を取得 
	inline HRESULT getTiffTypeName(uint16_t type, agaliaString** str)
	{
		const wchar_t* name = nullptr;
		switch (type)
		{
		case TIFFTYPEID::TYPE_BYTE:		name = L"BYTE";		break;
		case TIFFTYPEID::TYPE_ASCII:	name = L"ASCII";	break;
		case TIFFTYPEID::TYPE_SHORT:	name = L"SHORT";	break;
		case TIFFTYPEID::TYPE_LONG:		name = L"LONG";		break;
		case TIFFTYPEID::TYPE_RATIONAL:	name = L"RATIONAL";	break;
		case TIFFTYPEID::TYPE_SBYTE:	name = L"SBYTE";	break;
		case TIFFTYPEID::TYPE_UNDEFINED:name = L"UNDEFINED"; break;
		case TIFFTYPEID::TYPE_SSHORT:	name = L"SSHORT";	break;
		case TIFFTYPEID::TYPE_SLONG:	name = L"SLONG";	break;
		case TIFFTYPEID::TYPE_SRATIONAL:name = L"SRATIONAL"; break;
		case TIFFTYPEID::TYPE_FLOAT:	name = L"FLOAT";	break;
		case TIFFTYPEID::TYPE_DOUBLE:	name = L"DOUBLE";	break;
		case TIFFTYPEID::TYPE_IFD:		name = L"IFD";		break;
		case TIFFTYPEID::TYPE_LONG8:	name = L"LONG8";	break;
		case TIFFTYPEID::TYPE_SLONG8:	name = L"SLONG8";	break;
		case TIFFTYPEID::TYPE_IFD8:		name = L"IFD8";		break;
		}
		if (name == nullptr) return E_FAIL;
		*str = agaliaString::create(name);
		return S_OK;
	}

	// TIFFの型のサイズ（バイト単位）を取得 
	inline HRESULT getTiffTypeSize(uint16_t type, rsize_t* size)
	{
		switch (type)
		{
		case TIFFTYPEID::TYPE_BYTE:		*size = sizeof(TIFFBYTE);		return S_OK;
		case TIFFTYPEID::TYPE_ASCII:	*size = sizeof(TIFFASCII);		return S_OK;
		case TIFFTYPEID::TYPE_SHORT:	*size = sizeof(TIFFSHORT);		return S_OK;
		case TIFFTYPEID::TYPE_LONG:		*size = sizeof(TIFFLONG);		return S_OK;
		case TIFFTYPEID::TYPE_RATIONAL:	*size = sizeof(TIFFRATIONAL);	return S_OK;
		case TIFFTYPEID::TYPE_SBYTE:	*size = sizeof(TIFFSBYTE);		return S_OK;
		case TIFFTYPEID::TYPE_UNDEFINED:*size = sizeof(TIFFUNDEFINED);	return S_OK;
		case TIFFTYPEID::TYPE_SSHORT:	*size = sizeof(TIFFSSHORT);		return S_OK;
		case TIFFTYPEID::TYPE_SLONG:	*size = sizeof(TIFFSLONG);		return S_OK;
		case TIFFTYPEID::TYPE_SRATIONAL:*size = sizeof(TIFFSRATIONAL);	return S_OK;
		case TIFFTYPEID::TYPE_FLOAT:	*size = sizeof(TIFFFLOAT);		return S_OK;
		case TIFFTYPEID::TYPE_DOUBLE:	*size = sizeof(TIFFDOUBLE);		return S_OK;
		case TIFFTYPEID::TYPE_IFD:		*size = sizeof(uint32_t);		return S_OK;
		case TIFFTYPEID::TYPE_LONG8:	*size = sizeof(TIFFLONG8);		return S_OK;
		case TIFFTYPEID::TYPE_SLONG8:	*size = sizeof(TIFFSLONG8);		return S_OK;
		case TIFFTYPEID::TYPE_IFD8:		*size = sizeof(uint64_t);		return S_OK;
		}
		return E_FAIL;
	}

	// TIFFENTRYの値のオフセット位置を取得 
	// TYPE_IFD : エントリを含む IFD の型（TIFFIFD or TIFFIFD8）
	// ENTRY_TYPE : エントリの型（TIFFIFDENTRY or TIFFIFDENTRY8）
	// result : 取得した値を格納する変数へのポインタ 
	// image : 画像データ管理オブジェクト 
	// item : エントリ管理オブジェクト 
	// entry : エントリデータ 
	template <typename TYPE_IFD, typename ENTRY_TYPE>
	HRESULT getTiffValueOffset(uint64_t* result, const container_TIFF* image, const TIFF_item_Base* item, const ENTRY_TYPE* entry)
	{
		// エントリに含まれる値の型サイズを取得 
		rsize_t type_size = 0;
		auto hr = getTiffTypeSize(get_tiff_value(image->byte_order, entry->type), &type_size);
		if (FAILED(hr)) return hr;

		// エントリに含まれる値の総バイト数を取得 
		uint64_t value_size = static_cast<uint64_t>(type_size) * get_tiff_value(image->byte_order, entry->count);

		uint64_t value_offset = 0;
		if (sizeof(entry->offset) < value_size)
		{
			// 値がエントリオフセットのサイズを超えていたら、エントリオフセットの値を取得 
			value_offset = get_tiff_value(image->byte_order, entry->offset);
		}
		else
		{
			// 値がエントリオフセットのサイズに収まっていれば、エントリオフセットの位置を取得 
			value_offset = item->getOffset() + offsetof(TYPE_IFD::entry_type, offset);
		}
		*result = value_offset;
		return S_OK;
	}

	// SubIFDのアイテムを取得 
	// TYPE_IFD : 読み込むIFDの型（TIFFIFD or TIFFIFD8） 
	// image : 画像データ管理オブジェクト
	// child : 生成したオブジェクトを格納する変数へのポインタ 
	// value_offset : IFDの位置を指す値の位置 
	// ifdtype : 取得するIFDのID 
	template <typename TYPE_IFD>
	HRESULT getEntryChildItem(const container_TIFF* image, agaliaElement** child, uint32_t sibling, uint64_t value_offset, int ifdtype)
	{
		// IFDのオフセット位置を読み込む 
		uint32_t next_data_offset = 0;
		auto hr = image->ReadData(&next_data_offset, value_offset + sizeof(next_data_offset) * sibling, sizeof(next_data_offset));
		if (FAILED(hr)) return hr;
		next_data_offset = get_tiff_value(image->byte_order, next_data_offset);

		// IFDの先頭にあるカウント数を読み込む 
		decltype(TYPE_IFD::count) count = 0;
		hr = image->ReadData(&count, next_data_offset, sizeof(count));
		if (FAILED(hr)) return hr;

		// IFDのサイズを計算 
		uint64_t next_data_size =
			static_cast<uint64_t>(sizeof(TYPE_IFD::count)) +
			static_cast<uint64_t>(sizeof(TYPE_IFD::entry_type)) * get_tiff_value(image->byte_order, count) +
			sizeof(TYPE_IFD::next_offset_type);

		// item_IFD を生成 
		*child = new item_IFD<TYPE_IFD>(image, next_data_offset, next_data_size, ifdtype);
		return S_OK;
	}
}
