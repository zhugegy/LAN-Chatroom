#include "stdafx.h"
#include "Client.h"
#include "20150703ChatRoomClientDlg.h"
#include "resource.h"
#include "Inc.h"

//一个用于非阻塞的I/O模型函数
BOOL SOCKET_Select(SOCKET hSocket, int nTimeOut, BOOL bRead)
{
	fd_set fdset;
	timeval tv;
	FD_ZERO(&fdset);
	FD_SET(hSocket, &fdset);
	nTimeOut = nTimeOut > 1000 ? 1000 : nTimeOut;
	tv.tv_sec  = 0;
	tv.tv_usec = nTimeOut;

	int iRet = 0;
	if (bRead)
	{
		iRet = select(0, &fdset, NULL , NULL, &tv);
	}
	else
	{
		iRet = select(0, NULL , &fdset, NULL, &tv);
	}

	if (iRet <= 0)
	{
		return FALSE;
	}
	else if (FD_ISSET(hSocket, &fdset))
	{
		return TRUE;
	}

	return FALSE;
}

//客户端链接线程
DWORD WINAPI ConnectThreadFunc(LPVOID pParam)
{
	CString strSpecial1Start = _T("Special1Start");	
	CString strSpecial1Payload = _T("");
	CString strSpecial1End = _T("Special1End");	
	CMy20150703ChatRoomClientDlg *pChatRoom = 
		(CMy20150703ChatRoomClientDlg *)pParam;
	ASSERT(pChatRoom != NULL);
	pChatRoom->ShowMsg(_T("系统信息：正在连接服务器..."));
	pChatRoom->m_ConnectSock = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
	CString strTemp;	//用于接收特殊信息
	if (pChatRoom->m_ConnectSock == INVALID_SOCKET)
	{
		AfxMessageBox(_T("新建Socket失败！"));
		return FALSE;
	}
	CString strServIp;
	pChatRoom->GetDlgItemText(IDC_IPADDRESS_SERVER_IP, strServIp);
	int iPort = pChatRoom->GetDlgItemInt(IDC_EDIT_SERVER_PORT);
	if (iPort <= 0 || iPort > 65535)
	{
		AfxMessageBox(_T("请输入合适的端口：1 - 65535"));
		goto __Error_End;
	}

	char szIpAddr[16] = {0};
	USES_CONVERSION;
	strcpy_s(szIpAddr, 16, T2A(strServIp));

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(iPort);
	server.sin_addr.s_addr = inet_addr(szIpAddr);
	if (connect(pChatRoom->m_ConnectSock, (struct sockaddr *)&server,  
		sizeof(struct sockaddr)) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("连接失败，请重试！"));
		goto __Error_End;
	}

	//新建成功后，马上发送一条特殊信息
	//Sleep(1000);	//留1秒时间给服务端，让它建好对应的线程
	pChatRoom->ShowMsg(_T("系统信息：正在连接服务器..."));
	//特殊1信息的开始
	send(pChatRoom->m_ConnectSock, (char *)strSpecial1Start.GetBuffer(), 
		strSpecial1Start.GetLength()*sizeof(TCHAR), 0);
	strSpecial1Start.ReleaseBuffer();
	Sleep(220);
	pChatRoom->ShowMsg(_T("系统信息：正在连接服务器..."));
	//特殊1信息，内部包含用户昵称
	pChatRoom->GetDlgItemText(IDC_EDIT_NICKNAME, strSpecial1Payload);
	send(pChatRoom->m_ConnectSock, (char *)strSpecial1Payload.GetBuffer(), 
		strSpecial1Payload.GetLength()*sizeof(TCHAR), 0);
	strSpecial1Payload.ReleaseBuffer();
	Sleep(220);
	//特殊1信息的结束
	send(pChatRoom->m_ConnectSock, (char *)strSpecial1End.GetBuffer(), 
		strSpecial1End.GetLength()*sizeof(TCHAR), 0);
	strSpecial1End.ReleaseBuffer();

	pChatRoom->ShowMsg(_T("系统信息：连接服务器成功！"));
	pChatRoom->m_bIsConnected = TRUE;

	while(TRUE && !(pChatRoom->m_bShutDown))
	{
		if (SOCKET_Select(pChatRoom->m_ConnectSock))
		{
			TCHAR szBuf[MAX_BUF_SIZE] = {0};
			int iRet = recv(pChatRoom->m_ConnectSock, (char *)szBuf, 
				MAX_BUF_SIZE, 0);
			if (iRet > 0)
			{
				//right;
				strTemp.Format(_T("%s"), szBuf);

				//特殊信息0ADD的起始与结束
				if (strTemp == _T("Special0ADDStart"))
				{
					pChatRoom->m_bIsSpecial0_ADD = TRUE;
					continue;
				}
				else if (strTemp == _T("Special0ADDEnd"))
				{
					pChatRoom->m_bIsSpecial0_ADD = FALSE;
					continue;
				}

				if (pChatRoom->m_bIsSpecial0_ADD == TRUE)
				{
					pChatRoom->AddClientListControl(strTemp);
					continue;
				}

				//特殊信息0DEL的起始与结束
				if (strTemp == _T("Special0DELStart"))
				{
					pChatRoom->m_bIsSpecial0_DEL = TRUE;
					continue;
				}
				else if (strTemp == _T("Special0DELEnd"))
				{
					pChatRoom->m_bIsSpecial0_DEL = FALSE;
					continue;
				}

				if (pChatRoom->m_bIsSpecial0_DEL == TRUE)
				{
					pChatRoom->DeleteClientListControl(strTemp);
					continue;
				}

				//特殊信息0KICK的相应
				if (strTemp == _T("Special0KICK"))
				{
					//pChatRoom->OnBnClickedButtonDisconnectServer();
					//pChatRoom->StopClient();
					pChatRoom->ShowMsg(_T("系统信息：您已被踢出服务器。"));
					pChatRoom->m_bIsConnected = FALSE;
					pChatRoom->EnableWindow(IDC_BUTTON_DISCONNECT_SERVER, FALSE);
					pChatRoom->EnableWindow(IDC_BUTTON_CONNECT_SERVER, TRUE);
					pChatRoom->EnableWindow(IDC_EDIT_NICKNAME, TRUE);
					pChatRoom->m_ClientListControl.DeleteAllItems();
					//continue;
					goto __Error_End;
				}

				//正常信息
				pChatRoom->ShowMsg(szBuf);
			}
			else
			{
				//close socket;
				pChatRoom->ShowMsg(_T("聊天室服务器已停止，请重新进行连接！"));
				break;
			}
		}
		Sleep(30);
	}

__Error_End:
	closesocket(pChatRoom->m_ConnectSock);
	return TRUE;
}