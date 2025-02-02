#include "pch.h"
#include "analyze_RIFF_item_strf_vids.h"

#include <mmreg.h>



using namespace analyze_RIFF;



item_strf_vids::item_strf_vids(const container_RIFF* _image, uint64_t offset, uint64_t size)
	: RIFF_item_Base(_image, offset, size)
{
}



item_strf_vids::~item_strf_vids()
{
}



HRESULT item_strf_vids::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;

	if (getSize() < sizeof(RIFFCHUNK))
	{
		*count = 0;
	}
	else if (getSize() < sizeof(RIFFCHUNK) + sizeof(BITMAPINFOHEADER))
	{
		*count = prop_cb + 1;
	}
	else if (getSize() < sizeof(RIFFCHUNK) + sizeof(EXBMINFOHEADER))
	{
		*count = prop_biClrImportant + 1;
	}
	else if (getSize() < sizeof(RIFFCHUNK) + sizeof(EXBMINFOHEADER) + sizeof(JPEGINFOHEADER))
	{
		*count = prop_biExtDataOffset + 1;
	}
	else
	{
		BITMAPINFOHEADER bmpInfo = {};
		auto hr = image->ReadData(
			&bmpInfo.biCompression,
			getOffset() + sizeof(RIFFCHUNK) + offsetof(BITMAPINFOHEADER, biCompression),
			sizeof(bmpInfo.biCompression));

		if (SUCCEEDED(hr) && bmpInfo.biCompression == JPEG_DIB || bmpInfo.biCompression == MJPG_DIB)
		{
			*count = prop_last;
		}
		else
		{
			*count = prop_biExtDataOffset + 1;
		}
	}

	return S_OK;
}



HRESULT item_strf_vids::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	auto hr = RIFF_item_Base::getPropName(index, str);
	if (SUCCEEDED(hr)) return hr;

	switch (index)
	{
	case prop_biSize:			*str = agaliaString::create(L"BITMAPINFOHEADER::biSize"); break;
	case prop_biWidth:			*str = agaliaString::create(L"BITMAPINFOHEADER::biWidth"); break;
	case prop_biHeight:			*str = agaliaString::create(L"BITMAPINFOHEADER::biHeight"); break;
	case prop_biPlanes:			*str = agaliaString::create(L"BITMAPINFOHEADER::biPlanes"); break;
	case prop_biBitCount:		*str = agaliaString::create(L"BITMAPINFOHEADER::biBitCount"); break;
	case prop_biCompression:	*str = agaliaString::create(L"BITMAPINFOHEADER::biCompression"); break;
	case prop_biSizeImage:		*str = agaliaString::create(L"BITMAPINFOHEADER::biSizeImage"); break;
	case prop_biXPelsPerMeter:	*str = agaliaString::create(L"BITMAPINFOHEADER::biXPelsPerMeter"); break;
	case prop_biYPelsPerMeter:	*str = agaliaString::create(L"BITMAPINFOHEADER::biYPelsPerMeter"); break;
	case prop_biClrUsed:		*str = agaliaString::create(L"BITMAPINFOHEADER::biClrUsed"); break;
	case prop_biClrImportant:	*str = agaliaString::create(L"BITMAPINFOHEADER::biClrImportant"); break;
	case prop_biExtDataOffset:	*str = agaliaString::create(L"EXBMINFOHEADER::biExtDataOffset"); break;
	case prop_JPEGSize:			*str = agaliaString::create(L"JPEGINFOHEADER::JPEGSize"); break;
	case prop_JPEGProcess:		*str = agaliaString::create(L"JPEGINFOHEADER::JPEGProcess"); break;
	case prop_JPEGColorSpaceID:	*str = agaliaString::create(L"JPEGINFOHEADER::JPEGColorSpaceID"); break;
	case prop_JPEGBitsPerSample: *str = agaliaString::create(L"JPEGINFOHEADER::JPEGBitsPerSample"); break;
	case prop_JPEGHSubSampling:	*str = agaliaString::create(L"JPEGINFOHEADER::JPEGHSubSampling"); break;
	case prop_JPEGVSubSampling:	*str = agaliaString::create(L"JPEGINFOHEADER::JPEGVSubSampling"); break;
	default:
		return E_INVALIDARG;
	}

	return S_OK;
}



HRESULT item_strf_vids::getPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	auto hr = RIFF_item_Base::getPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	std::wstringstream temp;

	if (index == prop_biSize)
	{
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biSize)>(temp, offsetof(BITMAPINFOHEADER, biSize), this);
	}
	else if (index == prop_biWidth)
	{
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biWidth)>(temp, offsetof(BITMAPINFOHEADER, biWidth), this);
	}
	else if (index == prop_biHeight)
	{
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biHeight)>(temp, offsetof(BITMAPINFOHEADER, biHeight), this);
	}
	else if (index == prop_biPlanes)
	{
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biPlanes)>(temp, offsetof(BITMAPINFOHEADER, biPlanes), this);
	}
	else if (index == prop_biBitCount)
	{
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biBitCount)>(temp, offsetof(BITMAPINFOHEADER, biBitCount), this);
	}
	else if (index == prop_biCompression)
	{
		hr = format_chunksub_fcc<decltype(BITMAPINFOHEADER::biCompression)>(temp, offsetof(BITMAPINFOHEADER, biCompression), this);
	}
	else if (index == prop_biSizeImage)
	{
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biSizeImage)>(temp, offsetof(BITMAPINFOHEADER, biSizeImage), this);
	}
	else if (index == prop_biXPelsPerMeter)
	{
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biXPelsPerMeter)>(temp, offsetof(BITMAPINFOHEADER, biXPelsPerMeter), this);
	}
	else if (index == prop_biYPelsPerMeter)
	{
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biYPelsPerMeter)>(temp, offsetof(BITMAPINFOHEADER, biYPelsPerMeter), this);
	}
	else if (index == prop_biClrUsed)
	{
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biClrUsed)>(temp, offsetof(BITMAPINFOHEADER, biClrUsed), this);
	}
	else if (index == prop_biClrImportant)
	{
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biClrImportant)>(temp, offsetof(BITMAPINFOHEADER, biClrImportant), this);
	}
	else if (index == prop_biExtDataOffset)
	{
		hr = format_chunksub_dec<decltype(EXBMINFOHEADER::biExtDataOffset)>(temp, offsetof(EXBMINFOHEADER, biExtDataOffset), this);
	}
	else if (index == prop_JPEGSize)
	{
		hr = format_chunksub_dec<decltype(JPEGINFOHEADER::JPEGSize)>(temp, sizeof(EXBMINFOHEADER) + offsetof(JPEGINFOHEADER, JPEGSize), this);
	}
	else if (index == prop_JPEGProcess)
	{
		hr = format_chunksub_dec<decltype(JPEGINFOHEADER::JPEGProcess)>(temp, sizeof(EXBMINFOHEADER) + offsetof(JPEGINFOHEADER, JPEGProcess), this);
	}
	else if (index == prop_JPEGColorSpaceID)
	{
		hr = format_chunksub_dec<decltype(JPEGINFOHEADER::JPEGColorSpaceID)>(temp, sizeof(EXBMINFOHEADER) + offsetof(JPEGINFOHEADER, JPEGColorSpaceID), this);
	}
	else if (index == prop_JPEGBitsPerSample)
	{
		hr = format_chunksub_dec<decltype(JPEGINFOHEADER::JPEGBitsPerSample)>(temp, sizeof(EXBMINFOHEADER) + offsetof(JPEGINFOHEADER, JPEGBitsPerSample), this);
	}
	else if (index == prop_JPEGHSubSampling)
	{
		hr = format_chunksub_dec<decltype(JPEGINFOHEADER::JPEGHSubSampling)>(temp, sizeof(EXBMINFOHEADER) + offsetof(JPEGINFOHEADER, JPEGHSubSampling), this);
	}
	else if (index == prop_JPEGVSubSampling)
	{
		hr = format_chunksub_dec<decltype(JPEGINFOHEADER::JPEGVSubSampling)>(temp, sizeof(EXBMINFOHEADER) + offsetof(JPEGINFOHEADER, JPEGVSubSampling), this);
	}
	else
	{
		hr = E_INVALIDARG;
	}

	if (FAILED(hr)) return hr;

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}



HRESULT item_strf_vids::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_value)
	{
		std::wstringstream temp;
		temp << L"biSize:";
		format_chunksub_dec<decltype(BITMAPINFOHEADER::biSize)>(temp, offsetof(BITMAPINFOHEADER, biSize), this);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}

	return __super::getColumnValue(column, str);
}



HRESULT item_strf_vids::getAdditionalInfoCount(uint32_t* row) const
{
	if (getSize() < sizeof(RIFFCHUNK) + sizeof(BITMAPINFOHEADER))
	{
		*row = 0;
	}
	else if (getSize() < sizeof(RIFFCHUNK) + sizeof(EXBMINFOHEADER))
	{
		*row = addinf_biClrImportant + 1;
	}
	else if (getSize() < sizeof(RIFFCHUNK) + sizeof(EXBMINFOHEADER) + sizeof(JPEGINFOHEADER))
	{
		*row = addinf_biExtDataOffset + 1;
	}
	else
	{
		BITMAPINFOHEADER bmpInfo = {};
		auto hr = image->ReadData(
			&bmpInfo.biCompression,
			getOffset() + sizeof(RIFFCHUNK) + offsetof(BITMAPINFOHEADER, biCompression),
			sizeof(bmpInfo.biCompression));
		
		if (SUCCEEDED(hr) && bmpInfo.biCompression == JPEG_DIB || bmpInfo.biCompression == MJPG_DIB)
		{
			*row = addinf_last;
		}
		else
		{
			*row = addinf_biExtDataOffset + 1;
		}
	}

	return S_OK;
}



HRESULT item_strf_vids::getAdditionalInfoValue(uint32_t row, agaliaString** str) const
{
	HRESULT hr;
	std::wstringstream temp;

	if (row == addinf_biWidth)
	{
		temp << L"biWidth:";
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biWidth)>(temp, offsetof(BITMAPINFOHEADER, biWidth), this);
	}
	else if (row == addinf_biHeight)
	{
		temp << L"biHeight:";
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biHeight)>(temp, offsetof(BITMAPINFOHEADER, biHeight), this);
	}
	else if (row == addinf_biPlanes)
	{
		temp << L"biPlanes:";
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biPlanes)>(temp, offsetof(BITMAPINFOHEADER, biPlanes), this);
	}
	else if (row == addinf_biBitCount)
	{
		temp << L"biBitCount:";
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biBitCount)>(temp, offsetof(BITMAPINFOHEADER, biBitCount), this);
	}
	else if (row == addinf_biCompression)
	{
		temp << L"biCompression:";
		hr = format_chunksub_fcc<decltype(BITMAPINFOHEADER::biCompression)>(temp, offsetof(BITMAPINFOHEADER, biCompression), this);
	}
	else if (row == addinf_biSizeImage)
	{
		temp << L"biSizeImage:";
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biSizeImage)>(temp, offsetof(BITMAPINFOHEADER, biSizeImage), this);
	}
	else if (row == addinf_biXPelsPerMeter)
	{
		temp << L"biXPelsPerMeter:";
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biXPelsPerMeter)>(temp, offsetof(BITMAPINFOHEADER, biXPelsPerMeter), this);
	}
	else if (row == addinf_biYPelsPerMeter)
	{
		temp << L"biYPelsPerMeter:";
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biYPelsPerMeter)>(temp, offsetof(BITMAPINFOHEADER, biYPelsPerMeter), this);
	}
	else if (row == addinf_biClrUsed)
	{
		temp << L"biClrUsed:";
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biClrUsed)>(temp, offsetof(BITMAPINFOHEADER, biClrUsed), this);
	}
	else if (row == addinf_biClrImportant)
	{
		temp << L"biClrImportant:";
		hr = format_chunksub_dec<decltype(BITMAPINFOHEADER::biClrImportant)>(temp, offsetof(BITMAPINFOHEADER, biClrImportant), this);
	}
	else if (row == addinf_biExtDataOffset)
	{
		temp << L"biExtDataOffset:";
		hr = format_chunksub_dec<decltype(EXBMINFOHEADER::biExtDataOffset)>(temp, offsetof(EXBMINFOHEADER, biExtDataOffset), this);
	}
	else if (row == addinf_JPEGSize)
	{
		temp << L"JPEGSize:";
		hr = format_chunksub_dec<decltype(JPEGINFOHEADER::JPEGSize)>(temp, sizeof(EXBMINFOHEADER) + offsetof(JPEGINFOHEADER, JPEGSize), this);
	}
	else if (row == addinf_JPEGProcess)
	{
		temp << L"JPEGProcess:";
		hr = format_chunksub_dec<decltype(JPEGINFOHEADER::JPEGProcess)>(temp, sizeof(EXBMINFOHEADER) + offsetof(JPEGINFOHEADER, JPEGProcess), this);
	}
	else if (row == addinf_JPEGColorSpaceID)
	{
		temp << L"JPEGColorSpaceID:";
		hr = format_chunksub_dec<decltype(JPEGINFOHEADER::JPEGColorSpaceID)>(temp, sizeof(EXBMINFOHEADER) + offsetof(JPEGINFOHEADER, JPEGColorSpaceID), this);
	}
	else if (row == addinf_JPEGBitsPerSample)
	{
		temp << L"JPEGBitsPerSample:";
		hr = format_chunksub_dec<decltype(JPEGINFOHEADER::JPEGBitsPerSample)>(temp, sizeof(EXBMINFOHEADER) + offsetof(JPEGINFOHEADER, JPEGBitsPerSample), this);
	}
	else if (row == addinf_JPEGHSubSampling)
	{
		temp << L"JPEGHSubSampling:";
		hr = format_chunksub_dec<decltype(JPEGINFOHEADER::JPEGHSubSampling)>(temp, sizeof(EXBMINFOHEADER) + offsetof(JPEGINFOHEADER, JPEGHSubSampling), this);
	}
	else if (row == addinf_JPEGVSubSampling)
	{
		temp << L"JPEGVSubSampling:";
		hr = format_chunksub_dec<decltype(JPEGINFOHEADER::JPEGVSubSampling)>(temp, sizeof(EXBMINFOHEADER) + offsetof(JPEGINFOHEADER, JPEGVSubSampling), this);
	}
	else
	{
		hr = E_INVALIDARG;
	}

	if (FAILED(hr)) return hr;

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
