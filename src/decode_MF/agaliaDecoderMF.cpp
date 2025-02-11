#include "pch.h"
#include "agaliaDecoderMF.h"

#include <atlbase.h>

// for Media Foundation
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")



static HRESULT calculateThumbnailSize(UINT orgW, UINT orgH, UINT maxW, UINT maxH, UINT* thumbWidth, UINT* thumbHeight)
{
	if (!thumbWidth || !thumbHeight) return E_POINTER;
	if (orgH == 0 || orgW == 0) return E_INVALIDARG;

	double scaleH = (maxH == 0) ? 1 : (double)maxH / orgH;
	double scaleW = (maxW == 0) ? 1 : (double)maxW / orgW;
	double scale = min(scaleH, scaleW);
	*thumbWidth = static_cast<UINT>(orgW * scale);
	*thumbHeight = static_cast<UINT>(orgH * scale);

	return S_OK;
}


static HRESULT createHBitmap(UINT width, UINT height, HBITMAP* phBitmap, void** ppBits)
{
	if (!phBitmap || !ppBits) return E_POINTER;

	BITMAPINFOHEADER bmi = {};
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biWidth = width;
	bmi.biHeight = height;
	bmi.biPlanes = 1;
	bmi.biBitCount = 32;
	bmi.biCompression = BI_RGB;

	HDC hMemDC = ::CreateCompatibleDC(NULL);
	if (hMemDC == NULL) return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ::GetLastError());

	void* pBits = nullptr;
	HBITMAP hBitmap = ::CreateDIBSection(hMemDC, reinterpret_cast<BITMAPINFO*>(&bmi), DIB_RGB_COLORS, &pBits, NULL, 0);
	DWORD dwErr = ::GetLastError();

	::DeleteDC(hMemDC);
	if (!hBitmap) return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, dwErr);

	*phBitmap = hBitmap;
	*ppBits = pBits;
	return S_OK;
}










static HRESULT initializeMFSourceReader(IStream* stream, IMFSourceReader** ppSourceReader)
{
	if (!stream || !ppSourceReader) return E_POINTER;
	if (*ppSourceReader) return E_INVALIDARG;

	CComPtr<IMFAttributes> spAttributes;
	HRESULT hr = ::MFCreateAttributes(&spAttributes, 1);
	if (FAILED(hr)) return hr;

	hr = spAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
	if (FAILED(hr)) return hr;

	CComPtr<IMFByteStream> spByteStream;
	hr = ::MFCreateMFByteStreamOnStream(stream, &spByteStream);
	if (FAILED(hr)) return hr;

	hr = ::MFCreateSourceReaderFromByteStream(spByteStream, spAttributes, ppSourceReader);
	return hr;
}


static HRESULT getFirstVideoSample(IMFSourceReader* pSourceReader, IMFSample** ppSample)
{
	if (!pSourceReader || !ppSample) return E_POINTER;
	if (*ppSample) return E_INVALIDARG;

	CComPtr<IMFMediaType> spMediaType;
	HRESULT hr = ::MFCreateMediaType(&spMediaType);
	if (FAILED(hr)) return hr;

	hr = spMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	if (FAILED(hr)) return hr;

	hr = spMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
	if (FAILED(hr)) return hr;

	hr = pSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, spMediaType);
	if (FAILED(hr)) return hr;

	DWORD flags = 0;
	hr = pSourceReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, &flags, NULL, ppSample);
	return S_OK;
}

static HRESULT setupVideoProcessor(IMFSourceReader* pSourceReader, UINT32 thumbWidth, UINT32 thumbHeight, IMFSample* pSample, IMFTransform** ppVideoProcessor)
{
	if (!pSourceReader || !pSample || !ppVideoProcessor) return E_POINTER;
	if (*ppVideoProcessor) return E_INVALIDARG;

	CComPtr<IMFMediaType> spMediaType;
	HRESULT hr = pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &spMediaType);
	if (FAILED(hr)) return hr;

	CComPtr<IMFTransform> spVideoProcessor;
	hr = spVideoProcessor.CoCreateInstance(CLSID_VideoProcessorMFT, nullptr, CLSCTX_INPROC_SERVER);
	if (FAILED(hr)) return hr;

	hr = spVideoProcessor->SetInputType(0, spMediaType, 0);
	if (FAILED(hr)) return hr;

	CComPtr<IMFMediaType> spOutputType;
	hr = ::MFCreateMediaType(&spOutputType);
	if (FAILED(hr)) return hr;

	hr = spOutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	if (FAILED(hr)) return hr;

	hr = spOutputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
	if (FAILED(hr)) return hr;

	hr = ::MFSetAttributeSize(spOutputType, MF_MT_FRAME_SIZE, thumbWidth, thumbHeight);
	if (FAILED(hr)) return hr;

	hr = spVideoProcessor->SetOutputType(0, spOutputType, 0);
	if (FAILED(hr)) return hr;

	hr = spVideoProcessor->ProcessInput(0, pSample, 0);
	if (FAILED(hr)) return hr;

	*ppVideoProcessor = spVideoProcessor.Detach();
	return S_OK;
}

static HRESULT processVideoFrame(IMFTransform* pVideoProcessor, UINT32 thumbWidth, UINT32 thumbHeight, IMFMediaBuffer** ppOutputBuffer)
{
	if (!pVideoProcessor || !ppOutputBuffer) return E_POINTER;
	if (*ppOutputBuffer) return E_INVALIDARG;

	CComPtr<IMFSample> spOutputSample;
	HRESULT hr = ::MFCreateSample(&spOutputSample);
	if (FAILED(hr)) return hr;

	CComPtr<IMFMediaBuffer> spOutputBuffer;
	hr = ::MFCreateMemoryBuffer(thumbWidth * thumbHeight * 4, &spOutputBuffer);
	if (FAILED(hr)) return hr;

	hr = spOutputSample->AddBuffer(spOutputBuffer);
	if (FAILED(hr)) return hr;

	MFT_OUTPUT_DATA_BUFFER outputDataBuffer = { 0 };
	outputDataBuffer.pSample = spOutputSample;

	DWORD processOutputStatus = 0;
	hr = pVideoProcessor->ProcessOutput(0, 1, &outputDataBuffer, &processOutputStatus);
	if (FAILED(hr)) return hr;

	// WMVやAVIをロードするとアルファチャンネルが0クリア（透過）されてしまうので0xFF（不透過）に設定する 
	BYTE* p = nullptr;
	DWORD cb = 0;
	spOutputBuffer->Lock(&p, nullptr, &cb);
	for (DWORD i = 3; i < cb; i += 4)
		p[i] = 0xFF;
	spOutputBuffer->Unlock();

	*ppOutputBuffer = spOutputBuffer.Detach();
	return S_OK;
}


static HRESULT generateThumbnailMF(HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH, IStream* stream)
{
	if (!stream || !phBitmap) return E_POINTER;

	CComPtr<IMFSourceReader> spSourceReader;
	HRESULT hr = initializeMFSourceReader(stream, &spSourceReader);
	if (FAILED(hr)) return hr;

	CComPtr<IMFSample> spSample;
	hr = getFirstVideoSample(spSourceReader, &spSample);
	if (FAILED(hr)) return hr;

	CComPtr<IMFMediaType> spMediaType;
	hr = spSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &spMediaType);
	if (FAILED(hr)) return hr;

	UINT32 width = 0, height = 0;
	hr = ::MFGetAttributeSize(spMediaType, MF_MT_FRAME_SIZE, &width, &height);
	if (FAILED(hr)) return hr;

	UINT32 thumbWidth = 0, thumbHeight = 0;
	hr = calculateThumbnailSize(width, height, maxW, maxH, &thumbWidth, &thumbHeight);
	if (FAILED(hr)) return hr;

	CComPtr<IMFTransform> spVideoProcessor;
	hr = setupVideoProcessor(spSourceReader, thumbWidth, thumbHeight, spSample, &spVideoProcessor);
	if (FAILED(hr)) return hr;

	CComPtr<IMFMediaBuffer> spMediaBuffer;
	hr = processVideoFrame(spVideoProcessor, thumbWidth, thumbHeight, &spMediaBuffer);
	if (FAILED(hr)) return hr;

	BYTE* pBuffer = nullptr;
	DWORD cbBuffer = 0;
	hr = spMediaBuffer->Lock(&pBuffer, nullptr, &cbBuffer);
	if (FAILED(hr)) return hr;

	void* pBits = nullptr;
	hr = createHBitmap(thumbWidth, thumbHeight, phBitmap, &pBits);
	if (SUCCEEDED(hr)) {
		memcpy(pBits, pBuffer, cbBuffer);
	}

	spMediaBuffer->Unlock();

	return hr;
}



extern "C"
__declspec(dllexport) agaliaDecoder* GetAgaliaDecoder(void)
{
	return new agaliaDecoderMF();
}


agaliaDecoderMF::agaliaDecoderMF()
{
}

agaliaDecoderMF::~agaliaDecoderMF()
{
}

ULONG agaliaDecoderMF::AddRef(void)
{
	return ++refCount;
}

ULONG agaliaDecoderMF::Release(void)
{
	if (--refCount == 0)
	{
		delete this;
		return 0;
	}
	return refCount;
}

HRESULT agaliaDecoderMF::decode(IStream* stream, HBITMAP* phBitmap)
{
	HRESULT hr = ::MFStartup(MF_VERSION);
	if (SUCCEEDED(hr))
	{
		hr = generateThumbnailMF(phBitmap, 0, 0, stream);
		::MFShutdown();
	}
	return hr;
}

HRESULT agaliaDecoderMF::decode(IStream* stream, agaliaHeap** bmpInfo, void** ppBits)
{
	UNREFERENCED_PARAMETER(stream);
	UNREFERENCED_PARAMETER(bmpInfo);
	UNREFERENCED_PARAMETER(ppBits);

	return E_NOTIMPL;
}

HRESULT agaliaDecoderMF::decode(IStream* stream, uint32_t maxW, uint32_t maxH, HBITMAP* phBitmap)
{
	HRESULT hr = ::MFStartup(MF_VERSION);
	if (SUCCEEDED(hr))
	{
		hr = generateThumbnailMF(phBitmap, maxW, maxH, stream);
		::MFShutdown();
	}
	return hr;
}

HRESULT agaliaDecoderMF::decode(IStream* stream, uint32_t maxW, uint32_t maxH, agaliaHeap** bmpInfo, void** ppBits)
{
	UNREFERENCED_PARAMETER(stream);
	UNREFERENCED_PARAMETER(maxW);
	UNREFERENCED_PARAMETER(maxH);
	UNREFERENCED_PARAMETER(bmpInfo);
	UNREFERENCED_PARAMETER(ppBits);

	return E_NOTIMPL;
}
