#include "stdafx.h"
#include "../inc/agaliarept.h"
#include "asf_def.h"
#include "callback.h"

#include <objbase.h>

size_t print_ASF_Header_Object(int subItem, const uint8_t* base, uint64_t offset, uint64_t size, callback_cls* callback)
{
	size_t pos = 0;
	pos += sizeof(GUID);
	pos += sizeof(uint64_t);

	std::wstring str;
	wchar_t buf[128];

	{
		uint32_t objects;
		if (size < pos + sizeof(objects)) return 0;

		str = L"Number of Header Objects=";
		objects = *reinterpret_cast<const uint32_t*>(base + offset + pos);
		swprintf_s(buf, L"%u", objects);
		str += buf;
		callback->print(subItem, str);

		pos += sizeof(objects);
	}

	callback->next();
	{
		uint8_t reserved1;
		if (size < pos + sizeof(reserved1)) return 0;

		str = L"Reserved1=";
		reserved1 = *reinterpret_cast<const uint8_t*>(base + offset + pos);
		swprintf_s(buf, L"%u", reserved1);
		str += buf;
		callback->print(subItem, str);

		pos += sizeof(reserved1);
	}

	callback->next();
	{
		uint8_t reserved2;
		if (size < pos + sizeof(reserved2)) return 0;

		str = L"Reserved2=";
		reserved2 = *reinterpret_cast<const uint8_t*>(base + offset + pos);
		swprintf_s(buf, L"%u", reserved2);
		str += buf;
		callback->print(subItem, str);

		pos += sizeof(reserved2);
	}

	return pos;
}


size_t print_ASF_Content_Description_Object(int subItem, const uint8_t* base, uint64_t offset, uint64_t size, callback_cls* callback)
{
	size_t pos = 0;
	pos += sizeof(GUID);
	pos += sizeof(uint64_t);

	std::wstring str;
	wchar_t buf[128];

	uint16_t title_length;
	{
		if (size < pos + sizeof(title_length)) return 0;

		str = L"Title Length=";
		title_length = *reinterpret_cast<const uint16_t*>(base + offset + pos);
		swprintf_s(buf, L"%u", title_length);
		str += buf;
		callback->print(subItem, str);

		pos += sizeof(title_length);
	}

	callback->next();
	uint16_t author_length;
	{
		if (size < pos + sizeof(author_length)) return 0;

		str = L"Author Length=";
		author_length = *reinterpret_cast<const uint16_t*>(base + offset + pos);
		swprintf_s(buf, L"%u", author_length);
		str += buf;
		callback->print(subItem, str);

		pos += sizeof(author_length);
	}

	callback->next();
	uint16_t copyright_length;
	{
		if (size < pos + sizeof(copyright_length)) return 0;

		str = L"Copyright Length=";
		copyright_length = *reinterpret_cast<const uint16_t*>(base + offset + pos);
		swprintf_s(buf, L"%u", copyright_length);
		str += buf;
		callback->print(subItem, str);

		pos += sizeof(copyright_length);
	}

	callback->next();
	uint16_t description_length;
	{
		if (size < pos + sizeof(description_length)) return 0;

		str = L"Description Length=";
		description_length = *reinterpret_cast<const uint16_t*>(base + offset + pos);
		swprintf_s(buf, L"%u", description_length);
		str += buf;
		callback->print(subItem, str);

		pos += sizeof(description_length);
	}

	callback->next();
	uint16_t rating_length;
	{
		if (size < pos + sizeof(rating_length)) return 0;

		str = L"Rating Length=";
		rating_length = *reinterpret_cast<const uint16_t*>(base + offset + pos);
		swprintf_s(buf, L"%u", rating_length);
		str += buf;
		callback->print(subItem, str);

		pos += sizeof(rating_length);
	}

	callback->next();
	{
		if (size < pos + title_length) return 0;

		str = L"Title=\"";
		str += reinterpret_cast<const wchar_t*>(base + offset + pos);
		str += L"\"";
		callback->print(subItem, str);

		pos += title_length;
	}

	callback->next();
	{
		if (size < pos + author_length) return 0;

		str = L"Author=\"";
		str += reinterpret_cast<const wchar_t*>(base + offset + pos);
		str += L"\"";
		callback->print(subItem, str);

		pos += author_length;
	}

	callback->next();
	{
		if (size < pos + copyright_length) return 0;

		str = L"Copyright=\"";
		str += reinterpret_cast<const wchar_t*>(base + offset + pos);
		str += L"\"";
		callback->print(subItem, str);

		pos += copyright_length;
	}

	callback->next();
	{
		if (size < pos + description_length) return 0;

		str = L"Description=\"";
		str += reinterpret_cast<const wchar_t*>(base + offset + pos);
		str += L"\"";
		callback->print(subItem, str);

		pos += description_length;
	}

	callback->next();
	{
		if (size < pos + rating_length) return 0;

		str = L"Rating=\"";
		str += reinterpret_cast<const wchar_t*>(base + offset + pos);
		str += L"\"";
		callback->print(subItem, str);

		pos += rating_length;
	}

	return 0;
}


void parseAsfObject(const uint8_t* base, uint64_t offset, uint64_t endPos, uint64_t parent, callback_cls* callback, const agalia::config* config);


HRESULT analyzeAsfObject(uint64_t* object_size, const uint8_t* base, uint64_t offset, uint64_t endPos, uint64_t parent, callback_cls* callback, const agalia::config* config)
{
	if (object_size == nullptr) return E_POINTER;

	size_t pos = 0;

	// offset 
	int subItem = 0;
	{
		wchar_t sz[16] = {};
		swprintf_s(sz, L"%.8I64X", offset);
		callback->print(subItem, sz);
	}

	// parent  
	subItem++;
	if (parent != SIZE_MAX)
	{
		wchar_t sz[16] = {};
		swprintf_s(sz, L"%.8I64X", parent);
		callback->print(subItem, sz);
	}

	GUID ObjectID;
	if (endPos < offset + pos + sizeof(ObjectID)) return E_FAIL;
	ObjectID = *reinterpret_cast<const GUID*>(base + offset + pos);
	pos += sizeof(ObjectID);

	// id 
	subItem++;
	if (IsEqualGUID(ObjectID, ASF_Header_Object)) { callback->print(subItem, L"ASF_Header_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Data_Object)) { callback->print(subItem, L"ASF_Data_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Simple_Index_Object)) { callback->print(subItem, L"ASF_Simple_Index_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Index_Object)) { callback->print(subItem, L"ASF_Index_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Media_Object_Index_Object)) { callback->print(subItem, L"ASF_Media_Object_Index_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Timecode_Index_Object)) { callback->print(subItem, L"ASF_Timecode_Index_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_File_Properties_Object)) { callback->print(subItem, L"ASF_File_Properties_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Stream_Properties_Object)) { callback->print(subItem, L"ASF_Stream_Properties_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Header_Extension_Object)) { callback->print(subItem, L"ASF_Header_Extension_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Codec_List_Object)) { callback->print(subItem, L"ASF_Codec_List_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Script_Command_Object)) { callback->print(subItem, L"ASF_Script_Command_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Marker_Object)) { callback->print(subItem, L"ASF_Marker_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Bitrate_Mutual_Exclusion_Object)) { callback->print(subItem, L"ASF_Bitrate_Mutual_Exclusion_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Error_Correction_Object)) { callback->print(subItem, L"ASF_Error_Correction_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Content_Description_Object)) { callback->print(subItem, L"ASF_Content_Description_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Extended_Content_Description_Object)) { callback->print(subItem, L"ASF_Extended_Content_Description_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Content_Branding_Object)) { callback->print(subItem, L"ASF_Content_Branding_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Stream_Bitrate_Properties_Object)) { callback->print(subItem, L"ASF_Stream_Bitrate_Properties_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Extended_Stream_Properties_Object)) { callback->print(subItem, L"ASF_Extended_Stream_Properties_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Advanced_Mutual_Exclusion_Object)) { callback->print(subItem, L"ASF_Advanced_Mutual_Exclusion_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Group_Mutual_Exclusion_Object)) { callback->print(subItem, L"ASF_Group_Mutual_Exclusion_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Stream_Prioritization_Object)) { callback->print(subItem, L"ASF_Stream_Prioritization_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Bandwidth_Sharing_Object)) { callback->print(subItem, L"ASF_Bandwidth_Sharing_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Language_List_Object)) { callback->print(subItem, L"ASF_Language_List_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Metadata_Object)) { callback->print(subItem, L"ASF_Metadata_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Metadata_Library_Object)) { callback->print(subItem, L"ASF_Metadata_Library_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Index_Parameters_Object)) { callback->print(subItem, L"ASF_Index_Parameters_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Media_Object_Index_Parameters_Object)) { callback->print(subItem, L"ASF_Media_Object_Index_Parameters_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Timecode_Index_Parameters_Object)) { callback->print(subItem, L"ASF_Timecode_Index_Parameters_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Compatibility_Object)) { callback->print(subItem, L"ASF_Compatibility_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Advanced_Content_Encryption_Object)) { callback->print(subItem, L"ASF_Advanced_Content_Encryption_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Content_Encryption_Object)) { callback->print(subItem, L"ASF_Content_Encryption_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Extended_Content_Encryption_Object)) { callback->print(subItem, L"ASF_Extended_Content_Encryption_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Digital_Signature_Object)) { callback->print(subItem, L"ASF_Digital_Signature_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Padding_Object)) { callback->print(subItem, L"ASF_Padding_Object"); }
	else if (IsEqualGUID(ObjectID, ASF_Audio_Media)) { callback->print(subItem, L"ASF_Audio_Media"); }
	else if (IsEqualGUID(ObjectID, ASF_Video_Media)) { callback->print(subItem, L"ASF_Video_Media"); }
	else if (IsEqualGUID(ObjectID, ASF_Command_Media)) { callback->print(subItem, L"ASF_Command_Media"); }
	else if (IsEqualGUID(ObjectID, ASF_JFIF_Media)) { callback->print(subItem, L"ASF_JFIF_Media"); }
	else if (IsEqualGUID(ObjectID, ASF_Degradable_JPEG_Media)) { callback->print(subItem, L"ASF_Degradable_JPEG_Media"); }
	else if (IsEqualGUID(ObjectID, ASF_File_Transfer_Media)) { callback->print(subItem, L"ASF_File_Transfer_Media"); }
	else if (IsEqualGUID(ObjectID, ASF_Binary_Media)) { callback->print(subItem, L"ASF_Binary_Media"); }
	else if (IsEqualGUID(ObjectID, ASF_Web_Stream_Media_Subtype)) { callback->print(subItem, L"ASF_Web_Stream_Media_Subtype"); }
	else if (IsEqualGUID(ObjectID, ASF_Web_Stream_Format)) { callback->print(subItem, L"ASF_Web_Stream_Format"); }
	else if (IsEqualGUID(ObjectID, ASF_No_Error_Correction)) { callback->print(subItem, L"ASF_No_Error_Correction"); }
	else if (IsEqualGUID(ObjectID, ASF_Audio_Spread)) { callback->print(subItem, L"ASF_Audio_Spread"); }
	else if (IsEqualGUID(ObjectID, ASF_Reserved_1)) { callback->print(subItem, L"ASF_Reserved_1"); }
	else if (IsEqualGUID(ObjectID, ASF_Content_Encryption_System_Windows_Media_DRM_Network_Devices)) { callback->print(subItem, L"ASF_Content_Encryption_System_Windows_Media_DRM_Network_Devices"); }
	else if (IsEqualGUID(ObjectID, ASF_Reserved_2)) { callback->print(subItem, L"ASF_Reserved_2"); }
	else if (IsEqualGUID(ObjectID, ASF_Reserved_3)) { callback->print(subItem, L"ASF_Reserved_3"); }
	else if (IsEqualGUID(ObjectID, ASF_Reserved_4)) { callback->print(subItem, L"ASF_Reserved_4"); }
	else if (IsEqualGUID(ObjectID, ASF_Mutex_Language)) { callback->print(subItem, L"ASF_Mutex_Language"); }
	else if (IsEqualGUID(ObjectID, ASF_Mutex_Bitrate)) { callback->print(subItem, L"ASF_Mutex_Bitrate"); }
	else if (IsEqualGUID(ObjectID, ASF_Mutex_Unknown)) { callback->print(subItem, L"ASF_Mutex_Unknown"); }
	else if (IsEqualGUID(ObjectID, ASF_Bandwidth_Sharing_Exclusive)) { callback->print(subItem, L"ASF_Bandwidth_Sharing_Exclusive"); }
	else if (IsEqualGUID(ObjectID, ASF_Bandwidth_Sharing_Partial)) { callback->print(subItem, L"ASF_Bandwidth_Sharing_Partial"); }
	else if (IsEqualGUID(ObjectID, ASF_Payload_Extension_System_Timecode)) { callback->print(subItem, L"ASF_Payload_Extension_System_Timecode"); }
	else if (IsEqualGUID(ObjectID, ASF_Payload_Extension_System_File_Name)) { callback->print(subItem, L"ASF_Payload_Extension_System_File_Name"); }
	else if (IsEqualGUID(ObjectID, ASF_Payload_Extension_System_Content_Type)) { callback->print(subItem, L"ASF_Payload_Extension_System_Content_Type"); }
	else if (IsEqualGUID(ObjectID, ASF_Payload_Extension_System_Pixel_Aspect_Ratio)) { callback->print(subItem, L"ASF_Payload_Extension_System_Pixel_Aspect_Ratio"); }
	else if (IsEqualGUID(ObjectID, ASF_Payload_Extension_System_Sample_Duration)) { callback->print(subItem, L"ASF_Payload_Extension_System_Sample_Duration"); }
	else if (IsEqualGUID(ObjectID, ASF_Payload_Extension_System_Encryption_Sample_ID)) { callback->print(subItem, L"ASF_Payload_Extension_System_Encryption_Sample_ID"); }
	else
	{
		LPOLESTR str;
		if (SUCCEEDED(StringFromIID(ObjectID, &str)))
		{
			callback->print(subItem, str);
			::CoTaskMemFree(str);
		}
	}

	if (IsEqualGUID(ObjectID, GUID_NULL)) {
		return E_FAIL;
	}

	uint64_t size = 0;
	if (endPos < offset + pos + sizeof(size)) return E_FAIL;
	size = *reinterpret_cast<const uint64_t*>(base + offset + pos);
	pos += sizeof(size);

	// size 
	subItem++;
	{
		wchar_t temp[32] = {};
		_ui64tow_s(size, temp, _countof(temp), 10);
		callback->print(subItem, temp);
	}

	size_t childPos = 0;
	if (offset + pos + size <= endPos)
	{
		// data 
		subItem++;
		void putDataAsByteArray(int subItem, const uint8_t* base_addr, uint64_t offset, uint64_t size, callback_cls* callback, const agalia::config* config);
		putDataAsByteArray(subItem, base, offset + pos, size, callback, config);
		callback->set_extra_data(offset + pos, size);

		// value 
		subItem++;
		if (IsEqualGUID(ObjectID, ASF_Header_Object))
		{
			childPos = print_ASF_Header_Object(subItem, base, offset, size, callback);
		}
		else if (IsEqualGUID(ObjectID, ASF_Content_Description_Object))
		{
			childPos = print_ASF_Content_Description_Object(subItem, base, offset, size, callback);
		}
	}

	callback->next();

	if (childPos)
	{
		parseAsfObject(base, offset + childPos, offset + size, offset, callback, config);
	}

	*object_size = size;
	return S_OK;
}


void parseAsfObject(const uint8_t* base, uint64_t offset, uint64_t endPos, uint64_t parent, callback_cls* callback, const agalia::config* config)
{
	while (offset < endPos)
	{
		uint64_t size = 0;
		HRESULT result = analyzeAsfObject(&size, base, offset, endPos, parent, callback, config);
		if (FAILED(result)) {
			break;
		}

		if (size == 0) {
			break;
		}

		if (SIZE_MAX <= offset + size) {
			break;
		}
		offset += static_cast<size_t>(size);

		config->is_abort(agalia::config::throw_on_abort);
	}
}

#include "image_file.h"

bool image_file_asf::is_supported(const void* buf, uint64_t size)
{
	if (buf == nullptr) return false;
	if (size < sizeof(GUID)) return false;

	GUID ObjectID = *reinterpret_cast<const GUID*>(buf);

	bool ret = false;
	if (IsEqualGUID(ObjectID, ASF_Header_Object))
	{
		ret = true;
	}

	return ret;
}

void image_file_asf::parse(callback_cls* callback, const agalia::config* config)
{
	callback->insert_column(96, L"offset");
	callback->insert_column(96, L"parent");
	callback->insert_column(256, L"Object ID");
	int size_column_number = callback->insert_column(96, L"size");
	callback->set_right_align(size_column_number, true);
	callback->insert_column(128, L"data");
	callback->insert_column(320, L"value");
	callback->next();

	parseAsfObject(this->base_addr, this->base_offset, this->base_offset + this->data_size, SIZE_MAX, callback, config);
}
