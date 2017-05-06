#include "stdafx.h"
#include "jpeg_def.h"
using namespace agalia_jpeg;

#include "callback.h"

#include <stdlib.h>
#include <stdio.h>
#include <atlbase.h>

const wchar_t* get_marker_name(uint16_t marker)
{
	switch (marker)
	{
	case SOF0: return L"SOF0";
	case SOF1: return L"SOF1";
	case SOF2: return L"SOF2";
	case SOF3: return L"SOF3";
	case SOF5: return L"SOF5";
	case SOF6: return L"SOF6";
	case SOF7: return L"SOF7";
	case JPG: return L"JPG";
	case SOF9: return L"SOF9";
	case SOF10: return L"SOF10";
	case SOF11: return L"SOF11";
	case SOF13: return L"SOF13";
	case SOF14: return L"SOF14";
	case SOF15: return L"SOF15";
	case DHT: return L"DHT";
	case DAC: return L"DAC";
	case RST0: return L"RST0";
	case RST1: return L"RST1";
	case RST2: return L"RST2";
	case RST3: return L"RST3";
	case RST4: return L"RST4";
	case RST5: return L"RST5";
	case RST6: return L"RST6";
	case RST7: return L"RST7";
	case SOI: return L"SOI";
	case EOI: return L"EOI";
	case SOS: return L"SOS";
	case DQT: return L"DQT";
	case DNL: return L"DNL";
	case DRI: return L"DRI";
	case DHP: return L"DHP";
	case EXP: return L"EXP";
	case APP0: return L"APP0";
	case APP1: return L"APP1";
	case APP2: return L"APP2";
	case APP3: return L"APP3";
	case APP4: return L"APP4";
	case APP5: return L"APP5";
	case APP6: return L"APP6";
	case APP7: return L"APP7";
	case APP8: return L"APP8";
	case APP9: return L"APP9";
	case APPA: return L"APPA";
	case APPB: return L"APPB";
	case APPC: return L"APPC";
	case APPD: return L"APPD";
	case APPE: return L"APPE";
	case APPF: return L"APPF";
	case JPG0: return L"JPG0";
	case JPG1: return L"JPG1";
	case JPG2: return L"JPG2";
	case JPG3: return L"JPG3";
	case JPG4: return L"JPG4";
	case JPG5: return L"JPG5";
	case JPG6: return L"JPG6";
	case JPG7: return L"JPG7";
	case JPG8: return L"JPG8";
	case JPG9: return L"JPG9";
	case JPGA: return L"JPGA";
	case JPGB: return L"JPGB";
	case JPGC: return L"JPGC";
	case JPGD: return L"JPGD";
	case COM: return L"COM";
	case TEM: return L"TEM";
	}
	if (RES_Start <= marker && marker <= RES_End) {
		return L"RES";
	}
	return nullptr;
}

uint64_t parse_NoBody(int subItem, uint64_t offset, const uint8_t* base, callback_cls* callback)
{
	UNREFERENCED_PARAMETER(subItem);
	UNREFERENCED_PARAMETER(base);
	UNREFERENCED_PARAMETER(callback);
	offset += sizeof(JPEGSEGMENT::marker);
	callback->set_extra_data(offset, 0);
	return offset;
}

uint64_t parse_COM(int subItem, uint64_t offset, const uint8_t* base, callback_cls* callback)
{
	const JPEGSEGMENT_COM* pCOM = reinterpret_cast<const JPEGSEGMENT_COM*>(base + offset);
	uint16_t length = _byteswap_ushort(pCOM->Lc);

	callback->set_extra_data(offset + sizeof(JPEGSEGMENT::marker) + sizeof(pCOM->Lc), length);

	// length 
	{
		wchar_t temp[64] = {};
		_itow_s(length, temp, 10);
		callback->print(subItem, temp);
	}

	// value 
	{
		size_t bufSize = 32 + length;
		CHeapPtr<wchar_t> p;
		p.Allocate(bufSize);

		swprintf_s(p, bufSize, L"Lc=%u, Cm=", length);
		size_t k = wcslen(p);
		for (int i = 0; i < length - 2; i++)
		{
			swprintf_s(p + k + i, bufSize - k - i, L"%C", pCOM->Cm[i]);
		}
		callback->print(subItem + 1, p);
	}

	offset += sizeof(JPEGSEGMENT::marker) + length;
	return offset;
}

uint64_t parse_APP(int subItem, uint64_t offset, const uint8_t* base, callback_cls* callback)
{
	const JPEGSEGMENT_APP* pApp = reinterpret_cast<const JPEGSEGMENT_APP*>(base + offset);
	uint16_t length = _byteswap_ushort(pApp->length);

	callback->set_extra_data(offset + sizeof(JPEGSEGMENT::marker) + sizeof(pApp->length), length);

	// length 
	{
		wchar_t temp[64] = {};
		_itow_s(length, temp, 10);
		callback->print(subItem, temp);
	}

	// value 
	{
		std::wstring str;
		wchar_t temp[1024] = {};

		swprintf_s(temp, L"length=%u, ", length);
		str += temp;

		const unsigned char* p = base + offset + sizeof(JPEGSEGMENT_APP);

		str += L"{";
		for (size_t i = 0; i < min(_countof(temp), length) - 2; i++)
		{
			char v = p[i];
			if (v == '\0') break;
			swprintf_s(temp + i, _countof(temp) - i, L"%C", v);
		}
		str += temp;
		str += L"}";

		callback->print(subItem + 1, str);
	}

	offset += sizeof(JPEGSEGMENT::marker) + length;
	return offset;
}

uint64_t parse_APP0(int subItem, uint64_t offset, const uint8_t* base, callback_cls* callback)
{
	const JPEGSEGMENT_APP0* pApp0 = reinterpret_cast<const JPEGSEGMENT_APP0*>(base + offset);
	uint16_t length = _byteswap_ushort(pApp0->length);

	callback->set_extra_data(offset + sizeof(JPEGSEGMENT::marker) + sizeof(pApp0->length), length);

	// length 
	{
		wchar_t temp[64] = {};
		_itow_s(length, temp, 10);
		callback->print(subItem, temp);
	}

	// value 
	{
		std::wstring str;
		wchar_t temp[1024] = {};

		swprintf_s(temp, L"length=%u, ", length);
		str += temp;

		if (memcmp(pApp0->identifier, "JFIF", 5) == 0)
		{
			const JPEGSEGMENT_APP0_JFIF* pJFIF = reinterpret_cast<const JPEGSEGMENT_APP0_JFIF*>(base + offset);
			swprintf_s(temp, L"identifier=%S, ", pJFIF->identifier);
			str += temp;
			swprintf_s(temp, L"major=%.2x, ", pJFIF->major_version);
			str += temp;
			swprintf_s(temp, L"minor=%.2x, ", pJFIF->minor_version);
			str += temp;
			swprintf_s(temp, L"units=%.2x, ", pJFIF->units);
			str += temp;
			swprintf_s(temp, L"X=%u, ", _byteswap_ushort(pJFIF->Xdensity));
			str += temp;
			swprintf_s(temp, L"Y=%u, ", _byteswap_ushort(pJFIF->Ydensity));
			str += temp;
			swprintf_s(temp, L"Xthumb=%u, ", pJFIF->Xthumbnail);
			str += temp;
			swprintf_s(temp, L"Ythumb=%u", pJFIF->Ythumbnail);
			str += temp;
		}
		else
		{
			str += L"\"";
			for (size_t i = 0; i < min(_countof(temp), length) - 2; i++)
			{
				char v = *(&pApp0->identifier[0] + i);
				if (v == '\0') break;
				swprintf_s(temp + i, _countof(temp) - i, L"%C", v);
			}
			str += temp;
			str += L"\"";
		}
		callback->print(subItem + 1, str);
	}

	offset += sizeof(JPEGSEGMENT::marker) + length;
	return offset;
}

uint64_t parse_APP1(int subItem, uint64_t offset, const uint8_t* base, callback_cls* callback)
{
	const JPEGSEGMENT_APP1* pApp1 = reinterpret_cast<const JPEGSEGMENT_APP1*>(base + offset);
	uint16_t length = _byteswap_ushort(pApp1->length);

	callback->set_extra_data(offset + sizeof(JPEGSEGMENT::marker) + sizeof(pApp1->length), length);

	// length 
	{
		wchar_t temp[64] = {};
		_itow_s(length, temp, 10);
		callback->print(subItem, temp);
	}

	// value 
	{
		std::wstring str;
		wchar_t temp[1024] = {};

		swprintf_s(temp, L"length=%u, ", length);
		str += temp;

		if (memcmp(pApp1->identifier, "Exif\0\0", 6) == 0)
		{
			CHeapPtr<char> p;
			p.Allocate(_countof(pApp1->identifier) + 1);
			memcpy(p, pApp1->identifier, sizeof(pApp1->identifier));
			p[_countof(pApp1->identifier)] = '\0';
			swprintf_s(temp, L"identifier=%S", p.m_pData);
			str += temp;
		}
		else
		{
			str += L"{";
			for (size_t i = 0; i < min(_countof(temp), length) - 2; i++)
			{
				char v = *(&pApp1->identifier[0] + i);
				if (v == '\0') break;
				swprintf_s(temp + i, _countof(temp) - i, L"%C", v);
			}
			str += temp;
			str += L"}";
		}
		callback->print(subItem + 1, str);
	}

	offset += sizeof(JPEGSEGMENT::marker) + length;
	return offset;
}

uint64_t parse_DQT(int subItem, uint64_t offset, const uint8_t* base, callback_cls* callback)
{
	const JPEGSEGMENT_DQT* pDQT = reinterpret_cast<const JPEGSEGMENT_DQT*>(base + offset);
	uint16_t length = _byteswap_ushort(pDQT->Lq);

	callback->set_extra_data(offset + sizeof(JPEGSEGMENT::marker) + sizeof(pDQT->Lq), length);

	// length 
	{
		wchar_t temp[64] = {};
		_itow_s(length, temp, 10);
		callback->print(subItem, temp);
	}

	// value 
	{
		wchar_t temp[1024] = {};

		swprintf_s(temp, L"Lq=%u", length);
		callback->print(subItem + 1, temp);

		uint32_t last = length - 2;
		uint32_t cur = 0;
		while (cur + 1 + 64 * 1 <= last)
		{
			const JPEGSEGMENT_DQT_BODY* pBody = reinterpret_cast<const JPEGSEGMENT_DQT_BODY*>(base + offset + sizeof(JPEGSEGMENT::marker) + 2 + cur);

			callback->next();
			swprintf_s(temp, L"Pq=%u, Tq=%u", pBody->Pq, pBody->Tq);
			callback->print(subItem + 1, temp);

			std::wstring str;
			switch (pBody->Pq)
			{
			case 0:
				callback->next();
				str = L"Q={";
				swprintf_s(temp, L"%u", pBody->Q8[0]);
				str += temp;
				for (int i = 1; i < _countof(pBody->Q8); i++) {
					swprintf_s(temp, L",%u", pBody->Q8[i]);
					str += temp;
				}
				str += L"}";
				callback->print(subItem + 1, str);
				cur += 1 + 64 * 1;
				break;

			case 1:
				callback->next();
				str = L"Q={";
				if (last - cur >= 1 + 64 * 2)
				{
					swprintf_s(temp, L"%u", _byteswap_ushort(pBody->Q16[0]));
					str += temp;
					for (int i = 1; i < _countof(pBody->Q16); i++) {
						swprintf_s(temp, L",%u", _byteswap_ushort(pBody->Q16[i]));
						str += temp;
					}
				}
				str += L"}";
				callback->print(subItem + 1, str);
				cur += 1 + 64 * 2;
				break;

			default:
				cur = USHRT_MAX;
			}
		}
	}

	offset += sizeof(JPEGSEGMENT::marker) + length;
	return offset;
}

uint64_t parse_SOF(int subItem, uint64_t offset, const uint8_t* base, callback_cls* callback)
{
	const JPEGSEGMENT_SOF* pSOF = reinterpret_cast<const JPEGSEGMENT_SOF*>(base + offset);
	uint16_t length = _byteswap_ushort(pSOF->Lf);

	callback->set_extra_data(offset + sizeof(JPEGSEGMENT::marker) + sizeof(pSOF->Lf), length);

	// length 
	{
		wchar_t temp[64] = {};
		_itow_s(length, temp, 10);
		callback->print(subItem, temp);
	}

	// value 
	{
		wchar_t temp[1024] = {};

		std::wstring str;
		swprintf_s(temp, L"Lf=%u, ", length);
		str += temp;
		swprintf_s(temp, L"P=%u, ", pSOF->P);
		str += temp;
		swprintf_s(temp, L"Y=%u, ", _byteswap_ushort(pSOF->Y));
		str += temp;
		swprintf_s(temp, L"X=%u, ", _byteswap_ushort(pSOF->X));
		str += temp;
		swprintf_s(temp, L"Nf=%u", pSOF->Nf);
		str += temp;
		callback->print(subItem + 1, str);

		for (int i = 0; i < pSOF->Nf; i++)
		{
			str.clear();
			callback->next();
			swprintf_s(temp, L"C=%u, ", pSOF->comp[i].C);
			str += temp;
			swprintf_s(temp, L"H=%u, V=%u, ", pSOF->comp[i].H, pSOF->comp[i].V);
			str += temp;
			swprintf_s(temp, L"Tq=%u", pSOF->comp[i].Tq);
			str += temp;
			callback->print(subItem + 1, str);
		}
	}

	offset += sizeof(JPEGSEGMENT::marker) + length;
	return offset;
}

uint64_t parse_DHT(int subItem, uint64_t offset, const uint8_t* base, callback_cls* callback)
{
	const JPEGSEGMENT_DHT* pDHT = reinterpret_cast<const JPEGSEGMENT_DHT*>(base + offset);
	uint16_t length = _byteswap_ushort(pDHT->Lh);

	callback->set_extra_data(offset + sizeof(JPEGSEGMENT::marker) + sizeof(pDHT->Lh), length);

	// length 
	{
		wchar_t temp[64] = {};
		_itow_s(length, temp, 10);
		callback->print(subItem, temp);
	}

	// value 
	{
		wchar_t temp[1024] = {};

		swprintf_s(temp, L"Lh=%u", length);
		callback->print(subItem + 1, temp);

		uint32_t last = length - 2;
		uint32_t cur = 0;
		while (cur + 1 + 16 < last)
		{
			std::wstring str;

			const JPEGSEGMENT_DHT_BODY* pBody = reinterpret_cast<const JPEGSEGMENT_DHT_BODY*>(base + offset + sizeof(JPEGSEGMENT::marker) + 2 + cur);

			callback->next();
			swprintf_s(temp, L"Tc=%u", pBody->Tc);
			str += temp;
			swprintf_s(temp, L", Th=%u", pBody->Th);
			str += temp;
			callback->print(subItem + 1, str);

			str.clear();
			callback->next();
			uint32_t total = 0;
			for (int i = 0; i < _countof(pBody->L); i++)
			{
				if (i != 0) str += L", ";
				swprintf_s(temp, L"L%d=%.2x", i + 1, pBody->L[i]);
				str += temp;
				total += pBody->L[i];
			}
			callback->print(subItem + 1, str);

			const uint8_t* V = pBody->V;
			for (int i = 0; i < _countof(pBody->L); i++)
			{
				if (pBody->L[i])
				{
					callback->next();
					swprintf_s(temp, L"V[%d]={", i + 1);
					str = temp;
					for (int j = 1; j <= pBody->L[i]; j++) {
						if (j != 1) str += L",";
						swprintf_s(temp, L"%.2x", *V++);
						str += temp;
					}
					str += L"}";
					callback->print(subItem + 1, str);
				}
			}

			cur += 1 + 16 + total;
		}
	}

	offset += sizeof(JPEGSEGMENT::marker) + length;
	return offset;
}

uint64_t parse_DAC(int subItem, uint64_t offset, const uint8_t* base, callback_cls* callback)
{
	const JPEGSEGMENT_DAC* pDAC = reinterpret_cast<const JPEGSEGMENT_DAC*>(base + offset);
	uint16_t length = _byteswap_ushort(pDAC->La);

	callback->set_extra_data(offset + sizeof(JPEGSEGMENT::marker) + sizeof(pDAC->La), length);

	// length 
	{
		wchar_t temp[64] = {};
		_itow_s(length, temp, 10);
		callback->print(subItem, temp);
	}

	// value 
	{
		wchar_t temp[1024] = {};

		swprintf_s(temp, L"La=%u", length);
		callback->print(subItem + 1, temp);

		for ( size_t i = 0; i < (length - 2) / sizeof(JPEGSEGMENT_DAC::comp[0]); i++)
		{
			callback->next();
			swprintf_s(temp, L"Tc=%u, Th=%u, Cs=%u", pDAC->comp[i].Tc, pDAC->comp[i].Th, pDAC->comp[i].Cs);
			callback->print(subItem + 1, temp);
		}
	}

	offset += sizeof(JPEGSEGMENT::marker) + length;
	return offset;
}

uint64_t parse_DHI(int subItem, uint64_t offset, const uint8_t* base, callback_cls* callback)
{
	const JPEGSEGMENT_DRI* pDRI = reinterpret_cast<const JPEGSEGMENT_DRI*>(base + offset);
	uint16_t length = _byteswap_ushort(pDRI->Lr);

	callback->set_extra_data(offset + sizeof(JPEGSEGMENT::marker) + sizeof(pDRI->Lr), length);

	// length 
	{
		wchar_t temp[64] = {};
		_itow_s(length, temp, 10);
		callback->print(subItem, temp);
	}

	// value 
	{
		wchar_t temp[64] = {};
		swprintf_s(temp, L"Lr=%u, Ri=%u", length, _byteswap_ushort(pDRI->Ri));
		callback->print(subItem + 1, temp);
	}

	offset += sizeof(JPEGSEGMENT::marker) + length;
	return offset;
}

uint64_t parse_SOS(int subItem, uint64_t offset, const uint8_t* base, uint64_t endPos, callback_cls* callback)
{
	const JPEGSEGMENT_SOS* pSOS = reinterpret_cast<const JPEGSEGMENT_SOS*>(base + offset);
	uint16_t length = _byteswap_ushort(pSOS->Ls);

	callback->set_extra_data(offset + sizeof(JPEGSEGMENT::marker) + sizeof(pSOS->Ls), length);

	// length 
	{
		wchar_t temp[64] = {};
		_itow_s(length, temp, 10);
		callback->print(subItem, temp);
	}

	// value 
	{
		wchar_t temp[1024] = {};

		std::wstring str;
		swprintf_s(temp, L"Ls=%u, ", length);
		str += temp;
		swprintf_s(temp, L"Ns=%u", pSOS->Ns);
		str += temp;
		callback->print(subItem + 1, str);

		for (int i = 0; i < pSOS->Ns; i++)
		{
			str.clear();
			callback->next();
			swprintf_s(temp, L"Cs%d=%u, ", i, pSOS->comp[i].Cs);
			str += temp;
			swprintf_s(temp, L"Td%d=%u, ", i, pSOS->comp[i].Td);
			str += temp;
			swprintf_s(temp, L"Ta%d=%u", i, pSOS->comp[i].Ta);
			str += temp;
			callback->print(subItem + 1, str);
		}

		const JPEGSEGMENT_SOS_Sffix* pSOS_Suffix =
			reinterpret_cast<const JPEGSEGMENT_SOS_Sffix*>
			(base + offset
				+ sizeof(pSOS->marker) + sizeof(pSOS->Ls) + sizeof(pSOS->Ns)
				+ sizeof(pSOS->comp[0]) * pSOS->Ns);

		str.clear();
		callback->next();
		swprintf_s(temp, L"Ss=%u, ", pSOS_Suffix->Ss);
		str += temp;
		swprintf_s(temp, L"Se=%u, ", pSOS_Suffix->Se);
		str += temp;
		swprintf_s(temp, L"Ah=%u, ", pSOS_Suffix->Ah);
		str += temp;
		swprintf_s(temp, L"Al=%u", pSOS_Suffix->Al);
		str += temp;
		callback->print(subItem + 1, str);
	}

	offset += sizeof(JPEGSEGMENT::marker) + length;

	bool loop = true;
	do
	{
		if (*(base + offset) != 0xFF)
		{
			offset++;
		}
		else
		{
			switch (_byteswap_ushort(*reinterpret_cast<const uint16_t*>(base + offset)))
			{
			case 0xFF00:
			case RST0:
			case RST1:
			case RST2:
			case RST3:
			case RST4:
			case RST5:
			case RST6:
			case RST7:
				offset += 2;
				break;
			case 0xFFFF:
				offset += 1;
				break;
			default:
				loop = false;
			}
		}
	} while (loop && offset < endPos);

	return offset;
}

void parseJpegData(const uint8_t* base, uint64_t endPos, callback_cls* callback, const agalia::config* config)
{
	uint64_t offset = 0;

	do
	{
		uint16_t marker = _byteswap_ushort(*reinterpret_cast<const uint16_t*>(base + offset));
		if (marker == 0xFFFF) {
			offset += 1;
			continue;
		}

		// offset 
		{
			wchar_t sz[16] = {};
			swprintf_s(sz, L"%.8I64X", offset);
			callback->print(0, sz);
		}

		// marker 
		if (0xFF00 <= marker)
		{
			wchar_t sz[16] = {};
			swprintf_s(sz, L"%.2X\t", marker);
			callback->print(1, sz);
		}

		// name 
		{
			const wchar_t* name = get_marker_name(marker);
			if (name) {
				callback->print(2, name);
			}
			else {
				callback->print(2, L"(unknown)");
			}
		}

		// value 
		switch (marker)
		{
		case SOI:
		case EOI:
			offset = parse_NoBody(3, offset, base, callback);	break;
		case COM:
			offset = parse_COM(3, offset, base, callback);	break;
		case APP0:
			offset = parse_APP0(3, offset, base, callback);	break;
		case APP1:
			offset = parse_APP1(3, offset, base, callback);	break;
		case APP2:
		case APP3:
		case APP4:
		case APP5:
		case APP6:
		case APP7:
		case APP8:
		case APP9:
		case APPA:
		case APPB:
		case APPC:
		case APPD:
		case APPE:
		case APPF:
			offset = parse_APP(3, offset, base, callback);	break;
		case DQT:
			offset = parse_DQT(3, offset, base, callback);	break;
		case SOF0:
		case SOF1:
		case SOF2:
		case SOF3:
		case SOF9:
		case SOF10:
		case SOF11:
			offset = parse_SOF(3, offset, base, callback);	break;
		case DHT:
			offset = parse_DHT(3, offset, base, callback);	break;
		case DAC:
			offset = parse_DAC(3, offset, base, callback);	break;
		case DRI:
			offset = parse_DHI(3, offset, base, callback);	break;
		case SOS:
			offset = parse_SOS(3, offset, base, endPos, callback);	break;
		default:
			void putDataAsByteArray(int subItem, const uint8_t* base_addr, uint64_t offset, uint64_t size, callback_cls* callback, const agalia::config* config);
			if (offset < endPos)
			{
				uint64_t size = endPos - offset;
				wchar_t sz[16];
				swprintf_s(sz, L"%I64u", size);
				callback->print(3, sz);
				putDataAsByteArray(4, base, offset, size, callback, config);
			}
			offset = SIZE_MAX;
		}

		callback->next();

		config->is_abort(agalia::config::throw_on_abort);

	} while (offset < endPos);
}

#include "image_file.h"

bool image_file_jpeg::is_supported(const void* buf, uint64_t size)
{
	if (buf == nullptr) return false;
	if (size < 4) return false;

	return (_byteswap_ushort(*reinterpret_cast<const uint16_t*>(buf)) == SOI);
}

void image_file_jpeg::parse(callback_cls* callback, const agalia::config* config)
{
	callback->insert_column(96, L"offset");
	callback->insert_column(64, L"marker");
	callback->insert_column(64, L"name");
	int length_column_number = callback->insert_column(96, L"length");
	callback->set_right_align(length_column_number, true);
	callback->insert_column(512, L"value");
	callback->next();

	parseJpegData(this->base_addr + this->base_offset, this->data_size, callback, config);
}


