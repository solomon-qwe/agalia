#include "pch.h"
#include "analyze_PNG_util.h"

#include "analyze_PNG_item_Base.h"
#include "analyze_PNG_item_IHDR.h"
#include "analyze_PNG_item_iCCP.h"
#include "analyze_PNG_item_iTXt.h"
#include "analyze_PNG_item_eXIf.h"

using namespace analyze_PNG;

agaliaElement* analyze_PNG::createItem(const container_PNG* image, uint64_t offset, uint64_t endpos)
{
	Chunk chunk = {};
	auto hr = image->ReadData(&chunk, offset, offsetof(Chunk, ChunkData));
	if (FAILED(hr)) return nullptr;

	uint64_t chunk_size = offsetof(Chunk, ChunkData) + chunk.getLength() + sizeof(uint32_t);

	if (endpos < offset + chunk_size)
		return nullptr;

	switch (chunk.getChunkType())
	{
	case 'IHDR':
		return new item_IHDR(image, offset, chunk_size, endpos);
	case 'iCCP':
		return new item_iCCP(image, offset, chunk_size, endpos);
	case 'iTXt':
		return new item_iTXt(image, offset, chunk_size, endpos);
	case 'eXIf':
		return new item_eXIf(image, offset, chunk_size, endpos);
	}
	return new PNG_item_Base(image, offset, chunk_size, endpos);
}
