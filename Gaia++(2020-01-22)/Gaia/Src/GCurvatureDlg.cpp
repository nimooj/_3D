// GCurvatureDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "..\Gaia.h"
#include "..\MainFrm.h"
#include "..\GaiaDoc.h"
#include "..\GaiaView.h"
#include "GCurvatureDlg.h"

// GCurvatureDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(GCurvatureDlg, CDialog)

GCurvatureDlg::GCurvatureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GCurvatureDlg::IDD, pParent)
	, m_MinVal(0)
	, m_MaxVal(0)
{
	m_pMesh = NULL;
}

GCurvatureDlg::~GCurvatureDlg()
{
}

void GCurvatureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MIN_VAL, m_MinVal);
	DDX_Text(pDX, IDC_MAX_VAL, m_MaxVal);
	DDX_Control(pDX, IDC_CURVATURE_TYPE, m_cCurvatureType);
}


BEGIN_MESSAGE_MAP(GCurvatureDlg, CDialog)
	ON_EN_KILLFOCUS(IDC_MIN_VAL, &GCurvatureDlg::OnEnKillfocusMinVal)
	ON_EN_KILLFOCUS(IDC_MAX_VAL, &GCurvatureDlg::OnEnKillfocusMaxVal)
	ON_CBN_CLOSEUP(IDC_CURVATURE_TYPE, &GCurvatureDlg::OnCbnCloseupCurvatureType)
END_MESSAGE_MAP()


// GCurvatureDlg 메시지 처리기입니다.

void GCurvatureDlg::OnEnKillfocusMinVal()
{
	UpdateCurvatureRange();	
}

void GCurvatureDlg::OnEnKillfocusMaxVal()
{
	UpdateCurvatureRange();
}

BOOL GCurvatureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	static bool bInit = false;
	if (bInit == false)
	{
		bInit = true;
		return TRUE;
	}

	m_cCurvatureType.ResetContent();
	m_cCurvatureType.AddString("Gauss 곡률");
	m_cCurvatureType.AddString("Mean 곡률");
	m_cCurvatureType.AddString("Min 곡률");
	m_cCurvatureType.AddString("Max 곡률");
	m_cCurvatureType.SetCurSel(0);

	::get_curvature(m_pMesh, m_kG, m_kH, m_kMin, m_kMax);
	
	m_MinVal = m_MaxVal = m_kG[0];
	int NumVert = m_kG.size();
	for (int i = 1; i < NumVert; ++i)
	{
		m_MinVal = MIN(m_MinVal, m_kG[i]);
		m_MaxVal = MAX(m_MaxVal, m_kG[i]);
	}

	m_MinVal = (int)(m_MinVal * 10000 + 0.5) / 10000.0;
	m_MaxVal = (int)(m_MaxVal * 10000 + 0.5) / 10000.0;

	UpdateData(FALSE);
	UpdateCurvatureRange();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void GCurvatureDlg::OnOK()
{
	m_pMesh->Renderer.VertColorType = VERTEX_COLOR_ILLUMINATION;
	m_pMesh = NULL;
	m_kG.clear();
	m_kH.clear();
	m_kMin.clear();
	m_kMax.clear();
	CDialog::OnOK();
}

void GCurvatureDlg::OnCbnCloseupCurvatureType()
{
	UpdateData(TRUE);	// 콘트롤->변수

	GMesh *pMesh = ::FindMesh(theScene().MeshList, m_pMesh->Name);
	if (pMesh == NULL)
	{
		OnOK();
		return;
	}

	m_MinVal = 10000000.0;
	m_MaxVal = -10000000.0;
	int CurvatureType = m_cCurvatureType.GetCurSel();
	if (CurvatureType == -1 )
		return;

	std::vector<double> *K;
	switch (CurvatureType)
	{
	case 0:
		K = &m_kG;
		break;
	case 1:
		K = &m_kH;
		break;
	case 2:
		K = &m_kMin;
		break;
	case 3:
		K = &m_kMax;
		break;
	}

	for (int i = 1; i < pMesh->GetNumVert(); ++i)
	{
		m_MinVal = MIN(m_MinVal, (*K)[i]);
		m_MaxVal = MAX(m_MaxVal, (*K)[i]);			
	}

	m_MinVal = (int)(m_MinVal * 10000 + 0.5) / 10000.0;
	m_MaxVal = (int)(m_MaxVal * 10000 + 0.5) / 10000.0;

	UpdateData(FALSE);	// 변수->콘트롤
	UpdateCurvatureRange();	
}

void GCurvatureDlg::UpdateCurvatureRange()
{
	UpdateData(TRUE);	// 콘트롤->변수

	int CurvatureType = m_cCurvatureType.GetCurSel();
	if (CurvatureType == -1 )
		return;

	std::vector<double> *K;
	switch (CurvatureType)
	{
	case 0:
		K = &m_kG;
		break;
	case 1:
		K = &m_kH;
		break;
	case 2:
		K = &m_kMin;
		break;
	case 3:
		K = &m_kMax;
		break;
	}

	int NumVert = m_pMesh->GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		double t;

		if ((*K)[i] < m_MinVal)
			t = 0.0;
		else if ((*K)[i] > m_MaxVal)
			t = 1.0;
		else
			t = ((*K)[i] - m_MinVal) / (m_MaxVal - m_MinVal);

		//	m_pMesh->Verts[i].C = get_color_gradient_from_rainbow(t);
		m_pMesh->Verts[i].C = get_color_gradient(t, GVector3(0.0, 0.0, 1.0), GVector3(0.7, 0.7, 0.7), GVector3(1.0, 0.0, 0.0));
	}

	m_pMesh->ModifiedFlag = MODIFIED_NORMAL;
	m_pMesh->UpdateMesh();

	theDoc().UpdateAllViews(NULL);
}
