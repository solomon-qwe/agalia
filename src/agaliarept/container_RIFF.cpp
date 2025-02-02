#include "pch.h"

#include "agaliareptImpl.h"

#include "container_RIFF.h"
#include "analyze_RIFF_item_Base.h"
#include "analyze_RIFF_item_LIST.h"
#include "analyze_RIFF_item_avih.h"
#include "analyze_RIFF_item_strh.h"
#include "analyze_RIFF_item_strf_vids.h"
#include "analyze_RIFF_item_strf_auds.h"
#include "analyze_RIFF_item_idx1.h"
#include "analyze_RIFF_item_text.h"

#include "jpeg_def.h"
#include "thumbnail.h"

#define MJPGDHTSEG_STORAGE
#include <mmreg.h>



using namespace analyze_RIFF;



inline char fcc_snip(FOURCC fcc, int i)
{
	return reinterpret_cast<char*>(&fcc)[i];
}



RIFF_item_Base* analyze_RIFF::create_item(const container_RIFF* image, uint64_t offset, FOURCC fcc, DWORD cb, FOURCC fccType)
{
	RIFF_item_Base* item = nullptr;

	uint64_t next_item_size = 0;
	if (fcc == FOURCC_RIFF ||
		fcc == FOURCC_LIST)
	{
		next_item_size = sizeof(RIFFLIST);
		item = new item_LIST(image, offset, next_item_size);
		item->fccType = fccType;
	}
	else if (fcc == ckidMAINAVIHEADER)
	{
		next_item_size = sizeof(RIFFCHUNK) + RIFFROUND(uint64_t(cb));
		item = new item_avih(image, offset, next_item_size);
		item->fccType = fccType;
	}
	else if (fcc == ckidSTREAMHEADER)
	{
		next_item_size = sizeof(RIFFCHUNK) + RIFFROUND(uint64_t(cb));
		item = new item_strh(image, offset, next_item_size);

		AVISTREAMHEADER avistrh = {};
		auto hr = image->ReadData(&avistrh, offset, sizeof(avistrh));
		if (SUCCEEDED(hr)) {
			item->fccType = avistrh.fccType;
		}
		else {
			item->fccType = fccType;
		}
	}
	else if (fcc == ckidSTREAMFORMAT)
	{
		if (fccType == streamtypeVIDEO) {
			next_item_size = sizeof(RIFFCHUNK) + RIFFROUND(uint64_t(cb));
			item = new item_strf_vids(image, offset, next_item_size);
		}
		else if (fccType == streamtypeAUDIO) {
			next_item_size = sizeof(RIFFCHUNK) + RIFFROUND(uint64_t(cb));
			item = new item_strf_auds(image, offset, next_item_size);
		}
		else {
			next_item_size = sizeof(RIFFCHUNK) + RIFFROUND(uint64_t(cb));
			item = new RIFF_item_Base(image, offset, next_item_size);
		}
		item->fccType = fccType;
	}
	else if (fcc == ckidAVIOLDINDEX)
	{
		next_item_size = sizeof(RIFFCHUNK) + RIFFROUND(uint64_t(cb));
		item = new item_idx1(image, offset, next_item_size);
	}
	else if (
		fcc_snip(fcc, 0) == 'I' ||
		fcc == ckidSTREAMNAME)
	{
		next_item_size = sizeof(RIFFCHUNK) + RIFFROUND(uint64_t(cb));
		item = new item_text(image, offset, next_item_size);
	}
	else if (fcc == FCC('fmt '))
	{
		next_item_size = sizeof(RIFFCHUNK) + RIFFROUND(uint64_t(cb));
		item = new item_strf_auds(image, offset, next_item_size);
	}
	else
	{
		next_item_size = sizeof(RIFFCHUNK) + RIFFROUND(uint64_t(cb));
		item = new RIFF_item_Base(image, offset, next_item_size);
		item->fccType = fccType;
	}

	return item;
}



HRESULT getAVIMainHeader(const container_RIFF* format, AVIMAINHEADER* avimain)
{
	agaliaPtr<agaliaElement> root;
	auto hr = format->getRootElement(&root);
	if (FAILED(hr)) return E_FAIL;

	agaliaPtr<agaliaElement> item;
	hr = root->getChild(0, &item);
	if (FAILED(hr)) return E_FAIL;

	if (item->getGUID() != RIFF_item_Base::guid_riff)
		return E_FAIL;

	RIFFCHUNK list_chunk = {};
	hr = format->ReadData(&list_chunk, item->getOffset(), sizeof(list_chunk));
	if (FAILED(hr)) return E_FAIL;

	if (list_chunk.fcc != FOURCC_LIST)
		return E_FAIL;

	agaliaPtr<agaliaElement> avih;
	hr = item->getChild(0, &avih);
	if (FAILED(hr)) return E_FAIL;

	if (avih->getGUID() != RIFF_item_Base::guid_riff)
		return E_FAIL;

	hr = format->ReadData(avimain, avih->getOffset(), sizeof(AVIMAINHEADER));
	if (FAILED(hr)) return hr;

	return S_OK;
}



HRESULT getFirstFrame(const container_RIFF* format, uint64_t* offset, uint64_t* size)
{
	agaliaPtr<agaliaElement> root;
	auto hr = format->getRootElement(&root);
	if (FAILED(hr)) return E_FAIL;

	agaliaPtr<agaliaElement> item;
	hr = root->getChild(0, &item);
	while (SUCCEEDED(hr))
	{
		RIFFCHUNK chunk = {};
		hr = format->ReadData(&chunk, item->getOffset(), sizeof(chunk));
		if (FAILED(hr)) return E_FAIL;

		if (chunk.fcc == FOURCC_LIST)
		{
			RIFFLIST list_chunk = {};
			hr = format->ReadData(&list_chunk.fccListType, item->getOffset() + offsetof(RIFFLIST, fccListType), sizeof(list_chunk.fccListType));
			if (FAILED(hr)) return E_FAIL;

			if (list_chunk.fccListType == listtypeAVIMOVIE)
			{
				agaliaPtr<agaliaElement> child;
				hr = item->getChild(0, &child);
				if (FAILED(hr)) return E_FAIL;
				item.detach()->Release();
				item.attach(child.detach());

				while (SUCCEEDED(hr))
				{
					hr = format->ReadData(&chunk, item->getOffset(), sizeof(chunk));
					if (FAILED(hr)) return E_FAIL;

					if (chunk.fcc == FOURCC_LIST)
					{
						hr = item->getChild(0, &child);
						if (FAILED(hr)) return E_FAIL;

						hr = format->ReadData(&chunk, child->getOffset(), sizeof(chunk));
						if (FAILED(hr)) return E_FAIL;

						if (chunk.fcc == FCC('00dc'))
						{
							child->getValueAreaOffset(offset);
							child->getValueAreaSize(size);
							return S_OK;
						}
						child.detach()->Release();
					}
					else if (chunk.fcc == FCC('00dc'))
					{
						item->getValueAreaOffset(offset);
						item->getValueAreaSize(size);
						return S_OK;
					}

					agaliaPtr<agaliaElement> next;
					hr = item->getNext(&next);
					if (FAILED(hr)) return E_FAIL;

					item.detach()->Release();
					item.attach(next.detach());
				}
			}
		}

		agaliaPtr<agaliaElement> next;
		hr = item->getNext(&next);
		if (FAILED(hr)) return E_FAIL;

		item.detach()->Release();
		item.attach(next.detach());
	}

	return E_FAIL;
}




container_RIFF::container_RIFF(const wchar_t* path, IStream* stream)
	: _agaliaContainerBase(path, stream)
{
}



container_RIFF::~container_RIFF()
{
}



HRESULT container_RIFF::getRootElement(agaliaElement** root) const
{
	if (root == nullptr) return E_POINTER;
	auto item = new item_LIST(this, 0, sizeof(RIFFLIST));

	ULARGE_INTEGER pos = {};
	LockStream();
	auto hr = data_stream->Seek(int64_to_li(0), STREAM_SEEK_END, &pos);
	UnlockStream();
	if (FAILED(hr)) return 0;

	item->endPos = pos.QuadPart;
	*root = item;
	return S_OK;
}



HRESULT container_RIFF::getColumnCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = column_last;
	return S_OK;
}



HRESULT container_RIFF::getColumnWidth(uint32_t column, int32_t* length) const
{
	if (length == nullptr) return E_POINTER;

	switch (column)
	{
	case column_offset:	*length = 8;	break;
	case column_parent:	*length = 8;	break;
	case column_id:		*length = 8;	break;
	case column_size:	*length = -8;	break;
	case column_value:	*length = 0;	break;
	default:
		return E_FAIL;
	}

	return S_OK;
}



HRESULT container_RIFF::getColumnName(uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (column)
	{
	case column_offset:	name = L"offset";	break;
	case column_parent:	name = L"parent";	break;
	case column_id:		name = L"id";		break;
	case column_size:	name = L"size";		break;
	case column_value:	name = L"value";	break;
	default:
		return E_FAIL;
	}

	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT container_RIFF::getElementInfoCount(const agaliaElement* item, uint32_t* row) const
{
	if (item == nullptr) return E_POINTER;
	if (row == nullptr) return E_POINTER;

	if (item->getGUID() != RIFF_item_Base::guid_riff)
		return E_FAIL;

	*row = 1;
	auto hr = static_cast<const RIFF_item_Base*>(item)->getAdditionalInfoCount(row);
	if (SUCCEEDED(hr))
		(*row)++;
	return S_OK;
}



HRESULT container_RIFF::getElementInfoValue(const agaliaElement* item, uint32_t row, uint32_t column, agaliaString** str) const
{
	if (item == nullptr) return E_POINTER;
	if (str == nullptr) return E_POINTER;

	if (item->getGUID() != RIFF_item_Base::guid_riff)
		return E_FAIL;

	if (row == 0) {
		return static_cast<const RIFF_item_Base*>(item)->getColumnValue(column, str);
	}
	else if (column == 0) {
		*str = agaliaString::create(L"");
		return S_OK;
	}
	else if (column == column_value) {
		return static_cast<const RIFF_item_Base*>(item)->getAdditionalInfoValue(row - 1, str);
	}
	return E_FAIL;
}



HRESULT container_RIFF::getPropertyValue(PropertyType type, agaliaString** str) const
{
	if (type == ContainerType)
	{
		return GetContainerName(str);
	}
	else if (type == FormatType)
	{
		agaliaPtr<agaliaElement> root;
		auto hr = getRootElement(&root);
		if (SUCCEEDED(hr))
		{
			root->getName(str);
		}
		return S_OK;
	}
	else if (type == ImageWidth)
	{
		AVIMAINHEADER avimain = {};
		auto hr = getAVIMainHeader(this, &avimain);
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << avimain.dwWidth;
		*str = agaliaString::create(temp.str().c_str());

		return S_OK;
	}
	else if (type == ImageHeight)
	{
		AVIMAINHEADER avimain = {};
		auto hr = getAVIMainHeader(this, &avimain);
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << avimain.dwHeight;
		*str = agaliaString::create(temp.str().c_str());

		return S_OK;
	}

	return E_FAIL;
}



HRESULT container_RIFF::getThumbnailImage(HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH) const
{
	return loadThumbnailBitmap(phBitmap, maxW, maxH, data_stream);
}



_agaliaContainerBase* container_RIFF::CreateInstance(const wchar_t* path, IStream* stream)
{
	return new container_RIFF(path, stream);
}



bool container_RIFF::IsSupported(IStream* stream)
{
	if (!stream) return false;

	if (FAILED(stream->Seek(uint64_to_li(0), STREAM_SEEK_SET, nullptr)))
		return false;

	ULONG cbRead = 0;
	RIFFCHUNK riff = {};
	auto hr = stream->Read(&riff, sizeof(riff), &cbRead);
	if (hr != S_OK || cbRead != sizeof(riff))
		return false;

	return (riff.fcc == FOURCC_RIFF);
}



HRESULT container_RIFF::GetContainerName(agaliaString** name)
{
	if (name == nullptr) return E_POINTER;
	*name = agaliaString::create(L"RIFF");
	return S_OK;
}
