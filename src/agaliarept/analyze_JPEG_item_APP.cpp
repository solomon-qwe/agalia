#include "pch.h"
#include "analyze_JPEG_item_APP.h"

#include "analyze_JPEG_util.h"
#include "analyze_JPEG_item_JFIF.h"
#include "analyze_JPEG_item_Exif.h"
#include "analyze_JPEG_item_extdat.h"
#include "analyze_JPEG_item_ICC.h"

#include "container_ICC.h"

#include "jpeg_def.h"
#include "byteswap.h"

using namespace analyze_JPEG;




HRESULT create_jfif(const container_JPEG* image, const item_APP* item, agaliaItem** child)
{
	CHeapPtr<JPEGSEGMENT_APPX> buf;
	rsize_t bufsize = 0;
	auto hr = ReadSegment(buf, &bufsize, image, item);
	if (FAILED(hr)) return hr;
	if (bufsize <= offsetof(JPEGSEGMENT_APPX, identifier)) return E_FAIL;

	const char name[] = "JFIF";
	if (app_identify(name, _countof(name), buf, bufsize))
	{
		agaliaItem* newitem = new item_JFIF(image, item->getOffset(), item->getSize(), item_Base::jfif);
		*child = newitem;
		return S_OK;
	}

	return E_FAIL;
}



HRESULT create_iccprofile(const container_JPEG* image, const item_APP* item, agaliaItem** child)
{
	CHeapPtr<JPEGSEGMENT_APPX> buf;
	rsize_t bufsize = 0;
	auto hr = ReadSegment(buf, &bufsize, image, item);
	if (FAILED(hr)) return hr;
	if (bufsize <= offsetof(JPEGSEGMENT_APPX, identifier)) return E_FAIL;

#pragma pack(push, 1)
	struct ICCInfo
	{
		uint8_t SequenceNumber;
		uint8_t TotalNumber;
	};
#pragma pack(pop)

	const char name[] = "ICC_PROFILE";
	if (!app_identify(name, _countof(name), buf, bufsize))
		return E_FAIL;

	auto info = reinterpret_cast<ICCInfo*>(buf.m_pData->identifier + _countof(name));
	if (info->SequenceNumber != 1)
		return E_FAIL;

	agaliaStringPtr file_path;
	hr = image->getFilePath(&file_path);
	if (FAILED(hr)) return hr;

	uint64_t offset = 0, size = 0;
	hr = item->getValueAreaOffset(&offset);
	if (FAILED(hr)) return hr;
	hr = item->getValueAreaSize(&size);
	if (FAILED(hr)) return hr;

	uint64_t sigsize = _countof(name) + sizeof(ICCInfo);
	offset += sigsize;
	size -= sigsize;

	agaliaContainer* container;
	hr = getAgaliaImage(&container, file_path, offset, size);
	if (FAILED(hr)) return hr;

	auto newitem = new item_ICC(image, offset, size, item_Base::icc_profile);
	newitem->container = container;
	*child = newitem;

	return S_OK;
}



HRESULT create_ducky(const container_JPEG* image, const item_APP* item, agaliaItem** child)
{
	CHeapPtr<JPEGSEGMENT_APPX> buf;
	rsize_t bufsize = 0;
	auto hr = ReadSegment(buf, &bufsize, image, item);
	if (FAILED(hr)) return hr;
	if (bufsize <= offsetof(JPEGSEGMENT_APPX, identifier)) return E_FAIL;

	const char name[] = "Ducky";
	if (app_identify(name, _countof(name), buf, bufsize))
	{
		agaliaItem* newitem = new item_extdat(image, item->getOffset(), item->getSize(), item_Base::ducky);
		*child = newitem;
		return S_OK;
	}

	return E_FAIL;
}



HRESULT create_photoshop(const container_JPEG* image, const item_APP* item, agaliaItem** child)
{
	CHeapPtr<JPEGSEGMENT_APPX> buf;
	rsize_t bufsize = 0;
	auto hr = ReadSegment(buf, &bufsize, image, item);
	if (FAILED(hr)) return hr;
	if (bufsize <= offsetof(JPEGSEGMENT_APPX, identifier)) return E_FAIL;

	const char name[] = "Photoshop 3.0";
	if (app_identify(name, _countof(name), buf, bufsize))
	{
		agaliaItem* newitem = new item_extdat(image, item->getOffset(), item->getSize(), item_Base::photoshop);
		*child = newitem;
		return S_OK;
	}

	return E_FAIL;
}



HRESULT create_adobe(const container_JPEG* image, const item_APP* item, agaliaItem** child)
{
	CHeapPtr<JPEGSEGMENT_APPX> buf;
	rsize_t bufsize = 0;
	auto hr = ReadSegment(buf, &bufsize, image, item);
	if (FAILED(hr)) return hr;
	if (bufsize <= offsetof(JPEGSEGMENT_APPX, identifier)) return E_FAIL;

	const char name[] = "Adobe";
	if (app_identify(name, _countof(name), buf, bufsize))
	{
		agaliaItem* newitem = new item_extdat(image, item->getOffset(), item->getSize(), item_Base::adobe);
		*child = newitem;
		return S_OK;
	}

	return E_FAIL;
}



HRESULT create_exif(const container_JPEG* image, const item_APP* item, agaliaItem** child)
{
	CHeapPtr<JPEGSEGMENT_APPX> buf;
	rsize_t bufsize = 0;
	auto hr = ReadSegment(buf, &bufsize, image, item);
	if (FAILED(hr)) return hr;
	if (bufsize <= offsetof(JPEGSEGMENT_APPX, identifier)) return E_FAIL;

	const char exif_id[] = "Exif\0";
	if (app_identify(exif_id, _countof(exif_id), buf, bufsize))
	{
		agaliaStringPtr file_path;
		hr = image->getFilePath(&file_path);
		if (FAILED(hr)) return hr;

		agaliaContainer* exif_image = nullptr;
		hr = getAgaliaImage(&exif_image, file_path, item->getOffset() + 10, item->getSize() - 10);
		if (FAILED(hr)) return hr;

		auto newitem = new item_ExifJPEG(image, item->getOffset(), item->getSize(), item_Base::exif);
		newitem->tiff_image = exif_image;
		*child = newitem;

		return S_OK;
	}

	return E_FAIL;
}



HRESULT create_xmp(const container_JPEG* image, const item_APP* item, agaliaItem** child)
{
	CHeapPtr<JPEGSEGMENT_APPX> buf;
	rsize_t bufsize = 0;
	auto hr = ReadSegment(buf, &bufsize, image, item);
	if (FAILED(hr)) return hr;
	if (bufsize <= offsetof(JPEGSEGMENT_APPX, identifier)) return E_FAIL;

	const char xmp_id[] = "http://ns.adobe.com/xap/1.0/";
	if (app_identify(xmp_id, _countof(xmp_id), buf, bufsize))
	{
		agaliaItem* newitem = new item_extdat(image, item->getOffset(), item->getSize(), item_Base::xmp);
		*child = newitem;
		return S_OK;
	}

	return E_FAIL;
}










item_APP::item_APP(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type)
	:item_Marker(image, offset, size, type)
{
}



item_APP::~item_APP()
{
}



HRESULT item_APP::getChildItem(uint32_t sibling, agaliaItem** child) const
{
	if (sibling != 0) return E_FAIL;
	if (child == nullptr) return E_POINTER;

	if (data_type != item_Base::marker_segment)
		return E_FAIL;

	switch (getMarker(image, this))
	{
	case APP0:
		return create_jfif(image, this, child);

	case APP1:
		if (SUCCEEDED(create_exif(image, this, child)))
			return S_OK;
		return create_xmp(image, this, child);

	case APP2:
		return create_iccprofile(image, this, child);

	case APP12:
		return create_ducky(image, this, child);

	case APP13:
		return create_photoshop(image, this, child);

	case APP14:
		return create_adobe(image, this, child);
	}

	return E_FAIL;
}



HRESULT item_APP::getColumnValueAPPx(agaliaString** str) const
{
	CHeapPtr<JPEGSEGMENT_APPX> app;
	rsize_t bufsize = 0;
	auto hr = ReadSegment(app, &bufsize, image, this);
	if (FAILED(hr)) return hr;
	if (bufsize < sizeof(JPEGSEGMENT_APP)) return E_FAIL;

	std::wstringstream temp;
	temp << L"length=" << agalia_byteswap(app->length);

	temp << L", \"";
	for (size_t i = 0; i < bufsize - offsetof(JPEGSEGMENT_APPX, identifier) && app->identifier[i] != '\0'; i++)
	{
		wchar_t buf[16] = {};
		int ret = swprintf_s(buf, L"%C", app->identifier[i]);
		temp << ((ret <= 0) ? L"." : buf);
	}
	temp << L"\"";

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}



HRESULT item_APP::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_value)
	{
		CHeapPtr<JPEGSEGMENT_APPX> appX;
		size_t bufsize = 0;
		auto hr = ReadSegment(appX, &bufsize, image, this);
		if (FAILED(hr)) return hr;

		if (sizeof(JPEGSEGMENT_APP0_JFIF) <= bufsize &&
			memcmp(appX->identifier, "JFIF", 5) == 0)
		{
			return item_JFIF::getColumnValueForAPP0(column, image, this, str);
		}

		return getColumnValueAPPx(str);
	}

	return __super::getColumnValue(column, str);
}



HRESULT item_APP::getItemPropCount(uint32_t* count) const
{
	auto hr = __super::getItemPropCount(count);
	if (FAILED(hr)) return hr;

	*count += 2;
	return S_OK;
}



HRESULT item_APP::getItemPropName(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getItemPropName(index, str);
	if (SUCCEEDED(hr)) return hr;

	const wchar_t* name = nullptr;
	switch (index)
	{
	case 3: name = L"Lp"; break;
	case 4: name = L"identifier"; break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT item_APP::getItemPropValue(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getItemPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	rsize_t bufsize = 0;
	CHeapPtr<uint8_t> buf;
	hr = ReadSegment(buf, &bufsize, image, this);
	if (FAILED(hr)) return hr;
	JPEGSEGMENT_APPX* app = reinterpret_cast<JPEGSEGMENT_APPX*>(buf.m_pData);

	std::wstringstream temp;
	if (index == 3)
	{
		temp << agalia_byteswap(app->length);
	}
	else if (index == 4)
	{
		size_t max_count = min(agalia_byteswap(app->length) - sizeof(app->length), bufsize - offsetof(JPEGSEGMENT_APPX, identifier));
		int leng = 0;
		hr = SizeTToInt(max_count, &leng);
		if (FAILED(hr)) return hr;
		multibyte_to_widechar(reinterpret_cast<char*>(app->identifier), leng, CP_US_ASCII, temp);
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
