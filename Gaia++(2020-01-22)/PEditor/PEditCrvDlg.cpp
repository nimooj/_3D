// PEditCrvDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PEditCrvDlg.h"


// PEditCrvDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(PEditCrvDlg, CDialog)

PEditCrvDlg::PEditCrvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PEditCrvDlg::IDD, pParent)
	, m_SNumU(0)
	, m_SNumV(0)
	, m_CtrlPtSize(0)
	, m_RenderAsLine(TRUE)
{
}

PEditCrvDlg::~PEditCrvDlg()
{
}

void PEditCrvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SNUM_U, m_SNumU);
	DDX_Text(pDX, IDC_SNUM_V, m_SNumV);
	DDX_Control(pDX, IDC_SNUM_U_SPIN, m_cSNumU);
	DDX_Control(pDX, IDC_SNUM_V_SPIN, m_cSNumV);
	DDX_Text(pDX, IDC_CTRL_PT_SIZE, m_CtrlPtSize);
	DDX_Check(pDX, IDC_RENDER_AS_LINE, m_RenderAsLine);
}


BEGIN_MESSAGE_MAP(PEditCrvDlg, CDialog)
	ON_BN_CLICKED(IDC_REFINE_U, &PEditCrvDlg::OnBnClickedRefineU)
	ON_EN_KILLFOCUS(IDC_CTRL_PT_SIZE, &PEditCrvDlg::OnEnKillfocusCtrlPtSize)
	ON_WM_VSCROLL()
	ON_EN_KILLFOCUS(IDC_SNUM_U, &PEditCrvDlg::OnEnKillfocusSnumU)
	ON_EN_KILLFOCUS(IDC_SNUM_V, &PEditCrvDlg::OnEnKillfocusSnumV)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CNVRT_BZR_FORM, &PEditCrvDlg::OnBnClickedCnvrtBzrForm)
	ON_BN_CLICKED(IDC_CNVRT_BSP_FORM, &PEditCrvDlg::OnBnClickedCnvrtBspForm)
	ON_BN_CLICKED(IDC_ELEVATE_DEGREE, &PEditCrvDlg::OnBnClickedElevateDegree)
	ON_BN_CLICKED(IDC_MODIFY_KNOT_CLAMPED, &PEditCrvDlg::OnBnClickedModifyKnotClamped)
	ON_BN_CLICKED(IDC_MODIFY_KNOT_UNCLAMPED, &PEditCrvDlg::OnBnClickedModifyKnotUnclamped)
	ON_BN_CLICKED(IDC_RENDER_AS_LINE, &PEditCrvDlg::OnBnClickedRenderAsLine)
	ON_BN_CLICKED(IDC_REVERSE_DIRECTION, &PEditCrvDlg::OnBnClickedReverseDirection)
END_MESSAGE_MAP()


// PEditCrvDlg 메시지 처리기입니다.

BOOL PEditCrvDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	static bool bFirst = true;

	// 처음 호출된 경우라면,
	if (bFirst)
	{
		m_cSNumU.SetRange(2, 10000);
		m_cSNumV.SetRange(2, 10000);
		bFirst = false;
		return TRUE;
	}

	if (pEditor == NULL || pDoc == NULL)
		return TRUE;

	// 먼저 디폴트 값을 설정하고,
	m_SNumU = 0;
	m_SNumV = 0;
	
	// 편집 도구에 의해 선택된 편집 객체가 있다면,
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh != NULL)
	{
		// 렌더링 옵션을 재설정하고,
		m_SNumU = pMesh->Renderer.NumSampU;
		m_SNumV = pMesh->Renderer.NumSampV;
		m_CtrlPtSize = (int)(pMesh->Renderer.CtrlPtSize * 100.0 + 0.5) / 100.0;
		m_RenderAsLine = pMesh->Renderer.bRenderCrvAsLine;
	}
	UpdateData(FALSE);	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void PEditCrvDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData(true);

	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL)
		return;

	pEditor->Reset();
	pEditor->EditObjList.push_back(pMesh);
	pMesh->Renderer.NumSampU = m_SNumU;
	pMesh->Renderer.NumSampV = m_SNumV;
	pMesh->ModifiedFlag = MODIFIED_SAMPLE_NUM;
	pMesh->UpdateMesh();

	pDoc->UpdateAllViews(NULL);

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void PEditCrvDlg::OnEnKillfocusSnumU()
{
	UpdateData(true);

	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL)
		return;

	pEditor->Reset();
	pEditor->EditObjList.push_back(pMesh);
	pMesh->Renderer.NumSampU = m_SNumU;
	pMesh->Renderer.NumSampV = m_SNumV;
	pMesh->ModifiedFlag = MODIFIED_SAMPLE_NUM;
	pMesh->UpdateMesh();

	pDoc->UpdateAllViews(NULL);
}

void PEditCrvDlg::OnEnKillfocusSnumV()
{
	UpdateData(true);

	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL)
		return;

	pEditor->Reset();
	pEditor->EditObjList.push_back(pMesh);
	pMesh->Renderer.NumSampU = m_SNumU;
	pMesh->Renderer.NumSampV = m_SNumV;
	pMesh->ModifiedFlag = MODIFIED_SAMPLE_NUM;
	pMesh->UpdateMesh();

	pDoc->UpdateAllViews(NULL);
}

void PEditCrvDlg::OnEnKillfocusCtrlPtSize()
{
	UpdateData(true);
	if (pEditor->EditObjList.empty())
		return;

	int NumObj = pEditor->GetNumEditObj();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = (GMesh *)pEditor->EditObjList[i];
		pMesh->Renderer.CtrlPtSize = m_CtrlPtSize;
		pMesh->ModifiedFlag = MODIFIED_CTLPT_SIZE;
		pMesh->UpdateMesh();
		pEditor->pScene->CtrlPtSize = m_CtrlPtSize;
	}	

	pDoc->UpdateAllViews(NULL);
}

HBRUSH PEditCrvDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	unsigned char *BC = pEditor->pScene->UIBkgColor;
	unsigned char *TC = pEditor->pScene->UItxtColor;
	pDC->SetBkColor(RGB(BC[0], BC[1], BC[2]));
	pDC->SetTextColor(RGB(TC[0], TC[1], TC[2]));
	pDC->SetBkMode(OPAQUE);
	static CBrush BkgBrush(RGB(BC[0], BC[1], BC[2]));
	hbr = (HBRUSH)BkgBrush;

	return hbr;
}

void PEditCrvDlg::OnBnClickedRefineU()
{
	GMesh *pMesh = pEditor->GetEditMesh();

	if (pMesh == NULL)
	{
		AfxMessageBox("넙스 곡선을 선택하세요.");
		return;
	}

	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GNurbsCrv")
	{
		AfxMessageBox("넙스 곡선을 선택하세요.");
		return;
	}

	GNurbsCrv *pCrv = (GNurbsCrv *)pMesh;
	pCrv->KnotRefine();

	pEditor->Reset();
	pEditor->AddToEditObjList(pMesh);
	pDoc->UpdateAllViews(NULL);
}

void PEditCrvDlg::OnBnClickedElevateDegree()
{
	GMesh *pMesh = pEditor->GetEditMesh();

	if (pMesh == NULL)
	{
		AfxMessageBox("넙스 곡선을 선택하세요.");
		return;
	}

	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GNurbsCrv")
	{
		AfxMessageBox("넙스 곡선을 선택하세요.");
		return;
	}

	GNurbsCrv *pCrv = (GNurbsCrv *)pMesh;
	pCrv->ElevateDeg();

	pEditor->Reset();
	pEditor->AddToEditObjList(pMesh);
	pDoc->UpdateAllViews(NULL);
}


void PEditCrvDlg::OnBnClickedCnvrtBzrForm()
{
	GMesh *pMesh = pEditor->GetEditMesh();

	if (pMesh == NULL)
	{
		AfxMessageBox("넙스 곡선을 선택하세요.");
		return;
	}

	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GNurbsCrv")
	{
		AfxMessageBox("넙스 곡선을 선택하세요.");
		return;
	}

	GNurbsCrv *pCrv = (GNurbsCrv *)pMesh;
	pCrv->CnvrtBzrForm();

	pEditor->Reset();
	pEditor->AddToEditObjList(pMesh);
	pDoc->UpdateAllViews(NULL);
}

void PEditCrvDlg::OnBnClickedCnvrtBspForm()
{
	GMesh *pMesh = pEditor->GetEditMesh();

	if (pMesh == NULL)
	{
		AfxMessageBox("넙스 곡선을 선택하세요.");
		return;
	}

	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GNurbsCrv")
	{
		AfxMessageBox("넙스 곡선을 선택하세요.");
		return;
	}

	GNurbsCrv *pCrv = (GNurbsCrv *)pMesh;
	pCrv->CnvrtBspForm();

	pEditor->Reset();
	pEditor->AddToEditObjList(pMesh);
	pDoc->UpdateAllViews(NULL);
}


void PEditCrvDlg::OnBnClickedModifyKnotClamped()
{
	GMesh *pMesh = pEditor->GetEditMesh();

	if (pMesh == NULL)
	{
		AfxMessageBox("넙스 곡선을 선택하세요.");
		return;
	}

	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GNurbsCrv")
	{
		AfxMessageBox("넙스 곡선을 선택하세요.");
		return;
	}

	GNurbsCrv *pCrv = (GNurbsCrv *)pMesh;
	pCrv->KnotClamped();

	pEditor->Reset();
	pEditor->AddToEditObjList(pMesh);
	pDoc->UpdateAllViews(NULL);
}

void PEditCrvDlg::OnBnClickedModifyKnotUnclamped()
{
	GMesh *pMesh = pEditor->GetEditMesh();

	if (pMesh == NULL)
	{
		AfxMessageBox("넙스 곡선을 선택하세요.");
		return;
	}

	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GNurbsCrv")
	{
		AfxMessageBox("넙스 곡선을 선택하세요.");
		return;
	}

	GNurbsCrv *pCrv = (GNurbsCrv *)pMesh;
	pCrv->KnotUnclamped();

	pEditor->Reset();
	pEditor->AddToEditObjList(pMesh);
	pDoc->UpdateAllViews(NULL);
}

void PEditCrvDlg::OnBnClickedReverseDirection()
{
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL || pMesh->GetClassName() != "GNurbsCrv")
	{
		AfxMessageBox("넙스 곡선을 선택하세요.");
		return;
	}

	GNurbsCrv *pCrv = (GNurbsCrv *)pMesh;
	pCrv->Reverse();
	
	pEditor->Reset();
	pEditor->AddToEditObjList(pMesh);
	pDoc->UpdateAllViews(NULL);
}

void PEditCrvDlg::OnBnClickedRenderAsLine()
{
	GMesh *pMesh = pEditor->GetEditMesh();
	m_RenderAsLine = !m_RenderAsLine;
	pMesh->Renderer.bRenderCrvAsLine = !pMesh->Renderer.bRenderCrvAsLine;
	pDoc->UpdateAllViews(NULL);
}


