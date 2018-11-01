
// SimpleMessangerServerDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "SimpleMessangerServer.h"
#include "SimpleMessangerServerDlg.h"
#include "afxdialogex.h"
#include "ClientManage.h"
#include "Common.h"
#include "CompleationPort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDatabase CSimpleMessangerServerDlg::database;
CCriticalSection CSimpleMessangerServerDlg::CRISEC;

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSimpleMessangerServerDlg ��ȭ ����



CSimpleMessangerServerDlg::CSimpleMessangerServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SIMPLEMESSANGERSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimpleMessangerServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List_Members);
	DDX_Control(pDX, IDC_EDIT1, m_Edit_Message);
	DDX_Control(pDX, IDC_LIST_CURRENTUSERS, m_List_Users);
	DDX_Control(pDX, IDC_LIST2, m_List_Friends);
	DDX_Control(pDX, IDC_LIST3, m_List_State);
}

BEGIN_MESSAGE_MAP(CSimpleMessangerServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_BLOG, &CSimpleMessangerServerDlg::OnBnClickedButtonBlog)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CSimpleMessangerServerDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CSimpleMessangerServerDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_DB, &CSimpleMessangerServerDlg::OnBnClickedButtonDb)
	ON_MESSAGE(WM_SOCKETACCEPT, &CSimpleMessangerServerDlg::OnSocketaccept)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSimpleMessangerServerDlg �޽��� ó����

BOOL CSimpleMessangerServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	m_List_Members.InsertColumn(0, _T("���̵�"), LVCFMT_LEFT, 150);
	m_List_Members.InsertColumn(1, _T("��й�ȣ"), LVCFMT_LEFT, 150);

	m_List_Friends.InsertColumn(0, _T("���̵�"), LVCFMT_LEFT, 100);
	m_List_Friends.InsertColumn(1, _T("ģ�� ���̵�"), LVCFMT_LEFT, 100);

	m_List_State.InsertColumn(0, _T("���̵�"), LVCFMT_LEFT, 100);
	m_List_State.InsertColumn(1, _T("��ȭ��"), LVCFMT_CENTER, 600);


	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CSimpleMessangerServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CSimpleMessangerServerDlg::OnPaint()
{
	CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.
	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		Graphics graphics(dc);
		CRect ClientRect;
		GetClientRect(&ClientRect);

		dc.FillSolidRect(&ClientRect, RGB(255, 255, 255));
		LinearGradientBrush TopBrush(Rect(0, 0, ClientRect.BottomRight().x, 600), Color(250, 222, 231, 142), Color(0, 222, 231, 142), LinearGradientModeVertical);
		graphics.FillRectangle(&TopBrush, 0, 0, ClientRect.BottomRight().x, 600);

		CImage MarkImage;
		MarkImage.Load(_T("./res/mark.png"));
		MarkImage.StretchBlt(dc.m_hDC, 10, ClientRect.BottomRight().y - 30, 350, 20);
		//	CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CSimpleMessangerServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CSimpleMessangerServerDlg::OnSocketaccept(WPARAM wParam, LPARAM lParam)
{
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	ZeroMemory(&clientaddr, sizeof(clientaddr));

	int addrlen = sizeof(clientaddr);

	client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
	CCompleationPort::ConnectCompletionPort(client_sock);

	CClientManage::ClientAdd(client_sock);
	ShowUsers();
	return 0;
}

int CSimpleMessangerServerDlg::Send(SOCKET sock, void* buf, int length, int flag)
{
	PACKETHEADER packetheader;
	packetheader.flag = flag;
	packetheader.size = length;

	int allSize = sizeof(packetheader) + length;
	char Packet[MSGProtocol::BUFSIZE];
	CopyMemory(Packet, &packetheader, sizeof(packetheader));
	CopyMemory(Packet + sizeof(packetheader), buf, length);

	int retval = send(sock, Packet, allSize, 0);
	if (retval == SOCKET_ERROR)
	{
		AfxMessageBox(_T("send() ����"));
	}

	return 0;
}
unsigned int CSimpleMessangerServerDlg::Process(int flag, void* buf, void* param)
{
	CSimpleMessangerServerDlg* Dlg = (CSimpleMessangerServerDlg*)AfxGetApp()->m_pMainWnd;
	COMPLETIONPORTPARAM* pCPParam = (COMPLETIONPORTPARAM*)param;

	SOCKADDR_IN clientaddr;
	int addrsize = sizeof(clientaddr);
	getpeername(pCPParam->sock, (SOCKADDR*)&clientaddr, &addrsize);

	CString UserId = CClientManage::m_mapClntData[pCPParam->sock];
	UserId += _T(", ");

	if (flag == MSGProtocol::CLNT_MSG_MEMBERJOINREQUEST)
	{
		CClientManage::SignupMember(buf, param);
		Dlg->ShowMessage(_T("ȸ������ ��û"), UserId + inet_ntoa(clientaddr.sin_addr));
	}
	else if (flag == MSGProtocol::CLNT_MSG_LOGINREQUEST)
	{
		CClientManage::LoginMember(buf, param);
		Dlg->ShowMessage(_T("�α��� ��û"), UserId + inet_ntoa(clientaddr.sin_addr));
	}
	else if (flag == MSGProtocol::CLNT_MSG_FRIENDINSERT)
	{
		CClientManage::FriendInsert(buf, param);
		Dlg->ShowMessage(_T("ģ���߰� ��û"), UserId + inet_ntoa(clientaddr.sin_addr));
	}
	else if (flag == MSGProtocol::CLNT_MSG_FRIENDDELETE)
	{
		CClientManage::FriendDelete(buf, param);
		Dlg->ShowMessage(_T("ģ������ ��û"), UserId + inet_ntoa(clientaddr.sin_addr));
	}
	else if (flag == MSGProtocol::CLNT_MSG_FLISTREQUEST)
	{
		CClientManage::FlistRequest(buf, param);
		Dlg->ShowMessage(_T("ģ����� ��û"), UserId + inet_ntoa(clientaddr.sin_addr));
	}
	else if (flag == MSGProtocol::CLNT_MSG_SENDPAPER)
	{
		CClientManage::MessageRequest(buf, param);
		Dlg->ShowMessage(_T("�������� ��û"), UserId + inet_ntoa(clientaddr.sin_addr));
	}
	else if (flag == MSGProtocol::CLNT_MSG_SENDCHATMSG)
	{
		CClientManage::ChatmsgRequest(buf, param);
		Dlg->ShowMessage(_T("ä������ ��û"), UserId + inet_ntoa(clientaddr.sin_addr));
	}
	else if (flag == MSGProtocol::CLNT_MSG_STATEMESSAGE)
	{
		CClientManage::StateMessageRequest(buf, param);
		Dlg->ShowMessage(_T("��ȭ�� ���� ��û"), UserId + inet_ntoa(clientaddr.sin_addr));
	}
	else if (flag == MSGProtocol::CLNT_MSG_MYSTATEMESSAGE)
	{
		CClientManage::MyStateMessageRequest(buf, param);
		Dlg->ShowMessage(_T("�ڽ��� ��ȭ�� ��û"), UserId + inet_ntoa(clientaddr.sin_addr));
	}
	else if (flag == MSGProtocol::CLNT_MSG_FRSTATEMESSAGE)
	{
		CClientManage::FrStateMessageRequest(buf, param);
		Dlg->ShowMessage(_T("ģ���� ��ȭ�� ��û"), UserId + inet_ntoa(clientaddr.sin_addr));
	}
	else if (flag == MSGProtocol::CLNT_MSG_FILETRANSFERREQ)
	{
		CClientManage::ProcessFileTransper(buf, param);
		Dlg->ShowMessage(_T("���� ���� ��û"), UserId + inet_ntoa(clientaddr.sin_addr));
	}
	return 0;
}

void CSimpleMessangerServerDlg::OnBnClickedButtonOpen()
{
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);

	hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	listen_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listen_sock == INVALID_SOCKET)
		AfxMessageBox(_T("socket()����"));

	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(21000);

	bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	listen(listen_sock, SOMAXCONN);

	if (!CCompleationPort::CreateCompletionPort(CompletionPortRecvThread))
	{
		AfxMessageBox(_T("Server Open False!"));
		return;
	}

	WSAAsyncSelect(listen_sock, GetSafeHwnd(), WM_SOCKETACCEPT, FD_ACCEPT);

	//hAcceptThread = (HANDLE)_beginthreadex(NULL, 0, ClientAccept, (LPVOID)hComPort, 0, &AcceptThreadID);

	CSimpleMessangerServerDlg::database.OpenEx(_T("DSN=SimpleMessanger"), 0);
	ShowMessage(_T("���� ���� DB���� �Ϸ�"), _T("������"));

	CButton* pCloseButton = (CButton*)GetDlgItem(IDC_BUTTON_CLOSE);
	pCloseButton->EnableWindow(TRUE);

	CButton* pOpenButton = (CButton*)GetDlgItem(IDC_BUTTON_OPEN);
	pOpenButton->EnableWindow(FALSE);
}


UINT WINAPI CSimpleMessangerServerDlg::CompletionPortRecvThread(LPVOID arg)
{
	COMPLETIONPORTPARAM* pCPParam;			 // ���� ����
	OVERLAPPEDPARAM* pOverlappedParam;       // �����͹��� ����

	while (1)
	{
		DWORD recvSize = 0;

		GetQueuedCompletionStatus(arg,
			&recvSize,
			(DWORD*)&pCPParam,
			(LPOVERLAPPED*)&pOverlappedParam,
			INFINITE);

		if (pCPParam == NULL)
			return 0;

		if (recvSize <= 0)
		{
			// �α׾ƿ� ó��
			CSimpleMessangerServerDlg* Dlg = (CSimpleMessangerServerDlg*)AfxGetApp()->m_pMainWnd;
			Dlg->ShowMessage(_T("���� ��û"), CClientManage::m_mapClntData[pCPParam->sock]);
			CClientManage::MemberLogout(NULL, pCPParam);

			closesocket(pCPParam->sock);

			delete pCPParam;
			delete pOverlappedParam;

			Dlg->ShowUsers();

			return 0;
		}

		// ���ι��۷� ����
		char buf[MSGProtocol::BUFSIZE];
		CopyMemory(buf, pOverlappedParam->buf, recvSize);

		if (recvSize > MSGProtocol::BUFSIZE - 1)
		{
			AfxMessageBox(_T("��Ŷ�� �ʹ� Ů�ϴ�."));
			CSimpleMessangerServerDlg::ReWSARecv(pCPParam->sock, pOverlappedParam);
			continue;
		}

		PACKETHEADER PacketHeader;
		CopyMemory(&PacketHeader, buf, sizeof(PACKETHEADER));

		if (recvSize < sizeof(PACKETHEADER) + PacketHeader.size)
		{
			TRACE("recv but less than data size \n");
			CSimpleMessangerServerDlg::ReWSARecv(pCPParam->sock, pOverlappedParam);
			continue;
		}

		buf[sizeof(PACKETHEADER) + PacketHeader.size] = 0;

		char Data[MSGProtocol::BUFSIZE];
		CopyMemory(Data, buf + sizeof(PACKETHEADER), PacketHeader.size);

		CSimpleMessangerServerDlg::CRISEC.Lock();
		CSimpleMessangerServerDlg::Process(PacketHeader.flag, Data, pCPParam);	
		CSimpleMessangerServerDlg::CRISEC.Unlock();

		DWORD flags = 0;

		CSimpleMessangerServerDlg::ReWSARecv(pCPParam->sock, pOverlappedParam);
	}
	return 0;
}

void CSimpleMessangerServerDlg::OnBnClickedButtonClose()
{
	ShowMessage(_T("������ �ݽ��ϴ�.."), _T("������"));
	DestroyWindow();
}

void CSimpleMessangerServerDlg::OnBnClickedButtonDb()
{
	m_List_Friends.DeleteAllItems();
	m_List_Members.DeleteAllItems();
	m_List_State.DeleteAllItems();

	if (!database.IsOpen())
	{
		AfxMessageBox(_T("DB�� ����Ǿ� ���� �ʽ��ϴ�. ������ Ȯ���� �ּ���"));
		return;
	}

	//��� ��� ��ȸ
	CRecordset rs(&CSimpleMessangerServerDlg::database);

	try
	{
		rs.Open(rs.snapshot, _T("SELECT * FROM member"));
	}
	catch (CDBException* e)
	{
		e->ReportError();
		return;
	}

	ShowMessage(_T("DB������ ��ȸ �Ͽ����ϴ�."), _T("������"));
	short mb_col = rs.GetODBCFieldCount();

	for (int i = 0; i < 2; i++)
	{
		int cnt = 0;
		int index = 0;
		while (!rs.IsEOF())
		{
			for (short j = 0; j < mb_col; j++)
			{
				CString tmp;
				rs.GetFieldValue(j, tmp);

				if (tmp)
				{
					if (j == 0)
						index = m_List_Members.InsertItem(cnt, tmp);
					else
						m_List_Members.SetItemText(index, 1, tmp);
				}
			}
			rs.MoveNext();
			cnt++;
		}
	}

	//ģ�� ��� ��ȸ
	CRecordset rs2(&CSimpleMessangerServerDlg::database);

	try
	{
		rs2.Open(rs.snapshot, _T("SELECT * FROM friend_list"));
	}
	catch (CDBException* e)
	{
		e->ReportError();
		return;
	}

	short fr_col = rs2.GetODBCFieldCount();

	for (int i = 0; i < 2; i++)
	{
		int cnt = 0;
		int index = 0;
		while (!rs2.IsEOF())
		{
			for (short j = 0; j < fr_col; j++)
			{
				CString tmp;
				rs2.GetFieldValue(j, tmp);

				if (tmp)
				{
					if (j == 0)
						index = m_List_Friends.InsertItem(cnt, tmp);
					else
						m_List_Friends.SetItemText(index, 1, tmp);
				}
			}
			rs2.MoveNext();
			cnt++;
		}
	}

	CRecordset rs3(&CSimpleMessangerServerDlg::database);

	try
	{
		rs3.Open(rs.snapshot, _T("SELECT * FROM profile"));
	}
	catch (CDBException* e)
	{
		e->ReportError();
		return;
	}

	short st_col = rs3.GetODBCFieldCount();

	for (int i = 0; i < 2; i++)
	{
		int cnt = 0;
		int index = 0;
		while (!rs3.IsEOF())
		{
			for (short j = 0; j < st_col; j++)
			{
				CString tmp;
				rs3.GetFieldValue(j, tmp);

				if (tmp)
				{
					if (j == 0)
						index = m_List_State.InsertItem(cnt, tmp);
					else
						m_List_State.SetItemText(index, 1, tmp);
				}
			}
			rs3.MoveNext();
			cnt++;
		}
	}
}

void CSimpleMessangerServerDlg::ShowUsers()
{
	m_List_Users.ResetContent();

	for (auto itr : CClientManage::m_mapClntData)
		m_List_Users.AddString(itr.second);
}

void CSimpleMessangerServerDlg::ShowMessage(CString flag, CString sendId)
{
	SYSTEMTIME sysTime;
	CString timeStr = _T("");
	CString logStr = _T("");

	GetLocalTime(&sysTime);
	timeStr.Format(_T("    %d-%d-%d %d�� %d�� %d��\r\n"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour,
		sysTime.wMinute, sysTime.wSecond);

	logStr.Format(_T("���� �˸� : (MSG Protocol : %s) (From : %s)"), flag, sendId);

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	int len = pEdit->GetWindowTextLengthA();
	m_Edit_Message.SetSel(len, len);
	m_Edit_Message.ReplaceSel(logStr + timeStr + _T("\r\n"));
}

void CSimpleMessangerServerDlg::ReWSARecv(SOCKET p_sock, void * p_param)
{
	OVERLAPPEDPARAM* pOverlappedParam = (OVERLAPPEDPARAM*)p_param;

	memset(&(pOverlappedParam->overlapped), 0, sizeof(OVERLAPPED));
	pOverlappedParam->wsaBuf.len = MSGProtocol::BUFSIZE;
	DWORD flags = 0;

	WSARecv(p_sock, &(pOverlappedParam->wsaBuf), 1, NULL, &flags,
		&(pOverlappedParam->overlapped), NULL);
}

void CSimpleMessangerServerDlg::OnBnClickedButtonBlog()
{
	ShellExecute(m_hWnd, _T("open"), _T("http://blog.naver.com/kizxc4395"), NULL, NULL, SW_SHOW);
}

int CSimpleMessangerServerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CSimpleMessangerServerDlg::OnDestroy()
{
	closesocket(listen_sock);
	WSACleanup();

	CDialogEx::OnDestroy();
}


BOOL CSimpleMessangerServerDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


HBRUSH CSimpleMessangerServerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	UINT nID = pWnd->GetDlgCtrlID();

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		if (nID == IDC_STATIC_CURRENTUSER)
		{
			pDC->SetBkMode(TRANSPARENT);      // �������
			return (HBRUSH)::GetStockObject(NULL_BRUSH);
		}
		else if (nID == IDC_STATIC_SERVERMESSAGE)
		{
			pDC->SetBkMode(TRANSPARENT);      // �������
			return (HBRUSH)::GetStockObject(NULL_BRUSH);
		}
		else if (nID == IDC_STATIC_FRIENDLIST)
		{
			pDC->SetBkMode(TRANSPARENT);      // �������
			return (HBRUSH)::GetStockObject(NULL_BRUSH);
		}
		else if (nID == IDC_STATIC_MEMBERLIST)
		{
			pDC->SetBkMode(TRANSPARENT);      // �������
			return (HBRUSH)::GetStockObject(NULL_BRUSH);
		}
		else if (nID == IDC_STATIC_MESSAGELIST)
		{
			pDC->SetBkMode(TRANSPARENT);      // �������
			return (HBRUSH)::GetStockObject(NULL_BRUSH);
		}
		else if (nID == IDC_STATIC_SERVER)
		{
			pDC->SetBkMode(TRANSPARENT);      // �������
			return (HBRUSH)::GetStockObject(NULL_BRUSH);
		}
		else if (nID == IDC_STATIC_DATABASE)
		{
			pDC->SetBkMode(TRANSPARENT);      // �������
			return (HBRUSH)::GetStockObject(NULL_BRUSH);
		}
		else if (nID == IDC_STATIC_CONTROL)
		{
			pDC->SetBkMode(TRANSPARENT);      // �������
			return (HBRUSH)::GetStockObject(NULL_BRUSH);
		}
	}
	return hbr;
}
