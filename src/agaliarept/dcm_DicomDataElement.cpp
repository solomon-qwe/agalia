#include "stdafx.h"

#include "dcm_DicomDataElement.h"
#include "dcm_common.h"

HRESULT CDicomDataElement::getVR(uint16_t* vr) const
{
	uint16_t ret = 0;

	if (getGroup() == 0xFFFE)
	{
		// '\0\0' ��Ԃ� 
		ret = 0;
	}
	else if (getSyntax() == ImplicitVR_LittleEndian)
	{
		// �ÖٓI�Ȃ� DataDictionary ���Q�Ƃ��� 
		dcm_dic_elem_iterator it;
		if (findVR(it, getGroup(), getElement())) {
			ret =  *reinterpret_cast<const uint16_t*>(it->second.vr.data());
		}
		// ������Ȃ������� '\0\0' ��Ԃ�
	}
	else
	{
		// DICOM�t�@�C���ɋL�^����Ă���VR��Ԃ� 
		if (_end_pos < _cur_pos + 4 + 4) {	// sizeof(tag) + sizeof(vr) = 4 + 4 
			return E_FAIL;
		}
		ret = getRawVR();
	}

	*vr = ret;
	return S_OK;
}

uint16_t CDicomDataElement::getFormatedVR(void) const
{
	uint16_t vr = 0;
	getVR(&vr);
	return _byteswap_ushort(vr);
}
