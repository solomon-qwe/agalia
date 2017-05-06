
// agalia.h : main header file for the agalia application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

class CAgaliaController;

// CAgaliaApp:
// See agalia.cpp for the implementation of this class
//

class CAgaliaApp : public CWinApp
{
public:
	CAgaliaApp();

public:
	CAgaliaController* pController;

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CAgaliaApp theApp;
