#include "pch.h"
#include "analyze_PNG_item_eXIf.h"

#include "container_PNG.h"

using namespace analyze_PNG;

item_eXIf::item_eXIf(const container_PNG* image, uint64_t offset, uint64_t size, uint64_t endpos)
	: item_Base(image, offset, size, endpos)
{
	agaliaStringPtr file_path;
	auto hr = image->getFilePath(&file_path);
	if (FAILED(hr)) return;

	agaliaContainer* exif_image = nullptr;
	hr = getAgaliaImage(&exif_image, file_path, offset + offsetof(Chunk, ChunkData), size - offsetof(Chunk, ChunkData) - sizeof(uint32_t));
	if (FAILED(hr)) return;

	this->tiff_image = exif_image;
}

item_eXIf::~item_eXIf()
{
	if (tiff_image)
		tiff_image->Release();
}

HRESULT item_eXIf::getChildItem(uint32_t sibling, agaliaItem** child) const
{
	if (sibling != 0) return E_FAIL;

	if (tiff_image)
		return tiff_image->getRootItem(child);
	return E_FAIL;
}
