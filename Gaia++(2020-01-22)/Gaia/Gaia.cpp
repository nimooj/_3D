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

/*! \brief ���ø����̼��� ��ü */
GaiaApp theApp;

/*! 
*	\brief ���ø����̼ǰ� ������ Document Ŭ������ ��ȯ�ϴ� �Լ� 
*
*	\return ���ø����̼ǰ� ������ Document Ŭ������ ���۷���
*/
GaiaDoc &theDoc()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	GaiaDoc *pDoc = (GaiaDoc *)pFrame->GetActiveDocument();
	return *pDoc;
}

/*! 
*	\brief ���ø����̼ǰ� ������ MainFrame Ŭ������ ��ȯ�ϴ� �Լ� 
*
*	\return ���ø����̼ǰ� ������ MainFrame Ŭ������ ���۷���
*/
CMainFrame &theMainFrm()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	return *pFrame;
}

/*! 
*	\brief ���ø����̼ǿ� ���ѵ� 3���� ����� ��ȯ�ϴ� �Լ� 
*
*	\return ���ø����̼ǿ� ���ѵ� 3���� ����� ���۷���
*/
GScene &theScene()
{
	return theApp.theScene;
}

/*! 
*	\brief ���ø����̼ǿ��� ���Ǵ� 3���� ��ü�� �����⸦ ��ȯ�ϴ� �Լ�
*
*	\return ���ø����̼ǿ��� ���Ǵ� 3���� ��ü �������� ���۷���
*/
PCreator &theCreator()
{
	return theApp.theCreator;
}

/*! 
*	\brief ���ø����̼ǿ��� ���Ǵ� 3���� ��ü�� �����⸦ ��ȯ�ϴ� �Լ�
*
*	\return ���ø����̼ǿ��� ���Ǵ� 3���� ��ü �������� ���۷���
*/
PEditor &theEditor()
{
	return theApp.theEditor;
}

/*! 
*	\brief ���ø����̼ǿ��� ���Ǵ� 3���� ��ü�� ���ñ⸦ ��ȯ�ϴ� �Լ�
*
*	\return ���ø����̼ǿ��� ���Ǵ� 3���� ��ü ���ñ��� ���۷���
*/
PSelector &theSelector()
{
	return theApp.theEditor.theSelector;
}

// GaiaApp ����
GaiaApp::GaiaApp()
{
	// UI ���� �����ϱ�.
	theScene.UIBkgColor[0] = 68;
	theScene.UIBkgColor[1] = 68;
	theScene.UIBkgColor[2] = 68;

	theScene.UItxtColor[0] = 255;
	theScene.UItxtColor[1] = 255;
	theScene.UItxtColor[2] = 255;
}

// GaiaApp �ʱ�ȭ
BOOL GaiaApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	SetRegistryKey(_T("�������б� �׷��Ƚ����뿬����"));
	LoadStdProfileSettings(4);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.

	// ���� ���α׷��� ���� ���ø��� ����մϴ�
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(GaiaDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ������ â�Դϴ�.
		RUNTIME_CLASS(GaiaView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// DDE Execute ���⸦ Ȱ��ȭ�մϴ�.
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// ����ٿ� ������ ����� ����ġ�մϴ�.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// â �ϳ��� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
#ifdef _DEBUG
	m_pMainWnd->SetWindowText(_T("GAIA (Debug)"));
#else
	m_pMainWnd->SetWindowText(_T("GAIA (Release)"));
#endif // _DEBUG

	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
	m_pMainWnd->DragAcceptFiles();

	// �÷����� �ʱ�ȭ
	InitPlugin();

	return TRUE;
}

void GaiaApp::InitPlugin()
{
	// �÷����� �ʱ�ȭ �ϰ�,
	theScene.hWndView = ((CFrameWnd *)m_pMainWnd)->GetActiveView()->GetSafeHwnd();
	theCreator.InitPlugIn(&theScene, &(theMainFrm().m_WorkSpaceDlg), &theDoc());
	theEditor.InitPlugIn(&theScene, &(theMainFrm().m_WorkSpaceDlg), &theDoc());

	// �������쿡 ��Ŀ���� �ش�.
	SetFocus(theScene.hWndView);
}

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void GaiaApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// GaiaApp �޽��� ó����
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

