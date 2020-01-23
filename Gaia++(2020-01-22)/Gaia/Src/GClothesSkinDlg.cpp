// C:\Data\Project\ETRI\2015\3. ���α׷�\GaiaProject\Gaia\Src\GClothesSkinDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "..\Gaia.h"
#include "..\MainFrm.h"
#include "..\GaiaDoc.h"
#include "..\GaiaView.h"
#include "GClothesSkinDlg.h"

// GClothesSkinDlg ��ȭ �����Դϴ�.

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


// GClothesSkinDlg �޽��� ó�����Դϴ�.

GaiaDoc *GClothesSkinDlg::GetDocument()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	GaiaDoc *pDoc = (GaiaDoc *)pFrame->GetActiveDocument();
	return pDoc;
}


void GClothesSkinDlg::OnBnClickedBindByMinDist()
{
	GaiaDoc *pDoc = GetDocument();

	// ���ε��� ���� ������ Ȯ���� ��,
	GMesh *pTarClothesMesh = pDoc->m_pAvatarClothesMesh;
	double min_y = pDoc->m_pAvatarSkirtMesh->BBox.MinPt[1];
	double max_y = pDoc->m_pAvatarSkirtMesh->BBox.MaxPt[1];

	// ������ �� ������ ���Ͽ�,
	int NumClothesVert = pTarClothesMesh->GetNumVert();
	for (int i = 0; i < NumClothesVert; ++i)
	{
		GPoint3 p = pTarClothesMesh->Verts[i].P;
		int vidx = -1;
		double min_dist = 1e+15;
		GMesh *pSrcMesh = NULL;

		// �� ������ ��ĿƮ ������ �ִٸ�,
		if (min_y < p[1] && p[1] < max_y)
		{
			// ���� ����ġ�� �о�� �ҽ� �޽��� ��ĿƮ �޽��� �����ϰ�,
			pSrcMesh = pDoc->m_pAvatarSkirtMesh;
		}
		// �� ������ ��ĿƮ ���� �ۿ� �ִٸ�,
		else
		{
			// ���� ����ġ�� �о�� �ҽ� �޽��� ��Ų �޽��� �����Ѵ�.
			pSrcMesh = pDoc->m_pAvatarSkinMesh;
		}

		// ���� ����� �ҽ� �޽��� ������ �ε����� ã��,
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

		// ���� ����ġ�� �Ҵ��Ѵ�.
		if (vidx != -1)
			pTarClothesMesh->Verts[i].BoneInfoList = pSrcMesh->Verts[vidx].BoneInfoList;		
	}

	pTarClothesMesh->SemanticType = MESH_SKIN;
	pTarClothesMesh->InitRefBoneInfo();
}

void GClothesSkinDlg::OnBnClickedBindBySwpPln()
{
	// ���õ� ���� ��ü�� ������ ���Ͽ�,
	int NumVert = (int)(theEditor().EditObjList.size());

	// ���õ� ��ü�� ���ų�, ���õ� ��ü�� ������ �ƴ϶�� �����Ѵ�.
	if (NumVert == 0 || theEditor().EditObjList[0]->GetClassName() != "GVertex")
		return;

	GaiaDoc *pDoc = GetDocument();

	// ���õ� �������� Verts�� �����ϰ�,
	std::vector<GVertex *> Verts;
	for (int i = 0; i < NumVert; ++i)
		Verts.push_back((GVertex *)(theEditor().EditObjList[i]));
	
	// ���ε� ���� ���� ���� �ҽ� ��Ų �޽��� ���� ��,
	GBindRefPln *pBindPln = m_pBindRefPln;
	GMesh *pSrcSkinMesh = pDoc->m_pAvatarSkinMesh;
	GMesh *pTarClothesMesh = pDoc->m_pAvatarClothesMesh;

	// ���� ���� ���� ����� ���� ������� �ʱ�ȭ �ϰ�,
	GPlane PrevPln = pBindPln->EvalPlane(0.0);
	GPlane CurrPln = PrevPln;

	// ���� ����� ���ø��ϸ鼭,
	int NumPln = 50;
	for (int i = 1; i < NumPln; ++i)
	{
		// ���� ����� ���ϰ�,
		double t = (double)i / (double)(NumPln - 1);
		CurrPln = pBindPln->EvalPlane(t);

		// ���õ� ������ ������ ���Ͽ�,
		for (int j = 0; j < NumVert; ++j)
		{
			// ���� ���� ���� ����� ���̿� �����Ѵٸ�,
			GPoint3 p = Verts[j]->P;
			if (PrevPln.IsAbovePlane(p) && CurrPln.IsBelowPlane(p))
			{
				double t1 = (double)(i - 1) / (double)(NumPln - 1);
				double t2 = t;
				double d1 = ABS(dist(PrevPln, p));
				double d2 = ABS(dist(CurrPln, p));
				double alpha = d1 / (d1 + d2);

				// ������ ������� �Ÿ��� �̿��Ͽ�, ���ε� �Ķ���� t�� ���ϰ�,
				double tt = (1.0 - alpha) * t1 + alpha * t2;

				// ������ �������� r�� ���Ѵ�.
				GPoint3 q = cast_pt3(pBindPln->pTraCrv->Eval(tt));
				GVector3 v = (q - p).Normalize();

				// ������������ ������ ������ ������ ���ϰ�,
				GLine ray(p, v);
				GPoint3 ispt;
				double min_dist = 1000000.0;
				int fidx = -1;
				int NumFace = pSrcSkinMesh->GetNumFace();
				for (int k = 0; k < NumFace; ++k)
				{
					// �ҽ� ��Ų �޽��� ������ �ﰢ���� ���Ͽ�,
					int vidx0 = pSrcSkinMesh->Faces[k].vIdx[0];
					int vidx1 = pSrcSkinMesh->Faces[k].vIdx[1];
					int vidx2 = pSrcSkinMesh->Faces[k].vIdx[2];

					GPoint3 p0 = pSrcSkinMesh->Verts[vidx0].P;
					GPoint3 p1 = pSrcSkinMesh->Verts[vidx1].P;
					GPoint3 p2 = pSrcSkinMesh->Verts[vidx2].P;

					GPoint3 tmp;
					double t, alpha, beta, gamma;
					// ���� �ﰢ���� ������ �����ϸ�,
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

				// ������ �ﰢ���� �����Ѵٸ�,
				if (fidx != -1)
				{
					int vidx0 = pSrcSkinMesh->Faces[fidx].vIdx[0];
					int vidx1 = pSrcSkinMesh->Faces[fidx].vIdx[1];
					int vidx2 = pSrcSkinMesh->Faces[fidx].vIdx[2];

					double d0 = dist(ispt, pSrcSkinMesh->Verts[vidx0].P);
					double d1 = dist(ispt, pSrcSkinMesh->Verts[vidx1].P);
					double d2 = dist(ispt, pSrcSkinMesh->Verts[vidx2].P);

					// d0�� ���� ���� ���,
					if ((d0 < d1 && d1 < d2) || (d0 < d2 && d2 < d1))
						Verts[j]->BoneInfoList = pSrcSkinMesh->Verts[vidx0].BoneInfoList;
					
					// d1�� ���� ���� ���,
					if ((d1 < d2 && d2 < d0) || (d1 < d0 && d0 < d2))
						Verts[j]->BoneInfoList = pSrcSkinMesh->Verts[vidx1].BoneInfoList;
					
					// d2�� ���� ���� ���,
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
