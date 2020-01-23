#include "stdafx.h"

/*!
*	\brief ���� ������ ��ü�� �̸��� �����Ѵ�.
*
*	\param TypeObj ������ ��ü�� Ÿ��
*
*	\return ���� ������ ��ü�� �̸��� ��ȯ�Ѵ�.
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
*	\brief ���� ������ ���� �̸��� �����Ѵ�.
*
*	\param Prefix ������ ���� ���ξ�
*
*	\return ���� ������ ������ �̸��� ��ȯ�Ѵ�.
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
*	\brief 3���� �Ǹ���(Cylinder) �޽��� �����Ѵ�.
*
*	\param pAxisCrv �Ǹ����� �߽� �
*
*	\return ������ �Ǹ��� �޽��� ��ȯ�Ѵ�.
*/
GMesh *create_mesh_from_cylinder(GNurbsCrv *pAxisCrv)
{
	double radius = pAxisCrv->Renderer.CtrlPtSize * 0.4;
	int sv = pAxisCrv->Renderer.NumSampU;
	int su = 20;

	// RMF ����Ʈ�� ����Ѵ�.
	std::vector<GTransf> FrameList;
	::get_rotation_minimizing_frames(pAxisCrv, sv, FrameList);

	// �޽��� �����ϰ�,
	std::string Name = create_mesh_name("Cylinder");
	GMesh *pCylinder = new GMesh(Name, pAxisCrv->MC, FACE_QUAD, "");

	// ������� ������ ����Ѵ�.
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

	// �ﰢ���� �����ϰ� �߰��Ѵ�.
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
*	\brief 3���� ��(sphere) �޽��� �����Ѵ�.
*
*	\param Name ��ü�� �̸�
*	\param MC ��ü�� �𵨸� ��ǥ��
*	\param radius ������
*	\param slices ���� ��� ����
*	\param stacks �浵 ��� ����
*
*	\return ������ �� �޽��� ��ȯ�Ѵ�.
*/
GMesh *create_mesh_from_sphere(std::string Name, GTransf MC, double radius, int slices, int stacks)
{
	// ���� �����ϰ�,
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

	// �ﰢ���� �����ϰ� �߰��Ѵ�.
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
*	\brief 3���� icosahedron �޽��� �����Ѵ�.
*
*	\param Name ��ü�� �̸�
*	\param MC ��ü�� �𵨸� ��ǥ��
*	\param radius ������
*	\param subdiv_depth �ʱ� �޽� ���� �� ���� Ƚ��
*
*	\return ������ icosahedron �޽��� ��ȯ�Ѵ�.
*/
GMesh *create_mesh_from_icosahedron(std::string Name, GTransf MC, double radius, int subdiv_depth)
{
	GMesh *pSphere = new GMesh(Name, MC, FACE_TRI);
	
	// 12 ���� ������ �߰��Ѵ�.
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
	
	// 20���� �ﰢ���� �߰��Ѵ�.
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

	// ����ȭ �� ��ȯ
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
*	\brief Teapot �޽��� �����Ѵ�.
*
*	\param size ������ �޽��� ũ��
*
*	\return ������ Teapot�޽��� ��ȯ�Ѵ�.
*/
GMesh *create_mesh_from_teapot(double size)
{
	std::string Name = create_mesh_name("Teapot");

	GMesh *pTeapot = new GMesh(Name, GTransf(), FACE_TRI);

	return pTeapot;
}

/*!
*	\brief 3���� �ڽ� �޽��� �����Ѵ�.
*
*	\param p �𵨸� ��ǥ�迡���� �ּ���
*	\param q �𵨸� ��ǥ�迡���� �ּ���
*
*	\return ������ �ڽ� �޽��� ��ȯ�Ѵ�.
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
*	\brief �θ� �޽��� �κ� �޽�(submesh)�� �����Ѵ�.
*
*	\param Name �κ� �޽��� �̸�
*	\param pParentMesh �θ� �޽��� ���� ������
*	\param SltFaceList �θ� �޽��� ���õ� �ﰢ�� ����Ʈ
*	\param VertIdxMap �κ� �޽��� �θ� �޽��� ������ �������谡 ����ȴ�. �κ� �޽� ������ �ε��� = VertIdxMap[�θ� �޽� ������ �ε���].
*
*	\return ������ �κ� �޽��� ��ȯ�Ѵ�.
*/
GMesh *create_mesh_from_faces(std::string Name, GMesh *pParentMesh, std::vector<GFace *> &SltFaceList, std::map<int, int> &VertIdxMap)
{
	// ���õ� �ﰢ������ ������ ���ο� �޽��� �����ϰ�,
	GMesh *pSubMesh = new GMesh(Name, pParentMesh->MC, FACE_TRI, pParentMesh->Name);

	// ������ �ﰢ���� ���Ͽ�,
	int NumObj = (int)SltFaceList.size();
	for (int i = 0; i < NumObj; ++i)
	{
		// �ﰢ���� ������ �ε����� ���ϰ�,
		GFace *pFace = (GFace *)(SltFaceList[i]);
		int vidx0 = pFace->vIdx[0];
		int vidx1 = pFace->vIdx[1];
		int vidx2 = pFace->vIdx[2];

		// ������ �����ϰ�,
		GVertex v0 = pParentMesh->Verts[vidx0];
		GVertex v1 = pParentMesh->Verts[vidx1];
		GVertex v2 = pParentMesh->Verts[vidx2];

		// �� �����̸� �߰��ϰ�,
		if (VertIdxMap[vidx0] == 0)
		{
			pSubMesh->AddVertex(v0);
			VertIdxMap[vidx0] = pSubMesh->GetNumVert();
		}

		// �� �����̸� �߰��ϰ�,
		if (VertIdxMap[vidx1] == 0)
		{
			pSubMesh->AddVertex(v1);
			VertIdxMap[vidx1] = pSubMesh->GetNumVert();
		}

		// �� �����̸� �߰��ϰ�,
		if (VertIdxMap[vidx2] == 0)
		{
			pSubMesh->AddVertex(v2);
			VertIdxMap[vidx2] = pSubMesh->GetNumVert();
		}

		// �� ������ ������ �ε����� ���Ͽ�,
		vidx0 = VertIdxMap[vidx0] - 1;
		vidx1 = VertIdxMap[vidx1] - 1;
		vidx2 = VertIdxMap[vidx2] - 1;

		// �ﰢ���� �����Ͽ� �߰��Ѵ�.
		GFace f(vidx0, vidx1, vidx2);
		pSubMesh->AddFace(f);
	}

	// ��� ���ڿ� ������ ����ϰ�,
	pSubMesh->UpdateBndBox(false);
	pSubMesh->UpdateNormal(false);
	//pSubMesh->Edit(GVector3(), GQuater(), GVector3(1.0, 1.0, 1.0), pParentMesh);

	// ������ �ε��� ���� ���踦 �����ϰ�,
	std::map<int, int>::iterator it;
	for (it = VertIdxMap.begin(); it != VertIdxMap.end(); ++it)
		it->second--;		

	// ������ �κ� �޽��� ��ȯ�Ѵ�.
	return pSubMesh;	
}

/*!
*	\brief 2���� ������ �簢������ �Ű�ȭ�� �޽��� �����Ѵ�.
*
*	\param Name �Ű�ȭ �޽��� �̸�
*	\param pMesh �Ű�ȭ�� �޽�
*	\param StartVertIdx �Ķ���� ��(0.5, 0.0)�� ������ ���� ������ �ε���
*
*	\return �Ű�ȭ�� �޽��� ��ȯ�Ѵ�.
*/
static GMesh *create_mesh_unfolded_square(std::string Name, GMesh *pMesh, int StartVertIdx)
{
	// �Ű�ȭ�� 2���� ��� �޽��� �����ϰ�,
	GMesh *pRetMesh = new GMesh(Name, GTransf(), pMesh->FaceType, "");

	// ��� �޽��� �ʱ�ȭ �Ѵ�.
	pRetMesh->Verts = pMesh->Verts;
	pRetMesh->Faces = pMesh->Faces;

	// �޽����� ���� ����Ʈ�� ���ϰ�,
	std::vector<GVertex> &Verts = pMesh->Verts;
	std::vector<GVertex> &RetVerts = pRetMesh->Verts;

	////////////////////////////////////////////////////////////////////////
	// 1 �ܰ�: ���� �ý��� Ax = b�� �����ϱ� ���� ��� ��� A�� ����Ѵ�. //
	////////////////////////////////////////////////////////////////////////

	// ���� ���� ����Ʈ�� ������ ���ϰ�,
	std::vector<int> IntrVertList;
	::get_intr_verts(pMesh, IntrVertList);
	
	// ���� ������ �ѷ��� �̿� ������ ����Ʈ�� ���ϰ�,
	std::vector<std::vector<int>> AdjVertList;
	::get_neighbor_verts_of_vert(pMesh, AdjVertList);

	// ������ ���� ������ ���Ͽ� ����� �������� ����ġ�� ����Ѵ�. (Barycentric ��ǥ�� �̿��Ͽ�)
	std::map<std::string, float> EdgeWeightMap;
	int NumIntVert = (int)IntrVertList.size();
	for (int i = 0; i < NumIntVert; ++i)
	{		
		// ���� ������ �ѷ��� �̿� ������ ������ ���ϰ�,
		int vidx0 = IntrVertList[i];
		int NumVert = (int)AdjVertList[vidx0].size();

		// ������ �̿� ��������,
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

			// ���� ������ ����� ����ġ�� ����ϰ�,
			float weight = (theta1 + theta2) / len;
			tot_weight = tot_weight + weight;

			// ����ġ �ʿ� ����� ��,
			std::string key = cast_str(vidx0) + "-" + cast_str(vidx2);
			EdgeWeightMap[key] = weight;
		}

		// ����ġ ���� ����ȭ �Ѵ�.
		for (int j = 0; j < NumVert; ++j)
		{
			int vidx2 = AdjVertList[vidx0][j];
			std::string key = cast_str(vidx0) + "-" + cast_str(vidx2);
			EdgeWeightMap[key] = EdgeWeightMap[key] / tot_weight;
		}
	}

	// ����ġ ���� �����Ͽ�, �����ý��� (Ax1 = b1, Ax2 = b2)�� ��� ���(A)�� ���Ѵ�.
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
	// 2 �ܰ�: ���� �ý��� Ax = b�� �����ϱ� ���� ��� ���� b�� ����Ѵ�. //
	////////////////////////////////////////////////////////////////////////

	// ��� ���� ����Ʈ�� ���ϰ�,
	std::vector<std::vector<int>> BoundaryVertList;
	::get_bndry_verts(pMesh, BoundaryVertList);

	// �Ķ����(0.5, 0.0)�� ���� ������ �־����ٸ�, 
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
			// ��� ������ ����Ʈ�� �������Ѵ�.
			for (int i = vidx; i < NumBoundVert; ++i)
				tmpVertList.push_back(BoundaryVertList[0][i]);

			for (int i = 0; i < vidx; ++i)
				tmpVertList.push_back(BoundaryVertList[0][i]);

			BoundaryVertList[0] = tmpVertList;
		}		
	}

	// �� �ѷ��� ���̸� ����ϰ�,
	double BoundLen = 0.0;
	for (int i = 0; i < NumBoundVert; ++i)
	{
		int vidx0 = (i == 0) ? BoundaryVertList[0][NumBoundVert - 1] : BoundaryVertList[0][i - 1];
		int vidx1 = BoundaryVertList[0][i];
		BoundLen = BoundLen + dist(Verts[vidx0].P, Verts[vidx1].P);
	}

	// �ѷ��� �簢���� ������ų �Ѻ��� ���̸� ���Ѵ�.
	double Delta = BoundLen / 4.0;

	// ����� �������� (0.5, 0.0)�� �Ķ���� ���� �Ҵ��ϰ�,
	RetVerts[BoundaryVertList[0][0]].P.Set(0.5, 0.0, 0.0);
	BoundLen = Delta / 2.0;

	// �ѷ��� ���̿� �簢�� �Ѻ��� ���̸� �̿��Ͽ� ������ ��� ������ �Ķ���� ���� ����Ѵ�.
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

	// �����ý����� ��� ����(b1, b2)�� ���Ѵ�.
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
	// 3 �ܰ�: ���� �ý��� Ax = b�� �ظ� ���Ѵ�.//
	//////////////////////////////////////////////
	// �����ý����� ��(x1, x2)�� ���Ѵ�.
	Eigen::VectorXf x1(NumIntVert), x2(NumIntVert);
	Eigen::SparseLU<Eigen::SparseMatrix<float>> solver;
	solver.compute(A);

	x1 = solver.solve(b1);
	x2 = solver.solve(b2);

	// ���� ������ �Ķ���� ���� �����Ѵ�.
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
*	\brief 2���� ������ ���� ���·� �Ű�ȭ�� �޽��� �����Ѵ�.
*
*	\param Name �Ű�ȭ �޽��� �̸�
*	\param pMesh �Ű�ȭ�� �޽�
*	\param StartVertIdx �Ķ���� ��(1.0, 0.0)�� ������ ���� ������ �ε���
*
*	\return �Ű�ȭ�� �޽��� ��ȯ�Ѵ�.
*/
GMesh *create_mesh_unfolded_circle(std::string Name, GMesh *pMesh, int StartVertIdx)
{
	// �Ű�ȭ�� 2���� ��� �޽��� �����ϰ�,
	GMesh *pRetMesh = new GMesh(Name, GTransf(), pMesh->FaceType, "");

	// ��� �޽��� �ʱ�ȭ �Ѵ�.
	pRetMesh->Verts = pMesh->Verts;
	pRetMesh->Faces = pMesh->Faces;
	
	// �޽����� ���� ����Ʈ�� ���ϰ�,
	std::vector<GVertex> &Verts = pMesh->Verts;
	std::vector<GVertex> &RetVerts = pRetMesh->Verts;

	////////////////////////////////////////////////////////////////////////
	// 1 �ܰ�: ���� �ý��� Ax = b�� �����ϱ� ���� ��� ��� A�� ����Ѵ�. //
	////////////////////////////////////////////////////////////////////////

	// ���� ���� ����Ʈ�� ���ϰ�,
	std::vector<int> IntrVertList;
	::get_intr_verts(pMesh, IntrVertList);
	
	// ���� ������ �ѷ��� �̿� ������ ����Ʈ�� ���ϰ�,
	std::vector<std::vector<int>> AdjVertList;
	::get_neighbor_verts_of_vert(pMesh, AdjVertList);

	// ������ ���� ������ ���Ͽ� ����� �������� ����ġ�� ����Ѵ�. (Barycentric ��ǥ�� �̿��Ͽ�)
	std::map<std::string, float> EdgeWeightMap;
	int NumIntVert = (int)IntrVertList.size();
	for (int i = 0; i < NumIntVert; ++i)
	{		
		// ���� ������ �ѷ��� �̿� ������ ������ ���ϰ�,
		int vidx0 = IntrVertList[i];
		int NumVert = (int)AdjVertList[vidx0].size();

		// ������ �̿� ��������,
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

			// ���� ������ ����� ����ġ�� ����ϰ�,
			float weight = (theta1 + theta2) / dist(Verts[vidx0].P, Verts[vidx2].P);
			tot_weight = tot_weight + weight;

			// ����ġ �ʿ� ����� ��,
			std::string key = cast_str(vidx0) + "-" + cast_str(vidx2);
			EdgeWeightMap[key] = weight;
		}

		// ����ġ ���� ����ȭ �Ѵ�.
		for (int j = 0; j < NumVert; ++j)
		{
			int vidx2 = AdjVertList[vidx0][j];
			std::string key = cast_str(vidx0) + "-" + cast_str(vidx2);
			EdgeWeightMap[key] = EdgeWeightMap[key] / tot_weight;
		}
	}

	// ����ġ ���� �����Ͽ�, �����ý��� (Ax1 = b1, Ax2 = b2)�� ��� ���(A)�� ���Ѵ�.
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
	// 2 �ܰ�: ���� �ý��� Ax = b�� �����ϱ� ���� ��� ���� b�� ����Ѵ�. //
	////////////////////////////////////////////////////////////////////////

	// ��� ���� ����Ʈ�� ���ϰ�,
	std::vector<std::vector<int>> BndryVertList;
	::get_bndry_verts(pMesh, BndryVertList);

	// �Ķ������ ���� ������ �־����ٸ�, 
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
			// ��� ������ ����Ʈ�� �������Ѵ�.
			for (int i = vidx; i < NumBndryVert; ++i)
				tmpVertList.push_back(BndryVertList[0][i]);

			for (int i = 0; i < vidx; ++i)
				tmpVertList.push_back(BndryVertList[0][i]);

			BndryVertList[0] = tmpVertList;
		}		
	}

	// �� �ѷ��� ���̸� ����ϰ�,
	double BndryLen = 0.0;
	for (int i = 0; i < NumBndryVert; ++i)
		BndryLen = BndryLen + dist(Verts[BndryVertList[0][i]].P, Verts[BndryVertList[0][(i + 1) % NumBndryVert]].P);

	// ��� ������ ��ǥ�� �����Ѵ�.
	double CurrLen = 0.0;
	RetVerts[BndryVertList[0][0]].P.Set(1.0, 0.0, 0.0);
	for (int i = 1; i < NumBndryVert; ++i)
	{
		CurrLen = CurrLen + dist(Verts[BndryVertList[0][i]].P, Verts[BndryVertList[0][i - 1]].P);
		double theta = 2.0 * M_PI * (CurrLen / BndryLen);
		RetVerts[BndryVertList[0][i]].P.Set(cos(theta), sin(theta), 0.0);
	}

	// �����ý����� ��� ����(b1, b2)�� ���Ѵ�.
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
	// 3 �ܰ�: ���� �ý��� Ax = b�� �ظ� ���Ѵ�.//
	//////////////////////////////////////////////
	// �����ý����� ��(x1, x2)�� ���Ѵ�.
	Eigen::VectorXf x1(NumIntVert), x2(NumIntVert);
	Eigen::SparseLU<Eigen::SparseMatrix<float>> solver;
	solver.compute(A);

	x1 = solver.solve(b1);
	x2 = solver.solve(b2);

	// ���� ������ �Ķ���� ���� �����Ѵ�.
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
*	\brief 2���� ������ �Ű�ȭ�� �޽��� �����Ѵ�.
*
*	\param Name �Ű�ȭ �޽��� �̸�
*	\param pMesh �Ű�ȭ�� �޽�
*	\param StartVertIdx �Ķ���� ��(0.5, 0.0)�� ������ ���� ������ �ε���
*
*	\return �Ű�ȭ�� �޽��� ��ȯ�Ѵ�.
*/
GMesh *create_mesh_unfolded(std::string Name, GMesh *pMesh, int StartVertIdx, int BndryType)
{
	GMesh *pRetMesh;

	// �簢�� ���� �Ű�ȭ
	if (BndryType == 0) 
	{
		pRetMesh = create_mesh_unfolded_square(Name, pMesh, StartVertIdx);
	}

	// ��/Ÿ�� ���� �Ű�ȭ
	if (BndryType == 1)	
	{
		pRetMesh = create_mesh_unfolded_circle(Name, pMesh, StartVertIdx);
	}

	return pRetMesh;
}

/*!
*	\brief ��� �������κ��� �޽��� �����Ѵ�.
*
*	\param Name ������ �޽��� �̸�
*	\param BndryVerts ��� ������ ����Ʈ
*	\param FillType ���θ� ä��� ���� (0: �Ϲ��� ���, 1: ������ ���(�� ���·� ä����))
*
*	\return ��� ������ ä��� �޽��� �����Ͽ� ��ȯ�Ѵ�.
*/
GMesh *create_mesh_from_bndry_verts(std::string Name, std::vector<GVertex> &BndryVerts, int FillType)
{
	// ���ο� Ȧ �޽��� �����ϰ�,
	GMesh *pHoleMesh = new GMesh(Name, GTransf(), FACE_TRI);

	// ��� �������� �޽��� ���� ����Ʈ�� �����Ѵ�.
	if (FillType == 0)
	{
		int NumVert = (int)BndryVerts.size();
		for (int i = 0; i < NumVert; ++i)
			pHoleMesh->AddVertex(BndryVerts[i]);

		while(true)
		{
			// ���� ��� ������ ������ ���ϰ�,
			NumVert = (int)BndryVerts.size();

			// ��� ������ ������ 3�����,
			if (NumVert == 3)
			{
				// ���� ��� ������� �ﰢ���� �����,
				int vidx0 = pHoleMesh->GetVertIdx(&BndryVerts[0]);
				int vidx1 = pHoleMesh->GetVertIdx(&BndryVerts[1]);
				int vidx2 = pHoleMesh->GetVertIdx(&BndryVerts[2]);
				GFace f(vidx0, vidx1, vidx2);

				// �޽��� �߰��Ѵ�.
				pHoleMesh->AddFace(f);
				pHoleMesh->ModifiedFlag = MODIFIED_VERT_NUM;
				pHoleMesh->UpdateMesh();
				break;
			}

			// �̿��� �𼭸��� ���̰��� ���ϱ� ���Ͽ�,
			std::vector<double> Angles;
			for (int i = 0; i < NumVert; ++i)
			{
				int vidx0 = (i == 0) ? (NumVert - 1) : (i - 1);
				int vidx1 = i;
				int vidx2 = (i == NumVert - 1) ? 0 : (i + 1);

				// �̿��� �������� ã��,
				GVertex v0 = BndryVerts[vidx0];
				GVertex v1 = BndryVerts[vidx1];
				GVertex v2 = BndryVerts[vidx2];

				// �̿��� �𼭸��� ���̰��� ����Ͽ� ����Ʈ�� �߰��Ѵ�.
				GVector3 e0 = v0.P - v1.P;
				GVector3 e1 = v2.P - v1.P;
				double theta = angle(e1, e0, v1.N, false);
				Angles.push_back(theta);
			}

			// ���� ���� ���̰��� ���� ������ ã�´�.
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
			// ���� ���� ���̰��� ���� ������ �ε����� ������� �ε�������Ʈ�� ó���̶��,
			if (min_it == BndryVerts.begin())
			{
				// ������ �� ������ �ε����� ���Ѵ�.
				v0 = *(BndryVerts.end() - 1);
				v1 = *(min_it);
				v2 = *(min_it + 1);
			}
			else if (min_it == BndryVerts.end() - 1) // ���� ���� ���̰��� ���� ������ �ε����� ������� �ε�������Ʈ�� �������̶��,
			{
				// ������ �� ������ �ε����� ���Ѵ�.
				v0 = *(min_it - 1);
				v1 = *(min_it);
				v2 = *(BndryVerts.begin());
			}
			else	// ���� ���� ���̰��� ���� ������ �ε����� ������� �ε�������Ʈ�� �߰��̶��
			{
				// ������ �� ������ �ε����� ���Ѵ�.
				v0 = *(min_it - 1);
				v1 = *(min_it);
				v2 = *(min_it + 1);
			}

			// �̿��� ��� ������ �ε����� ���ϰ�,
			int vidx0 = pHoleMesh->GetVertIdx(&v0);
			int vidx1 = pHoleMesh->GetVertIdx(&v1);
			int vidx2 = pHoleMesh->GetVertIdx(&v2);

			// ���ο� �ﰢ���� �����, �޽��� �߰��ϰ�,
			GFace f(vidx0, vidx1, vidx2);
			pHoleMesh->AddFace(f);
			pHoleMesh->UpdateNormal(false);

			// ���� ���� ���̰��� ���� ������ ������� ����Ʈ���� �����Ѵ�.
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

			// ���ο� �ﰢ���� �����, �޽��� �߰��ϰ�,
			GFace f(vidx0, vidx1, vidx2);
			pHoleMesh->AddFace(f);			
		}
		pHoleMesh->ModifiedFlag = MODIFIED_VERT_NUM;
		pHoleMesh->UpdateMesh();
	}


	// ������ �޽��� ��ȯ�Ѵ�.
	return pHoleMesh;
}

/*!
*	\brief ���� ������κ��� �κ� �޽��� �����ϴ� �Լ�
*
*	\param pSrf ���� ���
*	\param ContParam ���� ����� ���� �ܸ��� V ���� �Ķ����
*	\param AuxParam ���� ����� ���� �ܸ��� V ���� �Ķ����
*	\param snum_u U-���� ���ø� ��
*	\param snum_v V-���� ���ø� ��
*
*	\return ������ �޽��� ��ȯ�Ѵ�.
*/
GMesh *create_mesh_from_sweep_srf(GSweepSrf *pSrf, double ContParam, double AuxParam, int snum_u, int snum_v)
{
	// �޽� ��ü�� �����Ѵ�.
	GMesh *pMesh = new GMesh("", GTransf(), FACE_QUAD, "");
	pMesh->Renderer.RenderType |= RENDER_WIRE;

	// ���� ����� ���ǿ��� ���ϰ� �˻��Ѵ�.
	double umin, umax, vmin, vmax;
	pSrf->GetDomain(&umin, &umax, &vmin, &vmax);
	if (AuxParam < vmin || ContParam > vmax)
	{
		std::cout << "Subdomian error" << std::endl;
		return NULL;
	}

	// ������ ����Ʈ�� �����Ѵ�.
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

			pMesh->AddFace(f0);
			pMesh->AddFace(f1);
		}
	}

	// ������ �޽��� ��ȯ�Ѵ�.
	return pMesh;
}

/*!
*	\brief �� �޽��� �浹 üũ�� �ǽ��Ѵ�.
*
*	\param pMesh1 ù ��° �޽�
*	\param pMesh2 �� ��° �޽�
*
*	\return �浹�� �ﰢ�� ���� ������ ��ȯ�Ѵ�.
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
*	\brief �浹 üũ�� �ǽ��Ѵ�.
*
*	\param MC1 pBVH1�� ��ȯ ���
*	\param pBVH1 �κ� �޽��� ���� BVH
*	\param pMesh2 �� ��° �޽�
*
*	\return �浹�� �ﰢ�� ���� ������ ��ȯ�Ѵ�.
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
*	\brief �� ���� ���󿡼� ������ ������ ��ȯ�Ѵ�.
*
*	\param t ���� �Ķ����
*	\param C1 ù ��° Ű����
*	\param C2 �� ��° Ű����
*	\param C3 �� ���� Ű����
*
*	\return ������ ������ ��ȯ�Ѵ�.
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
*	\brief ������ ���󿡼� ������ ���� ��´�.
*
*	\param t ���� �Ķ����: 0(����) <= t <= 1.0(����)
*
*	\return ������ �߰� ������ ��ȯ�Ѵ�.
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
*	\brief ������ ������ �����Ͽ� ��ȯ�ϴ� �Լ�
*
*	\return ������ ������ �����Ͽ� ��ȯ�Ѵ�.
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
*	\brief ����� ���� ������ �����Ͽ� ��ȯ�ϴ� �Լ�
*
*	\param diff_r ���ݻ��� R ����
*	\param diff_g ���ݻ��� G ����
*	\param diff_b ���ݻ��� B ����
*	\param ambi_r �ֺ����� R ����
*	\param ambi_g �ֺ����� G ����
*	\param ambi_b �ֺ����� B ����
*	\param spec_r ���ݻ��� R ����
*	\param spec_g ���ݻ��� G ����
*	\param spec_b ���ݻ��� B ����
*
*	\return ������ ������ �����Ͽ� ��ȯ�Ѵ�.
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
*	\brief �޽��� ���콺 ���(kG), ��� ���(kH), �ּ� ���(kMin) �׸��� �ִ� ���(kMax)�� ����ϴ� �Լ�
*	\note Meyer et al. "Discrete Differential-Geometry Operators for Triangulated 2-Manifolds" �� ����
*
*	\param pMesh ��� �޽�
*	\param kG ���콺 ����� ����ȴ�.
*	\param kH ��� ����� ����ȴ�.
*	\param kMin �ּ� ����� ����ȴ�.
*	\param kMax �ִ� ����� ����ȴ�.
*/
void get_curvature(GMesh *pMesh, std::vector<double> &kG, std::vector<double> &kH, std::vector<double> &kMin, std::vector<double> &kMax)
{
	// ������ ������ ���ϰ�, ��� ����Ʈ�� ���� �ʱ�ȭ �Ѵ�.
	int NumVert = pMesh->GetNumVert();
	kG.clear();
	kH.clear();
	kMin.clear();
	kMax.clear();
	kG.assign(NumVert, 2.0 * M_PI);
	kH.assign(NumVert, 0.0);
	kMin.assign(NumVert, 0.0);
	kMax.assign(NumVert, 0.0);

	// Mean ��� ���� ���͸� �����ϰ� �ʱ�ȭ �Ѵ�.
	std::vector<GVector3> Normals;
	Normals.assign(NumVert, GVector3());

	// ���� �ֺ��� ���� ���̸� ������ ������ �����Ѵ�.
	std::vector<double> Area;
	Area.assign(NumVert, 0.0);

	// ������ �ﰢ���� ���Ͽ�,
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		// ������ �ε����� ��ǥ�� ���ϰ�,
		int vidx0 = pMesh->Faces[i].vIdx[0];
		int vidx1 = pMesh->Faces[i].vIdx[1];
		int vidx2 = pMesh->Faces[i].vIdx[2];
		GPoint3 P = pMesh->Verts[vidx0].P;
		GPoint3 Q = pMesh->Verts[vidx1].P;
		GPoint3 R = pMesh->Verts[vidx2].P;

		// �� �������� ������ �� �𼭸��� ���̰��� ���ϰ�,
		double AngP = angle(Q - P, R - P, true);
		double AngQ = angle(R - Q, P - Q, true);
		double AngR = angle(P - R, Q - R, true);

		// �� ������ ��� ��� ���� ���͸� �����ϰ�,
		Normals[vidx0] += ((Q - P) / tan(AngR) + (R - P) / tan(AngQ));
		Normals[vidx1] += ((R - Q) / tan(AngP) + (P - Q) / tan(AngR));
		Normals[vidx2] += ((P - R) / tan(AngQ) + (Q - R) / tan(AngP));

		// �� ������ ���� ���̸� ����Ͽ�,
		double AreaP = (dist_sq(P, Q) / tan(AngR) + dist_sq(P, R) / tan(AngQ)) / 8.0;
		double AreaQ = (dist_sq(Q, R) / tan(AngP) + dist_sq(Q, P) / tan(AngR)) / 8.0;
		double AreaR = (dist_sq(R, P) / tan(AngQ) + dist_sq(R, Q) / tan(AngP)) / 8.0;
		double AreaTot = AreaP + AreaQ + AreaR;

		if (AngP > M_PI_2)		// �� P�� �а��� �ﰢ���̶��,
		{
			// �� ������ ���� ���̿� �����Ѵ�.
			Area[vidx0] += AreaTot * 0.5;
			Area[vidx1] += AreaTot * 0.25;
			Area[vidx2] += AreaTot * 0.25;
		}
		else if (AngQ > M_PI_2)	// �� Q�� �а��� �ﰢ���̶��,
		{
			// �� ������ ���� ���̿� �����Ѵ�.
			Area[vidx0] += AreaTot * 0.25;
			Area[vidx1] += AreaTot * 0.5;
			Area[vidx2] += AreaTot * 0.25;
		}
		else if (AngR > M_PI_2)	// �� R�� �а��� �ﰢ���̶��,
		{
			// �� ������ ���� ���̿� �����Ѵ�.
			Area[vidx0] += AreaTot * 0.25;
			Area[vidx1] += AreaTot * 0.25;
			Area[vidx2] += AreaTot * 0.5;
		}
		else					// ���� �ﰢ���̶��,
		{
			// �� ������ ���� ���̿� �����Ѵ�.
			Area[vidx0] += AreaP;
			Area[vidx1] += AreaQ;
			Area[vidx2] += AreaR;
		}

		// ���콺 ����� ����Ѵ�.
		kG[vidx0] -= AngP;
		kG[vidx1] -= AngQ;
		kG[vidx2] -= AngR;
	}

	// ������ ������ ���Ͽ�,
	for (int i = 0; i < NumVert; ++i)
	{
		// ���콺 ����� ��� ����� ���� ����ϰ�,
		kG[i] = kG[i] / Area[i];
		kH[i] = 0.5 * norm(Normals[i] / (2 * Area[i]));

		// �ּ�, �ִ� ����� ����Ѵ�.
		double delta = kH[i] * kH[i] - kG[i];
		if (delta < 0.0)
			delta = 0.0;
		kMin[i] = kH[i] - SQRT(delta);
		kMax[i] = kH[i] + SQRT(delta);
	}
}

/*!
*	\brief �޽��� �� ������ Gaussian ����� ����Ͽ� ��ȯ�ϴ� �Լ�
*
*	\param pMesh ��� �޽�
*	\param Curvatures �޽� ������ ����� ����ȴ�. (��� ������ ���, 0�� ����ȴ�)
*	\param MinK �ּ� ����� ����ȴ�.
*	\param MaxK �ִ� ����� ����ȴ�.
*/
void get_gaussian_curvature(GMesh *pMesh, std::vector<double> &Curvatures, double &MinK, double &MaxK)
{
	// ������ ������ ���ϰ�, ��� ����Ʈ�� ���� �ʱ�ȭ �Ѵ�.
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

		if (AngP > M_PI_2)	// �� P�� �а��� �ﰢ���̶��,
		{
			Area[vidx0] += AreaTot * 0.5;
			Area[vidx1] += AreaTot * 0.25;
			Area[vidx2] += AreaTot * 0.25;
		}
		else if (AngQ > M_PI_2)	// �� Q�� �а��� �ﰢ���̶��,
		{
			Area[vidx0] += AreaTot * 0.25;
			Area[vidx1] += AreaTot * 0.5;
			Area[vidx2] += AreaTot * 0.25;
		}
		else if (AngR > M_PI_2)	// �� R�� �а��� �ﰢ���̶��,
		{
			Area[vidx0] += AreaTot * 0.25;
			Area[vidx1] += AreaTot * 0.25;
			Area[vidx2] += AreaTot * 0.5;
		}
		else		// ���� �ﰢ���̶��,
		{
			Area[vidx0] += AreaP;
			Area[vidx1] += AreaQ;
			Area[vidx2] += AreaR;
		}

		Curvatures[vidx0] -= AngP;
		Curvatures[vidx1] -= AngQ;
		Curvatures[vidx2] -= AngR;
	}

	// �޽��� ��� ������ ������ ���ϰ�,
	std::vector<bool> InfoList;
	::get_bndry_vert_info(pMesh, InfoList);
	MinK = 1000000.0;
	MaxK = -1000000.0;

	// �޽��� ������ ������ ���Ͽ�,
	for (int i = 0; i < NumVert; ++i)
	{
		// ��� ������ �ƴ϶��,
		if (InfoList[i] == false)
		{
			// ���콺 ����� ���ϰ�, �ִ�, �ּ� ���� �����Ѵ�.
			Curvatures[i] /= Area[i];
			MinK = MIN(MinK, Curvatures[i]);
			MaxK = MAX(MaxK, Curvatures[i]);
		}
	}

	// ��� ������ Gaussian ����� �ּҰ����� �����Ѵ�.
	for (int i = 0; i < NumVert; ++i)
		Curvatures[i] = (InfoList[i] == true) ? MinK : Curvatures[i];		
}

/*!
*	\brief �޽��� �� ������ Mean ����� ����Ͽ� ��ȯ�ϴ� �Լ�
*
*	\param pMesh ��� �޽�
*	\param Curvatures �޽� ������ ����� ����ȴ�. (��� ������ ���, �ּҰ��� ����ȴ�)
*	\param MinK �ּ� ����� ����ȴ�.
*	\param MaxK �ִ� ����� ����ȴ�.
*/
void get_mean_curvature(GMesh *pMesh, std::vector<double> &Curvatures, double &MinK, double &MaxK)
{
	// ������ ������ ���ϰ�, ��� ����Ʈ�� ���� �ʱ�ȭ �Ѵ�.
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

		if (AngP > M_PI_2)	// �� P�� �а��� �ﰢ���̶��,
		{
			Area[vidx0] += AreaTot * 0.5;
			Area[vidx1] += AreaTot * 0.25;
			Area[vidx2] += AreaTot * 0.25;
		}
		else if (AngQ > M_PI_2)	// �� Q�� �а��� �ﰢ���̶��,
		{
			Area[vidx0] += AreaTot * 0.25;
			Area[vidx1] += AreaTot * 0.5;
			Area[vidx2] += AreaTot * 0.25;
		}
		else if (AngR > M_PI_2)	// �� R�� �а��� �ﰢ���̶��,
		{
			Area[vidx0] += AreaTot * 0.25;
			Area[vidx1] += AreaTot * 0.25;
			Area[vidx2] += AreaTot * 0.5;
		}
		else		// ���� �ﰢ���̶��,
		{
			Area[vidx0] += AreaP;
			Area[vidx1] += AreaQ;
			Area[vidx2] += AreaR;
		}
	}

	// �޽��� ��� ������ ������ ���ϰ�,
	std::vector<bool> InfoList;
	::get_bndry_vert_info(pMesh, InfoList);
	MinK = 1000000.0;
	MaxK = -1000000.0;

	// �޽��� ������ ������ ���Ͽ�,
	for (int i = 0; i < NumVert; ++i)
	{
		// ��� ������ �ƴ϶��,
		if (InfoList[i] == false)
		{
			// ��� ����� ���ϰ�, �ִ�, �ּ� ���� �����Ѵ�.
			Curvatures[i] = 0.5 * norm(Normals[i] / (2 * Area[i]));
			MinK = MIN(MinK, Curvatures[i]);
			MaxK = MAX(MaxK, Curvatures[i]);
		}
	}

	// ��� ������ ��� ����� �ּҰ����� �����Ѵ�.
	for (int i = 0; i < NumVert; ++i)
		Curvatures[i] = (InfoList[i] == true) ? MinK : Curvatures[i];	
}

/*!
*	\brief ��ü�� ǥ������ ����Ѵ�.
*
*	\pram pMesh ǥ������ ����� �޽�
*
*	\return �ﰢ���� ������ ������ ���� ��ü�� ǥ������ ��ȯ�Ѵ�.
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
*	\brief �޽��� ��źȭ �Ѵ�.
*
*	\pram pMesh ������ �޽�
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
*	\brief �޽��� ����ȭ ������ �����Ѵ�.
*	\note �ؽ�ó ��ǥ�� �׷��ε� �ﰢ�� ������ ���ŵ�
*
*	\pram pMesh ������ �޽�
*/
void edit_mesh_by_refine(GMesh *pMesh)
{
	// Quad �޽��� fan �޽��� ���̾������� ������ ������ ���� ���� ����
	if (pMesh->FaceType == FACE_QUAD || pMesh->FaceType == FACE_TRI_FAN)
		return;
	
	// �ʿ��� ������ �����Ѵ�.
	std::vector<GFace> NewFaces;
	std::map<std::string, int> vIdxMap;
	pMesh->GrpFaces.clear();
	
	// ������ �ﰢ���� ���Ͽ�,
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

		// ���� (vidx0, vidx1) �Ǵ� (vidx1, vidx0)�� ������ �߰����� �ʾҴٸ�,
		if (vIdxMap[edge01] == 0 && vIdxMap[edge10] == 0)
		{
			// ������ ������ ���Ͽ� ��������Ʈ�� �߰��ϰ�,
			GVertex v;
			v.P = cast_pt3((cast_vec3(pMesh->Verts[vidx0].P) + cast_vec3(pMesh->Verts[vidx1].P)) * 0.5);
			pMesh->AddVertex(v);

			// ������ ������ �ε����� ����Ѵ�.
			vIdxMap[edge01] = vIdxMap[edge10] = pMesh->GetNumVert() - 1;
		}		

		// ���� (vidx1, vidx2) �Ǵ� (vidx2, vidx1)�� ������ �߰����� �ʾҴٸ�,
		if (vIdxMap[edge12] == 0 && vIdxMap[edge21] == 0)
		{
			// ������ ������ ���Ͽ� ��������Ʈ�� �߰��ϰ�,
			GVertex v;
			v.P = cast_pt3((cast_vec3(pMesh->Verts[vidx1].P) + cast_vec3(pMesh->Verts[vidx2].P)) * 0.5);
			pMesh->AddVertex(v);

			// ������ ������ �ε����� ����Ѵ�.
			vIdxMap[edge12] = vIdxMap[edge21] = pMesh->GetNumVert() - 1;
		}

		// ���� (vidx2, vidx0) �Ǵ� (vidx0, vidx2)�� ������ �߰����� �ʾҴٸ�,
		if (vIdxMap[edge20] == 0 && vIdxMap[edge02] == 0)
		{
			// ������ ������ ���Ͽ� ��������Ʈ�� �߰��ϰ�,
			GVertex v;
			v.P = cast_pt3((cast_vec3(pMesh->Verts[vidx2].P) + cast_vec3(pMesh->Verts[vidx0].P)) * 0.5);
			pMesh->AddVertex(v);

			// ������ ������ �ε����� ����Ѵ�.
			vIdxMap[edge20] = vIdxMap[edge02] = pMesh->GetNumVert() - 1;
		}

		// ���� ������ �ε����� ���Ͽ�,
		int vidx3 = vIdxMap[edge01];
		int vidx4 = vIdxMap[edge12];
		int vidx5 = vIdxMap[edge20];

		// �ﰢ���� 4���� �κлﰢ������ �����Ͽ� �޽��� �߰��Ѵ�.
		GFace f0(vidx0, vidx3, vidx5, -1, -1, -1, mtl_idx); 
		GFace f1(vidx3, vidx1, vidx4, -1, -1, -1, mtl_idx); 
		GFace f2(vidx3, vidx4, vidx5, -1, -1, -1, mtl_idx); 
		GFace f3(vidx5, vidx4, vidx2, -1, -1, -1, mtl_idx); 
		NewFaces.push_back(f0);
		NewFaces.push_back(f1);
		NewFaces.push_back(f2);
		NewFaces.push_back(f3);
	}

	// ���ο� �ﰢ���� �����Ѵ�.
	pMesh->SetNewFaces(NewFaces);
	pMesh->ModifiedFlag = MODIFIED_VERT_NUM;
	pMesh->UpdateMesh();
}

/*!
*	\brief �޽��� �ߺ��� ������ ���� ��踦 ����.
*
*	\pram pMesh ������ �޽�
*	\param tol ������ �������� �����Ǵ� �Ÿ� ������
*/
void edit_mesh_by_stitching(GMesh *pMesh, double tol)
{
	// ������ ������ŭ �ε��� ���� �����ϰ�, -1�� �ʱ�ȭ�Ѵ�.
	int NumVert = pMesh->GetNumVert();
	std::vector<int> IdxMap;
	IdxMap.assign(NumVert, -1);

	// ������ ��������,
	for (int i = 0; i < NumVert; ++i)
	{
		// �̹� �ߺ��� �����̶�� ���� ������ �˻��ϰ�,
		if (IdxMap[i] != -1)
			continue;

		// ó�� ���� �����̶��, �ε����� �ο��Ѵ�.
		IdxMap[i] = i;

		// ���� ������ ��ġ�� ������ ���,
		GPoint3 &p = pMesh->Verts[i].P;
		GVector3 &pn = pMesh->Verts[i].N;

		// ������ ��� ������ ���Ͽ�, 
		for (int j = i + 1; j < NumVert; ++j)
		{
			// �̹� �ߺ��� �����̶��, ���� ������ �˻��ϰ�,
			if (IdxMap[j] != -1)
				continue;

			// �ƴ϶��, ������ ��ġ�� ���ؼ�,
			GPoint3 &q = pMesh->Verts[j].P;
			GVector3 &qn = pMesh->Verts[j].N;


			// ������ �������� ���� �ȴٸ�, �ε��� �ʿ� ǥ���Ѵ�.
			if (dist(p, q) < tol)
				IdxMap[j] = i;
		}
	}

	// ������ �ﰢ���� ���Ͽ�, �ߺ��� ������ �ε����� �����Ѵ�.
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		pMesh->Faces[i].vIdx[0] = IdxMap[pMesh->Faces[i].vIdx[0]];
		pMesh->Faces[i].vIdx[1] = IdxMap[pMesh->Faces[i].vIdx[1]];
		pMesh->Faces[i].vIdx[2] = IdxMap[pMesh->Faces[i].vIdx[2]];
	}

	// �ﰢ���� �������� �ʴ� �������� �����Ѵ�.
	edit_mesh_by_removing_unused_verts(pMesh);
}

/*! 
*	\brief �޽����� ������ �ʴ� ������ �����ϴ� �Լ� 
*
*	\param pMesh ������ �޽�
*/
void edit_mesh_by_removing_unused_verts(GMesh *pMesh)
{
	/////////////////////////////////////
	// 1 �ܰ�: �̻�� ������ �����Ѵ�. //
	/////////////////////////////////////

	// ������ ��� ���θ� üũ�ϱ� ����, ������ ������ŭ flag�迭�� �����ϰ�, false�� �ʱ�ȭ�� ��, 
	int NumVert = pMesh->GetNumVert();
	std::vector<bool> UsedFlag;
	UsedFlag.assign(NumVert, false);

	// ���� ������ �÷��׸� true�� �����Ѵ�.
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		UsedFlag[pMesh->Faces[i].vIdx[0]] = true;
		UsedFlag[pMesh->Faces[i].vIdx[1]] = true;
		UsedFlag[pMesh->Faces[i].vIdx[2]] = true;
	}

	// ���� �������� ������ ���ο� ���� ����Ʈ�� �����Ѵ�.
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
	// 2�ܰ�: ���� �������� ���ο� �ﰢ�� ����Ʈ�� �����Ѵ�. //
	///////////////////////////////////////////////////////////
	std::vector<GFace> OldFaces = pMesh->Faces;	
	pMesh->Faces.clear();

	std::vector<GFace> NewFaces;
	for (int i = 0; i < NumFace; ++i)
	{
		int vidx0 = OldFaces[i].vIdx[0];
		int vidx1 = OldFaces[i].vIdx[1];
		int vidx2 = OldFaces[i].vIdx[2];

		// ���� ������ ����ϴ� �ﰢ���̶��, 
		if (VertIdxMap[vidx0] != -1 && VertIdxMap[vidx1] != -1 && VertIdxMap[vidx2] != -1)
		{
			OldFaces[i].vIdx[0] = VertIdxMap[vidx0];
			OldFaces[i].vIdx[1] = VertIdxMap[vidx1];
			OldFaces[i].vIdx[2] = VertIdxMap[vidx2];

			// ���ο� �ε����� ���� �ﰢ���� ����Ʈ�� �߰��Ѵ�.
			pMesh->AddFace(OldFaces[i]);
		}
	}

	////////////////////////////////////////////////////////////////
	// 3�ܰ�: ��� ������ ����Ʈ�� �����, �����ڸ� �����Ѵ�. //
	////////////////////////////////////////////////////////////////			
	pMesh->ModifiedFlag = MODIFIED_VERT_NUM;
	pMesh->UpdateMesh();
}

/*! 
*	\brief �޽��� ���� ������ ���� ���ͷ� �����ϴ� �Լ�
*
*	\param pTarMesh ������ ������ ������ �޽�
*	\param pSrcMesh ���������� ������ �޽� (NULL �ΰ��, pTarMesh�� ����)
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
*	\brief �޽��� ���� ������ ���õ� �� �޽��� ���� ���� ����ġ�� �����ϴ� �Լ�
*
*	\param pSkinMesh ������ ������ ������ �޽�
*	\param pBoneMesh ���õ� �� �޽�
*/
void set_vertex_color_from_skin_wgt(GMesh *pSkinMesh, GMesh *pBoneMesh)
{
	// �� �޽��� �̸��� ���,
	std::string BoneMeshName = pBoneMesh->Name;

	// ��Ų �޽��� ������ ������ ���Ͽ�,
	int NumVert = pSkinMesh->GetNumVert();

	// ������ ��������,
	for (int i = 0; i < NumVert; ++i)
	{
		// �ϴ� ������ ���������� �ʱ�ȭ �ϰ�,
		pSkinMesh->Verts[i].C.Set(0.0, 0.0, 0.0);

		// ���ε��� �� �޽��� ������ ���Ͽ�,
		int NumBindInfo = pSkinMesh->Verts[i].BoneInfoList.size();

		// ������ ����� ������ �� �޽��� ���Ͽ�,
		for (int j = 0; j < NumBindInfo; ++j)
		{
			// ����� �� �޽��� ��� ���޽��� ���ٸ�,
			if (pSkinMesh->Verts[i].BoneInfoList[j].BoneMeshName == BoneMeshName)
			{
				// ������ ������ ���ε� ����ġ�� �����Ѵ�.
				double w = pSkinMesh->Verts[i].BoneInfoList[j].Wgt;
				pSkinMesh->Verts[i].C.Set(w, w, w);
				break;
			}
		}
	}
}

/*! 
*	\brief �޽��� �� �������� �־��� �ִ� �ݰ� �̳��� �� ���������� �����Ÿ��� ����ϴ� �Լ� 
*
*	\param pMesh ��� �޽�
*	\param pVert ���õ� �޽� ���� ����
*	\param Distances pVert���� i��° ���������� �����Ÿ��� Distances[i]�� ����ȴ�.
*	\param MaxRadius �����Ÿ��� ����� �ִ� �ݰ�
*/
void get_geodesic_dist_field(GMesh *pMesh, GVertex *pVert, std::vector<double> &Distances, double MaxRadius)
{
	// �̸� ���� ���� ������ �����ϱ� ���� ���� ������ ����Ѵ�.
	static GMesh *pPrevMesh = NULL;
	static int PrevNumVert = 0;
	static std::vector<std::vector<int>> VertList;	

	// ��� �޽��� �ٲ�ų� ���������� ����Ǿ��ٸ�,
	int NumVert = pMesh->GetNumVert();
	if (pPrevMesh != pMesh || PrevNumVert != NumVert)
	{
		// ���Ӱ� ���� ������ ���Ѵ�.
		::get_neighbor_verts_of_vert(pMesh, VertList);
		pPrevMesh = pMesh;
		PrevNumVert = NumVert;
	}

	// �Ÿ����� �ʱ�ȭ �ϰ�,
	Distances.assign(NumVert, 1000000.0);

	// �ּ� ���� �����Ѵ�.
	GHeap MinHeap(&Distances);

	// ���õ� ������ �Ÿ��� 0���� �ʱ�ȭ �ϰ�, �ε����� ���� �߰��Ѵ�.
	Distances[pVert->Idx] = 0.0;
	MinHeap.push(pVert->Idx);

	// ó���� ������ ���� �ִٸ�,
	while (!MinHeap.empty())
	{
		// �ּ� �Ÿ��� ���� ������ �ε����� ���Ͽ�,
		int j = MinHeap.get_candidate();

		// �̿��� ��� �ﰢ���� ���Ͽ�,
		int valance = (int)VertList[j].size();
		for (int l = 0; l < valance; ++l)
		{
			// �ﰢ�� (vi, vj, vk)�� ���Ͽ�,
			int i = VertList[j][l];
			int k = (l == 0) ? VertList[j][valance - 1] : VertList[j][l - 1];

			// �� ������ ��ǥ�� ���ϰ�,
			GPoint3 Pi = pMesh->Verts[i].P;
			GPoint3 Pj = pMesh->Verts[j].P;
			GPoint3 Pk = pMesh->Verts[k].P;

			// ���簡�� ���� �� ������ �ּҰŸ� Uj, Uk�� ���ϰ�,
			double Ui = 0.0;
			double Uj = Distances[j];
			double Uk = Distances[k];

			// �ﰢ���� �� ���� ���̿� ������ ����ϰ�,
			double dij_sq = dist_sq(Pi, Pj);
			double dik_sq = dist_sq(Pi, Pk);
			double djk_sq = dist_sq(Pj, Pk);
			double dij = SQRT(dij_sq);
			double dik = SQRT(dik_sq);			
			double djk = SQRT(djk_sq);

			// ����, �ﰢ���� �����ȴٸ�,
			if (ABS(Uj - Uk) < djk && djk < Uj + Uk)
			{
				// 2���� ��鿡 ������ S'(x0, y0), Pi(x1, y1), Pj(0, 0), Pk(-djk, 0)�� ��ǥ�� ���ϰ�,
				double x0 = (SQR(Uk) - SQR(Uj) - djk_sq) / (2 * djk);
				double y0 = SQRT(SQR(Uj) - SQR(x0));				
				double x1 = (dik_sq - dij_sq - djk_sq) / (2 * djk);
				double y1 = -SQRT(dij_sq - SQR(x1));

				// S'�� Pi�� �Ÿ��� ����Ͽ�, Ui�� �Ÿ��� �����Ѵ�.
				double m = (y1 - y0) / (x1 - x0);
				double t = x0 - y0 / m;

				// �ٰ��� (S', Pk, Pi, Pj)�� �����̶��,
				if (t > -djk && t < 0) 
				{
					// S'(x0, y0)�� Pi(x1, y1)�� �Ÿ��� ����Ͽ�, Ui�� �Ÿ��� �����Ѵ�.
					Ui = SQRT(SQR(x1 - x0) + SQR(y1 - y0));
				}
				else	// �ٰ��� (S', Pk, Pi, Pj)�� �����̶��,
				{
					// Dijkstra �˰����� �����Ѵ�.
					Ui = (Uj + dij < Uk + dik) ? (Uj + dij) : (Uk + dik);					
				}
			}
			else	// �ﰢ���� �������� �ʴ� �����,
			{
				// Dijkstra �˰����� �����Ѵ�.
				Ui = (Uj + dij < Uk + dik) ? (Uj + dij) : (Uk + dik);				
			}			

			// ���� �Ÿ��� ������ ���� �Ÿ����� �۴ٸ�,
			if (Ui < Distances[i])
			{
				// �ִ� �Ÿ��� �����ϰ�, ���� ������ �ε����� �߰��Ѵ�.
				Distances[i] = Ui;
				if (Ui < MaxRadius)
					MinHeap.push(i);
			}
		}
	}	
}

/*! 
*	\brief ���õ� ����(�ﰢ��)�鿡�� �޽��� �� ���������� �����Ÿ��� ����ϴ� �Լ�
*
*	\param pMesh ��� �޽�
*	\param VertList ���õ� �޽� ���� ����(�ﰢ��) ����Ʈ
*	\param Distances pVert���� i��° ���������� �����Ÿ��� Distances[i]�� ����ȴ�.
*	\param MaxRadius �����Ÿ��� ����� �ִ� �ݰ�
*/
void get_geodesic_dist_field(GMesh *pMesh, std::vector<GEditObj *> &ObjList, std::vector<double> &Distances, double MaxRadius)
{
	// �����̳� �ﰢ���� ���õ� ��찡 �ƴ϶�� �����Ѵ�.
	std::string ClsName = ObjList[0]->GetClassName();
	if (ClsName != "GVertex" && ClsName != "GFace")
		return;

	// �̸� ���� ���� ������ �����ϱ� ���� ���� ������ ����Ѵ�.
	static GMesh *pPrevMesh = NULL;
	static int PrevNumVert = 0;
	static std::vector<std::vector<int>> VertList;	

	// ��� �޽��� �ٲ�ų� ���������� ����Ǿ��ٸ�,
	int NumVert = pMesh->GetNumVert();
	if (pPrevMesh != pMesh || PrevNumVert != NumVert)
	{
		// ���Ӱ� ���� ������ ���Ѵ�.
		::get_neighbor_verts_of_vert(pMesh, VertList);
		pPrevMesh = pMesh;
		PrevNumVert = NumVert;
	}

	// �Ÿ����� �ʱ�ȭ �ϰ�,
	Distances.assign(NumVert, 1000000.0);

	// �ּ� ���� �����Ѵ�.
	GHeap MinHeap(&Distances);
	int NumSltObj = (int)ObjList.size();

	// ���õ� ��ü�� �����̶��,
	if (ClsName == "GVertex")
	{
		// �� ������ ���Ͽ�,
		for (int i = 0; i < NumSltObj; ++i)
		{
			GVertex *v = (GVertex *)ObjList[i];
			// �Ÿ��� 0���� �ʱ�ȭ �ϰ�, ���� �߰��Ѵ�.
			Distances[v->Idx] = 0.0;
			MinHeap.push(v->Idx);
		}
	}

	// ���õ� ��ü�� �ﰢ���̶��,
	if (ClsName == "GFace")
	{
		// �� �ﰢ���� ���Ͽ�,
		for (int i = 0; i < NumSltObj; ++i)
		{
			GFace *f = (GFace *)ObjList[i];
			int *vidx = f->vIdx;

			// �� ������ �Ÿ��� 0���� �ʱ�ȭ �ϰ�, ���� �߰��Ѵ�.
			Distances[vidx[0]] = 0.0;
			Distances[vidx[1]] = 0.0;
			Distances[vidx[2]] = 0.0;
			MinHeap.push(vidx[0]);
			MinHeap.push(vidx[1]);
			MinHeap.push(vidx[2]);
		}
	}

	// ó���� ������ ���� �ִٸ�,
	while (!MinHeap.empty())
	{
		// �ּ� �Ÿ��� ���� ������ �ε����� ���Ͽ�,
		int j = MinHeap.get_candidate();

		// �̿��� ��� �ﰢ���� ���Ͽ�,
		int valance = (int)VertList[j].size();
		for (int l = 0; l < valance; ++l)
		{
			// �ﰢ�� (vi, vj, vk)�� ���Ͽ�,
			int i = VertList[j][l];
			int k = (l == 0) ? VertList[j][valance - 1] : VertList[j][l - 1];

			// �� ������ ��ǥ�� ���ϰ�,
			GPoint3 Pi = pMesh->Verts[i].P;
			GPoint3 Pj = pMesh->Verts[j].P;
			GPoint3 Pk = pMesh->Verts[k].P;

			// ���簡�� ���� �� ������ �ּҰŸ� Uj, Uk�� ���ϰ�,
			double Ui = 0.0;
			double Uj = Distances[j];
			double Uk = Distances[k];

			// �ﰢ���� �� ���� ���̿� ������ ����ϰ�,
			double dij_sq = dist_sq(Pi, Pj);
			double dik_sq = dist_sq(Pi, Pk);
			double djk_sq = dist_sq(Pj, Pk);
			double dij = SQRT(dij_sq);
			double dik = SQRT(dik_sq);			
			double djk = SQRT(djk_sq);

			// ����, �ﰢ���� �����ȴٸ�,
			if (ABS(Uj - Uk) < djk && djk < Uj + Uk)
			{
				// 2���� ��鿡 ������ S'(x0, y0), Pi(x1, y1), Pj(0, 0), Pk(-djk, 0)�� ��ǥ�� ���ϰ�,
				double x0 = (SQR(Uk) - SQR(Uj) - djk_sq) / (2 * djk);
				double y0 = SQRT(SQR(Uj) - SQR(x0));				
				double x1 = (dik_sq - dij_sq - djk_sq) / (2 * djk);
				double y1 = -SQRT(dij_sq - SQR(x1));

				// S'�� Pi�� �Ÿ��� ����Ͽ�, Ui�� �Ÿ��� �����Ѵ�.
				double m = (y1 - y0) / (x1 - x0);
				double t = x0 - y0 / m;

				// �ٰ��� (S', Pk, Pi, Pj)�� �����̶��,
				if (t > -djk && t < 0) 
				{
					// S'(x0, y0)�� Pi(x1, y1)�� �Ÿ��� ����Ͽ�, Ui�� �Ÿ��� �����Ѵ�.
					Ui = SQRT(SQR(x1 - x0) + SQR(y1 - y0));
				}
				else	// �ٰ��� (S', Pk, Pi, Pj)�� �����̶��,
				{
					// Dijkstra �˰����� �����Ѵ�.
					Ui = (Uj + dij < Uk + dik) ? (Uj + dij) : (Uk + dik);					
				}
			}
			else	// �ﰢ���� �������� �ʴ� �����,
			{
				// Dijkstra �˰����� �����Ѵ�.
				Ui = (Uj + dij < Uk + dik) ? (Uj + dij) : (Uk + dik);				
			}	

			// ���� �Ÿ��� ������ ���� �Ÿ����� �۴ٸ�,
			if (Ui < Distances[i])
			{
				// �ִ� �Ÿ��� �����ϰ�, ���� ������ �ε����� �߰��Ѵ�.
				Distances[i] = Ui;
				if (Ui < MaxRadius)
					MinHeap.push(i);
			}
		}
	}
}

/*! 
*	\brief �ﰢ�� ������ �� ������ �޽��� �� ���������� �����Ÿ��� ����ϴ� �Լ�
*
*	\param pMesh ��� �޽�
*	\param fidx ���õ� ������ ���Ե� �ﰢ���� �ε���
*	\param Coords  �ﰢ�� ���ο��� ���õ� ������ baricentric coordinates
*	\param Distances ���õ� �ﰢ�� ������ �������� i��° ���������� �����Ÿ��� Distances[i]�� ����ȴ�.
*	\param MaxRadius �����Ÿ��� ����� �ִ� �ݰ�
*/
void get_geodesic_dist_field(GMesh *pMesh, int fidx, GVector3 Coords, std::vector<double> &Distances, double MaxRadius)
{
	// �̸� ���� ���� ������ �����ϱ� ���� ���� ������ ����Ѵ�.
	static GMesh *pPrevMesh = NULL;
	static int PrevNumVert = 0;
	static std::vector<std::vector<int>> VertList;	

	// ��� �޽��� �ٲ�ų� ���������� ����Ǿ��ٸ�,
	int NumVert = pMesh->GetNumVert();
	if (pPrevMesh != pMesh || PrevNumVert != NumVert)
	{
		// ���Ӱ� ���� ������ ���Ѵ�.
		::get_neighbor_verts_of_vert(pMesh, VertList);
		pPrevMesh = pMesh;
		PrevNumVert = NumVert;
	}

	// �Ÿ����� �ʱ�ȭ �ϰ�,
	Distances.assign(NumVert, 1000000.0);

	// �ּ� ���� �����Ѵ�.
	GHeap MinHeap(&Distances);
	
	// �ﰢ�� ������ ������ ��ǥ�� ����Ѵ�.
	int *vidx = pMesh->Faces[fidx].vIdx;
	GPoint3 p0 = pMesh->Verts[vidx[0]].P;
	GPoint3 p1 = pMesh->Verts[vidx[1]].P;
	GPoint3 p2 = pMesh->Verts[vidx[2]].P;
	GPoint3 pt = p2 + (p0 - p2) * Coords[0] + (p1 - p2) * Coords[1];

	// ���õ� �������κ����� �Ÿ��� ����ϰ� �ε����� ���� �߰��Ѵ�.
	Distances[vidx[0]] = dist(pt, p0);
	Distances[vidx[1]] = dist(pt, p1);
	Distances[vidx[2]] = dist(pt, p2);
	MinHeap.push(vidx[0]);
	MinHeap.push(vidx[1]);
	MinHeap.push(vidx[2]);

	// ó���� ������ ���� �ִٸ�,
	while (!MinHeap.empty())
	{
		// �ּ� �Ÿ��� ���� ������ �ε����� ���Ͽ�,
		int j = MinHeap.get_candidate();

		// �̿��� ��� �ﰢ���� ���Ͽ�,
		int valance = (int)VertList[j].size();
		for (int l = 0; l < valance; ++l)
		{
			// �ﰢ�� (vi, vj, vk)�� ���Ͽ�,
			int i = VertList[j][l];
			int k = (l == 0) ? VertList[j][valance - 1] : VertList[j][l - 1];

			// �� ������ ��ǥ�� ���ϰ�,
			GPoint3 Pi = pMesh->Verts[i].P;
			GPoint3 Pj = pMesh->Verts[j].P;
			GPoint3 Pk = pMesh->Verts[k].P;

			// ���簡�� ���� �� ������ �ּҰŸ� Uj, Uk�� ���ϰ�,
			double Ui = 0.0;
			double Uj = Distances[j];
			double Uk = Distances[k];

			// �ﰢ���� �� ���� ���̿� ������ ����ϰ�,
			double dij_sq = dist_sq(Pi, Pj);
			double dik_sq = dist_sq(Pi, Pk);
			double djk_sq = dist_sq(Pj, Pk);
			double dij = SQRT(dij_sq);
			double dik = SQRT(dik_sq);			
			double djk = SQRT(djk_sq);

			// ����, �ﰢ���� �����ȴٸ�,
			if (ABS(Uj - Uk) < djk && djk < Uj + Uk)
			{
				// 2���� ��鿡 ������ S'(x0, y0), Pi(x1, y1), Pj(0, 0), Pk(-djk, 0)�� ��ǥ�� ���ϰ�,
				double x0 = (SQR(Uk) - SQR(Uj) - djk_sq) / (2 * djk);
				double y0 = SQRT(SQR(Uj) - SQR(x0));				
				double x1 = (dik_sq - dij_sq - djk_sq) / (2 * djk);
				double y1 = -SQRT(dij_sq - SQR(x1));

				// S'�� Pi�� �Ÿ��� ����Ͽ�, Ui�� �Ÿ��� �����Ѵ�.
				double m = (y1 - y0) / (x1 - x0);
				double t = x0 - y0 / m;

				// �ٰ��� (S', Pk, Pi, Pj)�� �����̶��,
				if (t > -djk && t < 0) 
				{
					// S'(x0, y0)�� Pi(x1, y1)�� �Ÿ��� ����Ͽ�, Ui�� �Ÿ��� �����Ѵ�.
					Ui = SQRT(SQR(x1 - x0) + SQR(y1 - y0));
				}
				else	// �ٰ��� (S', Pk, Pi, Pj)�� �����̶��,
				{
					// Dijkstra �˰����� �����Ѵ�.
					Ui = (Uj + dij < Uk + dik) ? (Uj + dij) : (Uk + dik);					
				}
			}
			else	// �ﰢ���� �������� �ʴ� �����,
			{
				// Dijkstra �˰����� �����Ѵ�.
				Ui = (Uj + dij < Uk + dik) ? (Uj + dij) : (Uk + dik);				
			}			

			// ���� �Ÿ��� ������ ���� �Ÿ����� �۴ٸ�,
			if (Ui < Distances[i])
			{
				// �ִ� �Ÿ��� �����ϰ�, ���� ������ �ε����� �߰��Ѵ�.
				Distances[i] = Ui;
				if (Ui < MaxRadius)
					MinHeap.push(i);
			}
		}
	}
}

/*! 
*	\brief �� �������� �޽��� �� ���������� edge �Ÿ��� ����ϴ� �Լ�
*
*	\param pMesh ��� �޽�
*	\param pVert ���õ� �޽� ���� ����
*	\param Distances pVert���� i��° ���������� �����Ÿ��� Distances[i]�� ����ȴ�.
*	\param MaxRadius �����Ÿ��� ����� �ִ� �ݰ�
*/
void get_edge_dist_field(GMesh *pMesh, GVertex *pVert, std::vector<double> &Distances, double MaxRadius)
{
	// �̸� ���� ���� ������ �����ϱ� ���� ���� ������ ����Ѵ�.
	static GMesh *pPrevMesh = NULL;
	static int PrevNumVert = 0;
	static std::vector<std::vector<int>> VertList;	

	// ��� �޽��� �ٲ�ų� ���������� ����Ǿ��ٸ�,
	int NumVert = pMesh->GetNumVert();
	if (pPrevMesh != pMesh || PrevNumVert != NumVert)
	{
		// ���Ӱ� ���� ������ ���Ѵ�.
		::get_neighbor_verts_of_vert(pMesh, VertList);
		pPrevMesh = pMesh;
		PrevNumVert = NumVert;
	}

	// �Ÿ����� �ʱ�ȭ �ϰ�,
	Distances.assign(NumVert, 1000000.0);

	// �ּ� ���� �����Ѵ�.
	GHeap MinHeap(&Distances);

	// ���õ� ������ �Ÿ��� 0���� �ʱ�ȭ �ϰ�, �ε����� ���� �߰��Ѵ�.
	Distances[pVert->Idx] = 0.0;
	MinHeap.push(pVert->Idx);

	// ó���� ������ ���� �ִٸ�,
	while (!MinHeap.empty())
	{
		// �ּ� �Ÿ��� ���� ������ �ε����� ���Ͽ�,
		int j = MinHeap.get_candidate();

		// �̿��� ��� �ﰢ���� ���Ͽ�,
		int valance = (int)VertList[j].size();
		for (int l = 0; l < valance; ++l)
		{
			// �ﰢ�� (vi, vj, vk)�� ���Ͽ�,
			int i = VertList[j][l];
			int k = (l == 0) ? VertList[j][valance - 1] : VertList[j][l - 1];

			// �� ������ ��ǥ�� ���ϰ�,
			GPoint3 Pi = pMesh->Verts[i].P;
			GPoint3 Pj = pMesh->Verts[j].P;
			GPoint3 Pk = pMesh->Verts[k].P;

			// ���簡�� ���� �� ������ �ּҰŸ� Uj, Uk�� ���ϰ�,
			double Ui = 0.0;
			double Uj = Distances[j];
			double Uk = Distances[k];

			// �ﰢ���� �� ���� ���̸� ����ϰ�,
			double dij = dist(Pi, Pj);
			double dik = dist(Pi, Pk);
			double djk = dist(Pj, Pk);

			// Dijkstra �˰����� �����Ѵ�.
			Ui = (Uj + dij < Uk + dik) ? (Uj + dij) : (Uk + dik);

			// ���� �Ÿ��� ������ ���� �Ÿ����� �۴ٸ�,
			if (Ui < Distances[i])
			{
				// �ִ� �Ÿ��� �����ϰ�, ���� ������ �ε����� �߰��Ѵ�.
				Distances[i] = Ui;
				if (Ui < MaxRadius)
					MinHeap.push(i);
			}
		}
	}	
}

/*! 
*	\brief �ﰢ�� ������ �� ������ �޽��� �� ���������� �����Ÿ��� ����ϴ� �Լ�
*
*	\param pMesh ��� �޽�
*	\param fidx ���õ� ������ ���Ե� �ﰢ���� �ε���
*	\param Coords  �ﰢ�� ���ο��� ���õ� ������ baricentric coordinates
*	\param Distances ���õ� �ﰢ�� ������ �������� i��° ���������� �����Ÿ��� Distances[i]�� ����ȴ�.
*	\param MaxRadius �����Ÿ��� ����� �ִ� �ݰ�
*/
void get_edge_dist_field(GMesh *pMesh, int fidx, GVector3 Coords, std::vector<double> &Distances, double MaxRadius)
{
	// �̸� ���� ���� ������ �����ϱ� ���� ���� ������ ����Ѵ�.
	static GMesh *pPrevMesh = NULL;
	static int PrevNumVert = 0;
	static std::vector<std::vector<int>> VertList;	

	// ��� �޽��� �ٲ�ų� ���������� ����Ǿ��ٸ�,
	int NumVert = pMesh->GetNumVert();
	if (pPrevMesh != pMesh || PrevNumVert != NumVert)
	{
		// ���Ӱ� ���� ������ ���Ѵ�.
		::get_neighbor_verts_of_vert(pMesh, VertList);
		pPrevMesh = pMesh;
		PrevNumVert = NumVert;
	}

	// �Ÿ����� �ʱ�ȭ �ϰ�,
	Distances.assign(NumVert, 1000000.0);

	// �ּ� ���� �����Ѵ�.
	GHeap MinHeap(&Distances);

	// �ﰢ�� ������ ������ ��ǥ�� ����Ѵ�.
	int *vidx = pMesh->Faces[fidx].vIdx;
	GPoint3 p0 = pMesh->Verts[vidx[0]].P;
	GPoint3 p1 = pMesh->Verts[vidx[1]].P;
	GPoint3 p2 = pMesh->Verts[vidx[2]].P;
	GPoint3 pt = p2 + (p0 - p2) * Coords[0] + (p1 - p2) * Coords[1];

	// ���õ� �������κ����� �Ÿ��� ����ϰ� �ε����� ���� �߰��Ѵ�.
	Distances[vidx[0]] = dist(pt, p0);
	Distances[vidx[1]] = dist(pt, p1);
	Distances[vidx[2]] = dist(pt, p2);
	MinHeap.push(vidx[0]);
	MinHeap.push(vidx[1]);
	MinHeap.push(vidx[2]);

	// ó���� ������ ���� �ִٸ�,
	while (!MinHeap.empty())
	{
		// �ּ� �Ÿ��� ���� ������ �ε����� ���Ͽ�,
		int j = MinHeap.get_candidate();

		// �̿��� ��� �ﰢ���� ���Ͽ�,
		int valance = (int)VertList[j].size();
		for (int l = 0; l < valance; ++l)
		{
			// �ﰢ�� (vi, vj, vk)�� ���Ͽ�,
			int i = VertList[j][l];
			int k = (l == 0) ? VertList[j][valance - 1] : VertList[j][l - 1];

			// �� ������ ��ǥ�� ���ϰ�,
			GPoint3 Pi = pMesh->Verts[i].P;
			GPoint3 Pj = pMesh->Verts[j].P;
			GPoint3 Pk = pMesh->Verts[k].P;

			// ���簡�� ���� �� ������ �ּҰŸ� Uj, Uk�� ���ϰ�,
			double Ui = 0.0;
			double Uj = Distances[j];
			double Uk = Distances[k];

			// �ﰢ���� �� ���� ���̸� ����ϰ�,
			double dij = dist(Pi, Pj);
			double dik = dist(Pi, Pk);
			double djk = dist(Pj, Pk);

			// Dijkstra �˰����� �����Ѵ�.
			Ui = (Uj + dij < Uk + dik) ? (Uj + dij) : (Uk + dik);

			// ���� �Ÿ��� ������ ���� �Ÿ����� �۴ٸ�,
			if (Ui < Distances[i])
			{
				// �ִ� �Ÿ��� �����ϰ�, ���� ������ �ε����� �߰��Ѵ�.
				Distances[i] = Ui;
				if (Ui < MaxRadius)
					MinHeap.push(i);
			}
		}
	}	
}

/*!
*	\brief ���õ� �������� �޽��� �� ���������� �ִܰŸ��� �̿��� ����ǥ�� ����Ѵ�.
*
*	\param pMesh ��� �޽�
*	\param pVert ���õ� ����
*	\param Distances �� ������ ����ǥ ������ (Distances[i] = ���õ� �������� i��° ���������� �ִ� �Ÿ�)
*	\param Angles �� ������ ����ǥ ���� (Angles[i] = i��° ������ ����ǥ ����)
*	\param MaxRadius �Ÿ����� ����� �ִ� ������ �Ÿ���
*/
void get_geodesic_polar_coords(GMesh *pMesh, GVertex *pVert, std::vector<double> &Distances, std::vector<double> &Angles, double MaxRadius)
{
	// �̸� ���� ���� ������ �����ϱ� ���� ���� ������ ����Ѵ�.
	static GMesh *pPrevMesh = NULL;
	static int PrevNumVert = 0;
	static std::vector<std::vector<int>> VertList;	

	// ��� �޽��� �ٲ�ų� ���������� ����Ǿ��ٸ�,
	int NumVert = pMesh->GetNumVert();
	if (pPrevMesh != pMesh || PrevNumVert != NumVert)
	{
		// ���Ӱ� ���� ������ ���Ѵ�.
		::get_neighbor_verts_of_vert(pMesh, VertList);
		pPrevMesh = pMesh;
		PrevNumVert = NumVert;
	}

	// �Ÿ���� ������ �ʱ�ȭ �ϰ�,
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

	// �ּ� ���� �����Ѵ�.
	GHeap MinHeap(&Distances);

	// ���õ� ������ �Ÿ��� ������ 0���� �ʱ�ȭ �ϰ�, �ε����� ���� �߰��Ѵ�.
	Distances[pVert->Idx] = 0.0;
	Angles[pVert->Idx] = 0.0;
	MinHeap.push(pVert->Idx);

	// ó���� ������ ���� �ִٸ�,
	while (!MinHeap.empty())
	{
		// �ּ� �Ÿ��� ���� ������ �ε����� ���Ͽ�,
		int j = MinHeap.get_candidate();

		int valance = (int)VertList[j].size();
		for (int r = 0; r < 2; ++r)
		{
			for (int l = 0; l < valance; ++l)
			{
				// ���� Pj�� ������ �ﰢ��(i, j, k)�� ���� �ε��� i, k�� ���ϰ�,
				int i = VertList[j][l];
				int k = (l == 0) ? VertList[j][valance - 1] : VertList[j][l - 1];

				// �ﰢ�� �� ������ ��ġ�� ���ϰ�,
				GPoint3 Pi = pMesh->Verts[i].P;
				GPoint3 Pj = pMesh->Verts[j].P;
				GPoint3 Pk = pMesh->Verts[k].P;

				// ����� �Ÿ� Ui�� �̹� ������ �Ÿ� Uj, Uk�� ���ϰ�,
				double Ui = 0.0;
				double Uj = Distances[j];
				double Uk = Distances[k];

				// ����� ���� Thetai�� �̹� ������ ���� Thetaj, Thetak�� ���ϰ�,
				double Thetai = 0.0;
				double Thetaj = Angles[j];
				double Thetak = Angles[k];

				// �ﰢ���� ������ ������ ������ ���̸� ����Ѵ�.
				double dij_sq = dist_sq(Pi, Pj);
				double dik_sq = dist_sq(Pi, Pk);
				double djk_sq = dist_sq(Pj, Pk);
				double dij = SQRT(dij_sq);
				double dik = SQRT(dik_sq);
				double djk = SQRT(djk_sq);

				// ����, �ﰢ���� �����ȴٸ�,
				if (ABS(Uj - Uk) < djk && djk < Uj + Uk)
				{
					// 2���� ��鿡 ������ S'(x0, y0), Pi(x1, y1), Pj(0, 0), Pk(-djk, 0)�� ��ǥ�� ���ϰ�,
					double x0 = (SQR(Uk) - SQR(Uj) - djk_sq) / (2 * djk);
					double y0 = SQRT(SQR(Uj) - SQR(x0));				
					double x1 = (dik_sq - dij_sq - djk_sq) / (2 * djk);
					double y1 = -SQRT(dij_sq - SQR(x1));

					// S'�� Pi�� �Ÿ��� ����Ͽ�, Ui�� �Ÿ��� �����Ѵ�.
					double m = (y1 - y0) / (x1 - x0);
					double t = x0 - y0 / m;

					// �ٰ��� (S', Pk, Pi, Pj)�� �����̶��,
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
					else	// �ٰ��� (S', Pk, Pi, Pj)�� �����̶��,
					{
						// Dijkstra �˰����� ����Ѵ�.
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
				else	// �ﰢ���� �������� �ʴ� �����,
				{
					// Dijkstra �˰����� ����Ѵ�.
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

				// ���� �Ÿ��� ���� �Ÿ����� �۴ٸ�,
				if (Ui < Distances[i])
				{
					// ���� �Ÿ��� ������ �����ϰ�,
					Distances[i] = Ui;
					if (j != pVert->Idx)
						Angles[i] = Thetai;

					// ���� Pi�� �ּ� ���� �Է��Ѵ�.
					if (Ui < MaxRadius)
						MinHeap.push(i);
				}
			}
		}		
	}
}

/*!
*	\brief �޽� ������ �� ���� ������ �ִ� ��θ� ����Ѵ�.
*
*	\param pMesh ��� �޽�
*	\param P ���� ������ ���� ������
*	\param Q �� ������ ���� ������
*	\param Paths ���� P�� Q�� �����ϴ� �ִ� ��θ� �����ϴ� ������ ����ȴ�.
*/
void get_geodesic_paths(GMesh *pMesh, GVertex *P, GVertex *Q, std::vector<GPoint3> &Paths)
{
	// ��θ� �ʱ�ȭ �Ѵ�.
	Paths.clear();

	// �������� ������ ������ �����Ѵ�.
	if (P == Q)
		return;

	// ���� ���� �߰��Ѵ�.
	Paths.push_back(P->P);

	// �����Ÿ��� �̿��Ͽ� ������ �������� �������� ����ǥ(r, theta)�� ����Ѵ�.
	std::vector<double> Distances;
	std::vector<double> Angles;
	double Radius = dist(P->P, Q->P) * 1.5;
	::get_geodesic_polar_coords(pMesh, P, Distances, Angles, Radius);

	// ����鿡�� ���� P�� Q�� �� �������� �ϴ� ���� l(s)�� �����Ѵ�.
	double theta = Angles[Q->Idx];
	GLine l(GPoint3(0.0, 0.0, 0.0), GPoint3(Distances[Q->Idx] * cos(theta), Distances[Q->Idx] * sin(theta), 0.0));

	// P�� Q�� �ش� �ִܰ�� ������ ������ ������ ������ �����Ѵ�.
	std::map<double, GPathInfo> PathInfoList;

	// ���� �޽��� ������ �ﰢ���� ���Ͽ�,
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		// P���� �ﰢ���� �� ���������� �Ÿ��� ���ϰ�,
		int vidx0 = pMesh->Faces[i].vIdx[0];
		int vidx1 = pMesh->Faces[i].vIdx[1];
		int vidx2 = pMesh->Faces[i].vIdx[2];
		double d0 = Distances[vidx0];
		double d1 = Distances[vidx1];
		double d2 = Distances[vidx2];
		if (vidx0 == P->Idx || vidx1 == P->Idx || vidx2 == P->Idx)
			continue;

		// �ﰢ���� ���� ���� ���Եȴٸ�,
		if (d0 < Radius && d1 < Radius && d2 < Radius)
		{
			// ����鿡�� �ﰢ���� �� ������ ��ǥ�� ���Ͽ�,
			GPoint3 p0(Distances[vidx0] * cos(Angles[vidx0]), Distances[vidx0] * sin(Angles[vidx0]), 0.0);
			GPoint3 p1(Distances[vidx1] * cos(Angles[vidx1]), Distances[vidx1] * sin(Angles[vidx1]), 0.0);
			GPoint3 p2(Distances[vidx2] * cos(Angles[vidx2]), Distances[vidx2] * sin(Angles[vidx2]), 0.0);

			// �� ���п� �ش�Ǵ� ������ ������ l(t)�� �����Ѵ�.
			GLine l01(p0, p1);
			GLine l12(p1, p2);
			GLine l20(p2, p0);

			// �ִ� ��ο� ���а��� �������� �߰��Ѵ�.
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

	// ����鿡�� ���� �������� ������ ���� 3���� �޽����� �ִܰŸ� ������ ��ȯ�Ѵ�.
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

	// �������� ��ο� ���ԵǾ��ٸ�, ������ �߰��Ѵ�.
	if (Paths.size() < 2)
		Paths.push_back(Q->P);
}

/*!
*	\brief �޽��� �� ������ ������ �ﰢ�� �ε����� �����Ѵ�
*	\note i��° ������ ������ n ���� �ﰢ������ �ε����� FaceList[i][0], FaceList[i][1], ... FaceList[i][n-1]�� ��
*
*	\param pMesh ��� �޽�
*	\param FaceList �ﰢ�� �ε����� ����� ���� ����Ʈ (�ݽð� �������� ����)
*/
void get_neighbor_faces_of_vert(GMesh *pMesh, std::vector<std::vector<int>> &FaceList)
{
	std::vector<int> tmp;
	int vnum = pMesh->GetNumVert();
	FaceList.assign(vnum, tmp);

	std::vector<GFace> &Faces = pMesh->Faces;
	std::vector<GVertex> &Verts = pMesh->Verts;

	// �ϴ�, ������ �ε����� �ش��ϴ� ��ġ�� �ﰢ���� �ε����� �߰��ϰ�,
	int fnum = pMesh->GetNumFace();
	for (int i = 0; i < fnum; ++i)
	{
		FaceList[Faces[i].vIdx[0]].push_back(i);
		FaceList[Faces[i].vIdx[1]].push_back(i);
		FaceList[Faces[i].vIdx[2]].push_back(i);
	}

	// �ﰢ�� ����Ʈ�� �ݽð� �������� �����ϱ� ���ؼ�,
	for (int i = 0; i < vnum; ++i)
	{
		// �� ������ ������ �ﰢ���� ������ ���ϰ�,
		int deg = FaceList[i].size();
		for (int j = 0; j < deg; ++j)
		{
			// ���� �ﰢ���� ������ ��,
			tmp.clear();
			tmp.push_back(FaceList[i][j]);

			for (int k = 1; k < deg; ++k)
			{
				// ���� �ﰢ���� �ε����� ���ϰ�,
				int curr_fidx = tmp[tmp.size() - 1];

				// ���� i�� �������� �ﰢ���� �ε����� �������ϰ�,
				int vidx0 = i;
				int vidx1 = Faces[curr_fidx].GetNextVertIdx(vidx0);
				int vidx2 = Faces[curr_fidx].GetNextVertIdx(vidx1);

				bool flag = false;
				// ����(����) �ﰢ���� ������ ���� �ﰢ���� ã�´�.
				for (int l = 0; l < deg; ++l)
				{
					int next_fidx = FaceList[i][l];
					if (next_fidx == curr_fidx)
						continue;

					// ������ �ﰢ���̶��, �ε����� �����ϰ� ����.
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

			// ������ �ﰢ���� ��� ����Ǿ��ٸ�, 
			if (tmp.size() == deg)
			{
				// ����������,
				FaceList[i] = tmp;
				break;
			}
			// �׷��� �ʴٸ�, ���� �ﰢ���� �ٲپ� �ٽ� �����Ѵ�.
		}
	}
}

/*!
*	\brief �޽��� �� ������ ������ ������ �ε����� �����Ѵ�
*	\note i��° ������ ������ n���� ������ �ε����� VertList[i][0], VertList[i][1], ... VertList[i][n-1]�� �����
*
*	\param pMesh ��� �޽�
*	\param VertList ������ ������ �ε����� ����� (�ݽð� �������� ����)
*/
void get_neighbor_verts_of_vert(GMesh *pMesh, std::vector<std::vector<int>> &VertList)
{
	// ������ ������ �ﰢ�� �ε����� ���ϰ�,
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
			// ���� i�� ������ �ﰢ���鿡 ���Ͽ�,
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
*	\brief �޽��� �� �ﰢ���� ������ �ﰢ�� �ε����� �����Ѵ�.
*	\note [f0, f1, f2, f3, ..., fn] -> [(f00, f01, f02), (f10, f11, f12), ..., (fn0, fn1, fn2)]
*	\note i��° �ﰢ���� ������ �ﰢ���� �ε��� ������ FaceList[i * 3], FaceList[i * 3 + 1], FaceList[i * 3 + 2]�� �����
*	\note ������ �ﰢ���� ���� ������ e0, e1, e2�� ���� �𼭸��� ������ �ﰢ�� ������ �����, (�߿�: ei �� vi�� �����ϴ� �𼭸�).
*
*	\param pMesh ��� �޽�
*	\param FaceList �ﰢ���� ������ �ﰢ���� �ε����� ����� (������ �ﰢ���� ������ -1�� �����)
*/
void get_neighbor_faces_of_face(GMesh *pMesh, std::vector<int> &FaceList)
{
	// ������ ������ �ﰢ�� �ε����� ���ϰ�,
	std::vector<std::vector<int>> VertList;
	::get_neighbor_faces_of_vert(pMesh, VertList);

	std::vector<GFace> &Faces = pMesh->Faces;

	// �ﰢ�� ����Ʈ�� �ʱ�ȭ �ϰ�,
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
*	\brief �־��� ������ K-ring Neighborhood ������ ���Ѵ�.
*
*	\param pMesh ��� �޽�
*	\param VertIdx �־��� ������ �ε���
*	\param K Ž���� �ܰ�
*	\param VertList �� �ܰ�(0 ~ K)���� ������ ������ ����Ʈ�� ����� (VertList[0][]: 0-ring ����, VertList[1][]: 1-ring ����, ...)
*	\param FaceList �� �ܰ�(0 ~ K-1)���� ������ �ﰢ���� ����Ʈ�� ����� (FaceList[0][]: 0-ring �ﰢ��, FaceList[1][]: 1-ring �ﰢ��, ...)
*/
void get_neighbor_info_of_vert(GMesh *pMesh, int VertIdx, int K, std::vector<std::vector<int>> &VertList, std::vector<std::vector<int>> &FaceList)
{
	// ������ �ﰢ�� ����Ʈ�� �ʱ�ȭ �Ѵ�.
	std::vector<int> tmp;
	VertList.clear();
	FaceList.clear();
	VertList.assign(K + 1, tmp);
	FaceList.assign(K, tmp);

	// �޽��� ��� ������ ���Ͽ� ������ ������ ����Ʈ�� �����Ѵ�.
	std::vector<std::vector<int>> AdjVertList;
	::get_neighbor_verts_of_vert(pMesh, AdjVertList);

	// �޽��� ��� ������ ���Ͽ� ������ �ﰢ���� ����Ʈ�� �����Ѵ�.
	std::vector<std::vector<int>> AdjFaceList;
	::get_neighbor_faces_of_vert(pMesh, AdjFaceList);

	// �߰��� ������ �ﰢ���� ������ �����ϴ� ��
	std::vector<bool> vFlags;
	std::vector<bool> fFlags;
	vFlags.assign(pMesh->GetNumVert(), false);
	fFlags.assign(pMesh->GetNumFace(), false);

	// 0 �ܰ迡 ���� ������ �ε����� �߰��ϰ�,
	VertList[0].push_back(VertIdx);
	vFlags[VertIdx] = true;

	// �� �ܰ�(1, 2, ... , K)���� ������ ������ ����Ʈ�� ���Ѵ�.
	for (int i = 1; i < K + 1; ++i)
	{
		// ���� �ܰ��� ���� ����Ʈ�� ������ ���ϰ�,
		int PrevNumVert = (int)VertList[i - 1].size();

		// ���� �ܰ��� ������ ������ ���Ͽ�,
		for (int j = 0; j < PrevNumVert; ++j)
		{
			int PrevVertIdx = VertList[i - 1][j];
			int NewNumVert = (int)AdjVertList[PrevVertIdx].size();

			// ���� �ܰ��� ������ ������ �̿������鿡 ���Ͽ�,
			for (int k = 0; k < NewNumVert; ++k)
			{
				// �̿��������� �ε����� ���ϰ� ����Ʈ�� �߰��Ѵ�.
				int NewVertIdx = AdjVertList[PrevVertIdx][k];
				if (vFlags[NewVertIdx] == false)	// ���ο� ������ �ε������
				{
					VertList[i].push_back(NewVertIdx);
					vFlags[NewVertIdx] = true;					
				}
			}
		}
	}

	// �� �ܰ�(0, 1, ... , K - 1)���� ������ �ﰢ���� ����Ʈ�� ���Ѵ�.
	for (int i = 0; i < K; ++i)
	{
		// ���� ����Ʈ�� ������ ���ϰ�,
		int NumVert = (int)VertList[i].size();

		// ������ ������ ���Ͽ�,
		for (int j = 0; j < NumVert; ++j)
		{
			int VertIdx = VertList[i][j];
			int NumFace = (int)AdjFaceList[VertIdx].size();

			// ������ ������ ������ �ﰢ���鿡 ���Ͽ�,
			for (int k = 0; k < NumFace; ++k)
			{
				// �̿��������� �ε����� ���ϰ� ����Ʈ�� �߰��Ѵ�.
				int FaceIdx = AdjFaceList[VertIdx][k];
				if (fFlags[FaceIdx] == false)	// ���ο� �ﰢ�� �ε������,
				{
					FaceList[i].push_back(FaceIdx);
					fFlags[FaceIdx] = true;					
				}
			}
		}
	}
}

/*!
*	\brief �޽��� �� ������ ��� �������� �����Ѵ�.
*
*	\param pMesh ��� �޽�
*	\param InfoList ��� ���� ���ΰ� �����, ���� i�� ��� �����̸�, InfoList[i] = true.
*/
void get_bndry_vert_info(GMesh *pMesh, std::vector<bool> &InfoList)
{
	// ���� ����Ʈ�� �ʱ�ȭ�ϰ�,
	InfoList.clear();
	InfoList.assign(pMesh->GetNumVert(), false);

	// �� �ﰢ���� ������ �ﰢ�� ������ ���ϰ�, 
	std::vector<int> FaceList;
	::get_neighbor_faces_of_face(pMesh, FaceList);

	// ��� ���� ���θ� �Ǵ��Ͽ� ����Ѵ�.
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
*	\brief �޽��� ��� �𼭸��� ���Ѵ�.
*
*	\param pMesh ��� �޽�
*	\param EdgeList ��� �𼭸��� (�� �ε���, ���� �ε���)�� ���� ���·� �����Ѵ�.
*/
void get_bndry_edges(GMesh *pMesh, std::map<int, int> &EdgeList)
{
	// ��� �𼭸��� ������ �ʱ�ȭ �Ѵ�.
	EdgeList.clear();

	// �� �ﰢ���� ������ �ﰢ�� ������ ���ϰ�, 
	std::vector<int> FaceList;
	get_neighbor_faces_of_face(pMesh, FaceList);

	// ��� �𼭸��� ������ �����Ѵ�.
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
*	\brief �ܺ� ���� ���� ���(����) ���� �ε����� ��ȯ�Ѵ�.
*	\brief ���� ���� ���(����)�� ������ �� ������, 
*	\brief i��° ������ ��� ������ �ε����� VertList[i][0], VertList[i][1], ... VertList[i][n]�� ����ȴ�.
*
*	\param pMesh ��� �޽�
*	\param VertList ��� ������ �ε����� �����
*/
void get_bndry_verts(GMesh *pMesh, std::vector<std::vector<int>> &VertList)
{
	// �� �ﰢ���� ������ �ﰢ�� ������ ���ϰ�, 
	std::vector<int> FaceList;
	get_neighbor_faces_of_face(pMesh, FaceList);

	// ��� �𼭸��� <���� �ε���, ������ �ε���>�� ������ ���� �����ϰ�,
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

	// ��������� �ݽð� �������� �����Ѵ�.
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
*	\brief ���� ������ �ε����� ��ȯ�Ѵ�.
*
*	\param pMesh ��� �޽�
*	\param VertList ���� ������ �ε����� �����
*/
void get_intr_verts(GMesh *pMesh, std::vector<int> &VertList)
{
	// �� ������ ��� ���� ���θ� �����ϰ�,
	std::vector<bool> InfoList;
	get_bndry_vert_info(pMesh, InfoList);

	int NumVert = pMesh->GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		// ���� ������ ���, ���� ����Ʈ�� �����Ѵ�.
		if (InfoList[i] == false)
			VertList.push_back(i);
	}	
}

/*!
*	\brief �޽��� non-manifold �ﰢ���� ����Ʈ�� ã�´�.
*
*	\param pMesh ��� �޽�
*	\param FaceList non-manifold �ﰢ���� �����Ͱ� ����ȴ�.
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

	// Ȥ�� �ߺ��� �����Ѵ�.
	sort(FaceList.begin(), FaceList.end());
	FaceList.erase(unique(FaceList.begin(), FaceList.end()), FaceList.end());
}

/*!
*	\brief �޽��� degenerated �ﰢ���� ����Ʈ�� ã�´�.
*
*	\param pMesh ��� �޽�
*	\param FaceList degenerated �ﰢ���� �����Ͱ� ����ȴ�.
*	\param eps degenerated �ﰢ���� �Ǵ��ϱ� ���� ������ �ּҰ�
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

	// Ȥ�� �ߺ��� �����Ѵ�.
	sort(FaceList.begin(), FaceList.end());
	FaceList.erase(unique(FaceList.begin(), FaceList.end()), FaceList.end());
}

/*!
*	\brief �޽��� isolated ������ ����Ʈ�� ã�´�.
*
*	\param pMesh ��� �޽�
*	\param VertList isolated ������ �����Ͱ� ����ȴ�.
*/
void find_isolated_verts(GMesh *pMesh, std::vector<GVertex *> &VertList)
{
	// ������ ��� ���θ� üũ�ϱ� ����, ������ ������ŭ flag�迭�� �����ϰ�, false�� �ʱ�ȭ�� ��, 
	int NumVert = pMesh->GetNumVert();
	std::vector<bool> UsedFlag;
	UsedFlag.assign(NumVert, false);

	// ���� ������ �÷��׸� true�� �����Ѵ�.
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		UsedFlag[pMesh->Faces[i].vIdx[0]] = true;
		UsedFlag[pMesh->Faces[i].vIdx[1]] = true;
		UsedFlag[pMesh->Faces[i].vIdx[2]] = true;
	}

	// ������ ���� ������ ����Ʈ�� �߰��Ѵ�.
	for (int i = 0; i < NumVert; ++i)
	{
		if (UsedFlag[i] == false)
			VertList.push_back(&(pMesh->Verts[i]));
	}

	// Ȥ�� �ߺ��� �����Ѵ�.
	sort(VertList.begin(), VertList.end());
	VertList.erase(unique(VertList.begin(), VertList.end()), VertList.end());
}