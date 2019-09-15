#include "pch.h"
#include "analyze_JPEG_item_COM.h"

#include "analyze_JPEG_util.h"

#include "jpeg_def.h"
#include "byteswap.h"

using namespace analyze_JPEG;




item_COM::item_COM(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type)
	:item_Marker(image, offset, size, type)
{

}



item_COM::~item_COM()
{

}



HRESULT item_COM::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_value)
	{
		CHeapPtr<uint8_t> buf;
		rsize_t bufsize = 0;
		auto hr = ReadSegment(buf, &bufsize, image, this);
		if (FAILED(hr)) return hr;
		JPEGSEGMENT_COM* com = reinterpret_cast<JPEGSEGMENT_COM*>(buf.m_pData);

		std::wstringstream temp;
		temp << L"Lc=" << agalia_byteswap(com->Lc);
		temp << L", Cm=\"";

		size_t max_count = min(agalia_byteswap(com->Lc) - sizeof(com->Lc), bufsize - offsetof(JPEGSEGMENT_COM, Cm));
		int leng = 0;
		hr = SizeTToInt(max_count, &leng);
		if (FAILED(hr)) return hr;
		multibyte_to_widechar(reinterpret_cast<char*>(com->Cm), leng, CP_US_ASCII, temp);
		temp << L"\"";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}

	return __super::getColumnValue(column, str);
}



HRESULT item_COM::getItemPropCount(uint32_t* count) const
{
	auto hr = __super::getItemPropCount(count);
	if (FAILED(hr)) return hr;

	*count += 2;
	return S_OK;
}



HRESULT item_COM::getItemPropName(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getItemPropName(index, str);
	if (SUCCEEDED(hr)) return hr;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case 3: name = L"Lc"; break;
	case 4: name = L"Cm"; break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT item_COM::getItemPropValue(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getItemPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	rsize_t bufsize = 0;
	CHeapPtr<uint8_t> buf;
	hr = ReadSegment(buf, &bufsize, image, this);
	if (FAILED(hr)) return hr;
	JPEGSEGMENT_COM* com = reinterpret_cast<JPEGSEGMENT_COM*>(buf.m_pData);

	std::wstringstream temp;
	if (index == 3)
	{
		temp << agalia_byteswap(com->Lc);
	}
	else if (index == 4)
	{
		size_t max_count = min(agalia_byteswap(com->Lc) - sizeof(com->Lc), bufsize - offsetof(JPEGSEGMENT_COM, Cm));
		int leng = 0;
		hr = SizeTToInt(max_count, &leng);
		if (FAILED(hr)) return hr;
		multibyte_to_widechar(reinterpret_cast<char*>(com->Cm), leng, CP_US_ASCII, temp);
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
