// PCreator.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

PCreator::PCreator()
{
	pDoc = NULL;
	pScene = NULL;
	pBaseMesh = NULL;
	CreateType = CREATE_INACTIVE;
	MouseOverFaceIdx = -1;
	MouseOverVertIdx = -1;
	PrevSltVertIdx = -1;
}

PCreator::PCreator(const PCreator &cpy)
{
	pDoc = cpy.pDoc;
	pScene = cpy.pScene;
	pBaseMesh = cpy.pBaseMesh;
	CreateType = cpy.CreateType;
	PtList = cpy.PtList;
	CutLinePtList = cpy.CutLinePtList;
	MouseOverFaceIdx = cpy.MouseOverFaceIdx;
	MouseOverVertIdx = cpy.MouseOverVertIdx;
	PrevSltVertIdx = cpy.PrevSltVertIdx;
}

PCreator::~PCreator()
{
}

PCreator &PCreator::operator =(const PCreator &rhs)
{
	pDoc = rhs.pDoc;
	pScene = rhs.pScene;
	pBaseMesh = rhs.pBaseMesh;
	CreateType = rhs.CreateType;
	PtList = rhs.PtList;
	CutLinePtList = rhs.CutLinePtList;
	MouseOverFaceIdx = rhs.MouseOverFaceIdx;
	MouseOverVertIdx = rhs.MouseOverVertIdx;
	PrevSltVertIdx = rhs.PrevSltVertIdx;

	return *this;
}

void PCreator::InitPlugIn(GScene *_pScene, CDialog *pParent, CDocument *_pDoc)
{
	pDoc = _pDoc;
	pScene = _pScene;

	CreateCrvDlg.Create(IDD_CREATE_CRV_DLG, pParent);
	CreateCrvDlg.pCreator = this;

	CreateSrfDlg.Create(IDD_CREATE_SRF_DLG, pParent);
	CreateSrfDlg.pCreator = this;

	CreatePlyDlg.Create(IDD_CREATE_PLY_DLG, pParent);
	CreatePlyDlg.pCreator = this;

	CreatePlnDlg.Create(IDD_CREATE_PLN_DLG, pParent);
	CreatePlnDlg.pCreator = this;
}

void PCreator::InitBaseMesh(GMesh *pMesh)
{
	// 선택된 포인트 리스트를 초기화 하고, 임시로 한 점을 추가한다.
	PtList.clear();
	if (CreateType != CREATE_PTS_ON_MESH)
		PtList.push_back(GVector4(0.0, 0.0, 0.0, 1.0));

	pBaseMesh = pMesh;
	pDoc->UpdateAllViews(NULL);
}

/*!
*	\brief 현재 마우스의 포인터의 3차원 좌표를 반환한다.
*
*	\param x 마우스의 X 좌표
*	\param y 마우스의 Y 좌표
*
*	\return 마우스 포인터의 3차원 좌표를 반환한다.
*/
GVector3 PCreator::GetWorldCoord(int x, int y)
{
	double ox, oy, oz;

	if (pScene->MainCamera.CameraType == PROJECTION_ORTHO)
		pScene->MainCamera.GetWorldCoord(x, y, 0.0, &ox, &oy, &oz);
	else
		pScene->MainCamera.GetWorldCoord(x, y, 0.5, &ox, &oy, &oz);

	return GVector3(ox, oy, oz);
}

void PCreator::LButtonDown(int x, int y, unsigned flag)
{
	if (CreateType == CREATE_CUT_LINE)
	{
		// 기존의 절단선 정점을 모두 지우고,
		CutLinePtList.clear();

		// 현재 마우스 포인터와 중첩된 삼각형과 교차점을 계산하고,
		GPoint3 p;
		GFace *pFace = SelectVertOnFace(x, y, p);

		// 삼각형과 정점이 선택되었다면,
		if (pFace != NULL)
		{
			// 절단선 리스트에 정점을 추가하고,
			GCutPt pt(pFace, -1, -1, 0.0, p);
			CutLinePtList.push_back(pt);
			CutLinePtList.push_back(pt);
		}			
	}

	if (CreateType == CREATE_CUT_LINE_FROM_SKETCH)
	{
		// 기존의 절단선 정점을 모두 지우고,
 		CutLinePtList.clear();

		// 기존의 스케치 정점을 지운다.
		Sketch2DPtList.clear();
		Sketch3DPtList.clear();

		// 2D, 3D 경계점 리스트(렌더링 용)에 시작점을 추가한다.
		GVector3 v = GetWorldCoord(x, y);
		Sketch2DPtList.push_back(GVector2(x, y));
		Sketch3DPtList.push_back(v);
	}

	if (CreateType == CREATE_CRV_ON_MESH_FROM_SKETCH)
	{
		// 기존의 스케치 정점을 지운다.
		Sketch2DPtList.clear();
		Sketch3DPtList.clear();

		// 2D, 3D 스케치 정점을 리스트에 추가한다.
		GVector3 p = GetWorldCoord(x, y);
		Sketch2DPtList.push_back(GVector2(x, y));
		Sketch3DPtList.push_back(p);
	}
}

/*!
*	\brief 생성 중인 객체 타입에 따라, 관련 작업을 수행한다.
*
*	\param x 마우스의 X 좌표
*	\param y 마우스의 Y 좌표
*	\param flag 마우스가 움직일 때, 눌려진 버튼의 정보 (MK_LBUTTON, MK_RBUTTON, MK_MBUTTON)
*/
void PCreator::MouseMove(int x, int y, unsigned flag)
{
	switch(CreateType)
	{
		// 생성 중인 객체가 없다면 리턴한다.
	case CREATE_INACTIVE:
		return;

		// 생성 중인 객체가 절단선 이라면,
	case CREATE_CUT_LINE:
		MouseMoveForCutLine(x, y, flag);		
		break;

		// 생성 중인 객체가 메쉬위의 점들이라면,
	case CREATE_PTS_ON_MESH:
		MouseMoveForPtsOnMesh(x, y, flag);
		break;

		// 생성 중인 객체가 측지선, 측지 메쉬라면,
	case CREATE_GEODESIC_PATH:
	case CREATE_GEODESIC_MESH:
		SetMouseOverInfo(x, y, flag);
		break;

		// 생성 중인 객체가 ZX 평면위에 생성되는 객체라면,
	case CREATE_CRV_CTL_PT:
	case CREATE_CRV_INT_PT:
	case CREATE_SRF_CTL_PT:
	case CREATE_NURBS_SPHERE:
	case CREATE_NURBS_TEAPOT:
	case CREATE_POLYGON:
	case CREATE_POLYGON_SPHERE:
	case CREATE_POLYGON_ICOSAHEDRON:
	case CREATE_POLYGON_TEAPOT:
	case CREATE_PLANE:
	case CREATE_CIRCLE:
	case CREATE_POLYGON_DISK:
		MouseMoveForPtOnFloor(x, y, flag);			
		break;

		// 생성 중인 객체가 스케치 기반의 객체라면,
	case CREATE_CRV_ON_MESH_FROM_SKETCH:
	case CREATE_CUT_LINE_FROM_SKETCH:
		if (flag & MK_LBUTTON)
		{
			// 2D, 3D 스케치 정점을 리스트에 추가한다.
			GVector3 p = GetWorldCoord(x, y);
			Sketch2DPtList.push_back(GVector2(x, y));
			Sketch3DPtList.push_back(p);
		}
		break;
	}
}

void PCreator::MouseMoveForCutLine(int x, int y, unsigned flag)
{
	if (!(flag & MK_LBUTTON))
		return;

	// 절단선이 생성 중이라면,
	if (!CutLinePtList.empty())
	{
		// 현재 마우스 포인터와 삼각형의 교차점을 계산한 후,
		GPoint3 CurrPt;
		GFace *pCurrFace = SelectVertOnFace(x, y, CurrPt);

		// 교차한 삼각형이 있다면,
		if (pCurrFace != NULL)
		{
			// 마지막 정점을 현재 정점으로 갱신한다.
			int LastIdx = (int)CutLinePtList.size() - 1;
			CutLinePtList[LastIdx].P = CurrPt;

			// 선택된 삼각형이 변경된 경우,
			GFace *pPrevFace = CutLinePtList[LastIdx - 1].pFace;
			if (pCurrFace != pPrevFace)
			{
				// 참조 메쉬를 구하고,
				GMesh *pMesh = pCurrFace->pMesh;

				// 다시 이전에 선택된 삼각형으로 돌아간 경우라며,
				if (LastIdx > 1 && pCurrFace == CutLinePtList[LastIdx - 2].pFace)
				{
					CutLinePtList.pop_back();
					CutLinePtList[LastIdx - 1].P = CurrPt;
					CutLinePtList[LastIdx - 1].pFace = pCurrFace;
					return;
				}

				// 이전 선택된 삼각형과 현재 선택된 삼각형이 공유하는 정점의 인덱스를 찾는다.
				int sidx, eidx;
				int result = pCurrFace->GetSharedVertIdx(pPrevFace, sidx, eidx);

				// 하나의 정점만 공유한 경우,
				if (result == 0)
				{
					// 공유점을 구하여,
					GCutPt pt(pCurrFace, sidx, eidx, 0.0, pMesh->Verts[sidx].P);

					// 절단 정점의 리스트에 추가한다.
					CutLinePtList[LastIdx] = pt;
					CutLinePtList.push_back(pt);
				}

				// 현재 선택된 삼각형과 이전 삼각형이 인접하면,
				if (result == 1)
				{
					GPoint3 p0 = pMesh->Verts[sidx].P;
					GPoint3 p1 = pMesh->Verts[eidx].P;

					// 인접한 모서리의 직선의 방정식을 구하고,
					GLine l(p0, p1);

					// 현재 정점을 직선위로 투영하여, 새로운 절단 정점을 계산한 후,
					GVector3 v = p1 - p0;
					double t = ((CurrPt - p0) * v) / (v * v);
					GPoint3 q = p0 + t * v;

					// 절단 정점의 리스트에 추가한다.
					GCutPt pt = GCutPt(pCurrFace, sidx, eidx, t, q);

					// 절단 정점의 리스트에 추가한다.
					CutLinePtList[LastIdx] = pt;
					CutLinePtList.push_back(pt);
				}	
			}								
		}
	}
}

void PCreator::MouseMoveForPtsOnMesh(int x, int y, unsigned	flag)
{
	if (!(flag & MK_LBUTTON))
		return;

	static GVector2 PrevPos(-1.0, -1.0);
	GVector2 CurrPos(x, y);

	if (dist(PrevPos, CurrPos) > 2.0)
	{
		// 뷰포트 변환 정보를 얻어와서,
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		// 현재 마우스 위치에서 색상 버퍼의 픽셀 값을 읽어서,
		GLfloat depth;
		glReadPixels(x, viewport[3] - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

		if (0.0f < depth && depth < 1.0f)
		{
			GPoint3 p;
			pScene->MainCamera.GetWorldCoord(x, y, depth, &p[0], &p[1], &p[2]);
			PtList.push_back(GVector4(p[0], p[1], p[2], 1.0));
			PrevPos = CurrPos;
		}
	}
}

void PCreator::MouseMoveForPtOnFloor(int x, int y, unsigned	flag)
{
	// ZX 평면을 정의하고,
	static GPlane ZX(GVector3(0.0, 1.0, 0.0), GPoint3(0.0, 0.0, 0.0));

	// 현재 마우스 포인터의 위치를 지나며, 시점 방향과 평행인 직선을 구하여,
	GLine line = pScene->MainCamera.GetOrthoRay(x, y);

	// ZX 평면과의 교차점을 계산하여,
	GPoint3 r;
	::intersect_line_plane(r, line, ZX);
	if (GetKeyState(VK_SHIFT) < 0)
		r = round_off(r, -1);

	// 선택될 점의 좌표로 지정한다.
	int idx = (int)PtList.size() - 1;
	PtList[idx].Set(r[0], r[1], r[2], 1.0);
}

/*!
*	\brief 마우스 포인터와 교차하는 삼각형과 가장 가까운 정점의 인덱스를 찾는다.
*	\note 최근 수정일(2017/02/06)
*
*	\param x 마우스의 x좌표
*	\param y 마우스의 y좌표
*	\param flag 마우스의 부가정보 (MK_LBUTTON, MK_RBUTTON, MK_MBUTTON)
*
*	\return 마우스와 교차한 삼각형이 있으면 true, 없으면 false를 반환한다.
*/
bool PCreator::SetMouseOverInfo(int x, int y, unsigned flag)
{
	// 선택된 편집 객체가 없다면 -1을 설정하고 리턴한다.
	if (pBaseMesh == NULL)
	{
		MouseOverFaceIdx = -1;
		MouseOverVertIdx = -1;
		MouseOverPtCoords.Set(0.0, 0.0, 0.0);
		return false;
	}

	// 현재 마우스 포인터 위치에서 색상 버퍼의 픽셀 값을 읽어서,
	GLubyte pixel[3];
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

	// 삼각형의 렌더링 인덱스(1, 2, 3, ...)를 구하고,
	int RenderIdx = pixel[0] * 256 * 256 + pixel[1] * 256 + pixel[2];

	// 선택된 삼각형이 없다면,
	if (RenderIdx < 1 || RenderIdx > pBaseMesh->GetNumFace())
	{
		MouseOverFaceIdx = -1;
		MouseOverVertIdx = -1;
		MouseOverPtCoords.Set(0.0, 0.0, 0.0);
		return false;
	}

	// 마우스와 교차된 삼각형의 인덱스를 설정하고,
	MouseOverFaceIdx = RenderIdx - 1;

	// 마우스와 교차된 삼각형의 세 정점을 구하여,
	int vidx0 = pBaseMesh->Faces[MouseOverFaceIdx].vIdx[0];
	int vidx1 = pBaseMesh->Faces[MouseOverFaceIdx].vIdx[1];
	int vidx2 = pBaseMesh->Faces[MouseOverFaceIdx].vIdx[2];
	GVertex *v0 = &(pBaseMesh->Verts[vidx0]);
	GVertex *v1 = &(pBaseMesh->Verts[vidx1]);
	GVertex *v2 = &(pBaseMesh->Verts[vidx2]);

	// 세 정점이 투영된 스크린상의 좌표를 구하고,
	GPoint3 p0, p1, p2;
	pScene->MainCamera.GetScreenCoord(v0, &p0.V[0], &p0.V[1], &p0.V[2]);
	pScene->MainCamera.GetScreenCoord(v1, &p1.V[0], &p1.V[1], &p1.V[2]);
	pScene->MainCamera.GetScreenCoord(v2, &p2.V[0], &p2.V[1], &p2.V[2]);

	// 현재 마우스 위치와 거리를 계산하여, 
	GPoint3 q(x, y, 0.0);
	double dist0 = dist(q, p0);
	double dist1 = dist(q, p1);
	double dist2 = dist(q, p2);

	// 마우스 포인터와 가장 가까운 정점의 인덱스를 찾는다.
	double min_dist = MIN(dist0, MIN(dist1, dist2));
	if (min_dist == dist0)
		MouseOverVertIdx = vidx0;
	else if (min_dist == dist1)
		MouseOverVertIdx = vidx1;
	else
		MouseOverVertIdx = vidx2;

	// 월드 좌표계에서 표현된 삼각형의 정점을 좌표를 구하고,
	p0 = pBaseMesh->MC * pBaseMesh->Verts[vidx0].P;
	p1 = pBaseMesh->MC * pBaseMesh->Verts[vidx1].P;
	p2 = pBaseMesh->MC * pBaseMesh->Verts[vidx2].P;

	// 현재 마우스 위치에서 뷰에 수직한 직선의 방정식을 구하여,
	GLine ray = pScene->MainCamera.GetOrthoRay(x, y);

	// 현재 마우스 포인터와 교차된 삼각형위의 점의 Barycentric 좌표를 계산한다.
	double t, alpha, beta, gamma;
	GPoint3 ispt;
	if (intersect_line_triangle(p0, p1, p2, ray, t, ispt, alpha, beta, gamma, true))
		MouseOverPtCoords.Set(alpha, beta, 1.0 - alpha - beta);
	else
		MouseOverPtCoords.Set(0.0, 0.0, 0.0);

	return true;
}

void PCreator::LButtonUp(int x, int y, unsigned flag)
{
	// 객체 생성 중이 아니라면 리턴한다.
	if (CreateType == CREATE_INACTIVE)
		return;	 

	// 장면 관측을 위해 ALT 키가 눌린 경우 리턴한다.
	if ((GetAsyncKeyState(VK_MENU) & 0x8000))
		return;
	
	// 마지막 점을 중복하여 추가한다.
	if (PtList.size() > 0)
	{
		int NumPt = (int)PtList.size();
		PtList.push_back(PtList[NumPt - 1]);
	}
	
	// 생성 중인 객체의 타입에 따라서 생성을 종료한다.
	switch (CreateType)
	{
		// 마지막 입력을 명시해 주어야 하는 객체라면,
	case CREATE_CRV_CTL_PT:
	case CREATE_CRV_INT_PT:
	case CREATE_POLYGON:
		{
			// 스페이스 키가 눌린 상태로 점이 선택되었다면,
			if ((GetAsyncKeyState(VK_SPACE) & 0x8000))
			{
				// 마지막 점을 제거하고 곡선을 생성한다.
				PtList.pop_back();
				Finish();		
			}
		}
		break;

		// 두 개의 입력 정점으로 생성되는 객체라면,
	case CREATE_SRF_CTL_PT:
	case CREATE_NURBS_SPHERE:
	case CREATE_NURBS_TEAPOT:
	case CREATE_CIRCLE:
	case CREATE_PLANE:
	case CREATE_POLYGON_SPHERE:
	case CREATE_POLYGON_ICOSAHEDRON:
	case CREATE_POLYGON_TEAPOT:
	case CREATE_POLYGON_DISK:
		{
			if (PtList.size() == 3)
				Finish();
		}
		break;

		// 마우스 버튼을 놓을 때, 생성되는 객체라면, 
	case CREATE_CUT_LINE:
	case CREATE_CUT_LINE_FROM_SKETCH:
	case CREATE_PTS_ON_MESH:
	case CREATE_CRV_ON_MESH_FROM_SKETCH:
		{
			Finish();
		}		
		break;

		// 폴리곤 객체위에 측지 경로를 생성 중이라면,
	case CREATE_GEODESIC_PATH:
		{
			// 마우스와 교차한 정점이 있다면,
			if (MouseOverVertIdx != -1)
			{
				// 경로를 추가하고,
				GGeoPath *pPath = (GGeoPath *)pScene->pRecentMesh;
				pPath->AddPath(MouseOverVertIdx);
			}

			// 스페이스 키가 눌린 상태로 점이 선택되었다면, 경로 생성을 종료한다.
			if ((GetAsyncKeyState(VK_SPACE) & 0x8000))
				Finish();
		}
		break;

		// 폴리곤 객체위에 측지 메쉬를 생성 중이라면,
	case CREATE_GEODESIC_MESH:
		{
			// 마우스와 교차한 정점이 있다면,
			if (MouseOverVertIdx != -1)
			{
				// Coons 곡면의 코너 정점을 추가하고,
				GCoonsMesh *pMesh = (GCoonsMesh *)pScene->pRecentMesh;
				GMesh *pNewMesh = pMesh->AddVertIdx2(MouseOverVertIdx);
				if (pNewMesh != NULL)
				{
					pScene->AddMesh(pNewMesh);
					Finish();
				}
			}
		}
		break;
	}
}

/*!
*	\brief 브러쉬 툴의 좌표계를 계산한다.
*
*	\param x 마우스의 x좌표
*	\param y 마우스의 y좌표
*	\param ispt 마우스 포인터와 삼각형의 교차점이 저장됨
*
*	\return 마우스 포인터와 교차하는 삼각형의 포인터를 반환한다.
*/
GFace *PCreator::SelectVertOnFace(int x, int y, GPoint3 &ispt)
{
	if (pBaseMesh == NULL)
		return NULL;

	// 뷰포트 변환 정보를 얻어와서,
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// 현재 마우스 위치에서 색상 버퍼의 픽셀 값을 읽어서,
	GLubyte pixel[3];
	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

	// 마우스와 중첩된 삼각형의 인덱스를 구하고,
	int Idx = pixel[0] * 256 * 256 + pixel[1] * 256 + pixel[2] - 1;

	int NumFace = pBaseMesh->GetNumFace();
	if (Idx >= 0 && Idx < NumFace)
	{
		// 참조 메쉬에서 대응되는 삼각형을 찾아,
		GFace *pFace = &(pBaseMesh->Faces[Idx]);

		// 카메라 좌표계에서 표현된 삼각형의 정점을 좌표를 구하고,
		int vidx0 = pFace->vIdx[0];
		int vidx1 = pFace->vIdx[1];
		int vidx2 = pFace->vIdx[2];
		GPoint3 p0 = pBaseMesh->MC * pBaseMesh->Verts[vidx0].P;
		GPoint3 p1 = pBaseMesh->MC * pBaseMesh->Verts[vidx1].P;
		GPoint3 p2 = pBaseMesh->MC * pBaseMesh->Verts[vidx2].P;

		// 현재 마우스 위치에서 뷰에 수직한 직선의 방정식을 구하여,
		GLine ray = pScene->MainCamera.GetOrthoRay(x, y);
		
		// 삼각형과의 교차점을 구하여 브러쉬 툴의 좌표계를 계산한다.
		if (intersect_line_triangle(p0, p1, p2, ray, ispt, true))
		{
			ispt = inv(pBaseMesh->MC) * ispt;
			return pFace;
		}
	}

	return NULL;
}

void PCreator::Reset()
{
	PtList.clear();
	CutLinePtList.clear();
	Sketch2DPtList.clear();
	Sketch3DPtList.clear();
	CreateType = CREATE_INACTIVE;
	pBaseMesh = NULL;
	MouseOverFaceIdx = -1;
	MouseOverVertIdx = -1;
	PrevSltVertIdx = -1;
}

void PCreator::Render()
{
	switch (CreateType)
	{
	case CREATE_CRV_CTL_PT:
	case CREATE_CRV_INT_PT:
		RenderNurbsCrv();
		break;

	case CREATE_SRF_CTL_PT:
		RenderNurbsSrf();
		break;

	case CREATE_NURBS_SPHERE:
		RenderNurbsSphere();
		break;

	case CREATE_NURBS_TEAPOT:
		RenderNurbsTeapot();
		break;

	case CREATE_POLYGON:
		RenderPolygon();
		break;

	case CREATE_PLANE:
		RenderPolygonPlane();
		break;	

	case CREATE_POLYGON_DISK:
		RenderPolygonDisk();
		break;	

	case CREATE_CIRCLE:
		RenderNurbsCircle();
		break;

	case CREATE_CUT_LINE:
		RenderCutLine();
		break;

	case CREATE_POLYGON_SPHERE:
	case CREATE_POLYGON_ICOSAHEDRON:
		RenderPolygonSphere();
		break;

	case CREATE_POLYGON_TEAPOT:
		RenderPolygonTeapot();
		break;

	case CREATE_GEODESIC_PATH:
		RenderMouseOverVert();
		RenderGeodesicPath();
		break;

	case CREATE_GEODESIC_MESH:
		RenderMouseOverVert();
		RenderCoonsBndry();
		break;

	case CREATE_PTS_ON_MESH:
		RenderPtsOnMesh();
		break;

	case CREATE_CRV_ON_MESH_FROM_SKETCH:
		RenderCrvOnMeshFromSketch();
		break;

	case CREATE_CUT_LINE_FROM_SKETCH:
		RenderCutLineFromSketch();
		break;
	}
}

/*!
*	\brief 마우스와 교차한 참조 메쉬의 정점을 렌더링 한다.
*/
void PCreator::RenderMouseOverVert()
{
	// 불필요한 경우 리턴한다.
	if (pBaseMesh == NULL || MouseOverVertIdx == -1)
		return;

	// 조명과 깊이 테스트를 끄고,
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	// 월드 좌표계에서
	glPushMatrix();
	{
		// 선택 객체의 모델링 좌표계로 변환하고,
		glMultMatrixd(cast_mat(pBaseMesh->MC).M);

		// 선택된 정점을 그린다.
		GVertex *pVert = &(pBaseMesh->Verts[MouseOverVertIdx]);
		
		//glColor3f(0.2627, 1.0, 0.64);
		glColor3f(1.0, 0.0, 0.0);
		glPointSize(8.0);
		glBegin(GL_POINTS);
		{
			glVertex3dv(pVert->P.V);
		}
		glEnd();
		glPointSize(1.0);
	}
	glPopMatrix();		// 월드 좌표계로 복원한다.

	// 조명과 깊이 테스트를 다시 켠다.
	glEnable(GL_LIGHTING);	
	glEnable(GL_DEPTH_TEST);
}

/*!
*	\brief 생성되고 있는 최단 경로를 렌더링한다.
*/
void PCreator::RenderGeodesicPath()
{
	GGeoPath *pPath = (GGeoPath *)pScene->pRecentMesh;
	if (pPath->PathVertIdx.empty() || MouseOverVertIdx == -1)
		return;

	int PrevVertIdx = pPath->PathVertIdx[pPath->PathVertIdx.size() - 1];
	int CurrVertIdx = MouseOverVertIdx;

	GVertex *P = &(pPath->pRefMesh->Verts[PrevVertIdx]);
	GVertex *Q = &(pPath->pRefMesh->Verts[CurrVertIdx]);

	std::vector<GPoint3> Paths;
	::get_geodesic_paths(pPath->pRefMesh, P, Q, Paths);

	// Anti-aliasing 기능을 활성화하고,
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 월드 좌표계에서
	glPushMatrix();
	{
		// 선택 객체의 모델링 좌표계로 변환하고,
		glMultMatrixd(cast_mat(pPath->pRefMesh->MC).M);

		glColor3f(1.0, 0.0, 0.0);
		glPointSize(8.0);
		glBegin(GL_POINTS);
		glVertex3dv(P->P.V);
		glVertex3dv(Q->P.V);
		glEnd();

		int NumVert = Paths.size();
		glLineWidth(3.0);
		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < NumVert; ++i)
		{
			glVertex3dv(Paths[i].V);
		}
		glEnd();
		glLineWidth(1.0);
	}
	glPopMatrix();		// 월드 좌표계로 복원한다.

	// Anti-aliasing 비활성화 한다		
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POINT_SMOOTH);
}

/*!
*	\brief 생성되고 있는 Quad Layout를 렌더링한다.
*/
void PCreator::RenderQuadLayout()
{
	if (PrevSltVertIdx == -1 || MouseOverVertIdx == -1)
		return;

	int PrevVertIdx = PrevSltVertIdx;
	int CurrVertIdx = MouseOverVertIdx;

	GVertex *P = &(pBaseMesh->Verts[PrevVertIdx]);
	GVertex *Q = &(pBaseMesh->Verts[CurrVertIdx]);

	std::vector<GPoint3> Paths;
	::get_geodesic_paths(pBaseMesh, P, Q, Paths);

	// Anti-aliasing 기능을 활성화하고,
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 월드 좌표계에서
	glPushMatrix();
	{
		// 선택 객체의 모델링 좌표계로 변환하고,
		glMultMatrixd(cast_mat(pBaseMesh->MC).M);

		glColor3f(1.0, 0.0, 0.0);
		glPointSize(8.0);
		glBegin(GL_POINTS);
		glVertex3dv(P->P.V);
		glVertex3dv(Q->P.V);
		glEnd();

		int NumVert = Paths.size();
		glLineWidth(3.0);
		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < NumVert; ++i)
		{
			glVertex3dv(Paths[i].V);
		}
		glEnd();
		glLineWidth(1.0);
	}
	glPopMatrix();		// 월드 좌표계로 복원한다.

	// Anti-aliasing 비활성화 한다		
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
}

/*!
*	\brief 생성 중인 Coons 메쉬의 경계를 렌더링 한다.
*/
void PCreator::RenderCoonsBndry()
{
	// 생성 중인 Coons 메쉬를 구하고,
	GCoonsMesh *pMesh = (GCoonsMesh *)pScene->pRecentMesh;
	if (pMesh->VertIdx.empty() || MouseOverVertIdx == -1)
		return;

	// 임시 경로를 저장할 정적 변수를 선언하고,
	std::vector<GPoint3> Path[4];

	// 현재 선택된 정점의 인덱스 개수를 구하고,
	int NumIdx = pMesh->VertIdx.size();

	int vidx0, vidx1, vidx2, vidx3;

	if (NumIdx == 1)
	{
		vidx0 = pMesh->VertIdx[0];
		vidx1 = MouseOverVertIdx;
		vidx2 = MouseOverVertIdx;
		vidx3 = MouseOverVertIdx;
	}

	if (NumIdx == 2)
	{
		vidx0 = pMesh->VertIdx[0];
		vidx1 = pMesh->VertIdx[1];
		vidx2 = MouseOverVertIdx;
		vidx3 = MouseOverVertIdx;
	}

	if (NumIdx == 3)
	{
		vidx0 = pMesh->VertIdx[0];
		vidx1 = pMesh->VertIdx[1];
		vidx2 = pMesh->VertIdx[2];
		vidx3 = MouseOverVertIdx;
	}

	GVertex *P = &(pMesh->pRefMesh->Verts[vidx0]);
	GVertex *Q = &(pMesh->pRefMesh->Verts[vidx1]);
	GVertex *R = &(pMesh->pRefMesh->Verts[vidx2]);
	GVertex *S = &(pMesh->pRefMesh->Verts[vidx3]);
	::get_geodesic_paths(pMesh->pRefMesh, P, Q, Path[0]);
	::get_geodesic_paths(pMesh->pRefMesh, Q, R, Path[1]);
	::get_geodesic_paths(pMesh->pRefMesh, R, S, Path[2]);
	::get_geodesic_paths(pMesh->pRefMesh, S, P, Path[3]);

	// Anti-aliasing 기능을 활성화하고,
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 월드 좌표계에서
	glPushMatrix();
	{
		// 선택 객체의 모델링 좌표계로 변환하고,
		glMultMatrixd(cast_mat(pBaseMesh->MC).M);

		// Coons 메쉬의 경계를 렌더링 한다.
		glLineWidth(3.0);
		glColor3f(0.0, 0.0, 1.0);
		
		for (int i = 0; i < 4; ++i)
		{
			glBegin(GL_LINE_STRIP);
			int NumPt = Path[i].size();
			for (int j = 0; j < NumPt; ++j)
			{
				glVertex3dv(Path[i][j].V);
			}
			glEnd();
		}

		glLineWidth(1.0);
	}
	glPopMatrix();		// 월드 좌표계로 복원한다.

	// Anti-aliasing 비활성화 한다		
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
}

/*!
*	\brief 참조 메쉬의 정점과 삼각형 선택을 위해 렌더링 한다.
*/
void GetColorId(unsigned int &r, unsigned int &g, unsigned int &b)
{
	b++;
	if (b > 255)
	{
		b = 0;
		g++;
		if (g > 255)
		{
			g = 0;
			r++;
		}
	}
}
void PCreator::RenderForSelect()
{
	// 참조 메쉬가 선택되지 않았다면 바로 리턴한다.
	if (pBaseMesh == NULL)
		return;

	// 참조 메쉬위에 생성되는 객체라면,
	 if (CreateType == CREATE_CUT_LINE || CreateType == CREATE_GEODESIC_PATH || 
		 CreateType == CREATE_GEODESIC_MESH || CreateType == CREATE_CRV_ON_MESH_FROM_SKETCH ||
		 CreateType == CREATE_CUT_LINE_FROM_SKETCH)
	 {
		 // 색상 버퍼를 초기화 한후,
		 glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		 // 장면의 관측 공간 및 투영 방법 지정
		 glMatrixMode(GL_PROJECTION);
		 glLoadIdentity();	
		 pScene->SetupViewFrustum();

		 // 장면의 관측 변환을 수행
		 glMatrixMode(GL_MODELVIEW);
		 glLoadIdentity();
		 pScene->SetupViewTransform();

		 // 모든 렌더링 관련 기능을 끄고,
		 glDisable(GL_LIGHTING);
		 glDisable(GL_FOG);
		 glDisable(GL_TEXTURE_2D);

		 glPushMatrix();
		 {
			 // 월드 좌표계에서 메쉬 좌표계로의 변환하고,
			 glMultMatrixd(cast_mat(pBaseMesh->MC).M);

			 // 정점과 삼각형의 개수를 구하고,
			 int NumVert = pBaseMesh->GetNumVert();
			 int NumFace = pBaseMesh->GetNumFace();

			 // 색상의 아이디를 초기화 하고,
			 unsigned int r = 0, g = 0, b = 0;

			 // 삼각형을 색상 아이디로 렌더링 한다.
			 glBegin(GL_TRIANGLES);
			 for (int i = 0; i < NumFace; ++i)
			 {
				 int vidx0 = pBaseMesh->Faces[i].vIdx[0];
				 int vidx1 = pBaseMesh->Faces[i].vIdx[1];
				 int vidx2 = pBaseMesh->Faces[i].vIdx[2];

				 GetColorId(r, g, b);	// (0, 0, 1) 부터 시작함.
				 glColor3ub(r, g, b);

				 glVertex3dv(pBaseMesh->Verts[vidx0].P.V);
				 glVertex3dv(pBaseMesh->Verts[vidx1].P.V);
				 glVertex3dv(pBaseMesh->Verts[vidx2].P.V);
			 }
			 glEnd();	
		 }
		 glPopMatrix();
	 }
}

void PCreator::RenderCutLine()
{
	if (!CutLinePtList.empty())
	{
		int NumPt = (int)CutLinePtList.size();

		glDisable(GL_LIGHTING);

		// 절단 정점
		glColor3f(1.0, 0.0, 0.0);
		glPointSize(5.0);
		glBegin(GL_POINTS);
		for (int i = 0; i < NumPt; ++i)
		{
			GPoint3 p = CutLinePtList[i].P;
			p = pBaseMesh->MC * p;
			glVertex3f(p[0], p[1], p[2]);
		}
		glEnd();
		glPointSize(1.0);

		// 절단 선
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < NumPt; ++i)
		{
			GPoint3 p = CutLinePtList[i].P;
			p = pBaseMesh->MC * p;
			glVertex3f(p[0], p[1], p[2]);
		}
		glEnd();
		
		GFace *pCurrFace = CutLinePtList[NumPt - 1].pFace;
		int vidx0 = pCurrFace->vIdx[0];
		int vidx1 = pCurrFace->vIdx[1];
		int vidx2 = pCurrFace->vIdx[2];

		// 참조 메쉬를 구하고,
		GMesh *pMesh = pCurrFace->pMesh;
		GPoint3 p0 = pMesh->MC * pMesh->Verts[vidx0].P;
		GPoint3 p1 = pMesh->MC * pMesh->Verts[vidx1].P;
		GPoint3 p2 = pMesh->MC * pMesh->Verts[vidx2].P;

		glLineWidth(3.0);
		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_LINE_LOOP);
		glVertex3f(p0[0], p0[1], p0[2]);
		glVertex3f(p1[0], p1[1], p1[2]);
		glVertex3f(p2[0], p2[1], p2[2]);
		glEnd();
		glLineWidth(1.0); 	

		glEnable(GL_LIGHTING);
	}
}

void PCreator::RenderPtsOnMesh()
{
	if (PtList.empty())
		return;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glColor3d(1.0, 0.0, 0.0);
	glLineWidth(5.0f);

	int NumPt = PtList.size();
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < NumPt; ++i)
	{
		glVertex3f(PtList[i][0], PtList[i][1], PtList[i][2]);
	}
	glEnd();
	glEnable(GL_DEPTH_TEST);

	glLineWidth(1.0f);
	glEnable(GL_LIGHTING);
}

void PCreator::RenderCrvOnMeshFromSketch()
{
	if (Sketch3DPtList.empty())
		return;

	// 조명을 끄고 경계 색상과 경계선의 패턴을 지정한다.
	glDisable(GL_LIGHTING);
	glColor3f(0.2627, 1.0, 0.64);
	glLineWidth(4.0);

	// 현재 올가미 선택 영역을 표시한다.
	glBegin(GL_LINE_STRIP);
	{
		for (unsigned i = 0; i < Sketch3DPtList.size(); ++i)
			glVertex3f(Sketch3DPtList[i][0], Sketch3DPtList[i][1], Sketch3DPtList[i][2]);
	}			
	glEnd();

	// 조명을 활성화하고 경계선의 패턴을 비활성화 한다.
	glEnable(GL_LIGHTING);
	glLineWidth(1.0f);
}

void PCreator::CreateCrvOnMeshFromSketch()
{
	// 메쉬의 교차점을 저장하는 리스트 변수를 정의하고,
	std::vector<GPoint3> IsPtList;

	// 첫 번째 교차점을 저장한다.
	if (SetMouseOverInfo(Sketch2DPtList[0][0], Sketch2DPtList[0][1], 0))
	{
		int *vidx = pBaseMesh->Faces[MouseOverFaceIdx].vIdx;
		GPoint3 p0 = pBaseMesh->MC * pBaseMesh->Verts[vidx[0]].P;
		GPoint3 p1 = pBaseMesh->MC * pBaseMesh->Verts[vidx[1]].P;
		GPoint3 p2 = pBaseMesh->MC * pBaseMesh->Verts[vidx[2]].P;
		GPoint3 pt = p2 + (p0 - p2) * MouseOverPtCoords[0] + (p1 - p2) * MouseOverPtCoords[1];
		IsPtList.push_back(pt);
	}

	// 스케치의 두 인접한 정점들에 대하여,
	int NumPt = (int)Sketch2DPtList.size();
	for (int i = 0; i < NumPt - 1; ++i)
	{
		// 두 점 St와 Ed을 교차하는 모든 픽셀의 좌표를 구한다.
		std::vector<CPoint> PixelList;
		CPoint St(Sketch2DPtList[i][0], Sketch2DPtList[i][1]);
		CPoint Ed(Sketch2DPtList[i + 1][0], Sketch2DPtList[i + 1][1]);
		GetPixelList(St, Ed, PixelList);

		GFace *pPrevFace = NULL;
		for (unsigned j = 0; j < PixelList.size(); ++j)
		{
			// 각 픽셀의 위치에서 색상 버퍼의 픽셀 값을 읽어서,
			GLubyte color[3];
			GLint viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport);
			glReadPixels(PixelList[j].x, viewport[3] - PixelList[j].y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, color);

			// 선택된 삼각형을 구하고, 없다면 다음 픽셀로 넘어간다.
			int CurrFaceIdx = color[0] * 256 * 256 + color[1] * 256 + color[2] - 1;
			if (CurrFaceIdx < 0 || CurrFaceIdx > pBaseMesh->GetNumFace() - 1)
				continue;

			// 선택된 삼각형이 이전 삼각형과 다른 삼각형이라면,
			GFace *pCurrFace = &(pBaseMesh->Faces[CurrFaceIdx]);
			if (pCurrFace != pPrevFace && pPrevFace != NULL)
			{
				int *vIdx = pCurrFace->vIdx;
				GPoint3 p0 = pBaseMesh->MC * pBaseMesh->Verts[vIdx[0]].P;
				GPoint3 p1 = pBaseMesh->MC * pBaseMesh->Verts[vIdx[1]].P;
				GPoint3 p2 = pBaseMesh->MC * pBaseMesh->Verts[vIdx[2]].P;
				GLine ray = pScene->MainCamera.GetOrthoRay(PixelList[j].x, PixelList[j].y);

				GPoint3 ispt;
				if (intersect_line_triangle(p0, p1, p2, ray, ispt))
					IsPtList.push_back(ispt);
				else
					continue;
			}
			// 이전 삼각형을 갱신한다.
			pPrevFace = pCurrFace;
		}
	}

	// 마지막 교차점을 저장한다.
	if (SetMouseOverInfo(Sketch2DPtList[NumPt - 1][0], Sketch2DPtList[NumPt - 1][1], 0))
	{
		int *vidx = pBaseMesh->Faces[MouseOverFaceIdx].vIdx;
		GPoint3 p0 = pBaseMesh->MC * pBaseMesh->Verts[vidx[0]].P;
		GPoint3 p1 = pBaseMesh->MC * pBaseMesh->Verts[vidx[1]].P;
		GPoint3 p2 = pBaseMesh->MC * pBaseMesh->Verts[vidx[2]].P;
		GPoint3 pt = p2 + (p0 - p2) * MouseOverPtCoords[0] + (p1 - p2) * MouseOverPtCoords[1];
		IsPtList.push_back(pt);
	}

	if (IsPtList.size() < 4)
		return;

	// 교차점을 근사하는 3차 B-spline 곡선을 생성한다.
	int n = MAX(3, IsPtList.size() / 5);
	GNurbsCrv *pCrv = ::get_gnurbs_crv_approx(3, n, IsPtList, PARAM_CHORD_LEN, KNOT_APPROX);
	pCrv->RefMeshName = pBaseMesh->Name;
	pCrv->Name = create_mesh_name("Sketch Curve");
	//pCrv->Renderer.RenderType |= RENDER_CTRL_PT;
	pCrv->Renderer.SetSampleNum(20, pCrv->n * 10);
	double size = pBaseMesh->BBox.GetDiagonal() * 0.003;
	pCrv->Renderer.CtrlPtSize = (pScene->CtrlPtSize == 0.0) ? size : pScene->CtrlPtSize;
	
	// 생성된 곡선의 시작 점과 끝점의 제어점을 구한다.
	GPoint3 P0 = IsPtList[0];
	GPoint3 Pn = IsPtList[IsPtList.size() - 1];

	// 장면에 포함된 모든 메쉬를 구하고,
	std::vector<GMesh *> MeshList;
	GetPreOrderMeshList(pScene, MeshList, NULL);

	// 생성된 곡선의 시작 점과 끝 점에 가장 가까운 곡선을 저장할 변수를 정의한다.
	GNurbsCrv *pStProxiCrv, *pEdProxiCrv;
	double StParam, EdParam;
	double StMinDist = pCrv->Renderer.CtrlPtSize * 3.0;
	double EdMinDist = pCrv->Renderer.CtrlPtSize * 3.0;
	bool StFlag = false;
	bool EdFlag = false;

	// 장면의 모든 곡선 객체에 대하여,
	int NumMesh = (int)MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
	{
		GMesh *pMesh = MeshList[i];
		double d, tmp;
		if (pMesh->GetClassName() == "GNurbsCrv")
		{
			GNurbsCrv *pOtherCrv = (GNurbsCrv *)pMesh;
			// 곡선의 시작 점과 가장 가까운 다른 곡선위의 점을 찾는다.
			d = ::get_min_dist(P0, pOtherCrv, &tmp);
			if (d < StMinDist)
			{
				pStProxiCrv = pOtherCrv;
				StMinDist = d;
				StParam = tmp;
				StFlag = true;
			}

			// 곡선의 끝 점과 가장 가까운 다른 곡선위의 점을 찾는다.
			d = ::get_min_dist(Pn, pOtherCrv, &tmp);
			if (d < EdMinDist)
			{
				pEdProxiCrv = pOtherCrv;
				EdMinDist = d;
				EdParam = tmp;
				EdFlag = true;
			}
		}
	}

	if (StFlag)
	{
		double umin, umax;
		pStProxiCrv->GetDomain(&umin, &umax);
		StParam = ((StParam - umin) < (umax - umin) * 0.2) ? umin : StParam;
		StParam = ((umax - StParam) < (umax - umin) * 0.2) ? umax : StParam;
		GPoint3 StPt = cast_pt3(pStProxiCrv->Eval(StParam));
		pCrv->P[0].Set(StPt[0], StPt[1], StPt[2], 1.0);

		if (StParam != umin && StParam != umax)
		{
			GNurbsCrv *pLeftCrv, *pRightCrv;
			::subdivide(pStProxiCrv, StParam, &pLeftCrv, &pRightCrv);
			pScene->DelMesh(pScene->MeshList, pStProxiCrv->Name);
			
			if (pLeftCrv != NULL)
			{
				//pLeftCrv->Renderer.RenderType |= RENDER_CTRL_PT;
				pLeftCrv->Renderer.SetSampleNum(20, pLeftCrv->n * 10);
				pLeftCrv->Renderer.CtrlPtSize = pCrv->Renderer.CtrlPtSize;
				pScene->AddMesh(pLeftCrv);
				GMaterial mtl = ::get_material_random();
				pLeftCrv->AddMtl(mtl);	
				pLeftCrv->SetMtl(mtl.Name);	
			}

			if (pRightCrv != NULL)
			{
				//pRightCrv->Renderer.RenderType |= RENDER_CTRL_PT;
				pRightCrv->Renderer.SetSampleNum(20, pRightCrv->n * 10);
				pRightCrv->Renderer.CtrlPtSize = pCrv->Renderer.CtrlPtSize;
				pScene->AddMesh(pRightCrv);
				GMaterial mtl = ::get_material_random();
				pRightCrv->AddMtl(mtl);	
				pRightCrv->SetMtl(mtl.Name);
			}			
		}
	}

	if (EdFlag)
	{
		double umin, umax;
		pEdProxiCrv->GetDomain(&umin, &umax);
		EdParam = ((EdParam - umin) < (umax - umin) * 0.2) ? umin : EdParam;
		EdParam = ((umax - EdParam) < (umax - umin) * 0.2) ? umax : EdParam;
		GPoint3 EdPt = cast_pt3(pEdProxiCrv->Eval(EdParam));
		pCrv->P[pCrv->n].Set(EdPt[0], EdPt[1], EdPt[2], 1.0);

		if (EdParam != umin && EdParam != umax)
		{
			GNurbsCrv *pLeftCrv, *pRightCrv;
			::subdivide(pEdProxiCrv, EdParam, &pLeftCrv, &pRightCrv);
			pScene->DelMesh(pScene->MeshList, pEdProxiCrv->Name);

			if (pLeftCrv != NULL)
			{
				//pLeftCrv->Renderer.RenderType |= RENDER_CTRL_PT;
				pLeftCrv->Renderer.SetSampleNum(20, pLeftCrv->n * 10);
				pLeftCrv->Renderer.CtrlPtSize = pCrv->Renderer.CtrlPtSize;
				pScene->AddMesh(pLeftCrv);
				GMaterial mtl = ::get_material_random();
				pLeftCrv->AddMtl(mtl);	
				pLeftCrv->SetMtl(mtl.Name);	
			}

			if (pRightCrv != NULL)
			{
				//pRightCrv->Renderer.RenderType |= RENDER_CTRL_PT;
				pRightCrv->Renderer.SetSampleNum(20, pRightCrv->n * 10);
				pRightCrv->Renderer.CtrlPtSize = pCrv->Renderer.CtrlPtSize;
				pScene->AddMesh(pRightCrv);
				GMaterial mtl = ::get_material_random();
				pRightCrv->AddMtl(mtl);	
				pRightCrv->SetMtl(mtl.Name);
			}		
		}
	}

	// 객체를 장면에 추가한다.
	pScene->AddMesh(pCrv);
}

/*!
*	\brief 생성 중인 절단선을 렌더링 한다.
*/
void PCreator::RenderCutLineFromSketch()
{
	if (Sketch3DPtList.empty())
		return;

	// 조명을 끄고 경계 색상과 경계선의 패턴을 지정한다.
	glDisable(GL_LIGHTING);
	glColor3f(0.2627, 1.0, 0.64);
	glLineWidth(4.0);

	// 현재 올가미 선택 영역을 표시한다.
	glBegin(GL_LINE_STRIP);
	{
		for (unsigned i = 0; i < Sketch3DPtList.size(); ++i)
			glVertex3f(Sketch3DPtList[i][0], Sketch3DPtList[i][1], Sketch3DPtList[i][2]);
	}			
	glEnd();

	// 조명을 활성화하고 경계선의 패턴을 비활성화 한다.
	glEnable(GL_LIGHTING);
	glLineWidth(1.0f);
}

/*!
*	\brief 생성 중인 절단선을 완성한다.
*/
/*
void PCreator::CreateCutLineFromSketch()
{
	// 시작 삼각형과 교차점을 구하고, 없으면 리턴한다.
	GPoint3 CurrPt;
	GFace *pPrevFace = SelectVertOnFace(Sketch2DPtList[0][0], Sketch2DPtList[0][1], CurrPt);
	if (pPrevFace == NULL)
		return;

	std::vector<GPoint3> Pts;
	// 스케치 점의 순서쌍 (Sketch2DPtList[i], Sketch2DPtList[i + 1])에 대하여,
	for (unsigned i = 0; i < Sketch2DPtList.size() - 1; ++i)
	{
		// 직선 (St, Ed)와 교차하는 모든 픽셀의 좌표를 구한다.
		std::vector<CPoint> PixelList;
		CPoint St(Sketch2DPtList[i][0], Sketch2DPtList[i][1]);
		CPoint Ed(Sketch2DPtList[i + 1][0], Sketch2DPtList[i + 1][1]);
		GetPixelList(St, Ed, PixelList);

		for (unsigned j = 0; j < PixelList.size(); ++j)
		{
			// 픽셀과 삼각형의 교차점을 계산한 후,
			GFace *pCurrFace = SelectVertOnFace(PixelList[j].x, PixelList[j].y, CurrPt);

			// 교차한 삼각형이 이전 교차 삼각형과 다른 경우,
			if (pCurrFace != NULL && pCurrFace != pPrevFace)
			{
				Pts.push_back(GPoint3(PixelList[j].x, PixelList[j].y, 0.0));
				pPrevFace = pCurrFace;
			}
		}
	}

	int N = MAX(3, Pts.size() / 2); 
	GNurbsCrv *pCrv = ::get_gnurbs_crv_approx(3, N, Pts, PARAM_CHORD_LEN, KNOT_APPROX);

	GPoint3 p0 = cast_pt3(pCrv->Eval(0.0));
	for (unsigned i = 1; i < Pts.size(); ++i)
	{
		double u = (double)i / (double)(Pts.size() - 1);
		GPoint3 p1 = cast_pt3(pCrv->Eval(u));

		// 직선 (St, Ed)와 교차하는 모든 픽셀의 좌표를 구한다.
		std::vector<CPoint> PixelList;
		CPoint St(p0[0], p0[1]);
		CPoint Ed(p1[0], p1[1]);
		GetPixelList(St, Ed, PixelList);

		for (unsigned j = 0; j < PixelList.size(); ++j)
		{
			// 픽셀과 삼각형의 교차점을 계산한 후,
			GFace *pCurrFace = SelectVertOnFace(PixelList[j].x, PixelList[j].y, CurrPt);

			// 교차한 삼각형이 이전 교차 삼각형과 다른 경우,
			if (pCurrFace != NULL && pCurrFace != pPrevFace)
			{
				// 이전 선택된 삼각형과 현재 선택된 삼각형이 공유하는 정점의 인덱스를 찾는다.
				int sidx, eidx;
				int result = pCurrFace->GetSharedVertIdx(pPrevFace, sidx, eidx);

				// 하나의 정점만 공유한 경우,
				if (result == 0)
				{
					// 공유점을 구하여,
					GCutPt pt(pCurrFace, sidx, eidx, 0.0, pBaseMesh->Verts[sidx].P);

					// 절단 정점의 리스트에 추가한다.
					CutLinePtList.push_back(pt);

					std::cout << "Cut point on tiangle's vertex is found" << std::endl;
				}

				// 현재 선택된 삼각형과 이전 삼각형이 인접하면,
				if (result == 1)
				{
					GPoint3 p0 = pBaseMesh->Verts[sidx].P;
					GPoint3 p1 = pBaseMesh->Verts[eidx].P;

					// 인접한 모서리의 직선의 방정식을 구하고,
					GLine l(p0, p1);

					// 현재 정점을 직선위로 투영하여, 새로운 절단 정점을 계산한 후,
					GVector3 v = p1 - p0;
					double t = ((CurrPt - p0) * v) / (v * v);
					GPoint3 q = p0 + t * v;

					// 절단 정점의 리스트에 추가한다.
					GCutPt pt = GCutPt(pCurrFace, sidx, eidx, t, q);

					// 절단 정점의 리스트에 추가한다.
					CutLinePtList.push_back(pt);
				}
				// 이전 삼각형을 갱신한다.
				pPrevFace = pCurrFace;
			}
		}
		p0 = p1;
	}

	
	while (true)
	{
		bool bErased = false;
		std::vector<GCutPt>::iterator it = CutLinePtList.begin();
		while(it != CutLinePtList.end() - 1)
		{
			// 인접한 절단점이 하나의 모서리에 존재한다면,
			if ((*it).vIdx[0] == (*(it + 1)).vIdx[1] && (*it).vIdx[1] == (*(it + 1)).vIdx[0])
			{
				// 두 절단점을 제거한다.
				it = CutLinePtList.erase(it);
				it = CutLinePtList.erase(it);
				bErased = true;
				std::cout << "Cut point is removed" << std::endl;

				if (it == CutLinePtList.end() || it + 1 == CutLinePtList.end())
					break;
				else
					continue;
			}
			it++;
		}

		if (!bErased)
			break;
	}	

	std::string Name = create_mesh_name("Cut Line");
	GCutLine *pCutLine = new GCutLine(Name, pBaseMesh->MC, CutLinePtList);
	pScene->AddMesh(pCutLine);
}
*/
void PCreator::CreateCutLineFromSketch()
{
	// 시작 삼각형과 교차점을 구하고, 없으면 리턴한다.
	GPoint3 CurrPt;
	GFace *pPrevFace = SelectVertOnFace(Sketch2DPtList[0][0], Sketch2DPtList[0][1], CurrPt);
	if (pPrevFace == NULL)
		return;

	// 스케치 점의 순서쌍 (Sketch2DPtList[i], Sketch2DPtList[i + 1])에 대하여,
	for (unsigned i = 0; i < Sketch2DPtList.size() - 1; ++i)
	{
		// 직선 (St, Ed)와 교차하는 모든 픽셀의 좌표를 구한다.
		std::vector<CPoint> PixelList;
		CPoint St(Sketch2DPtList[i][0], Sketch2DPtList[i][1]);
		CPoint Ed(Sketch2DPtList[i + 1][0], Sketch2DPtList[i + 1][1]);
		GetPixelList(St, Ed, PixelList);

		for (unsigned j = 0; j < PixelList.size(); ++j)
		{
			// 픽셀과 삼각형의 교차점을 계산한 후,
			GFace *pCurrFace = SelectVertOnFace(PixelList[j].x, PixelList[j].y, CurrPt);

			// 교차한 삼각형이 이전 교차 삼각형과 다른 경우,
			if (pCurrFace != NULL && pCurrFace != pPrevFace)
			{
				// 이전 선택된 삼각형과 현재 선택된 삼각형이 공유하는 정점의 인덱스를 찾는다.
				int sidx, eidx;
				int result = pCurrFace->GetSharedVertIdx(pPrevFace, sidx, eidx);

				// 하나의 정점만 공유한 경우,
				if (result == 0)
				{
					// 공유점을 구하여,
					GCutPt pt(pCurrFace, sidx, eidx, 0.0, pBaseMesh->Verts[sidx].P);

					// 절단 정점의 리스트에 추가한다.
					CutLinePtList.push_back(pt);
					std::cout << "메쉬 정점이 절단점으로 추가되었음" << std::endl;
				}

				// 현재 선택된 삼각형과 이전 삼각형이 인접하면,
				if (result == 1)
				{
					GPoint3 p0 = pBaseMesh->Verts[sidx].P;
					GPoint3 p1 = pBaseMesh->Verts[eidx].P;

					// 인접한 모서리의 직선의 방정식을 구하고,
					GLine l(p0, p1);

					// 현재 정점을 직선위로 투영하여, 새로운 절단 정점을 계산한 후,
					GVector3 v = p1 - p0;
					double t = ((CurrPt - p0) * v) / (v * v);
					GPoint3 q = p0 + t * v;

					// 절단 정점의 리스트에 추가한다.
					GCutPt pt = GCutPt(pCurrFace, sidx, eidx, t, q);

					// 절단 정점의 리스트에 추가한다.
					CutLinePtList.push_back(pt);
				}
				// 이전 삼각형을 갱신한다.
				pPrevFace = pCurrFace;
			}
		}
	}
	
	while (true)
	{
		bool bErased = false;
		std::vector<GCutPt>::iterator it = CutLinePtList.begin();
		while(it != CutLinePtList.end() - 1)
		{
			// 절단점이 메쉬 정점이 아니라면,
			if (it->t != 0.0)
			{
				// 인접한 절단점이 하나의 모서리에 존재한다면,
				if ((*it).vIdx[0] == (*(it + 1)).vIdx[1] && (*it).vIdx[1] == (*(it + 1)).vIdx[0])
				{
					// 두 절단점을 제거한다.
					it = CutLinePtList.erase(it);
					it = CutLinePtList.erase(it);
					bErased = true;
					std::cout << "한 모서리에 존재하는 절단점이 제거되었음" << std::endl;

					if (it == CutLinePtList.end() || it + 1 == CutLinePtList.end())
						break;
					else
						continue;
				}
			}
			it++;
		}

		if (!bErased)
			break;
	}	

	std::string Name = create_mesh_name("Cut Line");
	GCutLine *pCutLine = new GCutLine(Name, pBaseMesh->MC, CutLinePtList);
	pScene->AddMesh(pCutLine);
}

/*!
*	\brief 두 점을 지나는 직선과 교차하는 픽셀의 리스트를 계산한다.
*
*	\param p 직선의 시작점
*	\param q 직선의 끝점
*/
void PCreator::GetPixelList(CPoint p, CPoint q, std::vector<CPoint> &PixelList)
{
	bool bSwap = false;
	std::vector<CPoint> TmpList;
	double m = (double)(q.y - p.y) / (double)(q.x - p.x);
	if (ABS(m) < 1.0)
	{
		if (p.x > q.x)
		{
			SWAP(CPoint, p, q);
			bSwap = true;
		}
		for (int x = p.x; x <= q.x; x++)
		{
			int y = (int)(p.y + m * (x - p.x) + 0.5);
			TmpList.push_back(CPoint(x, y));
		}
	}
	else
	{
		if (p.y > q.y)
		{
			SWAP(CPoint, p, q);
			bSwap = true;
		}
		for (int y = p.y; y <= q.y; y++)
		{
			int x = (int)(p.x + 1.0 / m * (y - p.y) + 0.5);
			TmpList.push_back(CPoint(x, y));
		}
	}

	if (bSwap)
	{
		int NumPixel = (int)TmpList.size();
		for (int i = 0; i < NumPixel; ++i)
			PixelList.push_back(TmpList[NumPixel - i - 1]);
	}
	else
	{
		PixelList = TmpList;
	}	
}

void PCreator::CreatePtsOnMesh()
{
	pScene->PtsOnMesh.clear();
	pScene->PtsOnMesh = PtList;
	pDoc->UpdateAllViews(NULL);
}

void PCreator::Finish()
{
	switch (CreateType)
	{
		// 넙스 곡선을 생성한다면,
	case CREATE_CRV_CTL_PT:
	case CREATE_CRV_INT_PT:
		{
			CreateNurbsCrv();
			CreateCrvDlg.SendMessage(WM_COMMAND, IDOK);
		}		
		break;

		// 넙스 곡면을 생성한다면,
	case CREATE_SRF_CTL_PT:
		{
			CreateNurbsSrf();
			CreateSrfDlg.SendMessage(WM_COMMAND, IDOK);
		}
		break;

		// 넙스 구면을 생성한다면,
	case CREATE_NURBS_SPHERE:
		{
			CreateNurbsSphere();
		}
		break;

	case CREATE_POLYGON:
		{
			CreatePolygon();
			CreatePlyDlg.SendMessage(WM_COMMAND, IDOK);
		}
		break;

	case CREATE_PLANE:
		{
			CreatePolygonPlane();
			CreatePlnDlg.SendMessage(WM_COMMAND, IDOK);
		}		
		break;

	case CREATE_POLYGON_DISK:
		{
			CreatePolygonDisk();
		}		
		break;

	case CREATE_CIRCLE:
		{
			CreateNurbsCircle();
		}		
		break;

	case CREATE_NURBS_TEAPOT:
		{
			CreateNurbsTeapot();
		}
		break;

 	case CREATE_CUT_LINE:
		{
			CreateCutLine();
		}
 		break;

	case CREATE_POLYGON_SPHERE:
		{
			CreatePolygonSphere();
		}
		break;

	case CREATE_POLYGON_ICOSAHEDRON:
		{
			CreatePolygonIcosahedron();
		}
		break;

	case CREATE_POLYGON_TEAPOT:
		{
			CreatePolygonTeapot();			
		}		
		break;

	case CREATE_PTS_ON_MESH:
		{
			CreatePtsOnMesh();			
		}		
		break;

	case CREATE_GEODESIC_PATH:
	case CREATE_GEODESIC_MESH:
		Reset();		
		break;

	case CREATE_CRV_ON_MESH_FROM_SKETCH:
		{
			CreateCrvOnMeshFromSketch();			
		}		
		break;

	case CREATE_CUT_LINE_FROM_SKETCH:
		{
			CreateCutLineFromSketch();			
		}
		break;
	}

	// 부모 윈도우(GWorkSpaceDlg)에 종료를 알리는 메시지를 보낸다.
	HWND hWnd = CreateCrvDlg.GetParent()->GetSafeHwnd();
	::PostMessage(hWnd, WM_EXIT_CREATOR, 0, 0);

	Reset();
}

/*!
*	\brief 생성중인 객체의 모델링 좌표계를 계산한다.
*
*	\return 생성중인 객체의 모델링 좌표계를 반환한다.
*/
GTransf PCreator::GetMC()
{
	GTransf X;

	// 생성 객체의 타입에 따라, 모델링 좌표계를 계산하고,
	switch (CreateType)
	{
	case CREATE_CIRCLE:
	case CREATE_NURBS_SPHERE:
	case CREATE_NURBS_TEAPOT:
	case CREATE_POLYGON_SPHERE:
	case CREATE_POLYGON_ICOSAHEDRON:
	case CREATE_POLYGON_TEAPOT:
		{
			X = GTransf(GVector3(PtList[0][0], PtList[0][1], PtList[0][2]), GQuater());
		}		
		break;

	default:
		{
			GVector3 p = cast_vec3(PtList[0]);
			GVector3 q = cast_vec3(PtList[0]);
			int NumPt = (int)PtList.size();
			for (int i = 0; i < NumPt; ++i)
			{
				p[0] = MIN(p[0], PtList[i][0]);
				p[1] = MIN(p[1], PtList[i][1]);
				p[2] = MIN(p[2], PtList[i][2]);
				q[0] = MAX(q[0], PtList[i][0]);
				q[1] = MAX(q[1], PtList[i][1]);
				q[2] = MAX(q[2], PtList[i][2]);
			}
			X = GTransf((p + q) * 0.5, GQuater());
		}
	}

	// 선택된 정점들을 모델링 좌표계에서 표현하고, 계산된 모델링 좌표계를 반환한다.
	int NumPt = (int)PtList.size();
	for (int i = 0; i < NumPt; ++i)
	{
		GPoint3 p = cast_pt3(PtList[i]);
		p = inv(X) * p;
		PtList[i].Set(p[0], p[1], p[2], 1.0);
	}
	return X;
}

/*
* \brief 생성 중인 넙스 곡선을 렌더링 한다
*/
void PCreator::RenderNurbsCrv()
{
	// 조명을 비활성화 하고,
	glDisable(GL_LIGHTING);
	
	// 현재까지 입력된 제어점을 렌더링 한다.
	glColor3d(1.0, 0.0, 0.0);
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	int NumPt = (int)PtList.size();
	for (int i = 0; i < NumPt; ++i)
		glVertex3d(PtList[i][0], PtList[i][1], PtList[i][2]);
	glEnd();

	// 입력된 제어점을 연결하는 제어 다각형을 렌더링 한다.
	glColor3d(0.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < NumPt; ++i)
		glVertex3d(PtList[i][0], PtList[i][1], PtList[i][2]);
	glEnd();

	// 생성 중인 임시 곡선을 렌더링 한다.
	if (NumPt > CreateCrvDlg.m_Degree)
	{
		GNurbsCrv *pCrv = NULL;
		int p = CreateCrvDlg.m_Degree;
		int n = (int)PtList.size() - 1;
		double *U = NULL;
		bool rational = (CreateCrvDlg.m_TypeRational == 0) ? true : false;

		// 지정된 낫 벡터의 타입에 따라서, 임시 곡선을 생성하여,
		switch (CreateCrvDlg.m_TypeKnot)
		{
		case 0:	// clamped 타입이라면,
			{
				if (CreateType == CREATE_CRV_CTL_PT)
				{
					double *U = ::get_knot_uniform_clamped(p, n, 0.0, 1.0);
					pCrv = new GNurbsCrv("", GTransf(), p, n, U, &PtList[0], rational);
					delete [] U;
				}
				else if (CreateType == CREATE_CRV_INT_PT)
				{
					pCrv = ::get_gnurbs_crv_inter(p, n, &PtList[0], 0.0, 1.0, PARAM_CHORD_LEN, KNOT_AVERAGING);
				}
			}			
			break;

		case 1:	// unclamped 타입이라면,
			{
				if (CreateType == CREATE_CRV_CTL_PT)
				{
					double *U = ::get_knot_uniform_unclamped(p, n, 0.0, 1.0);
					pCrv = new GNurbsCrv("", GTransf(), p, n, U, &PtList[0], rational);
					delete [] U;
				}
				else if (CreateType == CREATE_CRV_INT_PT)
				{
					pCrv = ::get_gnurbs_crv_inter(p, n, &PtList[0], 0.0, 1.0, PARAM_CHORD_LEN, KNOT_AVERAGING);
				}
			}			
			break;

		case 2:	// periodic 타입이라면,
			{
				if (CreateType == CREATE_CRV_CTL_PT)
				{
					n = n + p;
					double *U = ::get_knot_uniform_unclamped(p, n, 0.0, 1.0);
					std::vector<GVector4> ExtPtList = PtList;
					for (int i = 0; i < p; ++i)
						ExtPtList.push_back(PtList[i]);
					pCrv = new GNurbsCrv("", GTransf(), p, n, U, &ExtPtList[0], rational);
					delete [] U;
				}
				else if (CreateType == CREATE_CRV_INT_PT)
				{
					pCrv = ::get_gnurbs_crv_inter_closed(p, n, &PtList[0], PARAM_CHORD_LEN, KNOT_NATURAL);
				}
			}
			break;
		}		

		// 생성된 임시 곡선을 렌더링 한다.
		int snum = 20 * n;
		double umin, umax;
		pCrv->GetDomain(&umin, &umax);

		glColor3f(0.0, 0.0, 1.0);
		glLineWidth(2.0);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < snum; ++i)
		{
			double u = umin + (umax - umin) * i / (double)(snum - 1);
			GVector4 pt = pCrv->Eval(u);
			glVertex3d(pt[0], pt[1], pt[2]);
		}
		glEnd();
		glLineWidth(1.0);

		// 생성된 임시 곡선을 제거한다.
		delete pCrv;
	}

	// 조명을 활성화 한다.
	glEnable(GL_LIGHTING);
}

/*!
*	\brief 생성 중인 넙스 곡선을 완성한다.
*/
void PCreator::CreateNurbsCrv()
{
	// 제어점의 마지막 인덱스가 차수보다 작으면 리턴한다.
	int p = CreateCrvDlg.m_Degree;
	int n = (int)PtList.size() - 1;
	if (n < p)
		return;

	// 생성될 곡선의 모델링 좌표계를 구하고,
	GTransf X = GetMC();
	GNurbsCrv *pCrv = NULL;
	bool rational = (CreateCrvDlg.m_TypeRational == 0) ? true : false;
	std::string name = (CStringA)(CreateCrvDlg.m_Name);

	// 낫 벡터와 보간 타입에 따라 곡선을 생성한다.
	switch (CreateCrvDlg.m_TypeKnot)
	{
	case 0:	// clamped
		{
			if (CreateType == CREATE_CRV_CTL_PT)
			{
				double *U = ::get_knot_uniform_clamped(p, n, 0.0, 1.0);
				pCrv = new GNurbsCrv(name, X, p, n, U, &PtList[0], rational);
				delete [] U;
			}
			else if (CreateType == CREATE_CRV_INT_PT)
			{
				pCrv = ::get_gnurbs_crv_inter(p, n, &PtList[0], 0.0, 1.0, PARAM_CHORD_LEN, KNOT_AVERAGING);
				pCrv->Name = name;
				pCrv->MC = X;
			}
		}		
		break;

	case 1:	// unclamped
		{
			if (CreateType == CREATE_CRV_CTL_PT)
			{
				double *U = ::get_knot_uniform_unclamped(p, n, 0.0, 1.0);
				pCrv = new GNurbsCrv(name, X, p, n, U, &PtList[0], rational);
				delete [] U;
			}
			else if (CreateType == CREATE_CRV_INT_PT)
			{
				pCrv = ::get_gnurbs_crv_inter(p, n, &PtList[0], 0.0, 1.0, PARAM_CHORD_LEN, KNOT_AVERAGING);
				pCrv->Name = name;
				pCrv->MC = X;
			}
		}		
		break;

	case 2:	// periodic
		{
			if (CreateType == CREATE_CRV_CTL_PT)
			{
				n = n + p;
				double *U = ::get_knot_uniform_unclamped(p, n, 0.0, 1.0);

				std::vector<GVector4> tmp = PtList;
				for (int i = 0; i < p; ++i)
					tmp.push_back(PtList[i]);
				pCrv = new GNurbsCrv(name, X, p, n, U, &tmp[0], rational);
				pCrv->closed = true;
			}
			else if (CreateType == CREATE_CRV_INT_PT)
			{
				pCrv = ::get_gnurbs_crv_inter_closed(p, n, &PtList[0], PARAM_CHORD_LEN, KNOT_NATURAL);
				pCrv->Name = name;
				pCrv->MC = X;
			}
		}
		break;
	}

	pCrv->FaceType = FACE_QUAD;
	pCrv->Renderer.SetSampleNum(20, 20 * n);
	pCrv->Renderer.RenderType = RENDER_POLYGON | RENDER_CTRL_PT;
	pCrv->Renderer.bBackFaceCull = TRUE;
	pCrv->Renderer.CtrlPtSize = (pScene->CtrlPtSize == 0.0) ? 0.5 : pScene->CtrlPtSize; 
	pScene->AddMesh(pCrv);
}

/*
* \brief 생성 중인 넙스 곡면을 렌더링 한다
*/
void PCreator::RenderNurbsSrf()
{
	if (PtList.empty())
		return;

	glDisable(GL_LIGHTING);
	glColor3d(1.0, 0.0, 0.0);
	glPointSize(5.0f);

	int p = CreateSrfDlg.m_SrfDegU;
	int q = CreateSrfDlg.m_SrfDegV;
	int m = CreateSrfDlg.m_NumCtlPtU - 1;
	int n = CreateSrfDlg.m_NumCtlPtV - 1;

	GVector4 **P;
	ARR_ALLOC_2D(P, GVector4, (m + 1), (n + 1));

	GVector4 st, ed;
	if (PtList.size() > 1)
	{
		st = PtList[0];
		ed = PtList[1];
	}
	else
	{
		st = ed = PtList[0];
	}

	P[0][0] = st;
	P[m][0].Set(st[0], ed[1], ed[2], 1.0);
	P[m][n] = ed;
	P[0][n].Set(ed[0], st[1], st[2], 1.0);

	GVector4 dU = P[m][0] - P[0][0];
	GVector4 dV = P[0][n] - P[0][0];
	for (int i = 0; i < m + 1; ++i)
	{
		double u = (double)i / (double)m;
		for (int j = 0; j < n + 1; ++j)
		{
			double v = (double)j / (double)n;
			P[i][j] = P[0][0] + u * dU + v * dV;
		}
	}

	// 입력된 제어점
	glBegin(GL_POINTS);
	for (int i = 0; i < m + 1; ++i)
	{
		for (int j = 0; j < n + 1; ++j)
		{
			glVertex3dv(P[i][j].V);
		}
	}
	glEnd();

	// 입력된 제어메쉬
	glColor3d(0.0, 1.0, 0.0);
	for (int i = 0; i < n + 1; i++)
	{
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j < m + 1; j++)
		{
			glVertex3d(P[j][i][0], P[j][i][1], P[j][i][2]);
		}
		glEnd();
	}
	glColor3d(0.0, 0.0, 1.0);
	for (int i = 0; i < m + 1; i++)
	{
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j < n + 1; j++)
		{
			glVertex3d(P[i][j][0], P[i][j][1], P[i][j][2]);
		}
		glEnd();
	}

	double *U = NULL, *V = NULL;
	switch (CreateSrfDlg.m_TypeKnotU)
	{
	case 0:	// clamped
		U = ::get_knot_uniform_clamped(p, m, 0.0, 1.0);
		break;

	case 1:	// unclamped
		U = ::get_knot_uniform_unclamped(p, m, 0.0, 1.0);
		break;
	}

	switch (CreateSrfDlg.m_TypeKnotV)
	{
	case 0:	// clamped
		V = ::get_knot_uniform_clamped(q, n, 0.0, 1.0);
		break;

	case 1:	// unclamped
		V = ::get_knot_uniform_unclamped(q, n, 0.0, 1.0);
		break;
	}

	bool rational = (CreateSrfDlg.m_TypeRational == 0) ? true : false;
	GNurbsSrf *pSrf = new GNurbsSrf("", GTransf(), p, m, U, q, n, V, P, rational);

	GVector4 pt[4];
	pt[0] = pSrf->Eval(0.0, 0.0);
	pt[1] = pSrf->Eval(1.0, 0.0);
	pt[2] = pSrf->Eval(1.0, 1.0);
	pt[3] = pSrf->Eval(0.0, 1.0);

	glColor3f(0.3, 0.3, 0.3);
	glBegin(GL_POLYGON);
	glVertex3dv(pt[0].V);
	glVertex3dv(pt[1].V);
	glVertex3dv(pt[2].V);
	glVertex3dv(pt[3].V);
	glEnd();

	delete [] U;
	delete [] V;
	delete pSrf;
	ARR_FREE_2D(P, (m + 1));

	glEnable(GL_LIGHTING);
}

/*!
*	\brief 생성 중인 넙스 곡면을 완성한다.
*/
void PCreator::CreateNurbsSrf()
{
	GTransf X = GetMC();

	int p = CreateSrfDlg.m_SrfDegU;
	int q = CreateSrfDlg.m_SrfDegV;
	int m = CreateSrfDlg.m_NumCtlPtU - 1;
	int n = CreateSrfDlg.m_NumCtlPtV - 1;

	GVector4 **P;
	ARR_ALLOC_2D(P, GVector4, (m + 1), (n + 1));

	GVector4 st = PtList[0];
	GVector4 ed = PtList[1];

	if ((ed[0] > st[0] && ed[2] < st[2]) || (ed[0] < st[0] && ed[2] > st[2])) // 1, 3-사분면
	{
		P[0][0] = st;
		P[m][0].Set(ed[0], st[1], st[2], 1.0);
		P[0][n].Set(st[0], ed[1], ed[2], 1.0);
		P[m][n] = ed;		
	}

	if ((ed[0] < st[0] && ed[2] < st[2]) || (ed[0] > st[0] && ed[2] > st[2]))	// 2, 4-사분면
	{
		P[0][0] = st;
		P[m][0].Set(st[0], ed[1], ed[2], 1.0);
		P[0][n].Set(ed[0], st[1], st[2], 1.0);		
		P[m][n] = ed;		
	}

	GVector4 dU = P[m][0] - P[0][0];
	GVector4 dV = P[0][n] - P[0][0];
	for (int i = 0; i < m + 1; ++i)
	{
		double u = (double)i / (double)m;
		for (int j = 0; j < n + 1; ++j)
		{
			double v = (double)j / (double)n;
			P[i][j] = P[0][0] + u * dU + v * dV;
		}
	}

	double *U = NULL, *V = NULL;
	switch (CreateSrfDlg.m_TypeKnotU)
	{
	case 0:	// clamped
		U = ::get_knot_uniform_clamped(p, m, 0.0, 1.0);
		break;

	case 1:	// unclamped
		U = ::get_knot_uniform_unclamped(p, m, 0.0, 1.0);
		break;
	}

	switch (CreateSrfDlg.m_TypeKnotV)
	{
	case 0:	// clamped
		V = ::get_knot_uniform_clamped(q, n, 0.0, 1.0);
		break;

	case 1:	// unclamped
		V = ::get_knot_uniform_unclamped(q, n, 0.0, 1.0);
		break;
	}

	std::string name = (CStringA)(CreateSrfDlg.m_SrfName);
	bool rational = (CreateSrfDlg.m_TypeRational == 0) ? true : false;

	GNurbsSrf *pSrf = new GNurbsSrf(name, X, p, m, U, q, n, V, P, rational);
	pSrf->Renderer.SetSampleNum(30, 30);
	pSrf->Renderer.RenderType |= RENDER_CTRL_PT;
	pSrf->Renderer.CtrlPtSize = (pScene->CtrlPtSize == 0.0) ? 0.5 : pScene->CtrlPtSize;
	pScene->AddMesh(pSrf);

	delete [] U;
	delete [] V;

	ARR_FREE_2D(P, (m + 1));
}


void PCreator::RenderPolygon()
{
	if (PtList.empty())
		return;

	int NumPt = (int)PtList.size();

	glDisable(GL_LIGHTING);
	glColor3d(1.0, 0.0, 0.0);
	glPointSize(5.0f);

	// 입력된 정점
	glBegin(GL_POINTS);
	for (int i = 0; i < NumPt; ++i)
		glVertex3d(PtList[i][0], PtList[i][1], PtList[i][2]);
	glEnd();

	// 입력된 정점의 라인
	glColor3d(0.7, 0.7, 0.7);
	glBegin(GL_TRIANGLE_FAN);
	{
		glVertex3d(PtList[0][0], PtList[0][1], PtList[0][2]);
		for (int i = 1; i < NumPt; ++i)
			glVertex3d(PtList[i][0], PtList[i][1], PtList[i][2]);
	}
	glEnd();

	glEnable(GL_LIGHTING);
}

void PCreator::CreatePolygon()
{
	GTransf X = GetMC();
	std::string name = (CStringA)(CreatePlyDlg.m_PlyName);
	GMesh *pMesh = new GMesh(name, X, FACE_TRI_FAN);

	int NumPt = (int)PtList.size();
	for (int i = 0; i < NumPt; ++i)
		pMesh->AddVertex(GVertex(cast_pt3(PtList[i]), GVector3()));

	pMesh->UpdateBndBox(true);
	GPoint3 p = pMesh->BBox.MinPt;
	GPoint3 q = pMesh->BBox.MaxPt;
	double len_x = q[0] - p[0];
	double len_z = q[2] - p[2];
	for (int i = 0; i < NumPt; ++i)
	{
		GPoint3 r = pMesh->Verts[i].P;
		double s = (r[0] - p[0]) / len_x;
		double t = 1.0 - (r[2] - p[2]) / len_z;

		pMesh->AddTexel(GVector2(s, t));
	}

	for (int i = 0; i < NumPt - 2; ++i)
		pMesh->AddFace(GFace(0, i + 1, i + 2, 0, i + 1, i + 2, -1));

	pScene->AddMesh(pMesh);
}


void PCreator::RenderPolygonPlane()
{
	if (PtList.empty())
		return;

	glDisable(GL_LIGHTING);
	glColor3d(1.0, 0.0, 0.0);
	glPointSize(5.0f);

	GPoint3 P[4];
	P[0] = cast_pt3(PtList[0]);
	P[1] = cast_pt3(PtList[0]);

	if (PtList.size() > 1)
	{
		P[2] = cast_pt3(PtList[1]);
		P[3] = cast_pt3(PtList[1]);
	}
	else
	{
		P[2] = cast_pt3(PtList[0]);
		P[3] = cast_pt3(PtList[0]);
	}

	P[1].V[2] = P[2].V[2];
	P[3].V[2] = P[0].V[2];

	// 평면의 양끝점
	glBegin(GL_POINTS);
	{
		glVertex3dv(P[0].V);
		glVertex3dv(P[2].V);
	}
	glEnd();

	// 생성중인 평면
	glColor3d(0.0, 1.0, 0.0);
	glBegin(GL_POLYGON);
	{
		glVertex3dv(P[0].V);
		glVertex3dv(P[1].V);
		glVertex3dv(P[2].V);
		glVertex3dv(P[3].V);
	}
	glEnd();

	glEnable(GL_LIGHTING);
}

void PCreator::CreatePolygonPlane()
{
	GTransf X = GetMC();

	GPoint3 P[4];
	P[0] = cast_pt3(PtList[0]);
	P[2] = cast_pt3(PtList[1]);
	
	if ((P[0][0] > P[2][0] && P[0][2] > P[2][2]) || (P[0][0] < P[2][0] && P[0][2] < P[2][2]))
	{
		P[1].Set(P[0][0], P[0][1], P[2][2]);
		P[3].Set(P[2][0], P[0][1], P[0][2]);
	}

	if ((P[0][0] > P[2][0] && P[0][2] < P[2][2]) || (P[0][0] < P[2][0] && P[0][2] > P[2][2]))
	{
		P[1].Set(P[2][0], P[0][1], P[0][2]);
		P[3].Set(P[0][0], P[0][1], P[2][2]);
	}
	
	std::string name = (CStringA)(CreatePlnDlg.m_Name);
	GMesh *pMesh = new GMesh(name, X, FACE_QUAD);
		
	pMesh->AddVertex(GVertex(P[0], GVector3()));
	pMesh->AddVertex(GVertex(P[1], GVector3()));
	pMesh->AddVertex(GVertex(P[2], GVector3()));
	pMesh->AddVertex(GVertex(P[3], GVector3()));

	pMesh->AddTexel(GVector2(0.0, 1.0));
	pMesh->AddTexel(GVector2(0.0, 0.0));
	pMesh->AddTexel(GVector2(1.0, 0.0));
	pMesh->AddTexel(GVector2(1.0, 1.0));

	pMesh->AddFace(GFace(0, 1, 2, 0, 1, 2, -1));
	pMesh->AddFace(GFace(0, 2, 3, 0, 2, 3, -1));

	pScene->AddMesh(pMesh);	
}

void PCreator::RenderPolygonDisk()
{
}

void PCreator::CreatePolygonDisk()
{
}

void PCreator::RenderNurbsSphere()
{
}

void PCreator::CreateNurbsSphere()
{
}

void PCreator::RenderNurbsTeapot()
{
	// 선택된 점이 없다면 리턴한다.
	if (PtList.empty())
		return;

	// 첫 번째 선택 점 P[0]과 현재 마우스 위치 P[1]를 구하고,
	GPoint3 P[2];
	P[0] = cast_pt3(PtList[0]);
	P[1] = (PtList.size() > 1) ? cast_pt3(PtList[1]) : cast_pt3(PtList[0]);

	// 두 점사이의 직선을 그리고,
	glDisable(GL_LIGHTING);
	glPointSize(3.0);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	{
		glVertex3dv(P[0].V);
		glVertex3dv(P[1].V);
		glEnd();
	}	
	glPointSize(1.0);

	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_LINES);
	{
		glVertex3dv(P[0].V);
		glVertex3dv(P[1].V);
	}
	glEnd();	
	glEnable(GL_LIGHTING);

	// 두 점 사이의 거리를 계산하여, 생성될 teapot 모델을 일시적으로 렌더링한다.
	double size = dist(P[0], P[1]);
	glPushMatrix();
	{
		glTranslatef(P[0][0], P[0][1] + 3.0 * size / 4.0, P[0][2]);
		glutWireTeapot(size);
	}	
	glPopMatrix();	
}

void PCreator::CreateNurbsTeapot()
{
	// Teapot 모델의 모델링 좌표계를 구하고,
	GTransf X = GetMC();

	// 선택된 두 점 사이의 거리를 계산하여,
	double size = dist(cast_pt3(PtList[0]), cast_pt3(PtList[1]));

	// Teapot 모델을 구성하는 곡면들을 생성하여 장면에 추가한다.
	std::vector<GNurbsSrf *> SrfList;
	::get_gnurbs_srf_teapot(SrfList, size * 0.5);
	for (unsigned i = 0; i < SrfList.size(); ++i)
	{
		SrfList[i]->MC = X;
		SrfList[i]->Renderer.SetSampleNum(20, 20);
		SrfList[i]->Renderer.CtrlPtSize = (pScene->CtrlPtSize == 0.0) ? 0.5 : pScene->CtrlPtSize;
		pScene->AddMesh(SrfList[i]);
	}
}

void PCreator::CreateCutLine()
{
	if (CutLinePtList.empty())
		return;

	// 시작점과 끝점을 제외하고 복사한다.
	std::vector<GCutPt> PtList;
	for (unsigned i = 1; i < CutLinePtList.size() - 1; ++i)
		PtList.push_back(CutLinePtList[i]);

	std::string name = create_mesh_name("cut_line");
	GCutLine *pCutLine = new GCutLine(name, pBaseMesh->MC, PtList);
	pScene->AddMesh(pCutLine);
}

void PCreator::RenderNurbsCircle()
{
	// 선택된 점이 없다면 리턴한다.
	if (PtList.empty())
		return;

	// 첫 번째 선택 점 P[0]과 현재 마우스 위치 P[1]를 구하고,
	GPoint3 P[2];
	P[0] = cast_pt3(PtList[0]);
	P[1] = (PtList.size() > 1) ? cast_pt3(PtList[1]) : cast_pt3(PtList[0]);

	// 두 점사이의 직선을 그리고,
	glDisable(GL_LIGHTING);
	glColor3d(1.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	{
		glPointSize(3.0);
		glVertex3dv(P[0].V);
		glVertex3dv(P[1].V);
		glPointSize(1.0);
	}
	glEnd();

	glColor3d(1.0, 1.0, 0.0);
	glBegin(GL_LINES);
	{
		glVertex3dv(P[0].V);
		glVertex3dv(P[1].V);
	}
	glEnd();

	// 두 점 사이의 거리를 계산하여,
	double rad = dist(P[0], P[1]);

	// 근사원을 생성하여 임시로 렌더링 한다.
	GNurbsCrv *pCircle = ::get_gnurbs_approx_circle(rad, 2, true);
	glPushMatrix();
	{
		glTranslatef(P[0][0], P[0][1], P[0][2]);
		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < 100; ++i)
		{
			double t = (double)i / 99.0;
			GVector4 pt = pCircle->Eval(t);
			glVertex3dv(pt.V);
		}
		glEnd();
	}
	glPopMatrix();
	delete pCircle;

	// 조명을 활성화 한다.
	glEnable(GL_LIGHTING);
}

void PCreator::CreateNurbsCircle()
{
	// 근사 원을 생성하여 장면에 추가한다.
	double size = dist(cast_pt3(PtList[0]), cast_pt3(PtList[1]));
	GNurbsCrv *pCircle = ::get_gnurbs_approx_circle(size, 2, true);
	pCircle->Name = create_mesh_name("Circle");
	pCircle->MC = GetMC();
	pCircle->Renderer.RenderType |= RENDER_CTRL_PT;
	pCircle->Renderer.SetSampleNum(20, 80);
	pCircle->Renderer.CtrlPtSize = (pScene->CtrlPtSize == 0.0) ? 0.5 : pScene->CtrlPtSize;
	pScene->AddMesh(pCircle);
}

void PCreator::RenderPolygonSphere()
{
	// 선택된 점이 없다면 리턴한다.
	if (PtList.empty())
		return;

	// 첫 번째 선택 점 P[0]과 현재 마우스 위치 P[1]를 구하고,
	GPoint3 P[2];
	P[0] = cast_pt3(PtList[0]);
	P[1] = (PtList.size() > 1) ? cast_pt3(PtList[1]) : cast_pt3(PtList[0]);

	// 두 점사이의 직선을 그리고,
	glDisable(GL_LIGHTING);
	glPointSize(3.0);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	{
		glVertex3dv(P[0].V);
		glVertex3dv(P[1].V);
		glEnd();
	}	
	glPointSize(1.0);

	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_LINES);
	{
		glVertex3dv(P[0].V);
		glVertex3dv(P[1].V);
	}
	glEnd();	
	glEnable(GL_LIGHTING);

	// 두 점 사이의 거리를 계산하여, 생성될 구를 일시적으로 렌더링한다.
	double size = dist(P[0], P[1]);
	glPushMatrix();
	{
		glTranslatef(P[0][0], P[0][1], P[0][2]);
		glutWireSphere(size, 30, 30);
	}	
	glPopMatrix();	
}

void PCreator::CreatePolygonSphere()
{
	// Sphere 모델의 모델링 좌표계를 구하고,
	GTransf X = GetMC();

	// 선택된 두 점 사이의 거리를 계산하여,
	double rad = dist(cast_pt3(PtList[0]), cast_pt3(PtList[1]));

	// 구를 생성하여 장면에 추가한다.
	std::string Name = create_mesh_name("Sphere");
	GMesh *pSphere = ::create_mesh_from_sphere(Name, X, rad, 30, 30);
	pScene->AddMesh(pSphere);
}

void PCreator::CreatePolygonIcosahedron()
{
	// Sphere 모델의 모델링 좌표계를 구하고,
	GTransf X = GetMC();

	// 선택된 두 점 사이의 거리를 계산하여,
	double rad = dist(cast_pt3(PtList[0]), cast_pt3(PtList[1]));

	// 구를 생성하여 장면에 추가한다.
	std::string Name = create_mesh_name("Sphere");
	GMesh *pSphere = ::create_mesh_from_icosahedron(Name, X, rad, 5);
	pScene->AddMesh(pSphere);
}

void PCreator::RenderPolygonTeapot()
{
	// 선택된 점이 없다면 리턴한다.
	if (PtList.empty())
		return;

	// 첫 번째 선택 점 P[0]과 현재 마우스 위치 P[1]를 구하고,
	GPoint3 P[2];
	P[0] = cast_pt3(PtList[0]);
	P[1] = (PtList.size() > 1) ? cast_pt3(PtList[1]) : cast_pt3(PtList[0]);

	// 두 점사이의 직선을 그리고,
	glDisable(GL_LIGHTING);
	glPointSize(3.0);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	{
		glVertex3dv(P[0].V);
		glVertex3dv(P[1].V);
		glEnd();
	}	
	glPointSize(1.0);

	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_LINES);
	{
		glVertex3dv(P[0].V);
		glVertex3dv(P[1].V);
	}
	glEnd();	
	glEnable(GL_LIGHTING);

	// 두 점 사이의 거리를 계산하여, 생성될 teapot 모델을 일시적으로 렌더링한다.
	double size = dist(P[0], P[1]);
	glPushMatrix();
	{
		glTranslatef(P[0][0], P[0][1] + 3.0 * size / 4.0, P[0][2]);
		glutWireTeapot(size);
	}	
	glPopMatrix();
}

void PCreator::CreatePolygonTeapot()
{
	// 선택된 두 점 사이의 거리를 계산하여,
	double size = dist(cast_pt3(PtList[0]), cast_pt3(PtList[1])) * 4.0;

	// Teapot을 생성하여 장면에 추가한다.
	std::string file_name = pScene->RootDir + "\\res\\models\\teapot.obj";
	::load_mesh_from_obj_file(pScene, file_name.c_str());

	GMesh *pMesh = pScene->pRecentMesh;
	pMesh->MC = GetMC();
	pMesh->Name = create_mesh_name("Teapot");
	pMesh->Edit(GTransf(), GVector3(), GQuater(), GVector3(size, size, size));
	pMesh->UpdateMesh();
}
