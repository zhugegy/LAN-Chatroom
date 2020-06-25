
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
�Զ����Ա��������
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
�Զ����Ա��������#����##########################################################
##############################################################################*/

/*##############################################################################
�Զ����Ա����
##############################################################################*/
	SOCKET m_ListenSock;	//����socket
	HANDLE m_hListenThread;		//�����߳�
	CArray <CClientItem, CClientItem> m_ClientArray; //��������ڴ���ͻ�����Ϣ
	BOOL m_bShutDown;
/*##############################################################################
�Զ����Ա��������#����##########################################################
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
