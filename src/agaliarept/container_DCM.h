#pragma once

#include "agaliareptImpl.h"

namespace analyze_DCM
{
	class container_DCM : public _agaliaContainerBase
	{
	protected:
		container_DCM(const wchar_t* path, IStream* stream);
		virtual ~container_DCM();

	public:
		static _agaliaContainerBase* CreateInstance(const wchar_t* path, IStream* stream);
		static bool IsSupported(IStream* stream);
		static HRESULT GetContainerName(agaliaString** name);
	};

	enum TransferSyntax
	{
		TransferSyntax_Unknown,
		ImplicitVR_LittleEndian,
		ExplicitVR_LittleEndian,
		DeflatedExplicitVR_LittleEndian,
		ExplicitVR_BigEndian
	};
}