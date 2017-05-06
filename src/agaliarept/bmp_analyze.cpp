#include "stdafx.h"
#include "../inc/agaliarept.h"
#include "callback.h"

// biCompression の出力 
void print_Compresssion(const wchar_t* label, DWORD value, int subItem, callback_cls* callback)
{
	std::wstringbuf buf;
	std::wostream str(&buf, true);
	str << label;
	switch (value)
	{
	case BI_RGB:
		str << L"BI_RGB (" << value << L")";
		break;
	case BI_RLE8:
		str << L"BI_RLE8 (" << value << L")";
		break;
	case BI_RLE4:
		str << L"BI_RLE4 (" << value << L")";
		break;
	case BI_BITFIELDS:
		str << L"BI_BITFIELDS (" << value << L")";
		break;
	case BI_JPEG:
		str << L"BI_JPEG (" << value << L")";
		break;
	case BI_PNG:
		str << L"BI_PNG (" << value << L")";
		break;
	default:
		str << value;
	}
	callback->print(subItem, buf.str());
}

// BITMAPFILEHEADER の解析
bool parseBmpFileHeader(_In_ const uint8_t* base, _In_ uint64_t offset, _In_ uint64_t endPos, callback_cls* callback)
{
	// offset 
	int subItem = 0;
	{
		wchar_t sz[16] = {};
		swprintf_s(sz, L"%.8I64X", offset);
		callback->print(subItem, sz);
	}

	// BITMAPFILEHEADER 
	subItem++;
	callback->print(subItem, L"BITMAPFILEHEADER");

	if (endPos < offset + sizeof(BITMAPFILEHEADER)) return false;

	const BITMAPFILEHEADER* bmpFile = reinterpret_cast<const BITMAPFILEHEADER*>(base + offset);

	subItem++;
	{
		char buf[3] = {};
		memcpy(buf, base, 2);
		std::string str;
		str = "bfType:";
		str += buf;
		callback->print(subItem, str);
	}
	callback->next();

	print_value(L"bfSize", bmpFile->bfSize, subItem, callback);
	callback->next();

	print_value(L"bfReserved1", bmpFile->bfReserved1, subItem, callback);
	callback->next();

	print_value(L"bfReserved2", bmpFile->bfReserved2, subItem, callback);
	callback->next();

	print_value(L"bfOffBits", bmpFile->bfOffBits, subItem, callback);
	callback->next();

	return true;
}

// BITMAPINFO のカレーパレットの解析 
bool parseBmpInfoColors(_In_ const uint8_t* base, _In_ uint64_t offset, _In_ uint64_t endPos, callback_cls* callback, int Colors)
{
	// offset 
	int subItem = 0;
	{
		wchar_t sz[16] = {};
		swprintf_s(sz, L"%.8I64X", offset);
		callback->print(subItem, sz);
	}

	// BITMAPINFO 
	subItem++;
	callback->print(subItem, L"BITMAPINFO");

	const RGBQUAD* colors = reinterpret_cast<const RGBQUAD*>(base + offset);

	subItem++;
	for (int i = 0; i < Colors; i++)
	{
		if (endPos <= offset + sizeof(RGBQUAD) * i) {
			break;
		}
		std::wstringbuf buf;
		std::wostream str(&buf, true);
		str << L"bmiColors[" << i << L"]:";
		str << L"rgbBlue=" << colors[i].rgbBlue;
		str << L", rgbGreen=" << colors[i].rgbGreen;
		str << L", rgbRed=" << colors[i].rgbRed;
		str << L", rgbReserved=" << colors[i].rgbReserved;
		callback->print(subItem, buf.str());
		callback->next();
	}

	return true;
}

// BITMAPCOREHEADER の解析 
bool parseBmpCoreHeader(_In_ const uint8_t* base, _In_ uint64_t offset, _In_ uint64_t endPos, callback_cls* callback)
{
	// offset 
	int subItem = 0;
	{
		wchar_t sz[16] = {};
		swprintf_s(sz, L"%.8I64X", offset);
		callback->print(subItem, sz);
	}

	// BITMAPCOREHEADER 
	subItem++;
	callback->print(subItem, L"BITMAPCOREHEADER");

	if (endPos < offset + sizeof(BITMAPCOREHEADER)) {
		callback->next();
		return false;
	}

	const BITMAPCOREHEADER* bmpInfo = reinterpret_cast<const BITMAPCOREHEADER*>(base + offset);

	subItem++;

	print_value(L"bcSize", bmpInfo->bcSize, subItem, callback);
	callback->next();

	print_value(L"bcWidth", bmpInfo->bcWidth, subItem, callback);
	callback->next();

	print_value(L"bcHeight", bmpInfo->bcHeight, subItem, callback);
	callback->next();

	print_value(L"bcPlanes", bmpInfo->bcPlanes, subItem, callback);
	callback->next();

	print_value(L"bcBitCount", bmpInfo->bcBitCount, subItem, callback);
	callback->next();

	return true;
}

// BITMAPINFOHEADERの解析 
void parseInfoHeader(_In_ const uint8_t* base, _In_ uint64_t offset, _In_ uint64_t endPos, callback_cls* callback)
{
	// offset 
	int subItem = 0;
	{
		wchar_t sz[16] = {};
		swprintf_s(sz, L"%.8I64X", offset);
		callback->print(subItem, sz);
	}

	subItem++;
	callback->print(subItem, L"BITMAPINFOHEADER");

	const BITMAPV5HEADER* bmpInfo = reinterpret_cast<const BITMAPV5HEADER*>(base + offset);

	subItem++;

	uint32_t itemSize = 0;
	uint64_t pos = 0;

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5Size);
	if (endPos < offset + pos + itemSize) return;
	print_value(L"Size", bmpInfo->bV5Size, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5Width);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"Width", bmpInfo->bV5Width, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5Height);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"Height", bmpInfo->bV5Height, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5Planes);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"Planes", bmpInfo->bV5Planes, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5BitCount);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"BitCount", bmpInfo->bV5BitCount, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5Compression);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_Compresssion(L"Compression", bmpInfo->bV5Compression, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5SizeImage);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"SizeImage", bmpInfo->bV5SizeImage, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5XPelsPerMeter);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"XPelsPerMeter", bmpInfo->bV5XPelsPerMeter, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5YPelsPerMeter);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"YPelsPerMeter", bmpInfo->bV5YPelsPerMeter, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5ClrUsed);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"ClrUsed", bmpInfo->bV5ClrUsed, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5ClrImportant);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"ClrImportant", bmpInfo->bV5ClrImportant, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5RedMask);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value_hex(L"RedMask", bmpInfo->bV5RedMask, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5GreenMask);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value_hex(L"GreenMask", bmpInfo->bV5GreenMask, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5BlueMask);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value_hex(L"BlueMask", bmpInfo->bV5BlueMask, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5AlphaMask);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value_hex(L"AlphaMask", bmpInfo->bV5AlphaMask, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5CSType);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"CSType", bmpInfo->bV5CSType, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5Endpoints.ciexyzRed.ciexyzX);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"Endpoints.ciexyzRed.ciexyzX", bmpInfo->bV5Endpoints.ciexyzRed.ciexyzX, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5Endpoints.ciexyzRed.ciexyzY);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"Endpoints.ciexyzRed.ciexyzY", bmpInfo->bV5Endpoints.ciexyzRed.ciexyzY, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5Endpoints.ciexyzRed.ciexyzZ);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"Endpoints.ciexyzRed.ciexyzZ", bmpInfo->bV5Endpoints.ciexyzRed.ciexyzZ, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5Endpoints.ciexyzGreen.ciexyzX);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"Endpoints.ciexyzGreen.ciexyzX", bmpInfo->bV5Endpoints.ciexyzGreen.ciexyzX, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5Endpoints.ciexyzGreen.ciexyzY);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"Endpoints.ciexyzGreen.ciexyzY", bmpInfo->bV5Endpoints.ciexyzGreen.ciexyzY, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5Endpoints.ciexyzGreen.ciexyzZ);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"Endpoints.ciexyzGreen.ciexyzZ", bmpInfo->bV5Endpoints.ciexyzGreen.ciexyzZ, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5Endpoints.ciexyzBlue.ciexyzX);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"Endpoints.ciexyzBlue.ciexyzX", bmpInfo->bV5Endpoints.ciexyzBlue.ciexyzX, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5Endpoints.ciexyzBlue.ciexyzY);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"Endpoints.ciexyzBlue.ciexyzY", bmpInfo->bV5Endpoints.ciexyzBlue.ciexyzY, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5Endpoints.ciexyzBlue.ciexyzZ);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"Endpoints.ciexyzBlue.ciexyzZ", bmpInfo->bV5Endpoints.ciexyzBlue.ciexyzZ, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5GammaRed);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"GammaRed", bmpInfo->bV5GammaRed, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5GammaGreen);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"GammaGreen", bmpInfo->bV5GammaGreen, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5GammaBlue);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"GammaBlue", bmpInfo->bV5GammaBlue, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5Intent);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"Intent", bmpInfo->bV5Intent, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5ProfileData);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"ProfileData", bmpInfo->bV5ProfileData, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5ProfileSize);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"ProfileSize", bmpInfo->bV5ProfileSize, subItem, callback);
	callback->next();

	pos += itemSize;
	itemSize = sizeof(bmpInfo->bV5Reserved);
	if (bmpInfo->bV5Size < pos + itemSize) return;
	if (endPos < offset + pos + itemSize) return;
	print_value(L"Reserved", bmpInfo->bV5Reserved, subItem, callback);
	callback->next();
}

void parseBmp(_In_ const uint8_t* base, _In_ uint64_t size, callback_cls* callback, const agalia::config* config)
{
	uint64_t offset = 0;

	if (!parseBmpFileHeader(base, offset, size, callback)) {
		return;
	}

	offset += sizeof(BITMAPFILEHEADER);

	if (size < offset + sizeof(DWORD)) return;
	DWORD infoSize = *reinterpret_cast<const DWORD*>(base + offset);

	if (infoSize == sizeof(BITMAPCOREHEADER))
	{
		parseBmpCoreHeader(base, offset, size, callback);
		offset += infoSize;
	}
	else if (sizeof(BITMAPINFOHEADER) <= infoSize)
	{
		parseInfoHeader(base, offset, size, callback);

		const BITMAPINFOHEADER* bmpInfo = reinterpret_cast<const BITMAPINFOHEADER*>(base + offset);

		offset += infoSize;

		bool hasMask = false;
		if (bmpInfo->biSize == sizeof(BITMAPINFOHEADER))
		{
			switch (bmpInfo->biBitCount)
			{
			case 16:
			case 32:
				if (bmpInfo->biCompression == BI_BITFIELDS)
				{
					hasMask = true;
				}
				break;
			}
		}

		if (hasMask)
		{
			int subItem = 0;
			{
				wchar_t sz[16] = {};
				swprintf_s(sz, L"%.8I64X", offset);
				callback->print(subItem, sz);
			}

			subItem++;
			callback->print(subItem, L"Color-mask");

			subItem++;
			const DWORD* pal = reinterpret_cast<const DWORD*>(base + offset);
			print_value_hex(L"red", pal[0], subItem, callback);
			callback->next();
			print_value_hex(L"green", pal[1], subItem, callback);
			callback->next();
			print_value_hex(L"blue", pal[2], subItem, callback);
			callback->next();

			offset += (hasMask ? (sizeof(DWORD) * 3) : 0);
		}

		int colors = 0;
		if (bmpInfo->biClrUsed != 0)
		{
			colors = bmpInfo->biClrUsed;
		}
		else
		{
			switch (bmpInfo->biBitCount)
			{
			case 1:
			case 4:
			case 8:
				colors = (bmpInfo->biClrUsed == 0) ? (0x01 << bmpInfo->biBitCount) : bmpInfo->biClrUsed;
				break;
			}
		}
		if (colors) {
			parseBmpInfoColors(base, offset, size, callback, colors);
			offset += sizeof(RGBQUAD) * colors;
		}

		{
			int subItem = 0;
			{
				wchar_t sz[16] = {};
				swprintf_s(sz, L"%.8I64X", offset);
				callback->print(subItem, sz);
			}


			switch (bmpInfo->biCompression)
			{
			case BI_RGB:
			case BI_BITFIELDS:
				void putDataAsByteArray(int subItem, const uint8_t* base_addr, uint64_t offset, uint64_t size, callback_cls* callback, const agalia::config* config);
				{
					size_t scans = abs(bmpInfo->biHeight);
					size_t bytes_of_line = (bmpInfo->biBitCount * bmpInfo->biWidth / 8 + 3) & ~3;
					for (size_t y = 0; y < scans; y++)
					{
						subItem = 1;
						wchar_t sz[32] = {};
						swprintf_s(sz, L"Color-index[%zu]", y);
						callback->print(subItem, sz);

						subItem++;
						putDataAsByteArray(subItem, base, offset + bytes_of_line * y, bytes_of_line, callback, config);
						callback->next();

						config->is_abort(agalia::config::throw_on_abort);
					}
				}
				break;

			default:
				subItem++;
				callback->print(subItem, L"Color-index");

				subItem++;
				putDataAsByteArray(subItem, base, offset, size, callback, config);
			}
		}
	}
}

#include "image_file.h"

bool image_file_bmp::is_supported(const void* buf, uint64_t size)
{
	if (buf == nullptr) return false;
	if (size < sizeof(GUID)) return false;

	bool ret = false;
#pragma warning(suppress: 6276)	// Warning	C6276	Cast between semantically different string types : char * to wchar_t *.Use of invalid string can lead to undefined behavior. 
	if (*reinterpret_cast<const uint16_t*>(buf) == *reinterpret_cast<const uint16_t*>("BM"))
	{
		ret = true;
	}
	return ret;
}

void image_file_bmp::parse(callback_cls* callback, const agalia::config* config)
{
	callback->insert_column(96, L"offset");
	callback->insert_column(192, L"structure");
	callback->insert_column(512, L"value");
	callback->next();

	parseBmp(this->base_addr + this->base_offset, this->data_size, callback, config);
}
