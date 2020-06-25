#pragma once
#include "windows.h"
#include "resource.h"

//本文件内部函数声明
DWORD WINAPI ListenThreadFunc(LPVOID pParam);
DWORD WINAPI ClientThreadProc(LPVOID lpParameter);
BOOL SOCKET_Select(SOCKET hSocket, int nTimeOut, BOOL bRead);