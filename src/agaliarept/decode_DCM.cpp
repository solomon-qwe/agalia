#include "pch.h"

#include "decode_DCM.h"
#include "analyze_DCM_util.h"

#include "thumbnail.h"

using namespace analyze_DCM;

inline void remove_trailing_space(char* str)
{
	if (str && *str) {
		size_t last = strlen(str) - 1;
		if (str[last] == ' ') str[last] = '\0';
	}
}

enum Photometric_Interpretation
{
	Unknown,
	MONOCHROME1,
	MONOCHROME2,
	PALETTE_COLOR,
	RGB,
	HSV,
	ARGB,
	CMYK,
	YBR_FULL,
	YBR_FULL_422,
	YBR_PARTIAL_422,
	YBR_PARTIAL_420,
	YBR_ICT,
	YBR_RCT
};

static HRESULT getPhotometricInterpretation(const container_DCM_Impl* image, Photometric_Interpretation* photometric_interpretation)
{
	if (image == nullptr) return E_POINTER;
	if (photometric_interpretation == nullptr) return E_POINTER;

	CHeapPtr<char> str;
	auto hr = getTagValue(image, 0x0028, 0x0004, str, nullptr, true);
	if (FAILED(hr)) return hr;

	remove_trailing_space(str);

	if (strcmp(str, "MONOCHROME1") == 0)
		*photometric_interpretation = MONOCHROME1;
	else if (strcmp(str, "MONOCHROME2") == 0)
		*photometric_interpretation = MONOCHROME2;
	else if (strcmp(str, "PALETTE COLOR") == 0)
		*photometric_interpretation = PALETTE_COLOR;
	else if (strcmp(str, "RGB") == 0)
		*photometric_interpretation = RGB;
	else if (strcmp(str, "HSV") == 0)
		*photometric_interpretation = HSV;
	else if (strcmp(str, "ARGB") == 0)
		*photometric_interpretation = ARGB;
	else if (strcmp(str, "CMYK") == 0)
		*photometric_interpretation = CMYK;
	else if (strcmp(str, "YBR_FULL") == 0)
		*photometric_interpretation = YBR_FULL;
	else if (strcmp(str, "YBR_FULL_422") == 0)
		*photometric_interpretation = YBR_FULL_422;
	else if (strcmp(str, "YBR_PARTIAL_422") == 0)
		*photometric_interpretation = YBR_PARTIAL_422;
	else if (strcmp(str, "YBR_PARTIAL_420") == 0)
		*photometric_interpretation = YBR_PARTIAL_420;
	else if (strcmp(str, "YBR_ICT") == 0)
		*photometric_interpretation = YBR_ICT;
	else if (strcmp(str, "YBR_RCT") == 0)
		*photometric_interpretation = YBR_RCT;
	else
		*photometric_interpretation = Unknown;

	return S_OK;
}


static HRESULT getValueUS(const container_DCM_Impl* container, uint16_t group, uint16_t element, uint16_t* result)
{
	if (container == nullptr) return E_POINTER;
	if (result == nullptr) return E_POINTER;

	uint64_t tag_offset = 0;
	auto hr = findTag(container, group, element, &tag_offset);
	if (FAILED(hr)) return E_FAIL;

	dicom_element elem(container, tag_offset);

	CHeapPtr<uint16_t> buf;
	hr = loadValue(elem, buf);
	if (FAILED(hr)) return hr;

	*result = buf[0];
	return S_OK;
}


static HRESULT getValueDS(const container_DCM_Impl* container, uint16_t group, uint16_t element, double* result)
{
	if (container == nullptr) return E_POINTER;
	if (result == nullptr) return E_POINTER;

	uint64_t tag_offset = 0;
	auto hr = findTag(container, group, element, &tag_offset);
	if (FAILED(hr)) return E_FAIL;

	dicom_element elem(container, tag_offset);

	CHeapPtr<char> buf;
	hr = loadValue(elem, buf, nullptr, true);
	if (FAILED(hr)) return hr;

	*result = strtod(buf, nullptr);
	return S_OK;
}


template <typename T>
HRESULT getTagValueT(const container_DCM_Impl* container, uint16_t group, uint16_t element, T* p)
{
	return E_NOTIMPL;
}

template <>
HRESULT getTagValueT<uint16_t>(const container_DCM_Impl* container, uint16_t group, uint16_t element, uint16_t* p)
{
	return getValueUS(container, group, element, p);
}

template <>
HRESULT getTagValueT<double>(const container_DCM_Impl* container, uint16_t group, uint16_t element, double* p)
{
	return getValueDS(container, group, element, p);
}

template<typename T, uint16_t group, uint16_t element>
struct tag_value
{
	T value;
	enum { unloaded, valid, invalid } status;

	tag_value() : value(0), status(unloaded) {}
	tag_value(const container_DCM_Impl* container) : tag_value() { init(container); }
	~tag_value() {}

	bool init(const container_DCM_Impl* container)
	{
		if (status == unloaded)
		{
			status = SUCCEEDED(getTagValueT<T>(container, group, element, &value)) ? valid : invalid;
		}
		return (status == valid);
	}

	T get(const container_DCM_Impl* container)
	{
		if (status == unloaded) init(container);
		if (status != valid) throw E_FAIL;
		return value;
	}
};


class DCMImageDecoder
{
public:
	DCMImageDecoder(const container_DCM_Impl* image)
		: image(image)
	{
	}

	virtual ~DCMImageDecoder()
	{
	}

	HRESULT decode(HBITMAP* phBitmap);

protected:
	HRESULT decodeGrayscale8(HBITMAP* phBitmap);
	HRESULT decodeGrayscale16(HBITMAP* phBitmap);
	HRESULT decodeRGB24(HBITMAP* phBitmap);
	HRESULT decodeRLE8(HBITMAP* phBitmap);

	uint16_t getSamplesPerPixel(void) { return v_samples_per_pixel.get(image); }
	uint16_t getBitsAllocated(void) { return v_bits_allocated.get(image); }
	uint16_t getRows(void) { return v_rows.get(image); }
	uint16_t getColumns(void) { return v_columns.get(image); }
	uint16_t getBitsStored(void) { return v_bits_stored.get(image); }
	uint16_t getHighBit(void) { return v_high_bit.get(image); }
	uint16_t getPixelRepresentation(void) { return v_pixel_representation.get(image); }

protected:
	const container_DCM_Impl* image = nullptr;

	tag_value<uint16_t, 0x0028, 0x0002> v_samples_per_pixel;
	tag_value<uint16_t, 0x0028, 0x0100> v_bits_allocated;
	tag_value<uint16_t, 0x0028, 0x0010> v_rows;
	tag_value<uint16_t, 0x0028, 0x0011> v_columns;
	tag_value<uint16_t, 0x0028, 0x0101> v_bits_stored;
	tag_value<uint16_t, 0x0028, 0x0102> v_high_bit;
	tag_value<uint16_t, 0x0028, 0x0103> v_pixel_representation;
};


HRESULT DCMImageDecoder::decode(HBITMAP* phBitmap)
{
	try
	{
		CHeapPtr<char> transfer_syntax_uid;
		auto hr = getTagValue(image, 0x0002, 0x0010, transfer_syntax_uid, nullptr, true);
		if (FAILED(hr)) return E_FAIL;
		remove_trailing_space(transfer_syntax_uid);

		uint16_t samples_per_pixel = getSamplesPerPixel();
		uint16_t bits_allocated = getBitsAllocated();

		if (strcmp(transfer_syntax_uid, "1.2.840.10008.1.2.5") == 0)
		{
			if (bits_allocated == 8)
			{
				return decodeRLE8(phBitmap);
			}
		}
		else
		{
			if (samples_per_pixel == 1 && bits_allocated == 8)
			{
				return decodeGrayscale8(phBitmap);
			}
			else if (samples_per_pixel == 1 && bits_allocated == 16)
			{
				return decodeGrayscale16(phBitmap);
			}
			else if (samples_per_pixel == 3 && bits_allocated == 8)
			{
				return decodeRGB24(phBitmap);
			}
		}
	}
	catch (const HRESULT& e)
	{
		return e;
	}

	return E_FAIL;
}


HRESULT DCMImageDecoder::decodeGrayscale8(HBITMAP* phBitmap)
{
	uint16_t rows = getRows();
	uint16_t columns = getColumns();
	tag_value<double, 0x0028, 0x1050> window_center(image);
	tag_value<double, 0x0028, 0x1051> window_width(image);
	tag_value<double, 0x0028, 0x1052> rescale_intercept(image);
	tag_value<double, 0x0028, 0x1053> rescale_slope(image);

	CHeapPtr<BITMAPINFO> bmi;
	size_t headerSize = sizeof(BITMAPINFOHEADER);
	bmi.AllocateBytes(headerSize);
	memset(bmi.m_pData, 0, headerSize);

	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi->bmiHeader.biWidth = columns;
	bmi->bmiHeader.biHeight = rows;
	bmi->bmiHeader.biPlanes = 1;
	bmi->bmiHeader.biBitCount = 32;
	bmi->bmiHeader.biCompression = BI_RGB;

	CHeapPtr<uint8_t> buf;
	size_t bufSize = 0;
	auto hr = getTagValue(image, 0x7FE0, 0x0010, buf, &bufSize);
	if (FAILED(hr)) return hr;

	void* pBits = nullptr;
	HDC hdc = GetDC(NULL);
	HBITMAP hBitmap = CreateDIBSection(hdc, bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
	ReleaseDC(NULL, hdc);
	if (hBitmap == NULL) return E_FAIL;

	bool invert_colors = false;
	Photometric_Interpretation photometric_interpretation = Unknown;
	hr = getPhotometricInterpretation(image, &photometric_interpretation);
	if (FAILED(hr)) return hr;
	switch (photometric_interpretation)
	{
	case MONOCHROME1:
		invert_colors = true;
		break;
	case MONOCHROME2:
		invert_colors = false;
		break;
	default:
		return E_FAIL;
	}

	if (rescale_intercept.status == rescale_intercept.invalid) rescale_intercept.value = 0;
	if (rescale_slope.status == rescale_slope.invalid) rescale_slope.value = 1;

	int32_t minValue = INT_MAX, maxValue = INT_MIN;
	for (size_t i = 0; i < bufSize / 2; i++)
	{
		uint8_t value = reinterpret_cast<uint8_t*>(buf.m_pData)[i];
		if (value < minValue) minValue = value;
		if (value > maxValue) maxValue = value;
	}

	const bool isEnableWindow = (window_center.status == window_center.valid) && (window_width.status == window_width.valid);
	const uint16_t unsignedMax = 0xFF;
	const double lower_limit = isEnableWindow ? (window_center.value - (window_width.value / 2)) : minValue;
	const double upper_limit = isEnableWindow ? (window_center.value + (window_width.value / 2)) : maxValue;

	size_t stride = 4 * columns;
	uint8_t* src = reinterpret_cast<uint8_t*>(buf.m_pData);
	uint8_t* dst = reinterpret_cast<uint8_t*>(pBits) + stride * (rows - 1);
	for (size_t y = 0; y < rows; y++)
	{
		for (size_t x = 0, k = 0; x < columns; x += 1, k += 4)
		{
			double value = src[x] * rescale_slope.value + rescale_intercept.value;
			value = max(min(value, upper_limit), lower_limit);
			value = unsignedMax * (value - lower_limit) / (upper_limit - lower_limit);
			uint8_t v = invert_colors ? (unsignedMax - static_cast<uint8_t>(value)) : static_cast<uint8_t>(value);
			dst[k + 0] = dst[k + 1] = dst[k + 2] = v;
		}
		src += columns;
		dst -= stride;
	}

	*phBitmap = hBitmap;
	return S_OK;
}


HRESULT DCMImageDecoder::decodeGrayscale16(HBITMAP* phBitmap)
{
	uint16_t rows = getRows();
	uint16_t columns = getColumns();
	uint16_t pixel_representation = getPixelRepresentation();
	tag_value<double, 0x0028, 0x1050> window_center(image);
	tag_value<double, 0x0028, 0x1051> window_width(image);
	tag_value<double, 0x0028, 0x1052> rescale_intercept(image);
	tag_value<double, 0x0028, 0x1053> rescale_slope(image);

	bool invert_colors = false;
	Photometric_Interpretation photometric_interpretation = Unknown;
	auto hr = getPhotometricInterpretation(image, &photometric_interpretation);
	if (FAILED(hr)) return hr;
	switch (photometric_interpretation)
	{
	case MONOCHROME1:
		invert_colors = true;
		break;
	case MONOCHROME2:
		invert_colors = false;
		break;
	default:
		return E_FAIL;
	}

	CHeapPtr<BITMAPINFO> bmi;
	size_t bufSize = sizeof(BITMAPV5HEADER);
	bmi.AllocateBytes(bufSize);
	memset(bmi.m_pData, 0, bufSize);

	bmi->bmiHeader.biSize = sizeof(BITMAPV5HEADER);
	bmi->bmiHeader.biWidth = columns;
	bmi->bmiHeader.biHeight = rows;
	bmi->bmiHeader.biPlanes = 1;
	bmi->bmiHeader.biBitCount = 32;
	bmi->bmiHeader.biCompression = BI_BITFIELDS;
	reinterpret_cast<BITMAPV5HEADER*>(bmi.m_pData)->bV5RedMask = 0x3FF << 20;
	reinterpret_cast<BITMAPV5HEADER*>(bmi.m_pData)->bV5GreenMask = 0x3FF << 10;
	reinterpret_cast<BITMAPV5HEADER*>(bmi.m_pData)->bV5BlueMask = 0x3FF << 0;
	reinterpret_cast<BITMAPV5HEADER*>(bmi.m_pData)->bV5AlphaMask = 0;

	void* pBits = nullptr;
	HDC hdc = GetDC(NULL);
	HBITMAP hBitmap = CreateDIBSection(hdc, bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
	ReleaseDC(NULL, hdc);
	if (hBitmap == NULL) return E_FAIL;

	CHeapPtr<uint8_t> buf;
	size_t size = 0;
	hr = getTagValue(image, 0x7FE0, 0x0010, buf, &size);
	if (FAILED(hr)) return hr;

	if (rescale_intercept.status == rescale_intercept.invalid) rescale_intercept.value = 0;
	if (rescale_slope.status == rescale_slope.invalid) rescale_slope.value = 1;
	const bool isEnableWindow = (window_center.status == window_center.valid) && (window_width.status == window_width.valid);
	const uint16_t unsignedMax = 0x3FF;

	if (pixel_representation == 1)
	{
		int32_t minValue = INT_MAX, maxValue = INT_MIN;
		for (size_t i = 0; i < size / 2; i++)
		{
			int16_t value = reinterpret_cast<int16_t*>(buf.m_pData)[i];
			if (value < minValue) minValue = value;
			if (value > maxValue) maxValue = value;
		}

		const double lower_limit = isEnableWindow ? (window_center.value - (window_width.value / 2)) : minValue;
		const double upper_limit = isEnableWindow ? (window_center.value + (window_width.value / 2)) : maxValue;

		int16_t* p = reinterpret_cast<int16_t*>(buf.m_pData);
		uint32_t* q = reinterpret_cast<uint32_t*>(pBits) + columns * (rows - 1);
		for (size_t y = 0; y < rows; y++)
		{
			for (size_t x = 0; x < columns; x++)
			{
				double value = p[x] * rescale_slope.value + rescale_intercept.value;
				value = max(min(value, upper_limit), lower_limit);
				value = unsignedMax * (value - lower_limit) / (upper_limit - lower_limit);
				uint16_t t = invert_colors ? (unsignedMax - static_cast<uint16_t>(value)) : static_cast<uint16_t>(value);
				q[x] = (t << 20) | (t << 10) | t;
			}
			p += columns;
			q -= columns;
		}
	}
	else
	{
		uint32_t minValue = UINT_MAX, maxValue = 0;
		for (size_t i = 0; i < size / 2; i++)
		{
			uint16_t value = reinterpret_cast<uint16_t*>(buf.m_pData)[i];
			if (value < minValue) minValue = value;
			if (value > maxValue) maxValue = value;
		}

		const double lower_limit = isEnableWindow ? max(0, (window_center.value - (window_width.value / 2))) : minValue;
		const double upper_limit = isEnableWindow ? (window_center.value + (window_width.value / 2)) : maxValue;

		uint16_t* p = reinterpret_cast<uint16_t*>(buf.m_pData);
		uint32_t* q = reinterpret_cast<uint32_t*>(pBits) + columns * (rows - 1);
		for (size_t y = 0; y < rows; y++)
		{
			for (size_t x = 0; x < columns; x++)
			{
				double value = p[x] * rescale_slope.value + rescale_intercept.value;
				value = max(min(value, upper_limit), lower_limit);
				value = unsignedMax * (value - lower_limit) / (upper_limit - lower_limit);
				uint16_t t = invert_colors ? (unsignedMax - static_cast<uint16_t>(value)) : static_cast<uint16_t>(value);
				q[x] = (t << 20) | (t << 10) | t;
			}
			p += columns;
			q -= columns;
		}
	}

	*phBitmap = hBitmap;
	return S_OK;
}


inline void ybrfull_to_rgb(uint8_t y, uint8_t cb, uint8_t cr, uint8_t* r, uint8_t* g, uint8_t* b)
{
	const double k1 = 255.0 / 219.0;
	const double k2 = 255.0 / 224.0;
	const int m[3][3] = {
		{ static_cast<int>(65536 * k1), 0,                                                     static_cast<int>(65536 * k2 * 1.402)},
		{ static_cast<int>(65536 * k1), static_cast<int>(65536 * -k2 * 1.772 * 0.114 / 0.587), static_cast<int>(65536 * -k2 * 1.402 * 0.299 / 0.587)},
		{ static_cast<int>(65536 * k1), static_cast<int>(65536 * k2 * 1.772),                 0}
	};

	const int y0 = y - 16;
	const int cb0 = cb - 128;
	const int cr0 = cr - 128;

	const int r0 = (m[0][0] * y0 + m[0][2] * cr0 + 32768) >> 16;
	const int g0 = (m[1][0] * y0 + m[1][1] * cb0 + m[1][2] * cr0 + 32768) >> 16;
	const int b0 = (m[2][0] * y0 + m[2][1] * cb0 + 32768) >> 16;

	*r = static_cast<uint8_t>(max(min(r0, 255), 0));
	*g = static_cast<uint8_t>(max(min(g0, 255), 0));
	*b = static_cast<uint8_t>(max(min(b0, 255), 0));
}


HRESULT DCMImageDecoder::decodeRGB24(HBITMAP* phBitmap)
{
	uint16_t rows = getRows();
	uint16_t columns = getColumns();
	tag_value<uint16_t, 0x0028, 0x0006> planar_configuration(image);
	tag_value<double, 0x0028, 0x1050> window_center(image);
	tag_value<double, 0x0028, 0x1051> window_width(image);
	tag_value<double, 0x0028, 0x1052> rescale_intercept(image);
	tag_value<double, 0x0028, 0x1053> rescale_slope(image);

	Photometric_Interpretation photometric_interpretation = Unknown;
	auto hr = getPhotometricInterpretation(image, &photometric_interpretation);
	if (FAILED(hr)) return hr;

	if (photometric_interpretation != RGB)
		return E_FAIL;

	CHeapPtr<BITMAPINFO> bmi;
	size_t bufSize = sizeof(BITMAPINFOHEADER);
	bmi.AllocateBytes(bufSize);
	memset(bmi.m_pData, 0, bufSize);

	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi->bmiHeader.biWidth = columns;
	bmi->bmiHeader.biHeight = rows;
	bmi->bmiHeader.biPlanes = 1;
	bmi->bmiHeader.biBitCount = 32;
	bmi->bmiHeader.biCompression = BI_RGB;

	void* pBits = nullptr;
	HDC hdc = ::GetDC(NULL);
	HBITMAP hBitmap = ::CreateDIBSection(hdc, bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
	::ReleaseDC(NULL, hdc);
	if (hBitmap == NULL) return E_FAIL;

	CHeapPtr<uint8_t> buf;
	size_t size = 0;
	hr = getTagValue(image, 0x7FE0, 0x0010, buf, &size);
	if (FAILED(hr)) return hr;
	if (size < 3U * columns * rows) return E_FAIL;

	const size_t stride = 4 * columns;

	bool isColorByPlane = (planar_configuration.status == planar_configuration.valid) && (planar_configuration.value == 1);
	if (isColorByPlane)
	{
		uint8_t* p1 = reinterpret_cast<uint8_t*>(buf.m_pData);
		uint8_t* p2 = p1 + columns * rows;
		uint8_t* p3 = p2 + columns * rows;
		uint8_t* q = reinterpret_cast<uint8_t*>(pBits) + stride * (rows - 1);
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < columns; x++)
			{
				q[x * 4 + 0] = p1[x];
				q[x * 4 + 1] = p2[x];
				q[x * 4 + 2] = p3[x];
			}
			p1 += columns;
			p2 += columns;
			p3 += columns;
			q -= stride;
		}
	}
	else
	{
		uint8_t* p = reinterpret_cast<uint8_t*>(buf.m_pData);
		uint8_t* q = reinterpret_cast<uint8_t*>(pBits) + stride * (rows - 1);
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < columns; x++)
			{
				q[x * 4 + 0] = p[x * 3 + 2];
				q[x * 4 + 1] = p[x * 3 + 1];
				q[x * 4 + 2] = p[x * 3 + 0];
			}
			p += 3 * columns;
			q -= stride;
		}
	}

	*phBitmap = hBitmap;
	return S_OK;
}


static HRESULT decodeRLESegment(const uint8_t* src, uint64_t srcSize, std::vector<uint8_t>& dest, size_t expectedSize)
{
	size_t srcPos = 0;
	while (srcPos < srcSize && dest.size() < expectedSize)
	{
		int8_t n = static_cast<int8_t>(src[srcPos++]);
		if (n >= 0)
		{
			int count = n + 1;
			if (srcPos + count > srcSize)
				return E_FAIL;
			dest.insert(dest.end(), &src[srcPos], &src[srcPos + count]);
			srcPos += count;
		}
		else if (n >= -127)
		{
			int count = -n + 1;
			if (srcPos >= srcSize)
				return E_FAIL;
			uint8_t value = src[srcPos++];
			dest.insert(dest.end(), count, value);
		}
		else {
		}
	}

	if (dest.size() != expectedSize)
		return E_FAIL;

	return S_OK;
}


static HRESULT decodeDICOM_8bitRLE(const uint8_t* srcData, uint64_t srcSize, uint16_t columns, uint16_t rows, uint16_t samplesPerPixel, std::vector<uint8_t>& decodedData)
{
	if (srcSize < 64)
		return E_FAIL;

	const uint32_t* header = reinterpret_cast<const uint32_t*>(srcData);
	uint32_t numberOfSegments = header[0];

	if (numberOfSegments == 0 || numberOfSegments > 15)
		return E_FAIL;

	std::vector<uint32_t> segmentOffsets(numberOfSegments);
	memcpy(segmentOffsets.data(), header + 1, numberOfSegments * sizeof(uint32_t));

	decodedData.resize(columns * rows * samplesPerPixel);

	for (uint32_t segment = 0; segment < numberOfSegments; ++segment)
	{
		uint32_t segmentOffset = segmentOffsets[segment];
		if (segmentOffset >= srcSize)
			return E_FAIL;

		const uint8_t* segmentData = srcData + segmentOffset;
		uint64_t segmentSize = (segment + 1 < numberOfSegments ? segmentOffsets[segment + 1] : srcSize) - segmentOffset;

		std::vector<uint8_t> planeData;
		HRESULT hr = decodeRLESegment(segmentData, segmentSize, planeData, columns * rows);
		if (FAILED(hr)) return hr;

		for (size_t i = 0; i < planeData.size(); ++i)
		{
			decodedData[i * samplesPerPixel + segment] = planeData[i];
		}
	}

	return S_OK;
}


static HRESULT getPallet(const container_DCM_Impl* container, uint16_t group, uint16_t element, CHeapPtr<uint16_t>& pallet)
{
	uint64_t tag_offset = 0;
	auto hr = findTag(container, group, element, &tag_offset);
	if (FAILED(hr)) return hr;
	dicom_element elem(container, tag_offset);
	return loadValue(elem, pallet);
}


HRESULT DCMImageDecoder::decodeRLE8(HBITMAP* phBitmap)
{
	uint16_t samples_per_pixel = 0;
	auto hr = getValueUS(image, 0x0028, 0x0002, &samples_per_pixel);
	if (FAILED(hr)) return E_FAIL;

	uint16_t rows = 0;
	hr = getValueUS(image, 0x0028, 0x0010, &rows);
	if (FAILED(hr)) return E_FAIL;

	uint16_t columns = 0;
	hr = getValueUS(image, 0x0028, 0x0011, &columns);
	if (FAILED(hr)) return E_FAIL;

	uint16_t bits_allocated = 0;
	hr = getValueUS(image, 0x0028, 0x0100, &bits_allocated);
	if (FAILED(hr)) return E_FAIL;

	Photometric_Interpretation photometric_interpretation = Unknown;
	hr = getPhotometricInterpretation(image, &photometric_interpretation);
	if (FAILED(hr)) return E_FAIL;

	if (bits_allocated != 8)
		return E_FAIL;

	CHeapPtr<uint8_t> buf;
	size_t bufSize = 0;
	{
		agaliaPtr<agaliaElement> item;
		hr = findTag(image, 0x7FE0, 0x0010, &item);
		if (FAILED(hr)) return E_FAIL;

		agaliaPtr<agaliaElement> next;
		hr = item->getChild(0, &next);
		if (FAILED(hr)) return E_FAIL;

		item = next.detach();
		hr = item->getNext(&next);
		if (FAILED(hr)) return E_FAIL;

		dicom_element elem(image, next->getOffset());
		hr = loadValue(elem, buf, &bufSize);
		if (FAILED(hr)) return hr;
	}

	// RLEデータをデコードして生のピクセルデータを取得
	std::vector<uint8_t> decodedData;
	hr = decodeDICOM_8bitRLE(buf.m_pData, bufSize, columns, rows, samples_per_pixel, decodedData);
	if (FAILED(hr)) return hr;

	// ビットマップを作成
	CHeapPtr<BITMAPINFO> bmi;
	size_t headerSize = sizeof(BITMAPINFO::bmiHeader);
	bmi.AllocateBytes(headerSize);
	memset(bmi.m_pData, 0, headerSize);
	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi->bmiHeader.biWidth = columns;
	bmi->bmiHeader.biHeight = rows;
	bmi->bmiHeader.biPlanes = 1;

	if (samples_per_pixel == 1)
	{
		bmi->bmiHeader.biBitCount = 32;
		bmi->bmiHeader.biCompression = BI_RGB;

		void* pBits = nullptr;
		HDC hdc = ::GetDC(NULL);
		HBITMAP hBitmap = ::CreateDIBSection(hdc, bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
		::ReleaseDC(NULL, hdc);
		if (hBitmap == NULL) return E_FAIL;

		const int stride = 4 * columns;
		uint8_t* src = decodedData.data();
		uint8_t* dst = reinterpret_cast<uint8_t*>(pBits) + stride * (rows - 1);

		if (photometric_interpretation == PALETTE_COLOR)
		{
			CHeapPtr<uint16_t> r_pallet;
			CHeapPtr<uint16_t> g_pallet;
			CHeapPtr<uint16_t> b_pallet;
			hr = getPallet(image, 0x0028, 0x1201, r_pallet);
			if (FAILED(hr)) return hr;
			hr = getPallet(image, 0x0028, 0x1202, g_pallet);
			if (FAILED(hr)) return hr;
			hr = getPallet(image, 0x0028, 0x1203, b_pallet);
			if (FAILED(hr)) return hr;

			for (int y = 0; y < rows; y++)
			{
				for (int x = 0; x < columns; x++)
				{
					dst[x * 4 + 0] = b_pallet[src[x]] >> 8;
					dst[x * 4 + 1] = g_pallet[src[x]] >> 8;
					dst[x * 4 + 2] = r_pallet[src[x]] >> 8;
				}
				src += columns;
				dst -= stride;
			}
		}
		else if (photometric_interpretation == MONOCHROME2)
		{
			for (int y = 0; y < rows; y++)
			{
				for (int x = 0; x < columns; x++)
				{
					dst[x * 4 + 0] = dst[x * 4 + 1] = dst[x * 4 + 2] = src[x];
				}
				src += columns;
				dst -= stride;
			}
		}

		*phBitmap = hBitmap;
		return S_OK;
	}
	else if (samples_per_pixel == 3)
	{
		bmi->bmiHeader.biBitCount = 24;
		bmi->bmiHeader.biCompression = BI_RGB;

		void* pBits = nullptr;
		HDC hdc = ::GetDC(NULL);
		HBITMAP hBitmap = ::CreateDIBSection(hdc, bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
		::ReleaseDC(NULL, hdc);
		if (hBitmap == NULL) return E_FAIL;

		const int stride = (3 * columns + 3) & ~3;
		uint8_t* src = decodedData.data();
		uint8_t* dst = reinterpret_cast<uint8_t*>(pBits) + stride * (rows - 1);

		if (photometric_interpretation == YBR_FULL)
		{
			for (int y = 0; y < rows; y++)
			{
				for (int x = 0; x < columns * 3; x += 3)
				{
					ybrfull_to_rgb(
						src[x + 0],
						src[x + 1],
						src[x + 2],
						&dst[x + 2],
						&dst[x + 1],
						&dst[x + 0]);
				}
				src += 3 * columns;
				dst -= stride;
			}
		}
		else if (photometric_interpretation == RGB)
		{
			for (int y = 0; y < rows; y++)
			{
				for (int x = 0; x < columns * 3; x += 3)
				{
					dst[x + 0] = src[x + 2];
					dst[x + 1] = src[x + 1];
					dst[x + 2] = src[x + 0];
				}
				src += 3 * columns;
				dst -= stride;
			}
		}

		*phBitmap = hBitmap;
		return S_OK;
	}

	return E_FAIL;
}

static HRESULT decodeJPEGBaseline(const container_DCM_Impl* image, const wchar_t* path, HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH)
{
	agaliaPtr<agaliaElement> item;
	auto hr = findTag(image, 0x7FE0, 0x0010, &item);
	if (FAILED(hr)) return E_FAIL;

	agaliaPtr<agaliaElement> child;
	hr = item->getChild(0, &child);
	if (FAILED(hr)) return E_FAIL;

	agaliaPtr<agaliaElement> next;
	hr = child->getNext(&next);
	if (FAILED(hr)) return E_FAIL;

	dicom_element elem(image, next->getOffset());

	uint64_t value_offset = 0;
	hr = elem.getValueOffset(&value_offset);
	if (FAILED(hr)) return E_FAIL;
	value_offset += next->getOffset();

	uint32_t value_size = 0;
	hr = elem.getValueLength(&value_size);
	if (FAILED(hr)) return E_FAIL;

	CComPtr<IStream> stream;
	hr = getAgaliaStream(&stream, path, value_offset, value_size);
	if (FAILED(hr)) return hr;

	return loadThumbnailBitmap(phBitmap, maxW, maxH, stream);
}


HRESULT analyze_DCM::loadDCMImage(const container_DCM_Impl* image, const wchar_t* path, uint32_t maxW, uint32_t maxH, HBITMAP* phBitmap)
{
	CHeapPtr<char> transfer_syntax_uid;
	auto hr = getTagValue(image, 0x0002, 0x0010, transfer_syntax_uid, nullptr, true);
	if (FAILED(hr)) return E_FAIL;

	remove_trailing_space(transfer_syntax_uid);

	if (strcmp(transfer_syntax_uid, "1.2.840.10008.1.2.4.50") == 0)	// JPEG Baseline (Process 1)
	{
		return decodeJPEGBaseline(image, path, phBitmap, maxW, maxH);
	}
	else if (strcmp(transfer_syntax_uid, "1.2.840.10008.1.2") == 0 ||	// Implicit VR Little Endian
		strcmp(transfer_syntax_uid, "1.2.840.10008.1.2.1") == 0 ||		// Explicit VR Little Endian 
		strcmp(transfer_syntax_uid, "1.2.840.10008.1.2.5") == 0)		// RLE Lossless
	{
		DCMImageDecoder decoder(image);
		return decoder.decode(phBitmap);
	}

	return E_FAIL;
}
