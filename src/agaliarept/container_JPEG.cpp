#include "pch.h"
#include "container_JPEG.h"

#include "analyze_JPEG_util.h"
#include "analyze_JPEG_item_Marker.h"
#include "analyze_JPEG_item_APP.h"

#include "jpeg_def.h"
#include "byteswap.h"
#include "thumbnail.h"

using namespace analyze_JPEG;



inline bool IsSOF(uint16_t marker)
{
	switch (marker)
	{
	case SOF0: case SOF1: case SOF2: case SOF3:
	case SOF9: case SOF10: case SOF11:
		return true;
	}
	return false;
}



HRESULT getJPEGImageWidth(const container_JPEG* image, agaliaString** s)
{
	agaliaPtr<agaliaElement> item;
	auto hr = image->getRootElement(&item);
	if (FAILED(hr)) return hr;

	while (item && !IsSOF(getMarker(image, item)))
	{
		agaliaPtr<agaliaElement> next;
		item->getNext(&next);
		item.detach()->Release();
		item.attach(next.detach());
	}

	if (!item)
		return E_FAIL;

	JPEGSEGMENT_SOF sof = {};
	hr = image->ReadData(&sof, item->getOffset(), sizeof(sof));
	if (FAILED(hr)) return hr;

	std::wstringstream temp;
	temp << agalia_byteswap(sof.X);
	*s = agaliaString::create(temp.str().c_str());
	return S_OK;
}



HRESULT getJPEGImageHeight(const container_JPEG* image, agaliaString** s)
{
	agaliaPtr<agaliaElement> item;
	auto hr = image->getRootElement(&item);
	if (FAILED(hr)) return hr;

	while (item && !IsSOF(getMarker(image, item)))
	{
		agaliaPtr<agaliaElement> next;
		item->getNext(&next);
		item.detach()->Release();
		item.attach(next.detach());
	}

	if (!item)
		return E_FAIL;

	JPEGSEGMENT_SOF sof = {};
	hr = image->ReadData(&sof, item->getOffset(), sizeof(sof));
	if (FAILED(hr)) return hr;

	std::wstringstream temp;
	temp << agalia_byteswap(sof.Y);
	*s = agaliaString::create(temp.str().c_str());
	return S_OK;
}


HRESULT get_iccprofile_container(const container_JPEG* image, const item_APP* item, agaliaContainer** container)
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

	hr = getAgaliaImage(container, file_path, offset, size);
	if (FAILED(hr)) return hr;

	return S_OK;
}



HRESULT getICCProfileName(const container_JPEG* image, agaliaString** s)
{
	agaliaPtr<agaliaElement> item;
	auto hr = image->getRootElement(&item);
	if (FAILED(hr)) return hr;

	while (item)
	{
		if (getMarker(image, item) == APP2)
		{
			agaliaPtr<agaliaContainer> container;
			hr = get_iccprofile_container(image, static_cast<item_APP*>(item._p), &container);
			if (SUCCEEDED(hr))
			{
				hr = container->getPropertyValue(agaliaContainer::ICCProfile, s);
				if (SUCCEEDED(hr))
					return S_OK;
			}
		}

		agaliaPtr<agaliaElement> next;
		item->getNext(&next);
		item.detach()->Release();
		item.attach(next.detach());
	}

	return E_FAIL;
}


// BMPFormat 

container_JPEG::container_JPEG(const wchar_t* path, IStream* stream)
	:_agaliaContainerBase(path, stream)
{
}



container_JPEG::~container_JPEG()
{
}



HRESULT container_JPEG::getColumnCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = column_last;
	return S_OK;
}



HRESULT container_JPEG::getColumnWidth(uint32_t column, int32_t* length) const
{
	if (length == nullptr) return E_POINTER;

	switch (column)
	{
	case column_offset: *length = 8;	break;
	case column_marker:	*length = 4;	break;
	case column_name:	*length = 4;	break;
	case column_length:	*length = -6;	break;
	case column_value:	*length = 0;	break;
	default:
		return E_INVALIDARG;
	}

	return S_OK;
}



HRESULT container_JPEG::getColumnName(uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (column)
	{
	case column_offset: name = L"offset";	break;
	case column_marker:	name = L"marker";	break;
	case column_name:	name = L"name";		break;
	case column_length:	name = L"length";	break;
	case column_value:	name = L"value";	break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT container_JPEG::getElementInfoCount(const agaliaElement* item, uint32_t* row) const
{
	if (item == nullptr) return E_POINTER;
	if (row == nullptr) return E_POINTER;

	if (item->getGUID() != JPEG_item_Base::guid_jpeg)
		return E_FAIL;

	*row = 1;
	auto hr = static_cast<const JPEG_item_Base*>(item)->getAdditionalInfoCount(row);
	if (SUCCEEDED(hr))
		(*row)++;
	return S_OK;
}



HRESULT container_JPEG::getElementInfoValue(const agaliaElement* item, uint32_t row, uint32_t column, agaliaString** str) const
{
	if (item == nullptr) return E_POINTER;
	if (str == nullptr) return E_POINTER;

	if (item->getGUID() != JPEG_item_Base::guid_jpeg)
		return E_FAIL;

	if (row == 0) {
		return static_cast<const JPEG_item_Base*>(item)->getColumnValue(column, str);
	}
	else if (column == 0) {
		*str = agaliaString::create(L"");
		return S_OK;
	}
	else if (column == column_value) {
		return static_cast<const JPEG_item_Base*>(item)->getAdditionalInfoValue(row - 1, str);
	}
	return E_FAIL;
}



HRESULT container_JPEG::getRootElement(agaliaElement** root) const
{
	if (root == nullptr) return E_POINTER;
	return create_item(root, 0, this);
}



HRESULT container_JPEG::getPropertyValue(PropertyType type, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	switch (type)
	{
	case ContainerType:
		return GetContainerName(str);

	case ImageWidth:
		return getJPEGImageWidth(this, str);

	case ImageHeight:
		return getJPEGImageHeight(this, str);

	case ShootingDateTime:
		break;

	case CreationDateTime:
		break;

	case ICCProfile:
		return getICCProfileName(this, str);
		break;
	}

	return E_FAIL;
}



HRESULT container_JPEG::getThumbnailImage(HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH) const
{
	return loadThumbnailBitmap(phBitmap, maxW, maxH, data_stream);
}



HRESULT container_JPEG::getColorProfile(agaliaHeap** buf) const
{
	if (buf == nullptr) return E_POINTER;

	agaliaPtr<agaliaElement> item;
	auto hr = getRootElement(&item);
	if (FAILED(hr)) return hr;

	while (item && (getMarker(this, item) != APP2))
	{
		agaliaPtr<agaliaElement> next;
		item->getNext(&next);
		item.detach()->Release();
		item.attach(next.detach());
	}

	if (!item)
		return E_FAIL;

	CHeapPtr<JPEGSEGMENT_APPX> appX;
	rsize_t bufsize = 0;
	hr = ReadSegment(appX, &bufsize, this, item);
	if (FAILED(hr)) return hr;

	const char name[] = "ICC_PROFILE";
	if (!app_identify(name, _countof(name), appX, bufsize))
		return E_FAIL;

	rsize_t profile_size = bufsize - offsetof(JPEGSEGMENT_APPX, identifier) - _countof(name) - 2;

	_agaliaHeapImpl* _buf = new _agaliaHeapImpl;
	hr = _buf->AllocateBytes(profile_size);
	if (FAILED(hr)) return hr;

	memcpy(_buf->GetData(), reinterpret_cast<uint8_t*>(appX.m_pData) + (bufsize - profile_size), profile_size);
	*buf = _buf;

	return S_OK;
}



bool container_JPEG::IsSupported(IStream* stream)
{
	if (!stream) return false;

	if (FAILED(stream->Seek(uint64_to_li(0), STREAM_SEEK_SET, nullptr)))
		return false;

	// SOI マーカーセグメントで始まれば JPEG データをとみなす 
	ULONG cbRead = 0;
	JPEGSEGMENT data = {};
	auto hr = stream->Read(&data, sizeof(data), &cbRead);
	if (hr != S_OK || cbRead != sizeof(data))
		return false;

	return (agalia_byteswap(data.marker) == SOI);
}



_agaliaContainerBase* container_JPEG::CreateInstance(const wchar_t* path, IStream* stream)
{
	return new container_JPEG(path, stream);
}



HRESULT container_JPEG::GetContainerName(agaliaString** name)
{
	if (name == nullptr) return E_POINTER;
	*name = agaliaString::create(L"JPEG");
	return S_OK;
}
