#pragma once

class agaliaItem;
class agaliaContainer;
#include <stdint.h>

namespace analyze_ISO
{
	agaliaItem* createItem(const agaliaContainer* image, uint64_t offset, uint64_t endpos, uint32_t parent);
};
