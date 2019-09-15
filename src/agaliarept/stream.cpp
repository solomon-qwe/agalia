#include "pch.h"
#include "agaliareptImpl.h"

#include "agaliaStream.h"

HRESULT getAgaliaStream(IStream** stream, const wchar_t* path, uint64_t offset, uint64_t size)
{
	agaliaStream* s = nullptr;
	auto hr = agalia_create_stream(&s, path, offset, size);
	if (FAILED(hr)) return hr;
	*stream = s;
	return S_OK;
}
