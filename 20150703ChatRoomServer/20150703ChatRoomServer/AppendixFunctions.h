#pragma once

#define MAX_BUF_SIZE 1024
class CMy20150703ChatRoomServerDlg;

//���������ڷ�����ϣ�����ͻ��˵���Ϣ
class CClientItem {
public:
	CString m_strIp;
	SOCKET m_Socket;	//�洢�ӿͻ��˷�������˵���Ϣ
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

//���ļ��ڲ���������
CString GetLocalIP();
CString GetSystemTime(BOOL bIncludeDate = FALSE);