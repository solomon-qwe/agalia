#pragma once

#include "container_DCM_Impl.h"

namespace analyze_DCM
{
	HRESULT loadDCMImage(const container_DCM_Impl* image, const wchar_t* path, uint32_t maxW, uint32_t maxH, HBITMAP* phBitmap);
}
