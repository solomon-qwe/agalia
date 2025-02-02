#include "pch.h"
#include "analyze_BMP_item_BITMAPV5.h"

#include "analyze_BMP_item_EmbeddedProfile.h"
#include "analyze_BMP_item_LinkedProfile.h"
#include "analyze_BMP_item_RGBQUADParent.h"
#include "analyze_BMP_item_ColorMask.h"
#include "analyze_BMP_item_ColorIndexParent.h"

using namespace analyze_BMP;

item_BITMAPV5::item_BITMAPV5(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:BMP_item_Base(image, offset, size)
{
}

item_BITMAPV5::~item_BITMAPV5()
{
}

HRESULT item_BITMAPV5::getName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	uint64_t size = getSize();

	const wchar_t* name = nullptr;
	if (sizeof(BITMAPV5HEADER) <= size)
	{
		name = L"BITMAPV5HEADER";
	}
	else if (sizeof(BITMAPV4HEADER) <= size)
	{
		name = L"BITMAPV4HEADER";
	}
	else if (offsetof(BITMAPV5HEADER, bV5CSType) <= size)
	{
		name = L"BITMAPV3HEADER";
	}
	else if (offsetof(BITMAPV5HEADER, bV5AlphaMask) <= size)
	{
		name = L"BITMAPV2HEADER";
	}
	else if (sizeof(BITMAPINFOHEADER) <= size)
	{
		name = L"BITMAPINFOHEADER";
	}

	if (name == nullptr) return E_FAIL;
	*str = agaliaString::create(name);
	return S_OK;
}

HRESULT item_BITMAPV5::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	uint64_t size = getSize();

	switch (size)
	{
	case offsetof(BITMAPV5HEADER, bV5Size):
		*count = prop_Size;
		break;
	case offsetof(BITMAPV5HEADER, bV5Width):
		*count = prop_Width;
		break;
	case offsetof(BITMAPV5HEADER, bV5Height):
		*count = prop_Height;
		break;
	case offsetof(BITMAPV5HEADER, bV5Planes):
		*count = prop_Planes;
		break;
	case offsetof(BITMAPV5HEADER, bV5BitCount):
		*count = prop_BitCount;
		break;
	case offsetof(BITMAPV5HEADER, bV5Compression):
		*count = prop_Compression;
		break;
	case offsetof(BITMAPV5HEADER, bV5SizeImage):
		*count = prop_SizeImage;
		break;
	case offsetof(BITMAPV5HEADER, bV5XPelsPerMeter):
		*count = prop_XPelsPerMeter;
		break;
	case offsetof(BITMAPV5HEADER, bV5YPelsPerMeter):
		*count = prop_YPelsPerMeter;
		break;
	case offsetof(BITMAPV5HEADER, bV5ClrUsed):
		*count = prop_ClrUsed;
		break;
	case offsetof(BITMAPV5HEADER, bV5ClrImportant):
		*count = prop_ClrImportant;
		break;
	case offsetof(BITMAPV5HEADER, bV5RedMask):
		*count = prop_RedMask;
		break;
	case offsetof(BITMAPV5HEADER, bV5GreenMask):
		*count = prop_GreenMask;
		break;
	case offsetof(BITMAPV5HEADER, bV5BlueMask):
		*count = prop_BlueMask;
		break;
	case offsetof(BITMAPV5HEADER, bV5AlphaMask):
		*count = prop_AlphaMask;
		break;
	case offsetof(BITMAPV5HEADER, bV5CSType):
		*count = prop_CSType;
		break;
	case offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzRed.ciexyzX):
		*count = prop_Endpoints_ciexyzRed_ciexyzX;
		break;
	case offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzRed.ciexyzY):
		*count = prop_Endpoints_ciexyzRed_ciexyzY;
		break;
	case offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzRed.ciexyzZ):
		*count = prop_Endpoints_ciexyzRed_ciexyzZ;
		break;
	case offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzGreen.ciexyzX):
		*count = prop_Endpoints_ciexyzGreen_ciexyzX;
		break;
	case offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzGreen.ciexyzY):
		*count = prop_Endpoints_ciexyzGreen_ciexyzY;
		break;
	case offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzGreen.ciexyzZ):
		*count = prop_Endpoints_ciexyzGreen_ciexyzZ;
		break;
	case offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzBlue.ciexyzX):
		*count = prop_Endpoints_ciexyzBlue_ciexyzX;
		break;
	case offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzBlue.ciexyzY):
		*count = prop_Endpoints_ciexyzBlue_ciexyzY;
		break;
	case offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzBlue.ciexyzZ):
		*count = prop_Endpoints_ciexyzBlue_ciexyzZ;
		break;
	case offsetof(BITMAPV5HEADER, bV5GammaRed):
		*count = prop_GammaRed;
		break;
	case offsetof(BITMAPV5HEADER, bV5GammaGreen):
		*count = prop_GammaGreen;
		break;
	case offsetof(BITMAPV5HEADER, bV5GammaBlue):
		*count = prop_GammaBlue;
		break;
	case offsetof(BITMAPV5HEADER, bV5Intent):
		*count = prop_Intent;
		break;
	case offsetof(BITMAPV5HEADER, bV5ProfileData):
		*count = prop_ProfileData;
		break;
	case offsetof(BITMAPV5HEADER, bV5ProfileSize):
		*count = prop_ProfileSize;
		break;
	case offsetof(BITMAPV5HEADER, bV5Reserved):
		*count = prop_Reserved;
		break;
	default:
		if (sizeof(BITMAPV5HEADER) <= size) {
			*count = prop_last;
		}
		else {
			*count = prop_offset;
		}
	}

	return S_OK;
}

HRESULT item_BITMAPV5::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_Size: name = L"Size"; break;
	case prop_Width: name = L"Width"; break;
	case prop_Height: name = L"Height"; break;
	case prop_Planes: name = L"Planes"; break;
	case prop_BitCount: name = L"BitCount"; break;
	case prop_Compression: name = L"Compression"; break;
	case prop_SizeImage: name = L"SizeImage"; break;
	case prop_XPelsPerMeter: name = L"XPelsPerMeter"; break;
	case prop_YPelsPerMeter: name = L"YPelsPerMeter"; break;
	case prop_ClrUsed: name = L"ClrUsed"; break;
	case prop_ClrImportant: name = L"ClrImportant"; break;
	case prop_RedMask: name = L"RedMask"; break;
	case prop_GreenMask: name = L"GreenMask"; break;
	case prop_BlueMask: name = L"BlueMask"; break;
	case prop_AlphaMask: name = L"AlphaMask"; break;
	case prop_CSType: name = L"CSType"; break;
	case prop_Endpoints_ciexyzRed_ciexyzX: name = L"Endpoints.ciexyzRed.ciexyzX"; break;
	case prop_Endpoints_ciexyzRed_ciexyzY: name = L"Endpoints.ciexyzRed.ciexyzY"; break;
	case prop_Endpoints_ciexyzRed_ciexyzZ: name = L"Endpoints.ciexyzRed.ciexyzZ"; break;
	case prop_Endpoints_ciexyzGreen_ciexyzX: name = L"Endpoints.ciexyzGreen.ciexyzX"; break;
	case prop_Endpoints_ciexyzGreen_ciexyzY: name = L"Endpoints.ciexyzGreen.ciexyzY"; break;
	case prop_Endpoints_ciexyzGreen_ciexyzZ: name = L"Endpoints.ciexyzGreen.ciexyzZ"; break;
	case prop_Endpoints_ciexyzBlue_ciexyzX: name = L"Endpoints.ciexyzBlue.ciexyzX"; break;
	case prop_Endpoints_ciexyzBlue_ciexyzY: name = L"Endpoints.ciexyzBlue.ciexyzY"; break;
	case prop_Endpoints_ciexyzBlue_ciexyzZ: name = L"Endpoints.ciexyzBlue.ciexyzZ"; break;
	case prop_GammaRed: name = L"GammaRed"; break;
	case prop_GammaGreen: name = L"GammaGreen"; break;
	case prop_GammaBlue: name = L"GammaBlue"; break;
	case prop_Intent: name = L"Intent"; break;
	case prop_ProfileData: name = L"ProfileData"; break;
	case prop_ProfileSize: name = L"ProfileSize"; break;
	case prop_Reserved: name = L"Reserved"; break;
	default:
		return __super::getPropName(index, str);
	};

	*str = agaliaString::create(name);
	return S_OK;
}

HRESULT item_BITMAPV5::getPropValue(uint32_t index, agaliaString** str) const
{
	BITMAPV5HEADER bv5 = {};
	std::wstringstream temp;
	if (index == prop_Size)
	{
		auto hr = image->ReadData(&bv5.bV5Size, getOffset() + offsetof(BITMAPV5HEADER, bV5Size), sizeof(bv5.bV5Size));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5Size);
	}
	else if (index == prop_Width)
	{
		auto hr = image->ReadData(&bv5.bV5Width, getOffset() + offsetof(BITMAPV5HEADER, bV5Width), sizeof(bv5.bV5Width));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5Width);
	}
	else if (index == prop_Height)
	{
		auto hr = image->ReadData(&bv5.bV5Height, getOffset() + offsetof(BITMAPV5HEADER, bV5Height), sizeof(bv5.bV5Height));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5Height);
	}
	else if (index == prop_Planes)
	{
		auto hr = image->ReadData(&bv5.bV5Planes, getOffset() + offsetof(BITMAPV5HEADER, bV5Planes), sizeof(bv5.bV5Planes));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5Planes);
	}
	else if (index == prop_BitCount)
	{
		auto hr = image->ReadData(&bv5.bV5BitCount, getOffset() + offsetof(BITMAPV5HEADER, bV5BitCount), sizeof(bv5.bV5BitCount));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5BitCount);
	}
	else if (index == prop_Compression)
	{
		auto hr = image->ReadData(&bv5.bV5Compression, getOffset() + offsetof(BITMAPV5HEADER, bV5Compression), sizeof(bv5.bV5Compression));
		if (FAILED(hr)) return hr;
		bool k = true;
		switch (bv5.bV5Compression)
		{
		case BI_RGB:		temp << L"BI_RGB"; break;
		case BI_RLE8:		temp << L"BI_RLE8"; break;
		case BI_RLE4:		temp << L"BI_RLE4"; break;
		case BI_BITFIELDS:	temp << L"BI_BITFIELDS"; break;
		case BI_JPEG:		temp << L"BI_JPEG"; break;
		case BI_PNG:		temp << L"BI_PNG"; break;
		default:
			k = false;
		}
		if (k) temp << L" (";
		format_dec(temp, bv5.bV5Compression);
		if (k) temp << L")";
	}
	else if (index == prop_SizeImage)
	{
		auto hr = image->ReadData(&bv5.bV5SizeImage, getOffset() + offsetof(BITMAPV5HEADER, bV5SizeImage), sizeof(bv5.bV5SizeImage));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5SizeImage);
	}
	else if (index == prop_XPelsPerMeter)
	{
		auto hr = image->ReadData(&bv5.bV5XPelsPerMeter, getOffset() + offsetof(BITMAPV5HEADER, bV5XPelsPerMeter), sizeof(bv5.bV5XPelsPerMeter));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5XPelsPerMeter);
	}
	else if (index == prop_YPelsPerMeter)
	{
		auto hr = image->ReadData(&bv5.bV5YPelsPerMeter, getOffset() + offsetof(BITMAPV5HEADER, bV5YPelsPerMeter), sizeof(bv5.bV5YPelsPerMeter));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5YPelsPerMeter);
	}
	else if (index == prop_ClrUsed)
	{
		auto hr = image->ReadData(&bv5.bV5ClrUsed, getOffset() + offsetof(BITMAPV5HEADER, bV5ClrUsed), sizeof(bv5.bV5ClrUsed));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5ClrUsed);
	}
	else if (index == prop_ClrImportant)
	{
		auto hr = image->ReadData(&bv5.bV5ClrImportant, getOffset() + offsetof(BITMAPV5HEADER, bV5ClrImportant), sizeof(bv5.bV5ClrImportant));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5ClrImportant);
	}
	else if (index == prop_RedMask)
	{
		auto hr = image->ReadData(&bv5.bV5RedMask, getOffset() + offsetof(BITMAPV5HEADER, bV5RedMask), sizeof(bv5.bV5RedMask));
		if (FAILED(hr)) return hr;
		temp << L"0x";
		format_hex(temp, bv5.bV5RedMask, 8);
	}
	else if (index == prop_GreenMask)
	{
		auto hr = image->ReadData(&bv5.bV5GreenMask, getOffset() + offsetof(BITMAPV5HEADER, bV5GreenMask), sizeof(bv5.bV5GreenMask));
		if (FAILED(hr)) return hr;
		temp << L"0x";
		format_hex(temp, bv5.bV5GreenMask, 8);
	}
	else if (index == prop_BlueMask)
	{
		auto hr = image->ReadData(&bv5.bV5BlueMask, getOffset() + offsetof(BITMAPV5HEADER, bV5BlueMask), sizeof(bv5.bV5BlueMask));
		if (FAILED(hr)) return hr;
		temp << L"0x";
		format_hex(temp, bv5.bV5BlueMask, 8);
	}
	else if (index == prop_AlphaMask)
	{
		auto hr = image->ReadData(&bv5.bV5AlphaMask, getOffset() + offsetof(BITMAPV5HEADER, bV5AlphaMask), sizeof(bv5.bV5AlphaMask));
		if (FAILED(hr)) return hr;
		temp << L"0x";
		format_hex(temp, bv5.bV5AlphaMask, 8);
	}
	else if (index == prop_CSType)
	{
		auto hr = image->ReadData(&bv5.bV5CSType, getOffset() + offsetof(BITMAPV5HEADER, bV5CSType), sizeof(bv5.bV5CSType));
		if (FAILED(hr)) return hr;
		bool k = true;
		switch (bv5.bV5CSType)
		{
		case LCS_CALIBRATED_RGB:		temp << L"LCS_CALIBRATED_RGB"; break;
		case LCS_sRGB:					temp << L"LCS_sRGB"; break;
		case LCS_WINDOWS_COLOR_SPACE:	temp << L"LCS_WINDOWS_COLOR_SPACE"; break;
		case PROFILE_LINKED:			temp << L"PROFILE_LINKED"; break;
		case PROFILE_EMBEDDED:			temp << L"PROFILE_EMBEDDED"; break;
		default:
			k = false;
		}
		if (k) {
			temp << L" (0x";
		}
		format_hex(temp, bv5.bV5CSType);
		if (k) {
			temp << L")";
		}
	}
	else if (index == prop_Endpoints_ciexyzRed_ciexyzX)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzRed.ciexyzX, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzRed.ciexyzX), sizeof(bv5.bV5Endpoints.ciexyzRed.ciexyzX));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5Endpoints.ciexyzRed.ciexyzX);
	}
	else if (index == prop_Endpoints_ciexyzRed_ciexyzY)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzRed.ciexyzY, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzRed.ciexyzY), sizeof(bv5.bV5Endpoints.ciexyzRed.ciexyzY));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5Endpoints.ciexyzRed.ciexyzY);
	}
	else if (index == prop_Endpoints_ciexyzRed_ciexyzZ)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzRed.ciexyzZ, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzRed.ciexyzZ), sizeof(bv5.bV5Endpoints.ciexyzRed.ciexyzZ));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5Endpoints.ciexyzRed.ciexyzZ);
	}
	else if (index == prop_Endpoints_ciexyzGreen_ciexyzX)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzGreen.ciexyzX, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzGreen.ciexyzX), sizeof(bv5.bV5Endpoints.ciexyzGreen.ciexyzX));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5Endpoints.ciexyzGreen.ciexyzX);
	}
	else if (index == prop_Endpoints_ciexyzGreen_ciexyzY)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzGreen.ciexyzY, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzGreen.ciexyzY), sizeof(bv5.bV5Endpoints.ciexyzGreen.ciexyzY));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5Endpoints.ciexyzGreen.ciexyzY);
	}
	else if (index == prop_Endpoints_ciexyzGreen_ciexyzZ)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzGreen.ciexyzZ, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzGreen.ciexyzZ), sizeof(bv5.bV5Endpoints.ciexyzGreen.ciexyzZ));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5Endpoints.ciexyzGreen.ciexyzZ);
	}
	else if (index == prop_Endpoints_ciexyzBlue_ciexyzX)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzBlue.ciexyzX, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzBlue.ciexyzX), sizeof(bv5.bV5Endpoints.ciexyzBlue.ciexyzX));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5Endpoints.ciexyzBlue.ciexyzX);
	}
	else if (index == prop_Endpoints_ciexyzBlue_ciexyzY)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzBlue.ciexyzY, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzBlue.ciexyzY), sizeof(bv5.bV5Endpoints.ciexyzBlue.ciexyzY));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5Endpoints.ciexyzBlue.ciexyzY);
	}
	else if (index == prop_Endpoints_ciexyzBlue_ciexyzZ)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzBlue.ciexyzZ, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzBlue.ciexyzZ), sizeof(bv5.bV5Endpoints.ciexyzBlue.ciexyzZ));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5Endpoints.ciexyzBlue.ciexyzZ);
	}
	else if (index == prop_GammaRed)
	{
		auto hr = image->ReadData(&bv5.bV5GammaRed, getOffset() + offsetof(BITMAPV5HEADER, bV5GammaRed), sizeof(bv5.bV5GammaRed));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5GammaRed);
	}
	else if (index == prop_GammaGreen)
	{
		auto hr = image->ReadData(&bv5.bV5GammaGreen, getOffset() + offsetof(BITMAPV5HEADER, bV5GammaGreen), sizeof(bv5.bV5GammaGreen));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5GammaGreen);
	}
	else if (index == prop_GammaBlue)
	{
		auto hr = image->ReadData(&bv5.bV5GammaBlue, getOffset() + offsetof(BITMAPV5HEADER, bV5GammaBlue), sizeof(bv5.bV5GammaBlue));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5GammaBlue);
	}
	else if (index == prop_Intent)
	{
		auto hr = image->ReadData(&bv5.bV5Intent, getOffset() + offsetof(BITMAPV5HEADER, bV5Intent), sizeof(bv5.bV5Intent));
		if (FAILED(hr)) return hr;
		bool k = true;
		switch (bv5.bV5Intent)
		{
		case LCS_GM_ABS_COLORIMETRIC:	temp << L"LCS_GM_ABS_COLORIMETRIC"; break;
		case LCS_GM_BUSINESS:			temp << L"LCS_GM_BUSINESS"; break;
		case LCS_GM_GRAPHICS:			temp << L"LCS_GM_GRAPHICS"; break;
		case LCS_GM_IMAGES:				temp << L"LCS_GM_IMAGES"; break;
		}
		if (k) temp << L" (";
		format_dec(temp, bv5.bV5Intent);
		if (k) temp << L")";
	}
	else if (index == prop_ProfileData)
	{
		auto hr = image->ReadData(&bv5.bV5ProfileData, getOffset() + offsetof(BITMAPV5HEADER, bV5ProfileData), sizeof(bv5.bV5ProfileData));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5ProfileData);
	}
	else if (index == prop_ProfileSize)
	{
		auto hr = image->ReadData(&bv5.bV5ProfileSize, getOffset() + offsetof(BITMAPV5HEADER, bV5ProfileSize), sizeof(bv5.bV5ProfileSize));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5ProfileSize);
	}
	else if (index == prop_Reserved)
	{
		auto hr = image->ReadData(&bv5.bV5Reserved, getOffset() + offsetof(BITMAPV5HEADER, bV5Reserved), sizeof(bv5.bV5Reserved));
		if (FAILED(hr)) return hr;
		format_dec(temp, bv5.bV5Reserved);
	}
	else
	{
		return __super::getPropValue(index, str);
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}


HRESULT item_BITMAPV5::getChild(uint32_t sibling, agaliaElement** child) const
{
	if (sibling != 0) return E_FAIL;
	if (child == nullptr) return E_POINTER;

	if (getSize() < sizeof(BITMAPV5HEADER))
		return E_FAIL;

	BITMAPV5HEADER bv5 = {};
	auto hr = image->ReadData(&bv5, getOffset(), sizeof(bv5));
	if (FAILED(hr)) return hr;

	if (bv5.bV5CSType == PROFILE_EMBEDDED) {
		auto p = new item_EmbeddedProfile(image, getOffset() + bv5.bV5ProfileData, bv5.bV5ProfileSize);
		*child = p;
		return S_OK;
	}
	else if (bv5.bV5CSType == PROFILE_LINKED) {
		auto p = new item_LinkedProfile(image, getOffset() + bv5.bV5ProfileData, bv5.bV5ProfileSize);
		*child = p;
		return S_OK;
	}
	return E_FAIL;
}


HRESULT item_BITMAPV5::getNext(agaliaElement** next) const
{
	BITMAPINFOHEADER bih = {};
	auto hr = image->ReadData(&bih, getOffset(), sizeof(bih));
	if (FAILED(hr)) return hr;

	LONG scans = abs(bih.biHeight);
	LONG bytes_of_line = (bih.biBitCount * bih.biWidth / 8 + 3) & ~3;
	LONG size_image = bih.biSizeImage ? bih.biSizeImage : (bytes_of_line * scans);
	DWORD biCompression = bih.biCompression;

	if (bih.biSize == sizeof(BITMAPINFOHEADER))
	{
		if (bih.biBitCount == 16 || bih.biBitCount == 32)
		{
			if (bih.biCompression == BI_BITFIELDS)
			{
				auto p = new item_ColorMask(image, getOffset() + getSize(), sizeof(DWORD) * 3);
				p->bfOffBits = bfOffBits;
				p->scans = scans;
				p->bytes_of_line = bytes_of_line;
				p->biCompression = biCompression;
				p->biSizeImage = size_image;
				*next = p;
				return S_OK;
			}
		}
	}

	int colors = 0;
	if (bih.biClrUsed != 0)
	{
		colors = bih.biClrUsed;
	}
	else
	{
		switch (bih.biBitCount)
		{
		case 1:
		case 4:
		case 8:
			colors = (bih.biClrUsed == 0) ? (0x01 << bih.biBitCount) : bih.biClrUsed;
			break;
		}
	}
	if (colors != 0)
	{
		auto p = new item_RGBQUADParent(image, getOffset() + getSize(), static_cast<uint64_t>(sizeof(RGBQUAD)) * colors);
		p->bfOffBits = bfOffBits;
		p->scans = scans;
		p->bytes_of_line = bytes_of_line;
		p->biCompression = biCompression;
		p->biSizeImage = size_image;
		p->colors = colors;
		*next = p;
		return S_OK;
	}

	auto p = new item_ColorIndexParent(image, bfOffBits, size_image);
	p->bfOffBits = bfOffBits;
	p->scans = scans;
	p->bytes_of_line = bytes_of_line;
	p->biCompression = biCompression;
	p->biSizeImage = size_image;
	*next = p;
	return S_OK;
}

HRESULT item_BITMAPV5::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_offset)
	{
		std::wstringstream temp;
		format_hex(temp, getOffset(), 8);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (column == column_structure)
	{
		return getName(str);
	}
	else if (column == column_value)
	{
		BITMAPV5HEADER bv5 = {};
		auto hr = image->ReadData(&bv5.bV5Size, getOffset() + offsetof(BITMAPV5HEADER, bV5Size), sizeof(bv5.bV5Size));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"Size : ";
		format_dec(temp, bv5.bV5Size);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	return E_INVALIDARG;
}

HRESULT item_BITMAPV5::getAdditionalInfoCount(uint32_t* row) const
{
	uint32_t count = 0;
	auto hr = getPropCount(&count);
	if (FAILED(hr)) return hr;

	*row = count - prop_Width;
	return S_OK;
}

HRESULT item_BITMAPV5::getAdditionalInfoValue(uint32_t row, agaliaString** str) const
{
	BITMAPV5HEADER bv5 = {};
	std::wstringstream temp;

	if (row == addinf_Width)
	{
		auto hr = image->ReadData(&bv5.bV5Width, getOffset() + offsetof(BITMAPV5HEADER, bV5Width), sizeof(bv5.bV5Width));
		if (FAILED(hr)) return hr;
		temp << L"Width : ";
		format_dec(temp, bv5.bV5Width);
	}
	else if (row == addinf_Height)
	{
		auto hr = image->ReadData(&bv5.bV5Height, getOffset() + offsetof(BITMAPV5HEADER, bV5Height), sizeof(bv5.bV5Height));
		if (FAILED(hr)) return hr;
		temp << L"Height : ";
		format_dec(temp, bv5.bV5Height);
	}
	else if (row == addinf_Planes)
	{
		auto hr = image->ReadData(&bv5.bV5Planes, getOffset() + offsetof(BITMAPV5HEADER, bV5Planes), sizeof(bv5.bV5Planes));
		if (FAILED(hr)) return hr;
		temp << L"Planes : ";
		format_dec(temp, bv5.bV5Planes);
	}
	else if (row == addinf_BitCount)
	{
		auto hr = image->ReadData(&bv5.bV5BitCount, getOffset() + offsetof(BITMAPV5HEADER, bV5BitCount), sizeof(bv5.bV5BitCount));
		if (FAILED(hr)) return hr;
		temp << L"BitCount : ";
		format_dec(temp, bv5.bV5BitCount);
	}
	else if (row == addinf_Compression)
	{
		auto hr = image->ReadData(&bv5.bV5Compression, getOffset() + offsetof(BITMAPV5HEADER, bV5Compression), sizeof(bv5.bV5Compression));
		if (FAILED(hr)) return hr;
		temp << L"Compression : ";
		bool k = true;
		switch (bv5.bV5Compression)
		{
		case BI_RGB:		temp << L"BI_RGB"; break;
		case BI_RLE8:		temp << L"BI_RLE8"; break;
		case BI_RLE4:		temp << L"BI_RLE4"; break;
		case BI_BITFIELDS:	temp << L"BI_BITFIELDS"; break;
		case BI_JPEG:		temp << L"BI_JPEG"; break;
		case BI_PNG:		temp << L"BI_PNG"; break;
		default:
			k = false;
		}
		if (k) temp << L" (";
		format_dec(temp, bv5.bV5Compression);
		if (k) temp << L")";
	}
	else if (row == addinf_SizeImage)
	{
		auto hr = image->ReadData(&bv5.bV5SizeImage, getOffset() + offsetof(BITMAPV5HEADER, bV5SizeImage), sizeof(bv5.bV5SizeImage));
		if (FAILED(hr)) return hr;
		temp << L"SizeImage : ";
		format_dec(temp, bv5.bV5SizeImage);
	}
	else if (row == addinf_XPelsPerMeter)
	{
		auto hr = image->ReadData(&bv5.bV5XPelsPerMeter, getOffset() + offsetof(BITMAPV5HEADER, bV5XPelsPerMeter), sizeof(bv5.bV5XPelsPerMeter));
		if (FAILED(hr)) return hr;
		temp << L"XPelsPerMeter : ";
		format_dec(temp, bv5.bV5XPelsPerMeter);
	}
	else if (row == addinf_YPelsPerMeter)
	{
		auto hr = image->ReadData(&bv5.bV5YPelsPerMeter, getOffset() + offsetof(BITMAPV5HEADER, bV5YPelsPerMeter), sizeof(bv5.bV5YPelsPerMeter));
		if (FAILED(hr)) return hr;
		temp << L"YPelsPerMeter : ";
		format_dec(temp, bv5.bV5YPelsPerMeter);
	}
	else if (row == addinf_ClrUsed)
	{
		auto hr = image->ReadData(&bv5.bV5ClrUsed, getOffset() + offsetof(BITMAPV5HEADER, bV5ClrUsed), sizeof(bv5.bV5ClrUsed));
		if (FAILED(hr)) return hr;
		temp << L"ClrUsed : ";
		format_dec(temp, bv5.bV5ClrUsed);
	}
	else if (row == addinf_ClrImportant)
	{
		auto hr = image->ReadData(&bv5.bV5ClrImportant, getOffset() + offsetof(BITMAPV5HEADER, bV5ClrImportant), sizeof(bv5.bV5ClrImportant));
		if (FAILED(hr)) return hr;
		temp << L"ClrImportant : ";
		format_dec(temp, bv5.bV5ClrImportant);
	}
	else if (row == addinf_RedMask)
	{
		auto hr = image->ReadData(&bv5.bV5RedMask, getOffset() + offsetof(BITMAPV5HEADER, bV5RedMask), sizeof(bv5.bV5RedMask));
		if (FAILED(hr)) return hr;
		temp << L"RedMask : 0x";
		format_hex(temp, bv5.bV5RedMask, 8);
	}
	else if (row == addinf_GreenMask)
	{
		auto hr = image->ReadData(&bv5.bV5GreenMask, getOffset() + offsetof(BITMAPV5HEADER, bV5GreenMask), sizeof(bv5.bV5GreenMask));
		if (FAILED(hr)) return hr;
		temp << L"GreenMask : 0x";
		format_hex(temp, bv5.bV5GreenMask, 8);
	}
	else if (row == addinf_BlueMask)
	{
		auto hr = image->ReadData(&bv5.bV5BlueMask, getOffset() + offsetof(BITMAPV5HEADER, bV5BlueMask), sizeof(bv5.bV5BlueMask));
		if (FAILED(hr)) return hr;
		temp << L"BlueMask : 0x";
		format_hex(temp, bv5.bV5BlueMask, 8);
	}
	else if (row == addinf_AlphaMask)
	{
		auto hr = image->ReadData(&bv5.bV5AlphaMask, getOffset() + offsetof(BITMAPV5HEADER, bV5AlphaMask), sizeof(bv5.bV5AlphaMask));
		if (FAILED(hr)) return hr;
		temp << L"AlphaMask : 0x";
		format_hex(temp, bv5.bV5AlphaMask, 8);
	}
	else if (row == addinf_CSType)
	{
		auto hr = image->ReadData(&bv5.bV5CSType, getOffset() + offsetof(BITMAPV5HEADER, bV5CSType), sizeof(bv5.bV5CSType));
		if (FAILED(hr)) return hr;
		temp << L"CSType : ";
		bool k = true;
		switch (bv5.bV5CSType)
		{
		case LCS_CALIBRATED_RGB:		temp << L"LCS_CALIBRATED_RGB"; break;
		case LCS_sRGB:					temp << L"LCS_sRGB"; break;
		case LCS_WINDOWS_COLOR_SPACE:	temp << L"LCS_WINDOWS_COLOR_SPACE"; break;
		case PROFILE_LINKED:			temp << L"PROFILE_LINKED"; break;
		case PROFILE_EMBEDDED:			temp << L"PROFILE_EMBEDDED"; break;
		default:
			k = false;
		}
		if (k) {
			temp << L" (0x";
		}
		format_hex(temp, bv5.bV5CSType);
		if (k) {
			temp << L")";
		}
	}
	else if (row == addinf_Endpoints_ciexyzRed_ciexyzX)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzRed.ciexyzX, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzRed.ciexyzX), sizeof(bv5.bV5Endpoints.ciexyzRed.ciexyzX));
		if (FAILED(hr)) return hr;
		temp << L"Endpoints.ciexyzRed.ciexyzX : ";
		format_dec(temp, bv5.bV5Endpoints.ciexyzRed.ciexyzX);
	}
	else if (row == addinf_Endpoints_ciexyzRed_ciexyzY)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzRed.ciexyzY, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzRed.ciexyzY), sizeof(bv5.bV5Endpoints.ciexyzRed.ciexyzY));
		if (FAILED(hr)) return hr;
		temp << L"Endpoints.ciexyzRed.ciexyzY : ";
		format_dec(temp, bv5.bV5Endpoints.ciexyzRed.ciexyzY);
	}
	else if (row == addinf_Endpoints_ciexyzRed_ciexyzZ)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzRed.ciexyzZ, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzRed.ciexyzZ), sizeof(bv5.bV5Endpoints.ciexyzRed.ciexyzZ));
		if (FAILED(hr)) return hr;
		temp << L"Endpoints.ciexyzRed.ciexyzZ : ";
		format_dec(temp, bv5.bV5Endpoints.ciexyzRed.ciexyzZ);
	}
	else if (row == addinf_Endpoints_ciexyzGreen_ciexyzX)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzGreen.ciexyzX, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzGreen.ciexyzX), sizeof(bv5.bV5Endpoints.ciexyzGreen.ciexyzX));
		if (FAILED(hr)) return hr;
		temp << L"Endpoints.ciexyzGreen.ciexyzX : ";
		format_dec(temp, bv5.bV5Endpoints.ciexyzGreen.ciexyzX);
	}
	else if (row == addinf_Endpoints_ciexyzGreen_ciexyzY)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzGreen.ciexyzY, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzGreen.ciexyzY), sizeof(bv5.bV5Endpoints.ciexyzGreen.ciexyzY));
		if (FAILED(hr)) return hr;
		temp << L"Endpoints.ciexyzGreen.ciexyzY : ";
		format_dec(temp, bv5.bV5Endpoints.ciexyzGreen.ciexyzY);
	}
	else if (row == addinf_Endpoints_ciexyzGreen_ciexyzZ)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzGreen.ciexyzZ, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzGreen.ciexyzZ), sizeof(bv5.bV5Endpoints.ciexyzGreen.ciexyzZ));
		if (FAILED(hr)) return hr;
		temp << L"Endpoints.ciexyzGreen.ciexyzZ : ";
		format_dec(temp, bv5.bV5Endpoints.ciexyzGreen.ciexyzZ);
	}
	else if (row == addinf_Endpoints_ciexyzBlue_ciexyzX)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzBlue.ciexyzX, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzBlue.ciexyzX), sizeof(bv5.bV5Endpoints.ciexyzBlue.ciexyzX));
		if (FAILED(hr)) return hr;
		temp << L"Endpoints.ciexyzBlue.ciexyzX : ";
		format_dec(temp, bv5.bV5Endpoints.ciexyzBlue.ciexyzX);
	}
	else if (row == addinf_Endpoints_ciexyzBlue_ciexyzY)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzBlue.ciexyzY, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzBlue.ciexyzY), sizeof(bv5.bV5Endpoints.ciexyzBlue.ciexyzY));
		if (FAILED(hr)) return hr;
		temp << L"Endpoints.ciexyzBlue.ciexyzY : ";
		format_dec(temp, bv5.bV5Endpoints.ciexyzBlue.ciexyzY);
	}
	else if (row == addinf_Endpoints_ciexyzBlue_ciexyzZ)
	{
		auto hr = image->ReadData(&bv5.bV5Endpoints.ciexyzBlue.ciexyzZ, getOffset() + offsetof(BITMAPV5HEADER, bV5Endpoints.ciexyzBlue.ciexyzZ), sizeof(bv5.bV5Endpoints.ciexyzBlue.ciexyzZ));
		if (FAILED(hr)) return hr;
		temp << L"Endpoints.ciexyzBlue.ciexyzZ : ";
		format_dec(temp, bv5.bV5Endpoints.ciexyzBlue.ciexyzZ);
	}
	else if (row == addinf_GammaRed)
	{
		auto hr = image->ReadData(&bv5.bV5GammaRed, getOffset() + offsetof(BITMAPV5HEADER, bV5GammaRed), sizeof(bv5.bV5GammaRed));
		if (FAILED(hr)) return hr;
		temp << L"GammaRed : ";
		format_dec(temp, bv5.bV5GammaRed);
	}
	else if (row == addinf_GammaGreen)
	{
		auto hr = image->ReadData(&bv5.bV5GammaGreen, getOffset() + offsetof(BITMAPV5HEADER, bV5GammaGreen), sizeof(bv5.bV5GammaGreen));
		if (FAILED(hr)) return hr;
		temp << L"GammaGreen : ";
		format_dec(temp, bv5.bV5GammaGreen);
	}
	else if (row == addinf_GammaBlue)
	{
		auto hr = image->ReadData(&bv5.bV5GammaBlue, getOffset() + offsetof(BITMAPV5HEADER, bV5GammaBlue), sizeof(bv5.bV5GammaBlue));
		if (FAILED(hr)) return hr;
		temp << L"GammaBlue : ";
		format_dec(temp, bv5.bV5GammaBlue);
	}
	else if (row == addinf_Intent)
	{
		auto hr = image->ReadData(&bv5.bV5Intent, getOffset() + offsetof(BITMAPV5HEADER, bV5Intent), sizeof(bv5.bV5Intent));
		if (FAILED(hr)) return hr;
		temp << L"Intent : ";
		bool k = true;
		switch (bv5.bV5Intent)
		{
		case LCS_GM_ABS_COLORIMETRIC:	temp << L"LCS_GM_ABS_COLORIMETRIC"; break;
		case LCS_GM_BUSINESS:			temp << L"LCS_GM_BUSINESS"; break;
		case LCS_GM_GRAPHICS:			temp << L"LCS_GM_GRAPHICS"; break;
		case LCS_GM_IMAGES:				temp << L"LCS_GM_IMAGES"; break;
		}
		if (k) temp << L" (";
		format_dec(temp, bv5.bV5Intent);
		if (k) temp << L")";
	}
	else if (row == addinf_ProfileData)
	{
		auto hr = image->ReadData(&bv5.bV5ProfileData, getOffset() + offsetof(BITMAPV5HEADER, bV5ProfileData), sizeof(bv5.bV5ProfileData));
		if (FAILED(hr)) return hr;
		temp << L"ProfileData : ";
		format_dec(temp, bv5.bV5ProfileData);
	}
	else if (row == addinf_ProfileSize)
	{
		auto hr = image->ReadData(&bv5.bV5ProfileSize, getOffset() + offsetof(BITMAPV5HEADER, bV5ProfileSize), sizeof(bv5.bV5ProfileSize));
		if (FAILED(hr)) return hr;
		temp << L"ProfileSize : ";
		format_dec(temp, bv5.bV5ProfileSize);
	}
	else if (row == addinf_Reserved)
	{
		auto hr = image->ReadData(&bv5.bV5Reserved, getOffset() + offsetof(BITMAPV5HEADER, bV5Reserved), sizeof(bv5.bV5Reserved));
		if (FAILED(hr)) return hr;
		temp << L"Reserved : ";
		format_dec(temp, bv5.bV5Reserved);
	}
	else
	{
		return E_INVALIDARG;
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
