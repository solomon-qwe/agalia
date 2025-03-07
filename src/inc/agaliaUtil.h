#pragma once

#ifdef AGALIAUTIL_EXPORTS
#define AGALIAUTIL_API __declspec(dllexport)
#else
#define AGALIAUTIL_API __declspec(dllimport)
#endif

#include <windows.h>

class agaliaBitmap
{
public:
	virtual void Release() = 0;
	virtual HRESULT getHBitmap(HBITMAP* phBitmap) const = 0;
	virtual HRESULT getBitmapInfo(BITMAPV5HEADER* pbi) const = 0;
	virtual HRESULT getBits(void** pBits) const = 0;
	virtual HRESULT createBitmap(const BITMAPV5HEADER& bi) = 0;
	virtual HRESULT init(HBITMAP hBitmap, const BITMAPV5HEADER& bi, void* pBits) = 0;
};

extern "C"
{
	AGALIAUTIL_API HRESULT createAgaliaBitmap(agaliaBitmap** p, BITMAPV5HEADER* pbi);
}
