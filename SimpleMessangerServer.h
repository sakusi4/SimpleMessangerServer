
// SimpleMessangerServer.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CSimpleMessangerServerApp:
// �� Ŭ������ ������ ���ؼ��� SimpleMessangerServer.cpp�� �����Ͻʽÿ�.
//

class CSimpleMessangerServerApp : public CWinApp
{
public:
	CSimpleMessangerServerApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CSimpleMessangerServerApp theApp;