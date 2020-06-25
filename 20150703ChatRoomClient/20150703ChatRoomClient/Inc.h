#pragma once

#include "windows.h"
#include "atlstr.h"

#define MAX_BUF_SIZE 1024
class CMy20150703ChatRoomClientDlg;

//此类用于在服务端上，保存客户端的信息
class CClientItem {
public:
	CString m_strIp;
	SOCKET m_Socket;	//存储从客户端发到服务端的信息
	HANDLE hThread;
	CMy20150703ChatRoomClientDlg *m_pMainWnd;
	CClientItem(){
		m_pMainWnd = NULL;
		m_Socket = INVALID_SOCKET;
		hThread = NULL;
	}
};