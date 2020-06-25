
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
构造函数自定义部分
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
构造函数自定义部分#结束##########################################################
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
自定义成员函数定义
##############################################################################*/

//初始化设置ClientListControl,用于显示客户端信息
BOOL CMy20150703ChatRoomClientDlg::InitClientListControl()
{
	
	//设置扩展风格
	//*获得当前风格
	DWORD dwStyle = m_ClientListControl.GetExtendedStyle();
	//*选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_FULLROWSELECT;
	//*网格线（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_SHOWSELALWAYS;
	//*设置扩展风格
	m_ClientListControl.SetExtendedStyle(dwStyle); 

	//设置column信息
	m_ClientListControl.InsertColumn(0, _T("昵称"), LVCFMT_LEFT, 60);
	m_ClientListControl.InsertColumn(1, _T("状态"), LVCFMT_LEFT, 40);
	m_ClientListControl.InsertColumn(2, _T("IP"), LVCFMT_LEFT, 95);
	m_ClientListControl.InsertColumn(3, _T("登录时间"), LVCFMT_LEFT, 105);
	
	return TRUE;
}

//填写界面默认初始信息
BOOL CMy20150703ChatRoomClientDlg::InitCustomizeSetting()
{
	//填写服务器IP地址
	SetDlgItemText(IDC_IPADDRESS_SERVER_IP, _T("192.168.1.0"));

	//填写默认端口
	SetDlgItemText(IDC_EDIT_SERVER_PORT, _T("6789"));
	
	//填写输入框默认信息
	SetDlgItemText(IDC_EDIT_INPUT_BOX, _T("在此输入信息..."));

	//填写默认昵称信息
	SetDlgItemText(IDC_EDIT_NICKNAME, GetRandomNickname());

	//文本输出框最大文字容量设置 accept 1000 * 1000 k of text
	m_MsgEdit.SetLimitText(1000 * 1000 * 1024);

	//输出欢迎信息
	ShowMsg(_T("欢迎使用聊天室客户端!目前版本同一IP多开客户端时，有可能出现BUG。"));

	//禁用一些按钮
	EnableWindow(IDC_BUTTON_SEND_MESSAGE, FALSE);
	EnableWindow(IDC_BUTTON_DISCONNECT_SERVER, FALSE);
	return TRUE;
}

//窗口的激活禁用封装函数
BOOL CMy20150703ChatRoomClientDlg::EnableWindow(UINT uID, BOOL bEnable)
{
	return GetDlgItem(uID)->EnableWindow(bEnable);
}

//在聊天记录里显示信息，需要把richedit绑定一个成员变量 m_MsgEdit
void CMy20150703ChatRoomClientDlg::ShowMsg(CString strMsg)
{
	CString strTextOutputBox;
	GetDlgItemText(IDC_EDIT_OUTPUT_BOX, strTextOutputBox);
	m_MsgEdit.SetSel(strTextOutputBox.GetLength(), 
		strTextOutputBox.GetLength());
	m_MsgEdit.ReplaceSel(strMsg+_T("\r\n"));
}

//结束客户端链接线程
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

//根据服务端的Special0ADD命令，给客户端信息列表添加条目
void CMy20150703ChatRoomClientDlg::AddClientListControl(CString strClientInfo)
{
	//根据标识符分割信息串
	vector<CString> vecSzTemp;
	CString char1 = _T("#");
	vecSzTemp = SplitCString(strClientInfo, char1);

	//把对应的信息填入客户端信息列表
	int nRow = m_ClientListControl.InsertItem(m_nClientListItemNumber, 
		vecSzTemp[0]);
	m_ClientListControl.SetItemText(nRow, 1, vecSzTemp[1]);
	m_ClientListControl.SetItemText(nRow, 2, vecSzTemp[2]);
	m_ClientListControl.SetItemText(nRow, 3, vecSzTemp[3]);

	//用户数量加1
	m_nClientListItemNumber++;
}

//根据服务端的Special0DEL命令，给客户端信息列表删除条目
void CMy20150703ChatRoomClientDlg::DeleteClientListControl(
	CString strClientInfo)
{
	//根据标识符分割信息串
	vector<CString> vecSzTemp;
	CString char1 = _T("#");
	vecSzTemp = SplitCString(strClientInfo, char1);

	//比较所有四项数据是否符合
	for (int i = 0; m_ClientListControl.GetItemCount(); i++)
	{
		if (vecSzTemp[0] == m_ClientListControl.GetItemText(i, 0) && 
			vecSzTemp[1] == m_ClientListControl.GetItemText(i, 1) && 
			vecSzTemp[2] == m_ClientListControl.GetItemText(i, 2) && 
			vecSzTemp[3] == m_ClientListControl.GetItemText(i, 3))
		{
			//如果符合，删除这一个Item，并把用户数量减1
			m_ClientListControl.DeleteItem(i);
			m_nClientListItemNumber--;
			break;
		}
	}
}

//随机昵称生成
CString CMy20150703ChatRoomClientDlg::GetRandomNickname()
{
	CString strNicknameList[10] = {
		_T("屌丝男士"), 
		_T("唐僧"), 
		_T("孙悟空"), 
		_T("猪八戒"), 
		_T("沙僧"), 
		_T("赵四"),
		_T("刘能"), 
		_T("宋小宝"),
		_T("小沈阳"),
		_T("大鹏")
	};
	return strNicknameList[rand()%10];
}

/*##############################################################################
自定义成员函数定义#结束##########################################################
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
自定义初始化过程
##############################################################################*/
	srand((unsigned) time(NULL));

	if (!InitClientListControl())
	{
		AfxMessageBox(_T("客户端信息列表初始化失败"));
	}

	if (!InitCustomizeSetting())
	{
		AfxMessageBox(_T("对话框信息初始化失败"));
	}


/*##############################################################################
自定义初始化过程#结束############################################################
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
控件消息响应
##############################################################################*/

//连接服务器按钮，开启一个连接线程
void CMy20150703ChatRoomClientDlg::OnBnClickedButtonConnectServer()
{
	// TODO: Add your control notification handler code here
	m_hConnectThread = CreateThread(NULL, 0, ConnectThreadFunc, this, 0, NULL);
	EnableWindow(IDC_BUTTON_DISCONNECT_SERVER, TRUE);
	EnableWindow(IDC_BUTTON_CONNECT_SERVER, FALSE);
	EnableWindow(IDC_EDIT_NICKNAME, FALSE);
	m_ClientListControl.DeleteAllItems();
}


//断开连接按钮
void CMy20150703ChatRoomClientDlg::OnBnClickedButtonDisconnectServer()
{
	// TODO: Add your control notification handler code here
	StopClient();
	ShowMsg(_T("系统信息：断开连接成功！"));

	m_bIsConnected = FALSE;
	EnableWindow(IDC_BUTTON_DISCONNECT_SERVER, FALSE);
	EnableWindow(IDC_BUTTON_CONNECT_SERVER, TRUE);
	EnableWindow(IDC_EDIT_NICKNAME, TRUE);
	m_ClientListControl.DeleteAllItems();
}

//发送消息按钮
void CMy20150703ChatRoomClientDlg::OnBnClickedButtonSendMessage()
{
	// TODO: Add your control notification handler code here
	CString strMsg;
	GetDlgItemText(IDC_EDIT_INPUT_BOX, strMsg);

	if (m_bIsConnected)
	{
		//统一从服务器接收信息，本地发出的信息，不是立即显示，而是从服务器收到
		//CString strTmp = _T("本地客户端：>") + strMsg;
		//ShowMsg(strTmp);
		int iSend = send(m_ConnectSock, (char *)strMsg.GetBuffer(), 
			strMsg.GetLength()*sizeof(TCHAR), 0);
		strMsg.ReleaseBuffer();
		SetDlgItemText(IDC_EDIT_INPUT_BOX, _T(""));
	}
	else
	{
		AfxMessageBox(_T("请先连接服务器。"));
	}
}

//发送广播按钮根据文本框有无内容禁用和激活
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

//在此输入信息...提示 的自动消失
void CMy20150703ChatRoomClientDlg::OnEnSetfocusEditInputBox()
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	GetDlgItemText(IDC_EDIT_INPUT_BOX, strTemp);
	if (strTemp == _T("在此输入信息..."))
	{
		SetDlgItemText(IDC_EDIT_INPUT_BOX, _T(""));
	}
}

//昵称 的自动消失
void CMy20150703ChatRoomClientDlg::OnEnSetfocusEditNickname()
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	GetDlgItemText(IDC_EDIT_NICKNAME, strTemp);
	if (strTemp == _T("小明"))
	{
		SetDlgItemText(IDC_EDIT_NICKNAME, _T(""));
	}
}

//清屏键
void CMy20150703ChatRoomClientDlg::OnBnClickedButtonEmptyOutputBox()
{
	// TODO: Add your control notification handler code here
	m_MsgEdit.SetReadOnly(FALSE);
	m_MsgEdit.SetSel(0, -1);
	m_MsgEdit.Clear();
	m_MsgEdit.SetReadOnly(TRUE);
}

/*##############################################################################
控件消息响应#结束################################################################
##############################################################################*/

