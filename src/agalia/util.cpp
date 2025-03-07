#include "pch.h"
#include "util.h"

#include <io.h>


// refer. https://support.microsoft.com/ja-jp/help/242577/ 
void OnInitMenuPopup(CWnd* pWnd, CMenu* pPopupMenu, UINT /*nIndex*/, BOOL /*bSysMenu*/)
{
	ASSERT(pPopupMenu != NULL);
	// Check the enabled state of various menu items.

	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// Determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup).
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
		state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
	else if ((hParentMenu = ::GetMenu(pWnd->m_hWnd)) != NULL)
	{
		CWnd* pParent = pWnd;
		// Child windows don't have menus--need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
				{
					// When popup is found, m_pParentMenu is containing menu.
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // Menu separator or invalid cmd - ignore it.

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// Possibly a popup menu, route to first item of that popup.
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // First item of popup can't be routed to.
			}
			state.DoUpdate(pWnd, TRUE);   // Popups are never auto disabled.
		}
		else
		{
			// Normal menu item.
			// Auto enable/disable if frame window has m_bAutoMenuEnable
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(pWnd, FALSE);
		}

		// Adjust for menu deletions and additions.
		UINT nCount = pPopupMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}


HRESULT LoadFile(CHeapPtr<BYTE>& buf, long* file_size, const wchar_t* path)
{
	if (path == nullptr) return E_POINTER;
	if (file_size == nullptr) return E_POINTER;
	if (buf.m_pData) return E_FAIL;

	FILE* fp = nullptr;
	errno_t err = _wfopen_s(&fp, path, L"rb");
	if (err != 0) return E_FAIL;
	if (fp == nullptr) return E_FAIL;

	HRESULT ret = E_FAIL;
	*file_size = _filelength(_fileno(fp));
	if (buf.AllocateBytes(*file_size))
		if (fread(buf, *file_size, 1, fp) == 1)
			ret = S_OK;

	fclose(fp);

	return ret;
}


HRESULT GetMonitorColorProfilePath(_In_ HWND hwnd, _Inout_ LPDWORD pBufSize, LPWSTR pszFilename)
{
	HMONITOR hMonitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	if (!hMonitor) return HRESULT_FROM_WIN32(::GetLastError());

	MONITORINFOEX mi = {};
	mi.cbSize = sizeof(mi);
	BOOL ret = ::GetMonitorInfo(hMonitor, &mi);
	if (!ret) return HRESULT_FROM_WIN32(::GetLastError());

	HDC hdc = ::CreateDC(mi.szDevice, mi.szDevice, nullptr, nullptr);
	if (!hdc) return HRESULT_FROM_WIN32(::GetLastError());

	ret = ::GetICMProfile(hdc, pBufSize, pszFilename);
	HRESULT result = ret ? S_OK : HRESULT_FROM_WIN32(::GetLastError());

	::DeleteDC(hdc);

	return result;
}
