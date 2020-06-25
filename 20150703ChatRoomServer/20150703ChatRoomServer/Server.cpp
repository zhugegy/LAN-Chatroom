#include "stdafx.h"
#include "20150703ChatRoomServerDlg.h"
#include "resource.h"
#include "Server.h"

//һ�����ڷ�������I/Oģ�ͺ���
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

//��������ڷ���ͻ��˵��̣߳�ÿ����һ���ͻ������ӣ��͸�������һ����Ӧ���̣߳�ר�Ÿ���
DWORD WINAPI ClientThreadProc(LPVOID lpParameter)
{
	CString strMsg;
	CString strMsgAnother;
	//�������� m_ClientItem�������ã�Ӧ���Ǹ��βΣ��ں����ڲ��޷��ı��������ֵ
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
				//��Ϣ���ճɹ�;
				strMsg.Format(_T("%s"), szBuf);

				//Special1״̬�����ڴ���ͻ����ǳ�
				//*���롢�˳�Special1״̬
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
					
					//**��д��Ӧ��������¼ʱ�䣬�û�״̬��
					m_ClientItem.m_strLoginTime = 
						GetSystemTime(TRUE);
					(*(CClientItem *)lpParameter).m_strLoginTime = 
						GetSystemTime(TRUE);
					m_ClientItem.m_nStatus = 1;
					(*(CClientItem *)lpParameter).m_nStatus = 1;
					//**��дstrMsg, �����Ժ����ʾ��ϵͳ�㲥��
					strMsg = 
						GetSystemTime(TRUE) + 
						_T("�û� ") + 
						m_ClientItem.m_strNickname + 
						_T("(") + 
						m_ClientItem.m_strIp + 
						_T(") �����ӡ�");

					//**�������ʾ���û������ӡ���ʾ��
					m_ClientItem.m_pMainWnd->
						ShowMsg(
						_T("��ʾ��") + 
						strMsg);

					//**�㲥���ͻ�ӭ��Ϣ
					m_ClientItem.m_pMainWnd->
						SendClientsMsg(
						_T("ϵͳ�㲥��") + 
						strMsg);

					//**����˿ͻ��б���д
					m_ClientItem.m_pMainWnd->
						EmptyClientListControl();
					m_ClientItem.m_pMainWnd->
						FillInClientListControl();

					//�㲥���ͣ����������⣩�ͻ��˿ͻ��б�����ָ��,��дstrMsg
					Sleep(180);
					m_ClientItem.m_pMainWnd->
						SendClientsMsg(_T("Special0ADDStart"));
					
					Sleep(80);
					//**����ָ��
					switch ((*(CClientItem *)lpParameter).m_nStatus)
					{
					case 0:
						strMsg = 
							(*(CClientItem *)lpParameter).m_strNickname + 
							_T("#") + 
							_T("����") + 
							_T("#") +
							(*(CClientItem *)lpParameter).m_strIp + 
							_T("#") + 
							(*(CClientItem *)lpParameter).m_strLoginTime;
						break;
					case 1:
						strMsg = 
							(*(CClientItem *)lpParameter).m_strNickname + 
							_T("#") + 
							_T("����") + 
							_T("#") +
							(*(CClientItem *)lpParameter).m_strIp + 
							_T("#") + 
							(*(CClientItem *)lpParameter).m_strLoginTime;
						break;
					case 2:
						strMsg = 
							(*(CClientItem *)lpParameter).m_strNickname + 
							_T("#") + 
							_T("����") + 
							_T("#") +
							(*(CClientItem *)lpParameter).m_strIp + 
							_T("#") + 
							(*(CClientItem *)lpParameter).m_strLoginTime;
						break;						
					default:
						strMsg = 
							(*(CClientItem *)lpParameter).m_strNickname + 
							_T("#") + 
							_T("δ֪") + 
							_T("#") +
							(*(CClientItem *)lpParameter).m_strIp + 
							_T("#") + 
							(*(CClientItem *)lpParameter).m_strLoginTime;
						break;
					}
					strMsgAnother = strMsg;
					//������������˹㲥һ��ADDָ��
					m_ClientItem.m_pMainWnd->
						SendClientsMsg(strMsg, &(*(CClientItem *)lpParameter));

					//��������SOLO����һ��Ŀǰ���пͻ���Ϣ��ADDָ��
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
								_T("����") + 
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
								_T("����") + 
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
								_T("����") + 
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
								_T("δ֪") + 
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
				//*��Special1״̬�У��趨�ǳ�
				if (m_ClientItem.m_bSpecial1 == TRUE) 
				{
					//**����payload�ı��ǳ�
					m_ClientItem.m_strNickname = strMsg;
					(*(CClientItem *)lpParameter).m_strNickname = strMsg;
					//Sleep(500);
					continue;
				}

				//����״̬
				//*��Ϣ�ļӹ�
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
							_T("�����ԣ�ϵͳ�����أ�"));
						m_ClientItem.m_pMainWnd->SendClientsMsgSolo(
							_T("���ѱ����ԡ�"), &m_ClientItem);
					}
					else
					{
						/* m_ClientItem.m_pMainWnd->ShowMsg(strMsg + 
							_T("���࿪��ϵͳ�����أ�"));
						//��ʵ����һ�����ý����BUG��
						//ͬһ��IP�࿪�ͻ���ͬʱ˵�����Ͳ���ʵ�ֽ��Թ��ܡ�ֻ�ܶ�ѡһ
						m_ClientItem.m_pMainWnd->SendClientsMsgSolo(
							_T("ͬһIP�࿪�ͻ��ˣ���һ���ͻ�����ʱ����˵��(BUG)"), 
							&m_ClientItem); */

					m_ClientItem.m_pMainWnd->ShowMsg(strMsg);
					m_ClientItem.m_pMainWnd->SendClientsMsg(strMsg/*,
					&(*(CClientItem *)lpParameter)*/);
					}
					
				}
				
			}
			else
			{
				//���ӹرյ�״̬��
				strMsg = 
					GetSystemTime(TRUE) + 
					_T("�û� ") + 
					m_ClientItem.m_strNickname + 
					_T("(") + 
					m_ClientItem.m_strIp + 
					_T(") ") + 
					_T("�뿪�������ң�");

				//*�������ʾ���û����뿪����ʾ��
				m_ClientItem.m_pMainWnd->
					ShowMsg(
					_T("��ʾ��") + 
					strMsg);

				//*�㲥�����뿪��Ϣ
				m_ClientItem.m_pMainWnd->
					SendClientsMsg(
					_T("ϵͳ�㲥��") + 
					strMsg);
				
				//*�㲥ɾ���ͻ���Ϣָ���дstrMsg
				Sleep(80);
				m_ClientItem.m_pMainWnd->SendClientsMsg(_T("Special0DELStart"));
					
				Sleep(80);
				
				//*ɾ���˿ͻ�������
				m_ClientItem.m_pMainWnd->
					RemoveClientFromArray(m_ClientItem);

				//�㲥һ��DELָ��
				m_ClientItem.m_pMainWnd->
					SendClientsMsg(strMsgAnother);

				Sleep(80);
				m_ClientItem.m_pMainWnd->SendClientsMsg(_T("Special0DELEnd"));

				//*��д�ͻ��б�
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

//����˼����߳�
DWORD WINAPI ListenThreadFunc(LPVOID pParam)
{
	CMy20150703ChatRoomServerDlg *pChatRoom = 
		(CMy20150703ChatRoomServerDlg *)pParam;
	ASSERT(pChatRoom != NULL);
	pChatRoom->m_ListenSock = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
	if (pChatRoom->m_ListenSock == INVALID_SOCKET)
	{
		AfxMessageBox(_T("�½�Socketʧ�ܣ�"));
		return FALSE;
	}

	int iPort = pChatRoom->GetDlgItemInt(IDC_EDIT_LOCAL_PORT);
	if (iPort <= 0 || iPort > 65535)
	{
		AfxMessageBox(_T("��������ʵĶ˿ڣ�1 - 65535"));
		goto __Error_End;
	}

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY;
	service.sin_port = htons(iPort);

	if (bind(pChatRoom->m_ListenSock, (sockaddr*)&service, sizeof(sockaddr_in)) 
		== SOCKET_ERROR)
	{
		AfxMessageBox(_T("�󶨶˿�ʧ�ܣ�"));
		goto __Error_End;
	}

	if(listen(pChatRoom->m_ListenSock, 5) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("����ʧ�ܣ�"));
		goto __Error_End;
	}

	pChatRoom->ShowMsg(GetSystemTime(TRUE) + 
		_T("�����������ɹ����Ѿ���ʼ�����ͻ��ˡ�"));

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
			//��һЩ������������
			//����CClientItem���͵Ķ������ڴ���ͻ�����Ϣ����������Զ���������Ŀ
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