#include "stdafx.h"

/*!
*	\brief 새로 생성할 객체의 이름을 생성한다.
*
*	\param TypeObj 생성될 객체의 타입
*
*	\return 새로 생성할 객체의 이름을 반환한다.
*/
std::string create_mesh_name(std::string TypeObj)
{
	static std::map<std::string, int> ObjIdxMap;

	int id = ObjIdxMap[TypeObj];
	std::string name = TypeObj + "_" + cast_str(id);
	ObjIdxMap[TypeObj]++;

	return name;
}

/*!
*	\brief 새로 생성할 재질 이름을 생성한다.
*
*	\param Prefix 생성될 재질 접두어
*
*	\return 새로 생성할 재질의 이름을 반환한다.
*/
std::string create_mtl_name(std::string Prefix)
{
	static std::map<std::string, int> PrefixIdxMap;

	int id = PrefixIdxMap[Prefix];
	std::string name = Prefix + "_" + cast_str(id);
	PrefixIdxMap[Prefix]++;

	return name;
}

/*!
*	\brief 3차원 실린더(Cylinder) 메쉬를 생성한다.
*
*	\param pAxisCrv 실린더의 중심 곡선
*
*	\return 생성된 실린더 메쉬를 반환한다.
*/
GMesh *create_mesh_from_cylinder(GNurbsCrv *pAxisCrv)
{
	double radius = pAxisCrv->Renderer.CtrlPtSize * 0.4;
	int sv = pAxisCrv->Renderer.NumSampU;
	int su = 20;

	// RMF 리스트를 계산한다.
	std::vector<GTransf> FrameList;
	::get_rotation_minimizing_frames(pAxisCrv, sv, FrameList);

	// 메쉬를 생성하고,
	std::string Name = create_mesh_name("Cylinder");
	GMesh *pCylinder = new GMesh(Name, pAxisCrv->MC, FACE_QUAD, "");

	// 곡면위의 정점을 계산한다.
	GVector4 pt;
	for (int i = 0; i < su; ++i)
	{
		double u = (double)i / (double)(su - 1);
		double z = radius * cos(2.0 * u * M_PI);
		double x = radius * sin(2.0 * u * M_PI);

		for (int j = 0; j < sv; ++j)
		{
			GPoint3 pt = FrameList[j] * GPoint3(x, 0.0, z);
			GVertex vtx(pt, GVector3(0.0, 0.0, 0.0));
			pCylinder->AddVertex(vtx);
		}
	}

	// 삼각형을 생성하고 추가한다.
	for (int i = 0; i < su - 1; ++i)
	{
		for(int j = 0; j < sv - 1; ++j)
		{
			int vidx0 = i * sv + j;
			int vidx1 = vidx0 + sv;
			int vidx2 = vidx1 + 1;
			int vidx3 = vidx0 + 1;

			GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
			GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);

			pCylinder->AddFace(f0);
			pCylinder->AddFace(f1);
		}
	}

	return pCylinder;
}

/*!
*	\brief 3차원 구(sphere) 메쉬를 생성한다.
*
*	\param Name 객체의 이름
*	\param MC 객체의 모델링 좌표계
*	\param radius 반지름
*	\param slices 위도 곡선의 개수
*	\param stacks 경도 곡선의 개수
*
*	\return 생성된 구 메쉬를 반환한다.
*/
GMesh *create_mesh_from_sphere(std::string Name, GTransf MC, double radius, int slices, int stacks)
{
	// 구를 생성하고,
	GMesh *pSphere = new GMesh(Name, MC, FACE_TRI);

	for (int i = 0; i < slices; ++i)
	{
		double u = (double)i / (double)(slices - 1);
		double theta = M_PI * u;
		for (int j = 0; j < stacks; ++j)
		{
			double v = (double)j / (double)(stacks);
			double phi = 2 * M_PI * v;

			double x = radius * sin(theta) * cos(phi);
			double y = radius * sin(theta) * sin(phi);
			double z = radius * cos(theta);

			pSphere->AddVertex(GVertex(GPoint3(x, y, z), GVector3()));
			pSphere->AddTexel(GVector2(u, v));
		}
	}

	// 삼각형을 생성하고 추가한다.
	for (int i = 0; i < slices - 1; ++i)
	{
		for(int j = 0; j < stacks; ++j)
		{
			int vidx0 = i * stacks + j;
			int vidx1 = i * stacks + (vidx0 + 1) % stacks;
			int vidx2 = vidx1 + stacks;
			int vidx3 = vidx0 + stacks;

			GFace f0(vidx0, vidx2, vidx1, vidx0, vidx2, vidx1, -1);
			GFace f1(vidx0, vidx3, vidx2, vidx0, vidx3, vidx2, -1);

			pSphere->AddFace(f0);
			pSphere->AddFace(f1);
		}
	}

	return pSphere;
}

/*!
*	\brief 3차원 icosahedron 메쉬를 생성한다.
*
*	\param Name 객체의 이름
*	\param MC 객체의 모델링 좌표계
*	\param radius 반지름
*	\param subdiv_depth 초기 메쉬 생성 후 분할 횟수
*
*	\return 생성된 icosahedron 메쉬를 반환한다.
*/
GMesh *create_mesh_from_icosahedron(std::string Name, GTransf MC, double radius, int subdiv_depth)
{
	GMesh *pSphere = new GMesh(Name, MC, FACE_TRI);
	
	// 12 개의 정점을 추가한다.
	const double t = (1.0 + std::sqrt(5.0)) / 2.0; 
	pSphere->AddVertex(GVertex(GPoint3(-1.0,  t, 0.0), GVector3()));
	pSphere->AddVertex(GVertex(GPoint3( 1.0,  t, 0.0), GVector3()));
	pSphere->AddVertex(GVertex(GPoint3(-1.0, -t, 0.0), GVector3()));
	pSphere->AddVertex(GVertex(GPoint3( 1.0, -t, 0.0), GVector3()));
	pSphere->AddVertex(GVertex(GPoint3(0.0, -1.0,  t), GVector3()));
	pSphere->AddVertex(GVertex(GPoint3(0.0,  1.0,  t), GVector3()));
	pSphere->AddVertex(GVertex(GPoint3(0.0, -1.0, -t), GVector3()));
	pSphere->AddVertex(GVertex(GPoint3(0.0,  1.0, -t), GVector3()));
	pSphere->AddVertex(GVertex(GPoint3( t, 0.0, -1.0), GVector3()));
	pSphere->AddVertex(GVertex(GPoint3( t, 0.0,  1.0), GVector3()));
	pSphere->AddVertex(GVertex(GPoint3(-t, 0.0, -1.0), GVector3()));
	pSphere->AddVertex(GVertex(GPoint3(-t, 0.0,  1.0), GVector3()));
	
	// 20개의 삼각형을 추가한다.
	pSphere->AddFace(GFace(0, 5, 1, 0, 5, 1, -1));
	pSphere->AddFace(GFace(0, 1, 7, 0, 1, 7, -1));
	pSphere->AddFace(GFace(0, 7, 10, 0, 7, 10, -1));
	pSphere->AddFace(GFace(0, 10, 11, 0, 10, 11, -1));
	pSphere->AddFace(GFace(1, 5, 9, 1, 5, 0, -1));
	pSphere->AddFace(GFace(5, 11, 4, 5, 11, 4, -1));
	pSphere->AddFace(GFace(11, 10, 2, 11, 10, 2, -1));
	pSphere->AddFace(GFace(10, 7, 6, 10, 7, 6, -1));
	pSphere->AddFace(GFace(7, 1, 8, 7, 1, 8, -1));
	pSphere->AddFace(GFace(3, 9, 4, 3, 9, 4, -1));
	pSphere->AddFace(GFace(3, 4, 2, 3, 4, 2, -1));
	pSphere->AddFace(GFace(3, 2, 6, 3, 2, 6, -1));
	pSphere->AddFace(GFace(3, 6, 8, 3, 6, 8, -1));
	pSphere->AddFace(GFace(3, 8, 9, 3, 8, 9, -1));
	pSphere->AddFace(GFace(4, 9, 5, 4, 9, 5, -1));
	pSphere->AddFace(GFace(2, 4, 11, 2, 4, 11, -1));
	pSphere->AddFace(GFace(6, 2, 10, 6, 2, 10, -1));
	pSphere->AddFace(GFace(8, 6, 7, 8, 6, 7, -1));
	pSphere->AddFace(GFace(9, 8, 1, 9, 8, 1, -1));
	pSphere->AddFace(GFace(0, 11, 5, 0, 11, 5, -1));

	for (int i = 0; i < subdiv_depth; ++i)
		edit_mesh_by_refine(pSphere);

	// 정규화 및 변환
	for (int i = 0; i < pSphere->GetNumVert(); ++i)
	{
		double len = norm(pSphere->Verts[i].P);
		pSphere->Verts[i].P[0] /= len;
		pSphere->Verts[i].P[1] /= len;
		pSphere->Verts[i].P[2] /= len;
		pSphere->Verts[i].P[0] *= radius;
		pSphere->Verts[i].P[1] *= radius;
		pSphere->Verts[i].P[2] *= radius;
	}
	
	pSphere->ModifiedFlag = MODIFIED_VERT_POS_ALL;
	pSphere->UpdateMesh();

	return pSphere;
}

/*!
*	\brief Teapot 메쉬를 생성한다.
*
*	\param size 생성될 메쉬의 크기
*
*	\return 생성된 Teapot메쉬를 반환한다.
*/
GMesh *create_mesh_from_teapot(double size)
{
	std::string Name = create_mesh_name("Teapot");

	GMesh *pTeapot = new GMesh(Name, GTransf(), FACE_TRI);

	return pTeapot;
}

/*!
*	\brief 3차원 박스 메쉬를 생성한다.
*
*	\param p 모델링 좌표계에서의 최소점
*	\param q 모델링 좌표계에서의 최소점
*
*	\return 생성된 박스 메쉬를 반환한다.
*/
GMesh *create_mesh_from_box(GPoint3 p, GPoint3 q)
{
	std::string Name = create_mesh_name("Box");

	GMesh *pBox = new GMesh(Name, GTransf(), FACE_QUAD);

	GPoint3 P[8];
	P[0].Set(p[0], p[1], p[2]);
	P[1].Set(p[0], p[1], q[2]);
	P[2].Set(q[0], p[1], q[2]);
	P[3].Set(q[0], p[1], p[2]);

	P[4].Set(p[0], q[1], p[2]);
	P[5].Set(p[0], q[1], q[2]);
	P[6].Set(q[0], q[1], q[2]);
	P[7].Set(q[0], q[1], p[2]);

	for (int i = 0; i < 8; ++i)
		pBox->AddVertex(GVertex(P[i]));

	pBox->AddFace(GFace(2, 1, 0));
	pBox->AddFace(GFace(2, 0, 3));

	pBox->AddFace(GFace(4, 5, 6));
	pBox->AddFace(GFace(4, 6, 7));

	pBox->AddFace(GFace(4, 0, 1));
	pBox->AddFace(GFace(4, 1, 5));

	pBox->AddFace(GFace(6, 2, 3));
	pBox->AddFace(GFace(6, 3, 7));

	pBox->AddFace(GFace(5, 1, 2));
	pBox->AddFace(GFace(5, 2, 6));

	pBox->AddFace(GFace(4, 0, 3));
	pBox->AddFace(GFace(4, 3, 7));

	pBox->ModifiedFlag = MODIFIED_VERT_NUM;
	pBox->UpdateMesh();

	return pBox;
}

/*!
*	\brief 부모 메쉬의 부분 메쉬(submesh)를 생성한다.
*
*	\param Name 부분 메쉬의 이름
*	\param pParentMesh 부모 메쉬에 대한 포인터
*	\param SltFaceList 부모 메쉬의 선택된 삼각형 리스트
*	\param VertIdxMap 부분 메쉬와 부모 메쉬의 정점의 대응관계가 저장된다. 부분 메쉬 정점의 인덱스 = VertIdxMap[부모 메쉬 정점의 인덱스].
*
*	\return 생성된 부분 메쉬를 반환한다.
*/
GMesh *create_mesh_from_faces(std::string Name, GMesh *pParentMesh, std::vector<GFace *> &SltFaceList, std::map<int, int> &VertIdxMap)
{
	// 선택된 삼각형으로 구성될 새로운 메쉬를 생성하고,
	GMesh *pSubMesh = new GMesh(Name, pParentMesh->MC, FACE_TRI, pParentMesh->Name);

	// 각각의 삼각형에 대하여,
	int NumObj = (int)SltFaceList.size();
	for (int i = 0; i < NumObj; ++i)
	{
		// 삼각형의 정점의 인덱스를 구하고,
		GFace *pFace = (GFace *)(SltFaceList[i]);
		int vidx0 = pFace->vIdx[0];
		int vidx1 = pFace->vIdx[1];
		int vidx2 = pFace->vIdx[2];

		// 정점을 복사하고,
		GVertex v0 = pParentMesh->Verts[vidx0];
		GVertex v1 = pParentMesh->Verts[vidx1];
		GVertex v2 = pParentMesh->Verts[vidx2];

		// 새 정점이면 추가하고,
		if (VertIdxMap[vidx0] == 0)
		{
			pSubMesh->AddVertex(v0);
			VertIdxMap[vidx0] = pSubMesh->GetNumVert();
		}

		// 새 정점이면 추가하고,
		if (VertIdxMap[vidx1] == 0)
		{
			pSubMesh->AddVertex(v1);
			VertIdxMap[vidx1] = pSubMesh->GetNumVert();
		}

		// 새 정점이면 추가하고,
		if (VertIdxMap[vidx2] == 0)
		{
			pSubMesh->AddVertex(v2);
			VertIdxMap[vidx2] = pSubMesh->GetNumVert();
		}

		// 새 정점의 집합의 인덱스를 구하여,
		vidx0 = VertIdxMap[vidx0] - 1;
		vidx1 = VertIdxMap[vidx1] - 1;
		vidx2 = VertIdxMap[vidx2] - 1;

		// 삼각형을 생성하여 추가한다.
		GFace f(vidx0, vidx1, vidx2);
		pSubMesh->AddFace(f);
	}

	// 경계 상자와 법선을 계산하고,
	pSubMesh->UpdateBndBox(false);
	pSubMesh->UpdateNormal(false);
	//pSubMesh->Edit(GVector3(), GQuater(), GVector3(1.0, 1.0, 1.0), pParentMesh);

	// 정점의 인덱스 매핑 관계를 수정하고,
	std::map<int, int>::iterator it;
	for (it = VertIdxMap.begin(); it != VertIdxMap.end(); ++it)
		it->second--;		

	// 생성된 부분 메쉬를 반환한다.
	return pSubMesh;	
}

/*!
*	\brief 2차원 공간에 사각형으로 매개화된 메쉬를 생성한다.
*
*	\param Name 매개화 메쉬의 이름
*	\param pMesh 매개화될 메쉬
*	\param StartVertIdx 파라미터 값(0.5, 0.0)에 대응될 시작 정점의 인덱스
*
*	\return 매개화된 메쉬를 반환한다.
*/
static GMesh *create_mesh_unfolded_square(std::string Name, GMesh *pMesh, int StartVertIdx)
{
	// 매개화된 2차원 결과 메쉬를 생성하고,
	GMesh *pRetMesh = new GMesh(Name, GTransf(), pMesh->FaceType, "");

	// 대상 메쉬로 초기화 한다.
	pRetMesh->Verts = pMesh->Verts;
	pRetMesh->Faces = pMesh->Faces;

	// 메쉬들의 정점 리스트를 구하고,
	std::vector<GVertex> &Verts = pMesh->Verts;
	std::vector<GVertex> &RetVerts = pRetMesh->Verts;

	////////////////////////////////////////////////////////////////////////
	// 1 단계: 선형 시스템 Ax = b를 구성하기 위한 계수 행렬 A를 계산한다. //
	////////////////////////////////////////////////////////////////////////

	// 내부 정점 리스트와 개수를 구하고,
	std::vector<int> IntrVertList;
	::get_intr_verts(pMesh, IntrVertList);
	
	// 내부 정점을 둘러싼 이웃 정점의 리스트를 구하고,
	std::vector<std::vector<int>> AdjVertList;
	::get_neighbor_verts_of_vert(pMesh, AdjVertList);

	// 각각의 내부 정점에 대하여 연결된 정점간의 가중치를 계산한다. (Barycentric 좌표를 이용하여)
	std::map<std::string, float> EdgeWeightMap;
	int NumIntVert = (int)IntrVertList.size();
	for (int i = 0; i < NumIntVert; ++i)
	{		
		// 내부 정점을 둘러싼 이웃 정점의 개수를 구하고,
		int vidx0 = IntrVertList[i];
		int NumVert = (int)AdjVertList[vidx0].size();

		// 각각의 이웃 정점마다,
		float tot_weight = 0.0;
		for (int j = 0; j < NumVert; ++j)
		{
			int vidx1 = (j == 0) ? AdjVertList[vidx0][NumVert - 1] : AdjVertList[vidx0][j - 1];
			int vidx2 = AdjVertList[vidx0][j];
			int vidx3 = (j == NumVert - 1) ? AdjVertList[vidx0][0] : AdjVertList[vidx0][j + 1];

			float len = dist(Verts[vidx0].P, Verts[vidx2].P);
			GVector3 a = Verts[vidx1].P - Verts[vidx0].P;
			GVector3 b = Verts[vidx2].P - Verts[vidx0].P;
			a.Normalize();
			b.Normalize();
			float theta1 = (1.0 - a * b) / norm(a ^ b);

			GVector3 c = Verts[vidx2].P - Verts[vidx0].P;
			GVector3 d = Verts[vidx3].P - Verts[vidx0].P;
			c.Normalize();
			d.Normalize();
			float theta2 = (1.0 - c * d) / norm(c ^ d);

			// 내부 정점과 연결된 가중치를 계산하고,
			float weight = (theta1 + theta2) / len;
			tot_weight = tot_weight + weight;

			// 가중치 맵에 기록한 후,
			std::string key = cast_str(vidx0) + "-" + cast_str(vidx2);
			EdgeWeightMap[key] = weight;
		}

		// 가중치 맵을 정규화 한다.
		for (int j = 0; j < NumVert; ++j)
		{
			int vidx2 = AdjVertList[vidx0][j];
			std::string key = cast_str(vidx0) + "-" + cast_str(vidx2);
			EdgeWeightMap[key] = EdgeWeightMap[key] / tot_weight;
		}
	}

	// 가중치 맵을 참고하여, 선형시스템 (Ax1 = b1, Ax2 = b2)의 계수 행렬(A)을 구한다.
	Eigen::SparseMatrix<float> A(NumIntVert, NumIntVert);
	A.setZero();
	for (int i = 0; i < NumIntVert; ++i)
	{
		int vidx0 = IntrVertList[i];
		for (int j = 0; j < NumIntVert; ++j)
		{
			int vidx1 = IntrVertList[j];
			std::string key = cast_str(vidx0) + "-" + cast_str(vidx1);
			std::map<std::string, float>::iterator it = EdgeWeightMap.find(key);
			double lambda = (it == EdgeWeightMap.end()) ? 0.0 : it->second;
			if (i == j)
				A.insert(i, j) = 1.0;
			else if (lambda != 0.0)
				A.insert(i, j) = -lambda;
		}
	}
	A.makeCompressed();

	////////////////////////////////////////////////////////////////////////
	// 2 단계: 선형 시스템 Ax = b를 구성하기 위한 상수 벡터 b를 계산한다. //
	////////////////////////////////////////////////////////////////////////

	// 경계 정점 리스트를 구하고,
	std::vector<std::vector<int>> BoundaryVertList;
	::get_bndry_verts(pMesh, BoundaryVertList);

	// 파라미터(0.5, 0.0)의 시작 정점이 주어진다면, 
	int NumBoundVert = (int)BoundaryVertList[0].size();
	if (StartVertIdx != -1)
	{
		std::vector<int> tmpVertList;
		int vidx = -1;
		for (int i = 0; i < NumBoundVert; ++i)
		{
			if (StartVertIdx == BoundaryVertList[0][i])
			{
				vidx = i;
				break;
			}
		}

		if (vidx != -1)
		{
			// 경계 정점의 리스트를 재정렬한다.
			for (int i = vidx; i < NumBoundVert; ++i)
				tmpVertList.push_back(BoundaryVertList[0][i]);

			for (int i = 0; i < vidx; ++i)
				tmpVertList.push_back(BoundaryVertList[0][i]);

			BoundaryVertList[0] = tmpVertList;
		}		
	}

	// 총 둘레의 길이를 계산하고,
	double BoundLen = 0.0;
	for (int i = 0; i < NumBoundVert; ++i)
	{
		int vidx0 = (i == 0) ? BoundaryVertList[0][NumBoundVert - 1] : BoundaryVertList[0][i - 1];
		int vidx1 = BoundaryVertList[0][i];
		BoundLen = BoundLen + dist(Verts[vidx0].P, Verts[vidx1].P);
	}

	// 둘레를 사각형에 대응시킬 한변의 길이를 구한다.
	double Delta = BoundLen / 4.0;

	// 경계의 시작점에 (0.5, 0.0)의 파라미터 값을 할당하고,
	RetVerts[BoundaryVertList[0][0]].P.Set(0.5, 0.0, 0.0);
	BoundLen = Delta / 2.0;

	// 둘레의 길이와 사각형 한변의 길이를 이용하여 나머지 경계 정점의 파라미터 값을 계산한다.
	int vidx00, vidx10, vidx11, vidx01;
	double min_dist00 = 100.0, min_dist10 = 100.0, min_dist11 = 100.0, min_dist01 = 100.0;
	for (int i = 1; i < NumBoundVert; ++i)
	{
		int vidx0 = BoundaryVertList[0][i - 1];
		int vidx1 = BoundaryVertList[0][i];

		BoundLen = BoundLen + dist(Verts[vidx0].P, Verts[vidx1].P);
		int q = (int)(BoundLen / Delta);
		double r = BoundLen / Delta - q;

		switch (q)
		{
		case 0:
			RetVerts[vidx1].P.Set(r, 0.0, 0.0);
			break;
		case 1:
			RetVerts[vidx1].P.Set(1.0, r, 0.0);
			break;
		case 2:
			RetVerts[vidx1].P.Set(1.0 - r, 1.0, 0.0);
			break;
		case 3:
			RetVerts[vidx1].P.Set(0.0, 1.0 - r, 0.0);
			break;
		case 4:
			RetVerts[vidx1].P.Set(r, 0.0, 0.0);
			break;
		}

		double dist00 = dist(GPoint3(0.0, 0.0, 0.0), RetVerts[vidx1].P);
		double dist10 = dist(GPoint3(1.0, 0.0, 0.0), RetVerts[vidx1].P);
		double dist11 = dist(GPoint3(1.0, 1.0, 0.0), RetVerts[vidx1].P);
		double dist01 = dist(GPoint3(0.0, 1.0, 0.0), RetVerts[vidx1].P);

		if (dist00 < min_dist00)
		{
			vidx00 = vidx1;
			min_dist00 = dist00;
		}

		if (dist10 < min_dist10)
		{
			vidx10 = vidx1;
			min_dist10 = dist10;
		}

		if (dist11 < min_dist11)
		{
			vidx11 = vidx1;
			min_dist11 = dist11;
		}

		if (dist01 < min_dist01)
		{
			vidx01 = vidx1;
			min_dist01 = dist01;
		}
	}
	RetVerts[vidx00].P.Set(0.0, 0.0, 0.0);
	RetVerts[vidx10].P.Set(1.0, 0.0, 0.0);
	RetVerts[vidx11].P.Set(1.0, 1.0, 0.0);
	RetVerts[vidx01].P.Set(0.0, 1.0, 0.0);

	// 선형시스템의 상수 벡터(b1, b2)를 구한다.
	Eigen::VectorXf b1(NumIntVert), b2(NumIntVert);
	b1.setZero();
	b2.setZero();
	for (int i = 0; i < NumIntVert; ++i)
	{
		int vidx0 = IntrVertList[i];
		for (int j = 0; j < NumBoundVert; ++j)
		{
			int vidx1 = BoundaryVertList[0][j];

			std::string key = cast_str(vidx0) + "-" + cast_str(vidx1);
			std::map<std::string, float>::iterator it;
			it = EdgeWeightMap.find(key);
			double lambda = (it == EdgeWeightMap.end()) ? 0.0 : it->second;

			b1(i) = b1(i) + lambda * RetVerts[vidx1].P[0];
			b2(i) = b2(i) + lambda * RetVerts[vidx1].P[1];
		}
	}

	//////////////////////////////////////////////
	// 3 단계: 선형 시스템 Ax = b의 해를 구한다.//
	//////////////////////////////////////////////
	// 선형시스템의 해(x1, x2)를 구한다.
	Eigen::VectorXf x1(NumIntVert), x2(NumIntVert);
	Eigen::SparseLU<Eigen::SparseMatrix<float>> solver;
	solver.compute(A);

	x1 = solver.solve(b1);
	x2 = solver.solve(b2);

	// 내부 정점의 파라미터 값을 설정한다.
	for (int i = 0; i < NumIntVert; ++i)
	{
		int vidx = IntrVertList[i];
		RetVerts[vidx].P.Set(x1[i], x2[i], 0.0);
	}

	pRetMesh->ModifiedFlag = MODIFIED_VERT_NUM;
	pRetMesh->UpdateMesh();
	return pRetMesh;
}

/*!
*	\brief 2차원 공간에 원의 형태로 매개화된 메쉬를 생성한다.
*
*	\param Name 매개화 메쉬의 이름
*	\param pMesh 매개화될 메쉬
*	\param StartVertIdx 파라미터 값(1.0, 0.0)에 대응될 시작 정점의 인덱스
*
*	\return 매개화된 메쉬를 반환한다.
*/
GMesh *create_mesh_unfolded_circle(std::string Name, GMesh *pMesh, int StartVertIdx)
{
	// 매개화된 2차원 결과 메쉬를 생성하고,
	GMesh *pRetMesh = new GMesh(Name, GTransf(), pMesh->FaceType, "");

	// 대상 메쉬로 초기화 한다.
	pRetMesh->Verts = pMesh->Verts;
	pRetMesh->Faces = pMesh->Faces;
	
	// 메쉬들의 정점 리스트를 구하고,
	std::vector<GVertex> &Verts = pMesh->Verts;
	std::vector<GVertex> &RetVerts = pRetMesh->Verts;

	////////////////////////////////////////////////////////////////////////
	// 1 단계: 선형 시스템 Ax = b를 구성하기 위한 계수 행렬 A를 계산한다. //
	////////////////////////////////////////////////////////////////////////

	// 내부 정점 리스트를 구하고,
	std::vector<int> IntrVertList;
	::get_intr_verts(pMesh, IntrVertList);
	
	// 내부 정점을 둘러싼 이웃 정점의 리스트를 구하고,
	std::vector<std::vector<int>> AdjVertList;
	::get_neighbor_verts_of_vert(pMesh, AdjVertList);

	// 각각의 내부 정점에 대하여 연결된 정점간의 가중치를 계산한다. (Barycentric 좌표를 이용하여)
	std::map<std::string, float> EdgeWeightMap;
	int NumIntVert = (int)IntrVertList.size();
	for (int i = 0; i < NumIntVert; ++i)
	{		
		// 내부 정점을 둘러싼 이웃 정점의 개수를 구하고,
		int vidx0 = IntrVertList[i];
		int NumVert = (int)AdjVertList[vidx0].size();

		// 각각의 이웃 정점마다,
		float tot_weight = 0.0;
		for (int j = 0; j < NumVert; ++j)
		{
			int vidx1 = (j == 0) ? AdjVertList[vidx0][NumVert - 1] : AdjVertList[vidx0][j - 1];
			int vidx2 = AdjVertList[vidx0][j];
			int vidx3 = (j == NumVert - 1) ? AdjVertList[vidx0][0] : AdjVertList[vidx0][j + 1];

			GVector3 a = (Verts[vidx1].P - Verts[vidx0].P).Normalize();
			GVector3 b = (Verts[vidx2].P - Verts[vidx0].P).Normalize();
			GVector3 c = (Verts[vidx2].P - Verts[vidx0].P).Normalize();
			GVector3 d = (Verts[vidx3].P - Verts[vidx0].P).Normalize();

			float theta1 = (1.0 - a * b) / norm(a ^ b);
			float theta2 = (1.0 - c * d) / norm(c ^ d);

			// 내부 정점과 연결된 가중치를 계산하고,
			float weight = (theta1 + theta2) / dist(Verts[vidx0].P, Verts[vidx2].P);
			tot_weight = tot_weight + weight;

			// 가중치 맵에 기록한 후,
			std::string key = cast_str(vidx0) + "-" + cast_str(vidx2);
			EdgeWeightMap[key] = weight;
		}

		// 가중치 맵을 정규화 한다.
		for (int j = 0; j < NumVert; ++j)
		{
			int vidx2 = AdjVertList[vidx0][j];
			std::string key = cast_str(vidx0) + "-" + cast_str(vidx2);
			EdgeWeightMap[key] = EdgeWeightMap[key] / tot_weight;
		}
	}

	// 가중치 맵을 참고하여, 선형시스템 (Ax1 = b1, Ax2 = b2)의 계수 행렬(A)을 구한다.
	Eigen::SparseMatrix<float> A(NumIntVert, NumIntVert);
	A.setZero();
	for (int i = 0; i < NumIntVert; ++i)
	{
		int vidx0 = IntrVertList[i];
		for (int j = 0; j < NumIntVert; ++j)
		{
			int vidx1 = IntrVertList[j];
			std::string key = cast_str(vidx0) + "-" + cast_str(vidx1);
			std::map<std::string, float>::iterator it = EdgeWeightMap.find(key);
			double lambda = (it == EdgeWeightMap.end()) ? 0.0 : it->second;
			if (i == j)
				A.insert(i, j) = 1.0;
			else if (lambda != 0.0)
				A.insert(i, j) = -lambda;
		}
	}
	A.makeCompressed();

	////////////////////////////////////////////////////////////////////////
	// 2 단계: 선형 시스템 Ax = b를 구성하기 위한 상수 벡터 b를 계산한다. //
	////////////////////////////////////////////////////////////////////////

	// 경계 정점 리스트를 구하고,
	std::vector<std::vector<int>> BndryVertList;
	::get_bndry_verts(pMesh, BndryVertList);

	// 파라미터의 시작 정점이 주어진다면, 
	int NumBndryVert = (int)BndryVertList[0].size();
	if (StartVertIdx != -1)
	{
		std::vector<int> tmpVertList;
		int vidx = -1;
		for (int i = 0; i < NumBndryVert; ++i)
		{
			if (StartVertIdx == BndryVertList[0][i])
			{
				vidx = i;
				break;
			}
		}

		if (vidx != -1)
		{
			// 경계 정점의 리스트를 재정렬한다.
			for (int i = vidx; i < NumBndryVert; ++i)
				tmpVertList.push_back(BndryVertList[0][i]);

			for (int i = 0; i < vidx; ++i)
				tmpVertList.push_back(BndryVertList[0][i]);

			BndryVertList[0] = tmpVertList;
		}		
	}

	// 총 둘레의 길이를 계산하고,
	double BndryLen = 0.0;
	for (int i = 0; i < NumBndryVert; ++i)
		BndryLen = BndryLen + dist(Verts[BndryVertList[0][i]].P, Verts[BndryVertList[0][(i + 1) % NumBndryVert]].P);

	// 경계 정점의 좌표를 결정한다.
	double CurrLen = 0.0;
	RetVerts[BndryVertList[0][0]].P.Set(1.0, 0.0, 0.0);
	for (int i = 1; i < NumBndryVert; ++i)
	{
		CurrLen = CurrLen + dist(Verts[BndryVertList[0][i]].P, Verts[BndryVertList[0][i - 1]].P);
		double theta = 2.0 * M_PI * (CurrLen / BndryLen);
		RetVerts[BndryVertList[0][i]].P.Set(cos(theta), sin(theta), 0.0);
	}

	// 선형시스템의 상수 벡터(b1, b2)를 구한다.
	Eigen::VectorXf b1(NumIntVert), b2(NumIntVert);
	b1.setZero();
	b2.setZero();
	for (int i = 0; i < NumIntVert; ++i)
	{
		int vidx0 = IntrVertList[i];
		for (int j = 0; j < NumBndryVert; ++j)
		{
			int vidx1 = BndryVertList[0][j];

			std::string key = cast_str(vidx0) + "-" + cast_str(vidx1);
			std::map<std::string, float>::iterator it;
			it = EdgeWeightMap.find(key);
			double lambda = (it == EdgeWeightMap.end()) ? 0.0 : it->second;

			b1(i) = b1(i) + lambda * RetVerts[vidx1].P[0];
			b2(i) = b2(i) + lambda * RetVerts[vidx1].P[1];
		}
	}

	//////////////////////////////////////////////
	// 3 단계: 선형 시스템 Ax = b의 해를 구한다.//
	//////////////////////////////////////////////
	// 선형시스템의 해(x1, x2)를 구한다.
	Eigen::VectorXf x1(NumIntVert), x2(NumIntVert);
	Eigen::SparseLU<Eigen::SparseMatrix<float>> solver;
	solver.compute(A);

	x1 = solver.solve(b1);
	x2 = solver.solve(b2);

	// 내부 정점의 파라미터 값을 설정한다.
	for (int i = 0; i < NumIntVert; ++i)
	{
		int vidx = IntrVertList[i];
		RetVerts[vidx].P.Set(x1[i], x2[i], 0.0);
	}

	pRetMesh->ModifiedFlag = MODIFIED_VERT_NUM;
	pRetMesh->UpdateMesh();
	return pRetMesh;
}

/*!
*	\brief 2차원 공간에 매개화된 메쉬를 생성한다.
*
*	\param Name 매개화 메쉬의 이름
*	\param pMesh 매개화될 메쉬
*	\param StartVertIdx 파라미터 값(0.5, 0.0)에 대응될 시작 정점의 인덱스
*
*	\return 매개화된 메쉬를 반환한다.
*/
GMesh *create_mesh_unfolded(std::string Name, GMesh *pMesh, int StartVertIdx, int BndryType)
{
	GMesh *pRetMesh;

	// 사각형 경계로 매개화
	if (BndryType == 0) 
	{
		pRetMesh = create_mesh_unfolded_square(Name, pMesh, StartVertIdx);
	}

	// 원/타원 경계로 매개화
	if (BndryType == 1)	
	{
		pRetMesh = create_mesh_unfolded_circle(Name, pMesh, StartVertIdx);
	}

	return pRetMesh;
}

/*!
*	\brief 경계 정점으로부터 메쉬를 생성한다.
*
*	\param Name 생성될 메쉬의 이름
*	\param BndryVerts 경계 정점의 리스트
*	\param FillType 내부를 채우는 형태 (0: 일반적 경계, 1: 컨벡스 경계(팬 형태로 채워짐))
*
*	\return 경계 정점을 채우는 메쉬를 생성하여 반환한다.
*/
GMesh *create_mesh_from_bndry_verts(std::string Name, std::vector<GVertex> &BndryVerts, int FillType)
{
	// 새로운 홀 메쉬를 생성하고,
	GMesh *pHoleMesh = new GMesh(Name, GTransf(), FACE_TRI);

	// 경계 정점으로 메쉬의 정점 리스트를 구성한다.
	if (FillType == 0)
	{
		int NumVert = (int)BndryVerts.size();
		for (int i = 0; i < NumVert; ++i)
			pHoleMesh->AddVertex(BndryVerts[i]);

		while(true)
		{
			// 남은 경계 정점의 개수를 구하고,
			NumVert = (int)BndryVerts.size();

			// 경계 정점의 개수가 3개라면,
			if (NumVert == 3)
			{
				// 남은 경계 정점들로 삼각형을 만들고,
				int vidx0 = pHoleMesh->GetVertIdx(&BndryVerts[0]);
				int vidx1 = pHoleMesh->GetVertIdx(&BndryVerts[1]);
				int vidx2 = pHoleMesh->GetVertIdx(&BndryVerts[2]);
				GFace f(vidx0, vidx1, vidx2);

				// 메쉬에 추가한다.
				pHoleMesh->AddFace(f);
				pHoleMesh->ModifiedFlag = MODIFIED_VERT_NUM;
				pHoleMesh->UpdateMesh();
				break;
			}

			// 이웃한 모서리의 사이각을 구하기 위하여,
			std::vector<double> Angles;
			for (int i = 0; i < NumVert; ++i)
			{
				int vidx0 = (i == 0) ? (NumVert - 1) : (i - 1);
				int vidx1 = i;
				int vidx2 = (i == NumVert - 1) ? 0 : (i + 1);

				// 이웃한 정점들을 찾아,
				GVertex v0 = BndryVerts[vidx0];
				GVertex v1 = BndryVerts[vidx1];
				GVertex v2 = BndryVerts[vidx2];

				// 이웃한 모서리의 사이각을 계산하여 리스트에 추가한다.
				GVector3 e0 = v0.P - v1.P;
				GVector3 e1 = v2.P - v1.P;
				double theta = angle(e1, e0, v1.N, false);
				Angles.push_back(theta);
			}

			// 가장 작은 사이각을 갖는 정점을 찾는다.
			double min_angle = 1.0e6;
			std::vector<GVertex>::iterator min_it;
			std::vector<GVertex>::iterator it = BndryVerts.begin();
			for (int i = 0; i < NumVert; ++i)
			{
				if (Angles[i] < min_angle)
				{
					min_angle = Angles[i];
					min_it = it;
				}
				it++;
			}

			GVertex v0, v1, v2;
			// 가장 작은 사이각을 갖는 정점의 인덱스가 경계정점 인덱스리스트의 처음이라면,
			if (min_it == BndryVerts.begin())
			{
				// 인접한 세 정점의 인덱스를 구한다.
				v0 = *(BndryVerts.end() - 1);
				v1 = *(min_it);
				v2 = *(min_it + 1);
			}
			else if (min_it == BndryVerts.end() - 1) // 가장 작은 사이각을 갖는 정점의 인덱스가 경계정점 인덱스리스트의 마지막이라면,
			{
				// 인접한 세 정점의 인덱스를 구한다.
				v0 = *(min_it - 1);
				v1 = *(min_it);
				v2 = *(BndryVerts.begin());
			}
			else	// 가장 작은 사이각을 갖는 정점의 인덱스가 경계정점 인덱스리스트의 중간이라면
			{
				// 인접한 세 정점의 인덱스를 구한다.
				v0 = *(min_it - 1);
				v1 = *(min_it);
				v2 = *(min_it + 1);
			}

			// 이웃한 경계 정점의 인덱스를 구하고,
			int vidx0 = pHoleMesh->GetVertIdx(&v0);
			int vidx1 = pHoleMesh->GetVertIdx(&v1);
			int vidx2 = pHoleMesh->GetVertIdx(&v2);

			// 새로운 삼각형을 만들고, 메쉬에 추가하고,
			GFace f(vidx0, vidx1, vidx2);
			pHoleMesh->AddFace(f);
			pHoleMesh->UpdateNormal(false);

			// 가장 작은 사이각을 갖는 정점을 경계정점 리스트에서 제거한다.
			BndryVerts.erase(min_it);
		}
	}
	else
	{
		int NumVert = (int)BndryVerts.size();
		GVector3 CntPt;
		for (int i = 0; i < NumVert; ++i)
		{
			pHoleMesh->AddVertex(BndryVerts[i]);
			CntPt = CntPt + cast_vec3(BndryVerts[i].P);
		}
		CntPt /= (double)NumVert;
		pHoleMesh->AddVertex(GVertex(cast_pt3(CntPt)));

		for (int i = 0; i < NumVert; ++i)
		{
			int vidx0 = NumVert;
			int vidx1 = i;
			int vidx2 = (i + 1) % NumVert;

			// 새로운 삼각형을 만들고, 메쉬에 추가하고,
			GFace f(vidx0, vidx1, vidx2);
			pHoleMesh->AddFace(f);			
		}
		pHoleMesh->ModifiedFlag = MODIFIED_VERT_NUM;
		pHoleMesh->UpdateMesh();
	}


	// 생성된 메쉬를 반환한다.
	return pHoleMesh;
}

/*!
*	\brief 스윕 곡면으로부터 부분 메쉬를 생성하는 함수
*
*	\param pSrf 스윕 곡면
*	\param ContParam 스윕 곡면의 접촉 단면의 V 방향 파라미터
*	\param AuxParam 스윕 곡면의 보조 단면의 V 방향 파라미터
*	\param snum_u U-방향 샘플링 수
*	\param snum_v V-방향 샘플링 수
*
*	\return 생성된 메쉬를 반환한다.
*/
GMesh *create_mesh_from_sweep_srf(GSweepSrf *pSrf, double ContParam, double AuxParam, int snum_u, int snum_v)
{
	// 메쉬 객체를 생성한다.
	GMesh *pMesh = new GMesh("", GTransf(), FACE_QUAD, "");
	pMesh->Renderer.RenderType |= RENDER_WIRE;

	// 스윕 곡면의 정의역을 구하고 검사한다.
	double umin, umax, vmin, vmax;
	pSrf->GetDomain(&umin, &umax, &vmin, &vmax);
	if (AuxParam < vmin || ContParam > vmax)
	{
		std::cout << "Subdomian error" << std::endl;
		return NULL;
	}

	// 정점의 리스트를 생성한다.
	for (int i = 0; i < snum_u; ++i)
	{
		double u = umin + (umax - umin) * i / (double)(snum_u - 1);
		for (int j = 0; j < snum_v; ++j)
		{
			double v = AuxParam + (ContParam - AuxParam) * j / (double)(snum_v - 1);

			GPoint3 pt = cast_pt3(pSrf->Eval(u, v));
			pt = pSrf->MC * pt;
			pMesh->AddVertex(GVertex(pt));
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

			pMesh->AddFace(f0);
			pMesh->AddFace(f1);
		}
	}

	// 생성된 메쉬를 반환한다.
	return pMesh;
}

/*!
*	\brief 두 메쉬의 충돌 체크를 실시한다.
*
*	\param pMesh1 첫 번째 메쉬
*	\param pMesh2 두 번째 메쉬
*
*	\return 충돌한 삼각형 쌍의 개수를 반환한다.
*/
int collision_check(GMesh *pMesh1, GMesh *pMesh2)
{
	if (pMesh1->pBVH == NULL || pMesh2->pBVH == NULL)
	{
		//std::cout << "No BVH" << std::endl;
		return -1;
	}

	pMesh1->CollisionFaces.clear();
	pMesh2->CollisionFaces.clear();

	PQP_CollideResult cres;
	PQP_REAL R1[3][3];
	PQP_REAL T1[3];
	PQP_REAL R2[3][3];
	PQP_REAL T2[3];

	GMatrix M1 = cast_mat(pMesh1->MC, false);
	R1[0][0] = M1[0][0];	R1[0][1] = M1[0][1];	R1[0][2] = M1[0][2];	T1[0] = M1[0][3];
	R1[1][0] = M1[1][0];	R1[1][1] = M1[1][1];	R1[1][2] = M1[1][2];	T1[1] = M1[1][3];
	R1[2][0] = M1[2][0];	R1[2][1] = M1[2][1];	R1[2][2] = M1[2][2];	T1[2] = M1[2][3];

	GMatrix M2 = cast_mat(pMesh2->MC, false);
	R2[0][0] = M2[0][0];	R2[0][1] = M2[0][1];	R2[0][2] = M2[0][2];	T2[0] = M2[0][3];
	R2[1][0] = M2[1][0];	R2[1][1] = M2[1][1];	R2[1][2] = M2[1][2];	T2[1] = M2[1][3];
	R2[2][0] = M2[2][0];	R2[2][1] = M2[2][1];	R2[2][2] = M2[2][2];	T2[2] = M2[2][3];

	PQP_Collide(&cres, R1, T1, pMesh1->pBVH, R2, T2, pMesh2->pBVH);
	
	int NumPair = cres.NumPairs();
	for (int i = 0; i < NumPair; ++i)
	{
		int fidx1 = cres.Id1(i);
		int fidx2 = cres.Id2(i);
		pMesh1->CollisionFaces.push_back(fidx1);
		pMesh2->CollisionFaces.push_back(fidx2);
	}

	return NumPair;
}

/*!
*	\brief 충돌 체크를 실시한다.
*
*	\param MC1 pBVH1의 변환 행렬
*	\param pBVH1 부분 메쉬에 대한 BVH
*	\param pMesh2 두 번째 메쉬
*
*	\return 충돌한 삼각형 쌍의 개수를 반환한다.
*/
int collision_check(GTransf &MC1, PQP_Model *pBVH1, GMesh *pMesh2)
{
	if (pMesh2->pBVH == NULL)
	{
		//std::cout << "No BVH" << std::endl;
		return -1;
	}

	pMesh2->CollisionFaces.clear();

	PQP_CollideResult cres;
	PQP_REAL R1[3][3];
	PQP_REAL T1[3];
	PQP_REAL R2[3][3];
	PQP_REAL T2[3];

	GMatrix M1 = cast_mat(MC1, false);
	R1[0][0] = M1[0][0];	R1[0][1] = M1[0][1];	R1[0][2] = M1[0][2];	T1[0] = M1[0][3];
	R1[1][0] = M1[1][0];	R1[1][1] = M1[1][1];	R1[1][2] = M1[1][2];	T1[1] = M1[1][3];
	R1[2][0] = M1[2][0];	R1[2][1] = M1[2][1];	R1[2][2] = M1[2][2];	T1[2] = M1[2][3];

	GMatrix M2 = cast_mat(pMesh2->MC, false);
	R2[0][0] = M2[0][0];	R2[0][1] = M2[0][1];	R2[0][2] = M2[0][2];	T2[0] = M2[0][3];
	R2[1][0] = M2[1][0];	R2[1][1] = M2[1][1];	R2[1][2] = M2[1][2];	T2[1] = M2[1][3];
	R2[2][0] = M2[2][0];	R2[2][1] = M2[2][1];	R2[2][2] = M2[2][2];	T2[2] = M2[2][3];

	PQP_Collide(&cres, R1, T1, pBVH1, R2, T2, pMesh2->pBVH);

	int NumPair = cres.NumPairs();
	for (int i = 0; i < NumPair; ++i)
	{
		int fidx2 = cres.Id2(i);
		pMesh2->CollisionFaces.push_back(fidx2);
	}

	return NumPair;
}

/*!
*	\brief 세 개의 색상에서 보간된 색상을 반환한다.
*
*	\param t 색상 파라미터
*	\param C1 첫 번째 키색상
*	\param C2 두 번째 키색상
*	\param C3 세 번재 키색상
*
*	\return 보간된 색상을 반환한다.
*/
GVector3 get_color_gradient(double t, GVector3 C1, GVector3 C2, GVector3 C3)
{
	if (0.0 <= t && t < 0.5)
	{
		t = t * 2;
		return (1.0 - t) * C1 + t * C2;
	}
	else if (t >= 0.5 && t <= 1.0)
	{
		t = 2 * (t - 0.5);
		return (1.0 - t) * C2 + t * C3;
	}
	else
		return GVector3();
}

/*!
*	\brief 무지개 색상에서 보간된 색을 얻는다.
*
*	\param t 색상 파라미터: 0(빨강) <= t <= 1.0(보라)
*
*	\return 보간된 중간 색상을 반환한다.
*/
GVector3 get_color_gradient_from_rainbow(double t)
{
	double a = t / 0.2;
	int X = floor(a);
	int Y = floor(255 * (a - X));
	
	unsigned char r, g, b;
	switch(X)
	{
	case 0: 
		r = 255;
		g = Y;
		b = 0;
		break;
	case 1: 
		r = 255 - Y;
		g = 255;
		b = 0;
		break;
	case 2: 
		r = 0;
		g = 255;
		b = Y;
		break;
	case 3: 
		r = 0;
		g = 255 - Y;
		b = 255;
		break;
	case 4: 
		r = Y;
		g = 0;
		b = 255;
		break;
	case 5: 
		r = 255;
		g = 0;
		b = 255;
		break;
	}

	return GVector3(r / 255.0, g / 255.0, b / 255.0);
}

/*!
*	\brief 임의의 재질을 생성하여 반환하는 함수
*
*	\return 임의의 재질을 생성하여 반환한다.
*/
GMaterial get_material_random()
{
	static bool bInit = false;
	if (!bInit)
	{
		srand(time(NULL));
		bInit = true;
	}

	double r = (double)rand() / (double)RAND_MAX;
	double g = (double)rand() / (double)RAND_MAX;
	double b = (double)rand() / (double)RAND_MAX;

	GMaterial mtl;
	mtl.SetAmbient(r * 0.2, g * 0.2, b * 0.2);
	mtl.SetDiffuse(r, g, b);
	mtl.SetSpecular(1.0, 1.0, 1.0);

	return mtl;
}

/*!
*	\brief 사용자 정의 재질을 생성하여 반환하는 함수
*
*	\param diff_r 난반사의 R 성분
*	\param diff_g 난반사의 G 성분
*	\param diff_b 난반사의 B 성분
*	\param ambi_r 주변광의 R 성분
*	\param ambi_g 주변광의 G 성분
*	\param ambi_b 주변광의 B 성분
*	\param spec_r 전반사의 R 성분
*	\param spec_g 전반사의 G 성분
*	\param spec_b 전반사의 B 성분
*
*	\return 임의의 재질을 생성하여 반환한다.
*/
GMaterial get_material(double diff_r, double diff_g, double diff_b, 
					   double ambi_r, double ambi_g, double ambi_b, 
					   double spec_r, double spec_g, double spec_b)
{
	GMaterial mtl;
	mtl.SetAmbient(ambi_r, ambi_g, ambi_b);
	mtl.SetDiffuse(diff_r, diff_g, diff_b);
	mtl.SetSpecular(spec_r, spec_g, spec_b);
	mtl.SetAlpha(1.0);

	return mtl;
}

/*!
*	\brief 메쉬의 가우스 곡률(kG), 평균 곡률(kH), 최소 곡률(kMin) 그리고 최대 곡률(kMax)를 계산하는 함수
*	\note Meyer et al. "Discrete Differential-Geometry Operators for Triangulated 2-Manifolds" 논문 참조
*
*	\param pMesh 대상 메쉬
*	\param kG 가우스 곡률이 저장된다.
*	\param kH 평균 곡률이 저장된다.
*	\param kMin 최소 곡률이 저장된다.
*	\param kMax 최대 곡률이 저장된다.
*/
void get_curvature(GMesh *pMesh, std::vector<double> &kG, std::vector<double> &kH, std::vector<double> &kMin, std::vector<double> &kMax)
{
	// 정점의 개수를 구하고, 곡률 리스트의 값을 초기화 한다.
	int NumVert = pMesh->GetNumVert();
	kG.clear();
	kH.clear();
	kMin.clear();
	kMax.clear();
	kG.assign(NumVert, 2.0 * M_PI);
	kH.assign(NumVert, 0.0);
	kMin.assign(NumVert, 0.0);
	kMax.assign(NumVert, 0.0);

	// Mean 곡률 법선 벡터를 정의하고 초기화 한다.
	std::vector<GVector3> Normals;
	Normals.assign(NumVert, GVector3());

	// 정점 주변의 영역 넓이를 저장할 변수를 정의한다.
	std::vector<double> Area;
	Area.assign(NumVert, 0.0);

	// 각각의 삼각형에 대하여,
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		// 정점의 인덱스와 좌표를 구하고,
		int vidx0 = pMesh->Faces[i].vIdx[0];
		int vidx1 = pMesh->Faces[i].vIdx[1];
		int vidx2 = pMesh->Faces[i].vIdx[2];
		GPoint3 P = pMesh->Verts[vidx0].P;
		GPoint3 Q = pMesh->Verts[vidx1].P;
		GPoint3 R = pMesh->Verts[vidx2].P;

		// 각 정점에서 인접한 두 모서리의 사이각을 구하고,
		double AngP = angle(Q - P, R - P, true);
		double AngQ = angle(R - Q, P - Q, true);
		double AngR = angle(P - R, Q - R, true);

		// 각 정점의 평균 곡률 법선 벡터를 누적하고,
		Normals[vidx0] += ((Q - P) / tan(AngR) + (R - P) / tan(AngQ));
		Normals[vidx1] += ((R - Q) / tan(AngP) + (P - Q) / tan(AngR));
		Normals[vidx2] += ((P - R) / tan(AngQ) + (Q - R) / tan(AngP));

		// 각 정점의 영역 넓이를 계산하여,
		double AreaP = (dist_sq(P, Q) / tan(AngR) + dist_sq(P, R) / tan(AngQ)) / 8.0;
		double AreaQ = (dist_sq(Q, R) / tan(AngP) + dist_sq(Q, P) / tan(AngR)) / 8.0;
		double AreaR = (dist_sq(R, P) / tan(AngQ) + dist_sq(R, Q) / tan(AngP)) / 8.0;
		double AreaTot = AreaP + AreaQ + AreaR;

		if (AngP > M_PI_2)		// 각 P가 둔각인 삼각형이라면,
		{
			// 각 정점의 영역 넓이에 누적한다.
			Area[vidx0] += AreaTot * 0.5;
			Area[vidx1] += AreaTot * 0.25;
			Area[vidx2] += AreaTot * 0.25;
		}
		else if (AngQ > M_PI_2)	// 각 Q가 둔각인 삼각형이라면,
		{
			// 각 정점의 영역 넓이에 누적한다.
			Area[vidx0] += AreaTot * 0.25;
			Area[vidx1] += AreaTot * 0.5;
			Area[vidx2] += AreaTot * 0.25;
		}
		else if (AngR > M_PI_2)	// 각 R이 둔각인 삼각형이라면,
		{
			// 각 정점의 영역 넓이에 누적한다.
			Area[vidx0] += AreaTot * 0.25;
			Area[vidx1] += AreaTot * 0.25;
			Area[vidx2] += AreaTot * 0.5;
		}
		else					// 예각 삼각형이라면,
		{
			// 각 정점의 영역 넓이에 누적한다.
			Area[vidx0] += AreaP;
			Area[vidx1] += AreaQ;
			Area[vidx2] += AreaR;
		}

		// 가우스 곡률을 계산한다.
		kG[vidx0] -= AngP;
		kG[vidx1] -= AngQ;
		kG[vidx2] -= AngR;
	}

	// 각각의 정점에 대하여,
	for (int i = 0; i < NumVert; ++i)
	{
		// 가우스 곡률과 평균 곡률의 값을 계산하고,
		kG[i] = kG[i] / Area[i];
		kH[i] = 0.5 * norm(Normals[i] / (2 * Area[i]));

		// 최소, 최대 곡률을 계산한다.
		double delta = kH[i] * kH[i] - kG[i];
		if (delta < 0.0)
			delta = 0.0;
		kMin[i] = kH[i] - SQRT(delta);
		kMax[i] = kH[i] + SQRT(delta);
	}
}

/*!
*	\brief 메쉬의 각 정점의 Gaussian 곡률의 계산하여 반환하는 함수
*
*	\param pMesh 대상 메쉬
*	\param Curvatures 메쉬 정점의 곡률이 저장된다. (경계 정점의 경우, 0이 저장된다)
*	\param MinK 최소 곡률이 저장된다.
*	\param MaxK 최대 곡률이 저장된다.
*/
void get_gaussian_curvature(GMesh *pMesh, std::vector<double> &Curvatures, double &MinK, double &MaxK)
{
	// 정점의 개수를 구하고, 곡률 리스트의 값을 초기화 한다.
	int NumVert = pMesh->GetNumVert();
	Curvatures.clear();
	Curvatures.assign(NumVert, 2 * M_PI);

	std::vector<double> Area;
	Area.assign(NumVert, 0.0);

	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		int vidx0 = pMesh->Faces[i].vIdx[0];
		int vidx1 = pMesh->Faces[i].vIdx[1];
		int vidx2 = pMesh->Faces[i].vIdx[2];

		GPoint3 P = pMesh->Verts[vidx0].P;
		GPoint3 Q = pMesh->Verts[vidx1].P;
		GPoint3 R = pMesh->Verts[vidx2].P;

		double AngP = angle(Q - P, R - P, true);
		double AngQ = angle(R - Q, P - Q, true);
		double AngR = angle(P - R, Q - R, true);
		
		double AreaP = (dist_sq(P, Q) / tan(AngR) + dist_sq(P, R) / tan(AngQ)) / 8.0;
		double AreaQ = (dist_sq(Q, R) / tan(AngP) + dist_sq(Q, P) / tan(AngR)) / 8.0;
		double AreaR = (dist_sq(R, P) / tan(AngQ) + dist_sq(R, Q) / tan(AngP)) / 8.0;
		double AreaTot = AreaP + AreaQ + AreaR;

		if (AngP > M_PI_2)	// 각 P가 둔각인 삼각형이라면,
		{
			Area[vidx0] += AreaTot * 0.5;
			Area[vidx1] += AreaTot * 0.25;
			Area[vidx2] += AreaTot * 0.25;
		}
		else if (AngQ > M_PI_2)	// 각 Q가 둔각인 삼각형이라면,
		{
			Area[vidx0] += AreaTot * 0.25;
			Area[vidx1] += AreaTot * 0.5;
			Area[vidx2] += AreaTot * 0.25;
		}
		else if (AngR > M_PI_2)	// 각 R이 둔각인 삼각형이라면,
		{
			Area[vidx0] += AreaTot * 0.25;
			Area[vidx1] += AreaTot * 0.25;
			Area[vidx2] += AreaTot * 0.5;
		}
		else		// 예각 삼각형이라면,
		{
			Area[vidx0] += AreaP;
			Area[vidx1] += AreaQ;
			Area[vidx2] += AreaR;
		}

		Curvatures[vidx0] -= AngP;
		Curvatures[vidx1] -= AngQ;
		Curvatures[vidx2] -= AngR;
	}

	// 메쉬의 경계 정점의 정보를 구하고,
	std::vector<bool> InfoList;
	::get_bndry_vert_info(pMesh, InfoList);
	MinK = 1000000.0;
	MaxK = -1000000.0;

	// 메쉬의 각각의 정점에 대하여,
	for (int i = 0; i < NumVert; ++i)
	{
		// 경계 정점이 아니라면,
		if (InfoList[i] == false)
		{
			// 가우스 곡률을 구하고, 최대, 최소 값을 갱신한다.
			Curvatures[i] /= Area[i];
			MinK = MIN(MinK, Curvatures[i]);
			MaxK = MAX(MaxK, Curvatures[i]);
		}
	}

	// 경계 정점의 Gaussian 곡률을 최소값으로 설정한다.
	for (int i = 0; i < NumVert; ++i)
		Curvatures[i] = (InfoList[i] == true) ? MinK : Curvatures[i];		
}

/*!
*	\brief 메쉬의 각 정점의 Mean 곡률의 계산하여 반환하는 함수
*
*	\param pMesh 대상 메쉬
*	\param Curvatures 메쉬 정점의 곡률이 저장된다. (경계 정점의 경우, 최소값이 저장된다)
*	\param MinK 최소 곡률이 저장된다.
*	\param MaxK 최대 곡률이 저장된다.
*/
void get_mean_curvature(GMesh *pMesh, std::vector<double> &Curvatures, double &MinK, double &MaxK)
{
	// 정점의 개수를 구하고, 곡률 리스트의 값을 초기화 한다.
	int NumVert = pMesh->GetNumVert();
	Curvatures.clear();
	Curvatures.assign(NumVert, 0);

	std::vector<GVector3> Normals;
	Normals.assign(NumVert, GVector3());

	std::vector<double> Area;
	Area.assign(NumVert, 0.0);

	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		int vidx0 = pMesh->Faces[i].vIdx[0];
		int vidx1 = pMesh->Faces[i].vIdx[1];
		int vidx2 = pMesh->Faces[i].vIdx[2];

		GPoint3 P = pMesh->Verts[vidx0].P;
		GPoint3 Q = pMesh->Verts[vidx1].P;
		GPoint3 R = pMesh->Verts[vidx2].P;

		double AngP = angle(Q - P, R - P, true);
		double AngQ = angle(R - Q, P - Q, true);
		double AngR = angle(P - R, Q - R, true);

		Normals[vidx0] += ((Q - P) / tan(AngR) + (R - P) / tan(AngQ));
		Normals[vidx1] += ((R - Q) / tan(AngP) + (P - Q) / tan(AngR));
		Normals[vidx2] += ((P - R) / tan(AngQ) + (Q - R) / tan(AngP));

		double AreaP = (dist_sq(P, Q) / tan(AngR) + dist_sq(P, R) / tan(AngQ)) / 8.0;
		double AreaQ = (dist_sq(Q, R) / tan(AngP) + dist_sq(Q, P) / tan(AngR)) / 8.0;
		double AreaR = (dist_sq(R, P) / tan(AngQ) + dist_sq(R, Q) / tan(AngP)) / 8.0;
		double AreaTot = AreaP + AreaQ + AreaR;

		if (AngP > M_PI_2)	// 각 P가 둔각인 삼각형이라면,
		{
			Area[vidx0] += AreaTot * 0.5;
			Area[vidx1] += AreaTot * 0.25;
			Area[vidx2] += AreaTot * 0.25;
		}
		else if (AngQ > M_PI_2)	// 각 Q가 둔각인 삼각형이라면,
		{
			Area[vidx0] += AreaTot * 0.25;
			Area[vidx1] += AreaTot * 0.5;
			Area[vidx2] += AreaTot * 0.25;
		}
		else if (AngR > M_PI_2)	// 각 R이 둔각인 삼각형이라면,
		{
			Area[vidx0] += AreaTot * 0.25;
			Area[vidx1] += AreaTot * 0.25;
			Area[vidx2] += AreaTot * 0.5;
		}
		else		// 예각 삼각형이라면,
		{
			Area[vidx0] += AreaP;
			Area[vidx1] += AreaQ;
			Area[vidx2] += AreaR;
		}
	}

	// 메쉬의 경계 정점의 정보를 구하고,
	std::vector<bool> InfoList;
	::get_bndry_vert_info(pMesh, InfoList);
	MinK = 1000000.0;
	MaxK = -1000000.0;

	// 메쉬의 각각의 정점에 대하여,
	for (int i = 0; i < NumVert; ++i)
	{
		// 경계 정점이 아니라면,
		if (InfoList[i] == false)
		{
			// 평균 곡률을 구하고, 최대, 최소 값을 갱신한다.
			Curvatures[i] = 0.5 * norm(Normals[i] / (2 * Area[i]));
			MinK = MIN(MinK, Curvatures[i]);
			MaxK = MAX(MaxK, Curvatures[i]);
		}
	}

	// 경계 정점의 평균 곡률을 최소값으로 설정한다.
	for (int i = 0; i < NumVert; ++i)
		Curvatures[i] = (InfoList[i] == true) ? MinK : Curvatures[i];	
}

/*!
*	\brief 객체의 표면적을 계산한다.
*
*	\pram pMesh 표면적을 계산할 메쉬
*
*	\return 삼각형의 면적의 합으로 계산된 객체의 표면적을 반환한다.
*/
double get_mesh_area(GMesh *pMesh)
{
	double Area = 0.0;
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		int *vidx = pMesh->Faces[i].vIdx;
		GPoint3 p = pMesh->Verts[vidx[0]].P;
		GPoint3 q = pMesh->Verts[vidx[1]].P;
		GPoint3 r = pMesh->Verts[vidx[2]].P;

		Area += norm((q - p) ^ (r - p)) * 0.5;
	}

	return Area;
}

/*!
*	\brief 메쉬를 평탄화 한다.
*
*	\pram pMesh 편집될 메쉬
*/
void edit_mesh_by_smoothing(GMesh *pMesh)
{
	int vNum = pMesh->GetNumVert();

	std::vector<int> Deg;
	Deg.assign(vNum, 0);

	std::vector<GVector3> COG;
	COG.assign(vNum, GVector3(0.0, 0.0, 0.0));

	int fNum = pMesh->GetNumFace();
	for (int i = 0; i < fNum; ++i)
	{
		int vidx0 = pMesh->Faces[i].vIdx[0];
		int vidx1 = pMesh->Faces[i].vIdx[1];
		int vidx2 = pMesh->Faces[i].vIdx[2];

		GVector3 g = cast_vec3(pMesh->Faces[i].GetCOG(true));
		COG[vidx0] += g;
		COG[vidx1] += g;
		COG[vidx2] += g;
		Deg[vidx0]++;
		Deg[vidx1]++;
		Deg[vidx2]++;
	}

	std::vector<int> Info;
	::get_intr_verts(pMesh, Info);

	int num = Info.size();
	for (int i = 0; i < num; ++i)
	{
		int idx = Info[i];
		COG[idx] = COG[idx] / Deg[idx];
		pMesh->Verts[idx].P = cast_pt3(COG[idx]);
	}

	pMesh->ModifiedFlag = MODIFIED_VERT_POS_ALL;
	pMesh->UpdateMesh();
}

/*!
*	\brief 메쉬에 세분화 연산을 적용한다.
*	\note 텍스처 좌표와 그룹핑된 삼각형 정보가 제거됨
*
*	\pram pMesh 편집될 메쉬
*/
void edit_mesh_by_refine(GMesh *pMesh)
{
	// Quad 메쉬와 fan 메쉬는 와이어프레임 렌더링 문제로 지원 하지 않음
	if (pMesh->FaceType == FACE_QUAD || pMesh->FaceType == FACE_TRI_FAN)
		return;
	
	// 필요한 변수를 정의한다.
	std::vector<GFace> NewFaces;
	std::map<std::string, int> vIdxMap;
	pMesh->GrpFaces.clear();
	
	// 각각의 삼각형에 대하여,
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		int vidx0 = pMesh->Faces[i].vIdx[0];
		int vidx1 = pMesh->Faces[i].vIdx[1];
		int vidx2 = pMesh->Faces[i].vIdx[2];
		int mtl_idx = pMesh->Faces[i].MtlIdx;

		std::string edge01 = cast_str(vidx0) + std::string("-") + cast_str(vidx1);
		std::string edge10 = cast_str(vidx1) + std::string("-") + cast_str(vidx0);
		std::string edge12 = cast_str(vidx1) + std::string("-") + cast_str(vidx2);
		std::string edge21 = cast_str(vidx2) + std::string("-") + cast_str(vidx1);
		std::string edge20 = cast_str(vidx2) + std::string("-") + cast_str(vidx0);
		std::string edge02 = cast_str(vidx0) + std::string("-") + cast_str(vidx2);

		// 에지 (vidx0, vidx1) 또는 (vidx1, vidx0)의 중점이 추가되지 않았다면,
		if (vIdxMap[edge01] == 0 && vIdxMap[edge10] == 0)
		{
			// 에지의 중점을 구하여 정점리스트에 추가하고,
			GVertex v;
			v.P = cast_pt3((cast_vec3(pMesh->Verts[vidx0].P) + cast_vec3(pMesh->Verts[vidx1].P)) * 0.5);
			pMesh->AddVertex(v);

			// 에지의 중점의 인덱스를 기록한다.
			vIdxMap[edge01] = vIdxMap[edge10] = pMesh->GetNumVert() - 1;
		}		

		// 에지 (vidx1, vidx2) 또는 (vidx2, vidx1)의 중점이 추가되지 않았다면,
		if (vIdxMap[edge12] == 0 && vIdxMap[edge21] == 0)
		{
			// 에지의 중점을 구하여 정점리스트에 추가하고,
			GVertex v;
			v.P = cast_pt3((cast_vec3(pMesh->Verts[vidx1].P) + cast_vec3(pMesh->Verts[vidx2].P)) * 0.5);
			pMesh->AddVertex(v);

			// 에지의 중점의 인덱스를 기록한다.
			vIdxMap[edge12] = vIdxMap[edge21] = pMesh->GetNumVert() - 1;
		}

		// 에지 (vidx2, vidx0) 또는 (vidx0, vidx2)의 중점이 추가되지 않았다면,
		if (vIdxMap[edge20] == 0 && vIdxMap[edge02] == 0)
		{
			// 에지의 중점을 구하여 정점리스트에 추가하고,
			GVertex v;
			v.P = cast_pt3((cast_vec3(pMesh->Verts[vidx2].P) + cast_vec3(pMesh->Verts[vidx0].P)) * 0.5);
			pMesh->AddVertex(v);

			// 에지의 중점의 인덱스를 기록한다.
			vIdxMap[edge20] = vIdxMap[edge02] = pMesh->GetNumVert() - 1;
		}

		// 에지 중점의 인덱스를 구하여,
		int vidx3 = vIdxMap[edge01];
		int vidx4 = vIdxMap[edge12];
		int vidx5 = vIdxMap[edge20];

		// 삼각형을 4개의 부분삼각형으로 분할하여 메쉬에 추가한다.
		GFace f0(vidx0, vidx3, vidx5, -1, -1, -1, mtl_idx); 
		GFace f1(vidx3, vidx1, vidx4, -1, -1, -1, mtl_idx); 
		GFace f2(vidx3, vidx4, vidx5, -1, -1, -1, mtl_idx); 
		GFace f3(vidx5, vidx4, vidx2, -1, -1, -1, mtl_idx); 
		NewFaces.push_back(f0);
		NewFaces.push_back(f1);
		NewFaces.push_back(f2);
		NewFaces.push_back(f3);
	}

	// 새로운 삼각형을 설정한다.
	pMesh->SetNewFaces(NewFaces);
	pMesh->ModifiedFlag = MODIFIED_VERT_NUM;
	pMesh->UpdateMesh();
}

/*!
*	\brief 메쉬에 중복된 정점과 내부 경계를 꿰멘다.
*
*	\pram pMesh 편집될 메쉬
*	\param tol 동일한 정점으로 인정되는 거리 허용오차
*/
void edit_mesh_by_stitching(GMesh *pMesh, double tol)
{
	// 정점의 개수만큼 인덱스 맵을 정의하고, -1로 초기화한다.
	int NumVert = pMesh->GetNumVert();
	std::vector<int> IdxMap;
	IdxMap.assign(NumVert, -1);

	// 각각의 정점마다,
	for (int i = 0; i < NumVert; ++i)
	{
		// 이미 중복된 정점이라면 다음 정점을 검사하고,
		if (IdxMap[i] != -1)
			continue;

		// 처음 나온 정점이라면, 인데스를 부여한다.
		IdxMap[i] = i;

		// 현재 정점의 위치와 법선를 얻고,
		GPoint3 &p = pMesh->Verts[i].P;
		GVector3 &pn = pMesh->Verts[i].N;

		// 이후의 모든 정점에 대하여, 
		for (int j = i + 1; j < NumVert; ++j)
		{
			// 이미 중복된 정점이라면, 다음 정점을 검사하고,
			if (IdxMap[j] != -1)
				continue;

			// 아니라며, 정점의 위치를 구해서,
			GPoint3 &q = pMesh->Verts[j].P;
			GVector3 &qn = pMesh->Verts[j].N;


			// 동일한 정점으로 인정 된다면, 인덱스 맵에 표기한다.
			if (dist(p, q) < tol)
				IdxMap[j] = i;
		}
	}

	// 각각의 삼각형에 대하여, 중복된 정점의 인덱스를 수정한다.
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		pMesh->Faces[i].vIdx[0] = IdxMap[pMesh->Faces[i].vIdx[0]];
		pMesh->Faces[i].vIdx[1] = IdxMap[pMesh->Faces[i].vIdx[1]];
		pMesh->Faces[i].vIdx[2] = IdxMap[pMesh->Faces[i].vIdx[2]];
	}

	// 삼각형이 참조하지 않는 정점들을 제거한다.
	edit_mesh_by_removing_unused_verts(pMesh);
}

/*! 
*	\brief 메쉬에서 사용되지 않는 정점을 제거하는 함수 
*
*	\param pMesh 편집될 메쉬
*/
void edit_mesh_by_removing_unused_verts(GMesh *pMesh)
{
	/////////////////////////////////////
	// 1 단계: 미사용 정점을 삭제한다. //
	/////////////////////////////////////

	// 정점의 사용 여부를 체크하기 위해, 정점의 개수만큼 flag배열을 정의하고, false로 초기화한 후, 
	int NumVert = pMesh->GetNumVert();
	std::vector<bool> UsedFlag;
	UsedFlag.assign(NumVert, false);

	// 사용된 정점의 플래그만 true로 변경한다.
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		UsedFlag[pMesh->Faces[i].vIdx[0]] = true;
		UsedFlag[pMesh->Faces[i].vIdx[1]] = true;
		UsedFlag[pMesh->Faces[i].vIdx[2]] = true;
	}

	// 사용된 정점으로 구성된 새로운 정점 리스트를 생성한다.
	std::map<int, int> VertIdxMap;
	std::vector<GVertex> OldVerts = pMesh->Verts;
	pMesh->Verts.clear();

	for (int i = 0, vidx = 0; i < NumVert; ++i)
	{
		if (UsedFlag[i] == true)
		{
			pMesh->AddVertex(OldVerts[i]);
			VertIdxMap[i] = vidx;
			vidx++;
		}
		else
			VertIdxMap[i] = -1;
	}

	///////////////////////////////////////////////////////////
	// 2단계: 남은 정점으로 새로운 삼각형 리스트를 생성한다. //
	///////////////////////////////////////////////////////////
	std::vector<GFace> OldFaces = pMesh->Faces;	
	pMesh->Faces.clear();

	std::vector<GFace> NewFaces;
	for (int i = 0; i < NumFace; ++i)
	{
		int vidx0 = OldFaces[i].vIdx[0];
		int vidx1 = OldFaces[i].vIdx[1];
		int vidx2 = OldFaces[i].vIdx[2];

		// 남은 정점을 사용하는 삼각형이라면, 
		if (VertIdxMap[vidx0] != -1 && VertIdxMap[vidx1] != -1 && VertIdxMap[vidx2] != -1)
		{
			OldFaces[i].vIdx[0] = VertIdxMap[vidx0];
			OldFaces[i].vIdx[1] = VertIdxMap[vidx1];
			OldFaces[i].vIdx[2] = VertIdxMap[vidx2];

			// 새로운 인덱스를 갖는 삼각형을 리스트에 추가한다.
			pMesh->AddFace(OldFaces[i]);
		}
	}

	////////////////////////////////////////////////////////////////
	// 3단계: 경계 정점의 리스트를 지우고, 경계상자를 재계산한다. //
	////////////////////////////////////////////////////////////////			
	pMesh->ModifiedFlag = MODIFIED_VERT_NUM;
	pMesh->UpdateMesh();
}

/*! 
*	\brief 메쉬의 정점 색상을 법선 벡터로 설정하는 함수
*
*	\param pTarMesh 정점의 색상을 설정할 메쉬
*	\param pSrcMesh 법선정보를 가져올 메쉬 (NULL 인경우, pTarMesh와 동일)
*/
void set_vertex_color_from_normal(GMesh *pTarMesh, GMesh *pSrcMesh)
{
	if (pSrcMesh == NULL)
		pSrcMesh = pTarMesh;

	if (pTarMesh->Verts.size() != pSrcMesh->Verts.size())
		return;

	double min_r = pSrcMesh->Verts[0].N[0];
	double max_r = pSrcMesh->Verts[0].N[0];
	double min_g = pSrcMesh->Verts[0].N[1];
	double max_g = pSrcMesh->Verts[0].N[1];
	double min_b = pSrcMesh->Verts[0].N[2];
	double max_b = pSrcMesh->Verts[0].N[2];

	int vnum = pSrcMesh->Verts.size();
	for (int i = 0; i < vnum; ++i)
	{
		pTarMesh->Verts[i].N = pSrcMesh->Verts[i].N;
		min_r = MIN(min_r, pSrcMesh->Verts[i].N[0]);
		max_r = MAX(max_r, pSrcMesh->Verts[i].N[0]);

		min_g= MIN(min_g, pSrcMesh->Verts[i].N[1]);
		max_g = MAX(max_g, pSrcMesh->Verts[i].N[1]);

		min_b = MIN(min_b, pSrcMesh->Verts[i].N[2]);
		max_b = MAX(max_b, pSrcMesh->Verts[i].N[2]);
	}

	for (int i = 0; i < vnum; ++i)
	{
		double r = pTarMesh->Verts[i].N[0];
		double g = pTarMesh->Verts[i].N[1];
		double b = pTarMesh->Verts[i].N[2];

		r = (r - min_r) / (max_r - min_r);
		g = (g - min_g) / (max_g - min_g);
		b = (b - min_b) / (max_b - min_b);

		pTarMesh->Verts[i].C.Set(r, g, b);
	}

	pTarMesh->ModifiedFlag = MODIFIED_NORMAL;
	pTarMesh->UpdateMesh();
}

/*! 
*	\brief 메쉬의 정점 색상을 선택된 본 메쉬에 대한 변형 가중치로 설정하는 함수
*
*	\param pSkinMesh 정점의 색상을 설정할 메쉬
*	\param pBoneMesh 선택된 본 메쉬
*/
void set_vertex_color_from_skin_wgt(GMesh *pSkinMesh, GMesh *pBoneMesh)
{
	// 본 메쉬의 이름을 얻고,
	std::string BoneMeshName = pBoneMesh->Name;

	// 스킨 메쉬의 정점의 개수를 구하여,
	int NumVert = pSkinMesh->GetNumVert();

	// 각각의 정점마다,
	for (int i = 0; i < NumVert; ++i)
	{
		// 일단 색상을 검정색으로 초기화 하고,
		pSkinMesh->Verts[i].C.Set(0.0, 0.0, 0.0);

		// 바인딩된 본 메쉬의 개수를 구하여,
		int NumBindInfo = pSkinMesh->Verts[i].BoneInfoList.size();

		// 정점이 연결된 각각의 본 메쉬에 대하여,
		for (int j = 0; j < NumBindInfo; ++j)
		{
			// 연결된 본 메쉬가 대상 본메쉬와 같다면,
			if (pSkinMesh->Verts[i].BoneInfoList[j].BoneMeshName == BoneMeshName)
			{
				// 정점의 색상을 바인딩 가중치로 설정한다.
				double w = pSkinMesh->Verts[i].BoneInfoList[j].Wgt;
				pSkinMesh->Verts[i].C.Set(w, w, w);
				break;
			}
		}
	}
}

/*! 
*	\brief 메쉬의 한 정점에서 주어진 최대 반경 이내의 각 정점까지의 측지거리를 계산하는 함수 
*
*	\param pMesh 대상 메쉬
*	\param pVert 선택된 메쉬 상의 정점
*	\param Distances pVert에서 i번째 정점까지의 측지거리가 Distances[i]에 저장된다.
*	\param MaxRadius 측지거리를 계산할 최대 반경
*/
void get_geodesic_dist_field(GMesh *pMesh, GVertex *pVert, std::vector<double> &Distances, double MaxRadius)
{
	// 미리 계산된 위상 정보를 저장하기 위한 정적 변수를 사용한다.
	static GMesh *pPrevMesh = NULL;
	static int PrevNumVert = 0;
	static std::vector<std::vector<int>> VertList;	

	// 대상 메쉬가 바뀌거나 기하정보가 변경되었다면,
	int NumVert = pMesh->GetNumVert();
	if (pPrevMesh != pMesh || PrevNumVert != NumVert)
	{
		// 새롭게 위상 정보를 구한다.
		::get_neighbor_verts_of_vert(pMesh, VertList);
		pPrevMesh = pMesh;
		PrevNumVert = NumVert;
	}

	// 거리장을 초기화 하고,
	Distances.assign(NumVert, 1000000.0);

	// 최소 힙을 생성한다.
	GHeap MinHeap(&Distances);

	// 선택된 정점의 거리를 0으로 초기화 하고, 인덱스를 힙에 추가한다.
	Distances[pVert->Idx] = 0.0;
	MinHeap.push(pVert->Idx);

	// 처리할 정점이 남아 있다면,
	while (!MinHeap.empty())
	{
		// 최소 거리를 갖는 정점의 인덱스를 구하여,
		int j = MinHeap.get_candidate();

		// 이웃한 모든 삼각형에 대하여,
		int valance = (int)VertList[j].size();
		for (int l = 0; l < valance; ++l)
		{
			// 삼각형 (vi, vj, vk)에 대하여,
			int i = VertList[j][l];
			int k = (l == 0) ? VertList[j][valance - 1] : VertList[j][l - 1];

			// 세 정점의 좌표를 구하고,
			GPoint3 Pi = pMesh->Verts[i].P;
			GPoint3 Pj = pMesh->Verts[j].P;
			GPoint3 Pk = pMesh->Verts[k].P;

			// 현재가지 계산된 두 정점의 최소거리 Uj, Uk를 구하고,
			double Ui = 0.0;
			double Uj = Distances[j];
			double Uk = Distances[k];

			// 삼각형의 각 변의 길이와 제곱을 계산하고,
			double dij_sq = dist_sq(Pi, Pj);
			double dik_sq = dist_sq(Pi, Pk);
			double djk_sq = dist_sq(Pj, Pk);
			double dij = SQRT(dij_sq);
			double dik = SQRT(dik_sq);			
			double djk = SQRT(djk_sq);

			// 만약, 삼각형이 생성된다면,
			if (ABS(Uj - Uk) < djk && djk < Uj + Uk)
			{
				// 2차원 평면에 매핑한 S'(x0, y0), Pi(x1, y1), Pj(0, 0), Pk(-djk, 0)의 좌표를 구하고,
				double x0 = (SQR(Uk) - SQR(Uj) - djk_sq) / (2 * djk);
				double y0 = SQRT(SQR(Uj) - SQR(x0));				
				double x1 = (dik_sq - dij_sq - djk_sq) / (2 * djk);
				double y1 = -SQRT(dij_sq - SQR(x1));

				// S'과 Pi의 거리를 계산하여, Ui의 거리로 설정한다.
				double m = (y1 - y0) / (x1 - x0);
				double t = x0 - y0 / m;

				// 다각형 (S', Pk, Pi, Pj)가 볼록이라면,
				if (t > -djk && t < 0) 
				{
					// S'(x0, y0)과 Pi(x1, y1)의 거리를 계산하여, Ui의 거리로 설정한다.
					Ui = SQRT(SQR(x1 - x0) + SQR(y1 - y0));
				}
				else	// 다각형 (S', Pk, Pi, Pj)가 오목이라면,
				{
					// Dijkstra 알고리즘을 적용한다.
					Ui = (Uj + dij < Uk + dik) ? (Uj + dij) : (Uk + dik);					
				}
			}
			else	// 삼각형이 형성되지 않는 경우라면,
			{
				// Dijkstra 알고리즘을 적용한다.
				Ui = (Uj + dij < Uk + dik) ? (Uj + dij) : (Uk + dik);				
			}			

			// 계산된 거리가 이전에 계산된 거리보다 작다면,
			if (Ui < Distances[i])
			{
				// 최단 거리를 갱신하고, 힙에 정점의 인덱스를 추가한다.
				Distances[i] = Ui;
				if (Ui < MaxRadius)
					MinHeap.push(i);
			}
		}
	}	
}

/*! 
*	\brief 선택된 정점(삼각형)들에서 메쉬의 각 정점까지의 측지거리를 계산하는 함수
*
*	\param pMesh 대상 메쉬
*	\param VertList 선택된 메쉬 상의 정점(삼각형) 리스트
*	\param Distances pVert에서 i번째 정점까지의 측지거리가 Distances[i]에 저장된다.
*	\param MaxRadius 측지거리를 계산할 최대 반경
*/
void get_geodesic_dist_field(GMesh *pMesh, std::vector<GEditObj *> &ObjList, std::vector<double> &Distances, double MaxRadius)
{
	// 정점이나 삼각형이 선택된 경우가 아니라면 리턴한다.
	std::string ClsName = ObjList[0]->GetClassName();
	if (ClsName != "GVertex" && ClsName != "GFace")
		return;

	// 미리 계산된 위상 정보를 저장하기 위한 정적 변수를 사용한다.
	static GMesh *pPrevMesh = NULL;
	static int PrevNumVert = 0;
	static std::vector<std::vector<int>> VertList;	

	// 대상 메쉬가 바뀌거나 기하정보가 변경되었다면,
	int NumVert = pMesh->GetNumVert();
	if (pPrevMesh != pMesh || PrevNumVert != NumVert)
	{
		// 새롭게 위상 정보를 구한다.
		::get_neighbor_verts_of_vert(pMesh, VertList);
		pPrevMesh = pMesh;
		PrevNumVert = NumVert;
	}

	// 거리장을 초기화 하고,
	Distances.assign(NumVert, 1000000.0);

	// 최소 힙을 생성한다.
	GHeap MinHeap(&Distances);
	int NumSltObj = (int)ObjList.size();

	// 선택된 객체가 정점이라면,
	if (ClsName == "GVertex")
	{
		// 각 정점에 대하여,
		for (int i = 0; i < NumSltObj; ++i)
		{
			GVertex *v = (GVertex *)ObjList[i];
			// 거리를 0으로 초기화 하고, 힙에 추가한다.
			Distances[v->Idx] = 0.0;
			MinHeap.push(v->Idx);
		}
	}

	// 선택된 객체가 삼각형이라면,
	if (ClsName == "GFace")
	{
		// 각 삼각형에 대하여,
		for (int i = 0; i < NumSltObj; ++i)
		{
			GFace *f = (GFace *)ObjList[i];
			int *vidx = f->vIdx;

			// 각 정점의 거리를 0으로 초기화 하고, 힙에 추가한다.
			Distances[vidx[0]] = 0.0;
			Distances[vidx[1]] = 0.0;
			Distances[vidx[2]] = 0.0;
			MinHeap.push(vidx[0]);
			MinHeap.push(vidx[1]);
			MinHeap.push(vidx[2]);
		}
	}

	// 처리할 정점이 남아 있다면,
	while (!MinHeap.empty())
	{
		// 최소 거리를 갖는 정점의 인덱스를 구하여,
		int j = MinHeap.get_candidate();

		// 이웃한 모든 삼각형에 대하여,
		int valance = (int)VertList[j].size();
		for (int l = 0; l < valance; ++l)
		{
			// 삼각형 (vi, vj, vk)에 대하여,
			int i = VertList[j][l];
			int k = (l == 0) ? VertList[j][valance - 1] : VertList[j][l - 1];

			// 세 정점의 좌표를 구하고,
			GPoint3 Pi = pMesh->Verts[i].P;
			GPoint3 Pj = pMesh->Verts[j].P;
			GPoint3 Pk = pMesh->Verts[k].P;

			// 현재가지 계산된 두 정점의 최소거리 Uj, Uk를 구하고,
			double Ui = 0.0;
			double Uj = Distances[j];
			double Uk = Distances[k];

			// 삼각형의 각 변의 길이와 제곱을 계산하고,
			double dij_sq = dist_sq(Pi, Pj);
			double dik_sq = dist_sq(Pi, Pk);
			double djk_sq = dist_sq(Pj, Pk);
			double dij = SQRT(dij_sq);
			double dik = SQRT(dik_sq);			
			double djk = SQRT(djk_sq);

			// 만약, 삼각형이 생성된다면,
			if (ABS(Uj - Uk) < djk && djk < Uj + Uk)
			{
				// 2차원 평면에 매핑한 S'(x0, y0), Pi(x1, y1), Pj(0, 0), Pk(-djk, 0)의 좌표를 구하고,
				double x0 = (SQR(Uk) - SQR(Uj) - djk_sq) / (2 * djk);
				double y0 = SQRT(SQR(Uj) - SQR(x0));				
				double x1 = (dik_sq - dij_sq - djk_sq) / (2 * djk);
				double y1 = -SQRT(dij_sq - SQR(x1));

				// S'과 Pi의 거리를 계산하여, Ui의 거리로 설정한다.
				double m = (y1 - y0) / (x1 - x0);
				double t = x0 - y0 / m;

				// 다각형 (S', Pk, Pi, Pj)가 볼록이라면,
				if (t > -djk && t < 0) 
				{
					// S'(x0, y0)과 Pi(x1, y1)의 거리를 계산하여, Ui의 거리로 설정한다.
					Ui = SQRT(SQR(x1 - x0) + SQR(y1 - y0));
				}
				else	// 다각형 (S', Pk, Pi, Pj)가 오목이라면,
				{
					// Dijkstra 알고리즘을 적용한다.
					Ui = (Uj + dij < Uk + dik) ? (Uj + dij) : (Uk + dik);					
				}
			}
			else	// 삼각형이 형성되지 않는 경우라면,
			{
				// Dijkstra 알고리즘을 적용한다.
				Ui = (Uj + dij < Uk + dik) ? (Uj + dij) : (Uk + dik);				
			}	

			// 계산된 거리가 이전에 계산된 거리보다 작다면,
			if (Ui < Distances[i])
			{
				// 최단 거리를 갱신하고, 힙에 정점의 인덱스를 추가한다.
				Distances[i] = Ui;
				if (Ui < MaxRadius)
					MinHeap.push(i);
			}
		}
	}
}

/*! 
*	\brief 삼각형 내부의 한 점에서 메쉬의 각 정점까지의 측지거리를 계산하는 함수
*
*	\param pMesh 대상 메쉬
*	\param fidx 선택된 정점이 포함된 삼각형의 인덱스
*	\param Coords  삼각형 내부에서 선택된 정점의 baricentric coordinates
*	\param Distances 선택된 삼각형 내부의 정점에서 i번째 정점까지의 측지거리가 Distances[i]에 저장된다.
*	\param MaxRadius 측지거리를 계산할 최대 반경
*/
void get_geodesic_dist_field(GMesh *pMesh, int fidx, GVector3 Coords, std::vector<double> &Distances, double MaxRadius)
{
	// 미리 계산된 위상 정보를 저장하기 위한 정적 변수를 사용한다.
	static GMesh *pPrevMesh = NULL;
	static int PrevNumVert = 0;
	static std::vector<std::vector<int>> VertList;	

	// 대상 메쉬가 바뀌거나 기하정보가 변경되었다면,
	int NumVert = pMesh->GetNumVert();
	if (pPrevMesh != pMesh || PrevNumVert != NumVert)
	{
		// 새롭게 위상 정보를 구한다.
		::get_neighbor_verts_of_vert(pMesh, VertList);
		pPrevMesh = pMesh;
		PrevNumVert = NumVert;
	}

	// 거리장을 초기화 하고,
	Distances.assign(NumVert, 1000000.0);

	// 최소 힙을 생성한다.
	GHeap MinHeap(&Distances);
	
	// 삼각형 내부의 정점의 좌표를 계산한다.
	int *vidx = pMesh->Faces[fidx].vIdx;
	GPoint3 p0 = pMesh->Verts[vidx[0]].P;
	GPoint3 p1 = pMesh->Verts[vidx[1]].P;
	GPoint3 p2 = pMesh->Verts[vidx[2]].P;
	GPoint3 pt = p2 + (p0 - p2) * Coords[0] + (p1 - p2) * Coords[1];

	// 선택된 정점으로부터의 거리를 계산하고 인덱스를 힙에 추가한다.
	Distances[vidx[0]] = dist(pt, p0);
	Distances[vidx[1]] = dist(pt, p1);
	Distances[vidx[2]] = dist(pt, p2);
	MinHeap.push(vidx[0]);
	MinHeap.push(vidx[1]);
	MinHeap.push(vidx[2]);

	// 처리할 정점이 남아 있다면,
	while (!MinHeap.empty())
	{
		// 최소 거리를 갖는 정점의 인덱스를 구하여,
		int j = MinHeap.get_candidate();

		// 이웃한 모든 삼각형에 대하여,
		int valance = (int)VertList[j].size();
		for (int l = 0; l < valance; ++l)
		{
			// 삼각형 (vi, vj, vk)에 대하여,
			int i = VertList[j][l];
			int k = (l == 0) ? VertList[j][valance - 1] : VertList[j][l - 1];

			// 세 정점의 좌표를 구하고,
			GPoint3 Pi = pMesh->Verts[i].P;
			GPoint3 Pj = pMesh->Verts[j].P;
			GPoint3 Pk = pMesh->Verts[k].P;

			// 현재가지 계산된 두 정점의 최소거리 Uj, Uk를 구하고,
			double Ui = 0.0;
			double Uj = Distances[j];
			double Uk = Distances[k];

			// 삼각형의 각 변의 길이와 제곱을 계산하고,
			double dij_sq = dist_sq(Pi, Pj);
			double dik_sq = dist_sq(Pi, Pk);
			double djk_sq = dist_sq(Pj, Pk);
			double dij = SQRT(dij_sq);
			double dik = SQRT(dik_sq);			
			double djk = SQRT(djk_sq);

			// 만약, 삼각형이 생성된다면,
			if (ABS(Uj - Uk) < djk && djk < Uj + Uk)
			{
				// 2차원 평면에 매핑한 S'(x0, y0), Pi(x1, y1), Pj(0, 0), Pk(-djk, 0)의 좌표를 구하고,
				double x0 = (SQR(Uk) - SQR(Uj) - djk_sq) / (2 * djk);
				double y0 = SQRT(SQR(Uj) - SQR(x0));				
				double x1 = (dik_sq - dij_sq - djk_sq) / (2 * djk);
				double y1 = -SQRT(dij_sq - SQR(x1));

				// S'과 Pi의 거리를 계산하여, Ui의 거리로 설정한다.
				double m = (y1 - y0) / (x1 - x0);
				double t = x0 - y0 / m;

				// 다각형 (S', Pk, Pi, Pj)가 볼록이라면,
				if (t > -djk && t < 0) 
				{
					// S'(x0, y0)과 Pi(x1, y1)의 거리를 계산하여, Ui의 거리로 설정한다.
					Ui = SQRT(SQR(x1 - x0) + SQR(y1 - y0));
				}
				else	// 다각형 (S', Pk, Pi, Pj)가 오목이라면,
				{
					// Dijkstra 알고리즘을 적용한다.
					Ui = (Uj + dij < Uk + dik) ? (Uj + dij) : (Uk + dik);					
				}
			}
			else	// 삼각형이 형성되지 않는 경우라면,
			{
				// Dijkstra 알고리즘을 적용한다.
				Ui = (Uj + dij < Uk + dik) ? (Uj + dij) : (Uk + dik);				
			}			

			// 계산된 거리가 이전에 계산된 거리보다 작다면,
			if (Ui < Distances[i])
			{
				// 최단 거리를 갱신하고, 힙에 정점의 인덱스를 추가한다.
				Distances[i] = Ui;
				if (Ui < MaxRadius)
					MinHeap.push(i);
			}
		}
	}
}

/*! 
*	\brief 한 정점에서 메쉬의 각 정점까지의 edge 거리를 계산하는 함수
*
*	\param pMesh 대상 메쉬
*	\param pVert 선택된 메쉬 상의 정점
*	\param Distances pVert에서 i번째 정점까지의 측지거리가 Distances[i]에 저장된다.
*	\param MaxRadius 측지거리를 계산할 최대 반경
*/
void get_edge_dist_field(GMesh *pMesh, GVertex *pVert, std::vector<double> &Distances, double MaxRadius)
{
	// 미리 계산된 위상 정보를 저장하기 위한 정적 변수를 사용한다.
	static GMesh *pPrevMesh = NULL;
	static int PrevNumVert = 0;
	static std::vector<std::vector<int>> VertList;	

	// 대상 메쉬가 바뀌거나 기하정보가 변경되었다면,
	int NumVert = pMesh->GetNumVert();
	if (pPrevMesh != pMesh || PrevNumVert != NumVert)
	{
		// 새롭게 위상 정보를 구한다.
		::get_neighbor_verts_of_vert(pMesh, VertList);
		pPrevMesh = pMesh;
		PrevNumVert = NumVert;
	}

	// 거리장을 초기화 하고,
	Distances.assign(NumVert, 1000000.0);

	// 최소 힙을 생성한다.
	GHeap MinHeap(&Distances);

	// 선택된 정점의 거리를 0으로 초기화 하고, 인덱스를 힙에 추가한다.
	Distances[pVert->Idx] = 0.0;
	MinHeap.push(pVert->Idx);

	// 처리할 정점이 남아 있다면,
	while (!MinHeap.empty())
	{
		// 최소 거리를 갖는 정점의 인덱스를 구하여,
		int j = MinHeap.get_candidate();

		// 이웃한 모든 삼각형에 대하여,
		int valance = (int)VertList[j].size();
		for (int l = 0; l < valance; ++l)
		{
			// 삼각형 (vi, vj, vk)에 대하여,
			int i = VertList[j][l];
			int k = (l == 0) ? VertList[j][valance - 1] : VertList[j][l - 1];

			// 세 정점의 좌표를 구하고,
			GPoint3 Pi = pMesh->Verts[i].P;
			GPoint3 Pj = pMesh->Verts[j].P;
			GPoint3 Pk = pMesh->Verts[k].P;

			// 현재가지 계산된 두 정점의 최소거리 Uj, Uk를 구하고,
			double Ui = 0.0;
			double Uj = Distances[j];
			double Uk = Distances[k];

			// 삼각형의 각 변의 길이를 계산하고,
			double dij = dist(Pi, Pj);
			double dik = dist(Pi, Pk);
			double djk = dist(Pj, Pk);

			// Dijkstra 알고리즘을 적용한다.
			Ui = (Uj + dij < Uk + dik) ? (Uj + dij) : (Uk + dik);

			// 계산된 거리가 이전에 계산된 거리보다 작다면,
			if (Ui < Distances[i])
			{
				// 최단 거리를 갱신하고, 힙에 정점의 인덱스를 추가한다.
				Distances[i] = Ui;
				if (Ui < MaxRadius)
					MinHeap.push(i);
			}
		}
	}	
}

/*! 
*	\brief 삼각형 내부의 한 점에서 메쉬의 각 정점까지의 측지거리를 계산하는 함수
*
*	\param pMesh 대상 메쉬
*	\param fidx 선택된 정점이 포함된 삼각형의 인덱스
*	\param Coords  삼각형 내부에서 선택된 정점의 baricentric coordinates
*	\param Distances 선택된 삼각형 내부의 정점에서 i번째 정점까지의 측지거리가 Distances[i]에 저장된다.
*	\param MaxRadius 측지거리를 계산할 최대 반경
*/
void get_edge_dist_field(GMesh *pMesh, int fidx, GVector3 Coords, std::vector<double> &Distances, double MaxRadius)
{
	// 미리 계산된 위상 정보를 저장하기 위한 정적 변수를 사용한다.
	static GMesh *pPrevMesh = NULL;
	static int PrevNumVert = 0;
	static std::vector<std::vector<int>> VertList;	

	// 대상 메쉬가 바뀌거나 기하정보가 변경되었다면,
	int NumVert = pMesh->GetNumVert();
	if (pPrevMesh != pMesh || PrevNumVert != NumVert)
	{
		// 새롭게 위상 정보를 구한다.
		::get_neighbor_verts_of_vert(pMesh, VertList);
		pPrevMesh = pMesh;
		PrevNumVert = NumVert;
	}

	// 거리장을 초기화 하고,
	Distances.assign(NumVert, 1000000.0);

	// 최소 힙을 생성한다.
	GHeap MinHeap(&Distances);

	// 삼각형 내부의 정점의 좌표를 계산한다.
	int *vidx = pMesh->Faces[fidx].vIdx;
	GPoint3 p0 = pMesh->Verts[vidx[0]].P;
	GPoint3 p1 = pMesh->Verts[vidx[1]].P;
	GPoint3 p2 = pMesh->Verts[vidx[2]].P;
	GPoint3 pt = p2 + (p0 - p2) * Coords[0] + (p1 - p2) * Coords[1];

	// 선택된 정점으로부터의 거리를 계산하고 인덱스를 힙에 추가한다.
	Distances[vidx[0]] = dist(pt, p0);
	Distances[vidx[1]] = dist(pt, p1);
	Distances[vidx[2]] = dist(pt, p2);
	MinHeap.push(vidx[0]);
	MinHeap.push(vidx[1]);
	MinHeap.push(vidx[2]);

	// 처리할 정점이 남아 있다면,
	while (!MinHeap.empty())
	{
		// 최소 거리를 갖는 정점의 인덱스를 구하여,
		int j = MinHeap.get_candidate();

		// 이웃한 모든 삼각형에 대하여,
		int valance = (int)VertList[j].size();
		for (int l = 0; l < valance; ++l)
		{
			// 삼각형 (vi, vj, vk)에 대하여,
			int i = VertList[j][l];
			int k = (l == 0) ? VertList[j][valance - 1] : VertList[j][l - 1];

			// 세 정점의 좌표를 구하고,
			GPoint3 Pi = pMesh->Verts[i].P;
			GPoint3 Pj = pMesh->Verts[j].P;
			GPoint3 Pk = pMesh->Verts[k].P;

			// 현재가지 계산된 두 정점의 최소거리 Uj, Uk를 구하고,
			double Ui = 0.0;
			double Uj = Distances[j];
			double Uk = Distances[k];

			// 삼각형의 각 변의 길이를 계산하고,
			double dij = dist(Pi, Pj);
			double dik = dist(Pi, Pk);
			double djk = dist(Pj, Pk);

			// Dijkstra 알고리즘을 적용한다.
			Ui = (Uj + dij < Uk + dik) ? (Uj + dij) : (Uk + dik);

			// 계산된 거리가 이전에 계산된 거리보다 작다면,
			if (Ui < Distances[i])
			{
				// 최단 거리를 갱신하고, 힙에 정점의 인덱스를 추가한다.
				Distances[i] = Ui;
				if (Ui < MaxRadius)
					MinHeap.push(i);
			}
		}
	}	
}

/*!
*	\brief 선택된 정점에서 메쉬의 각 정점까지의 최단거리를 이용한 극좌표를 계산한다.
*
*	\param pMesh 대상 메쉬
*	\param pVert 선택된 정점
*	\param Distances 각 정점의 극좌표 반지름 (Distances[i] = 선택된 정점에서 i번째 정점까지의 최단 거리)
*	\param Angles 각 정점의 극좌표 각도 (Angles[i] = i번째 정점의 극좌표 각도)
*	\param MaxRadius 거리장을 계산할 최대 반지름 거리값
*/
void get_geodesic_polar_coords(GMesh *pMesh, GVertex *pVert, std::vector<double> &Distances, std::vector<double> &Angles, double MaxRadius)
{
	// 미리 계산된 위상 정보를 저장하기 위한 정적 변수를 사용한다.
	static GMesh *pPrevMesh = NULL;
	static int PrevNumVert = 0;
	static std::vector<std::vector<int>> VertList;	

	// 대상 메쉬가 바뀌거나 기하정보가 변경되었다면,
	int NumVert = pMesh->GetNumVert();
	if (pPrevMesh != pMesh || PrevNumVert != NumVert)
	{
		// 새롭게 위상 정보를 구한다.
		::get_neighbor_verts_of_vert(pMesh, VertList);
		pPrevMesh = pMesh;
		PrevNumVert = NumVert;
	}

	// 거리장과 각도를 초기화 하고,
	Distances.assign(NumVert, 1000000.0);
	Angles.assign(NumVert, 0.0);

	double TotAngle = 0.0;
	int vidx0 = pVert->Idx;
	GPoint3 p0 = pMesh->Verts[vidx0].P;

	int Deg = (int)VertList[pVert->Idx].size();
	for (int i = 0; i < Deg; ++i)
	{
		int vidx1 = VertList[vidx0][i];
		int vidx2 = (i == Deg - 1) ? VertList[vidx0][0] : VertList[vidx0][i + 1];

		GPoint3 p1 = pMesh->Verts[vidx1].P;
		GPoint3 p2 = pMesh->Verts[vidx2].P;

		double theta = angle(p1 - p0, p2 - p0, true);
		TotAngle += theta;
	}

	double alpha = 2 * M_PI / TotAngle;

	Angles[VertList[vidx0][0]] = 0.0;
	for (int i = 1; i < Deg; ++i)
	{
		int vidx1 = VertList[vidx0][i - 1];
		int vidx2 = VertList[vidx0][i];

		GPoint3 p1 = pMesh->Verts[vidx1].P;
		GPoint3 p2 = pMesh->Verts[vidx2].P;

		Angles[vidx2] = Angles[vidx1] + alpha * angle(p1 - p0, p2 - p0, true);
	}

	// 최소 힙을 생성한다.
	GHeap MinHeap(&Distances);

	// 선택된 정점의 거리와 각도를 0으로 초기화 하고, 인덱스를 힙에 추가한다.
	Distances[pVert->Idx] = 0.0;
	Angles[pVert->Idx] = 0.0;
	MinHeap.push(pVert->Idx);

	// 처리할 정점이 남아 있다면,
	while (!MinHeap.empty())
	{
		// 최소 거리를 갖는 정점의 인덱스를 구하여,
		int j = MinHeap.get_candidate();

		int valance = (int)VertList[j].size();
		for (int r = 0; r < 2; ++r)
		{
			for (int l = 0; l < valance; ++l)
			{
				// 정점 Pj를 포함한 삼각형(i, j, k)의 정점 인덱스 i, k를 구하고,
				int i = VertList[j][l];
				int k = (l == 0) ? VertList[j][valance - 1] : VertList[j][l - 1];

				// 삼각형 각 정점의 위치를 구하고,
				GPoint3 Pi = pMesh->Verts[i].P;
				GPoint3 Pj = pMesh->Verts[j].P;
				GPoint3 Pk = pMesh->Verts[k].P;

				// 계산할 거리 Ui와 이미 결정된 거리 Uj, Uk를 구하고,
				double Ui = 0.0;
				double Uj = Distances[j];
				double Uk = Distances[k];

				// 계산할 각도 Thetai와 이미 결정된 각도 Thetaj, Thetak를 구하고,
				double Thetai = 0.0;
				double Thetaj = Angles[j];
				double Thetak = Angles[k];

				// 삼각형의 각변의 길이의 제곱과 길이를 계산한다.
				double dij_sq = dist_sq(Pi, Pj);
				double dik_sq = dist_sq(Pi, Pk);
				double djk_sq = dist_sq(Pj, Pk);
				double dij = SQRT(dij_sq);
				double dik = SQRT(dik_sq);
				double djk = SQRT(djk_sq);

				// 만약, 삼각형이 생성된다면,
				if (ABS(Uj - Uk) < djk && djk < Uj + Uk)
				{
					// 2차원 평면에 매핑한 S'(x0, y0), Pi(x1, y1), Pj(0, 0), Pk(-djk, 0)의 좌표를 구하고,
					double x0 = (SQR(Uk) - SQR(Uj) - djk_sq) / (2 * djk);
					double y0 = SQRT(SQR(Uj) - SQR(x0));				
					double x1 = (dik_sq - dij_sq - djk_sq) / (2 * djk);
					double y1 = -SQRT(dij_sq - SQR(x1));

					// S'과 Pi의 거리를 계산하여, Ui의 거리로 설정한다.
					double m = (y1 - y0) / (x1 - x0);
					double t = x0 - y0 / m;

					// 다각형 (S', Pk, Pi, Pj)가 볼록이라면,
					if (t > -djk && t < 0) 
					{
						Ui = SQRT(SQR(x1 - x0) + SQR(y1 - y0));

						GVector3 u(-djk - x0, -y0, 0.0);
						GVector3 v(x1 - x0, y1 - y0, 0.0);
						GVector3 w(-x0, -y0, 0.0);

						double delta_kj = angle(u, w);
						double delta_ki = angle(u, v);
						double alpha = delta_ki / delta_kj;
						double diff = ABS((Thetaj - Thetak));

						if (diff < 0.00000001)
						{
							Thetai = Angles[j];
						}
						else
						{
							if (diff > M_PI) 
							{
								//Make the interpolation modulo 2pi
								if(Thetaj < Thetak) 
									Thetaj += 2 * M_PI;
								else 
									Thetak += 2 * M_PI;
							}

							Thetai = (1.0 - alpha) * Thetak + alpha * Thetaj;

							if(Thetai > 2 * M_PI)
								Thetai -= 2 * M_PI;	
						}
					}
					else	// 다각형 (S', Pk, Pi, Pj)가 오목이라면,
					{
						// Dijkstra 알고리즘을 사용한다.
						if(Uj + dij < Uk + dik)
						{
							Ui = Uj + dij;
							Thetai = Thetaj;
						}
						else 
						{
							Ui = Uk + dik;
							Thetai = Thetak;
						}
					}
				}
				else	// 삼각형이 형성되지 않는 경우라면,
				{
					// Dijkstra 알고리즘을 사용한다.
					if(Uj + dij < Uk + dik)
					{
						Ui = Uj + dij;
						Thetai = Thetaj;
					}
					else 
					{
						Ui = Uk + dik;
						Thetai = Thetak;
					}
				}

				// 계산된 거리가 현재 거리보다 작다면,
				if (Ui < Distances[i])
				{
					// 현재 거리와 각도를 갱신하고,
					Distances[i] = Ui;
					if (j != pVert->Idx)
						Angles[i] = Thetai;

					// 정점 Pi를 최소 힙에 입력한다.
					if (Ui < MaxRadius)
						MinHeap.push(i);
				}
			}
		}		
	}
}

/*!
*	\brief 메쉬 위에서 두 정점 사이의 최단 경로를 계산한다.
*
*	\param pMesh 대상 메쉬
*	\param P 시작 정점에 대한 포인터
*	\param Q 끝 정점에 대한 포인터
*	\param Paths 정점 P와 Q를 연결하는 최단 경로를 구성하는 점들이 저장된다.
*/
void get_geodesic_paths(GMesh *pMesh, GVertex *P, GVertex *Q, std::vector<GPoint3> &Paths)
{
	// 경로를 초기화 한다.
	Paths.clear();

	// 시작점과 끝점이 같으면 리턴한다.
	if (P == Q)
		return;

	// 시작 점을 추가한다.
	Paths.push_back(P->P);

	// 측지거리를 이용하여 정해진 영역내의 정점들의 극좌표(r, theta)를 계산한다.
	std::vector<double> Distances;
	std::vector<double> Angles;
	double Radius = dist(P->P, Q->P) * 1.5;
	::get_geodesic_polar_coords(pMesh, P, Distances, Angles, Radius);

	// 접평면에서 정점 P와 Q를 양 끝점으로 하는 직선 l(s)을 생성한다.
	double theta = Angles[Q->Idx];
	GLine l(GPoint3(0.0, 0.0, 0.0), GPoint3(Distances[Q->Idx] * cos(theta), Distances[Q->Idx] * sin(theta), 0.0));

	// P와 Q를 잊는 최단경로 정점의 정보를 저장할 변수를 정의한다.
	std::map<double, GPathInfo> PathInfoList;

	// 편집 메쉬의 각각의 삼각형에 대하여,
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		// P에서 삼각형의 각 정점까지의 거리를 구하고,
		int vidx0 = pMesh->Faces[i].vIdx[0];
		int vidx1 = pMesh->Faces[i].vIdx[1];
		int vidx2 = pMesh->Faces[i].vIdx[2];
		double d0 = Distances[vidx0];
		double d1 = Distances[vidx1];
		double d2 = Distances[vidx2];
		if (vidx0 == P->Idx || vidx1 == P->Idx || vidx2 == P->Idx)
			continue;

		// 삼각형이 영역 내에 포함된다면,
		if (d0 < Radius && d1 < Radius && d2 < Radius)
		{
			// 접평면에서 삼각형의 세 정점의 좌표를 구하여,
			GPoint3 p0(Distances[vidx0] * cos(Angles[vidx0]), Distances[vidx0] * sin(Angles[vidx0]), 0.0);
			GPoint3 p1(Distances[vidx1] * cos(Angles[vidx1]), Distances[vidx1] * sin(Angles[vidx1]), 0.0);
			GPoint3 p2(Distances[vidx2] * cos(Angles[vidx2]), Distances[vidx2] * sin(Angles[vidx2]), 0.0);

			// 각 선분에 해당되는 직선의 방정식 l(t)를 생성한다.
			GLine l01(p0, p1);
			GLine l12(p1, p2);
			GLine l20(p2, p0);

			// 최단 경로와 선분과의 교차점을 추가한다.
			double s, t;
			GPoint3 ispt;
			if (intersect_line_line(ispt, l, l01, s, t))
			{
				std::pair<double, GPathInfo> p(s, GPathInfo(vidx0, vidx1, t));
				PathInfoList.insert(p);
			}
			if (intersect_line_line(ispt, l, l12, s, t))
			{
				std::pair<double, GPathInfo> p(s, GPathInfo(vidx1, vidx2, t));
				PathInfoList.insert(p);
			}
			if (intersect_line_line(ispt, l, l20, s, t))
			{
				std::pair<double, GPathInfo> p(s, GPathInfo(vidx2, vidx0, t));
				PathInfoList.insert(p);
			}
		}
	}

	// 접평면에서 계산된 교차점의 정보를 실제 3차원 메쉬상의 최단거리 정보로 변환한다.
	std::map<double, GPathInfo>::iterator it;
	for (it = PathInfoList.begin(); it != PathInfoList.end(); ++it)
	{
		GPoint3 p0 = pMesh->Verts[it->second.sidx].P;
		GPoint3 p1 = pMesh->Verts[it->second.eidx].P;
		double t = it->second.t;
		GPoint3 p = p0 + t * (p1 - p0);
		int NumPt = (int)Paths.size();
		if (dist(Paths[NumPt - 1], p) > 1.0e-6)
			Paths.push_back(p);
	}

	// 시작점만 경로에 포함되었다면, 끝점을 추가한다.
	if (Paths.size() < 2)
		Paths.push_back(Q->P);
}

/*!
*	\brief 메쉬의 각 정점을 공유한 삼각형 인덱스를 생성한다
*	\note i번째 정점을 공유한 n 개의 삼각형들의 인덱스는 FaceList[i][0], FaceList[i][1], ... FaceList[i][n-1]이 됨
*
*	\param pMesh 대상 메쉬
*	\param FaceList 삼각형 인덱스가 저장될 벡터 리스트 (반시계 방향으로 정렬)
*/
void get_neighbor_faces_of_vert(GMesh *pMesh, std::vector<std::vector<int>> &FaceList)
{
	std::vector<int> tmp;
	int vnum = pMesh->GetNumVert();
	FaceList.assign(vnum, tmp);

	std::vector<GFace> &Faces = pMesh->Faces;
	std::vector<GVertex> &Verts = pMesh->Verts;

	// 일단, 정점의 인덱스에 해당하는 위치에 삼각형의 인덱스를 추가하고,
	int fnum = pMesh->GetNumFace();
	for (int i = 0; i < fnum; ++i)
	{
		FaceList[Faces[i].vIdx[0]].push_back(i);
		FaceList[Faces[i].vIdx[1]].push_back(i);
		FaceList[Faces[i].vIdx[2]].push_back(i);
	}

	// 삼각형 리스트를 반시계 방향으로 정렬하기 위해서,
	for (int i = 0; i < vnum; ++i)
	{
		// 각 정점을 공유한 삼각형의 개수를 구하고,
		int deg = FaceList[i].size();
		for (int j = 0; j < deg; ++j)
		{
			// 시작 삼각형을 결정한 후,
			tmp.clear();
			tmp.push_back(FaceList[i][j]);

			for (int k = 1; k < deg; ++k)
			{
				// 현재 삼각형의 인덱스를 구하고,
				int curr_fidx = tmp[tmp.size() - 1];

				// 정점 i를 기준으로 삼각형의 인덱스를 재정렬하고,
				int vidx0 = i;
				int vidx1 = Faces[curr_fidx].GetNextVertIdx(vidx0);
				int vidx2 = Faces[curr_fidx].GetNextVertIdx(vidx1);

				bool flag = false;
				// 현재(시작) 삼각형과 인접한 다음 삼각형을 찾는다.
				for (int l = 0; l < deg; ++l)
				{
					int next_fidx = FaceList[i][l];
					if (next_fidx == curr_fidx)
						continue;

					// 인접한 삼각형이라면, 인덱스를 저장하고 빠짐.
					if (vidx2 == Faces[next_fidx].GetNextVertIdx(vidx0))
					{
						tmp.push_back(next_fidx);
						flag = true;
						break;
					}
				}

				if (!flag)
					break;
			}

			// 인접한 삼각형이 모두 저장되었다면, 
			if (tmp.size() == deg)
			{
				// 빠져나오고,
				FaceList[i] = tmp;
				break;
			}
			// 그렇지 않다면, 시작 삼각형을 바꾸어 다시 시작한다.
		}
	}
}

/*!
*	\brief 메쉬의 각 정점에 인접한 정점의 인덱스를 생성한다
*	\note i번째 정점에 인접한 n개의 정점의 인덱스는 VertList[i][0], VertList[i][1], ... VertList[i][n-1]에 저장됨
*
*	\param pMesh 대상 메쉬
*	\param VertList 인접한 정점의 인덱스가 저장됨 (반시계 방향으로 정렬)
*/
void get_neighbor_verts_of_vert(GMesh *pMesh, std::vector<std::vector<int>> &VertList)
{
	// 정점을 공유한 삼각형 인덱스를 구하고,
	std::vector<std::vector<int>> FaceList;
	get_neighbor_faces_of_vert(pMesh, FaceList);

	std::vector<int> dummy;
	int vnum = pMesh->GetNumVert();
	VertList.assign(vnum, dummy);

	for (int i = 0; i < vnum; ++i)
	{
		int val = FaceList[i].size();
		for (int j = 0; j < val; ++j)
		{
			// 정점 i를 공유한 삼각형들에 대하여,
			int fidx = FaceList[i][j];
			int vidx1 = pMesh->Faces[fidx].GetNextVertIdx(i);
			int vidx2 = pMesh->Faces[fidx].GetNextVertIdx(vidx1);

			if (j == 0)
			{
				VertList[i].push_back(vidx1);
				VertList[i].push_back(vidx2);
			}
			else if (vidx2 != VertList[i][0])
				VertList[i].push_back(vidx2);
		}
	}
}

/*!
*	\brief 메쉬의 각 삼각형의 인접한 삼각형 인덱스를 생성한다.
*	\note [f0, f1, f2, f3, ..., fn] -> [(f00, f01, f02), (f10, f11, f12), ..., (fn0, fn1, fn2)]
*	\note i번째 삼각형의 인접한 삼각형의 인덱스 정보는 FaceList[i * 3], FaceList[i * 3 + 1], FaceList[i * 3 + 2]에 저장됨
*	\note 인접한 삼각형의 저장 순서는 e0, e1, e2에 대응 모서리를 공유한 삼각형 순서로 저장됨, (중요: ei 는 vi에 대응하는 모서리).
*
*	\param pMesh 대상 메쉬
*	\param FaceList 삼각형의 인접한 삼각형의 인덱스가 저장됨 (인접한 삼각형이 없으면 -1이 저장됨)
*/
void get_neighbor_faces_of_face(GMesh *pMesh, std::vector<int> &FaceList)
{
	// 정점을 공유한 삼각형 인덱스를 구하고,
	std::vector<std::vector<int>> VertList;
	::get_neighbor_faces_of_vert(pMesh, VertList);

	std::vector<GFace> &Faces = pMesh->Faces;

	// 삼각형 리스트를 초기화 하고,
	int fnum = pMesh->GetNumFace();
	FaceList.assign(fnum * 3, -1);

	for (int i = 0; i < fnum; ++i)
	{
		int vidx0 = Faces[i].vIdx[0];
		int vidx1 = Faces[i].vIdx[1];
		int vidx2 = Faces[i].vIdx[2];

		int val = (int)VertList[vidx0].size();
		for (int j = 0; j < val; ++j)
		{
			int fidx = VertList[vidx0][j];
			int prev_vidx = Faces[fidx].GetPrevVertIdx(vidx0);
			if (prev_vidx == vidx1)
			{
				FaceList[i * 3 + 2] = fidx;
				break;
			}
		}

		val = (int)VertList[vidx1].size();
		for (int j = 0; j < val; ++j)
		{
			int fidx = VertList[vidx1][j];
			int prev_vidx = Faces[fidx].GetPrevVertIdx(vidx1);
			if (prev_vidx == vidx2)
			{
				FaceList[i * 3] = fidx;
				break;
			}
		}

		val = (int)VertList[vidx2].size();
		for (int j = 0; j < val; ++j)
		{
			int fidx = VertList[vidx2][j];
			int prev_vidx = Faces[fidx].GetPrevVertIdx(vidx2);
			if (prev_vidx == vidx0)
			{
				FaceList[i * 3 + 1] = fidx;
				break;
			}
		}
	}
}

/*!
*	\brief 주어진 정점의 K-ring Neighborhood 정보를 구한다.
*
*	\param pMesh 대상 메쉬
*	\param VertIdx 주어진 정점의 인덱스
*	\param K 탐색할 단계
*	\param VertList 각 단계(0 ~ K)에서 인접한 정점의 리스트가 저장됨 (VertList[0][]: 0-ring 정점, VertList[1][]: 1-ring 정점, ...)
*	\param FaceList 각 단계(0 ~ K-1)에서 인접한 삼각형의 리스트가 저장됨 (FaceList[0][]: 0-ring 삼각형, FaceList[1][]: 1-ring 삼각형, ...)
*/
void get_neighbor_info_of_vert(GMesh *pMesh, int VertIdx, int K, std::vector<std::vector<int>> &VertList, std::vector<std::vector<int>> &FaceList)
{
	// 정점과 삼각형 리스트를 초기화 한다.
	std::vector<int> tmp;
	VertList.clear();
	FaceList.clear();
	VertList.assign(K + 1, tmp);
	FaceList.assign(K, tmp);

	// 메쉬의 모든 정점에 대하여 인접한 정점의 리스트를 생성한다.
	std::vector<std::vector<int>> AdjVertList;
	::get_neighbor_verts_of_vert(pMesh, AdjVertList);

	// 메쉬의 모든 정점에 대하여 인접한 삼각형의 리스트를 생성한다.
	std::vector<std::vector<int>> AdjFaceList;
	::get_neighbor_faces_of_vert(pMesh, AdjFaceList);

	// 추가된 정점과 삼각형의 정보를 저장하는 맵
	std::vector<bool> vFlags;
	std::vector<bool> fFlags;
	vFlags.assign(pMesh->GetNumVert(), false);
	fFlags.assign(pMesh->GetNumFace(), false);

	// 0 단계에 시작 정점의 인덱스를 추가하고,
	VertList[0].push_back(VertIdx);
	vFlags[VertIdx] = true;

	// 각 단계(1, 2, ... , K)에서 인접한 정점의 리스트를 구한다.
	for (int i = 1; i < K + 1; ++i)
	{
		// 이전 단계의 정점 리스트의 개수를 구하고,
		int PrevNumVert = (int)VertList[i - 1].size();

		// 이전 단계의 각각의 정점에 대하여,
		for (int j = 0; j < PrevNumVert; ++j)
		{
			int PrevVertIdx = VertList[i - 1][j];
			int NewNumVert = (int)AdjVertList[PrevVertIdx].size();

			// 이전 단계의 각각의 정점의 이웃정점들에 대하여,
			for (int k = 0; k < NewNumVert; ++k)
			{
				// 이웃정점들의 인덱스를 구하고 리스트에 추가한다.
				int NewVertIdx = AdjVertList[PrevVertIdx][k];
				if (vFlags[NewVertIdx] == false)	// 새로운 정점의 인덱스라면
				{
					VertList[i].push_back(NewVertIdx);
					vFlags[NewVertIdx] = true;					
				}
			}
		}
	}

	// 각 단계(0, 1, ... , K - 1)에서 인접한 삼각형의 리스트를 구한다.
	for (int i = 0; i < K; ++i)
	{
		// 정점 리스트의 개수를 구하고,
		int NumVert = (int)VertList[i].size();

		// 각각의 정점에 대하여,
		for (int j = 0; j < NumVert; ++j)
		{
			int VertIdx = VertList[i][j];
			int NumFace = (int)AdjFaceList[VertIdx].size();

			// 각각의 정점의 인접한 삼각형들에 대하여,
			for (int k = 0; k < NumFace; ++k)
			{
				// 이웃정점들의 인덱스를 구하고 리스트에 추가한다.
				int FaceIdx = AdjFaceList[VertIdx][k];
				if (fFlags[FaceIdx] == false)	// 새로운 삼각형 인덱스라면,
				{
					FaceList[i].push_back(FaceIdx);
					fFlags[FaceIdx] = true;					
				}
			}
		}
	}
}

/*!
*	\brief 메쉬의 각 정점이 경계 정점인지 조사한다.
*
*	\param pMesh 대상 메쉬
*	\param InfoList 경계 정점 여부가 저장됨, 정점 i가 경계 정점이면, InfoList[i] = true.
*/
void get_bndry_vert_info(GMesh *pMesh, std::vector<bool> &InfoList)
{
	// 정보 리스트를 초기화하고,
	InfoList.clear();
	InfoList.assign(pMesh->GetNumVert(), false);

	// 각 삼각형의 인접한 삼각형 정보를 구하고, 
	std::vector<int> FaceList;
	::get_neighbor_faces_of_face(pMesh, FaceList);

	// 경계 정점 여부를 판단하여 기록한다.
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		int *vidx = pMesh->Faces[i].vIdx;

		if (FaceList[i * 3] == -1)
			InfoList[vidx[1]] = true;

		if (FaceList[i * 3 + 1] == -1)
			InfoList[vidx[2]] = true;

		if (FaceList[i * 3 + 2] == -1)
			InfoList[vidx[0]] = true;
	}
}

/*!
*	\brief 메쉬의 경계 모서리를 구한다.
*
*	\param pMesh 대상 메쉬
*	\param EdgeList 경계 모서리의 (끝 인덱스, 시작 인덱스)를 맵의 형태로 저장한다.
*/
void get_bndry_edges(GMesh *pMesh, std::map<int, int> &EdgeList)
{
	// 경계 모서리의 정보를 초기화 한다.
	EdgeList.clear();

	// 각 삼각형의 인접한 삼각형 정보를 구하고, 
	std::vector<int> FaceList;
	get_neighbor_faces_of_face(pMesh, FaceList);

	// 경계 모서리의 정보를 저장한다.
	int Numface = pMesh->GetNumFace();
	for (int i = 0; i < Numface; ++i)
	{
		int vidx0 = pMesh->Faces[i].vIdx[0];
		int vidx1 = pMesh->Faces[i].vIdx[1];
		int vidx2 = pMesh->Faces[i].vIdx[2];

// 		if (FaceList[i * 3] == -1)
// 			EdgeList[vidx1] = vidx2;
// 
// 		if (FaceList[i * 3 + 1] == -1)
// 			EdgeList[vidx2] = vidx0;
// 
// 		if (FaceList[i * 3 + 2] == -1)
// 			EdgeList[vidx0] = vidx1;

		if (FaceList[i * 3] == -1)
			EdgeList[vidx2] = vidx1;

		if (FaceList[i * 3 + 1] == -1)
			EdgeList[vidx0] = vidx2;

		if (FaceList[i * 3 + 2] == -1)
			EdgeList[vidx1] = vidx0;
	}
}

/*!
*	\brief 외부 경계와 내부 경계(구멍) 정점 인덱스를 반환한다.
*	\brief 여러 개의 경계(구멍)가 구해질 수 있으며, 
*	\brief i번째 구멍의 경계 정점의 인덱스는 VertList[i][0], VertList[i][1], ... VertList[i][n]에 저장된다.
*
*	\param pMesh 대상 메쉬
*	\param VertList 경계 정점의 인덱스가 저장됨
*/
void get_bndry_verts(GMesh *pMesh, std::vector<std::vector<int>> &VertList)
{
	// 각 삼각형의 인접한 삼각형 정보를 구하고, 
	std::vector<int> FaceList;
	get_neighbor_faces_of_face(pMesh, FaceList);

	// 경계 모서리의 <끝점 인덱스, 시작점 인덱스>로 구성된 맵을 생성하고,
	std::map<int, int> EdgeList;
	int fnum = pMesh->GetNumFace();
	for (int i = 0; i < fnum; ++i)
	{
		int vidx0 = pMesh->Faces[i].vIdx[0];
		int vidx1 = pMesh->Faces[i].vIdx[1];
		int vidx2 = pMesh->Faces[i].vIdx[2];

		if (FaceList[i * 3] == -1)
			EdgeList[vidx2] = vidx1;

		if (FaceList[i * 3 + 1] == -1)
			EdgeList[vidx0] = vidx2;

		if (FaceList[i * 3 + 2] == -1)
			EdgeList[vidx1] = vidx0;
	}

	if (EdgeList.empty())
		return;

	// 경계점들을 반시계 방향으로 정렬한다.
	std::map<int, int>::iterator it;
	while (!EdgeList.empty())
	{
		it = EdgeList.begin();
		int start_key = it->first;
		int current_key = start_key;

		std::vector<int> tmp;
		do 
		{
			int val = EdgeList[current_key];
			tmp.push_back(val);
			EdgeList.erase(current_key);
			current_key = val;
		} while (current_key != start_key);

		VertList.push_back(tmp);
	}
}

/*!
*	\brief 내부 정점의 인덱스를 반환한다.
*
*	\param pMesh 대상 메쉬
*	\param VertList 내부 정점의 인덱스가 저장됨
*/
void get_intr_verts(GMesh *pMesh, std::vector<int> &VertList)
{
	// 각 정점의 경계 정점 여부를 조사하고,
	std::vector<bool> InfoList;
	get_bndry_vert_info(pMesh, InfoList);

	int NumVert = pMesh->GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		// 내부 정점인 경우, 정점 리스트에 저장한다.
		if (InfoList[i] == false)
			VertList.push_back(i);
	}	
}

/*!
*	\brief 메쉬의 non-manifold 삼각형의 리스트를 찾는다.
*
*	\param pMesh 대상 메쉬
*	\param FaceList non-manifold 삼각형의 포인터가 저장된다.
*/
void find_non_manifold_faces(GMesh *pMesh, std::vector<GFace *> &FaceList)
{
	int NumFace = pMesh->GetNumFace();
	std::map<std::string, bool> EdgeCount;

	for (int i = 0; i < NumFace; ++i)
	{
		int vidx0 = pMesh->Faces[i].vIdx[0];
		int vidx1 = pMesh->Faces[i].vIdx[1];
		int vidx2 = pMesh->Faces[i].vIdx[2];

		std::string e0 = cast_str(vidx0) + "-" + cast_str(vidx1);
		std::string e1 = cast_str(vidx1) + "-" + cast_str(vidx2);
		std::string e2 = cast_str(vidx2) + "-" + cast_str(vidx0);

		if (EdgeCount[e0] == false)
			EdgeCount[e0] = true;
		else
			FaceList.push_back(&(pMesh->Faces[i]));
		
		if (EdgeCount[e1] == false)
			EdgeCount[e1] = true;
		else
			FaceList.push_back(&(pMesh->Faces[i]));

		if (EdgeCount[e2] == false)
			EdgeCount[e2] = true;
		else
			FaceList.push_back(&(pMesh->Faces[i]));
	}

	// 혹시 중복을 제거한다.
	sort(FaceList.begin(), FaceList.end());
	FaceList.erase(unique(FaceList.begin(), FaceList.end()), FaceList.end());
}

/*!
*	\brief 메쉬의 degenerated 삼각형의 리스트를 찾는다.
*
*	\param pMesh 대상 메쉬
*	\param FaceList degenerated 삼각형의 포인터가 저장된다.
*	\param eps degenerated 삼각형을 판단하기 위한 면적의 최소값
*/
void find_degenerated_faces(GMesh *pMesh, std::vector<GFace *> &FaceList, double eps)
{
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		int *vidx = pMesh->Faces[i].vIdx;
		GPoint3 p0 = pMesh->Verts[vidx[0]].P;
		GPoint3 p1 = pMesh->Verts[vidx[1]].P;
		GPoint3 p2 = pMesh->Verts[vidx[2]].P;

		GVector3 area = (p1 - p0) ^ (p2 - p0);
		if (norm(area) <= eps)
			FaceList.push_back(&(pMesh->Faces[i]));
	}

	// 혹시 중복을 제거한다.
	sort(FaceList.begin(), FaceList.end());
	FaceList.erase(unique(FaceList.begin(), FaceList.end()), FaceList.end());
}

/*!
*	\brief 메쉬의 isolated 정점의 리스트를 찾는다.
*
*	\param pMesh 대상 메쉬
*	\param VertList isolated 정점의 포인터가 저장된다.
*/
void find_isolated_verts(GMesh *pMesh, std::vector<GVertex *> &VertList)
{
	// 정점의 사용 여부를 체크하기 위해, 정점의 개수만큼 flag배열을 정의하고, false로 초기화한 후, 
	int NumVert = pMesh->GetNumVert();
	std::vector<bool> UsedFlag;
	UsedFlag.assign(NumVert, false);

	// 사용된 정점의 플래그만 true로 변경한다.
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		UsedFlag[pMesh->Faces[i].vIdx[0]] = true;
		UsedFlag[pMesh->Faces[i].vIdx[1]] = true;
		UsedFlag[pMesh->Faces[i].vIdx[2]] = true;
	}

	// 사용되지 않은 정점을 리스트에 추가한다.
	for (int i = 0; i < NumVert; ++i)
	{
		if (UsedFlag[i] == false)
			VertList.push_back(&(pMesh->Verts[i]));
	}

	// 혹시 중복을 제거한다.
	sort(VertList.begin(), VertList.end());
	VertList.erase(unique(VertList.begin(), VertList.end()), VertList.end());
}