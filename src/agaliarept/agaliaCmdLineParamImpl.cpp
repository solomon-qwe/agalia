#include "pch.h"
#include "agaliaCmdLineParamImpl.h"

#include <shellapi.h>	// for CommandLineToArgvW 

_agaliaCmdLineParamImpl::_agaliaCmdLineParamImpl()
{
}

_agaliaCmdLineParamImpl::~_agaliaCmdLineParamImpl()
{
}

void _agaliaCmdLineParamImpl::Release(void)
{
	delete this;
}

uint64_t _agaliaCmdLineParamImpl::getOffset(void) const
{
	return offset;
}

uint64_t _agaliaCmdLineParamImpl::getSize(void) const
{
	return size;
}

const wchar_t* _agaliaCmdLineParamImpl::getTargetFilePath(void) const
{
	return target_file_path.c_str();
}

const wchar_t* _agaliaCmdLineParamImpl::getOutputFilePath(void) const
{
	return output_file_path.c_str();
}


HRESULT agaliaCmdLineParam::parseCmdLine(agaliaCmdLineParam** param)
{
	int nNumArg = 0;
	LPWSTR* ppsz = ::CommandLineToArgvW(::GetCommandLine(), &nNumArg);

	if (nNumArg == 1) {
		return S_FALSE;
	}

	HRESULT result = E_INVALIDARG;

	auto p = new _agaliaCmdLineParamImpl;

	for (int i = 1; i < nNumArg; i++)
	{
		// ��͑Ώۃt�@�C�� 
		if (ppsz[i][0] != L'/')
		{
			p->target_file_path = ppsz[i];
			result = S_OK;
			continue;
		}

		const wchar_t* arg;

		// ���̓f�[�^�� DICOM �� Explicit VR �ł����Ă��Adictionary �Ŏw�肳�ꂽ VR ��D�悷�� 
		arg = L"/force_dictionary_vr";
		if (_wcsnicmp(ppsz[i], arg, wcslen(arg)) == 0)
		{
			setAgaliaPreference(dicom_force_dictionary_vr, 1);
			continue;
		}

		arg = L"/dictionary_lang:";
		if (_wcsnicmp(ppsz[i], arg, wcslen(arg)) == 0)
		{
			std::wstring lang = ppsz[i] + wcslen(arg);
			if (lang == L"jpn") {
				setAgaliaPreference(dicom_force_dictionary_vr, Japanese);
			}
			else {
				setAgaliaPreference(dicom_force_dictionary_vr, English);
			}
			continue;
		}

		// ���̓f�[�^�̓Ǐo���J�n�I�t�Z�b�g 
		arg = L"/offset:";
		if (_wcsnicmp(ppsz[i], arg, wcslen(arg)) == 0)
		{
			p->offset = _wcstoui64(ppsz[i] + wcslen(arg), nullptr, 10);
			continue;
		}

		// ���̓f�[�^�̓Ǐo���ő�T�C�Y 
		arg = L"/size:";
		if (_wcsnicmp(ppsz[i], arg, wcslen(arg)) == 0)
		{
			p->size = _wcstoui64(ppsz[i] + wcslen(arg), nullptr, 10);
			continue;
		}

		// �o�͐�t�@�C�� 
		arg = L"/out:";
		if (_wcsnicmp(ppsz[i], arg, wcslen(arg)) == 0)
		{
			p->output_file_path = ppsz[i] + wcslen(arg);
			continue;
		}

		// ���m�̈����͖��� 
	}

	if (SUCCEEDED(result)) {
		*param = p;
	}
	else {
		p->Release();
	}

	return result;
}
