#pragma once

#include <objidl.h>
#include <stdint.h>
#include "agaliaDecoder.h"

#ifdef AGALIADECODE_EXPORTS
#define AGALIADECODE_API __declspec(dllexport)
#else
#define AGALIADECODE_API __declspec(dllimport)
#endif

extern "C"
{
	AGALIADECODE_API HRESULT initializeAgaliaDecoder(void);
	AGALIADECODE_API HRESULT finalizeAgaliaDecoder(void);
	AGALIADECODE_API HRESULT getAgaliaDecoder(agaliaDecoder** decoder);
}
