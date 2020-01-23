#include "stdafx.h"

/*!
*	\brief 메쉬 위에서 측지거리로 정의된 두 원을 채우는 메쉬를 생성한다.
*
*	\param pMesh 두 원이 놓인 메쉬
*	\param pVert 원의 중심 정점
*	\param r0 시작 원의 반지름
*	\param r1 끝 원의 반지름
*	\param snum_u U-방향 샘플링 수
*	\param snum_v V-방향 샘플링 수
*
*	\return 생성된 메쉬를 반환한다.
*/
GMesh *create_mesh_from_two_circles_on_mesh(GMesh *pMesh, GVertex *pVert, double r0, double r1, int snum_u, int snum_v)
{
	double MaxRadius = MAX(r0, r1) * 1.5;
	std::vector<double> Distances;
	std::vector<double> Angles;
	get_geodesic_polar_coords(pMesh, pVert, Distances, Angles, MaxRadius);

	std::map<int, int> FaceIdxMap;
	GMesh *pUnfoldMesh = new GMesh("", GTransf(), FACE_TRI, "");
	for (unsigned i = 0; i < pMesh->Faces.size(); ++i)
	{
		// 삼각형 세 점의 좌표와 거리를 구하여,
		int *vidx = pMesh->Faces[i].vIdx;
		double d0 = Distances[vidx[0]];
		double d1 = Distances[vidx[1]];
		double d2 = Distances[vidx[2]];

		// 삼각형이 편집 영역내에 들어간다면 건너띈다.
		if (d0 < MaxRadius || d1 < MaxRadius || d2 < MaxRadius)
		{
			// 펼쳐진 삼각형을 추가한다.
			GPoint3 p0(Distances[vidx[0]] * cos(Angles[vidx[0]]), Distances[vidx[0]] * sin(Angles[vidx[0]]), 0.0);
			GPoint3 p1(Distances[vidx[1]] * cos(Angles[vidx[1]]), Distances[vidx[1]] * sin(Angles[vidx[1]]), 0.0);
			GPoint3 p2(Distances[vidx[2]] * cos(Angles[vidx[2]]), Distances[vidx[2]] * sin(Angles[vidx[2]]), 0.0);
			pUnfoldMesh->AddVertex(GVertex(p0));
			pUnfoldMesh->AddVertex(GVertex(p1));
			pUnfoldMesh->AddVertex(GVertex(p2));
			int NumVert = pUnfoldMesh->GetNumVert();
			pUnfoldMesh->AddFace(GFace(NumVert - 3, NumVert - 2, NumVert - 1));

			// 삼각형의 대응 정보를 저장한다.
			FaceIdxMap[pUnfoldMesh->GetNumFace() - 1] = i;		
		}		
	}
	pUnfoldMesh->CreateMesh();

	GQTree QuadTree(pUnfoldMesh, 7, 5);
	GMesh *pRetMesh = new GMesh("", GTransf(), FACE_QUAD, "");
	pRetMesh->Renderer.RenderType |= RENDER_WIRE;

	// 정점의 리스트를 생성한다.
	for (int i = 0; i < snum_u; ++i)
	{
		double u = 2 * M_PI * (double) i / (double)(snum_u - 1) ;
		for (int j = 0; j < snum_v; ++j)
		{
			double v = (double) j / (double)(snum_v - 1);
			double r = r0 + v * (r1 - r0);
			GVector2 pt;
			if (r0 > r1)
				pt.Set(r * cos(u), r * sin(u));
			else
				pt.Set(r * cos(-u), r * sin(-u));

			// 계산된 정점 p가 포함된 삼각형의 인덱스와 barycentric 좌표를 계산하고,
			double alpha, beta;
			int fidx = QuadTree.FindFace(pt, &alpha, &beta);
			if (fidx != -1)
			{
				fidx = FaceIdxMap[fidx];
				int vidx0 = pMesh->Faces[fidx].vIdx[0];
				int vidx1 = pMesh->Faces[fidx].vIdx[1];
				int vidx2 = pMesh->Faces[fidx].vIdx[2];

				// 3차원 객체의 좌표로 복원하여 측지선 메쉬에 추가한다.
				GPoint3 p0 = pMesh->Verts[vidx0].P;
				GPoint3 p1 = pMesh->Verts[vidx1].P;
				GPoint3 p2 = pMesh->Verts[vidx2].P;
				GPoint3 p3 = p0 + alpha * (p1 - p0) + beta * (p2 - p0);
				p3 = pMesh->MC * p3;
				pRetMesh->AddVertex(GVertex(p3));
			}	
		}
	}

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

			pRetMesh->AddFace(f0);
			pRetMesh->AddFace(f1);
		}
	}

	delete pUnfoldMesh;
	return pRetMesh;
}

/*!
*	\brief 정점이 2D 폴리곤 내부에 존재하는지 여부를 판단
*	\note 폴리곤 경계에 위치한 정점은 false를 반환한다.
*
*	\param x 정점의 x좌표
*	\param y 정점의 y좌표
*	\param PtList 2D 폴리곤을 구성하는 정점 리스트
*/
static bool IsPtInPolygon(double x, double y, std::vector<GVector2> &PtList)
{
	// 알고리즘 참조(Real-time Rendering, 2nd ed., p585)
	bool inside = false;
	GVector2 e0 = PtList[0];
	bool y0 = (e0[1] >= y);
	for (unsigned i = 1; i < PtList.size(); ++i)
	{
		GVector2 e1 = PtList[i];
		bool y1 = (e1[1] >= y);
		if (y0 != y1)
		{
			if (((e1[1] - y) * (e0[0] - e1[0]) >= (e1[0] - x) * (e0[1] - e1[1])) == y1)
				inside = !inside;
		}
		y0 = y1;
		e0 = e1;		
	}
	return inside;
}

/*!
*	\brief 메쉬 위에서 두 경계 페곡선 사이의 메쉬를 생성
*	\note 두 곡선은 [0, 1]에서 정의된 XY-평면위의 곡선으로 가정한다.
*	\note 생성된 메쉬는 월드 좌표계에서 표현된다.
*
*	\param pMesh 대상 메쉬
*	\param pVert 기준 정점
*	\param pCrv0 시작 경계 곡선
*	\param pCrv1 끝 경계 곡선
*	\param snum_u U-방향 샘플링 수
*	\param snum_v V-방향 샘플링 수
*	\param trim 메쉬의 트리밍 여부
*
*	\return 생성된 메쉬를 반환한다.
*/
GMesh *create_mesh_from_two_curves_on_mesh(GMesh *pMesh, GVertex *pVert, GNurbsCrv *pCrv0, GNurbsCrv *pCrv1, int snum_u, int snum_v, bool trim)
{
	// 정점 pVert를 기준으로 측지 극좌표를 계산한다.
	double r0 = pCrv0->BBox.GetDiagonal();
	double r1 = pCrv1->BBox.GetDiagonal();
	double MaxRadius = MAX(r0, r1);
	std::vector<double> Distances;
	std::vector<double> Angles;
	get_geodesic_polar_coords(pMesh, pVert, Distances, Angles, MaxRadius);

	// 2D 메쉬를 생성한다.
	std::map<int, int> FaceIdxMap;	// 2D 메쉬와 3D 메쉬간의 삼각형의 대응 정보 (2D 삼각형 인덱스, 3D 삼각형 인덱스)를 저장하는 변수
	GMesh *pUnfoldMesh = new GMesh("", GTransf(), FACE_TRI, "");
	for (unsigned i = 0; i < pMesh->Faces.size(); ++i)
	{
		// 삼각형 세 점의 좌표와 거리를 구하여,
		int *vidx = pMesh->Faces[i].vIdx;
		double d0 = Distances[vidx[0]];
		double d1 = Distances[vidx[1]];
		double d2 = Distances[vidx[2]];

		// 삼각형이 편집 영역내에 들어간다면 스킵한다.
		if (d0 < MaxRadius || d1 < MaxRadius || d2 < MaxRadius)
		{
			// 펼쳐진 삼각형을 추가한다.
			GPoint3 p0(Distances[vidx[0]] * cos(Angles[vidx[0]]), Distances[vidx[0]] * sin(Angles[vidx[0]]), 0.0);
			GPoint3 p1(Distances[vidx[1]] * cos(Angles[vidx[1]]), Distances[vidx[1]] * sin(Angles[vidx[1]]), 0.0);
			GPoint3 p2(Distances[vidx[2]] * cos(Angles[vidx[2]]), Distances[vidx[2]] * sin(Angles[vidx[2]]), 0.0);
			pUnfoldMesh->AddVertex(GVertex(p0));
			pUnfoldMesh->AddVertex(GVertex(p1));
			pUnfoldMesh->AddVertex(GVertex(p2));
			int NumVert = pUnfoldMesh->GetNumVert();
			pUnfoldMesh->AddFace(GFace(NumVert - 3, NumVert - 2, NumVert - 1));

			// 삼각형의 대응 정보를 저장한다.
			FaceIdxMap[pUnfoldMesh->GetNumFace() - 1] = i;		
		}		
	}
	pUnfoldMesh->CreateMesh();

	// 펼쳐진 2D 메쉬를 이용하여 쿼드 트리를 생성한다.
	GQTree QuadTree(pUnfoldMesh, 10, 3);

	// 결과 메쉬를 생성한다.
	GMesh *pRetMesh = new GMesh("", GTransf(), FACE_QUAD, "");
	pRetMesh->Renderer.RenderType |= RENDER_WIRE;

	// 정점의 리스트를 생성한다.
	for (int i = 0; i < snum_u; ++i)
	{
		double u = i / (double)(snum_u - 1);
		for (int j = 0; j < snum_v; ++j)
		{
			double v = j / (double)(snum_v - 1);

			// 2차원 평면상의 점을 계산하여,
			GVector4 pt0 = pCrv0->Eval(u);
			GVector4 pt1 = pCrv1->Eval(u);
			GVector4 tmp = pt0 + v * (pt1 - pt0);
			GVector2 pt(tmp[0], tmp[1]);

			// 계산된 정점(pt)이 포함된 삼각형의 인덱스와 barycentric 좌표를 계산하고,
			double alpha, beta;
			int fidx = QuadTree.FindFace(pt, &alpha, &beta);
			if (fidx != -1)
			{
				fidx = FaceIdxMap[fidx];
				int vidx0 = pMesh->Faces[fidx].vIdx[0];
				int vidx1 = pMesh->Faces[fidx].vIdx[1];
				int vidx2 = pMesh->Faces[fidx].vIdx[2];

				// 3차원 객체의 좌표로 복원하여 측지선 메쉬에 추가한다.
				GPoint3 p0 = pMesh->Verts[vidx0].P;
				GPoint3 p1 = pMesh->Verts[vidx1].P;
				GPoint3 p2 = pMesh->Verts[vidx2].P;
				GPoint3 p3 = p0 + alpha * (p1 - p0) + beta * (p2 - p0);
				p3 = pMesh->MC * p3;
				pRetMesh->AddVertex(GVertex(p3));
			}	
		}
	}

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

			pRetMesh->AddFace(f0);
			pRetMesh->AddFace(f1);
		}
	}

	// 외각 곡선으로 메쉬를 절단한다면,
	if (trim)
	{
		// 외각 곡선으로 대상 메쉬를 절단한다.
		std::vector<int> BndryFaces;
		std::vector<GVector2> BndryPts;
		GNurbsCrv *pCutCrv = (MaxRadius == r0) ? pCrv0 : pCrv1;

		for (int i = 0; i < 200; ++i)
		{
			// 2차원 평면상의 점을 계산하여,
			double u = i / 199.0;
			GVector4 tmp = pCutCrv->Eval(u);		
			GVector2 pt(tmp[0], tmp[1]);
			BndryPts.push_back(pt);	
		}

		for (int i = 0; i < pUnfoldMesh->GetNumFace(); ++i)
		{
			int *vIdx = pUnfoldMesh->Faces[i].vIdx;
			double x0 = pUnfoldMesh->Verts[vIdx[0]].P[0];
			double y0 = pUnfoldMesh->Verts[vIdx[0]].P[1];
			double x1 = pUnfoldMesh->Verts[vIdx[1]].P[0];
			double y1 = pUnfoldMesh->Verts[vIdx[1]].P[1];
			double x2 = pUnfoldMesh->Verts[vIdx[2]].P[0];
			double y2 = pUnfoldMesh->Verts[vIdx[2]].P[1];

			if (IsPtInPolygon(x0, y0, BndryPts) || IsPtInPolygon(x1, y1, BndryPts) || IsPtInPolygon(x2, y2, BndryPts))
				BndryFaces.push_back(FaceIdxMap[i]);
		}
		sort(BndryFaces.begin(), BndryFaces.end());
		BndryFaces.erase(unique(BndryFaces.begin(), BndryFaces.end()), BndryFaces.end());
		pMesh->DelFaces(BndryFaces);
	}

	// 2D 메쉬를 제거하고 결과 메쉬를 반환한다.
	delete pUnfoldMesh;
	return pRetMesh;
}

/*!
*	\brief 두 메쉬를 연결하는 브리지 메쉬를 생성한다.
*
*	\param pStMesh 시작 메쉬
*	\param pStVert 시작 메쉬위의 원의 중심 정점
*	\param st_cont_rad 시작 메쉬위의 접촉 원의 반지름 (st_cont_rad > st_aux_rad)
*	\param st_aux_rad 시작 메쉬위의 보조 원의 반지름 (st_cont_rad > st_aux_rad)
*	\param pEdMesh 종료 메쉬
*	\param pEdVert 종료 메쉬위의 원의 중심 정점
*	\param ed_cont_rad 종료 메쉬위의 접촉 원의 반지름 (ed_aux_rad < ed_cont_rad)
*	\param ed_aux_rad 종료 메쉬위의 보조 원의 반지름 (ed_aux_rad < ed_cont_rad)
*	\param snum_u U-방향 샘플링 수
*	\param snum_v V-방향 샘플링 수
*
*	\return 생성된 메쉬를 반환한다.
*/
GMesh *create_mesh_bridge(GMesh *pStMesh, GVertex *pStVert, double st_cont_rad, double st_aux_rad, 
						  GMesh *pEdMesh, GVertex *pEdVert, double ed_cont_rad, double ed_aux_rad,
						  int snum_u, int snum_v)
{
	// 시작 메쉬의 부분 메쉬를 구한다. 이때, st_cont_rad > st_aux_rad 을 만족해야 한다.
	GMesh *pStSubMesh = ::create_mesh_from_two_circles_on_mesh(pStMesh, pStVert, st_cont_rad, st_aux_rad, snum_u, snum_v);

	// 끝 메쉬의 부분 메쉬를 구한다. 이때, st_aux_rad < ed_cont_rad 을 만족해야 한다.
	GMesh *pEdSubMesh = ::create_mesh_from_two_circles_on_mesh(pEdMesh, pEdVert, st_aux_rad, ed_cont_rad, snum_u, snum_v);

	// 두 부분 메쉬의 시작 파라미터를 조정한다.
	int st_idx = 0, ed_idx = 0;
	double min_dist = 10000000.0;
	for (int i = 0; i < snum_u; ++i)
	{
		GPoint3 p = pStSubMesh->Verts[i * snum_v].P;
		for (int j = 0; j < snum_u; ++j)
		{
			GPoint3 q = pEdSubMesh->Verts[j * snum_v + (snum_v - 1)].P;
			double pq_dist = dist(p, q);
			if (pq_dist < min_dist)
			{
				min_dist = pq_dist;
				st_idx = i * snum_v;
				ed_idx = j * snum_v;
			}
		}
	}

	std::vector<GPoint3> StVerts, EdVerts;
	int NumVert = pStSubMesh->GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		int vidx0 = ((st_idx + i) >= NumVert) ? ((st_idx + i) % NumVert + snum_v) : ((st_idx + i) % NumVert);
		StVerts.push_back(pStSubMesh->Verts[vidx0].P);

		int vidx1 = ((ed_idx + i) >= NumVert) ? ((ed_idx + i) % NumVert + snum_v) : ((ed_idx + i) % NumVert);
		EdVerts.push_back(pEdSubMesh->Verts[vidx1].P);
	}

	// 메쉬를 생성 후, 정점을 추가한다.
	GMesh *pRetMesh = new GMesh("", GTransf(), FACE_QUAD, "");
	for (int i = 0; i < snum_u; ++i)
	{
		for (int j = 0; j < snum_v; ++j)
		{
			double v = (double) j / (double)(snum_v - 1);
			double alpha = 2 * v * v * v - 3 * v * v + 1;

			GPoint3 p = StVerts[i * snum_v + j];
			GPoint3 q = EdVerts[i * snum_v + j];
			GPoint3 pt = q + alpha * (p - q);
			pRetMesh->AddVertex(GVertex(pt, GVector3()));
		}
	}

	// 삼각형을 생성하고 추가한다.
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

			pRetMesh->AddFace(f0);
			pRetMesh->AddFace(f1);
		}
	}

	// 메모리 해제 후 결과 메쉬 반환
	delete pStSubMesh;
	delete pEdSubMesh;
	return pRetMesh;
}

/*!
*	\brief	두 메쉬를 연결하는 브리지 메쉬를 생성한다.
*	\note	접촉곡선을 스케일하여 보조곡선을 사용하는 함수를 작성한 후, 본 함수는 미사용함.
*
*	\param	pStMesh 시작 메쉬
*	\param	pStVert 시작 메쉬의 좌표계 중심 정점
*	\param	pStContCrv 시작 메쉬위의 접촉 곡선
*	\param	pStAuxCrv 시작 메쉬위의 보조 곡선
*	\param	pEdMesh 종료 메쉬
*	\param	pEdVert 종료 메쉬의 좌표계 중심 정점
*	\param	pEdContCrv 종료 메쉬위의 접촉 곡선
*	\param	pEdAuxCrv 종료 메쉬위의 보조 곡선
*	\param	Mu 블렌딩 함수의 파라미터
*	\param	snum_u U-방향 샘플링 수
*	\param	snum_v V-방향 샘플링 수
*	\param	trim 시작 메쉬와 끝 메쉬의 트리밍 여부
*
*	\return 생성된 메쉬를 반환한다.
*/
GMesh *create_mesh_bridge(GMesh *pStMesh, GVertex *pStVert, GNurbsCrv *pStContCrv, GNurbsCrv *pStAuxCrv,
						  GMesh *pEdMesh, GVertex *pEdVert, GNurbsCrv *pEdContCrv, GNurbsCrv *pEdAuxCrv,
						  double Mu, int snum_u, int snum_v, bool trim)
{
	// 시작 메쉬의 부분 메쉬와 끝 메쉬의 부분 메쉬를 구한다.
	pStContCrv->CreateMesh();
	pStAuxCrv->CreateMesh();
	pEdContCrv->CreateMesh();
	pEdAuxCrv->CreateMesh();
	GMesh *pStSubMesh = create_mesh_from_two_curves_on_mesh(pStMesh, pStVert, pStContCrv, pStAuxCrv, snum_u, snum_v, trim);
	GMesh *pEdSubMesh = create_mesh_from_two_curves_on_mesh(pEdMesh, pEdVert, pEdAuxCrv, pEdContCrv, snum_u, snum_v, trim);

	// 두 부분 메쉬의 시작 파라미터를 조정한다.
	int st_idx = 0, ed_idx = 0;
	double min_dist = 10000000.0;
	for (int i = 0; i < snum_u; ++i)
	{
		GPoint3 p = pStSubMesh->Verts[i * snum_v].P;
		for (int j = 0; j < snum_u; ++j)
		{
			GPoint3 q = pEdSubMesh->Verts[j * snum_v + (snum_v - 1)].P;
			double pq_dist = dist(p, q);
			if (pq_dist < min_dist)
			{
				min_dist = pq_dist;
				st_idx = i * snum_v;
				ed_idx = j * snum_v;
			}
		}
	}

	std::vector<GPoint3> StVerts, EdVerts;
	int NumVert = pStSubMesh->GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		int vidx0 = ((st_idx + i) >= NumVert) ? ((st_idx + i) % NumVert + snum_v) : ((st_idx + i) % NumVert);
		StVerts.push_back(pStSubMesh->Verts[vidx0].P);

		int vidx1 = ((ed_idx + i) >= NumVert) ? ((ed_idx + i) % NumVert + snum_v) : ((ed_idx + i) % NumVert);
		EdVerts.push_back(pEdSubMesh->Verts[vidx1].P);
	}

	// 메쉬를 생성 후, 정점을 추가한다.
	GMesh *pRetMesh = new GMesh("", GTransf(), FACE_QUAD, "");
	for (int i = 0; i < snum_u; ++i)
	{
		for (int j = 0; j < snum_v; ++j)
		{
			double v = (double) j / (double)(snum_v - 1);
			//double alpha = 2 * v * v * v - 3 * v * v + 1;
			double tmp = Mu * (1 - v) * (1 - v) * (1 - v);
			double alpha = tmp / (tmp + (1 - Mu) * v * v * v);

			GPoint3 p = StVerts[i * snum_v + j];
			GPoint3 q = EdVerts[i * snum_v + j];
			GPoint3 pt = q + alpha * (p - q);
			pRetMesh->AddVertex(GVertex(pt, GVector3()));
		}
	}

	// 삼각형을 생성하고 추가한다.
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

			pRetMesh->AddFace(f0);
			pRetMesh->AddFace(f1);
		}
	}

	// 트리밍을 한다면,
	if (trim)
	{
		GTransf invStMC = inv(pStMesh->MC);
		GTransf invEdMC = inv(pEdMesh->MC);
		std::vector<GPoint3> BrdgStVerts;
		std::vector<GPoint3> BrdgEdVerts;
		for (int i = 0; i < snum_u - 1; ++i)
		{
			BrdgStVerts.push_back(invStMC * pRetMesh->Verts[i * snum_v].P);
			BrdgEdVerts.push_back(invEdMC * pRetMesh->Verts[(snum_u - 2 - i) * snum_v + (snum_v - 1)].P);
		}

		// 시작 부분에 브리지 메쉬의 연결부위를 추가한다.
		connect_mesh_with_verts(pStMesh, BrdgStVerts);
		connect_mesh_with_verts(pEdMesh, BrdgEdVerts);
	}	

	// 메모리 해제 후 결과 메쉬 반환
	delete pStSubMesh;
	delete pEdSubMesh;

	return pRetMesh;
}

/*!
*	\brief 두 메쉬를 연결하는 브리지 메쉬를 생성한다.
*
*	\param pStMesh 시작 메쉬
*	\param pStVert 시작 메쉬의 좌표계 중심 정점
*	\param pStContCrv 시작 메쉬위의 접촉 곡선
*	\param alpha 시작 메쉬의 보조 곡선의 스케일 파라미터
*	\param pEdMesh 종료 메쉬
*	\param pEdVert 종료 메쉬의 좌표계 중심 정점
*	\param pEdContCrv 종료 메쉬위의 접촉 곡선
*	\param beta 종료 메쉬의 보조 곡선의 스케일 파라미터
*	\param mu 블렌딩 함수의 파라미터
*	\param snum_u U-방향 샘플링 수
*	\param snum_v V-방향 샘플링 수
*	\param trim 시작 메쉬와 끝 메쉬의 트리밍 여부
*
*	\return 생성된 메쉬를 반환한다.
*/
GMesh *create_mesh_bridge(GMesh *pStMesh, GVertex *pStVert, GNurbsCrv *pStContCrv, double alpha,
						  GMesh *pEdMesh, GVertex *pEdVert, GNurbsCrv *pEdContCrv, double beta,
						  double mu, int snum_u, int snum_v, bool trim)
{
	// 시작 메쉬의 부분 메쉬와 끝 메쉬의 부분 메쉬를 구한다.
	GNurbsCrv *pStAuxCrv = pStContCrv->CopyObject();
	pEdContCrv->Reverse();
	GNurbsCrv *pEdAuxCrv = pEdContCrv->CopyObject();
	pStAuxCrv->Scale(alpha);
	pEdAuxCrv->Scale(beta);

	// 시작 메쉬의 부분 메쉬와 끝 메쉬의 부분 메쉬를 구한다.
	pStContCrv->CreateMesh();
	pStAuxCrv->CreateMesh();
	pEdContCrv->CreateMesh();
	pEdAuxCrv->CreateMesh();
	GMesh *pStSubMesh = create_mesh_from_two_curves_on_mesh(pStMesh, pStVert, pStContCrv, pStAuxCrv, snum_u, snum_v, trim);
	GMesh *pEdSubMesh = create_mesh_from_two_curves_on_mesh(pEdMesh, pEdVert, pEdAuxCrv, pEdContCrv, snum_u, snum_v, trim);

	// 두 부분 메쉬의 시작 파라미터를 조정한다.
	int st_idx = 0, ed_idx = 0;
	double min_dist = 10000000.0;
	for (int i = 0; i < snum_u; ++i)
	{
		GPoint3 p = pStSubMesh->Verts[i * snum_v].P;
		for (int j = 0; j < snum_u; ++j)
		{
			GPoint3 q = pEdSubMesh->Verts[j * snum_v + (snum_v - 1)].P;
			double pq_dist = dist(p, q);
			if (pq_dist < min_dist)
			{
				min_dist = pq_dist;
				st_idx = i * snum_v;
				ed_idx = j * snum_v;
			}
		}
	}

	std::vector<GPoint3> StVerts, EdVerts;
	int NumVert = pStSubMesh->GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		int vidx0 = ((st_idx + i) >= NumVert) ? ((st_idx + i) % NumVert + snum_v) : ((st_idx + i) % NumVert);
		StVerts.push_back(pStSubMesh->Verts[vidx0].P);

		int vidx1 = ((ed_idx + i) >= NumVert) ? ((ed_idx + i) % NumVert + snum_v) : ((ed_idx + i) % NumVert);
		EdVerts.push_back(pEdSubMesh->Verts[vidx1].P);
	}

	// 메쉬를 생성 후, 정점을 추가한다.
	GMesh *pRetMesh = new GMesh("", GTransf(), FACE_QUAD, "");
	for (int i = 0; i < snum_u; ++i)
	{
		for (int j = 0; j < snum_v; ++j)
		{
			double v = (double) j / (double)(snum_v - 1);
			//double alpha = 2 * v * v * v - 3 * v * v + 1;
			double tmp = mu * (1 - v) * (1 - v) * (1 - v);
			double alpha = tmp / (tmp + (1 - mu) * v * v * v);

			GPoint3 p = StVerts[i * snum_v + j];
			GPoint3 q = EdVerts[i * snum_v + j];
			GPoint3 pt = q + alpha * (p - q);
			pRetMesh->AddVertex(GVertex(pt, GVector3()));
		}
	}

	// 삼각형을 생성하고 추가한다.
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

			pRetMesh->AddFace(f0);
			pRetMesh->AddFace(f1);
		}
	}

	// 트리밍을 한다면,
	if (trim)
	{
		GTransf invStMC = inv(pStMesh->MC);
		GTransf invEdMC = inv(pEdMesh->MC);
		std::vector<GPoint3> BrdgStVerts;
		std::vector<GPoint3> BrdgEdVerts;
		for (int i = 0; i < snum_u - 1; ++i)
		{
			BrdgStVerts.push_back(invStMC * pRetMesh->Verts[i * snum_v].P);
			BrdgEdVerts.push_back(invEdMC * pRetMesh->Verts[(snum_u - 2 - i) * snum_v + (snum_v - 1)].P);
		}

		// 시작 부분에 브리지 메쉬의 연결부위를 추가한다.
		connect_mesh_with_verts(pStMesh, BrdgStVerts);
		connect_mesh_with_verts(pEdMesh, BrdgEdVerts);
	}	

	// 메모리 해제 후 결과 메쉬 반환
	delete pStSubMesh;
	delete pEdSubMesh;
	delete pStAuxCrv;
	delete pEdAuxCrv;
	pEdContCrv->Reverse();

	return pRetMesh;
}

/*!
*	\brief 생성된 브리지 메쉬를 갱신하는 함수
*
*	\param pBridgeMesh 갱신할 브리지 메쉬
*	\param pStMesh 시작 메쉬
*	\param pStVert 시작 메쉬의 좌표계 중심 정점
*	\param pStContCrv 시작 메쉬위의 접촉 곡선
*	\param alpha 시작 메쉬의 보조 곡선의 스케일 파라미터
*	\param pEdMesh 종료 메쉬
*	\param pEdVert 종료 메쉬의 좌표계 중심 정점
*	\param pEdContCrv 종료 메쉬위의 접촉 곡선
*	\param beta 종료 메쉬의 보조 곡선의 스케일 파라미터
*	\param mu 블렌딩 함수의 파라미터
*	\param snum_u U-방향 샘플링 수
*	\param snum_v V-방향 샘플링 수
*	\param trim 시작 메쉬와 끝 메쉬의 트리밍 여부
*
*	\return 생성된 메쉬를 반환한다.
*/
void update_mesh_bridge(GMesh *pBridgeMesh,
						GMesh *pStMesh, GVertex *pStVert, GNurbsCrv *pStContCrv, double alpha,
						GMesh *pEdMesh, GVertex *pEdVert, GNurbsCrv *pEdContCrv, double beta,
						double mu, int snum_u, int snum_v, bool trim)
{
	// 시작 메쉬의 부분 메쉬와 끝 메쉬의 부분 메쉬를 구한다.
	GNurbsCrv *pStAuxCrv = pStContCrv->CopyObject();
	pEdContCrv->Reverse();
	GNurbsCrv *pEdAuxCrv = pEdContCrv->CopyObject();
	pStAuxCrv->Scale(alpha);
	pEdAuxCrv->Scale(beta);

	// 시작 메쉬의 부분 메쉬와 끝 메쉬의 부분 메쉬를 구한다.
	pStContCrv->CreateMesh();
	pStAuxCrv->CreateMesh();
	pEdContCrv->CreateMesh();
	pEdAuxCrv->CreateMesh();
	GMesh *pStSubMesh = create_mesh_from_two_curves_on_mesh(pStMesh, pStVert, pStContCrv, pStAuxCrv, snum_u, snum_v, trim);
	GMesh *pEdSubMesh = create_mesh_from_two_curves_on_mesh(pEdMesh, pEdVert, pEdAuxCrv, pEdContCrv, snum_u, snum_v, trim);

	// 두 부분 메쉬의 시작 파라미터를 조정한다.
	int st_idx = 0, ed_idx = 0;
	double min_dist = 10000000.0;
	for (int i = 0; i < snum_u; ++i)
	{
		GPoint3 p = pStSubMesh->Verts[i * snum_v].P;
		for (int j = 0; j < snum_u; ++j)
		{
			GPoint3 q = pEdSubMesh->Verts[j * snum_v + (snum_v - 1)].P;
			double pq_dist = dist(p, q);
			if (pq_dist < min_dist)
			{
				min_dist = pq_dist;
				st_idx = i * snum_v;
				ed_idx = j * snum_v;
			}
		}
	}

	std::vector<GPoint3> StVerts, EdVerts;
	int NumVert = pStSubMesh->GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		int vidx0 = ((st_idx + i) >= NumVert) ? ((st_idx + i) % NumVert + snum_v) : ((st_idx + i) % NumVert);
		StVerts.push_back(pStSubMesh->Verts[vidx0].P);

		int vidx1 = ((ed_idx + i) >= NumVert) ? ((ed_idx + i) % NumVert + snum_v) : ((ed_idx + i) % NumVert);
		EdVerts.push_back(pEdSubMesh->Verts[vidx1].P);
	}

	// 메쉬를 생성 후, 정점을 추가한다.
	pBridgeMesh->Verts.clear();
	for (int i = 0; i < snum_u; ++i)
	{
		for (int j = 0; j < snum_v; ++j)
		{
			double v = (double) j / (double)(snum_v - 1);
			//double alpha = 2 * v * v * v - 3 * v * v + 1;
			double tmp = mu * (1 - v) * (1 - v) * (1 - v);
			double alpha = tmp / (tmp + (1 - mu) * v * v * v);

			GPoint3 p = StVerts[i * snum_v + j];
			GPoint3 q = EdVerts[i * snum_v + j];
			GPoint3 pt = q + alpha * (p - q);
			pBridgeMesh->AddVertex(GVertex(pt, GVector3()));
		}
	}

	// 트리밍을 한다면,
	if (trim)
	{
		GTransf invStMC = inv(pStMesh->MC);
		GTransf invEdMC = inv(pEdMesh->MC);

		std::vector<GPoint3> BrdgStVerts;
		std::vector<GPoint3> BrdgEdVerts;
		for (int i = 0; i < snum_u - 1; ++i)
		{
			BrdgStVerts.push_back(invStMC * pBridgeMesh->Verts[i * snum_v].P);
			BrdgEdVerts.push_back(invEdMC * pBridgeMesh->Verts[(snum_u - 2 - i) * snum_v + (snum_v - 1)].P);
		}

		// 시작 부분에 브리지 메쉬의 연결부위를 추가한다.
		connect_mesh_with_verts(pStMesh, BrdgStVerts);
		connect_mesh_with_verts(pEdMesh, BrdgEdVerts);
	}	

	// 메모리 해제 후 결과 메쉬 반환
	delete pStSubMesh;
	delete pEdSubMesh;
	delete pStAuxCrv;
	delete pEdAuxCrv;
	pEdContCrv->Reverse();
}

/*! 
*	\brief 메쉬와 추가 정점을 연결하는 삼각형을 메쉬에 추가하는 함수
*
*	\param pMesh	확장할 메쉬
*	\param Verts	추가할 정점의 리스트
*/
void connect_mesh_with_verts(GMesh *pMesh, std::vector<GPoint3> &Verts)
{
	std::map<int, int> BndryEdges;
	::get_bndry_edges(pMesh, BndryEdges);

	// 블렌딩 메쉬의 시작점과 연결할 가장 가까운 경계 정점의 시작 인덱스를 찾는다.
	int st_vidx = -1;
	double min_dist = 1.0e+10;
	std::map<int, int>::iterator it;
	for (it = BndryEdges.begin(); it != BndryEdges.end(); ++it)
	{
		double d = dist(Verts[0], pMesh->Verts[it->first].P);
		if (d < min_dist)
		{
			min_dist = d;
			st_vidx = it->first;
		}
	}

	// 시작 인덱스로부터 경계 정점의 인덱스를 구한다.
	std::vector<int> BndryVerts;
	int curr_vidx = st_vidx;
	while (true)
	{
		if (BndryEdges[curr_vidx] == st_vidx)
		{
			BndryVerts.push_back(curr_vidx);
			break;
		}
		BndryVerts.push_back(curr_vidx);
		curr_vidx = BndryEdges[curr_vidx];	
	}

	// 베이스 정점 인덱스를 구하고 브리지 시작 경계 정점을 추가한다.
	int base_vidx = pMesh->GetNumVert();
	int NumVert = (int)Verts.size();
	for (int i = 0; i < NumVert; ++i)
		pMesh->AddVertex(GVertex(Verts[i]));

	int prev_idx = -1;
	int MtlIdx = pMesh->Faces[0].MtlIdx;
	std::string grp_name = (!pMesh->GrpFaces.empty()) ? pMesh->GrpFaces.begin()->first : "";

	for (int i = 0; i < NumVert; ++i)
	{
		// 추가된 에지의 두 정점에 대하여,
		GPoint3 pt0 = pMesh->Verts[base_vidx + i].P;
		GPoint3 pt1 = pMesh->Verts[base_vidx + ((i + 1) % NumVert)].P;

		// 가장 가까운 경계 정점의 두 인덱스를 구한다.
		int idx0 = -1, idx1 = -1;
		double min_dist0 = 1.0e+10, min_dist1 = 1.0e+10;
		for (unsigned j = 0; j < BndryVerts.size(); ++j)
		{
			double d0 = dist(pt0, pMesh->Verts[BndryVerts[j]].P);
			double d1 = dist(pt1, pMesh->Verts[BndryVerts[j]].P);
			if (d0 < min_dist0)
			{
				min_dist0 = d0;
				idx0 = j;
			}
			if (d1 < min_dist1)
			{
				min_dist1 = d1;
				idx1 = j;
			}
		}

		idx1 = (idx0 <= idx1) ? idx1 : BndryVerts.size();
		for (int k = idx0; k < idx1 + 1; ++k)
		{
			int vidx0 = base_vidx + i;
			int vidx1 = BndryVerts[k % BndryVerts.size()];
			int vidx2 = (k == idx1) ? (base_vidx + (i + 1) % NumVert) : BndryVerts[(k + 1) % BndryVerts.size()];
			pMesh->AddFace(GFace(vidx0, vidx1, vidx2, -1, -1, -1, MtlIdx), grp_name);
		}		
	}

	pMesh->ModifiedFlag = MODIFIED_VERT_NUM;
	pMesh->UpdateMesh();
}

/*!
*	\brief 메쉬와 스윕 곡면을 연결하는 브리지 메쉬를 생성한다.
*
*	\param pMesh	메쉬
*	\param pVert	메쉬위의 원의 중심 정점
*	\param ContRad	메쉬위의 접촉 원의 반지름 (ContRad > AuxRad)
*	\param AuxRad	메쉬위의 보조 원의 반지름 (ContRad > AuxRad)
*	\param pSrf		스윕 곡면
*	\param ContParam 스윕 곡면의 접촉 곡선의 파라미터 (ContParam > AuxParam)
*	\param AuxParam  스윕 곡면의 보조 곡선의 파라미터 (ContParam > AuxParam)
*	\param snum_u U-방향 샘플링 수
*	\param snum_v V-방향 샘플링 수
*
*	\return 생성된 메쉬를 반환한다.
*/
GMesh *create_mesh_bridge(GMesh *pMesh, GVertex *pVert, double ContRad, double AuxRad, 
						  GSweepSrf *pSrf, double ContParam, double AuxParam, 
						  int snum_u, int snum_v)
{
	// 시작 메쉬의 부분 메쉬를 구한다. 이때, ContRad > AuxRad 을 만족해야 한다.
	GMesh *pStSubMesh = ::create_mesh_from_two_circles_on_mesh(pMesh, pVert, ContRad, AuxRad, snum_u, snum_v);

	// 스윕 곡면으로부터 끝 메쉬의 부분 메쉬를 구한다. 이때, ContParam > AuxParam 을 만족해야 한다.
	GMesh *pEdSubMesh = ::create_mesh_from_sweep_srf(pSrf, ContParam, AuxParam, snum_u, snum_v);

	int st_idx = 0, ed_idx = 0;
	double min_dist = 10000000.0;
	for (int i = 0; i < snum_u; ++i)
	{
		GPoint3 p = pStSubMesh->Verts[i * snum_v].P;
		for (int j = 0; j < snum_u; ++j)
		{
			GPoint3 q = pEdSubMesh->Verts[j * snum_v + (snum_v - 1)].P;
			double pq_dist = dist(p, q);
			if (pq_dist < min_dist)
			{
				min_dist = pq_dist;
				st_idx = i * snum_v;
				ed_idx = j * snum_v;
			}
		}
	}

	std::vector<GPoint3> StVerts, EdVerts;
	int NumVert = pStSubMesh->GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		int vidx0 = ((st_idx + i) >= NumVert) ? ((st_idx + i) % NumVert + snum_v) : ((st_idx + i) % NumVert);
		StVerts.push_back(pStSubMesh->Verts[vidx0].P);

		int vidx1 = ((ed_idx + i) >= NumVert) ? ((ed_idx + i) % NumVert + snum_v) : ((ed_idx + i) % NumVert);
		EdVerts.push_back(pEdSubMesh->Verts[vidx1].P);
	}

	// 메쉬를 생성 후, 정점을 추가한다.
	GMesh *pRetMesh = new GMesh("", GTransf(), FACE_QUAD, "");
	for (int i = 0; i < snum_u; ++i)
	{
		for (int j = 0; j < snum_v; ++j)
		{
			double v = (double) j / (double)(snum_v - 1);
			double alpha = 2 * v * v * v - 3 * v * v + 1;

			GPoint3 p = StVerts[i * snum_v + j];
			GPoint3 q = EdVerts[i * snum_v + j];
			GPoint3 pt = q + alpha * (p - q);
			pRetMesh->AddVertex(GVertex(pt, GVector3()));
		}
	}

	// 삼각형을 생성하고 추가한다.
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

			pRetMesh->AddFace(f0);
			pRetMesh->AddFace(f1);
		}
	}

	// 메모리 해제 후 결과 메쉬 반환
	delete pStSubMesh;
	delete pEdSubMesh;
	return pRetMesh;
}