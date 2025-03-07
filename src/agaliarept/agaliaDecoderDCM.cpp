#include "pch.h"
#include "agaliaDecoderDCM.h"
#include "container_DCM_Impl.h"
#include "analyze_DCM_util.h"
#include "thumbnail.h"

using namespace analyze_DCM;

#include <memory>
#include <wincodec.h>

namespace analyze_DCM
{

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

	struct param
	{
		std::unique_ptr<std::string> transfer_syntax_uid;
		std::unique_ptr<uint16_t> samples_per_pixel;
		std::unique_ptr<uint16_t> bits_allocated;
		std::unique_ptr<uint16_t> rows;
		std::unique_ptr<uint16_t> columns;
		std::unique_ptr<uint16_t> bits_stored;
		std::unique_ptr<uint16_t> high_bit;
		std::unique_ptr<uint16_t> pixel_representation;
		std::unique_ptr<uint16_t> planar_configuration;
		std::unique_ptr<double> window_center;
		std::unique_ptr<double> window_width;
		std::unique_ptr<double> rescale_intercept;
		std::unique_ptr<double> rescale_slope;
		std::unique_ptr<Photometric_Interpretation> photometric_interpretation;
	};

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

	static HRESULT getDecodeParam(const container_DCM_Impl* image, param& param)
	{
		{
			CHeapPtr<char> transfer_syntax_uid;
			auto hr = getTagValue(image, 0x0002, 0x0010, transfer_syntax_uid, nullptr, true);
			if (FAILED(hr)) return E_FAIL;
			remove_trailing_space(transfer_syntax_uid);
			param.transfer_syntax_uid = std::make_unique<std::string>(transfer_syntax_uid.m_pData);
		}
		{
			uint16_t samples_per_pixel = 0;
			auto hr = getTagValueT<uint16_t>(image, 0x0028, 0x0002, &samples_per_pixel);
			if (SUCCEEDED(hr)) {
				param.samples_per_pixel = std::make_unique<uint16_t>(samples_per_pixel);
			}
		}
		{
			uint16_t bits_allocated = 0;
			auto hr = getTagValueT<uint16_t>(image, 0x0028, 0x0100, &bits_allocated);
			if (SUCCEEDED(hr)) {
				param.bits_allocated = std::make_unique<uint16_t>(bits_allocated);
			}
		}
		{
			uint16_t rows = 0;
			auto hr = getTagValueT<uint16_t>(image, 0x0028, 0x0010, &rows);
			if (SUCCEEDED(hr)) {
				param.rows = std::make_unique<uint16_t>(rows);
			}
		}
		{
			uint16_t columns = 0;
			auto hr = getTagValueT<uint16_t>(image, 0x0028, 0x0011, &columns);
			if (SUCCEEDED(hr)) {
				param.columns = std::make_unique<uint16_t>(columns);
			}
		}
		{
			uint16_t bits_stored = 0;
			auto hr = getTagValueT<uint16_t>(image, 0x0028, 0x0101, &bits_stored);
			if (SUCCEEDED(hr)) {
				param.bits_stored = std::make_unique<uint16_t>(bits_stored);
			}
		}
		{
			uint16_t high_bit = 0;
			auto hr = getTagValueT<uint16_t>(image, 0x0028, 0x0102, &high_bit);
			if (SUCCEEDED(hr)) {
				param.high_bit = std::make_unique<uint16_t>(high_bit);
			}
		}
		{
			uint16_t pixel_representation = 0;
			auto hr = getTagValueT<uint16_t>(image, 0x0028, 0x0103, &pixel_representation);
			if (SUCCEEDED(hr)) {
				param.pixel_representation = std::make_unique<uint16_t>(pixel_representation);
			}
		}
		{
			uint16_t planar_configuration = 0;
			auto hr = getTagValueT<uint16_t>(image, 0x0028, 0x0006, &planar_configuration);
			if (SUCCEEDED(hr)) {
				param.planar_configuration = std::make_unique<uint16_t>(planar_configuration);
			}
		}
		{
			double window_center = 0;
			auto hr = getTagValueT<double>(image, 0x0028, 0x1050, &window_center);
			if (SUCCEEDED(hr)) {
				param.window_center = std::make_unique<double>(window_center);
			}
		}
		{
			double window_width = 0;
			auto hr = getTagValueT<double>(image, 0x0028, 0x1051, &window_width);
			if (SUCCEEDED(hr)) {
				param.window_width = std::make_unique<double>(window_width);
			}
		}
		{
			double rescale_intercept = 0;
			auto hr = getTagValueT<double>(image, 0x0028, 0x1052, &rescale_intercept);
			if (SUCCEEDED(hr)) {
				param.rescale_intercept = std::make_unique<double>(rescale_intercept);
			}
		}
		{
			double rescale_slope = 0;
			auto hr = getTagValueT<double>(image, 0x0028, 0x1053, &rescale_slope);
			if (SUCCEEDED(hr)) {
				param.rescale_slope = std::make_unique<double>(rescale_slope);
			}
		}
		{
			Photometric_Interpretation photometric_interpretation = Unknown;
			auto hr = getPhotometricInterpretation(image, &photometric_interpretation);
			if (SUCCEEDED(hr)) {
				param.photometric_interpretation = std::make_unique<Photometric_Interpretation>(photometric_interpretation);
			}
		}
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

	static HRESULT getPallet(const container_DCM_Impl* container, uint16_t group, uint16_t element, CHeapPtr<uint16_t>& pallet)
	{
		uint64_t tag_offset = 0;
		auto hr = findTag(container, group, element, &tag_offset);
		if (FAILED(hr)) return hr;
		dicom_element elem(container, tag_offset);
		return loadValue(elem, pallet);
	}

	static HRESULT decodeRLE8(const uint8_t* srcData, uint64_t srcSize, size_t expectedPixels, uint16_t samples_per_pixel, std::vector<uint8_t>& decodedData)
	{
		if (!srcData || srcSize < 4)
			return E_INVALIDARG;

		const uint32_t* header = reinterpret_cast<const uint32_t*>(srcData);
		uint32_t numSegments = header[0];

		uint32_t requiredSegments = samples_per_pixel * sizeof(uint8_t);
		if (numSegments < requiredSegments)
			return E_FAIL;
		if (srcSize < 4 + numSegments * 4)
			return E_FAIL;

		decodedData.clear();
		decodedData.resize(expectedPixels * samples_per_pixel);

		for (uint16_t s = 0; s < samples_per_pixel; s++)
		{
			uint32_t segIndex = s * sizeof(uint8_t);
			uint32_t segStart = header[segIndex + 1];
			uint32_t segEnd = (segIndex + 1 < numSegments) ? header[segIndex + 2] : static_cast<uint32_t>(srcSize);

			std::vector<uint8_t> segData;
			segData.reserve(expectedPixels);
			size_t outCount = 0;
			uint64_t pos = segStart;
			while (pos < segEnd && outCount < expectedPixels)
			{
				if (pos >= srcSize)
					return E_FAIL;

				int8_t n = static_cast<int8_t>(srcData[pos++]);
				if (n >= 0)
				{
					int count = n + 1;
					if (pos + count > segEnd)
						return E_FAIL;
					for (int i = 0; i < count && outCount < expectedPixels; i++)
					{
						segData.push_back(srcData[pos++]);
						outCount++;
					}
				}
				else if (n >= -127)
				{
					int repeatCount = -n + 1;
					if (pos >= segEnd)
						return E_FAIL;
					uint8_t val = srcData[pos++];
					for (int i = 0; i < repeatCount && outCount < expectedPixels; i++)
					{
						segData.push_back(val);
						outCount++;
					}
				}
			}

			if (outCount != expectedPixels)
				return E_FAIL;

			for (size_t i = 0; i < expectedPixels; i++)
			{
				size_t index = i * samples_per_pixel + s;
				decodedData[index] = segData[i];
			}
		}

		return S_OK;
	}

	static HRESULT decodeRLE16(const uint8_t* srcData, uint64_t srcSize, size_t expectedPixels, uint16_t samples_per_pixel, std::vector<uint16_t>& decodedData)
	{
		if (!srcData || srcSize < 4)
			return E_INVALIDARG;

		const uint32_t* header = reinterpret_cast<const uint32_t*>(srcData);
		uint32_t numSegments = header[0];

		uint32_t requiredSegments = samples_per_pixel * sizeof(uint16_t);
		if (numSegments < requiredSegments)
			return E_FAIL;
		if (srcSize < 4 + numSegments * 4)
			return E_FAIL;

		decodedData.clear();
		decodedData.resize(expectedPixels * samples_per_pixel, 0);

		for (uint16_t s = 0; s < samples_per_pixel; s++)
		{
			for (int byteIdx = 0; byteIdx < 2; byteIdx++)
			{
				uint32_t segIndex = s * sizeof(uint16_t) + byteIdx;
				uint32_t segStart = header[segIndex + 1];
				uint32_t segEnd = (segIndex + 1 < numSegments) ? header[segIndex + 2] : static_cast<uint32_t>(srcSize);

				std::vector<uint8_t> segDecoded;
				segDecoded.reserve(expectedPixels);
				size_t outCount = 0;
				uint64_t pos = segStart;
				while (pos < segEnd && outCount < expectedPixels)
				{
					if (pos >= srcSize)
						return E_FAIL;

					int8_t n = static_cast<int8_t>(srcData[pos++]);
					if (n >= 0)
					{
						int count = n + 1;
						if (pos + count > segEnd)
							return E_FAIL;
						for (int i = 0; i < count && outCount < expectedPixels; i++)
						{
							segDecoded.push_back(srcData[pos++]);
							outCount++;
						}
					}
					else if (n >= -127)
					{
						int repeatCount = -n + 1;
						if (pos >= segEnd)
							return E_FAIL;
						uint8_t val = srcData[pos++];
						for (int i = 0; i < repeatCount && outCount < expectedPixels; i++)
						{
							segDecoded.push_back(val);
							outCount++;
						}
					}
				}
				if (outCount != expectedPixels)
					return E_FAIL;

				for (size_t i = 0; i < expectedPixels; i++)
				{
					size_t index = i * samples_per_pixel + s;
					if (byteIdx == 0)
						decodedData[index] |= (static_cast<uint16_t>(segDecoded[i]) << 8);
					else
						decodedData[index] |= segDecoded[i];
				}
			}
		}

		return S_OK;
	}


//////////////////////////////////////////////

	template<class T>
	class decoderBase
	{
	public:
		virtual HRESULT decode(const container_DCM_Impl* image, const param& param) = 0;
		virtual HRESULT getResult(T** ppBitmap) = 0;
	};

	template<class T>
	class decoderProcess
	{
	};

	template<>
	class decoderProcess<agaliaBitmap>
	{
	public:
		HRESULT createBuffer(uint16_t columns, uint16_t rows, void** ppBits)
		{
			if (ppBits == nullptr) return E_POINTER;

			bmi.bV5Size = sizeof(BITMAPV5HEADER);
			bmi.bV5Width = columns;
			bmi.bV5Height = rows;
			bmi.bV5Planes = 1;
			bmi.bV5BitCount = 32;
			bmi.bV5Compression = BI_BITFIELDS;
			bmi.bV5RedMask = (0xFFu << 16);
			bmi.bV5GreenMask = (0xFFu << 8);
			bmi.bV5BlueMask = (0xFFu << 0);
			bmi.bV5AlphaMask = 0;

			auto hr = createAgaliaBitmap(&bitmap, &bmi);
			if (FAILED(hr)) return hr;
			return bitmap->getBits(ppBits);
		}

		HRESULT postProcess(void)
		{
			return S_OK;
		}

		HRESULT detachInnerObject(agaliaBitmap** ppBitmap)
		{
			if (!bitmap) return E_FAIL;
			*ppBitmap = bitmap.detach();
			return S_OK;
		}

		void* getScan0(void)
		{
			void* pBits = nullptr;
			bitmap->getBits(&pBits);
			return (PBYTE)pBits + 4 * bmi.bV5Width * (bmi.bV5Height - 1);
		}

		int32_t getScanOffset(void) { return -bmi.bV5Width; }
		uint32_t getRIndex(void) { return 2; }
		uint32_t getGIndex(void) { return 1; }
		uint32_t getBIndex(void) { return 0; }

	protected:
		agaliaPtr<agaliaBitmap> bitmap;
		BITMAPV5HEADER bmi = {};
		const uint32_t bpc = 8;
		uint32_t dummy = 0;
	};

	template<>
	class decoderProcess<IWICBitmap>
	{
	public:
		HRESULT createBuffer(uint16_t columns, uint16_t rows, void** ppBits)
		{
			if (!buffer.AllocateBytes(8 * columns * rows))
				return E_OUTOFMEMORY;
			*ppBits = buffer.m_pData;
			width = columns;
			height = rows;
			return S_OK;
		}

		HRESULT postProcess(void)
		{
			CComPtr<IWICImagingFactory> wicFactory;
			auto hr = ::CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
			if (FAILED(hr)) return hr;

			UINT stride = 8 * width;
			hr = wicFactory->CreateBitmapFromMemory(
				width, height,
				GUID_WICPixelFormat64bppRGBA,
				stride, stride * height,
				buffer,
				&bitmap);
			return hr;
		}

		HRESULT detachInnerObject(IWICBitmap** ppBitmap)
		{
			if (!bitmap) return E_FAIL;
			*ppBitmap = bitmap.Detach();
			return S_OK;
		}

		void* getScan0(void)
		{
			return buffer.m_pData;
		}

		int32_t getScanOffset(void) { return width; }
		uint32_t getRIndex(void) { return 0; }
		uint32_t getGIndex(void) { return 1; }
		uint32_t getBIndex(void) { return 2; }

	protected:
		CComPtr<IWICBitmap> bitmap;
		CHeapPtr<uint8_t> buffer;
		uint32_t width = 0;
		uint32_t height = 0;
		const uint32_t bpc = 16;
		uint64_t dummy = 0;
	};

	template<class T>
	class decoderGrayscale8 : public decoderBase<T>, public decoderProcess<T>
	{
	public:
		HRESULT decode(const container_DCM_Impl* image, const param& param) override
		{
			const uint32_t dstColorBits = decoderProcess<T>::bpc;
			const uint32_t dstMask = (1 << dstColorBits) - 1;

			CHeapPtr<uint8_t> bufSrc;
			size_t bufSrcSize = 0;
			auto hr = getTagValue(image, 0x7FE0, 0x0010, bufSrc, &bufSrcSize);
			if (FAILED(hr)) return hr;

			if (!param.rows) return E_FAIL;
			uint16_t rows = *param.rows;

			if (!param.columns) return E_FAIL;
			uint16_t columns = *param.columns;

			bool invert_colors = false;
			if (!param.photometric_interpretation) return E_FAIL;
			switch (*param.photometric_interpretation)
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

			double rescale_intercept = param.rescale_intercept ? *param.rescale_intercept : 0;
			double rescale_slope = param.rescale_slope ? *param.rescale_slope : 1;

			double lower_limit = 0;
			double upper_limit = 0;
			if (param.window_center && param.window_width)
			{
				lower_limit = *param.window_center - (*param.window_width / 2);
				upper_limit = *param.window_center + (*param.window_width / 2);
			}
			else
			{
				int32_t minValue = INT_MAX, maxValue = INT_MIN;
				for (size_t i = 0; i < bufSrcSize; i++)
				{
					uint8_t value = reinterpret_cast<uint8_t*>(bufSrc.m_pData)[i];
					if (value < minValue) minValue = value;
					if (value > maxValue) maxValue = value;
				}
				lower_limit = minValue;
				upper_limit = maxValue;
			}

			void* pBits = nullptr;
			hr = decoderProcess<T>::createBuffer(columns, rows, &pBits);
			if (FAILED(hr)) return hr;

			using type = decltype(decoderProcess<T>::dummy);
			type alpha = ~static_cast<type>(0) ^ ((static_cast<type>(1) << (dstColorBits * 3)) - 1);

			uint8_t* src = reinterpret_cast<uint8_t*>(bufSrc.m_pData);
			type* dst = reinterpret_cast<type*>(decoderProcess<T>::getScan0());
			for (size_t y = 0; y < rows; y++)
			{
				for (size_t x = 0; x < columns; x += 1)
				{
					double value = src[x] * rescale_slope + rescale_intercept;
					value = max(min(value, upper_limit), lower_limit);
					value = dstMask * (value - lower_limit) / (upper_limit - lower_limit);
					type v = invert_colors ? (dstMask - static_cast<type>(value)) : static_cast<type>(value);
					dst[x] = alpha |
						(v << (dstColorBits * decoderProcess<T>::getRIndex())) |
						(v << (dstColorBits * decoderProcess<T>::getGIndex())) |
						(v << (dstColorBits * decoderProcess<T>::getBIndex()));
				}
				src += columns;
				dst += decoderProcess<T>::getScanOffset();
			}

			return decoderProcess<T>::postProcess();
		}

		virtual HRESULT getResult(T** ppBitmap) override
		{
			return decoderProcess<T>::detachInnerObject(ppBitmap);
		}
	};

	template<class T>
	class decoderGrayscale16_unsigned : public decoderBase<T>, public decoderProcess<T>
	{
	public:
		HRESULT decode(const container_DCM_Impl* image, const param& param) override
		{
			const uint32_t dstColorBits = decoderProcess<T>::bpc;
			const uint32_t dstMask = (1 << dstColorBits) - 1;

			CHeapPtr<uint8_t> bufSrc;
			size_t bufSrcSize = 0;
			auto hr = getTagValue(image, 0x7FE0, 0x0010, bufSrc, &bufSrcSize);
			if (FAILED(hr)) return hr;

			if (!param.rows) return E_FAIL;
			uint16_t rows = *param.rows;

			if (!param.columns) return E_FAIL;
			uint16_t columns = *param.columns;

			bool invert_colors = false;
			if (!param.photometric_interpretation) return E_FAIL;
			switch (*param.photometric_interpretation)
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

			double rescale_intercept = param.rescale_intercept ? *param.rescale_intercept : 0;
			double rescale_slope = param.rescale_slope ? *param.rescale_slope : 1;

			double lower_limit = 0;
			double upper_limit = 0;
			if (param.window_center && param.window_width)
			{
				lower_limit = *param.window_center - (*param.window_width / 2);
				upper_limit = *param.window_center + (*param.window_width / 2);
			}
			else
			{
				int32_t minValue = INT_MAX, maxValue = INT_MIN;
				for (size_t i = 0; i < bufSrcSize / 2; i++)
				{
					uint16_t value = reinterpret_cast<uint16_t*>(bufSrc.m_pData)[i];
					if (value < minValue) minValue = value;
					if (value > maxValue) maxValue = value;
				}
				lower_limit = minValue;
				upper_limit = maxValue;
			}

			void* pBits = nullptr;
			hr = decoderProcess<T>::createBuffer(columns, rows, &pBits);
			if (FAILED(hr)) return hr;

			using type = decltype(decoderProcess<T>::dummy);
			type alpha = ~static_cast<type>(0) ^ ((static_cast<type>(1) << (dstColorBits * 3)) - 1);

			uint16_t* src = reinterpret_cast<uint16_t*>(bufSrc.m_pData);
			type* dst = reinterpret_cast<type*>(decoderProcess<T>::getScan0());
			for (size_t y = 0; y < rows; y++)
			{
				for (size_t x = 0; x < columns; x += 1)
				{
					double value = src[x] * rescale_slope + rescale_intercept;
					value = max(min(value, upper_limit), lower_limit);
					value = dstMask * (value - lower_limit) / (upper_limit - lower_limit);
					type v = invert_colors ? (dstMask - static_cast<type>(value)) : static_cast<type>(value);
					dst[x] = alpha |
						(v << (dstColorBits * decoderProcess<T>::getRIndex())) |
						(v << (dstColorBits * decoderProcess<T>::getGIndex())) |
						(v << (dstColorBits * decoderProcess<T>::getBIndex()));
				}
				src += columns;
				dst += decoderProcess<T>::getScanOffset();
			}

			return decoderProcess<T>::postProcess();
		}

		virtual HRESULT getResult(T** ppBitmap) override
		{
			return decoderProcess<T>::detachInnerObject(ppBitmap);
		}
	};

	template<class T>
	class decoderGrayscale16_signed : public decoderBase<T>, public decoderProcess<T>
	{
	public:
		HRESULT decode(const container_DCM_Impl* image, const param& param) override
		{
			const uint32_t dstColorBits = decoderProcess<T>::bpc;
			const uint32_t dstMask = (1 << dstColorBits) - 1;

			CHeapPtr<uint8_t> bufSrc;
			size_t bufSrcSize = 0;
			auto hr = getTagValue(image, 0x7FE0, 0x0010, bufSrc, &bufSrcSize);
			if (FAILED(hr)) return hr;

			if (!param.rows) return E_FAIL;
			uint16_t rows = *param.rows;

			if (!param.columns) return E_FAIL;
			uint16_t columns = *param.columns;

			bool invert_colors = false;
			if (!param.photometric_interpretation) return E_FAIL;
			switch (*param.photometric_interpretation)
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

			double rescale_intercept = param.rescale_intercept ? *param.rescale_intercept : 0;
			double rescale_slope = param.rescale_slope ? *param.rescale_slope : 1;

			double lower_limit = 0;
			double upper_limit = 0;
			if (param.window_center && param.window_width)
			{
				lower_limit = *param.window_center - (*param.window_width / 2);
				upper_limit = *param.window_center + (*param.window_width / 2);
			}
			else
			{
				int32_t minValue = INT_MAX, maxValue = INT_MIN;
				for (size_t i = 0; i < bufSrcSize / 2; i++)
				{
					int16_t value = reinterpret_cast<int16_t*>(bufSrc.m_pData)[i];
					if (value < minValue) minValue = value;
					if (value > maxValue) maxValue = value;
				}
				lower_limit = minValue;
				upper_limit = maxValue;
			}

			void* pBits = nullptr;
			hr = decoderProcess<T>::createBuffer(columns, rows, &pBits);
			if (FAILED(hr)) return hr;

			using type = decltype(decoderProcess<T>::dummy);
			type alpha = ~static_cast<type>(0) ^ ((static_cast<type>(1) << (dstColorBits * 3)) - 1);

			int16_t* src = reinterpret_cast<int16_t*>(bufSrc.m_pData);
			type* dst = reinterpret_cast<type*>(decoderProcess<T>::getScan0());
			for (size_t y = 0; y < rows; y++)
			{
				for (size_t x = 0; x < columns; x += 1)
				{
					double value = src[x] * rescale_slope + rescale_intercept;
					value = max(min(value, upper_limit), lower_limit);
					value = dstMask * (value - lower_limit) / (upper_limit - lower_limit);
					type v = invert_colors ? (dstMask - static_cast<type>(value)) : static_cast<type>(value);
					dst[x] = alpha |
						(v << (dstColorBits * decoderProcess<T>::getRIndex())) |
						(v << (dstColorBits * decoderProcess<T>::getGIndex())) |
						(v << (dstColorBits * decoderProcess<T>::getBIndex()));
				}
				src += columns;
				dst += decoderProcess<T>::getScanOffset();
			}

			return decoderProcess<T>::postProcess();
		}

		virtual HRESULT getResult(T** ppBitmap) override
		{
			return decoderProcess<T>::detachInnerObject(ppBitmap);
		}
	};

	template<class T>
	class decoderPaletteColor16 : public decoderBase<T>, public decoderProcess<T>
	{
	public:
		HRESULT decode(const container_DCM_Impl* image, const param& param) override
		{
			const uint32_t dstColorBits = decoderProcess<T>::bpc;

			CHeapPtr<uint16_t> bufSrc;
			size_t bufSrcSize = 0;
			auto hr = getTagValue(image, 0x7FE0, 0x0010, bufSrc, &bufSrcSize);
			if (FAILED(hr)) return hr;

			if (!param.rows) return E_FAIL;
			uint16_t rows = *param.rows;

			if (!param.columns) return E_FAIL;
			uint16_t columns = *param.columns;

			CHeapPtr<uint16_t> r_pallet;
			CHeapPtr<uint16_t> g_pallet;
			CHeapPtr<uint16_t> b_pallet;
			hr = getPallet(image, 0x0028, 0x1201, r_pallet);
			if (FAILED(hr)) return hr;
			hr = getPallet(image, 0x0028, 0x1202, g_pallet);
			if (FAILED(hr)) return hr;
			hr = getPallet(image, 0x0028, 0x1203, b_pallet);
			if (FAILED(hr)) return hr;

			void* pBits = nullptr;
			hr = decoderProcess<T>::createBuffer(columns, rows, &pBits);
			if (FAILED(hr)) return hr;

			using type = decltype(decoderProcess<T>::dummy);
			type alpha = ~static_cast<type>(0) ^ ((static_cast<type>(1) << (dstColorBits * 3)) - 1);

			uint16_t* src = reinterpret_cast<uint16_t*>(bufSrc.m_pData);
			type* dst = reinterpret_cast<type*>(decoderProcess<T>::getScan0());
			for (size_t y = 0; y < rows; y++)
			{
				for (size_t x = 0; x < columns; x += 1)
				{
					uint16_t index = src[x];
					if (index >= bufSrcSize / 2) return E_FAIL;
					uint16_t r = r_pallet[index];
					uint16_t g = g_pallet[index];
					uint16_t b = b_pallet[index];
					dst[x] = alpha |
						(((type)r >> (16 - dstColorBits)) << (dstColorBits * decoderProcess<T>::getRIndex())) |
						(((type)g >> (16 - dstColorBits)) << (dstColorBits * decoderProcess<T>::getGIndex())) |
						(((type)b >> (16 - dstColorBits)) << (dstColorBits * decoderProcess<T>::getBIndex()));
				}
				src += columns;
				dst += decoderProcess<T>::getScanOffset();
			}

			return decoderProcess<T>::postProcess();
		}

		virtual HRESULT getResult(T** ppBitmap) override
		{
			return decoderProcess<T>::detachInnerObject(ppBitmap);
		}
	};

	template<class T>
	class decoderRGB24_pixel : public decoderBase<T>, public decoderProcess<T>
	{
	public:
		HRESULT decode(const container_DCM_Impl* image, const param& param) override
		{
			const uint32_t dstColorBits = decoderProcess<T>::bpc;

			CHeapPtr<uint8_t> bufSrc;
			size_t bufSrcSize = 0;
			auto hr = getTagValue(image, 0x7FE0, 0x0010, bufSrc, &bufSrcSize);
			if (FAILED(hr)) return hr;

			if (!param.rows) return E_FAIL;
			uint16_t rows = *param.rows;

			if (!param.columns) return E_FAIL;
			uint16_t columns = *param.columns;

			void* pBits = nullptr;
			hr = decoderProcess<T>::createBuffer(columns, rows, &pBits);
			if (FAILED(hr)) return hr;

			using type = decltype(decoderProcess<T>::dummy);
			type alpha = ~static_cast<type>(0) ^ ((static_cast<type>(1) << (dstColorBits * 3)) - 1);

			uint8_t* p = reinterpret_cast<uint8_t*>(bufSrc.m_pData);
			type* dst = reinterpret_cast<type*>(decoderProcess<T>::getScan0());
			for (int y = 0; y < rows; y++)
			{
				for (int x = 0; x < columns; x++)
				{
					const uint8_t& r = *p++;
					const uint8_t& g = *p++;
					const uint8_t& b = *p++;
					dst[x] = alpha |
						((type)r << (dstColorBits * decoderProcess<T>::getRIndex() + (dstColorBits - 8))) |
						((type)g << (dstColorBits * decoderProcess<T>::getGIndex() + (dstColorBits - 8))) |
						((type)b << (dstColorBits * decoderProcess<T>::getBIndex() + (dstColorBits - 8)));
				}
				dst += decoderProcess<T>::getScanOffset();
			}

			return decoderProcess<T>::postProcess();
		}
		virtual HRESULT getResult(T** ppBitmap) override
		{
			return decoderProcess<T>::detachInnerObject(ppBitmap);
		}
	};

	template<class T>
	class decoderRGB24_planar : public decoderBase<T>, public decoderProcess<T>
	{
	public:
		HRESULT decode(const container_DCM_Impl* image, const param& param) override
		{
			const uint32_t dstColorBits = decoderProcess<T>::bpc;

			CHeapPtr<uint8_t> bufSrc;
			size_t bufSrcSize = 0;
			auto hr = getTagValue(image, 0x7FE0, 0x0010, bufSrc, &bufSrcSize);
			if (FAILED(hr)) return hr;

			if (!param.rows) return E_FAIL;
			uint16_t rows = *param.rows;

			if (!param.columns) return E_FAIL;
			uint16_t columns = *param.columns;

			void* pBits = nullptr;
			hr = decoderProcess<T>::createBuffer(columns, rows, &pBits);
			if (FAILED(hr)) return hr;

			using type = decltype(decoderProcess<T>::dummy);
			type alpha = ~static_cast<type>(0) ^ ((static_cast<type>(1) << (dstColorBits * 3)) - 1);

			uint8_t* pR = reinterpret_cast<uint8_t*>(bufSrc.m_pData);
			uint8_t* pG = pR + columns * rows;
			uint8_t* pB = pG + columns * rows;
			type* dst = reinterpret_cast<type*>(decoderProcess<T>::getScan0());
			for (int y = 0; y < rows; y++)
			{
				for (int x = 0; x < columns; x++)
				{
					dst[x] = alpha |
						((type)pR[x] << (dstColorBits * decoderProcess<T>::getRIndex() + (dstColorBits - 8))) |
						((type)pG[x] << (dstColorBits * decoderProcess<T>::getGIndex() + (dstColorBits - 8))) |
						((type)pB[x] << (dstColorBits * decoderProcess<T>::getBIndex() + (dstColorBits - 8)));
				}
				pR += columns;
				pG += columns;
				pB += columns;
				dst += decoderProcess<T>::getScanOffset();
			}

			return decoderProcess<T>::postProcess();
		}

		virtual HRESULT getResult(T** ppBitmap) override
		{
			return decoderProcess<T>::detachInnerObject(ppBitmap);
		}
	};

	template<class T>
	class decoderRLE8_palette_color : public decoderBase<T>, public decoderProcess<T>
	{
	public:
		HRESULT decode(const container_DCM_Impl* image, const param& param) override
		{
			const uint32_t dstColorBits = decoderProcess<T>::bpc;

			CHeapPtr<uint8_t> bufSrc;
			size_t bufSrcSize = 0;
			{
				agaliaPtr<agaliaElement> item;
				auto hr = findTag(image, 0x7FE0, 0x0010, &item);
				if (FAILED(hr)) return E_FAIL;

				agaliaPtr<agaliaElement> next;
				hr = item->getChild(0, &next);
				if (FAILED(hr)) return E_FAIL;

				item = next.detach();
				hr = item->getNext(&next);
				if (FAILED(hr)) return E_FAIL;

				dicom_element elem(image, next->getOffset());
				hr = loadValue(elem, bufSrc, &bufSrcSize);
				if (FAILED(hr)) return hr;
			}

			if (!param.rows) return E_FAIL;
			uint16_t rows = *param.rows;

			if (!param.columns) return E_FAIL;
			uint16_t columns = *param.columns;

			if (!param.samples_per_pixel) return E_FAIL;
			uint16_t samples_per_pixel = *param.samples_per_pixel;

			std::vector<uint8_t> decodedData;
			auto hr = decodeRLE8(bufSrc.m_pData, bufSrcSize, columns * rows, samples_per_pixel, decodedData);
			if (FAILED(hr)) return hr;

			CHeapPtr<uint16_t> r_pallet;
			CHeapPtr<uint16_t> g_pallet;
			CHeapPtr<uint16_t> b_pallet;
			hr = getPallet(image, 0x0028, 0x1201, r_pallet);
			if (FAILED(hr)) return hr;
			hr = getPallet(image, 0x0028, 0x1202, g_pallet);
			if (FAILED(hr)) return hr;
			hr = getPallet(image, 0x0028, 0x1203, b_pallet);
			if (FAILED(hr)) return hr;

			void* pBits = nullptr;
			hr = decoderProcess<T>::createBuffer(columns, rows, &pBits);
			if (FAILED(hr)) return hr;

			using type = decltype(decoderProcess<T>::dummy);
			type alpha = ~static_cast<type>(0) ^ ((static_cast<type>(1) << (dstColorBits * 3)) - 1);

			uint8_t* src = decodedData.data();
			type* dst = reinterpret_cast<type*>(decoderProcess<T>::getScan0());
			for (int y = 0; y < rows; y++)
			{
				for (int x = 0; x < columns; x++)
				{
					const uint8_t& v = src[x];
					dst[x] = alpha |
						(((type)r_pallet[v] >> (16 - dstColorBits)) << (dstColorBits * decoderProcess<T>::getRIndex())) |
						(((type)g_pallet[v] >> (16 - dstColorBits)) << (dstColorBits * decoderProcess<T>::getGIndex())) |
						(((type)b_pallet[v] >> (16 - dstColorBits)) << (dstColorBits * decoderProcess<T>::getBIndex()));

				}
				src += columns;
				dst += decoderProcess<T>::getScanOffset();
			}

			return decoderProcess<T>::postProcess();
		}

		virtual HRESULT getResult(T** ppBitmap) override
		{
			return decoderProcess<T>::detachInnerObject(ppBitmap);
		}
	};

	template<class T>
	class decoderRLE16_palette_color : public decoderBase<T>, public decoderProcess<T>
	{
	public:
		HRESULT decode(const container_DCM_Impl* image, const param& param) override
		{
			const uint32_t dstColorBits = decoderProcess<T>::bpc;

			CHeapPtr<uint8_t> bufSrc;
			size_t bufSrcSize = 0;
			{
				agaliaPtr<agaliaElement> item;
				auto hr = findTag(image, 0x7FE0, 0x0010, &item);
				if (FAILED(hr)) return E_FAIL;

				agaliaPtr<agaliaElement> next;
				hr = item->getChild(0, &next);
				if (FAILED(hr)) return E_FAIL;

				item = next.detach();
				hr = item->getNext(&next);
				if (FAILED(hr)) return E_FAIL;

				dicom_element elem(image, next->getOffset());
				hr = loadValue(elem, bufSrc, &bufSrcSize);
				if (FAILED(hr)) return hr;
			}

			if (!param.rows) return E_FAIL;
			uint16_t rows = *param.rows;

			if (!param.columns) return E_FAIL;
			uint16_t columns = *param.columns;

			if (!param.samples_per_pixel) return E_FAIL;
			uint16_t samples_per_pixel = *param.samples_per_pixel;

			std::vector<uint16_t> decodedData;
			auto hr = decodeRLE16(bufSrc, bufSrcSize, columns * rows, samples_per_pixel, decodedData);
			if (FAILED(hr)) return hr;

			CHeapPtr<uint16_t> r_pallet;
			CHeapPtr<uint16_t> g_pallet;
			CHeapPtr<uint16_t> b_pallet;
			hr = getPallet(image, 0x0028, 0x1201, r_pallet);
			if (FAILED(hr)) return hr;
			hr = getPallet(image, 0x0028, 0x1202, g_pallet);
			if (FAILED(hr)) return hr;
			hr = getPallet(image, 0x0028, 0x1203, b_pallet);
			if (FAILED(hr)) return hr;

			void* pBits = nullptr;
			hr = decoderProcess<T>::createBuffer(columns, rows, &pBits);
			if (FAILED(hr)) return hr;

			using type = decltype(decoderProcess<T>::dummy);
			type alpha = ~static_cast<type>(0) ^ ((static_cast<type>(1) << (dstColorBits * 3)) - 1);

			uint16_t* src = reinterpret_cast<uint16_t*>(decodedData.data());
			type* dst = reinterpret_cast<type*>(decoderProcess<T>::getScan0());
			for (int y = 0; y < rows; y++)
			{
				for (int x = 0; x < columns; x++)
				{
					const uint16_t& v = src[x];
					dst[x] = alpha |
						(((type)r_pallet[v] >> (16 - dstColorBits)) << (dstColorBits * decoderProcess<T>::getRIndex())) |
						(((type)g_pallet[v] >> (16 - dstColorBits)) << (dstColorBits * decoderProcess<T>::getGIndex())) |
						(((type)b_pallet[v] >> (16 - dstColorBits)) << (dstColorBits * decoderProcess<T>::getBIndex()));
				}
				src += columns;
				dst += decoderProcess<T>::getScanOffset();
			}

			return decoderProcess<T>::postProcess();
		}

		virtual HRESULT getResult(T** ppBitmap) override
		{
			return decoderProcess<T>::detachInnerObject(ppBitmap);
		}
	};

	template<class T>
	class decoderRLE24_rgb : public decoderBase<T>, public decoderProcess<T>
	{
	public:
		HRESULT decode(const container_DCM_Impl* image, const param& param) override
		{
			const uint32_t dstColorBits = decoderProcess<T>::bpc;

			CHeapPtr<uint8_t> bufSrc;
			size_t bufSrcSize = 0;
			{
				agaliaPtr<agaliaElement> item;
				auto hr = findTag(image, 0x7FE0, 0x0010, &item);
				if (FAILED(hr)) return E_FAIL;

				agaliaPtr<agaliaElement> next;
				hr = item->getChild(0, &next);
				if (FAILED(hr)) return E_FAIL;

				item = next.detach();
				hr = item->getNext(&next);
				if (FAILED(hr)) return E_FAIL;

				dicom_element elem(image, next->getOffset());
				hr = loadValue(elem, bufSrc, &bufSrcSize);
				if (FAILED(hr)) return hr;
			}

			if (!param.rows) return E_FAIL;
			uint16_t rows = *param.rows;

			if (!param.columns) return E_FAIL;
			uint16_t columns = *param.columns;

			if (!param.samples_per_pixel) return E_FAIL;
			uint16_t samples_per_pixel = *param.samples_per_pixel;

			std::vector<uint8_t> decodedData;
			auto hr = decodeRLE8(bufSrc.m_pData, bufSrcSize, columns * rows, samples_per_pixel, decodedData);
			if (FAILED(hr)) return hr;

			void* pBits = nullptr;
			hr = decoderProcess<T>::createBuffer(columns, rows, &pBits);
			if (FAILED(hr)) return hr;

			using type = decltype(decoderProcess<T>::dummy);
			type alpha = ~static_cast<type>(0) ^ ((static_cast<type>(1) << (dstColorBits * 3)) - 1);

			uint8_t* src = decodedData.data();
			type* dst = reinterpret_cast<type*>(decoderProcess<T>::getScan0());
			for (int y = 0; y < rows; y++)
			{
				for (int x = 0, k = 0; x < columns; x++, k += 3)
				{
					dst[x] = alpha |
						((type)src[k + 0] << (dstColorBits * decoderProcess<T>::getRIndex() + (dstColorBits - 8))) |
						((type)src[k + 1] << (dstColorBits * decoderProcess<T>::getGIndex() + (dstColorBits - 8))) |
						((type)src[k + 2] << (dstColorBits * decoderProcess<T>::getBIndex() + (dstColorBits - 8)));
				}
				src += 3 * columns;
				dst += decoderProcess<T>::getScanOffset();
			}

			return decoderProcess<T>::postProcess();
		}

		virtual HRESULT getResult(T** ppBitmap) override
		{
			return decoderProcess<T>::detachInnerObject(ppBitmap);
		}
	};

	template<class T>
	class decoderRLE24_ybr_full : public decoderBase<T>, public decoderProcess<T>
	{
	public:
		HRESULT decode(const container_DCM_Impl* image, const param& param) override
		{
			const uint32_t dstColorBits = decoderProcess<T>::bpc;

			CHeapPtr<uint8_t> bufSrc;
			size_t bufSrcSize = 0;
			{
				agaliaPtr<agaliaElement> item;
				auto hr = findTag(image, 0x7FE0, 0x0010, &item);
				if (FAILED(hr)) return E_FAIL;

				agaliaPtr<agaliaElement> next;
				hr = item->getChild(0, &next);
				if (FAILED(hr)) return E_FAIL;

				item = next.detach();
				hr = item->getNext(&next);
				if (FAILED(hr)) return E_FAIL;

				dicom_element elem(image, next->getOffset());
				hr = loadValue(elem, bufSrc, &bufSrcSize);
				if (FAILED(hr)) return hr;
			}

			if (!param.rows) return E_FAIL;
			uint16_t rows = *param.rows;

			if (!param.columns) return E_FAIL;
			uint16_t columns = *param.columns;

			if (!param.samples_per_pixel) return E_FAIL;
			uint16_t samples_per_pixel = *param.samples_per_pixel;

			std::vector<uint8_t> decodedData;
			auto hr = decodeRLE8(bufSrc.m_pData, bufSrcSize, columns * rows, samples_per_pixel, decodedData);
			if (FAILED(hr)) return hr;

			void* pBits = nullptr;
			hr = decoderProcess<T>::createBuffer(columns, rows, &pBits);
			if (FAILED(hr)) return hr;

			using type = decltype(decoderProcess<T>::dummy);
			type alpha = ~static_cast<type>(0) ^ ((static_cast<type>(1) << (dstColorBits * 3)) - 1);

			uint8_t* src = decodedData.data();
			type* dst = reinterpret_cast<type*>(decoderProcess<T>::getScan0());
			for (int y = 0; y < rows; y++)
			{
				for (int x = 0, k = 0; x < columns; x++, k += 3)
				{
					uint8_t r, g, b;
					ybrfull_to_rgb(
						src[k + 0], src[k + 1], src[k + 2],
						&r, &g, &b);
					dst[x] = alpha |
						((type)r << (dstColorBits * decoderProcess<T>::getRIndex() + (dstColorBits - 8))) |
						((type)g << (dstColorBits * decoderProcess<T>::getGIndex() + (dstColorBits - 8))) |
						((type)b << (dstColorBits * decoderProcess<T>::getBIndex() + (dstColorBits - 8)));
				}
				src += 3 * columns;
				dst += decoderProcess<T>::getScanOffset();
			}

			return decoderProcess<T>::postProcess();
		}

		virtual HRESULT getResult(T** ppBitmap) override
		{
			return decoderProcess<T>::detachInnerObject(ppBitmap);
		}
	};

	template<class T>
	static HRESULT decodeOther(T** ppBitmap, IWICColorContext** ppColorContext, const container_DCM_Impl* image, uint32_t maxW, uint32_t maxH)
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

		agaliaStringPtr path;
		hr = image->getFilePath(&path);
		if (FAILED(hr)) return hr;

		agaliaPtr<agaliaContainer> jpeg;
		hr = getAgaliaImage(&jpeg, path, value_offset, value_size, agalia_format_auto | agalia_format_force);
		if (FAILED(hr)) return hr;

		if (ppColorContext)
			return loadWICBitmap(reinterpret_cast<IWICBitmap**>(ppBitmap), ppColorContext, jpeg);
		else if (maxW != 0 && maxH != 0)
			return loadThumbnail(reinterpret_cast<agaliaBitmap**>(ppBitmap), jpeg, maxW, maxH);
		else
			return loadBitmap(reinterpret_cast<agaliaBitmap**>(ppBitmap), jpeg);
	}

	template<class T>
	HRESULT decodeDCM(T** ppBitmap, IWICColorContext** ppColorContext, const container_DCM_Impl* image, uint32_t maxW, uint32_t maxH)
	{
		UNREFERENCED_PARAMETER(maxW);
		UNREFERENCED_PARAMETER(maxH);

		param param;
		auto hr = getDecodeParam(image, param);
		if (FAILED(hr)) return hr;

		auto& transfer_syntax_uid = *param.transfer_syntax_uid;

		if (transfer_syntax_uid == "1.2.840.10008.1.2" ||	// Implicit VR Little Endian
			transfer_syntax_uid == "1.2.840.10008.1.2.1")	// Explicit VR Little Endian 
		{
			if (!param.samples_per_pixel) return E_FAIL;
			switch (*param.samples_per_pixel)
			{
			case 1:
				if (!param.bits_allocated) return E_FAIL;
				switch (*param.bits_allocated)
				{
				case 8:
				{
					decoderGrayscale8<T> decoder;
					hr = decoder.decode(image, param);
					if (FAILED(hr)) return hr;
					return decoder.getResult(ppBitmap);
				}
				case 16:
					if (!param.photometric_interpretation) return E_FAIL;
					switch (*param.photometric_interpretation)
					{
					case MONOCHROME1:
					case MONOCHROME2:
						if (!param.pixel_representation) return E_FAIL;
						if (*param.pixel_representation == 1) {
							decoderGrayscale16_signed<T> decoder;
							hr = decoder.decode(image, param);
							if (FAILED(hr)) return hr;
							return decoder.getResult(ppBitmap);
						}
						else {
							decoderGrayscale16_unsigned<T> decoder;
							hr = decoder.decode(image, param);
							if (FAILED(hr)) return hr;
							return decoder.getResult(ppBitmap);
						}
						break;
					case PALETTE_COLOR:
					{
						decoderPaletteColor16<T> decoder;
						hr = decoder.decode(image, param);
						if (FAILED(hr)) return hr;
						return decoder.getResult(ppBitmap);
					}
					}
				}
				break;

			case 3:
				if (!param.photometric_interpretation) return E_FAIL;
				switch (*param.photometric_interpretation)
				{
				case RGB:
					if (!param.planar_configuration) return E_FAIL;
					if (*param.planar_configuration == 0) {
						decoderRGB24_pixel<T> decoder;
						hr = decoder.decode(image, param);
						if (FAILED(hr)) return hr;
						return decoder.getResult(ppBitmap);
					}
					else {
						decoderRGB24_planar<T> decoder;
						hr = decoder.decode(image, param);
						if (FAILED(hr)) return hr;
						return decoder.getResult(ppBitmap);
					}
				}
			}
		}
		else if (transfer_syntax_uid == "1.2.840.10008.1.2.5")	// RLE Lossless
		{

			if (!param.samples_per_pixel) return E_FAIL;
			switch (*param.samples_per_pixel)
			{
			case 1:
				if (!param.bits_allocated) return E_FAIL;
				switch (*param.bits_allocated)
				{
				case 8:
					if (!param.photometric_interpretation) return E_FAIL;
					switch (*param.photometric_interpretation)
					{
					case MONOCHROME1:
					case MONOCHROME2:
						return E_FAIL;
					case PALETTE_COLOR:
					{
						decoderRLE8_palette_color<T> decoder;
						hr = decoder.decode(image, param);
						if (FAILED(hr)) return hr;
						return decoder.getResult(ppBitmap);
					}
					}
				case 16:
					if (!param.photometric_interpretation) return E_FAIL;
					switch (*param.photometric_interpretation)
					{
					case MONOCHROME1:
					case MONOCHROME2:
						return E_FAIL;
					case PALETTE_COLOR:
					{
						decoderRLE16_palette_color<T> decoder;
						hr = decoder.decode(image, param);
						if (FAILED(hr)) return hr;
						return decoder.getResult(ppBitmap);
					}
					}
				}
				break;
			case 3:
				if (!param.photometric_interpretation) return E_FAIL;
				switch (*param.photometric_interpretation)
				{
				case YBR_FULL: {
					decoderRLE24_ybr_full<T> decoder;
					hr = decoder.decode(image, param);
					if (FAILED(hr)) return hr;
					return decoder.getResult(ppBitmap);
				}
				case RGB: {
					decoderRLE24_rgb<T> decoder;
					hr = decoder.decode(image, param);
					if (FAILED(hr)) return hr;
					return decoder.getResult(ppBitmap);
				}
				}
			}
		}
		else if (transfer_syntax_uid == "1.2.840.10008.1.2.4.50" ||	// JPEG Baseline (Process 1) 
			transfer_syntax_uid == "1.2.840.10008.1.2.4.90" ||	// JPEG 2000 Image Compression (Lossless Only) 
			transfer_syntax_uid == "1.2.840.10008.1.2.4.91")	// JPEG 2000 Image Compression 
		{
			return decodeOther(ppBitmap, ppColorContext, image, maxW, maxH);
		}
		else
		{
			return decodeOther(ppBitmap, ppColorContext, image, maxW, maxH);
		}

		return E_FAIL;
	}
};

agaliaDecoderDCM::agaliaDecoderDCM()
{
}

agaliaDecoderDCM::~agaliaDecoderDCM()
{
}

ULONG agaliaDecoderDCM::AddRef(void)
{
	return ++refCount;
}

ULONG agaliaDecoderDCM::Release(void)
{
	if (--refCount == 0)
	{
		delete this;
		return 0;
	}
	return refCount;
}

HRESULT agaliaDecoderDCM::decode(agaliaBitmap** ppBitmap, const agaliaContainer* image)
{
	if (!ppBitmap) return E_POINTER;
	if (!image) return E_POINTER;

	auto container = dynamic_cast<const analyze_DCM::container_DCM_Impl*>(image);
	if (!container) return E_FAIL;

	return decodeDCM(ppBitmap, nullptr, container, 0, 0);
}

HRESULT agaliaDecoderDCM::decode(IWICBitmap** ppBitmap, IWICColorContext** ppColorContext, const agaliaContainer* image)
{
	if (!ppBitmap) return E_POINTER;
	if (!ppColorContext) return E_POINTER;
	if (!image) return E_POINTER;

	auto container = dynamic_cast<const analyze_DCM::container_DCM_Impl*>(image);
	if (!container) return E_FAIL;

	CComPtr<IWICColorContext> colorContext;
	auto hr = decodeDCM(ppBitmap, &colorContext, container, 0, 0);
	if (SUCCEEDED(hr)) {
		*ppColorContext = colorContext.Detach();
	}

	return hr;
}

HRESULT agaliaDecoderDCM::decodeThumbnail(agaliaBitmap** ppBitmap, const agaliaContainer* image, uint32_t maxW, uint32_t maxH)
{
	if (!ppBitmap) return E_POINTER;
	if (!image) return E_POINTER;

	auto container = dynamic_cast<const analyze_DCM::container_DCM_Impl*>(image);
	if (!container) return E_FAIL;

	return decodeDCM(ppBitmap, nullptr, container, maxW, maxH);
}
