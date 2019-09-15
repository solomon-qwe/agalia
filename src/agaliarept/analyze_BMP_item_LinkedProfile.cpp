#include "pch.h"
#include "analyze_BMP_item_LinkedProfile.h"

using namespace analyze_BMP;

item_LinkedProfile::item_LinkedProfile(const agaliaContainer* image, uint64_t offset, uint64_t size)
	:item_Base(image, offset, size)
{
}

item_LinkedProfile::~item_LinkedProfile()
{
}

HRESULT item_LinkedProfile::getItemName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	*str = agaliaString::create(L"Profile");
	return S_OK;
}

HRESULT item_LinkedProfile::getItemPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT item_LinkedProfile::getItemPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	if (index == prop_filename) {
		*str = agaliaString::create(L"file name");
		return S_OK;
	}
	return __super::getItemPropName(index, str);
}

HRESULT item_LinkedProfile::getItemPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	if (index == prop_filename) {
		CHeapPtr<char> buf;
		rsize_t bufsize = 0;
		auto hr = UInt64ToSizeT(getSize(), &bufsize);
		if (FAILED(hr)) return hr;
		if (!buf.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
		hr = image->ReadData(buf, getOffset(), getSize());
		if (FAILED(hr)) return hr;
		int size = 0;
		hr = UInt64ToInt(getSize(), &size);
		if (FAILED(hr)) return hr;
		return multibyte_to_widechar(buf, size, CP_ACP, str);
	}
	return __super::getItemPropValue(index, str);
}
