#include "pch.h"
#include "analyze_RIFF_item_avih.h"



using namespace analyze_RIFF;



item_avih::item_avih(const container_RIFF* _image, uint64_t offset, uint64_t size)
	: RIFF_item_Base(_image, offset, size)
{
}



item_avih::~item_avih()
{
}



HRESULT item_avih::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}



HRESULT item_avih::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	auto hr = RIFF_item_Base::getPropName(index, str);
	if (SUCCEEDED(hr)) return hr;

	switch (index)
	{
	case prop_dwMicroSecPerFrame:	*str = agaliaString::create(L"dwMicroSecPerFrame"); break;
	case prop_dwMaxBytesPerSec:		*str = agaliaString::create(L"dwMaxBytesPerSec"); break;
	case prop_dwPaddingGranularity:	*str = agaliaString::create(L"dwPaddingGranularity"); break;
	case prop_dwFlags:				*str = agaliaString::create(L"dwFlags"); break;
	case prop_dwTotalFrames:		*str = agaliaString::create(L"dwTotalFrames"); break;
	case prop_dwInitialFrames:		*str = agaliaString::create(L"dwInitialFrames"); break;
	case prop_dwStreams:			*str = agaliaString::create(L"dwStreams"); break;
	case prop_dwSuggestedBufferSize:*str = agaliaString::create(L"dwSuggestedBufferSize"); break;
	case prop_dwWidth:				*str = agaliaString::create(L"dwWidth"); break;
	case prop_dwHeight:				*str = agaliaString::create(L"dwHeight"); break;
	case prop_dwReserved:			*str = agaliaString::create(L"dwReserved"); break;
	default:
		return E_INVALIDARG;
	}

	return S_OK;
}



HRESULT item_avih::getPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	auto hr = RIFF_item_Base::getPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	std::wstringstream temp;
	if (index == prop_dwMicroSecPerFrame)
	{
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwMicroSecPerFrame)>(temp, offsetof(AVIMAINHEADER, dwMicroSecPerFrame), this);
	}
	else if (index == prop_dwMaxBytesPerSec)
	{
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwMaxBytesPerSec)>(temp, offsetof(AVIMAINHEADER, dwMaxBytesPerSec), this);
	}
	else if (index == prop_dwPaddingGranularity)
	{
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwPaddingGranularity)>(temp, offsetof(AVIMAINHEADER, dwPaddingGranularity), this);
	}
	else if (index == prop_dwFlags)
	{
		hr = format_chunk_hex<decltype(AVIMAINHEADER::dwFlags)>(temp, offsetof(AVIMAINHEADER, dwFlags), this);
	}
	else if (index == prop_dwTotalFrames)
	{
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwTotalFrames)>(temp, offsetof(AVIMAINHEADER, dwTotalFrames), this);
	}
	else if (index == prop_dwInitialFrames)
	{
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwInitialFrames)>(temp, offsetof(AVIMAINHEADER, dwInitialFrames), this);
	}
	else if (index == prop_dwStreams)
	{
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwStreams)>(temp, offsetof(AVIMAINHEADER, dwStreams), this);
	}
	else if (index == prop_dwSuggestedBufferSize)
	{
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwSuggestedBufferSize)>(temp, offsetof(AVIMAINHEADER, dwSuggestedBufferSize), this);
	}
	else if (index == prop_dwWidth)
	{
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwWidth)>(temp, offsetof(AVIMAINHEADER, dwWidth), this);
	}
	else if (index == prop_dwHeight)
	{
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwHeight)>(temp, offsetof(AVIMAINHEADER, dwHeight), this);
	}
	else if (index == prop_dwReserved)
	{
		AVIMAINHEADER avimain = {};
		hr = image->ReadData(&avimain.dwReserved, getOffset() + offsetof(AVIMAINHEADER, dwReserved), sizeof(avimain.dwReserved));
		if (SUCCEEDED(hr))
		{
			for (rsize_t i = 0; i < _countof(avimain.dwReserved); i++) {
				if (i != 0) temp << L", ";
				format_dec(temp, avimain.dwReserved[i]);
			}
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}

	if (FAILED(hr)) return hr;

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}



HRESULT item_avih::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_value)
	{
		RIFFCHUNK chunk = {};
		auto hr = image->ReadData(&chunk, getOffset(), sizeof(chunk));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"fcc:";
		format_fcc(temp, chunk.fcc);
		temp << L", cb:";
		format_dec(temp, chunk.cb);

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}

	return __super::getColumnValue(column, str);
}



HRESULT item_avih::getAdditionalInfoCount(uint32_t* row) const
{
	*row = addinf_last;
	return S_OK;
}



HRESULT item_avih::getAdditionalInfoValue(uint32_t row, agaliaString** str) const
{
	HRESULT hr;

	std::wstringstream temp;
	if (row == addinf_dwMicroSecPerFrame)
	{
		temp << L"dwMicroSecPerFrame:";
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwMicroSecPerFrame)>(temp, offsetof(AVIMAINHEADER, dwMicroSecPerFrame), this);
	}
	else if (row == addinf_dwMaxBytesPerSec)
	{
		temp << L"dwMaxBytesPerSec:";
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwMaxBytesPerSec)>(temp, offsetof(AVIMAINHEADER, dwMaxBytesPerSec), this);
	}
	else if (row == addinf_dwPaddingGranularity)
	{
		temp << L"dwPaddingGranularity:";
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwPaddingGranularity)>(temp, offsetof(AVIMAINHEADER, dwPaddingGranularity), this);
	}
	else if (row == addinf_dwFlags)
	{
		temp << L"dwFlags:";
		hr = format_chunk_hex<decltype(AVIMAINHEADER::dwFlags)>(temp, offsetof(AVIMAINHEADER, dwFlags), this);
	}
	else if (row == addinf_dwTotalFrames)
	{
		temp << L"dwTotalFrames:";
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwTotalFrames)>(temp, offsetof(AVIMAINHEADER, dwTotalFrames), this);
	}
	else if (row == addinf_dwInitialFrames)
	{
		temp << L"dwInitialFrames:";
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwInitialFrames)>(temp, offsetof(AVIMAINHEADER, dwInitialFrames), this);
	}
	else if (row == addinf_dwStreams)
	{
		temp << L"dwStreams:";
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwStreams)>(temp, offsetof(AVIMAINHEADER, dwStreams), this);
	}
	else if (row == addinf_dwSuggestedBufferSize)
	{
		temp << L"dwSuggestedBufferSize:";
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwSuggestedBufferSize)>(temp, offsetof(AVIMAINHEADER, dwSuggestedBufferSize), this);
	}
	else if (row == addinf_dwWidth)
	{
		temp << L"dwWidth:";
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwWidth)>(temp, offsetof(AVIMAINHEADER, dwWidth), this);
	}
	else if (row == addinf_dwHeight)
	{
		temp << L"dwHeight:";
		hr = format_chunk_dec<decltype(AVIMAINHEADER::dwHeight)>(temp, offsetof(AVIMAINHEADER, dwHeight), this);
	}
	else if (row == addinf_dwReserved)
	{
		AVIMAINHEADER avimain = {};
		hr = image->ReadData(&avimain.dwReserved, getOffset() + offsetof(AVIMAINHEADER, dwReserved), sizeof(avimain.dwReserved));
		if (SUCCEEDED(hr))
		{
			for (rsize_t i = 0; i < _countof(avimain.dwReserved); i++) {
				if (i != 0) temp << L", ";
				temp << L"dwReserved[" << i << L"]:";
				format_dec(temp, avimain.dwReserved[i]);
			}
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}

	if (FAILED(hr)) return hr;

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
