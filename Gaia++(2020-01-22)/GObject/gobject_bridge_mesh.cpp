#include "stdafx.h"

/*!
*	\brief �޽� ������ �����Ÿ��� ���ǵ� �� ���� ä��� �޽��� �����Ѵ�.
*
*	\param pMesh �� ���� ���� �޽�
*	\param pVert ���� �߽� ����
*	\param r0 ���� ���� ������
*	\param r1 �� ���� ������
*	\param snum_u U-���� ���ø� ��
*	\param snum_v V-���� ���ø� ��
*
*	\return ������ �޽��� ��ȯ�Ѵ�.
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
		// �ﰢ�� �� ���� ��ǥ�� �Ÿ��� ���Ͽ�,
		int *vidx = pMesh->Faces[i].vIdx;
		double d0 = Distances[vidx[0]];
		double d1 = Distances[vidx[1]];
		double d2 = Distances[vidx[2]];

		// �ﰢ���� ���� �������� ���ٸ� �ǳʶ��.
		if (d0 < MaxRadius || d1 < MaxRadius || d2 < MaxRadius)
		{
			// ������ �ﰢ���� �߰��Ѵ�.
			GPoint3 p0(Distances[vidx[0]] * cos(Angles[vidx[0]]), Distances[vidx[0]] * sin(Angles[vidx[0]]), 0.0);
			GPoint3 p1(Distances[vidx[1]] * cos(Angles[vidx[1]]), Distances[vidx[1]] * sin(Angles[vidx[1]]), 0.0);
			GPoint3 p2(Distances[vidx[2]] * cos(Angles[vidx[2]]), Distances[vidx[2]] * sin(Angles[vidx[2]]), 0.0);
			pUnfoldMesh->AddVertex(GVertex(p0));
			pUnfoldMesh->AddVertex(GVertex(p1));
			pUnfoldMesh->AddVertex(GVertex(p2));
			int NumVert = pUnfoldMesh->GetNumVert();
			pUnfoldMesh->AddFace(GFace(NumVert - 3, NumVert - 2, NumVert - 1));

			// �ﰢ���� ���� ������ �����Ѵ�.
			FaceIdxMap[pUnfoldMesh->GetNumFace() - 1] = i;		
		}		
	}
	pUnfoldMesh->CreateMesh();

	GQTree QuadTree(pUnfoldMesh, 7, 5);
	GMesh *pRetMesh = new GMesh("", GTransf(), FACE_QUAD, "");
	pRetMesh->Renderer.RenderType |= RENDER_WIRE;

	// ������ ����Ʈ�� �����Ѵ�.
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

			// ���� ���� p�� ���Ե� �ﰢ���� �ε����� barycentric ��ǥ�� ����ϰ�,
			double alpha, beta;
			int fidx = QuadTree.FindFace(pt, &alpha, &beta);
			if (fidx != -1)
			{
				fidx = FaceIdxMap[fidx];
				int vidx0 = pMesh->Faces[fidx].vIdx[0];
				int vidx1 = pMesh->Faces[fidx].vIdx[1];
				int vidx2 = pMesh->Faces[fidx].vIdx[2];

				// 3���� ��ü�� ��ǥ�� �����Ͽ� ������ �޽��� �߰��Ѵ�.
				GPoint3 p0 = pMesh->Verts[vidx0].P;
				GPoint3 p1 = pMesh->Verts[vidx1].P;
				GPoint3 p2 = pMesh->Verts[vidx2].P;
				GPoint3 p3 = p0 + alpha * (p1 - p0) + beta * (p2 - p0);
				p3 = pMesh->MC * p3;
				pRetMesh->AddVertex(GVertex(p3));
			}	
		}
	}

	// �ﰢ�� ����Ʈ�� �����Ѵ�.
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
*	\brief ������ 2D ������ ���ο� �����ϴ��� ���θ� �Ǵ�
*	\note ������ ��迡 ��ġ�� ������ false�� ��ȯ�Ѵ�.
*
*	\param x ������ x��ǥ
*	\param y ������ y��ǥ
*	\param PtList 2D �������� �����ϴ� ���� ����Ʈ
*/
static bool IsPtInPolygon(double x, double y, std::vector<GVector2> &PtList)
{
	// �˰��� ����(Real-time Rendering, 2nd ed., p585)
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
*	\brief �޽� ������ �� ��� �� ������ �޽��� ����
*	\note �� ��� [0, 1]���� ���ǵ� XY-������� ����� �����Ѵ�.
*	\note ������ �޽��� ���� ��ǥ�迡�� ǥ���ȴ�.
*
*	\param pMesh ��� �޽�
*	\param pVert ���� ����
*	\param pCrv0 ���� ��� �
*	\param pCrv1 �� ��� �
*	\param snum_u U-���� ���ø� ��
*	\param snum_v V-���� ���ø� ��
*	\param trim �޽��� Ʈ���� ����
*
*	\return ������ �޽��� ��ȯ�Ѵ�.
*/
GMesh *create_mesh_from_two_curves_on_mesh(GMesh *pMesh, GVertex *pVert, GNurbsCrv *pCrv0, GNurbsCrv *pCrv1, int snum_u, int snum_v, bool trim)
{
	// ���� pVert�� �������� ���� ����ǥ�� ����Ѵ�.
	double r0 = pCrv0->BBox.GetDiagonal();
	double r1 = pCrv1->BBox.GetDiagonal();
	double MaxRadius = MAX(r0, r1);
	std::vector<double> Distances;
	std::vector<double> Angles;
	get_geodesic_polar_coords(pMesh, pVert, Distances, Angles, MaxRadius);

	// 2D �޽��� �����Ѵ�.
	std::map<int, int> FaceIdxMap;	// 2D �޽��� 3D �޽����� �ﰢ���� ���� ���� (2D �ﰢ�� �ε���, 3D �ﰢ�� �ε���)�� �����ϴ� ����
	GMesh *pUnfoldMesh = new GMesh("", GTransf(), FACE_TRI, "");
	for (unsigned i = 0; i < pMesh->Faces.size(); ++i)
	{
		// �ﰢ�� �� ���� ��ǥ�� �Ÿ��� ���Ͽ�,
		int *vidx = pMesh->Faces[i].vIdx;
		double d0 = Distances[vidx[0]];
		double d1 = Distances[vidx[1]];
		double d2 = Distances[vidx[2]];

		// �ﰢ���� ���� �������� ���ٸ� ��ŵ�Ѵ�.
		if (d0 < MaxRadius || d1 < MaxRadius || d2 < MaxRadius)
		{
			// ������ �ﰢ���� �߰��Ѵ�.
			GPoint3 p0(Distances[vidx[0]] * cos(Angles[vidx[0]]), Distances[vidx[0]] * sin(Angles[vidx[0]]), 0.0);
			GPoint3 p1(Distances[vidx[1]] * cos(Angles[vidx[1]]), Distances[vidx[1]] * sin(Angles[vidx[1]]), 0.0);
			GPoint3 p2(Distances[vidx[2]] * cos(Angles[vidx[2]]), Distances[vidx[2]] * sin(Angles[vidx[2]]), 0.0);
			pUnfoldMesh->AddVertex(GVertex(p0));
			pUnfoldMesh->AddVertex(GVertex(p1));
			pUnfoldMesh->AddVertex(GVertex(p2));
			int NumVert = pUnfoldMesh->GetNumVert();
			pUnfoldMesh->AddFace(GFace(NumVert - 3, NumVert - 2, NumVert - 1));

			// �ﰢ���� ���� ������ �����Ѵ�.
			FaceIdxMap[pUnfoldMesh->GetNumFace() - 1] = i;		
		}		
	}
	pUnfoldMesh->CreateMesh();

	// ������ 2D �޽��� �̿��Ͽ� ���� Ʈ���� �����Ѵ�.
	GQTree QuadTree(pUnfoldMesh, 10, 3);

	// ��� �޽��� �����Ѵ�.
	GMesh *pRetMesh = new GMesh("", GTransf(), FACE_QUAD, "");
	pRetMesh->Renderer.RenderType |= RENDER_WIRE;

	// ������ ����Ʈ�� �����Ѵ�.
	for (int i = 0; i < snum_u; ++i)
	{
		double u = i / (double)(snum_u - 1);
		for (int j = 0; j < snum_v; ++j)
		{
			double v = j / (double)(snum_v - 1);

			// 2���� ������ ���� ����Ͽ�,
			GVector4 pt0 = pCrv0->Eval(u);
			GVector4 pt1 = pCrv1->Eval(u);
			GVector4 tmp = pt0 + v * (pt1 - pt0);
			GVector2 pt(tmp[0], tmp[1]);

			// ���� ����(pt)�� ���Ե� �ﰢ���� �ε����� barycentric ��ǥ�� ����ϰ�,
			double alpha, beta;
			int fidx = QuadTree.FindFace(pt, &alpha, &beta);
			if (fidx != -1)
			{
				fidx = FaceIdxMap[fidx];
				int vidx0 = pMesh->Faces[fidx].vIdx[0];
				int vidx1 = pMesh->Faces[fidx].vIdx[1];
				int vidx2 = pMesh->Faces[fidx].vIdx[2];

				// 3���� ��ü�� ��ǥ�� �����Ͽ� ������ �޽��� �߰��Ѵ�.
				GPoint3 p0 = pMesh->Verts[vidx0].P;
				GPoint3 p1 = pMesh->Verts[vidx1].P;
				GPoint3 p2 = pMesh->Verts[vidx2].P;
				GPoint3 p3 = p0 + alpha * (p1 - p0) + beta * (p2 - p0);
				p3 = pMesh->MC * p3;
				pRetMesh->AddVertex(GVertex(p3));
			}	
		}
	}

	// �ﰢ�� ����Ʈ�� �����Ѵ�.
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

	// �ܰ� ����� �޽��� �����Ѵٸ�,
	if (trim)
	{
		// �ܰ� ����� ��� �޽��� �����Ѵ�.
		std::vector<int> BndryFaces;
		std::vector<GVector2> BndryPts;
		GNurbsCrv *pCutCrv = (MaxRadius == r0) ? pCrv0 : pCrv1;

		for (int i = 0; i < 200; ++i)
		{
			// 2���� ������ ���� ����Ͽ�,
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

	// 2D �޽��� �����ϰ� ��� �޽��� ��ȯ�Ѵ�.
	delete pUnfoldMesh;
	return pRetMesh;
}

/*!
*	\brief �� �޽��� �����ϴ� �긮�� �޽��� �����Ѵ�.
*
*	\param pStMesh ���� �޽�
*	\param pStVert ���� �޽����� ���� �߽� ����
*	\param st_cont_rad ���� �޽����� ���� ���� ������ (st_cont_rad > st_aux_rad)
*	\param st_aux_rad ���� �޽����� ���� ���� ������ (st_cont_rad > st_aux_rad)
*	\param pEdMesh ���� �޽�
*	\param pEdVert ���� �޽����� ���� �߽� ����
*	\param ed_cont_rad ���� �޽����� ���� ���� ������ (ed_aux_rad < ed_cont_rad)
*	\param ed_aux_rad ���� �޽����� ���� ���� ������ (ed_aux_rad < ed_cont_rad)
*	\param snum_u U-���� ���ø� ��
*	\param snum_v V-���� ���ø� ��
*
*	\return ������ �޽��� ��ȯ�Ѵ�.
*/
GMesh *create_mesh_bridge(GMesh *pStMesh, GVertex *pStVert, double st_cont_rad, double st_aux_rad, 
						  GMesh *pEdMesh, GVertex *pEdVert, double ed_cont_rad, double ed_aux_rad,
						  int snum_u, int snum_v)
{
	// ���� �޽��� �κ� �޽��� ���Ѵ�. �̶�, st_cont_rad > st_aux_rad �� �����ؾ� �Ѵ�.
	GMesh *pStSubMesh = ::create_mesh_from_two_circles_on_mesh(pStMesh, pStVert, st_cont_rad, st_aux_rad, snum_u, snum_v);

	// �� �޽��� �κ� �޽��� ���Ѵ�. �̶�, st_aux_rad < ed_cont_rad �� �����ؾ� �Ѵ�.
	GMesh *pEdSubMesh = ::create_mesh_from_two_circles_on_mesh(pEdMesh, pEdVert, st_aux_rad, ed_cont_rad, snum_u, snum_v);

	// �� �κ� �޽��� ���� �Ķ���͸� �����Ѵ�.
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

	// �޽��� ���� ��, ������ �߰��Ѵ�.
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

	// �ﰢ���� �����ϰ� �߰��Ѵ�.
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

	// �޸� ���� �� ��� �޽� ��ȯ
	delete pStSubMesh;
	delete pEdSubMesh;
	return pRetMesh;
}

/*!
*	\brief	�� �޽��� �����ϴ� �긮�� �޽��� �����Ѵ�.
*	\note	���˰�� �������Ͽ� ������� ����ϴ� �Լ��� �ۼ��� ��, �� �Լ��� �̻����.
*
*	\param	pStMesh ���� �޽�
*	\param	pStVert ���� �޽��� ��ǥ�� �߽� ����
*	\param	pStContCrv ���� �޽����� ���� �
*	\param	pStAuxCrv ���� �޽����� ���� �
*	\param	pEdMesh ���� �޽�
*	\param	pEdVert ���� �޽��� ��ǥ�� �߽� ����
*	\param	pEdContCrv ���� �޽����� ���� �
*	\param	pEdAuxCrv ���� �޽����� ���� �
*	\param	Mu ���� �Լ��� �Ķ����
*	\param	snum_u U-���� ���ø� ��
*	\param	snum_v V-���� ���ø� ��
*	\param	trim ���� �޽��� �� �޽��� Ʈ���� ����
*
*	\return ������ �޽��� ��ȯ�Ѵ�.
*/
GMesh *create_mesh_bridge(GMesh *pStMesh, GVertex *pStVert, GNurbsCrv *pStContCrv, GNurbsCrv *pStAuxCrv,
						  GMesh *pEdMesh, GVertex *pEdVert, GNurbsCrv *pEdContCrv, GNurbsCrv *pEdAuxCrv,
						  double Mu, int snum_u, int snum_v, bool trim)
{
	// ���� �޽��� �κ� �޽��� �� �޽��� �κ� �޽��� ���Ѵ�.
	pStContCrv->CreateMesh();
	pStAuxCrv->CreateMesh();
	pEdContCrv->CreateMesh();
	pEdAuxCrv->CreateMesh();
	GMesh *pStSubMesh = create_mesh_from_two_curves_on_mesh(pStMesh, pStVert, pStContCrv, pStAuxCrv, snum_u, snum_v, trim);
	GMesh *pEdSubMesh = create_mesh_from_two_curves_on_mesh(pEdMesh, pEdVert, pEdAuxCrv, pEdContCrv, snum_u, snum_v, trim);

	// �� �κ� �޽��� ���� �Ķ���͸� �����Ѵ�.
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

	// �޽��� ���� ��, ������ �߰��Ѵ�.
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

	// �ﰢ���� �����ϰ� �߰��Ѵ�.
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

	// Ʈ������ �Ѵٸ�,
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

		// ���� �κп� �긮�� �޽��� ��������� �߰��Ѵ�.
		connect_mesh_with_verts(pStMesh, BrdgStVerts);
		connect_mesh_with_verts(pEdMesh, BrdgEdVerts);
	}	

	// �޸� ���� �� ��� �޽� ��ȯ
	delete pStSubMesh;
	delete pEdSubMesh;

	return pRetMesh;
}

/*!
*	\brief �� �޽��� �����ϴ� �긮�� �޽��� �����Ѵ�.
*
*	\param pStMesh ���� �޽�
*	\param pStVert ���� �޽��� ��ǥ�� �߽� ����
*	\param pStContCrv ���� �޽����� ���� �
*	\param alpha ���� �޽��� ���� ��� ������ �Ķ����
*	\param pEdMesh ���� �޽�
*	\param pEdVert ���� �޽��� ��ǥ�� �߽� ����
*	\param pEdContCrv ���� �޽����� ���� �
*	\param beta ���� �޽��� ���� ��� ������ �Ķ����
*	\param mu ���� �Լ��� �Ķ����
*	\param snum_u U-���� ���ø� ��
*	\param snum_v V-���� ���ø� ��
*	\param trim ���� �޽��� �� �޽��� Ʈ���� ����
*
*	\return ������ �޽��� ��ȯ�Ѵ�.
*/
GMesh *create_mesh_bridge(GMesh *pStMesh, GVertex *pStVert, GNurbsCrv *pStContCrv, double alpha,
						  GMesh *pEdMesh, GVertex *pEdVert, GNurbsCrv *pEdContCrv, double beta,
						  double mu, int snum_u, int snum_v, bool trim)
{
	// ���� �޽��� �κ� �޽��� �� �޽��� �κ� �޽��� ���Ѵ�.
	GNurbsCrv *pStAuxCrv = pStContCrv->CopyObject();
	pEdContCrv->Reverse();
	GNurbsCrv *pEdAuxCrv = pEdContCrv->CopyObject();
	pStAuxCrv->Scale(alpha);
	pEdAuxCrv->Scale(beta);

	// ���� �޽��� �κ� �޽��� �� �޽��� �κ� �޽��� ���Ѵ�.
	pStContCrv->CreateMesh();
	pStAuxCrv->CreateMesh();
	pEdContCrv->CreateMesh();
	pEdAuxCrv->CreateMesh();
	GMesh *pStSubMesh = create_mesh_from_two_curves_on_mesh(pStMesh, pStVert, pStContCrv, pStAuxCrv, snum_u, snum_v, trim);
	GMesh *pEdSubMesh = create_mesh_from_two_curves_on_mesh(pEdMesh, pEdVert, pEdAuxCrv, pEdContCrv, snum_u, snum_v, trim);

	// �� �κ� �޽��� ���� �Ķ���͸� �����Ѵ�.
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

	// �޽��� ���� ��, ������ �߰��Ѵ�.
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

	// �ﰢ���� �����ϰ� �߰��Ѵ�.
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

	// Ʈ������ �Ѵٸ�,
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

		// ���� �κп� �긮�� �޽��� ��������� �߰��Ѵ�.
		connect_mesh_with_verts(pStMesh, BrdgStVerts);
		connect_mesh_with_verts(pEdMesh, BrdgEdVerts);
	}	

	// �޸� ���� �� ��� �޽� ��ȯ
	delete pStSubMesh;
	delete pEdSubMesh;
	delete pStAuxCrv;
	delete pEdAuxCrv;
	pEdContCrv->Reverse();

	return pRetMesh;
}

/*!
*	\brief ������ �긮�� �޽��� �����ϴ� �Լ�
*
*	\param pBridgeMesh ������ �긮�� �޽�
*	\param pStMesh ���� �޽�
*	\param pStVert ���� �޽��� ��ǥ�� �߽� ����
*	\param pStContCrv ���� �޽����� ���� �
*	\param alpha ���� �޽��� ���� ��� ������ �Ķ����
*	\param pEdMesh ���� �޽�
*	\param pEdVert ���� �޽��� ��ǥ�� �߽� ����
*	\param pEdContCrv ���� �޽����� ���� �
*	\param beta ���� �޽��� ���� ��� ������ �Ķ����
*	\param mu ���� �Լ��� �Ķ����
*	\param snum_u U-���� ���ø� ��
*	\param snum_v V-���� ���ø� ��
*	\param trim ���� �޽��� �� �޽��� Ʈ���� ����
*
*	\return ������ �޽��� ��ȯ�Ѵ�.
*/
void update_mesh_bridge(GMesh *pBridgeMesh,
						GMesh *pStMesh, GVertex *pStVert, GNurbsCrv *pStContCrv, double alpha,
						GMesh *pEdMesh, GVertex *pEdVert, GNurbsCrv *pEdContCrv, double beta,
						double mu, int snum_u, int snum_v, bool trim)
{
	// ���� �޽��� �κ� �޽��� �� �޽��� �κ� �޽��� ���Ѵ�.
	GNurbsCrv *pStAuxCrv = pStContCrv->CopyObject();
	pEdContCrv->Reverse();
	GNurbsCrv *pEdAuxCrv = pEdContCrv->CopyObject();
	pStAuxCrv->Scale(alpha);
	pEdAuxCrv->Scale(beta);

	// ���� �޽��� �κ� �޽��� �� �޽��� �κ� �޽��� ���Ѵ�.
	pStContCrv->CreateMesh();
	pStAuxCrv->CreateMesh();
	pEdContCrv->CreateMesh();
	pEdAuxCrv->CreateMesh();
	GMesh *pStSubMesh = create_mesh_from_two_curves_on_mesh(pStMesh, pStVert, pStContCrv, pStAuxCrv, snum_u, snum_v, trim);
	GMesh *pEdSubMesh = create_mesh_from_two_curves_on_mesh(pEdMesh, pEdVert, pEdAuxCrv, pEdContCrv, snum_u, snum_v, trim);

	// �� �κ� �޽��� ���� �Ķ���͸� �����Ѵ�.
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

	// �޽��� ���� ��, ������ �߰��Ѵ�.
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

	// Ʈ������ �Ѵٸ�,
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

		// ���� �κп� �긮�� �޽��� ��������� �߰��Ѵ�.
		connect_mesh_with_verts(pStMesh, BrdgStVerts);
		connect_mesh_with_verts(pEdMesh, BrdgEdVerts);
	}	

	// �޸� ���� �� ��� �޽� ��ȯ
	delete pStSubMesh;
	delete pEdSubMesh;
	delete pStAuxCrv;
	delete pEdAuxCrv;
	pEdContCrv->Reverse();
}

/*! 
*	\brief �޽��� �߰� ������ �����ϴ� �ﰢ���� �޽��� �߰��ϴ� �Լ�
*
*	\param pMesh	Ȯ���� �޽�
*	\param Verts	�߰��� ������ ����Ʈ
*/
void connect_mesh_with_verts(GMesh *pMesh, std::vector<GPoint3> &Verts)
{
	std::map<int, int> BndryEdges;
	::get_bndry_edges(pMesh, BndryEdges);

	// ���� �޽��� �������� ������ ���� ����� ��� ������ ���� �ε����� ã�´�.
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

	// ���� �ε����κ��� ��� ������ �ε����� ���Ѵ�.
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

	// ���̽� ���� �ε����� ���ϰ� �긮�� ���� ��� ������ �߰��Ѵ�.
	int base_vidx = pMesh->GetNumVert();
	int NumVert = (int)Verts.size();
	for (int i = 0; i < NumVert; ++i)
		pMesh->AddVertex(GVertex(Verts[i]));

	int prev_idx = -1;
	int MtlIdx = pMesh->Faces[0].MtlIdx;
	std::string grp_name = (!pMesh->GrpFaces.empty()) ? pMesh->GrpFaces.begin()->first : "";

	for (int i = 0; i < NumVert; ++i)
	{
		// �߰��� ������ �� ������ ���Ͽ�,
		GPoint3 pt0 = pMesh->Verts[base_vidx + i].P;
		GPoint3 pt1 = pMesh->Verts[base_vidx + ((i + 1) % NumVert)].P;

		// ���� ����� ��� ������ �� �ε����� ���Ѵ�.
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
*	\brief �޽��� ���� ����� �����ϴ� �긮�� �޽��� �����Ѵ�.
*
*	\param pMesh	�޽�
*	\param pVert	�޽����� ���� �߽� ����
*	\param ContRad	�޽����� ���� ���� ������ (ContRad > AuxRad)
*	\param AuxRad	�޽����� ���� ���� ������ (ContRad > AuxRad)
*	\param pSrf		���� ���
*	\param ContParam ���� ����� ���� ��� �Ķ���� (ContParam > AuxParam)
*	\param AuxParam  ���� ����� ���� ��� �Ķ���� (ContParam > AuxParam)
*	\param snum_u U-���� ���ø� ��
*	\param snum_v V-���� ���ø� ��
*
*	\return ������ �޽��� ��ȯ�Ѵ�.
*/
GMesh *create_mesh_bridge(GMesh *pMesh, GVertex *pVert, double ContRad, double AuxRad, 
						  GSweepSrf *pSrf, double ContParam, double AuxParam, 
						  int snum_u, int snum_v)
{
	// ���� �޽��� �κ� �޽��� ���Ѵ�. �̶�, ContRad > AuxRad �� �����ؾ� �Ѵ�.
	GMesh *pStSubMesh = ::create_mesh_from_two_circles_on_mesh(pMesh, pVert, ContRad, AuxRad, snum_u, snum_v);

	// ���� ������κ��� �� �޽��� �κ� �޽��� ���Ѵ�. �̶�, ContParam > AuxParam �� �����ؾ� �Ѵ�.
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

	// �޽��� ���� ��, ������ �߰��Ѵ�.
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

	// �ﰢ���� �����ϰ� �߰��Ѵ�.
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

	// �޸� ���� �� ��� �޽� ��ȯ
	delete pStSubMesh;
	delete pEdSubMesh;
	return pRetMesh;
}