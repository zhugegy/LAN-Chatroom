#pragma once
#include "windows.h"

//内部函数声明
BOOL SOCKET_Select(SOCKET hSocket, int nTimeOut = 100, BOOL bRead = FALSE);
DWORD WINAPI ConnectThreadFunc(LPVOID pParam);
