#pragma once
#include "Common.h"

struct COMPLETIONPORTPARAM
{
	SOCKET sock;
};

// 데이터 버퍼 구조체(완료포트 큐에서 꺼내질 구조체(WSARecv))
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
	static void ConnectCompletionPort(SOCKET p_sock);					// 완료포트에 연결 함수
	static BOOL CreateCompletionPort(UINT(WINAPI *PThread)(LPVOID));	// 완료포트 생성 함수

	static HANDLE m_hCompletionPort;			// 완료포트 핸들
	static int m_numOfThread;					// 스레드 생성 수
	static HANDLE* m_hThread;					// 스레드 핸들 
};

