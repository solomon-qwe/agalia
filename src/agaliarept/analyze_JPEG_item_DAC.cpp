#include "pch.h"
#include "analyze_JPEG_item_DAC.h"

#include "analyze_JPEG_util.h"

#include "jpeg_def.h"
#include "byteswap.h"

using namespace analyze_JPEG;




item_DAC::item_DAC(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type)
	:item_Marker(image, offset, size, type)
{
}



item_DAC::~item_DAC()
{
}



HRESULT item_DAC::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_value)
	{
		JPEGSEGMENT_DAC dac = {};
		auto hr = image->ReadData(&dac, getOffset(), sizeof(dac));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"La=" << agalia_byteswap(dac.La);
		*str = agaliaString::create(temp.str().c_str());

		return S_OK;
	}

	return __super::getColumnValue(column, str);
}



HRESULT item_DAC::getAdditionalInfoCount(uint32_t* row) const
{
	JPEGSEGMENT_DAC dac = {};
	auto hr = image->ReadData(&dac, getOffset(), sizeof(dac));
	if (FAILED(hr)) return hr;

	*row = (agalia_byteswap(dac.La) - sizeof(JPEGSEGMENT_DAC::La)) / sizeof(JPEGSEGMENT_DAC::comp);
	return S_OK;
}



HRESULT item_DAC::getAdditionalInfoValue(uint32_t row, agaliaString** str) const
{
	CHeapPtr<JPEGSEGMENT_DAC> dac;
	rsize_t bufsize = 0;
	auto hr = ReadSegment(dac, &bufsize, image, this);
	if (FAILED(hr)) return hr;

	std::wstringstream temp;
	temp << L"Tc=" << dac->comp[row].Tc;
	temp << L", Tb=" << dac->comp[row].Tb;
	temp << L", Cs=" << agalia_byteswap(dac->comp[row].Cs);
	*str = agaliaString::create(temp.str().c_str());

	return S_OK;
}



HRESULT item_DAC::getPropCount(uint32_t* count) const
{
	auto hr = __super::getPropCount(count);
	if (FAILED(hr)) return hr;

	JPEGSEGMENT_DAC dac = {};
	hr = image->ReadData(&dac, getOffset(), sizeof(dac));
	if (FAILED(hr)) return hr;

	int n = (agalia_byteswap(dac.La) - 2) / 2;

	*count += 1 + 3 * n;

	return S_OK;
}



HRESULT item_DAC::getPropName(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getPropName(index, str);
	if (SUCCEEDED(hr)) return hr;

	const wchar_t* name = nullptr;
	if (index == 3) {
		name = L"La";
	}
	else {
		switch ((index - 4) % 3)
		{
		case 0: name = L"Tc"; break;
		case 1: name = L"Tb"; break;
		case 2: name = L"Cs"; break;
		}
	}

	if (name == nullptr) return E_INVALIDARG;
	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT item_DAC::getPropValue(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	CHeapPtr<JPEGSEGMENT_DAC> dac;
	rsize_t bufsize = 0;
	hr = ReadSegment(dac, &bufsize, image, this);
	if (FAILED(hr)) return hr;

	std::wstringstream temp;
	if (index == 3) {
		temp << agalia_byteswap(dac->La);
	}
	else {
		switch ((index - 4) % 3)
		{
		case 0: temp << dac->comp[(index - 4) / 3].Tc;	break;
		case 1: temp << dac->comp[(index - 4) / 3].Tb;	break;
		case 2: temp << dac->comp[(index - 4) / 3].Cs;	break;
		}
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
