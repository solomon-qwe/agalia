#include "pch.h"
#include "analyze_DCM_item_elem.h"

#include "analyze_DCM_util.h"

using namespace analyze_DCM;


// SpecificCharacterSetの文字列をリスト構造のコードページに変換
void interpretSpecificCharacterSet(std::list<int>* codepage, const char* str)
{
	// 末尾のスペースをトリム 
	std::string strbuf = str;
	size_t trim_length = strbuf.find_last_not_of(' ');
	strbuf.erase(trim_length + 1);

	// std::getline の入力とするため istream に変換 
	std::istringstream stream(strbuf);

	// 区切り文字ごとに分割してコードページ変換 
	std::string token;
	while (std::getline(stream, token, '\\'))
	{
		int cp = 0;

		if (token == "" || token == "ISO 2022 IR 6") {
			cp = agalia::CP_US_ASCII;
		}
		else if (token == "ISO_IR 100" || token == "ISO 2022 IR 100") {
			cp = agalia::CP_LATIN1;
		}
		else if (token == "ISO_IR 144" || token == "ISO 2022 IR 144") {
			cp = agalia::CP_CYRILLIC;
		}
		else if (token == "ISO_IR 127" || token == "ISO 2022 IR 127") {
			cp = agalia::CP_ARABIC;
		}
		else if (token == "ISO_IR 126" || token == "ISO 2022 IR 126") {
			cp = agalia::CP_GREEK;
		}
		else if (token == "ISO_IR 138" || token == "ISO 2022 IR 138") {
			cp = agalia::CP_HEBREW;
		}
		else if (token == "ISO_IR 13" || token == "ISO 2022 IR 13") {
			cp = agalia::CP_JIS;
		}
		else if (token == "ISO 2022 IR 87") {
			cp = agalia::CP_JIS;
		}
		else if (token == "ISO 2022 IR 159") {
			cp = agalia::CP_JIS;
		}
		else if (token == "ISO 2022 IR 149") {
			cp = agalia::CP_KOREAN;
		}
		else if (token == "ISO_IR 192") {
			cp = CP_UTF8;	// Unicode in UTF-8 
		}
		else if (token == "GB18030") {
			cp = agalia::CP_GB18030;
		}

		codepage->push_back(cp);
	}
}


HRESULT format_tag(std::wstringstream& dst, const container_DCM_Impl* image, const agaliaItem* item)
{
	dicom_element elem(image, item->getOffset());

	uint16_t group = 0, element = 0;
	auto hr = elem.getGroup(&group);
	if (FAILED(hr)) return hr;
	hr = elem.getElement(&element);
	if (FAILED(hr)) return hr;

	format_hex(dst, group);
	dst << L",";
	format_hex(dst, element);

	return S_OK;
}





_agaliaItemDICOMElement::_agaliaItemDICOMElement(const container_DCM_Impl* image, uint64_t offset, uint64_t size)
	: _agaliaItemDICOMBase(image, offset, size)
{
}


_agaliaItemDICOMElement::~_agaliaItemDICOMElement()
{
}



HRESULT _agaliaItemDICOMElement::getItemName(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	std::wstringstream temp;
	temp << L"(";
	auto hr = format_tag(temp, image, this);
	temp << L")";
	if (FAILED(hr)) return hr;

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}



HRESULT _agaliaItemDICOMElement::getItemPropCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = prop_max;
	return S_OK;
}



HRESULT _agaliaItemDICOMElement::getItemPropName(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case prop_offset:		name = L"Offset";		break;
	case prop_tag:			name = L"Tag";			break;
	case prop_name:			name = L"Name";			break;
	case prop_vr:			name = L"VR";			break;
	case prop_length:		name = L"Length";		break;
	case prop_value_offset: name = L"Value Offset"; break;
	case prop_value:		name = L"Value";		break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT _agaliaItemDICOMElement::getItemPropValue(uint32_t index, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	std::wstringstream temp;
	if (index == prop_offset)
	{
		format_dec(temp, getOffset());
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_tag)
	{
		std::wstring nest;
		nest.assign(layer, L'>');

		temp << nest << L"(";
		auto hr = format_tag(temp, image, this);
		temp << L")";
		if (FAILED(hr)) return hr;

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_name)
	{
		agaliaStringPtr name;
		auto hr = getItemName(&name);
		if (FAILED(hr)) return hr;

		dcm_dic_elem_iterator it = g_dcm_dic_elem->find(name->GetData());
		if (it == g_dcm_dic_elem->end())
			return E_FAIL;	// 辞書に見つからなかった 

		*str = agaliaString::create(it->second.name.c_str());
		return S_OK;
	}
	else if (index == prop_vr)
	{
		dicom_element elem(image, getOffset());

		uint16_t vrOrg = 0;
		auto hr = elem.getFormatedVR(&vrOrg);
		if (FAILED(hr)) return hr;
		format_asc(temp, vrOrg);

		uint16_t vrPref = 0;
		if (SUCCEEDED(elem.getPreferredVR(&vrPref)))
		{
			if (vrOrg != vrPref)
			{
				temp << L"->";
				format_asc(temp, vrPref);
			}
		}

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_length)
	{
		dicom_element elem(image, getOffset());

		uint32_t length = 0;
		auto hr = elem.getValueLength(&length);
		if (FAILED(hr)) return hr;

		if (length == 0xFFFFFFFF) {
			format_hex(temp, length, 8);
		}
		else {
			format_dec(temp, length);
		}

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_value_offset)
	{
		dicom_element elem(image, getOffset());

		uint32_t length = 0;
		auto hr = elem.getValueLength(&length);
		if (FAILED(hr)) return hr;

		if (length != 0) {
			uint64_t value_offset = 0;
			getValueAreaOffset(&value_offset);
			format_dec(temp, value_offset);
		}

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (index == prop_value)
	{
		dicom_element elem(image, getOffset());

		uint64_t value_offset = 0;
		auto hr = elem.getValueOffset(&value_offset);
		if (FAILED(hr)) return hr;
		value_offset += getOffset();

		uint32_t value_size = 0;
		hr = elem.getValueLength(&value_size);
		if (FAILED(hr)) return hr;

		uint16_t vr = 0;
		if (FAILED(elem.getPreferredVR(&vr)))
		{
			hr = elem.getFormatedVR(&vr);
			if (FAILED(hr)) return hr;
		}

		hr = format_dicm_value(temp, image, value_offset, value_size, vr, codepage);
		if (FAILED(hr)) return hr;
		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}

	return E_INVALIDARG;
}



HRESULT _agaliaItemDICOMElement::getChildItem(uint32_t sibling, agaliaItem** child) const
{
	if (sibling != 0) return E_FAIL;

	dicom_element elem(image, getOffset());

	uint16_t group = 0;
	uint16_t element = 0;
	uint16_t vr = 0;
	uint32_t value_length = 0;
	auto hr = elem.getGroup(&group);
	if (FAILED(hr)) return hr;
	hr = elem.getElement(&element);
	if (FAILED(hr)) return hr;
	hr = elem.getVR(&vr);
	if (FAILED(hr)) return hr;
	hr = elem.getValueLength(&value_length);
	if (FAILED(hr)) return hr;

	if (agalia_byteswap(vr) == 'SQ')
	{
		if (value_length == 0)
			return E_FAIL;	// サイズがゼロのシーケンスであれば中断 

		uint64_t child_data_offset = 0;
		hr = elem.getValueOffset(&child_data_offset);
		if (FAILED(hr)) return hr;
		child_data_offset += getOffset();

		dicom_element child_elem(image, child_data_offset);

		uint64_t child_data_size = 0;
		hr = child_elem.getElementSize(&child_data_size);
		if (FAILED(hr)) return hr;

		// 子アイテムを生成 
		auto item = new _agaliaItemDICOMElement(image, child_data_offset, child_data_size);
		item->owner = agalia_byteswap(vr);
		item->layer = layer + 1;
		if (value_length == 0xFFFFFFFF) {
			item->endPos = endPos;
			item->is_undefine_length_sequence = true;
		}
		else {
			item->endPos = getOffset() + getSize();
		}
		std::copy(codepage.begin(), codepage.end(), std::back_inserter(item->codepage));

		*child = item;
		return S_OK;
	}
	else if (!(group == 0xFFFE && element == 0xE000) && value_length == 0xFFFFFFFF)
	{
		uint64_t child_data_offset = 0;
		hr = elem.getValueOffset(&child_data_offset);
		if (FAILED(hr)) return hr;
		child_data_offset += getOffset();

		dicom_element child_elem(image, child_data_offset);

		uint64_t child_data_size = 0;
		hr = child_elem.getElementSize(&child_data_size);
		if (FAILED(hr)) return hr;

		// 子アイテムを生成 
		auto item = new _agaliaItemDICOMElement(image, child_data_offset, child_data_size);
		item->owner = agalia_byteswap(vr);
		item->layer = layer;
		item->endPos = endPos;
		item->is_undefine_length_sequence = is_undefine_length_sequence;
		std::copy(codepage.begin(), codepage.end(), std::back_inserter(item->codepage));

		*child = item;
		return S_OK;
	}

	return E_FAIL;
}



HRESULT _agaliaItemDICOMElement::getNextItem(agaliaItem** next) const
{
	if (next == nullptr) return E_POINTER;

	dicom_element elem(image, getOffset());

	// タグ情報を取得 
	uint16_t group = 0;
	uint16_t element = 0;
	uint16_t vr = 0;
	uint32_t value_length = 0;
	auto hr = elem.getGroup(&group);
	if (FAILED(hr)) return hr;
	hr = elem.getElement(&element);
	if (FAILED(hr)) return hr;
	hr = elem.getVR(&vr);
	if (FAILED(hr)) return hr;
	hr = elem.getValueLength(&value_length);
	if (FAILED(hr)) return hr;

	uint64_t next_data_offset = 0;

	if (group == 0xFFFE && element == 0xE0DD) {
		// (FFFE,E0DD) であった場合はシーケンス終端 
		return E_FAIL;
	}
	else if (owner == 'SQ' && group == 0xFFFE && element == 0xE000) {
		// シーケンス内のデータ区切り(FFFE,E000)の場合は、次のアイテムはデータ集合の中  
		uint64_t value_offset = 0;
		hr = elem.getValueOffset(&value_offset);
		if (FAILED(hr)) return hr;
		next_data_offset = getOffset() + value_offset;
	}
	else if (!(group == 0xFFFE && element == 0xE000) && value_length == 0xFFFFFFFF) {
		// 未定義長のシーケンスであれば、次のアイテムは、終端の子アイテムの後ろ 
		agaliaPtr<agaliaItem> child;
		hr = getChildItem(0, &child);
		if (FAILED(hr)) return hr;
		agaliaPtr<agaliaItem> e;
		while (SUCCEEDED(child->getNextItem(&e))) {
			child = e.detach();
		}
		uint64_t last_offset = 0, last_size = 0;
		child->getValueAreaOffset(&last_offset);
		child->getValueAreaSize(&last_size);
		next_data_offset = last_offset + last_size;
	}
	else {
		// 通常は次のアイテムは現在のアイテムの後ろ 
		uint64_t elem_size = 0;
		hr = elem.getElementSize(&elem_size);
		if (FAILED(hr)) return hr;
		next_data_offset = getOffset() + elem_size;
	}

	if (endPos <= next_data_offset)
		return E_FAIL;	// termination  

	// 次のアイテムの要素を準備 
	dicom_element next_elem(image, next_data_offset);

	uint64_t next_data_size = 0;
	hr = next_elem.getElementSize(&next_data_size);
	if (FAILED(hr)) return hr;

	// 次のアイテムが(0008,0005)であれば特定文字集合を取得しておく 
	std::list<int> next_codepage;
	uint16_t next_group = 0, next_element = 0;
	if (SUCCEEDED(next_elem.getGroup(&next_group)) &&
		SUCCEEDED(next_elem.getElement(&next_element)) &&
		next_group == 0x0008 && next_element == 0x0005)
	{
		uint32_t value_size = 0;
		uint64_t value_offset = 0;
		if (SUCCEEDED(next_elem.getValueLength(&value_size)) &&
			SUCCEEDED(next_elem.getValueOffset(&value_offset)))
		{
			CHeapPtr<char> buf;
			if (buf.AllocateBytes(size_t(value_size) + 1))
			{
				if (SUCCEEDED(image->ReadData(buf, next_data_offset + value_offset, value_size)))
				{
					buf[value_size] = '\0';
					interpretSpecificCharacterSet(&next_codepage, buf.m_pData);
				}
			}
		}
	}

	// 次のアイテムを生成 
	auto item = new _agaliaItemDICOMElement(image, next_data_offset, next_data_size);
	item->owner = owner;
	item->layer = layer;
	item->endPos = endPos;
	item->is_undefine_length_sequence = is_undefine_length_sequence;
	if (next_codepage.size()) {
		std::copy(next_codepage.begin(), next_codepage.end(), std::back_inserter(item->codepage));
	}
	else {
		std::copy(codepage.begin(), codepage.end(), std::back_inserter(item->codepage));
	}

	*next = item;
	return S_OK;
}



HRESULT _agaliaItemDICOMElement::getValueAreaOffset(uint64_t* offset) const
{
	if (offset == nullptr) return E_POINTER;

	dicom_element elem(image, getOffset());
	uint64_t value_offset = 0;
	auto hr = elem.getValueOffset(&value_offset);
	if (FAILED(hr)) return hr;

	*offset = getOffset() + value_offset;
	return S_OK;
}



HRESULT _agaliaItemDICOMElement::getValueAreaSize(uint64_t* size) const
{
	if (size == nullptr) return E_POINTER;

	dicom_element elem(image, getOffset());
	uint32_t value_size = 0;
	auto hr = elem.getValueLength(&value_size);
	if (FAILED(hr)) return hr;

	*size = value_size;
	return S_OK;
}



HRESULT _agaliaItemDICOMElement::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	if (column == column_offset) {
		std::wstringstream temp;
		format_hex(temp, getOffset(), 8);

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (column == column_tag) {
		std::wstring nest;
		nest.assign(layer, L'>');

		std::wstringstream temp;
		temp << nest;
		auto hr = format_tag(temp, image, this);
		if (FAILED(hr)) return hr;

		*str = agaliaString::create(temp.str().c_str());
		return S_OK;
	}
	else if (column == column_name) {
		return getItemPropValue(prop_name, str);
	}
	else if (column == column_vr) {
		return getItemPropValue(prop_vr, str);
	}
	else if (column == column_length) {
		return getItemPropValue(prop_length, str);
	}
	else if (column == column_value) {
		return getItemPropValue(prop_value, str);
	}

	return E_INVALIDARG;
}
