#include "stdafx.h"

#include "dcm_common.h"
#include "dcm_DicomDataElement.h"

#include <iomanip>
#include "callback.h"
#include "util.h"

#include <mlang.h>	// for CLSID_CMultiLanguage, etc. 
#include <atlbase.h>	// for CComPtr, CHeapPtr, etc. 

void putDataAsString(int subItem, const void* srcBuf, size_t srcLength, callback_cls* callback)
{
	size_t size = 1 + srcLength + 1 + 1;
	CHeapPtr<char> buf;
	buf.Allocate(size);
	buf[0] = '\"';
	memcpy(buf + 1, srcBuf, srcLength);
	buf[size - 2] = '\0';

	size_t leng = strlen(buf);
	buf[leng] = '\"';
	buf[leng + 1] = '\0';

	callback->print(subItem, buf);
}

void putDataAsUID(int subItem, const void* srcBuf, size_t srcLength, callback_cls* callback)
{
	std::string buf(reinterpret_cast<const char*>(srcBuf), srcLength);

	dcm_dic_uid_iterator it;
	if (findUID(it, buf.c_str()))
	{
		size_t leng = it->second.name.length() + 2 + srcLength + 2;
		CHeapPtr<wchar_t> str;
		str.Allocate(leng);
		swprintf_s(str, leng, L"%s (%S)", it->second.name.c_str(), buf.c_str());
		callback->print(subItem, str);
	}
	else
	{
		putDataAsString(subItem, srcBuf, srcLength, callback);
	}
}

void putDataAsLocalizeString(int subItem, const void* srcBuf, size_t srcLength, uint32_t codepage, callback_cls* callback)
{
	size_t size = 1 + srcLength + 1 + 1;
	CHeapPtr<char> buf;
	buf.Allocate(size);
	buf[0] = '\"';
	memcpy(buf + 1, srcBuf, srcLength);
	buf[size - 2] = '\0';

	size_t leng = strlen(buf);
	buf[leng] = '\"';
	buf[leng + 1] = '\0';

	if (codepage)
	{
		UINT wleng = 0;
		DWORD mode = 0;
		DWORD cp = codepage;
		CComPtr<IMultiLanguage> spMult;
		HRESULT result = ::CoCreateInstance(CLSID_CMultiLanguage, 0, CLSCTX_ALL, IID_IMultiLanguage, (void**)&spMult);
		if (SUCCEEDED(result))
		{
			result = spMult->ConvertStringToUnicode(&mode, cp, buf, nullptr, nullptr, &wleng);
			if (SUCCEEDED(result))
			{
				wleng += 1;
				CHeapPtr<wchar_t> wbuf;
				wbuf.Allocate(wleng);
				wbuf[wleng - 1] = L'\0';
				spMult->ConvertStringToUnicode(&mode, cp, buf, nullptr, wbuf, &wleng);
				callback->set_codepage(codepage);
				callback->print(subItem, wbuf);
			}
		}
	}
	else
	{
		callback->print(subItem, buf);
	}
}

void putDataAsByteArray(int subItem, const void* srcBuf, uint64_t srcLength, callback_cls* callback, const agalia::config* config)
{
	if (srcLength == 0xFFFFFFFF) {
		callback->print(subItem, L"");
		return;
	}

	uint64_t counts = srcLength / sizeof(uint8_t);
	const uint8_t* pVal = reinterpret_cast<const uint8_t*>(srcBuf);

	std::wstring str;
	const size_t limit = config->byte_stream_limit_length;
	str.reserve(limit * 3 + 8);

	str += L"[";
	if (counts != 0)
	{
		wchar_t temp[16];
		fast_itow16(pVal[0], temp);	// 1
		str += temp;

		for (unsigned int i = 1; i < min(limit, counts); i++)
		{
			temp[0] = L',';
			fast_itow16(pVal[i], temp + 1);	// 2,...
			str += temp;
		}

		if (limit < counts) {
			str += L",...";
		}
	}
	str += L"]";

	callback->print(subItem, str);
}

void putDataAsUShortArray(int subItem, const void* srcBuf, size_t srcLength, callback_cls* callback)
{
	if (srcLength == 0xFFFFFFFF) {
		callback->print(subItem, L"");
		return;
	}

	size_t counts = srcLength / sizeof(uint16_t);
	const uint16_t* pVal = reinterpret_cast<const uint16_t*>(srcBuf);

	std::wstringbuf buf;
	std::wostream str(&buf, true);
	str.setf(std::ios::hex, std::ios::basefield);
	str.setf(std::ios::uppercase);
	str.fill('0');

	str << "[";
	if (counts != 0)
	{
		str << std::setw(4) << (int)pVal[0];
		for (unsigned int i = 1; i < counts; i++) {
			str << "," << std::setw(4) << (int)pVal[i];
		}
	}
	str << "]";

	callback->print(subItem, buf.str());
}

template<class T>
void putDataAsIntegerArray(int subItem, const void* srcBuf, size_t srcLength, callback_cls* callback)
{
	if (srcLength == 0xFFFFFFFF) {
		callback->print(subItem, L"");
		return;
	}

	size_t counts = srcLength / sizeof(T);
	if (counts == 0) return;

	const T* pVal = reinterpret_cast<const T*>(srcBuf);

	std::wstringbuf buf;
	std::wostream str(&buf, true);
	str.setf(std::ios::dec, std::ios::basefield);

	if (1 < counts) { str << "["; }
	str << pVal[0];
	for (unsigned int i = 1; i < counts; i++) {
		str << ", " << pVal[i];
	}
	if (1 < counts) { str << "]"; }

	callback->print(subItem, buf.str());
}

template<class T>
void putDataAsFloatArray(int subItem, const void* srcBuf, size_t srcLength, callback_cls* callback)
{
	if (srcLength == 0xFFFFFFFF) {
		callback->print(subItem, L"");
		return;
	}

	size_t counts = srcLength / sizeof(T);
	if (counts == 0) return;

	const T* pVal = reinterpret_cast<const T*>(srcBuf);

	// 指数表示でeは小文字にする 
	std::wstringbuf buf;
	std::wostream str(&buf, true);
	str.setf(std::ios::scientific, std::ios::floatfield | std::ios::uppercase);

	if (1 < counts) { str << "["; }
	str << pVal[0];
	for (unsigned int i = 1; i < counts; i++) {
		str << ", " << pVal[i];
	}
	if (1 < counts) { str << "]"; }

	callback->print(subItem, buf.str());
}

void print_dcm_value(int subItem, const CDicomDataElement& de, callback_cls* callback, const agalia::config* config)
{
	// 実データのアドレス 
	const uint8_t* addr = de.getDataAddr();
	if (addr == nullptr) {
		callback->print(subItem, L"");
		return;
	}

	// 実データのサイズ 
	uint32_t length = 0;
	de.getLength(&length);

	// 実データの型 
	uint16_t vr = 0;
	de.getVR(&vr);
	if (config->force_dictionary_vr == true)
	{
		// 強制的に DataDictionary を参照させる 
		dcm_dic_elem_iterator it;
		if (findVR(it, de.getGroup(), de.getElement()))
		{
			vr = *reinterpret_cast<const uint16_t*>(it->second.vr.data());
		}
	}

	switch (_byteswap_ushort(vr))
	{
	case 'AE': putDataAsString(subItem, addr, length, callback);
		break;
	case 'AS': putDataAsString(subItem, addr, length, callback);
		break;
	case 'AT': putDataAsUShortArray(subItem, addr, length, callback);
		break;
	case 'CS': putDataAsString(subItem, addr, length, callback);
		break;
	case 'DA': putDataAsString(subItem, addr, length, callback);
		break;
	case 'DS': putDataAsString(subItem, addr, length, callback);
		break;
	case 'DT': putDataAsString(subItem, addr, length, callback);
		break;
	case 'FL': putDataAsFloatArray<float>(subItem, addr, length, callback);
		break;
	case 'FD': putDataAsFloatArray<double>(subItem, addr, length, callback);
		break;
	case 'IS': putDataAsString(subItem, addr, length, callback);
		break;
	case 'LO': putDataAsLocalizeString(subItem, addr, length, de.getCodepage(), callback);
		break;
	case 'LT': putDataAsLocalizeString(subItem, addr, length, de.getCodepage(), callback);
		break;
	case 'OB': // OB と OW は値解釈しない 
		break;
	case 'OF': putDataAsFloatArray<float>(subItem, addr, length, callback);
		break;
	case 'OW': // OB と OW は値解釈しない 
		break;
	case 'PN': putDataAsLocalizeString(subItem, addr, length, de.getCodepage(), callback);
		break;
	case 'SH': putDataAsLocalizeString(subItem, addr, length, de.getCodepage(), callback);
		break;
	case 'SL': putDataAsIntegerArray<int32_t>(subItem, addr, length, callback);
		break;
	case 'SQ': // シーケンスなので値解釈しない 
		break;
	case 'SS': putDataAsIntegerArray<int16_t>(subItem, addr, length, callback);
		break;
	case 'ST': putDataAsLocalizeString(subItem, addr, length, de.getCodepage(), callback);
		break;
	case 'TM': putDataAsString(subItem, addr, length, callback);
		break;
	case 'UI': putDataAsUID(subItem, addr, length, callback);
		break;
	case 'UL': putDataAsIntegerArray<uint32_t>(subItem, addr, length, callback);
		break;
	case 'UN': // Unknown なので値解釈しない 
		break;
	case 'US': putDataAsIntegerArray<uint16_t>(subItem, addr, length, callback);
		break;
	case 'UT': putDataAsLocalizeString(subItem, addr, length, de.getCodepage(), callback);
		break;
	}
}

size_t getDataSize(int layer, const CDicomDataElement& de)
{
	uint32_t length = 0;
	de.getLength(&length);
	if (length == 0xFFFFFFFF)
		return 0;

	if (layer != 0 && de.getGroup() == 0xFFFE && de.getElement() == 0xE000)
		return 0;

	return length;
}

// TransferSyntax の UID を enum 値に変換
TransferSyntax interpretTransferSyntax(const CDicomDataElement& de, TransferSyntax current_syntax)
{
	TransferSyntax ret = current_syntax;

	uint32_t length = 0;
	de.getLength(&length);

	char buf[64 + 1] = {};
	memcpy(buf, de.getDataAddr(), min(64U, length));
	std::string str(buf);

	if (str == "1.2.840.10008.1.2") {
		ret = ImplicitVR_LittleEndian;
	}
	else if (str == "1.2.840.10008.1.2.1") {
		ret = ExplicitVR_LittleEndian;
	}
	else if (str == "1.2.840.10008.1.2.1.99") {
		ret = DeflatedExplicitVR_LittleEndian;
	}
	else if (str == "1.2.840.10008.1.2.2") {
		ret = ExplicitVR_BigEndian;
	}
	return ret;
}

// SpecificCharacterSet からコードページに変換 
// 複数指定されていた場合は、最後のものを参照する
// （大抵、後ろの指定は前の指定を包含している、はずという勝手な前提に基づく） 
uint32_t interpretSpecificCharacterSet(const CDicomDataElement& de, uint32_t current_codepage)
{
	uint32_t ret = current_codepage;

	uint32_t length = 0;
	de.getLength(&length);

	std::string strbuf;
	strbuf.assign(reinterpret_cast<const char*>(de.getDataAddr()), length);
	size_t trim_length = strbuf.find_last_not_of(' ');
	strbuf.erase(trim_length + 1);
	std::istringstream str(strbuf);

	std::string token;
	while (std::getline(str, token, '\\'))
	{
		// refer. D.6.2-1 

		if (token == "ISO 2022 IR 13" || token == "ISO_IR 13")
		{
			ret = 50221;	// JIS X 0201 
		}
		else if (token == "ISO 2022 IR 87")
		{
			ret = 50221;	// JIS X 0208 
		}
		else if (token == "ISO 2022 IR 159")
		{
			ret = 50221;	// JIS X 0212 
		}
		else if (token == "ISO_IR 100" || token == "ISO 2022 IR 100")
		{
			ret = 1252;		// Latin-1 
		}
		else if (token == "ISO_IR 192")
		{
			ret = CP_UTF8;
		}
		else
		{
			ret = 0;
		}
	}
	return ret;
}


void print_dcm_offset(int subItem, uint64_t offset, callback_cls* callback)
{
	wchar_t sz[16] = {};
	swprintf_s(sz, L"%.8I64X", offset);

	callback->print(subItem, sz);
}


void print_dcm_tag(int subItem, int layer, const CDicomDataElement& de, callback_cls* callback)
{
	wchar_t sz[16];
	swprintf_s(sz, L"%.4X,%.4X", de.getGroup(), de.getElement());

	std::wstring str(layer, L'>');
	str += sz;

	callback->print(subItem, str);
}


void print_dcm_tag_name(int subItem, const CDicomDataElement& de, callback_cls* callback)
{
	wchar_t sz[16];

	swprintf_s(sz, L"(%.4X,%.4X)", de.getGroup(), de.getElement());
	dcm_dic_elem_iterator it = g_dcm_dic_elem->find(sz);
	if (it != g_dcm_dic_elem->end()) {
		callback->print(subItem, it->second.name);
		return;
	}

	switch (de.getGroup() & 0xFF00)
	{
	case 0x5000:
	case 0x6000:
		if ((de.getGroup() & 0x1) == 0)
		{
			swprintf_s(sz, L"(%.2Xxx,%.4X)", de.getGroup() >> 8, de.getElement());
			it = g_dcm_dic_elem->find(sz);
			if (it != g_dcm_dic_elem->end()) {
				callback->print(subItem, it->second.name);
				return;
			}
		}
		break;
	}

	// 見つからなかった 
	callback->print(subItem, L"");
}


void print_dcm_vr(int subItem, const CDicomDataElement& de, callback_cls* callback, const agalia::config* config)
{
	if (de.getGroup() == 0xFFFE && de.getElement() == 0xE000) {
		callback->print(subItem, L"");
		return;
	}

	uint16_t vr = 0;
	if (FAILED(de.getVR(&vr))) {
		throw E_FAIL;
	}

	char buf[sizeof(uint16_t) + 1] = {};
	*reinterpret_cast<uint16_t*>(buf) = vr;
	std::string str(buf);

	if (config->force_dictionary_vr)
	{
		dcm_dic_elem_iterator it;
		if (findVR(it, de.getGroup(), de.getElement()))
		{
			uint16_t dicVR = *reinterpret_cast<const uint16_t*>(it->second.vr.data());
			if (dicVR != 0 && dicVR != vr)
			{
				*reinterpret_cast<uint16_t*>(buf) = dicVR;
				str += "->";
				str += buf;
			}
		}
	}

	callback->print(subItem, str);
}


void print_dcm_length(int subItem, const CDicomDataElement& de, callback_cls* callback)
{
	uint32_t length = 0;
	if (FAILED(de.getLength(&length))) {
		throw E_FAIL;
	}

	wchar_t sz[16] = {};
	swprintf_s(sz, L"%u", length);

	callback->print(subItem, sz);
}

// データセット解析用クラス 
class dicom_dataset_parser
{
public:
	dicom_dataset_parser() {
		_transferSyntax = ExplicitVR_LittleEndian;
		_codepage = 0;
	}
	virtual ~dicom_dataset_parser() {}

	HRESULT parse(const uint8_t* base, uint64_t offset, uint64_t endPos, int layer, uint64_t* nextPos, bool enableSequenceDelimit, callback_cls* callback, const agalia::config* config);

protected:
	TransferSyntax _transferSyntax;
	uint32_t _codepage;
};

// データセットの解析 
HRESULT dicom_dataset_parser::parse(
	const uint8_t* base,
	uint64_t offset,
	uint64_t endPos,
	int layer,
	uint64_t* nextPos,
	bool enableSequenceDelimit,
	callback_cls* callback,
	const agalia::config* config)
{
	HRESULT result = S_OK;

	try
	{
		uint64_t curPos = offset;

		do
		{
			int subItem = 0;
			print_dcm_offset(subItem++, curPos, callback);

			// Tag (group number + element number)
			uint32_t item_size = 4;
			if (endPos < curPos + item_size) {
				throw E_FAIL;
			}

			CDicomDataElement de(base, curPos, endPos, _transferSyntax, _codepage);

			print_dcm_tag(subItem++, layer, de, callback);
			print_dcm_tag_name(subItem++, de, callback);

			if (de.getGroup() == 0x0002 && de.getElement() == 0x0010)
			{
				// (0002,0010) であった場合、転送構文を解釈する 
				_transferSyntax = interpretTransferSyntax(de, _transferSyntax);
				switch (_transferSyntax)
				{
				case ImplicitVR_LittleEndian:
				case ExplicitVR_LittleEndian:
					de.set_transfer_syntax(_transferSyntax);
					break;
				default:
					throw E_FAIL;	// BigEndian 等は面倒なので非サポート 
				}
			}
			else if (de.getGroup() == 0x0008 && de.getElement() == 0x0005)
			{
				// (0008,0005) であった場合、コードページを解釈する 
				_codepage = interpretSpecificCharacterSet(de, _codepage);
			}

			print_dcm_vr(subItem++, de, callback, config);
			print_dcm_length(subItem++, de, callback);

			uint16_t vr = 0;
			de.getVR(&vr);

			uint32_t length = 0;
			de.getLength(&length);
			bool is_undefine_length = (length == 0xFFFFFFFF);

			if (!is_undefine_length && endPos < de.getDataOffset() + length) {
				throw E_FAIL;
			}

			const uint8_t* data = de.getDataAddr();
			if (data != nullptr)
			{
				putDataAsByteArray(subItem++, data, length, callback, config);
				print_dcm_value(subItem++, de, callback, config);
				if (!is_undefine_length) {
					callback->set_extra_data(curPos + de.getDataOffset(), length);
				}
			}

			callback->next();

			// シーケンスのときは再帰する 
			if (_byteswap_ushort(vr) == 'SQ' && length != 0)
			{
				uint64_t dataOffset = de.getDataOffset();
				uint64_t childStartPos = curPos + dataOffset;
				uint64_t childEndPos = is_undefine_length ? endPos : (curPos + dataOffset + length);
				uint64_t newPos = curPos;
				uint64_t* pNewPos = is_undefine_length ? &newPos : nullptr;

				HRESULT ret_child = parse(base, childStartPos, childEndPos, layer + 1, pNewPos, is_undefine_length, callback, config);
				if (is_undefine_length && FAILED(ret_child)) {
					throw E_FAIL;
				}

				if (pNewPos) {
					curPos = newPos;
				}
				else
				{
					// 次のタグへ移動 
					curPos += de.getDataOffset() + getDataSize(layer, de);
				}
			}
			else
			{
				// 次のタグへ移動 
				curPos += de.getDataOffset() + getDataSize(layer, de);
			}

			if (enableSequenceDelimit && de.getGroup() == 0xFFFE && de.getElement() == 0xE0DD)
			{
				// 未定義長 かつ (FFFE,E0DD) であった場合、ループを脱出 
				break;
			}

			config->is_abort(agalia::config::throw_on_abort);

		} while (curPos < endPos);

		if (nextPos) {
			*nextPos = curPos;
		}
	}
	catch (HRESULT err)
	{
		callback->next();
		result = err;
	}

	return result;
}

// DICOM ファイルの解析 
void analyzeDicomFile(_In_ const uint8_t* base_addr, _In_ uint64_t data_size, callback_cls* callback, const agalia::config* config)
{
	uint64_t pos = 0;
	uint32_t item_size = 0;

	// プリアンブルをスキップ 
	item_size = 128;
	if (data_size < pos + item_size) return;
	pos += item_size;

	// プレフィックスをチェック 
	item_size = sizeof(uint32_t);
	if (data_size < pos + item_size) return;
	uint32_t prefix = *reinterpret_cast<const uint32_t*>(base_addr + pos);
	if (prefix != *reinterpret_cast<uint32_t*>("DICM")) {
		return;
	}
	pos += item_size;

	// DICOMデータセットをパース 
	dicom_dataset_parser parser;
	parser.parse(base_addr, pos, data_size, 0, nullptr, false, callback, config);
}

#include "image_file.h"

class dcm_dictinary_wrapper
{
public:
	dcm_dictinary_wrapper(const wchar_t* lang) 
	{
		init_dcm_dictionary(lang);
	}

	virtual ~dcm_dictinary_wrapper()
	{
		clear_dcm_dictionary();
	}
};

bool image_file_dicom::is_supported(const void* buf, uint64_t size)
{
	if (buf == nullptr) return false;
	if (size < 128 + 4) return false;

	uint32_t prefix = *reinterpret_cast<const uint32_t*>(reinterpret_cast<const unsigned char*>(buf) + 128);
	return (prefix == *reinterpret_cast<uint32_t*>("DICM"));
}

void image_file_dicom::parse(callback_cls* callback, const agalia::config* config)
{
	callback->insert_column(96, L"offset");
	callback->insert_column(96, L"tag");
	callback->insert_column(256, L"name");
	callback->insert_column(64, L"VR");
	int length_column_number = callback->insert_column(96, L"length");
	callback->set_right_align(length_column_number, true);
	callback->insert_column(320, L"data");
	callback->insert_column(320, L"value");
	callback->next();

	dcm_dictinary_wrapper dic((config->preferred_language == agalia::config::jpn) ? L"jpn" : L"enu");
	analyzeDicomFile(this->base_addr + this->base_offset, this->data_size, callback, config);
}
