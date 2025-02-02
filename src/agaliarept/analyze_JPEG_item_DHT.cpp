#include "pch.h"
#include "analyze_JPEG_item_DHT.h"

#include "analyze_JPEG_util.h"

#include "jpeg_def.h"
#include "byteswap.h"

using namespace analyze_JPEG;




item_DHT::item_DHT(const container_JPEG* image, uint64_t offset, uint64_t size, item_type type)
	:item_Marker(image, offset, size, type)
{
}



item_DHT::~item_DHT()
{
}



HRESULT item_DHT::getColumnValue(uint32_t column, agaliaString** str) const
{
	if (column == column_value)
	{
		JPEGSEGMENT_DHT dht = {};
		auto hr = image->ReadData(&dht, getOffset(), sizeof(dht));
		if (FAILED(hr)) return hr;

		std::wstringstream temp;
		temp << L"Lh=" << agalia_byteswap(dht.Lh);
		*str = agaliaString::create(temp.str().c_str());

		return S_OK;
	}

	return __super::getColumnValue(column, str);
}



HRESULT item_DHT::getAdditionalInfoCount(uint32_t* row) const
{
	CHeapPtr<uint8_t> buf;
	rsize_t bufsize = 0;
	auto hr = ReadSegment(buf, &bufsize, image, this);
	if (FAILED(hr)) return hr;

	int count = 0;
	size_t cur = sizeof(JPEGSEGMENT_DHT);
	while (cur + 17 < bufsize)
	{
		JPEGSEGMENT_DHT_BODY* body = reinterpret_cast<JPEGSEGMENT_DHT_BODY*>(buf + cur);

		count += 2;
		cur += 17;
		for (int i = 0; i < _countof(JPEGSEGMENT_DHT_BODY::L); i++)
		{
			if (agalia_byteswap(body->L[i]) != 0)
				count++;
			cur += agalia_byteswap(body->L[i]);
		}
	}

	*row = count;
	return S_OK;
}



HRESULT item_DHT::getAdditionalInfoValue(uint32_t row, agaliaString** str) const
{
	CHeapPtr<uint8_t> buf;
	rsize_t bufsize = 0;
	auto hr = ReadSegment(buf, &bufsize, image, this);
	if (FAILED(hr)) return hr;

	std::wstringstream temp;

	uint32_t index = 0;
	size_t cur = sizeof(JPEGSEGMENT_DHT);
	while (cur + 17 < bufsize)
	{
		JPEGSEGMENT_DHT_BODY* body = reinterpret_cast<JPEGSEGMENT_DHT_BODY*>(buf + cur);
		if (row == index)
		{
			temp << L"Tc=" << body->Tc;
			temp << L", Th=" << body->Th;
			break;
		}
		index++;

		if (row == index)
		{
			for (int i = 0; i < _countof(JPEGSEGMENT_DHT_BODY::L); i++)
			{
				if (i != 0)
					temp << L", ";
				temp << L"L" << (i + 1) << L"=" << std::setw(2) << std::setfill(L'0') << std::hex << agalia_byteswap(body->L[i]);
			}
			break;
		}
		index++;

		bool found = false;
		size_t pos = 0;
		for (int i = 0; i < _countof(JPEGSEGMENT_DHT_BODY::L); i++)
		{
			if (agalia_byteswap(body->L[i]) == 0)
				continue;
			if (row == index)
			{
				temp << L"V[" << (i + 1) << L"]={";
				for (int j = 0; j < agalia_byteswap(body->L[i]); j++)
				{
					if (j != 0)
						temp << L",";
					temp << std::setw(2) << std::setfill(L'0') << std::hex << agalia_byteswap(body->V[pos++]);
				}
				temp << L"}";
				found = true;
				break;
			}
			pos += agalia_byteswap(body->L[i]);
			index++;
		}
		if (found)
			break;

		cur += 17 + pos;
	}

	*str = agaliaString::create(temp.str().c_str());
	return S_OK;
}



HRESULT item_DHT::getPropCount(uint32_t* count) const
{
	auto hr = __super::getPropCount(count);
	if (FAILED(hr)) return hr;

	CHeapPtr<uint8_t> buf;
	rsize_t bufsize = 0;
	hr = ReadSegment(buf, &bufsize, image, this);
	if (FAILED(hr)) return hr;

	int cnt = 1;
	size_t cur = sizeof(JPEGSEGMENT_DHT);
	while (cur + 17 < bufsize)
	{
		JPEGSEGMENT_DHT_BODY* body = reinterpret_cast<JPEGSEGMENT_DHT_BODY*>(buf + cur);

		cnt += 3;
		cur += 17;
		for (int i = 0; i < _countof(JPEGSEGMENT_DHT_BODY::L); i++)
		{
			if (agalia_byteswap(body->L[i]) != 0)
				cnt++;
			cur += agalia_byteswap(body->L[i]);
		}
	}

	*count += cnt;
	return S_OK;
}



HRESULT item_DHT::getPropName(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getPropName(index, str);
	if (SUCCEEDED(hr)) return hr;

	agaliaStringPtr s;
	if (index == 3) {
		s = agaliaString::create(L"Lh");
	}
	else {
		CHeapPtr<uint8_t> buf;
		rsize_t bufsize = 0;
		hr = ReadSegment(buf, &bufsize, image, this);
		if (FAILED(hr)) return hr;

		uint32_t cnt = 4;
		size_t cur = sizeof(JPEGSEGMENT_DHT);
		while (cur + 17 < bufsize)
		{
			JPEGSEGMENT_DHT_BODY* body = reinterpret_cast<JPEGSEGMENT_DHT_BODY*>(buf + cur);

			if (cnt == index)
			{
				s = agaliaString::create(L"Tc");
				break;
			}
			else if (index == cnt + 1)
			{
				s = agaliaString::create(L"Th");
				break;
			}
			else if (index == cnt + 2)
			{
				s = agaliaString::create(L"Li");
				break;
			}

			cnt += 3;
			cur += 17;
			for (int i = 0; i < _countof(JPEGSEGMENT_DHT_BODY::L); i++)
			{
				if (agalia_byteswap(body->L[i]) != 0)
				{
					if (cnt == index)
					{
						std::wstringstream temp;
						temp << L"V";
						if ((i + 1) >= 10) {
							temp << int_to_subscript_wchar((i + 1) / 10);
						}
						temp << int_to_subscript_wchar((i + 1) % 10);
						s = agaliaString::create(temp.str().c_str());
						break;
					}
					cnt++;
				}
				cur += agalia_byteswap(body->L[i]);
			}

			if (s) {
				break;
			}
		}
	}

	if (!s) return E_FAIL;
	*str = s.detach();

	return S_OK;
}



HRESULT item_DHT::getPropValue(uint32_t index, agaliaString** str) const
{
	auto hr = __super::getPropValue(index, str);
	if (SUCCEEDED(hr)) return hr;

	CHeapPtr<uint8_t> buf;
	rsize_t bufsize = 0;
	hr = ReadSegment(buf, &bufsize, image, this);
	if (FAILED(hr)) return hr;

	JPEGSEGMENT_DHT* dht = reinterpret_cast<JPEGSEGMENT_DHT*>(buf.m_pData);

	std::wstringstream temp;
	if (index == 3) {
		temp << agalia_byteswap(dht->Lh);
	}
	else {

		uint32_t cnt = 4;
		size_t cur = sizeof(JPEGSEGMENT_DHT);
		while (cur + 17 < bufsize)
		{
			JPEGSEGMENT_DHT_BODY* body = reinterpret_cast<JPEGSEGMENT_DHT_BODY*>(buf + cur);

			if (cnt == index)
			{
				temp << body->Tc;
				break;
			}
			else if (index == cnt + 1)
			{
				temp << body->Th;
				break;
			}
			else if (index == cnt + 2)
			{
				for (int i = 0; i < _countof(JPEGSEGMENT_DHT_BODY::L); i++)
				{
					if (i != 0)
						temp << L",";
					temp << std::setw(2) << std::setfill(L'0') << std::hex << agalia_byteswap(body->L[i]);
				}
				break;
			}

			cnt += 3;
			bool found = false;
			size_t pos = 0;
			for (int i = 0; i < _countof(JPEGSEGMENT_DHT_BODY::L); i++)
			{
				if (agalia_byteswap(body->L[i]) == 0)
					continue;

				if (cnt == index)
				{
					for (int j = 0; j < agalia_byteswap(body->L[i]); j++)
					{
						if (j != 0)
							temp << L",";
						temp << std::setw(2) << std::setfill(L'0') << std::hex << agalia_byteswap(body->V[pos++]);
					}
					found = true;
					break;
				}
				pos += agalia_byteswap(body->L[i]);
				cnt++;
			}
			if (found)
				break;
			cur += 17 + pos;
		}
	}

	*str = agaliaString::create(temp.str().c_str());

	return S_OK;
}
