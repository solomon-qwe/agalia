// agaliarept.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "../inc/agaliarept.h"
#include "../inc/handle_wrapper.h"

#include "image_file.h"
#include "callback.h"
#include "util.h"
#include <Objbase.h>	// for CoInitialize 


HRESULT create_callback_instance(agalia_ptr<callback_cls>* callback, const agalia::config* config)
{
	if (config->hwndListCtrl) {
		callback->p = new callback_obj_ListCtrl(config->hwndListCtrl, config->abort_event);
	}
	else if (config->stream) {
		callback->p = new callback_obj_stream(config->stream, L"\t");
	}
	else {
		callback->p = new callback_obj_null;
	}

	return S_OK;
}


HRESULT determin_format(agalia_ptr<image_file>* image, uint8_t* base_addr, uint64_t base_offset, uint64_t data_size)
{
	// �t�H�[�}�b�g���� 
	uint8_t* p = base_addr + base_offset;
	if (image_file_bmp::is_supported(p, data_size))
	{
		image->p = new image_file_bmp;
	}
	else if (image_file_tiff::is_supported(p, data_size))
	{
		image->p = new image_file_tiff;
	}
	else if (image_file_riff::is_supported(p, data_size))
	{
		image->p = new image_file_riff;
	}
	else if (image_file_jpeg::is_supported(p, data_size))
	{
		image->p = new image_file_jpeg;
	}
	else if (image_file_dicom::is_supported(p, data_size))
	{
		image->p = new image_file_dicom;
	}
	else if (image_file_iso::is_supported(p, data_size))
	{
		image->p = new image_file_iso;
	}
	else if (image_file_asf::is_supported(p, data_size))
	{
		image->p = new image_file_asf;
	}
	else if (image_file_m2p::is_supported(p, data_size))
	{
		image->p = new image_file_m2p;
	}
	else
	{
		return AGALIA_ERR_UNSUPPORTED;
	}

	image->p->attach(base_addr, base_offset, data_size);

	return S_OK;
}


HRESULT _AgaliaMain_Run(const agalia::config* config, uint8_t* base_addr, uint64_t base_offset, uint64_t data_size)
{
	// �t�H�[�}�b�g���� 
	agalia_ptr<image_file> image;
	HRESULT result = determin_format(&image, base_addr, base_offset, data_size);
	if (FAILED(result)) {
		return result;
	}

	// �R�[���o�b�N������������ 
	agalia_ptr<callback_cls> callback;
	create_callback_instance(&callback, config);

	// �t�H�[�}�b�g�ɉ�������͂����s 
	image.p->parse(callback.p, config);

	return S_OK;
}


HRESULT _AgaliaMain_Open(const agalia::config* config)
{
	// ��͑Ώۃt�@�C�����������Ƀ}�b�v 
	handle_wrapper hFile(INVALID_HANDLE_VALUE);
	hFile = ::CreateFile(config->get_input_file_path(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile.isErr()) {
		return HRESULT_FROM_WIN32(::GetLastError());
	}
	handle_wrapper hMap(NULL);
	hMap = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hMap.isErr()) {
		return HRESULT_FROM_WIN32(::GetLastError());
	}
	LPVOID pFile = ::MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	if (pFile == NULL) {
		return HRESULT_FROM_WIN32(::GetLastError());
	}

	// �A�N�Z�X�͈͂����� 
	LARGE_INTEGER liFileSize = {};
	::GetFileSizeEx(hFile, &liFileSize);
	uint64_t data_size = config->data_size ? min(liFileSize.QuadPart - config->offset, config->data_size) : (liFileSize.QuadPart - config->offset);
	uint64_t base_offset = config->offset;
	uint8_t* base_addr = reinterpret_cast<uint8_t*>(pFile);

	// ��͎��s 
	HRESULT result= _AgaliaMain_Run(config, base_addr, base_offset, data_size);

	// �A���}�b�v�i�n���h���̓f�X�g���N�^�ŃN���[�Y�j 
	::UnmapViewOfFile(pFile);

	return result;
}


void _agalia_trans_func(unsigned int u, struct _EXCEPTION_POINTERS* /*e*/)
{
	// �\������O��C++��O�ɕϊ� 
	switch (u)
	{
	case EXCEPTION_IN_PAGE_ERROR:
		// �l�b�g���[�N�ؒf�AUSB���������� �Ȃǂɂ��y�[�W�G���[ 
		throw agalia::exception(AGALIA_ERR_IN_PAGE_ERROR);
	}

	// �ϊ����Ȃ��G���[�̓f�t�H���g�̏��� 
}


HRESULT _AgaliaMain_ExceptionHandling(const agalia::config* config)
{
	HRESULT result = S_OK;

	// �������}�b�v���g���Ă���̂Ńy�[�W�G���[���g���b�v���� 
	_se_translator_function pre = _set_se_translator(_agalia_trans_func);
	try
	{
		result = _AgaliaMain_Open(config);
	}
	catch (agalia::exception e)
	{
		result = e.e;
	}
	_set_se_translator(pre);

	return result;
}


HRESULT _AgaliaMain_LeakCheck(const agalia::config* param)
{
	_CrtMemState s1 = {}, s2 = {}, s3 = {};

	// ���C�������̑O��Ńf�o�b�O�q�[�v��Ԃ̃X�i�b�v�V���b�g���쐬 
	_CrtMemCheckpoint(&s1);
	HRESULT result = _AgaliaMain_ExceptionHandling(param);
	_CrtMemCheckpoint(&s2);
	
	// ���[�N���Ă���΃R���\�[���ɏo�� 
	if (_CrtMemDifference(&s3, &s1, &s2)) {
		_CrtMemDumpStatistics(&s3);
#if defined(_DEBUG)
		result = AGALIA_ERR_MEMORY_LEAK;
#endif
	}

	return result;
}


extern "C" AGALIAREPT_API
HRESULT AgaliaMain(const agalia::config* param)
{
	return _AgaliaMain_LeakCheck(param);
}
