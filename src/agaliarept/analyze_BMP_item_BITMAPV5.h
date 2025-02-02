#pragma once

#include "agaliareptImpl.h"
#include "analyze_BMP_item_Base.h"

namespace analyze_BMP
{
	class item_BITMAPV5 : public BMP_item_Base
	{
	public:
		item_BITMAPV5(const agaliaContainer* image, uint64_t offset, uint64_t size);
		virtual ~item_BITMAPV5();

		virtual HRESULT getName(agaliaString** str) const override;
		virtual HRESULT getPropCount(uint32_t* count) const override;
		virtual HRESULT getPropName(uint32_t index, agaliaString** str) const override;
		virtual HRESULT getPropValue(uint32_t index, agaliaString** str) const override;

		virtual HRESULT getChild(uint32_t sibling, agaliaElement** child) const override;
		virtual HRESULT getNext(agaliaElement** next) const override;

		virtual HRESULT getColumnValue(uint32_t column, agaliaString** str) const override;
		virtual HRESULT getAdditionalInfoCount(uint32_t* row) const override;
		virtual HRESULT getAdditionalInfoValue(uint32_t row, agaliaString** str) const override;

		enum {
			prop_offset = BMP_item_Base::prop_last,
			prop_Size,
			prop_Width,
			prop_Height,
			prop_Planes,
			prop_BitCount,
			prop_Compression,
			prop_SizeImage,
			prop_XPelsPerMeter,
			prop_YPelsPerMeter,
			prop_ClrUsed,
			prop_ClrImportant,
			prop_RedMask,
			prop_GreenMask,
			prop_BlueMask,
			prop_AlphaMask,
			prop_CSType,
			prop_Endpoints_ciexyzRed_ciexyzX,
			prop_Endpoints_ciexyzRed_ciexyzY,
			prop_Endpoints_ciexyzRed_ciexyzZ,
			prop_Endpoints_ciexyzGreen_ciexyzX,
			prop_Endpoints_ciexyzGreen_ciexyzY,
			prop_Endpoints_ciexyzGreen_ciexyzZ,
			prop_Endpoints_ciexyzBlue_ciexyzX,
			prop_Endpoints_ciexyzBlue_ciexyzY,
			prop_Endpoints_ciexyzBlue_ciexyzZ,
			prop_GammaRed,
			prop_GammaGreen,
			prop_GammaBlue,
			prop_Intent,
			prop_ProfileData,
			prop_ProfileSize,
			prop_Reserved,
			prop_last
		};

		enum {
			addinf_Width,
			addinf_Height,
			addinf_Planes,
			addinf_BitCount,
			addinf_Compression,
			addinf_SizeImage,
			addinf_XPelsPerMeter,
			addinf_YPelsPerMeter,
			addinf_ClrUsed,
			addinf_ClrImportant,
			addinf_RedMask,
			addinf_GreenMask,
			addinf_BlueMask,
			addinf_AlphaMask,
			addinf_CSType,
			addinf_Endpoints_ciexyzRed_ciexyzX,
			addinf_Endpoints_ciexyzRed_ciexyzY,
			addinf_Endpoints_ciexyzRed_ciexyzZ,
			addinf_Endpoints_ciexyzGreen_ciexyzX,
			addinf_Endpoints_ciexyzGreen_ciexyzY,
			addinf_Endpoints_ciexyzGreen_ciexyzZ,
			addinf_Endpoints_ciexyzBlue_ciexyzX,
			addinf_Endpoints_ciexyzBlue_ciexyzY,
			addinf_Endpoints_ciexyzBlue_ciexyzZ,
			addinf_GammaRed,
			addinf_GammaGreen,
			addinf_GammaBlue,
			addinf_Intent,
			addinf_ProfileData,
			addinf_ProfileSize,
			addinf_Reserved,
			addinf_last
		};
	};
}