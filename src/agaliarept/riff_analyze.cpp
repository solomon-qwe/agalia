#include "stdafx.h"
#include "callback.h"
#include "util.h"

#include <mmiscapi.h>
#include <Aviriff.h>
#include <mmreg.h>
#include <atlbase.h>

void print_riff(int subItem, const uint8_t* base, uint64_t offset, uint32_t size, callback_cls* callback, FOURCC* fourCC)
{
	if (size < sizeof(uint32_t)) return;

	char sz[5] = {};
	*fourCC = *reinterpret_cast<const uint32_t*>(base + offset);
	*reinterpret_cast<uint32_t*>(sz) = *fourCC;
	callback->print(subItem, sz);
}

void print_riff_avih(int subItem, const uint8_t* base, uint64_t offset, uint32_t size, callback_cls* callback)
{
	if (size < sizeof(AVIMAINHEADER) - 8) return;

	char fcc[5] = {};
	wchar_t temp[128] = {};
	std::wstring str;

	const AVIMAINHEADER* avih = reinterpret_cast<const AVIMAINHEADER*>(base + offset - 8);

	str = L"fcc:";
	*reinterpret_cast<uint32_t*>(fcc) = avih->fcc;
	swprintf_s(temp, L"%S", fcc);
	str += temp;

	str += L", cb:";
	swprintf_s(temp, L"%u", avih->cb);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwMicroSecPerFrame:";
	swprintf_s(temp, L"%u", avih->dwMicroSecPerFrame);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwMaxBytesPerSec:";
	swprintf_s(temp, L"%u", avih->dwMaxBytesPerSec);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwPaddingGranularity:";
	swprintf_s(temp, L"%u", avih->dwPaddingGranularity);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwFlags:";
	swprintf_s(temp, L"%u", avih->dwFlags);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwTotalFrames:";
	swprintf_s(temp, L"%u", avih->dwTotalFrames);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwInitialFrames:";
	swprintf_s(temp, L"%u", avih->dwInitialFrames);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwStreams:";
	swprintf_s(temp, L"%u", avih->dwStreams);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwSuggestedBufferSize:";
	swprintf_s(temp, L"%u", avih->dwSuggestedBufferSize);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwWidth:";
	swprintf_s(temp, L"%u", avih->dwWidth);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwHeight:";
	swprintf_s(temp, L"%u", avih->dwHeight);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwReserved[0]:";
	swprintf_s(temp, L"%u", avih->dwReserved[0]);
	str += temp;

	str += L", dwReserved[1]:";
	swprintf_s(temp, L"%u", avih->dwReserved[1]);
	str += temp;

	str += L", dwReserved[2]:";
	swprintf_s(temp, L"%u", avih->dwReserved[2]);
	str += temp;

	str += L", dwReserved[3]:";
	swprintf_s(temp, L"%u", avih->dwReserved[3]);
	str += temp;
	callback->print(subItem, str);
}

void print_riff_strh(int subItem, const uint8_t* base, uint64_t offset, uint32_t size, callback_cls* callback, FOURCC* fccType)
{
	if (size < sizeof(AVISTREAMHEADER) - 8) return;

	char fcc[5] = {};
	wchar_t temp[128] = {};
	std::wstring str;

	const AVISTREAMHEADER* strh = reinterpret_cast<const AVISTREAMHEADER*>(base + offset - 8);
	*fccType = strh->fccType;

	str = L"fcc:";
	*reinterpret_cast<uint32_t*>(fcc) = strh->fcc;
	swprintf_s(temp, L"%S", fcc);
	str += temp;

	str += L", cb:";
	swprintf_s(temp, L"%u", strh->cb);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"fccType:";
	*reinterpret_cast<uint32_t*>(fcc) = strh->fccType;
	swprintf_s(temp, L"%S", fcc);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"fccHandler:";
	*reinterpret_cast<uint32_t*>(fcc) = strh->fccHandler;
	swprintf_s(temp, L"%S", fcc);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwFlags:";
	swprintf_s(temp, L"%u", strh->dwFlags);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"wPriority:";
	swprintf_s(temp, L"%u", strh->wPriority);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"wLanguage:";
	swprintf_s(temp, L"%u", strh->wLanguage);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwInitialFrames:";
	swprintf_s(temp, L"%u", strh->dwInitialFrames);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwScale:";
	swprintf_s(temp, L"%u", strh->dwScale);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwRate:";
	swprintf_s(temp, L"%u", strh->dwRate);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwStart:";
	swprintf_s(temp, L"%u", strh->dwStart);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwLength:";
	swprintf_s(temp, L"%u", strh->dwLength);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwSuggestedBufferSize:";
	swprintf_s(temp, L"%u", strh->dwSuggestedBufferSize);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwQuality:";
	swprintf_s(temp, L"%u", strh->dwQuality);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"dwSampleSize:";
	swprintf_s(temp, L"%u", strh->dwSampleSize);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"rcFrame.left:";
	swprintf_s(temp, L"%d", strh->rcFrame.left);
	str += temp;

	str += L", rcFrame.top:";
	swprintf_s(temp, L"%d", strh->rcFrame.top);
	str += temp;

	str += L", rcFrame.right:";
	swprintf_s(temp, L"%d", strh->rcFrame.right);
	str += temp;

	str += L", rcFrame.bottom:";
	swprintf_s(temp, L"%d", strh->rcFrame.bottom);
	str += temp;
	callback->print(subItem, str);
}

void print_riff_strf_vids(int subItem, const uint8_t* base_addr, uint64_t offset, uint32_t size, callback_cls* callback)
{
	if (size < sizeof(BITMAPINFOHEADER)) return;

	char fcc[5] = {};
	wchar_t temp[128] = {};
	std::wstring str;

	const BITMAPINFOHEADER* vids = reinterpret_cast<const BITMAPINFOHEADER*>(base_addr + offset);

	str = L"biSize:";
	swprintf_s(temp, L"%u", vids->biSize);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"biWidth:";
	swprintf_s(temp, L"%d", vids->biWidth);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"biHeight:";
	swprintf_s(temp, L"%d", vids->biHeight);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"biPlanes:";
	swprintf_s(temp, L"%u", vids->biPlanes);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"biBitCount:";
	swprintf_s(temp, L"%u", vids->biBitCount);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"biCompression:";
	switch (vids->biCompression)
	{
	case BI_RGB:
		wcscpy_s(temp, L"BI_RGB");
		break;
	case BI_BITFIELDS:
		wcscpy_s(temp, L"BI_BITFIELDS");
		break;
	default:
		*reinterpret_cast<uint32_t*>(fcc) = vids->biCompression;
		swprintf_s(temp, L"'%S'", fcc);
	}
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"biSizeImage:";
	swprintf_s(temp, L"%u", vids->biSizeImage);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"biXPelsPerMeter:";
	swprintf_s(temp, L"%d", vids->biXPelsPerMeter);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"biYPelsPerMeter:";
	swprintf_s(temp, L"%d", vids->biYPelsPerMeter);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"biClrUsed:";
	swprintf_s(temp, L"%u", vids->biClrUsed);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"biClrImportant:";
	swprintf_s(temp, L"%u", vids->biClrImportant);
	str += temp;
	callback->print(subItem, str);
}

void print_riff_strf_auds(int subItem, const uint8_t* base_addr, uint64_t offset, uint32_t size, callback_cls* callback)
{
	if (size < sizeof(WAVEFORMATEX)) return;

	wchar_t temp[128] = {};
	std::wstring str;

	const WAVEFORMATEX* auds = reinterpret_cast<const WAVEFORMATEX*>(base_addr + offset);

	str = L"wFormatTag:";
	swprintf_s(temp, L"%u", auds->wFormatTag);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"nChannels:";
	swprintf_s(temp, L"%u", auds->nChannels);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"nSamplesPerSec:";
	swprintf_s(temp, L"%u", auds->nSamplesPerSec);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"nAvgBytesPerSec:";
	swprintf_s(temp, L"%u", auds->nAvgBytesPerSec);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"nBlockAlign:";
	swprintf_s(temp, L"%u", auds->nBlockAlign);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"wBitsPerSample:";
	swprintf_s(temp, L"%u", auds->wBitsPerSample);
	str += temp;
	callback->print(subItem, str);

	callback->next();
	str = L"cbSize:";
	swprintf_s(temp, L"%u", auds->cbSize);
	str += temp;
	callback->print(subItem, str);
}

void print_riff_ascii(int subItem, const uint8_t* base_addr, uint64_t offset, uint32_t size, callback_cls* callback)
{
	CHeapPtr<char> p;
	p.Allocate(size + 1);
	memcpy(p, base_addr + offset, size);
	p[size] = '\0';

	size_t bufSize = 1 + size + 1 + 1;
	CHeapPtr<wchar_t> q;
	q.Allocate(bufSize);
	q[bufSize - 1] = 0;

	swprintf_s(q, bufSize, L"\"%S\"", p.m_pData);
	callback->print(subItem, q);
}

void print_riff_idx1(int subItem, const uint8_t* base_addr, uint64_t offset, uint32_t size, callback_cls* callback)
{
	if (size < sizeof(AVIOLDINDEX) - 8) return;

	char fcc[5] = {};
	wchar_t temp[128] = {};
	std::wstring str;

	const AVIOLDINDEX* idx1 = reinterpret_cast<const AVIOLDINDEX*>(base_addr + offset - 8);

	str = L"fcc:";
	*reinterpret_cast<uint32_t*>(fcc) = idx1->fcc;
	swprintf_s(temp, L"%S", fcc);
	str += temp;

	str += L", cb:";
	swprintf_s(temp, L"%u", idx1->cb);
	str += temp;
	callback->print(subItem, str);

	for (size_t i = 0; i < idx1->cb / sizeof(AVIOLDINDEX::_avioldindex_entry); i++)
	{
		callback->next();
		*reinterpret_cast<uint32_t*>(fcc) = idx1->aIndex[i].dwChunkId;
		swprintf_s(temp, L"[%zd].dwChunkId=%S", i, fcc);
		str = temp;

		swprintf_s(temp, L", dwFlags=%u", idx1->aIndex[i].dwFlags);
		str += temp;

		swprintf_s(temp, L", dwOffset=%u", idx1->aIndex[i].dwOffset);
		str += temp;

		swprintf_s(temp, L", dwSize=%u", idx1->aIndex[i].dwSize);
		str += temp;

		callback->print(subItem, str);
	}
}

void print_riff_dmlh(int subItem, const uint8_t* base_addr, uint64_t offset, uint32_t size, callback_cls* callback)
{
	if (size < sizeof(AVIEXTHEADER) - 8) return;

	char fcc[5] = {};
	wchar_t temp[128] = {};
	std::wstring str;

	const AVIEXTHEADER* dmlh = reinterpret_cast<const AVIEXTHEADER*>(base_addr + offset - 8);

	str += L"fcc:";
	*reinterpret_cast<uint32_t*>(fcc) = dmlh->fcc;
	swprintf_s(temp, L"%S", fcc);
	str += temp;

	str += L", cb:";
	swprintf_s(temp, L"%u", dmlh->cb);
	str += temp;

	str += L", dwGrandFrames:";
	swprintf_s(temp, L"%u", dmlh->dwGrandFrames);
	str += temp;

	for (int i = 0; i < _countof(AVIEXTHEADER::dwFuture); i++)
	{
		swprintf_s(temp, L", dwFuture[%d]:%u", i, dmlh->dwFuture[i]);
		str += temp;
	}

	callback->print(subItem, str);
}

void print_riff_indx(int subItem, const uint8_t* base_addr, uint64_t offset, uint32_t size, callback_cls* callback)
{
	if (size < sizeof(AVISUPERINDEX) - 8) return;

	char fcc[5] = {};
	wchar_t temp[128] = {};
	std::wstring str;

	const AVISUPERINDEX* indx = reinterpret_cast<const AVISUPERINDEX*>(base_addr + offset - 8);

	str += L"fcc:";
	*reinterpret_cast<uint32_t*>(fcc) = indx->fcc;
	swprintf_s(temp, L"%S", fcc);
	str += temp;

	str += L", cb:";
	swprintf_s(temp, L"%u", indx->cb);
	str += temp;

	str += L", wLongsPerEntry:";
	swprintf_s(temp, L"%u", indx->wLongsPerEntry);
	str += temp;

	str += L", bIndexSubType:";
	swprintf_s(temp, L"%u", indx->bIndexSubType);
	str += temp;

	str += L", bIndexType:";
	swprintf_s(temp, L"%u", indx->bIndexType);
	str += temp;

	str += L", nEntriesInUse:";
	swprintf_s(temp, L"%u", indx->nEntriesInUse);
	str += temp;

	str += L", dwChunkId:";
	*reinterpret_cast<uint32_t*>(fcc) = indx->dwChunkId;
	swprintf_s(temp, L"%S", fcc);
	str += temp;

	str += L", dwReserved[0]:";
	swprintf_s(temp, L"%u", indx->dwReserved[0]);
	str += temp;

	str += L", dwReserved[1]:";
	swprintf_s(temp, L"%u", indx->dwReserved[1]);
	str += temp;

	str += L", dwReserved[2]:";
	swprintf_s(temp, L"%u", indx->dwReserved[2]);
	str += temp;

	for (int i = 0; i < _countof(AVISUPERINDEX::aIndex); i++)
	{
		swprintf_s(temp, L", qwOffset[%d]:%llu", i, indx->aIndex[i].qwOffset);
		str += temp;

		swprintf_s(temp, L", dwSize[%d]:%u", i, indx->aIndex[i].dwSize);
		str += temp;

		swprintf_s(temp, L", dwDuration[%d]:%u", i, indx->aIndex[i].dwDuration);
		str += temp;
	}

	callback->print(subItem, str);
}

void print_riff_ix00(int subItem, const uint8_t* base_addr, uint64_t offset, uint32_t size, callback_cls* callback)
{
	if (size < sizeof(AVIFIELDINDEX) - 8) return;

	char fcc[5] = {};
	wchar_t temp[128] = {};
	std::wstring str;

	const AVIFIELDINDEX* ix00 = reinterpret_cast<const AVIFIELDINDEX*>(base_addr + offset - 8);

	str += L"fcc:";
	*reinterpret_cast<uint32_t*>(fcc) = ix00->fcc;
	swprintf_s(temp, L"%S", fcc);
	str += temp;

	str += L", cb:";
	swprintf_s(temp, L"%u", ix00->cb);
	str += temp;

	str += L", wLongsPerEntry:";
	swprintf_s(temp, L"%u", ix00->wLongsPerEntry);
	str += temp;

	str += L", bIndexSubType:";
	swprintf_s(temp, L"%u", ix00->bIndexSubType);
	str += temp;

	str += L", bIndexType:";
	swprintf_s(temp, L"%u", ix00->bIndexType);
	str += temp;

	str += L", nEntriesInUse:";
	swprintf_s(temp, L"%u", ix00->nEntriesInUse);
	str += temp;

	str += L", dwChunkId:";
	*reinterpret_cast<uint32_t*>(fcc) = ix00->dwChunkId;
	swprintf_s(temp, L"%S", fcc);
	str += temp;

	str += L", qwBaseOffset:";
	swprintf_s(temp, L"%llu", ix00->qwBaseOffset);
	str += temp;

	str += L", dwReserved3:";
	swprintf_s(temp, L"%u", ix00->dwReserved3);
	str += temp;

	str += L", ...";

	callback->print(subItem, str);
}

void putDataAsByteArray(int subItem, const uint8_t* base_addr, uint64_t offset, uint64_t size, callback_cls* callback, const agalia::config* config)
{
	const uint8_t* pVal = base_addr + offset;

	std::wstring str;
	const size_t limit = config->byte_stream_limit_length;
	str.reserve(limit * 3 + 8);

	str += L"[";
	if (size != 0)
	{
		wchar_t temp[16];
		fast_itow16(pVal[0], temp);	// 1
		str += temp;

		for (unsigned int i = 1; i < min(limit, size); i++)
		{
			temp[0] = L',';
			fast_itow16(pVal[i], temp + 1);	// 2,...
			str += temp;
		}

		if (limit < size) {
			str += L",...";
		}
	}
	str += L"]";

	callback->print(subItem, str);
}


void parseRiffDirectory(const uint8_t* base_addr, uint64_t offset, uint64_t data_size, uint32_t parent, callback_cls* callback, const agalia::config* config)
{
	uint32_t chunkSize = 0;
	FOURCC fccType = 0;

	uint64_t pos = offset;
	do
	{
		// offset 
		int subItem = 0;
		{
			wchar_t sz[16] = {};
			swprintf_s(sz, L"%.8I64X", pos);
			callback->print(subItem, sz);
		}

		// parent  
		subItem++;
		{
			char sz[5] = {};
			*reinterpret_cast<uint32_t*>(sz) = parent;
			callback->print(subItem, sz);
		}

		if (data_size < sizeof(uint32_t)) break;
		uint32_t chunkID = *reinterpret_cast<const uint32_t*>(base_addr + pos);
		pos += sizeof(chunkID);

		// ID 
		subItem++;
		{
			char sz[5] = {};
			*reinterpret_cast<uint32_t*>(sz) = chunkID;
			wchar_t temp[64] = {};
			swprintf_s(temp, L"%S (%.4X)", sz, chunkID);
			callback->print(subItem, temp);
		}

		if (data_size < sizeof(chunkID) + sizeof(uint32_t)) break;
		chunkSize = *reinterpret_cast<const uint32_t*>(base_addr + pos);
		pos += sizeof(chunkSize);

		// Size 
		subItem++;
		{
			wchar_t temp[16] = {};
			_ultow_s(chunkSize, temp, 10);
			callback->print(subItem, temp);
		}

		uint32_t read_size = static_cast<uint32_t>(min(data_size - 8, chunkSize));
		callback->set_extra_data(pos, read_size);

		// data 
		subItem++;
		putDataAsByteArray(subItem, base_addr, pos, read_size, callback, config);

		// Value 
		subItem++;
		FOURCC fourCC = 0;
		switch (_byteswap_ulong(chunkID))
		{
		case 'RIFF':
		case 'LIST':
			print_riff(subItem, base_addr, pos, read_size, callback, &fourCC);
			break;

		case 'avih':
			print_riff_avih(subItem, base_addr, pos, read_size, callback);
			break;

		case 'strh':
			print_riff_strh(subItem, base_addr, pos, read_size, callback, &fccType);
			break;

		case 'strf':
			if (fccType == streamtypeVIDEO) {
				print_riff_strf_vids(subItem, base_addr, pos, read_size, callback);
			}
			else if (fccType == streamtypeAUDIO) {
				print_riff_strf_auds(subItem, base_addr, pos, read_size, callback);
			}
			break;

		case 'ISBJ':
		case 'ISFT':
		case 'ICOP':
		case 'IDIT':
		case 'strn':
			print_riff_ascii(subItem, base_addr, pos, read_size, callback);
			break;

		case 'idx1':
			print_riff_idx1(subItem, base_addr, pos, read_size, callback);
			break;

		case 'dmlh':
			print_riff_dmlh(subItem, base_addr, pos, read_size, callback);
			break;

		case 'indx':
			print_riff_indx(subItem, base_addr, pos, read_size, callback);
			break;

		case 'ix00':
			print_riff_ix00(subItem, base_addr, pos, read_size, callback);
			break;
		}

		callback->next();

		switch (_byteswap_ulong(chunkID))
		{
		case 'RIFF':
		case 'LIST':
			parseRiffDirectory(
				base_addr,
				pos + sizeof(uint32_t),
				min(chunkSize - sizeof(uint32_t), data_size - (pos - offset + sizeof(uint32_t))),
				fourCC,
				callback, config);
			break;
		}

		pos += ((chunkSize + 1)&~1);

		config->is_abort(agalia::config::throw_on_abort);

	} while (chunkSize != 0 && (pos - offset) < data_size);
}

#include "image_file.h"

bool image_file_riff::is_supported(const void* buf, uint64_t size)
{
	if (buf == nullptr) return false;
	if (size < 4) return false;

	return (*reinterpret_cast<const uint32_t*>(buf) == *reinterpret_cast<const uint32_t*>("RIFF"));
}

void image_file_riff::parse(callback_cls* callback, const agalia::config* config)
{
	callback->insert_column(96, L"offset");
	callback->insert_column(96, L"parent");
	callback->insert_column(128, L"id");
	int size_column_number = callback->insert_column(96, L"size");
	callback->set_right_align(size_column_number, true);
	callback->insert_column(128, L"data");
	callback->insert_column(512, L"value");
	callback->next();

	parseRiffDirectory(this->base_addr, this->base_offset, this->data_size, 0, callback, config);
}


