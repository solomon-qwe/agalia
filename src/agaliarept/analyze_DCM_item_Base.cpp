#include "pch.h"
#include "analyze_DCM_item_Base.h"

using namespace analyze_DCM;

// {21A33158-ADF0-4223-A3E2-19152A824B97}
const GUID _agaliaItemDICOMBase::guid_dicom =
{ 0x21a33158, 0xadf0, 0x4223, { 0xa3, 0xe2, 0x19, 0x15, 0x2a, 0x82, 0x4b, 0x97 } };


_agaliaItemDICOMBase::_agaliaItemDICOMBase(const container_DCM_Impl* image, uint64_t offset, uint64_t size)
	: _agaliaItemBase(guid_dicom, offset, size)
{
	this->image = image;
}


_agaliaItemDICOMBase::~_agaliaItemDICOMBase()
{
}
