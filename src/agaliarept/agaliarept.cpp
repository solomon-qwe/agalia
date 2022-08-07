#include "pch.h"
#include "agaliareptImpl.h"




_agaliaHeapImpl::_agaliaHeapImpl()
{
}

_agaliaHeapImpl::~_agaliaHeapImpl()
{
	Free();
}

void _agaliaHeapImpl::Release(void)
{
	delete this;
}

HRESULT _agaliaHeapImpl::Free(void)
{
	if (_p == nullptr) return E_FAIL;

	auto temp = _p;
	_p = nullptr;
	free(temp);

	return S_OK;
}

HRESULT _agaliaHeapImpl::AllocateBytes(rsize_t size, bool init)
{
	if (_p) return E_FAIL;

	void* p = nullptr;
	if (init) {
		p = calloc(size, 1);
	}
	else {
		p = malloc(size);
	}

	if (p == nullptr) return E_OUTOFMEMORY;
	_p = p;
	_size = size;

	return S_OK;
}

rsize_t _agaliaHeapImpl::GetSize(void) const
{
	return _size;
}

void* _agaliaHeapImpl::GetData(void) const
{
	return _p;
}





// item class 

_agaliaItemBase::_agaliaItemBase(const GUID& guid, uint64_t offset, uint64_t size)
{
	this->guid = guid;
	item_data_offset = offset;
	item_data_size = size;
}

_agaliaItemBase::~_agaliaItemBase()
{
}

void _agaliaItemBase::Release(void)
{
	delete this;
}



#include "container_ASF.h"
#include "container_BMP.h"
#include "container_DCM.h"
#include "container_ISO.h"
#include "container_JPEG.h"
#include "container_M2P.h"
#include "container_RIFF.h"
#include "container_TIFF.h"
#include "container_PNG.h"

// そのクラスがサポートしているフォーマットであれば、オブジェクトを生成してtrueを返す 
template <class T>
bool generator(agaliaContainer** image, const wchar_t* path, IStream* stream)
{
	return T::IsSupported(stream) ? (*image = T::CreateInstance(path, stream)) : false;
}


typedef bool(*tf)(agaliaContainer**, const wchar_t*, IStream*);
static tf generators[] =
{
	generator<analyze_ASF::container_ASF>,
	generator<analyze_BMP::container_BMP>,
	generator<analyze_DCM::container_DCM>,
	generator<analyze_ISO::container_ISO>,
	generator<analyze_JPEG::container_JPEG>,
	generator<analyze_M2P::container_M2P>,
	generator<analyze_RIFF::container_RIFF>,
	generator<analyze_TIFF::container_TIFF>,
	generator<analyze_PNG::container_PNG>
};


typedef HRESULT (*cn)(agaliaString** name);
static cn container_names[] = {
	analyze_ASF::container_ASF::GetContainerName,
	analyze_BMP::container_BMP::GetContainerName,
	analyze_DCM::container_DCM::GetContainerName,
	analyze_ISO::container_ISO::GetContainerName,
	analyze_JPEG::container_JPEG::GetContainerName,
	analyze_M2P::container_M2P::GetContainerName,
	analyze_RIFF::container_RIFF::GetContainerName,
	analyze_TIFF::container_TIFF::GetContainerName,
	analyze_PNG::container_PNG::GetContainerName
};



HRESULT getAgaliaImage(agaliaContainer** image, const wchar_t* path, uint64_t offset, uint64_t size, int format)
{
	if (image == nullptr) return E_POINTER;

	CComPtr<IStream> stream;
	auto hr = getAgaliaStream(&stream, path, offset, size);
	if (FAILED(hr)) return hr;

	if (format == agalia_format_auto)
	{
		for (int i = 0; i < _countof(generators); i++)
			if (generators[i](image, path, stream))
				return S_OK;
	}
	else if (0 <= format && format < _countof(generators))
	{
		if (generators[format](image, path, stream))
			return S_OK;
	}
	else
	{
		return E_INVALIDARG;
	}
	return HRESULT_FROM_WIN32(ERROR_UNSUPPORTED_TYPE);
}



HRESULT getAgaliaSupportTypeCount(int* count)
{
	if (count == nullptr) return E_POINTER;
	*count = _countof(container_names);
	return S_OK;
}



HRESULT getAgaliaSupportTypeName(int index, agaliaString** name)
{
	if (name == nullptr) return E_POINTER;
	if (index < 0 || _countof(container_names) <= index) return E_INVALIDARG;
	return container_names[index](name);
}





int agalia_pref_dic_lang = PreferredDictonaryLanguage::English;
int agalia_pref_dicom_vr = 0;

HRESULT setAgaliaPreference(int property, int value)
{
	switch (property)
	{
	case dictionary_lang:
		switch (value)
		{
		case PreferredDictonaryLanguage::English:
		case PreferredDictonaryLanguage::Japanese:
			agalia_pref_dic_lang = value;
			return S_OK;
		}
		break;

	case dicom_force_dictionary_vr:
		switch (value)
		{
		case 0:
		case 1:
			agalia_pref_dicom_vr = value;
			return S_OK;
		}
		break;
	}

	return E_INVALIDARG;
}

HRESULT getAgaliaPreference(int property, int* value)
{
	if (value == nullptr) return E_POINTER;

	switch (property)
	{
	case dictionary_lang:
		*value = agalia_pref_dic_lang;
		return S_OK;

	case dicom_force_dictionary_vr:
		*value = agalia_pref_dicom_vr;
		return S_OK;
	}

	return E_INVALIDARG;

}
