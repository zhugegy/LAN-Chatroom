#pragma once
#include "windows.h"

//�ڲ���������
BOOL SOCKET_Select(SOCKET hSocket, int nTimeOut = 100, BOOL bRead = FALSE);
DWORD WINAPI ConnectThreadFunc(LPVOID pParam);
