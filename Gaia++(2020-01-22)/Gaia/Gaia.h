#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.

#pragma comment(lib, "pcreator.lib")
#pragma comment(lib, "peditor.lib")
//#pragma comment(lib, "alglib.lib")
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

class GaiaApp : public CWinApp
{
public:
	// 생성자
	GaiaApp();

public:
	// 가상함수 재정의
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);

	// 메시지 처리함수
	DECLARE_MESSAGE_MAP()
	afx_msg void OnAppAbout();

	// 사용자 정의 멤버 함수
	void InitPlugin();

public:
	// 데이터 멤버
	/*! \brief 어플리케이션의 3차원 장면을 표현하는 객체 */
	GScene theScene;
	
	/*! \brief 3차원 객체의 생성을 담당하는 객체 */
	PCreator theCreator;

	/*! \brief 3차원 객체의 편집(이동, 회전, 스케일 등)을 담당하는 객체 */
	PEditor theEditor;
};

// 클래스 선언
extern GaiaApp theApp;
class GaiaDoc;
class CMainFrame;

/*! \brief 어플리케이션과 연동된 Document 객체를 반환하는 함수 */
GaiaDoc &theDoc();

/*! \brief 어플리케이션과 연동된 MainFrame 객체를 반환하는 함수 */
CMainFrame &theMainFrm();

/*! \brief 어플리케이션에 포한된 3차원 장면 객체를 반환하는 함수 */
GScene &theScene();

/*! \brief 어플리케이션에서 사용되는 3차원 객체의 생성기를 반환하는 함수 */
PCreator &theCreator();

/*! \brief 어플리케이션에서 사용되는 3차원 객체의 편집기를 반환하는 함수 */
PEditor &theEditor();

/*! \brief 어플리케이션에서 사용되는 3차원 객체의 선택기를 반환하는 함수 */
PSelector &theSelector();

