#include "pch.h"
#include "analyze_PNG_item_IHDR.h"

#include "container_PNG.h"
#include "byteswap.h"

using namespace analyze_PNG;

item_IHDR::item_IHDR(const container_PNG* image, uint64_t offset, uint64_t size, uint64_t endpos)
	: item_Base(image, offset, size, endpos)
{
}

item_IHDR::~item_IHDR()
{
}

HRESULT item_IHDR::getItemPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT item_IHDR::getItemPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_width: name = L"Width"; break;
	case prop_height: name = L"Height"; break;
	case prop_bit_depth: name = L"Bit depth"; break;
	case prop_color_type: name = L"Color type"; break;
	case prop_compression_method: name = L"Compression method"; break;
	case prop_filter_method: name = L"Filter method"; break;
	case prop_interlace_method: name = L"Interlace method"; break;
	default:
		return __super::getItemPropName(index, str);
	}

	*str = agaliaString::create(name);
	return S_OK;
}

HRESULT item_IHDR::getItemPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	std::wstringstream temp;
	if (index == prop_width)
	{
		uint32_t length = 0;
		auto hr = image->ReadData(&length, getOffset(), sizeof(length));
		if (FAILED(hr)) return hr;
		length = agalia_byteswap(length);

		if (length < offsetof(IHDR, Width) + sizeof(IHDR::Width))
			return E_FAIL;

		uint64_t pos = getOffset() + offsetof(Chunk, ChunkData) + offsetof(IHDR, Width);

		decltype(IHDR::Width) width;
		hr = image->ReadData(&width, pos, sizeof(width));
		if (FAILED(hr)) return hr;
		width = agalia_byteswap(width);

		format_dec(temp, width);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_height)
	{
		uint32_t length = 0;
		auto hr = image->ReadData(&length, getOffset(), sizeof(length));
		if (FAILED(hr)) return hr;
		length = agalia_byteswap(length);

		if (length < offsetof(IHDR, Height) + sizeof(IHDR::Height))
			return E_FAIL;

		uint64_t pos = getOffset() + offsetof(Chunk, ChunkData) + offsetof(IHDR, Height);

		decltype(IHDR::Height) height;
		hr = image->ReadData(&height, pos, sizeof(height));
		if (FAILED(hr)) return hr;
		height = agalia_byteswap(height);

		format_dec(temp, height);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_bit_depth)
	{
		uint32_t length = 0;
		auto hr = image->ReadData(&length, getOffset(), sizeof(length));
		if (FAILED(hr)) return hr;
		length = agalia_byteswap(length);

		if (length < offsetof(IHDR, BitDepth) + sizeof(IHDR::BitDepth))
			return E_FAIL;

		uint64_t pos = getOffset() + offsetof(Chunk, ChunkData) + offsetof(IHDR, BitDepth);

		decltype(IHDR::BitDepth) bit_depth;
		hr = image->ReadData(&bit_depth, pos, sizeof(bit_depth));
		if (FAILED(hr)) return hr;
		bit_depth = agalia_byteswap(bit_depth);

		format_dec(temp, bit_depth);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_color_type)
	{
		uint32_t length = 0;
		auto hr = image->ReadData(&length, getOffset(), sizeof(length));
		if (FAILED(hr)) return hr;
		length = agalia_byteswap(length);

		if (length < offsetof(IHDR, ColorType) + sizeof(IHDR::ColorType))
			return E_FAIL;

		uint64_t pos = getOffset() + offsetof(Chunk, ChunkData) + offsetof(IHDR, ColorType);

		decltype(IHDR::ColorType) color_type;
		hr = image->ReadData(&color_type, pos, sizeof(color_type));
		if (FAILED(hr)) return hr;
		color_type = agalia_byteswap(color_type);

		const wchar_t* typeName = nullptr;
		switch (color_type)
		{
		case 0: typeName = L"Greyscale"; break;
		case 2: typeName = L"Truecolour"; break;
		case 3: typeName = L"Indexed-colour"; break;
		case 4: typeName = L"Greyscale with alpha"; break;
		case 6: typeName = L"Truecolour with alpha"; break;
		default: typeName = L"Unknown"; break;
		}
		temp << typeName << L" (" << std::dec << color_type << L")";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_compression_method)
	{
		uint32_t length = 0;
		auto hr = image->ReadData(&length, getOffset(), sizeof(length));
		if (FAILED(hr)) return hr;
		length = agalia_byteswap(length);

		if (length < offsetof(IHDR, CompressionMethod) + sizeof(IHDR::CompressionMethod))
			return E_FAIL;

		uint64_t pos = getOffset() + offsetof(Chunk, ChunkData) + offsetof(IHDR, CompressionMethod);

		decltype(IHDR::CompressionMethod) compression_method;
		hr = image->ReadData(&compression_method, pos, sizeof(compression_method));
		if (FAILED(hr)) return hr;
		compression_method = agalia_byteswap(compression_method);

		format_dec(temp, compression_method);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_filter_method)
	{
		uint32_t length = 0;
		auto hr = image->ReadData(&length, getOffset(), sizeof(length));
		if (FAILED(hr)) return hr;
		length = agalia_byteswap(length);

		if (length < offsetof(IHDR, FilterMethod) + sizeof(IHDR::FilterMethod))
			return E_FAIL;

		uint64_t pos = getOffset() + offsetof(Chunk, ChunkData) + offsetof(IHDR, FilterMethod);

		decltype(IHDR::FilterMethod) filter_method;
		hr = image->ReadData(&filter_method, pos, sizeof(filter_method));
		if (FAILED(hr)) return hr;
		filter_method = agalia_byteswap(filter_method);

		format_dec(temp, filter_method);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_interlace_method)
	{
		uint32_t length = 0;
		auto hr = image->ReadData(&length, getOffset(), sizeof(length));
		if (FAILED(hr)) return hr;
		length = agalia_byteswap(length);

		if (length < offsetof(IHDR, InterlaceMethod) + sizeof(IHDR::InterlaceMethod))
			return E_FAIL;

		uint64_t pos = getOffset() + offsetof(Chunk, ChunkData) + offsetof(IHDR, InterlaceMethod);

		decltype(IHDR::InterlaceMethod) interlace_method;
		hr = image->ReadData(&interlace_method, pos, sizeof(interlace_method));
		if (FAILED(hr)) return hr;
		interlace_method = agalia_byteswap(interlace_method);

		format_dec(temp, interlace_method);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else
	{
		return __super::getItemPropValue(index, str);
	}

	return E_FAIL;
}

HRESULT item_IHDR::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_value)
	{
		Chunk chunk;
		auto hr = image->ReadData(&chunk, getOffset(), sizeof(chunk));
		if (FAILED(hr)) return hr;

		if (chunk.getLength() < sizeof(IHDR))
			return E_FAIL;

		IHDR ihdr = {};
		hr = image->ReadData(&ihdr, getOffset() + offsetof(Chunk, ChunkData), sizeof(ihdr));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"Width=";
		format_dec(temp, ihdr.getWidth());
		temp << L", Height=";
		format_dec(temp, ihdr.getHeight());
		temp << L", Bit depth=";
		format_dec(temp, ihdr.getBitDepth());
		temp << L", Color type=";
		format_dec(temp, ihdr.getColorType());
		temp << L", Compression method=";
		format_dec(temp, ihdr.getCompressionMethod());
		temp << L", Filter method=";
		format_dec(temp, ihdr.getFilterMethod());
		temp << L", Interlace method=";
		format_dec(temp, ihdr.getInterlaceMethod());

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	return __super::getColumnValue(column, str);
}
