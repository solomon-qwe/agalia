#include "pch.h"
#include "analyze_RIFF_item_strf_auds.h"

#include <mmreg.h>



using namespace analyze_RIFF;



item_strf_auds::item_strf_auds(const container_RIFF* _image, uint64_t offset, uint64_t size)
	: RIFF_item_Base(_image, offset, size)
{
}



item_strf_auds::~item_strf_auds()
{
}



HRESULT item_strf_auds::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;

	if (getSize() < sizeof(RIFFCHUNK))
	{
		*count = 0;
	}
	else if (getSize() < sizeof(RIFFCHUNK) + sizeof(WAVEFORMAT))
	{
		*count = prop_cb + 1;
	}
	else if (getSize() < sizeof(RIFFCHUNK) + sizeof(PCMWAVEFORMAT))
	{
		*count = prop_nBlockAlign + 1;
	}
	else if (getSize() < sizeof(RIFFCHUNK) + sizeof(WAVEFORMATEX))
	{
		*count = prop_wBitsPerSample + 1;
	}
	else
	{
		*count = prop_last;
	}

	return S_OK;
}



HRESULT item_strf_auds::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	auto hr = RIFF_item_Base::getPropName(index, str);
	if (SUCCEEDED(hr)) return hr;

	switch (index)
	{
	case prop_wFormatTag:		*str = agaliaString::create(L"wFormatTag"); break;
	case prop_nChannels:		*str = agaliaString::create(L"nChannels"); break;
	case prop_nSamplesPerSec:	*str = agaliaString::create(L"nSamplesPerSec"); break;
	case prop_nAvgBytesPerSec:	*str = agaliaString::create(L"nAvgBytesPerSec"); break;
	case prop_nBlockAlign:		*str = agaliaString::create(L"nBlockAlign"); break;
	case prop_wBitsPerSample:	*str = agaliaString::create(L"wBitsPerSample"); break;
	case prop_cbSize:			*str = agaliaString::create(L"cbSize"); break;
	default:
		return E_INVALIDARG;
	}

	return S_OK;
}



HRESULT item_strf_auds::getPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	auto hr = RIFF_item_Base::getPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	std::wstringstream temp;

	if (index == prop_wFormatTag)
	{
		hr = format_chunksub_dec<decltype(WAVEFORMATEX::wFormatTag)>(temp, offsetof(WAVEFORMATEX, wFormatTag), this);
	}
	else if (index == prop_nChannels)
	{
		hr = format_chunksub_dec<decltype(WAVEFORMATEX::nChannels)>(temp, offsetof(WAVEFORMATEX, nChannels), this);
	}
	else if (index == prop_nSamplesPerSec)
	{
		hr = format_chunksub_dec<decltype(WAVEFORMATEX::nSamplesPerSec)>(temp, offsetof(WAVEFORMATEX, nSamplesPerSec), this);
	}
	else if (index == prop_nAvgBytesPerSec)
	{
		hr = format_chunksub_dec<decltype(WAVEFORMATEX::nAvgBytesPerSec)>(temp, offsetof(WAVEFORMATEX, nAvgBytesPerSec), this);
	}
	else if (index == prop_nBlockAlign)
	{
		hr = format_chunksub_dec<decltype(WAVEFORMATEX::nBlockAlign)>(temp, offsetof(WAVEFORMATEX, nBlockAlign), this);
	}
	else if (index == prop_wBitsPerSample)
	{
		hr = format_chunksub_dec<decltype(WAVEFORMATEX::wBitsPerSample)>(temp, offsetof(WAVEFORMATEX, wBitsPerSample), this);
	}
	else if (index == prop_cbSize)
	{
		hr = format_chunksub_dec<decltype(WAVEFORMATEX::cbSize)>(temp, offsetof(WAVEFORMATEX, cbSize), this);
	}
	else
	{
		hr = E_INVALIDARG;
	}

	if (FAILED(hr)) return hr;

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}



HRESULT item_strf_auds::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_value)
	{
		std::wstringstream temp;

		temp << L"wFormatTag:";
		auto hr = format_chunksub_dec<decltype(WAVEFORMATEX::wFormatTag)>(temp, offsetof(WAVEFORMATEX, wFormatTag), this);
		if (FAILED(hr)) return hr;

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}

	return __super::getColumnValue(column, str);
}



HRESULT item_strf_auds::getAdditionalInfoCount(uint32_t* row) const
{
	if (getSize() < sizeof(RIFFCHUNK) + sizeof(WAVEFORMAT))
	{
		*row = 0;
	}
	else if (getSize() < sizeof(RIFFCHUNK) + sizeof(PCMWAVEFORMAT))
	{
		*row = addinf_nBlockAlign + 1;
	}
	else if (getSize() < sizeof(RIFFCHUNK) + sizeof(WAVEFORMATEX))
	{
		*row = addinf_wBitsPerSample + 1;
	}
	else
	{
		*row = addinf_last;
	}

	return S_OK;
}



HRESULT item_strf_auds::getAdditionalInfoValue(uint32_t row, agaliaString** str) const
{
	HRESULT hr;
	std::wstringstream temp;

	if (row == addinf_nChannels)
	{
		temp << L"nChannels:";
		hr = format_chunksub_dec<decltype(WAVEFORMATEX::nChannels)>(temp, offsetof(WAVEFORMATEX, nChannels), this);
	}
	else if (row == addinf_nSamplesPerSec)
	{
		temp << L"nSamplesPerSec:";
		hr = format_chunksub_dec<decltype(WAVEFORMATEX::nSamplesPerSec)>(temp, offsetof(WAVEFORMATEX, nSamplesPerSec), this);
	}
	else if (row == addinf_nAvgBytesPerSec)
	{
		temp << L"nAvgBytesPerSec:";
		hr = format_chunksub_dec<decltype(WAVEFORMATEX::nAvgBytesPerSec)>(temp, offsetof(WAVEFORMATEX, nAvgBytesPerSec), this);
	}
	else if (row == addinf_nBlockAlign)
	{
		temp << L"nBlockAlign:";
		hr = format_chunksub_dec<decltype(WAVEFORMATEX::nBlockAlign)>(temp, offsetof(WAVEFORMATEX, nBlockAlign), this);
	}
	else if (row == addinf_wBitsPerSample)
	{
		temp << L"wBitsPerSample:";
		hr = format_chunksub_dec<decltype(WAVEFORMATEX::wBitsPerSample)>(temp, offsetof(WAVEFORMATEX, wBitsPerSample), this);
	}
	else if (row == addinf_cbSize)
	{
		temp << L"cbSize:";
		hr = format_chunksub_dec<decltype(WAVEFORMATEX::cbSize)>(temp, offsetof(WAVEFORMATEX, cbSize), this);
	}
	else
	{
		hr = E_INVALIDARG;
	}

	if (FAILED(hr)) return hr;

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
