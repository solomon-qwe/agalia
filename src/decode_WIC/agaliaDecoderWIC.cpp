#include "pch.h"
#include "agaliaDecoderWIC.h"

#include <wincodec.h>
#include <atlbase.h>



static HRESULT LoadThumbnailWIC(agaliaBitmap** ppBitmap, IStream* stream, uint32_t maxW, uint32_t maxH)
{
    if (!ppBitmap || !stream)
        return E_INVALIDARG;

    HRESULT hr = S_OK;

    CComPtr<IWICImagingFactory> wicFactory;
    hr = ::CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
    if (FAILED(hr)) return hr;

    CComPtr<IWICBitmapDecoder> decoder;
    hr = wicFactory->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnDemand, &decoder);
    if (FAILED(hr)) return hr;

    CComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) return hr;

    CComPtr<IWICFormatConverter> converter;
    hr = wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr)) return hr;

    hr = converter->Initialize(
        frame,
        GUID_WICPixelFormat32bppBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeMedianCut);
    if (FAILED(hr)) return hr;

    UINT srcWidth = 0, srcHeight = 0;
    hr = converter->GetSize(&srcWidth, &srcHeight);
    if (FAILED(hr)) return hr;

    UINT targetWidth = srcWidth;
    UINT targetHeight = srcHeight;
    CComPtr<IWICBitmapSource> source;
    if (maxW < srcWidth || maxH < srcHeight)
    {
        CComPtr<IWICBitmapScaler> scaler;
        hr = wicFactory->CreateBitmapScaler(&scaler);
        if (FAILED(hr)) return hr;

        double scaleX = static_cast<double>(maxW) / srcWidth;
        double scaleY = static_cast<double>(maxH) / srcHeight;
        double scale = (scaleX < scaleY) ? scaleX : scaleY;
        targetWidth = static_cast<UINT>(srcWidth * scale);
        targetHeight = static_cast<UINT>(srcHeight * scale);

        hr = scaler->Initialize(converter, targetWidth, targetHeight, WICBitmapInterpolationModeFant);
        if (FAILED(hr)) return hr;

        source = scaler;
    }
    else
    {
        source = converter;
    }

    CComPtr<IWICBitmapFlipRotator> flipRotator;
    hr = wicFactory->CreateBitmapFlipRotator(&flipRotator);
    if (FAILED(hr)) return hr;

    hr = flipRotator->Initialize(source, WICBitmapTransformFlipVertical);
    if (FAILED(hr)) return hr;

    BITMAPV5HEADER bi = {};
    bi.bV5Size = sizeof(BITMAPV5HEADER);
    bi.bV5Width = targetWidth;
    bi.bV5Height = targetHeight;
    bi.bV5Planes = 1;
    bi.bV5BitCount = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask   = 0xFFu << (8 * 2);
    bi.bV5GreenMask = 0xFFu << (8 * 1);
    bi.bV5BlueMask  = 0xFFu << (8 * 0);
    bi.bV5AlphaMask = 0xFFu << (8 * 3);

	agaliaPtr<agaliaBitmap> bitmap;
	hr = createAgaliaBitmap(&bitmap, &bi);
	if (FAILED(hr)) return hr;

    void* pBits = nullptr;
	bitmap->getBits(&pBits);

    const UINT stride = targetWidth * 4;
	const UINT bufferSize = stride * targetHeight;
    hr = flipRotator->CopyPixels(nullptr, stride, bufferSize, static_cast<BYTE*>(pBits));
    if (FAILED(hr)) return hr;

    *ppBitmap = bitmap.detach();
    
    return S_OK;
}

static HRESULT LoadImageWIC(agaliaBitmap** ppBitmap, IStream* stream)
{
    if (!ppBitmap || !stream)
        return E_INVALIDARG;

    HRESULT hr = S_OK;

    CComPtr<IWICImagingFactory> wicFactory;
    hr = ::CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
    if (FAILED(hr)) return hr;

    CComPtr<IWICBitmapDecoder> decoder;
    hr = wicFactory->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnLoad, &decoder);
    if (FAILED(hr)) return hr;

    CComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) return hr;

    WICPixelFormatGUID pixelFormatGUID = { 0 };
    hr = frame->GetPixelFormat(&pixelFormatGUID);
    if (FAILED(hr)) return hr;

    CComPtr<IWICComponentInfo> componentInfo;
	hr = wicFactory->CreateComponentInfo(pixelFormatGUID, &componentInfo);
	if (FAILED(hr)) return hr;

	CComPtr<IWICPixelFormatInfo> pixelFormatInfo;
	hr = componentInfo.QueryInterface(&pixelFormatInfo);
	if (FAILED(hr)) return hr;

	UINT channelCount = 0;
	hr = pixelFormatInfo->GetChannelCount(&channelCount);
	if (FAILED(hr)) return hr;

    CComPtr<IWICFormatConverter> converter;
    hr = wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr))
        return hr;

    bool useAlpha = (3 < channelCount);

    hr = converter->Initialize(
        frame,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeMedianCut);
    if (FAILED(hr)) return hr;

    UINT width = 0, height = 0;
    hr = converter->GetSize(&width, &height);
    if (FAILED(hr)) return hr;

    CComPtr<IWICBitmapFlipRotator> flipRotator;
    hr = wicFactory->CreateBitmapFlipRotator(&flipRotator);
    if (FAILED(hr)) return hr;

    hr = flipRotator->Initialize(converter, WICBitmapTransformFlipVertical);
    if (FAILED(hr)) return hr;

    BITMAPV5HEADER bi = {};
    bi.bV5Size = sizeof(BITMAPV5HEADER);
    bi.bV5Width = width;
    bi.bV5Height = height;
    bi.bV5Planes = 1;
    bi.bV5BitCount = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask   = 0xFFu << (8 * 2);
    bi.bV5GreenMask = 0xFFu << (8 * 1);
    bi.bV5BlueMask  = 0xFFu << (8 * 0);
    bi.bV5AlphaMask = useAlpha ? (0xFFu << (8 * 3)) : 0;

	agaliaPtr<agaliaBitmap> bitmap;
	hr = createAgaliaBitmap(&bitmap, &bi);
	if (FAILED(hr)) return hr;

	void* pBits = nullptr;
	bitmap->getBits(&pBits);

    const UINT stride = width * 4;
	const UINT bufferSize = stride * height;
    hr = flipRotator->CopyPixels(nullptr, stride, bufferSize, static_cast<BYTE*>(pBits));
    if (FAILED(hr)) return hr;

	*ppBitmap = bitmap.detach();
    return S_OK;
}

static HRESULT LoadImageWIC(IWICBitmap** ppBitmap, IWICColorContext** ppColorContext, IStream* stream)
{
    if (ppBitmap == nullptr) return E_POINTER;
	if (ppColorContext == nullptr) return E_POINTER;
    if (stream == nullptr) return E_POINTER;

    HRESULT hr = S_OK;

    CComPtr<IWICImagingFactory> wicFactory;
    hr = ::CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
    if (FAILED(hr)) return hr;

    CComPtr<IWICBitmapDecoder> decoder;
    hr = wicFactory->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnDemand, &decoder);
    if (FAILED(hr)) return hr;

    CComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) return hr;

    CComPtr<IWICFormatConverter> converter;
    hr = wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr)) return hr;

    WICPixelFormatNumericRepresentation pixelFormat;
    {
        WICPixelFormatGUID pixelFormatGUID;
        hr = frame->GetPixelFormat(&pixelFormatGUID);
        if (FAILED(hr)) return hr;

        CComPtr<IWICComponentInfo> spWicComponentInfo;
        hr = wicFactory->CreateComponentInfo(pixelFormatGUID, &spWicComponentInfo);
        if (FAILED(hr)) return hr;

        CComPtr<IWICPixelFormatInfo2> spWicPixelFormatInfo2;
		hr = spWicComponentInfo.QueryInterface(&spWicPixelFormatInfo2);
        if (FAILED(hr)) return hr;

        hr = spWicPixelFormatInfo2->GetNumericRepresentation(&pixelFormat);
        if (FAILED(hr)) return hr;
    }

    WICPixelFormatGUID dstFormat =
        (pixelFormat == WICPixelFormatNumericRepresentationFloat) ?
        GUID_WICPixelFormat64bppRGBAHalf :
        GUID_WICPixelFormat64bppRGBA;

    hr = converter->Initialize(
        frame,
        dstFormat,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeMedianCut);
    if (FAILED(hr)) return hr;

    CComPtr<IWICBitmap> bitmap;
    hr = wicFactory->CreateBitmapFromSource(converter, WICBitmapCacheOnDemand, &bitmap);
    if (FAILED(hr)) return hr;

    CComPtr<IWICColorContext> colorContext;
    hr = wicFactory->CreateColorContext(&colorContext);
    if (FAILED(hr)) return hr;

    unsigned int actualCount = 0;
    hr = frame->GetColorContexts(1, &colorContext.p, &actualCount);
    *ppColorContext = (SUCCEEDED(hr) && 0 < actualCount) ? colorContext.Detach() : nullptr;

    *ppBitmap = bitmap.Detach();

    return S_OK;
}


extern "C"
__declspec(dllexport) agaliaDecoder* GetAgaliaDecoder(void)
{
	return new agaliaDecoderWIC();
}


agaliaDecoderWIC::agaliaDecoderWIC()
{
}

agaliaDecoderWIC::~agaliaDecoderWIC()
{
}

ULONG agaliaDecoderWIC::AddRef(void)
{
	return ++refCount;
}

ULONG agaliaDecoderWIC::Release(void)
{
	if (--refCount == 0)
	{
		delete this;
		return 0;
	}
	return refCount;
}

HRESULT agaliaDecoderWIC::decode(agaliaBitmap** ppBitmap, const agaliaContainer* image)
{
    if (!image || !ppBitmap) return E_POINTER;

    CComPtr<IStream> stream;
    auto hr = image->getAsocStream(&stream);
    if (FAILED(hr)) return hr;

    stream->Seek({ 0 }, STREAM_SEEK_SET, nullptr);
    
    return LoadImageWIC(ppBitmap, stream);
}

#include <io.h>

static HRESULT LoadFile(CHeapPtr<BYTE>& buf, long* file_size, const wchar_t* path)
{
    if (path == nullptr) return E_POINTER;
    if (file_size == nullptr) return E_POINTER;
    if (buf.m_pData) return E_FAIL;

    FILE* fp = nullptr;
    errno_t err = _wfopen_s(&fp, path, L"rb");
    if (err != 0) return E_FAIL;
    if (fp == nullptr) return E_FAIL;

    HRESULT ret = E_FAIL;
    *file_size = _filelength(_fileno(fp));
    if (buf.AllocateBytes(*file_size))
        if (fread(buf, *file_size, 1, fp) == 1)
            ret = S_OK;

    fclose(fp);

    return ret;
}

HRESULT agaliaDecoderWIC::decode(IWICBitmap** ppBitmap, IWICColorContext** ppColorContext, const agaliaContainer* image)
{
    if (!ppBitmap || !ppColorContext || !image) return E_POINTER;

    CComPtr<IStream> stream;
    auto hr = image->getAsocStream(&stream);
    if (FAILED(hr)) return hr;

	CComPtr<IStream> stream2;
	stream->Clone(&stream2);

    return LoadImageWIC(ppBitmap, ppColorContext, stream2);
}

HRESULT agaliaDecoderWIC::decodeThumbnail(agaliaBitmap** ppBitmap, const agaliaContainer* image, uint32_t maxW, uint32_t maxH)
{
    if (!image || !ppBitmap) return E_POINTER;

    CComPtr<IStream> stream;
    auto hr = image->getAsocStream(&stream);
    if (FAILED(hr)) return hr;

    stream->Seek({ 0 }, STREAM_SEEK_SET, nullptr);

    return LoadThumbnailWIC(ppBitmap, stream, maxW, maxH); 
}
