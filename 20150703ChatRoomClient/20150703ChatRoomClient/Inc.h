#pragma once

#include "windows.h"
#include "atlstr.h"

#define MAX_BUF_SIZE 1024
class CMy20150703ChatRoomClientDlg;

//���������ڷ�����ϣ�����ͻ��˵���Ϣ
class CClientItem {
public:
	CString m_strIp;
	SOCKET m_Socket;	//�洢�ӿͻ��˷�������˵���Ϣ
	HANDLE hThread;
	CMy20150703ChatRoomClientDlg *m_pMainWnd;
	CClientItem(){
		m_pMainWnd = NULL;
		m_Socket = INVALID_SOCKET;
		hThread = NULL;
	}
};