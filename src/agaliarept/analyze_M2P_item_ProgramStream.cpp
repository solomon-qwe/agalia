#include "pch.h"
#include "analyze_M2P_item_ProgramStream.h"

#include "analyze_M2P_item_ProgramStreamPack.h"

using namespace analyze_M2P;


item_ProgramStream::item_ProgramStream(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:item_Base(image, offset, size)
{
}



item_ProgramStream::~item_ProgramStream()
{
}



HRESULT item_ProgramStream::getItemName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	*str = agaliaString::create(L"MPEG2_program_stream");
	return S_OK;
}



HRESULT item_ProgramStream::getChildItem(uint32_t sibling, agaliaItem** child) const
{
	if (sibling != 0) return E_FAIL;
	if (child == nullptr) return E_POINTER;

	uint64_t size = item_ProgramStreamPack::calc_item_size(image, getOffset());
	if (size == 0) return E_FAIL;

	auto p = new item_ProgramStreamPack(image, getOffset(), size);
	*child = p;

	return S_OK;
}



HRESULT item_ProgramStream::getNextItem(agaliaItem** next) const
{
	UNREFERENCED_PARAMETER(next);
	return E_NOTIMPL;
}
