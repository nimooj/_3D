#include "stdafx.h"

/***********************************/
/* Implementation of GVertex Class */
/***********************************/
IMPLEMENT_GEDIT_OBJECT(GEditObj)

/*!
*	\brief ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GEditObj::GetRefMesh()
{
	return NULL;
}

/*!
*	\brief ���� ��ǥ�迡�� ǥ���� ���� ��ü�� ��ȯ ������ ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ǥ���� ���� ��ü�� ��ȯ ������ ��ȯ�Ѵ�.
*/
GTransf GEditObj::GetTransf()
{
	return GTransf();
}

/*!
*	\brief ���õ� ��ü�� �����Ѵ�.
*
*	\param EC ���� ��ǥ��
*	\param t ������ ��ü�� �̵� ����
*	\param q ������ ��ü�� ȸ�� ����
*	\param s ������ ��ü�� ������ ����
*	\param pParentMesh ���� ��ü�� �θ� �޽�
*/
void GEditObj::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
}

/*!
*	\brief ���õ� ��ü�� ������ �Ѵ�.
*/
void GEditObj::RenderForEdit(float r, float g, float b)
{
}

/***********************/
/* GVertex Ŭ���� ���� */
/***********************/
IMPLEMENT_GEDIT_OBJECT(GVertex)

/*!
*	\brief ������ ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return ������ ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GVertex::GetRefMesh()
{
	return pMesh;
}

/*!
*	\brief ������ �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ������ �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*/
GTransf GVertex::GetTransf()
{
// 	GQuater q;
// 	q.SetFromArcBall(GVector3(0.0, 1.0, 0.0), N);
//	return pMesh->MC * GTransf(cast_vec3(P), q);
	return pMesh->MC * GTransf(cast_vec3(P));
}

/*!
*	\brief ������ ���� �޽��� �����Ѵ�.
*
*	\param EC �������� ��ǥ��
*	\param t ������ �̵� ����
*	\param q ������ ȸ�� ����
*	\param s ������ ������ ����
*	\param pParentMesh ������ ���� �޽��� �θ� �޽�
*/
void GVertex::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	// ���� ��ȯ�� ���� ���� ������ �����ϰ�,
	static GTransf MC2MC;

	// ���� ��ü�� Ŭ���� �̸��� ���Ͽ�,
	std::string ClsName = pMesh->GetClassName();

	// GMesh ��ü�� ������ ���,
	if (ClsName == "GMesh")
	{
		// �����Ǵ� ù ��° �����̰ų� �޽� �ﰢ���� ��ġ�� ����Ǿ��ٸ�,
		if (pMesh->ModifiedFlag == MODIFIED_NONE || pMesh->ModifiedFlag == MODIFIED_FACE_POS)
		{
			// ���� ��ȯ�� ����ϰ�,
			MC2MC = inv(pMesh->MC) * EC * GTransf(t, q) * inv(EC) * pMesh->MC;

			// ������ �����Ǿ����� ����ϰ�,
			pMesh->ModifiedFlag = MODIFIED_VERT_POS;

			// ������ �޽��� ���� �޽����,
			if (pMesh->SemanticType == MESH_CTRL && !pMesh->LinkedMeshNameList.empty())
			{
				// ��ũ�� ���� �޽��� ���Ͽ�,
				GBlendMesh *pBlendMesh = (GBlendMesh *)(FindMesh(pMesh->pScene->MeshList, pMesh->LinkedMeshNameList[0]));
				if (pBlendMesh != NULL)
				{
					// ���� �޽��� ���� ������ ����Ǿ����� ����Ѵ�.
					pBlendMesh->ModifiedFlag = MODIFIED_INTPT_POS;
					int NumFace = pMesh->GetNumFace();
					for (int i = 0; i < NumFace; ++i)
						pBlendMesh->PatchType[i] |= PATCH_TYPE_MODIFIED;
				}
				else
					pMesh->LinkedMeshNameList.clear();
			}
		}

		// ������ ������ ��ǥ�� ����Ͽ�,
		GPoint3 pt = MC2MC * P;

		// ������ ��ȯ�� �����ϰ�,
		if (s[0] != 1.0) pt[0] *= s[0];
		if (s[1] != 1.0) pt[1] *= s[1];
		if (s[2] != 1.0) pt[2] *= s[2];

		// ���� ������ ��ġ�� �����ϰ�,
		P[0] = (1.0 - EditWgt) * P[0] + EditWgt * pt[0];
		P[1] = (1.0 - EditWgt) * P[1] + EditWgt * pt[1];
		P[2] = (1.0 - EditWgt) * P[2] + EditWgt * pt[2];

		// ���� ������ ���� ���������� �����Ѵ�.
		pMesh->VtxBuf[Idx * 3] = P[0];
		pMesh->VtxBuf[Idx * 3 + 1] = P[1];
		pMesh->VtxBuf[Idx * 3 + 2] = P[2];
	}

	// GNurbsCrv ��ü�� ������ ���,
	if (ClsName == "GNurbsCrv")
	{
		// GNurbsCrv ��ü�� �����͸� ���Ͽ�,
		GNurbsCrv *pCrv = (GNurbsCrv *)pMesh;

		// �����Ǵ� ù ��° �����̶��,
		if (pCrv->ModifiedFlag == MODIFIED_NONE)
		{
			// ���� ��ȯ�� ����ϰ�,
			MC2MC = inv(pCrv->MC) * EC * GTransf(t, q) * inv(EC) * pCrv->MC;

			// ������ ������ ����ϰ�,
			pCrv->ModifiedFlag = MODIFIED_CTLPT_POS;						
		}

		// ������ ������ ����Ͽ�,
		GPoint3 pt = MC2MC * P;
		GVector3 dp = pt - P;

		// ���õ� ��� �����Ѵ�.
		double t = pMesh->Texs[Idx][1];
		pCrv->Edit(t, cast_vec4(dp, 0.0));		
	}

	// GNurbsSrf ��ü�� ������ ���,
	if (ClsName == "GNurbsSrf")
	{
		// GNurbsSrf ��ü�� �����͸� ���Ͽ�,
		GNurbsSrf *pSrf = (GNurbsSrf *)pMesh;

		// �����Ǵ� ù ��° �����̶��,
		if (pSrf->ModifiedFlag == MODIFIED_NONE)
		{
			// ���� ��ȯ�� ����ϰ�,
			MC2MC = inv(pSrf->MC) * EC * GTransf(t, q) * inv(EC) * pSrf->MC;

			// ������ ������ ����ϰ�,
			pSrf->ModifiedFlag = MODIFIED_CTLPT_POS;						
		}

		// ������ ������ ����Ͽ�,
		GPoint3 pt = MC2MC * P;
		GVector3 dp = pt - P;

		// ���õ� ����� �����Ѵ�.
		double u = pMesh->Texs[Idx][0];
		double v = pMesh->Texs[Idx][1];
		pSrf->Edit(u, v, cast_vec4(dp, 0.0));
	}
}

/*!
*	\brief ���� ��ü�� ������ �Ѵ�.
*/
void GVertex::RenderForEdit(float r, float g, float b)
{
	// ���� ��ǥ��: ����(���)
	glPushMatrix();
	{
		// ������ ������ �޽� ��ü�� �𵨸� ��ǥ��� ��ȯ
		glMultMatrixd(cast_mat(pMesh->MC).M);

		int RenderMode = 0;
		switch (RenderMode)
		{
			// ����Ʈ ������
		case 0:
			{
				// ���� ���� �����ϰ�,
				glDisable(GL_LIGHTING);
				glDisable(GL_DEPTH_TEST);
				glEnable(GL_POINT_SMOOTH);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				//glColor4f(0.2627, 1.0, 0.64, 0.5);
				glColor4f(0, 0, 1, 1.0);
				glPointSize(9.0);
				glBegin(GL_POINTS);
				glVertex3dv(P.V);
				glEnd();		

				// ���� ��忡�� ���´�.
				glDisable(GL_BLEND);
				glDisable(GL_POINT_SMOOTH);
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_LIGHTING);
			}
			break;

			// �� ������
		case 1:
			{
				// ���õ� ������ �׸���.
				double size = pMesh->BBox.GetDiagonal() * 0.003;
				glTranslated(P.V[0], P.V[1], P.V[2]);
				GMaterial mtl_blue;
				mtl_blue.SetAmbient(0.2, 0.2, 0.2);
				mtl_blue.SetDiffuse(0.0, 0.0, 1.0);
				mtl_blue.SetSpecular(0.5, 0.5, 0.5);
				mtl_blue.Enable(false);
				glutSolidSphere(size, 10, 10);
			}
			break;
		}		
	}
	glPopMatrix();
}

/*!
*	\brief ������ ����Ʈ ������
*
*	\param _P ������ ��ǥ
*	\param _N ������ ��������
*	\param _C ������ ����
*/
GVertex::GVertex(GPoint3 _P, GVector3 _N, GVector3 _C) 
{
	pMesh = NULL;
	Idx = -1;
	P = _P;
	N = _N;
	C = _C;
	EditWgt = 0.0;
}

/*!
*	\brief ������ ���� ������
*/
GVertex::GVertex(const GVertex &cpy)
{
	pMesh = cpy.pMesh;
	Idx = cpy.Idx;
	P = cpy.P;
	N = cpy.N;
	C = cpy.C;
	EditWgt = cpy.EditWgt;
	BoneInfoList = cpy.BoneInfoList;
}

/*!
*	\brief ������ �Ҹ���
*/
GVertex::~GVertex()
{
}

/*!
*	\brief ������ ���Կ�����
*
*	\param rhs ���Կ������� ������ �ǿ�����
*
*	\return ���Ե� �ڽ��� ��ȯ�Ѵ�.
*/
GVertex &GVertex::operator =(const GVertex &rhs)
{
	pMesh = rhs.pMesh;
	Idx = rhs.Idx;
	P = rhs.P;
	N = rhs.N;
	C = rhs.C;
	EditWgt = rhs.EditWgt;
	BoneInfoList = rhs.BoneInfoList;

	return *this;
}

/*!
*	\brief ������ ��ȣ������
*
*	\param lhs ���� �ǿ�����
*	\param rhs ������ �ǿ�����
*
*	\return ���Ե� �ڽ��� ��ȯ�Ѵ�.
*/
bool operator ==(const GVertex &lhs, const GVertex &rhs)
{
	return (lhs.P == rhs.P);
}

/*!
*	\brief �� �޽��� ���� ������ ���ε� ������ �����Ͽ� �߰��Ѵ�.
*
*	\param BoneMeshName ������ ���ε� �� �� �޽��� �̸�
*	\param Wgt ������ ���ε� ����ġ
*/
void GVertex::AddRefBoneInfo(std::string &BoneMeshName, double Wgt)
{
	GRefBoneInfo NewInfo(BoneMeshName, Wgt, GPoint3(), GVector3());
	BoneInfoList.push_back(NewInfo);
}

/*********************/
/* GFace Ŭ���� ���� */
/*********************/
IMPLEMENT_GEDIT_OBJECT(GFace)

/*!
*	\brief �ﰢ���� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return �ﰢ���� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GFace::GetRefMesh()
{
	return pMesh;
}

/*!
*	\brief �ﰢ���� �����߽ɿ��� ���ǵ� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return �ﰢ���� �����߽ɿ��� ���ǵ� ��ǥ��(���� ��ǥ�� ����)�� ��ȯ�Ѵ�.
*/
GTransf GFace::GetTransf()
{
	GVector3 v0 = cast_vec3(pMesh->Verts[vIdx[0]].P);
	GVector3 v1 = cast_vec3(pMesh->Verts[vIdx[1]].P);
	GVector3 v2 = cast_vec3(pMesh->Verts[vIdx[2]].P);
	GVector3 t = (v0 + v1 + v2) / 3.0;

	return pMesh->MC * GTransf(t);
}

/*!
*	\brief �ﰢ���� ���� �߽��� ����Ѵ�.
*
*	\param bLocal ��ü�� �𵨸� ��ǥ�迡�� ǥ���� ��� true, ���� ��ǥ�迡�� ǥ���� ��� false
*
*	\return �ﰢ���� ���� �߽��� ��ġ�� ��ȯ�Ѵ�.
*/
GPoint3 GFace::GetCOG(bool bLocal)
{
	double *v0 = pMesh->Verts[vIdx[0]].P.V;
	double *v1 = pMesh->Verts[vIdx[1]].P.V;
	double *v2 = pMesh->Verts[vIdx[2]].P.V;

	double x = (v0[0] + v1[0] + v2[0]) / 3.0;
	double y = (v0[1] + v1[1] + v2[1]) / 3.0;
	double z = (v0[2] + v1[2] + v2[2]) / 3.0;

	GPoint3 p(x, y, z);

	if (bLocal)
		return p;
	else
		return pMesh->MC * p;
}

/*!
*	\brief �ﰢ���� ���� ���͸� ��ȯ�Ѵ�.
*
*	\param bLocal ��ü�� �𵨸� ��ǥ�迡�� ǥ���� ������ ��� true, ���� ��ǥ�迡�� ǥ���� ��� false
*
*	\return �ﰢ���� ���� ���͸� ��ȯ�Ѵ�.
*/
GVector3 GFace::GetNormal(bool bLocal)
{
	GVector3 e1 = pMesh->Verts[vIdx[1]].P - pMesh->Verts[vIdx[0]].P;
	GVector3 e2 = pMesh->Verts[vIdx[2]].P - pMesh->Verts[vIdx[0]].P;
	GVector3 n = (e1 ^ e2).Normalize();
	
	if (bLocal)
		return n;
	else
		return pMesh->MC * n;
}

/*!
*	\brief �ﰢ���� �ﰢ���� ���Ե� ��ü�� �����Ѵ�.
*
*	\param EC �������� ��ǥ��
*	\param t �ﰢ���� �̵� ����
*	\param q �ﰢ���� ȸ�� ����
*	\param s �ﰢ���� ������ ����
*	\param pParentMesh ���� ��ü�� �θ� �޽�
*/
void GFace::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	// �޽��� ��ӹ��� ��ü��(GNurbsCrv, GNurbsSrf, GUcbsSrf, GUcbsVol, GSweepSrf, GBlendCrv) ���� 
	// ��������� ���� ������ ������.
	if (pMesh->GetClassName() != "GMesh")
		return;

	// ���� ��ȯ�� ���� ���� ������ �����ϰ�,
	static GTransf MC2MC;
	static std::vector<bool> VertFlag;

	// �����Ǵ� ù ��° �ﰢ���̶��,
	if (pMesh->ModifiedFlag == MODIFIED_NONE)
	{
		int NumVert = pMesh->GetNumVert();
		VertFlag.clear();
		VertFlag.assign(NumVert, false);

		// ���� ��ȯ�� ����ϰ�,
		MC2MC = inv(pMesh->MC) * EC * GTransf(t, q) * inv(EC) * pMesh->MC;

		// �޽� �ﰢ���� ��ġ�� �������� ����ϰ�,
		pMesh->ModifiedFlag = MODIFIED_FACE_POS;

		// �ﰢ���� �޽��� ���� �޽����,
		if (pMesh->SemanticType == MESH_CTRL && !pMesh->LinkedMeshNameList.empty())
		{
			// ��ũ�� ���� �޽��� ���Ͽ�,
			GBlendMesh *pBlendMesh = (GBlendMesh *)(FindMesh(pMesh->pScene->MeshList, pMesh->LinkedMeshNameList[0]));
			if (pBlendMesh != NULL)
			{
				// ���� �޽��� ���� ������ ����Ǿ����� ����Ѵ�.
				pBlendMesh->ModifiedFlag = MODIFIED_INTPT_POS;
				int NumFace = pMesh->GetNumFace();
				for (int i = 0; i < NumFace; ++i)
					pBlendMesh->PatchType[i] |= PATCH_TYPE_MODIFIED;
			}
			else
				pMesh->LinkedMeshNameList.clear();
		}
	}

	// �ﰢ���� �� ������ ��ǥ�� �����Ѵ�.
	if (VertFlag[vIdx[0]] == false)
	{
		VertFlag[vIdx[0]] = true;
		pMesh->Verts[vIdx[0]].P = MC2MC * pMesh->Verts[vIdx[0]].P;

		// ���� ������ ���� ���������� �����Ѵ�.
		pMesh->VtxBuf[vIdx[0] * 3] = pMesh->Verts[vIdx[0]].P[0];
		pMesh->VtxBuf[vIdx[0] * 3 + 1] = pMesh->Verts[vIdx[0]].P[1];
		pMesh->VtxBuf[vIdx[0] * 3 + 2] = pMesh->Verts[vIdx[0]].P[2];
	}
	if (VertFlag[vIdx[1]] == false)
	{
		VertFlag[vIdx[1]] = true;
		pMesh->Verts[vIdx[1]].P = MC2MC * pMesh->Verts[vIdx[1]].P;
		
		// ���� ������ ���� ���������� �����Ѵ�.
		pMesh->VtxBuf[vIdx[1] * 3] = pMesh->Verts[vIdx[1]].P[0];
		pMesh->VtxBuf[vIdx[1] * 3 + 1] = pMesh->Verts[vIdx[1]].P[1];
		pMesh->VtxBuf[vIdx[1] * 3 + 2] = pMesh->Verts[vIdx[1]].P[2];
	}
	if (VertFlag[vIdx[2]] == false)
	{
		VertFlag[vIdx[2]] = true;
		pMesh->Verts[vIdx[2]].P = MC2MC * pMesh->Verts[vIdx[2]].P;
		
		// ���� ������ ���� ���������� �����Ѵ�.
		pMesh->VtxBuf[vIdx[2] * 3] = pMesh->Verts[vIdx[2]].P[0];
		pMesh->VtxBuf[vIdx[2] * 3 + 1] = pMesh->Verts[vIdx[2]].P[1];
		pMesh->VtxBuf[vIdx[2] * 3 + 2] = pMesh->Verts[vIdx[2]].P[2];
	}	
}

/*!
*	\brief ���� �ﰢ���� ������ �Ѵ�.
*/
void GFace::RenderForEdit(float r, float g, float b)
{
	// ���� ����� Ȱ���ϰ�, ������ ��Ȱ��ȭ �Ѵ�.
	glDisable(GL_LIGHTING);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(0.3, 0.3);

	// ���� ��ǥ��: ����(���)
	glPushMatrix();
	{
		// �޽� ��ǥ��� ��ȯ
		glMultMatrixd(cast_mat(pMesh->MC).M);
	
		glColor3f(0.2627, 1.0, 0.64);
		glBegin(GL_TRIANGLES);
		{
			glVertex3dv(pMesh->Verts[vIdx[0]].P.V);
			glVertex3dv(pMesh->Verts[vIdx[1]].P.V);
			glVertex3dv(pMesh->Verts[vIdx[2]].P.V);
		}
		glEnd();
	
//		glColor3f(1.0, 0.0, 0.0);
//		glLineWidth(2.0);
// 		glBegin(GL_LINE_LOOP);
// 		{
// 			glVertex3dv(pMesh->Verts[vIdx[0]].P.V);
// 			glVertex3dv(pMesh->Verts[vIdx[1]].P.V);
// 			glVertex3dv(pMesh->Verts[vIdx[2]].P.V);
// 		}
// 		glEnd();
//		glLineWidth(1.0);
		
	}
	glPopMatrix();

	// ������ ����� ��Ȱ���ϰ�, ������ Ȱ��ȭ �Ѵ�.
	glEnable(GL_LIGHTING);
	glDisable(GL_POLYGON_OFFSET_FILL);
}

/*!
*	\brief ����Ʈ ������
*
*	\param vidx0 �ﰢ���� �����ϴ� ù��° ������ �ε���
*	\param vidx1 �ﰢ���� �����ϴ� �ι�° ������ �ε���
*	\param vidx2 �ﰢ���� �����ϴ� ����° ������ �ε���
*	\param tidx0 �ﰢ���� �����ϴ� ù��° ������ �����ϴ� �ؽ�ó ��ǥ �ε���
*	\param tidx1 �ﰢ���� �����ϴ� �ι�° ������ �����ϴ� �ؽ�ó ��ǥ �ε���
*	\param tidx2 �ﰢ���� �����ϴ� ����° ������ �����ϴ� �ؽ�ó ��ǥ �ε���
*	\param mtl_idx �ﰢ���� ����� ������ �ε���
*/
GFace::GFace(int vidx0, int vidx1, int vidx2, int tidx0, int tidx1, int tidx2, int mtl_idx)
{
	pMesh = NULL;
	Idx = -1;
	vIdx[0] = vidx0;
	vIdx[1] = vidx1;
	vIdx[2] = vidx2;
	tIdx[0] = tidx0;
	tIdx[1] = tidx1;
	tIdx[2] = tidx2;
	MtlIdx = mtl_idx;
}

/*!
*	\brief ���������
*
*	\cpy ����� ��ü
*/
GFace::GFace(const GFace &cpy)
{
	pMesh = cpy.pMesh;
	Idx = cpy.Idx;
	ARR_COPY(vIdx, cpy.vIdx, 3);
	ARR_COPY(tIdx, cpy.tIdx, 3);
	MtlIdx = cpy.MtlIdx;
}

/*!
*	\brief �Ҹ���
*/
GFace::~GFace()
{
}

/*!
*	\brief ���� ������ �ߺ�
*
*	\param rhs ���� �������� ������ ��ü
*
*	\return ���Ե� �ڽ��� ��ȯ�Ѵ�
*/
GFace &GFace::operator =(const GFace &rhs)
{
	pMesh = rhs.pMesh;
	Idx = rhs.Idx;
	ARR_COPY(vIdx, rhs.vIdx, 3);
	ARR_COPY(tIdx, rhs.tIdx, 3);
	MtlIdx = rhs.MtlIdx;

	return *this;
}

/*!
*	\brief �ﰢ���� �����ϴ� \a vidx ���� ������ �ε����� ��ȯ�Ѵ�.
*	\note ���࿡ \a vidx�� �ﰢ���� ������ �ε����� �ƴ϶�� -1�� ��ȯ�Ѵ�.
*
*	\param vidx ���� ������ �ε���
*
*	\return �ﰢ���� �����ϴ� \a vidx ���� ������ �ε����� ��ȯ�Ѵ�.
*/
int GFace::GetPrevVertIdx(int vidx)
{
	int PrevVertIdx = -1;

	if (vidx == vIdx[0])
		PrevVertIdx = vIdx[2];

	if (vidx == vIdx[1])
		PrevVertIdx = vIdx[0];

	if (vidx == vIdx[2])
		PrevVertIdx = vIdx[1];

	return PrevVertIdx;
}

/*!
*	\brief �ﰢ���� �����ϴ� \a vidx ���� ������ �ε����� ��ȯ�Ѵ�.
*	\note ���࿡ \a vidx�� �ﰢ���� ������ �ε����� �ƴ϶�� -1�� ��ȯ�Ѵ�.
*
*	\param vidx ���� ������ �ε���
*
*	\return �ﰢ���� �����ϴ� \a vidx ���� ������ �ε����� ��ȯ�Ѵ�.
*/
int GFace::GetNextVertIdx(int vidx)
{
	int NextVertIdx = -1;

	if (vidx == vIdx[0])
		NextVertIdx = vIdx[1];

	if (vidx == vIdx[1])
		NextVertIdx = vIdx[2];

	if (vidx == vIdx[2])
		NextVertIdx = vIdx[0];

	return NextVertIdx;
}

/*!
*	\brief �� �ﰢ���� ������ �𼭸��� ���� �ε����� ã�´�.
*
*	\param pAdjFace ������ �ﰢ��
*	\param sidx ���� �ﰢ���� ������ ���� �ε����� �����
*	\param eidx ���� �ﰢ���� ������ �� �ε����� �����
*
*	\return �ﰢ���� ������ ��� 1, �ﰢ���� �� ������ �����ϴ� ��� 0, �ﰢ���� �������� �ʴ� ��� -1�� ��ȯ�Ѵ�.
*/
int GFace::GetSharedVertIdx(GFace *pAdjFace, int &sidx, int &eidx)
{
	// 01 �� ���� �ε����� ���,
	if ((vIdx[0] == pAdjFace->vIdx[0] && vIdx[1] == pAdjFace->vIdx[2]) ||
		(vIdx[0] == pAdjFace->vIdx[2] && vIdx[1] == pAdjFace->vIdx[1]) ||
		(vIdx[0] == pAdjFace->vIdx[1] && vIdx[1] == pAdjFace->vIdx[0]))
	{
		sidx = vIdx[0];
		eidx = vIdx[1];

		return 1;
	}

	// 12�� ���� �ε����� ���,
	if ((vIdx[1] == pAdjFace->vIdx[0] && vIdx[2] == pAdjFace->vIdx[2]) ||
		(vIdx[1] == pAdjFace->vIdx[2] && vIdx[2] == pAdjFace->vIdx[1]) ||
		(vIdx[1] == pAdjFace->vIdx[1] && vIdx[2] == pAdjFace->vIdx[0]))
	{
		sidx = vIdx[1];
		eidx = vIdx[2];

		return 1;
	}

	// 20 �� ���� �ε����� ���,
	if ((vIdx[2] == pAdjFace->vIdx[0] && vIdx[0] == pAdjFace->vIdx[2]) ||
		(vIdx[2] == pAdjFace->vIdx[2] && vIdx[0] == pAdjFace->vIdx[1]) ||
		(vIdx[2] == pAdjFace->vIdx[1] && vIdx[0] == pAdjFace->vIdx[0]))
	{
		sidx = vIdx[2];
		eidx = vIdx[0];

		return 1;
	}

	// ���� vIdx[0]�� �����ϴ� ���,
	if (vIdx[0] == pAdjFace->vIdx[0] || vIdx[0] == pAdjFace->vIdx[1] || vIdx[0] == pAdjFace->vIdx[2])
	{
		sidx = vIdx[0];
		eidx = vIdx[1];

		return 0;
	}

	// ���� vIdx[1]�� �����ϴ� ���,
	if (vIdx[1] == pAdjFace->vIdx[0] || vIdx[1] == pAdjFace->vIdx[1] || vIdx[1] == pAdjFace->vIdx[2])
	{
		sidx = vIdx[1];
		eidx = vIdx[2];

		return 0;
	}

	// ���� vIdx[2]�� �����ϴ� ���,
	if (vIdx[2] == pAdjFace->vIdx[0] || vIdx[2] == pAdjFace->vIdx[1] || vIdx[2] == pAdjFace->vIdx[2])
	{
		sidx = vIdx[2];
		eidx = vIdx[0];

		return 0;
	}

	// �� �ﰢ���� �����ϴ� ������ ���ٸ�, -1�� ��ȯ�Ѵ�.
	return -1;
}

/*************************/
/* GPtOnFace Ŭ���� ���� */
/*************************/
IMPLEMENT_GEDIT_OBJECT(GPtOnFace)

/*!
*	\brief ���� ���� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return ���� ���� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GPtOnFace::GetRefMesh()
{
	return pFace->pMesh;
}

/*!
*	\brief ���� ��ǥ�迡�� ǥ���� ���� ���� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ǥ���� ���� ���� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*/
GTransf GPtOnFace::GetTransf()
{
	GMesh *pMesh = GetRefMesh();

	int *vidx = pFace->vIdx;
	GPoint3 p0 = pMesh->Verts[vidx[0]].P;
	GPoint3 p1 = pMesh->Verts[vidx[1]].P;
	GPoint3 p2 = pMesh->Verts[vidx[2]].P;
	GPoint3 pt = p2 + (p0 - p2) * Coords[0] + (p1 - p2) * Coords[1];

	return pMesh->MC * GTransf(cast_vec3(pt));
}

/*!
*	\brief ���õ� ���� �����Ѵ�.
*
*	\param EC ���� ��ǥ��
*	\param t ������ ��ü�� �̵� ����
*	\param q ������ ��ü�� ȸ�� ����
*	\param s ������ ��ü�� ������ ����
*	\param pParentMesh ���� ��ü�� �θ� �޽�
*/
void GPtOnFace::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	// ���� ���� �޽��� Ŭ���� �̸��� ���Ͽ�,
	GMesh *pMesh = pFace->pMesh;
	std::string ClsName = pMesh->GetClassName();

	// ���� ��ȯ�� ���� ���� ������ �����ϰ�,
	static GTransf MC2MC;

	// GMesh ��ü�� ������ ���,
	if (ClsName == "GMesh")
	{
		// �����Ǵ� ù ��° �����̶��,
		if (pMesh->ModifiedFlag == MODIFIED_NONE)
		{
			// ���� ��ȯ�� ����ϰ�,
			MC2MC = inv(pMesh->MC) * EC * GTransf(t, q) * inv(EC) * pMesh->MC;

			// ������ �����Ǿ����� ����ϰ�,
			pMesh->ModifiedFlag = MODIFIED_VERT_POS;

			// ������ �޽��� ���� �޽����,
			if (pMesh->SemanticType == MESH_CTRL && !pMesh->LinkedMeshNameList.empty())
			{
				// ��ũ�� ���� �޽��� ���Ͽ�,
				GBlendMesh *pBlendMesh = (GBlendMesh *)(FindMesh(pMesh->pScene->MeshList, pMesh->LinkedMeshNameList[0]));
				if (pBlendMesh != NULL)
				{
					// ���� �޽��� ���� ������ ����Ǿ����� ����Ѵ�.
					pBlendMesh->ModifiedFlag = MODIFIED_INTPT_POS;
					int NumFace = pMesh->GetNumFace();
					for (int i = 0; i < NumFace; ++i)
						pBlendMesh->PatchType[i] |= PATCH_TYPE_MODIFIED;
				}
				else
					pMesh->LinkedMeshNameList.clear();
			}
		}

		// ���� ���� ���Ե� �ﰢ���� �� ������ ���Ͽ�,
		int *vidx = pFace->vIdx;
		GPoint3 p0 = pMesh->Verts[vidx[0]].P;
		GPoint3 p1 = pMesh->Verts[vidx[1]].P;
		GPoint3 p2 = pMesh->Verts[vidx[2]].P;
		
		// ������ �� ������ ��ġ�� ���ϰ�,
		p0 = MC2MC * p0;
		p1 = MC2MC * p1;
		p2 = MC2MC * p2;

		// ���� ������ ��ġ�� �����ϰ�,
		pMesh->Verts[vidx[0]].P = p0;
		pMesh->Verts[vidx[1]].P = p1;
		pMesh->Verts[vidx[2]].P = p2;
		
		// ���� ������ ���� ���������� �����Ѵ�.
		pMesh->VtxBuf[vidx[0] * 3] = p0[0];
		pMesh->VtxBuf[vidx[0] * 3 + 1] = p0[1];
		pMesh->VtxBuf[vidx[0] * 3 + 2] = p0[2];

		pMesh->VtxBuf[vidx[1] * 3] = p1[0];
		pMesh->VtxBuf[vidx[1] * 3 + 1] = p1[1];
		pMesh->VtxBuf[vidx[1] * 3 + 2] = p1[2];

		pMesh->VtxBuf[vidx[2] * 3] = p2[0];
		pMesh->VtxBuf[vidx[2] * 3 + 1] = p2[1];
		pMesh->VtxBuf[vidx[2] * 3 + 2] = p2[2];
	}

	// GNurbsCrv ��ü�� ������ ���,
	if (ClsName == "GNurbsCrv")
	{
		// GNurbsCrv ��ü�� �����͸� ���Ͽ�,
		GNurbsCrv *pCrv = (GNurbsCrv *)pMesh;

		// �����Ǵ� ù ��° �����̶��,
		if (pCrv->ModifiedFlag == MODIFIED_NONE)
		{
			// ���� ��ȯ�� ����ϰ�,
			MC2MC = inv(pCrv->MC) * EC * GTransf(t, q) * inv(EC) * pCrv->MC;

			// ������ ������ ����ϰ�,
			pCrv->ModifiedFlag = MODIFIED_CTLPT_POS;						
		}

		// ���� ���� ���Ե� �ﰢ���� �� ������ ���� ���� ��ǥ�� ���ϰ�,
		int *vidx = pFace->vIdx;
		GPoint3 p0 = pMesh->Verts[vidx[0]].P;
		GPoint3 p1 = pMesh->Verts[vidx[1]].P;
		GPoint3 p2 = pMesh->Verts[vidx[2]].P;
		GPoint3 p = p2 + (p0 - p2) * Coords[0] + (p1 - p2) * Coords[1];

		// �ؽ�ó ��ǥ���� ����Ͽ�,
		GVector2 st0 = pMesh->Texs[vidx[0]];
		GVector2 st1 = pMesh->Texs[vidx[1]];
		GVector2 st2 = pMesh->Texs[vidx[2]];
		GVector2 st = st2 + (st0 - st2) * Coords[0] + (st1 - st2) * Coords[1];

		// ������ ������ ����Ͽ�,
		GPoint3 q = MC2MC * p;
		GVector3 dp = q - p;

		// ���õ� ��� �����Ѵ�.
		double t = st[1];
		pCrv->Edit(t, cast_vec4(dp, 0.0));
	}

	// GNurbsSrf ��ü�� ������ ���,
	if (ClsName == "GNurbsSrf")
	{
		// GNurbsSrf ��ü�� �����͸� ���Ͽ�,
		GNurbsSrf *pSrf = (GNurbsSrf *)pMesh;

		// �����Ǵ� ù ��° �����̶��,
		if (pSrf->ModifiedFlag == MODIFIED_NONE)
		{
			// ���� ��ȯ�� ����ϰ�,
			MC2MC = inv(pSrf->MC) * EC * GTransf(t, q) * inv(EC) * pSrf->MC;

			// ������ ������ ����ϰ�,
			pSrf->ModifiedFlag = MODIFIED_CTLPT_POS;						
		}

		// ���� ���� ���Ե� �ﰢ���� �� ������ ���� ���� ��ǥ�� ���ϰ�,
		int *vidx = pFace->vIdx;
		GPoint3 p0 = pMesh->Verts[vidx[0]].P;
		GPoint3 p1 = pMesh->Verts[vidx[1]].P;
		GPoint3 p2 = pMesh->Verts[vidx[2]].P;
		GPoint3 p = p2 + (p0 - p2) * Coords[0] + (p1 - p2) * Coords[1];

		// �ؽ�ó ��ǥ���� ����Ͽ�,
		GVector2 st0 = pMesh->Texs[vidx[0]];
		GVector2 st1 = pMesh->Texs[vidx[1]];
		GVector2 st2 = pMesh->Texs[vidx[2]];
		GVector2 st = st2 + (st0 - st2) * Coords[0] + (st1 - st2) * Coords[1];

		// ������ ������ ����Ͽ�,
		GPoint3 q = MC2MC * p;
		GVector3 dp = q - p;

		// ���õ� ����� �����Ѵ�.
		pSrf->Edit(st[0], st[1], cast_vec4(dp, 0.0));
	}
}

/*!
*	\brief ���� ���� ������ �Ѵ�.
*/
void GPtOnFace::RenderForEdit(float r, float g, float b)
{
	int RenderType = 1;

	switch (RenderType)
	{
	case 0:	// ����Ʈ ������
		{
			// ����� ���� �׽�Ʈ�� ��Ȱ��ȭ �ϰ�,
			glDisable(GL_LIGHTING);
			glDisable(GL_DEPTH_TEST);

			// ���� ��ǥ�迡��
			glPushMatrix();
			{
				// ���� ���� �𵨸� ��ǥ��� ��ȯ�ϰ�,
				GTransf MC = GetTransf();
				glMultMatrixd(cast_mat(MC).M);		

				// ���� ���� ǥ���ϰ�,
				glPointSize(8.0);
				glBegin(GL_POINTS);
				{
					glColor3f(0.0, 0.0, 0.0);
					glVertex3f(0.0, 0.0, 0.0);
				}
				glEnd();
				glPointSize(1.0);
			}
			// ���� ��ǥ��� �����Ѵ�.
			glPopMatrix();

			// ����� ���� �׽�Ʈ�� Ȱ��ȭ �Ѵ�.
			glEnable(GL_LIGHTING);
			glEnable(GL_DEPTH_TEST);

		}
		break;

	case 1:	// �� ������
		{
			// ���� ��ǥ�迡��
			glPushMatrix();
			{
				// ���� ���� �𵨸� ��ǥ��� ��ȯ�ϰ�,
				GTransf MC = GetTransf();
				glMultMatrixd(cast_mat(MC).M);		
				
				// ������ �����ϰ�,
				GMaterial mtl_blue;
				mtl_blue.SetAmbient(0.2, 0.2, 0.2);
				mtl_blue.SetDiffuse(0.0, 0.0, 1.0);
				mtl_blue.SetSpecular(0.5, 0.5, 0.5);
				mtl_blue.Enable(false);

				// ���� ũ�⸦ ����Ͽ� �������Ѵ�.
				GMesh *pMesh = pFace->pMesh;
				double size = pMesh->BBox.GetDiagonal() * 0.003;
				glutSolidSphere(size, 20, 20);
			}
			// ���� ��ǥ��� �����Ѵ�.
			glPopMatrix();
		}
		break;
	}
}

/*!
*	\brief ������
*
*	\param _pFace ���� ���� ���� �ﰢ���� ���� ������
*	\param _Coords �ﰢ���� �� ������ �������� ǥ���� ���� ���� barycentric ��ǥ
*/
GPtOnFace::GPtOnFace(GFace *_pFace, GVector3 _Coords)
{
	pFace = _pFace;
	Coords = _Coords;
}

/*!
*	\brief ���������
*
*	\cpy ����� ��ü
*/
GPtOnFace::GPtOnFace(const GPtOnFace &cpy)
{
	pFace = cpy.pFace;
	Coords = cpy.Coords;
}

/*!
*	\brief �Ҹ���
*/
GPtOnFace::~GPtOnFace()
{
}

/*!
*	\brief ���Կ����� �ߺ�
*
*	\param rhs ���Կ������� ������ ��ü
*
*	\return ���Ե� �ڽ��� ��ȯ�Ѵ�
*/
GPtOnFace &GPtOnFace::operator =(const GPtOnFace &rhs)
{
	pFace = rhs.pFace;
	Coords = rhs.Coords;
	
	return *this;
}

/*!
*	\brief ���� ������ ������ �����Ѵ�.
*
*	\param _pFace ���� ������ ���ϴ� �ﰢ���� ���� ������
*	\param _Coords �ﰢ���� ������ �������� ǥ���� ���� ������ barycentric ��ǥ
*/
void GPtOnFace::Set(GFace *_pFace, GVector3 _Coords)
{
	pFace = _pFace;
	Coords = _Coords;
}

/*****************************/
/* GRenderer Ŭ���� ���� */
/*****************************/
/*!
*	\brief ����Ʈ ������
*/
GRenderer::GRenderer()
{
	// �޽� ������ �Ӽ�(5)
	bBackFaceCull = FALSE;
	bTexture = FALSE;
	RenderType = RENDER_POLYGON;
	ShadeType = SHADE_SMOOTH;
	VertColorType = VERTEX_COLOR_ILLUMINATION;

	// �/���/���� ������ �Ӽ�(13)
	NumSampU = 30;
	NumSampV = 30;
	NumSampW = 30;
	bRenderCrvAsLine = FALSE;
	IsoSrfParam[0] = 0.0;
	IsoSrfParam[1] = 1.0;
	IsoSrfParam[2] = 0.0;
	IsoSrfParam[3] = 1.0;
	IsoSrfParam[4] = 0.0;
	IsoSrfParam[5] = 1.0;
	CtrlPtSize = 0.5;
	CrvMtlIdx = -1;
	// NodePt.clear();

	// ���� ������ �Ӽ�(2)
	SwpPlnWidth = 10.0;
	SwpPlnHeight = 10.0;
}

/*!
*	\brief ���� ������
*	
*	\param cpy ������ ��ü
*/
GRenderer::GRenderer(const GRenderer &cpy)
{
	// �޽� ������ �Ӽ�(5)
	bBackFaceCull = cpy.bBackFaceCull;
	bTexture = cpy.bTexture;
	RenderType = cpy.RenderType;
	ShadeType = cpy.ShadeType;
	VertColorType = cpy.VertColorType;

	// �/���/���� ������ �Ӽ�(13)
	NumSampU = cpy.NumSampU;
	NumSampV = cpy.NumSampV;
	NumSampW = cpy.NumSampW;
	bRenderCrvAsLine = cpy.bRenderCrvAsLine;
	ARR_COPY(IsoSrfParam, cpy.IsoSrfParam, 6);
	CtrlPtSize = cpy.CtrlPtSize;
	NodePts = cpy.NodePts;
	CrvMtlIdx = cpy.CrvMtlIdx;

	// ���� ������ �Ӽ�(2)
	SwpPlnWidth = cpy.SwpPlnWidth;
	SwpPlnHeight = cpy.SwpPlnHeight;	
}

/*!
*	\brief �Ҹ���
*/
GRenderer::~GRenderer()
{
}

/*!
*	\brief ���Կ�����
*
*	\param rhs ���Ե� ��ü
*	
*	\return ���Ե� �ڽ��� ��ȯ
*/
GRenderer &GRenderer::operator =(const GRenderer &rhs)
{
	// �޽� ������ �Ӽ�(5)
	bBackFaceCull = rhs.bBackFaceCull;
	bTexture = rhs.bTexture;
	RenderType = rhs.RenderType;
	ShadeType = rhs.ShadeType;
	VertColorType = rhs.VertColorType;

	// �/���/���� ������ �Ӽ�(13)
	NumSampU = rhs.NumSampU;
	NumSampV = rhs.NumSampV;
	NumSampW = rhs.NumSampW;
	bRenderCrvAsLine = rhs.bRenderCrvAsLine;
	ARR_COPY(IsoSrfParam, rhs.IsoSrfParam, 6);
	CtrlPtSize = rhs.CtrlPtSize;
	NodePts = rhs.NodePts;
	CrvMtlIdx = rhs.CrvMtlIdx;

	// ���� ������ �Ӽ�(2)
	SwpPlnWidth = rhs.SwpPlnWidth;
	SwpPlnHeight = rhs.SwpPlnHeight;	

	return *this;
}

/*!
*	\brief �޽��� ����ȭ ���θ� �˻��Ѵ�.
*
*	\return ������ �ȴٸ� true, �ƴϸ� false�� ��ȯ�Ѵ�.
*/
bool GRenderer::IsVisibleMesh()
{
	if ((RenderType & RENDER_POLYGON) || (RenderType & RENDER_WIRE) || (RenderType & RENDER_POINT))
		return true;
	else
		return false;
}

/*!
*	\brief ��ü�� ���ø� ���� �����Ѵ�.
*
*	\param NumU U-���� ���ø� ��
*	\param NumV V-���� ���ø� ��
*	\param NumW W-���� ���ø� ��
*/
void GRenderer::SetSampleNum(int NumU, int NumV, int NumW)
{
	NumSampU = NumU;
	NumSampV = NumV;
	NumSampW = NumW;
}

/*************************/
/* GMaterial Ŭ���� ���� */
/*************************/
/*!
*	\brief ������ ������
*
*	\param _Name ������ �̸�
*/
GMaterial::GMaterial(const std::string &_Name)
{
	Name = _Name;
	Ambient[0] = 0.2;	Ambient[1] = 0.2;	Ambient[2] = 0.2;	Ambient[3] = 1.0;
	Diffuse[0] = 0.7;	Diffuse[1] = 0.7;	Diffuse[2] = 0.7;	Diffuse[3] = 1.0;
	Specular[0] = 0.7;	Specular[1] = 0.7;	Specular[2] = 0.7;	Specular[3] = 1.0;
	Shiness = 32.0f;
	BindTexId = 0;
}

/*!
*	\brief ���� ������
*
*	\param cpy ����� ���� ��ü
*/
GMaterial::GMaterial(const GMaterial &cpy)
{
	Name = cpy.Name;
	ARR_COPY(Ambient, cpy.Ambient, 4);
	ARR_COPY(Diffuse, cpy.Diffuse, 4);
	ARR_COPY(Specular, cpy.Specular, 4);
	Shiness = cpy.Shiness;
	TexFileName = cpy.TexFileName;
	BindTexId = cpy.BindTexId;
}

/*!
*	\brief �Ҹ���
*/
GMaterial::~GMaterial()
{
}

/*!
*	\brief ���� ������
*
*	\param rhs ���� �������� ������ ��ü
*
*	\return ���Ե� �ڽ��� ��ȯ�Ѵ�.
*/
GMaterial &GMaterial::operator =(const GMaterial &rhs)
{
	Name = rhs.Name;
	ARR_COPY(Ambient, rhs.Ambient, 4);
	ARR_COPY(Diffuse, rhs.Diffuse, 4);
	ARR_COPY(Specular, rhs.Specular, 4);
	Shiness = rhs.Shiness;
	TexFileName = rhs.TexFileName;
	BindTexId = rhs.BindTexId;

	return *this;
}

/*!
*	\brief ������ �ֺ��� �ݻ����� �����Ѵ�.
*
*	\param r ������ ������ �ֺ��� �ݻ���
*	\param g ������ �ʷϻ� �ֺ��� �ݻ���
*	\param b ������ �Ķ��� �ֺ��� �ݻ���
*/
void GMaterial::SetAmbient(GLfloat r, GLfloat g, GLfloat b)
{
	Ambient[0] = r;
	Ambient[1] = g;
	Ambient[2] = b;
	Ambient[3] = 1.0f;
}

/*!
*	\brief ������ ���ݻ� ����� �����Ѵ�.
*
*	\param r ������ ������ ���ݻ� ���
*	\param g ������ �ʷϻ� ���ݻ� ���
*	\param b ������ �Ķ��� ���ݻ� ���
*/
void GMaterial::SetDiffuse(GLfloat r, GLfloat g, GLfloat b)
{
	Diffuse[0] = r;
	Diffuse[1] = g;
	Diffuse[2] = b;
	Diffuse[3] = 1.0f;
}

/*!
*	\brief ������ ���ݻ� ����� �����Ѵ�.
*
*	\param r ������ ������ ���ݻ� ���
*	\param g ������ �ʷϻ� ���ݻ� ���
*	\param b ������ �Ķ��� ���ݻ� ���
*/
void GMaterial::SetSpecular(GLfloat r, GLfloat g, GLfloat b)
{
	Specular[0] = r;
	Specular[1] = g;
	Specular[2] = b;
	Specular[3] = 1.0f;
}

/*!
*	\brief ������ ���ݻ� ������ �����Ѵ�.
*
*	\param shiness ������ ���ݻ� ����
*/
void GMaterial::SetShiness(GLfloat shiness)
{
	Shiness = shiness;
}

/*!
*	\brief ������ �������� �����Ѵ�.
*
*	\param alpha ������ ������ (0: ����, 1: ������)
*/
void GMaterial::SetAlpha(GLfloat alpha)
{
	Ambient[3] = alpha;
	Diffuse[3] = alpha;
	Specular[3] = alpha;
}

/*!
*	\brief ������ �ؽ�ó�� �����Ѵ�.
*	\note �ݵ�� OpenGL ������ ���¿��� ȣ��Ǿ�� �Ѵ�.
*
*	\param fname �ؽ�ó ���ϸ�
*/
void GMaterial::LoadTexture(const char *fname)
{
	// �ؽ�ó ������ Ȯ���ڸ� ���Ͽ�
	std::string file_ext = get_file_ext(fname);

	// Ȯ���ڰ� *.tga ������ ���,
	if (file_ext == "tga")
	{
		// �ؽ�ó ���ϸ��� �����ϰ�,
		TexFileName = std::string(fname);

		// �̹��� �����͸� �ε��Ͽ�,
		int w, h;
		GLenum format;
		unsigned char *imageData = load_image_from_tga(fname, &w, &h, &format);

		// �ؽ�ó�� �����Ѵ�.
		if (imageData != NULL)
		{
			glGenTextures(1, &BindTexId);
			glBindTexture(GL_TEXTURE_2D, BindTexId);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// or, GL_NEAREST
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// or, GL_NEAREST
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);		// or, GL_REPLACE
			glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, imageData);
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, imageData);

			free(imageData);
		}		
	}

	// Ȯ���ڰ� *.bmp ������ ���,
	if (file_ext == "bmp")
	{
		// �ؽ�ó ���ϸ��� �����ϰ�,
		TexFileName = std::string(fname);
											
		// �̹��� �����͸� �ε��Ͽ�,
		BITMAPINFO	*info;
		unsigned char *imageData = load_image_from_bmp(fname, &info);
	
		// �ؽ�ó�� �����Ѵ�.
		if (imageData != NULL)
		{
			// BGR�� RGB�� ��ȯ�Ѵ�.
			int i;
			unsigned char *ptr;
			int w = info->bmiHeader.biWidth;
			int h = info->bmiHeader.biHeight;			
			for (i = w * h, ptr = imageData; i > 0; i--, ptr += 3)
				SWAP(GLubyte, ptr[0], ptr[2])

			glGenTextures(1, &BindTexId);
			glBindTexture(GL_TEXTURE_2D, BindTexId);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// or, GL_NEAREST
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// or, GL_NEAREST
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);		// or, GL_REPLACE
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, imageData);

			free(info);
			free(imageData);
		}
	}
}

/*!
*	\brief �޽��� ���� �Ӽ��� Ȱ��ȭ �Ѵ�.
*
*	\param texture ������ �ؽ��縦 ������ ���, �ؽ�ó Ȱ��ȭ ����
*/
void GMaterial::Enable(bool texture)
{
	// ������ Ȱ��ȭ�ϰ�,
	if (Diffuse[3] != 1.0f)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	// ������ �����Ѵ�.
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, Shiness);

	// ������ �ؽ�ó�� ����� ��� �ؽ�ó ���ε��� Ȱ��ȭ �Ѵ�.
	if (texture && BindTexId != 0)
		glBindTexture(GL_TEXTURE_2D, BindTexId);
}

/*!
*	\brief �޽��� ���� �Ӽ��� ��Ȱ��ȭ �Ѵ�.
*/
void GMaterial::Disable()
{
	// ������ Ȱ��ȭ�ϰ�,
	if (Diffuse[3] != 1.0f)
		glDisable(GL_BLEND);
}

/***********************/
/* GBndBox Ŭ���� ���� */
/***********************/
/*!
*	\brief ������
*
*	\param minx ��� ������ �ּ����� x ��ǥ
*	\param miny ��� ������ �ּ����� y ��ǥ
*	\param minz ��� ������ �ּ����� z ��ǥ
*	\param maxx ��� ������ �ִ����� x ��ǥ
*	\param maxy ��� ������ �ִ����� y ��ǥ
*	\param maxz ��� ������ �ִ����� z ��ǥ
*/
GBndBox::GBndBox(double minx, double miny, double minz, double maxx, double maxy, double maxz)
{
	pMesh = NULL;
	MinPt.Set(minx, miny, minz);
	MaxPt.Set(maxx, maxy, maxz);
}

/*!
*	\brief ���� ������
*
*	\param cpy ����� ��ü
*/
GBndBox::GBndBox(const GBndBox &cpy)
{
	pMesh = cpy.pMesh;
	MinPt = cpy.MinPt;
	MaxPt = cpy.MaxPt;
}

/*!
*	\brief ���� ������
*
*	\param rhs ���Ե� ��ü
*
*	\return ���Ե� �ڽ��� ��ȯ�Ѵ�.
*/
GBndBox &GBndBox::operator =(const GBndBox &rhs)
{
	pMesh = rhs.pMesh;
	MinPt = rhs.MinPt;
	MaxPt = rhs.MaxPt;

	return *this;
}

/*!
*	\brief ���� ��ǥ�迡�� ǥ���� ��� ������ �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ǥ���� ��� ������ �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*/
GTransf GBndBox::GetMC()
{
	GVector3 t;
	GQuater q;
	
	GPoint3 p = MinPt + (MaxPt - MinPt) * 0.5;
	if (pMesh != NULL)
	{
		t = cast_vec3(pMesh->MC * p);
		q = pMesh->MC.q;
	}
	else
		t = cast_vec3(p);

	return GTransf(t, q);
}

/*!
*	\brief �������� x�� ������ �ʺ� ��ȯ�Ѵ�.
*/
double GBndBox::GetWidth()
{
	return ABS(MaxPt[0] - MinPt[0]);
}

/*!
*	\brief �������� y�� ������ ���̸� ��ȯ�Ѵ�.
*/
double GBndBox::GetHeight()
{
	return ABS(MaxPt[1] - MinPt[1]);
}

/*!
*	\brief �������� z�� ������ ���̸� ��ȯ�Ѵ�.
*/
double GBndBox::GetDepth()
{
	return ABS(MaxPt[2] - MinPt[2]);
}

/*!
*	\brief �������� �밢���� ���̸� ��ȯ�Ѵ�.
*/
double GBndBox::GetDiagonal()
{
	return dist(MinPt, MaxPt);
}

/*!
*	\brief �������� 8���� �𼭸� ������ ��ȯ�Ѵ�.
*	\note
*
*           (2) *------------* (3)
*             + |          + | 
*           +   |        +   |
*     (6) *------------* (7) |
*         |     |      |     |
*         | (0) *------|-----* (1)
*         |   +        |   +
*         | +          | +
*     (4) *------------* (5)
*
*	\param Pt �������� 8���� �𼭸� ��ġ�� ����ȴ�.
*	\param bLocal �𼭸� ������ ��ǥ��(true: �𵨸� ��ǥ��, false: ���� ��ǥ��)
*/
void GBndBox::GetCornerPts(GPoint3 *Pt, bool bLocal)
{
	Pt[0] = MinPt;
	Pt[1] = GPoint3(MaxPt[0], MinPt[1], MinPt[2]);
	Pt[2] = GPoint3(MinPt[0], MaxPt[1], MinPt[2]);
	Pt[3] = GPoint3(MaxPt[0], MaxPt[1], MinPt[2]);

	Pt[4] = GPoint3(MinPt[0], MinPt[1], MaxPt[2]);
	Pt[5] = GPoint3(MaxPt[0], MinPt[1], MaxPt[2]);
	Pt[6] = GPoint3(MinPt[0], MaxPt[1], MaxPt[2]);
	Pt[7] = MaxPt;

	if (!bLocal)
	{
		for (int i = 0; i < 8; ++i)
			Pt[i] = pMesh->MC * Pt[i];
	}
}

/*!
*	\brief ��� ���ڸ� ������ �Ѵ�.
*
*	\param r ��� ���� �׵θ��� ������ ����
*	\param g ��� ���� �׵θ��� �ʷϻ� ����
*	\param b ��� ���� �׵θ��� �Ķ��� ����
*	\param width ��� ���� �׵θ��� �β�
*/
void GBndBox::Render(double r, double g, double b, double width)
{
	// ���� ��ǥ��: ����(���)
	glPushMatrix();
	{
		// ��� ������ �𵨸� ��ǥ��� ��ȯ
		glMultMatrixd(cast_mat(pMesh->MC).M);
		
		GPoint3 p = MinPt;
		GPoint3 q = MaxPt;
		double dx = (q[0] - p[0]) * 0.25;
		double dy = (q[1] - p[1]) * 0.25;
		double dz = (q[2] - p[2]) * 0.25;

		glDisable(GL_LIGHTING);

		glColor3d(r, g, b);
		glLineWidth(width);
		glBegin(GL_LINES);
		{
			glVertex3d(p[0], p[1], p[2]);
			glVertex3d(p[0] + dx, p[1], p[2]);
			glVertex3d(p[0], p[1], p[2]);
			glVertex3d(p[0], p[1] + dy, p[2]);
			glVertex3d(p[0], p[1], p[2]);
			glVertex3d(p[0], p[1], p[2] + dz);

			glVertex3d(p[0], p[1], q[2]);
			glVertex3d(p[0] + dx, p[1], q[2]);
			glVertex3d(p[0], p[1], q[2]);
			glVertex3d(p[0], p[1] + dy, q[2]);
			glVertex3d(p[0], p[1], q[2]);
			glVertex3d(p[0], p[1], q[2] - dz);

			glVertex3d(q[0], p[1], q[2]);
			glVertex3d(q[0] - dx, p[1], q[2]);
			glVertex3d(q[0], p[1], q[2]);
			glVertex3d(q[0], p[1] + dy, q[2]);
			glVertex3d(q[0], p[1], q[2]);
			glVertex3d(q[0], p[1], q[2] - dz);

			glVertex3d(q[0], p[1], p[2]);
			glVertex3d(q[0] - dx, p[1], p[2]);
			glVertex3d(q[0], p[1], p[2]);
			glVertex3d(q[0], p[1] + dy, p[2]);
			glVertex3d(q[0], p[1], p[2]);
			glVertex3d(q[0], p[1], p[2] + dz);

			glVertex3d(q[0], q[1], q[2]);
			glVertex3d(q[0] - dx, q[1], q[2]);
			glVertex3d(q[0], q[1], q[2]);
			glVertex3d(q[0], q[1] - dy, q[2]);
			glVertex3d(q[0], q[1], q[2]);
			glVertex3d(q[0], q[1], q[2] - dz);

			glVertex3d(q[0], q[1], p[2]);
			glVertex3d(q[0] - dx, q[1], p[2]);
			glVertex3d(q[0], q[1], p[2]);
			glVertex3d(q[0], q[1] - dy, p[2]);
			glVertex3d(q[0], q[1], p[2]);
			glVertex3d(q[0], q[1], p[2] + dz);

			glVertex3d(p[0], q[1], p[2]);
			glVertex3d(p[0] + dx, q[1], p[2]);
			glVertex3d(p[0], q[1], p[2]);
			glVertex3d(p[0], q[1] - dy, p[2]);
			glVertex3d(p[0], q[1], p[2]);
			glVertex3d(p[0], q[1], p[2] + dz);

			glVertex3d(p[0], q[1], q[2]);
			glVertex3d(p[0] + dx, q[1], q[2]);
			glVertex3d(p[0], q[1], q[2]);
			glVertex3d(p[0], q[1] - dy, q[2]);
			glVertex3d(p[0], q[1], q[2]);
			glVertex3d(p[0], q[1], q[2] - dz);
			glEnd();
		}

		//glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
	}	
	glPopMatrix();
}

/*********************/
/* GMesh Ŭ���� ���� */
/*********************/
IMPLEMENT_GMESH_OBJECT(GMesh)

/*!
*	\brief ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GMesh::GetRefMesh()
{
	return this;
}

/*!
*	\brief ���� ��ǥ�迡�� ǥ���� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ǥ���� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*/
GTransf GMesh::GetTransf()
{
	return MC;
}

/*!
*	\brief ���õ� ��ü�� �����Ѵ�.
*
*	\param EC ���� ���� ��ǥ��
*	\param t ������ ��ü�� �̵� ����
*	\param q ������ ��ü�� ȸ�� ����
*	\param s ������ ��ü�� ������ ����
*	\param pParentMesh ���� ��ü�� �θ� �޽�
*/
void GMesh::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	// �θ� �޽��� ���Ͽ�, 
	if (pParentMesh == NULL)
		pParentMesh = FindMesh(pScene->MeshList, ParentMeshName);

	// �θ� �޽��� ���� ���,
	if (pParentMesh == NULL)
	{
		// �޽��� �𵨸� ��ǥ�踦 �����Ѵ�.
		GTransf EC2MC = inv(EC) * MC;
		MC = EC * GTransf(t, q) * EC2MC;
	}
	else // �θ� �޽��� �ִ� ���,
	{
		// �θ� �޽������� ����� ��ȯ�� �����Ѵ�.
		DelX = DelX * GTransf(t, q);
		MC = pParentMesh->MC * DelX;
	}
	
	// ũ�� ��ȯ�� �����Ѵٸ�,
	if (s != GVector3(1.0, 1.0, 1.0))
	{
		// �޽��� �� ������ ���Ͽ� ũ�� ��ȯ�� �����ϰ�,
		int NumVert = GetNumVert();
		for (int i = 0; i < NumVert; ++i)
		{
			Verts[i].P[0] *= s[0];
			Verts[i].P[1] *= s[1];
			Verts[i].P[2] *= s[2];
		}

		// ��� ���ڿ� ���ؼ��� ũ�� ��ȯ�� �����Ѵ�.
		BBox.MinPt[0] *= s[0];
		BBox.MinPt[1] *= s[1];
		BBox.MinPt[2] *= s[2];
		BBox.MaxPt[0] *= s[0];
		BBox.MaxPt[1] *= s[1];
		BBox.MaxPt[2] *= s[2];
		S[0] *= s[0];
		S[1] *= s[1];
		S[2] *= s[2];

		// �޽��� ��� ������ ��ġ�� ����Ǿ����� ����Ѵ�.
		ModifiedFlag = MODIFIED_VERT_POS_ALL;
	}	

	// ������ �ڽ� �޽��� �𵨸� ��ǥ�踦 �����Ѵ�.
	int NumSubMesh = (int)SubMeshList.size();
	for (int i = 0; i < NumSubMesh; ++i)
	{
		// ��, ���� ������ ���� �޽��� Forward Kinematic�� Ǫ�� �Ͱ� ����
		SubMeshList[i]->Edit(GTransf(), GVector3(), GQuater(), GVector3(1.0, 1.0, 1.0), this);
	}

	// ������ �޽��� Semantic Ÿ�Կ� ����, ��ó�� ������ �����Ѵ�.
	switch (SemanticType)
	{
		// ������ �޽��� �� �޽����,
	case MESH_BONE:
		{
			// �� �޽��� ����� ������ ��Ų �޽��� ���Ͽ�,
			int NumSkinMesh = (int)LinkedMeshNameList.size();
			for (int i = 0; i < NumSkinMesh; ++i)
			{
				// ����� ��Ų �޽��� ã��,
				GMesh *pSkinMesh = FindMesh(pScene->MeshList, LinkedMeshNameList[i]);
				if (pSkinMesh != NULL)
				{
					// ��Ų �޽��� ���ŵǾ�� ���� �˸���.
					pSkinMesh->ModifiedFlag = MODIFIED_REF_BONE;
				}
				else
				{
					// �������� �ʴ� ��Ų �޽���� �̸� ����Ʈ�� �����Ѵ�.
					LinkedMeshNameList.clear();
				}
			}
		}
		break;

		// ������ �޽���(������, Ű �ܸ�, Ű ������) �޽����,
	case MESH_PIVOT:
	case MESH_KEY_CRV:
	case MESH_KEY_FRAME:
		if (!LinkedMeshNameList.empty())
		{
			// ����� �޽��� ã��,
			GMesh *pMesh = FindMesh(pScene->MeshList, LinkedMeshNameList[0]);
			if (pMesh != NULL)
			{
				// �޽��� ���ŵǾ�� ���� ����Ѵ�.
				pMesh->ModifiedFlag = MODIFIED_SAMPLE_NUM;
			}
			else
			{
				// ����� �޽��� ���ٸ�, ���������� �����Ѵ�.
				LinkedMeshNameList.clear();
			}
		}
		break;
	}	
}

/*!
*	\brief	�޽��� �𵨸� ��ǥ�踦 �����Ѵ�.
*
*	\param RefType ����� �𵨸� ��ǥ���� Ÿ��(0: ���� ��ǥ��, 1: AABB, 2: OBB)
*/
void GMesh::UpdateMC(int RefType)
{
	if (RefType == 0)
	{
		// ���� ��ǥ�迡�� ǥ���� ������ ��ǥ�� ���ϰ�,
		int NumVert = GetNumVert();
		for (int i = 0; i < NumVert; ++i)
			Verts[i].P = MC * Verts[i].P;
		
		// �𵨸� ��ǥ�踦 ���� ��ǥ��� �����ϰ�,
		MC = GTransf();

		// ������ ������ (1.0, 1.0, 1.0)���� �ʱ�ȭ�Ѵ�.
		S.Set(1.0, 1.0, 1.0);
	}
	else if (RefType == 1)
	{
		// ���� ��ǥ�踦 �𵨸� ��ǥ��� �����ϰ�,
		UpdateMC(0);

		// ��� ������ �߽��� ��ǥ�� ���ϰ�,
		GPoint3 p = BBox.MinPt + (BBox.MaxPt - BBox.MinPt) * 0.5;

		// �޽��� �𵨸� ��ǥ�踦 �����Ѵ�.
		MC = GTransf(cast_vec3(p), GQuater());

		// ���ο� �𵨸� ��ǥ�迡�� ǥ���� ������ ��ǥ�� ����Ѵ�.
		int NumVert = GetNumVert();
		GTransf invMC = inv(MC);
		for (int i = 0; i < NumVert; ++i)
			Verts[i].P = invMC * Verts[i].P;
	}
	else if (RefType == 2)
	{
		// �ּ��� �м��� ���� ������ �����ϰ�,
		int NumVert = GetNumVert();
		GMatrix Data(NumVert, 3), PCs(3, 3);
		GVector Sigma(3), Mean(3);
		
		// ������ ������ ���Ͽ�,
		for (int i = 0; i < NumVert; ++i)
		{
			// ���� ��ǥ�迡�� ǥ���� ������ ��ǥ�� ���ϰ�,
			Verts[i].P = MC * Verts[i].P;
			
			// �� ���� ���·� ��Ŀ� �߰��� ��,
			GVector r = cast_vec(Verts[i].P);
			Data.SetRowVec(i, r);			
		}

		// �ּ��� �м��� �����Ͽ�
		pca(Data, PCs, Sigma, Mean);

		// �𵨸� ��ǥ�踦 �����Ѵ�.
		GVector3 origin = cast_vec3(Mean);
		GVector3 x_axis = cast_vec3(PCs.GetColVec(0)).Normalize();
		GVector3 y_axis = cast_vec3(PCs.GetColVec(1)).Normalize();
		GQuater q(x_axis, y_axis);
		MC = GTransf(origin, q);
		
		// �޽��� �𵨸� ��ǥ�迡�� ǥ���� ������ ��ǥ�� ����Ѵ�.
		GTransf invMC = inv(MC);
		for (int i = 0; i < NumVert; ++i)
			Verts[i].P = invMC * Verts[i].P;

		// ������ ������ (1.0, 1.0, 1.0)���� �� �����Ѵ�.
		S.Set(1.0, 1.0, 1.0);
	}

	// �θ� �޽��� ã��,
	GMesh *pParentMesh = FindMesh(pScene->MeshList, ParentMeshName);

	// �θ� �޽��� �ִ� ���,
	if (pParentMesh != NULL)
	{
		// ���� �޽��� �θ� �޽��κ����� ��� ��ȯ ������ �����ϰ�,
		DelX = inv(pParentMesh->MC) * MC;
	}	
	
	// �޽��� ������Ʈ �Ѵ�.
	ModifiedFlag = MODIFIED_VERT_POS_ALL;
	UpdateMesh();
}

/*!
*	\brief	�޽��� ������ ���뿡 ���� �޽��� �����Ѵ�.
*/
void GMesh::UpdateMesh() 
{
	//  �޽��� ������ ������ ����,
	switch (ModifiedFlag)
	{
		// �Ϻ� ������ ��ġ�� ����Ǿ��ٸ�,
	case MODIFIED_VERT_POS:
		{
			UpdateBndBox(false);
			int NumSubMesh = (int)SubMeshList.size();
			for (int i = 0; i < NumSubMesh; ++i)
				SubMeshList[i]->UpdateMesh();
		}		
		break;

		// ��� ������ ��ġ�� ����Ǿ��ٸ�,
	case MODIFIED_VERT_POS_ALL:
		{
			UpdateNormal(false);
			UpdateBndBox(false);
			UpdateBufVertex();
			UpdateBufNormal();
			int NumSubMesh = (int)SubMeshList.size();
			for (int i = 0; i < NumSubMesh; ++i)
				SubMeshList[i]->UpdateMesh();
		}
		break;

		// ������ ������ ����Ǿ��ٸ�,
	case MODIFIED_VERT_NUM:
		{
			InitBndryEdgeList();
			UpdateNormal(false);
			UpdateBndBox(false);
			UpdateBufVertex();
			UpdateBufFace();
			UpdateBufNormal();			
			int NumSubMesh = (int)SubMeshList.size();
			for (int i = 0; i < NumSubMesh; ++i)
				SubMeshList[i]->UpdateMesh();
		}
		break;

		// ������ ����Ǿ��ٸ�,
	case MODIFIED_NORMAL:
		{
			UpdateNormal(false);
			UpdateBufNormal();
			int NumSubMesh = (int)SubMeshList.size();
			for (int i = 0; i < NumSubMesh; ++i)
				SubMeshList[i]->UpdateMesh();
		}
		break;

		// ������ ����Ǿ��ٸ�,
	case MODIFIED_MTL:
		UpdateBufFace();
		break;

		// �� �޽��� ����� �޽����, ��Ű���� �����Ѵ�.
	case MODIFIED_REF_BONE:
		{
			ApplySSD();
			UpdateBndBox(false);
			UpdateBufVertex();
		}		
		break;
	}

	// �޽��� ���� ������ ó�� �Ǿ����� ����Ѵ�.
	ModifiedFlag = MODIFIED_NONE;
}

/*!
*	\brief ���õ� ��ü�� ������ �Ѵ�.
*/
void GMesh::RenderForEdit(float r, float g, float b)
{
	// �� �޽����,
	if (SemanticType == MESH_BONE)
	{
		glDisable(GL_DEPTH_TEST);
		// ���� ��ǥ��: ����(���)
		glPushMatrix();
		{
			// �޽��� �𵨸� ��ǥ��� ��ȯ
			glMultMatrixd(cast_mat(MC).M);
			RenderMeshWire(r, g, b, 1.0);
		}
		glPopMatrix();
		glEnable(GL_DEPTH_TEST);

		int NumSubMesh = (int)SubMeshList.size();
		for (int i = 0; i < NumSubMesh; ++i)
			SubMeshList[i]->RenderForEdit(0.0, 0.0, 1.0);
	}
	else	// �Ϲ� �޽����,
	{
		BBox.Render(r, g, b, 1.0);

		int NumSubMesh = (int)SubMeshList.size();
		for (int i = 0; i < NumSubMesh; ++i)
			SubMeshList[i]->RenderForEdit(1.0, 1.0, 1.0);
	}
}

/*!
*	\brief �޽��� ������
*
*	\param _Name ������ �޽��� �̸�
*	\param _MC ��ü�� ���ǵǴ� �𵨸� ��ǥ��
*	\param _FaceType �޽��� ������ Ÿ��(FACE_TRI, FACE_QUAD, FACE_TRI_FAN)
*	\param _ParentName ������ �޽��� ����� �θ� �޽��� �̸�
*/
GMesh::GMesh(const std::string &_Name, GTransf _MC, TypeMeshFace _FaceType, const std::string &_ParentName)
{
	// �⺻ ������ ���(19)
	Name = _Name;						// �޽��� �̸�
	MC = _MC;							// �޽��� ���ǵ� �𵨸� ��ǥ��
	S = GVector3(1.0, 1.0, 1.0);		// �޽��� ũ�� ��ȯ ����
	FaceType = _FaceType;				// �޽��� ������ Ÿ��(�ﰢ��, �簢��) ����
	Verts.clear();						// �޽��� �����ϴ� ������ �迭
	Texs.clear();						// �޽��� �����ϴ� �ؽ�ó ������ �迭
	Faces.clear();						// �޽��� �����ϴ� �ﰢ���� �迭
	GrpFaces.clear();					// �׷��ε� �޽��� �ﰢ�� �ε��� �迭
	BBox.pMesh = this;					// �޽��� �ѷ��δ� ��� ������ �� ����
	pBVH = NULL;						// �޽��� ��� ���� ���� ����
	CollisionFaces.clear();				// �浹�� ���� �ﰢ���� �ε��� ����Ʈ
	Renderer = GRenderer();				// �޽��� ������
	MtlList.clear();					// �޽��� ����ϴ� ���� ����Ʈ
	DispTexMesh = 0;					// �ؽ�ó ���ε� �޽��� ���÷��� ����Ʈ
	DispVertColMesh = 0;				// ���� Į�� �������� ���� ���÷��� ����Ʈ
	DispFaceColMesh = 0;				// �ﰢ�� �÷� �������� ���� ���÷��� ����Ʈ */
	VtxBuf.clear();						// ���� ���۸� �ʱ�ȭ
	NorBuf.clear();						// ���� ���۸� �ʱ�ȭ
	FaceIdxBuf.clear();					// ������ �ε��� ���۸� �ʱ�ȭ
	
	// �߰� ������ ���(11)
	GScene *pScene = NULL;				// �޽��� ���Ե� ��鿡 ���� ������
	ParentMeshName = _ParentName;		// �޽��� ����� �θ� �޽��� �̸�
	DelX = GTransf();					// �θ� �޽����κ����� ��ȯ (MC = �θ� MC * DelX)
	SubMeshList.clear();				// �޽��� ����� �ڽ� �޽� ����Ʈ
	SemanticType = MESH_STD;			// �޽��� �ø�ƽ Ÿ�� ����
	LinkedMeshNameList.clear();			// �޽��� �ø�ƽ Ÿ�Կ� ���� ����� �޽��� �̸� ����Ʈ
	ClipPlaneNameList.clear();			// ���� ����� �̸� ����Ʈ
	ModifiedFlag = MODIFIED_NONE;		// �޽��� ������Ʈ ����
	BndryEdgeList.clear();				// �޽��� ��� ������ ����Ʈ
	Animation.clear();					// �ִϸ��̼��� ���� ��ȯ ����
	FrameIdx = 0;						// �ִϸ��̼��� ���� ������ �ε���
}

/*!
*	\brief �޽��� ���� ������
*
*	\param cpy ����� ��ü
*/
GMesh::GMesh(const GMesh &cpy)
{
	// �⺻ ������ ���(19)
	Name = cpy.Name;								// �޽��� �̸�
	MC = cpy.MC;									// �޽��� ���ǵ� �𵨸� ��ǥ��
	S = cpy.S;										// �޽��� ũ�� ��ȯ ����
	FaceType = cpy.FaceType;						// �޽��� ������ Ÿ��(�ﰢ��, �簢��) ����
	Verts = cpy.Verts;								// �޽��� �����ϴ� ������ �迭
	Texs = cpy.Texs;								// �޽��� �����ϴ� �ؽ�ó ������ �迭
	Faces = cpy.Faces;								// �޽��� �����ϴ� �ﰢ���� �迭
	GrpFaces = cpy.GrpFaces;						// �׷��ε� �޽��� �ﰢ�� �ε��� �迭
	BBox = cpy.BBox; BBox.pMesh = this;				// �޽��� �ѷ��δ� ��� ������ �� ����
	pBVH = NULL;
	CollisionFaces.clear();							// �浹�� ���� �ﰢ���� �ε��� ����Ʈ
	Renderer = cpy.Renderer;						// �޽��� ������ �Ӽ�
	MtlList = cpy.MtlList;							// �޽��� ����ϴ� ���� ����Ʈ
	DispTexMesh = cpy.DispTexMesh;					// �ؽ�ó ���ε� �޽��� ���÷��� ����Ʈ
	DispVertColMesh = cpy.DispVertColMesh;			// �÷� ���� �������� ���� ���÷��� ����Ʈ
	DispFaceColMesh = cpy.DispFaceColMesh;			// �ﰢ�� �÷� �������� ���� ���÷��� ����Ʈ
	VtxBuf = cpy.VtxBuf;							// ���� ���۸� �ʱ�ȭ
	NorBuf = cpy.NorBuf;							// ���� ���۸� �ʱ�ȭ
	FaceIdxBuf = cpy.FaceIdxBuf;					// ������ �ε��� ���۸� �ʱ�ȭ

	
	// �߰� ������ ���(11)
	pScene = cpy.pScene;							// �޽��� ���Ե� ��鿡 ���� ������
	ParentMeshName = cpy.ParentMeshName;			// �޽��� ����� �θ� �޽��� �̸�
	DelX = cpy.DelX;								// �θ� �޽����κ����� ��ȯ (MC = �θ� MC * DelX)
	SemanticType = cpy.SemanticType;				// �޽��� �ø�ƽ Ÿ�� ����
	LinkedMeshNameList = cpy.LinkedMeshNameList;	// �޽��� �ø�ƽ Ÿ�Կ� ���� ����� �޽��� �̸� ����Ʈ
	ClipPlaneNameList = cpy.ClipPlaneNameList;		// ���� ����� �̸� ����Ʈ
	ModifiedFlag = cpy.ModifiedFlag;				// �޽��� ������Ʈ ����
	BndryEdgeList = cpy.BndryEdgeList;				// �޽��� ��� ������ ����Ʈ
	Animation = cpy.Animation;						// �ִϸ��̼��� ���� ��ȯ ����
	FrameIdx = cpy.FrameIdx;						// �ִϸ��̼��� ���� ������ �ε���
	if (!cpy.SubMeshList.empty())					// ���� �޽��� ������ ����
	{
		int NumSubMesh = (int)cpy.SubMeshList.size();
		for (int i = 0; i < NumSubMesh; ++i)
		{
			GMesh *pMesh = new GMesh((*cpy.SubMeshList[i]));
			SubMeshList.push_back(pMesh);
		}
	}
}

/*!
*	\brief �޽��� �Ҹ���
*/
GMesh::~GMesh()
{
	int NumSubMesh = (int)SubMeshList.size();
	for (int i = 0; i < NumSubMesh; ++i)
		delete SubMeshList[i];

	if (DispTexMesh != 0)
		glDeleteLists(DispTexMesh, 1);

	if (DispVertColMesh != 0)
		glDeleteLists(DispVertColMesh, 1);

	if (DispFaceColMesh != 0)
		glDeleteLists(DispFaceColMesh, 1);

	if (pBVH != NULL)
		delete pBVH;
}

/*!
*	\brief �޽��� ���Կ�����
*
*	\param rhs ���Ե� �޽� ��ü
*
*	\return ���Ե� �ڽ��� ��ȯ�Ѵ�.
*/
GMesh &GMesh::operator =(const GMesh &rhs)
{
	// �⺻ ������ ���(19)
	Name = rhs.Name;								// �޽��� �̸�
	MC = rhs.MC;									// �޽��� ���ǵ� �𵨸� ��ǥ��
	S = rhs.S;										// �޽��� ũ�� ��ȯ ����
	FaceType = rhs.FaceType;						// �޽��� ������ Ÿ��(�ﰢ��, �簢��) ����
	Verts = rhs.Verts;								// �޽��� �����ϴ� ������ �迭
	Texs = rhs.Texs;								// �޽��� �����ϴ� �ؽ�ó ������ �迭
	Faces = rhs.Faces;								// �޽��� �����ϴ� �ﰢ���� �迭
	GrpFaces = rhs.GrpFaces;						// �׷��ε� �޽��� �ﰢ�� �ε��� �迭
	BBox = rhs.BBox; BBox.pMesh = this;				// �޽��� �ѷ��δ� ��� ������ �� ����
	pBVH = NULL;
	CollisionFaces.clear();							// �浹�� ���� �ﰢ���� �ε��� ����Ʈ
	Renderer = rhs.Renderer;						// �޽��� ������ �Ӽ�
	MtlList = rhs.MtlList;							// �޽��� ����ϴ� ���� ����Ʈ
	DispTexMesh = rhs.DispTexMesh;					// �ؽ�ó ���ε� �޽��� ���÷��� ����Ʈ
	DispVertColMesh = rhs.DispVertColMesh;			// �÷� ���� �������� ���� ���÷��� ����Ʈ
	DispFaceColMesh = rhs.DispFaceColMesh;			// �ﰢ�� �÷� �������� ���� ���÷��� ����Ʈ
	VtxBuf = rhs.VtxBuf;							// ���� ���۸� �ʱ�ȭ
	NorBuf = rhs.NorBuf;							// ���� ���۸� �ʱ�ȭ
	FaceIdxBuf = rhs.FaceIdxBuf;					// ������ �ε��� ���۸� �ʱ�ȭ

	// �߰� ������ ���(11)
	pScene = rhs.pScene;							// �޽��� ���Ե� ��鿡 ���� ������
	ParentMeshName = rhs.ParentMeshName;			// �޽��� ����� �θ� �޽��� �̸�
	DelX = rhs.DelX;								// �θ� �޽����κ����� ��ȯ (MC = �θ� MC * DelX)
	SemanticType = rhs.SemanticType;				// �޽��� �ø�ƽ Ÿ�� ����
	LinkedMeshNameList = rhs.LinkedMeshNameList;	// �޽��� �ø�ƽ Ÿ�Կ� ���� ����� �޽��� �̸� ����Ʈ
	ClipPlaneNameList = rhs.ClipPlaneNameList;		// ���� ����� �̸� ����Ʈ
	ModifiedFlag = rhs.ModifiedFlag;				// �޽��� ������Ʈ ����
	BndryEdgeList = rhs.BndryEdgeList;				// �޽��� ��� ������ ����Ʈ
	Animation = rhs.Animation;						// �ִϸ��̼��� ���� ��ȯ ����
	FrameIdx = rhs.FrameIdx;						// �ִϸ��̼��� ���� ������ �ε���
	if (!rhs.SubMeshList.empty())					// ���� �޽��� ������ ����
	{
		// ���� �޽� ����Ʈ �����
		int NumSubMesh = (int)SubMeshList.size();
		for (int i = 0; i < NumSubMesh; ++i)
			delete SubMeshList[i];
		SubMeshList.clear();

		// ���� �޽� ����Ʈ �����ϱ�
		NumSubMesh = (int)rhs.SubMeshList.size();
		for (int i = 0; i < NumSubMesh; ++i)
		{
			GMesh *pMesh = new GMesh((*rhs.SubMeshList[i]));
			SubMeshList.push_back(pMesh);
		}
	}
	
	return *this;
}

/*!
*	\brief �޽��� ������ �߰��Ѵ�.
*
*	\param v �߰��� ����
*/
void GMesh::AddVertex(GVertex v)
{
	v.pMesh = this;
	v.Idx = GetNumVert();
	Verts.push_back(v);
}

/*!
*	\brief �޽��� �ﰢ���� �߰��Ѵ�.
*
*	\param f �߰��� �ﰢ��
*	\param GroupName �ﰢ���� �׷� �̸�
*/
void GMesh::AddFace(GFace f, std::string GroupName)
{
	f.pMesh = this;
	f.Idx = GetNumFace();
	Faces.push_back(f);

	if (!GroupName.empty())
		GrpFaces[GroupName].push_back(f.Idx);
}

/*!
*	\brief �޽��� �ؽ�ó ������ �߰��Ѵ�.
*
*	\param t �߰��� �ؽ�ó ����
*/
void GMesh::AddTexel(GVector2 t)
{
	Texs.push_back(t);
}

/*!
*	\brief �޽��� ������ �߰��Ѵ�.
*
*	\param mtl �߰��� ����
*/
void GMesh::AddMtl(GMaterial mtl)
{
	MtlList.push_back(mtl);
}

/*!
*	\brief �޽��� 6���� ���ܸ��� �߰��Ѵ�.
*/
void GMesh::AddClipPlanes()
{
	// ���� ��ǥ�迡�� ǥ���� ��� ������ �ڳ� ������ ��ġ�� ���ϰ�,
	GPoint3 P[8];
	BBox.GetCornerPts(P, false);

	// 6���� ���� ����� ��ȯ�� �����Ѵ�.
	double w = BBox.GetWidth();
	double h = BBox.GetHeight();
	double d = BBox.GetDepth();
	GTransf dX[6] = {
		GTransf(GVector3(-w / 2.0, 0.0, 0.0), GQuater(GVector3(0.0, 0.0, 1.0), -90.0)),
		GTransf(GVector3(w / 2.0, 0.0, 0.0), GQuater(GVector3(0.0, 0.0, 1.0), 90.0)), 
		GTransf(GVector3(0.0, -h / 2.0, 0.0), GQuater()),
		GTransf(GVector3(0.0, h / 2.0, 0.0), GQuater(GVector3(0.0, 0.0, 1.0), 180.0)),
		GTransf(GVector3(0.0, 0.0, d / 2.0), GQuater(GVector3(1.0, 0.0, 0.0), -90.0)),
		GTransf(GVector3(0.0, 0.0, -d / 2.0), GQuater(GVector3(1.0, 0.0, 0.0), 90.0))
	};

	// 6���� ���� ����� �����Ѵ�.
	int vidx[] = {0, 2, 6, 4, 1, 5, 7, 3, 0, 4, 5, 1, 2, 3, 7, 6, 4, 6, 7, 5, 0, 1, 3, 2};
	for (int i = 0; i < 6; ++i)
	{
		GMesh *pPlane = new GMesh("", GTransf(), FACE_QUAD, Name);
		pPlane->Name = ::create_mesh_name("Clip Plane");
		pPlane->Renderer.RenderType = RENDER_WIRE;
		pPlane->MC = BBox.GetMC() * dX[i];

		// ������ �߰��Ѵ�.
		for (int j = 0; j < 4; ++j)
		{
			GPoint3 p = inv(pPlane->MC) * P[vidx[4 * i + j]];
			pPlane->AddVertex(GVertex(p, GVector3()));
		}

		// �ﰢ���� �߰��Ѵ�.
		pPlane->AddFace(GFace(0, 1, 2, 0, 1, 2, -1));
		pPlane->AddFace(GFace(0, 2, 3, 0, 2, 3, -1));

		// ���ܸ��� ��鿡 �߰��Ѵ�.
		pScene->AddMesh(pPlane);
		ClipPlaneNameList.push_back(pPlane->Name);
	}
}

/*!
*	\brief �޽��� �ﰢ���� �����Ѵ�.
*
*	\param ������ �ﰢ���� �ε���
*/
void GMesh::DelFaces(std::vector<int> &FaceList)
{
	//////////////////////////////////////
	// 1�ܰ�: ���õ� �ﰢ���� �����Ѵ�. //
	//////////////////////////////////////	

	// ������ �ﰢ���� DelFaceFlag �迭�� true�� ����Ѵ�.
	std::vector<bool> DelFaceFlag(GetNumFace(), false);
	for (unsigned i = 0; i < FaceList.size(); ++i)
		DelFaceFlag[FaceList[i]] = true;

	// ���� �ﰢ���� ����Ʈ�� �ﰢ�� �ε��� ���� �����Ѵ�.
	std::map<int, int> FaceIdxMap;
	std::vector<GFace> SavedFaces = Faces;
	Faces.clear();
	for (unsigned i = 0; i < SavedFaces.size(); ++i)
	{
		// ���� �ﰢ���̶��, 
		if (!DelFaceFlag[i])
		{
			// �߰��ϰ� (���� �ε���, ���ο� �ε��� + 1)�� ����Ѵ�.
			AddFace(SavedFaces[i]);
			FaceIdxMap[i] = GetNumFace();
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// 2�ܰ�: ������ �ʴ� �������� �����ϰ�, �ﰢ���� �ε����� �����Ѵ�. //
	/////////////////////////////////////////////////////////////////////////	

	// ������ ������ DelVertFlag �迭�� ����Ѵ�.
	std::vector<bool> DelVertFlag(GetNumVert(), true);
	for (int i = 0; i < GetNumFace(); ++i)
	{
		int *vidx = Faces[i].vIdx;
		DelVertFlag[vidx[0]] = false;
		DelVertFlag[vidx[1]] = false;
		DelVertFlag[vidx[2]] = false;
	}

	// ���Ǵ� ���� ����Ʈ�� ���� �ε��� (���� �ε���, ���ο� �ε��� + 1) ���� �����Ѵ�.
	std::map<int, int> VertIdxMap;
	std::vector<GVertex> SavedVerts = Verts;
	Verts.clear();
	for (unsigned i = 0; i < SavedVerts.size(); ++i)
	{
		// ���� �����̶��, 
		if (!DelVertFlag[i])
		{
			// ���� ����Ʈ�� �߰��ϰ� (���� �ε���, ���ο� �ε��� + 1)�� ����Ѵ�.
			AddVertex(SavedVerts[i]);
			VertIdxMap[i] = GetNumVert();
		}
	}

	// ���� �ﰢ���� ������ �ε����� �����Ѵ�.
	for (int i = 0; i < GetNumFace(); ++i)
	{
		Faces[i].vIdx[0] = VertIdxMap[Faces[i].vIdx[0]] - 1;
		Faces[i].vIdx[1] = VertIdxMap[Faces[i].vIdx[1]] - 1;
		Faces[i].vIdx[2] = VertIdxMap[Faces[i].vIdx[2]] - 1;
	}

	/////////////////////////////////////////////
	// 3�ܰ�: �ﰢ�� �׷��� �ε����� �����Ѵ�. //
	/////////////////////////////////////////////
	std::map<std::string, std::vector<int>>::iterator it_group_name;
	for (it_group_name = GrpFaces.begin(); it_group_name != GrpFaces.end(); ++it_group_name)
	{
		std::vector<int> OldFaceIdx = it_group_name->second;
		it_group_name->second.clear();
		std::vector<int>::iterator it_face_idx;
		for (it_face_idx = OldFaceIdx.begin(); it_face_idx != OldFaceIdx.end(); ++it_face_idx)
		{
			if (FaceIdxMap[*it_face_idx] != 0)
				it_group_name->second.push_back(FaceIdxMap[*it_face_idx] - 1);
		}
	}

	// ���������� ������� ����ϰ�, �����ڸ� �����Ѵ�.
	ModifiedFlag = MODIFIED_VERT_NUM;
	UpdateMesh();
}

/*!
*	\brief �޽��� ������ �����Ѵ�.
*
*	\param ������ ������ �ε���
*/
void GMesh::DelVerts(std::vector<int> &VertList)
{
	/////////////////////////////////////
	// 1 �ܰ�: ���õ� ������ �����Ѵ�. //
	/////////////////////////////////////	

	// ������ ������ SltVertFlag �迭�� ����Ѵ�.
	std::vector<bool> SltVertFlag(GetNumVert(), false);
	for (unsigned i = 0; i < VertList.size(); ++i)
		SltVertFlag[VertList[i]] = true;

	std::vector<GVertex> SavedVerts = Verts;
	Verts.clear();

	// ���� ���� ����Ʈ�� ���� �ε��� ��(���� �ε���, ���ο� �ε��� + 1)�� �����Ѵ�.
	std::map<int, int> VertIdxMap;
	for (unsigned i = 0; i < SavedVerts.size(); ++i)
	{
		// ���� �����̶��,
		if (!SltVertFlag[i])
		{
			// �޽��� �߰��ϰ�, (���� �ε���, ���ο� �ε��� + 1)�� ����Ѵ�.
			AddVertex(SavedVerts[i]);
			VertIdxMap[i] = GetNumVert();
		}
	}

	///////////////////////////////////////////////////////////////////////
	// 2�ܰ�: ���� �������� ���ο� �ﰢ�� ����Ʈ�� �ε��� ���� �����Ѵ�. //
	///////////////////////////////////////////////////////////////////////
	std::vector<GFace> SavedFaces = Faces;
	Faces.clear();	

	std::map<int, int> FaceIdxMap;
	for (unsigned i = 0; i < SavedFaces.size(); ++i)
	{
		// �ﰢ���� ���� �ε����� ���Ͽ�, 
		int vidx0 = VertIdxMap[SavedFaces[i].vIdx[0]] - 1;
		int vidx1 = VertIdxMap[SavedFaces[i].vIdx[1]] - 1;
		int vidx2 = VertIdxMap[SavedFaces[i].vIdx[2]] - 1;

		// ���� �ﰢ���̶��,
		if (vidx0 != -1 && vidx1 != -1 && vidx2 != -1)
		{
			// �ﰢ�� ����Ʈ�� �߰��ϰ�,
			int *tidx = SavedFaces[i].tIdx;
			GFace f(vidx0, vidx1, vidx2, tidx[0], tidx[1], tidx[2], SavedFaces[i].MtlIdx);
			AddFace(f);

			// (���� �ε���, ���ο� �ε��� + 1)�� ����Ѵ�.
			FaceIdxMap[i] = GetNumFace();
		}
	}

	/////////////////////////////////////////////
	// 3�ܰ�: �ﰢ�� �׷��� �ε����� �����Ѵ�. //
	/////////////////////////////////////////////
	std::map<std::string, std::vector<int>>::iterator it_group_name;
	for (it_group_name = GrpFaces.begin(); it_group_name != GrpFaces.end(); ++it_group_name)
	{
		// �׷��� ���� �ﰢ�� �ε����� �����ϰ�,
		std::vector<int> SavedFaceIdx = it_group_name->second;
		it_group_name->second.clear();

		// �׷��� ���ο� �ﰢ�� �ε����� �����Ѵ�.
		std::vector<int>::iterator it_face_idx;
		for (it_face_idx = SavedFaceIdx.begin(); it_face_idx != SavedFaceIdx.end(); ++it_face_idx)
		{
			// ���� �ﰢ���� �ε����� �߰��Ѵ�.
			if (FaceIdxMap[*it_face_idx] != 0)
				it_group_name->second.push_back(FaceIdxMap[*it_face_idx] - 1);
		}
	}

	// ���������� ������� ����ϰ� �޽��� �����Ѵ�.
	ModifiedFlag = MODIFIED_VERT_NUM;
	UpdateMesh();
}

/*!
*	\brief �޽��� ������ ������ ��ȯ�Ѵ�.
*
*	\return �޽��� �����ϴ� ������ ����
*/
int GMesh::GetNumVert() 
{ 
	return (int)Verts.size(); 
}

/*!
*	\brief �޽��� �ﰢ���� ������ ��ȯ�Ѵ�.
*
*	\return �޽��� �����ϴ� �ﰢ���� ����
*/
int GMesh::GetNumFace() 
{ 
	return (int)Faces.size(); 
}

/*!
*	\brief �޽��� �ؽ�ó ������ ������ ��ȯ�Ѵ�.
*
*	\return �޽��� �ؽ�ó ������ ����
*/
int GMesh::GetNumTex()	
{ 
	return (int)Texs.size(); 
}

/*!
*	\brief �޽��� ������ ������ ��ȯ�Ѵ�.
*
*	\return �޽��� ���Ե� ������ ����
*/
int GMesh::GetNumMtl() 
{ 
	return (int)MtlList.size(); 
}

/*!
*	\brief �־��� ������ ������ ��ġ�� ���� ������ �ε����� ���Ѵ�.
*
*	\param pVert �־��� ����
*
*	\return pVert�� ������ ��ġ�� ���� ������ �ε��� (������ -1)�� ��ȯ�Ѵ�.
*/
int GMesh::GetVertIdx(GVertex *pVert)
{
	int NumVert = GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		if (pVert->P == Verts[i].P)
			return i;
	}

	return -1;
}

/*!
*	\brief �ﰢ�� ����Ʈ���� �־��� �ﰢ���� �ε����� ���Ѵ�.
*
*	\param pFace �־��� �ﰢ��
*
*	\return �ﰢ�� ����Ʈ���� pFace�� �ε��� (������ -1)�� ��ȯ�Ѵ�.
*/
int GMesh::GetFaceIdx(GFace *pFace)
{
	int vidx0 = pFace->vIdx[0];
	int vidx1 = pFace->vIdx[1];
	int vidx2 = pFace->vIdx[2];

	int NumFace = GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		if (Faces[i].vIdx[0] == vidx0 && Faces[i].vIdx[1] == vidx1 && Faces[i].vIdx[2] == vidx2)
			return i;
	}

	return -1;
}

/*!
*	\brief �־��� ������ �����Ǵ� �ؽ�ó�� ��ǥ�� �ε����� ��ȯ�Ѵ�.
*
*	\param pVert �־��� ����
*
*	\return �־��� ������ �����Ǵ� �ؽ�ó�� ��ǥ�� �ε����� ��ȯ�Ѵ�.
*/
int GMesh::GetTexIdx(GVertex *pVert)
{
	int tIdx = -1;
	int vIdx = pVert->Idx;
	int NumFace = GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		int *vidx = Faces[i].vIdx;

		if (vIdx == vidx[0])
		{
			tIdx = Faces[i].tIdx[0];
			break;			
		}

		if (vIdx == vidx[1])
		{
			tIdx = Faces[i].tIdx[1];
			break;
		}

		if (vIdx == vidx[2])
		{
			tIdx = Faces[i].tIdx[2];
			break;
		}
	}

	return tIdx;
}

/*!
*	\brief ���� ����Ʈ���� �̸��� \a Name�� ������ ã�� �ε����� ��ȯ�Ѵ�.
*
*	\param Name ã�� ������ �̸�
*
*	\return ������ �ε����� ��ȯ�Ѵ� (�ش� �̸��� ������ ���� ���, -1�� ��ȯ)
*/
int GMesh::GetMtlIdx(std::string Name)
{
	int MtlIdx = -1;
	int NumMtl = (int)MtlList.size();

	for (int i = 0; i < NumMtl; ++i)
	{
		if (MtlList[i].Name == Name)
			MtlIdx = i;
	}

	return MtlIdx;
}

/*!
*	\brief ��ü�� ������ �����Ѵ�.
*	\note ��ü�� ��鿡 �߰��� �Ŀ� ȣ���ؾ� �Ѵ�.
*
*	\param MtlName ��ü�� ���� ����Ʈ�� �����ϴ� ������ �̸�
*/
void GMesh::SetMtl(std::string MtlName)
{
	int MtlIdx = GetMtlIdx(MtlName);
	if (MtlIdx != -1)
	{
		// ��ü �ﰢ���� ���Ͽ� ������ �����Ѵ�.
		int NumFace = GetNumFace();
		for (int i = 0; i < NumFace; ++i)
			Faces[i].MtlIdx = MtlIdx;
	}

	// �ﰢ���� ���� � ��ü�� ���Ͽ� ������ �����Ѵ�.
	Renderer.CrvMtlIdx = MtlIdx;
	ModifiedFlag = MODIFIED_MTL;
	UpdateMesh();
}

/*!
*	\brief �޽��� ���ο� �ﰢ�� ����Ʈ�� �����Ѵ�.
*
*	\param NewFaces ���ο� �ﰢ�� ����Ʈ
*/
void GMesh::SetNewFaces(std::vector<GFace> &NewFaces)
{
	for (unsigned i = 0; i < NewFaces.size(); ++i)
	{
		NewFaces[i].Idx = i;
		NewFaces[i].pMesh = this;		
	}
	Faces = NewFaces;
}

/*!
*	\brief (sidx, edix)�� ������ �ε����� ���� �ﰢ���� ã�´�.
*
*	\param sidx ���� ������ �ε���
*	\param eidx �� ������ �ε���
*
*	\param (sidx, edix)�� ������ �ε����� ���� �ﰢ���� �����͸� ��ȯ�Ѵ�.
*/
GFace *GMesh::FindFace(int sidx, int eidx)
{
	for (unsigned i = 0; i < Faces.size(); ++i)
	{
		int *vidx = Faces[i].vIdx;
		if ((vidx[0] == sidx && vidx[1] == eidx) ||
			(vidx[1] == sidx && vidx[2] == eidx) ||
			(vidx[2] == sidx && vidx[0] == eidx))
			return &Faces[i];
	}

	return NULL;
}

/*!
*	\brief ���� ����Ʈ���� �̸��� \a Name�� ������ ã�� �����͸� ��ȯ�Ѵ�.
*
*	\param Name ã�� ������ �̸�
*
*	\return ������ ������ (�ش� �̸��� ������ ���� ���, ����Ʈ ������ �����͸� ��ȯ)
*/
GMaterial *GMesh::FindMtl(std::string Name)
{
	static GMaterial mtl_default;

	int NumMtl = (int)MtlList.size();
	for (int i = 0; i < NumMtl; ++i)
	{
		if (MtlList[i].Name == Name)
		{
			return &MtlList[i];
		}
	}

	return &mtl_default;
}

/*!
*	\brief �޽� �ﰢ���� ��/�� ���� �����´�.
*/
void GMesh::ReverseNormal()
{
	int NumFace = GetNumFace();
	if (FaceType == FACE_QUAD)
	{
		for (int i = 0; i < NumFace; i += 2)
		{
			SWAP(int, Faces[i].vIdx[0], Faces[i].vIdx[2]);
			SWAP(int, Faces[i].tIdx[0], Faces[i].tIdx[2]);
			SWAP(int, Faces[i + 1].vIdx[0], Faces[i + 1].vIdx[1]);
			SWAP(int, Faces[i + 1].tIdx[0], Faces[i + 1].tIdx[1]);
		}
	}
	else
	{
		for (int i = 0; i < NumFace; ++i)
		{
			SWAP(int, Faces[i].vIdx[1], Faces[i].vIdx[2]);
			SWAP(int, Faces[i].tIdx[1], Faces[i].tIdx[2]);
		}
	}

	ModifiedFlag = MODIFIED_VERT_NUM;
	UpdateMesh();
}

/*!
*	\brief �޽� ������ �������͸� �����Ѵ�.
*
*	\param all �ڽ� �޽��� ���� ����
*/
void GMesh::UpdateNormal(bool all)
{
	int NumVert = GetNumVert();
	for (int i = 0; i < NumVert; ++i)
		Verts[i].N.Set(0.0, 0.0, 0.0);
	
	int NumFace = GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		int *vidx = Faces[i].vIdx;

// 		GPoint3 p = Verts[vidx[0]].P;
// 		GPoint3 q = Verts[vidx[1]].P;
// 		GPoint3 r = Verts[vidx[2]].P;
// 		double alpha = angle(q - p, r - p, true);
// 		double beta = angle(r - q, p - q, true);
// 		double gamma = angle(p - r, q - r, true);
// 		GVector3 n = ((q - p) ^ (r - p)).Normalize();
// 		Verts[vidx[0]].N += n * alpha / M_PI;
// 		Verts[vidx[1]].N += n * beta / M_PI;
// 		Verts[vidx[2]].N += n * gamma / M_PI;
		
		GVector3 p, q, n;
		p = Verts[vidx[1]].P - Verts[vidx[0]].P;
		q = Verts[vidx[2]].P - Verts[vidx[0]].P;
		n = (p ^ q).Normalize();

		Verts[vidx[0]].N += n;
		Verts[vidx[1]].N += n;
		Verts[vidx[2]].N += n;
	}
	
	for (int i = 0; i < NumVert; ++i)
		Verts[i].N.Normalize();

	if (all)
	{
		int NumSubMesh = (int)SubMeshList.size();
		for (int i = 0; i < NumSubMesh; ++i)
			SubMeshList[i]->UpdateNormal(all);
	}	
}

/*!
*	\brief �޽��� �ѷ��δ� �����ڸ� �����Ѵ�.
*
*	\param all �ڽ� �޽��� ���� ����
*/
void GMesh::UpdateBndBox(bool all)
{
	if (Verts.empty())
		return;

	BBox.MinPt = BBox.MaxPt = Verts[0].P;
	int NumVert = GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		double *p = Verts[i].P.V;
		BBox.MinPt[0] = MIN(BBox.MinPt[0], p[0]);
		BBox.MinPt[1] = MIN(BBox.MinPt[1], p[1]);
		BBox.MinPt[2] = MIN(BBox.MinPt[2], p[2]);

		BBox.MaxPt[0] = MAX(BBox.MaxPt[0], p[0]);
		BBox.MaxPt[1] = MAX(BBox.MaxPt[1], p[1]);
		BBox.MaxPt[2] = MAX(BBox.MaxPt[2], p[2]);
	}
	
	if (all)
	{
		int NumSubMesh = (int)SubMeshList.size();
		for (int i = 0; i < NumSubMesh; ++i)
			SubMeshList[i]->UpdateBndBox(all);
	}
}

/*!
*	\brief ������ ���ε� ������ ������Ʈ �Ѵ�.
*/
void GMesh::InitRefBoneInfo()
{
	// ��Ų �޽��� �ƴ϶�� �����Ѵ�.
	if (SemanticType != MESH_SKIN)
		return;

	// ��Ų �޽���� ������ ������ ���ϰ�,
	int NumVert = GetNumVert();

	// ������ ������ ���Ͽ�	
	for (int i = 0; i < NumVert; ++i)
	{
		// ���ε� ������ ���ͼ�,
		std::vector<GRefBoneInfo> &InfoList = Verts[i].BoneInfoList;

		// ���ε� ������ ���ٸ� ���� ������ ó���ϰ�,
		if (InfoList.empty())
			continue;
		
		// ���ε� ������ �ִٸ�, 
		std::vector<GRefBoneInfo>::iterator it = InfoList.begin();

		// ������ ���ε� ������ ���Ͽ�,
		while (it != InfoList.end())
		{
			// ����� �� �޽��� ã�Ƽ�,
			GMesh *pBoneMesh = FindMesh(pScene->MeshList, it->BoneMeshName);

			// ����� �� �޽��� �ִٸ�,
			if (pBoneMesh != NULL)
			{
				// ���ε� ������ �ʱ�ȭ �ϰ�,
				it->pRefMC = &(pBoneMesh->MC);
				it->P = inv(pBoneMesh->MC) * MC * Verts[i].P;
				it->N = inv(pBoneMesh->MC) * MC * Verts[i].N;

				// �� �޽��� ��Ų �޽� �̸��� �߰��Ѵ�.
				if (pBoneMesh->LinkedMeshNameList.end() == find(pBoneMesh->LinkedMeshNameList.begin(), pBoneMesh->LinkedMeshNameList.end(), Name))
					pBoneMesh->LinkedMeshNameList.push_back(Name);
				
			}
			// ����� �� �޽��� ���ٸ�,
			else
			{
				// �ش� ���ε� ������ �����Ѵ�.
				it = InfoList.erase(it);
				continue;
			}
			it++;
		}
	}

	// ��� ���� �޽��� ���Ͽ� �۾��� �ݺ��Ѵ�.
	int NumSubMesh = (int)SubMeshList.size();
	for (int i = 0; i < NumSubMesh; ++i)
		SubMeshList[i]->InitRefBoneInfo();
}

/*!
*	\brief �޽��� �ִϸ��̼� ������ �̿��Ͽ� ��ȯ ������ �����Ѵ�.
*
*	\param pParent �޽��� �����ϴ� �θ� �޽�
*/
void GMesh::Animate(GMesh *pParent)
{
	GTransf InX;
	if (!Animation.empty())
		InX = Animation[FrameIdx];

	if (pParent == NULL)
		pParent = FindMesh(pScene->MeshList, ParentMeshName);

	if (pParent == NULL)
		MC = DelX * InX;
	else
		MC = pParent->MC * DelX * InX;

	if (FrameIdx == (int)Animation.size() - 1)
		FrameIdx = 0;
	else
		FrameIdx++;

	int NumSubMesh = (int)SubMeshList.size();
	for (int i = 0; i < NumSubMesh; ++i)
		SubMeshList[i]->Animate(this);
}

/*!
*	\brief ���� ���ε� ������ �̿��Ͽ� ��Ű���� �����Ѵ�.
*/
void GMesh::ApplySSD()
{
	int NumVert = GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		std::vector<GRefBoneInfo> &InfoList = Verts[i].BoneInfoList;
		if (InfoList.empty())
			continue;

		GVector3 P, N;
		int NumInfo = (int)InfoList.size();
		for (int j = 0; j < NumInfo; ++j)
		{
			if (EQ(InfoList[j].Wgt, 0.0, 0.0001))
				continue;

			P += cast_vec3((*InfoList[j].pRefMC) * InfoList[j].P) * InfoList[j].Wgt;
			N += ((*InfoList[j].pRefMC) * InfoList[j].N) * InfoList[j].Wgt;
		}

		Verts[i].P = cast_pt3(P);
		Verts[i].N = N;
	}
}

/*!
*	\brief ��ü(�޽�/�/���/���� ��)�� ������ �Ѵ�.
*
*	\param bSubMesh ���� �޽��� ������ ����(���� ��忡���� false�� ����)
*/
void GMesh::Render(bool bSubMesh)
{
	// ���̴� �޽����,
	if (Renderer.IsVisibleMesh())
	{
		// ���� ��ǥ�迡��
		glPushMatrix();
		{
			// ��ü�� �𵨸� ��ǥ����� ��ȯ�ϰ�,
			glMultMatrixd(cast_mat(MC).M);

			// ����� ���� ����� �����ϰ�,
			if (!ClipPlaneNameList.empty())
			{
				int idx = 0;
				std::vector<std::string>::iterator it = ClipPlaneNameList.begin();
				while(it != ClipPlaneNameList.end() && idx < 6)
				{
					GMesh *pPlane = FindMesh(SubMeshList, *it);
					if (pPlane == NULL)
					{
						it = ClipPlaneNameList.erase(it);
						continue;
					}

					GPlane pln = pPlane->DelX * GPlane(0.0, 1.0, 0.0, 0.0);
					GLdouble eqn[4] = {pln.n[0], pln.n[1], pln.n[2], pln.d};

					glEnable(GL_CLIP_PLANE0 + idx);
					glClipPlane(GL_CLIP_PLANE0 + idx, eqn);
					idx++;
					it++;
				}								
			}

			// �޸� ���� �ɼ��� �����ϰ�,
			if (Renderer.bBackFaceCull)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
			}			

			// ���̵� Ÿ���� �����ϰ�, 
			switch (Renderer.ShadeType)
			{
			case SHADE_FLAT:
				glShadeModel(GL_FLAT);
				break;

			case SHADE_SMOOTH:
				glShadeModel(GL_SMOOTH);
				break;
			}

			// ������ �������ϴ� � ��ü���,
			if (GetClassName() == "GNurbsCrv" && Renderer.bRenderCrvAsLine)
			{
				// ��� �β��� ������ ������ ��,
				glDisable(GL_LIGHTING);
				glLineWidth(2.0);
				int NumMtl = GetNumMtl();
				int MtlIdx = Renderer.CrvMtlIdx;
				if (MtlIdx >= 0 && MtlIdx < NumMtl)
					glColor3fv(MtlList[MtlIdx].Diffuse);
				else
					glColor3f(0.5f, 0.5f, 0.5f);

				// ��� �׷��ְ�,
				double u_min, u_max;
				GetDomain(&u_min, &u_max);
				glBegin(GL_LINE_STRIP);
				{
					int NumSampV = Renderer.NumSampV;
					for (int i = 0; i < NumSampV; ++i)
					{
						double u = u_min + (u_max - u_min) * i / (NumSampV - 1);
						GVector4 pt = Eval(u);
						glVertex3dv(pt.V);
					}
				}
				glEnd();
				
				glLineWidth(1.0);
				glEnable(GL_LIGHTING);				
			}
			else // ���������� ������ �Ϲ� ��ü���,
			{
				// ������ �������� ����� ���,
				if (Renderer.RenderType & RENDER_POLYGON)
				{
					// �ؽ�ó ������ �����,
					if (Renderer.bTexture && !Texs.empty())
						RenderMeshTexture();
					else	// �Ϲ� ������ ������ �����,
						RenderMeshPolygon();
				}

				// ���̾� �������� ����� ���,
				if (Renderer.RenderType & RENDER_WIRE)
					RenderMeshWire(0.0, 0.0, 0.0, 1.0);

				// ����Ʈ �������� ����� ���,
				if (Renderer.RenderType & RENDER_POINT)
					RenderMeshPoint(1.0, 0.0, 0.0, 3.0);

				// ��� �������� ����� ���,
				if (Renderer.RenderType & RENDER_BOUNDARY)
					RenderMeshBoundary(1.0, 0.0, 0.0, 2.0);

				// ���̵� ��带 �����ϰ�,
				glShadeModel(GL_SMOOTH);

				// ������ ���� ���͸� ������ �ɼǿ� ����,
				if (Renderer.RenderType & RENDER_NORMAL)
				{
					double scale = dist(BBox.MaxPt, BBox.MinPt) * 0.03;
					// ������ ����, ������ ���� �β��� ������ ��,
					glDisable(GL_LIGHTING);
					glColor3f(1.0, 0.0, 0.0);
					glLineWidth(1.5);

					// ���� ���͸� �������Ѵ�.
					glBegin(GL_LINES);
					{
						int NumVert = GetNumVert();
						for (int i = 0; i < NumVert; ++i)
						{
							GPoint3 p = Verts[i].P;
							GPoint3 q = p + Verts[i].N * scale;
							glVertex3f(p[0], p[1], p[2]);
							glVertex3f(q[0], q[1], q[2]);
						}
					}
					glEnd();

					// ������ �Ѱ�, ������ �β��� �ʱ�ȭ �Ѵ�.
					glLineWidth(1.0);
					glEnable(GL_LIGHTING);
				}

				// �浹�� �ﰢ���� �ִٸ�,
				if (!CollisionFaces.empty())
				{
					glDisable(GL_LIGHTING);
					glColor3f(1.0, 1.0, 0.0);
					glBegin(GL_TRIANGLES);
					int NumFace = (int)CollisionFaces.size();
					for (int i = 0; i < NumFace; ++i)
					{
						int fidx = CollisionFaces[i];
						int *vidx = Faces[fidx].vIdx;
						glVertex3dv(Verts[vidx[0]].P.V);
						glVertex3dv(Verts[vidx[1]].P.V);
						glVertex3dv(Verts[vidx[2]].P.V);
					}
					glEnd();
					glEnable(GL_LIGHTING);
				}
			}

			if (Faces.empty())
			{
				// ������ ��Ȱ��ȭ �ϰ�,
				glDisable(GL_LIGHTING);

				// ��� �β��� ������ ������ ��,
				glLineWidth(2.0);
				int NumMtl = GetNumMtl();
				int MtlIdx = Renderer.CrvMtlIdx;
				if (MtlIdx >= 0 && MtlIdx < NumMtl)
					glColor3fv(MtlList[MtlIdx].Diffuse);
				else
					glColor3f(0.5f, 0.5f, 0.5f);

				// ��� �׷��ְ�,
				glBegin(GL_LINE_STRIP);
				{
					int NumVert = (int)Verts.size();
					for (int i = 0; i < NumVert; ++i)
						glVertex3dv(Verts[i].P.V);
				}
				glEnd();

				// �� �β��� �⺻ ������ �� ������ ��,
				glLineWidth(1.0);

				// ������ Ȱ��ȭ �Ѵ�.
				glEnable(GL_LIGHTING);
			}

			// �ø��� Ȱ��ȭ �Ǿ��ٸ� ��Ȱ��ȭ �ϰ�,
			if (Renderer.bBackFaceCull)
				glDisable(GL_CULL_FACE);

			// ���� ����� ������ �ִٸ�, ��Ȱ��ȭ �Ѵ�.
			if (!ClipPlaneNameList.empty())
			{
				for (int i = 0; i < 6; ++i)
					glDisable(GL_CLIP_PLANE0 + i);
			}
		}
		glPopMatrix();	// ���� ��ǥ��� �����Ѵ�.
	}

	// ���ǿ� ���� ���� �޽��� ������ �Ѵ�.
	if (bSubMesh)
	{
		int NumSubMesh = (int)SubMeshList.size();
		for (int i = 0; i < NumSubMesh; ++i)
			SubMeshList[i]->Render();
	}
}

/*!
*	\brief �޽��� �����Ѵ�.
*/
void GMesh::CreateMesh()
{
	InitBndryEdgeList();
	UpdateNormal(false);
	UpdateBndBox(false);
	UpdateBufVertex();
	UpdateBufNormal();
	UpdateBufFace();
}

/*!
*	\brief GMesh�� �Ļ� Ŭ������ ���� �����Լ�
*/
GVector4 GMesh::Eval(double u, ...) 
{ 
	return GVector4(); 
}

/*!
*	\brief GMesh�� �Ļ� Ŭ������ ���� �����Լ�
*/
void GMesh::GetDomain(double *umin, ...) 
{ 
	*umin = 0.0; 
}

/*!
*	\brief GMesh�� �Ļ� Ŭ������ ���� �����Լ�
*/
void GMesh::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList) 
{
}

/*!
*	\brief �ؽ�ó �޽��� ������ �Ѵ�.
*/
void GMesh::RenderMeshTexture()
{
	// �ؽ�ó ���� �ɼ��� �����ϰ�,
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_TEXTURE_2D);
	
	// �ؽ�ó���� �޽��� ���÷��� ����Ʈ�� �ִٸ�,
	if (DispTexMesh != 0)
		glCallList(DispTexMesh);
	else	// �ؽ�ó���� �޽��� ���÷��� ����Ʈ�� ���ٸ�,
	{
		// �ؽ�ó ������ ���·� �������� ���÷��� ����Ʈ�� �����Ѵ�.
		DispTexMesh = glGenLists(1);
		glNewList(DispTexMesh, GL_COMPILE_AND_EXECUTE);
		{
			// ������ ������ Ȱ��ȭ �ϰ�,
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);

			// ������ �ε����� �ʱ�ȭ �ϰ�,
			int PrevMtlIdx = -1;
			int NumFace = GetNumFace();
			int NumMtl = GetNumMtl();

			// ��� �ﰢ���� ������ �Ѵ�.
			glBegin(GL_TRIANGLES);
			for (int i = 0; i < NumFace; ++i)
			{
				int CurrMtlIdx = Faces[i].MtlIdx;
				if (CurrMtlIdx != PrevMtlIdx)
				{
					// ���� ������ �����ϰ�,
					glEnd();

					// ���ο� ������ ������ ��,
					if (CurrMtlIdx >= 0 && CurrMtlIdx < NumMtl)
						MtlList[CurrMtlIdx].Enable(true);
					PrevMtlIdx = CurrMtlIdx;

					// �ﰢ�� �������� �����Ѵ�.
					glBegin(GL_TRIANGLES);				
				}

				// �ؽ�ó�� ������ �ε����� ���Ͽ�,
				int *tidx = Faces[i].tIdx;
				int *vidx = Faces[i].vIdx;

				// �ﰢ���� �������Ѵ�.
				glTexCoord2dv(Texs[tidx[0]].V);
				glNormal3dv(Verts[vidx[0]].N.V);
				glVertex3dv(Verts[vidx[0]].P.V);

				glTexCoord2dv(Texs[tidx[1]].V);
				glNormal3dv(Verts[vidx[1]].N.V);
				glVertex3dv(Verts[vidx[1]].P.V);

				glTexCoord2dv(Texs[tidx[2]].V);
				glNormal3dv(Verts[vidx[2]].N.V);
				glVertex3dv(Verts[vidx[2]].P.V);
			}
			glEnd();

			// ������ �������� ��Ȱ��ȭ �Ѵ�.
			glDisable(GL_POLYGON_OFFSET_FILL);		
		}	
		glEndList();
	}

	// �ؽ�ó ����� ��Ȱ��ȭ �Ѵ�.
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, NULL);
}

/*!
*	\brief �޽��� ������ ���·� ������ �Ѵ�.
*/
void GMesh::RenderMeshPolygon()
{
	// ���� ���� �����Ѵٸ�,
	if (Renderer.VertColorType == VERTEX_COLOR_ILLUMINATION && !VtxBuf.empty() && !NorBuf.empty())
	{
		// ������ ������ Ȱ��ȭ �ϰ�,
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);

		// ������ �븻 ���۸� �����ϰ�,
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, &(VtxBuf[0]));
		glNormalPointer(GL_FLOAT, 0, &(NorBuf[0]));

		// ������ ���� ������ �׷��� ������ �Ѵ�.
		std::map<int, std::vector<int>>::iterator it;
		for (it = FaceIdxBuf.begin(); it != FaceIdxBuf.end(); ++it)
		{
			// ������ �����Ͽ�
			int MtlIdx = it->first;
			if (MtlIdx != -1 && !MtlList.empty())
				MtlList[MtlIdx].Enable(false);

			// �������� �������ϰ�,
			if (FaceType == FACE_QUAD)
				glDrawElements(GL_QUADS, it->second.size(), GL_UNSIGNED_INT, &(it->second[0]));
			else
				glDrawElements(GL_TRIANGLES, it->second.size(), GL_UNSIGNED_INT, &(it->second[0]));

			// ������ ��Ȱ��ȭ �Ѵ�.
			if (MtlIdx != -1 && !MtlList.empty())
				MtlList[MtlIdx].Disable();
		}

		// ������ �븻 ���۸� �����ϰ� ������ �������� ��Ȱ��ȭ �Ѵ�.
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	// ������ ������ �Ҵ� �Ǿ��ٸ�,
	else
	{
		// �÷����� �޽��� ���÷��� ����Ʈ�� �ִٸ�,
		if (DispVertColMesh != 0)
			glCallList(DispVertColMesh);
		else	// �÷����� �޽��� ���÷��� ����Ʈ�� ���ٸ�,,
		{
			// ���÷��� ����Ʈ�� �����Ѵ�.
			DispVertColMesh = glGenLists(1);
			glNewList(DispVertColMesh, GL_COMPILE_AND_EXECUTE);
			{
				// ������ ���� ������ ������ Ȱ��ȭ �ϰ�,
				glDisable(GL_LIGHTING);
				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(1.0, 1.0);

				// ��� �ﰢ���� ������ �Ѵ�.
				glBegin(GL_TRIANGLES);
				{
					int NumFace = GetNumFace();
					for (int i = 0; i < NumFace; ++i)
					{
						int *vidx = Faces[i].vIdx;
						glColor3d(Verts[vidx[0]].C[0], Verts[vidx[0]].C[1], Verts[vidx[0]].C[2]);
						glVertex3dv(Verts[vidx[0]].P.V);

						glColor3d(Verts[vidx[1]].C[0], Verts[vidx[1]].C[1], Verts[vidx[1]].C[2]);
						glVertex3dv(Verts[vidx[1]].P.V);

						glColor3d(Verts[vidx[2]].C[0], Verts[vidx[2]].C[1], Verts[vidx[2]].C[2]);
						glVertex3dv(Verts[vidx[2]].P.V);				
					}
				}
				glEnd();

				// ������ �������� ��Ȱ��ȭ, ������ Ȱ��ȭ �Ѵ�.
				glDisable(GL_POLYGON_OFFSET_FILL);
				glEnable(GL_LIGHTING);
			}
			glEndList();
		}
	}	
}

/*!
*	\brief �޽��� ���̾������� ���·� ������ �Ѵ�.
*	
*	\param r ������ ������ ����
*	\param g ������ �ʷϻ� ����
*	\param b ������ �Ķ��� ����
*	\param width ������ �β�
*/
void GMesh::RenderMeshWire(float r, float g, float b, float width)
{
	// ������ ������ ��带 �������� �����ϰ�, ������ ����, ������ ������ ��,
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_LIGHTING);
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(r, g, b, 1.0);
	glLineWidth(0.00001f);

	// �������� Ÿ�Կ� ����,
	switch (FaceType)
	{
		// �ﰢ���̶��,
	case FACE_TRI:
		{			
			// ���� ���ۿ� �ε��� ���۸� �̿��Ͽ� ������ �Ѵ�.
			if (!VtxBuf.empty())
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(3, GL_FLOAT, 0, &(VtxBuf[0]));
				std::map<int, std::vector<int>>::iterator it;
				for (it = FaceIdxBuf.begin(); it != FaceIdxBuf.end(); ++it)
					glDrawElements(GL_TRIANGLES, it->second.size(), GL_UNSIGNED_INT, &(it->second[0]));
				glDisableClientState(GL_VERTEX_ARRAY);
			}			
		}
		break;

		// �ﰢ�� ���̶��,
	case FACE_TRI_FAN:
		{
			// �Ϲ� ���� ������ �Ѵ�.
			glBegin(GL_LINE_LOOP);
			for (int i = 0; i < GetNumVert(); ++i)
				glVertex3dv(Verts[i].P.V);
			glEnd();
		}
		break;

		// �簢���̶��,
	case FACE_QUAD:
		{
			// ���� ����� ������ ����� ������ �Ѵٸ�,
			if (Renderer.RenderType & RENDER_SWEEP_PLANE)
			{
				// �Ϲ� ���� ������ �Ѵ�.
				glBegin(GL_QUADS);
				for (int i = 0; i < GetNumVert(); i += 4)
				{
					glVertex3dv(Verts[i].P.V);
					glVertex3dv(Verts[i + 1].P.V);
					glVertex3dv(Verts[i + 2].P.V);
					glVertex3dv(Verts[i + 3].P.V);
				}
				glEnd();
			}
			else	// �Ϲ����� �簢 �޽����,
			{
				// ���� ���ۿ� �ε��� ���۸� �̿��Ͽ� ������ �Ѵ�.
				if (!VtxBuf.empty())
				{
					glEnableClientState(GL_VERTEX_ARRAY);
					glVertexPointer(3, GL_FLOAT, 0, &(VtxBuf[0]));
					std::map<int, std::vector<int>>::iterator it;
					for (it = FaceIdxBuf.begin(); it != FaceIdxBuf.end(); ++it)
						glDrawElements(GL_QUADS, it->second.size(), GL_UNSIGNED_INT, &(it->second[0]));
					glDisableClientState(GL_VERTEX_ARRAY);
				}
			}	
		}
		break;
	}

	// ������ ������ ��带 �缳���ϰ�, ������ Ȱ��ȭ �Ѵ�.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glDisable(GL_BLEND);
	//glDisable(GL_LINE_SMOOTH);
	glEnable(GL_LIGHTING);
}

/*!
*	\brief �޽��� ����Ʈ ���·� ������ �Ѵ�.
*
*	\param r ������ ������ ����
*	\param g ������ �ʷϻ� ����
*	\param b ������ �Ķ��� ����
*	\param size ������ ũ��
*/
void GMesh::RenderMeshPoint(float r, float g, float b, float size)
{
	glDisable(GL_LIGHTING);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(r, g, b, 0.5f);
	glPointSize(size);
	glBegin(GL_POINTS);
	{
		int NumVert = GetNumVert();
		for (int i = 0; i < NumVert; ++i)
			glVertex3dv(Verts[i].P.V);
	}
	glEnd();
	glPointSize(1.0);

	glDisable(GL_BLEND);
	glDisable(GL_POINT_SMOOTH);
	glEnable(GL_LIGHTING);
}

/*!
*	\brief �޽��� �����ڸ� ��踦 ������ �Ѵ�.
*
*	\param r ������ ������ ����
*	\param g ������ �ʷϻ� ����
*	\param b ������ �Ķ��� ����
*	\param width ��輱�� �β�
*/
void GMesh::RenderMeshBoundary(float r, float g, float b, float width)
{
	// �޽��� ��� ������ ����Ʈ�� ã������ �ʾҴٸ�,
	if (BndryEdgeList.empty())
		::get_bndry_edges(this, BndryEdgeList);
	
	glDisable(GL_LIGHTING);
	glColor3f(r, g, b);
	glLineWidth(width);

	glBegin(GL_LINES);
	for (std::map<int, int>::iterator it = BndryEdgeList.begin(); it != BndryEdgeList.end(); ++it)
	{
		glVertex3dv(Verts[it->first].P.V);
		glVertex3dv(Verts[it->second].P.V);
	}
	glEnd();

	glLineWidth(1.0);
	glEnable(GL_LIGHTING);
}

/*!
*	\brief �޽��� ���� ���۸� �����Ѵ�.
*/
void GMesh::UpdateBufVertex()
{
	VtxBuf.clear();
	
	int NumVert = GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		VtxBuf.push_back(Verts[i].P[0]);
		VtxBuf.push_back(Verts[i].P[1]);
		VtxBuf.push_back(Verts[i].P[2]);
	}

	if (DispTexMesh != 0)
		glDeleteLists(DispTexMesh, 1);
	DispTexMesh = 0;

	if (DispVertColMesh != 0)
		glDeleteLists(DispVertColMesh, 1);
	DispVertColMesh = 0;

	if (DispFaceColMesh != 0)
		glDeleteLists(DispFaceColMesh, 1);
	DispFaceColMesh = 0;
}

/*!
*	\brief �޽��� ���� ���۸� �����Ѵ�.
*/
void GMesh::UpdateBufNormal()
{
	NorBuf.clear();

	int NumVert = GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		NorBuf.push_back(Verts[i].N[0]);
		NorBuf.push_back(Verts[i].N[1]);
		NorBuf.push_back(Verts[i].N[2]);
	}

	if (DispTexMesh != 0)
		glDeleteLists(DispTexMesh, 1);
	DispTexMesh = 0;

	if (DispVertColMesh != 0)
		glDeleteLists(DispVertColMesh, 1);
	DispVertColMesh = 0;

	if (DispFaceColMesh != 0)
		glDeleteLists(DispFaceColMesh, 1);
	DispFaceColMesh = 0;
}

/*!
*	\brief �޽��� ������ �ε��� ���۸� �����Ѵ�.
*/
void GMesh::UpdateBufFace()
{
	// ������ �ε��� ���۸� �ʱ�ȭ�ϰ�,
	FaceIdxBuf.clear();

	int NumFace = GetNumFace();
	switch (FaceType)
	{
		// �ﰢ�� �Ǵ� �ﰢ�� �� Ÿ���̶��,
	case FACE_TRI:
	case FACE_TRI_FAN:
		{
			for (int i = 0; i < NumFace; ++i)
			{
				int MtlIdx = Faces[i].MtlIdx;
				int *vidx = Faces[i].vIdx;

				// �ﰢ���� ���� �ε����� �߰��Ѵ�.
				FaceIdxBuf[MtlIdx].push_back(vidx[0]);
				FaceIdxBuf[MtlIdx].push_back(vidx[1]);
				FaceIdxBuf[MtlIdx].push_back(vidx[2]);
			}
		}
		break;

		// �簢�� Ÿ���̶��,
	case FACE_QUAD:
		{
			for (int i = 0; i < NumFace - 1; i += 2)
			{
				int MtlIdx = Faces[i].MtlIdx;
				int *vidx = Faces[i].vIdx;
				int vidx3 = Faces[i + 1].vIdx[2];

				// �簢���� ���� �ε����� �߰��Ѵ�.
				FaceIdxBuf[MtlIdx].push_back(vidx[0]);
				FaceIdxBuf[MtlIdx].push_back(vidx[1]);
				FaceIdxBuf[MtlIdx].push_back(vidx[2]);
				FaceIdxBuf[MtlIdx].push_back(vidx3);
			}
		}
		break;		
	}

	if (DispTexMesh != 0)
		glDeleteLists(DispTexMesh, 1);
	DispTexMesh = 0;

	if (DispVertColMesh != 0)
		glDeleteLists(DispVertColMesh, 1);
	DispVertColMesh = 0;

	if (DispFaceColMesh != 0)
		glDeleteLists(DispFaceColMesh, 1);
	DispFaceColMesh = 0;
}

/*!
*	\brief �޽��� ��� ������ ����Ʈ�� �ʱ�ȭ �Ѵ�.
*/
void GMesh::InitBndryEdgeList()
{
	BndryEdgeList.clear();
}

/*!
*	\brief �޽��� �𵨸� ��ǥ��� ũ�⸦ ����ȭ �Ѵ�.
*/
void GMesh::Normalize()
{
	// ���� ��ǥ�迡�� ǥ���� ������ ��ǥ�� ���ϰ�,
	int NumVert = GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		Verts[i].P = MC * Verts[i].P;
		Verts[i].N = MC * Verts[i].N;
	}

	// ������ǥ�迡�� ǥ���� �������� ��ȯ�� ����ϰ�,
	// �� ��ȯ�� �޽��� �𵨸� ��ǥ��� ����
	MC = BBox.GetMC();

	// �޽��� �𵨸� ��ǥ�迡�� ǥ���� ������ ��ǥ�� ����Ѵ�.
	for (int i = 0; i < NumVert; ++i)
	{
		Verts[i].P = inv(MC) * Verts[i].P;
		Verts[i].N = inv(MC) * Verts[i].N;
	}

	// �𵨸� ��ǥ�踦 ���� ��ǥ��� ��ġ��Ų��.
	MC = GTransf();

	double scale = BBox.GetDiagonal();
	// �޽��� �𵨸� ��ǥ�迡�� ǥ���� ������ ��ǥ�� ����Ѵ�.
	for (int i = 0; i < NumVert; ++i)
	{
		Verts[i].P[0] /= scale;
		Verts[i].P[1] /= scale;
		Verts[i].P[2] /= scale;
	}

	// �޽��� ������Ʈ �Ѵ�.
	ModifiedFlag = MODIFIED_VERT_POS_ALL;
	UpdateMesh();
}

/*!
*	\brief ������ �Ӽ��� �޽� �������� Ȱ��ȭ �Ѵ�.
*
*	\param bSubMesh ����޽��� ���뿩��
*/
void GMesh::ShowMesh(bool bSubMesh)
{
	Renderer.RenderType = RENDER_POLYGON;
	if (bSubMesh)
	{
		int NumMesh = (int)SubMeshList.size();
		for (int i = 0; i < NumMesh; ++i)
		{
			SubMeshList[i]->ShowMesh(bSubMesh);
		}
	}
}

/*!
*	\brief ������ �Ӽ��� �޽� �������� ��Ȱ��ȭ �Ѵ�.
*
*	\param bSubMesh ����޽��� ���뿩��
*/
void GMesh::HideMesh(bool bSubMesh)
{
	Renderer.RenderType = 0;
	if (bSubMesh)
	{
		int NumMesh = (int)SubMeshList.size();
		for (int i = 0; i < NumMesh; ++i)
		{
			SubMeshList[i]->HideMesh(bSubMesh);
		}
	}
}

/*!
*	\brief �޽��� �ٿ�� ���� ����(BVH) ������ �����Ѵ�.
*	\note PQP ���̺귯���� �̿��Ѵ�.
*/
void GMesh::CreateBVH()
{
	CollisionFaces.clear();
	if (pBVH != NULL)
		delete pBVH;

	pBVH = new PQP_Model();
	int NumFace = GetNumFace();
	pBVH->BeginModel();
	for (int i = 0; i < NumFace; ++i)
	{
		int *vidx = Faces[i].vIdx;
		GPoint3 p0 = Verts[vidx[0]].P;
		GPoint3 p1 = Verts[vidx[1]].P;
		GPoint3 p2 = Verts[vidx[2]].P;
		pBVH->AddTri(p0.V, p1.V, p2.V, i);
	}
	pBVH->EndModel();
}

/*!
*	\brief �޽��� �ٿ�� ���� ����(BVH) ������ �����Ѵ�.
*/
void GMesh::DeleteBVH()
{
	if (pBVH != NULL)
		delete pBVH;

	CollisionFaces.clear();
	pBVH = NULL;
}

/****************************/
/* GRefBoneInfo Ŭ���� ���� */
/****************************/

/*!
*	\brief ����Ʈ ������
*/
GRefBoneInfo::GRefBoneInfo()
{
	pRefMC = NULL;
	Wgt = 0.0;
}

/*!
*	\brief ������
*/
GRefBoneInfo::GRefBoneInfo(std::string &_BoneMeshName, double _Wgt, GPoint3 &_P, GVector3 &_N)
: BoneMeshName(_BoneMeshName), pRefMC(NULL), Wgt(_Wgt), P(_P), N(_N)
{
}

/*!
*	\brief ���� ������
*
*	\param cpy ����� ��ü
*/
GRefBoneInfo::GRefBoneInfo(const GRefBoneInfo &cpy)
: BoneMeshName(cpy.BoneMeshName), pRefMC(cpy.pRefMC), Wgt(cpy.Wgt), P(cpy.P), N(cpy.N)
{
}

/*!
*	\brief �Ҹ���
*/
GRefBoneInfo::~GRefBoneInfo()
{
}

/*!
*	\brief ���Կ�����
*
*	\param rhs ���Ե� ��ü
*
*	\return ���Ե� �ڽ��� ��ȯ
*/
GRefBoneInfo &GRefBoneInfo::operator =(const GRefBoneInfo &rhs)
{
	BoneMeshName = rhs.BoneMeshName;
	pRefMC = rhs.pRefMC;
	Wgt = rhs.Wgt;
	P = rhs.P;
	N = rhs.N;

	return *this;
}

/*************************/
/* GPathInfo Ŭ���� ���� */
/*************************/

/*!
*	\brief ������
*/
GPathInfo::GPathInfo(int _sidx, int _eidx, double _t)
{
	sidx = _sidx;
	eidx = _eidx;
	t = _t;
}

/*!
*	\brief ���� ������
*
*	\param cpy ����� ��ü
*/
GPathInfo::GPathInfo(const GPathInfo &cpy)
{
	sidx = cpy.sidx;
	eidx = cpy.eidx;
	t = cpy.t;
}

/*!
*	\brief �Ҹ���
*/
GPathInfo::~GPathInfo()
{
}

/*!
*	\brief ���Կ�����
*
*	\param rhs ���Ե� ��ü
*
*	\return ���Ե� �ڽ��� ��ȯ
*/
GPathInfo &GPathInfo::operator =(const GPathInfo &rhs)
{
	sidx = rhs.sidx;
	eidx = rhs.eidx;
	t = rhs.t;

	return *this;
}

/***********************/
/* GCtrlPt Ŭ���� ���� */
/***********************/
IMPLEMENT_GEDIT_OBJECT(GCtrlPt)

/*!
*	\brief �������� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return �������� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GCtrlPt::GetRefMesh()
{
	return pMesh;
}

/*!
*	\brief �������� ����� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ǥ���� �������� ����� ��ǥ�踦 ��ȯ�Ѵ�.
*/
GTransf GCtrlPt::GetTransf()
{
	return pMesh->MC * GTransf(GVector3(V[0], V[1], V[2]));
}

/*!
*	\brief ���õ� �������� �����Ѵ�.
*
*	\param EC ���� ��ǥ��
*	\param t ������ ��ü�� �̵� ����
*	\param q ������ ��ü�� ȸ�� ����
*	\param s ������ ��ü�� ������ ����
*	\param pParentMesh ���� ��ü�� �θ� �޽�
*/
void GCtrlPt::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	// ���� ��ȯ�� ���� ������ �����ϰ�,
	static GTransf MC2MC;

	// �����Ǵ� ù��° �����̶��,
	if (pMesh->ModifiedFlag == MODIFIED_NONE)
	{
		// ���� ��ȯ�� ����ϰ�,
		MC2MC = inv(pMesh->MC) * EC * GTransf(t, q) * inv(EC) * pMesh->MC;

		// ������ ������ ����ϰ�,
		pMesh->ModifiedFlag = MODIFIED_CTLPT_POS;						
	}

	// ������ ������ ��ǥ�� ����Ͽ�,
	GPoint3 pt = MC2MC * GPoint3(V[0], V[1], V[2]);

	// ������ ��ȯ�� �����ϰ�,
	if (s[0] != 1.0) pt[0] *= s[0];
	if (s[1] != 1.0) pt[1] *= s[1];
	if (s[2] != 1.0) pt[2] *= s[2];

	// ���� ������ ��ġ�� �����Ѵ�.
	V[0] = pt[0];
	V[1] = pt[1];
	V[2] = pt[2];
}

/*!
*	\brief ���� �������� ������ �Ѵ�.
*/
void GCtrlPt::RenderForEdit(float r, float g, float b)
{
	// ���� ��ǥ��: ����(���)
	glPushMatrix();
	{
		// ��ü�� �𵨸� ��ǥ��� ��ȯ
		glMultMatrixd(cast_mat(pMesh->MC).M);

		glDisable(GL_LIGHTING);
		glColor3f(0.2627, 1.0, 0.64);
		
		double size = pMesh->Renderer.CtrlPtSize * 1.1;
		glTranslated(V[0], V[1], V[2]);
		glutWireSphere(size, 10, 10);
		
		glEnable(GL_LIGHTING);
	}
	glPopMatrix();
}

/*!
*	\brief ����Ʈ ������
*/
GCtrlPt::GCtrlPt()
: GVector4()
{
	pMesh = NULL;
}

/*!
*	\brief ������
*
*	\param _P �������� ��ǥ
*/
GCtrlPt::GCtrlPt(GVector4 _P)
: GVector4(_P)
{
	pMesh = NULL;
}

/*!
*	\brief ���� ������
*
*	\param cpy ����� ��ü
*/
GCtrlPt::GCtrlPt(const GCtrlPt &cpy)
: GVector4(cpy)
{
	pMesh = cpy.pMesh;
}

/*!
*	\brief �Ҹ���
*/
GCtrlPt::~GCtrlPt()
{
}

/*!
*	\brief ���Կ�����
*
*	\param rhs ���Ե� ��ü
*	
*	\return ���� ��, �ڽ��� ��ȯ�Ѵ�.
*/
GCtrlPt &GCtrlPt::operator =(const GVector4 &rhs)
{
	GVector4::operator =(rhs);

	return *this;
}

/*!
*	\brief ���Կ�����
*
*	\param rhs ���Ե� ��ü
*	
*	\return ���� ��, �ڽ��� ��ȯ�Ѵ�.
*/
GCtrlPt &GCtrlPt::operator =(const GCtrlPt &rhs)
{
	GVector4::operator =(rhs);

	pMesh = rhs.pMesh;

	return *this;
}

/*!
*	\brief (+)������
*
*	\param lhs ���� �ǿ�����
*	\param rhs ������ �ǿ�����
*	
*	\return ������ ����� ��ȯ�Ѵ�.
*/
GCtrlPt operator +(const GCtrlPt &lhs, const GCtrlPt &rhs)
{
	GCtrlPt ret;
	ret.pMesh = lhs.pMesh;
	ret.V[0] = lhs.V[0] + rhs.V[0];
	ret.V[1] = lhs.V[1] + rhs.V[1];
	ret.V[2] = lhs.V[2] + rhs.V[2];
	ret.V[3] = lhs.V[3] + rhs.V[3];

	return ret;
}

/*!
*	\brief (+)������
*
*	\param lhs ���� �ǿ�����
*	\param rhs ������ �ǿ�����
*	
*	\return ������ ����� ��ȯ�Ѵ�.
*/
GCtrlPt operator +(const GVector4 &lhs, const GCtrlPt &rhs)
{
	GCtrlPt ret;
	ret.pMesh = rhs.pMesh;
	ret.V[0] = lhs.V[0] + rhs.V[0];
	ret.V[1] = lhs.V[1] + rhs.V[1];
	ret.V[2] = lhs.V[2] + rhs.V[2];
	ret.V[3] = lhs.V[3] + rhs.V[3];

	return ret;
}

/*!
*	\brief (+)������
*
*	\param lhs ���� �ǿ�����
*	\param rhs ������ �ǿ�����
*	
*	\return ������ ����� ��ȯ�Ѵ�.
*/
GCtrlPt operator +(const GCtrlPt &lhs, const GVector4 &rhs)
{
	GCtrlPt ret;
	ret.pMesh = lhs.pMesh;
	ret.V[0] = lhs.V[0] + rhs.V[0];
	ret.V[1] = lhs.V[1] + rhs.V[1];
	ret.V[2] = lhs.V[2] + rhs.V[2];
	ret.V[3] = lhs.V[3] + rhs.V[3];

	return ret;
}

/**********************************/
/* Implementation of GCutPt Class */
/**********************************/
IMPLEMENT_GEDIT_OBJECT(GCutPt)

/*!
*	\brief ���� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return ���� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GCutPt::GetRefMesh()
{
	if (pFace == NULL)
		return NULL;
	else
		return pFace->pMesh;
}

/*!
*	\brief �������� ����� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ǥ���� �������� ����� ��ǥ�踦 ��ȯ�Ѵ�.
*/
GTransf GCutPt::GetTransf()
{
	GTransf ret;

	if (pFace != NULL)
		ret = pFace->pMesh->MC * GTransf(cast_vec3(P));

	return ret;
}

/*!
*	\brief ���õ� ���� �����Ѵ�.
*
*	\param EC ���� ��ǥ��
*	\param t ������ ��ü�� �̵� ����
*	\param q ������ ��ü�� ȸ�� ����
*	\param s ������ ��ü�� ������ ����
*	\param pParentMesh ���� ��ü�� �θ� �޽�
*/
void GCutPt::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	// �������� ��ġ�� �����Ѵ�.
	P[0] += t[0];
	P[1] += t[1];
	P[2] += t[2];
}

/*!
*	\brief ���� ������ �Ѵ�.
*/
void GCutPt::RenderForEdit(float r, float g, float b)
{
	if (pFace == NULL)
		return;

	// ���� ��ǥ��: ����(���)
	glPushMatrix();
	{
		// ��ü�� �𵨸� ��ǥ��� ��ȯ
		glMultMatrixd(cast_mat(pFace->pMesh->MC).M);

		glDisable(GL_LIGHTING);
		glColor3f(0.2627, 1.0, 0.64);

		double size = pFace->pMesh->Renderer.CtrlPtSize * 1.1;
		glTranslated(P[0], P[1], P[2]);
		glutWireSphere(size, 10, 10);

		glEnable(GL_LIGHTING);
	}
	glPopMatrix();
}

/*!
*	\brief ����Ʈ ������
*/
GCutPt::GCutPt()
{
	pFace = NULL;
	vIdx[0] = -1;
	vIdx[1] = -1;
	vIdx[2] = -1;
	t = 0.0;
}

/*!
*	\brief ������
*
*	\param _pFace ���� ���� �ִ� �𼭸��� ������ �ﰢ��
*	\param vidx0 ���� ���� �ִ� �𼭸��� ���� ������ �ε���
*	\param vidx1 ���� ���� �ִ� �𼭸��� �� ������ �ε���
*	\param _P ������ ��ġ
*/
GCutPt::GCutPt(GFace *_pFace, int vidx0, int vidx1, double _t, GPoint3 _P)
{
	pFace = _pFace;
	vIdx[0] = vidx0;
	vIdx[1] = vidx1;

	if (vIdx[0] == pFace->vIdx[0])
		vIdx[2] = pFace->vIdx[2];
	else if (vIdx[0] == pFace->vIdx[1])
		vIdx[2] = pFace->vIdx[0];
	else
		vIdx[2] = pFace->vIdx[1];

	t = _t;
	P = _P;
}

/*!
*	\brief ���� ������
*
*	\param cpy ����� ��ü
*/
GCutPt::GCutPt(const GCutPt &cpy)
{
	pFace = cpy.pFace;
	vIdx[0] = cpy.vIdx[0];
	vIdx[1] = cpy.vIdx[1];
	vIdx[2] = cpy.vIdx[2];
	t = cpy.t;
	P = cpy.P;
}

/*!
*	\brief �Ҹ���
*/
GCutPt::~GCutPt()
{
}

/*!
*	\brief ���Կ�����
*
*	\param rhs ���Ե� ��ü
*	
*	\return ���� ��, �ڽ��� ��ȯ�Ѵ�.
*/
GCutPt &GCutPt::operator =(const GCutPt &rhs)
{
	pFace = rhs.pFace;
	vIdx[0] = rhs.vIdx[0];
	vIdx[1] = rhs.vIdx[1];
	vIdx[2] = rhs.vIdx[2];
	t = rhs.t;
	P = rhs.P;

	return *this;
}

/***********************************/
/* Implementation of GCirArc Class */
/***********************************/
/*!
*	\brief ����Ʈ ������
*/
GCirArc::GCirArc()
{
	// �⺻ ������ ��� (5)
	Center = GPoint3(0.0, 0.0, 0.0);	// ��ũ�� �߽�
	Radius = 1.0;						// ��ũ�� ������
	AngleStart = 0.0;					// ��ũ�� ���� ����
	AngleEnd = 2 * M_PI;				// ��ũ�� �� ����
	Direction = DIR_CCW;				// ��ũ�� ����
	LenTangent = 0.0;					// ��ũ�� ���κ� ������ ����
}

/*!
*	\brief ������
*
*	\param _Center ��ũ�� �߽�
*	\param _Radius ��ũ�� �ݰ�
*	\param _AngleStart ��ũ�� ���� ����
*	\param _AngleEnd ��ũ�� ���� ����
*	\param _Direction ��ũ�� ����
*/
GCirArc::GCirArc(GPoint3 _Center, double _Radius, double _AngleStart, double _AngleEnd, TypeCirDir _Direction, double _LenTangent)
{
	Center = _Center;					// ��ũ�� �߽�
	Radius = _Radius;					// ��ũ�� ������
	AngleStart = DEG2RAD(_AngleStart);	// ��ũ�� ���� ����
	AngleEnd = DEG2RAD(_AngleEnd);		// ��ũ�� �� ����
	Direction = _Direction;				// ��ũ�� ����
	LenTangent = _LenTangent;			// ��ũ�� ���κ� ������ ����

	// ��ũ�� ���⿡ ���� ���� ������ �� ������ �����Ѵ�.
	switch (Direction)
	{
	case DIR_CCW:
		if (AngleStart > AngleEnd)
			AngleEnd = AngleEnd + 2 * M_PI;
		break;

	case DIR_CW:
		if (AngleStart < AngleEnd)
			AngleStart = AngleStart + 2 * M_PI;
		break;
	}
}

/*!
*	\brief ���������
*
*	\param cpy ����� ��ü
*/
GCirArc::GCirArc(const GCirArc &cpy)
{
	Center = cpy.Center;			// ��ũ�� �߽�
	Radius = cpy.Radius;			// ��ũ�� ������
	AngleStart = cpy.AngleStart;	// ��ũ�� ���� ����
	AngleEnd = cpy.AngleEnd;		// ��ũ�� �� ����
	Direction = cpy.Direction;		// ��ũ�� ����
	LenTangent = cpy.LenTangent;	// ��ũ�� ���κ� ������ ����
}

/*!
*	\brief �Ҹ���
*/
GCirArc::~GCirArc()
{
}

/*!
*	\brief ���� ������
*
*	\param rhs ������ �ǿ�����
*
*	\return ���Ե� �ڽ��� ��ȯ�Ѵ�.
*/
GCirArc &GCirArc::operator =(const GCirArc &rhs)
{
	Center = rhs.Center;			// ��ũ�� �߽�
	Radius = rhs.Radius;			// ��ũ�� ������
	AngleStart = rhs.AngleStart;	// ��ũ�� ���� ����
	AngleEnd = rhs.AngleEnd;		// ��ũ�� �� ����
	Direction = rhs.Direction;		// ��ũ�� ����
	LenTangent = rhs.LenTangent;	// ��ũ�� ���κ� ������ ����

	return *this;
}

/*!
*	\brief ��ũ���� ���� ���
*
*	\param t �Ű������� ��
*
*	\return ����� �� C(t)�� ���
*/
GVector4 GCirArc::Eval(double t)
{
	double theta = AngleStart + (AngleEnd - AngleStart) * t;
	double x = Radius * cos(theta) + Center[0];
	double y = Radius * sin(theta) + Center[1];

	return GVector4(x, y, 0.0, 0.0);
}

/*!
*	\brief ��ũ���� ������ ������ ����Ѵ�.
*
*	\param t �Ű������� ��
*
*	\return ��ũ���� ������ ���� C'(t)�� ����Ͽ� ��ȯ�Ѵ�.
*/
GVector4 GCirArc::EvalDeriv(double t)
{
	double theta = AngleStart + (AngleEnd - AngleStart) * t;
	double x = cos(theta);
	double y = sin(theta);

	if (Direction == DIR_CCW)
		return GVector4(-y, x, 0.0, 0.0);
	else
		return GVector4(y, -x, 0.0, 0.0);
}

