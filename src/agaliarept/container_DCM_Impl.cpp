#include "pch.h"
#include "container_DCM_Impl.h"

#include "analyze_DCM_item_Base.h"
#include "analyze_DCM_item_preamble.h"
#include "analyze_DCM_util.h"


using namespace analyze_DCM;



// TransferSyntax の UID を enum 値に変換
TransferSyntax interpretTransferSyntax(const std::string& str)
{
	TransferSyntax ret = TransferSyntax_Unknown;

	if (str == "1.2.840.10008.1.2") {
		ret = ImplicitVR_LittleEndian;
	}
	else if (str == "1.2.840.10008.1.2.1") {
		ret = ExplicitVR_LittleEndian;
	}
	else if (str == "1.2.840.10008.1.2.1.99") {
		ret = DeflatedExplicitVR_LittleEndian;
	}
	else if (str == "1.2.840.10008.1.2.2") {
		ret = ExplicitVR_BigEndian;
	}
	return ret;
}



TransferSyntax search_transfer_syntax(const container_DCM_Impl* image)
{
	TransferSyntax ret = TransferSyntax_Unknown;

	agaliaPtr<agaliaItem> item;
	auto hr = image->getRootItem(&item);
	if (FAILED(hr)) return ret;

	agaliaPtr<agaliaItem> next;
	while (SUCCEEDED(item->getNextItem(&next)))
	{
		item = next.detach();
		dicom_element elem(image, item->getOffset());

		uint16_t group = 0;
		uint16_t element = 0;
		if (FAILED(elem.getGroup(&group))) return ret;
		if (FAILED(elem.getElement(&element))) return ret;

		// 通り過ぎていたらループ中断 
		if (0x0002 < group) return ret;
		if (0x0010 < element) return ret;

		if (group == 0x0002 && element == 0x0010)
		{
			uint32_t value_size = 0;
			uint64_t value_offset = 0;
			if (FAILED(elem.getValueLength(&value_size))) break;
			if (FAILED(elem.getValueOffset(&value_offset))) break;

			CHeapPtr<char> buf;
			if (!buf.AllocateBytes(size_t(value_size) + 1)) break;
			buf[value_size] = '\0';
			if (FAILED(image->ReadData(buf, item->getOffset() + value_offset, value_size))) break;

			// 最初に見つかった (0002,0010) タグから転送構文を取得 
			return interpretTransferSyntax(buf.m_pData);
		}
	}

	return ret;
}



// container_DCM_Impl class 

container_DCM_Impl::container_DCM_Impl(const wchar_t* path, IStream* stream)
	: container_DCM(path, stream)
{
	TransferSyntax syntax = search_transfer_syntax(this);
	if (syntax == TransferSyntax_Unknown) {
		_transferSyntax = ExplicitVR_LittleEndian;	// default 
	}
	else {
		_transferSyntax = syntax;
	}

	init_dcm_dictionary(agalia_pref_dic_lang == Japanese ? L"jpn" : L"enu");
}


container_DCM_Impl::~container_DCM_Impl()
{
	clear_dcm_dictionary();
}


HRESULT container_DCM_Impl::getColumnCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = column_max;
	return S_OK;
}


HRESULT container_DCM_Impl::getColumnWidth(uint32_t column, int32_t* length) const
{
	if (length == nullptr) return E_POINTER;

	switch (column)
	{
	case column_offset:	*length = 8;	break;
	case column_tag:	*length = 9;	break;
	case column_name:	*length = 20;	break;
	case column_vr:		*length = 5;	break;
	case column_length:	*length = -6;	break;
	case column_value:	*length = 0;	break;
	default:
		return E_INVALIDARG;
	}

	return S_OK;
}


HRESULT container_DCM_Impl::getColumnName(uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (column)
	{
	case column_offset:	name = L"offset";	break;
	case column_tag:	name = L"tag";		break;
	case column_name:	name = L"name";		break;
	case column_vr:		name = L"VR";		break;
	case column_length:	name = L"length";	break;
	case column_value:	name = L"value";	break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(name);
	return S_OK;
}


HRESULT container_DCM_Impl::getGridRowCount(const agaliaItem* item, uint32_t* row) const
{
	if (item == nullptr) return E_POINTER;
	if (row == nullptr) return E_POINTER;

	if (item->getGUID() != _agaliaItemDICOMBase::guid_dicom)
		return E_FAIL;

	*row = 1;
	return S_OK;
}



HRESULT container_DCM_Impl::getGridValue(const agaliaItem* item, uint32_t row, uint32_t column, agaliaString** str) const
{
	if (item == nullptr) return E_POINTER;
	if (str == nullptr) return E_POINTER;

	if (item->getGUID() != _agaliaItemDICOMBase::guid_dicom)
		return E_FAIL;

	if (row != 0) return E_INVALIDARG;

	return static_cast<const _agaliaItemDICOMBase*>(item)->getColumnValue(column, str);
}


HRESULT container_DCM_Impl::getRootItem(agaliaItem** root) const
{
	if (root == nullptr) return E_POINTER;
	auto item = new _agaliaItemDICOMPreamble(this, 0, dicom_preamble_size + sizeof(uint32_t));
	*root = item;
	return S_OK;
}


HRESULT container_DCM_Impl::getPropertyValue(PropertyType type, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	if (type == ContainerType)
	{
		return GetContainerName(str);
	}
	return E_FAIL;
}


HRESULT container_DCM_Impl::getThumbnailImage(HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH) const
{
	// todo
	phBitmap; maxW; maxH;
	return E_NOTIMPL;
}
