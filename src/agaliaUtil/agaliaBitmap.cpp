#include "pch.h"
#include "../inc/agaliaUtil.h"

class agaliaBitmapImpl : public agaliaBitmap
{
public:
	agaliaBitmapImpl();
	virtual ~agaliaBitmapImpl();
	virtual void Release(void) override;
	virtual HRESULT getHBitmap(HBITMAP* phBitmap) const override;
	virtual HRESULT getBitmapInfo(BITMAPV5HEADER* pbi) const override;
	virtual HRESULT getBits(void** ppBits) const override;
	virtual HRESULT createBitmap(const BITMAPV5HEADER& bi) override;
	virtual HRESULT init(HBITMAP hBitmap, const BITMAPV5HEADER& bi, void* pBits) override;

protected:
	HBITMAP hBitmap = nullptr;
	BITMAPV5HEADER bi = {};
	void* pBits = nullptr;
};

agaliaBitmapImpl::agaliaBitmapImpl()
{
}

agaliaBitmapImpl::~agaliaBitmapImpl()
{
	if (hBitmap)
		::DeleteObject(hBitmap);
}

void agaliaBitmapImpl::Release(void)
{
	delete this;
}

HRESULT agaliaBitmapImpl::getHBitmap(HBITMAP* phBitmap) const
{
	if (phBitmap == nullptr) return E_POINTER;
	*phBitmap = hBitmap;
	return S_OK;
}

HRESULT agaliaBitmapImpl::getBitmapInfo(BITMAPV5HEADER* pbi) const
{
	if (pbi == nullptr) return E_POINTER;
	*pbi = bi;
	return S_OK;
}

HRESULT agaliaBitmapImpl::getBits(void** ppBits) const
{
	if (ppBits == nullptr) return E_POINTER;
	*ppBits = pBits;
	return S_OK;
}

HRESULT agaliaBitmapImpl::createBitmap(const BITMAPV5HEADER& bih)
{
	if (hBitmap)
		return E_FAIL;

	HDC hdc = ::GetDC(NULL);
	hBitmap = ::CreateDIBSection(nullptr, (BITMAPINFO*)&bih, DIB_RGB_COLORS, &pBits, nullptr, 0);
	DWORD dwErr = ::GetLastError();
	::ReleaseDC(NULL, hdc);

	if (hBitmap == NULL)
		return HRESULT_FROM_WIN32(dwErr);

	this->bi = bih;

	return S_OK;
}

HRESULT agaliaBitmapImpl::init(HBITMAP arg_hBitmap, const BITMAPV5HEADER& arg_bi, void* arg_pBits)
{
	if (arg_hBitmap == nullptr) return E_INVALIDARG;
	if (arg_pBits == nullptr) return E_POINTER;
	if (hBitmap) return E_FAIL;

	hBitmap = arg_hBitmap;
	bi = arg_bi;
	pBits = arg_pBits;

	return S_OK;
}

HRESULT createAgaliaBitmap(agaliaBitmap** p, BITMAPV5HEADER* pbi)
{
	if (p == nullptr) return E_POINTER;
	*p = new agaliaBitmapImpl();
	if (*p == nullptr) return E_OUTOFMEMORY;
	if (pbi == nullptr) return S_OK;
	return (*p)->createBitmap(*pbi);
}