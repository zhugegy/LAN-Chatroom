#include "stdafx.h"
#include "AppendixFunctions.h"

//遍历本机IP地址并输出，用于在IDC_STATIC_LOCAL_IP中显示
CString GetLocalIP()
{
	char szHostName[128];
	const char *pszAddr;
	struct hostent *pHost; 
	CString output = _T("本机IP地址是：");
	if(gethostname(szHostName, 128) == 0)
	{
		pHost = gethostbyname(szHostName); 
		for(int i = 0; pHost != NULL && pHost->h_addr_list[i] != NULL; i++) 
		{

			//对每一个IP地址进行处理
			pszAddr=inet_ntoa (*(struct in_addr *)pHost->h_addr_list[i]);
			output = output + (CString)pszAddr + _T("  ");
			//output.AppendFormat(_T("%s "), pszAddr);
		}
	}
	return output;
}

CString GetSystemTime(BOOL IsFullFormat)
{
	SYSTEMTIME Time={0};
	GetLocalTime(&Time);//获得当前本地时间
	CString strTime;
	if (IsFullFormat == TRUE)
	{
		strTime.Format(_T("%d-%02d-%02d %02d:%02d:%02d "), Time.wYear, 
			Time.wMonth, Time.wDay, Time.wHour, Time.wMinute, Time.wSecond);
	}
	else
	{
		strTime.Format(_T("%02d:%02d:%02d "), Time.wHour, Time.wMinute, 
			Time.wSecond);
	}
	return strTime;
}
