// GBridgeMeshDlg.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "..\Gaia.h"
#include "..\MainFrm.h"
#include "..\GaiaDoc.h"
#include "..\GaiaView.h"
#include "GBridgeMeshDlg.h"
#include "GBridgeCrvView.h"


// GBridgeMeshDlg 대화 상자입니다.
IMPLEMENT_DYNAMIC(GBridgeMeshDlg, CDialog)

GBridgeMeshDlg::GBridgeMeshDlg(CWnd* pParent /*=NULL*/) 
: CDialog(GBridgeMeshDlg::IDD, pParent)
{
	m_pView = NULL;
	m_SltMesh = 0;
	m_BlendParam = 0.5;	

	/*! \brief 브리지의 시작 메쉬에 대한 정보 */
	m_pStartMesh = NULL;
	m_pStartVert = NULL;
	m_pStartUnfoldMesh = NULL;
	m_pStartContCrv = NULL;
	m_StartCrvSize = 0.0;
	m_StartCrvEditParam = 0.0;
	m_StartCrvEditRes = 0;
	m_StartAuxCrvSclParam = 0.01;
		
	/*! \brief 브리지의 끝 메쉬에 대한 정보 */
	m_pEndMesh = NULL;
	m_pEndVert = NULL;
	m_pEndUnfoldMesh = NULL;
	m_pEndContCrv = NULL;
	m_EndCrvSize = 0.0;
	m_EndCrvEditParam = 0.0;
	m_EndCrvEditRes = 0;
	m_EndAuxCrvSclParam = 0.01;
	
	/*! \brief 생성된 브리지 메쉬에 대한 정보 */
	m_pBridgeMesh = NULL;
}

GBridgeMeshDlg::~GBridgeMeshDlg()
{
	if (m_pView != NULL)
		delete m_pView;

	if (m_pStartContCrv != NULL)
		delete m_pStartContCrv;

	if (m_pStartUnfoldMesh != NULL)
		delete m_pStartUnfoldMesh;

	if (m_pEndContCrv != NULL)
		delete m_pEndContCrv;
	
	if (m_pEndUnfoldMesh != NULL)
		delete m_pEndUnfoldMesh;
}

void GBridgeMeshDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONTACT_CRV_TYPE, m_cContactCrvType);
	DDX_Radio(pDX, IDC_START_MESH, m_SltMesh);
}


BEGIN_MESSAGE_MAP(GBridgeMeshDlg, CDialog)
	ON_BN_CLICKED(IDC_SCREEN_CAPTURE, &GBridgeMeshDlg::OnBnClickedScreenCapture)
	ON_BN_CLICKED(IDC_CREATE_BRIDGE_MESH, &GBridgeMeshDlg::OnBnClickedCreateBridgeMesh)
	ON_WM_HSCROLL()
	ON_CBN_CLOSEUP(IDC_CONTACT_CRV_TYPE, &GBridgeMeshDlg::OnCbnCloseupContactCrvType)
	ON_BN_CLICKED(IDC_END_MESH, &GBridgeMeshDlg::OnBnClickedEndMesh)
	ON_BN_CLICKED(IDC_START_MESH, &GBridgeMeshDlg::OnBnClickedStartMesh)
	ON_BN_CLICKED(IDC_COMPLETE_BRIDGE_MESH, &GBridgeMeshDlg::OnBnClickedCompleteBridgeMesh)
END_MESSAGE_MAP()


// GBridgeMeshDlg 메시지 처리기입니다.


BOOL GBridgeMeshDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	static bool bInit = false;

	if (m_pView == NULL)
	{
		// OpenGL 뷰를 생성하고,
		m_pView = new GBridgeCrvView();
		m_pView->Create(NULL, "Contact Curve View", WS_VISIBLE | WS_CHILD | WS_BORDER, CRect(12, 12, 524, 524), this, IDW_BRIDGE_CRV_VIEW);
		m_pView->m_pParentDlg = this;
	}

	if (!bInit)
	{
		m_cContactCrvType.AddString("Circle");
		m_cContactCrvType.AddString("Square");
		m_cContactCrvType.SetCurSel(0);

		((CSliderCtrl *)GetDlgItem(IDC_CRV_PARAM))->SetRange(0, 100);
		((CSliderCtrl *)GetDlgItem(IDC_CRV_PARAM))->SetPos(0);

		((CSliderCtrl *)GetDlgItem(IDC_CRV_RESOLUTION))->SetRange(0, 10);
		((CSliderCtrl *)GetDlgItem(IDC_CRV_RESOLUTION))->SetPos(0);

		((CSliderCtrl *)GetDlgItem(IDC_AUX_CRV_SCALE_PARAM))->SetRange(1, 100);
		((CSliderCtrl *)GetDlgItem(IDC_AUX_CRV_SCALE_PARAM))->SetPos(1);

		((CSliderCtrl *)GetDlgItem(IDC_BLEND_PARAM))->SetRange(1, 99);
		((CSliderCtrl *)GetDlgItem(IDC_BLEND_PARAM))->SetPos(50);

		bInit = true;
	}	
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void GBridgeMeshDlg::UpdateGLView()
{
	if (m_SltMesh == 0)
	{
		m_pView->m_SltMesh = 0;
		m_pView->m_pMesh = m_pStartMesh;
		m_pView->m_pUnfoldMesh = m_pStartUnfoldMesh;
		m_pView->m_pContCrv = m_pStartContCrv;
		m_pView->m_CrvParam = m_StartCrvEditParam;
		m_pView->m_CrvResolution = m_StartCrvEditRes;
	}
	else
	{
		m_pView->m_SltMesh = 1;
		m_pView->m_pMesh = m_pEndMesh;
		m_pView->m_pUnfoldMesh = m_pEndUnfoldMesh;
		m_pView->m_pContCrv = m_pEndContCrv;
		m_pView->m_CrvParam = m_EndCrvEditParam;
		m_pView->m_CrvResolution = m_EndCrvEditRes;
	}
	m_pView->Invalidate(false);
}


void GBridgeMeshDlg::UpdateBridgeMesh()
{
	update_mesh_bridge(m_pBridgeMesh,
					   m_pStartMesh, m_pStartVert, m_pStartContCrv, m_StartAuxCrvSclParam,
					   m_pEndMesh, m_pEndVert, m_pEndContCrv, m_EndAuxCrvSclParam, 
					   m_BlendParam, 100, 30, false);
	m_pBridgeMesh->CreateMesh();
}

void GBridgeMeshDlg::OnBnClickedScreenCapture()
{
	m_pView->OnEditCopy();
}

void GBridgeMeshDlg::OnBnClickedCreateBridgeMesh()
{
	UpdateData(TRUE);

	GMesh *pBridgeMesh = ::create_mesh_bridge(
		m_pStartMesh, m_pStartVert, m_pStartContCrv, m_StartAuxCrvSclParam,
		m_pEndMesh, m_pEndVert, m_pEndContCrv, m_EndAuxCrvSclParam, 
		m_BlendParam, 100, 30, false);
	pBridgeMesh->Name = ::create_mesh_name("Bridge Mesh");
	theScene().AddMesh(pBridgeMesh);
	theMainFrm().UpdateWorkSpaceDlg();
	theDoc().UpdateAllViews(NULL);
}

void GBridgeMeshDlg::OnBnClickedCompleteBridgeMesh()
{
	UpdateData(TRUE);

	update_mesh_bridge(m_pBridgeMesh,
		m_pStartMesh, m_pStartVert, m_pStartContCrv, m_StartAuxCrvSclParam,
		m_pEndMesh, m_pEndVert, m_pEndContCrv, m_EndAuxCrvSclParam, 
		m_BlendParam, 100, 30, true);
	m_pBridgeMesh->CreateMesh();

	// 초기화
	m_pBridgeMesh = NULL;
	m_pStartMesh = NULL;
	m_pStartVert = NULL;
	m_StartUnfoldFaceMap.clear();
	m_StartCrvSize = 0;
	if (m_pStartContCrv != NULL)
	{
		delete m_pStartContCrv;
		m_pStartContCrv = NULL;
	}
	if (m_pStartUnfoldMesh != NULL)
	{
		delete m_pStartUnfoldMesh;
		m_pStartUnfoldMesh = NULL;
	}

	m_pEndMesh = NULL;
	m_pEndVert = NULL;
	m_EndUnfoldFaceMap.clear();
	m_EndCrvSize = 0;
	if (m_pEndContCrv != NULL)
	{
		delete m_pEndContCrv;
		m_pEndContCrv = NULL;
	}
	if (m_pEndUnfoldMesh != NULL)
	{
		delete m_pEndUnfoldMesh;
		m_pEndUnfoldMesh = NULL;
	}

	theEditor().Reset();
	theMainFrm().UpdateWorkSpaceDlg();
	theDoc().UpdateAllViews(NULL);

	OnOK();	
}

// GBridgeMeshDlg 메시지 처리기입니다.

void GBridgeMeshDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// 컨트롤의 값을 변수에 대입하고,
	UpdateData(true);

	int pos;
	switch (pScrollBar->GetDlgCtrlID())
	{
	case IDC_CRV_PARAM:
		{
			pos = ((CSliderCtrl *)GetDlgItem(IDC_CRV_PARAM))->GetPos();
			if (m_SltMesh == 0)
				m_StartCrvEditParam = pos / 100.0;
			else
				m_EndCrvEditParam = pos / 100.0;
			UpdateGLView();
			UpdateBridgeMesh();
		}
		break;

	case IDC_CRV_RESOLUTION:
		{
			pos = ((CSliderCtrl *)GetDlgItem(IDC_CRV_RESOLUTION))->GetPos();
			if (m_SltMesh == 0)
				m_StartCrvEditRes = pos;
			else
				m_EndCrvEditRes = pos;
			UpdateGLView();
			UpdateBridgeMesh();
		}
		break;

	case IDC_AUX_CRV_SCALE_PARAM:
		{
			pos = ((CSliderCtrl *)GetDlgItem(IDC_AUX_CRV_SCALE_PARAM))->GetPos();
			if (m_SltMesh == 0)
				m_StartAuxCrvSclParam = pos / 100.0;
			else
				m_EndAuxCrvSclParam = pos / 100.0;
			UpdateGLView();
			UpdateBridgeMesh();
		}
		break;

	case IDC_BLEND_PARAM:
		{
			pos = ((CSliderCtrl *)GetDlgItem(IDC_BLEND_PARAM))->GetPos();
			m_BlendParam = pos / 100.0;
			UpdateGLView();
			UpdateBridgeMesh();
		}
		break;
	}

	theDoc().UpdateAllViews(NULL);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void GBridgeMeshDlg::OnCbnCloseupContactCrvType()
{
	int type = m_cContactCrvType.GetCurSel();
	switch (type)
	{
	case 0:	// Circle
		{
			if (m_SltMesh == 0)
			{
				GNurbsCrv *pCrv = ::get_gnurbs_approx_circle(m_StartCrvSize, 0, true);
				GMaterial mat_blue = get_material(0.0, 0.0, 1.0);
				pCrv->KnotRefine();
				pCrv->KnotRefine();
				pCrv->KnotRefine();
				pCrv->KnotRefine();	
				pCrv->Renderer.RenderType = RENDER_POLYGON;
				pCrv->Renderer.SetSampleNum(20, 200);
				pCrv->Renderer.CtrlPtSize = 0.1;
				pCrv->Renderer.bRenderCrvAsLine = true;
				pCrv->AddMtl(mat_blue);
				pCrv->SetMtl(mat_blue.Name);
				pCrv->CreateMesh();
				if (m_pStartContCrv != NULL)
					delete m_pStartContCrv;
				m_pStartContCrv = pCrv;
			}
			else
			{
				GNurbsCrv *pCrv = ::get_gnurbs_approx_circle(m_EndCrvSize, 0, true);
				GMaterial mat_blue = get_material(0.0, 0.0, 1.0);
				pCrv->KnotRefine();
				pCrv->KnotRefine();
				pCrv->KnotRefine();
				pCrv->KnotRefine();	
				pCrv->Renderer.RenderType = RENDER_POLYGON;
				pCrv->Renderer.SetSampleNum(20, 200);
				pCrv->Renderer.CtrlPtSize = 0.1;
				pCrv->Renderer.bRenderCrvAsLine = true;
				pCrv->AddMtl(mat_blue);
				pCrv->SetMtl(mat_blue.Name);
				pCrv->CreateMesh();
				if (m_pEndContCrv != NULL)
					delete m_pEndContCrv;
				m_pEndContCrv = pCrv;
			}	
		}
		break;

	case 1:	// Square
		{
			if (m_SltMesh == 0)
			{
				GNurbsCrv *pCrv = ::get_gnurbs_rect(m_StartCrvSize * 1.5, m_StartCrvSize * 1.5, 0);
				GMaterial mat_blue = get_material(0.0, 0.0, 1.0);
				pCrv->KnotRefine();
				pCrv->KnotRefine();
				pCrv->KnotRefine();
				pCrv->KnotRefine();	
				pCrv->Renderer.RenderType = RENDER_POLYGON;
				pCrv->Renderer.SetSampleNum(20, 200);
				pCrv->Renderer.CtrlPtSize = 0.1;
				pCrv->Renderer.bRenderCrvAsLine = true;
				pCrv->AddMtl(mat_blue);
				pCrv->SetMtl(mat_blue.Name);
				pCrv->CreateMesh();
				if (m_pStartContCrv != NULL)
					delete m_pStartContCrv;
				m_pStartContCrv = pCrv;
			}
			else
			{
				GNurbsCrv *pCrv = ::get_gnurbs_rect(m_EndCrvSize * 1.5, m_EndCrvSize * 1.5, 0);
				GMaterial mat_blue = get_material(0.0, 0.0, 1.0);
				pCrv->KnotRefine();
				pCrv->KnotRefine();
				pCrv->KnotRefine();
				pCrv->KnotRefine();	
				pCrv->Renderer.RenderType = RENDER_POLYGON;
				pCrv->Renderer.SetSampleNum(20, 200);
				pCrv->Renderer.CtrlPtSize = 0.1;
				pCrv->Renderer.bRenderCrvAsLine = true;
				pCrv->AddMtl(mat_blue);
				pCrv->SetMtl(mat_blue.Name);
				pCrv->CreateMesh();
				if (m_pEndContCrv != NULL)
					delete m_pEndContCrv;
				m_pEndContCrv = pCrv;
			}
		}
		break;
	}

	UpdateGLView();
}

void GBridgeMeshDlg::OnBnClickedStartMesh()
{
	m_SltMesh = 0;
	UpdateGLView();
	((CSliderCtrl *)GetDlgItem(IDC_CRV_PARAM))->SetPos(m_StartCrvEditParam * 100);
	((CSliderCtrl *)GetDlgItem(IDC_CRV_RESOLUTION))->SetPos(m_StartCrvEditRes);
	((CSliderCtrl *)GetDlgItem(IDC_AUX_CRV_SCALE_PARAM))->SetPos(m_StartAuxCrvSclParam * 100);
	
}

void GBridgeMeshDlg::OnBnClickedEndMesh()
{
	m_SltMesh = 1;
	UpdateGLView();
	((CSliderCtrl *)GetDlgItem(IDC_CRV_PARAM))->SetPos(m_EndCrvEditParam * 100);
	((CSliderCtrl *)GetDlgItem(IDC_CRV_RESOLUTION))->SetPos(m_EndCrvEditRes);
	((CSliderCtrl *)GetDlgItem(IDC_AUX_CRV_SCALE_PARAM))->SetPos(m_EndAuxCrvSclParam * 100);
}

