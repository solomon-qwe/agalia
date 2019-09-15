#include "pch.h"
#include "analyze_JPEG_item_JFIF.h"

#include "analyze_JPEG_util.h"

#include "jpeg_def.h"

using namespace analyze_JPEG;





item_JFIF::item_JFIF(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type)
	:item_Base(image, offset, size, type)
{
}

item_JFIF::~item_JFIF()
{
}


HRESULT item_JFIF::getItemPropCount(uint32_t* count) const
{
	*count = 7;
	return S_OK;
}

HRESULT item_JFIF::getItemName(agaliaString** str) const
{
	*str = agaliaString::create(L"JFIF");
	return S_OK;
}



HRESULT item_JFIF::getItemPropName(uint32_t index, agaliaString** str) const
{
	const wchar_t* name = nullptr;
	switch (index)
	{
	case 0: name = L"major_version"; break;
	case 1: name = L"minor_version"; break;
	case 2: name = L"units"; break;
	case 3: name = L"Xdensity"; break;
	case 4: name = L"Ydensity"; break;
	case 5: name = L"Xthumbnail"; break;
	case 6: name = L"Ythumbnail"; break;
	default:
		return E_INVALIDARG;
	}
	*str = agaliaString::create(name);
	return S_OK;
}

HRESULT item_JFIF::getItemPropValue(uint32_t index, agaliaString** str) const
{
	JPEGSEGMENT_APP0_JFIF jfif = {};
	auto hr = image->ReadData(&jfif, getOffset(), sizeof(jfif));
	if (FAILED(hr)) return hr;

	std::wstringstream temp;
	switch (index)
	{
	case 0: temp << agalia_byteswap(jfif.major_version); break;
	case 1: temp << agalia_byteswap(jfif.minor_version); break;
	case 2: temp << agalia_byteswap(jfif.units); break;
	case 3: temp << agalia_byteswap(jfif.Xdensity); break;
	case 4: temp << agalia_byteswap(jfif.Ydensity); break;
	case 5: temp << agalia_byteswap(jfif.Xthumbnail); break;
	case 6: temp << agalia_byteswap(jfif.Ythumbnail); break;
	default:
		return E_INVALIDARG;
	}
	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}


HRESULT item_JFIF::getNextItem(agaliaItem**) const
{
	return E_FAIL;
}



HRESULT item_JFIF::getColumnValueForAPP0(uint32_t column, const container_JPEG* image, const item_Base* item, agaliaString** str)
{
	if (column == column_value)
	{
		JPEGSEGMENT_APP0_JFIF jfif = {};
		auto hr = image->ReadData(&jfif, item->getOffset(), sizeof(jfif));
		if (FAILED(hr)) return hr;

		wchar_t identifier[16] = {};
		swprintf_s(identifier, L"%S", jfif.identifier);

		std::wstringstream temp;
		temp << L"length=" << agalia_byteswap(jfif.length);
		temp << L", identifier={" << identifier << L"}";
		temp << L", major=" << agalia_byteswap(jfif.major_version);
		temp << L", minor=" << agalia_byteswap(jfif.minor_version);
		temp << L", units=" << agalia_byteswap(jfif.units);
		temp << L", X=" << agalia_byteswap(jfif.Xdensity);
		temp << L", Y=" << agalia_byteswap(jfif.Ydensity);
		temp << L", Xthumb=" << agalia_byteswap(jfif.Xthumbnail);
		temp << L", Ythumb=" << agalia_byteswap(jfif.Ythumbnail);
		*str = agaliaString::create(temp.str().c_str());

		return S_OK;
	}

	return E_FAIL;
}
