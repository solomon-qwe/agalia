#include "pch.h"
#include "analyze_JPEG_item_SOS.h"

#include "analyze_JPEG_util.h"

#include "jpeg_def.h"
#include "byteswap.h"

using namespace analyze_JPEG;




item_SOS::item_SOS(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type)
	:item_Marker(image, offset, size, type)
{

}

item_SOS::~item_SOS()
{

}


HRESULT item_SOS::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_value)
	{
		JPEGSEGMENT_SOS sos = {};
		auto hr = image->ReadData(&sos, getOffset(), sizeof(sos));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"Ls=" << agalia_byteswap(sos.Ls);
		temp << L", Ns=" << agalia_byteswap(sos.Ns);
		*str = agaliaString::create(temp.str().c_str());

		return S_OK;
	}

	return __super::getColumnValue(column, str);
}



HRESULT item_SOS::getAdditionalInfoCount(uint32_t* row) const
{
	JPEGSEGMENT_SOS sos = {};
	auto hr = image->ReadData(&sos, getOffset(), sizeof(sos));
	if (FAILED(hr)) return hr;

	*row = sos.Ns + 1;

	return S_OK;
}



HRESULT item_SOS::getAdditionalInfoValue(uint32_t row, agaliaString** str) const
{
	CHeapPtr<JPEGSEGMENT_SOS> sos;
	if (!sos.AllocateBytes(sizeof(JPEGSEGMENT_SOS))) return E_OUTOFMEMORY;
	auto hr = image->ReadData(sos, getOffset(), sizeof(JPEGSEGMENT_SOS));
	if (FAILED(hr)) return hr;

	uint32_t Ns = agalia_byteswap(sos->Ns);
	if (Ns + 1 < row) {
		return E_FAIL;
	}

	sos.Free();

	size_t suffix_pos = offsetof(JPEGSEGMENT_SOS, comp) + sizeof(JPEGSEGMENT_SOS::comp[0]) * Ns;
	size_t bufsize = suffix_pos + sizeof(JPEGSEGMENT_SOS_Suffix);
	if (!sos.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
	hr = image->ReadData(sos, getOffset(), bufsize);
	if (FAILED(hr)) return hr;

	std::wstringstream temp;
	if (row < Ns)
	{
		temp << L"Cs" << int_to_subscript_wchar(row + 1) << L"=" << sos->comp[row].Cs;
		temp << L", Td" << int_to_subscript_wchar(row + 1) << L"=" << sos->comp[row].Td;
		temp << L", Ta" << int_to_subscript_wchar(row + 1) << L"=" << sos->comp[row].Ta;
	}
	else
	{
		JPEGSEGMENT_SOS_Suffix* suffix = reinterpret_cast<JPEGSEGMENT_SOS_Suffix*>(reinterpret_cast<uint8_t*>(sos.m_pData) + suffix_pos);
		temp << L"Ss=" << suffix->Ss;
		temp << L", Se=" << suffix->Se;
		temp << L", Ah=" << suffix->Ah;
		temp << L", Al=" << suffix->Al;
	}
	*str = agaliaString::create(temp.str().c_str());

	return S_OK;
}



HRESULT item_SOS::getItemPropCount(uint32_t* count) const
{
	auto hr = __super::getItemPropCount(count);
	if (FAILED(hr)) return hr;

	JPEGSEGMENT_SOS sos = {};
	hr = image->ReadData(&sos, getOffset(), sizeof(sos));
	if (FAILED(hr)) return hr;

	*count += 2 + 3 * agalia_byteswap(sos.Ns) + 4;

	return S_OK;
}



HRESULT item_SOS::getItemPropName(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getItemPropName(index, str);
	if (SUCCEEDED(hr)) return hr;

	agaliaStringPtr s;

	if (index == 3) {
		s = agaliaString::create(L"Ls");
	}
	else if (index == 4) {
		s = agaliaString::create(L"Ns");
	}
	else {
		JPEGSEGMENT_SOS sos = {};
		hr = image->ReadData(&sos, getOffset(), sizeof(sos));
		if (FAILED(hr)) return hr;

		uint32_t Ns = agalia_byteswap(sos.Ns);

		if (index == 5 + 3 * Ns) {
			s = agaliaString::create(L"Ss");
		}
		else if (index == 5 + 3 * Ns + 1) {
			s = agaliaString::create(L"Se");
		}
		else if (index == 5 + 3 * Ns + 2) {
			s = agaliaString::create(L"Ah");
		}
		else if (index == 5 + 3 * Ns + 3) {
			s = agaliaString::create(L"Al");
		}
		else if (index <= 5 + 3 * Ns) {
			std::wstringstream temp;
			switch ((index - 5) % 3)
			{
			case 0:
				temp << L"Cs" << int_to_subscript_wchar((index - 5) / 3 + 1);
				break;
			case 1:
				temp << L"Td" << int_to_subscript_wchar((index - 5) / 3 + 1);
				break;
			case 2:
				temp << L"Ta" << int_to_subscript_wchar((index - 5) / 3 + 1);
				break;
			}
			s = agaliaString::create(temp.str().c_str());
		}
	}

	if (!s) return E_FAIL;
	*str = s.detach();

	return S_OK;
}



HRESULT item_SOS::getItemPropValue(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getItemPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	std::wstringstream temp;

	CHeapPtr<JPEGSEGMENT_SOS> sos;
	if (!sos.AllocateBytes(sizeof(JPEGSEGMENT_SOS))) return E_OUTOFMEMORY;
	hr = image->ReadData(sos, getOffset(), sizeof(JPEGSEGMENT_SOS));
	if (FAILED(hr)) return hr;

	if (index == 3) {
		temp << agalia_byteswap(sos->Ls);
	}
	else if (index == 4) {
		temp << agalia_byteswap(sos->Ns);
	}
	else {
		uint32_t Ns = agalia_byteswap(sos->Ns);
		sos.Free();

		size_t suffix_pos = offsetof(JPEGSEGMENT_SOS, comp) + sizeof(JPEGSEGMENT_SOS::comp[0]) * Ns;
		size_t bufsize = suffix_pos + sizeof(JPEGSEGMENT_SOS_Suffix);
		if (!sos.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
		hr = image->ReadData(sos, getOffset(), bufsize);
		if (FAILED(hr)) return hr;
		JPEGSEGMENT_SOS_Suffix* suffix = reinterpret_cast<JPEGSEGMENT_SOS_Suffix*>(reinterpret_cast<uint8_t*>(sos.m_pData) + suffix_pos);

		if (index == 5 + 3 * Ns) {
			temp << suffix->Ss;
		}
		else if (index == 5 + 3 * Ns + 1) {
			temp << suffix->Se;
		}
		else if (index == 5 + 3 * Ns + 2) {
			temp << suffix->Ah;
		}
		else if (index == 5 + 3 * Ns + 3) {
			temp << suffix->Al;
		}
		else if (index <= 5 + 3 * Ns) {
			switch ((index - 5) % 3)
			{
			case 0:
				temp << sos->comp[(index - 5) / 3].Cs;
				break;
			case 1:
				temp << sos->comp[(index - 5) / 3].Td;
				break;
			case 2:
				temp << sos->comp[(index - 5) / 3].Ta;
				break;
			}
		}
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
