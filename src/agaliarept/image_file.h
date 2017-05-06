#pragma once

class callback_cls;

class image_file
{
public:
	image_file()
	{
		base_addr = nullptr;
		base_offset = 0;
		data_size = 0;
	}

	virtual ~image_file()
	{
	}

	HRESULT attach(const void* p, uint64_t b, uint64_t s)
	{
		base_addr = reinterpret_cast<const uint8_t*>(p);
		base_offset = b;
		data_size = s;
		return S_OK;
	}

	virtual void parse(callback_cls* callback, const agalia::config* config) = 0;

protected:
	const uint8_t* base_addr;	// ファイルの先頭アドレス 
	uint64_t base_offset;		// 解析対象領域の先頭位置 
	uint64_t data_size;			// 解析対象領域のサイズ 
};


// ASF 
class image_file_asf : public image_file
{
public:
	image_file_asf() {}
	virtual ~image_file_asf() {}
	virtual void parse(callback_cls* callback, const agalia::config* config);
	static bool is_supported(const void* buf, uint64_t size);
};

// BMP 
class image_file_bmp : public image_file
{
public:
	image_file_bmp() {}
	virtual ~image_file_bmp() {}
	virtual void parse(callback_cls* callback, const agalia::config* config);
	static bool is_supported(const void* buf, uint64_t size);
};

// DICOM  
class image_file_dicom : public image_file
{
public:
	image_file_dicom() {}
	virtual ~image_file_dicom() {}
	virtual void parse(callback_cls* callback, const agalia::config* config);
	static bool is_supported(const void* buf, uint64_t size);
};

// ISO base media file format   
class image_file_iso : public image_file
{
public:
	image_file_iso() {}
	virtual ~image_file_iso() {}
	virtual void parse(callback_cls* callback, const agalia::config* config);
	static bool is_supported(const void* buf, uint64_t size);
};

// JPEG  
class image_file_jpeg : public image_file
{
public:
	image_file_jpeg() {}
	virtual ~image_file_jpeg() {}
	virtual void parse(callback_cls* callback, const agalia::config* config);
	static bool is_supported(const void* buf, uint64_t size);
};

// RIFF 
class image_file_riff : public image_file
{
public:
	image_file_riff() {}
	virtual ~image_file_riff() {}
	virtual void parse(callback_cls* callback, const agalia::config* config);
	static bool is_supported(const void* buf, uint64_t size);
};

// TIFF 
class image_file_tiff : public image_file
{
public:
	image_file_tiff() {}
	virtual ~image_file_tiff() {}
	virtual void parse(callback_cls* callback, const agalia::config* config);
	static bool is_supported(const void* buf, uint64_t size);
};

// MPEG-2 PS 
class image_file_m2p : public image_file
{
public:
	image_file_m2p() {}
	virtual ~image_file_m2p() {}
	virtual void parse(callback_cls* callback, const agalia::config* config);
	static bool is_supported(const void* buf, uint64_t size);
};
