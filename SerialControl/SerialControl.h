
// SerialControl.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CSerialControlApp:
// �� Ŭ������ ������ ���ؼ��� SerialControl.cpp�� �����Ͻʽÿ�.
//

class CSerialControlApp : public CWinApp
{
public:
	CSerialControlApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CSerialControlApp theApp;