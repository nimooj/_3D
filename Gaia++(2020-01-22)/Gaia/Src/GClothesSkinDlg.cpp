// C:\Data\Project\ETRI\2015\3. 프로그램\GaiaProject\Gaia\Src\GClothesSkinDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "..\Gaia.h"
#include "..\MainFrm.h"
#include "..\GaiaDoc.h"
#include "..\GaiaView.h"
#include "GClothesSkinDlg.h"

// GClothesSkinDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(GClothesSkinDlg, CDialog)

GClothesSkinDlg::GClothesSkinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GClothesSkinDlg::IDD, pParent)
	, m_BindRefPlnIdx(0)
{

}

GClothesSkinDlg::~GClothesSkinDlg()
{
}

void GClothesSkinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_SHOW_BIND_REF_PLN_LS, m_BindRefPlnIdx);
}


BEGIN_MESSAGE_MAP(GClothesSkinDlg, CDialog)
	ON_BN_CLICKED(IDC_BIND_BY_MIN_DIST, &GClothesSkinDlg::OnBnClickedBindByMinDist)
	ON_BN_CLICKED(IDC_BIND_BY_SWP_PLN, &GClothesSkinDlg::OnBnClickedBindBySwpPln)
	ON_BN_CLICKED(IDC_SHOW_BIND_REF_PLN_LS, &GClothesSkinDlg::OnBnClickedShowBindRefPlnLs)
	ON_BN_CLICKED(IDC_SHOW_BIND_REF_PLN_RS, &GClothesSkinDlg::OnBnClickedShowBindRefPlnRs)
	ON_BN_CLICKED(IDC_HIDE_BIND_REF_PLN, &GClothesSkinDlg::OnBnClickedHideBindRefPln)
END_MESSAGE_MAP()


// GClothesSkinDlg 메시지 처리기입니다.

GaiaDoc *GClothesSkinDlg::GetDocument()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	GaiaDoc *pDoc = (GaiaDoc *)pFrame->GetActiveDocument();
	return pDoc;
}


void GClothesSkinDlg::OnBnClickedBindByMinDist()
{
	GaiaDoc *pDoc = GetDocument();

	// 바인딩을 위한 조건을 확인한 후,
	GMesh *pTarClothesMesh = pDoc->m_pAvatarClothesMesh;
	double min_y = pDoc->m_pAvatarSkirtMesh->BBox.MinPt[1];
	double max_y = pDoc->m_pAvatarSkirtMesh->BBox.MaxPt[1];

	// 각각의 옷 정점에 대하여,
	int NumClothesVert = pTarClothesMesh->GetNumVert();
	for (int i = 0; i < NumClothesVert; ++i)
	{
		GPoint3 p = pTarClothesMesh->Verts[i].P;
		int vidx = -1;
		double min_dist = 1e+15;
		GMesh *pSrcMesh = NULL;

		// 옷 정점이 스커트 영역에 있다면,
		if (min_y < p[1] && p[1] < max_y)
		{
			// 변형 가중치를 읽어올 소스 메쉬를 스커트 메쉬로 설정하고,
			pSrcMesh = pDoc->m_pAvatarSkirtMesh;
		}
		// 옷 정점이 스커트 영역 밖에 있다면,
		else
		{
			// 변형 가중치를 읽어올 소스 메쉬를 스킨 메쉬로 설정한다.
			pSrcMesh = pDoc->m_pAvatarSkinMesh;
		}

		// 가장 가까운 소스 메쉬의 정점의 인덱스를 찾아,
		int NumVert = pSrcMesh->GetNumVert();
		for (int j = 0; j < NumVert; ++j)
		{
			GPoint3 q = pSrcMesh->Verts[j].P;

			double d = dist(p, q);
			if (d < min_dist)
			{
				vidx = j;
				min_dist = d;
			}
		}

		// 변형 가중치를 할당한다.
		if (vidx != -1)
			pTarClothesMesh->Verts[i].BoneInfoList = pSrcMesh->Verts[vidx].BoneInfoList;		
	}

	pTarClothesMesh->SemanticType = MESH_SKIN;
	pTarClothesMesh->InitRefBoneInfo();
}

void GClothesSkinDlg::OnBnClickedBindBySwpPln()
{
	// 선택된 편집 객체의 개수를 구하여,
	int NumVert = (int)(theEditor().EditObjList.size());

	// 선택된 객체가 없거나, 선택된 객체가 정점이 아니라면 리턴한다.
	if (NumVert == 0 || theEditor().EditObjList[0]->GetClassName() != "GVertex")
		return;

	GaiaDoc *pDoc = GetDocument();

	// 선택된 정점들을 Verts에 복사하고,
	std::vector<GVertex *> Verts;
	for (int i = 0; i < NumVert; ++i)
		Verts.push_back((GVertex *)(theEditor().EditObjList[i]));
	
	// 바인딩 참조 스윕 평면과 소스 스킨 메쉬를 구한 후,
	GBindRefPln *pBindPln = m_pBindRefPln;
	GMesh *pSrcSkinMesh = pDoc->m_pAvatarSkinMesh;
	GMesh *pTarClothesMesh = pDoc->m_pAvatarClothesMesh;

	// 이전 평면과 현재 평면을 시작 평면으로 초기화 하고,
	GPlane PrevPln = pBindPln->EvalPlane(0.0);
	GPlane CurrPln = PrevPln;

	// 스윕 평면을 샘플링하면서,
	int NumPln = 50;
	for (int i = 1; i < NumPln; ++i)
	{
		// 현재 평면을 구하고,
		double t = (double)i / (double)(NumPln - 1);
		CurrPln = pBindPln->EvalPlane(t);

		// 선택된 각각의 정점에 대하여,
		for (int j = 0; j < NumVert; ++j)
		{
			// 이전 평면과 현재 평면의 사이에 존재한다면,
			GPoint3 p = Verts[j]->P;
			if (PrevPln.IsAbovePlane(p) && CurrPln.IsBelowPlane(p))
			{
				double t1 = (double)(i - 1) / (double)(NumPln - 1);
				double t2 = t;
				double d1 = ABS(dist(PrevPln, p));
				double d2 = ABS(dist(CurrPln, p));
				double alpha = d1 / (d1 + d2);

				// 정점과 평면들과의 거리를 이용하여, 바인딩 파라미터 t를 구하고,
				double tt = (1.0 - alpha) * t1 + alpha * t2;

				// 정점의 투영방향 r을 구한다.
				GPoint3 q = cast_pt3(pBindPln->pTraCrv->Eval(tt));
				GVector3 v = (q - p).Normalize();

				// 투영방향으로 정점을 지나는 직선을 구하고,
				GLine ray(p, v);
				GPoint3 ispt;
				double min_dist = 1000000.0;
				int fidx = -1;
				int NumFace = pSrcSkinMesh->GetNumFace();
				for (int k = 0; k < NumFace; ++k)
				{
					// 소스 스킨 메쉬의 각각의 삼각형에 대하여,
					int vidx0 = pSrcSkinMesh->Faces[k].vIdx[0];
					int vidx1 = pSrcSkinMesh->Faces[k].vIdx[1];
					int vidx2 = pSrcSkinMesh->Faces[k].vIdx[2];

					GPoint3 p0 = pSrcSkinMesh->Verts[vidx0].P;
					GPoint3 p1 = pSrcSkinMesh->Verts[vidx1].P;
					GPoint3 p2 = pSrcSkinMesh->Verts[vidx2].P;

					GPoint3 tmp;
					double t, alpha, beta, gamma;
					// 만약 삼각형과 직선이 교차하면,
					if (intersect_line_triangle(p0, p1, p2, ray, t, tmp, alpha, beta, gamma, true))
					{
						if ((ABS(t) < ABS(min_dist)))
						{
							ispt = tmp;
							min_dist = t;
							fidx = k;
						}
					}
				}

				// 교차한 삼각형이 존재한다면,
				if (fidx != -1)
				{
					int vidx0 = pSrcSkinMesh->Faces[fidx].vIdx[0];
					int vidx1 = pSrcSkinMesh->Faces[fidx].vIdx[1];
					int vidx2 = pSrcSkinMesh->Faces[fidx].vIdx[2];

					double d0 = dist(ispt, pSrcSkinMesh->Verts[vidx0].P);
					double d1 = dist(ispt, pSrcSkinMesh->Verts[vidx1].P);
					double d2 = dist(ispt, pSrcSkinMesh->Verts[vidx2].P);

					// d0가 가장 작은 경우,
					if ((d0 < d1 && d1 < d2) || (d0 < d2 && d2 < d1))
						Verts[j]->BoneInfoList = pSrcSkinMesh->Verts[vidx0].BoneInfoList;
					
					// d1이 가장 작은 경우,
					if ((d1 < d2 && d2 < d0) || (d1 < d0 && d0 < d2))
						Verts[j]->BoneInfoList = pSrcSkinMesh->Verts[vidx1].BoneInfoList;
					
					// d2가 가장 작은 경우,
					if ((d2 < d1 && d1 < d0) || (d2 < d0 && d0 < d1))
						Verts[j]->BoneInfoList = pSrcSkinMesh->Verts[vidx2].BoneInfoList;
				}
			}
		}
		PrevPln = CurrPln;
	}

	pTarClothesMesh->SemanticType = MESH_SKIN;
	pTarClothesMesh->InitRefBoneInfo();
}

void GClothesSkinDlg::OnBnClickedShowBindRefPlnLs()
{
	GaiaDoc *pDoc = GetDocument();

	m_BindRefPlnIdx = 0;
	m_pBindRefPln = (GBindRefPln *)pDoc->m_pBindRefPlnLeftShoulder;

	GMesh *pLS_BindRefPln = FindMesh(theScene().MeshList, "LS_BindRefPln");
	GMesh *pRS_BindRefPln = FindMesh(theScene().MeshList, "RS_BindRefPln");
	pLS_BindRefPln->ShowMesh(false);
	pRS_BindRefPln->HideMesh(false);
	pLS_BindRefPln->Renderer.RenderType = RENDER_POLYGON | RENDER_WIRE;

	GMesh *pLS_Pivot = FindMesh(theScene().MeshList, "LS_Pivot");
	GMesh *pRS_Pivot = FindMesh(theScene().MeshList, "RS_Pivot");
	pLS_Pivot->ShowMesh(false);
	pRS_Pivot->HideMesh(false);

	theEditor().theSelector.SelectMesh("LS_Pivot");
	pDoc->UpdateAllViews(NULL);
}

void GClothesSkinDlg::OnBnClickedShowBindRefPlnRs()
{
	GaiaDoc *pDoc = GetDocument();

	m_BindRefPlnIdx = 1;
	m_pBindRefPln = (GBindRefPln *)pDoc->m_pBindRefPlnRightShoulder;

	GMesh *pLS_BindRefPln = FindMesh(theScene().MeshList, "LS_BindRefPln");
	GMesh *pRS_BindRefPln = FindMesh(theScene().MeshList, "RS_BindRefPln");
	pLS_BindRefPln->HideMesh(false);
	pRS_BindRefPln->ShowMesh(false);
	pRS_BindRefPln->Renderer.RenderType = RENDER_POLYGON | RENDER_WIRE;

	GMesh *pLS_Pivot = FindMesh(theScene().MeshList, "LS_Pivot");
	GMesh *pRS_Pivot = FindMesh(theScene().MeshList, "RS_Pivot");
	pLS_Pivot->HideMesh(false);
	pRS_Pivot->ShowMesh(false);


	theEditor().theSelector.SelectMesh("RS_Pivot");
	pDoc->UpdateAllViews(NULL);
}

void GClothesSkinDlg::OnBnClickedHideBindRefPln()
{
	GaiaDoc *pDoc = GetDocument();

	m_BindRefPlnIdx = 1;
	m_pBindRefPln = (GBindRefPln *)pDoc->m_pBindRefPlnRightShoulder;

	GMesh *pLS_BindRefPln = FindMesh(theScene().MeshList, "LS_BindRefPln");
	GMesh *pRS_BindRefPln = FindMesh(theScene().MeshList, "RS_BindRefPln");
	pLS_BindRefPln->HideMesh(false);
	pRS_BindRefPln->HideMesh(false);

	GMesh *pLS_Pivot = FindMesh(theScene().MeshList, "LS_Pivot");
	GMesh *pRS_Pivot = FindMesh(theScene().MeshList, "RS_Pivot");
	pLS_Pivot->HideMesh(false);
	pRS_Pivot->HideMesh(false);


	theEditor().theSelector.Reset();
	pDoc->UpdateAllViews(NULL);
}

BOOL GClothesSkinDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GaiaDoc *pDoc = GetDocument();
	m_BindRefPlnIdx = 0;
	m_pBindRefPln = (GBindRefPln *)pDoc->m_pBindRefPlnLeftShoulder;

	return TRUE;  // return TRUE unless you set the focus to a control
}
