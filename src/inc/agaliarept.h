#pragma once 

#ifdef AGALIAREPT_EXPORTS
#define AGALIAREPT_API __declspec(dllexport)
#else
#define AGALIAREPT_API __declspec(dllimport)
#endif

#include <stdint.h>	// for uint**_t 
#include <stdio.h>	// for FILE* 
#include <Windows.h>	// for HWND 


namespace agalia
{
	class config
	{
	public:
		enum lang {
			enu,
			jpn
		};
		enum out_stream {
			console,
			file
		};
		// コマンドライン引数 
		unsigned int byte_stream_limit_length;
		unsigned int preferred_language;
		bool force_dictionary_vr;
		uint64_t offset;
		uint64_t data_size;

		// 共通変数 
		HWND hwndListCtrl;
		HANDLE abort_event;

	protected:
		config()
		{
			// default settings 
			byte_stream_limit_length = 128;
			force_dictionary_vr = true;
			input_file_path = nullptr;
			output_file_path = nullptr;

			offset = 0;
			data_size = 0;

			hwndListCtrl = NULL;
			stream = nullptr;
			abort_event = NULL;
		}
		virtual ~config();

	public:
		static config* create_instance(void);

		virtual void release(void);

		virtual void set_output_file_path(const wchar_t* path);
		virtual void set_input_file_path(const wchar_t* path);
		virtual const wchar_t* get_output_file_path(void) const { return output_file_path; }
		virtual const wchar_t* get_input_file_path(void) const { return input_file_path; }

		virtual HRESULT open_out_stream(int mode);
		virtual void close_out_stream(void);
		virtual bool is_open_stream(void) const { return (stream != nullptr); }

		virtual HRESULT parse_command_line(void);

		enum abort_flag {
			no_throw_on_abort,
			throw_on_abort
		};
		virtual bool is_abort(abort_flag flag) const;

		virtual void usage(void);

	protected:
		virtual void set_file_path(wchar_t** pp, const wchar_t* path);
		wchar_t* input_file_path;
		wchar_t* output_file_path;

	public:
		FILE* stream;
	};

	// config クラスのスマートポインタ 
	class config_ptr
	{
	public:
		config_ptr() { p = nullptr; }
		virtual ~config_ptr() { if(p) p->release(); }
		config_ptr(config* body) { p = body; }
		operator config*() const { return p; }
		config* operator =(config* body) { p = body;  return p; }
		config* operator->(void) const { return p; }
		config** operator&(void) { return &p; }

	public:
		config* p;
	};

	class exception
	{
	public:
		exception(HRESULT e) { this->e = e; }
		HRESULT e;
	};

	// リストビューコントロールの LPARAM 
	struct list_item_param
	{
		uint64_t offset;
		uint64_t size;
	};
}

/// <summary>
/// Analyze image file.</summary>
/// <returns> 
/// Returns S_OK if successful, or an error value otherwise.</returns>
/// <param name="param">Specifies the pointer to a agalia::config structure containing analyze configuration</param>
extern "C" AGALIAREPT_API
HRESULT AgaliaMain(const agalia::config* param);

// <summary>
/// Create agalia::config structure.</summary>
/// <returns> 
/// Returns S_OK if successful, or an error value otherwise.</returns>
/// <param name="param">Points to a buffer allocated by this method and initialized default configuration</param>
extern "C" AGALIAREPT_API
HRESULT AgaliaGetConfig(agalia::config** config);

#define AGALIA_ERR_UNSUPPORTED		MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0001)
#define AGALIA_ERR_IN_PAGE_ERROR	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0002)
#define AGALIA_ERR_MEMORY_LEAK		MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0003)
#define AGALIA_ERR_ABORT			E_ABORT
#define AGALIA_ERR_PATH_NOT_FOUND	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_PATH_NOT_FOUND)
