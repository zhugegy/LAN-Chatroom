#pragma once
#include "windows.h"
#include "resource.h"

//���ļ��ڲ���������
DWORD WINAPI ListenThreadFunc(LPVOID pParam);
DWORD WINAPI ClientThreadProc(LPVOID lpParameter);
BOOL SOCKET_Select(SOCKET hSocket, int nTimeOut, BOOL bRead);