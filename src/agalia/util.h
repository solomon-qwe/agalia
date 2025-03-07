#pragma once

void OnInitMenuPopup(CWnd* pWnd, CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);

HRESULT LoadFile(CHeapPtr<BYTE>& buf, long* file_size, const wchar_t* path);
HRESULT GetMonitorColorProfilePath(_In_ HWND hwnd, _Inout_ LPDWORD pBufSize, LPWSTR pszFilename);