#include "pch.h"
#include "agaliaDecoderOpenJPEG.h"

#include <atlbase.h>
#include <wincodec.h>

#include "openjpeg/inc/openjpeg.h"

#if defined(_M_IX86)
#pragma comment(lib, "openjpeg/lib/x86/openjp2.lib")
#elif defined(_M_X64)
#pragma comment(lib, "openjpeg/lib/x64/openjp2.lib")
#endif


static void error_handler(const char* msg, void* client_data)
{
    UNREFERENCED_PARAMETER(msg);
    UNREFERENCED_PARAMETER(client_data);
}


static void warning_handler(const char* msg, void* client_data)
{
    UNREFERENCED_PARAMETER(msg);
    UNREFERENCED_PARAMETER(client_data);
}


static void info_handler(const char* msg, void* client_data)
{
    UNREFERENCED_PARAMETER(msg);
    UNREFERENCED_PARAMETER(client_data);
}


static OPJ_SIZE_T istream_read(void* p_buffer, OPJ_SIZE_T nb_bytes, void* p_user_data)
{
    IStream* pStream = static_cast<IStream*>(p_user_data);

    ULONG bytesRead = 0;
    HRESULT hr = pStream->Read(p_buffer, static_cast<ULONG>(nb_bytes), &bytesRead);
    if (FAILED(hr) || bytesRead == 0) {
        return (OPJ_SIZE_T)-1;
    }
    return static_cast<OPJ_SIZE_T>(bytesRead);
}


static OPJ_OFF_T istream_skip(OPJ_OFF_T nb_bytes, void* p_user_data)
{
    IStream* pStream = static_cast<IStream*>(p_user_data);

    LARGE_INTEGER offset;
    offset.QuadPart = nb_bytes;
    ULARGE_INTEGER newPos = {};

    HRESULT hr = pStream->Seek(offset, STREAM_SEEK_CUR, &newPos);
    if (FAILED(hr)) {
        return (OPJ_OFF_T)-1;
    }
    return nb_bytes;
}

static OPJ_BOOL istream_seek(OPJ_OFF_T nb_bytes, void* p_user_data)
{
    IStream* pStream = static_cast<IStream*>(p_user_data);

    LARGE_INTEGER offset;
    offset.QuadPart = nb_bytes;

    HRESULT hr = pStream->Seek(offset, STREAM_SEEK_SET, nullptr);
    return SUCCEEDED(hr) ? OPJ_TRUE : OPJ_FALSE;
}

static void istream_destroy(void* p_user_data)
{
    IStream* pStream = static_cast<IStream*>(p_user_data);
    if (pStream) {
        pStream->Release();
    }
}

static HRESULT decodeJPEG2000stub(IStream* pStream, opj_image_t** ppImage, OPJ_CODEC_FORMAT format)
{
	if (!pStream || !ppImage)
		return E_POINTER;

    ULARGE_INTEGER uliSize = {};
	{
		LARGE_INTEGER zero = {};
		HRESULT hr = pStream->Seek(zero, STREAM_SEEK_END, &uliSize);
		if (FAILED(hr)) return hr;
		hr = pStream->Seek(zero, STREAM_SEEK_SET, nullptr);
		if (FAILED(hr)) return hr;
	}
	OPJ_SIZE_T streamSize = static_cast<OPJ_SIZE_T>(uliSize.QuadPart);

	opj_dparameters_t parameters = {};
	opj_set_default_decoder_parameters(&parameters);

	opj_codec_t* codec = opj_create_decompress(format);
	if (!codec) {
		return E_FAIL;
	}

	if (!opj_setup_decoder(codec, &parameters)) {
		opj_destroy_codec(codec);
		return E_FAIL;
	}

	opj_set_error_handler(codec, error_handler, nullptr);
	opj_set_warning_handler(codec, warning_handler, nullptr);
	opj_set_info_handler(codec, info_handler, nullptr);

	opj_stream_t* opjStream = opj_stream_default_create(OPJ_TRUE);
	if (!opjStream) {
		opj_destroy_codec(codec);
		return E_FAIL;
	}

	pStream->AddRef();
	opj_stream_set_user_data(opjStream, pStream, istream_destroy);
	opj_stream_set_user_data_length(opjStream, streamSize);
	opj_stream_set_read_function(opjStream, istream_read);
	opj_stream_set_seek_function(opjStream, istream_seek);
	opj_stream_set_skip_function(opjStream, istream_skip);

	opj_image_t* image = nullptr;
	if (!opj_read_header(opjStream, codec, &image)) {
		opj_destroy_codec(codec);
		opj_stream_destroy(opjStream);
		return E_FAIL;
	}

	if (!opj_decode(codec, opjStream, image)) {
		opj_image_destroy(image);
		opj_destroy_codec(codec);
		opj_stream_destroy(opjStream);
		return E_FAIL;
	}

	opj_destroy_codec(codec);
	opj_stream_destroy(opjStream);
	*ppImage = image;

	return S_OK;
}


auto convert_sample = [](opj_image_comp_t* comp, int sample_val, uint16_t limit) -> uint16_t
    {
        if (comp->sgnd) {
            sample_val += (1 << (comp->prec - 1));
        }
        int max_val = (1 << comp->prec) - 1;
        sample_val = max(min(sample_val, max_val), 0);
        return static_cast<uint16_t>((sample_val * limit) / max_val);
    };

template<class T>
class decodeProcess
{
};

template<>
class decodeProcess<agaliaBitmap>
{
protected:
    void* getScan0(void* pBits, uint32_t stride, uint32_t height)
	{
		return static_cast<uint8_t*>(pBits) + stride * (height - 1);
	}

	int32_t getScanOffset(uint32_t width) const
	{
		return -static_cast<int32_t>(width);
	}

    uint32_t getBpc() const
    {
        return 8;
    }

	uint32_t getRIndex() const
	{
		return 2;
	}

	uint32_t getGIndex() const
	{
		return 1;
	}

	uint32_t getBIndex() const
	{
		return 0;
	}

    uint32_t dummy = 0;
};

template<>
class decodeProcess<IWICBitmap>
{
protected:
    void* getScan0(void* pBits, uint32_t stride, uint32_t height)
    {
		UNREFERENCED_PARAMETER(stride);
		UNREFERENCED_PARAMETER(height);
        return static_cast<uint8_t*>(pBits);
    }

    int32_t getScanOffset(uint32_t width) const
    {
        return width;
    }

    uint32_t getBpc() const
    {
        return 16;
    }

	uint32_t getRIndex() const
	{
		return 0;
	}

	uint32_t getGIndex() const
	{
		return 1;
	}

	uint32_t getBIndex() const
	{
		return 2;
	}

    uint64_t dummy = 0;
};


template<class T>
class opj_conv_ycc : public decodeProcess<T>
{
public:
    void conv(const opj_image_t* image, uint8_t* pDest)
    {
        opj_image_comp_t* compY = &image->comps[0];
        opj_image_comp_t* compCb = &image->comps[1];
        opj_image_comp_t* compCr = &image->comps[2];

        uint32_t width = compY->w;
        uint32_t height = compY->h;
        uint32_t stride = width * sizeof(decodeProcess<T>::dummy);

        using type = decltype(decodeProcess<T>::dummy);
		type mask = (1 << decodeProcess<T>::getBpc()) - 1;
        type alpha = ~static_cast<type>(0) ^ ((static_cast<type>(1) << (decodeProcess<T>::getBpc() * 3)) - 1);

        float x_ratio_cb = static_cast<float>(compCb->w) / compY->w;
        float y_ratio_cb = static_cast<float>(compCb->h) / compY->h;
        float x_ratio_cr = static_cast<float>(compCr->w) / compY->w;
        float y_ratio_cr = static_cast<float>(compCr->h) / compY->h;

        const int k = 1 << (decodeProcess<T>::getBpc() - 1);

        type* line = static_cast<type*>(decodeProcess<T>::getScan0(pDest, stride, height));
        for (size_t y = 0; y < height; y++)
        {
            size_t idxY = y * compY->w;

            for (size_t x = 0; x < width; x++)
            {
                size_t posY = idxY + x;

                size_t xCb = static_cast<size_t>(x * x_ratio_cb);
                size_t yCb = static_cast<size_t>(y * y_ratio_cb);
                size_t posCb = yCb * compCb->w + xCb;

                size_t xCr = static_cast<size_t>(x * x_ratio_cr);
                size_t yCr = static_cast<size_t>(y * y_ratio_cr);
                size_t posCr = yCr * compCr->w + xCr;

                auto Y = convert_sample(compY, compY->data[posY], (uint16_t)mask);
                auto Cb = convert_sample(compCb, compCb->data[posCb], (uint16_t)mask);
                auto Cr = convert_sample(compCr, compCr->data[posCr], (uint16_t)mask);

                int r = static_cast<int>(Y + 1.402 * (Cr - k));
                int g = static_cast<int>(Y - 0.344136 * (Cb - k) - 0.714136 * (Cr - k));
                int b = static_cast<int>(Y + 1.772 * (Cb - k));

				line[x] = alpha |
					(static_cast<type>(min(max(r, 0), (int)mask)) << (decodeProcess<T>::getBpc() * decodeProcess<T>::getRIndex())) |
					(static_cast<type>(min(max(g, 0), (int)mask)) << (decodeProcess<T>::getBpc() * decodeProcess<T>::getGIndex())) |
					(static_cast<type>(min(max(b, 0), (int)mask)) << (decodeProcess<T>::getBpc() * decodeProcess<T>::getBIndex()));
            }
			line += decodeProcess<T>::getScanOffset(width);
        }
    }
};

template<class T>
class opj_conv_rgb : public decodeProcess<T>
{
public:
    void conv(const opj_image_t* image, uint8_t* pDest)
    {
        uint32_t width = image->comps[0].w;
        uint32_t height = image->comps[0].h;

        opj_image_comp_t* comp0 = &image->comps[0];
        opj_image_comp_t* comp1 = &image->comps[1];
        opj_image_comp_t* comp2 = &image->comps[2];

        uint32_t stride = width * sizeof(decodeProcess<T>::dummy);

        using type = decltype(decodeProcess<T>::dummy);
        type mask = (1 << decodeProcess<T>::getBpc()) - 1;
        type alpha = ~static_cast<type>(0) ^ ((static_cast<type>(1) << (decodeProcess<T>::getBpc() * 3)) - 1);

        type* line = static_cast<type*>(decodeProcess<T>::getScan0(pDest, stride, height));
        size_t pos = 0;
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++, pos++)
            {
				line[x] = alpha |
					((type)convert_sample(comp0, comp0->data[pos], (uint16_t)mask) << (decodeProcess<T>::getBpc() * decodeProcess<T>::getRIndex())) |
					((type)convert_sample(comp1, comp1->data[pos], (uint16_t)mask) << (decodeProcess<T>::getBpc() * decodeProcess<T>::getGIndex())) |
					((type)convert_sample(comp2, comp2->data[pos], (uint16_t)mask) << (decodeProcess<T>::getBpc() * decodeProcess<T>::getBIndex()));
            }
			line += decodeProcess<T>::getScanOffset(width);
        }
    }
};

template<class T>
class opj_conv_gray : public decodeProcess<T>
{
public:
    void conv(const opj_image_t* image, uint8_t* pDest)
    {
        uint32_t width = image->comps[0].w;
        uint32_t height = image->comps[0].h;

        opj_image_comp_t* comp = &image->comps[0];

        uint32_t stride = width * sizeof(decodeProcess<T>::dummy);

		using type = decltype(decodeProcess<T>::dummy);
        type mask = (1 << decodeProcess<T>::getBpc()) - 1;
        type alpha = ~static_cast<type>(0) ^ ((static_cast<type>(1) << (decodeProcess<T>::getBpc() * 3)) - 1);

        type* line = static_cast<type*>(decodeProcess<T>::getScan0(pDest, stride, height));
        size_t pos = 0;
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++, pos++)
            {
                auto gray = convert_sample(comp, comp->data[pos], (uint16_t)mask);
				line[x] = alpha |
					((type)gray << (decodeProcess<T>::getBpc() * decodeProcess<T>::getRIndex())) |
					((type)gray << (decodeProcess<T>::getBpc() * decodeProcess<T>::getGIndex())) |
					((type)gray << (decodeProcess<T>::getBpc() * decodeProcess<T>::getBIndex()));
            }
			line += decodeProcess<T>::getScanOffset(width);
        }
    }
};


static HRESULT opjimage_to_HBITMAP(const opj_image_t* image, agaliaBitmap** ppBitmap)
{
    if (!image || !ppBitmap)
        return E_POINTER;

    int width = image->comps[0].w;
    int height = image->comps[0].h;

	BITMAPV5HEADER bi = {};
	bi.bV5Size = sizeof(BITMAPV5HEADER);
	bi.bV5Width = width;
	bi.bV5Height = height;
	bi.bV5Planes = 1;
	bi.bV5BitCount = 32;
	bi.bV5Compression = BI_BITFIELDS;
	bi.bV5RedMask = 0x00FF0000;
	bi.bV5GreenMask = 0x0000FF00;
	bi.bV5BlueMask = 0x000000FF;
	bi.bV5AlphaMask = 0x00000000;

	agaliaPtr<agaliaBitmap> bitmap;
	auto hr = createAgaliaBitmap(&bitmap, &bi);
	if (FAILED(hr))
		return hr;

	void* pBits = nullptr;
	hr = bitmap->getBits(&pBits);
	if (FAILED(hr))
		return hr;

    if (image->numcomps >= 3)
    {
        if (image->color_space == OPJ_CLRSPC_SYCC)
        {
			opj_conv_ycc<agaliaBitmap> conv;
			conv.conv(image, static_cast<uint8_t*>(pBits));
		}
		else
		{
            opj_conv_rgb<agaliaBitmap> conv;
            conv.conv(image, static_cast<uint8_t*>(pBits));
        }
    }
    else if (image->numcomps == 1)
    {
        opj_conv_gray<agaliaBitmap> conv;
        conv.conv(image, static_cast<uint8_t*>(pBits));
    }
    else
    {
        return E_FAIL;
    }

	*ppBitmap = bitmap.detach();
    return S_OK;
}

static HRESULT opjimage_to_IWICBitmap(const opj_image_t* image, IWICBitmap** ppBitmap)
{
    if (!image || !ppBitmap)
        return E_POINTER;

	HRESULT hr = S_OK;

    int width = image->comps[0].w;
    int height = image->comps[0].h;
    UINT stride = width * 8;

	CHeapPtr<uint8_t> buf;
	buf.Allocate(stride * height);
	if (!buf)
		return E_OUTOFMEMORY;

    if (image->numcomps >= 3)
    {
        if (image->color_space == OPJ_CLRSPC_SYCC)
        {
            opj_conv_ycc<IWICBitmap> conv;
            conv.conv(image, buf);
        }
        else
        {
            opj_conv_rgb<IWICBitmap> conv;
            conv.conv(image, buf);

        }
    }
    else if (image->numcomps == 1)
    {
        opj_conv_gray<IWICBitmap> conv;
        conv.conv(image, buf);
    }
    else
    {
        return E_FAIL;
    }

    CComPtr<IWICImagingFactory> wicFactory;
    hr = ::CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
    if (FAILED(hr)) return hr;

    hr = wicFactory->CreateBitmapFromMemory(
        width, height,
        GUID_WICPixelFormat64bppRGBA,
        stride, stride * height,
        buf,
        ppBitmap);
    if (FAILED(hr)) return hr;

    return S_OK;
}


static HRESULT decodeJPEG2000(agaliaBitmap** ppBitmap, const agaliaContainer* image, OPJ_CODEC_FORMAT format)
{
    if (!image || !ppBitmap)
        return E_POINTER;

	CComPtr<IStream> stream;
	auto hr = image->getAsocStream(&stream);
	if (FAILED(hr)) return hr;

    opj_image_t* opj_image = nullptr;
    hr = decodeJPEG2000stub(stream, &opj_image, format);
    if (SUCCEEDED(hr))
    {
        hr = opjimage_to_HBITMAP(opj_image, ppBitmap);
        opj_image_destroy(opj_image);
    }
    return hr;
}


static HRESULT decodeJPEG2000(IWICBitmap** ppBitmap, const agaliaContainer* image, OPJ_CODEC_FORMAT format)
{
    if (!image || !ppBitmap)
        return E_POINTER;

    CComPtr<IStream> stream;
    auto hr = image->getAsocStream(&stream);
    if (FAILED(hr)) return hr;

    opj_image_t* opj_image = nullptr;
    hr = decodeJPEG2000stub(stream, &opj_image, format);
    if (SUCCEEDED(hr))
    {
        hr = opjimage_to_IWICBitmap(opj_image, ppBitmap);
        opj_image_destroy(opj_image);
    }
    return hr;
}


agaliaDecoderOpenJPEG::agaliaDecoderOpenJPEG()
{
}


agaliaDecoderOpenJPEG::~agaliaDecoderOpenJPEG()
{
}


ULONG agaliaDecoderOpenJPEG::AddRef(void)
{
	return ++refCount;
}


ULONG agaliaDecoderOpenJPEG::Release(void)
{
	if (--refCount == 0)
	{
		delete this;
		return 0;
	}
	return refCount;
}


HRESULT agaliaDecoderOpenJPEG::decode(agaliaBitmap** ppBitmap, const agaliaContainer* image)
{
    auto hr = decodeJPEG2000(ppBitmap, image, OPJ_CODEC_JP2);
    if (SUCCEEDED(hr))
        return hr;
    return decodeJPEG2000(ppBitmap, image, OPJ_CODEC_J2K);
}

HRESULT agaliaDecoderOpenJPEG::decode(IWICBitmap** ppBitmap, IWICColorContext** ppColorContext, const agaliaContainer* image)
{
    auto hr = decodeJPEG2000(ppBitmap, image, OPJ_CODEC_JP2);
    if (SUCCEEDED(hr))
        return hr;
    hr = decodeJPEG2000(ppBitmap, image, OPJ_CODEC_J2K);
	if (SUCCEEDED(hr))
		*ppColorContext = nullptr;
	return hr;
}

HRESULT agaliaDecoderOpenJPEG::decodeThumbnail(agaliaBitmap** ppBitmap, const agaliaContainer* image, uint32_t maxW, uint32_t maxH)
{
    UNREFERENCED_PARAMETER(maxW);
	UNREFERENCED_PARAMETER(maxH);
	return decode(ppBitmap, image);
}


extern "C"
__declspec(dllexport) agaliaDecoder* GetAgaliaDecoder(void)
{
    return new agaliaDecoderOpenJPEG();
}
