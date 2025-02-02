#include "pch.h"
#include "analyze_BMP_item_ColorIndexParent.h"

#include "analyze_BMP_item_ColorIndex.h"

using namespace analyze_BMP;

item_ColorIndexParent::item_ColorIndexParent(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:BMP_item_Base(image, offset, size)
{
}

item_ColorIndexParent::~item_ColorIndexParent()
{
}

HRESULT item_ColorIndexParent::getName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	*str = agaliaString::create(L"Color-index");
	return S_OK;
}

HRESULT item_ColorIndexParent::getChild(uint32_t sibling, agaliaElement** child) const
{
	if (sibling != 0) return E_FAIL;

	if (biCompression == BI_RGB || biCompression == BI_BITFIELDS)
	{
		auto* p = new item_ColorIndex(image, getOffset(), bytes_of_line);
		p->scans = scans;
		p->bytes_of_line = bytes_of_line;
		p->index = 0;
		*child = p;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT item_ColorIndexParent::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (biCompression == BI_RGB || biCompression == BI_BITFIELDS)
	{
		return E_FAIL;
	}

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
		CHeapPtr<uint8_t> buf;
		rsize_t bufsize = 0;
		auto hr = UInt64ToSizeT(getSize(), &bufsize);
		if (FAILED(hr)) return hr;
		if (!buf.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
		hr = image->ReadData(buf, getOffset(), getSize());
		if (FAILED(hr)) return hr;

		uint64_t counts = getSize() / sizeof(uint8_t);
		const uint8_t* pVal = buf;

		std::wstring temp;
		const rsize_t limit = 64;	//config->byte_stream_limit_length;
		temp.reserve(limit * 3 + 8);

		temp += L"[";
		if (counts != 0)
		{
			wchar_t c[16];
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

		*str = agaliaString::create(temp.c_str());
		return S_OK;
	}
	return E_INVALIDARG;
}
