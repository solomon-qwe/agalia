#include "pch.h"
#include "analyze_ASF_item_Base.h"

#include "asf_def.h"
#include "analyze_ASF_util.h"

using namespace analyze_ASF;

// {C4333FE9-23F5-448C-9101-D0A8C525BE9A}
const GUID ASF_item_Base::guid_asf =
{ 0xc4333fe9, 0x23f5, 0x448c, { 0x91, 0x1, 0xd0, 0xa8, 0xc5, 0x25, 0xbe, 0x9a } };

ASF_item_Base::ASF_item_Base(const agaliaContainer* image, uint64_t offset, uint64_t size, uint64_t endpos)
	:_agaliaItemBase(guid_asf, offset, size)
{
	this->image = image;
	this->endpos = endpos;
}

ASF_item_Base::~ASF_item_Base()
{

}

HRESULT ASF_item_Base::getName(agaliaString** str) const
{
	GUID ObjectID = {};
	auto hr = image->ReadData(&ObjectID, getOffset(), sizeof(ObjectID));
	if (FAILED(hr)) return hr;

	const wchar_t* name = nullptr;
	if (IsEqualGUID(ObjectID, ASF_Header_Object)) { name = L"ASF_Header_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Data_Object)) { name = L"ASF_Data_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Simple_Index_Object)) { name = L"ASF_Simple_Index_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Index_Object)) { name = L"ASF_Index_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Media_Object_Index_Object)) { name = L"ASF_Media_Object_Index_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Timecode_Index_Object)) { name = L"ASF_Timecode_Index_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_File_Properties_Object)) { name = L"ASF_File_Properties_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Stream_Properties_Object)) { name = L"ASF_Stream_Properties_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Header_Extension_Object)) { name = L"ASF_Header_Extension_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Codec_List_Object)) { name = L"ASF_Codec_List_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Script_Command_Object)) { name = L"ASF_Script_Command_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Marker_Object)) { name = L"ASF_Marker_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Bitrate_Mutual_Exclusion_Object)) { name = L"ASF_Bitrate_Mutual_Exclusion_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Error_Correction_Object)) { name = L"ASF_Error_Correction_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Content_Description_Object)) { name = L"ASF_Content_Description_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Extended_Content_Description_Object)) { name = L"ASF_Extended_Content_Description_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Content_Branding_Object)) { name = L"ASF_Content_Branding_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Stream_Bitrate_Properties_Object)) { name = L"ASF_Stream_Bitrate_Properties_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Extended_Stream_Properties_Object)) { name = L"ASF_Extended_Stream_Properties_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Advanced_Mutual_Exclusion_Object)) { name = L"ASF_Advanced_Mutual_Exclusion_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Group_Mutual_Exclusion_Object)) { name = L"ASF_Group_Mutual_Exclusion_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Stream_Prioritization_Object)) { name = L"ASF_Stream_Prioritization_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Bandwidth_Sharing_Object)) { name = L"ASF_Bandwidth_Sharing_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Language_List_Object)) { name = L"ASF_Language_List_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Metadata_Object)) { name = L"ASF_Metadata_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Metadata_Library_Object)) { name = L"ASF_Metadata_Library_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Index_Parameters_Object)) { name = L"ASF_Index_Parameters_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Media_Object_Index_Parameters_Object)) { name = L"ASF_Media_Object_Index_Parameters_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Timecode_Index_Parameters_Object)) { name = L"ASF_Timecode_Index_Parameters_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Compatibility_Object)) { name = L"ASF_Compatibility_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Advanced_Content_Encryption_Object)) { name = L"ASF_Advanced_Content_Encryption_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Content_Encryption_Object)) { name = L"ASF_Content_Encryption_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Extended_Content_Encryption_Object)) { name = L"ASF_Extended_Content_Encryption_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Digital_Signature_Object)) { name = L"ASF_Digital_Signature_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Padding_Object)) { name = L"ASF_Padding_Object"; }
	else if (IsEqualGUID(ObjectID, ASF_Audio_Media)) { name = L"ASF_Audio_Media"; }
	else if (IsEqualGUID(ObjectID, ASF_Video_Media)) { name = L"ASF_Video_Media"; }
	else if (IsEqualGUID(ObjectID, ASF_Command_Media)) { name = L"ASF_Command_Media"; }
	else if (IsEqualGUID(ObjectID, ASF_JFIF_Media)) { name = L"ASF_JFIF_Media"; }
	else if (IsEqualGUID(ObjectID, ASF_Degradable_JPEG_Media)) { name = L"ASF_Degradable_JPEG_Media"; }
	else if (IsEqualGUID(ObjectID, ASF_File_Transfer_Media)) { name = L"ASF_File_Transfer_Media"; }
	else if (IsEqualGUID(ObjectID, ASF_Binary_Media)) { name = L"ASF_Binary_Media"; }
	else if (IsEqualGUID(ObjectID, ASF_Web_Stream_Media_Subtype)) { name = L"ASF_Web_Stream_Media_Subtype"; }
	else if (IsEqualGUID(ObjectID, ASF_Web_Stream_Format)) { name = L"ASF_Web_Stream_Format"; }
	else if (IsEqualGUID(ObjectID, ASF_No_Error_Correction)) { name = L"ASF_No_Error_Correction"; }
	else if (IsEqualGUID(ObjectID, ASF_Audio_Spread)) { name = L"ASF_Audio_Spread"; }
	else if (IsEqualGUID(ObjectID, ASF_Reserved_1)) { name = L"ASF_Reserved_1"; }
	else if (IsEqualGUID(ObjectID, ASF_Content_Encryption_System_Windows_Media_DRM_Network_Devices)) { name = L"ASF_Content_Encryption_System_Windows_Media_DRM_Network_Devices"; }
	else if (IsEqualGUID(ObjectID, ASF_Reserved_2)) { name = L"ASF_Reserved_2"; }
	else if (IsEqualGUID(ObjectID, ASF_Reserved_3)) { name = L"ASF_Reserved_3"; }
	else if (IsEqualGUID(ObjectID, ASF_Reserved_4)) { name = L"ASF_Reserved_4"; }
	else if (IsEqualGUID(ObjectID, ASF_Mutex_Language)) { name = L"ASF_Mutex_Language"; }
	else if (IsEqualGUID(ObjectID, ASF_Mutex_Bitrate)) { name = L"ASF_Mutex_Bitrate"; }
	else if (IsEqualGUID(ObjectID, ASF_Mutex_Unknown)) { name = L"ASF_Mutex_Unknown"; }
	else if (IsEqualGUID(ObjectID, ASF_Bandwidth_Sharing_Exclusive)) { name = L"ASF_Bandwidth_Sharing_Exclusive"; }
	else if (IsEqualGUID(ObjectID, ASF_Bandwidth_Sharing_Partial)) { name = L"ASF_Bandwidth_Sharing_Partial"; }
	else if (IsEqualGUID(ObjectID, ASF_Payload_Extension_System_Timecode)) { name = L"ASF_Payload_Extension_System_Timecode"; }
	else if (IsEqualGUID(ObjectID, ASF_Payload_Extension_System_File_Name)) { name = L"ASF_Payload_Extension_System_File_Name"; }
	else if (IsEqualGUID(ObjectID, ASF_Payload_Extension_System_Content_Type)) { name = L"ASF_Payload_Extension_System_Content_Type"; }
	else if (IsEqualGUID(ObjectID, ASF_Payload_Extension_System_Pixel_Aspect_Ratio)) { name = L"ASF_Payload_Extension_System_Pixel_Aspect_Ratio"; }
	else if (IsEqualGUID(ObjectID, ASF_Payload_Extension_System_Sample_Duration)) { name = L"ASF_Payload_Extension_System_Sample_Duration"; }
	else if (IsEqualGUID(ObjectID, ASF_Payload_Extension_System_Encryption_Sample_ID)) { name = L"ASF_Payload_Extension_System_Encryption_Sample_ID"; }
	else
	{
		LPOLESTR strGUID;
		if (SUCCEEDED(StringFromIID(ObjectID, &strGUID)))
		{
			*str = agaliaString::create(strGUID);
			::CoTaskMemFree(strGUID);
			return S_OK;
		}
		return E_FAIL;
	}
	*str = agaliaString::create(name);
	return S_OK;
}

HRESULT ASF_item_Base::getPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT ASF_item_Base::getPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_offset: name = L"offset"; break;
	case prop_object_guid: name = L"Object GUID"; break;
	case prop_object_size: name = L"Object Size"; break;
	default:
		return E_INVALIDARG;
	}
	*str = agaliaString::create(name);
	return S_OK;
}

HRESULT ASF_item_Base::getPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	if (index == prop_offset)
	{
		std::wstringstream temp;
		format_dec(temp, getOffset());
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_object_guid)
	{
		return getName(str);
	}
	else if (index == prop_object_size)
	{
		uint64_t ObjectSize = 0;
		auto hr = image->ReadData(&ObjectSize, getOffset() + sizeof(GUID), sizeof(ObjectSize));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		format_dec(temp, ObjectSize);
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}

	return E_FAIL;
}

HRESULT ASF_item_Base::getChild(uint32_t sibling, agaliaElement** child) const
{
	UNREFERENCED_PARAMETER(sibling);
	UNREFERENCED_PARAMETER(child);
	return E_NOTIMPL;
}


HRESULT ASF_item_Base::getNext(agaliaElement** next) const
{
	if (next == nullptr) return E_POINTER;
	auto p = createItem(image, getOffset() + getSize(), endpos);
	if (p)
	{
		if (parent) {
			p->parent.reset(new uint64_t(*parent));
		}
		*next = p;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT ASF_item_Base::getAsocImage(const agaliaContainer** imageAsoc) const
{
	if (imageAsoc == nullptr) return E_POINTER;
	*imageAsoc = image;
	return S_OK;
}



HRESULT ASF_item_Base::getElementInfoCount(uint32_t* row) const
{
	if (row == nullptr) return E_POINTER;
	*row = 1;
	return S_OK;
}



HRESULT ASF_item_Base::getElementInfoValue(uint32_t row, uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	if (row != 0) return E_INVALIDARG;

	std::wstringstream temp;
	if (column == column_offset)
	{
		format_hex(temp, getOffset(), 8);
	}
	else if (column == column_parent)
	{
		if (parent) {
			format_hex(temp, *parent, 8);
		}
	}
	else if (column == column_objectID)
	{
		return getPropValue(prop_object_guid, str);
	}
	else if (column == column_size)
	{
		return getPropValue(prop_object_size, str);
	}
	else
	{
		return E_INVALIDARG;
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
