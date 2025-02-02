#include "pch.h"
#include "analyze_BMP_item_EmbeddedProfile.h"

using namespace analyze_BMP;

item_EmbeddedProfile::item_EmbeddedProfile(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:BMP_item_Base(image, offset, size)
{
}

item_EmbeddedProfile::~item_EmbeddedProfile()
{
}

HRESULT item_EmbeddedProfile::getName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	*str = agaliaString::create(L"Profile");
	return S_OK;
}
