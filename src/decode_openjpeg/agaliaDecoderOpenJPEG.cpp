#include "pch.h"
#include "agaliaDecoderOpenJPEG.h"

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

auto convert_sample = [](opj_image_comp_t* comp, int sample_val) -> uint8_t
    {
        if (comp->sgnd) {
            sample_val += (1 << (comp->prec - 1));
        }
        int max_val = (1 << comp->prec) - 1;
        sample_val = max(min(sample_val, max_val), 0);
        return static_cast<uint8_t>((sample_val * 255) / max_val);
    };

static void ycc_to_rgb32(const opj_image_t* image, uint8_t* pDest)
{
    opj_image_comp_t* compY = &image->comps[0];
    opj_image_comp_t* compCb = &image->comps[1];
    opj_image_comp_t* compCr = &image->comps[2];

    size_t width = compY->w;
    size_t height = compY->h;
    size_t stride = width * 4;

    float x_ratio_cb = static_cast<float>(compCb->w) / compY->w;
    float y_ratio_cb = static_cast<float>(compCb->h) / compY->h;
    float x_ratio_cr = static_cast<float>(compCr->w) / compY->w;
    float y_ratio_cr = static_cast<float>(compCr->h) / compY->h;

    uint8_t* line = pDest + stride * (height - 1);

    for (size_t y = 0; y < height; y++)
    {
        size_t idxY = y * compY->w;
        size_t idxLine = 0;

        for (size_t x = 0; x < width; x++)
        {
            size_t posY = idxY + x;

            size_t xCb = static_cast<size_t>(x * x_ratio_cb);
            size_t yCb = static_cast<size_t>(y * y_ratio_cb);
            size_t posCb = yCb * compCb->w + xCb;

            size_t xCr = static_cast<size_t>(x * x_ratio_cr);
            size_t yCr = static_cast<size_t>(y * y_ratio_cr);
            size_t posCr = yCr * compCr->w + xCr;

            uint8_t Y = convert_sample(compY, compY->data[posY]);
            uint8_t Cb = convert_sample(compCb, compCb->data[posCb]);
            uint8_t Cr = convert_sample(compCr, compCr->data[posCr]);

            int r = static_cast<int>(Y + 1.402 * (Cr - 128));
            int g = static_cast<int>(Y - 0.344136 * (Cb - 128) - 0.714136 * (Cr - 128));
            int b = static_cast<int>(Y + 1.772 * (Cb - 128));

            line[idxLine + 0] = static_cast<uint8_t>(min(max(b, 0), 255));
            line[idxLine + 1] = static_cast<uint8_t>(min(max(g, 0), 255));
            line[idxLine + 2] = static_cast<uint8_t>(min(max(r, 0), 255));
            line[idxLine + 3] = 0xFF;

            idxLine += 4;
        }
        line -= stride;
    }
}


static void rgb_to_rgb32(const opj_image_t* image, uint8_t* pDest)
{
    size_t width = image->comps[0].w;
    size_t height = image->comps[0].h;

	opj_image_comp_t* comp0 = &image->comps[0];
	opj_image_comp_t* comp1 = &image->comps[1];
	opj_image_comp_t* comp2 = &image->comps[2];

    size_t pos = 0;
    size_t stride = width * 4;

    uint8_t* line = pDest + stride * (height - 1);
    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < stride; x += 4, pos++)
        {
			line[x + 0] = convert_sample(comp2, comp2->data[pos]);
			line[x + 1] = convert_sample(comp1, comp1->data[pos]);
			line[x + 2] = convert_sample(comp0, comp0->data[pos]);
			line[x + 3] = 0xFF;
        }
        line -= stride;
    }
}

static void gray_to_rgb32(const opj_image_t* image, uint8_t* pDest)
{
    size_t width = image->comps[0].w;
    size_t height = image->comps[0].h;

	opj_image_comp_t* comp = &image->comps[0];

	size_t pos = 0;
	size_t stride = width * 4;

	uint8_t* line = pDest + stride * (height - 1);
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < stride; x += 4, pos++)
		{
			uint8_t gray = convert_sample(comp, comp->data[pos]);
			line[x + 0] = gray;
			line[x + 1] = gray;
			line[x + 2] = gray;
			line[x + 3] = 0xFF;
		}
		line -= stride;
	}
}

static HRESULT opjimage_to_HBITMAP(const opj_image_t* image, HBITMAP* phBitmap)
{
    if (!image || !phBitmap)
        return E_POINTER;

    int width = image->comps[0].w;
    int height = image->comps[0].h;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    if (!hBitmap) {
        return E_FAIL;
    }

    if (image->numcomps >= 3)
    {
        if (image->color_space == OPJ_CLRSPC_SYCC)
        {
			ycc_to_rgb32(image, static_cast<uint8_t*>(pBits));
		}
		else
		{
			rgb_to_rgb32(image, static_cast<uint8_t*>(pBits));
        }
    }
    else if (image->numcomps == 1)
    {
		gray_to_rgb32(image, static_cast<uint8_t*>(pBits));
    }
    else
    {
        DeleteObject(hBitmap);
        return E_FAIL;
    }

    *phBitmap = hBitmap;
    return S_OK;
}


static HRESULT decodeJPEG2000(IStream* pStream, HBITMAP* phBitmap, OPJ_CODEC_FORMAT format)
{
    if (!pStream || !phBitmap)
        return E_POINTER;

    opj_image_t* image = nullptr;
    auto hr = decodeJPEG2000stub(pStream, &image, format);
    if (SUCCEEDED(hr))
    {
        hr = opjimage_to_HBITMAP(image, phBitmap);
        opj_image_destroy(image);
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


HRESULT agaliaDecoderOpenJPEG::decode(IStream* stream, HBITMAP* phBitmap)
{
    auto hr = decodeJPEG2000(stream, phBitmap, OPJ_CODEC_JP2);
    if (SUCCEEDED(hr))
        return hr;
	return decodeJPEG2000(stream, phBitmap, OPJ_CODEC_J2K);
}


HRESULT agaliaDecoderOpenJPEG::decode(IStream* stream, agaliaHeap** bmpInfo, void** ppBits)
{
    UNREFERENCED_PARAMETER(stream);
    UNREFERENCED_PARAMETER(bmpInfo);
	UNREFERENCED_PARAMETER(ppBits);
	return E_NOTIMPL;
}


HRESULT agaliaDecoderOpenJPEG::decode(IStream* stream, uint32_t maxW, uint32_t maxH, HBITMAP* phBitmap)
{
	UNREFERENCED_PARAMETER(maxW);
	UNREFERENCED_PARAMETER(maxH);
    return decode(stream, phBitmap);
}


HRESULT agaliaDecoderOpenJPEG::decode(IStream* stream, uint32_t maxW, uint32_t maxH, agaliaHeap** bmpInfo, void** ppBits)
{
    UNREFERENCED_PARAMETER(stream);
	UNREFERENCED_PARAMETER(maxW);
	UNREFERENCED_PARAMETER(maxH);
	UNREFERENCED_PARAMETER(bmpInfo);
	UNREFERENCED_PARAMETER(ppBits);
    return E_NOTIMPL;
}


extern "C"
__declspec(dllexport) agaliaDecoder* GetAgaliaDecoder(void)
{
    return new agaliaDecoderOpenJPEG();
}
