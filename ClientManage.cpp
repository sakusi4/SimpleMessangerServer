#include "stdafx.h"
#include "SimpleMessangerServerDlg.h"
#include "ClientManage.h"
#include "CompleationPort.h"

map<SOCKET, CString> CClientManage::m_mapClntData;

CClientManage::CClientManage()
{
}

CClientManage::~CClientManage()
{
}

void CClientManage::ClientAdd(SOCKET p_hCSock)
{
	CString id = _T("Guest");	// �α��� ���� Ŭ���̾�Ʈ id�� Guest

	m_mapClntData[p_hCSock] = id;
}

int CClientManage::SignupMember(void* arg, void* param)
{
	COMPLETIONPORTPARAM* pCParam = (COMPLETIONPORTPARAM*)param;
	CSimpleMessangerServerDlg* Dlg = (CSimpleMessangerServerDlg*)AfxGetApp()->m_pMainWnd;
	SOCKET client_sock = pCParam->sock;

	CMember* Member;
	Member = (CMember*)arg;

	CString selSql;
	selSql.Format(_T("%s%s"), _T("SELECT * FROM member WHERE id = '"), Member->Member_ID);
	selSql += _T("'");

	int result = 0;

	try
	{
		CRecordset rs(&CSimpleMessangerServerDlg::database);
		rs.Open(rs.snapshot, selSql);

		if (rs.IsEOF())  // ��ϵ��� ���� ���̵� ���� ����
		{
			CString strSQL = _T("");
			strSQL = _T("INSERT INTO member(id, pw) VALUES('");
			strSQL += Member->Member_ID;
			strSQL += _T("','");
			strSQL += Member->Member_PW;
			strSQL += _T("'");
			strSQL += _T(")");

			CSimpleMessangerServerDlg::database.ExecuteSQL(strSQL);

			result = 1;
		}
		else
		{
			result = 0;
		}
	}
	catch (CDBException* e)
	{
		AfxMessageBox(e->ReportError());
		e->Delete();

		result = 0;
	}

	Dlg->Send(client_sock, &result, sizeof(result), MSGProtocol::SERV_MSG_MEMBERJOINRESULT);

	return 0;
}

int CClientManage::LoginMember(void* arg, void* param)
{
	COMPLETIONPORTPARAM* pCParam = (COMPLETIONPORTPARAM*)param;
	CSimpleMessangerServerDlg* Dlg = (CSimpleMessangerServerDlg*)AfxGetApp()->m_pMainWnd;
	SOCKET client_sock = pCParam->sock;

	CMember* Member;
	Member = (CMember*)arg;

	int result = 0;

	BOOL bLogin = FALSE;
	for (auto begin = m_mapClntData.begin(); begin != m_mapClntData.end(); ++begin)
	{
		if (!(((CString)Member->Member_ID).Compare(begin->second)))
		{
			bLogin = TRUE;
			result = 2;
		}
	}

	if (!bLogin)
	{
		CString selSql;
		selSql.Format(_T("%s%s"), _T("SELECT * FROM member WHERE id = '"), Member->Member_ID);
		selSql += _T("' AND pw = '");
		selSql += Member->Member_PW;
		selSql += _T("'");

		CRecordset rs(&CSimpleMessangerServerDlg::database);

		try
		{
			rs.Open(CRecordset::snapshot, selSql);
			if (rs.IsEOF())  // ��ϵ� ���̵� �ƴ϶�� �α��� �Ұ�
			{
				result = 0;
			}
			else
			{
				m_mapClntData[client_sock] = Member->Member_ID;
				result = 1;

				CString sql;
				sql = _T("SELECT id FROM friend_list WHERE friend_id = '");
				sql += Member->Member_ID;
				sql += _T("'");

				CRecordset longinNoticeRs(&CSimpleMessangerServerDlg::database);
				longinNoticeRs.Open(CRecordset::snapshot, sql);

				CString friend_id = _T("");


				while (!longinNoticeRs.IsEOF())
				{
					longinNoticeRs.GetFieldValue(short(0), friend_id);

					SOCKET friendSock = getSocketFromId(friend_id);
					if (friendSock != INVALID_SOCKET)
					{

						FriendState friendState(Member->Member_ID, MSGProtocol::USER_STATE_LOGIN);
						Dlg->Send(friendSock, &friendState, sizeof(friendState), MSGProtocol::SERV_MSG_FRIENDSTATE);
					}

					longinNoticeRs.MoveNext();
				}

				longinNoticeRs.Close();

			}
		}
		catch (CDBException* e)
		{
			e->ReportError();
			e->Delete();
			result = 0;
		}

		Dlg->ShowUsers();

	}
	Dlg->Send(client_sock, &result, sizeof(result), MSGProtocol::SERV_MSG_LOGINRESULT);
	
	if (result == 1)
	{
		CClientManage::FlistRequest(NULL, param);
		CClientManage::MyStateMessageRequest(NULL, param);
		CClientManage::FrStateMessageRequest(NULL, param);
	}
	return 0;
}

int CClientManage::StateMessageRequest(void * arg, void* param)
{
	COMPLETIONPORTPARAM* pCParam = (COMPLETIONPORTPARAM*)param;
	CSimpleMessangerServerDlg* Dlg = (CSimpleMessangerServerDlg*)AfxGetApp()->m_pMainWnd;
	SOCKET client_sock = pCParam->sock;


	FriendStateMessage* friendstatemessage = (FriendStateMessage*)arg;

	CString sql;
	sql = _T("SELECT id FROM profile WHERE id='");
	sql += friendstatemessage->id;
	sql += _T("'");

	CRecordset ProfileRs(&CSimpleMessangerServerDlg::database);
	ProfileRs.Open(CRecordset::snapshot, sql);

	if (ProfileRs.IsEOF())
	{
		CString strSQL = _T("");
		strSQL = _T("INSERT INTO profile(id, message) VALUES('");
		strSQL += friendstatemessage->id;
		strSQL += _T("','");
		strSQL += friendstatemessage->Message;
		strSQL += _T("'");
		strSQL += _T(")");

		CSimpleMessangerServerDlg::database.ExecuteSQL(strSQL);
	}
	else
	{
		CString strSQL = _T("");
		strSQL = _T("update profile set message='");
		strSQL += friendstatemessage->Message;
		strSQL += _T("' WHERE id='");
		strSQL += friendstatemessage->id;
		strSQL += _T("'");

		CSimpleMessangerServerDlg::database.ExecuteSQL(strSQL);
	}

	int result = 0;

	try
	{
		CString MyID = friendstatemessage->id;
		result = 1;

		CString sql;
		sql = _T("SELECT id FROM friend_list WHERE friend_id = '");
		sql += MyID;
		sql += _T("'");

		CRecordset rs(&CSimpleMessangerServerDlg::database);
		rs.Open(CRecordset::snapshot, sql);

		CString friend_id = _T("");


		while (!rs.IsEOF())
		{
			rs.GetFieldValue(short(0), friend_id);

			SOCKET friendSock = getSocketFromId(friend_id);
			if (friendSock != INVALID_SOCKET)
			{
				Dlg->Send(friendSock, friendstatemessage, sizeof(FriendStateMessage), MSGProtocol::SERV_MSG_STATEMESSAGE);
			}

			rs.MoveNext();
		}

		rs.Close();

	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		result = 0;
	}

	Dlg->Send(client_sock, &result, sizeof(result), MSGProtocol::SERV_MSG_STATEMESSAGE);

	return 0;
}

int CClientManage::MyStateMessageRequest(void * arg, void* param)
{
	COMPLETIONPORTPARAM* pCParam = (COMPLETIONPORTPARAM*)param;
	CSimpleMessangerServerDlg* Dlg = (CSimpleMessangerServerDlg*)AfxGetApp()->m_pMainWnd;
	SOCKET client_sock = pCParam->sock;

	CString sql;
	sql = _T("SELECT message FROM profile WHERE id='");
	sql += m_mapClntData[client_sock];
	sql += _T("'");

	CRecordset ProfileRs(&CSimpleMessangerServerDlg::database);
	ProfileRs.Open(CRecordset::snapshot, sql);

	if (ProfileRs.IsBOF())
	{
		char buf[30] = _T("");
		Dlg->Send(client_sock, buf, sizeof(buf), MSGProtocol::SERV_MSG_MYSTATEMESSAGE);
	}
	else
	{
		char buf[30];
		CString temp = _T("");
		ProfileRs.GetFieldValue(short(0), temp);

		strcpy(buf, (LPCTSTR)temp);
		Dlg->Send(client_sock, buf, sizeof(buf), MSGProtocol::SERV_MSG_MYSTATEMESSAGE);
	}

	return 0;
}

int CClientManage::FrStateMessageRequest(void * arg, void* param)
{
	COMPLETIONPORTPARAM* pCParam = (COMPLETIONPORTPARAM*)param;
	CSimpleMessangerServerDlg* Dlg = (CSimpleMessangerServerDlg*)AfxGetApp()->m_pMainWnd;
	SOCKET client_sock = pCParam->sock;

	CString id = m_mapClntData[client_sock];

	CString sqlstate;
	sqlstate = _T("SELECT friend_id FROM friend_list WHERE id = '");
	sqlstate += id;
	sqlstate += _T("'");

	CRecordset StateRs(&CSimpleMessangerServerDlg::database);
	StateRs.Open(CRecordset::snapshot, sqlstate);

	CString Friendid = _T("");
	while (!StateRs.IsEOF())
	{
		StateRs.GetFieldValue(short(0), Friendid);

		CString sqlselect;
		sqlselect = _T("SELECT message FROM profile WHERE id = '");
		sqlselect += Friendid;
		sqlselect += _T("'");

		CRecordset SelectRs(&CSimpleMessangerServerDlg::database);
		SelectRs.Open(CRecordset::snapshot, sqlselect);

		if (!SelectRs.IsEOF())
		{
			FriendStateMessage friendstatemessage;
			CString temp = _T("");

			SelectRs.GetFieldValue(short(0), temp);
			strcpy(friendstatemessage.id, (LPCTSTR)Friendid);
			strcpy(friendstatemessage.Message, (LPCTSTR)temp);

			Dlg->Send(client_sock, &friendstatemessage, sizeof(friendstatemessage), MSGProtocol::SERV_MSG_FRSTATEMESSAGE);
		}

		StateRs.MoveNext();
	}

	StateRs.Close();
	return 0;
}

int CClientManage::FriendInsert(void * arg, void* param)
{
	COMPLETIONPORTPARAM* pCParam = (COMPLETIONPORTPARAM*)param;
	CSimpleMessangerServerDlg* Dlg = (CSimpleMessangerServerDlg*)AfxGetApp()->m_pMainWnd;
	SOCKET client_sock = pCParam->sock;

	CString id = m_mapClntData[client_sock];
	CString friendid = (char*)arg;
	int result = 0;

	CString selSql;
	selSql.Format(_T("%s%s"), _T("SELECT * FROM member WHERE id = '"), friendid);
	selSql += _T("'");

	try
	{
		CRecordset rs(&CSimpleMessangerServerDlg::database);
		rs.Open(rs.snapshot, selSql);

		if (rs.IsEOF())
		{
			result = 0;
		}
		else
		{
			if (!id.Compare(friendid));
			else
			{
				CString sql = _T("");

				sql = _T("INSERT INTO friend_list VALUES('");
				sql += id;
				sql += _T("', '");
				sql += friendid;
				sql += _T("'");

				sql += _T(")");

				CSimpleMessangerServerDlg::database.ExecuteSQL(sql);

				// ���������� �����Ͱ� ���ٸ� �����޽��� ����
				result = 1;
			}
		}
	}
	catch (CDBException& e)
	{
		e.ReportError();
		e.Delete();
		result = 0;
	}

	Dlg->Send(client_sock, &result, sizeof(result), MSGProtocol::SERV_MSG_FRIENDINSERTRST);

	return 0;
}

int CClientManage::FriendDelete(void * arg, void* param)
{
	COMPLETIONPORTPARAM* pCParam = (COMPLETIONPORTPARAM*)param;
	SOCKET client_sock = pCParam->sock;

	CString id = m_mapClntData[client_sock];
	CString friendid = (char*)arg;

	int result = 0;

	try
	{
		if (!id.Compare(friendid));
		else
		{
			CString sql = _T("");

			sql = _T("DELETE FROM friend_list WHERE id = '");
			sql += id;
			sql += _T("' AND friend_id = '");
			sql += friendid;
			sql += _T("'");

			CSimpleMessangerServerDlg::database.ExecuteSQL(sql);

			// ���������� �����Ͱ� ���ٸ� �����޽��� ����
			result = 1;
		}
	}
	catch (CDBException& e)
	{
		e.ReportError();
		e.Delete();
		result = 0;
	}


	if (result == 1)
		FlistRequest(arg, param);

	return 0;
}

int CClientManage::FlistRequest(void * arg, void* param)
{
	COMPLETIONPORTPARAM* pCParam = (COMPLETIONPORTPARAM*)param;
	CSimpleMessangerServerDlg* Dlg = (CSimpleMessangerServerDlg*)AfxGetApp()->m_pMainWnd;
	SOCKET client_sock = pCParam->sock;

	CString id = m_mapClntData[client_sock];

	CString sql = _T("");
	sql = _T("SELECT friend_id FROM friend_list WHERE id = '");
	sql += id;
	sql += _T("'");

	CRecordset rs(&CSimpleMessangerServerDlg::database);

	try
	{
		rs.Open(CRecordset::snapshot, sql);

		CString friendID = _T("");

		// DB�� �Էµ� ģ�� ��ü ����
		while (!rs.IsEOF())
		{
			rs.GetFieldValue(short(0), friendID);

			// ģ�� ����
			FriendState friendState((LPCTSTR)friendID, getUserState(friendID));

			CString sql = _T("");
			sql = _T("SELECT message FROM profile WHERE id = '");
			sql += friendID;
			sql += _T("'");

			CRecordset SelectRs(&CSimpleMessangerServerDlg::database);
			SelectRs.Open(CRecordset::snapshot, sql);

			CString message;
			if (!SelectRs.IsEOF())
			{
				SelectRs.GetFieldValue(short(0), message);
				strcpy(friendState.statemessage, (LPCTSTR)message);
			}
			else
			{
				strcpy(friendState.statemessage, (LPCTSTR)_T(""));
			}

			Dlg->Send(client_sock, &friendState, sizeof(friendState), MSGProtocol::SERV_MSG_FLIST);

			rs.MoveNext();
		}
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
	}

	rs.Close();
	return 0;
}

int CClientManage::MessageRequest(void * arg, void* param)
{
	COMPLETIONPORTPARAM* pCParam = (COMPLETIONPORTPARAM*)param;
	CSimpleMessangerServerDlg* Dlg = (CSimpleMessangerServerDlg*)AfxGetApp()->m_pMainWnd;
	SOCKET client_sock = pCParam->sock;

	CMessageBox* ClientMessage;
	ClientMessage = (CMessageBox*)arg;

	CMessageBox ReceiveMessage;
	strcpy(ReceiveMessage.Id, (LPCTSTR)m_mapClntData[client_sock]);
	strcpy(ReceiveMessage.Message, ClientMessage->Message);

	SOCKET friendSock = getSocketFromId(ClientMessage->Id);

	if (friendSock != INVALID_SOCKET)
	{
		Dlg->Send(friendSock, (char*)&ReceiveMessage, sizeof(ReceiveMessage), MSGProtocol::SERV_MSG_RECVPAPER);
	}

	return 0;
}

int CClientManage::ChatmsgRequest(void * arg, void* param)
{
	COMPLETIONPORTPARAM* pCParam = (COMPLETIONPORTPARAM*)param;
	CSimpleMessangerServerDlg* Dlg = (CSimpleMessangerServerDlg*)AfxGetApp()->m_pMainWnd;
	SOCKET client_sock = pCParam->sock;

	CMessageBox* ClientMessage;
	ClientMessage = (CMessageBox*)arg;

	CMessageBox ReceiveMessage;
	strcpy(ReceiveMessage.Id, (LPCTSTR)m_mapClntData[client_sock]);
	strcpy(ReceiveMessage.Message, ClientMessage->Message);

	SOCKET friendSock = getSocketFromId(ClientMessage->Id);

	if (friendSock != INVALID_SOCKET)
	{
		Dlg->Send(friendSock, (char*)&ReceiveMessage, sizeof(ReceiveMessage), MSGProtocol::SERV_MSG_CHATMSG);
	}
	return 0;
}

void CClientManage::MemberLogout(void * arg, void* param)
{
	COMPLETIONPORTPARAM* pCParam = (COMPLETIONPORTPARAM*)param;
	CSimpleMessangerServerDlg* Dlg = (CSimpleMessangerServerDlg*)AfxGetApp()->m_pMainWnd;
	SOCKET client_sock = pCParam->sock;

	// ģ���鿡�� �α׾ƿ��� �˸�
	CString id = m_mapClntData[client_sock];

	CString sql = _T("");
	sql = _T("SELECT id FROM friend_list WHERE friend_id = '");
	sql += id;
	sql += _T("'");

	CRecordset rs(&CSimpleMessangerServerDlg::database);

	try
	{
		rs.Open(CRecordset::snapshot, sql);

		CString friendId = _T("");

		// �ش�ģ���� ����� ������� ���� ����
		while (!rs.IsEOF())
		{
			rs.GetFieldValue(short(0), friendId);

			// �α����� ������ ģ���� ����� ������ �α��λ��¶�� �˸�
			SOCKET friendSock = getSocketFromId(friendId);
			if (friendSock != INVALID_SOCKET)
			{
				// ģ�� ����
				FriendState friendState(id, MSGProtocol::USER_STATE_LOGOUT);

				Dlg->Send(friendSock, &friendState, sizeof(friendState), MSGProtocol::SERV_MSG_FRIENDSTATE);
			}

			rs.MoveNext();
		}
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
	}

	rs.Close();

	// Ŭ���̾�Ʈ �����̳ʿ��� ����
	m_mapClntData.erase(client_sock);
}

int CClientManage::getUserState(const CString& userId)
{
	for (auto itr = m_mapClntData.begin(); itr != m_mapClntData.end(); itr++)
	{
		// ��ҹ��ڸ� �������� �ʱ� ���� �۾�
		CString reqId = userId;
		CString oriId = itr->second;

		reqId.MakeUpper();
		oriId.MakeUpper();

		if (reqId == oriId)
			return MSGProtocol::USER_STATE_LOGIN;
	}

	return MSGProtocol::USER_STATE_LOGOUT;
}

SOCKET CClientManage::getSocketFromId(const CString& id)
{
	for (auto itr = m_mapClntData.begin(); itr != m_mapClntData.end(); itr++)
	{
		// ��ҹ��ڸ� �������� �ʱ� ���� �۾�
		CString reqId = id;
		CString oriId = itr->second;

		reqId.MakeUpper();
		oriId.MakeUpper();

		if (reqId == oriId)
		{
			return itr->first;
		}
	}

	return INVALID_SOCKET;
}

int CClientManage::ProcessFileTransper(void * arg, void* param)
{
	COMPLETIONPORTPARAM* pCParam = (COMPLETIONPORTPARAM*)param;
	CSimpleMessangerServerDlg* Dlg = (CSimpleMessangerServerDlg*)AfxGetApp()->m_pMainWnd;
	SOCKET client_sock = pCParam->sock;


	CFileTransferInfo* fileinfo;
	fileinfo = (CFileTransferInfo*)arg;

	Dlg->Send(getSocketFromId(fileinfo->recvId), fileinfo, sizeof(CFileTransferInfo), MSGProtocol::SERV_MSG_FILETRANSFERREQ);
	return 0;
}


