#include "pch.h"
#include "container_DCM_Impl.h"

#include "analyze_DCM_item_Base.h"
#include "analyze_DCM_item_preamble.h"
#include "analyze_DCM_util.h"
#include "analyze_DCM_item_elem.h"
#include "../inc/decode.h"
#include "agaliaDecoderDCM.h"
#include "thumbnail.h"


using namespace analyze_DCM;


// TransferSyntax の UID を enum 値に変換
static TransferSyntax interpretTransferSyntax(const std::string& str)
{
	if (str == "1.2.840.10008.1.2")
		return ImplicitVR_LittleEndian;
	else if (str == "1.2.840.10008.1.2.1")
		return ExplicitVR_LittleEndian;
	else if (str == "1.2.840.10008.1.2.1.99")
		return DeflatedExplicitVR_LittleEndian;
	else if (str == "1.2.840.10008.1.2.2")
		return ExplicitVR_BigEndian;
	else
		return TransferSyntax_Unknown;
}


static TransferSyntax search_transfer_syntax(const container_DCM_Impl* image)
{
	TransferSyntax ret = TransferSyntax_Unknown;

	CHeapPtr<char> buf;
	auto hr = getTagValue(image, 0x0002, 0x0010, buf, nullptr, true);
	if (FAILED(hr)) return ret;

	return interpretTransferSyntax(buf.m_pData);
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

	static bool initialized = false;
	if (!initialized)
	{
		addAgaliaDecoder(new agaliaDecoderDCM());
		initialized = true;
	}
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


HRESULT container_DCM_Impl::getElementInfoCount(const agaliaElement* item, uint32_t* row) const
{
	if (item == nullptr) return E_POINTER;
	if (row == nullptr) return E_POINTER;

	if (item->getGUID() != _agaliaItemDICOMBase::guid_dicom)
		return E_FAIL;

	*row = 1;
	return S_OK;
}



HRESULT container_DCM_Impl::getElementInfoValue(const agaliaElement* item, uint32_t row, uint32_t column, agaliaString** str) const
{
	if (item == nullptr) return E_POINTER;
	if (str == nullptr) return E_POINTER;

	if (item->getGUID() != _agaliaItemDICOMBase::guid_dicom)
		return E_FAIL;

	if (row != 0) return E_INVALIDARG;

	return static_cast<const _agaliaItemDICOMBase*>(item)->getColumnValue(column, str);
}


HRESULT container_DCM_Impl::getRootElement(agaliaElement** root) const
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
	else if (type == ImageWidth)
	{
		agaliaPtr<agaliaElement> item;
		auto hr = findTag(this, 0x0028, 0x0011, &item);
		if (FAILED(hr)) return E_FAIL;
		return item->getPropValue(_agaliaItemDICOMElement::prop_value, str);
	}
	else if (type == ImageHeight)
	{
		agaliaPtr<agaliaElement> item;
		auto hr = findTag(this, 0x0028, 0x0010, &item);
		if (FAILED(hr)) return E_FAIL;
		return item->getPropValue(_agaliaItemDICOMElement::prop_value, str);
	}

	return E_FAIL;
}

HRESULT container_DCM_Impl::loadBitmap(agaliaBitmap** ppBitmap) const
{
	return ::loadBitmap(ppBitmap, this);
}

HRESULT container_DCM_Impl::loadThumbnail(agaliaBitmap** ppBitmap, uint32_t maxW, uint32_t maxH) const
{
	return ::loadThumbnail(ppBitmap, this, maxW, maxH);
}
