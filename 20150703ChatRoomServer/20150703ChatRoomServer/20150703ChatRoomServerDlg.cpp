
// 20150703ChatRoomServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "20150703ChatRoomServer.h"
#include "20150703ChatRoomServerDlg.h"
#include "afxdialogex.h"
#include "AppendixFunctions.h"
#include "Server.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMy20150703ChatRoomServerDlg dialog

CMy20150703ChatRoomServerDlg::CMy20150703ChatRoomServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy20150703ChatRoomServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
/*##############################################################################
���캯���Զ��岿��
##############################################################################*/
	m_ListenSock = INVALID_SOCKET;
	m_hListenThread = NULL;	
	m_bShutDown = FALSE;
/*##############################################################################
���캯���Զ��岿��#����##########################################################
##############################################################################*/
}

void CMy20150703ChatRoomServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLIENT_CONTROL, m_ClientListControl);
	DDX_Control(pDX, IDC_EDIT_OUTPUT_BOX, m_MsgEdit);
}

BEGIN_MESSAGE_MAP(CMy20150703ChatRoomServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_SERVER, &CMy20150703ChatRoomServerDlg::OnBnClickedButtonOpenServer)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_SERVER, &CMy20150703ChatRoomServerDlg::OnBnClickedButtonCloseServer)
	ON_BN_CLICKED(IDC_BUTTON_SEND_BROADCAST, &CMy20150703ChatRoomServerDlg::OnBnClickedButtonSendBroadcast)
	ON_EN_CHANGE(IDC_EDIT_SERVER_BROADCAST, &CMy20150703ChatRoomServerDlg::OnEnChangeEditServerBroadcast)
	ON_BN_CLICKED(IDC_BUTTON_EMPTY_OUTPUT_BOX, &CMy20150703ChatRoomServerDlg::OnBnClickedButtonEmptyOutputBox)
	ON_BN_CLICKED(IDC_BUTTON_KICK_CLIENT, &CMy20150703ChatRoomServerDlg::OnBnClickedButtonKickClient)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_SILENCE, &CMy20150703ChatRoomServerDlg::OnBnClickedButtonClientSilence)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_NOSILENCE, &CMy20150703ChatRoomServerDlg::OnBnClickedButtonClientNosilence)
END_MESSAGE_MAP()


/*##############################################################################
�Զ����Ա��������
##############################################################################*/

//��ʼ������ClientListControl,������ʾ�ͻ�����Ϣ
BOOL CMy20150703ChatRoomServerDlg::InitClientListControl()
{
	
	//������չ���
	//*��õ�ǰ���
	DWORD dwStyle = m_ClientListControl.GetExtendedStyle();
	//*ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
	dwStyle |= LVS_EX_FULLROWSELECT;
	//*�����ߣ�ֻ������report����listctrl��
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_SHOWSELALWAYS;
	//*������չ���
	m_ClientListControl.SetExtendedStyle(dwStyle); 

	//����column��Ϣ
	m_ClientListControl.InsertColumn(0, _T("�ǳ�"), LVCFMT_LEFT, 60);
	m_ClientListControl.InsertColumn(1, _T("״̬"), LVCFMT_LEFT, 40);
	m_ClientListControl.InsertColumn(2, _T("IP"), LVCFMT_LEFT, 95);
	m_ClientListControl.InsertColumn(3, _T("��¼ʱ��"), LVCFMT_LEFT, 105);

	return TRUE;
}

//��д����Ĭ�ϳ�ʼ��Ϣ
BOOL CMy20150703ChatRoomServerDlg::InitCustomizeSetting()
{
	//��д����IP��ַ
	SetDlgItemText(IDC_STATIC_LOCAL_IP, GetLocalIP());

	//��дĬ�϶˿�
	SetDlgItemText(IDC_EDIT_LOCAL_PORT, _T("6789"));

	//�����ӭ��Ϣ
	ShowMsg(_T("��ӭʹ�������ҷ����!"));

	//����һЩ��ť
	EnableWindow(IDC_BUTTON_SEND_BROADCAST, FALSE);
	EnableWindow(IDC_BUTTON_CLOSE_SERVER, FALSE);
	return TRUE;
}

//���ڵļ�����÷�װ����
BOOL CMy20150703ChatRoomServerDlg::EnableWindow(UINT uID, BOOL bEnable)
{
	return GetDlgItem(uID)->EnableWindow(bEnable);
}

//�������¼����ʾ��Ϣ����Ҫ��richedit��һ����Ա���� m_MsgEdit
void CMy20150703ChatRoomServerDlg::ShowMsg(CString strMsg)
{
	CString strTextOutputBox;
	GetDlgItemText(IDC_EDIT_OUTPUT_BOX, strTextOutputBox);
	m_MsgEdit.SetSel(strTextOutputBox.GetLength(), 
		strTextOutputBox.GetLength());
	m_MsgEdit.ReplaceSel(strMsg+_T("\r\n"));
}

//������Ƴ��ͻ�
void CMy20150703ChatRoomServerDlg::RemoveClientFromArray(CClientItem in_Item)
{
	for (int idx = 0; idx < m_ClientArray.GetCount(); idx++)
	{
		CClientItem tItem = m_ClientArray.GetAt(idx);
		if (tItem.m_Socket == in_Item.m_Socket && 
			tItem.hThread == in_Item.hThread && 
			tItem.m_strIp == in_Item.m_strIp)
		{
				m_ClientArray.RemoveAt(idx);
		}
	}
}

//������������ͻ��˷�����Ϣ
void CMy20150703ChatRoomServerDlg::SendClientsMsg(CString strMsg, 
												  CClientItem *pNotSend)
{
	TCHAR szBuf[MAX_BUF_SIZE] = {0};
	_tcscpy_s(szBuf, MAX_BUF_SIZE, strMsg);
	for (INT_PTR idx = 0; idx < m_ClientArray.GetCount(); idx++ )
	{
		if (!pNotSend || 
			pNotSend->m_Socket != m_ClientArray.GetAt(idx).m_Socket
			 || pNotSend->hThread != m_ClientArray.GetAt(idx).hThread || 
			 pNotSend->m_strIp != m_ClientArray.GetAt(idx).m_strIp)
		{
			send(m_ClientArray.GetAt(idx).m_Socket, (char *)szBuf, 
				_tcslen(szBuf)*sizeof(TCHAR), 0);
		}
	}
}

//�������1���ͻ��˷�����Ϣ
void CMy20150703ChatRoomServerDlg::SendClientsMsgSolo(CString strMsg, 
												  CClientItem *pSend)
{
	TCHAR szBuf[MAX_BUF_SIZE] = {0};
	_tcscpy_s(szBuf, MAX_BUF_SIZE, strMsg);
	for (INT_PTR idx = 0; idx < m_ClientArray.GetCount(); idx++ )
	{
		if (pSend->m_Socket == m_ClientArray.GetAt(idx).m_Socket && 
			pSend->hThread == m_ClientArray.GetAt(idx).hThread && 
			pSend->m_strIp == m_ClientArray.GetAt(idx).m_strIp)
		{
			send(m_ClientArray.GetAt(idx).m_Socket, (char *)szBuf, 
				_tcslen(szBuf)*sizeof(TCHAR), 0);
			break;
		}
	}
}

//��������˶������߳�
void CMy20150703ChatRoomServerDlg::StopServer()
{
	UINT nCount = m_ClientArray.GetCount();
	HANDLE *m_pHandles = new HANDLE[nCount+1];
	m_pHandles[0] = m_hListenThread;
	for (int idx = 0; idx < nCount; idx++) 
	{
		m_pHandles[idx+1] = m_ClientArray.GetAt(idx).hThread;
	}
	m_bShutDown = TRUE;
	DWORD dwRet = WaitForMultipleObjects(nCount+1, m_pHandles, TRUE, 1000);
	if (dwRet != WAIT_OBJECT_0) 
	{
		for(INT_PTR i = 0; i < m_ClientArray.GetCount(); i++) 
		{
			TerminateThread(m_ClientArray.GetAt(i).hThread, -1);
			closesocket(m_ClientArray.GetAt(i).m_Socket);
		}
		TerminateThread(m_hListenThread, -1);
		closesocket(m_ListenSock);
	}
	delete [] m_pHandles;
	m_hListenThread = NULL;
	m_ListenSock = INVALID_SOCKET;
	m_bShutDown = FALSE;
	ShowMsg(_T("�������ѹرա�"));
}

void CMy20150703ChatRoomServerDlg::EmptyClientListControl()
{
	m_ClientListControl.DeleteAllItems();
}

void CMy20150703ChatRoomServerDlg::FillInClientListControl()
{
	for (INT_PTR idx = 0; idx < m_ClientArray.GetCount(); idx++ )
	{
		int nRow = m_ClientListControl.InsertItem(idx, 
			m_ClientArray.GetAt(idx).m_strNickname);

		switch (m_ClientArray.GetAt(idx).m_nStatus)
		{
		case 0:
			m_ClientListControl.SetItemText(nRow, 1, _T("����"));
			break;
		case 1:
			m_ClientListControl.SetItemText(nRow, 1, _T("����"));
			break;
		case 2:
			m_ClientListControl.SetItemText(nRow, 1, _T("����"));
			break;
		default:
			m_ClientListControl.SetItemText(nRow, 1, _T("δ֪"));
			break;
		}
		
		m_ClientListControl.SetItemText(nRow, 2, 
			m_ClientArray.GetAt(idx).m_strIp);

		m_ClientListControl.SetItemText(nRow, 3, 
			m_ClientArray.GetAt(idx).m_strLoginTime);
	}
}

CString CMy20150703ChatRoomServerDlg::GenerateADD_DELCode(int nStatus, int nNum)
{
	CString strMsg;
	switch (nStatus)
	{
	case 0:
		strMsg = 
			m_ClientArray[nNum].m_strNickname + 
			_T("#") + 
			_T("����") + 
			_T("#") +
			m_ClientArray[nNum].m_strIp + 
			_T("#") + 
			m_ClientArray[nNum].m_strLoginTime;
		break;
	case 1:
		strMsg = 
			m_ClientArray[nNum].m_strNickname + 
			_T("#") + 
			_T("����") + 
			_T("#") +
			m_ClientArray[nNum].m_strIp + 
			_T("#") + 
			m_ClientArray[nNum].m_strLoginTime;
		break;
	case 2:
		strMsg = 
			m_ClientArray[nNum].m_strNickname + 
			_T("#") + 
			_T("����") + 
			_T("#") +
			m_ClientArray[nNum].m_strIp + 
			_T("#") + 
			m_ClientArray[nNum].m_strLoginTime;
		break;						
	default:
		strMsg = 
			m_ClientArray[nNum].m_strNickname + 
			_T("#") + 
			_T("δ֪") + 
			_T("#") +
			m_ClientArray[nNum].m_strIp + 
			_T("#") + 
			m_ClientArray[nNum].m_strLoginTime;
		break;
	}
	return strMsg;
}
/*##############################################################################
�Զ����Ա��������#����##########################################################
##############################################################################*/

// CMy20150703ChatRoomServerDlg message handlers

BOOL CMy20150703ChatRoomServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
/*##############################################################################
�Զ����ʼ������
##############################################################################*/
	if (!InitClientListControl())
	{
		AfxMessageBox(_T("�ͻ�����Ϣ�б��ʼ��ʧ��"));
	}

	if (!InitCustomizeSetting())
	{
		AfxMessageBox(_T("�Ի�����Ϣ��ʼ��ʧ��"));
	}
	

/*##############################################################################
�Զ����ʼ������#����############################################################
##############################################################################*/
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMy20150703ChatRoomServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMy20150703ChatRoomServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMy20150703ChatRoomServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/*##############################################################################
�ؼ���Ϣ��Ӧ
##############################################################################*/

//���������������ť
void CMy20150703ChatRoomServerDlg::OnBnClickedButtonOpenServer()
{
	// TODO: Add your control notification handler code here
	m_hListenThread = CreateThread(NULL, 0, ListenThreadFunc, this, 0, NULL);
	EnableWindow(IDC_BUTTON_OPEN_SERVER, FALSE);
	EnableWindow(IDC_BUTTON_CLOSE_SERVER, TRUE);
}

//����رշ�������ť
void CMy20150703ChatRoomServerDlg::OnBnClickedButtonCloseServer()
{
	// TODO: Add your control notification handler code here
	StopServer();
	EnableWindow(IDC_BUTTON_OPEN_SERVER, TRUE);
	EnableWindow(IDC_BUTTON_CLOSE_SERVER, FALSE);
}

//�������ϵͳ�㲥��ť
void CMy20150703ChatRoomServerDlg::OnBnClickedButtonSendBroadcast()
{
	// TODO: Add your control notification handler code here
	CString strMsg;
	GetDlgItemText(IDC_EDIT_SERVER_BROADCAST, strMsg);
	
	strMsg = _T("��������>") + strMsg;
	ShowMsg(strMsg);
	SendClientsMsg(strMsg);
	
	SetDlgItemText(IDC_EDIT_SERVER_BROADCAST, _T(""));
}

//���͹㲥��ť�����ı����������ݽ��úͼ���
void CMy20150703ChatRoomServerDlg::OnEnChangeEditServerBroadcast()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString strMsg;
	GetDlgItemText(IDC_EDIT_SERVER_BROADCAST, strMsg);
	if (strMsg.IsEmpty())
	{
		EnableWindow(IDC_BUTTON_SEND_BROADCAST, FALSE);
	}
	else
	{
		EnableWindow(IDC_BUTTON_SEND_BROADCAST, TRUE);
	}
}

//������
void CMy20150703ChatRoomServerDlg::OnBnClickedButtonEmptyOutputBox()
{
	// TODO: Add your control notification handler code here
	m_MsgEdit.SetReadOnly(FALSE);
	m_MsgEdit.SetSel(0, -1);
	m_MsgEdit.Clear();
	m_MsgEdit.SetReadOnly(TRUE);
}

/*##############################################################################
�ؼ���Ϣ��Ӧ#����################################################################
##############################################################################*/

//���˼�
void CMy20150703ChatRoomServerDlg::OnBnClickedButtonKickClient()
{
	// TODO: Add your control notification handler code here
	CString strClientInfo[4];
	//��ȡlist�ռ佹��
	for (int i = 0; i < m_ClientListControl.GetItemCount(); i++)
	{
		if(m_ClientListControl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			strClientInfo[0] = m_ClientListControl.GetItemText(i, 0);//nickname
			strClientInfo[1] = m_ClientListControl.GetItemText(i, 1);//status
			strClientInfo[2] = m_ClientListControl.GetItemText(i, 2);//ip
			strClientInfo[3] = m_ClientListControl.GetItemText(i, 3);//logintime
			break;
		}
	}

	for (int i = 0; i < m_ClientArray.GetCount(); i++)
	{
		if (strClientInfo[0] == m_ClientArray[i].m_strNickname &&
			strClientInfo[2] == m_ClientArray[i].m_strIp && 
			strClientInfo[3] == m_ClientArray[i].m_strLoginTime)
		{
			SendClientsMsgSolo(_T("Special0KICK"), &m_ClientArray[i]);
			ShowMsg(
				_T("��ʾ���û� ") + 
				strClientInfo[0] + 
				_T("��") + 
				strClientInfo[2] + 
				_T("�� �ѱ��߳���������"));
			Sleep(80);
			break;
		}
		if (i == m_ClientArray.GetCount() - 1)
		{
			ShowMsg(_T("��ʾ������û�гɹ���"));
		}
	}
}

//���԰�ť
void CMy20150703ChatRoomServerDlg::OnBnClickedButtonClientSilence()
{
	// TODO: Add your control notification handler code here
	CString strClientInfo[4];
	CString strMsg;
	//��ȡlist�ռ佹��
	for (int i = 0; i < m_ClientListControl.GetItemCount(); i++)
	{
		if(m_ClientListControl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			strClientInfo[0] = m_ClientListControl.GetItemText(i, 0);//nickname
			strClientInfo[1] = m_ClientListControl.GetItemText(i, 1);//status
			strClientInfo[2] = m_ClientListControl.GetItemText(i, 2);//ip
			strClientInfo[3] = m_ClientListControl.GetItemText(i, 3);//logintime
			m_ClientListControl.SetItemText(i, 1, _T("����"));
			break;
		}
	}

	for (int i = 0; i < m_ClientArray.GetCount(); i++)
	{
		if (strClientInfo[0] == m_ClientArray[i].m_strNickname &&
			strClientInfo[2] == m_ClientArray[i].m_strIp && 
			strClientInfo[3] == m_ClientArray[i].m_strLoginTime)
		{
			strMsg = GenerateADD_DELCode(m_ClientArray[i].m_nStatus, i);

			
			ShowMsg(
				_T("��ʾ���û� ") + 
				strClientInfo[0] + 
				_T("��") + 
				strClientInfo[2] + 
				_T("�� �ѱ����ԡ�"));
			Sleep(80);
			//*�㲥ɾ���ͻ���Ϣָ��
			/*Sleep(80);
			SendClientsMsg(_T("Special0DELStart"));

			//�㲥һ��DELָ��
			
			Sleep(80);
			SendClientsMsg(strMsg);

			Sleep(80);
			SendClientsMsg(_T("Special0DELEnd"));

			Sleep(80);
			SendClientsMsg(_T("Special0ADDStart"));

			Sleep(80);
			strMsg = GenerateADD_DELCode(m_ClientArray[i].m_nStatus, i);
			SendClientsMsg(strMsg);

			Sleep(80);
			SendClientsMsg(_T("Special0ADDEnd"));*/
			
			Sleep(80);
			SendClientsMsgSolo(_T("ϵͳ��ʾ�����ѱ����ԡ�"), &m_ClientArray[i]);
			
			Sleep(80);
			SendClientsMsg(
				_T("ϵͳ�㲥���û� ") + 
				m_ClientArray[i].m_strNickname + 
				_T("��") + 
				m_ClientArray[i].m_strIp + 
				_T("�� �ѱ����ԡ�"));

			m_ClientArray[i].m_nStatus = 2;		//��Ϊ����״̬
			
			break;
		}
		if (i == m_ClientArray.GetCount() - 1)
		{
			ShowMsg(_T("��ʾ������û�гɹ���"));
		}
	}
}

//������԰�ť
void CMy20150703ChatRoomServerDlg::OnBnClickedButtonClientNosilence()
{
	// TODO: Add your control notification handler code here
	CString strClientInfo[4];
	CString strMsg;
	//��ȡlist�ռ佹��
	for (int i = 0; i < m_ClientListControl.GetItemCount(); i++)
	{
		if(m_ClientListControl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			strClientInfo[0] = m_ClientListControl.GetItemText(i, 0);//nickname
			strClientInfo[1] = m_ClientListControl.GetItemText(i, 1);//status
			strClientInfo[2] = m_ClientListControl.GetItemText(i, 2);//ip
			strClientInfo[3] = m_ClientListControl.GetItemText(i, 3);//logintime
			m_ClientListControl.SetItemText(i, 1, _T("����"));
			break;
		}
	}

	for (int i = 0; i < m_ClientArray.GetCount(); i++)
	{
		if (strClientInfo[0] == m_ClientArray[i].m_strNickname &&
			strClientInfo[2] == m_ClientArray[i].m_strIp && 
			strClientInfo[3] == m_ClientArray[i].m_strLoginTime)
		{
			strMsg = GenerateADD_DELCode(m_ClientArray[i].m_nStatus, i);

			m_ClientArray[i].m_nStatus = 1;;;		//��Ϊ����״̬
			ShowMsg(
				_T("��ʾ���û� ") + 
				strClientInfo[0] + 
				_T("��") + 
				strClientInfo[2] + 
				_T("�� �ѱ�������ԡ�"));
			Sleep(80);
			//*�㲥ɾ���ͻ���Ϣָ��
			/*Sleep(80);
			SendClientsMsg(_T("Special0DELStart"));

			//�㲥һ��DELָ��
			
			Sleep(80);
			SendClientsMsg(strMsg);

			Sleep(80);
			SendClientsMsg(_T("Special0DELEnd"));

			Sleep(80);
			SendClientsMsg(_T("Special0ADDStart"));

			Sleep(80);
			strMsg = GenerateADD_DELCode(m_ClientArray[i].m_nStatus, i);
			SendClientsMsg(strMsg);

			Sleep(80);
			SendClientsMsg(_T("Special0ADDEnd"));*/
			
			Sleep(80);
			SendClientsMsgSolo(_T("ϵͳ��ʾ�����ѱ�������ԡ�"), 
				&m_ClientArray[i]);
			
			Sleep(80);
			SendClientsMsg(
				_T("ϵͳ�㲥���û� ") + 
				m_ClientArray[i].m_strNickname + 
				_T("��") + 
				m_ClientArray[i].m_strIp + 
				_T("�� �ѱ�������ԡ�"));
			
			break;
		}
		if (i == m_ClientArray.GetCount() - 1)
		{
			ShowMsg(_T("��ʾ���������û�гɹ���"));
		}
	}
}
