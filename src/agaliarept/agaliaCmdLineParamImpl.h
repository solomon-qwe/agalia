#pragma once

#include "../inc/agaliarept.h"

#include <string>

class _agaliaCmdLineParamImpl : public agaliaCmdLineParam
{
public:
	_agaliaCmdLineParamImpl();
	virtual ~_agaliaCmdLineParamImpl();
	virtual void Release(void) override;
	virtual uint64_t getOffset(void) const override;
	virtual uint64_t getSize(void) const override;
	virtual const wchar_t* getTargetFilePath(void) const override;
	virtual const wchar_t* getOutputFilePath(void) const override;

	uint64_t offset = 0;
	uint64_t size = 0;
	std::wstring target_file_path;
	std::wstring output_file_path;
};
