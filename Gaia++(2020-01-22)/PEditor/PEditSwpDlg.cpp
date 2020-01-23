// PEditSwpDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PEditSwpDlg.h"


// PEditSwpDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(PEditSwpDlg, CDialog)

PEditSwpDlg::PEditSwpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PEditSwpDlg::IDD, pParent)
	, m_SNumU(0)
	, m_SNumV(0)
	, m_SwpRenderType(0)
	, m_SwpPlnWidth(0)
	, m_SwpPlnHeight(0)
{

}

PEditSwpDlg::~PEditSwpDlg()
{
}

void PEditSwpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SNUM_U, m_SNumU);
	DDX_Text(pDX, IDC_SNUM_V, m_SNumV);
	DDX_Control(pDX, IDC_SNUM_U_SPIN, m_cSNumU);
	DDX_Control(pDX, IDC_SNUM_V_SPIN, m_cSNumV);
	DDX_Radio(pDX, IDC_SWP_RENDER_TYPE_SRF, m_SwpRenderType);
	DDX_Text(pDX, IDC_SWP_PLN_WIDTH, m_SwpPlnWidth);
	DDX_Text(pDX, IDC_SWP_PLN_HEIGHT, m_SwpPlnHeight);
}


BEGIN_MESSAGE_MAP(PEditSwpDlg, CDialog)
	ON_WM_VSCROLL()
	ON_EN_KILLFOCUS(IDC_SNUM_U, &PEditSwpDlg::OnEnKillfocusSnumU)
	ON_EN_KILLFOCUS(IDC_SNUM_V, &PEditSwpDlg::OnEnKillfocusSnumV)
	ON_EN_KILLFOCUS(IDC_SWP_PLN_WIDTH, &PEditSwpDlg::OnEnKillfocusSwpPlnWidth)
	ON_EN_KILLFOCUS(IDC_SWP_PLN_HEIGHT, &PEditSwpDlg::OnEnKillfocusSwpPlnHeight)
	ON_BN_CLICKED(IDC_SWP_RENDER_TYPE_SRF, &PEditSwpDlg::OnBnClickedSwpRenderTypeSrf)
	ON_BN_CLICKED(IDC_SWP_RENDER_TYPE_PLN, &PEditSwpDlg::OnBnClickedSwpRenderTypePln)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// PEditSwpDlg �޽��� ó�����Դϴ�.

BOOL PEditSwpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	static bool bFirst = true;

	// ó�� ȣ��� �����,
	if (bFirst)
	{
		m_cSNumU.SetRange(2, 10000);
		m_cSNumV.SetRange(2, 10000);
		bFirst = false;
		return TRUE;
	}

	if (pEditor == NULL || pDoc == NULL)
		return TRUE;

	// ���� ����Ʈ ���� �����ϰ�,
	m_SNumU = 0;
	m_SNumV = 0;
	m_SwpRenderType = 0;
	m_SwpPlnWidth = 0.0;
	m_SwpPlnHeight = 0.0;

	// ���� ������ ���� ���õ� ���� ��ü�� �ִٸ�,
	if (!pEditor->EditObjList.empty())
	{
		// ����Ʈ�� ù��° ��ü�� ���� �޽��� ������ �Ӽ��� ���,
		GMesh *pMesh = pEditor->EditObjList[0]->GetRefMesh();
		GRenderer attrib = pMesh->Renderer;

		// ������ �ɼ��� �缳���ϰ�,
		m_SNumU = attrib.NumSampU;
		m_SNumV = attrib.NumSampV;
		
		// ��������̶��,
		if (pMesh->GetClassName() == "GSweepSrf")
		{
			if (pMesh->Renderer.RenderType & RENDER_SWEEP_PLANE)
				m_SwpRenderType = 1;
			else
				m_SwpRenderType = 0;

			m_SwpPlnWidth = pMesh->Renderer.SwpPlnWidth;
			m_SwpPlnHeight = pMesh->Renderer.SwpPlnHeight;
		}
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void PEditSwpDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData(true);
	if (pEditor->EditObjList.empty())
		return;

	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();

	pSltMesh->Renderer.NumSampU = m_SNumU;
	pSltMesh->Renderer.NumSampV = m_SNumV;
	pSltMesh->ModifiedFlag = MODIFIED_SAMPLE_NUM;
	pSltMesh->UpdateMesh();

	pDoc->UpdateAllViews(NULL);

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void PEditSwpDlg::OnEnKillfocusSnumU()
{
	UpdateData(true);
	if (pEditor->EditObjList.empty())
		return;

	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();

	pSltMesh->Renderer.NumSampU = m_SNumU;
	pSltMesh->Renderer.NumSampV = m_SNumV;
	pSltMesh->ModifiedFlag = MODIFIED_SAMPLE_NUM;
	pSltMesh->UpdateMesh();

	pDoc->UpdateAllViews(NULL);
}

void PEditSwpDlg::OnEnKillfocusSnumV()
{
	UpdateData(true);
	if (pEditor->EditObjList.empty())
		return;

	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();

	pSltMesh->Renderer.NumSampU = m_SNumU;
	pSltMesh->Renderer.NumSampV = m_SNumV;
	pSltMesh->ModifiedFlag = MODIFIED_SAMPLE_NUM;
	pSltMesh->UpdateMesh();

	pDoc->UpdateAllViews(NULL);
}

void PEditSwpDlg::OnEnKillfocusSwpPlnWidth()
{
	UpdateData(true);
	if (pEditor->EditObjList.empty())
		return;

	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh(); 
	if (pSltMesh->GetClassName() == "GSweepSrf")
	{
		pSltMesh->Renderer.SwpPlnWidth = m_SwpPlnWidth;
		pSltMesh->ModifiedFlag = MODIFIED_CTLPT_POS;
		pSltMesh->UpdateMesh();
	}

	pDoc->UpdateAllViews(NULL);
}

void PEditSwpDlg::OnEnKillfocusSwpPlnHeight()
{
	UpdateData(true);
	if (pEditor->EditObjList.empty())
		return;

	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh(); 
	if (pSltMesh->GetClassName() == "GSweepSrf")
	{
		pSltMesh->Renderer.SwpPlnHeight =m_SwpPlnHeight;
		pSltMesh->ModifiedFlag = MODIFIED_CTLPT_POS;
		pSltMesh->UpdateMesh();
	}

	pDoc->UpdateAllViews(NULL);
}

void PEditSwpDlg::OnBnClickedSwpRenderTypeSrf()
{
	if (pEditor->EditObjList.empty())
		return;

	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();
	if (pSltMesh->GetClassName() == "GSweepSrf")
	{
		pSltMesh->Renderer.RenderType &= ~(RENDER_SWEEP_PLANE);
		pSltMesh->ModifiedFlag = MODIFIED_CTLPT_POS;
		pSltMesh->UpdateMesh();
	}

	pDoc->UpdateAllViews(NULL);
}

void PEditSwpDlg::OnBnClickedSwpRenderTypePln()
{
	if (pEditor->EditObjList.empty())
		return;

	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();
	if (pSltMesh->GetClassName() == "GSweepSrf")
	{
		pSltMesh->Renderer.RenderType |= RENDER_SWEEP_PLANE;
		pSltMesh->ModifiedFlag = MODIFIED_CTLPT_POS;
		pSltMesh->UpdateMesh();
	}

	pDoc->UpdateAllViews(NULL);
}

HBRUSH PEditSwpDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
