// PEditVolDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PEditVolDlg.h"


// PEditVolDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(PEditVolDlg, CDialog)

PEditVolDlg::PEditVolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PEditVolDlg::IDD, pParent)
	, m_SNumV(0)
	, m_SNumW(0)
	, m_SNumU(0)
	, m_IsoUMin(0)
	, m_IsoUMax(0)
	, m_IsoVMin(0)
	, m_IsoVMax(0)
	, m_IsoWMin(0)
	, m_IsoWMax(0)
{

}

PEditVolDlg::~PEditVolDlg()
{
}

void PEditVolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SNUM_V, m_SNumV);
	DDX_Text(pDX, IDC_SNUM_W, m_SNumW);
	DDX_Text(pDX, IDC_SNUM_U, m_SNumU);
	DDX_Control(pDX, IDC_SNUM_U_SPIN, m_cSNumU);
	DDX_Control(pDX, IDC_SNUM_V_SPIN, m_cSNumV);
	DDX_Control(pDX, IDC_SNUM_W_SPIN, m_cSNumW);
	DDX_Control(pDX, IDC_ISO_U_MIN, m_cIsoUMin);
	DDX_Control(pDX, IDC_ISO_U_MAX, m_cIsoUMax);
	DDX_Control(pDX, IDC_ISO_V_MIN, m_cIsoVMin);
	DDX_Control(pDX, IDC_ISO_V_MAX, m_cIsoVMax);
	DDX_Control(pDX, IDC_ISO_W_MIN, m_cIsoWMin);
	DDX_Control(pDX, IDC_ISO_W_MAX, m_cIsoWMax);
	DDX_Slider(pDX, IDC_ISO_U_MIN, m_IsoUMin);
	DDX_Slider(pDX, IDC_ISO_U_MAX, m_IsoUMax);
	DDX_Slider(pDX, IDC_ISO_V_MIN, m_IsoVMin);
	DDX_Slider(pDX, IDC_ISO_V_MAX, m_IsoVMax);
	DDX_Slider(pDX, IDC_ISO_W_MIN, m_IsoWMin);
	DDX_Slider(pDX, IDC_ISO_W_MAX, m_IsoWMax);
}


BEGIN_MESSAGE_MAP(PEditVolDlg, CDialog)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_EN_KILLFOCUS(IDC_SNUM_U, &PEditVolDlg::OnEnKillfocusSnumU)
	ON_EN_KILLFOCUS(IDC_SNUM_V, &PEditVolDlg::OnEnKillfocusSnumV)
	ON_EN_KILLFOCUS(IDC_SNUM_W, &PEditVolDlg::OnEnKillfocusSnumW)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// PEditVolDlg 메시지 처리기입니다.

BOOL PEditVolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	static bool bFirst = true;

	// 처음 호출된 경우라면,
	if (bFirst)
	{
		m_cSNumU.SetRange(2, 10000);
		m_cSNumV.SetRange(2, 10000);
		m_cSNumW.SetRange(2, 10000);
		m_cIsoUMin.SetRange(0, 100);
		m_cIsoUMax.SetRange(0, 100);
		m_cIsoVMin.SetRange(0, 100);
		m_cIsoVMax.SetRange(0, 100);
		m_cIsoWMin.SetRange(0, 100);
		m_cIsoWMax.SetRange(0, 100);

		bFirst = false;
		return TRUE;
	}

	if (pEditor == NULL || pDoc == NULL)
		return TRUE;

	// 먼저 디폴트 값을 설정하고,
	m_SNumU = 0;
	m_SNumV = 0;
	m_SNumW = 0;
	m_IsoUMin = 0;
	m_IsoUMax = 100;
	m_IsoVMin = 0;
	m_IsoVMax = 100;
	m_IsoWMin = 0;
	m_IsoWMax = 100;
	
	// 편집 도구에 의해 선택된 편집 객체가 있다면,
	if (!pEditor->EditObjList.empty())
	{
		// 리스트의 첫번째 객체가 속한 메쉬와 렌더링 속성을 얻어,
		GMesh *pMesh = pEditor->EditObjList[0]->GetRefMesh();
		GRenderer attrib = pMesh->Renderer;

		// 렌더링 옵션을 재설정하고,
		m_SNumU = attrib.NumSampU;
		m_SNumV = attrib.NumSampV;
		m_SNumW = attrib.NumSampW;
		m_IsoUMin = attrib.IsoSrfParam[0] * 100;
		m_IsoUMax = attrib.IsoSrfParam[1] * 100;
		m_IsoVMin = attrib.IsoSrfParam[2] * 100;
		m_IsoVMax = attrib.IsoSrfParam[3] * 100;
		m_IsoWMin = attrib.IsoSrfParam[4] * 100;
		m_IsoWMax = attrib.IsoSrfParam[5] * 100;
		//m_CtrlPtSize = attrib.CtrlPtSize;
	}

	UpdateData(FALSE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void PEditVolDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData(true);
	if (pEditor->EditObjList.empty())
		return;

	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();

	pSltMesh->Renderer.NumSampU = m_SNumU;
	pSltMesh->Renderer.NumSampV = m_SNumV;
	pSltMesh->Renderer.NumSampW = m_SNumW;
	pSltMesh->ModifiedFlag = MODIFIED_SAMPLE_NUM;
	pSltMesh->UpdateMesh();

	pDoc->UpdateAllViews(NULL);

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void PEditVolDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pEditor->EditObjList.empty())
		return;

	// 컨트롤의 값을 변수에 대입하고,
	UpdateData(TRUE);

	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();

	pSltMesh->Renderer.IsoSrfParam[0] = m_IsoUMin / 100.0;
	pSltMesh->Renderer.IsoSrfParam[1] = m_IsoUMax / 100.0;
	pSltMesh->Renderer.IsoSrfParam[2] = m_IsoVMin / 100.0;
	pSltMesh->Renderer.IsoSrfParam[3] = m_IsoVMax / 100.0;
	pSltMesh->Renderer.IsoSrfParam[4] = m_IsoWMin / 100.0;
	pSltMesh->Renderer.IsoSrfParam[5] = m_IsoWMax / 100.0;
	pSltMesh->UpdateMesh();

	pDoc->UpdateAllViews(NULL);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void PEditVolDlg::OnEnKillfocusSnumU()
{
	UpdateData(true);
	if (pEditor->EditObjList.empty())
		return;

	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();

	pSltMesh->Renderer.NumSampU = m_SNumU;
	pSltMesh->Renderer.NumSampV = m_SNumV;
	pSltMesh->Renderer.NumSampW = m_SNumW;
	pSltMesh->ModifiedFlag = MODIFIED_SAMPLE_NUM;
	pSltMesh->UpdateMesh();

	pDoc->UpdateAllViews(NULL);
}

void PEditVolDlg::OnEnKillfocusSnumV()
{
	UpdateData(true);
	if (pEditor->EditObjList.empty())
		return;

	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();

	pSltMesh->Renderer.NumSampU = m_SNumU;
	pSltMesh->Renderer.NumSampV = m_SNumV;
	pSltMesh->Renderer.NumSampW = m_SNumW;
	pSltMesh->ModifiedFlag = MODIFIED_SAMPLE_NUM;
	pSltMesh->UpdateMesh();

	pDoc->UpdateAllViews(NULL);
}

void PEditVolDlg::OnEnKillfocusSnumW()
{
	UpdateData(true);
	if (pEditor->EditObjList.empty())
		return;

	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();

	pSltMesh->Renderer.NumSampU = m_SNumU;
	pSltMesh->Renderer.NumSampV = m_SNumV;
	pSltMesh->Renderer.NumSampW = m_SNumW;
	pSltMesh->ModifiedFlag = MODIFIED_SAMPLE_NUM;
	pSltMesh->UpdateMesh();

	pDoc->UpdateAllViews(NULL);
}

HBRUSH PEditVolDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
