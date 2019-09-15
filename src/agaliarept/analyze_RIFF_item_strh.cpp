#include "pch.h"
#include "analyze_RIFF_item_strh.h"

using namespace analyze_RIFF;

item_strh::item_strh(const container_RIFF* _image, uint64_t offset, uint64_t size)
	: item_Base(_image, offset, size)
{
}



item_strh::~item_strh()
{
}



HRESULT item_strh::getItemPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}



HRESULT item_strh::getItemPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	auto hr = item_Base::getItemPropName(index, str);
	if (SUCCEEDED(hr)) return hr;

	switch (index)
	{
	case prop_fccType:			*str = agaliaString::create(L"fccType"); break;
	case prop_fccHandler:		*str = agaliaString::create(L"fccHandler"); break;
	case prop_dwFlags:			*str = agaliaString::create(L"dwFlags"); break;
	case prop_wPriority:		*str = agaliaString::create(L"wPriority"); break;
	case prop_wLanguage:		*str = agaliaString::create(L"wLanguage"); break;
	case prop_dwInitialFrames:	*str = agaliaString::create(L"dwInitialFrames"); break;
	case prop_dwScale:			*str = agaliaString::create(L"dwScale"); break;
	case prop_dwRate:			*str = agaliaString::create(L"dwRate"); break;
	case prop_dwStart:			*str = agaliaString::create(L"dwStart"); break;
	case prop_dwLength:			*str = agaliaString::create(L"dwLength"); break;
	case prop_dwSuggestedBufferSize: *str = agaliaString::create(L"dwSuggestedBufferSize"); break;
	case prop_dwQuality:		*str = agaliaString::create(L"dwQuality"); break;
	case prop_dwSampleSize:		*str = agaliaString::create(L"dwSampleSize"); break;
	case prop_rcFrame_left:		*str = agaliaString::create(L"rcFrame.left"); break;
	case prop_rcFrame_top:		*str = agaliaString::create(L"rcFrame.top"); break;
	case prop_rcFrame_right:	*str = agaliaString::create(L"rcFrame.right"); break;
	case prop_rcFrame_bottom:	*str = agaliaString::create(L"rcFrame.bottom"); break;
	default:
		return E_INVALIDARG;
	}
	return S_OK;
}



HRESULT item_strh::getItemPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	auto hr = item_Base::getItemPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	std::wstringstream temp;

	if (index == prop_fccType)
	{
		hr = format_chunk_fcc<decltype(AVISTREAMHEADER::fccType)>(temp, offsetof(AVISTREAMHEADER, fccType), this);
	}
	else if (index == prop_fccHandler)
	{
		hr = format_chunk_fcc<decltype(AVISTREAMHEADER::fccHandler)>(temp, offsetof(AVISTREAMHEADER, fccHandler), this);
	}
	else if (index == prop_dwFlags)
	{
		hr = format_chunk_hex<decltype(AVISTREAMHEADER::dwFlags)>(temp, offsetof(AVISTREAMHEADER, dwFlags), this);
	}
	else if (index == prop_wPriority)
	{
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::wPriority)>(temp, offsetof(AVISTREAMHEADER, wPriority), this);
	}
	else if (index == prop_wLanguage)
	{
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::wLanguage)>(temp, offsetof(AVISTREAMHEADER, wLanguage), this);
	}
	else if (index == prop_dwInitialFrames)
	{
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::dwInitialFrames)>(temp, offsetof(AVISTREAMHEADER, dwInitialFrames), this);
	}
	else if (index == prop_dwScale)
	{
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::dwScale)>(temp, offsetof(AVISTREAMHEADER, dwScale), this);
	}
	else if (index == prop_dwRate)
	{
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::dwRate)>(temp, offsetof(AVISTREAMHEADER, dwRate), this);
	}
	else if (index == prop_dwStart)
	{
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::dwStart)>(temp, offsetof(AVISTREAMHEADER, dwStart), this);
	}
	else if (index == prop_dwLength)
	{
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::dwLength)>(temp, offsetof(AVISTREAMHEADER, dwLength), this);
	}
	else if (index == prop_dwSuggestedBufferSize)
	{
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::dwSuggestedBufferSize)>(temp, offsetof(AVISTREAMHEADER, dwSuggestedBufferSize), this);
	}
	else if (index == prop_dwQuality)
	{
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::dwQuality)>(temp, offsetof(AVISTREAMHEADER, dwQuality), this);
	}
	else if (index == prop_dwSampleSize)
	{
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::dwSampleSize)>(temp, offsetof(AVISTREAMHEADER, dwSampleSize), this);
	}
	else if (index == prop_rcFrame_left)
	{
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::rcFrame.left)>(temp, offsetof(AVISTREAMHEADER, rcFrame.left), this);
	}
	else if (index == prop_rcFrame_top)
	{
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::rcFrame.top)>(temp, offsetof(AVISTREAMHEADER, rcFrame.top), this);
	}
	else if (index == prop_rcFrame_right)
	{
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::rcFrame.right)>(temp, offsetof(AVISTREAMHEADER, rcFrame.right), this);
	}
	else if (index == prop_rcFrame_bottom)
	{
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::rcFrame.bottom)>(temp, offsetof(AVISTREAMHEADER, rcFrame.bottom), this);
	}
	else 
	{
		hr = E_INVALIDARG;
	}

	if (FAILED(hr)) return hr;

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}



HRESULT item_strh::getColumnValue(uint32_t column, agaliaString** str) const
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



HRESULT item_strh::getAdditionalInfoCount(uint32_t* row) const
{
	*row = addinf_last;
	return S_OK;
}



HRESULT item_strh::getAdditionalInfoValue(uint32_t row, agaliaString** str) const
{
	HRESULT hr;
	std::wstringstream temp;

	if (row == addinf_fccType)
	{
		temp << L"fccType:";
		hr = format_chunk_fcc<decltype(AVISTREAMHEADER::fccType)>(temp, offsetof(AVISTREAMHEADER, fccType), this);
	}
	else if (row == addinf_fccHandler)
	{
		temp << L"fccHandler:";
		hr = format_chunk_fcc<decltype(AVISTREAMHEADER::fccHandler)>(temp, offsetof(AVISTREAMHEADER, fccHandler), this);
	}
	else if (row == addinf_dwFlags)
	{
		temp << L"dwFlags:";
		hr = format_chunk_hex<decltype(AVISTREAMHEADER::dwFlags)>(temp, offsetof(AVISTREAMHEADER, dwFlags), this);
	}
	else if (row == addinf_wPriority)
	{
		temp << L"wPriority:";
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::wPriority)>(temp, offsetof(AVISTREAMHEADER, wPriority), this);
	}
	else if (row == addinf_wLanguage)
	{
		temp << L"wLanguage:";
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::wLanguage)>(temp, offsetof(AVISTREAMHEADER, wLanguage), this);
	}
	else if (row == addinf_dwInitialFrames)
	{
		temp << L"dwInitialFrames:";
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::dwInitialFrames)>(temp, offsetof(AVISTREAMHEADER, dwInitialFrames), this);
	}
	else if (row == addinf_dwScale)
	{
		temp << L"dwScale:";
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::dwScale)>(temp, offsetof(AVISTREAMHEADER, dwScale), this);
	}
	else if (row == addinf_dwRate)
	{
		temp << L"dwRate:";
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::dwRate)>(temp, offsetof(AVISTREAMHEADER, dwRate), this);
	}
	else if (row == addinf_dwStart)
	{
		temp << L"dwStart:";
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::dwStart)>(temp, offsetof(AVISTREAMHEADER, dwStart), this);
	}
	else if (row == addinf_dwLength)
	{
		temp << L"dwLength:";
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::dwLength)>(temp, offsetof(AVISTREAMHEADER, dwLength), this);
	}
	else if (row == addinf_dwSuggestedBufferSize)
	{
		temp << L"dwSuggestedBufferSize:";
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::dwSuggestedBufferSize)>(temp, offsetof(AVISTREAMHEADER, dwSuggestedBufferSize), this);
	}
	else if (row == addinf_dwQuality)
	{
		temp << L"dwQuality:";
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::dwQuality)>(temp, offsetof(AVISTREAMHEADER, dwQuality), this);
	}
	else if (row == addinf_dwSampleSize)
	{
		temp << L"dwSampleSize:";
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::dwSampleSize)>(temp, offsetof(AVISTREAMHEADER, dwSampleSize), this);
	}
	else if (row == addinf_rcFrame)
	{
		temp << L"rcFrame.left:";
		hr = format_chunk_dec<decltype(AVISTREAMHEADER::rcFrame.left)>(temp, offsetof(AVISTREAMHEADER, rcFrame.left), this);
		if (SUCCEEDED(hr))
		{
			temp << L", rcFrame.top:";
			hr = format_chunk_dec<decltype(AVISTREAMHEADER::rcFrame.top)>(temp, offsetof(AVISTREAMHEADER, rcFrame.top), this);
			if (SUCCEEDED(hr))
			{
				temp << L", rcFrame.right:";
				hr = format_chunk_dec<decltype(AVISTREAMHEADER::rcFrame.right)>(temp, offsetof(AVISTREAMHEADER, rcFrame.right), this);
				if (SUCCEEDED(hr))
				{
					temp << L", rcFrame.bottom:";
					hr = format_chunk_dec<decltype(AVISTREAMHEADER::rcFrame.bottom)>(temp, offsetof(AVISTREAMHEADER, rcFrame.bottom), this);
				}
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
