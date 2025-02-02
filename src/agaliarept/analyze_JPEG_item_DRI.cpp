#include "pch.h"
#include "analyze_JPEG_item_DRI.h"

#include "analyze_JPEG_util.h"

#include "jpeg_def.h"
#include "byteswap.h"

using namespace analyze_JPEG;




item_DRI::item_DRI(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type)
	:item_Marker(image, offset, size, type)
{
}



item_DRI::~item_DRI()
{
}



HRESULT item_DRI::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_value)
	{
		JPEGSEGMENT_DRI dri = {};
		auto hr = image->ReadData(&dri, getOffset(), sizeof(dri));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"Lr=" << agalia_byteswap(dri.Lr);
		temp << L", Ri=" << agalia_byteswap(dri.Ri);
		*str = agaliaString::create(temp.str().c_str());

		return S_OK;
	}

	return __super::getColumnValue(column, str);
}



HRESULT item_DRI::getPropCount(uint32_t* count) const
{
	auto hr = __super::getPropCount(count);
	if (FAILED(hr)) return hr;

	*count += 2;
	return S_OK;
}



HRESULT item_DRI::getPropName(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getPropName(index, str);
	if (SUCCEEDED(hr)) return hr;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case 3: name = L"Lr"; break;
	case 4: name = L"Ri"; break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT item_DRI::getPropValue(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	JPEGSEGMENT_DRI dri = {};
	hr = image->ReadData(&dri, getOffset(), sizeof(dri));
	if (FAILED(hr)) return hr;

	std::wstringstream temp;
	switch (index)
	{
	case 3: temp << agalia_byteswap(dri.Lr); break;
	case 4: temp << agalia_byteswap(dri.Ri); break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
