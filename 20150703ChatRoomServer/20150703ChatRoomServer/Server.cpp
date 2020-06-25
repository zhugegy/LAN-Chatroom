#include "stdafx.h"
#include "20150703ChatRoomServerDlg.h"
#include "resource.h"
#include "Server.h"

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

//服务端用于服务客户端的线程，每当有一个客户端连接，就给它分配一个对应的线程，专门负责
DWORD WINAPI ClientThreadProc(LPVOID lpParameter)
{
	CString strMsg;
	CString strMsgAnother;
	//定义的这个 m_ClientItem，不好用，应该是个形参，在函数内部无法改变外面的数值
	CClientItem m_ClientItem = *(CClientItem *)lpParameter;
	while (TRUE && !(m_ClientItem.m_pMainWnd->m_bShutDown))
	{
		if (SOCKET_Select(m_ClientItem.m_Socket, 100, TRUE))
		{
			TCHAR szBuf[MAX_BUF_SIZE] = {0};
			int iRet = recv(m_ClientItem.m_Socket, 
				(char *)szBuf, MAX_BUF_SIZE, 0);
			if (iRet > 0)
			{
				//信息接收成功;
				strMsg.Format(_T("%s"), szBuf);

				//Special1状态，用于传输客户端昵称
				//*进入、退出Special1状态
				if (strMsg == _T("Special1Start"))
				{
					/*(*(CClientItem *)lpParameter).m_bSpecial1 = TRUE;*/
					m_ClientItem.m_bSpecial1 = TRUE;
					//Sleep(500);
					continue;
				}
				else if (strMsg == _T("Special1End"))
				{
					m_ClientItem.m_bSpecial1 = FALSE;
					//Sleep(500);
					
					//**填写对应参数（登录时间，用户状态）
					m_ClientItem.m_strLoginTime = 
						GetSystemTime(TRUE);
					(*(CClientItem *)lpParameter).m_strLoginTime = 
						GetSystemTime(TRUE);
					m_ClientItem.m_nStatus = 1;
					(*(CClientItem *)lpParameter).m_nStatus = 1;
					//**重写strMsg, 用于稍后的提示和系统广播。
					strMsg = 
						GetSystemTime(TRUE) + 
						_T("用户 ") + 
						m_ClientItem.m_strNickname + 
						_T("(") + 
						m_ClientItem.m_strIp + 
						_T(") 已连接。");

					//**服务端显示“用户已连接”提示。
					m_ClientItem.m_pMainWnd->
						ShowMsg(
						_T("提示：") + 
						strMsg);

					//**广播发送欢迎信息
					m_ClientItem.m_pMainWnd->
						SendClientsMsg(
						_T("系统广播：") + 
						strMsg);

					//**服务端客户列表重写
					m_ClientItem.m_pMainWnd->
						EmptyClientListControl();
					m_ClientItem.m_pMainWnd->
						FillInClientListControl();

					//广播发送（除了他以外）客户端客户列表增加指令,重写strMsg
					Sleep(180);
					m_ClientItem.m_pMainWnd->
						SendClientsMsg(_T("Special0ADDStart"));
					
					Sleep(80);
					//**生成指令
					switch ((*(CClientItem *)lpParameter).m_nStatus)
					{
					case 0:
						strMsg = 
							(*(CClientItem *)lpParameter).m_strNickname + 
							_T("#") + 
							_T("离线") + 
							_T("#") +
							(*(CClientItem *)lpParameter).m_strIp + 
							_T("#") + 
							(*(CClientItem *)lpParameter).m_strLoginTime;
						break;
					case 1:
						strMsg = 
							(*(CClientItem *)lpParameter).m_strNickname + 
							_T("#") + 
							_T("在线") + 
							_T("#") +
							(*(CClientItem *)lpParameter).m_strIp + 
							_T("#") + 
							(*(CClientItem *)lpParameter).m_strLoginTime;
						break;
					case 2:
						strMsg = 
							(*(CClientItem *)lpParameter).m_strNickname + 
							_T("#") + 
							_T("禁言") + 
							_T("#") +
							(*(CClientItem *)lpParameter).m_strIp + 
							_T("#") + 
							(*(CClientItem *)lpParameter).m_strLoginTime;
						break;						
					default:
						strMsg = 
							(*(CClientItem *)lpParameter).m_strNickname + 
							_T("#") + 
							_T("未知") + 
							_T("#") +
							(*(CClientItem *)lpParameter).m_strIp + 
							_T("#") + 
							(*(CClientItem *)lpParameter).m_strLoginTime;
						break;
					}
					strMsgAnother = strMsg;
					//给除他以外的人广播一个ADD指令
					m_ClientItem.m_pMainWnd->
						SendClientsMsg(strMsg, &(*(CClientItem *)lpParameter));

					//给他单独SOLO发送一个目前所有客户信息的ADD指令
					Sleep(80);
					for (int i = 0; i < m_ClientItem.m_pMainWnd->
						m_ClientArray.GetCount(); i++)
					{
						switch (m_ClientItem.m_pMainWnd->
							m_ClientArray[i].m_nStatus)
						{
						case 0:
							strMsg = 
								m_ClientItem.m_pMainWnd->
								m_ClientArray[i].m_strNickname + 
								_T("#") + 
								_T("离线") + 
								_T("#") +
								m_ClientItem.m_pMainWnd->
								m_ClientArray[i].m_strIp + 
								_T("#") + 
								m_ClientItem.m_pMainWnd->
								m_ClientArray[i].m_strLoginTime;
							break;
						case 1:
							strMsg = 
								m_ClientItem.m_pMainWnd->
								m_ClientArray[i].m_strNickname + 
								_T("#") + 
								_T("在线") + 
								_T("#") +
								m_ClientItem.m_pMainWnd->
								m_ClientArray[i].m_strIp + 
								_T("#") + 
								m_ClientItem.m_pMainWnd->
								m_ClientArray[i].m_strLoginTime;
							break;
						case 2:
							strMsg = 
								m_ClientItem.m_pMainWnd->
								m_ClientArray[i].m_strNickname + 
								_T("#") + 
								_T("禁言") + 
								_T("#") +
								m_ClientItem.m_pMainWnd->
								m_ClientArray[i].m_strIp + 
								_T("#") + 
								m_ClientItem.m_pMainWnd->
								m_ClientArray[i].m_strLoginTime;
							break;						
						default:
							strMsg = 
								m_ClientItem.m_pMainWnd->
								m_ClientArray[i].m_strNickname + 
								_T("#") + 
								_T("未知") + 
								_T("#") +
								m_ClientItem.m_pMainWnd->
								m_ClientArray[i].m_strIp + 
								_T("#") + 
								m_ClientItem.m_pMainWnd->
								m_ClientArray[i].m_strLoginTime;
							break;
						}
						//solosend
						m_ClientItem.m_pMainWnd->
							SendClientsMsgSolo(strMsg, 
							&(*(CClientItem *)lpParameter));
						Sleep(80);
					}

					Sleep(80);
					m_ClientItem.m_pMainWnd->
						SendClientsMsg(_T("Special0ADDEnd"));

					continue;
				}
				//*在Special1状态中，设定昵称
				if (m_ClientItem.m_bSpecial1 == TRUE) 
				{
					//**根据payload改变昵称
					m_ClientItem.m_strNickname = strMsg;
					(*(CClientItem *)lpParameter).m_strNickname = strMsg;
					//Sleep(500);
					continue;
				}

				//正常状态
				//*信息的加工
				strMsg = 
					m_ClientItem.m_strNickname + 
					_T("(") + 
					m_ClientItem.m_strIp + 
					_T(") ") + 
					GetSystemTime(TRUE) +
					_T(" ") +
					_T("> ") + 
					strMsg;

				m_ClientItem.m_nStatus = 
					(*(CClientItem *)lpParameter).m_nStatus;
				if (m_ClientItem.m_nStatus == 1)
				{
					m_ClientItem.m_pMainWnd->ShowMsg(strMsg);
					m_ClientItem.m_pMainWnd->SendClientsMsg(strMsg/*,
					&(*(CClientItem *)lpParameter)*/);
				}
				else
				{
					if (m_ClientItem.m_nStatus == 2)
					{
						m_ClientItem.m_pMainWnd->ShowMsg(strMsg + 
							_T("（禁言，系统已拦截）"));
						m_ClientItem.m_pMainWnd->SendClientsMsgSolo(
							_T("您已被禁言。"), &m_ClientItem);
					}
					else
					{
						/* m_ClientItem.m_pMainWnd->ShowMsg(strMsg + 
							_T("（多开，系统已拦截）"));
						//其实这是一个不好解决的BUG。
						//同一个IP多开客户端同时说话，就不能实现禁言功能。只能二选一
						m_ClientItem.m_pMainWnd->SendClientsMsgSolo(
							_T("同一IP多开客户端，第一个客户端有时不能说话(BUG)"), 
							&m_ClientItem); */

					m_ClientItem.m_pMainWnd->ShowMsg(strMsg);
					m_ClientItem.m_pMainWnd->SendClientsMsg(strMsg/*,
					&(*(CClientItem *)lpParameter)*/);
					}
					
				}
				
			}
			else
			{
				//链接关闭的状态。
				strMsg = 
					GetSystemTime(TRUE) + 
					_T("用户 ") + 
					m_ClientItem.m_strNickname + 
					_T("(") + 
					m_ClientItem.m_strIp + 
					_T(") ") + 
					_T("离开了聊天室！");

				//*服务端显示“用户已离开”提示。
				m_ClientItem.m_pMainWnd->
					ShowMsg(
					_T("提示：") + 
					strMsg);

				//*广播发送离开信息
				m_ClientItem.m_pMainWnd->
					SendClientsMsg(
					_T("系统广播：") + 
					strMsg);
				
				//*广播删除客户信息指令，重写strMsg
				Sleep(80);
				m_ClientItem.m_pMainWnd->SendClientsMsg(_T("Special0DELStart"));
					
				Sleep(80);
				
				//*删除此客户端数据
				m_ClientItem.m_pMainWnd->
					RemoveClientFromArray(m_ClientItem);

				//广播一个DEL指令
				m_ClientItem.m_pMainWnd->
					SendClientsMsg(strMsgAnother);

				Sleep(80);
				m_ClientItem.m_pMainWnd->SendClientsMsg(_T("Special0DELEnd"));

				//*重写客户列表
				m_ClientItem.m_pMainWnd->
					EmptyClientListControl();
				m_ClientItem.m_pMainWnd->
					FillInClientListControl();

				break;
			}
		}
		Sleep(200);
	}
	return TRUE;
}

//服务端监听线程
DWORD WINAPI ListenThreadFunc(LPVOID pParam)
{
	CMy20150703ChatRoomServerDlg *pChatRoom = 
		(CMy20150703ChatRoomServerDlg *)pParam;
	ASSERT(pChatRoom != NULL);
	pChatRoom->m_ListenSock = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
	if (pChatRoom->m_ListenSock == INVALID_SOCKET)
	{
		AfxMessageBox(_T("新建Socket失败！"));
		return FALSE;
	}

	int iPort = pChatRoom->GetDlgItemInt(IDC_EDIT_LOCAL_PORT);
	if (iPort <= 0 || iPort > 65535)
	{
		AfxMessageBox(_T("请输入合适的端口：1 - 65535"));
		goto __Error_End;
	}

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY;
	service.sin_port = htons(iPort);

	if (bind(pChatRoom->m_ListenSock, (sockaddr*)&service, sizeof(sockaddr_in)) 
		== SOCKET_ERROR)
	{
		AfxMessageBox(_T("绑定端口失败！"));
		goto __Error_End;
	}

	if(listen(pChatRoom->m_ListenSock, 5) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("监听失败！"));
		goto __Error_End;
	}

	pChatRoom->ShowMsg(GetSystemTime(TRUE) + 
		_T("服务器开启成功，已经开始监听客户端。"));

	while (TRUE && !(pChatRoom->m_bShutDown))
	{
		if (SOCKET_Select(pChatRoom->m_ListenSock, 100, TRUE))
		{
			sockaddr_in clientAddr;
			int iLen = sizeof(sockaddr_in);
			SOCKET accSock = accept(pChatRoom->m_ListenSock, 
				(struct sockaddr *)&clientAddr , &iLen);
			if (accSock == INVALID_SOCKET)
			{
				continue;
			}
			//做一些其他操作……
			//创建CClientItem类型的对象，用于储存客户的信息，这里可以自定义其他项目
			CClientItem tItem;
			tItem.m_Socket = accSock;
			tItem.m_strIp = inet_ntoa(clientAddr.sin_addr);
			tItem.m_pMainWnd = pChatRoom;
			INT_PTR idx = pChatRoom->m_ClientArray.Add(tItem);
			tItem.hThread = CreateThread(NULL, 0, ClientThreadProc, 
				&(pChatRoom->m_ClientArray.GetAt(idx)), CREATE_SUSPENDED, NULL);
			pChatRoom->m_ClientArray.GetAt(idx).hThread = tItem.hThread;
			ResumeThread(tItem.hThread);

			Sleep(100);
		}
	}

__Error_End:
	closesocket(pChatRoom->m_ListenSock);
	return TRUE;
}