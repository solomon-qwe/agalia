#include "pch.h"
#include "analyze_ISO_util.h"

#include "../inc/agaliarept.h"
#include "analyze_ISO_item_Box.h"
#include "analyze_ISO_item_FileTypeBox.h"

using namespace analyze_ISO;

agaliaItem* analyze_ISO::createItem(const agaliaContainer* image, uint64_t offset, uint64_t endpos, uint32_t parent)
{
	uint64_t boxsize = Box::getBoxSize(image, offset, endpos);

	if (endpos < offset + boxsize)
		return nullptr;

	Box box = {};
	auto hr = image->ReadData(&box, offset, sizeof(box));
	if (FAILED(hr)) return nullptr;

	switch (box.getType())
	{
	case 'ftyp':
		return new item_FileTypeBox(image, offset, boxsize, endpos, parent);
	}
	return new item_Box(image, offset, boxsize, endpos, parent);
}
