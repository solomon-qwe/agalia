#include "pch.h"
#include "container_DCM.h"

#include "analyze_DCM_item_preamble.h"

using namespace analyze_DCM;


// BMPFormat class 

container_DCM::container_DCM(const wchar_t* path, IStream* stream)
	: _agaliaContainerBase(path, stream)
{
}


container_DCM::~container_DCM()
{
}


bool container_DCM::IsSupported(IStream* stream)
{
	if (!stream) return false;

	// プリアンブルをスキップ 
	if (FAILED(stream->Seek(uint64_to_li(dicom_preamble_size), STREAM_SEEK_SET, nullptr)))
		return false;

	ULONG cbRead = 0;
	uint32_t prefix = 0;
	auto hr = stream->Read(&prefix, sizeof(prefix), &cbRead);
	if (hr != S_OK || cbRead != sizeof(prefix))
		return false;

	return (prefix == *reinterpret_cast<const uint32_t*>("DICM"));
}


_agaliaContainerBase* container_DCM::CreateInstance(const wchar_t* path, IStream* stream)
{
	return new container_DCM_Impl(path, stream);
}



HRESULT container_DCM::GetContainerName(agaliaString** name)
{
	if (name == nullptr) return E_POINTER;
	*name = agaliaString::create(L"DICOM");
	return S_OK;
}
