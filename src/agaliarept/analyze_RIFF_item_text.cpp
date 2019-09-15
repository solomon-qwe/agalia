#include "pch.h"
#include "analyze_RIFF_item_text.h"



using namespace analyze_RIFF;



item_text::item_text(const container_RIFF* _image, uint64_t offset, uint64_t size)
	: item_Base(_image, offset, size)
{
}



item_text::~item_text()
{
}



HRESULT item_text::getItemPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}



HRESULT item_text::getItemPropName(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getItemPropName(index, str);
	if (SUCCEEDED(hr)) return hr;

	switch (index)
	{
	case prop_text: *str = agaliaString::create(L"text"); break;
	default:
		return E_INVALIDARG;
	}
	return S_OK;
}



HRESULT item_text::getItemPropValue(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getItemPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	if (index == prop_text)
	{
		rsize_t bufsize = 0;
		hr = UInt64ToSizeT(getSize() - sizeof(RIFFCHUNK), &bufsize);
		if (FAILED(hr)) return hr;

		CHeapPtr<char> buf;
		if (!buf.AllocateBytes(bufsize)) return E_OUTOFMEMORY;

		hr = image->ReadData(buf, getOffset() + sizeof(RIFFCHUNK), bufsize);
		if (FAILED(hr)) return hr;

		int leng = 0;
		hr = SizeTToInt(bufsize, &leng);
		if (FAILED(hr)) return hr;

		return multibyte_to_widechar(buf, leng, CP_US_ASCII, str);
	}

	return E_FAIL;
}



HRESULT item_text::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_value)
	{
		rsize_t bufsize = 0;
		auto hr = UInt64ToSizeT(getSize() - sizeof(RIFFCHUNK), &bufsize);
		if (FAILED(hr)) return hr;

		CHeapPtr<char> buf;
		if (!buf.AllocateBytes(bufsize)) return E_OUTOFMEMORY;

		hr = image->ReadData(buf, getOffset() + sizeof(RIFFCHUNK), bufsize);
		if (FAILED(hr)) return hr;

		int leng = 0;
		hr = SizeTToInt(bufsize, &leng);
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"\"";
		hr = multibyte_to_widechar(buf, leng, CP_US_ASCII, temp);
		if (FAILED(hr)) return hr;
		temp << L"\"";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}

	return __super::getColumnValue(column, str);
}
