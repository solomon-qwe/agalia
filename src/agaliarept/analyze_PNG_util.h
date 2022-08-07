#pragma once

class agaliaItem;
#include <stdint.h>

namespace analyze_PNG
{
	class container_PNG;
	agaliaItem* createItem(const container_PNG* image, uint64_t offset, uint64_t endpos);
};
