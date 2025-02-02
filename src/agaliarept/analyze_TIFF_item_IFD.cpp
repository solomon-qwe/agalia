#include "pch.h"
#include "analyze_TIFF_item_IFD.h"



using namespace analyze_TIFF;



item_IFDBase::item_IFDBase(const container_TIFF* image, uint64_t offset, uint64_t size, int ifdtype)
	: TIFF_item_Base(image, offset, size)
{
	ifd_type_id = ifdtype;
}



item_IFDBase::~item_IFDBase()
{
}



HRESULT item_IFDBase::getName(agaliaString** str) const
{
	const wchar_t* name = nullptr;
	switch (ifd_type_id)
	{
	case ifd_exif:		name = L"Exif IFD";	break;
	case ifd_gps:		name = L"GPSInfo IFD";	break;
	case ifd_interoperability:	name = L"Interoperability IFD";	break;
	case ifd_subifds:	name = L"Sub IFD";	break;
	case ifd_subifd:	name = L"Sub IFD";	break;
	case 1:				name = L"1st IFD";	break;
	case 2:				name = L"2nd IFD";	break;
	case 3:				name = L"3rd IFD";	break;
	default:
		if (0 <= ifd_type_id)
		{
			std::wstringstream temp;
			temp << ifd_type_id << L"th IFD";
			*str = agaliaString::create(temp.str().c_str());
			return S_OK;
		}
		return E_FAIL;
	}

	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT item_IFDBase::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}



HRESULT item_IFDBase::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_offset:		name = L"offset";		break;
	case prop_count:		name = L"count";		break;
	case prop_next_offset:	name = L"next_offset";	break;
	default:
		return E_FAIL;
	}
	*str = agaliaString::create(name);

	return S_OK;
}
