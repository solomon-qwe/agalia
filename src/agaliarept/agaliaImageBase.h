#pragma once
#include "../inc/agaliarept.h"

struct IStream;

class _agaliaContainerBase : public agaliaContainer
{
public:
	_agaliaContainerBase(const wchar_t* path, IStream* stream);
	virtual ~_agaliaContainerBase();
	virtual void Release(void) override;

	virtual HRESULT getElementInfoCount(const agaliaElement* item, uint32_t* row) const override;
	virtual HRESULT getElementInfoValue(const agaliaElement* item, uint32_t row, uint32_t column, agaliaString** str) const override;

	virtual HRESULT loadBitmap(IWICBitmap** ppBitmap, IWICColorContext** ppColorContext) const override;

	virtual HRESULT LockStream(void) const override;
	virtual HRESULT UnlockStream(void) const override;
	virtual HRESULT ReadData(void* buf, uint64_t pos, uint64_t size) const override;
	virtual HRESULT getAsocStream(IStream** stream) const;

	virtual HRESULT getFilePath(agaliaString** str) const override;
	virtual HRESULT getColorProfile(agaliaHeap**) const override {
		return E_NOTIMPL;
	}

protected:
	IStream* data_stream = nullptr;
	agaliaString* file_path = nullptr;
	CRITICAL_SECTION cs = {};
};
