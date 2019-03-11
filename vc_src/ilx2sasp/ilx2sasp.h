
// ilx2sasp.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cilx2saspApp:
// See ilx2sasp.cpp for the implementation of this class
//

class Cilx2saspApp : public CWinAppEx
{
public:
	Cilx2saspApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cilx2saspApp theApp;