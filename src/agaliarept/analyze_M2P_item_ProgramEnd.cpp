#include "pch.h"
#include "analyze_M2P_item_ProgramEnd.h"

#include "analyze_M2P_util.h"

using namespace analyze_M2P;



item_ProgramEnd::item_ProgramEnd(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:item_Base(image, offset, size)
{
}



item_ProgramEnd::~item_ProgramEnd()
{
}



HRESULT item_ProgramEnd::getItemName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	*str = agaliaString::create(L"MPEG_program_end_code");
	return S_OK;
}



HRESULT item_ProgramEnd::getGridRowCount(uint32_t* row) const
{
	if (row == nullptr) return E_POINTER;
	*row = 1;
	return S_OK;
}



HRESULT item_ProgramEnd::getGridValue(uint32_t row, uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	if (row != 0) return E_INVALIDARG;

	std::wstringstream temp;
	if (column == column_offset)
	{
		format_hex(temp, getOffset(), 8);
	}
	else if (column == column_field)
	{
		return getItemName(str);
	}
	else if (column == column_bits)
	{
		format_dec(temp, 32);
	}
	else if (column == column_value)
	{
		const rsize_t read_size = 4;
		CHeapPtr<uint8_t> buf;
		if (!buf.AllocateBytes(read_size + 8)) return E_OUTOFMEMORY;
		auto hr = image->ReadData(buf, getOffset(), read_size);
		if (FAILED(hr)) return hr;

		uint32_t pack_start_code = get_value_32(buf, 0, 32);

		temp << L"0x";
		format_hex(temp, pack_start_code);
	}
	else
	{
		return E_INVALIDARG;
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
