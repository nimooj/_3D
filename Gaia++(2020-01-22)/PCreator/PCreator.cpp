// PCreator.cpp : �ش� DLL�� �ʱ�ȭ ��ƾ�� �����մϴ�.
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
	// ���õ� ����Ʈ ����Ʈ�� �ʱ�ȭ �ϰ�, �ӽ÷� �� ���� �߰��Ѵ�.
	PtList.clear();
	if (CreateType != CREATE_PTS_ON_MESH)
		PtList.push_back(GVector4(0.0, 0.0, 0.0, 1.0));

	pBaseMesh = pMesh;
	pDoc->UpdateAllViews(NULL);
}

/*!
*	\brief ���� ���콺�� �������� 3���� ��ǥ�� ��ȯ�Ѵ�.
*
*	\param x ���콺�� X ��ǥ
*	\param y ���콺�� Y ��ǥ
*
*	\return ���콺 �������� 3���� ��ǥ�� ��ȯ�Ѵ�.
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
		// ������ ���ܼ� ������ ��� �����,
		CutLinePtList.clear();

		// ���� ���콺 �����Ϳ� ��ø�� �ﰢ���� �������� ����ϰ�,
		GPoint3 p;
		GFace *pFace = SelectVertOnFace(x, y, p);

		// �ﰢ���� ������ ���õǾ��ٸ�,
		if (pFace != NULL)
		{
			// ���ܼ� ����Ʈ�� ������ �߰��ϰ�,
			GCutPt pt(pFace, -1, -1, 0.0, p);
			CutLinePtList.push_back(pt);
			CutLinePtList.push_back(pt);
		}			
	}

	if (CreateType == CREATE_CUT_LINE_FROM_SKETCH)
	{
		// ������ ���ܼ� ������ ��� �����,
 		CutLinePtList.clear();

		// ������ ����ġ ������ �����.
		Sketch2DPtList.clear();
		Sketch3DPtList.clear();

		// 2D, 3D ����� ����Ʈ(������ ��)�� �������� �߰��Ѵ�.
		GVector3 v = GetWorldCoord(x, y);
		Sketch2DPtList.push_back(GVector2(x, y));
		Sketch3DPtList.push_back(v);
	}

	if (CreateType == CREATE_CRV_ON_MESH_FROM_SKETCH)
	{
		// ������ ����ġ ������ �����.
		Sketch2DPtList.clear();
		Sketch3DPtList.clear();

		// 2D, 3D ����ġ ������ ����Ʈ�� �߰��Ѵ�.
		GVector3 p = GetWorldCoord(x, y);
		Sketch2DPtList.push_back(GVector2(x, y));
		Sketch3DPtList.push_back(p);
	}
}

/*!
*	\brief ���� ���� ��ü Ÿ�Կ� ����, ���� �۾��� �����Ѵ�.
*
*	\param x ���콺�� X ��ǥ
*	\param y ���콺�� Y ��ǥ
*	\param flag ���콺�� ������ ��, ������ ��ư�� ���� (MK_LBUTTON, MK_RBUTTON, MK_MBUTTON)
*/
void PCreator::MouseMove(int x, int y, unsigned flag)
{
	switch(CreateType)
	{
		// ���� ���� ��ü�� ���ٸ� �����Ѵ�.
	case CREATE_INACTIVE:
		return;

		// ���� ���� ��ü�� ���ܼ� �̶��,
	case CREATE_CUT_LINE:
		MouseMoveForCutLine(x, y, flag);		
		break;

		// ���� ���� ��ü�� �޽����� �����̶��,
	case CREATE_PTS_ON_MESH:
		MouseMoveForPtsOnMesh(x, y, flag);
		break;

		// ���� ���� ��ü�� ������, ���� �޽����,
	case CREATE_GEODESIC_PATH:
	case CREATE_GEODESIC_MESH:
		SetMouseOverInfo(x, y, flag);
		break;

		// ���� ���� ��ü�� ZX ������� �����Ǵ� ��ü���,
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

		// ���� ���� ��ü�� ����ġ ����� ��ü���,
	case CREATE_CRV_ON_MESH_FROM_SKETCH:
	case CREATE_CUT_LINE_FROM_SKETCH:
		if (flag & MK_LBUTTON)
		{
			// 2D, 3D ����ġ ������ ����Ʈ�� �߰��Ѵ�.
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

	// ���ܼ��� ���� ���̶��,
	if (!CutLinePtList.empty())
	{
		// ���� ���콺 �����Ϳ� �ﰢ���� �������� ����� ��,
		GPoint3 CurrPt;
		GFace *pCurrFace = SelectVertOnFace(x, y, CurrPt);

		// ������ �ﰢ���� �ִٸ�,
		if (pCurrFace != NULL)
		{
			// ������ ������ ���� �������� �����Ѵ�.
			int LastIdx = (int)CutLinePtList.size() - 1;
			CutLinePtList[LastIdx].P = CurrPt;

			// ���õ� �ﰢ���� ����� ���,
			GFace *pPrevFace = CutLinePtList[LastIdx - 1].pFace;
			if (pCurrFace != pPrevFace)
			{
				// ���� �޽��� ���ϰ�,
				GMesh *pMesh = pCurrFace->pMesh;

				// �ٽ� ������ ���õ� �ﰢ������ ���ư� �����,
				if (LastIdx > 1 && pCurrFace == CutLinePtList[LastIdx - 2].pFace)
				{
					CutLinePtList.pop_back();
					CutLinePtList[LastIdx - 1].P = CurrPt;
					CutLinePtList[LastIdx - 1].pFace = pCurrFace;
					return;
				}

				// ���� ���õ� �ﰢ���� ���� ���õ� �ﰢ���� �����ϴ� ������ �ε����� ã�´�.
				int sidx, eidx;
				int result = pCurrFace->GetSharedVertIdx(pPrevFace, sidx, eidx);

				// �ϳ��� ������ ������ ���,
				if (result == 0)
				{
					// �������� ���Ͽ�,
					GCutPt pt(pCurrFace, sidx, eidx, 0.0, pMesh->Verts[sidx].P);

					// ���� ������ ����Ʈ�� �߰��Ѵ�.
					CutLinePtList[LastIdx] = pt;
					CutLinePtList.push_back(pt);
				}

				// ���� ���õ� �ﰢ���� ���� �ﰢ���� �����ϸ�,
				if (result == 1)
				{
					GPoint3 p0 = pMesh->Verts[sidx].P;
					GPoint3 p1 = pMesh->Verts[eidx].P;

					// ������ �𼭸��� ������ �������� ���ϰ�,
					GLine l(p0, p1);

					// ���� ������ �������� �����Ͽ�, ���ο� ���� ������ ����� ��,
					GVector3 v = p1 - p0;
					double t = ((CurrPt - p0) * v) / (v * v);
					GPoint3 q = p0 + t * v;

					// ���� ������ ����Ʈ�� �߰��Ѵ�.
					GCutPt pt = GCutPt(pCurrFace, sidx, eidx, t, q);

					// ���� ������ ����Ʈ�� �߰��Ѵ�.
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
		// ����Ʈ ��ȯ ������ ���ͼ�,
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		// ���� ���콺 ��ġ���� ���� ������ �ȼ� ���� �о,
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
	// ZX ����� �����ϰ�,
	static GPlane ZX(GVector3(0.0, 1.0, 0.0), GPoint3(0.0, 0.0, 0.0));

	// ���� ���콺 �������� ��ġ�� ������, ���� ����� ������ ������ ���Ͽ�,
	GLine line = pScene->MainCamera.GetOrthoRay(x, y);

	// ZX ������ �������� ����Ͽ�,
	GPoint3 r;
	::intersect_line_plane(r, line, ZX);
	if (GetKeyState(VK_SHIFT) < 0)
		r = round_off(r, -1);

	// ���õ� ���� ��ǥ�� �����Ѵ�.
	int idx = (int)PtList.size() - 1;
	PtList[idx].Set(r[0], r[1], r[2], 1.0);
}

/*!
*	\brief ���콺 �����Ϳ� �����ϴ� �ﰢ���� ���� ����� ������ �ε����� ã�´�.
*	\note �ֱ� ������(2017/02/06)
*
*	\param x ���콺�� x��ǥ
*	\param y ���콺�� y��ǥ
*	\param flag ���콺�� �ΰ����� (MK_LBUTTON, MK_RBUTTON, MK_MBUTTON)
*
*	\return ���콺�� ������ �ﰢ���� ������ true, ������ false�� ��ȯ�Ѵ�.
*/
bool PCreator::SetMouseOverInfo(int x, int y, unsigned flag)
{
	// ���õ� ���� ��ü�� ���ٸ� -1�� �����ϰ� �����Ѵ�.
	if (pBaseMesh == NULL)
	{
		MouseOverFaceIdx = -1;
		MouseOverVertIdx = -1;
		MouseOverPtCoords.Set(0.0, 0.0, 0.0);
		return false;
	}

	// ���� ���콺 ������ ��ġ���� ���� ������ �ȼ� ���� �о,
	GLubyte pixel[3];
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

	// �ﰢ���� ������ �ε���(1, 2, 3, ...)�� ���ϰ�,
	int RenderIdx = pixel[0] * 256 * 256 + pixel[1] * 256 + pixel[2];

	// ���õ� �ﰢ���� ���ٸ�,
	if (RenderIdx < 1 || RenderIdx > pBaseMesh->GetNumFace())
	{
		MouseOverFaceIdx = -1;
		MouseOverVertIdx = -1;
		MouseOverPtCoords.Set(0.0, 0.0, 0.0);
		return false;
	}

	// ���콺�� ������ �ﰢ���� �ε����� �����ϰ�,
	MouseOverFaceIdx = RenderIdx - 1;

	// ���콺�� ������ �ﰢ���� �� ������ ���Ͽ�,
	int vidx0 = pBaseMesh->Faces[MouseOverFaceIdx].vIdx[0];
	int vidx1 = pBaseMesh->Faces[MouseOverFaceIdx].vIdx[1];
	int vidx2 = pBaseMesh->Faces[MouseOverFaceIdx].vIdx[2];
	GVertex *v0 = &(pBaseMesh->Verts[vidx0]);
	GVertex *v1 = &(pBaseMesh->Verts[vidx1]);
	GVertex *v2 = &(pBaseMesh->Verts[vidx2]);

	// �� ������ ������ ��ũ������ ��ǥ�� ���ϰ�,
	GPoint3 p0, p1, p2;
	pScene->MainCamera.GetScreenCoord(v0, &p0.V[0], &p0.V[1], &p0.V[2]);
	pScene->MainCamera.GetScreenCoord(v1, &p1.V[0], &p1.V[1], &p1.V[2]);
	pScene->MainCamera.GetScreenCoord(v2, &p2.V[0], &p2.V[1], &p2.V[2]);

	// ���� ���콺 ��ġ�� �Ÿ��� ����Ͽ�, 
	GPoint3 q(x, y, 0.0);
	double dist0 = dist(q, p0);
	double dist1 = dist(q, p1);
	double dist2 = dist(q, p2);

	// ���콺 �����Ϳ� ���� ����� ������ �ε����� ã�´�.
	double min_dist = MIN(dist0, MIN(dist1, dist2));
	if (min_dist == dist0)
		MouseOverVertIdx = vidx0;
	else if (min_dist == dist1)
		MouseOverVertIdx = vidx1;
	else
		MouseOverVertIdx = vidx2;

	// ���� ��ǥ�迡�� ǥ���� �ﰢ���� ������ ��ǥ�� ���ϰ�,
	p0 = pBaseMesh->MC * pBaseMesh->Verts[vidx0].P;
	p1 = pBaseMesh->MC * pBaseMesh->Verts[vidx1].P;
	p2 = pBaseMesh->MC * pBaseMesh->Verts[vidx2].P;

	// ���� ���콺 ��ġ���� �信 ������ ������ �������� ���Ͽ�,
	GLine ray = pScene->MainCamera.GetOrthoRay(x, y);

	// ���� ���콺 �����Ϳ� ������ �ﰢ������ ���� Barycentric ��ǥ�� ����Ѵ�.
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
	// ��ü ���� ���� �ƴ϶�� �����Ѵ�.
	if (CreateType == CREATE_INACTIVE)
		return;	 

	// ��� ������ ���� ALT Ű�� ���� ��� �����Ѵ�.
	if ((GetAsyncKeyState(VK_MENU) & 0x8000))
		return;
	
	// ������ ���� �ߺ��Ͽ� �߰��Ѵ�.
	if (PtList.size() > 0)
	{
		int NumPt = (int)PtList.size();
		PtList.push_back(PtList[NumPt - 1]);
	}
	
	// ���� ���� ��ü�� Ÿ�Կ� ���� ������ �����Ѵ�.
	switch (CreateType)
	{
		// ������ �Է��� ����� �־�� �ϴ� ��ü���,
	case CREATE_CRV_CTL_PT:
	case CREATE_CRV_INT_PT:
	case CREATE_POLYGON:
		{
			// �����̽� Ű�� ���� ���·� ���� ���õǾ��ٸ�,
			if ((GetAsyncKeyState(VK_SPACE) & 0x8000))
			{
				// ������ ���� �����ϰ� ��� �����Ѵ�.
				PtList.pop_back();
				Finish();		
			}
		}
		break;

		// �� ���� �Է� �������� �����Ǵ� ��ü���,
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

		// ���콺 ��ư�� ���� ��, �����Ǵ� ��ü���, 
	case CREATE_CUT_LINE:
	case CREATE_CUT_LINE_FROM_SKETCH:
	case CREATE_PTS_ON_MESH:
	case CREATE_CRV_ON_MESH_FROM_SKETCH:
		{
			Finish();
		}		
		break;

		// ������ ��ü���� ���� ��θ� ���� ���̶��,
	case CREATE_GEODESIC_PATH:
		{
			// ���콺�� ������ ������ �ִٸ�,
			if (MouseOverVertIdx != -1)
			{
				// ��θ� �߰��ϰ�,
				GGeoPath *pPath = (GGeoPath *)pScene->pRecentMesh;
				pPath->AddPath(MouseOverVertIdx);
			}

			// �����̽� Ű�� ���� ���·� ���� ���õǾ��ٸ�, ��� ������ �����Ѵ�.
			if ((GetAsyncKeyState(VK_SPACE) & 0x8000))
				Finish();
		}
		break;

		// ������ ��ü���� ���� �޽��� ���� ���̶��,
	case CREATE_GEODESIC_MESH:
		{
			// ���콺�� ������ ������ �ִٸ�,
			if (MouseOverVertIdx != -1)
			{
				// Coons ����� �ڳ� ������ �߰��ϰ�,
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
*	\brief �귯�� ���� ��ǥ�踦 ����Ѵ�.
*
*	\param x ���콺�� x��ǥ
*	\param y ���콺�� y��ǥ
*	\param ispt ���콺 �����Ϳ� �ﰢ���� �������� �����
*
*	\return ���콺 �����Ϳ� �����ϴ� �ﰢ���� �����͸� ��ȯ�Ѵ�.
*/
GFace *PCreator::SelectVertOnFace(int x, int y, GPoint3 &ispt)
{
	if (pBaseMesh == NULL)
		return NULL;

	// ����Ʈ ��ȯ ������ ���ͼ�,
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// ���� ���콺 ��ġ���� ���� ������ �ȼ� ���� �о,
	GLubyte pixel[3];
	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

	// ���콺�� ��ø�� �ﰢ���� �ε����� ���ϰ�,
	int Idx = pixel[0] * 256 * 256 + pixel[1] * 256 + pixel[2] - 1;

	int NumFace = pBaseMesh->GetNumFace();
	if (Idx >= 0 && Idx < NumFace)
	{
		// ���� �޽����� �����Ǵ� �ﰢ���� ã��,
		GFace *pFace = &(pBaseMesh->Faces[Idx]);

		// ī�޶� ��ǥ�迡�� ǥ���� �ﰢ���� ������ ��ǥ�� ���ϰ�,
		int vidx0 = pFace->vIdx[0];
		int vidx1 = pFace->vIdx[1];
		int vidx2 = pFace->vIdx[2];
		GPoint3 p0 = pBaseMesh->MC * pBaseMesh->Verts[vidx0].P;
		GPoint3 p1 = pBaseMesh->MC * pBaseMesh->Verts[vidx1].P;
		GPoint3 p2 = pBaseMesh->MC * pBaseMesh->Verts[vidx2].P;

		// ���� ���콺 ��ġ���� �信 ������ ������ �������� ���Ͽ�,
		GLine ray = pScene->MainCamera.GetOrthoRay(x, y);
		
		// �ﰢ������ �������� ���Ͽ� �귯�� ���� ��ǥ�踦 ����Ѵ�.
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
*	\brief ���콺�� ������ ���� �޽��� ������ ������ �Ѵ�.
*/
void PCreator::RenderMouseOverVert()
{
	// ���ʿ��� ��� �����Ѵ�.
	if (pBaseMesh == NULL || MouseOverVertIdx == -1)
		return;

	// ����� ���� �׽�Ʈ�� ����,
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	// ���� ��ǥ�迡��
	glPushMatrix();
	{
		// ���� ��ü�� �𵨸� ��ǥ��� ��ȯ�ϰ�,
		glMultMatrixd(cast_mat(pBaseMesh->MC).M);

		// ���õ� ������ �׸���.
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
	glPopMatrix();		// ���� ��ǥ��� �����Ѵ�.

	// ����� ���� �׽�Ʈ�� �ٽ� �Ҵ�.
	glEnable(GL_LIGHTING);	
	glEnable(GL_DEPTH_TEST);
}

/*!
*	\brief �����ǰ� �ִ� �ִ� ��θ� �������Ѵ�.
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

	// Anti-aliasing ����� Ȱ��ȭ�ϰ�,
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// ���� ��ǥ�迡��
	glPushMatrix();
	{
		// ���� ��ü�� �𵨸� ��ǥ��� ��ȯ�ϰ�,
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
	glPopMatrix();		// ���� ��ǥ��� �����Ѵ�.

	// Anti-aliasing ��Ȱ��ȭ �Ѵ�		
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POINT_SMOOTH);
}

/*!
*	\brief �����ǰ� �ִ� Quad Layout�� �������Ѵ�.
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

	// Anti-aliasing ����� Ȱ��ȭ�ϰ�,
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// ���� ��ǥ�迡��
	glPushMatrix();
	{
		// ���� ��ü�� �𵨸� ��ǥ��� ��ȯ�ϰ�,
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
	glPopMatrix();		// ���� ��ǥ��� �����Ѵ�.

	// Anti-aliasing ��Ȱ��ȭ �Ѵ�		
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
}

/*!
*	\brief ���� ���� Coons �޽��� ��踦 ������ �Ѵ�.
*/
void PCreator::RenderCoonsBndry()
{
	// ���� ���� Coons �޽��� ���ϰ�,
	GCoonsMesh *pMesh = (GCoonsMesh *)pScene->pRecentMesh;
	if (pMesh->VertIdx.empty() || MouseOverVertIdx == -1)
		return;

	// �ӽ� ��θ� ������ ���� ������ �����ϰ�,
	std::vector<GPoint3> Path[4];

	// ���� ���õ� ������ �ε��� ������ ���ϰ�,
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

	// Anti-aliasing ����� Ȱ��ȭ�ϰ�,
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// ���� ��ǥ�迡��
	glPushMatrix();
	{
		// ���� ��ü�� �𵨸� ��ǥ��� ��ȯ�ϰ�,
		glMultMatrixd(cast_mat(pBaseMesh->MC).M);

		// Coons �޽��� ��踦 ������ �Ѵ�.
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
	glPopMatrix();		// ���� ��ǥ��� �����Ѵ�.

	// Anti-aliasing ��Ȱ��ȭ �Ѵ�		
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
}

/*!
*	\brief ���� �޽��� ������ �ﰢ�� ������ ���� ������ �Ѵ�.
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
	// ���� �޽��� ���õ��� �ʾҴٸ� �ٷ� �����Ѵ�.
	if (pBaseMesh == NULL)
		return;

	// ���� �޽����� �����Ǵ� ��ü���,
	 if (CreateType == CREATE_CUT_LINE || CreateType == CREATE_GEODESIC_PATH || 
		 CreateType == CREATE_GEODESIC_MESH || CreateType == CREATE_CRV_ON_MESH_FROM_SKETCH ||
		 CreateType == CREATE_CUT_LINE_FROM_SKETCH)
	 {
		 // ���� ���۸� �ʱ�ȭ ����,
		 glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		 // ����� ���� ���� �� ���� ��� ����
		 glMatrixMode(GL_PROJECTION);
		 glLoadIdentity();	
		 pScene->SetupViewFrustum();

		 // ����� ���� ��ȯ�� ����
		 glMatrixMode(GL_MODELVIEW);
		 glLoadIdentity();
		 pScene->SetupViewTransform();

		 // ��� ������ ���� ����� ����,
		 glDisable(GL_LIGHTING);
		 glDisable(GL_FOG);
		 glDisable(GL_TEXTURE_2D);

		 glPushMatrix();
		 {
			 // ���� ��ǥ�迡�� �޽� ��ǥ����� ��ȯ�ϰ�,
			 glMultMatrixd(cast_mat(pBaseMesh->MC).M);

			 // ������ �ﰢ���� ������ ���ϰ�,
			 int NumVert = pBaseMesh->GetNumVert();
			 int NumFace = pBaseMesh->GetNumFace();

			 // ������ ���̵� �ʱ�ȭ �ϰ�,
			 unsigned int r = 0, g = 0, b = 0;

			 // �ﰢ���� ���� ���̵�� ������ �Ѵ�.
			 glBegin(GL_TRIANGLES);
			 for (int i = 0; i < NumFace; ++i)
			 {
				 int vidx0 = pBaseMesh->Faces[i].vIdx[0];
				 int vidx1 = pBaseMesh->Faces[i].vIdx[1];
				 int vidx2 = pBaseMesh->Faces[i].vIdx[2];

				 GetColorId(r, g, b);	// (0, 0, 1) ���� ������.
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

		// ���� ����
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

		// ���� ��
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

		// ���� �޽��� ���ϰ�,
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

	// ������ ���� ��� ����� ��輱�� ������ �����Ѵ�.
	glDisable(GL_LIGHTING);
	glColor3f(0.2627, 1.0, 0.64);
	glLineWidth(4.0);

	// ���� �ð��� ���� ������ ǥ���Ѵ�.
	glBegin(GL_LINE_STRIP);
	{
		for (unsigned i = 0; i < Sketch3DPtList.size(); ++i)
			glVertex3f(Sketch3DPtList[i][0], Sketch3DPtList[i][1], Sketch3DPtList[i][2]);
	}			
	glEnd();

	// ������ Ȱ��ȭ�ϰ� ��輱�� ������ ��Ȱ��ȭ �Ѵ�.
	glEnable(GL_LIGHTING);
	glLineWidth(1.0f);
}

void PCreator::CreateCrvOnMeshFromSketch()
{
	// �޽��� �������� �����ϴ� ����Ʈ ������ �����ϰ�,
	std::vector<GPoint3> IsPtList;

	// ù ��° �������� �����Ѵ�.
	if (SetMouseOverInfo(Sketch2DPtList[0][0], Sketch2DPtList[0][1], 0))
	{
		int *vidx = pBaseMesh->Faces[MouseOverFaceIdx].vIdx;
		GPoint3 p0 = pBaseMesh->MC * pBaseMesh->Verts[vidx[0]].P;
		GPoint3 p1 = pBaseMesh->MC * pBaseMesh->Verts[vidx[1]].P;
		GPoint3 p2 = pBaseMesh->MC * pBaseMesh->Verts[vidx[2]].P;
		GPoint3 pt = p2 + (p0 - p2) * MouseOverPtCoords[0] + (p1 - p2) * MouseOverPtCoords[1];
		IsPtList.push_back(pt);
	}

	// ����ġ�� �� ������ �����鿡 ���Ͽ�,
	int NumPt = (int)Sketch2DPtList.size();
	for (int i = 0; i < NumPt - 1; ++i)
	{
		// �� �� St�� Ed�� �����ϴ� ��� �ȼ��� ��ǥ�� ���Ѵ�.
		std::vector<CPoint> PixelList;
		CPoint St(Sketch2DPtList[i][0], Sketch2DPtList[i][1]);
		CPoint Ed(Sketch2DPtList[i + 1][0], Sketch2DPtList[i + 1][1]);
		GetPixelList(St, Ed, PixelList);

		GFace *pPrevFace = NULL;
		for (unsigned j = 0; j < PixelList.size(); ++j)
		{
			// �� �ȼ��� ��ġ���� ���� ������ �ȼ� ���� �о,
			GLubyte color[3];
			GLint viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport);
			glReadPixels(PixelList[j].x, viewport[3] - PixelList[j].y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, color);

			// ���õ� �ﰢ���� ���ϰ�, ���ٸ� ���� �ȼ��� �Ѿ��.
			int CurrFaceIdx = color[0] * 256 * 256 + color[1] * 256 + color[2] - 1;
			if (CurrFaceIdx < 0 || CurrFaceIdx > pBaseMesh->GetNumFace() - 1)
				continue;

			// ���õ� �ﰢ���� ���� �ﰢ���� �ٸ� �ﰢ���̶��,
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
			// ���� �ﰢ���� �����Ѵ�.
			pPrevFace = pCurrFace;
		}
	}

	// ������ �������� �����Ѵ�.
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

	// �������� �ٻ��ϴ� 3�� B-spline ��� �����Ѵ�.
	int n = MAX(3, IsPtList.size() / 5);
	GNurbsCrv *pCrv = ::get_gnurbs_crv_approx(3, n, IsPtList, PARAM_CHORD_LEN, KNOT_APPROX);
	pCrv->RefMeshName = pBaseMesh->Name;
	pCrv->Name = create_mesh_name("Sketch Curve");
	//pCrv->Renderer.RenderType |= RENDER_CTRL_PT;
	pCrv->Renderer.SetSampleNum(20, pCrv->n * 10);
	double size = pBaseMesh->BBox.GetDiagonal() * 0.003;
	pCrv->Renderer.CtrlPtSize = (pScene->CtrlPtSize == 0.0) ? size : pScene->CtrlPtSize;
	
	// ������ ��� ���� ���� ������ �������� ���Ѵ�.
	GPoint3 P0 = IsPtList[0];
	GPoint3 Pn = IsPtList[IsPtList.size() - 1];

	// ��鿡 ���Ե� ��� �޽��� ���ϰ�,
	std::vector<GMesh *> MeshList;
	GetPreOrderMeshList(pScene, MeshList, NULL);

	// ������ ��� ���� ���� �� ���� ���� ����� ��� ������ ������ �����Ѵ�.
	GNurbsCrv *pStProxiCrv, *pEdProxiCrv;
	double StParam, EdParam;
	double StMinDist = pCrv->Renderer.CtrlPtSize * 3.0;
	double EdMinDist = pCrv->Renderer.CtrlPtSize * 3.0;
	bool StFlag = false;
	bool EdFlag = false;

	// ����� ��� � ��ü�� ���Ͽ�,
	int NumMesh = (int)MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
	{
		GMesh *pMesh = MeshList[i];
		double d, tmp;
		if (pMesh->GetClassName() == "GNurbsCrv")
		{
			GNurbsCrv *pOtherCrv = (GNurbsCrv *)pMesh;
			// ��� ���� ���� ���� ����� �ٸ� ����� ���� ã�´�.
			d = ::get_min_dist(P0, pOtherCrv, &tmp);
			if (d < StMinDist)
			{
				pStProxiCrv = pOtherCrv;
				StMinDist = d;
				StParam = tmp;
				StFlag = true;
			}

			// ��� �� ���� ���� ����� �ٸ� ����� ���� ã�´�.
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

	// ��ü�� ��鿡 �߰��Ѵ�.
	pScene->AddMesh(pCrv);
}

/*!
*	\brief ���� ���� ���ܼ��� ������ �Ѵ�.
*/
void PCreator::RenderCutLineFromSketch()
{
	if (Sketch3DPtList.empty())
		return;

	// ������ ���� ��� ����� ��輱�� ������ �����Ѵ�.
	glDisable(GL_LIGHTING);
	glColor3f(0.2627, 1.0, 0.64);
	glLineWidth(4.0);

	// ���� �ð��� ���� ������ ǥ���Ѵ�.
	glBegin(GL_LINE_STRIP);
	{
		for (unsigned i = 0; i < Sketch3DPtList.size(); ++i)
			glVertex3f(Sketch3DPtList[i][0], Sketch3DPtList[i][1], Sketch3DPtList[i][2]);
	}			
	glEnd();

	// ������ Ȱ��ȭ�ϰ� ��輱�� ������ ��Ȱ��ȭ �Ѵ�.
	glEnable(GL_LIGHTING);
	glLineWidth(1.0f);
}

/*!
*	\brief ���� ���� ���ܼ��� �ϼ��Ѵ�.
*/
/*
void PCreator::CreateCutLineFromSketch()
{
	// ���� �ﰢ���� �������� ���ϰ�, ������ �����Ѵ�.
	GPoint3 CurrPt;
	GFace *pPrevFace = SelectVertOnFace(Sketch2DPtList[0][0], Sketch2DPtList[0][1], CurrPt);
	if (pPrevFace == NULL)
		return;

	std::vector<GPoint3> Pts;
	// ����ġ ���� ������ (Sketch2DPtList[i], Sketch2DPtList[i + 1])�� ���Ͽ�,
	for (unsigned i = 0; i < Sketch2DPtList.size() - 1; ++i)
	{
		// ���� (St, Ed)�� �����ϴ� ��� �ȼ��� ��ǥ�� ���Ѵ�.
		std::vector<CPoint> PixelList;
		CPoint St(Sketch2DPtList[i][0], Sketch2DPtList[i][1]);
		CPoint Ed(Sketch2DPtList[i + 1][0], Sketch2DPtList[i + 1][1]);
		GetPixelList(St, Ed, PixelList);

		for (unsigned j = 0; j < PixelList.size(); ++j)
		{
			// �ȼ��� �ﰢ���� �������� ����� ��,
			GFace *pCurrFace = SelectVertOnFace(PixelList[j].x, PixelList[j].y, CurrPt);

			// ������ �ﰢ���� ���� ���� �ﰢ���� �ٸ� ���,
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

		// ���� (St, Ed)�� �����ϴ� ��� �ȼ��� ��ǥ�� ���Ѵ�.
		std::vector<CPoint> PixelList;
		CPoint St(p0[0], p0[1]);
		CPoint Ed(p1[0], p1[1]);
		GetPixelList(St, Ed, PixelList);

		for (unsigned j = 0; j < PixelList.size(); ++j)
		{
			// �ȼ��� �ﰢ���� �������� ����� ��,
			GFace *pCurrFace = SelectVertOnFace(PixelList[j].x, PixelList[j].y, CurrPt);

			// ������ �ﰢ���� ���� ���� �ﰢ���� �ٸ� ���,
			if (pCurrFace != NULL && pCurrFace != pPrevFace)
			{
				// ���� ���õ� �ﰢ���� ���� ���õ� �ﰢ���� �����ϴ� ������ �ε����� ã�´�.
				int sidx, eidx;
				int result = pCurrFace->GetSharedVertIdx(pPrevFace, sidx, eidx);

				// �ϳ��� ������ ������ ���,
				if (result == 0)
				{
					// �������� ���Ͽ�,
					GCutPt pt(pCurrFace, sidx, eidx, 0.0, pBaseMesh->Verts[sidx].P);

					// ���� ������ ����Ʈ�� �߰��Ѵ�.
					CutLinePtList.push_back(pt);

					std::cout << "Cut point on tiangle's vertex is found" << std::endl;
				}

				// ���� ���õ� �ﰢ���� ���� �ﰢ���� �����ϸ�,
				if (result == 1)
				{
					GPoint3 p0 = pBaseMesh->Verts[sidx].P;
					GPoint3 p1 = pBaseMesh->Verts[eidx].P;

					// ������ �𼭸��� ������ �������� ���ϰ�,
					GLine l(p0, p1);

					// ���� ������ �������� �����Ͽ�, ���ο� ���� ������ ����� ��,
					GVector3 v = p1 - p0;
					double t = ((CurrPt - p0) * v) / (v * v);
					GPoint3 q = p0 + t * v;

					// ���� ������ ����Ʈ�� �߰��Ѵ�.
					GCutPt pt = GCutPt(pCurrFace, sidx, eidx, t, q);

					// ���� ������ ����Ʈ�� �߰��Ѵ�.
					CutLinePtList.push_back(pt);
				}
				// ���� �ﰢ���� �����Ѵ�.
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
			// ������ �������� �ϳ��� �𼭸��� �����Ѵٸ�,
			if ((*it).vIdx[0] == (*(it + 1)).vIdx[1] && (*it).vIdx[1] == (*(it + 1)).vIdx[0])
			{
				// �� �������� �����Ѵ�.
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
	// ���� �ﰢ���� �������� ���ϰ�, ������ �����Ѵ�.
	GPoint3 CurrPt;
	GFace *pPrevFace = SelectVertOnFace(Sketch2DPtList[0][0], Sketch2DPtList[0][1], CurrPt);
	if (pPrevFace == NULL)
		return;

	// ����ġ ���� ������ (Sketch2DPtList[i], Sketch2DPtList[i + 1])�� ���Ͽ�,
	for (unsigned i = 0; i < Sketch2DPtList.size() - 1; ++i)
	{
		// ���� (St, Ed)�� �����ϴ� ��� �ȼ��� ��ǥ�� ���Ѵ�.
		std::vector<CPoint> PixelList;
		CPoint St(Sketch2DPtList[i][0], Sketch2DPtList[i][1]);
		CPoint Ed(Sketch2DPtList[i + 1][0], Sketch2DPtList[i + 1][1]);
		GetPixelList(St, Ed, PixelList);

		for (unsigned j = 0; j < PixelList.size(); ++j)
		{
			// �ȼ��� �ﰢ���� �������� ����� ��,
			GFace *pCurrFace = SelectVertOnFace(PixelList[j].x, PixelList[j].y, CurrPt);

			// ������ �ﰢ���� ���� ���� �ﰢ���� �ٸ� ���,
			if (pCurrFace != NULL && pCurrFace != pPrevFace)
			{
				// ���� ���õ� �ﰢ���� ���� ���õ� �ﰢ���� �����ϴ� ������ �ε����� ã�´�.
				int sidx, eidx;
				int result = pCurrFace->GetSharedVertIdx(pPrevFace, sidx, eidx);

				// �ϳ��� ������ ������ ���,
				if (result == 0)
				{
					// �������� ���Ͽ�,
					GCutPt pt(pCurrFace, sidx, eidx, 0.0, pBaseMesh->Verts[sidx].P);

					// ���� ������ ����Ʈ�� �߰��Ѵ�.
					CutLinePtList.push_back(pt);
					std::cout << "�޽� ������ ���������� �߰��Ǿ���" << std::endl;
				}

				// ���� ���õ� �ﰢ���� ���� �ﰢ���� �����ϸ�,
				if (result == 1)
				{
					GPoint3 p0 = pBaseMesh->Verts[sidx].P;
					GPoint3 p1 = pBaseMesh->Verts[eidx].P;

					// ������ �𼭸��� ������ �������� ���ϰ�,
					GLine l(p0, p1);

					// ���� ������ �������� �����Ͽ�, ���ο� ���� ������ ����� ��,
					GVector3 v = p1 - p0;
					double t = ((CurrPt - p0) * v) / (v * v);
					GPoint3 q = p0 + t * v;

					// ���� ������ ����Ʈ�� �߰��Ѵ�.
					GCutPt pt = GCutPt(pCurrFace, sidx, eidx, t, q);

					// ���� ������ ����Ʈ�� �߰��Ѵ�.
					CutLinePtList.push_back(pt);
				}
				// ���� �ﰢ���� �����Ѵ�.
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
			// �������� �޽� ������ �ƴ϶��,
			if (it->t != 0.0)
			{
				// ������ �������� �ϳ��� �𼭸��� �����Ѵٸ�,
				if ((*it).vIdx[0] == (*(it + 1)).vIdx[1] && (*it).vIdx[1] == (*(it + 1)).vIdx[0])
				{
					// �� �������� �����Ѵ�.
					it = CutLinePtList.erase(it);
					it = CutLinePtList.erase(it);
					bErased = true;
					std::cout << "�� �𼭸��� �����ϴ� �������� ���ŵǾ���" << std::endl;

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
*	\brief �� ���� ������ ������ �����ϴ� �ȼ��� ����Ʈ�� ����Ѵ�.
*
*	\param p ������ ������
*	\param q ������ ����
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
		// �ҽ� ��� �����Ѵٸ�,
	case CREATE_CRV_CTL_PT:
	case CREATE_CRV_INT_PT:
		{
			CreateNurbsCrv();
			CreateCrvDlg.SendMessage(WM_COMMAND, IDOK);
		}		
		break;

		// �ҽ� ����� �����Ѵٸ�,
	case CREATE_SRF_CTL_PT:
		{
			CreateNurbsSrf();
			CreateSrfDlg.SendMessage(WM_COMMAND, IDOK);
		}
		break;

		// �ҽ� ������ �����Ѵٸ�,
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

	// �θ� ������(GWorkSpaceDlg)�� ���Ḧ �˸��� �޽����� ������.
	HWND hWnd = CreateCrvDlg.GetParent()->GetSafeHwnd();
	::PostMessage(hWnd, WM_EXIT_CREATOR, 0, 0);

	Reset();
}

/*!
*	\brief �������� ��ü�� �𵨸� ��ǥ�踦 ����Ѵ�.
*
*	\return �������� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*/
GTransf PCreator::GetMC()
{
	GTransf X;

	// ���� ��ü�� Ÿ�Կ� ����, �𵨸� ��ǥ�踦 ����ϰ�,
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

	// ���õ� �������� �𵨸� ��ǥ�迡�� ǥ���ϰ�, ���� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
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
* \brief ���� ���� �ҽ� ��� ������ �Ѵ�
*/
void PCreator::RenderNurbsCrv()
{
	// ������ ��Ȱ��ȭ �ϰ�,
	glDisable(GL_LIGHTING);
	
	// ������� �Էµ� �������� ������ �Ѵ�.
	glColor3d(1.0, 0.0, 0.0);
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	int NumPt = (int)PtList.size();
	for (int i = 0; i < NumPt; ++i)
		glVertex3d(PtList[i][0], PtList[i][1], PtList[i][2]);
	glEnd();

	// �Էµ� �������� �����ϴ� ���� �ٰ����� ������ �Ѵ�.
	glColor3d(0.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < NumPt; ++i)
		glVertex3d(PtList[i][0], PtList[i][1], PtList[i][2]);
	glEnd();

	// ���� ���� �ӽ� ��� ������ �Ѵ�.
	if (NumPt > CreateCrvDlg.m_Degree)
	{
		GNurbsCrv *pCrv = NULL;
		int p = CreateCrvDlg.m_Degree;
		int n = (int)PtList.size() - 1;
		double *U = NULL;
		bool rational = (CreateCrvDlg.m_TypeRational == 0) ? true : false;

		// ������ �� ������ Ÿ�Կ� ����, �ӽ� ��� �����Ͽ�,
		switch (CreateCrvDlg.m_TypeKnot)
		{
		case 0:	// clamped Ÿ���̶��,
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

		case 1:	// unclamped Ÿ���̶��,
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

		case 2:	// periodic Ÿ���̶��,
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

		// ������ �ӽ� ��� ������ �Ѵ�.
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

		// ������ �ӽ� ��� �����Ѵ�.
		delete pCrv;
	}

	// ������ Ȱ��ȭ �Ѵ�.
	glEnable(GL_LIGHTING);
}

/*!
*	\brief ���� ���� �ҽ� ��� �ϼ��Ѵ�.
*/
void PCreator::CreateNurbsCrv()
{
	// �������� ������ �ε����� �������� ������ �����Ѵ�.
	int p = CreateCrvDlg.m_Degree;
	int n = (int)PtList.size() - 1;
	if (n < p)
		return;

	// ������ ��� �𵨸� ��ǥ�踦 ���ϰ�,
	GTransf X = GetMC();
	GNurbsCrv *pCrv = NULL;
	bool rational = (CreateCrvDlg.m_TypeRational == 0) ? true : false;
	std::string name = (CStringA)(CreateCrvDlg.m_Name);

	// �� ���Ϳ� ���� Ÿ�Կ� ���� ��� �����Ѵ�.
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
* \brief ���� ���� �ҽ� ����� ������ �Ѵ�
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

	// �Էµ� ������
	glBegin(GL_POINTS);
	for (int i = 0; i < m + 1; ++i)
	{
		for (int j = 0; j < n + 1; ++j)
		{
			glVertex3dv(P[i][j].V);
		}
	}
	glEnd();

	// �Էµ� ����޽�
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
*	\brief ���� ���� �ҽ� ����� �ϼ��Ѵ�.
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

	if ((ed[0] > st[0] && ed[2] < st[2]) || (ed[0] < st[0] && ed[2] > st[2])) // 1, 3-��и�
	{
		P[0][0] = st;
		P[m][0].Set(ed[0], st[1], st[2], 1.0);
		P[0][n].Set(st[0], ed[1], ed[2], 1.0);
		P[m][n] = ed;		
	}

	if ((ed[0] < st[0] && ed[2] < st[2]) || (ed[0] > st[0] && ed[2] > st[2]))	// 2, 4-��и�
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

	// �Էµ� ����
	glBegin(GL_POINTS);
	for (int i = 0; i < NumPt; ++i)
		glVertex3d(PtList[i][0], PtList[i][1], PtList[i][2]);
	glEnd();

	// �Էµ� ������ ����
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

	// ����� �糡��
	glBegin(GL_POINTS);
	{
		glVertex3dv(P[0].V);
		glVertex3dv(P[2].V);
	}
	glEnd();

	// �������� ���
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
	// ���õ� ���� ���ٸ� �����Ѵ�.
	if (PtList.empty())
		return;

	// ù ��° ���� �� P[0]�� ���� ���콺 ��ġ P[1]�� ���ϰ�,
	GPoint3 P[2];
	P[0] = cast_pt3(PtList[0]);
	P[1] = (PtList.size() > 1) ? cast_pt3(PtList[1]) : cast_pt3(PtList[0]);

	// �� �������� ������ �׸���,
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

	// �� �� ������ �Ÿ��� ����Ͽ�, ������ teapot ���� �Ͻ������� �������Ѵ�.
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
	// Teapot ���� �𵨸� ��ǥ�踦 ���ϰ�,
	GTransf X = GetMC();

	// ���õ� �� �� ������ �Ÿ��� ����Ͽ�,
	double size = dist(cast_pt3(PtList[0]), cast_pt3(PtList[1]));

	// Teapot ���� �����ϴ� ������ �����Ͽ� ��鿡 �߰��Ѵ�.
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

	// �������� ������ �����ϰ� �����Ѵ�.
	std::vector<GCutPt> PtList;
	for (unsigned i = 1; i < CutLinePtList.size() - 1; ++i)
		PtList.push_back(CutLinePtList[i]);

	std::string name = create_mesh_name("cut_line");
	GCutLine *pCutLine = new GCutLine(name, pBaseMesh->MC, PtList);
	pScene->AddMesh(pCutLine);
}

void PCreator::RenderNurbsCircle()
{
	// ���õ� ���� ���ٸ� �����Ѵ�.
	if (PtList.empty())
		return;

	// ù ��° ���� �� P[0]�� ���� ���콺 ��ġ P[1]�� ���ϰ�,
	GPoint3 P[2];
	P[0] = cast_pt3(PtList[0]);
	P[1] = (PtList.size() > 1) ? cast_pt3(PtList[1]) : cast_pt3(PtList[0]);

	// �� �������� ������ �׸���,
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

	// �� �� ������ �Ÿ��� ����Ͽ�,
	double rad = dist(P[0], P[1]);

	// �ٻ���� �����Ͽ� �ӽ÷� ������ �Ѵ�.
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

	// ������ Ȱ��ȭ �Ѵ�.
	glEnable(GL_LIGHTING);
}

void PCreator::CreateNurbsCircle()
{
	// �ٻ� ���� �����Ͽ� ��鿡 �߰��Ѵ�.
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
	// ���õ� ���� ���ٸ� �����Ѵ�.
	if (PtList.empty())
		return;

	// ù ��° ���� �� P[0]�� ���� ���콺 ��ġ P[1]�� ���ϰ�,
	GPoint3 P[2];
	P[0] = cast_pt3(PtList[0]);
	P[1] = (PtList.size() > 1) ? cast_pt3(PtList[1]) : cast_pt3(PtList[0]);

	// �� �������� ������ �׸���,
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

	// �� �� ������ �Ÿ��� ����Ͽ�, ������ ���� �Ͻ������� �������Ѵ�.
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
	// Sphere ���� �𵨸� ��ǥ�踦 ���ϰ�,
	GTransf X = GetMC();

	// ���õ� �� �� ������ �Ÿ��� ����Ͽ�,
	double rad = dist(cast_pt3(PtList[0]), cast_pt3(PtList[1]));

	// ���� �����Ͽ� ��鿡 �߰��Ѵ�.
	std::string Name = create_mesh_name("Sphere");
	GMesh *pSphere = ::create_mesh_from_sphere(Name, X, rad, 30, 30);
	pScene->AddMesh(pSphere);
}

void PCreator::CreatePolygonIcosahedron()
{
	// Sphere ���� �𵨸� ��ǥ�踦 ���ϰ�,
	GTransf X = GetMC();

	// ���õ� �� �� ������ �Ÿ��� ����Ͽ�,
	double rad = dist(cast_pt3(PtList[0]), cast_pt3(PtList[1]));

	// ���� �����Ͽ� ��鿡 �߰��Ѵ�.
	std::string Name = create_mesh_name("Sphere");
	GMesh *pSphere = ::create_mesh_from_icosahedron(Name, X, rad, 5);
	pScene->AddMesh(pSphere);
}

void PCreator::RenderPolygonTeapot()
{
	// ���õ� ���� ���ٸ� �����Ѵ�.
	if (PtList.empty())
		return;

	// ù ��° ���� �� P[0]�� ���� ���콺 ��ġ P[1]�� ���ϰ�,
	GPoint3 P[2];
	P[0] = cast_pt3(PtList[0]);
	P[1] = (PtList.size() > 1) ? cast_pt3(PtList[1]) : cast_pt3(PtList[0]);

	// �� �������� ������ �׸���,
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

	// �� �� ������ �Ÿ��� ����Ͽ�, ������ teapot ���� �Ͻ������� �������Ѵ�.
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
	// ���õ� �� �� ������ �Ÿ��� ����Ͽ�,
	double size = dist(cast_pt3(PtList[0]), cast_pt3(PtList[1])) * 4.0;

	// Teapot�� �����Ͽ� ��鿡 �߰��Ѵ�.
	std::string file_name = pScene->RootDir + "\\res\\models\\teapot.obj";
	::load_mesh_from_obj_file(pScene, file_name.c_str());

	GMesh *pMesh = pScene->pRecentMesh;
	pMesh->MC = GetMC();
	pMesh->Name = create_mesh_name("Teapot");
	pMesh->Edit(GTransf(), GVector3(), GQuater(), GVector3(size, size, size));
	pMesh->UpdateMesh();
}
