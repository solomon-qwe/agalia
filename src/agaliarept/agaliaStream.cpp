#include "pch.h"

#include <intsafe.h>
#include <initguid.h>
#include "agaliaStream.h"

using namespace agalia;



struct agalia::agaliaStreamParam
{
	ULONG m_cRef = 0;

	wchar_t* m_pszPath = nullptr;
	uint64_t offset = 0;
	uint64_t size = 0;

	HANDLE m_hFile = INVALID_HANDLE_VALUE;
	HANDLE m_hMap = NULL;
	void* m_pBase = nullptr;

	uint64_t m_uPos = 0;
};



agaliaStream::agaliaStream()
{
	param = new agaliaStreamParam;
}



agaliaStream::~agaliaStream()
{
	if (param == nullptr)
		return;

	if (param->m_pszPath)
	{
		void* p = param->m_pszPath;
		param->m_pszPath = nullptr;
		free(p);
	}

	if (param->m_pBase != nullptr)
	{
		void* temp = param->m_pBase;
		param->m_pBase = nullptr;
		::UnmapViewOfFile(temp);
	}

	if (param->m_hMap != NULL)
	{
		HANDLE temp = param->m_hMap;
		param->m_hMap = NULL;
		::CloseHandle(temp);
	}

	if (param->m_hFile != INVALID_HANDLE_VALUE)
	{
		HANDLE temp = param->m_hFile;
		param->m_hFile = INVALID_HANDLE_VALUE;
		::CloseHandle(temp);
	}

	{
		agaliaStreamParam* temp = param;
		param = nullptr;
		delete temp;
	}
}



HRESULT agaliaStream::QueryInterface(
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject)
{
	if (ppvObject == nullptr) return E_POINTER;

	if (riid == IID_AgaliaStream)
	{
		*ppvObject = this;
	}
	else if (riid == IID_IStream)
	{
		*ppvObject = static_cast<IUnknown*>(this);
	}
	else if (riid == IID_ISequentialStream)
	{
		*ppvObject = static_cast<ISequentialStream*>(this);
	}
	else if (riid == IID_IUnknown)
	{
		*ppvObject = static_cast<IUnknown*>(this);
	}
	else
	{
		*ppvObject = nullptr;
		return E_NOINTERFACE;
	}
	return S_OK;
}



ULONG agaliaStream::AddRef(void)
{
	return ::InterlockedIncrement(&param->m_cRef);
}



ULONG agaliaStream::Release(void)
{
	ULONG ret = ::InterlockedDecrement(&param->m_cRef);
	if (ret == 0)
	{
		delete this;
	}
	return ret;
}



HRESULT agaliaStream::Read(
	/* [annotation] */
	_Out_writes_bytes_to_(cb, *pcbRead)  void* pv,
	/* [annotation][in] */
	_In_  ULONG cb,
	/* [annotation] */
	_Out_opt_  ULONG* pcbRead)
{
	void* src = reinterpret_cast<char*>(param->m_pBase) + param->offset + param->m_uPos;

	ULONG size;
	if (param->size < param->m_uPos + cb) {
		auto hr = LongLongToULong(param->size - param->m_uPos, &size);
		if (FAILED(hr)) return hr;
	}
	else {
		size = cb;
	}

	memcpy(pv, src, size);
	param->m_uPos += size;
	if (pcbRead) {
		*pcbRead = size;
	}
	return S_OK;
}



HRESULT agaliaStream::Write(
	/* [annotation] */
	_In_reads_bytes_(cb)  const void* pv,
	/* [annotation][in] */
	_In_  ULONG cb,
	/* [annotation] */
	_Out_opt_  ULONG* pcbWritten)
{
	UNREFERENCED_PARAMETER(pv);
	UNREFERENCED_PARAMETER(cb);
	UNREFERENCED_PARAMETER(pcbWritten);
	return E_FAIL;
}



HRESULT agaliaStream::Seek(
	/* [in] */ LARGE_INTEGER dlibMove,
	/* [in] */ DWORD dwOrigin,
	/* [annotation] */
	_Out_opt_  ULARGE_INTEGER* plibNewPosition)
{
	//LARGE_INTEGER newPos = {};
	uint64_t newPos = 0;

	HRESULT hr = E_INVALIDARG;
	if (dwOrigin == STREAM_SEEK_SET)
	{
		hr = LongLongToUInt64(dlibMove.QuadPart, &newPos);
	}
	else if (dwOrigin == STREAM_SEEK_CUR)
	{
		hr = LongLongToUInt64(param->m_uPos + dlibMove.QuadPart, &newPos);
	}
	else if (dwOrigin == STREAM_SEEK_END)
	{
		hr = LongLongToUInt64(param->size + dlibMove.QuadPart, &newPos);
	}

	if (FAILED(hr))
		return hr;

	if (param->size < newPos) {
		return E_FAIL;
	}

	param->m_uPos = newPos;
	if (plibNewPosition) {
		plibNewPosition->QuadPart = newPos;
	}
	return S_OK;
}



HRESULT agaliaStream::SetSize(
	/* [in] */ ULARGE_INTEGER libNewSize)
{
	UNREFERENCED_PARAMETER(libNewSize);
	return E_FAIL;
}



HRESULT agaliaStream::CopyTo(
	/* [annotation][unique][in] */
	_In_  IStream* pstm,
	/* [in] */ ULARGE_INTEGER cb,
	/* [annotation] */
	_Out_opt_  ULARGE_INTEGER* pcbRead,
	/* [annotation] */
	_Out_opt_  ULARGE_INTEGER* pcbWritten)
{
	UNREFERENCED_PARAMETER(pstm);
	UNREFERENCED_PARAMETER(cb);
	UNREFERENCED_PARAMETER(pcbRead);
	UNREFERENCED_PARAMETER(pcbWritten);
	return E_NOTIMPL;
}



HRESULT agaliaStream::Commit(
	/* [in] */ DWORD grfCommitFlags)
{
	UNREFERENCED_PARAMETER(grfCommitFlags);
	return E_FAIL;
}



HRESULT agaliaStream::Revert(void)
{
	return E_FAIL;
}



HRESULT agaliaStream::LockRegion(
	/* [in] */ ULARGE_INTEGER libOffset,
	/* [in] */ ULARGE_INTEGER cb,
	/* [in] */ DWORD dwLockType)
{
	UNREFERENCED_PARAMETER(libOffset);
	UNREFERENCED_PARAMETER(cb);
	UNREFERENCED_PARAMETER(dwLockType);
	return STG_E_INVALIDFUNCTION;
}



HRESULT agaliaStream::UnlockRegion(
	/* [in] */ ULARGE_INTEGER libOffset,
	/* [in] */ ULARGE_INTEGER cb,
	/* [in] */ DWORD dwLockType)
{
	UNREFERENCED_PARAMETER(libOffset);
	UNREFERENCED_PARAMETER(cb);
	UNREFERENCED_PARAMETER(dwLockType);
	return E_FAIL;
}



HRESULT agaliaStream::Stat(
	/* [out] */ __RPC__out STATSTG* pstatstg,
	/* [in] */ DWORD grfStatFlag)
{
	if (pstatstg == nullptr) return E_POINTER;
	memset(pstatstg, 0, sizeof(STATSTG));

	BY_HANDLE_FILE_INFORMATION inf = {};
	::GetFileInformationByHandle(param->m_hFile, &inf);

	if (grfStatFlag & STATFLAG_NONAME) {
		pstatstg->pwcsName = nullptr;
	}
	else {
		rsize_t strleng = wcslen(param->m_pszPath) + 1;
		rsize_t bufsize = sizeof(wchar_t) * strleng;
		wchar_t* buf = reinterpret_cast<wchar_t*>(::CoTaskMemAlloc(bufsize));
		if (buf == nullptr) return E_OUTOFMEMORY;
		wcscpy_s(buf, strleng, param->m_pszPath);
		pstatstg->pwcsName = static_cast<LPOLESTR>(buf);
	}
	pstatstg->type = STGTY_STREAM;
	pstatstg->cbSize.HighPart = inf.nFileSizeHigh;
	pstatstg->cbSize.LowPart = inf.nFileSizeLow;
	pstatstg->mtime = inf.ftLastWriteTime;
	pstatstg->ctime = inf.ftCreationTime;
	pstatstg->atime = inf.ftLastAccessTime;
	pstatstg->clsid = IID_IStream;
	pstatstg->grfMode = STGM_READ;
	return S_OK;
}



HRESULT agaliaStream::Clone(
	/* [out] */ __RPC__deref_out_opt IStream** ppstm)
{
	UNREFERENCED_PARAMETER(ppstm);
	return E_NOTIMPL;
}



HRESULT agaliaStream::GetRawAddr(void* addr)
{
	if (addr == nullptr) return E_POINTER;
	addr = reinterpret_cast<char*>(param->m_pBase) + param->offset + param->m_uPos;
	return S_OK;
}



HRESULT agalia::agalia_create_stream(agaliaStream** stream, const wchar_t* path, uint64_t offset, uint64_t size)
{
	if (path == nullptr) return E_POINTER;
	if (stream == nullptr) return E_POINTER;

	HANDLE hFile = ::CreateFile(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return HRESULT_FROM_WIN32(::GetLastError());
	}

	HANDLE hMap = ::CreateFileMapping(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
	if (hMap == NULL) {
		HRESULT ret = HRESULT_FROM_WIN32(::GetLastError());
		::CloseHandle(hFile);
		return ret;
	}

	LPVOID pBase = ::MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	if (pBase == nullptr) {
		HRESULT ret = HRESULT_FROM_WIN32(::GetLastError());
		::CloseHandle(hMap);
		::CloseHandle(hFile);
		return ret;
	}

	agaliaStream* p = new agaliaStream;
	p->param->m_hFile = hFile;
	p->param->m_hMap = hMap;
	p->param->m_pBase = pBase;

	rsize_t strleng = wcslen(path) + 1;
	rsize_t bufsize = sizeof(wchar_t) * strleng;
	wchar_t* buf = reinterpret_cast<wchar_t*>(malloc(bufsize));
	if (buf == nullptr) {
		delete p;
		return E_OUTOFMEMORY;
	}
	wcscpy_s(buf, strleng, path);
	p->param->m_pszPath = buf;

	LARGE_INTEGER liSize = {};
	::GetFileSizeEx(p->param->m_hFile, &liSize);
	uint64_t filesize = 0;
	auto hr = LongLongToUInt64(liSize.QuadPart, &filesize);
	if (FAILED(hr) || filesize < offset + size) {
		delete p;
		return E_FAIL;
	}

	p->param->size = size ? size : (liSize.QuadPart - offset);
	p->param->offset = offset;
	p->param->m_uPos = 0;

	*stream = p;
	(*stream)->AddRef();
	return S_OK;
}
