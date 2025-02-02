#pragma once

class agaliaContainer;

#include <stdint.h>

namespace analyze_ASF
{
	class ASF_item_Base;
	ASF_item_Base* createItem(const agaliaContainer* image, uint64_t offset, uint64_t endpos);
}
