#pragma once

#include "container_DCM_Impl.h"

#include "byteswap.h"

#include "dcm_common.h"

namespace analyze_DCM
{
	HRESULT format_dicm_value(std::wstringstream& dst, const container_DCM_Impl* image, uint64_t value_offset, uint32_t value_size, uint16_t vr, const std::list<int>& codepage);


	class dicom_element
	{
	public:
		dicom_element(const container_DCM_Impl* image, uint64_t offset)
		{
			this->image = image;
			data_offset = offset;
		}

		virtual ~dicom_element()
		{
		}

		HRESULT getGroup(uint16_t* group) const
		{
			uint16_t temp = 0;
			auto hr = image->ReadData(&temp, data_offset, sizeof(temp));
			if (FAILED(hr)) return hr;
			*group = temp;
			return S_OK;
		}

		HRESULT getElement(uint16_t* element) const
		{
			uint16_t temp = 0;
			auto hr = image->ReadData(&temp, data_offset + 2, sizeof(temp));
			if (FAILED(hr)) return hr;
			*element = temp;
			return S_OK;
		}

		HRESULT getVR(uint16_t* vr) const
		{
			uint16_t temp = 0;
			auto hr = image->ReadData(&temp, data_offset + 4, sizeof(temp));
			if (FAILED(hr)) return hr;
			*vr = temp;
			return S_OK;
		}

		HRESULT getValueOffset(uint64_t* value_offset) const
		{
			uint64_t offset = 0;

			TransferSyntax syntax = ExplicitVR_LittleEndian;
			auto hr = getSyntax(&syntax);
			if (FAILED(hr)) return hr;

			uint16_t group = 0;
			hr = getGroup(&group);
			if (FAILED(hr)) return hr;

			if (syntax == ImplicitVR_LittleEndian || group == 0xFFFE)
			{
				offset = 4 + 4;
			}
			else
			{
				uint16_t vr = 0;
				hr = getFormatedVR(&vr);
				if (FAILED(hr)) return hr;
				switch (agalia_byteswap(vr))
				{
				case 'OB':
				case 'OW':
				case 'OF':
				case 'SQ':
				case 'UT':
				case 'OR':
				case 'UN':
					offset = 4 + 4 + 4;	// tag(4) + vr(4) + length(4) 
					break;
				default:
					offset = 4 + 4;		// tag(4) + vr(2) + length(2) or tag(4) + length(4)
				}
			}
			*value_offset = offset;
			return S_OK;
		}

		HRESULT getValueLength(uint32_t* value_size) const
		{
			uint64_t length_offset = 0;
			uint64_t length_size = 0;

			TransferSyntax syntax = ExplicitVR_LittleEndian;
			auto hr = getSyntax(&syntax);
			if (FAILED(hr)) return hr;

			uint16_t group = 0;
			hr = getGroup(&group);
			if (FAILED(hr)) return hr;

			if (syntax == ImplicitVR_LittleEndian || group == 0xFFFE)
			{
				length_offset = 4;
				length_size = 4;
			}
			else
			{
				uint16_t vr = 0;
				hr = getFormatedVR(&vr);
				if (FAILED(hr)) return hr;
				switch (agalia_byteswap(vr))
				{
				case 'OB':
				case 'OW':
				case 'OF':
				case 'SQ':
				case 'UT':
				case 'OR':
				case 'UN':
					length_offset = 4 + 4;
					length_size = 4;
					break;
				default:
					length_offset = 4 + 2;
					length_size = 2;
				}
			}

			if (length_size == 4)
			{
				uint32_t temp = 0;
				hr = image->ReadData(&temp, data_offset + length_offset, sizeof(temp));
				if (FAILED(hr)) return hr;
				*value_size = temp;
				return S_OK;
			}
			else if (length_size == 2)
			{
				uint16_t temp = 0;
				hr = image->ReadData(&temp, data_offset + length_offset, sizeof(temp));
				if (FAILED(hr)) return hr;
				*value_size = temp;
				return S_OK;
			}
			return E_FAIL;
		}

		HRESULT getElementSize(uint64_t* size) const
		{
			uint64_t value_offset = 0;
			if (FAILED(getValueOffset(&value_offset))) return E_FAIL;

			uint32_t value_size = 0;
			if (FAILED(getValueLength(&value_size))) return E_FAIL;
			if (value_size == 0xFFFFFFFF) {
				value_size = 0;
			}

			*size = value_offset + value_size;
			return S_OK;
		}

		HRESULT getSyntax(TransferSyntax* syntax) const
		{
			uint16_t group = 0;
			auto hr = getGroup(&group);
			if (FAILED(hr)) return hr;

			switch (group)
			{
			case 0x0002:
			case 0xFFFE:
				*syntax = ExplicitVR_LittleEndian;
				break;
			default:
				*syntax = image->_transferSyntax;
			}
			return S_OK;
		}

		HRESULT getFormatedVR(uint16_t* vr) const
		{
			uint16_t group = 0;
			auto hr = getGroup(&group);
			if (FAILED(hr)) return hr;
			if (group == 0xFFFE) {
				*vr = 0;
				return S_OK;
			}

			TransferSyntax syntax = ExplicitVR_LittleEndian;
			hr = getSyntax(&syntax);
			if (FAILED(hr)) return hr;
			if (syntax == ImplicitVR_LittleEndian)
			{
				// 暗黙的なら DataDictionary を参照する 
				uint16_t element = 0;
				hr = getElement(&element);
				if (FAILED(hr)) return hr;
				dcm_dic_elem_iterator it;
				if (findVR(it, group, element)) {
					*vr = *reinterpret_cast<const uint16_t*>(it->second.vr.data());
				}
				else {
					*vr = 0;
				}
				return S_OK;
			}

			uint16_t rawVR = 0;
			hr = getVR(&rawVR);
			if (FAILED(hr)) return hr;

			*vr = rawVR;
			return S_OK;
		}

		HRESULT getPreferredVR(uint16_t* vr) const
		{
			if (agalia_pref_dicom_vr)
			{
				// 強制的に DataDictionary を参照させる 
				uint16_t group = 0;
				uint16_t element = 0;
				if (SUCCEEDED(getGroup(&group)) &&
					SUCCEEDED(getElement(&element)))
				{
					dcm_dic_elem_iterator it;
					if (findVR(it, group, element))
					{
						*vr = *reinterpret_cast<const uint16_t*>(it->second.vr.data());
						return S_OK;
					}
				}
			}
			return E_FAIL;
		}

		const container_DCM_Impl* image = nullptr;
		uint64_t data_offset = 0;
	};
}
