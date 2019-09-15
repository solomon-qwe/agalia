#pragma once

class agaliaContainer;

#include <stdint.h>

namespace analyze_ASF
{
	class item_Base;
	item_Base* createItem(const agaliaContainer* image, uint64_t offset, uint64_t endpos);
}
