
// IMotion_Testbed.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CIMotion_TestbedApp:
// See IMotion_Testbed.cpp for the implementation of this class
//

class CIMotion_TestbedApp : public CWinApp
{
public:
	CIMotion_TestbedApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CIMotion_TestbedApp theApp;