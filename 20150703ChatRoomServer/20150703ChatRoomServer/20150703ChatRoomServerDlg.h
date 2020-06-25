
// 20150703ChatRoomServerDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "AppendixFunctions.h"
#include "resource.h"

// CMy20150703ChatRoomServerDlg dialog
class CMy20150703ChatRoomServerDlg : public CDialogEx
{
// Construction
public:
	CMy20150703ChatRoomServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MY20150703CHATROOMSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

/*##############################################################################
自定义成员函数声明
##############################################################################*/
public:
	BOOL InitClientListControl();
	BOOL InitCustomizeSetting();
	BOOL EnableWindow(UINT uID, BOOL bEnable = TRUE);
	void ShowMsg(CString strMsg);
	void RemoveClientFromArray(CClientItem in_Item);
	void SendClientsMsg(CString strMsg, CClientItem *pNotSend = NULL);
	void SendClientsMsgSolo(CString strMsg, CClientItem *pSend);
	void StopServer();
	void EmptyClientListControl();
	void FillInClientListControl();
	CString GenerateADD_DELCode(int nStatus, int nNum);
/*##############################################################################
自定义成员函数声明#结束##########################################################
##############################################################################*/

/*##############################################################################
自定义成员变量
##############################################################################*/
	SOCKET m_ListenSock;	//监听socket
	HANDLE m_hListenThread;		//监听线程
	CArray <CClientItem, CClientItem> m_ClientArray; //服务端用于储存客户端信息
	BOOL m_bShutDown;
/*##############################################################################
自定义成员变量声明#结束##########################################################
##############################################################################*/
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ClientListControl;
	CEdit m_MsgEdit;
	afx_msg void OnBnClickedButtonOpenServer();
	afx_msg void OnBnClickedButtonCloseServer();
	afx_msg void OnBnClickedButtonSendBroadcast();
	afx_msg void OnEnChangeEditServerBroadcast();
	afx_msg void OnBnClickedButtonEmptyOutputBox();
	afx_msg void OnBnClickedButtonKickClient();
	afx_msg void OnBnClickedButtonClientSilence();
	afx_msg void OnBnClickedButtonClientNosilence();
};
