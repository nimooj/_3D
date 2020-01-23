#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.

#pragma comment(lib, "pcreator.lib")
#pragma comment(lib, "peditor.lib")
//#pragma comment(lib, "alglib.lib")
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

class GaiaApp : public CWinApp
{
public:
	// ������
	GaiaApp();

public:
	// �����Լ� ������
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);

	// �޽��� ó���Լ�
	DECLARE_MESSAGE_MAP()
	afx_msg void OnAppAbout();

	// ����� ���� ��� �Լ�
	void InitPlugin();

public:
	// ������ ���
	/*! \brief ���ø����̼��� 3���� ����� ǥ���ϴ� ��ü */
	GScene theScene;
	
	/*! \brief 3���� ��ü�� ������ ����ϴ� ��ü */
	PCreator theCreator;

	/*! \brief 3���� ��ü�� ����(�̵�, ȸ��, ������ ��)�� ����ϴ� ��ü */
	PEditor theEditor;
};

// Ŭ���� ����
extern GaiaApp theApp;
class GaiaDoc;
class CMainFrame;

/*! \brief ���ø����̼ǰ� ������ Document ��ü�� ��ȯ�ϴ� �Լ� */
GaiaDoc &theDoc();

/*! \brief ���ø����̼ǰ� ������ MainFrame ��ü�� ��ȯ�ϴ� �Լ� */
CMainFrame &theMainFrm();

/*! \brief ���ø����̼ǿ� ���ѵ� 3���� ��� ��ü�� ��ȯ�ϴ� �Լ� */
GScene &theScene();

/*! \brief ���ø����̼ǿ��� ���Ǵ� 3���� ��ü�� �����⸦ ��ȯ�ϴ� �Լ� */
PCreator &theCreator();

/*! \brief ���ø����̼ǿ��� ���Ǵ� 3���� ��ü�� �����⸦ ��ȯ�ϴ� �Լ� */
PEditor &theEditor();

/*! \brief ���ø����̼ǿ��� ���Ǵ� 3���� ��ü�� ���ñ⸦ ��ȯ�ϴ� �Լ� */
PSelector &theSelector();

