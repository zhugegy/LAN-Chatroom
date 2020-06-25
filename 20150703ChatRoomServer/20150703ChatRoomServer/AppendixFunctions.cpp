#include "stdafx.h"
#include "AppendixFunctions.h"

//��������IP��ַ�������������IDC_STATIC_LOCAL_IP����ʾ
CString GetLocalIP()
{
	char szHostName[128];
	const char *pszAddr;
	struct hostent *pHost; 
	CString output = _T("����IP��ַ�ǣ�");
	if(gethostname(szHostName, 128) == 0)
	{
		pHost = gethostbyname(szHostName); 
		for(int i = 0; pHost != NULL && pHost->h_addr_list[i] != NULL; i++) 
		{

			//��ÿһ��IP��ַ���д���
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
	GetLocalTime(&Time);//��õ�ǰ����ʱ��
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
