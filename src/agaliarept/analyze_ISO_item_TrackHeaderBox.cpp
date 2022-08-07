#include "pch.h"
#include "analyze_ISO_item_TrackHeaderBox.h"

/* Seconds between 00:00 1904-01-01 and 00:00 1970-01-01 */
const unsigned long SecsUntil1970 = 2082844800;

using namespace analyze_ISO;

item_TrackHeaderBox::item_TrackHeaderBox(const agaliaContainer* image, uint64_t offset, uint64_t size, uint64_t endpos, uint32_t parent)
	:item_Box(image, offset, size, endpos, parent)
{

}

item_TrackHeaderBox::~item_TrackHeaderBox()
{

}

HRESULT item_TrackHeaderBox::getItemPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT item_TrackHeaderBox::getItemPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_creation_time: name = L"creation_time"; break;
	case prop_modification_time: name = L"modification_time"; break;
	case prop_track_ID: name = L"track_ID"; break;
	case prop_reserved1: name = L"reserved"; break;
	case prop_duration: name = L"duration"; break;
	case prop_reserved2: name = L"reserved"; break;
	case prop_layer: name = L"layer"; break;
	case prop_alternate_group: name = L"alternate_group"; break;
	case prop_volume: name = L"volume"; break;
	case prop_reserved3: name = L"reserved"; break;
	case prop_matrix: name = L"matrix"; break;
	case prop_width: name = L"width"; break;
	case prop_height: name = L"height"; break;
	default:
		return __super::getItemPropName(index, str);
	}

	*str = agaliaString::create(name);
	return S_OK;
}

HRESULT item_TrackHeaderBox::getItemPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	std::wstringstream temp;
	if (index == prop_version)
	{
		uint64_t box_data_offset = Box::getDataOffset(image, getOffset());

		TrackHeaderBox mvhd = {};
		auto hr = image->ReadData(&mvhd, getOffset() + box_data_offset, sizeof(mvhd.version));
		if (FAILED(hr)) return hr;

		format_dec(temp, mvhd.version);
	}
	else if (index == prop_creation_time)
	{
		uint64_t box_data_offset = Box::getDataOffset(image, getOffset());

		TrackHeaderBox mvhd = {};
		auto hr = image->ReadData(&mvhd, getOffset() + box_data_offset, sizeof(mvhd.version));
		if (FAILED(hr)) return hr;

		switch (mvhd.version)
		{
		case 0:
			hr = image->ReadData(&mvhd.version0.creation_time, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version0.creation_time), sizeof(mvhd.version0.creation_time));
			if (FAILED(hr)) return hr;
			format_time32(temp, agalia_byteswap(mvhd.version0.creation_time) - SecsUntil1970);
			break;

		case 1:
			hr = image->ReadData(&mvhd.version1.creation_time, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version1.creation_time), sizeof(mvhd.version1.creation_time));
			if (FAILED(hr)) return hr;
			format_time64(temp, agalia_byteswap(mvhd.version1.creation_time) - SecsUntil1970);
			break;
		}
	}
	else if (index == prop_modification_time)
	{
		uint64_t box_data_offset = Box::getDataOffset(image, getOffset());

		TrackHeaderBox mvhd = {};
		auto hr = image->ReadData(&mvhd, getOffset() + box_data_offset, sizeof(mvhd.version));
		if (FAILED(hr)) return hr;

		switch (mvhd.version)
		{
		case 0:
			hr = image->ReadData(&mvhd.version0.modification_time, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version0.modification_time), sizeof(mvhd.version0.modification_time));
			if (FAILED(hr)) return hr;
			format_time32(temp, agalia_byteswap(mvhd.version0.modification_time) - SecsUntil1970);
			break;

		case 1:
			hr = image->ReadData(&mvhd.version1.modification_time, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version1.modification_time), sizeof(mvhd.version1.modification_time));
			if (FAILED(hr)) return hr;
			format_time64(temp, agalia_byteswap(mvhd.version1.modification_time) - SecsUntil1970);
			break;
		}
	}
	else if (index == prop_track_ID)
	{
		uint64_t box_data_offset = Box::getDataOffset(image, getOffset());

		TrackHeaderBox mvhd = {};
		auto hr = image->ReadData(&mvhd, getOffset() + box_data_offset, sizeof(mvhd.version));
		if (FAILED(hr)) return hr;

		switch (mvhd.version)
		{
		case 0:
			hr = image->ReadData(&mvhd.version0.track_ID, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version0.track_ID), sizeof(mvhd.version0.track_ID));
			if (FAILED(hr)) return hr;
			format_dec(temp, agalia_byteswap(mvhd.version0.track_ID));
			break;

		case 1:
			hr = image->ReadData(&mvhd.version1.track_ID, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version1.track_ID), sizeof(mvhd.version1.track_ID));
			if (FAILED(hr)) return hr;
			format_dec(temp, agalia_byteswap(mvhd.version1.track_ID));
			break;
		}
	}
	else if (index == prop_reserved1)
	{
		uint64_t box_data_offset = Box::getDataOffset(image, getOffset());

		TrackHeaderBox mvhd = {};
		auto hr = image->ReadData(&mvhd, getOffset() + box_data_offset, sizeof(mvhd.version));
		if (FAILED(hr)) return hr;

		switch (mvhd.version)
		{
		case 0:
			hr = image->ReadData(&mvhd.version0.reserved1, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version0.reserved1), sizeof(mvhd.version0.reserved1));
			if (FAILED(hr)) return hr;
			format_dec(temp, agalia_byteswap(mvhd.version0.reserved1));
			break;

		case 1:
			hr = image->ReadData(&mvhd.version1.reserved1, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version1.reserved1), sizeof(mvhd.version1.reserved1));
			if (FAILED(hr)) return hr;
			format_dec(temp, agalia_byteswap(mvhd.version1.reserved1));
			break;
		}
	}
	else if (index == prop_duration)
	{
		uint64_t box_data_offset = Box::getDataOffset(image, getOffset());

		TrackHeaderBox mvhd = {};
		auto hr = image->ReadData(&mvhd, getOffset() + box_data_offset, sizeof(mvhd.version));
		if (FAILED(hr)) return hr;

		switch (mvhd.version)
		{
		case 0:
			hr = image->ReadData(&mvhd.version0.duration, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version0.duration), sizeof(mvhd.version0.duration));
			if (FAILED(hr)) return hr;
			format_dec(temp, agalia_byteswap(mvhd.version0.duration));
			break;

		case 1:
			hr = image->ReadData(&mvhd.version1.duration, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version1.duration), sizeof(mvhd.version1.duration));
			if (FAILED(hr)) return hr;
			format_dec(temp, agalia_byteswap(mvhd.version1.duration));
			break;
		}
	}
	else if (index == prop_reserved2)
	{
		uint64_t box_data_offset = Box::getDataOffset(image, getOffset());

		TrackHeaderBox mvhd = {};
		auto hr = image->ReadData(&mvhd, getOffset() + box_data_offset, sizeof(mvhd.version));
		if (FAILED(hr)) return hr;

		switch (mvhd.version)
		{
		case 0:
			hr = image->ReadData(&mvhd.version0.reserved2, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version0.reserved2), sizeof(mvhd.version0.reserved2));
			if (FAILED(hr)) return hr;
			format_dec(temp, agalia_byteswap(mvhd.version0.reserved2[0]));
			temp << L", ";
			format_dec(temp, agalia_byteswap(mvhd.version0.reserved2[1]));
			break;

		case 1:
			hr = image->ReadData(&mvhd.version1.reserved2, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version1.reserved2), sizeof(mvhd.version1.reserved2));
			if (FAILED(hr)) return hr;
			format_dec(temp, agalia_byteswap(mvhd.version1.reserved2[0]));
			temp << L", ";
			format_dec(temp, agalia_byteswap(mvhd.version1.reserved2[1]));
			break;
		}
	}
	else if (index == prop_layer)
	{
		uint64_t box_data_offset = Box::getDataOffset(image, getOffset());

		TrackHeaderBox mvhd = {};
		auto hr = image->ReadData(&mvhd, getOffset() + box_data_offset, sizeof(mvhd.version));
		if (FAILED(hr)) return hr;

		switch (mvhd.version)
		{
		case 0:
			hr = image->ReadData(&mvhd.version0.layer, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version0.layer), sizeof(mvhd.version0.layer));
			if (FAILED(hr)) return hr;
			format_dec(temp, agalia_byteswap(mvhd.version0.layer));
			break;

		case 1:
			hr = image->ReadData(&mvhd.version1.layer, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version1.layer), sizeof(mvhd.version1.layer));
			if (FAILED(hr)) return hr;
			format_dec(temp, agalia_byteswap(mvhd.version1.layer));
			break;
		}
	}
	else if (index == prop_alternate_group)
	{
		uint64_t box_data_offset = Box::getDataOffset(image, getOffset());

		TrackHeaderBox mvhd = {};
		auto hr = image->ReadData(&mvhd, getOffset() + box_data_offset, sizeof(mvhd.version));
		if (FAILED(hr)) return hr;

		switch (mvhd.version)
		{
		case 0:
			hr = image->ReadData(&mvhd.version0.alternate_group, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version0.alternate_group), sizeof(mvhd.version0.alternate_group));
			if (FAILED(hr)) return hr;
			format_dec(temp, agalia_byteswap(mvhd.version0.alternate_group));
			break;

		case 1:
			hr = image->ReadData(&mvhd.version1.alternate_group, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version1.alternate_group), sizeof(mvhd.version1.alternate_group));
			if (FAILED(hr)) return hr;
			format_dec(temp, agalia_byteswap(mvhd.version1.alternate_group));
			break;
		}
	}
	else if (index == prop_volume)
	{
		uint64_t box_data_offset = Box::getDataOffset(image, getOffset());

		TrackHeaderBox mvhd = {};
		auto hr = image->ReadData(&mvhd, getOffset() + box_data_offset, sizeof(mvhd.version));
		if (FAILED(hr)) return hr;

		switch (mvhd.version)
		{
		case 0:
			hr = image->ReadData(&mvhd.version0.volume, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version0.volume), sizeof(mvhd.version0.volume));
			if (FAILED(hr)) return hr;
			temp << L"0x";
			format_hex(temp, agalia_byteswap(mvhd.version0.volume));
			break;

		case 1:
			hr = image->ReadData(&mvhd.version1.volume, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version1.volume), sizeof(mvhd.version1.volume));
			if (FAILED(hr)) return hr;
			temp << L"0x";
			format_hex(temp, agalia_byteswap(mvhd.version1.volume));
			break;
		}
	}
	else if (index == prop_reserved3)
	{
		uint64_t box_data_offset = Box::getDataOffset(image, getOffset());

		TrackHeaderBox mvhd = {};
		auto hr = image->ReadData(&mvhd, getOffset() + box_data_offset, sizeof(mvhd.version));
		if (FAILED(hr)) return hr;

		switch (mvhd.version)
		{
		case 0:
			hr = image->ReadData(&mvhd.version0.reserved3, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version0.reserved3), sizeof(mvhd.version0.reserved3));
			if (FAILED(hr)) return hr;
			format_dec(temp, agalia_byteswap(mvhd.version0.reserved3));
			break;

		case 1:
			hr = image->ReadData(&mvhd.version1.reserved3, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version1.reserved3), sizeof(mvhd.version1.reserved3));
			if (FAILED(hr)) return hr;
			format_dec(temp, agalia_byteswap(mvhd.version1.reserved3));
			break;
		}
	}
	else if (index == prop_matrix)
	{
		uint64_t box_data_offset = Box::getDataOffset(image, getOffset());

		TrackHeaderBox mvhd = {};
		auto hr = image->ReadData(&mvhd, getOffset() + box_data_offset, sizeof(mvhd.version));
		if (FAILED(hr)) return hr;

		switch (mvhd.version)
		{
		case 0:
			hr = image->ReadData(&mvhd.version0.matrix, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version0.matrix), sizeof(mvhd.version0.matrix));
			if (FAILED(hr)) return hr;
			for (size_t i = 0; i < _countof(mvhd.version0.matrix); i++)
			{
				temp << ((i == 0) ? L"0x" : L", 0x");
				format_hex(temp, agalia_byteswap(mvhd.version0.matrix[i]));
			}
			break;

		case 1:
			hr = image->ReadData(&mvhd.version1.matrix, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version1.matrix), sizeof(mvhd.version1.matrix));
			if (FAILED(hr)) return hr;
			for (size_t i = 0; i < _countof(mvhd.version1.matrix); i++)
			{
				temp << ((i == 0) ? L"0x" : L", 0x");
				format_hex(temp, agalia_byteswap(mvhd.version1.matrix[i]));
			}
			break;
		}
	}
	else if (index == prop_width)
	{
		uint64_t box_data_offset = Box::getDataOffset(image, getOffset());

		TrackHeaderBox mvhd = {};
		auto hr = image->ReadData(&mvhd, getOffset() + box_data_offset, sizeof(mvhd.version));
		if (FAILED(hr)) return hr;

		switch (mvhd.version)
		{
		case 0:
			hr = image->ReadData(&mvhd.version0.width, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version0.width), sizeof(mvhd.version0.width));
			if (FAILED(hr)) return hr;
			format_fixed(temp, (double)agalia_byteswap(mvhd.version0.width) / 0x10000);
			break;

		case 1:
			hr = image->ReadData(&mvhd.version1.width, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version1.width), sizeof(mvhd.version1.width));
			if (FAILED(hr)) return hr;
			format_fixed(temp, (double)agalia_byteswap(mvhd.version1.width) / 0x10000);
			break;
		}
	}
	else if (index == prop_height)
	{
		uint64_t box_data_offset = Box::getDataOffset(image, getOffset());

		TrackHeaderBox mvhd = {};
		auto hr = image->ReadData(&mvhd, getOffset() + box_data_offset, sizeof(mvhd.version));
		if (FAILED(hr)) return hr;

		switch (mvhd.version)
		{
		case 0:
			hr = image->ReadData(&mvhd.version0.height, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version0.height), sizeof(mvhd.version0.height));
			if (FAILED(hr)) return hr;
			format_fixed(temp, (double)agalia_byteswap(mvhd.version0.height) / 0x10000);
			break;

		case 1:
			hr = image->ReadData(&mvhd.version1.height, getOffset() + box_data_offset + offsetof(TrackHeaderBox, version1.height), sizeof(mvhd.version1.height));
			if (FAILED(hr)) return hr;
			format_fixed(temp, (double)agalia_byteswap(mvhd.version1.height) / 0x10000);
			break;
		}
	}
	else
	{
		return __super::getItemPropValue(index, str);
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}

