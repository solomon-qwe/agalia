#include "stdafx.h"
#include "callback.h"

#include <CommCtrl.h>	// for ListView_***
#include <objbase.h>	// for CoTaskMemAlloc 
#include <atlcore.h>	// for CHeapPtr 

int callback_obj_ListCtrl::insert_column(int width, wchar_t* name)
{
	if (::WaitForSingleObject(handle_abort, 0) == WAIT_TIMEOUT)
	{
		LVCOLUMN lvc = {};
		lvc.mask = LVCF_TEXT | LVCF_WIDTH;
		lvc.cx = width;
		lvc.pszText = name;
		ListView_InsertColumn(hwnd_listctrl, subitem, &lvc);
	}

	return subitem++;
}

void callback_obj_ListCtrl::print(int subItem, wchar_t* sz)
{
	if (::WaitForSingleObject(handle_abort, 0) == WAIT_TIMEOUT)
	{
		if (cur_index != new_index)
		{
			LVITEM lvItem = {};
			lvItem.iItem = new_index;
			ListView_InsertItem(hwnd_listctrl, &lvItem);
			cur_index = new_index;
		}

		ListView_SetItemText(hwnd_listctrl, cur_index, subItem, sz);
	}
}

void callback_obj_ListCtrl::set_extra_data(uint64_t offset, uint64_t size)
{
	if (::WaitForSingleObject(handle_abort, 0) == WAIT_TIMEOUT)
	{
		LPVOID p = ::CoTaskMemAlloc(sizeof(agalia::list_item_param));
		if (p)
		{
			agalia::list_item_param* param = reinterpret_cast<agalia::list_item_param*>(p);
			param->offset = offset;
			param->size = size;

			LVITEM item = {};
			item.mask = LVIF_PARAM;
			item.iItem = cur_index;
			item.lParam = reinterpret_cast<LPARAM>(param);
			ListView_SetItem(hwnd_listctrl, &item);
		}
	}
}

void callback_obj_ListCtrl::set_right_align(int col, bool enable)
{
	if (::WaitForSingleObject(handle_abort, 0) == WAIT_TIMEOUT)
	{
		LVCOLUMN lvcol = {};
		lvcol.mask = LVCF_FMT;
		lvcol.fmt = enable ? LVCFMT_RIGHT : LVCFMT_LEFT;
		ListView_SetColumn(hwnd_listctrl, col, &lvcol);
	}
}

void callback_obj_ListCtrl::print(int subItem, const char* sz)
{
	size_t ret = 0;
	mbstowcs_s(&ret, nullptr, 0, sz, _TRUNCATE);
	ATL::CHeapPtr<wchar_t> heap;
	if (heap.Allocate(ret))
	{
		mbstowcs_s(&ret, heap, ret, sz, _TRUNCATE);
		print(subItem, heap);
	}
}

void callback_obj_stream::print(int subItem, const char* sz)
{
	UNREFERENCED_PARAMETER(subItem);
	if (column != 0) {
		fputws(_delimit.c_str(), _stream);
	}

	size_t ret = 0;
	mbstowcs_s(&ret, nullptr, 0, sz, _TRUNCATE);
	ATL::CHeapPtr<wchar_t> heap;
	if (heap.Allocate(ret))
	{
		mbstowcs_s(&ret, heap, ret, sz, _TRUNCATE);
		fputws(heap, _stream);
	}

	column++;
}
