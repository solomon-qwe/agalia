#include "pch.h"
#include "container_ICC.h"

#include "agaliareptImpl.h"
#include "byteswap.h"

#include "analyze_ICC_item_Header.h"
#include "analyze_ICC_item_Element.h"

using namespace analyze_ICC;

container_ICC::container_ICC(const wchar_t* path, IStream* stream)
	: _agaliaContainerBase(path, stream)
{

}

container_ICC::~container_ICC()
{

}

HRESULT container_ICC::getColumnCount(uint32_t* count) const
{
	if (count == nullptr) return E_POINTER;
	*count = column_last;
	return S_OK;
}

HRESULT container_ICC::getColumnWidth(uint32_t column, int32_t* length) const
{
	if (length == nullptr) return E_POINTER;

	switch (column)
	{
	case column_offset: *length = 8; break;
	case column_signature: *length = 8; break;
	case column_value_offset: *length = 8; break;
	case column_element_size: *length = 8; break;
	case column_value: *length = 8; break;
	default:
		return E_INVALIDARG;
	}

	return S_OK;
}

HRESULT container_ICC::getColumnName(uint32_t column, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	const wchar_t* name = nullptr;
	switch (column)
	{
	case column_offset: name = L"offset"; break;
	case column_signature: name = L"Signature"; break;
	case column_value_offset: name = L"Value Offset"; break;
	case column_element_size: name = L"Element Size"; break;
	case column_value: name = L"value"; break;
	default:
		return E_INVALIDARG;
	}

	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT container_ICC::getElementInfoCount(const agaliaElement* item, uint32_t* row) const
{
	if (item == nullptr) return E_POINTER;
	if (row == nullptr) return E_POINTER;

	if (item->getGUID() != item_Header::guid_icc)
		return E_FAIL;

	*row = 1;
	return S_OK;
}



HRESULT container_ICC::getElementInfoValue(const agaliaElement* item, uint32_t row, uint32_t column, agaliaString** str) const
{
	if (item == nullptr) return E_POINTER;
	if (str == nullptr) return E_POINTER;

	if (item->getGUID() != item_Header::guid_icc)
		return E_FAIL;

	if (row != 0) return E_INVALIDARG;

	return static_cast<const item_Header*>(item)->getColumnValue(column, str);
}




HRESULT container_ICC::getRootElement(agaliaElement** root) const
{
	if (root == nullptr) return E_POINTER;

	ULARGE_INTEGER endpos = {};
	LockStream();
	auto hr = data_stream->Seek(uint64_to_li(0), STREAM_SEEK_END, &endpos);
	UnlockStream();
	if (FAILED(hr)) return hr;

//	*root = createItem(this, 8, endpos.QuadPart);
	*root = new item_Header(this, 0, 128, endpos.QuadPart);

	return S_OK;
}


HRESULT container_ICC::getPropertyValue(PropertyType type, agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;

	if (type == ContainerType)
	{
		return GetContainerName(str);
	}
	else if (type == ICCProfile)
	{
		agaliaPtr<agaliaElement> root;
		auto hr = getRootElement(&root);
		if (FAILED(hr)) return hr;

		agaliaPtr<agaliaElement> tag_table;
		hr = root->getChild(0, &tag_table);
		if (FAILED(hr)) return hr;

		agaliaPtr<agaliaElement> element;
		hr = tag_table->getChild(0, &element);
		if (FAILED(hr)) return hr;

		do
		{
			auto p = static_cast<item_Element*>(element._p);
			agaliaStringPtr sig;
			hr = p->getPropValue(item_Element::prop_signature, &sig);
			if (SUCCEEDED(hr))
			{
				if (wcscmp(sig->GetData(), L"desc") == 0)
				{
					return p->getProfileName(str);
				}
			}

			agaliaElement* next = nullptr;
			element->getNext(&next);
			element = next;
		} while (element);
	}
	return E_FAIL;
}


HRESULT container_ICC::loadBitmap(agaliaBitmap** ppBitmap) const
{
	UNREFERENCED_PARAMETER(ppBitmap);
	return E_FAIL;
}

HRESULT container_ICC::loadThumbnail(agaliaBitmap** ppBitmap, uint32_t maxW, uint32_t maxH) const
{
	UNREFERENCED_PARAMETER(ppBitmap);
	UNREFERENCED_PARAMETER(maxW);
	UNREFERENCED_PARAMETER(maxH);
	return E_FAIL;
}


_agaliaContainerBase* container_ICC::CreateInstance(const wchar_t* path, IStream* stream)
{
	return new container_ICC(path, stream);
}

bool container_ICC::IsSupported(IStream* stream)
{
	if (!stream) return false;

	if (FAILED(stream->Seek(uint64_to_li(0), STREAM_SEEK_SET, nullptr)))
		return false;

	Header header = {};

	ULONG cbRead = 0;
	auto hr = stream->Read(&header, sizeof(header), &cbRead);
	if (hr != S_OK || cbRead != sizeof(header))
		return false;

	if (agalia_byteswap(header.ProfileFileSignature) == 'acsp')
		return true;

	return false;
}



HRESULT container_ICC::GetContainerName(agaliaString** name)
{
	if (name == nullptr) return E_POINTER;
	*name = agaliaString::create(L"ICC Profile");
	return S_OK;
}
