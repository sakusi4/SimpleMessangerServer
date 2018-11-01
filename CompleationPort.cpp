#include "stdafx.h"
#include "CompleationPort.h"

HANDLE CCompleationPort::m_hCompletionPort;		
int CCompleationPort::m_numOfThread;					
HANDLE* CCompleationPort::m_hThread;					

CCompleationPort::CCompleationPort()
{
}


CCompleationPort::~CCompleationPort()
{
}

void CCompleationPort::ConnectCompletionPort(SOCKET p_sock)
{
	if (m_hCompletionPort)
	{
		// 완료포트에 연결
		COMPLETIONPORTPARAM* pCPParam = new COMPLETIONPORTPARAM;
		pCPParam->sock = p_sock;

		CreateIoCompletionPort((HANDLE)p_sock, m_hCompletionPort, (DWORD)pCPParam, 0);

		// 해당 소켓을 비동기로 리시브모드로 전환
		OVERLAPPEDPARAM* pOverlappedParam = new OVERLAPPEDPARAM;
		pOverlappedParam->wsaBuf.len = MSGProtocol::BUFSIZE;
		pOverlappedParam->wsaBuf.buf = pOverlappedParam->buf;
		memset(&(pOverlappedParam->overlapped), 0, sizeof(OVERLAPPED));

		DWORD flags = 0;

		WSARecv(p_sock,
			&(pOverlappedParam->wsaBuf),
			1,
			NULL,
			&flags,
			&(pOverlappedParam->overlapped),
			NULL);
	}
}

BOOL CCompleationPort::CreateCompletionPort(UINT (WINAPI *PThread)(LPVOID))
{
	// 완료포트 생성
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	// 스레드 생성
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	m_numOfThread = sysInfo.dwNumberOfProcessors;

	m_hThread = new HANDLE[m_numOfThread];

	// cpu 갯수만큼 스레드 생성
	for (int i = 0; i < m_numOfThread; i++)
		m_hThread[i] = (HANDLE)_beginthreadex(NULL, 0, PThread,
		(LPVOID)m_hCompletionPort, 0, NULL);

	return m_hCompletionPort != NULL;
}
