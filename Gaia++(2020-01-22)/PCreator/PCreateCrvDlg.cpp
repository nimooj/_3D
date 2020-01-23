// PCreateCrvDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PCreateCrvDlg.h"

// PCreateCrvDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(PCreateCrvDlg, CDialog)

PCreateCrvDlg::PCreateCrvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PCreateCrvDlg::IDD, pParent)
	, m_Name(_T("Crv_"))
	, m_Degree(3)
	, m_TypeCreate(0)
	, m_TypeKnot(0)
	, m_TypeRational(0)
	, pCreator(NULL)
	, m_bFillCrv(FALSE)
	, m_bViewPlnCrv(FALSE)
{

}

PCreateCrvDlg::~PCreateCrvDlg()
{
}

void PCreateCrvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CRV_NAME, m_Name);
	DDX_Text(pDX, IDC_CRV_DEGREE, m_Degree);
	DDX_Radio(pDX, IDC_CRV_CTRL_PT, m_TypeCreate);
	DDX_Radio(pDX, IDC_KNOT_CALMPED, m_TypeKnot);
	DDX_Radio(pDX, IDC_CRV_RATIONAL, m_TypeRational);
	DDX_Check(pDX, IDC_FILL_CRV, m_bFillCrv);
	DDX_Check(pDX, IDC_VIEW_PLN_CRV, m_bViewPlnCrv);
}


BEGIN_MESSAGE_MAP(PCreateCrvDlg, CDialog)
	ON_EN_KILLFOCUS(IDC_CRV_NAME, &PCreateCrvDlg::OnEnKillfocusCrvName)
	ON_EN_KILLFOCUS(IDC_CRV_DEGREE, &PCreateCrvDlg::OnEnKillfocusCrvDegree)
	ON_BN_CLICKED(IDC_CRV_CTRL_PT, &PCreateCrvDlg::OnBnClickedCrvCtrlPt)
	ON_BN_CLICKED(IDC_CRV_INTER_PT, &PCreateCrvDlg::OnBnClickedCrvInterPt)
	ON_BN_CLICKED(IDC_KNOT_CALMPED, &PCreateCrvDlg::OnBnClickedKnotCalmped)
	ON_BN_CLICKED(IDC_KNOT_UNCLAMPED, &PCreateCrvDlg::OnBnClickedKnotUnclamped)
	ON_BN_CLICKED(IDC_KNOT_PERIOD, &PCreateCrvDlg::OnBnClickedKnotPeriod)
	ON_BN_CLICKED(IDC_CRV_RATIONAL, &PCreateCrvDlg::OnBnClickedCrvRational)
	ON_BN_CLICKED(IDC_CRV_POLYNOMIAL, &PCreateCrvDlg::OnBnClickedCrvPolynomial)
	ON_BN_CLICKED(IDC_FILL_CRV, &PCreateCrvDlg::OnBnClickedFillCrv)
	ON_BN_CLICKED(IDC_VIEW_PLN_CRV, &PCreateCrvDlg::OnBnClickedViewPlnCrv)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


BOOL PCreateCrvDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	if (pCreator != NULL)
	{
		std::string name = create_mesh_name("crv");
		m_Name = name.c_str();
		UpdateData(false);
	}

	m_Degree = 3;
	m_TypeCreate = 0;
	m_TypeKnot = 0;
	m_TypeRational = 0;
	m_bFillCrv = FALSE;

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}


void PCreateCrvDlg::OnEnKillfocusCrvName()
{
	UpdateData();
}

void PCreateCrvDlg::OnEnKillfocusCrvDegree()
{
	UpdateData();
}

void PCreateCrvDlg::OnBnClickedCrvCtrlPt()
{
	UpdateData();
	pCreator->CreateType = CREATE_CRV_CTL_PT;
}

void PCreateCrvDlg::OnBnClickedCrvInterPt()
{
	UpdateData();
	pCreator->CreateType = CREATE_CRV_INT_PT;
	m_TypeKnot = 0;
	UpdateData(FALSE);
}

void PCreateCrvDlg::OnBnClickedKnotCalmped()
{
	UpdateData();
}

void PCreateCrvDlg::OnBnClickedKnotUnclamped()
{
	UpdateData();
}

void PCreateCrvDlg::OnBnClickedKnotPeriod()
{
	UpdateData();
}

void PCreateCrvDlg::OnBnClickedCrvRational()
{
	UpdateData();
}

void PCreateCrvDlg::OnBnClickedCrvPolynomial()
{
	UpdateData();
}

void PCreateCrvDlg::OnBnClickedFillCrv()
{
	m_bFillCrv = !m_bFillCrv;
}

void PCreateCrvDlg::OnBnClickedViewPlnCrv()
{
	m_bViewPlnCrv = !m_bViewPlnCrv;
}

HBRUSH PCreateCrvDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	unsigned char *BC = pCreator->pScene->UIBkgColor;
	unsigned char *TC = pCreator->pScene->UItxtColor;
	pDC->SetBkColor(RGB(BC[0], BC[1], BC[2]));
	pDC->SetTextColor(RGB(TC[0], TC[1], TC[2]));
	pDC->SetBkMode(OPAQUE);
	static CBrush BkgBrush(RGB(BC[0], BC[1], BC[2]));
	hbr = (HBRUSH)BkgBrush;

	return hbr;
}
