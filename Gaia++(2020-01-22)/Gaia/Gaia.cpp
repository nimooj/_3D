#include "stdafx.h"
#include "Gaia.h"
#include "MainFrm.h"
#include "GaiaDoc.h"
#include "GaiaView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// GaiaApp
BEGIN_MESSAGE_MAP(GaiaApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &GaiaApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////

/*! \brief 어플리케이션의 객체 */
GaiaApp theApp;

/*! 
*	\brief 어플리케이션과 연동된 Document 클래스를 반환하는 함수 
*
*	\return 어플리케이션과 연동된 Document 클래스의 렌퍼런스
*/
GaiaDoc &theDoc()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	GaiaDoc *pDoc = (GaiaDoc *)pFrame->GetActiveDocument();
	return *pDoc;
}

/*! 
*	\brief 어플리케이션과 연동된 MainFrame 클래스를 반환하는 함수 
*
*	\return 어플리케이션과 연동된 MainFrame 클래스의 렌퍼런스
*/
CMainFrame &theMainFrm()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	return *pFrame;
}

/*! 
*	\brief 어플리케이션에 포한된 3차원 장면을 반환하는 함수 
*
*	\return 어플리케이션에 포한된 3차원 장면의 렌퍼런스
*/
GScene &theScene()
{
	return theApp.theScene;
}

/*! 
*	\brief 어플리케이션에서 사용되는 3차원 객체의 생성기를 반환하는 함수
*
*	\return 어플리케이션에서 사용되는 3차원 객체 생성기의 렌퍼런스
*/
PCreator &theCreator()
{
	return theApp.theCreator;
}

/*! 
*	\brief 어플리케이션에서 사용되는 3차원 객체의 편집기를 반환하는 함수
*
*	\return 어플리케이션에서 사용되는 3차원 객체 편집기의 렌퍼런스
*/
PEditor &theEditor()
{
	return theApp.theEditor;
}

/*! 
*	\brief 어플리케이션에서 사용되는 3차원 객체의 선택기를 반환하는 함수
*
*	\return 어플리케이션에서 사용되는 3차원 객체 선택기의 렌퍼런스
*/
PSelector &theSelector()
{
	return theApp.theEditor.theSelector;
}

// GaiaApp 생성
GaiaApp::GaiaApp()
{
	// UI 색상 설정하기.
	theScene.UIBkgColor[0] = 68;
	theScene.UIBkgColor[1] = 68;
	theScene.UIBkgColor[2] = 68;

	theScene.UItxtColor[0] = 255;
	theScene.UItxtColor[1] = 255;
	theScene.UItxtColor[2] = 255;
}

// GaiaApp 초기화
BOOL GaiaApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	SetRegistryKey(_T("동국대학교 그래픽스응용연구실"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.

	// 응용 프로그램의 문서 템플릿을 등록합니다
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(GaiaDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(GaiaView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// DDE Execute 열기를 활성화합니다.
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 명령줄에 지정된 명령을 디스패치합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
#ifdef _DEBUG
	m_pMainWnd->SetWindowText(_T("GAIA (Debug)"));
#else
	m_pMainWnd->SetWindowText(_T("GAIA (Release)"));
#endif // _DEBUG

	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
	m_pMainWnd->DragAcceptFiles();

	// 플러그인 초기화
	InitPlugin();

	return TRUE;
}

void GaiaApp::InitPlugin()
{
	// 플러그인 초기화 하고,
	theScene.hWndView = ((CFrameWnd *)m_pMainWnd)->GetActiveView()->GetSafeHwnd();
	theCreator.InitPlugIn(&theScene, &(theMainFrm().m_WorkSpaceDlg), &theDoc());
	theEditor.InitPlugIn(&theScene, &(theMainFrm().m_WorkSpaceDlg), &theDoc());

	// 뷰윈도우에 포커스를 준다.
	SetFocus(theScene.hWndView);
}

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void GaiaApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// GaiaApp 메시지 처리기
BOOL GaiaApp::OnIdle(LONG lCount)
{
	CMainFrame *pFrame = (CMainFrame *)GetMainWnd();
	if (pFrame->m_CheckFPS)
	{
		GaiaView *pView = (GaiaView *)pFrame->GetActiveView();
		pView->Invalidate(false);
		return true;
	}
	
//	if (pView->GetDocument()->m_bAnimation)
// 	{
// 		theScene.AnimateScene();
// 	  	pView->Invalidate(false);
// 	  	return true;
// 	}	

	return CWinApp::OnIdle(lCount);
}

