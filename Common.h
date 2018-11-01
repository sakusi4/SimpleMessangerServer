#pragma once

#include "stdafx.h"

class MSGProtocol
{
public:
	static enum
	{
		BUFSIZE = 4096,
		IDLENGTH = 15,
		PWLENGTH = 15,
		SERVER_PORT = 21000
	};

	// user state
	static enum
	{
		USER_STATE_LOGOUT = 0,
		USER_STATE_LOGIN = 1
	};

	// server message(flag)
	static enum
	{
		TEST = 0,
		SERV_MSG_MEMBERJOINRESULT = 5000,
		SERV_MSG_LOGINRESULT,
		SERV_MSG_RECVNOTE,
		SERV_MSG_FLIST,
		SERV_MSG_FRIENDSTATE,
		SERV_MSG_FRIENDINSERTRST,
		SERV_MSG_FRIENDDELETERST,
		SERV_MSG_RECVPAPER,
		SERV_MSG_CHATMSG,
		SERV_MSG_FILETRANSFERREQ,
		SERV_MSG_FILETRANSFERREJECTION,
		SERV_MSG_STATEMESSAGE,
		SERV_MSG_MYSTATEMESSAGE,
		SERV_MSG_FRSTATEMESSAGE,
		SERV_MSG_SETIMAGE,
		SERV_MSG_MYIMAGE,
		SERV_MSG_FRIMAGE
	};

	// client message(flag)
	static enum
	{
		CLNT_MSG_MEMBERJOINREQUEST = 6000,
		CLNT_MSG_LOGINREQUEST,
		CLNT_MSG_SENDPAPER,
		CLNT_MSG_FLISTREQUEST,
		CLNT_MSG_FRIENDINSERT,
		CLNT_MSG_FRIENDDELETE,
		CLNT_MSG_SENDCHATMSG,
		CLNT_MSG_FILETRANSFERREQ,
		CLNT_MSG_FILETRANSFERREJECTION,
		CLNT_MSG_STATEMESSAGE,
		CLNT_MSG_MYSTATEMESSAGE,
		CLNT_MSG_FRSTATEMESSAGE,
		CLNT_MSG_SETIMAGE,
		CLNT_MSG_MYIMAGE,
		CLNT_MSG_FRIMAGE
	};
};


class CMember
{
public:
	CMember() {}
	~CMember() {}

	char Member_ID[MSGProtocol::IDLENGTH] = { 0 };
	char Member_PW[MSGProtocol::PWLENGTH] = { 0 };
};

class FriendState
{
public:
	char id[MSGProtocol::IDLENGTH] = { 0 };
	char statemessage[30] = { 0 };
	int state = 0;

	FriendState(const char* p_id, int p_state)
	{
		strcpy(id, p_id);
		state = p_state;
	}
};

class FriendStateMessage
{
public:
	char id[MSGProtocol::IDLENGTH] = { 0 };
	char Message[30] = { 0 };
};

// 패킷 헤더 구조체
class PACKETHEADER
{
public:
	int flag = 0;	// 메시지 플래그
	int size = 0;	// 메시지의 크기
};

class CMessageBox
{
public:
	char Id[MSGProtocol::IDLENGTH] = { 0 };
	char Message[400] = { 0 };
};

class CFileTransferInfo
{
public:
	char sendId[MSGProtocol::IDLENGTH] = { 0 };		// 보낸이
	char recvId[MSGProtocol::IDLENGTH] = { 0 };		// 받는이
	SOCKADDR_IN sendAddr = { 0 };					// 보낸이 주소
	char fileName[FILENAME_MAX] = { 0 };			// 파일명
	unsigned long fileSize = 0;					// 파일크기
};
