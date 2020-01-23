// PCreateSrfDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PCreateSrfDlg.h"


// PCreateSrfDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(PCreateSrfDlg, CDialog)

PCreateSrfDlg::PCreateSrfDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PCreateSrfDlg::IDD, pParent)
	, m_SrfName(_T("srf_"))
	, m_SrfDegU(3)
	, m_SrfDegV(3)
	, m_NumCtlPtU(4)
	, m_NumCtlPtV(4)
	, m_TypeKnotU(0)
	, m_TypeKnotV(0)
	, m_TypeRational(0)
	, pCreator(NULL)
{
}

PCreateSrfDlg::~PCreateSrfDlg()
{
}

void PCreateSrfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SRF_NAME, m_SrfName);
	DDX_Text(pDX, IDC_SRF_DEG_U, m_SrfDegU);
	DDX_Text(pDX, IDC_SRF_DEG_V, m_SrfDegV);
	DDX_Text(pDX, IDC_NUM_CTLPT_U, m_NumCtlPtU);
	DDX_Text(pDX, IDC_NUM_CTLPT_V, m_NumCtlPtV);
	DDX_Radio(pDX, IDC_KNOT_CLAMPED_U, m_TypeKnotU);
	DDX_Radio(pDX, IDC_KNOT_CLAMPED_V, m_TypeKnotV);
	DDX_Radio(pDX, IDC_SRF_RATIONAL, m_TypeRational);
}


BEGIN_MESSAGE_MAP(PCreateSrfDlg, CDialog)
	ON_EN_KILLFOCUS(IDC_SRF_NAME, &PCreateSrfDlg::OnEnKillfocusSrfName)
	ON_EN_KILLFOCUS(IDC_SRF_DEG_U, &PCreateSrfDlg::OnEnKillfocusSrfDegU)
	ON_EN_SETFOCUS(IDC_SRF_DEG_V, &PCreateSrfDlg::OnEnSetfocusSrfDegV)
	ON_EN_KILLFOCUS(IDC_NUM_CTLPT_U, &PCreateSrfDlg::OnEnKillfocusNumCtlptU)
	ON_EN_KILLFOCUS(IDC_NUM_CTLPT_V, &PCreateSrfDlg::OnEnKillfocusNumCtlptV)
	ON_BN_CLICKED(IDC_KNOT_CLAMPED_U, &PCreateSrfDlg::OnBnClickedKnotClampedU)
	ON_BN_CLICKED(IDC_KNOT_UNCLAMPED_U, &PCreateSrfDlg::OnBnClickedKnotUnclampedU)
	ON_BN_CLICKED(IDC_KNOT_CLAMPED_V, &PCreateSrfDlg::OnBnClickedKnotClampedV)
	ON_BN_CLICKED(IDC_KNOT_UNCLAMPED_V, &PCreateSrfDlg::OnBnClickedKnotUnclampedV)
	ON_BN_CLICKED(IDC_SRF_RATIONAL, &PCreateSrfDlg::OnBnClickedSrfRational)
	ON_BN_CLICKED(IDC_SRF_POLYNOMIAL, &PCreateSrfDlg::OnBnClickedSrfPolynomial)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// PCreateSrfDlg 메시지 처리기입니다.
BOOL PCreateSrfDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (pCreator != NULL)
	{
		std::string name = create_mesh_name("srf");
		m_SrfName = name.c_str();
		UpdateData(false);
	}

	return TRUE;
}

void PCreateSrfDlg::OnEnKillfocusSrfName()
{
	UpdateData();
}

void PCreateSrfDlg::OnEnKillfocusSrfDegU()
{
	UpdateData();
}

void PCreateSrfDlg::OnEnSetfocusSrfDegV()
{
	UpdateData();
}

void PCreateSrfDlg::OnEnKillfocusNumCtlptU()
{
	UpdateData();
}

void PCreateSrfDlg::OnEnKillfocusNumCtlptV()
{
	UpdateData();
}

void PCreateSrfDlg::OnBnClickedKnotClampedU()
{
	UpdateData();
}

void PCreateSrfDlg::OnBnClickedKnotUnclampedU()
{
	UpdateData();
}

void PCreateSrfDlg::OnBnClickedKnotClampedV()
{
	UpdateData();
}

void PCreateSrfDlg::OnBnClickedKnotUnclampedV()
{
	UpdateData();
}

void PCreateSrfDlg::OnBnClickedSrfRational()
{
	UpdateData();
}

void PCreateSrfDlg::OnBnClickedSrfPolynomial()
{
	UpdateData();
}

HBRUSH PCreateSrfDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
