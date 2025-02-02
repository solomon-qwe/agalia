#include "pch.h"
#include "analyze_DCM_item_preamble.h"

#include "analyze_DCM_item_elem.h"
#include "analyze_DCM_util.h"

using namespace analyze_DCM;

_agaliaItemDICOMPreamble::_agaliaItemDICOMPreamble(const container_DCM_Impl* image, uint64_t offset, uint64_t size)
	: _agaliaItemDICOMBase(image, offset, size)
{
}


_agaliaItemDICOMPreamble::~_agaliaItemDICOMPreamble()
{
}


HRESULT _agaliaItemDICOMPreamble::getName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	*str = agaliaString::create(L"Preamble");
	return S_OK;
}


HRESULT _agaliaItemDICOMPreamble::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_max;
	return S_OK;
}


HRESULT _agaliaItemDICOMPreamble::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_offset:	name = L"Offset";	break;
	case prop_preamble:	name = L"Preamble";	break;
	case prop_prefix:	name = L"Prefix";	break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(name);
	return S_OK;
}


HRESULT _agaliaItemDICOMPreamble::getPropValue(uint32_t index, agaliaString** str) const
{
	std::wstringstream temp;
	if (index == prop_offset) {
		format_dec(temp, getOffset());
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_preamble) {
		CHeapPtr<uint8_t> buf;
		if (!buf.AllocateBytes(dicom_preamble_size)) return E_OUTOFMEMORY;
		auto hr = image->ReadData(buf, getOffset(), dicom_preamble_size);
		if (FAILED(hr)) return hr;

		for (int i = 0; i < dicom_preamble_size; i++)
		{
			if (i != 0) temp << L",";
			format_hex(temp, buf[i]);
		}
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_prefix) {
		uint32_t prefix = 0;
		auto hr = image->ReadData(&prefix, getOffset() + dicom_preamble_size, sizeof(prefix));
		if (FAILED(hr)) return hr;

		format_asc(temp, prefix);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}

	return E_INVALIDARG;
}


HRESULT _agaliaItemDICOMPreamble::getChild(uint32_t sibling, agaliaElement** child) const
{
	UNREFERENCED_PARAMETER(sibling);
	UNREFERENCED_PARAMETER(child);
	return E_FAIL;
}


HRESULT _agaliaItemDICOMPreamble::getNext(agaliaElement** next) const
{
	uint64_t next_data_offset = getSize();
	uint64_t next_data_size = 0;

	dicom_element elem(image, next_data_offset);
	auto hr = elem.getElementSize(&next_data_size);
	if (FAILED(hr)) return hr;

	auto item = new _agaliaItemDICOMElement(image, next_data_offset, next_data_size);
	item->endPos = image->getImageSize();
	item->codepage.push_back(CP_US_ASCII);	// default character set 
	*next = item;

	return S_OK;
}
