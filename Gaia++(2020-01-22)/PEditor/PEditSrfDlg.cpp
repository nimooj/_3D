// PEditSrfDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PEditSrfDlg.h"


// PEditSrfDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(PEditSrfDlg, CDialog)

PEditSrfDlg::PEditSrfDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PEditSrfDlg::IDD, pParent)
	, m_SNumU(0)
	, m_SNumV(0)
	, m_CtrlPtSize(0)
{
	pEditor = NULL;
	pDoc = NULL;
}

PEditSrfDlg::~PEditSrfDlg()
{
}

void PEditSrfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SNUM_U, m_SNumU);
	DDX_Text(pDX, IDC_SNUM_V, m_SNumV);
	DDX_Control(pDX, IDC_SNUM_U_SPIN, m_cSNumU);
	DDX_Control(pDX, IDC_SNUM_V_SPIN, m_cSNumV);
	DDX_Text(pDX, IDC_CTRL_PT_SIZE, m_CtrlPtSize);	
}


BEGIN_MESSAGE_MAP(PEditSrfDlg, CDialog)
	ON_BN_CLICKED(IDC_REFINE_U, &PEditSrfDlg::OnBnClickedRefineU)
	ON_BN_CLICKED(IDC_REFINE_V, &PEditSrfDlg::OnBnClickedRefineV)
	ON_WM_VSCROLL()
	ON_EN_KILLFOCUS(IDC_CTRL_PT_SIZE, &PEditSrfDlg::OnEnKillfocusCtrlPtSize)
	ON_EN_KILLFOCUS(IDC_SNUM_U, &PEditSrfDlg::OnEnKillfocusSnumU)
	ON_EN_KILLFOCUS(IDC_SNUM_V, &PEditSrfDlg::OnEnKillfocusSnumV)	
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_ELEVATE_DEG_U, &PEditSrfDlg::OnBnClickedElevateDegU)
	ON_BN_CLICKED(IDC_ELEVATE_DEG_V, &PEditSrfDlg::OnBnClickedElevateDegV)
	ON_BN_CLICKED(IDC_CNVRT_BZR_FORM, &PEditSrfDlg::OnBnClickedCnvrtBzrForm)
	ON_BN_CLICKED(IDC_CNVRT_BSP_FORM, &PEditSrfDlg::OnBnClickedCnvrtBspForm)
	ON_BN_CLICKED(IDC_MODIFY_KNOT_CLAMPED_U, &PEditSrfDlg::OnBnClickedModifyKnotClampedU)
	ON_BN_CLICKED(IDC_MODIFY_KNOT_CLAMPED_V, &PEditSrfDlg::OnBnClickedModifyKnotClampedV)
	ON_BN_CLICKED(IDC_MODIFY_KNOT_UNCLAMPED_U, &PEditSrfDlg::OnBnClickedModifyKnotUnclampedU)
	ON_BN_CLICKED(IDC_MODIFY_KNOT_UNCLAMPED_V, &PEditSrfDlg::OnBnClickedModifyKnotUnclampedV)
END_MESSAGE_MAP()


// PEditSrfDlg �޽��� ó�����Դϴ�.

BOOL PEditSrfDlg::OnInitDialog()
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
	m_CtrlPtSize = 0.0;
	
	// ���� ������ ���� ���õ� ���� ��ü�� �ִٸ�,
	if (!pEditor->EditObjList.empty())
 	{
		// ����Ʈ�� ù��° ��ü�� ���� �޽��� ������ �Ӽ��� ���,
		GMesh *pMesh = pEditor->EditObjList[0]->GetRefMesh();
		GRenderer attrib = pMesh->Renderer;

		// ������ �ɼ��� �缳���ϰ�,
		m_SNumU = attrib.NumSampU;
		m_SNumV = attrib.NumSampV;
		m_CtrlPtSize = attrib.CtrlPtSize;
	}

	UpdateData(FALSE);	
	return TRUE;
}

void PEditSrfDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
 	UpdateData(true);
	
	GMesh *pSltMesh = pEditor->GetEditMesh();
	if (pSltMesh == NULL)
		return;

	pEditor->Reset();
	pEditor->EditObjList.push_back(pSltMesh);
	pSltMesh->Renderer.NumSampU = m_SNumU;
	pSltMesh->Renderer.NumSampV = m_SNumV;
	pSltMesh->ModifiedFlag = MODIFIED_SAMPLE_NUM;
	pSltMesh->UpdateMesh();

	pDoc->UpdateAllViews(NULL);

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void PEditSrfDlg::OnEnKillfocusCtrlPtSize()
{
 	UpdateData(true);
	GMesh *pSltMesh = pEditor->GetEditMesh();
	if (pSltMesh == NULL)
		return;

 	pSltMesh->Renderer.CtrlPtSize = m_CtrlPtSize;
	pEditor->pScene->CtrlPtSize = m_CtrlPtSize;

 	pDoc->UpdateAllViews(NULL);
}

void PEditSrfDlg::OnEnKillfocusSnumU()
{
 	UpdateData(true);
	GMesh *pSltMesh = pEditor->GetEditMesh();
	if (pSltMesh == NULL)
		return;

	pEditor->Reset();
	pEditor->EditObjList.push_back(pSltMesh);
	pSltMesh->Renderer.NumSampU = m_SNumU;
	pSltMesh->ModifiedFlag = MODIFIED_SAMPLE_NUM;
	pSltMesh->UpdateMesh();

	pDoc->UpdateAllViews(NULL);
}

void PEditSrfDlg::OnEnKillfocusSnumV()
{
	UpdateData(true);
	GMesh *pSltMesh = pEditor->GetEditMesh();
	if (pSltMesh == NULL)
		return;

	pEditor->Reset();
	pEditor->EditObjList.push_back(pSltMesh);
	pSltMesh->Renderer.NumSampV = m_SNumV;
	pSltMesh->ModifiedFlag = MODIFIED_SAMPLE_NUM;
	pSltMesh->UpdateMesh();

	pDoc->UpdateAllViews(NULL);
}


HBRUSH PEditSrfDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void PEditSrfDlg::OnBnClickedRefineU()
{
	GMesh *pMesh = pEditor->GetEditMesh();

	if (pMesh == NULL)
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GNurbsSrf")
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	GNurbsSrf *pSrf = (GNurbsSrf *)pMesh;
	pSrf->KnotRefine(true, false);

	pEditor->Reset();
	pEditor->AddToEditObjList(pMesh);
	pDoc->UpdateAllViews(NULL);
}

void PEditSrfDlg::OnBnClickedRefineV()
{
	GMesh *pMesh = pEditor->GetEditMesh();

	if (pMesh == NULL)
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GNurbsSrf")
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	GNurbsSrf *pSrf = (GNurbsSrf *)pMesh;
	pSrf->KnotRefine(false, true);

	pEditor->Reset();
	pEditor->AddToEditObjList(pMesh);
	pDoc->UpdateAllViews(NULL);
}

void PEditSrfDlg::OnBnClickedElevateDegU()
{
	GMesh *pMesh = pEditor->GetEditMesh();

	if (pMesh == NULL)
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GNurbsSrf")
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	GNurbsSrf *pSrf = (GNurbsSrf *)pMesh;
	pSrf->ElevateDeg(true, false);

	pEditor->Reset();
	pEditor->AddToEditObjList(pMesh);
	pDoc->UpdateAllViews(NULL);
}

void PEditSrfDlg::OnBnClickedElevateDegV()
{
	GMesh *pMesh = pEditor->GetEditMesh();

	if (pMesh == NULL)
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GNurbsSrf")
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	GNurbsSrf *pSrf = (GNurbsSrf *)pMesh;
	pSrf->ElevateDeg(false, true);

	pEditor->Reset();
	pEditor->AddToEditObjList(pMesh);
	pDoc->UpdateAllViews(NULL);
}

void PEditSrfDlg::OnBnClickedCnvrtBzrForm()
{
	GMesh *pMesh = pEditor->GetEditMesh();

	if (pMesh == NULL)
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GNurbsSrf")
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	GNurbsSrf *pSrf = (GNurbsSrf *)pMesh;
	pSrf->CnvrtBzrForm(true, false);
	pSrf->CnvrtBzrForm(false, true);

	pEditor->Reset();
	pEditor->AddToEditObjList(pMesh);
	pDoc->UpdateAllViews(NULL);
}

void PEditSrfDlg::OnBnClickedCnvrtBspForm()
{
	GMesh *pMesh = pEditor->GetEditMesh();

	if (pMesh == NULL)
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GNurbsSrf")
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	GNurbsSrf *pSrf = (GNurbsSrf *)pMesh;
	pSrf->CnvrtBspForm(true, true);

	pEditor->Reset();
	pEditor->AddToEditObjList(pMesh);
	pDoc->UpdateAllViews(NULL);
}

void PEditSrfDlg::OnBnClickedModifyKnotClampedU()
{
	GMesh *pMesh = pEditor->GetEditMesh();

	if (pMesh == NULL)
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GNurbsSrf")
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	GNurbsSrf *pSrf = (GNurbsSrf *)pMesh;
	pSrf->KnotClamped(true, false);

	pEditor->Reset();
	pEditor->AddToEditObjList(pMesh);
	pDoc->UpdateAllViews(NULL);
}

void PEditSrfDlg::OnBnClickedModifyKnotClampedV()
{
	GMesh *pMesh = pEditor->GetEditMesh();

	if (pMesh == NULL)
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GNurbsSrf")
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	GNurbsSrf *pSrf = (GNurbsSrf *)pMesh;
	pSrf->KnotClamped(false, true);

	pEditor->Reset();
	pEditor->AddToEditObjList(pMesh);
	pDoc->UpdateAllViews(NULL);
}

void PEditSrfDlg::OnBnClickedModifyKnotUnclampedU()
{
	GMesh *pMesh = pEditor->GetEditMesh();

	if (pMesh == NULL)
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GNurbsSrf")
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	GNurbsSrf *pSrf = (GNurbsSrf *)pMesh;
	pSrf->KnotUnclamped(true, false);

	pEditor->Reset();
	pEditor->AddToEditObjList(pMesh);
	pDoc->UpdateAllViews(NULL);
}

void PEditSrfDlg::OnBnClickedModifyKnotUnclampedV()
{
	GMesh *pMesh = pEditor->GetEditMesh();

	if (pMesh == NULL)
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GNurbsSrf")
	{
		AfxMessageBox("�ҽ� ����� �����ϼ���.");
		return;
	}

	GNurbsSrf *pSrf = (GNurbsSrf *)pMesh;
	pSrf->KnotUnclamped(false, true);

	pEditor->Reset();
	pEditor->AddToEditObjList(pMesh);
	pDoc->UpdateAllViews(NULL);
}
