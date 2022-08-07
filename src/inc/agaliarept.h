#pragma once

#ifdef AGALIAREPT_EXPORTS
#define AGALIAREPT_API __declspec(dllexport)
#else
#define AGALIAREPT_API __declspec(dllimport)
#endif

#include <stdint.h>

struct IStream;
class agaliaContainer;

// heap memory wrapper 

class agaliaHeap
{
public:
	virtual void Release(void) = 0;
	virtual HRESULT Free(void) = 0;
	virtual HRESULT AllocateBytes(rsize_t size, bool init = false) = 0;
	virtual rsize_t GetSize(void) const = 0;
	virtual void* GetData(void) const = 0;
};


// string wrapper 

class agaliaString
{
public:
	agaliaString();
	agaliaString(const wchar_t* src);
	virtual ~agaliaString();
	virtual void Release(void);

	virtual void Free(void);
	HRESULT Alloc(rsize_t size);

	HRESULT Copy(const wchar_t* src);

	wchar_t* GetData(void) const { return reinterpret_cast<wchar_t*>(_p); }

	wchar_t* _p = nullptr;

	static agaliaString* create(const wchar_t* src);
};



// command line parser 

class agaliaCmdLineParam
{
public:
	virtual void Release(void) = 0;
	virtual uint64_t getOffset(void) const = 0;
	virtual uint64_t getSize(void) const = 0;
	virtual const wchar_t* getTargetFilePath(void) const = 0;
	virtual const wchar_t* getOutputFilePath(void) const = 0;

	AGALIAREPT_API static HRESULT parseCmdLine(agaliaCmdLineParam** param);
};



// core object 

class agaliaItem
{
public:
	virtual void Release(void) = 0;

	virtual const GUID& getGUID(void) const = 0;
	virtual uint64_t getOffset(void) const = 0;
	virtual uint64_t getSize(void) const = 0;

	virtual HRESULT getItemName(agaliaString** str) const = 0;
	virtual HRESULT getItemPropCount(uint32_t* count) const = 0;
	virtual HRESULT getItemPropName(uint32_t index, agaliaString** str) const = 0;
	virtual HRESULT getItemPropValue(uint32_t index, agaliaString** str) const = 0;

	virtual HRESULT getChildItem(uint32_t sibling, agaliaItem** child) const = 0;
	virtual HRESULT getNextItem(agaliaItem** next) const = 0;

	virtual HRESULT getAsocImage(const agaliaContainer** imageAsoc) const = 0;
	virtual HRESULT getValueAreaOffset(uint64_t* offset) const = 0;
	virtual HRESULT getValueAreaSize(uint64_t* size) const = 0;
};


class agaliaContainer
{
public:
	virtual void Release(void) = 0;

	virtual HRESULT getRootItem(agaliaItem** root) const = 0;

	virtual HRESULT getColumnCount(uint32_t* count) const = 0;
	virtual HRESULT getColumnWidth(uint32_t column, int32_t* length) const = 0;
	virtual HRESULT getColumnName(uint32_t column, agaliaString** str) const = 0;

	virtual HRESULT getGridRowCount(const agaliaItem* item, uint32_t* row) const = 0;
	virtual HRESULT getGridValue(const agaliaItem* item, uint32_t row, uint32_t column, agaliaString** str) const = 0;

	enum PropertyType
	{
		ContainerType,
		FormatType,
		ImageWidth,
		ImageHeight,
		ShootingDateTime,
		CreationDateTime,
		ICCProfile
	};
	virtual HRESULT getPropertyValue(PropertyType type, agaliaString** str) const = 0;
	virtual HRESULT getThumbnailImage(HBITMAP* phBitmap, uint32_t maxW, uint32_t maxH) const = 0;
	virtual HRESULT getColorProfile(agaliaHeap** buf) const = 0;

	virtual HRESULT LockStream(void) const = 0;
	virtual HRESULT UnlockStream(void) const = 0;
	virtual HRESULT ReadData(void* buf, uint64_t pos, uint64_t size) const = 0;
	virtual HRESULT getAsocStream(IStream** stream) const = 0;

	virtual HRESULT getFilePath(agaliaString** str) const = 0;
};


// smart pointer 

template <class T>
class agaliaPtr
{
public:
	agaliaPtr() {}
	agaliaPtr(T* p) { _p = p; }
	~agaliaPtr() { attach(nullptr); }

	void attach(T* p) { if (_p) { auto temp = _p; _p = p; temp->Release(); } else { _p = p; } }

	T* detach(void) { auto temp = _p; _p = nullptr; return temp; }

	T* operator =(T* p) { attach(p); return _p; }
	T** operator &() { attach(nullptr); return &_p; }
	T* operator->() { return _p; }
	operator T* () { return _p; }

	T* _p = nullptr;
};

class agaliaStringPtr : public agaliaPtr<agaliaString>
{
public:
	agaliaStringPtr() {}
	agaliaStringPtr(agaliaString* p) :agaliaPtr(p) {}
	virtual ~agaliaStringPtr() {}

	agaliaString* operator =(agaliaString* p) { attach(p); return _p; }
	operator bool() const { return _p && _p->GetData(); }
	operator const wchar_t* () const { return _p->GetData(); }
};


enum PreferenceProperty {
	dictionary_lang,
	dicom_force_dictionary_vr
};

enum PreferredDictonaryLanguage {
	English,
	Japanese
};


extern "C"
{
	const int agalia_format_auto = -1;
	AGALIAREPT_API HRESULT getAgaliaImage(agaliaContainer** image, const wchar_t* path, uint64_t offset, uint64_t size, int format = agalia_format_auto);
	AGALIAREPT_API HRESULT getAgaliaStream(IStream** stream, const wchar_t* path, uint64_t offset, uint64_t size);
	AGALIAREPT_API HRESULT setAgaliaPreference(int property, int value);
	AGALIAREPT_API HRESULT getAgaliaPreference(int property, int* value);

	AGALIAREPT_API HRESULT getAgaliaSupportTypeCount(int* count);
	AGALIAREPT_API HRESULT getAgaliaSupportTypeName(int index, agaliaString** name);
}

namespace agalia
{
	const uint32_t CP_US_ASCII = 20127;	// ISO 646 
	const uint32_t CP_LATIN1 = 28591;	// ISO 8859-1 Latin 1 
	const uint32_t CP_CYRILLIC = 28595;	// ISO 8859-5 Cyrillic 
	const uint32_t CP_ARABIC = 28596;	// ISO 8859-6 Arabic 
	const uint32_t CP_GREEK = 28597;	// ISO 8859-7 Greek 
	const uint32_t CP_HEBREW = 28598;	// ISO 8859-8 Hebrew 
	const uint32_t CP_JIS = 50221;		// JIS X 0201/0208/0212 
	const uint32_t CP_KOREAN = 949;		// Korean 
	const uint32_t CP_GB18030 = 54936;	// GB18030 


	const HRESULT AGALIA_ERR_DATA_CORRUPT = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0001);
}
