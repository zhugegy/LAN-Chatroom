
// 20150703ChatRoomClientDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"
#include "Inc.h"

// CMy20150703ChatRoomClientDlg dialog
class CMy20150703ChatRoomClientDlg : public CDialogEx
{
// Construction
public:
	CMy20150703ChatRoomClientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MY20150703CHATROOMCLIENT_DIALOG };

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
	void SendClientsMsg(CString strMsg, CClientItem *pNotSend = NULL);
	void StopClient();
	void AddClientListControl(CString strClientInfo);
	void DeleteClientListControl(CString strClientInfo);
	CString GetRandomNickname();
/*##############################################################################
�Զ����Ա��������#����##########################################################
##############################################################################*/

/*##############################################################################
�Զ����Ա����
##############################################################################*/
	HANDLE m_hConnectThread;
	SOCKET m_ConnectSock;	//�洢�ӷ���˷����ͻ��˵���Ϣ
	BOOL m_bShutDown;
	BOOL m_bIsConnected;
	int m_nClientListItemNumber;
	BOOL m_bIsSpecial0_ADD;
	BOOL m_bIsSpecial0_DEL;
	//BOOL m_bIsKicked;
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
	CEdit m_MsgEdit;
	CListCtrl m_ClientListControl;
	afx_msg void OnBnClickedButtonConnectServer();
	afx_msg void OnBnClickedButtonDisconnectServer();
	afx_msg void OnBnClickedButtonSendMessage();
	afx_msg void OnEnChangeEditInputBox();
	afx_msg void OnEnSetfocusEditInputBox();
	afx_msg void OnEnSetfocusEditNickname();
	afx_msg void OnBnClickedButtonEmptyOutputBox();

	afx_msg void OnEnChangeEditOutputBox();
	afx_msg void OnEnSetfocusEditOutputBox();
};
