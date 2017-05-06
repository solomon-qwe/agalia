#pragma once

#include <stdint.h>	// for uint**_t 

enum TransferSyntax
{
	ImplicitVR_LittleEndian,
	ExplicitVR_LittleEndian,
	DeflatedExplicitVR_LittleEndian,
	ExplicitVR_BigEndian
};

class CDicomDataElement
{
public:
	CDicomDataElement(const uint8_t* p, uint64_t cur_pos, uint64_t end_pos, TransferSyntax syntax, uint32_t codepage)
	{
		_p = reinterpret_cast<const unsigned char*>(p);
		_cur_pos = cur_pos;
		_end_pos = end_pos;
		_transferSyntax = syntax;
		_codepage = codepage;
	}
	virtual ~CDicomDataElement() {}

	uint32_t getTag(void) const { return *reinterpret_cast<const uint32_t*>(_p + _cur_pos); }
	uint16_t getGroup(void) const { return *reinterpret_cast<const uint16_t*>(_p + _cur_pos); }
	uint16_t getElement(void) const { return *reinterpret_cast<const uint16_t*>(_p + _cur_pos + 2); }

	// DICOMファイルに記録されているVRを返す 
	uint16_t getRawVR(void) const { return *reinterpret_cast<const uint16_t*>(_p + _cur_pos + 4); }

	// DataDictionary解釈などをしてVRを返す 
	HRESULT getVR(uint16_t* vr) const;

	// getVR() の値を Little Endian にして返す 
	uint16_t getFormatedVR(void) const;

	TransferSyntax getSyntax(void) const
	{
		switch (getGroup())
		{
		case 0x0002:
		case 0xFFFE:
			return ExplicitVR_LittleEndian;
		}
		return _transferSyntax;
	}

	uint32_t getCodepage(void) const
	{
		return _codepage;
	}

	const uint8_t* getDataAddr(void) const
	{
		uint32_t length = 0;
		getLength(&length);
		if (_cur_pos + getDataOffset() + length <= _end_pos) {
			return _p + _cur_pos + getDataOffset();
		}
		return nullptr;
	}

	HRESULT getLength(uint32_t* length) const
	{
		int offset = 0;
		int data_size = 0;

		if (getSyntax() == ImplicitVR_LittleEndian
			|| getGroup() == 0xFFFE)
		{
			offset = 4;
			data_size = 4;
		}
		else
		{
			switch (getFormatedVR())
			{
			case 'OB':
			case 'OW':
			case 'OF':
			case 'SQ':
			case 'UT':
			case 'OR':
			case 'UN':
				offset = 4 + 4;
				data_size = 4;
				break;

			default:
				offset = 4 + 2;
				data_size = 2;
			}
		}

		if (_end_pos < _cur_pos + offset + data_size) {
			return E_FAIL;
		}

		switch (data_size)
		{
		case 4:
			*length = *reinterpret_cast<const uint32_t*>(_p + _cur_pos + offset);
			break;
		case 2:
			*length = *reinterpret_cast<const uint16_t*>(_p + _cur_pos + offset);
			break;
		default:
			return E_FAIL;
		}
		return S_OK;
	}

	uint64_t getDataOffset(void) const
	{
		uint64_t offset = 0;

		if (getSyntax() == ImplicitVR_LittleEndian
			|| getGroup() == 0xFFFE)
		{
			offset = 4 + 4;
		}
		else
		{
			switch (getFormatedVR())
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
		return offset;
	}

	void set_transfer_syntax(TransferSyntax syntax)
	{
		_transferSyntax = syntax;
	}

	void set_codepage(uint32_t codepage)
	{
		_codepage = codepage;
	}

protected:
	const uint8_t* _p;
	uint64_t _cur_pos;
	uint64_t _end_pos;
	TransferSyntax _transferSyntax;
	uint32_t _codepage;
};

