#include "pch.h"
#include "../inc/agaliarept.h"

#include <stdlib.h>

// string wrapper 

agaliaString::agaliaString()
{
}



agaliaString::agaliaString(const wchar_t* src)
{
	Copy(src);
}



agaliaString::~agaliaString()
{
	Free();
}



void agaliaString::Release(void)
{
	delete this;
}



void agaliaString::Free(void)
{
	if (_p)
	{
		auto temp = _p;
		_p = nullptr;
		free(temp);
	}
}



HRESULT agaliaString::Alloc(rsize_t size)
{
	Free();
	_p = reinterpret_cast<wchar_t*>(malloc(sizeof(wchar_t) * size));
	return _p ? S_OK : E_OUTOFMEMORY;
}



HRESULT agaliaString::Copy(const wchar_t* src)
{
	if (src == nullptr) return E_POINTER;

	size_t len = wcslen(src) + 1;
	auto hr = Alloc(len);
	if (FAILED(hr)) return hr;

	errno_t err = wcscpy_s(_p, len, src);
	return (err == 0) ? S_OK : E_FAIL;
}



agaliaString* agaliaString::create(const wchar_t* src)
{
	agaliaStringPtr s(new agaliaString);
	if (src)
		if (FAILED(s->Copy(src)))
			return nullptr;
	return s.detach();
}
