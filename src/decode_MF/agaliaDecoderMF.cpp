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

#include <wincodec.h>



static HRESULT LoadImageMF(IWICBitmap** ppBitmap, IStream* stream)
{
	if (!stream || !ppBitmap)
		return E_INVALIDARG;

	HRESULT hr = S_OK;

	CComPtr<IMFByteStream> spByteStream;
	hr = ::MFCreateMFByteStreamOnStream(stream, &spByteStream);
	if (FAILED(hr)) return hr;

	CComPtr<IMFAttributes> spAttributes;
	hr = ::MFCreateAttributes(&spAttributes, 1);
	if (FAILED(hr)) return hr;

	hr = spAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING, TRUE);
	if (FAILED(hr)) {
		hr = spAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
		if (FAILED(hr)) return hr;
	}

	CComPtr<IMFSourceReader> spSourceReader;
	hr = ::MFCreateSourceReaderFromByteStream(spByteStream, spAttributes, &spSourceReader);
	if (FAILED(hr)) return hr;

	CComPtr<IMFMediaType> spMediaType;
	hr = ::MFCreateMediaType(&spMediaType);
	if (FAILED(hr)) return hr;

	hr = spMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	if (FAILED(hr)) return hr;

	GUID subtype = MFVideoFormat_ARGB32;
	hr = spMediaType->SetGUID(MF_MT_SUBTYPE, subtype);
	if (FAILED(hr)) {
		subtype = MFVideoFormat_RGB32;
		hr = spMediaType->SetGUID(MF_MT_SUBTYPE, subtype);
		if (FAILED(hr)) return hr;
	}

	hr = spSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, spMediaType);
	if (FAILED(hr)) return hr;

	DWORD flags = 0;
	CComPtr<IMFSample> spSample;
	hr = spSourceReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, nullptr, &flags, nullptr, &spSample);
	if (FAILED(hr)) return hr;

	CComPtr<IMFMediaType> spCurrentMediaType;
	hr = spSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &spCurrentMediaType);
	if (FAILED(hr)) return hr;

	UINT32 width = 0, height = 0;
	hr = ::MFGetAttributeSize(spCurrentMediaType, MF_MT_FRAME_SIZE, &width, &height);
	if (FAILED(hr)) return hr;

	CComPtr<IMFMediaBuffer> spMediaBuffer;
	hr = spSample->GetBufferByIndex(0, &spMediaBuffer);
	if (FAILED(hr)) return hr;

	BYTE* pData = nullptr;
	DWORD currentLength = 0;
	hr = spMediaBuffer->Lock(&pData, nullptr, &currentLength);
	if (FAILED(hr)) return hr;

	CComPtr<IWICImagingFactory> wicFactory;
	hr = ::CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
	if (FAILED(hr)) return hr;

	UINT stride = width * 4;
	hr = wicFactory->CreateBitmapFromMemory(
		width, height,
		GUID_WICPixelFormat32bppBGRA,
		stride, currentLength,
		pData,
		ppBitmap);
	if (FAILED(hr)) return hr;

	spMediaBuffer->Unlock();

	return S_OK;
}

static HRESULT LoadImageMF(agaliaBitmap** ppBitmap, IStream* stream, uint32_t maxW, uint32_t maxH)
{
	if (!stream || !ppBitmap)
		return E_INVALIDARG;

	HRESULT hr = S_OK;

	CComPtr<IMFByteStream> spByteStream;
	hr = ::MFCreateMFByteStreamOnStream(stream, &spByteStream);
	if (FAILED(hr)) return hr;

	CComPtr<IMFAttributes> spAttributes;
	hr = ::MFCreateAttributes(&spAttributes, 1);
	if (FAILED(hr)) return hr;

	hr = spAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING, TRUE);
	if (FAILED(hr)) {
		hr = spAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
		if (FAILED(hr)) return hr;
	}

	CComPtr<IMFSourceReader> spSourceReader;
	hr = ::MFCreateSourceReaderFromByteStream(spByteStream, spAttributes, &spSourceReader);
	if (FAILED(hr)) return hr;

	CComPtr<IMFMediaType> spMediaType;
	hr = ::MFCreateMediaType(&spMediaType);
	if (FAILED(hr)) return hr;

	hr = spMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	if (FAILED(hr)) return hr;

	GUID subtype = MFVideoFormat_ARGB32;
	hr = spMediaType->SetGUID(MF_MT_SUBTYPE, subtype);
	if (FAILED(hr)) {
		subtype = MFVideoFormat_RGB32;
		hr = spMediaType->SetGUID(MF_MT_SUBTYPE, subtype);
		if (FAILED(hr)) return hr;
	}

	hr = spSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, spMediaType);
	if (FAILED(hr)) return hr;

	DWORD flags = 0;
	CComPtr<IMFSample> spSample;
	hr = spSourceReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, nullptr, &flags, nullptr, &spSample);
	if (FAILED(hr)) return hr;

	CComPtr<IMFMediaType> spCurrentMediaType;
	hr = spSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &spCurrentMediaType);
	if (FAILED(hr)) return hr;

	UINT32 srcWidth = 0, srcHeight = 0;
	hr = ::MFGetAttributeSize(spCurrentMediaType, MF_MT_FRAME_SIZE, &srcWidth, &srcHeight);
	if (FAILED(hr)) return hr;

	UINT targetWidth = srcWidth;
	UINT targetHeight = srcHeight;
	if (maxW != 0 && maxH != 0 && (maxW < srcWidth || maxH < srcHeight))
	{
		double scaleX = static_cast<double>(maxW) / srcWidth;
		double scaleY = static_cast<double>(maxH) / srcHeight;
		double scale = (scaleX < scaleY) ? scaleX : scaleY;
		targetWidth = static_cast<UINT>(srcWidth * scale);
		targetHeight = static_cast<UINT>(srcHeight * scale);
	}

	CComPtr<IMFTransform> spVideoProcessor;
	hr = spVideoProcessor.CoCreateInstance(CLSID_VideoProcessorMFT, nullptr, CLSCTX_INPROC_SERVER);
	if (FAILED(hr)) return hr;

	hr = spVideoProcessor->SetInputType(0, spCurrentMediaType, 0);
	if (FAILED(hr)) return hr;

	CComPtr<IMFMediaType> spOutputType;
	hr = ::MFCreateMediaType(&spOutputType);
	if (FAILED(hr)) return hr;

	hr = spOutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	if (FAILED(hr)) return hr;

	hr = spOutputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_ARGB32);
	if (FAILED(hr)) return hr;

	hr = ::MFSetAttributeSize(spOutputType, MF_MT_FRAME_SIZE, targetWidth, targetHeight);
	if (FAILED(hr)) return hr;

	hr = spVideoProcessor->SetOutputType(0, spOutputType, 0);
	if (FAILED(hr)) return hr;

	hr = spVideoProcessor->ProcessInput(0, spSample, 0);
	if (FAILED(hr)) return hr;

	CComPtr<IMFSample> spOutputSample;
	hr = ::MFCreateSample(&spOutputSample);
	if (FAILED(hr)) return hr;

	CComPtr<IMFMediaBuffer> spMediaBuffer;
	hr = ::MFCreateMemoryBuffer(4 * targetWidth * targetHeight, &spMediaBuffer);
	if (FAILED(hr)) return hr;

	hr = spOutputSample->AddBuffer(spMediaBuffer);
	if (FAILED(hr)) return hr;

	MFT_OUTPUT_DATA_BUFFER outputDataBuffer = { 0 };
	outputDataBuffer.pSample = spOutputSample;

	DWORD processOutputStatus = 0;
	hr = spVideoProcessor->ProcessOutput(0, 1, &outputDataBuffer, &processOutputStatus);
	if (FAILED(hr)) return hr;

	BYTE* pData = nullptr;
	DWORD currentLength = 0;
	hr = spMediaBuffer->Lock(&pData, nullptr, &currentLength);
	if (FAILED(hr)) return hr;

	BITMAPV5HEADER bi = {};
	bi.bV5Size = sizeof(BITMAPV5HEADER);
	bi.bV5Width = targetWidth;
	bi.bV5Height = targetHeight;
	bi.bV5Planes = 1;
	bi.bV5BitCount = 32;
	bi.bV5Compression = BI_BITFIELDS;
	bi.bV5RedMask = 0x00FF0000;
	bi.bV5GreenMask = 0x0000FF00;
	bi.bV5BlueMask = 0x000000FF;
	bi.bV5AlphaMask = 0x00000000;

	agaliaPtr<agaliaBitmap> bitmap;
	hr = createAgaliaBitmap(&bitmap, &bi);
	if (FAILED(hr)) return hr;

	void* pBits = nullptr;
	hr = bitmap->getBits(&pBits);
	if (FAILED(hr)) return hr;

	size_t size = 4 * targetWidth * targetHeight;
	memcpy(pBits, pData, size);

	spMediaBuffer->Unlock();

	*ppBitmap = bitmap.detach();

	return S_OK;
}



extern "C"
__declspec(dllexport) agaliaDecoder* GetAgaliaDecoder(void)
{
	return new agaliaDecoderMF();
}


agaliaDecoderMF::agaliaDecoderMF()
{
	hrInit = ::MFStartup(MF_VERSION);
}

agaliaDecoderMF::~agaliaDecoderMF()
{
	if (SUCCEEDED(hrInit))
		::MFShutdown();
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

HRESULT agaliaDecoderMF::decode(agaliaBitmap** ppBitmap, const agaliaContainer* image)
{
	if (!ppBitmap || !image) return E_POINTER;

	CComPtr<IStream> stream;
	auto hr = image->getAsocStream(&stream);
	if (FAILED(hr)) return hr;

	stream->Seek({ 0 }, STREAM_SEEK_SET, nullptr);

	return LoadImageMF(ppBitmap, stream, 0, 0);
}

HRESULT agaliaDecoderMF::decode(IWICBitmap** ppBitmap, IWICColorContext** ppColorContext, const agaliaContainer* image)
{
	if (!ppBitmap || !ppColorContext || !image) return E_POINTER;

	CComPtr<IStream> stream;
	auto hr = image->getAsocStream(&stream);
	if (FAILED(hr)) return hr;

	stream->Seek({ 0 }, STREAM_SEEK_SET, nullptr);

	hr = LoadImageMF(ppBitmap, stream);
	if (FAILED(hr)) return hr;

	*ppColorContext = nullptr;

	return S_OK;
}

HRESULT agaliaDecoderMF::decodeThumbnail(agaliaBitmap** ppBitmap, const agaliaContainer* image, uint32_t maxW, uint32_t maxH)
{
	if (!ppBitmap || !image) return E_POINTER;

	CComPtr<IStream> stream;
	auto hr = image->getAsocStream(&stream);
	if (FAILED(hr)) return hr;

	stream->Seek({ 0 }, STREAM_SEEK_SET, nullptr);

	return LoadImageMF(ppBitmap, stream, maxW, maxH);
}
