#include "pch.h"
#include "analyze_M2P_item_ProgramStreamPack.h"

#include "analyze_M2P_item_ProgramStreamPackHeader.h"
#include "analyze_M2P_item_PESPacket.h"
#include "analyze_M2P_item_ProgramEnd.h"
#include "analyze_M2P_util.h"

using namespace analyze_M2P;


item_ProgramStreamPack::item_ProgramStreamPack(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:item_Base(image, offset, size)
{
}



item_ProgramStreamPack::~item_ProgramStreamPack()
{
}



HRESULT item_ProgramStreamPack::getItemName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	*str = agaliaString::create(L"pack");
	return S_OK;
}



HRESULT item_ProgramStreamPack::getChildItem(uint32_t sibling, agaliaItem** child) const
{
	if (sibling != 0) return E_FAIL;
	if (child == nullptr) return E_POINTER;

	uint64_t size = item_ProgramStreamPackHeader::calc_item_size(image, getOffset());
	if (size == 0) return E_FAIL;

	auto p = new item_ProgramStreamPackHeader(image, getOffset(), size);
	*child = p;

	return S_OK;
}



HRESULT item_ProgramStreamPack::getNextItem(agaliaItem** next) const
{
	if (next == nullptr) return E_POINTER;

	uint64_t size = item_ProgramStreamPack::calc_item_size(image, getOffset() + getSize());
	if (size != 0)
	{
		auto p = new item_ProgramStreamPack(image, getOffset() + getSize(), size);
		*next = p;
		return S_OK;
	}

	uint8_t buf[4];
	auto hr = image->ReadData(buf, getOffset() + getSize(), sizeof(buf));
	if (FAILED(hr)) return 0;

	uint32_t code = get_value_32(buf, 0, 32);
	if (code == 0x000001B9)
	{
		auto p = new item_ProgramEnd(image, getOffset() + getSize(), 4);
		*next = p;
		return S_OK;
	}

	return E_FAIL;
}



uint64_t item_ProgramStreamPack::calc_item_size(const agaliaContainer* image, uint64_t offset)
{
	uint64_t size = item_ProgramStreamPackHeader::calc_item_size(image, offset);
	if (size == 0) return 0;

	uint64_t total_size = size;

	offset += size;
	while ((size = item_PES_packet::calc_item_size(image, offset)) != 0)
	{
		total_size += size;
		offset += size;
	}

	return total_size;
}
