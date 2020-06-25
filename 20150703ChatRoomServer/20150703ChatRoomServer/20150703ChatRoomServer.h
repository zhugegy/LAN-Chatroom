
// 20150703ChatRoomServer.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMy20150703ChatRoomServerApp:
// See 20150703ChatRoomServer.cpp for the implementation of this class
//

class CMy20150703ChatRoomServerApp : public CWinApp
{
public:
	CMy20150703ChatRoomServerApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMy20150703ChatRoomServerApp theApp;