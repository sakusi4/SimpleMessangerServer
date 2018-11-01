#pragma once
#include "Common.h"

struct COMPLETIONPORTPARAM
{
	SOCKET sock;
};

// ������ ���� ����ü(�Ϸ���Ʈ ť���� ������ ����ü(WSARecv))
struct OVERLAPPEDPARAM
{
	OVERLAPPED overlapped;
	char buf[MSGProtocol::BUFSIZE];
	WSABUF wsaBuf;
};
class CCompleationPort
{
public:
	CCompleationPort();
	~CCompleationPort();

public:
	static void ConnectCompletionPort(SOCKET p_sock);					// �Ϸ���Ʈ�� ���� �Լ�
	static BOOL CreateCompletionPort(UINT(WINAPI *PThread)(LPVOID));	// �Ϸ���Ʈ ���� �Լ�

	static HANDLE m_hCompletionPort;			// �Ϸ���Ʈ �ڵ�
	static int m_numOfThread;					// ������ ���� ��
	static HANDLE* m_hThread;					// ������ �ڵ� 
};

