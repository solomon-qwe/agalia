#include "pch.h"
#include "analyze_JPEG_item_SOF.h"

#include "analyze_JPEG_util.h"

#include "jpeg_def.h"
#include "byteswap.h"

using namespace analyze_JPEG;




item_SOF::item_SOF(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type)
	:item_Marker(image, offset, size, type)
{
}



item_SOF::~item_SOF()
{
}



HRESULT item_SOF::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_value)
	{
		JPEGSEGMENT_SOF sof = {};
		auto hr = image->ReadData(&sof, getOffset(), sizeof(sof));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"Lf=" << agalia_byteswap(sof.Lf);
		temp << L", P=" << agalia_byteswap(sof.P);
		temp << L", Y=" << agalia_byteswap(sof.Y);
		temp << L", X=" << agalia_byteswap(sof.X);
		temp << L", Nf=" << agalia_byteswap(sof.Nf);
		*str = agaliaString::create(temp.str().c_str());

		return S_OK;
	}

	return __super::getColumnValue(column, str);
}



HRESULT item_SOF::getAdditionalInfoCount(uint32_t* row) const
{
	JPEGSEGMENT_SOF sof = {};
	auto hr = image->ReadData(&sof, getOffset(), sizeof(sof));
	if (FAILED(hr)) return hr;
	*row = sof.Nf;

	return S_OK;
}



HRESULT item_SOF::getAdditionalInfoValue(uint32_t row, agaliaString** str) const
{
	CHeapPtr<JPEGSEGMENT_SOF> sof;
	rsize_t bufsize = 0;
	auto hr = ReadSegment(sof, &bufsize, image, this);
	if (FAILED(hr)) return hr;

	if (sof->Nf <= row) return E_INVALIDARG;

	std::wstringstream temp;
	temp << L"C=" << agalia_byteswap(sof->comp[row].C);
	temp << L", H=" << static_cast<uint8_t>(sof->comp[row].H);
	temp << L", V=" << static_cast<uint8_t>(sof->comp[row].V);
	temp << L", Tq=" << agalia_byteswap(sof->comp[row].Tq);
	*str = agaliaString::create(temp.str().c_str());

	return S_OK;
}



HRESULT item_SOF::getPropCount(uint32_t* count) const
{
	auto hr = __super::getPropCount(count);
	if (FAILED(hr)) return hr;

	JPEGSEGMENT_SOF sof = {};
	hr = image->ReadData(&sof, getOffset(), sizeof(sof));
	if (FAILED(hr)) return hr;

	*count += 5;			// Lf, P, Y, X, Nf 
	*count += 4 * sof.Nf;	// C, V, H, Tq 

	return S_OK;
}



HRESULT item_SOF::getPropName(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getPropName(index, str);
	if (SUCCEEDED(hr)) return hr;

	agaliaStringPtr s;
	switch (index)
	{
	case 3: s = agaliaString::create(L"Lf"); break;
	case 4: s = agaliaString::create(L"P"); break;
	case 5: s = agaliaString::create(L"Y"); break;
	case 6: s = agaliaString::create(L"X"); break;
	case 7: s = agaliaString::create(L"Nf"); break;
	default:
		std::wstringstream temp;
		switch ((index - 8) % 4)
		{
		case 0: temp << L"C"; break;
		case 1: temp << L"H"; break;
		case 2: temp << L"V"; break;
		case 3: temp << L"Tq"; break;
		}
		temp << int_to_subscript_wchar((index - 8) / 4 + 1);
		s = agaliaString::create(temp.str().c_str());
	}

	if (!s) return E_FAIL;
	*str = s.detach();

	return S_OK;
}



HRESULT item_SOF::getPropValue(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	CHeapPtr<JPEGSEGMENT_SOF> sof;
	rsize_t bufsize = 0;
	hr = ReadSegment(sof, &bufsize, image, this);
	if (FAILED(hr)) return hr;

	std::wstringstream temp;
	switch (index)
	{
	case 3: temp << agalia_byteswap(sof->Lf);	break;
	case 4: temp << agalia_byteswap(sof->P);	break;
	case 5: temp << agalia_byteswap(sof->Y);	break;
	case 6: temp << agalia_byteswap(sof->X);	break;
	case 7: temp << agalia_byteswap(sof->Nf);	break;

	default:
		switch ((index - 8) % 4)
		{
		case 0: temp << agalia_byteswap(sof->comp[(index - 8) / 4].C);	break;
		case 1: temp << sof->comp[(index - 8) / 4].H;	break;
		case 2: temp << sof->comp[(index - 8) / 4].V;	break;
		case 3: temp << agalia_byteswap(sof->comp[(index - 8) / 4].Tq);	break;
		}
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
