#include "pch.h"
#include "analyze_TIFF_util.h"

using namespace analyze_TIFF;



HRESULT analyze_TIFF::format_exif_user_comment(std::wstringstream& str, const container_TIFF* image, uint64_t value_offset, uint64_t value_size)
{
	if (value_size < 8) {
		return E_FAIL;
	}

	rsize_t bufsize = 0;
	auto hr = UInt64ToSizeT(value_size + 2, &bufsize);
	if (FAILED(hr)) return hr;

	CHeapPtr<char> buf;
	if (!buf.AllocateBytes(bufsize)) return E_OUTOFMEMORY;
	hr = image->ReadData(buf, value_offset, value_size);
	if (FAILED(hr)) return hr;

	buf.m_pData[bufsize - 2] = '\0';
	buf.m_pData[bufsize - 1] = '\0';

	int codepage = 0;
	if (memcmp(buf, "ASCII\0\0\0", 8) == 0)
	{
		codepage = CP_US_ASCII;
	}
	else if (memcmp(buf, "JIS\0\0\0\0\0\0\0\0", 8) == 0)
	{
		codepage = CP_JIS;
	}
	else if (memcmp(buf, "\0\0\0\0\0\0\0\0", 8) == 0)
	{
		codepage = CP_ACP;
	}
	else if (memcmp(buf, "Unicode\0", 8) == 0)
	{
		str << reinterpret_cast<const wchar_t*>(buf + 8);
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}

	agaliaStringPtr temp;
	multibyte_to_widechar(buf.m_pData + 8, -1, codepage, &temp);
	str << L"\"" << temp->GetData() << L"\"";
	return S_OK;
}



// TIFFENTRY�̒l���擾 
// T : �擾����l�̌^ 
// result : �擾�����l���i�[����ϐ��ւ̃|�C���^ 
// image : �摜�f�[�^�Ǘ��I�u�W�F�N�g 
// value_offset : �l�̈ʒu 
// type : �l�̌^ 
// index : �l�̗v�f�ԍ� 
template <typename T>
HRESULT getTiffValue(T* result, const container_TIFF* image, uint64_t value_offset, uint16_t type, rsize_t index)
{
	// �^�̃T�C�Y���擾 
	rsize_t typesize = 0;
	auto hr = getTiffTypeSize(type, &typesize);
	if (FAILED(hr)) return hr;

	// �l��ǂݍ��� 
	T val = 0;
	hr = image->ReadData(&val, value_offset + static_cast<uint64_t>(typesize) * index, sizeof(val));
	if (FAILED(hr)) return hr;

	// �o�C�g�I�[�_�[�𒲐� 
	*result = get_tiff_value(image->byte_order, val);

	return S_OK;
}



// �C�ӂ̌^�̒l��10�i���̕�����Ƃ��Ď擾 
template <typename T>
HRESULT format_tiff_value_dec(std::wstringstream& str, const container_TIFF* image, uint64_t value_offset, uint16_t type, rsize_t index)
{
	// �l��ǂݍ��� 
	T val = 0;
	auto hr = getTiffValue(&val, image, value_offset, type, index);
	if (FAILED(hr)) return hr;

	// ������ 
	if (index != 0) str << L", ";
	format_dec(str, val);

	return S_OK;
}



// �C�ӂ̌^�̒l��16�i���̕�����Ƃ��Ď擾 
template <typename T>
HRESULT format_tiff_value_hex(std::wstringstream& str, const container_TIFF* image, uint64_t value_offset, uint16_t type, rsize_t index)
{
	// �l��ǂݍ��� 
	T val = 0;
	auto hr = getTiffValue(&val, image, value_offset, type, index);
	if (FAILED(hr)) return hr;

	// ������ 
	if (index != 0) str << L", ";
	format_hex(str, val);
	
	return S_OK;
}



// �C�ӂ̌^�̒l�𕂓������_���̕�����Ƃ��Ď擾 
// T : TIFFFLOAT or TIFFDOUBLE 
template <typename T>
HRESULT format_tiff_value_float(std::wstringstream& str, const container_TIFF* image, uint64_t value_offset, uint16_t type, rsize_t index)
{
	// �l��ǂݍ��� 
	T val = 0;
	auto hr = getTiffValue(&val, image, value_offset, type, index);
	if (FAILED(hr)) return hr;

	// ������ 
	if (index != 0) str << L", ";
	format_float(str, val);
	
	return S_OK;
}



// �����^�̒l�𕶎���Ƃ��Ď擾
// T : TIFFRATIONAL or TIFFSRATIONAL 
template <typename T>
HRESULT format_tiff_value_fraction(std::wstringstream& str, const container_TIFF* image, uint64_t value_offset, uint16_t type, rsize_t index)
{
	// �^�̃T�C�Y���擾 
	rsize_t typesize = 0;
	auto hr = getTiffTypeSize(type, &typesize);
	if (FAILED(hr)) return hr;

	// �l��ǂݍ��� 
	T val(0, 0);
	hr = image->ReadData(&val, value_offset + static_cast<uint64_t>(typesize) * index, sizeof(val));
	if (FAILED(hr)) return hr;

	// ������ 
	if (index != 0) str << L", ";
	format_dec(str, get_tiff_value(image->byte_order, val.fraction));
	str << L"/";
	format_dec(str, get_tiff_value(image->byte_order, val.denominator));

	return S_OK;
}



// ASCII �^�̒l�𕶎���Ƃ��Ď擾 
HRESULT format_tiff_value_ascii(std::wstringstream& str, const container_TIFF* image, uint64_t value_offset, uint16_t type, rsize_t index)
{
	// �^�̃T�C�Y���擾 
	rsize_t typesize = 0;
	auto hr = getTiffTypeSize(type, &typesize);
	if (FAILED(hr)) return hr;

	// �l��ǂݍ��� 
	TIFFASCII val = 0;
	hr = image->ReadData(&val, value_offset + static_cast<uint64_t>(typesize) * index, sizeof(val));
	if (FAILED(hr)) return hr;

	// ������ 
	if (index == 0) str << L"\"";
	if (val) {
		wchar_t temp[8] = {};
		swprintf_s(temp, L"%C", val);
		str << temp;
	}
	else {
		str << L"\"";
		return S_FALSE;
	}
	return S_OK;
}



HRESULT analyze_TIFF::format_tiff_value(std::wstringstream& str, const container_TIFF* image, uint64_t value_offset, uint16_t type, rsize_t index)
{
	switch (type)
	{
	case TIFFTYPEID::TYPE_BYTE:		return format_tiff_value_dec<TIFFBYTE>(str, image, value_offset, type, index);
	case TIFFTYPEID::TYPE_ASCII:	return format_tiff_value_ascii(str, image, value_offset, type, index);
	case TIFFTYPEID::TYPE_SHORT:	return format_tiff_value_dec<TIFFSHORT>(str, image, value_offset, type, index);
	case TIFFTYPEID::TYPE_LONG:		return format_tiff_value_dec<TIFFLONG>(str, image, value_offset, type, index);
	case TIFFTYPEID::TYPE_RATIONAL:	return format_tiff_value_fraction<TIFFRATIONAL>(str, image, value_offset, type, index);
	case TIFFTYPEID::TYPE_SBYTE:	return format_tiff_value_dec<TIFFSBYTE>(str, image, value_offset, type, index);
	case TIFFTYPEID::TYPE_UNDEFINED:return format_tiff_value_hex<TIFFUNDEFINED>(str, image, value_offset, type, index);
	case TIFFTYPEID::TYPE_SSHORT:	return format_tiff_value_dec<TIFFSSHORT>(str, image, value_offset, type, index);
	case TIFFTYPEID::TYPE_SLONG:	return format_tiff_value_dec<TIFFSLONG>(str, image, value_offset, type, index);
	case TIFFTYPEID::TYPE_SRATIONAL:return format_tiff_value_fraction<TIFFSRATIONAL>(str, image, value_offset, type, index);
	case TIFFTYPEID::TYPE_FLOAT:	return format_tiff_value_float<TIFFFLOAT>(str, image, value_offset, type, index);
	case TIFFTYPEID::TYPE_DOUBLE:	return format_tiff_value_float<TIFFDOUBLE>(str, image, value_offset, type, index);
	case TIFFTYPEID::TYPE_IFD:		return format_tiff_value_dec<uint32_t>(str, image, value_offset, type, index);
	case TIFFTYPEID::TYPE_LONG8:	return format_tiff_value_dec<TIFFLONG8>(str, image, value_offset, type, index);
	case TIFFTYPEID::TYPE_SLONG8:	return format_tiff_value_dec<TIFFSLONG8>(str, image, value_offset, type, index);
	case TIFFTYPEID::TYPE_IFD8:		return format_tiff_value_dec<uint64_t>(str, image, value_offset, type, index);
	}
	return E_FAIL;
}
