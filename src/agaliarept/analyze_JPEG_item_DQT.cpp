#include "pch.h"
#include "analyze_JPEG_item_DQT.h"

#include "analyze_JPEG_util.h"

#include "jpeg_def.h"
#include "byteswap.h"

using namespace analyze_JPEG;




item_DQT::item_DQT(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type)
	:item_Marker(image, offset, size, type)
{

}



item_DQT::~item_DQT()
{

}



HRESULT item_DQT::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_value)
	{
		JPEGSEGMENT_DQT dqt = {};
		auto hr = image->ReadData(&dqt, getOffset(), sizeof(dqt));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"Lq=" << agalia_byteswap(dqt.Lq);
		*str = agaliaString::create(temp.str().c_str());

		return S_OK;
	}

	return __super::getColumnValue(column, str);
}



HRESULT item_DQT::getAdditionalInfoCount(uint32_t* row) const
{
	CHeapPtr<uint8_t> buf;
	rsize_t bufsize = 0;
	auto hr = ReadSegment(buf, &bufsize, image, this);
	if (FAILED(hr)) return hr;

	JPEGSEGMENT_DQT* dqt = reinterpret_cast<JPEGSEGMENT_DQT*>(buf.m_pData);

	uint32_t count = 0;

	size_t pos = sizeof(JPEGSEGMENT_DQT);
	while (pos < sizeof(JPEGSEGMENT) + agalia_byteswap(dqt->Lq))
	{
		JPEGSEGMENT_DQT_BODY* body = reinterpret_cast<JPEGSEGMENT_DQT_BODY*>(buf.m_pData + pos);
		pos += 65 + 64 * static_cast<size_t>(body->Pq);
		count += 2;
	}
	*row = count;

	return S_OK;
}



HRESULT item_DQT::getAdditionalInfoValue(uint32_t row, agaliaString** str) const
{
	CHeapPtr<uint8_t> buf;
	rsize_t bufsize = 0;
	auto hr = ReadSegment(buf, &bufsize, image, this);
	if (FAILED(hr)) return hr;

	size_t pos = sizeof(JPEGSEGMENT_DQT);
	for (uint32_t i = 1; i <= row / 2; i++)
	{
		JPEGSEGMENT_DQT_BODY* body = reinterpret_cast<JPEGSEGMENT_DQT_BODY*>(buf.m_pData + pos);
		pos += 1;	// Pq:4 + Tq:4 
		switch (body->Pq)
		{
		case 0:
			pos += sizeof(uint8_t) * 64;
			break;
		case 1:
			pos += sizeof(uint16_t) * 64;
			break;
		}
	}

	if (bufsize < pos + 1) {
		return E_FAIL;
	}
	JPEGSEGMENT_DQT_BODY* body = reinterpret_cast<JPEGSEGMENT_DQT_BODY*>(buf.m_pData + pos);
	if (bufsize < pos + 1 + static_cast<size_t>(body->Pq ? 2 : 1) * 64) {
		return E_FAIL;
	}

	std::wstringstream temp;
	if (row % 2 == 0)
	{
		temp << L"Pq=" << body->Pq;
		temp << L", Tq=" << body->Tq;
	}
	else
	{
		temp << L"Q={";
		int i;
		if (body->Pq == 0)
		{
			temp << body->Q8[0];
			for (i = 1; i < 64; i++)
				temp << L"," << body->Q8[i];
		}
		else if (body->Pq == 1)
		{
			temp << body->Q16[0];
			for (i = 1; i < 64; i++)
				temp << L"," << body->Q16[i];
		}
		temp << L"}";
	}
	*str = agaliaString::create(temp.str().c_str());

	return S_OK;
}



HRESULT item_DQT::getPropCount(uint32_t* count) const
{
	auto hr = __super::getPropCount(count);
	if (FAILED(hr)) return hr;

	CHeapPtr<uint8_t> buf;
	rsize_t bufsize = 0;
	hr = ReadSegment(buf, &bufsize, image, this);
	if (FAILED(hr)) return hr;

	JPEGSEGMENT_DQT* dqt = reinterpret_cast<JPEGSEGMENT_DQT*>(buf.m_pData);

	*count += 1;	// Lq

	size_t pos = sizeof(JPEGSEGMENT_DQT);
	while (pos < sizeof(JPEGSEGMENT) + agalia_byteswap(dqt->Lq))
	{
		JPEGSEGMENT_DQT_BODY* body = reinterpret_cast<JPEGSEGMENT_DQT_BODY*>(buf.m_pData + pos);
		pos += 65 + 64 * static_cast<size_t>(body->Pq);
		*count += 3;	// Pq, Tq, Q
	}

	return S_OK;
}



HRESULT item_DQT::getPropName(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getPropName(index, str);
	if (SUCCEEDED(hr)) return hr;

	const wchar_t* name = nullptr;
	if (index == 3)
	{
		name = L"Lq";
	}
	else
	{
		switch ((index - 4) % 3)
		{
		case 0: name = L"Pq";	break;
		case 1: name = L"Tq";	break;
		case 2: name = L"Q";	break;
		}
	}

	if (name == nullptr) return E_INVALIDARG;
	*str = agaliaString::create(name);
	return S_OK;
}



HRESULT item_DQT::getPropValue(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	CHeapPtr<uint8_t> buf;
	rsize_t bufsize = 0;
	hr = ReadSegment(buf, &bufsize, image, this);
	if (FAILED(hr)) return hr;

	JPEGSEGMENT_DQT* dqt = reinterpret_cast<JPEGSEGMENT_DQT*>(buf.m_pData);

	std::wstringstream temp;
	if (index == 3)
	{
		temp << agalia_byteswap(dqt->Lq);
	}
	else
	{
		JPEGSEGMENT_DQT_BODY* body = reinterpret_cast<JPEGSEGMENT_DQT_BODY*>(buf.m_pData + sizeof(JPEGSEGMENT_DQT));
		size_t pos = sizeof(JPEGSEGMENT_DQT);
		for (uint32_t i = 0; i < (index - 4) / 3; i++)
		{
			pos += 65 + 64 * static_cast<size_t>(body->Pq);
			if (agalia_byteswap(dqt->Lq) <= pos) {
				return E_FAIL;
			}
			body = reinterpret_cast<JPEGSEGMENT_DQT_BODY*>(buf.m_pData + pos);
		}

		switch ((index - 4) % 3)
		{
		case 0:
			temp << body->Pq;
			break;
		case 1:
			temp << body->Tq;
			break;
		case 2:
			int i;
			if (body->Pq == 0)
			{
				temp << body->Q8[0];
				for (i = 1; i < 64; i++)
					temp << L"," << body->Q8[i];
			}
			else if (body->Pq == 1)
			{
				temp << body->Q16[0];
				for (i = 1; i < 64; i++)
					temp << L"," << body->Q16[i];
			}
			break;
		}
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}
