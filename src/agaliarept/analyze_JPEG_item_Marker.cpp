#include "pch.h"
#include "analyze_JPEG_item_Marker.h"

#include "analyze_JPEG_util.h"
#include "analyze_JPEG_item_SOI.h"
#include "analyze_JPEG_item_SOF.h"
#include "analyze_JPEG_item_DHT.h"
#include "analyze_JPEG_item_DAC.h"
#include "analyze_JPEG_item_SOS.h"
#include "analyze_JPEG_item_DQT.h"
#include "analyze_JPEG_item_DRI.h"
#include "analyze_JPEG_item_COM.h"
#include "analyze_JPEG_item_APP.h"

#include "jpeg_def.h"

using namespace analyze_JPEG;




// マーカーの文字列を取得（解放不可） 
const wchar_t* get_marker_name(uint16_t marker)
{
	switch (marker)
	{
	case SOF0: return L"SOF₀";
	case SOF1: return L"SOF₁";
	case SOF2: return L"SOF₂";
	case SOF3: return L"SOF₃";
	case SOF5: return L"SOF₅";
	case SOF6: return L"SOF₆";
	case SOF7: return L"SOF₇";
	case JPG: return L"JPG";
	case SOF9: return L"SOF₉";
	case SOF10: return L"SOF₁₀";
	case SOF11: return L"SOF₁₁";
	case SOF13: return L"SOF₁₃";
	case SOF14: return L"SOF₁₄";
	case SOF15: return L"SOF₁₅";
	case DHT: return L"DHT";
	case DAC: return L"DAC";
	case RST0: return L"RST₀";
	case RST1: return L"RST₁";
	case RST2: return L"RST₂";
	case RST3: return L"RST₃";
	case RST4: return L"RST₄";
	case RST5: return L"RST₅";
	case RST6: return L"RST₆";
	case RST7: return L"RST₇";
	case SOI: return L"SOI";
	case EOI: return L"EOI";
	case SOS: return L"SOS";
	case DQT: return L"DQT";
	case DNL: return L"DNL";
	case DRI: return L"DRI";
	case DHP: return L"DHP";
	case EXP: return L"EXP";
	case APP0: return L"APP₀";
	case APP1: return L"APP₁";
	case APP2: return L"APP₂";
	case APP3: return L"APP₃";
	case APP4: return L"APP₄";
	case APP5: return L"APP₅";
	case APP6: return L"APP₆";
	case APP7: return L"APP₇";
	case APP8: return L"APP₈";
	case APP9: return L"APP₉";
	case APP10: return L"APP₁₀";
	case APP11: return L"APP₁₁";
	case APP12: return L"APP₁₂";
	case APP13: return L"APP₁₃";
	case APP14: return L"APP₁₄";
	case APP15: return L"APP₁₅";
	case JPG0: return L"JPG₀";
	case JPG1: return L"JPG₁";
	case JPG2: return L"JPG₂";
	case JPG3: return L"JPG₃";
	case JPG4: return L"JPG₄";
	case JPG5: return L"JPG₅";
	case JPG6: return L"JPG₆";
	case JPG7: return L"JPG₇";
	case JPG8: return L"JPG₈";
	case JPG9: return L"JPG₉";
	case JPG10: return L"JPG₁₀";
	case JPG11: return L"JPG₁₁";
	case JPG12: return L"JPG₁₂";
	case JPG13: return L"JPG₁₃";
	case COM: return L"COM";
	case TEM: return L"TEM";
	}
	if (RES_Start <= marker && marker <= RES_End) {
		return L"RES";
	}
	return L"(unknown)";
}



uint64_t searchNextMarker(const container_JPEG* image, uint64_t next_offset)
{
	uint64_t pos = 0;

	uint8_t byte_data = 0;
	while (SUCCEEDED(image->ReadData(&byte_data, next_offset + pos, sizeof(byte_data))))
	{
		if (byte_data != 0xFF)
		{
			pos++;
			continue;
		}

		// マーカーの可能性があるので読み込みしなおす 
		uint16_t word_data = 0;
		if (FAILED(image->ReadData(&word_data, next_offset + pos, sizeof(word_data))))
		{
			pos++;
			return pos;	// データが0xFFで終端しているのでループ中断 
		}

		switch (agalia_byteswap(word_data))
		{
		case 0xFFFF:	// １バイト目はスタッフィングバイトなのでスキップ
			pos += 1;
			break;
		case 0xFF00:	// マーカーではなくデータなのでスキップ 
		case RST0:		// リスタートマーカーもスキップ 
		case RST1:
		case RST2:
		case RST3:
		case RST4:
		case RST5:
		case RST6:
		case RST7:
			pos += 2;
			break;
		default:		// マーカーが見つかったらループ中断 
			return pos;
		}
	}

	return pos;
}



HRESULT analyze_JPEG::create_item(agaliaElement** next, uint64_t next_item_offset, const container_JPEG* image)
{
	if (next == nullptr) return E_POINTER;

	JPEGSEGMENT next_seg = {};
	auto hr = image->ReadData(&next_seg, next_item_offset, sizeof(next_seg));
	if (FAILED(hr)) return hr;

	uint64_t next_item_size = 0;
	switch (agalia_byteswap(next_seg.marker))
	{
	case SOI:
	case EOI:
		next_item_size = sizeof(JPEGSEGMENT);
		break;

	default:
		if ((agalia_byteswap(next_seg.marker) & 0xFF00) == 0xFF00)
		{
			uint16_t length = 0;
			hr = image->ReadData(&length, next_item_offset + sizeof(JPEGSEGMENT), sizeof(length));
			if (FAILED(hr)) return hr;
			next_item_size = static_cast<uint64_t>(sizeof(JPEGSEGMENT)) + agalia_byteswap(length);
		}
	}
	next_item_size += searchNextMarker(image, next_item_offset + next_item_size);


	switch (agalia_byteswap(next_seg.marker))
	{
	case SOF0: case SOF1: case SOF2: case SOF3:
	case SOF9: case SOF10: case SOF11:
		*next = new item_SOF(image, next_item_offset, next_item_size, JPEG_item_Base::marker_segment);
		break;

	case DHT:
		*next = new item_DHT(image, next_item_offset, next_item_size, JPEG_item_Base::marker_segment);
		break;

	case DAC:
		*next = new item_DAC(image, next_item_offset, next_item_size, JPEG_item_Base::marker_segment);
		break;

	case DQT:
		*next = new item_DQT(image, next_item_offset, next_item_size, JPEG_item_Base::marker_segment);
		break;

	case SOS:
		*next = new item_SOS(image, next_item_offset, next_item_size, JPEG_item_Base::marker_segment);
		break;

	case COM:
		*next = new item_COM(image, next_item_offset, next_item_size, JPEG_item_Base::marker_segment);
		break;

	case DRI:
		*next = new item_DRI(image, next_item_offset, next_item_size, JPEG_item_Base::marker_segment);
		break;

	case APP0: case APP1: case APP2: case APP3:
	case APP4: case APP5: case APP6: case APP7:
	case APP8: case APP9: case APP10: case APP11:
	case APP12: case APP13: case APP14: case APP15:
		*next = new item_APP(image, next_item_offset, next_item_size, JPEG_item_Base::marker_segment);
		break;

	case SOI:
	case EOI:
		*next = new item_SOI(image, next_item_offset, next_item_size, JPEG_item_Base::marker_segment);
		break;

	default:
		*next = new item_Marker(image, next_item_offset, next_item_size, JPEG_item_Base::marker_segment);
	}
	return S_OK;
}






item_Marker::item_Marker(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type)
	: JPEG_item_Base(image, offset, size, type)
{
}



item_Marker::~item_Marker()
{
}



HRESULT item_Marker::getName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	*str = agaliaString::create(get_marker_name(getMarker(image, this)));
	return S_OK;
}



HRESULT item_Marker::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;

	*count = 3;
	return S_OK;
}



HRESULT item_Marker::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case 0:	name = L"offset";	break;
	case 1: name = L"marker";	break;
	case 2: name = L"name";		break;
	default:
		return E_FAIL;
	}

	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT item_Marker::getPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	switch (index)
	{
	case 0:
	{
		wchar_t temp[128];
		swprintf_s(temp, L"%I64u", getOffset());
		*str = agaliaString::create(temp);
	}
	return S_OK;

	case 1:
	{
		JPEGSEGMENT segment = {};
		auto hr = image->ReadData(&segment, getOffset(), sizeof(segment));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << std::hex << std::uppercase << agalia_byteswap(segment.marker);
		*str = agaliaString::create(temp.str().c_str());
	}
	return S_OK;

	case 2:
		return getName(str);
	}

	return E_FAIL;
}



HRESULT item_Marker::getNext(agaliaElement** next) const
{
	if (next == nullptr) return E_POINTER;

	return create_item(next, getOffset() + getSize(), image);
}



HRESULT item_Marker::getValueAreaOffset(uint64_t* offset) const
{
	if (offset == nullptr) return E_FAIL;

	JPEGSEGMENT next_seg = {};
	auto hr = image->ReadData(&next_seg, getOffset(), sizeof(next_seg));
	if (FAILED(hr)) return hr;

	if ((agalia_byteswap(next_seg.marker) & 0xFF00) == 0xFF00)
	{
		*offset = getOffset() + sizeof(JPEGSEGMENT) + sizeof(uint16_t);
		return S_OK;
	}

	return E_FAIL;
}



HRESULT item_Marker::getValueAreaSize(uint64_t* size) const
{
	if (size == nullptr) return E_FAIL;

	JPEGSEGMENT next_seg = {};
	auto hr = image->ReadData(&next_seg, getOffset(), sizeof(next_seg));
	if (FAILED(hr)) return hr;

	if ((agalia_byteswap(next_seg.marker) & 0xFF00) == 0xFF00)
	{
		*size = getSize() - (sizeof(JPEGSEGMENT) + sizeof(uint16_t));
		return S_OK;
	}

	return E_FAIL;
}



HRESULT item_Marker::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_offset)
	{
		std::wstringstream temp;
		format_hex(temp, getOffset(), 8);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (column == column_marker)
	{
		uint16_t marker = 0;
		if (SUCCEEDED(image->ReadData(&marker, this->getOffset(), sizeof(marker))))
		{
			std::wstringstream temp;
			format_hex(temp, getMarker(image, this));
			*str = agaliaString::create(temp.str().c_str());
			return S_OK;
		}
	}
	else if (column == column_name)
	{
		uint16_t marker = 0;
		if (SUCCEEDED(image->ReadData(&marker, this->getOffset(), sizeof(marker))))
		{
			*str = agaliaString::create(get_marker_name(agalia_byteswap(marker)));
			return S_OK;
		}
	}
	else if (column == column_length)
	{
		uint16_t marker = 0;
		if (SUCCEEDED(image->ReadData(&marker, this->getOffset(), sizeof(marker))))
		{
			if ((agalia_byteswap(marker) & 0xFF00) == 0xFF00)
			{
				uint16_t length = 0;
				if (SUCCEEDED(image->ReadData(&length, this->getOffset() + sizeof(marker), sizeof(length))))
				{
					std::wstringstream temp;
					format_dec(temp, agalia_byteswap(length));
					*str = agaliaString::create(temp.str().c_str());
					return S_OK;
				}
			}
		}
	}
	else if (column == column_value)
	{
		uint16_t marker = 0;
		if (SUCCEEDED(image->ReadData(&marker, this->getOffset(), sizeof(marker))))
		{
			CHeapPtr<uint8_t> buf;
			rsize_t bufsize = 0;
			auto hr = ReadSegment(buf, &bufsize, image, this);
			if (FAILED(hr)) return hr;

			uint64_t counts = getSize() / sizeof(uint8_t);
			const uint8_t* pVal = buf;

			std::wstring temp;
			const rsize_t limit = 128;	//config->byte_stream_limit_length;
			temp.reserve(limit * 3 + 8);

			temp += L"[";
			if (counts != 0)
			{
				wchar_t c[16] = {};
				fast_itow16(pVal[0], c);
				temp += c;

				for (unsigned int i = 1; i < min(limit, counts); i++)
				{
					temp += L',';
					fast_itow16(pVal[i], c);
					temp += c;
				}

				if (limit < counts) {
					temp += L",...";
				}
			}
			temp += L"]";

			*str = agaliaString::create(temp.c_str());
			return S_OK;
		}
	}

	return E_FAIL;
}



HRESULT item_Marker::getAdditionalInfoCount(uint32_t* row) const
{
	*row = 0;
	return S_OK;
}



HRESULT item_Marker::getAdditionalInfoValue(uint32_t, agaliaString**) const
{
	return E_INVALIDARG;
}
