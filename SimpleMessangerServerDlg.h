
// SimpleMessangerServerDlg.h : ��� ����
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "Common.h"

// CSimpleMessangerServerDlg ��ȭ ����
class CSimpleMessangerServerDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CSimpleMessangerServerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMPLEMESSANGERSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP();
public:

	enum
	{
		WM_SOCKETACCEPT = WM_USER + 100	// WSAAsycSelect�� accept ó�� �޽���
	};

	WSADATA wsaData;
	WORD wVersionRequested;
	SOCKET listen_sock;
	SOCKADDR_IN serveraddr;
	HANDLE hAcceptThread;
	HANDLE hComPort;

	static CCriticalSection CRISEC;

	int err;
	
	static CDatabase database;
	static unsigned int Process(int flag, void* buf, void* param);

	int Send(SOCKET scok, void* buf, int length, int flag);

	void ShowUsers();
	void ShowMessage(CString flag, CString sendId);
	static void ReWSARecv(SOCKET p_sock, void* p_param);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	CListCtrl m_List_Members;
	afx_msg void OnBnClickedButtonBlog();
	CEdit m_Edit_Message;
	CListBox m_List_Users;
	afx_msg void OnBnClickedButtonOpen();
	static UINT WINAPI CompletionPortRecvThread(LPVOID arg);
	
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonDb();

	CListCtrl m_List_Friends;
	CListCtrl m_List_State;

	afx_msg LRESULT OnSocketaccept(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

class ParamClass
{
public:
	ParamClass(CSimpleMessangerServerDlg* Dlg, SOCKET client_sock)
	{
		this->Dlg = Dlg;
		this->client_sock = client_sock;
	}
	CSimpleMessangerServerDlg* Dlg;
	SOCKET client_sock;
};

typedef struct SocketInfo
{
	SOCKET ClientSock;
	SOCKADDR_IN ClientAddr;
}SOCKETINFO, *PSOCKETINFO;

typedef struct 
{
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buf[MSGProtocol::BUFSIZE];
}BUFINFO, *PBUFINFO;


