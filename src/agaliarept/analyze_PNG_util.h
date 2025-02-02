#pragma once

class agaliaElement;
#include <stdint.h>

namespace analyze_PNG
{
	class container_PNG;
	agaliaElement* createItem(const container_PNG* image, uint64_t offset, uint64_t endpos);
};
