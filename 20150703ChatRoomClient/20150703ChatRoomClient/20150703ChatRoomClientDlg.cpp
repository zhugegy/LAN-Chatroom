
// 20150703ChatRoomClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "20150703ChatRoomClient.h"
#include "20150703ChatRoomClientDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "Inc.h"
#include "Client.h"
#include <vector>
#include "AppendixFunctions.h"
#include <time.h> 
#include <stdlib.h>

using namespace std;

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


// CMy20150703ChatRoomClientDlg dialog



CMy20150703ChatRoomClientDlg::CMy20150703ChatRoomClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy20150703ChatRoomClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
/*##############################################################################
���캯���Զ��岿��
##############################################################################*/
	m_ConnectSock = INVALID_SOCKET;
	m_hConnectThread = NULL;	
	m_bShutDown = FALSE;
	m_bIsConnected = FALSE;
	m_nClientListItemNumber = 0;
	m_bIsSpecial0_DEL = FALSE;
	m_bIsSpecial0_ADD = FALSE;
	//m_bIsKicked = FALSE;
/*##############################################################################
���캯���Զ��岿��#����##########################################################
##############################################################################*/
}

void CMy20150703ChatRoomClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_OUTPUT_BOX, m_MsgEdit);
	DDX_Control(pDX, IDC_LIST_CLIENT_CONTROL, m_ClientListControl);
}

BEGIN_MESSAGE_MAP(CMy20150703ChatRoomClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT_SERVER, &CMy20150703ChatRoomClientDlg::OnBnClickedButtonConnectServer)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT_SERVER, &CMy20150703ChatRoomClientDlg::OnBnClickedButtonDisconnectServer)
	ON_BN_CLICKED(IDC_BUTTON_SEND_MESSAGE, &CMy20150703ChatRoomClientDlg::OnBnClickedButtonSendMessage)
	ON_EN_CHANGE(IDC_EDIT_INPUT_BOX, &CMy20150703ChatRoomClientDlg::OnEnChangeEditInputBox)
	ON_EN_SETFOCUS(IDC_EDIT_INPUT_BOX, &CMy20150703ChatRoomClientDlg::OnEnSetfocusEditInputBox)
	ON_EN_SETFOCUS(IDC_EDIT_NICKNAME, &CMy20150703ChatRoomClientDlg::OnEnSetfocusEditNickname)
	ON_BN_CLICKED(IDC_BUTTON_EMPTY_OUTPUT_BOX, &CMy20150703ChatRoomClientDlg::OnBnClickedButtonEmptyOutputBox)
END_MESSAGE_MAP()


/*##############################################################################
�Զ����Ա��������
##############################################################################*/

//��ʼ������ClientListControl,������ʾ�ͻ�����Ϣ
BOOL CMy20150703ChatRoomClientDlg::InitClientListControl()
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
BOOL CMy20150703ChatRoomClientDlg::InitCustomizeSetting()
{
	//��д������IP��ַ
	SetDlgItemText(IDC_IPADDRESS_SERVER_IP, _T("192.168.1.0"));

	//��дĬ�϶˿�
	SetDlgItemText(IDC_EDIT_SERVER_PORT, _T("6789"));
	
	//��д�����Ĭ����Ϣ
	SetDlgItemText(IDC_EDIT_INPUT_BOX, _T("�ڴ�������Ϣ..."));

	//��дĬ���ǳ���Ϣ
	SetDlgItemText(IDC_EDIT_NICKNAME, GetRandomNickname());

	//�ı��������������������� accept 1000 * 1000 k of text
	m_MsgEdit.SetLimitText(1000 * 1000 * 1024);

	//�����ӭ��Ϣ
	ShowMsg(_T("��ӭʹ�������ҿͻ���!Ŀǰ�汾ͬһIP�࿪�ͻ���ʱ���п��ܳ���BUG��"));

	//����һЩ��ť
	EnableWindow(IDC_BUTTON_SEND_MESSAGE, FALSE);
	EnableWindow(IDC_BUTTON_DISCONNECT_SERVER, FALSE);
	return TRUE;
}

//���ڵļ�����÷�װ����
BOOL CMy20150703ChatRoomClientDlg::EnableWindow(UINT uID, BOOL bEnable)
{
	return GetDlgItem(uID)->EnableWindow(bEnable);
}

//�������¼����ʾ��Ϣ����Ҫ��richedit��һ����Ա���� m_MsgEdit
void CMy20150703ChatRoomClientDlg::ShowMsg(CString strMsg)
{
	CString strTextOutputBox;
	GetDlgItemText(IDC_EDIT_OUTPUT_BOX, strTextOutputBox);
	m_MsgEdit.SetSel(strTextOutputBox.GetLength(), 
		strTextOutputBox.GetLength());
	m_MsgEdit.ReplaceSel(strMsg+_T("\r\n"));
}

//�����ͻ��������߳�
void CMy20150703ChatRoomClientDlg::StopClient()
{
	m_bShutDown = TRUE;
	DWORD dwRet = WaitForSingleObject(m_hConnectThread, 1000);
	if (dwRet != WAIT_OBJECT_0)
	{
		TerminateThread(m_hConnectThread, -1);
		closesocket(m_ConnectSock);
	}
	m_hConnectThread = NULL;
	m_ConnectSock = INVALID_SOCKET;
	m_bShutDown = FALSE;
}

//���ݷ���˵�Special0ADD������ͻ�����Ϣ�б������Ŀ
void CMy20150703ChatRoomClientDlg::AddClientListControl(CString strClientInfo)
{
	//���ݱ�ʶ���ָ���Ϣ��
	vector<CString> vecSzTemp;
	CString char1 = _T("#");
	vecSzTemp = SplitCString(strClientInfo, char1);

	//�Ѷ�Ӧ����Ϣ����ͻ�����Ϣ�б�
	int nRow = m_ClientListControl.InsertItem(m_nClientListItemNumber, 
		vecSzTemp[0]);
	m_ClientListControl.SetItemText(nRow, 1, vecSzTemp[1]);
	m_ClientListControl.SetItemText(nRow, 2, vecSzTemp[2]);
	m_ClientListControl.SetItemText(nRow, 3, vecSzTemp[3]);

	//�û�������1
	m_nClientListItemNumber++;
}

//���ݷ���˵�Special0DEL������ͻ�����Ϣ�б�ɾ����Ŀ
void CMy20150703ChatRoomClientDlg::DeleteClientListControl(
	CString strClientInfo)
{
	//���ݱ�ʶ���ָ���Ϣ��
	vector<CString> vecSzTemp;
	CString char1 = _T("#");
	vecSzTemp = SplitCString(strClientInfo, char1);

	//�Ƚ��������������Ƿ����
	for (int i = 0; m_ClientListControl.GetItemCount(); i++)
	{
		if (vecSzTemp[0] == m_ClientListControl.GetItemText(i, 0) && 
			vecSzTemp[1] == m_ClientListControl.GetItemText(i, 1) && 
			vecSzTemp[2] == m_ClientListControl.GetItemText(i, 2) && 
			vecSzTemp[3] == m_ClientListControl.GetItemText(i, 3))
		{
			//������ϣ�ɾ����һ��Item�������û�������1
			m_ClientListControl.DeleteItem(i);
			m_nClientListItemNumber--;
			break;
		}
	}
}

//����ǳ�����
CString CMy20150703ChatRoomClientDlg::GetRandomNickname()
{
	CString strNicknameList[10] = {
		_T("��˿��ʿ"), 
		_T("��ɮ"), 
		_T("�����"), 
		_T("��˽�"), 
		_T("ɳɮ"), 
		_T("����"),
		_T("����"), 
		_T("��С��"),
		_T("С����"),
		_T("����")
	};
	return strNicknameList[rand()%10];
}

/*##############################################################################
�Զ����Ա��������#����##########################################################
##############################################################################*/\

// CMy20150703ChatRoomClientDlg message handlers

BOOL CMy20150703ChatRoomClientDlg::OnInitDialog()
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
	srand((unsigned) time(NULL));

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

void CMy20150703ChatRoomClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMy20150703ChatRoomClientDlg::OnPaint()
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
HCURSOR CMy20150703ChatRoomClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*##############################################################################
�ؼ���Ϣ��Ӧ
##############################################################################*/

//���ӷ�������ť������һ�������߳�
void CMy20150703ChatRoomClientDlg::OnBnClickedButtonConnectServer()
{
	// TODO: Add your control notification handler code here
	m_hConnectThread = CreateThread(NULL, 0, ConnectThreadFunc, this, 0, NULL);
	EnableWindow(IDC_BUTTON_DISCONNECT_SERVER, TRUE);
	EnableWindow(IDC_BUTTON_CONNECT_SERVER, FALSE);
	EnableWindow(IDC_EDIT_NICKNAME, FALSE);
	m_ClientListControl.DeleteAllItems();
}


//�Ͽ����Ӱ�ť
void CMy20150703ChatRoomClientDlg::OnBnClickedButtonDisconnectServer()
{
	// TODO: Add your control notification handler code here
	StopClient();
	ShowMsg(_T("ϵͳ��Ϣ���Ͽ����ӳɹ���"));

	m_bIsConnected = FALSE;
	EnableWindow(IDC_BUTTON_DISCONNECT_SERVER, FALSE);
	EnableWindow(IDC_BUTTON_CONNECT_SERVER, TRUE);
	EnableWindow(IDC_EDIT_NICKNAME, TRUE);
	m_ClientListControl.DeleteAllItems();
}

//������Ϣ��ť
void CMy20150703ChatRoomClientDlg::OnBnClickedButtonSendMessage()
{
	// TODO: Add your control notification handler code here
	CString strMsg;
	GetDlgItemText(IDC_EDIT_INPUT_BOX, strMsg);

	if (m_bIsConnected)
	{
		//ͳһ�ӷ�����������Ϣ�����ط�������Ϣ������������ʾ�����Ǵӷ������յ�
		//CString strTmp = _T("���ؿͻ��ˣ�>") + strMsg;
		//ShowMsg(strTmp);
		int iSend = send(m_ConnectSock, (char *)strMsg.GetBuffer(), 
			strMsg.GetLength()*sizeof(TCHAR), 0);
		strMsg.ReleaseBuffer();
		SetDlgItemText(IDC_EDIT_INPUT_BOX, _T(""));
	}
	else
	{
		AfxMessageBox(_T("�������ӷ�������"));
	}
}

//���͹㲥��ť�����ı����������ݽ��úͼ���
void CMy20150703ChatRoomClientDlg::OnEnChangeEditInputBox()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString strMsg;
	GetDlgItemText(IDC_EDIT_INPUT_BOX, strMsg);
	if (strMsg.IsEmpty())
	{
		EnableWindow(IDC_BUTTON_SEND_MESSAGE, FALSE);
	}
	else
	{
		EnableWindow(IDC_BUTTON_SEND_MESSAGE, TRUE);
	}
}

//�ڴ�������Ϣ...��ʾ ���Զ���ʧ
void CMy20150703ChatRoomClientDlg::OnEnSetfocusEditInputBox()
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	GetDlgItemText(IDC_EDIT_INPUT_BOX, strTemp);
	if (strTemp == _T("�ڴ�������Ϣ..."))
	{
		SetDlgItemText(IDC_EDIT_INPUT_BOX, _T(""));
	}
}

//�ǳ� ���Զ���ʧ
void CMy20150703ChatRoomClientDlg::OnEnSetfocusEditNickname()
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	GetDlgItemText(IDC_EDIT_NICKNAME, strTemp);
	if (strTemp == _T("С��"))
	{
		SetDlgItemText(IDC_EDIT_NICKNAME, _T(""));
	}
}

//������
void CMy20150703ChatRoomClientDlg::OnBnClickedButtonEmptyOutputBox()
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

