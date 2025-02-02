#include "pch.h"
#include "agaliaImageBase.h"

#include "agaliareptImpl.h"



_agaliaContainerBase::_agaliaContainerBase(const wchar_t* path, IStream* stream)
{
	::InitializeCriticalSection(&cs);
	data_stream = stream;
	data_stream->AddRef();
	file_path = agaliaString::create(path);
}



_agaliaContainerBase::~_agaliaContainerBase()
{
	if (data_stream)
	{
		auto temp = data_stream;
		data_stream = nullptr;
		temp->Release();
	}

	if (file_path)
	{
		auto temp = file_path;
		file_path = nullptr;
		temp->Release();
	}
	::DeleteCriticalSection(&cs);
}



void _agaliaContainerBase::Release(void)
{
	delete this;
}



HRESULT _agaliaContainerBase::getElementInfoCount(const agaliaElement* item, uint32_t* row) const
{
	UNREFERENCED_PARAMETER(item);
	if (row == nullptr) return E_POINTER;
	*row = 0;
	return S_OK;
}



HRESULT _agaliaContainerBase::getElementInfoValue(const agaliaElement*, uint32_t, uint32_t, agaliaString**) const
{
	return E_FAIL;
}



HRESULT _agaliaContainerBase::LockStream(void) const
{
	::EnterCriticalSection(const_cast<CRITICAL_SECTION*>(&cs));
	return S_OK;
}



HRESULT _agaliaContainerBase::UnlockStream(void) const
{
	::LeaveCriticalSection(const_cast<CRITICAL_SECTION*>(&cs));
	return S_OK;
}



HRESULT _agaliaContainerBase::ReadData(void* buf, uint64_t pos, uint64_t size) const
{
	if (buf == nullptr) return E_POINTER;

	ULONG readSize = 0;
	auto hr = UInt64ToULong(size, &readSize);
	if (FAILED(hr)) return hr;

	ULONG cbRead = 0;

	LockStream();
	hr = data_stream->Seek(uint64_to_li(pos), STREAM_SEEK_SET, nullptr);
	if (SUCCEEDED(hr)) {
		hr = data_stream->Read(buf, readSize, &cbRead);
	}
	UnlockStream();
	if (hr != S_OK) return E_FAIL;
	if (size != cbRead) return E_FAIL;

	return S_OK;
}



HRESULT _agaliaContainerBase::getFilePath(agaliaString** str) const
{
	if (str == nullptr) return E_POINTER;
	if (file_path == nullptr) return E_FAIL;
	auto ret = agaliaString::create(file_path->_p);
	if (ret == nullptr) return E_OUTOFMEMORY;
	*str = ret;
	return S_OK;
}



HRESULT _agaliaContainerBase::getAsocStream(IStream** stream) const
{
	if (stream == nullptr) return E_POINTER;
	*stream = data_stream;
	if (data_stream)
		data_stream->AddRef();
	return S_OK;
}
