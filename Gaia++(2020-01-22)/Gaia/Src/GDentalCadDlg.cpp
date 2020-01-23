// C:\Data\Research\Gaia++\Gaia\Src\GDentalCadDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "..\Gaia.h"
#include "..\MainFrm.h"
#include "GDentalCadDlg.h"

// GDentalCadDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(GDentalCadDlg, CDialog)

GDentalCadDlg::GDentalCadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GDentalCadDlg::IDD, pParent)
	, m_SplintWidth(0)
	, m_SplintWidthByNormal(0)
	, m_bShowUpperJaw(FALSE)
	, m_bShowLowerJaw(FALSE)
	, m_bShowKeyPlane(FALSE)
	, m_bShowSwpPlane(FALSE)
	, m_SplintOffset(0.1)
{
	m_pUpperJaw = NULL;
	m_pLowerJaw = NULL;
	m_pAbutment = NULL;
	m_pSplintInner = NULL;
	m_pSplintOuter = NULL;
	m_pMovingPlanes = NULL;
}

GDentalCadDlg::~GDentalCadDlg()
{
}

void GDentalCadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPLINT_WIDTH, m_cSplintWidth);
	DDX_Slider(pDX, IDC_SPLINT_WIDTH, m_SplintWidth);
	DDX_Control(pDX, IDC_SPLINT_WIDTH_BY_NORMAL, m_cSplintWidthByNormal);
	DDX_Slider(pDX, IDC_SPLINT_WIDTH_BY_NORMAL, m_SplintWidthByNormal);
	DDX_Check(pDX, IDC_SHOW_UPPER_JAW, m_bShowUpperJaw);
	DDX_Check(pDX, IDC_SHOW_LOWER_JAW, m_bShowLowerJaw);
	DDX_Check(pDX, IDC_SHOW_KEY_PLANE, m_bShowKeyPlane);
	DDX_Check(pDX, IDC_SHOW_SWP_PLANE, m_bShowSwpPlane);
	DDX_Text(pDX, IDC_SPLINT_OFFSET, m_SplintOffset);
}


BEGIN_MESSAGE_MAP(GDentalCadDlg, CDialog)
	ON_BN_CLICKED(IDC_IMPORT_UPPER_JAW, &GDentalCadDlg::OnBnClickedImportUpperJaw)
	ON_BN_CLICKED(IDC_IMPORT_LOWER_JAW, &GDentalCadDlg::OnBnClickedImportLowerJaw)
	ON_BN_CLICKED(IDC_IMPORT_ABUTMENT, &GDentalCadDlg::OnBnClickedImportAbutment)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_SHOW_UPPER_JAW, &GDentalCadDlg::OnBnClickedShowUpperJaw)
	ON_BN_CLICKED(IDC_SHOW_LOWER_JAW, &GDentalCadDlg::OnBnClickedShowLowerJaw)
	ON_BN_CLICKED(IDC_SHOW_KEY_PLANE, &GDentalCadDlg::OnBnClickedShowKeyPlane)
	ON_BN_CLICKED(IDC_SHOW_SWP_PLANE, &GDentalCadDlg::OnBnClickedShowSwpPlane)
	ON_BN_CLICKED(IDC_CREATE_SPLINT, &GDentalCadDlg::OnBnClickedCreateSplint)
	ON_BN_CLICKED(IDC_CREATE_SWP_PLANE, &GDentalCadDlg::OnBnClickedCreateSwpPlane)
	ON_EN_KILLFOCUS(IDC_SPLINT_OFFSET, &GDentalCadDlg::OnEnKillfocusSplintOffset)
END_MESSAGE_MAP()


// GDentalCadDlg 메시지 처리기입니다.

BOOL GDentalCadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_cSplintWidth.SetRange(0, 100);
	m_cSplintWidth.SetPos(0);

	m_cSplintWidthByNormal.SetRange(0, 100);
	m_cSplintWidthByNormal.SetPos(0);

	m_bShowUpperJaw = FALSE;
	m_bShowLowerJaw = FALSE;
	m_bShowKeyPlane = FALSE;
	m_bShowSwpPlane = FALSE;

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void GDentalCadDlg::OnBnClickedImportUpperJaw()
{
	// 파일의 이름을 포함한 경로와 파일 이름 변수를 선언하고,
	CString FilePath, FileName;

	// 파일의 확장자 필터와 인덱스를 정의하고,
	LPCTSTR filter = _T("OBJ file(*.obj)|*.obj||");
	int idxFilter = 0;

	// 파일 열기 대화상자를 출력하고,
	CFileDialog FileDlg(true, NULL, NULL, OFN_HIDEREADONLY, filter);
	if(FileDlg.DoModal() != IDOK) 
		return;

	// 사용자가 선택한 파일의 경로명, 파일 이름 및 필터 인덱스를 구하고,
	FilePath = FileDlg.GetPathName(); // 파일 경로 및 이름까지 포함
	FileName = FileDlg.GetFileName();	// 파일 이름만 포함
	idxFilter = FileDlg.m_ofn.nFilterIndex; 

	// 현재 파일의 경로를 얻어와서, 작업 디렉토리로 설정한다.
	std::string file_path = std::string((CStringA)(FilePath));
	file_path = get_file_path(file_path);
	_chdir(file_path.c_str());

	AfxGetApp()->BeginWaitCursor();
	switch (idxFilter)
	{
	case 1: // OBJ 파일 읽기
		::load_mesh_from_obj_file(&theScene(), (CStringA)FilePath);
		break;
	}
	AfxGetApp()->EndWaitCursor();

	m_pLowerJaw = NULL;
	m_pUpperJaw = theScene().pRecentMesh;
	m_pSplintInner = NULL;
	m_pSplintOuter = NULL;
	m_pSplintSide0 = NULL;
	m_pSplintSide1 = NULL;
	m_pSplintSide2 = NULL;
	m_pSplintSide3 = NULL;
	m_KeyPlaneList.clear();
	m_pMovingPlanes = NULL;
	m_bShowLowerJaw = FALSE;
	m_bShowUpperJaw = TRUE;
	m_bShowKeyPlane = FALSE;
	m_bShowSwpPlane = FALSE;

	UpdateData(FALSE);
	theMainFrm().UpdateWorkSpaceDlg();
	theDoc().UpdateAllViews(NULL);
}

void GDentalCadDlg::OnBnClickedImportLowerJaw()
{
	// 파일의 이름을 포함한 경로와 파일 이름 변수를 선언하고,
	CString FilePath, FileName;

	// 파일의 확장자 필터와 인덱스를 정의하고,
	LPCTSTR filter = _T("OBJ file(*.obj)|*.obj||");
	int idxFilter = 0;

	// 파일 열기 대화상자를 출력하고,
	CFileDialog FileDlg(true, NULL, NULL, OFN_HIDEREADONLY, filter);
	if(FileDlg.DoModal() != IDOK) 
		return;

	// 사용자가 선택한 파일의 경로명, 파일 이름 및 필터 인덱스를 구하고,
	FilePath = FileDlg.GetPathName(); // 파일 경로 및 이름까지 포함
	FileName = FileDlg.GetFileName();	// 파일 이름만 포함
	idxFilter = FileDlg.m_ofn.nFilterIndex; 

	// 현재 파일의 경로를 얻어와서, 작업 디렉토리로 설정한다.
	std::string file_path = std::string((CStringA)(FilePath));
	file_path = get_file_path(file_path);
	_chdir(file_path.c_str());

	AfxGetApp()->BeginWaitCursor();
	switch (idxFilter)
	{
	case 1: // OBJ 파일 읽기
		::load_mesh_from_obj_file(&theScene(), (CStringA)(FilePath));
		break;
	}
	AfxGetApp()->EndWaitCursor();

	m_pLowerJaw = theScene().pRecentMesh;
	m_pUpperJaw = NULL;
	m_pSplintInner = NULL;
	m_pSplintOuter = NULL;
	m_pSplintSide0 = NULL;
	m_pSplintSide1 = NULL;
	m_pSplintSide2 = NULL;
	m_pSplintSide3 = NULL;
	m_KeyPlaneList.clear();
	m_pMovingPlanes = NULL;
	m_bShowLowerJaw = TRUE;
	m_bShowUpperJaw = FALSE;
	m_bShowKeyPlane = FALSE;
	m_bShowSwpPlane = FALSE;

	UpdateData(FALSE);
	theMainFrm().UpdateWorkSpaceDlg();
	theDoc().UpdateAllViews(NULL);
}

void GDentalCadDlg::OnBnClickedImportAbutment()
{
	// 파일의 이름을 포함한 경로와 파일 이름 변수를 선언하고,
	CString FilePath, FileName;

	// 파일의 확장자 필터와 인덱스를 정의하고,
	LPCTSTR filter = _T("OBJ file(*.obj)|*.obj||");
	int idxFilter = 0;

	// 파일 열기 대화상자를 출력하고,
	CFileDialog FileDlg(true, NULL, NULL, OFN_HIDEREADONLY, filter);
	if(FileDlg.DoModal() != IDOK) 
		return;

	// 사용자가 선택한 파일의 경로명, 파일 이름 및 필터 인덱스를 구하고,
	FilePath = FileDlg.GetPathName(); // 파일 경로 및 이름까지 포함
	FileName = FileDlg.GetFileName();	// 파일 이름만 포함
	idxFilter = FileDlg.m_ofn.nFilterIndex; 

	// 현재 파일의 경로를 얻어와서, 작업 디렉토리로 설정한다.
	std::string file_path = std::string((CStringA)(FilePath));
	file_path = get_file_path(file_path);
	_chdir(file_path.c_str());

	AfxGetApp()->BeginWaitCursor();
	switch (idxFilter)
	{
	case 1: // OBJ 파일 읽기
		::load_mesh_from_obj_file(&theScene(), (CStringA)(FilePath));
		break;
	}
	AfxGetApp()->EndWaitCursor();

	m_pAbutment = theScene().pRecentMesh;
	theMainFrm().UpdateWorkSpaceDlg();
	theDoc().UpdateAllViews(NULL);
}


void GDentalCadDlg::OnBnClickedCreateSwpPlane()
{
	// 특징점이 선택되지 않았다면 리턴한다.
	int NumVert = theEditor().GetNumEditObj();
	if (theEditor().GetClsNameEditObj() != "GVertex" || NumVert < 4)
	{
		AfxMessageBox("최소 4개 이상의 특징점들을 선택하세요 (SHIFT + LEFT MOUSE BUTTON).");
		return;
	}

	// 특징점의 참조 메쉬와 경계 상자의 정보를 구하고
	GMesh *pMesh = theEditor().GetEditMesh();
	double z_max = pMesh->BBox.MaxPt[2];
	double z_min = pMesh->BBox.MinPt[2];
	double len = ABS(z_max - z_min) * 0.5;

	// 키 절단면을 생성한다.
	m_KeyPlaneList.clear();
	for (int i = 0; i < NumVert; i += 2)
	{
		// 각 특징점 쌍 (p, q)에 대하여,
		GPoint3 p = ((GVertex *)(theEditor().EditObjList[i]))->P;
		GPoint3 q = ((GVertex *)(theEditor().EditObjList[i + 1]))->P;

		// 키 절단면의 중심의 좌표를 구하고,
		GVector3 cnt;
		cnt[0] = (p[0] + q[0]) / 2.0;
		cnt[1] = (p[1] + q[1]) / 2.0;
		cnt[2] = (z_min + z_max) / 2.0;

		// 키 절단면의 방향을 구한다.
		GVector3 x_axis(q[0] - p[0], q[1] - p[1], 0.0);
		x_axis.Normalize();
		GVector3 y_axis(-x_axis[1], x_axis[0], 0.0);	// 90도 회전.
		GQuater qt(x_axis, y_axis);

		// 키 절단면의 코너점을 구한다.
		GPoint3 pt[4];
		pt[0].Set(len, 0.0, -len);
		pt[1].Set(-len, 0.0, -len);
		pt[2].Set(-len, 0.0, len);
		pt[3].Set(len, 0.0, len);

		// 키 절단면을 생성하여 장면에 추가한다.
		std::string Name = create_mesh_name("Cut_Plane");
		GMesh *pPlane = new GMesh(Name, GTransf(cnt, qt), FACE_QUAD, pMesh->Name);
		pPlane->AddVertex(GVertex(pt[0]));
		pPlane->AddVertex(GVertex(pt[1]));
		pPlane->AddVertex(GVertex(pt[2]));
		pPlane->AddVertex(GVertex(pt[3]));
		pPlane->AddFace(GFace(0, 1, 2, 0, 1, 2, -1));
		pPlane->AddFace(GFace(0, 2, 3, 0, 2, 3, -1));
		pPlane->Renderer.bBackFaceCull = false;
		theScene().AddMesh(pPlane);

		// 키 절단면을 메쉬 리스트에 추가한다.
		m_KeyPlaneList.push_back(pPlane);
	}

	// 추가할 모션의 이름을 생성하고,
	std::string Name = create_mesh_name("Mot");
	int deg = MIN(3, (m_KeyPlaneList.size() - 1));
	m_pMovingPlanes = new GNurbsMot(Name, GTransf(), m_KeyPlaneList, deg, 1, "");
	m_pMovingPlanes->Renderer.SwpPlnWidth = len * 2.0;
	m_pMovingPlanes->Renderer.SwpPlnHeight = len * 2.0;
	m_pMovingPlanes->Renderer.bBackFaceCull = false;
	m_pMovingPlanes->Renderer.RenderType |= RENDER_WIRE;
	theScene().AddMesh(m_pMovingPlanes);

	m_bShowKeyPlane = TRUE;
	m_bShowSwpPlane = TRUE;
	UpdateData(FALSE);

	theMainFrm().UpdateWorkSpaceDlg();
	theDoc().UpdateAllViews(NULL);
}

void GDentalCadDlg::OnBnClickedCreateSplint()
{
	// 선택된 메쉬가 없다면 리턴한다.
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL || theEditor().GetClsNameEditObj() != "GMesh")
	{
		AfxMessageBox("임플란트 가이드를 생성할 상악 또는 하악을 선택하세요.");
		return;
	}

	// 절단면이 생성되지 않았다면 리턴한다.
	if (m_pMovingPlanes == NULL)
	{
		AfxMessageBox("임플란트 가이드 스윕평면을 생성하세요.");
		return;
	}

	// 상악에 대한 가이드를 생성한다.
	if (pMesh == m_pUpperJaw)
		CreateUpperSplint(pMesh);
	
	// 하악에 대한 가이드를 생성한다.
	if (pMesh == m_pLowerJaw)
		CreateLowerSplint(pMesh);

	theMainFrm().UpdateWorkSpaceDlg();
	theDoc().UpdateAllViews(NULL);
}

void GDentalCadDlg::GroupFacesByPlane(GMesh *pMesh, GNurbsMot *pSwpPlanes, int NumGrp, std::vector<std::vector<int>> &FaceIdxList)
{
	// 절단면의 개수와 영역의 개수(절단면 -1)를 정의한다.
	int NumPln = NumGrp + 1;
	std::vector<int> dummy;
	FaceIdxList.assign(NumGrp, dummy);

	// 시작 절단면을 구하고,
	GPlane PrevPln = pSwpPlanes->EvalPlane(0.0);
	GPlane CurrPln = PrevPln;
	for (int i = 1; i < NumPln; ++i)
	{
		// 현재 절단면을 구하여,
		double t = (double)i / (double)(NumPln - 1);
		CurrPln = pSwpPlanes->EvalPlane(t);

		// 모든 삼각형에 대하여
		for (int j = 0; j < pMesh->GetNumFace(); ++j)
		{
			// 삼각형의 세 정점 중
			int *vidx = pMesh->Faces[j].vIdx;
			GPoint3 p1 = pMesh->Verts[vidx[0]].P;
			GPoint3 p2 = pMesh->Verts[vidx[1]].P;
			GPoint3 p3 = pMesh->Verts[vidx[2]].P;

			// 하나라도 이전 절단면과 현재 절단면 사이에 포함된다면 삼각형의 인덱스를 해당 영역에 포함시킨다.
			if (PrevPln.IsAbovePlane(p1) && CurrPln.IsBelowPlane(p1))
				FaceIdxList[i - 1].push_back(j);
			else if (PrevPln.IsAbovePlane(p2) && CurrPln.IsBelowPlane(p2))
				FaceIdxList[i - 1].push_back(j);
			else if (PrevPln.IsAbovePlane(p3) && CurrPln.IsBelowPlane(p3))
				FaceIdxList[i - 1].push_back(j);
		}
		PrevPln = CurrPln;
	}
}

void GDentalCadDlg::CreateUpperSplint(GMesh *pMesh)
{
	// 평면에 의해 구분된 삼각형의 인덱스를 구한다.
	int NumGrp = 20;
	std::vector<std::vector<int>> FaceIdxList;
	GroupFacesByPlane(pMesh, m_pMovingPlanes, NumGrp, FaceIdxList);

	// 내부/외부 메쉬를 생성한다.
	m_pSplintInner = new GMesh(create_mesh_name("Inner_Splint"), GTransf(), FACE_QUAD, pMesh->Name);
	m_pSplintOuter = new GMesh(create_mesh_name("Outer_Splint"), GTransf(), FACE_QUAD, pMesh->Name);

	// 샘플링 수와 샘플링 결과를 저장할 변수를 정의한다.
	int NumKey = (int)m_KeyPlaneList.size();
	int snum_u = 50;			// theta 방향
	int snum_v = NumKey * 30;	// t 방향
		
	// 샘플링을 위한 시작 각도 (-90도)와 끝 각도(90도)를 설정하고,
	double theta0 = -90.0 * M_PI / 180.0;
	double theta1 = 90.0 * M_PI / 180.0;

	// Offset 샘플링을 수행한다.
	for (int i = 0; i < snum_v; ++i)
	{
		// V 방향 샘플링 파라미터를 구하고,
		double v = i / (double)(snum_v - 1);
		GTransf MC = m_pMovingPlanes->EvalFrame(v);
		int idx = (i == snum_v - 1) ? (NumGrp - 1) : ((int)(v * NumGrp));
		std::vector<GVector4> Pts;
		
		for (int j = 0; j < snum_u; ++j)
		{
			// U 방향 샘플링 파라미터와 광선의 식을 구하고,
			double u = j / (double)(snum_u - 1);
			double theta = theta0 + (theta1 - theta0) * u;
			GVector3 dir(sin(theta), 0.0, cos(theta));			
			GLine ray = MC * GLine(GPoint3(), dir);

			// 광선과 교차하는 삼각형까지의 offset을 구한다.
			bool flag = false;
			double offset = 10000000.0;
			
			for (unsigned k = 0; k < FaceIdxList[idx].size(); ++k)
			{
				int fidx = FaceIdxList[idx][k];
				int *vidx = pMesh->Faces[fidx].vIdx;
				GPoint3 p1 = pMesh->Verts[vidx[0]].P;
				GPoint3 p2 = pMesh->Verts[vidx[1]].P;
				GPoint3 p3 = pMesh->Verts[vidx[2]].P;

				GPoint3 q;
				double t, alpha, beta, gamma;
				bool ret = ::intersect_line_triangle(p1, p2, p3, ray, t, q, alpha, beta, gamma, false);
				if (ret)
				{
					if (t > 0.0 && t < offset)
					{
						flag = true;
						offset = t;
					}
				}
			}

			// 광선의 교차점이 구해졌다면 샘플링 데이터에 추가한다.
			if (flag)
				Pts.push_back(GVector4(offset * sin(theta), 0.0, offset * cos(theta), 1.0));
		}

		// 교차점의 수가 너무 적다면 리턴한다.
		if (Pts.size() < 10)
		{
			AfxMessageBox("키 단면의 위치를 조정하세요");
			delete m_pSplintInner;
			delete m_pSplintOuter;
			return;
		}

		// 내부 메쉬의 단면 곡선과, 오프셋 방향을 계산하기 위한 곡선을 구한다.
		GNurbsCrv *pCrv0 = ::get_gnurbs_crv_inter(2, Pts.size() - 1, &Pts[0], 0.0, 1.0, PARAM_CHORD_LEN, KNOT_AVERAGING);
		GNurbsCrv *pCrv1 = ::get_gnurbs_crv_approx(2, 3, &Pts[0], Pts.size() - 1, PARAM_CHORD_LEN, KNOT_APPROX);
		pCrv1->P[0] = Pts[0];
		pCrv1->P[pCrv1->n] = Pts[Pts.size() - 1];

		// 내부/외부 메쉬의 정점리스트를 생성한다.
		for (int j = 0; j < snum_u; ++j)
		{
			// U 방향 샘플링 파라미터와 광선의 식을 구하고,
			double u = j / (double)(snum_u - 1);
			GPoint3 pt = cast_pt3(pCrv0->Eval(u));
			GVector3 Cu =cast_vec3(pCrv1->EvalDeriv(u, 1));
			Cu.Normalize();
			
			GVector3 offset_dir(-Cu[2], 0.0, Cu[0]);
			GPoint3 p0 = MC * pt;
			GPoint3 p1 = MC * (pt + m_SplintOffset * offset_dir);

			m_pSplintInner->AddVertex(GVertex(p0, GVector3()));
			m_pSplintOuter->AddVertex(GVertex(p1, GVector3()));
		}

		delete pCrv0;
		delete pCrv1;
	}

	// 삼각형 리스트를 생성한다.
	for (int i = 0; i < snum_v - 1; ++i)
	{
		for(int j = 0; j < snum_u - 1; ++j)
		{
			int vidx0 = i * snum_u + j;
			int vidx1 = vidx0 + 1;
			int vidx2 = vidx1 + snum_u;
			int vidx3 = vidx0 + snum_u;

			GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
			GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);
			m_pSplintInner->AddFace(f0);
			m_pSplintInner->AddFace(f1);
			m_pSplintOuter->AddFace(f0);
			m_pSplintOuter->AddFace(f1);
		}
	}

	// 내부 메쉬를 장면에 추가한다.
	GMaterial mtl = get_material_random();
	m_pSplintInner->Renderer.bBackFaceCull = false;
	m_pSplintInner->AddMtl(mtl.Name);
	m_pSplintInner->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintInner);

	// 외부 메쉬를 스무딩하여 장면에 추가한다.
	for (int i = 0; i < 5; ++i)
		edit_mesh_by_smoothing(m_pSplintOuter);
	m_pSplintOuter->Renderer.bBackFaceCull = false;
	mtl = get_material_random();
	m_pSplintOuter->AddMtl(mtl);
	m_pSplintOuter->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintOuter);
 
 	// 사이드 스플린트0, 1 메쉬를 생성하고,
 	m_pSplintSide0 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);
 	m_pSplintSide1 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);
 
 	// 정점의 리스트를 생성하고,
 	for (int i = 0; i < snum_u; ++i)
 	{
 		GVertex v0 = m_pSplintInner->Verts[i];
 		GVertex v1 = m_pSplintOuter->Verts[i];
 		m_pSplintSide0->AddVertex(v0);
		m_pSplintSide0->AddVertex(v1);

		v0 = m_pSplintOuter->Verts[snum_u * (snum_v - 1) + i];
		v1 = m_pSplintInner->Verts[snum_u * (snum_v - 1) + i];
		m_pSplintSide1->AddVertex(v0);
		m_pSplintSide1->AddVertex(v1);
	}

	// 삼각형 리스트를 생성한다.
	for (int i = 0; i < snum_u - 1; ++i)
	{
		int vidx0 = i * 2;
		int vidx1 = vidx0 + 2;
		int vidx2 = vidx1 + 1;
		int vidx3 = vidx0 + 1;
		GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
		GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);

		m_pSplintSide0->AddFace(f0);
		m_pSplintSide0->AddFace(f1);

		m_pSplintSide1->AddFace(f0);
		m_pSplintSide1->AddFace(f1);
	}

	// 사이드 메쉬를 장면에 추가한다.
	m_pSplintSide0->Renderer.bBackFaceCull = true;
	//m_pSplintSide0->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide0->AddMtl(mtl);
	m_pSplintSide0->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide0);

	m_pSplintSide1->Renderer.bBackFaceCull = true;
	//m_pSplintSide1->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide1->AddMtl(mtl);
	m_pSplintSide1->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide1);
 
 	// 사이드 스플린트2, 3메쉬를 생성하고,
	m_pSplintSide2 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);
	m_pSplintSide3 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);

	// 정점을 리스트를 생성하고,
	for (int i = 0; i < snum_v; ++i)
	{
		GVertex v0 = m_pSplintOuter->Verts[i * snum_u];
		GVertex v1 = m_pSplintInner->Verts[i * snum_u];
		m_pSplintSide2->AddVertex(v0);
		m_pSplintSide2->AddVertex(v1);

		v0 = m_pSplintInner->Verts[(i + 1) * snum_u - 1];
		v1 = m_pSplintOuter->Verts[(i + 1) * snum_u - 1];
		m_pSplintSide3->AddVertex(v0);
		m_pSplintSide3->AddVertex(v1);
	}

	// 삼각형 리스트를 생성하여,
	for (int i = 0; i < snum_v - 1; ++i)
	{
		int vidx0 = i * 2;
		int vidx1 = vidx0 + 2;
		int vidx2 = vidx1 + 1;
		int vidx3 = vidx0 + 1;
		GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
		GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);

		m_pSplintSide2->AddFace(f0);
		m_pSplintSide2->AddFace(f1);
		m_pSplintSide3->AddFace(f0);
		m_pSplintSide3->AddFace(f1);
	}

	// 장면에 추가한다.
	m_pSplintSide2->Renderer.bBackFaceCull = true;
	//m_pSplintSide2->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide2->AddMtl(mtl);
	m_pSplintSide2->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide2);

	m_pSplintSide3->Renderer.bBackFaceCull = true;
	//m_pSplintSide3->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide3->AddMtl(mtl);
	m_pSplintSide3->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide3);
}

void GDentalCadDlg::CreateLowerSplint(GMesh *pMesh)
{
	// 평면에 의해 구분된 삼각형의 인덱스를 구한다.
	int NumGrp = 20;
	std::vector<std::vector<int>> FaceIdxList;
	GroupFacesByPlane(pMesh, m_pMovingPlanes, NumGrp, FaceIdxList);

	// 내부/외부 메쉬를 생성한다.
	m_pSplintInner = new GMesh(create_mesh_name("Inner_Splint"), GTransf(), FACE_QUAD, pMesh->Name);
	m_pSplintOuter = new GMesh(create_mesh_name("Outer_Splint"), GTransf(), FACE_QUAD, pMesh->Name);

	// 샘플링 수와 샘플링 결과를 저장할 변수를 정의한다.
	int NumKey = (int)m_KeyPlaneList.size();
	int snum_u = 50;			// theta 방향
	int snum_v = NumKey * 30;	// t 방향

	// 샘플링을 위한 시작 각도 (90도)와 끝 각도(270도)를 설정하고,
	double theta0 = 90.0 * M_PI / 180.0;
	double theta1 = 270.0 * M_PI / 180.0;

	// Offset 샘플링을 수행한다.
	for (int i = 0; i < snum_v; ++i)
	{
		// V 방향 샘플링 파라미터를 구하고,
		double v = i / (double)(snum_v - 1);
		GTransf MC = m_pMovingPlanes->EvalFrame(v);
		int idx = (i == snum_v - 1) ? (NumGrp - 1) : ((int)(v * NumGrp));
		std::vector<GVector4> Pts;

		for (int j = 0; j < snum_u; ++j)
		{
			// U 방향 샘플링 파라미터와 광선의 식을 구하고,
			double u = j / (double)(snum_u - 1);
			double theta = theta0 + (theta1 - theta0) * u;
			GVector3 dir(sin(theta), 0.0, cos(theta));			
			GLine ray = MC * GLine(GPoint3(), dir);

			// 광선과 교차하는 삼각형까지의 offset을 구한다.
			bool flag = false;
			double offset = 10000000.0;

			for (unsigned k = 0; k < FaceIdxList[idx].size(); ++k)
			{
				int fidx = FaceIdxList[idx][k];
				int *vidx = pMesh->Faces[fidx].vIdx;
				GPoint3 p1 = pMesh->Verts[vidx[0]].P;
				GPoint3 p2 = pMesh->Verts[vidx[1]].P;
				GPoint3 p3 = pMesh->Verts[vidx[2]].P;

				GPoint3 q;
				double t, alpha, beta, gamma;
				bool ret = ::intersect_line_triangle(p1, p2, p3, ray, t, q, alpha, beta, gamma, false);
				if (ret)
				{
					if (t > 0.0 && t < offset)
					{
						flag = true;
						offset = t;
					}
				}
			}

			// 광선의 교차점이 구해졌다면 샘플링 데이터에 추가한다.
			if (flag)
				Pts.push_back(GVector4(offset * sin(theta), 0.0, offset * cos(theta), 1.0));
		}
		
		// 교차점의 수가 너무 적다면 리턴한다.
		if (Pts.size() < 10)
		{
			AfxMessageBox("키 단면의 위치를 조정하세요");
			delete m_pSplintInner;
			delete m_pSplintOuter;
			return;
		}
		
		// 내부 메쉬의 단면 곡선과, 오프셋 방향을 계산하기 위한 곡선을 구한다.
		GNurbsCrv *pCrv0 = ::get_gnurbs_crv_inter(2, Pts.size() - 1, &Pts[0], 0.0, 1.0, PARAM_CHORD_LEN, KNOT_AVERAGING);
		GNurbsCrv *pCrv1 = ::get_gnurbs_crv_approx(2, 3, &Pts[0], Pts.size() - 1, PARAM_CHORD_LEN, KNOT_APPROX);
		pCrv1->P[0] = Pts[0];
		pCrv1->P[pCrv1->n] = Pts[Pts.size() - 1];

		// 내부/외부 메쉬의 정점리스트를 생성한다.
		for (int j = 0; j < snum_u; ++j)
		{
			// U 방향 샘플링 파라미터와 광선의 식을 구하고,
			double u = j / (double)(snum_u - 1);
			GPoint3 pt = cast_pt3(pCrv0->Eval(u));
			GVector3 Cu =cast_vec3(pCrv1->EvalDeriv(u, 1));
			Cu.Normalize();

			GVector3 offset_dir(-Cu[2], 0.0, Cu[0]);
			GPoint3 p0 = MC * pt;
			GPoint3 p1 = MC * (pt + m_SplintOffset * offset_dir);

			m_pSplintInner->AddVertex(GVertex(p0, GVector3()));
			m_pSplintOuter->AddVertex(GVertex(p1, GVector3()));
		}

		delete pCrv0;
		delete pCrv1;
	}

	// 삼각형 리스트를 생성한다.
	for (int i = 0; i < snum_v - 1; ++i)
	{
		for(int j = 0; j < snum_u - 1; ++j)
		{
			int vidx0 = i * snum_u + j;
			int vidx1 = vidx0 + 1;
			int vidx2 = vidx1 + snum_u;
			int vidx3 = vidx0 + snum_u;

			GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
			GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);
			m_pSplintInner->AddFace(f0);
			m_pSplintInner->AddFace(f1);
			m_pSplintOuter->AddFace(f0);
			m_pSplintOuter->AddFace(f1);
		}
	}

	// 내부 메쉬를 장면에 추가한다.
	GMaterial mtl = get_material_random();
	m_pSplintInner->Renderer.bBackFaceCull = false;
	m_pSplintInner->AddMtl(mtl.Name);
	m_pSplintInner->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintInner);

	// 외부 메쉬를 스무딩하여 장면에 추가한다.
	for (int i = 0; i < 5; ++i)
		edit_mesh_by_smoothing(m_pSplintOuter);
	m_pSplintOuter->Renderer.bBackFaceCull = false;
	mtl = get_material_random();
	m_pSplintOuter->AddMtl(mtl);
	m_pSplintOuter->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintOuter);

	// 사이드 스플린트0, 1 메쉬를 생성하고,
	m_pSplintSide0 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);
	m_pSplintSide1 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);

	// 정점의 리스트를 생성하고,
	for (int i = 0; i < snum_u; ++i)
	{
		GVertex v0 = m_pSplintInner->Verts[i];
		GVertex v1 = m_pSplintOuter->Verts[i];
		m_pSplintSide0->AddVertex(v0);
		m_pSplintSide0->AddVertex(v1);

		v0 = m_pSplintOuter->Verts[snum_u * (snum_v - 1) + i];
		v1 = m_pSplintInner->Verts[snum_u * (snum_v - 1) + i];
		m_pSplintSide1->AddVertex(v0);
		m_pSplintSide1->AddVertex(v1);
	}

	// 삼각형 리스트를 생성한다.
	for (int i = 0; i < snum_u - 1; ++i)
	{
		int vidx0 = i * 2;
		int vidx1 = vidx0 + 2;
		int vidx2 = vidx1 + 1;
		int vidx3 = vidx0 + 1;
		GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
		GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);

		m_pSplintSide0->AddFace(f0);
		m_pSplintSide0->AddFace(f1);

		m_pSplintSide1->AddFace(f0);
		m_pSplintSide1->AddFace(f1);
	}

	// 사이드 메쉬를 장면에 추가한다.
	m_pSplintSide0->Renderer.bBackFaceCull = true;
	//m_pSplintSide0->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide0->AddMtl(mtl);
	m_pSplintSide0->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide0);

	m_pSplintSide1->Renderer.bBackFaceCull = true;
	//m_pSplintSide1->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide1->AddMtl(mtl);
	m_pSplintSide1->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide1);

	// 사이드 스플린트2, 3메쉬를 생성하고,
	m_pSplintSide2 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);
	m_pSplintSide3 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);

	// 정점을 리스트를 생성하고,
	for (int i = 0; i < snum_v; ++i)
	{
		GVertex v0 = m_pSplintOuter->Verts[i * snum_u];
		GVertex v1 = m_pSplintInner->Verts[i * snum_u];
		m_pSplintSide2->AddVertex(v0);
		m_pSplintSide2->AddVertex(v1);

		v0 = m_pSplintInner->Verts[(i + 1) * snum_u - 1];
		v1 = m_pSplintOuter->Verts[(i + 1) * snum_u - 1];
		m_pSplintSide3->AddVertex(v0);
		m_pSplintSide3->AddVertex(v1);
	}

	// 삼각형 리스트를 생성하여,
	for (int i = 0; i < snum_v - 1; ++i)
	{
		int vidx0 = i * 2;
		int vidx1 = vidx0 + 2;
		int vidx2 = vidx1 + 1;
		int vidx3 = vidx0 + 1;
		GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
		GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);

		m_pSplintSide2->AddFace(f0);
		m_pSplintSide2->AddFace(f1);
		m_pSplintSide3->AddFace(f0);
		m_pSplintSide3->AddFace(f1);
	}

	// 장면에 추가한다.
	m_pSplintSide2->Renderer.bBackFaceCull = true;
	//m_pSplintSide2->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide2->AddMtl(mtl);
	m_pSplintSide2->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide2);

	m_pSplintSide3->Renderer.bBackFaceCull = true;
	//m_pSplintSide3->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide3->AddMtl(mtl);
	m_pSplintSide3->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide3);
}

/*
void GDentalCadDlg::CreateLowerSplint(GMesh *pMesh)
{
	// 절단면으로 구분된 삼각형의 인덱스를 저장하기 위한 변수를 정의한다.
	std::vector<std::vector<int>> FaceIdxList;
	std::vector<int> dummy;

	// 절단면의 개수와 영역의 개수(절단면 -1)를 정의한다.
	int NumPln = 20;
	FaceIdxList.assign(NumPln - 1, dummy);

	// 시작 절단면을 구하고,
	GPlane PrevPln = m_pMovingPlanes->EvalPlane(0.0);
	GPlane CurrPln = PrevPln;
	for (int i = 1; i < NumPln; ++i)
	{
		// 현재 절단면을 구하여,
		double t = (double)i / (double)(NumPln - 1);
		CurrPln = m_pMovingPlanes->EvalPlane(t);

		// 모든 삼각형에 대하여
		int NumFace = pMesh->GetNumFace();
		for (int j = 0; j < NumFace; ++j)
		{
			// 삼각형의 세 정점 중
			int *vidx = pMesh->Faces[j].vIdx;
			GPoint3 p1 = pMesh->Verts[vidx[0]].P;
			GPoint3 p2 = pMesh->Verts[vidx[1]].P;
			GPoint3 p3 = pMesh->Verts[vidx[2]].P;

			// 하나라도 이전 절단면과 현재 절단면 사이에 포함된다면 삼각형의 인덱스를 해당 영역에 포함시킨다.
			if (PrevPln.IsAbovePlane(p1) && CurrPln.IsBelowPlane(p1))
				FaceIdxList[i - 1].push_back(j);
			else if (PrevPln.IsAbovePlane(p2) && CurrPln.IsBelowPlane(p2))
				FaceIdxList[i - 1].push_back(j);
			else if (PrevPln.IsAbovePlane(p3) && CurrPln.IsBelowPlane(p3))
				FaceIdxList[i - 1].push_back(j);
		}
		PrevPln = CurrPln;
	}

	// 키 절단면을 생성하여 장면에 추가한다.
	m_pSplintInner = new GMesh(create_mesh_name("Inner_Splint"), GTransf(), FACE_QUAD, pMesh->Name);
	m_pSplintOuter = new GMesh(create_mesh_name("Outer_Splint"), GTransf(), FACE_QUAD, pMesh->Name);

	// 샘플링 수와 샘플링 결과를 저장할 변수를 정의한다.
	int NumKey = (int)m_KeyPlaneList.size();
	int snum_u = 50;			// theta 방향
	int snum_v = NumKey * 30;	// t 방향
	std::vector<GVector2> Params;
	std::vector<double> Pts;

	// 샘플링을 위한 시작 각도 (90도)와 끝 각도(270도)를 설정하고,
	double theta0 = 90.0 * M_PI / 180.0;
	double theta1 = 270.0 * M_PI / 180.0;

	// Offset 샘플링을 수행한다.
	for (int i = 0; i < snum_u; ++i)
	{
		// U 방향 샘플링 파라미터를 구하고,
		double u = i / (double)(snum_u - 1);
		double theta = theta0 + (theta1 - theta0) * u;
		GVector3 dir(sin(theta), 0.0, cos(theta));

		for (int j = 0; j < snum_v; ++j)
		{
			// V 방향 샘플링 파라미터와 광선의 식을 구하고,
			double v = j / (double)(snum_v - 1);
			GTransf MC = m_pMovingPlanes->EvalFrame(v);
			GLine ray = MC * GLine(GPoint3(), dir);

			// 광선과 교차하는 삼각형까지의 offset을 구한다.
			bool flag = false;
			double offset = 10000000.0;
			int idx = (j == snum_v - 1) ? (NumPln - 2) : ((int)(v * (NumPln - 1)));
			int NumFace = (int)FaceIdxList[idx].size();
			for (int k = 0; k < NumFace; ++k)
			{
				int fidx = FaceIdxList[idx][k];
				int *vidx = pMesh->Faces[fidx].vIdx;
				GPoint3 p1 = pMesh->Verts[vidx[0]].P;
				GPoint3 p2 = pMesh->Verts[vidx[1]].P;
				GPoint3 p3 = pMesh->Verts[vidx[2]].P;

				GPoint3 q;
				double t, alpha, beta, gamma;
				bool ret = ::intersect_line_triangle(p1, p2, p3, ray, t, q, alpha, beta, gamma, false);
				if (ret)
				{
					if (t > 0.0 && t < offset)
					{
						flag = true;
						offset = t;
					}
				}
			}

			// 광선의 교차점이 구해졌다면 샘플링 데이터에 추가한다.
			if (flag)
			{
				Params.push_back(GVector2(u, v));
				Pts.push_back(offset);
			}
		}		
	}

	int NumPts = (int)Pts.size();
	double *params = new double [NumPts * 2];
	double *pts = new double [NumPts];

	for (int i = 0; i < NumPts; ++i)
	{
		params[2 * i] = Params[i][0];
		params[2 * i + 1] = Params[i][1];
		pts[i] = Pts[i];
	}
	GUcbsSrf1D *pSrf = get_gucbs_srf_approx(10, 5 * NumKey, NumPts, params, pts, 6);
	GUcbsSrf1D *pSrf1 = get_gucbs_srf_approx(snum_u / 5, NumKey, NumPts, params, pts, 6);
	delete [] params;
	delete [] pts;

	// 정점의 위치를 구한다.
	for (int i = 0; i < snum_u; ++i)
	{
		// U 방향 샘플링 파라미터를 구하고,
		double u = i / (double)(snum_u - 1);
		double theta = theta0 + (theta1 - theta0) * u;

		for (int j = 0; j < snum_v; ++j)
		{
			// V 방향 샘플링 파라미터와 광선의 식을 구하고,
			double v = j / (double)(snum_v - 1);
			GTransf MC = m_pMovingPlanes->EvalFrame(v);

			GVector4 pt = pSrf->Eval(u, v);
			double offset0 = pt[0];
			// 안쪽 교차점과 바깥쪽 교차점을 구하여 스플린트 메쉬에 추가한다.
			GPoint3 p0(offset0 * sin(theta), 0.0, offset0 * cos(theta));

			pt = pSrf1->Eval(u, v);

			double offset1 = pt[0] + m_SplintOffset;
			GPoint3 p1(offset1 * sin(theta), 0.0, offset1 * cos(theta));
			p0 = MC * p0;
			p1 = MC * p1;
			m_pSplintInner->AddVertex(GVertex(p0, GVector3()));
			m_pSplintOuter->AddVertex(GVertex(p1, GVector3()));	
		}
	}

	delete pSrf;
	delete pSrf1;

	// 삼각형 리스트를 생성한다.
	for (int i = 0; i < snum_u - 1; ++i)
	{
		for(int j = 0; j < snum_v - 1; ++j)
		{
			int vidx0 = i * snum_v + j;
			int vidx1 = vidx0 + snum_v;
			int vidx2 = vidx1 + 1;
			int vidx3 = vidx0 + 1;

			GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
			GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);

			m_pSplintInner->AddFace(f0);
			m_pSplintInner->AddFace(f1);
			m_pSplintOuter->AddFace(f0);
			m_pSplintOuter->AddFace(f1);
		}
	}

	// 스플린트의 렌더링 속성을 지정하고 장면에 추가한다.
	GMaterial mtl = get_material_random();
	m_pSplintInner->Renderer.bBackFaceCull = false;
	m_pSplintInner->AddMtl(mtl.Name);
	m_pSplintInner->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintInner);

	m_pSplintOuter->Renderer.bBackFaceCull = false;
	//m_pSplintOuter->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintOuter->AddMtl(mtl);
	m_pSplintOuter->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintOuter);

	// 사이드 스플린트0, 1 메쉬를 생성하고,
	m_pSplintSide0 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);
	m_pSplintSide1 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);

	// 정점을 추가하고,
	for (int i = 0; i < snum_u; ++i)
	{
		GVertex v0 = m_pSplintInner->Verts[i * snum_v];
		GVertex v1 = m_pSplintOuter->Verts[i * snum_v];
		m_pSplintSide0->AddVertex(v0);
		m_pSplintSide0->AddVertex(v1);

		v0 = m_pSplintOuter->Verts[i * snum_v + snum_v - 1];
		v1 = m_pSplintInner->Verts[i * snum_v + snum_v - 1];
		m_pSplintSide1->AddVertex(v0);
		m_pSplintSide1->AddVertex(v1);
	}

	// 삼각형을 추가한다.
	for (int i = 0; i < snum_u - 1; ++i)
	{
		int vidx0 = i * 2;
		int vidx1 = vidx0 + 2;
		int vidx2 = vidx1 + 1;
		int vidx3 = vidx0 + 1;
		GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
		GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);

		m_pSplintSide0->AddFace(f0);
		m_pSplintSide0->AddFace(f1);

		m_pSplintSide1->AddFace(f0);
		m_pSplintSide1->AddFace(f1);
	}

	// 스플린트의 렌더링 속성을 지정하고 장면에 추가한다.
	m_pSplintSide0->Renderer.bBackFaceCull = false;
	//m_pSplintSide0->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide0->AddMtl(mtl);
	m_pSplintSide0->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide0);

	m_pSplintSide1->Renderer.bBackFaceCull = false;
	//m_pSplintSide1->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide1->AddMtl(mtl);
	m_pSplintSide1->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide1);

	// 사이드 스플린트2, 3메쉬를 생성하고,
	m_pSplintSide2 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);
	m_pSplintSide3 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);

	// 정점을 추가하고,
	for (int i = 0; i < snum_v; ++i)
	{
		GVertex v0 = m_pSplintOuter->Verts[i];
		GVertex v1 = m_pSplintInner->Verts[i];
		m_pSplintSide2->AddVertex(v0);
		m_pSplintSide2->AddVertex(v1);

		v0 = m_pSplintInner->Verts[(snum_u - 1) * snum_v + i];
		v1 = m_pSplintOuter->Verts[(snum_u - 1) * snum_v + i];
		m_pSplintSide3->AddVertex(v0);
		m_pSplintSide3->AddVertex(v1);
	}

	// 삼각형을 추가하여,
	for (int i = 0; i < snum_v - 1; ++i)
	{
		int vidx0 = i * 2;
		int vidx1 = vidx0 + 2;
		int vidx2 = vidx1 + 1;
		int vidx3 = vidx0 + 1;
		GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
		GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);

		m_pSplintSide2->AddFace(f0);
		m_pSplintSide2->AddFace(f1);
		m_pSplintSide3->AddFace(f0);
		m_pSplintSide3->AddFace(f1);
	}

	// 렌더링 속성을 지정하여 장면에 추가한다.
	m_pSplintSide2->Renderer.bBackFaceCull = false;
	//m_pSplintSide2->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide2->AddMtl(mtl);
	m_pSplintSide2->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide2);

	m_pSplintSide3->Renderer.bBackFaceCull = false;
	//m_pSplintSide3->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide3->AddMtl(mtl);
	m_pSplintSide3->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide3);
}

void GDentalCadDlg::CreateUpperSplint(GMesh *pMesh)
{
	// 절단면으로 구분된 삼각형의 인덱스를 저장하기 위한 변수를 정의한다.
	std::vector<std::vector<int>> FaceIdxList;
	std::vector<int> dummy;

	// 절단면의 개수와 영역의 개수(절단면 -1)를 정의한다.
	int NumPln = 20;
	FaceIdxList.assign(NumPln - 1, dummy);

	// 시작 절단면을 구하고,
	GPlane PrevPln = m_pMovingPlanes->EvalPlane(0.0);
	GPlane CurrPln = PrevPln;
	for (int i = 1; i < NumPln; ++i)
	{
		// 현재 절단면을 구하여,
		double t = (double)i / (double)(NumPln - 1);
		CurrPln = m_pMovingPlanes->EvalPlane(t);

		// 모든 삼각형에 대하여
		for (int j = 0; j < pMesh->GetNumFace(); ++j)
		{
			// 삼각형의 세 정점 중
			int *vidx = pMesh->Faces[j].vIdx;
			GPoint3 p1 = pMesh->Verts[vidx[0]].P;
			GPoint3 p2 = pMesh->Verts[vidx[1]].P;
			GPoint3 p3 = pMesh->Verts[vidx[2]].P;

			// 하나라도 이전 절단면과 현재 절단면 사이에 포함된다면 삼각형의 인덱스를 해당 영역에 포함시킨다.
			if (PrevPln.IsAbovePlane(p1) && CurrPln.IsBelowPlane(p1))
				FaceIdxList[i - 1].push_back(j);
			else if (PrevPln.IsAbovePlane(p2) && CurrPln.IsBelowPlane(p2))
				FaceIdxList[i - 1].push_back(j);
			else if (PrevPln.IsAbovePlane(p3) && CurrPln.IsBelowPlane(p3))
				FaceIdxList[i - 1].push_back(j);
		}
		PrevPln = CurrPln;
	}

	// 키 절단면을 생성하여 장면에 추가한다.
	m_pSplintInner = new GMesh(create_mesh_name("Inner_Splint"), GTransf(), FACE_QUAD, pMesh->Name);
	m_pSplintOuter = new GMesh(create_mesh_name("Outer_Splint"), GTransf(), FACE_QUAD, pMesh->Name);

	// 샘플링 수와 샘플링 결과를 저장할 변수를 정의한다.
	int NumKey = (int)m_KeyPlaneList.size();
	int snum_u = 50;			// theta 방향
	int snum_v = NumKey * 30;	// t 방향
	std::vector<GVector2> Params;
	std::vector<double> Pts;

	// 샘플링을 위한 시작 각도 (-90도)와 끝 각도(90도)를 설정하고,
	double theta0 = -90.0 * M_PI / 180.0;
	double theta1 = 90.0 * M_PI / 180.0;

	// Offset 샘플링을 수행한다.
	for (int i = 0; i < snum_u; ++i)
	{
		// U 방향 샘플링 파라미터를 구하고,
		double u = i / (double)(snum_u - 1);
		double theta = theta0 + (theta1 - theta0) * u;
		GVector3 dir(sin(theta), 0.0, cos(theta));

		for (int j = 0; j < snum_v; ++j)
		{
			// V 방향 샘플링 파라미터와 광선의 식을 구하고,
			double v = j / (double)(snum_v - 1);
			GTransf MC = m_pMovingPlanes->EvalFrame(v);
			GLine ray = MC * GLine(GPoint3(), dir);

			// 광선과 교차하는 삼각형까지의 offset을 구한다.
			bool flag = false;
			double offset = 10000000.0;
			int idx = (j == snum_v - 1) ? (NumPln - 2) : ((int)(v * (NumPln - 1)));
			int NumFace = (int)FaceIdxList[idx].size();
			for (int k = 0; k < NumFace; ++k)
			{
				int fidx = FaceIdxList[idx][k];
				int *vidx = pMesh->Faces[fidx].vIdx;
				GPoint3 p1 = pMesh->Verts[vidx[0]].P;
				GPoint3 p2 = pMesh->Verts[vidx[1]].P;
				GPoint3 p3 = pMesh->Verts[vidx[2]].P;

				GPoint3 q;
				double t, alpha, beta, gamma;
				bool ret = ::intersect_line_triangle(p1, p2, p3, ray, t, q, alpha, beta, gamma, false);
				if (ret)
				{
					if (t > 0.0 && t < offset)
					{
						flag = true;
						offset = t;
					}
				}
			}

			// 광선의 교차점이 구해졌다면 샘플링 데이터에 추가한다.
			if (flag)
			{
				Params.push_back(GVector2(u, v));
				Pts.push_back(offset);
			}
		}		
	}

	int NumPts = (int)Pts.size();
	double *params = new double [NumPts * 2];
	double *pts = new double [NumPts];

	for (int i = 0; i < NumPts; ++i)
	{
		params[2 * i] = Params[i][0];
		params[2 * i + 1] = Params[i][1];
		pts[i] = Pts[i];
	}
	GUcbsSrf1D *pSrf = get_gucbs_srf_approx(10, 5 * NumKey, NumPts, params, pts, 6);
	GUcbsSrf1D *pSrf1 = get_gucbs_srf_approx(10, 5 * NumKey, NumPts, params, pts, 6);
	delete [] params;
	delete [] pts;

	// 정점의 위치를 구한다.
	for (int i = 0; i < snum_u; ++i)
	{
		// U 방향 샘플링 파라미터를 구하고,
		double u = i / (double)(snum_u - 1);
		double theta = theta0 + (theta1 - theta0) * u;

		for (int j = 0; j < snum_v; ++j)
		{
			// V 방향 샘플링 파라미터와 광선의 식을 구하고,
			double v = j / (double)(snum_v - 1);
			GTransf MC = m_pMovingPlanes->EvalFrame(v);

			GVector4 pt = pSrf->Eval(u, v);
			double offset0 = pt[0];
			// 안쪽 교차점과 바깥쪽 교차점을 구하여 스플린트 메쉬에 추가한다.
			GPoint3 p0(offset0 * sin(theta), 0.0, offset0 * cos(theta));

			pt = pSrf1->Eval(u, v);

			double offset1 = pt[0] + m_SplintOffset;
			GPoint3 p1(offset1 * sin(theta), 0.0, offset1 * cos(theta));
			p0 = MC * p0;
			p1 = MC * p1;
			m_pSplintInner->AddVertex(GVertex(p0, GVector3()));
			m_pSplintOuter->AddVertex(GVertex(p1, GVector3()));	
		}
	}

	delete pSrf;
	delete pSrf1;

	// 삼각형 리스트를 생성한다.
	for (int i = 0; i < snum_u - 1; ++i)
	{
		for(int j = 0; j < snum_v - 1; ++j)
		{
			int vidx0 = i * snum_v + j;
			int vidx1 = vidx0 + snum_v;
			int vidx2 = vidx1 + 1;
			int vidx3 = vidx0 + 1;

			GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
			GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);

			m_pSplintInner->AddFace(f0);
			m_pSplintInner->AddFace(f1);
			m_pSplintOuter->AddFace(f0);
			m_pSplintOuter->AddFace(f1);
		}
	}

	// 스플린트의 렌더링 속성을 지정하고 장면에 추가한다.
	GMaterial mtl = get_material_random();
	m_pSplintInner->Renderer.bBackFaceCull = false;
	m_pSplintInner->AddMtl(mtl.Name);
	m_pSplintInner->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintInner);

	m_pSplintOuter->Renderer.bBackFaceCull = false;
	//m_pSplintOuter->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintOuter->AddMtl(mtl);
	m_pSplintOuter->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintOuter);

	// 사이드 스플린트0, 1 메쉬를 생성하고,
	m_pSplintSide0 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);
	m_pSplintSide1 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);

	// 정점을 추가하고,
	for (int i = 0; i < snum_u; ++i)
	{
		GVertex v0 = m_pSplintInner->Verts[i * snum_v];
		GVertex v1 = m_pSplintOuter->Verts[i * snum_v];
		m_pSplintSide0->AddVertex(v0);
		m_pSplintSide0->AddVertex(v1);

		v0 = m_pSplintOuter->Verts[i * snum_v + snum_v - 1];
		v1 = m_pSplintInner->Verts[i * snum_v + snum_v - 1];
		m_pSplintSide1->AddVertex(v0);
		m_pSplintSide1->AddVertex(v1);
	}

	// 삼각형을 추가한다.
	for (int i = 0; i < snum_u - 1; ++i)
	{
		int vidx0 = i * 2;
		int vidx1 = vidx0 + 2;
		int vidx2 = vidx1 + 1;
		int vidx3 = vidx0 + 1;
		GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
		GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);

		m_pSplintSide0->AddFace(f0);
		m_pSplintSide0->AddFace(f1);

		m_pSplintSide1->AddFace(f0);
		m_pSplintSide1->AddFace(f1);
	}

	// 스플린트의 렌더링 속성을 지정하고 장면에 추가한다.
	m_pSplintSide0->Renderer.bBackFaceCull = false;
	//m_pSplintSide0->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide0->AddMtl(mtl);
	m_pSplintSide0->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide0);

	m_pSplintSide1->Renderer.bBackFaceCull = false;
	//m_pSplintSide1->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide1->AddMtl(mtl);
	m_pSplintSide1->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide1);

	// 사이드 스플린트2, 3메쉬를 생성하고,
	m_pSplintSide2 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);
	m_pSplintSide3 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);

	// 정점을 추가하고,
	for (int i = 0; i < snum_v; ++i)
	{
		GVertex v0 = m_pSplintOuter->Verts[i];
		GVertex v1 = m_pSplintInner->Verts[i];
		m_pSplintSide2->AddVertex(v0);
		m_pSplintSide2->AddVertex(v1);

		v0 = m_pSplintInner->Verts[(snum_u - 1) * snum_v + i];
		v1 = m_pSplintOuter->Verts[(snum_u - 1) * snum_v + i];
		m_pSplintSide3->AddVertex(v0);
		m_pSplintSide3->AddVertex(v1);
	}

	// 삼각형을 추가하여,
	for (int i = 0; i < snum_v - 1; ++i)
	{
		int vidx0 = i * 2;
		int vidx1 = vidx0 + 2;
		int vidx2 = vidx1 + 1;
		int vidx3 = vidx0 + 1;
		GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
		GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);

		m_pSplintSide2->AddFace(f0);
		m_pSplintSide2->AddFace(f1);
		m_pSplintSide3->AddFace(f0);
		m_pSplintSide3->AddFace(f1);
	}

	// 렌더링 속성을 지정하여 장면에 추가한다.
	m_pSplintSide2->Renderer.bBackFaceCull = false;
	//m_pSplintSide2->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide2->AddMtl(mtl);
	m_pSplintSide2->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide2);

	m_pSplintSide3->Renderer.bBackFaceCull = false;
	//m_pSplintSide3->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide3->AddMtl(mtl);
	m_pSplintSide3->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide3);
}

void GDentalCadDlg::CreateLowerSplint(GMesh *pMesh)
{
	// 절단면으로 구분된 삼각형의 인덱스를 저장하기 위한 변수를 정의한다.
	std::vector<std::vector<int>> FaceIdxList;
	std::vector<int> dummy;

	// 절단면의 개수와 영역의 개수(절단면 -1)를 정의한다.
	int NumPln = 20;
	FaceIdxList.assign(NumPln - 1, dummy);

	// 시작 절단면을 구하고,
	GPlane PrevPln = m_pMovingPlanes->EvalPlane(0.0);
	GPlane CurrPln = PrevPln;
	for (int i = 1; i < NumPln; ++i)
	{
		// 현재 절단면을 구하여,
		double t = (double)i / (double)(NumPln - 1);
		CurrPln = m_pMovingPlanes->EvalPlane(t);

		// 모든 삼각형에 대하여
		int NumFace = pMesh->GetNumFace();
		for (int j = 0; j < NumFace; ++j)
		{
			// 삼각형의 세 정점 중
			int *vidx = pMesh->Faces[j].vIdx;
			GPoint3 p1 = pMesh->Verts[vidx[0]].P;
			GPoint3 p2 = pMesh->Verts[vidx[1]].P;
			GPoint3 p3 = pMesh->Verts[vidx[2]].P;

			// 하나라도 이전 절단면과 현재 절단면 사이에 포함된다면 삼각형의 인덱스를 해당 영역에 포함시킨다.
			if (PrevPln.IsAbovePlane(p1) && CurrPln.IsBelowPlane(p1))
				FaceIdxList[i - 1].push_back(j);
			else if (PrevPln.IsAbovePlane(p2) && CurrPln.IsBelowPlane(p2))
				FaceIdxList[i - 1].push_back(j);
			else if (PrevPln.IsAbovePlane(p3) && CurrPln.IsBelowPlane(p3))
				FaceIdxList[i - 1].push_back(j);
		}
		PrevPln = CurrPln;
	}

	// 키 절단면을 생성하여 장면에 추가한다.
	m_pSplintInner = new GMesh(create_mesh_name("Inner_Splint"), GTransf(), FACE_QUAD, pMesh->Name);
	m_pSplintOuter = new GMesh(create_mesh_name("Outer_Splint"), GTransf(), FACE_QUAD, pMesh->Name);

	// 샘플링 수와 샘플링 결과를 저장할 변수를 정의한다.
	int NumKey = (int)m_KeyPlaneList.size();
	int snum_u = 50;			// theta 방향
	int snum_v = NumKey * 30;	// t 방향
	std::vector<GVector2> Params;
	std::vector<double> Pts;

	// 샘플링을 위한 시작 각도 (90도)와 끝 각도(270도)를 설정하고,
	double theta0 = 90.0 * M_PI / 180.0;
	double theta1 = 270.0 * M_PI / 180.0;

	// Offset 샘플링을 수행한다.
	for (int i = 0; i < snum_u; ++i)
	{
		// U 방향 샘플링 파라미터를 구하고,
		double u = i / (double)(snum_u - 1);
		double theta = theta0 + (theta1 - theta0) * u;
		GVector3 dir(sin(theta), 0.0, cos(theta));

		for (int j = 0; j < snum_v; ++j)
		{
			// V 방향 샘플링 파라미터와 광선의 식을 구하고,
			double v = j / (double)(snum_v - 1);
			GTransf MC = m_pMovingPlanes->EvalFrame(v);
			GLine ray = MC * GLine(GPoint3(), dir);

			// 광선과 교차하는 삼각형까지의 offset을 구한다.
			bool flag = false;
			double offset = 10000000.0;
			int idx = (j == snum_v - 1) ? (NumPln - 2) : ((int)(v * (NumPln - 1)));
			int NumFace = (int)FaceIdxList[idx].size();
			for (int k = 0; k < NumFace; ++k)
			{
				int fidx = FaceIdxList[idx][k];
				int *vidx = pMesh->Faces[fidx].vIdx;
				GPoint3 p1 = pMesh->Verts[vidx[0]].P;
				GPoint3 p2 = pMesh->Verts[vidx[1]].P;
				GPoint3 p3 = pMesh->Verts[vidx[2]].P;

				GPoint3 q;
				double t, alpha, beta, gamma;
				bool ret = ::intersect_line_triangle(p1, p2, p3, ray, t, q, alpha, beta, gamma, false);
				if (ret)
				{
					if (t > 0.0 && t < offset)
					{
						flag = true;
						offset = t;
					}
				}
			}

			// 광선의 교차점이 구해졌다면 샘플링 데이터에 추가한다.
			if (flag)
			{
				Params.push_back(GVector2(u, v));
				Pts.push_back(offset);
			}
		}		
	}

	int NumPts = (int)Pts.size();
	double *params = new double [NumPts * 2];
	double *pts = new double [NumPts];

	for (int i = 0; i < NumPts; ++i)
	{
		params[2 * i] = Params[i][0];
		params[2 * i + 1] = Params[i][1];
		pts[i] = Pts[i];
	}
	GUcbsSrf1D *pSrf = get_gucbs_srf_approx(10, 5 * NumKey, NumPts, params, pts, 6);
	GUcbsSrf1D *pSrf1 = get_gucbs_srf_approx(snum_u / 5, NumKey, NumPts, params, pts, 6);
	delete [] params;
	delete [] pts;

	// 정점의 위치를 구한다.
	for (int i = 0; i < snum_u; ++i)
	{
		// U 방향 샘플링 파라미터를 구하고,
		double u = i / (double)(snum_u - 1);
		double theta = theta0 + (theta1 - theta0) * u;

		for (int j = 0; j < snum_v; ++j)
		{
			// V 방향 샘플링 파라미터와 광선의 식을 구하고,
			double v = j / (double)(snum_v - 1);
			GTransf MC = m_pMovingPlanes->EvalFrame(v);

			GVector4 pt = pSrf->Eval(u, v);
			double offset0 = pt[0];
			// 안쪽 교차점과 바깥쪽 교차점을 구하여 스플린트 메쉬에 추가한다.
			GPoint3 p0(offset0 * sin(theta), 0.0, offset0 * cos(theta));

			pt = pSrf1->Eval(u, v);

			double offset1 = pt[0] + m_SplintOffset;
			GPoint3 p1(offset1 * sin(theta), 0.0, offset1 * cos(theta));
			p0 = MC * p0;
			p1 = MC * p1;
			m_pSplintInner->AddVertex(GVertex(p0, GVector3()));
			m_pSplintOuter->AddVertex(GVertex(p1, GVector3()));	
		}
	}

	delete pSrf;
	delete pSrf1;

	// 삼각형 리스트를 생성한다.
	for (int i = 0; i < snum_u - 1; ++i)
	{
		for(int j = 0; j < snum_v - 1; ++j)
		{
			int vidx0 = i * snum_v + j;
			int vidx1 = vidx0 + snum_v;
			int vidx2 = vidx1 + 1;
			int vidx3 = vidx0 + 1;

			GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
			GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);

			m_pSplintInner->AddFace(f0);
			m_pSplintInner->AddFace(f1);
			m_pSplintOuter->AddFace(f0);
			m_pSplintOuter->AddFace(f1);
		}
	}

	// 스플린트의 렌더링 속성을 지정하고 장면에 추가한다.
	GMaterial mtl = get_material_random();
	m_pSplintInner->Renderer.bBackFaceCull = false;
	m_pSplintInner->AddMtl(mtl.Name);
	m_pSplintInner->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintInner);

	m_pSplintOuter->Renderer.bBackFaceCull = false;
	//m_pSplintOuter->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintOuter->AddMtl(mtl);
	m_pSplintOuter->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintOuter);

	// 사이드 스플린트0, 1 메쉬를 생성하고,
	m_pSplintSide0 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);
	m_pSplintSide1 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);

	// 정점을 추가하고,
	for (int i = 0; i < snum_u; ++i)
	{
		GVertex v0 = m_pSplintInner->Verts[i * snum_v];
		GVertex v1 = m_pSplintOuter->Verts[i * snum_v];
		m_pSplintSide0->AddVertex(v0);
		m_pSplintSide0->AddVertex(v1);

		v0 = m_pSplintOuter->Verts[i * snum_v + snum_v - 1];
		v1 = m_pSplintInner->Verts[i * snum_v + snum_v - 1];
		m_pSplintSide1->AddVertex(v0);
		m_pSplintSide1->AddVertex(v1);
	}

	// 삼각형을 추가한다.
	for (int i = 0; i < snum_u - 1; ++i)
	{
		int vidx0 = i * 2;
		int vidx1 = vidx0 + 2;
		int vidx2 = vidx1 + 1;
		int vidx3 = vidx0 + 1;
		GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
		GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);

		m_pSplintSide0->AddFace(f0);
		m_pSplintSide0->AddFace(f1);

		m_pSplintSide1->AddFace(f0);
		m_pSplintSide1->AddFace(f1);
	}

	// 스플린트의 렌더링 속성을 지정하고 장면에 추가한다.
	m_pSplintSide0->Renderer.bBackFaceCull = false;
	//m_pSplintSide0->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide0->AddMtl(mtl);
	m_pSplintSide0->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide0);

	m_pSplintSide1->Renderer.bBackFaceCull = false;
	//m_pSplintSide1->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide1->AddMtl(mtl);
	m_pSplintSide1->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide1);

	// 사이드 스플린트2, 3메쉬를 생성하고,
	m_pSplintSide2 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);
	m_pSplintSide3 = new GMesh(create_mesh_name("Side_Splint"), GTransf(), FACE_QUAD, pMesh->Name);

	// 정점을 추가하고,
	for (int i = 0; i < snum_v; ++i)
	{
		GVertex v0 = m_pSplintOuter->Verts[i];
		GVertex v1 = m_pSplintInner->Verts[i];
		m_pSplintSide2->AddVertex(v0);
		m_pSplintSide2->AddVertex(v1);

		v0 = m_pSplintInner->Verts[(snum_u - 1) * snum_v + i];
		v1 = m_pSplintOuter->Verts[(snum_u - 1) * snum_v + i];
		m_pSplintSide3->AddVertex(v0);
		m_pSplintSide3->AddVertex(v1);
	}

	// 삼각형을 추가하여,
	for (int i = 0; i < snum_v - 1; ++i)
	{
		int vidx0 = i * 2;
		int vidx1 = vidx0 + 2;
		int vidx2 = vidx1 + 1;
		int vidx3 = vidx0 + 1;
		GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
		GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);

		m_pSplintSide2->AddFace(f0);
		m_pSplintSide2->AddFace(f1);
		m_pSplintSide3->AddFace(f0);
		m_pSplintSide3->AddFace(f1);
	}

	// 렌더링 속성을 지정하여 장면에 추가한다.
	m_pSplintSide2->Renderer.bBackFaceCull = false;
	//m_pSplintSide2->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide2->AddMtl(mtl);
	m_pSplintSide2->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide2);

	m_pSplintSide3->Renderer.bBackFaceCull = false;
	//m_pSplintSide3->Renderer.Alpha = 0.5;
	mtl = get_material_random();
	m_pSplintSide3->AddMtl(mtl);
	m_pSplintSide3->SetMtl(mtl.Name);
	theScene().AddMesh(m_pSplintSide3);
}
*/

void GDentalCadDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// 컨트롤의 값을 변수에 대입하고,
	UpdateData(true);

	if (m_pSplintOuter == NULL)
		return;

	switch (pScrollBar->GetDlgCtrlID())
	{
	case IDC_SPLINT_WIDTH:
		{
			double ratio = (double)m_SplintWidth / 100.0;
			int NumVert = m_pSplintOuter->GetNumVert();
			for (int i = 0; i < NumVert; ++i)
				m_pSplintOuter->Verts[i].P = m_pSplintInner->Verts[i].P + m_pSplintOuter->Verts[i].C * ratio;
			m_pSplintOuter->ModifiedFlag = MODIFIED_VERT_POS_ALL;
			m_pSplintOuter->UpdateMesh();			
		}
		break;

	case IDC_SPLINT_WIDTH_BY_NORMAL:
		{
			double ratio = (double)m_SplintWidthByNormal / 100.0;
			int NumVert = m_pSplintOuter->GetNumVert();
			for (int i = 0; i < NumVert; ++i)
				m_pSplintOuter->Verts[i].P = m_pSplintInner->Verts[i].P + m_pSplintInner->Verts[i].N * ratio;
			m_pSplintOuter->ModifiedFlag = MODIFIED_VERT_POS_ALL;
			m_pSplintOuter->UpdateMesh();			
		}
		break;
	}

	theDoc().UpdateAllViews(NULL);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void GDentalCadDlg::OnBnClickedShowUpperJaw()
{
	if (m_pUpperJaw != NULL)
	{
		if (m_pUpperJaw->Renderer.IsVisibleMesh())
		{
			m_pUpperJaw->HideMesh(false);
			m_bShowUpperJaw = FALSE;
		}
		else
		{
			m_pUpperJaw->ShowMesh(false);
			m_bShowUpperJaw = TRUE;
		}
	}

	theDoc().UpdateAllViews(NULL);
	UpdateData(FALSE);
}

void GDentalCadDlg::OnBnClickedShowLowerJaw()
{
	if (m_pLowerJaw != NULL)
	{
		if (m_pLowerJaw->Renderer.IsVisibleMesh())
		{
			m_pLowerJaw->HideMesh(false);
			m_bShowLowerJaw = FALSE;
		}
		else
		{
			m_pLowerJaw->ShowMesh(false);
			m_bShowLowerJaw = TRUE;
		}
	}

	theDoc().UpdateAllViews(NULL);
	UpdateData(FALSE);
}

void GDentalCadDlg::OnBnClickedShowKeyPlane()
{
	if (m_KeyPlaneList.empty())
		return;

	for (unsigned i = 0; i < m_KeyPlaneList.size(); ++i)
	{
		if (m_KeyPlaneList[i]->Renderer.IsVisibleMesh())
		{
			m_KeyPlaneList[i]->HideMesh(false);
			m_bShowKeyPlane = FALSE;
		}
		else
		{
			m_KeyPlaneList[i]->ShowMesh(false);
			m_bShowKeyPlane = TRUE;
		}
	}
	
	theDoc().UpdateAllViews(NULL);
	UpdateData(FALSE);	
}

void GDentalCadDlg::OnBnClickedShowSwpPlane()
{
	if (m_pMovingPlanes != NULL)
	{
		if (m_pMovingPlanes->Renderer.IsVisibleMesh())
		{
			m_pMovingPlanes->HideMesh(false);
			m_bShowSwpPlane = FALSE;
		}
		else
		{
			m_pMovingPlanes->ShowMesh(false);
			m_bShowSwpPlane = TRUE;
		}
	}

	theDoc().UpdateAllViews(NULL);
	UpdateData(FALSE);
}

void GDentalCadDlg::OnEnKillfocusSplintOffset()
{
	UpdateData(TRUE);
}
