#include "pch.h"
#include "analyze_ASF_util.h"

#include "../inc/agaliarept.h"
#include "asf_def.h"

#include "analyze_ASF_item_HeaderObject.h"

using namespace analyze_ASF;

ASF_item_Base* analyze_ASF::createItem(const agaliaContainer* image, uint64_t offset, uint64_t endpos)
{
	ASFObject obj = {};
	auto hr = image->ReadData(&obj, offset, sizeof(obj));
	if (FAILED(hr)) return nullptr;

	if (endpos < offset + obj.ObjectSize)
		return nullptr;

	if (IsEqualGUID(obj.ObjectID, ASF_Header_Object)) {
		return new item_HeaderObject(image, offset, obj.ObjectSize, endpos);
	}
	else if (IsEqualGUID(obj.ObjectID, ASF_Data_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Simple_Index_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Index_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Media_Object_Index_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Timecode_Index_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_File_Properties_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Stream_Properties_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Header_Extension_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Codec_List_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Script_Command_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Marker_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Bitrate_Mutual_Exclusion_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Error_Correction_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Content_Description_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Extended_Content_Description_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Content_Branding_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Stream_Bitrate_Properties_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Extended_Stream_Properties_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Advanced_Mutual_Exclusion_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Group_Mutual_Exclusion_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Stream_Prioritization_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Bandwidth_Sharing_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Language_List_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Metadata_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Metadata_Library_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Index_Parameters_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Media_Object_Index_Parameters_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Timecode_Index_Parameters_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Compatibility_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Advanced_Content_Encryption_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Content_Encryption_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Extended_Content_Encryption_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Digital_Signature_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Padding_Object)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Audio_Media)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Video_Media)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Command_Media)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_JFIF_Media)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Degradable_JPEG_Media)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_File_Transfer_Media)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Binary_Media)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Web_Stream_Media_Subtype)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Web_Stream_Format)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_No_Error_Correction)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Audio_Spread)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Reserved_1)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Content_Encryption_System_Windows_Media_DRM_Network_Devices)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Reserved_2)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Reserved_3)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Reserved_4)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Mutex_Language)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Mutex_Bitrate)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Mutex_Unknown)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Bandwidth_Sharing_Exclusive)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Bandwidth_Sharing_Partial)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Payload_Extension_System_Timecode)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Payload_Extension_System_File_Name)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Payload_Extension_System_Content_Type)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Payload_Extension_System_Pixel_Aspect_Ratio)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Payload_Extension_System_Sample_Duration)) {}
	else if (IsEqualGUID(obj.ObjectID, ASF_Payload_Extension_System_Encryption_Sample_ID)) {}

	return new ASF_item_Base(image, offset, obj.ObjectSize, endpos);
}
