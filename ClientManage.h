#pragma once

#include <winsock2.h>
#include <map>
#include "Common.h"

class CClientManage
{
public:
	CClientManage();
	~CClientManage();

	static map<SOCKET, CString> m_mapClntData;

	static void ClientAdd(SOCKET);

	static SOCKET getSocketFromId(const CString&);
	static int getUserState(const CString& userId);

	static void MemberLogout(void * arg, void* param);
	static int SignupMember(void* arg, void* param);
	static int LoginMember(void* arg, void* param);
	static int FriendInsert(void* arg, void* param);
	static int FriendDelete(void* arg, void* param);
	static int FlistRequest(void* arg, void* param);
	static int MessageRequest(void* arg, void* param);
	static int ChatmsgRequest(void* arg, void* param);
	static int StateMessageRequest(void* arg, void* param);
	static int MyStateMessageRequest(void* arg, void* param);
	static int FrStateMessageRequest(void* arg, void* param);
	static int ProcessFileTransper(void* arg, void* param);
};

