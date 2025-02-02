#pragma once

#include <stdint.h>
class agaliaContainer;
class agaliaElement;

class controller
{
public:
	agaliaContainer* image = nullptr;
	CString data_path;
	uint64_t data_offset = 0;
	uint64_t data_size = 0;
	int data_format = 0;

	CWinThread* parse_thread = nullptr;
	HANDLE terminate_thread_request = NULL;

	void load_registry(void);
	void save_registry(void);
	int startup_view = 0;
	int startup_properties_show = 0;
	int rendering_engine = 0;
	int monitor_color_profile = 0;
};


struct watch_progress_param
{
	IProgressDialog* m_pProgressDlg = nullptr;
	HANDLE parse_finished_event = NULL;
	HANDLE abort_request_event = NULL;
};

UINT AFX_CDECL watch_progress_proc(LPVOID param);
watch_progress_param* prepare_progress(HWND hwdParent, const wchar_t* title);


class CMainFrame;

struct parser_thread_param
{
	controller* ctrl = nullptr;
	HWND hwndTarget = NULL;
	HANDLE parse_finished_event = NULL;
	HANDLE abort_request_event = NULL;
	HANDLE terminate_thread_request = NULL;
};


UINT ResetHierarchyViewThreadProc(LPVOID param);
UINT ResetFlatViewThreadProc(LPVOID param);
void launch_parser(controller* ctrl, AFX_THREADPROC pfnThreadProc, watch_progress_param* param1, parser_thread_param* param2);
