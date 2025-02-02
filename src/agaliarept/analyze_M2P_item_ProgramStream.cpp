#include "pch.h"
#include "analyze_M2P_item_ProgramStream.h"

#include "analyze_M2P_item_ProgramStreamPack.h"

using namespace analyze_M2P;


item_ProgramStream::item_ProgramStream(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:M2P_item_Base(image, offset, size)
{
}



item_ProgramStream::~item_ProgramStream()
{
}



HRESULT item_ProgramStream::getName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	*str = agaliaString::create(L"MPEG2_program_stream");
	return S_OK;
}



HRESULT item_ProgramStream::getChild(uint32_t sibling, agaliaElement** child) const
{
	if (sibling != 0) return E_FAIL;
	if (child == nullptr) return E_POINTER;

	uint64_t size = item_ProgramStreamPack::calc_item_size(image, getOffset());
	if (size == 0) return E_FAIL;

	auto p = new item_ProgramStreamPack(image, getOffset(), size);
	*child = p;

	return S_OK;
}



HRESULT item_ProgramStream::getNext(agaliaElement** next) const
{
	UNREFERENCED_PARAMETER(next);
	return E_NOTIMPL;
}
