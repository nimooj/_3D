// PCutterDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PCutter.h"
#include "PCutterDlg.h"

class GaiaApp;
extern GaiaApp *theApp;

// PCutterDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(PCutterDlg, CDialog)

PCutterDlg::PCutterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PCutterDlg::IDD, pParent)
{
	pSubMesh = NULL;
	pParamMesh = NULL;
	p2DCutCrv = NULL;
	p3DCutCrv = NULL;
	pQTree = NULL;
}

PCutterDlg::~PCutterDlg()
{
	if (pSubMesh != NULL)
		delete pSubMesh;

	if (pQTree != NULL)
		delete pQTree;
}

void PCutterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PCutterDlg, CDialog)
	ON_BN_CLICKED(IDC_CUT_MESH, &PCutterDlg::OnBnClickedCutMesh)
	ON_BN_CLICKED(IDC_REFINE_CRV, &PCutterDlg::OnBnClickedRefineCrv)
	ON_BN_CLICKED(IDC_SELECT_PIVOT_VERT, &PCutterDlg::OnBnClickedSelectPivotVert)
	ON_BN_CLICKED(IDC_EDIT_CURVE, &PCutterDlg::OnBnClickedEditCurve)
	ON_BN_CLICKED(IDC_MAP_SLT_MESH, &PCutterDlg::OnBnClickedMapSltMesh)
	ON_BN_CLICKED(IDC_MAP_SLT_FACE, &PCutterDlg::OnBnClickedMapSltFace)
END_MESSAGE_MAP()


// PCutterDlg 메시지 처리기입니다.

BOOL PCutterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Create view for rendering control curves in parametric space
	if (m_pCutterView == NULL)
	{
		// OpenGL 뷰를 생성하고,
		m_pCutterView = new PCutterView();
		m_pCutterView->Create(NULL, "Cutter Domain", WS_VISIBLE | WS_CHILD | WS_BORDER, CRect(12, 12, 800, 800), this, IDW_CUTTER_VIEW);
		m_pCutterView->m_pCutterDlg = this;
		
		// 절단 곡선을 생성하여 장면에 추가하고,
		GVector4 P[4];
		P[0].Set(0.35, 0.35, 0.0, 1.0);
		P[1].Set(0.65, 0.35, 0.0, 1.0);
		P[2].Set(0.65, 0.65, 0.0, 1.0);
		P[3].Set(0.35, 0.65, 0.0, 1.0);
		p2DCutCrv = ::get_gnurbs_crv_closed(2, 3, P, false);
		p2DCutCrv->Name = "ParamCutCrv";
		p2DCutCrv->RenderAttrib.NameMtrl = "mat_blue";
		p2DCutCrv->RenderAttrib.NumSampU = 100;
		p2DCutCrv->RenderAttrib.RenderType |= RENDER_CTRL_PT; 
		p2DCutCrv->RenderAttrib.CtrlPtSize = 0.01;
		p2DCutCrv->CreateMesh();
		theCutterScene.AddMesh(p2DCutCrv);

		// 매개화 메쉬를 생성하여 장면에 추가한다.
		pParamMesh = new GMesh("ParamMesh", MESH_TRI, "");
		theCutterScene.AddMesh(pParamMesh);
	}
	return TRUE; 
}

void PCutterDlg::UpdateQuadTree()
{
	if (pQTree != NULL)
		delete pQTree;

	pQTree = new GQTree(pParamMesh, 10, 10);
}

void PCutterDlg::Update3DCutCrv()
{
	// 응용프로그램의 절단 곡선을 얻어오고,
	p3DCutCrv = FindMesh(theAppScene->MeshList, p2DCutCrv->Name);

	// 응용프로그램에 절단 곡선이 없다면,
	if (p3DCutCrv == NULL)
	{
		// 절단 곡선을 생성하여 추가한다.
		p3DCutCrv = new GNurbsCrv();
		p3DCutCrv->Name = p2DCutCrv->Name;
		//p3DCutCrv->RenderAttrib.RenderType = RENDER_POLYGON | RENDER_WIRE;
		p3DCutCrv->RenderAttrib.NameMtrl = "mat_blue";
		theAppScene->AddMesh(p3DCutCrv);
	}

	p3DCutCrv->Verts.clear();

	int snum = p2DCutCrv->RenderAttrib.NumSampU;
	for (int i = 0; i < snum; ++i)
	{
		double t = (double)i / (double)(snum - 1);
		GVector4 param = p2DCutCrv->Eval(t);
		double alpha, beta;
		int fidx = pQTree->FindFace(GVector2(param[0], param[1]), &alpha, &beta);
		if (fidx != -1)
		{
			GPoint3 p0 = pSubMesh->Verts[pSubMesh->Faces[fidx].vIdx[0]].P;
			GPoint3 p1 = pSubMesh->Verts[pSubMesh->Faces[fidx].vIdx[1]].P;
			GPoint3 p2 = pSubMesh->Verts[pSubMesh->Faces[fidx].vIdx[2]].P;
			GPoint3 p = p0 + (p1 - p0) * alpha + (p2 - p0) * beta;
			p3DCutCrv->AddVertex(GVertex(p, GVector3()));
		}
	}
	
	theAppDoc->UpdateAllViews(NULL);
}

void PCutterDlg::OnBnClickedCutMesh()
{
	if (theAppEditor->EditObjList.empty())
	{
		AfxMessageBox("Select faces to be parameterized in parameter space.");
		return;
	}

	if (theAppEditor->EditObjList[0]->GetClassName() != "GFace")
	{
		AfxMessageBox("Select faces to be parameterized in parameter space.");
		return;
	}
}

void PCutterDlg::OnBnClickedRefineCrv()
{
	theCutterEditor.Reset();
	GNurbsCrv *pCutterCrv = (GNurbsCrv *)theCutterScene.MeshList[0];
	pCutterCrv->KnotRefine();
	m_pCutterView->Invalidate(FALSE);
}

void PCutterDlg::OnBnClickedSelectPivotVert()
{
	theCutterEditor.theSelector.SelectObjType = SELECT_VERT;
	theCutterEditor.EditType = EDIT_INACTIVE;
}

void PCutterDlg::OnBnClickedEditCurve()
{
	theCutterEditor.theSelector.SelectObjType = SELECT_CTRL_PT;
	theCutterEditor.EditType = EDIT_TRA;
}

void PCutterDlg::OnBnClickedMapSltMesh()
{
	// 선택된 삼각형이 없다면 리턴하고,
	if (theAppEditor->EditObjList.empty())
	{
		AfxMessageBox("Select a mesh to be parameterized in parameter space.");
		return;
	}

	// 선택된 객체가 삼각형이 아니라면 리턴하고,
	if (theAppEditor->EditObjList[0]->GetClassName() != "GMesh")
	{
		AfxMessageBox("Select a mesh to be parameterized in parameter space.");
		return;
	}

	// 선택된 삼각형이 포함된 메쉬를 구하고,
	GMesh *pParentMesh = theAppEditor->EditObjList[0]->GetRefMesh();

	// UV 매핑 정보가 없는 메쉬라면 리턴하고,
	if (pParentMesh->Texs.empty())
	{
		AfxMessageBox("Selected mesh doesn't have avaialable UV mapping information.");
		return;
	}

	// 이전 매개화 메쉬를 지우고,
	delete theCutterScene.MeshList[1];

	// 새로운 매개화 메쉬를 생성하고,
	pParamMesh = new GMesh("ParamMesh", MESH_TRI);

	// 선택된 객체의 텍스춰 좌표로 정점의 좌표를 생성하고,
	int NumTex = pParentMesh->GetNumTex();
	for (int i = 0; i < NumTex; ++i)
	{
		GVector2 uv = pParentMesh->Texs[i];
		GVertex v(GPoint3(uv[0], uv[1], 0.0));
		pParamMesh->AddVertex(v);
	}

	int NumFace = pParentMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		int vidx0 = pParentMesh->Faces[i].tIdx[0];
		int vidx1 = pParentMesh->Faces[i].tIdx[1];
		int vidx2 = pParentMesh->Faces[i].tIdx[2];

		GFace f(vidx0, vidx1, vidx2);
		pParamMesh->AddFace(f);
	}
	pParamMesh->CalcBoundBox(false);

	// 매개화 메쉬의 속성을 설정하고,
	pParamMesh->pScene = &theCutterScene;
	pParamMesh->RenderAttrib.RenderType = RENDER_WIRE | RENDER_POINT;

	// 매개화 메쉬를 장면에 추가한다.
	theCutterEditor.Reset();
	theCutterScene.MeshList[1] = pParamMesh;
	m_pCutterView->Invalidate(FALSE);

	// 매개화 메쉬로 쿼드 트리를 갱신(생성)하고,
//	UpdateQuadTree();

	// 응용프로그램의 절단 곡선을 갱신(생성)한다.
//	Update3DCutCrv();	
}

void PCutterDlg::OnBnClickedMapSltFace()
{
	// 선택된 삼각형이 없다면 리턴하고,
	if (theAppEditor->EditObjList.empty())
	{
		AfxMessageBox("Select faces to be parameterized in parameter space.");
		return;
	}

	// 선택된 객체가 삼각형이 아니라면 리턴하고,
	if (theAppEditor->EditObjList[0]->GetClassName() != "GFace")
	{
		AfxMessageBox("Select faces to be parameterized in parameter space.");
		return;
	}

	// 선택된 삼각형이 포함된 메쉬를 구하고,
	GMesh *pParentMesh = theAppEditor->EditObjList[0]->GetRefMesh();

	// 선택된 삼각형으로 부분 메쉬를 생성하고,
	if (pSubMesh)
	{
		delete pSubMesh;
		SubMeshVertIdxMap.clear();
	}
	pSubMesh = ::create_mesh_from_faces(pParentMesh, theAppEditor->EditObjList, SubMeshVertIdxMap);

	int StartVertIdx = -1;
	// 매개화 메쉬의 정점이 선택되었다면, 시작 정점의 인덱스를 구하고,
	if (!theCutterEditor.EditObjList.empty())
	{
		GMesh *pMesh = theCutterEditor.EditObjList[0]->GetRefMesh();
		if (pMesh->Name == "ParamMesh")
			StartVertIdx = pMesh->GetVertIdx((GVertex *)(theCutterEditor.EditObjList[0]));
	}

	// 이전 매개화 메쉬를 지우고,
	delete theCutterScene.MeshList[1];

	// 매개화 메쉬를 계산하고,
	pParamMesh = ::create_mesh_from_uv_map(pSubMesh, StartVertIdx);
	pParamMesh->CalcBoundBox(false);

	// 매개화 메쉬의 속성을 설정하고,
	pParamMesh->Name = "ParamMesh";
	pParamMesh->pScene = &theCutterScene;
	pParamMesh->RenderAttrib.RenderType = RENDER_WIRE | RENDER_POINT;

	// 매개화 메쉬를 장면에 추가한다.
	theCutterEditor.Reset();
	theCutterScene.MeshList[1] = pParamMesh;
	m_pCutterView->Invalidate(FALSE);

	// 매개화 메쉬로 쿼드 트리를 갱신(생성)하고,
	UpdateQuadTree();

	// 응용프로그램의 절단 곡선을 갱신(생성)한다.
	Update3DCutCrv();	
}
