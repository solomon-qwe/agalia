#include "pch.h"
#include "analyze_ICC_item_Header.h"

#include "analyze_ICC_item_TagTable.h"
#include "byteswap.h"

using namespace analyze_ICC;

item_Header::item_Header(const container_ICC* image, uint64_t offset, uint64_t size, uint64_t endpos)
	: item_Base(image, offset, size, endpos)
{
}

item_Header::~item_Header()
{
}

HRESULT item_Header::getItemName(agaliaString** str) const
{
	*str = agaliaString::create(L"Profile header");
	return S_OK;
}

HRESULT item_Header::getItemPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_last;
	return S_OK;
}

HRESULT item_Header::getItemPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_profile_size: name = L"Profile size"; break;
	case prop_preferred_cmm_type: name = L"Preferred CMM type"; break;
	case prop_profile_version: name = L"Profile version"; break;
	case prop_profile_device_class: name = L"Profile/device class"; break;
	case prop_data_colour_space: name = L"Data colour space"; break;
	case prop_pcs: name = L"PCS"; break;
	case prop_date_and_time: name = L"Date and time"; break;
	case prop_profile_file_signature: name = L"Profile file signature"; break;
	case prop_primary_platform: name = L"Primary platform"; break;
	case prop_profile_flags: name = L"Profile flags"; break;
	case prop_device_manufacturer: name = L"Device manufacturer"; break;
	case prop_device_model: name = L"Device model"; break;
	case prop_device_attributes: name = L"Device attributes"; break;
	case prop_rendering_intent: name = L"RenderingIntent"; break;
	case prop_pcs_illuminant: name = L"PCS illuminant"; break;
	case prop_profile_creator: name = L"Profile creator"; break;
	case prop_profile_id: name = L"Profile ID"; break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(name);
	return S_OK;
}


HRESULT item_Header::getItemPropValue(uint32_t index, agaliaString** str) const
{
	if (index == prop_profile_size)
	{
		decltype(Header::ProfileSize) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Header, ProfileSize), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		format_dec(temp, agalia_byteswap(val));

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_preferred_cmm_type)
	{
		decltype(Header::PreferredCMMType) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Header, PreferredCMMType), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"\"";
		multibyte_to_widechar((char*) & val, sizeof(val), CP_US_ASCII, temp);
		temp << L"\"";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_profile_version)
	{
		decltype(Header::ProfileVersion) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Header, ProfileVersion), sizeof(val));
		if (FAILED(hr)) return hr;
		val = agalia_byteswap(val);

		std::wstringstream temp;
		temp << std::dec
			<< ((val >> 24) & 0xFF) << L"."
			<< ((val >> 20) & 0x0F) << L"."
			<< ((val >> 16) & 0x0F)
			<< L" (0x" << std::hex << val << L")";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_profile_device_class)
	{
		decltype(Header::ProfileDeviceClass) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Header, ProfileDeviceClass), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"\"";
		multibyte_to_widechar((char*)&val, sizeof(val), CP_US_ASCII, temp);
		temp << L"\"";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_data_colour_space)
	{
		decltype(Header::DataColourSpace) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Header, DataColourSpace), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"\"";
		multibyte_to_widechar((char*)&val, sizeof(val), CP_US_ASCII, temp);
		temp << L"\"";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_pcs)
	{
		decltype(Header::PCS) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Header, PCS), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"\"";
		multibyte_to_widechar((char*)&val, sizeof(val), CP_US_ASCII, temp);
		temp << L"\"";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_date_and_time)
	{
		decltype(Header::DateAndTime) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Header, DateAndTime), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << std::dec << std::setfill(L'0')
			<< agalia_byteswap(val[0]) << L"/" << std::setw(2) << agalia_byteswap(val[1]) << L"/" << std::setw(2) << agalia_byteswap(val[2])
			<< L" "
			<< std::setw(2) << agalia_byteswap(val[3]) << L":" << std::setw(2) << agalia_byteswap(val[4]) << L":" << std::setw(2) << agalia_byteswap(val[5]);

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_profile_file_signature)
	{
		decltype(Header::ProfileFileSignature) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Header, ProfileFileSignature), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"\"";
		multibyte_to_widechar((char*)&val, sizeof(val), CP_US_ASCII, temp);
		temp << L"\"";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_primary_platform)
	{
		decltype(Header::PrimaryPlatform) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Header, PrimaryPlatform), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"\"";
		multibyte_to_widechar((char*)&val, sizeof(val), CP_US_ASCII, temp);
		temp << L"\"";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_profile_flags)
	{
		decltype(Header::ProfileFlags) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Header, ProfileFlags), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"0x";
		format_hex(temp, agalia_byteswap(val));

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_device_manufacturer)
	{
		decltype(Header::DeviceManufacturer) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Header, DeviceManufacturer), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"\"";
		multibyte_to_widechar((char*)&val, sizeof(val), CP_US_ASCII, temp);
		temp << L"\"";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_device_model)
	{
		decltype(Header::DeviceModel) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Header, DeviceModel), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"\"";
		multibyte_to_widechar((char*)&val, sizeof(val), CP_US_ASCII, temp);
		temp << L"\"";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_device_attributes)
	{
		decltype(Header::DeviceAttributes) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Header, DeviceAttributes), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"0x";
		format_hex(temp, agalia_byteswap(val));

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_rendering_intent)
	{
		decltype(Header::RenderingIntent) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Header, RenderingIntent), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"0x";
		format_hex(temp, agalia_byteswap(val));

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_pcs_illuminant)
	{
		decltype(Header::PCSIlluminant) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Header, PCSIlluminant), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		for (size_t i = 0; i < _countof(val); i++)
		{
			format_fixed(temp, (double)agalia_byteswap(val[i]) / 65536);
			if (i != _countof(val) - 1)
				temp << L", ";
		}

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_profile_creator)
	{
		decltype(Header::ProfileCreator) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Header, ProfileCreator), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"\"";
		multibyte_to_widechar((char*)&val, sizeof(val), CP_US_ASCII, temp);
		temp << L"\"";

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_profile_id)
	{
		decltype(Header::ProfileID) val;
		auto hr = image->ReadData(&val, getOffset() + offsetof(Header, ProfileID), sizeof(val));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		for (size_t i = 0; i < sizeof(val); i++)
			format_hex(temp, val[i]);

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}

	return E_FAIL;
}

HRESULT item_Header::getChildItem(uint32_t sibling, agaliaItem** child) const
{
	if (sibling != 0)
		return E_INVALIDARG;
	if (child == nullptr)
		return E_POINTER;

	*child = new item_TagTable(image, getOffset() + getSize(), sizeof(uint32_t), endpos);
	return S_OK;
}

HRESULT item_Header::getNextItem(agaliaItem** next) const
{
	UNREFERENCED_PARAMETER(next);
	return E_FAIL;
}

HRESULT item_Header::getColumnValue(uint32_t column, agaliaString** str) const
{
	UNREFERENCED_PARAMETER(column);
	UNREFERENCED_PARAMETER(str);
	return E_FAIL;
}
