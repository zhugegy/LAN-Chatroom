#pragma once

#define MAX_BUF_SIZE 1024
class CMy20150703ChatRoomServerDlg;

//此类用于在服务端上，保存客户端的信息
class CClientItem {
public:
	CString m_strIp;
	SOCKET m_Socket;	//存储从客户端发到服务端的信息
	HANDLE hThread;
	CString m_strNickname;
	BOOL m_bSpecial1;
	CString m_strLoginTime;
	int m_nStatus;
	CMy20150703ChatRoomServerDlg *m_pMainWnd;
	CClientItem(){
		m_pMainWnd = NULL;
		m_Socket = INVALID_SOCKET;
		hThread = NULL;
		m_bSpecial1 = FALSE;
		m_nStatus = 0;
	}
};

//本文件内部函数声明
CString GetLocalIP();
CString GetSystemTime(BOOL bIncludeDate = FALSE);