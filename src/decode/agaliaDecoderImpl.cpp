#include "pch.h"
#include "../inc/decode.h"
#include "agaliaDecoderImpl.h"

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

agaliaDecoderImpl::agaliaDecoderImpl()
{
}

agaliaDecoderImpl::~agaliaDecoderImpl()
{
	finalize();
}

ULONG agaliaDecoderImpl::AddRef(void)
{
	return ++refCount;
}

ULONG agaliaDecoderImpl::Release(void)
{
	if (--refCount == 0)
	{
		delete this;
		return 0;
	}
	return refCount;
}

HRESULT agaliaDecoderImpl::decode(agaliaBitmap** ppBitmap, const agaliaContainer* image)
{
	if (decoders == nullptr) return E_FAIL;
	for (auto decoder : *decoders)
	{
		HRESULT hr = decoder.decoder->decode(ppBitmap, image);
		if (SUCCEEDED(hr)) return hr;
	}
	return E_FAIL;
}

HRESULT agaliaDecoderImpl::decode(IWICBitmap** ppBitmap, IWICColorContext** ppColorContext, const agaliaContainer* image)
{
	if (decoders == nullptr) return E_FAIL;
	for (auto decoder : *decoders)
	{
		HRESULT hr = decoder.decoder->decode(ppBitmap, ppColorContext, image);
		if (SUCCEEDED(hr)) return hr;
	}
	return E_FAIL;
}

HRESULT agaliaDecoderImpl::decodeThumbnail(agaliaBitmap** ppBitmap, const agaliaContainer* image, uint32_t maxW, uint32_t maxH)
{
	if (decoders == nullptr) return E_FAIL;
	for (auto decoder : *decoders)
	{
		HRESULT hr = decoder.decoder->decodeThumbnail(ppBitmap, image, maxW, maxH);
		if (SUCCEEDED(hr)) return hr;
	}
	return E_FAIL;
}

HRESULT agaliaDecoderImpl::initialize(void)
{
	decoders = new std::list<item>();

	wchar_t path[MAX_PATH] = {};
	::GetModuleFileName(NULL, path, MAX_PATH);
	::PathRemoveFileSpec(path);
	
	wchar_t search_path[MAX_PATH] = {};
	::PathCombine(search_path, path, L"decode_*.dll");

	WIN32_FIND_DATA wfd = {};
	HANDLE hFind = FindFirstFile(search_path, &wfd);
	if (hFind == INVALID_HANDLE_VALUE) return E_FAIL;

	do
	{
		HMODULE hModule = ::LoadLibrary(wfd.cFileName);
		if (hModule == nullptr) continue;

		typedef agaliaDecoder* (*GetAgaliaDecoderFunc)(void);
		GetAgaliaDecoderFunc GetAgaliaDecoder = (GetAgaliaDecoderFunc)GetProcAddress(hModule, "GetAgaliaDecoder");
		if (GetAgaliaDecoder == nullptr) continue;

		agaliaDecoder* decoder = GetAgaliaDecoder();
		if (decoder == nullptr) continue;

		item it = { hModule, decoder };
		decoders->push_back(it);

	} while (::FindNextFile(hFind, &wfd));

	decoders->sort([](const item& a, const item& b) -> bool
		{
			return a.decoder->getPriority() < b.decoder->getPriority();
		});

	return S_OK;
}

HRESULT agaliaDecoderImpl::finalize(void)
{
	if (decoders == nullptr) return S_OK;

	for (auto decoder : *decoders)
	{
		auto tempDecoder = decoder.decoder;
		decoder.decoder = nullptr;
		tempDecoder->Release();

		auto tempModule = decoder.hModule;
		decoder.hModule = NULL;
		if (tempModule)
		{
			::FreeLibrary(tempModule);
		}
	}

	auto temp = decoders;
	decoders = nullptr;
	delete temp;

	return S_OK;
}

HRESULT agaliaDecoderImpl::add(agaliaDecoder* decoder)
{
	if (decoders == nullptr) return E_FAIL;
	if (decoder == nullptr) return E_POINTER;

	item it = { NULL, decoder };
	decoders->push_back(it);
	decoders->sort([](const item& a, const item& b) -> bool
		{
			return a.decoder->getPriority() < b.decoder->getPriority();
		});
	return S_OK;
}

static agaliaDecoderImpl* g_decoder = nullptr;

HRESULT getAgaliaDecoder(agaliaDecoder** decoder)
{
	if (g_decoder == nullptr)
		return agalia::AGALIA_ERR_NOTINITIALIZED;

	*decoder = g_decoder;
	(*decoder)->AddRef();
	return S_OK;
}

HRESULT addAgaliaDecoder(agaliaDecoder* decoder)
{
	if (g_decoder == nullptr)
		return agalia::AGALIA_ERR_NOTINITIALIZED;
	return g_decoder->add(decoder);
}


HRESULT initializeAgaliaDecoder(void)
{
	if (g_decoder) return S_OK;

	agaliaDecoderImpl* temp = new agaliaDecoderImpl();
	if (temp == nullptr) return E_OUTOFMEMORY;

	HRESULT hr = temp->initialize();
	if (SUCCEEDED(hr))
		g_decoder = temp;
	else
		temp->Release();
	return hr;
}


HRESULT finalizeAgaliaDecoder(void)
{
	if (g_decoder == nullptr)
		return agalia::AGALIA_ERR_NOTINITIALIZED;

	auto temp = g_decoder;
	g_decoder = nullptr;
	temp->finalize();
	temp->Release();
	return S_OK;
}
