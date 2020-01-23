#include "stdafx.h"

/***********************************/
/* Implementation of GVertex Class */
/***********************************/
IMPLEMENT_GEDIT_OBJECT(GEditObj)

/*!
*	\brief 객체가 포함된 메쉬의 포인터를 반환한다.
*
*	\return 객체가 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GEditObj::GetRefMesh()
{
	return NULL;
}

/*!
*	\brief 월드 좌표계에서 표현된 편집 객체의 변환 정보를 반환한다.
*
*	\return 월드 좌표계에서 표현된 편집 객체의 변환 정보를 반환한다.
*/
GTransf GEditObj::GetTransf()
{
	return GTransf();
}

/*!
*	\brief 선택된 객체를 편집한다.
*
*	\param EC 편집 좌표계
*	\param t 편집될 객체의 이동 변위
*	\param q 편집될 객체의 회전 변위
*	\param s 편집될 객체의 스케일 변위
*	\param pParentMesh 편집 객체의 부모 메쉬
*/
void GEditObj::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
}

/*!
*	\brief 선택된 객체를 렌더링 한다.
*/
void GEditObj::RenderForEdit(float r, float g, float b)
{
}

/***********************/
/* GVertex 클래스 구현 */
/***********************/
IMPLEMENT_GEDIT_OBJECT(GVertex)

/*!
*	\brief 정점이 포함된 메쉬의 포인터를 반환한다.
*
*	\return 정점이 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GVertex::GetRefMesh()
{
	return pMesh;
}

/*!
*	\brief 정점의 모델링 좌표계를 반환한다.
*
*	\return 정점의 모델링 좌표계를 반환한다.
*/
GTransf GVertex::GetTransf()
{
// 	GQuater q;
// 	q.SetFromArcBall(GVector3(0.0, 1.0, 0.0), N);
//	return pMesh->MC * GTransf(cast_vec3(P), q);
	return pMesh->MC * GTransf(cast_vec3(P));
}

/*!
*	\brief 정점이 속한 메쉬를 편집한다.
*
*	\param EC 편집툴의 좌표계
*	\param t 정점의 이동 변위
*	\param q 정점의 회전 변위
*	\param s 정점의 스케일 변위
*	\param pParentMesh 정점이 속한 메쉬의 부모 메쉬
*/
void GVertex::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	// 편집 변환을 위한 정적 변수를 선언하고,
	static GTransf MC2MC;

	// 편집 객체의 클래스 이름을 구하여,
	std::string ClsName = pMesh->GetClassName();

	// GMesh 객체의 정점일 경우,
	if (ClsName == "GMesh")
	{
		// 편집되는 첫 번째 정점이거나 메쉬 삼각형의 위치가 변경되었다면,
		if (pMesh->ModifiedFlag == MODIFIED_NONE || pMesh->ModifiedFlag == MODIFIED_FACE_POS)
		{
			// 편집 변환을 계산하고,
			MC2MC = inv(pMesh->MC) * EC * GTransf(t, q) * inv(EC) * pMesh->MC;

			// 정점이 편집되었음을 기록하고,
			pMesh->ModifiedFlag = MODIFIED_VERT_POS;

			// 정점의 메쉬가 제어 메쉬라면,
			if (pMesh->SemanticType == MESH_CTRL && !pMesh->LinkedMeshNameList.empty())
			{
				// 링크된 블렌딩 메쉬를 구하여,
				GBlendMesh *pBlendMesh = (GBlendMesh *)(FindMesh(pMesh->pScene->MeshList, pMesh->LinkedMeshNameList[0]));
				if (pBlendMesh != NULL)
				{
					// 블렌딩 메쉬의 보간 정점이 변경되었음을 기록한다.
					pBlendMesh->ModifiedFlag = MODIFIED_INTPT_POS;
					int NumFace = pMesh->GetNumFace();
					for (int i = 0; i < NumFace; ++i)
						pBlendMesh->PatchType[i] |= PATCH_TYPE_MODIFIED;
				}
				else
					pMesh->LinkedMeshNameList.clear();
			}
		}

		// 편집된 정점의 좌표를 계산하여,
		GPoint3 pt = MC2MC * P;

		// 스케일 변환을 수행하고,
		if (s[0] != 1.0) pt[0] *= s[0];
		if (s[1] != 1.0) pt[1] *= s[1];
		if (s[2] != 1.0) pt[2] *= s[2];

		// 최종 정점의 위치를 수정하고,
		P[0] = (1.0 - EditWgt) * P[0] + EditWgt * pt[0];
		P[1] = (1.0 - EditWgt) * P[1] + EditWgt * pt[1];
		P[2] = (1.0 - EditWgt) * P[2] + EditWgt * pt[2];

		// 정점 버퍼의 값을 선택적으로 변경한다.
		pMesh->VtxBuf[Idx * 3] = P[0];
		pMesh->VtxBuf[Idx * 3 + 1] = P[1];
		pMesh->VtxBuf[Idx * 3 + 2] = P[2];
	}

	// GNurbsCrv 객체의 정점일 경우,
	if (ClsName == "GNurbsCrv")
	{
		// GNurbsCrv 객체의 포인터를 구하여,
		GNurbsCrv *pCrv = (GNurbsCrv *)pMesh;

		// 편집되는 첫 번째 정점이라면,
		if (pCrv->ModifiedFlag == MODIFIED_NONE)
		{
			// 편집 변환을 계산하고,
			MC2MC = inv(pCrv->MC) * EC * GTransf(t, q) * inv(EC) * pCrv->MC;

			// 정점의 편집을 기록하고,
			pCrv->ModifiedFlag = MODIFIED_CTLPT_POS;						
		}

		// 편집된 변위를 계산하여,
		GPoint3 pt = MC2MC * P;
		GVector3 dp = pt - P;

		// 선택된 곡선을 편집한다.
		double t = pMesh->Texs[Idx][1];
		pCrv->Edit(t, cast_vec4(dp, 0.0));		
	}

	// GNurbsSrf 객체의 정점일 경우,
	if (ClsName == "GNurbsSrf")
	{
		// GNurbsSrf 객체의 포인터를 구하여,
		GNurbsSrf *pSrf = (GNurbsSrf *)pMesh;

		// 편집되는 첫 번째 정점이라면,
		if (pSrf->ModifiedFlag == MODIFIED_NONE)
		{
			// 편집 변환을 계산하고,
			MC2MC = inv(pSrf->MC) * EC * GTransf(t, q) * inv(EC) * pSrf->MC;

			// 정점의 편집을 기록하고,
			pSrf->ModifiedFlag = MODIFIED_CTLPT_POS;						
		}

		// 편집된 변위를 계산하여,
		GPoint3 pt = MC2MC * P;
		GVector3 dp = pt - P;

		// 선택된 곡면을 편집한다.
		double u = pMesh->Texs[Idx][0];
		double v = pMesh->Texs[Idx][1];
		pSrf->Edit(u, v, cast_vec4(dp, 0.0));
	}
}

/*!
*	\brief 편집 객체를 렌더링 한다.
*/
void GVertex::RenderForEdit(float r, float g, float b)
{
	// 현재 좌표계: 월드(장면)
	glPushMatrix();
	{
		// 정점을 포함한 메쉬 객체의 모델링 좌표계로 변환
		glMultMatrixd(cast_mat(pMesh->MC).M);

		int RenderMode = 0;
		switch (RenderMode)
		{
			// 포인트 렌더링
		case 0:
			{
				// 블렌딩 모드로 진입하고,
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

				// 블렌딩 모드에서 나온다.
				glDisable(GL_BLEND);
				glDisable(GL_POINT_SMOOTH);
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_LIGHTING);
			}
			break;

			// 구 렌더링
		case 1:
			{
				// 선택된 정점을 그린다.
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
*	\brief 정점의 디폴트 생성자
*
*	\param _P 정점의 좌표
*	\param _N 정점의 법선벡터
*	\param _C 정점의 색상
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
*	\brief 정점의 복사 생성자
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
*	\brief 정점의 소멸자
*/
GVertex::~GVertex()
{
}

/*!
*	\brief 정점의 대입연산자
*
*	\param rhs 대입연산자의 오른쪽 피연산자
*
*	\return 대입된 자신을 반환한다.
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
*	\brief 정점의 등호연산자
*
*	\param lhs 왼쪽 피연산자
*	\param rhs 오른쪽 피연산자
*
*	\return 대입된 자신을 반환한다.
*/
bool operator ==(const GVertex &lhs, const GVertex &rhs)
{
	return (lhs.P == rhs.P);
}

/*!
*	\brief 본 메쉬에 대한 정점의 바인딩 정보를 생성하여 추가한다.
*
*	\param BoneMeshName 정점이 바인딩 된 본 메쉬의 이름
*	\param Wgt 정점의 바인딩 가중치
*/
void GVertex::AddRefBoneInfo(std::string &BoneMeshName, double Wgt)
{
	GRefBoneInfo NewInfo(BoneMeshName, Wgt, GPoint3(), GVector3());
	BoneInfoList.push_back(NewInfo);
}

/*********************/
/* GFace 클래스 구현 */
/*********************/
IMPLEMENT_GEDIT_OBJECT(GFace)

/*!
*	\brief 삼각형이 포함된 메쉬의 포인터를 반환한다.
*
*	\return 삼각형이 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GFace::GetRefMesh()
{
	return pMesh;
}

/*!
*	\brief 삼각형의 무게중심에서 정의된 좌표계를 반환한다.
*
*	\return 삼각형의 무게중심에서 정의된 좌표계(월드 좌표계 기준)를 반환한다.
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
*	\brief 삼각형의 무게 중심을 계산한다.
*
*	\param bLocal 객체의 모델링 좌표계에서 표현된 경우 true, 월드 좌표계에서 표현될 경우 false
*
*	\return 삼각형의 무게 중심의 위치를 반환한다.
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
*	\brief 삼각형의 법선 벡터를 반환한다.
*
*	\param bLocal 객체의 모델링 좌표계에서 표현된 법선일 경우 true, 월드 좌표계에서 표현될 경우 false
*
*	\return 삼각형의 법선 벡터를 반환한다.
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
*	\brief 삼각형과 삼각형이 포함된 객체을 편집한다.
*
*	\param EC 편집툴의 좌표계
*	\param t 삼각형의 이동 변위
*	\param q 삼각형의 회전 변위
*	\param s 삼각형의 스케일 변위
*	\param pParentMesh 편집 객체의 부모 메쉬
*/
void GFace::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	// 메쉬를 상속받은 객체들(GNurbsCrv, GNurbsSrf, GUcbsSrf, GUcbsVol, GSweepSrf, GBlendCrv) 등의 
	// 직접제어는 추후 구현할 예정임.
	if (pMesh->GetClassName() != "GMesh")
		return;

	// 편집 변환을 위한 정적 변수를 선언하고,
	static GTransf MC2MC;
	static std::vector<bool> VertFlag;

	// 편집되는 첫 번째 삼각형이라면,
	if (pMesh->ModifiedFlag == MODIFIED_NONE)
	{
		int NumVert = pMesh->GetNumVert();
		VertFlag.clear();
		VertFlag.assign(NumVert, false);

		// 편집 변환을 계산하고,
		MC2MC = inv(pMesh->MC) * EC * GTransf(t, q) * inv(EC) * pMesh->MC;

		// 메쉬 삼각형의 위치가 편집됨을 기록하고,
		pMesh->ModifiedFlag = MODIFIED_FACE_POS;

		// 삼각형의 메쉬가 제어 메쉬라면,
		if (pMesh->SemanticType == MESH_CTRL && !pMesh->LinkedMeshNameList.empty())
		{
			// 링크된 블렌딩 메쉬를 구하여,
			GBlendMesh *pBlendMesh = (GBlendMesh *)(FindMesh(pMesh->pScene->MeshList, pMesh->LinkedMeshNameList[0]));
			if (pBlendMesh != NULL)
			{
				// 블렌딩 메쉬의 보간 정점이 변경되었음을 기록한다.
				pBlendMesh->ModifiedFlag = MODIFIED_INTPT_POS;
				int NumFace = pMesh->GetNumFace();
				for (int i = 0; i < NumFace; ++i)
					pBlendMesh->PatchType[i] |= PATCH_TYPE_MODIFIED;
			}
			else
				pMesh->LinkedMeshNameList.clear();
		}
	}

	// 삼각형의 세 정점의 좌표를 편집한다.
	if (VertFlag[vIdx[0]] == false)
	{
		VertFlag[vIdx[0]] = true;
		pMesh->Verts[vIdx[0]].P = MC2MC * pMesh->Verts[vIdx[0]].P;

		// 정점 버퍼의 값을 선택적으로 변경한다.
		pMesh->VtxBuf[vIdx[0] * 3] = pMesh->Verts[vIdx[0]].P[0];
		pMesh->VtxBuf[vIdx[0] * 3 + 1] = pMesh->Verts[vIdx[0]].P[1];
		pMesh->VtxBuf[vIdx[0] * 3 + 2] = pMesh->Verts[vIdx[0]].P[2];
	}
	if (VertFlag[vIdx[1]] == false)
	{
		VertFlag[vIdx[1]] = true;
		pMesh->Verts[vIdx[1]].P = MC2MC * pMesh->Verts[vIdx[1]].P;
		
		// 정점 버퍼의 값을 선택적으로 변경한다.
		pMesh->VtxBuf[vIdx[1] * 3] = pMesh->Verts[vIdx[1]].P[0];
		pMesh->VtxBuf[vIdx[1] * 3 + 1] = pMesh->Verts[vIdx[1]].P[1];
		pMesh->VtxBuf[vIdx[1] * 3 + 2] = pMesh->Verts[vIdx[1]].P[2];
	}
	if (VertFlag[vIdx[2]] == false)
	{
		VertFlag[vIdx[2]] = true;
		pMesh->Verts[vIdx[2]].P = MC2MC * pMesh->Verts[vIdx[2]].P;
		
		// 정점 버퍼의 값을 선택적으로 변경한다.
		pMesh->VtxBuf[vIdx[2] * 3] = pMesh->Verts[vIdx[2]].P[0];
		pMesh->VtxBuf[vIdx[2] * 3 + 1] = pMesh->Verts[vIdx[2]].P[1];
		pMesh->VtxBuf[vIdx[2] * 3 + 2] = pMesh->Verts[vIdx[2]].P[2];
	}	
}

/*!
*	\brief 편집 삼각형을 렌더링 한다.
*/
void GFace::RenderForEdit(float r, float g, float b)
{
	// 블렌딩 기능을 활성하고, 조명을 비활성화 한다.
	glDisable(GL_LIGHTING);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(0.3, 0.3);

	// 현재 좌표계: 월드(장면)
	glPushMatrix();
	{
		// 메쉬 좌표계로 변환
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

	// 오프셋 기능을 비활성하고, 조명을 활성화 한다.
	glEnable(GL_LIGHTING);
	glDisable(GL_POLYGON_OFFSET_FILL);
}

/*!
*	\brief 디폴트 생성자
*
*	\param vidx0 삼각형을 구성하는 첫번째 정점의 인덱스
*	\param vidx1 삼각형을 구성하는 두번째 정점의 인덱스
*	\param vidx2 삼각형을 구성하는 세번째 정점의 인덱스
*	\param tidx0 삼각형을 구성하는 첫번째 정점이 참조하는 텍스처 좌표 인덱스
*	\param tidx1 삼각형을 구성하는 두번째 정점이 참조하는 텍스처 좌표 인덱스
*	\param tidx2 삼각형을 구성하는 세번째 정점이 참조하는 텍스처 좌표 인덱스
*	\param mtl_idx 삼각형에 적용될 재질의 인덱스
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
*	\brief 복사생성자
*
*	\cpy 복사될 객체
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
*	\brief 소멸자
*/
GFace::~GFace()
{
}

/*!
*	\brief 대입 연산자 중복
*
*	\param rhs 대입 연산자의 오른쪽 객체
*
*	\return 대입된 자신을 반환한다
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
*	\brief 삼각형을 구성하는 \a vidx 이전 정점의 인덱스를 반환한다.
*	\note 만약에 \a vidx가 삼각형의 정점의 인덱스가 아니라면 -1을 반환한다.
*
*	\param vidx 현재 정점의 인덱스
*
*	\return 삼각형을 구성하는 \a vidx 이전 정점의 인덱스를 반환한다.
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
*	\brief 삼각형을 구성하는 \a vidx 다음 정점의 인덱스를 반환한다.
*	\note 만약에 \a vidx가 삼각형의 정점의 인덱스가 아니라면 -1을 반환한다.
*
*	\param vidx 현재 정점의 인덱스
*
*	\return 삼각형을 구성하는 \a vidx 다음 정점의 인덱스를 반환한다.
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
*	\brief 두 삼각형의 인접한 모서리의 정점 인덱스를 찾는다.
*
*	\param pAdjFace 조사할 삼각형
*	\param sidx 현재 삼각형의 정점의 시작 인덱스가 저장됨
*	\param eidx 현재 삼각형의 정점의 끝 인덱스가 저장됨
*
*	\return 삼각형이 인접한 경우 1, 삼각형이 한 정점만 공유하는 경우 0, 삼각형이 인접하지 않는 경우 -1을 반환한다.
*/
int GFace::GetSharedVertIdx(GFace *pAdjFace, int &sidx, int &eidx)
{
	// 01 이 공통 인덱스인 경우,
	if ((vIdx[0] == pAdjFace->vIdx[0] && vIdx[1] == pAdjFace->vIdx[2]) ||
		(vIdx[0] == pAdjFace->vIdx[2] && vIdx[1] == pAdjFace->vIdx[1]) ||
		(vIdx[0] == pAdjFace->vIdx[1] && vIdx[1] == pAdjFace->vIdx[0]))
	{
		sidx = vIdx[0];
		eidx = vIdx[1];

		return 1;
	}

	// 12가 공통 인덱스인 경우,
	if ((vIdx[1] == pAdjFace->vIdx[0] && vIdx[2] == pAdjFace->vIdx[2]) ||
		(vIdx[1] == pAdjFace->vIdx[2] && vIdx[2] == pAdjFace->vIdx[1]) ||
		(vIdx[1] == pAdjFace->vIdx[1] && vIdx[2] == pAdjFace->vIdx[0]))
	{
		sidx = vIdx[1];
		eidx = vIdx[2];

		return 1;
	}

	// 20 이 공통 인덱스인 경우,
	if ((vIdx[2] == pAdjFace->vIdx[0] && vIdx[0] == pAdjFace->vIdx[2]) ||
		(vIdx[2] == pAdjFace->vIdx[2] && vIdx[0] == pAdjFace->vIdx[1]) ||
		(vIdx[2] == pAdjFace->vIdx[1] && vIdx[0] == pAdjFace->vIdx[0]))
	{
		sidx = vIdx[2];
		eidx = vIdx[0];

		return 1;
	}

	// 정점 vIdx[0]를 공유하는 경우,
	if (vIdx[0] == pAdjFace->vIdx[0] || vIdx[0] == pAdjFace->vIdx[1] || vIdx[0] == pAdjFace->vIdx[2])
	{
		sidx = vIdx[0];
		eidx = vIdx[1];

		return 0;
	}

	// 정점 vIdx[1]를 공유하는 경우,
	if (vIdx[1] == pAdjFace->vIdx[0] || vIdx[1] == pAdjFace->vIdx[1] || vIdx[1] == pAdjFace->vIdx[2])
	{
		sidx = vIdx[1];
		eidx = vIdx[2];

		return 0;
	}

	// 정점 vIdx[2]를 공유하는 경우,
	if (vIdx[2] == pAdjFace->vIdx[0] || vIdx[2] == pAdjFace->vIdx[1] || vIdx[2] == pAdjFace->vIdx[2])
	{
		sidx = vIdx[2];
		eidx = vIdx[0];

		return 0;
	}

	// 두 삼각형이 공유하는 정점이 없다면, -1을 반환한다.
	return -1;
}

/*************************/
/* GPtOnFace 클래스 구현 */
/*************************/
IMPLEMENT_GEDIT_OBJECT(GPtOnFace)

/*!
*	\brief 선택 점이 포함된 메쉬의 포인터를 반환한다.
*
*	\return 선택 점이 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GPtOnFace::GetRefMesh()
{
	return pFace->pMesh;
}

/*!
*	\brief 월드 좌표계에서 표현된 선택 점의 모델링 좌표계를 반환한다.
*
*	\return 월드 좌표계에서 표현된 선택 점의 모델링 좌표계를 반환한다.
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
*	\brief 선택된 점을 편집한다.
*
*	\param EC 편집 좌표계
*	\param t 편집될 객체의 이동 변위
*	\param q 편집될 객체의 회전 변위
*	\param s 편집될 객체의 스케일 변위
*	\param pParentMesh 편집 객체의 부모 메쉬
*/
void GPtOnFace::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	// 선택 점의 메쉬와 클래스 이름을 구하여,
	GMesh *pMesh = pFace->pMesh;
	std::string ClsName = pMesh->GetClassName();

	// 편집 변환을 위한 정적 변수를 선언하고,
	static GTransf MC2MC;

	// GMesh 객체의 정점일 경우,
	if (ClsName == "GMesh")
	{
		// 편집되는 첫 번째 정점이라면,
		if (pMesh->ModifiedFlag == MODIFIED_NONE)
		{
			// 편집 변환을 계산하고,
			MC2MC = inv(pMesh->MC) * EC * GTransf(t, q) * inv(EC) * pMesh->MC;

			// 정점이 편집되었음을 기록하고,
			pMesh->ModifiedFlag = MODIFIED_VERT_POS;

			// 정점의 메쉬가 제어 메쉬라면,
			if (pMesh->SemanticType == MESH_CTRL && !pMesh->LinkedMeshNameList.empty())
			{
				// 링크된 블렌딩 메쉬를 구하여,
				GBlendMesh *pBlendMesh = (GBlendMesh *)(FindMesh(pMesh->pScene->MeshList, pMesh->LinkedMeshNameList[0]));
				if (pBlendMesh != NULL)
				{
					// 블렌딩 메쉬의 보간 정점이 변경되었음을 기록한다.
					pBlendMesh->ModifiedFlag = MODIFIED_INTPT_POS;
					int NumFace = pMesh->GetNumFace();
					for (int i = 0; i < NumFace; ++i)
						pBlendMesh->PatchType[i] |= PATCH_TYPE_MODIFIED;
				}
				else
					pMesh->LinkedMeshNameList.clear();
			}
		}

		// 편집 점이 포함된 삼각형의 세 정점을 구하여,
		int *vidx = pFace->vIdx;
		GPoint3 p0 = pMesh->Verts[vidx[0]].P;
		GPoint3 p1 = pMesh->Verts[vidx[1]].P;
		GPoint3 p2 = pMesh->Verts[vidx[2]].P;
		
		// 편집된 세 정점의 위치를 구하고,
		p0 = MC2MC * p0;
		p1 = MC2MC * p1;
		p2 = MC2MC * p2;

		// 최종 정점의 위치를 수정하고,
		pMesh->Verts[vidx[0]].P = p0;
		pMesh->Verts[vidx[1]].P = p1;
		pMesh->Verts[vidx[2]].P = p2;
		
		// 정점 버퍼의 값을 선택적으로 변경한다.
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

	// GNurbsCrv 객체의 정점일 경우,
	if (ClsName == "GNurbsCrv")
	{
		// GNurbsCrv 객체의 포인터를 구하여,
		GNurbsCrv *pCrv = (GNurbsCrv *)pMesh;

		// 편집되는 첫 번째 정점이라면,
		if (pCrv->ModifiedFlag == MODIFIED_NONE)
		{
			// 편집 변환을 계산하고,
			MC2MC = inv(pCrv->MC) * EC * GTransf(t, q) * inv(EC) * pCrv->MC;

			// 정점의 편집을 기록하고,
			pCrv->ModifiedFlag = MODIFIED_CTLPT_POS;						
		}

		// 편집 점이 포함된 삼각형의 세 정점과 편집 점의 좌표를 구하고,
		int *vidx = pFace->vIdx;
		GPoint3 p0 = pMesh->Verts[vidx[0]].P;
		GPoint3 p1 = pMesh->Verts[vidx[1]].P;
		GPoint3 p2 = pMesh->Verts[vidx[2]].P;
		GPoint3 p = p2 + (p0 - p2) * Coords[0] + (p1 - p2) * Coords[1];

		// 텍스처 좌표값을 계산하여,
		GVector2 st0 = pMesh->Texs[vidx[0]];
		GVector2 st1 = pMesh->Texs[vidx[1]];
		GVector2 st2 = pMesh->Texs[vidx[2]];
		GVector2 st = st2 + (st0 - st2) * Coords[0] + (st1 - st2) * Coords[1];

		// 편집된 변위를 계산하여,
		GPoint3 q = MC2MC * p;
		GVector3 dp = q - p;

		// 선택된 곡선을 편집한다.
		double t = st[1];
		pCrv->Edit(t, cast_vec4(dp, 0.0));
	}

	// GNurbsSrf 객체의 정점일 경우,
	if (ClsName == "GNurbsSrf")
	{
		// GNurbsSrf 객체의 포인터를 구하여,
		GNurbsSrf *pSrf = (GNurbsSrf *)pMesh;

		// 편집되는 첫 번째 정점이라면,
		if (pSrf->ModifiedFlag == MODIFIED_NONE)
		{
			// 편집 변환을 계산하고,
			MC2MC = inv(pSrf->MC) * EC * GTransf(t, q) * inv(EC) * pSrf->MC;

			// 정점의 편집을 기록하고,
			pSrf->ModifiedFlag = MODIFIED_CTLPT_POS;						
		}

		// 편집 점이 포함된 삼각형의 세 정점과 편집 점의 좌표를 구하고,
		int *vidx = pFace->vIdx;
		GPoint3 p0 = pMesh->Verts[vidx[0]].P;
		GPoint3 p1 = pMesh->Verts[vidx[1]].P;
		GPoint3 p2 = pMesh->Verts[vidx[2]].P;
		GPoint3 p = p2 + (p0 - p2) * Coords[0] + (p1 - p2) * Coords[1];

		// 텍스처 좌표값을 계산하여,
		GVector2 st0 = pMesh->Texs[vidx[0]];
		GVector2 st1 = pMesh->Texs[vidx[1]];
		GVector2 st2 = pMesh->Texs[vidx[2]];
		GVector2 st = st2 + (st0 - st2) * Coords[0] + (st1 - st2) * Coords[1];

		// 편집된 변위를 계산하여,
		GPoint3 q = MC2MC * p;
		GVector3 dp = q - p;

		// 선택된 곡면을 편집한다.
		pSrf->Edit(st[0], st[1], cast_vec4(dp, 0.0));
	}
}

/*!
*	\brief 선택 점을 렌더링 한다.
*/
void GPtOnFace::RenderForEdit(float r, float g, float b)
{
	int RenderType = 1;

	switch (RenderType)
	{
	case 0:	// 포인트 렌더링
		{
			// 조명과 깊이 테스트를 비활성화 하고,
			glDisable(GL_LIGHTING);
			glDisable(GL_DEPTH_TEST);

			// 월드 좌표계에서
			glPushMatrix();
			{
				// 선택 점의 모델링 좌표계로 변환하고,
				GTransf MC = GetTransf();
				glMultMatrixd(cast_mat(MC).M);		

				// 선택 점을 표시하고,
				glPointSize(8.0);
				glBegin(GL_POINTS);
				{
					glColor3f(0.0, 0.0, 0.0);
					glVertex3f(0.0, 0.0, 0.0);
				}
				glEnd();
				glPointSize(1.0);
			}
			// 월드 좌표계로 복원한다.
			glPopMatrix();

			// 조명과 깊이 테스트를 활성화 한다.
			glEnable(GL_LIGHTING);
			glEnable(GL_DEPTH_TEST);

		}
		break;

	case 1:	// 구 렌더링
		{
			// 월드 좌표계에서
			glPushMatrix();
			{
				// 선택 점의 모델링 좌표계로 변환하고,
				GTransf MC = GetTransf();
				glMultMatrixd(cast_mat(MC).M);		
				
				// 재질을 설정하고,
				GMaterial mtl_blue;
				mtl_blue.SetAmbient(0.2, 0.2, 0.2);
				mtl_blue.SetDiffuse(0.0, 0.0, 1.0);
				mtl_blue.SetSpecular(0.5, 0.5, 0.5);
				mtl_blue.Enable(false);

				// 구의 크기를 계산하여 렌더링한다.
				GMesh *pMesh = pFace->pMesh;
				double size = pMesh->BBox.GetDiagonal() * 0.003;
				glutSolidSphere(size, 20, 20);
			}
			// 월드 좌표계로 복원한다.
			glPopMatrix();
		}
		break;
	}
}

/*!
*	\brief 생성자
*
*	\param _pFace 선택 점이 속한 삼각형에 대한 포인터
*	\param _Coords 삼각형의 세 정점을 기준으로 표현된 선택 점의 barycentric 좌표
*/
GPtOnFace::GPtOnFace(GFace *_pFace, GVector3 _Coords)
{
	pFace = _pFace;
	Coords = _Coords;
}

/*!
*	\brief 복사생성자
*
*	\cpy 복사될 객체
*/
GPtOnFace::GPtOnFace(const GPtOnFace &cpy)
{
	pFace = cpy.pFace;
	Coords = cpy.Coords;
}

/*!
*	\brief 소멸자
*/
GPtOnFace::~GPtOnFace()
{
}

/*!
*	\brief 대입연산자 중복
*
*	\param rhs 대입연산자의 오른쪽 객체
*
*	\return 대입된 자신을 반환한다
*/
GPtOnFace &GPtOnFace::operator =(const GPtOnFace &rhs)
{
	pFace = rhs.pFace;
	Coords = rhs.Coords;
	
	return *this;
}

/*!
*	\brief 편집 정점의 정보를 설정한다.
*
*	\param _pFace 편집 정점이 속하는 삼각형에 대한 포인터
*	\param _Coords 삼각형의 정점을 기준으로 표현된 편집 정점의 barycentric 좌표
*/
void GPtOnFace::Set(GFace *_pFace, GVector3 _Coords)
{
	pFace = _pFace;
	Coords = _Coords;
}

/*****************************/
/* GRenderer 클래스 구현 */
/*****************************/
/*!
*	\brief 디폴트 생성자
*/
GRenderer::GRenderer()
{
	// 메쉬 렌더링 속성(5)
	bBackFaceCull = FALSE;
	bTexture = FALSE;
	RenderType = RENDER_POLYGON;
	ShadeType = SHADE_SMOOTH;
	VertColorType = VERTEX_COLOR_ILLUMINATION;

	// 곡선/곡면/볼륨 렌더링 속성(13)
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

	// 스윕 렌더링 속성(2)
	SwpPlnWidth = 10.0;
	SwpPlnHeight = 10.0;
}

/*!
*	\brief 복사 생성자
*	
*	\param cpy 복사할 객체
*/
GRenderer::GRenderer(const GRenderer &cpy)
{
	// 메쉬 렌더링 속성(5)
	bBackFaceCull = cpy.bBackFaceCull;
	bTexture = cpy.bTexture;
	RenderType = cpy.RenderType;
	ShadeType = cpy.ShadeType;
	VertColorType = cpy.VertColorType;

	// 곡선/곡면/볼륨 렌더링 속성(13)
	NumSampU = cpy.NumSampU;
	NumSampV = cpy.NumSampV;
	NumSampW = cpy.NumSampW;
	bRenderCrvAsLine = cpy.bRenderCrvAsLine;
	ARR_COPY(IsoSrfParam, cpy.IsoSrfParam, 6);
	CtrlPtSize = cpy.CtrlPtSize;
	NodePts = cpy.NodePts;
	CrvMtlIdx = cpy.CrvMtlIdx;

	// 스윕 렌더링 속성(2)
	SwpPlnWidth = cpy.SwpPlnWidth;
	SwpPlnHeight = cpy.SwpPlnHeight;	
}

/*!
*	\brief 소멸자
*/
GRenderer::~GRenderer()
{
}

/*!
*	\brief 대입연산자
*
*	\param rhs 대입될 객체
*	
*	\return 대입된 자신을 반환
*/
GRenderer &GRenderer::operator =(const GRenderer &rhs)
{
	// 메쉬 렌더링 속성(5)
	bBackFaceCull = rhs.bBackFaceCull;
	bTexture = rhs.bTexture;
	RenderType = rhs.RenderType;
	ShadeType = rhs.ShadeType;
	VertColorType = rhs.VertColorType;

	// 곡선/곡면/볼륨 렌더링 속성(13)
	NumSampU = rhs.NumSampU;
	NumSampV = rhs.NumSampV;
	NumSampW = rhs.NumSampW;
	bRenderCrvAsLine = rhs.bRenderCrvAsLine;
	ARR_COPY(IsoSrfParam, rhs.IsoSrfParam, 6);
	CtrlPtSize = rhs.CtrlPtSize;
	NodePts = rhs.NodePts;
	CrvMtlIdx = rhs.CrvMtlIdx;

	// 스윕 렌더링 속성(2)
	SwpPlnWidth = rhs.SwpPlnWidth;
	SwpPlnHeight = rhs.SwpPlnHeight;	

	return *this;
}

/*!
*	\brief 메쉬의 가시화 여부를 검사한다.
*
*	\return 렌더링 된다면 true, 아니면 false를 반환한다.
*/
bool GRenderer::IsVisibleMesh()
{
	if ((RenderType & RENDER_POLYGON) || (RenderType & RENDER_WIRE) || (RenderType & RENDER_POINT))
		return true;
	else
		return false;
}

/*!
*	\brief 객체의 샘플링 수를 설정한다.
*
*	\param NumU U-방향 샘플링 수
*	\param NumV V-방향 샘플링 수
*	\param NumW W-방향 샘플링 수
*/
void GRenderer::SetSampleNum(int NumU, int NumV, int NumW)
{
	NumSampU = NumU;
	NumSampV = NumV;
	NumSampW = NumW;
}

/*************************/
/* GMaterial 클래스 구현 */
/*************************/
/*!
*	\brief 재질의 생성자
*
*	\param _Name 재질의 이름
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
*	\brief 복사 생성자
*
*	\param cpy 복사될 재질 객체
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
*	\brief 소멸자
*/
GMaterial::~GMaterial()
{
}

/*!
*	\brief 대입 연산자
*
*	\param rhs 대입 연산자의 오른쪽 객체
*
*	\return 대입된 자신을 반환한다.
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
*	\brief 재질의 주변광 반사계수를 설정한다.
*
*	\param r 재질의 붉은색 주변광 반사계수
*	\param g 재질의 초록색 주변광 반사계수
*	\param b 재질의 파란색 주변광 반사계수
*/
void GMaterial::SetAmbient(GLfloat r, GLfloat g, GLfloat b)
{
	Ambient[0] = r;
	Ambient[1] = g;
	Ambient[2] = b;
	Ambient[3] = 1.0f;
}

/*!
*	\brief 재질의 난반사 계수를 설정한다.
*
*	\param r 재질의 붉은색 난반사 계수
*	\param g 재질의 초록색 난반사 계수
*	\param b 재질의 파란색 난반사 계수
*/
void GMaterial::SetDiffuse(GLfloat r, GLfloat g, GLfloat b)
{
	Diffuse[0] = r;
	Diffuse[1] = g;
	Diffuse[2] = b;
	Diffuse[3] = 1.0f;
}

/*!
*	\brief 재질의 전반사 계수를 설정한다.
*
*	\param r 재질의 붉은색 전반사 계수
*	\param g 재질의 초록색 전반사 계수
*	\param b 재질의 파란색 전반사 계수
*/
void GMaterial::SetSpecular(GLfloat r, GLfloat g, GLfloat b)
{
	Specular[0] = r;
	Specular[1] = g;
	Specular[2] = b;
	Specular[3] = 1.0f;
}

/*!
*	\brief 재질의 전반사 지수를 설정한다.
*
*	\param shiness 재질의 전반사 지수
*/
void GMaterial::SetShiness(GLfloat shiness)
{
	Shiness = shiness;
}

/*!
*	\brief 재질의 불투명도를 설정한다.
*
*	\param alpha 재질의 불투명도 (0: 투명, 1: 불투명)
*/
void GMaterial::SetAlpha(GLfloat alpha)
{
	Ambient[3] = alpha;
	Diffuse[3] = alpha;
	Specular[3] = alpha;
}

/*!
*	\brief 재질의 텍스처를 설정한다.
*	\note 반드시 OpenGL 렌더링 상태에서 호출되어야 한다.
*
*	\param fname 텍스처 파일명
*/
void GMaterial::LoadTexture(const char *fname)
{
	// 텍스처 파일의 확장자를 구하여
	std::string file_ext = get_file_ext(fname);

	// 확장자가 *.tga 파일은 경우,
	if (file_ext == "tga")
	{
		// 텍스처 파일명을 저장하고,
		TexFileName = std::string(fname);

		// 이미지 데이터를 로드하여,
		int w, h;
		GLenum format;
		unsigned char *imageData = load_image_from_tga(fname, &w, &h, &format);

		// 텍스처를 생성한다.
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

	// 확장자가 *.bmp 파일은 경우,
	if (file_ext == "bmp")
	{
		// 텍스처 파일명을 저장하고,
		TexFileName = std::string(fname);
											
		// 이미지 데이터를 로드하여,
		BITMAPINFO	*info;
		unsigned char *imageData = load_image_from_bmp(fname, &info);
	
		// 텍스처를 생성한다.
		if (imageData != NULL)
		{
			// BGR을 RGB로 변환한다.
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
*	\brief 메쉬의 재질 속성을 활성화 한다.
*
*	\param texture 재질이 텍스춰를 포함할 경우, 텍스처 활성화 여부
*/
void GMaterial::Enable(bool texture)
{
	// 블렌딩을 활성화하고,
	if (Diffuse[3] != 1.0f)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	// 재질을 적용한다.
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, Shiness);

	// 재질이 텍스처를 사용할 경우 텍스처 바인딩을 활성화 한다.
	if (texture && BindTexId != 0)
		glBindTexture(GL_TEXTURE_2D, BindTexId);
}

/*!
*	\brief 메쉬의 재질 속성을 비활성화 한다.
*/
void GMaterial::Disable()
{
	// 블렌딩을 활성화하고,
	if (Diffuse[3] != 1.0f)
		glDisable(GL_BLEND);
}

/***********************/
/* GBndBox 클래스 구현 */
/***********************/
/*!
*	\brief 생성자
*
*	\param minx 경계 상자의 최소점의 x 좌표
*	\param miny 경계 상자의 최소점의 y 좌표
*	\param minz 경계 상자의 최소점의 z 좌표
*	\param maxx 경계 상자의 최대점의 x 좌표
*	\param maxy 경계 상자의 최대점의 y 좌표
*	\param maxz 경계 상자의 최대점의 z 좌표
*/
GBndBox::GBndBox(double minx, double miny, double minz, double maxx, double maxy, double maxz)
{
	pMesh = NULL;
	MinPt.Set(minx, miny, minz);
	MaxPt.Set(maxx, maxy, maxz);
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사될 객체
*/
GBndBox::GBndBox(const GBndBox &cpy)
{
	pMesh = cpy.pMesh;
	MinPt = cpy.MinPt;
	MaxPt = cpy.MaxPt;
}

/*!
*	\brief 대입 연산자
*
*	\param rhs 대입될 객체
*
*	\return 대입된 자신을 반환한다.
*/
GBndBox &GBndBox::operator =(const GBndBox &rhs)
{
	pMesh = rhs.pMesh;
	MinPt = rhs.MinPt;
	MaxPt = rhs.MaxPt;

	return *this;
}

/*!
*	\brief 월드 좌표계에서 표현된 경계 상자의 모델링 좌표계를 반환한다.
*
*	\return 월드 좌표계에서 표현된 경계 상자의 모델링 좌표계를 반환한다.
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
*	\brief 경계상자의 x축 방향의 너비를 반환한다.
*/
double GBndBox::GetWidth()
{
	return ABS(MaxPt[0] - MinPt[0]);
}

/*!
*	\brief 경계상자의 y축 방향의 높이를 반환한다.
*/
double GBndBox::GetHeight()
{
	return ABS(MaxPt[1] - MinPt[1]);
}

/*!
*	\brief 경계상자의 z축 방향의 깊이를 반환한다.
*/
double GBndBox::GetDepth()
{
	return ABS(MaxPt[2] - MinPt[2]);
}

/*!
*	\brief 경계상자의 대각선의 길이를 반환한다.
*/
double GBndBox::GetDiagonal()
{
	return dist(MinPt, MaxPt);
}

/*!
*	\brief 경계상자의 8개의 모서리 정점을 반환한다.
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
*	\param Pt 경계상자의 8개의 모서리 위치가 저장된다.
*	\param bLocal 모서리 정점의 좌표계(true: 모델링 좌표계, false: 월드 좌표계)
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
*	\brief 경계 상자를 렌더링 한다.
*
*	\param r 경계 상자 테두리의 빨간색 성분
*	\param g 경계 상자 테두리의 초록색 성분
*	\param b 경계 상자 테두리의 파란색 성분
*	\param width 경계 상자 테두리의 두께
*/
void GBndBox::Render(double r, double g, double b, double width)
{
	// 현재 좌표계: 월드(장면)
	glPushMatrix();
	{
		// 경계 상자의 모델링 좌표계로 변환
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
/* GMesh 클래스 구현 */
/*********************/
IMPLEMENT_GMESH_OBJECT(GMesh)

/*!
*	\brief 객체가 포함된 메쉬의 포인터를 반환한다.
*
*	\return 객체가 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GMesh::GetRefMesh()
{
	return this;
}

/*!
*	\brief 월드 좌표계에서 표현된 객체의 모델링 좌표계를 반환한다.
*
*	\return 월드 좌표계에서 표현된 객체의 모델링 좌표계를 반환한다.
*/
GTransf GMesh::GetTransf()
{
	return MC;
}

/*!
*	\brief 선택된 객체를 편집한다.
*
*	\param EC 편집 툴의 좌표계
*	\param t 편집될 객체의 이동 변위
*	\param q 편집될 객체의 회전 변위
*	\param s 편집될 객체의 스케일 변위
*	\param pParentMesh 편집 객체의 부모 메쉬
*/
void GMesh::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	// 부모 메쉬를 구하여, 
	if (pParentMesh == NULL)
		pParentMesh = FindMesh(pScene->MeshList, ParentMeshName);

	// 부모 메쉬가 없는 경우,
	if (pParentMesh == NULL)
	{
		// 메쉬의 모델링 좌표계를 편집한다.
		GTransf EC2MC = inv(EC) * MC;
		MC = EC * GTransf(t, q) * EC2MC;
	}
	else // 부모 메쉬가 있는 경우,
	{
		// 부모 메쉬에서의 상대적 변환을 갱신한다.
		DelX = DelX * GTransf(t, q);
		MC = pParentMesh->MC * DelX;
	}
	
	// 크기 변환을 수행한다면,
	if (s != GVector3(1.0, 1.0, 1.0))
	{
		// 메쉬의 각 정점에 대하여 크기 변환을 수행하고,
		int NumVert = GetNumVert();
		for (int i = 0; i < NumVert; ++i)
		{
			Verts[i].P[0] *= s[0];
			Verts[i].P[1] *= s[1];
			Verts[i].P[2] *= s[2];
		}

		// 경계 상자에 대해서도 크기 변환을 수행한다.
		BBox.MinPt[0] *= s[0];
		BBox.MinPt[1] *= s[1];
		BBox.MinPt[2] *= s[2];
		BBox.MaxPt[0] *= s[0];
		BBox.MaxPt[1] *= s[1];
		BBox.MaxPt[2] *= s[2];
		S[0] *= s[0];
		S[1] *= s[1];
		S[2] *= s[2];

		// 메쉬의 모든 정점의 위치가 변경되었음을 기록한다.
		ModifiedFlag = MODIFIED_VERT_POS_ALL;
	}	

	// 각각의 자식 메쉬의 모델링 좌표계를 편집한다.
	int NumSubMesh = (int)SubMeshList.size();
	for (int i = 0; i < NumSubMesh; ++i)
	{
		// 즉, 계층 구조를 가진 메쉬의 Forward Kinematic를 푸는 것과 동일
		SubMeshList[i]->Edit(GTransf(), GVector3(), GQuater(), GVector3(1.0, 1.0, 1.0), this);
	}

	// 편집된 메쉬의 Semantic 타입에 따라, 후처리 과정을 수행한다.
	switch (SemanticType)
	{
		// 편집된 메쉬가 본 메쉬라면,
	case MESH_BONE:
		{
			// 본 메쉬와 연결된 각각의 스킨 메쉬에 대하여,
			int NumSkinMesh = (int)LinkedMeshNameList.size();
			for (int i = 0; i < NumSkinMesh; ++i)
			{
				// 연결된 스킨 메쉬를 찾아,
				GMesh *pSkinMesh = FindMesh(pScene->MeshList, LinkedMeshNameList[i]);
				if (pSkinMesh != NULL)
				{
					// 스킨 메쉬가 갱신되어야 함을 알린다.
					pSkinMesh->ModifiedFlag = MODIFIED_REF_BONE;
				}
				else
				{
					// 존재하지 않는 스킨 메쉬라면 이름 리스트를 제거한다.
					LinkedMeshNameList.clear();
				}
			}
		}
		break;

		// 편집된 메쉬가(기준점, 키 단면, 키 프레임) 메쉬라면,
	case MESH_PIVOT:
	case MESH_KEY_CRV:
	case MESH_KEY_FRAME:
		if (!LinkedMeshNameList.empty())
		{
			// 연결된 메쉬를 찾아,
			GMesh *pMesh = FindMesh(pScene->MeshList, LinkedMeshNameList[0]);
			if (pMesh != NULL)
			{
				// 메쉬가 갱신되어야 함을 기록한다.
				pMesh->ModifiedFlag = MODIFIED_SAMPLE_NUM;
			}
			else
			{
				// 연결된 메쉬가 없다면, 연결정보를 제거한다.
				LinkedMeshNameList.clear();
			}
		}
		break;
	}	
}

/*!
*	\brief	메쉬의 모델링 좌표계를 변경한다.
*
*	\param RefType 변경될 모델링 좌표계의 타입(0: 월드 좌표계, 1: AABB, 2: OBB)
*/
void GMesh::UpdateMC(int RefType)
{
	if (RefType == 0)
	{
		// 월드 좌표계에서 표현된 정점의 좌표를 구하고,
		int NumVert = GetNumVert();
		for (int i = 0; i < NumVert; ++i)
			Verts[i].P = MC * Verts[i].P;
		
		// 모델링 좌표계를 월드 좌표계로 설정하고,
		MC = GTransf();

		// 스케일 정보를 (1.0, 1.0, 1.0)으로 초기화한다.
		S.Set(1.0, 1.0, 1.0);
	}
	else if (RefType == 1)
	{
		// 월드 좌표계를 모델링 좌표계로 설정하고,
		UpdateMC(0);

		// 경계 상자의 중심의 좌표를 구하고,
		GPoint3 p = BBox.MinPt + (BBox.MaxPt - BBox.MinPt) * 0.5;

		// 메쉬의 모델링 좌표계를 설정한다.
		MC = GTransf(cast_vec3(p), GQuater());

		// 새로운 모델링 좌표계에서 표현된 정점의 좌표를 계산한다.
		int NumVert = GetNumVert();
		GTransf invMC = inv(MC);
		for (int i = 0; i < NumVert; ++i)
			Verts[i].P = invMC * Verts[i].P;
	}
	else if (RefType == 2)
	{
		// 주성분 분석을 위한 변수를 선언하고,
		int NumVert = GetNumVert();
		GMatrix Data(NumVert, 3), PCs(3, 3);
		GVector Sigma(3), Mean(3);
		
		// 각각의 정점에 대하여,
		for (int i = 0; i < NumVert; ++i)
		{
			// 월드 좌표계에서 표현된 정점의 좌표를 구하고,
			Verts[i].P = MC * Verts[i].P;
			
			// 행 벡터 형태로 행렬에 추가한 후,
			GVector r = cast_vec(Verts[i].P);
			Data.SetRowVec(i, r);			
		}

		// 주성분 분석을 수행하여
		pca(Data, PCs, Sigma, Mean);

		// 모델링 좌표계를 설정한다.
		GVector3 origin = cast_vec3(Mean);
		GVector3 x_axis = cast_vec3(PCs.GetColVec(0)).Normalize();
		GVector3 y_axis = cast_vec3(PCs.GetColVec(1)).Normalize();
		GQuater q(x_axis, y_axis);
		MC = GTransf(origin, q);
		
		// 메쉬의 모델링 좌표계에서 표현된 정점의 좌표를 계산한다.
		GTransf invMC = inv(MC);
		for (int i = 0; i < NumVert; ++i)
			Verts[i].P = invMC * Verts[i].P;

		// 스케일 정보를 (1.0, 1.0, 1.0)으로 재 설정한다.
		S.Set(1.0, 1.0, 1.0);
	}

	// 부모 메쉬를 찾고,
	GMesh *pParentMesh = FindMesh(pScene->MeshList, ParentMeshName);

	// 부모 메쉬가 있는 경우,
	if (pParentMesh != NULL)
	{
		// 현재 메쉬의 부모 메쉬로부터의 상대 변환 정보를 편집하고,
		DelX = inv(pParentMesh->MC) * MC;
	}	
	
	// 메쉬를 업데이트 한다.
	ModifiedFlag = MODIFIED_VERT_POS_ALL;
	UpdateMesh();
}

/*!
*	\brief	메쉬가 수정된 내용에 따라 메쉬를 갱신한다.
*/
void GMesh::UpdateMesh() 
{
	//  메쉬의 수정된 정보에 따라서,
	switch (ModifiedFlag)
	{
		// 일부 정점의 위치가 변경되었다면,
	case MODIFIED_VERT_POS:
		{
			UpdateBndBox(false);
			int NumSubMesh = (int)SubMeshList.size();
			for (int i = 0; i < NumSubMesh; ++i)
				SubMeshList[i]->UpdateMesh();
		}		
		break;

		// 모든 정점의 위치가 변경되었다면,
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

		// 정점의 개수가 변경되었다면,
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

		// 법선이 변경되었다면,
	case MODIFIED_NORMAL:
		{
			UpdateNormal(false);
			UpdateBufNormal();
			int NumSubMesh = (int)SubMeshList.size();
			for (int i = 0; i < NumSubMesh; ++i)
				SubMeshList[i]->UpdateMesh();
		}
		break;

		// 재질이 변경되었다면,
	case MODIFIED_MTL:
		UpdateBufFace();
		break;

		// 본 메쉬와 연결된 메쉬라면, 스키닝을 수행한다.
	case MODIFIED_REF_BONE:
		{
			ApplySSD();
			UpdateBndBox(false);
			UpdateBufVertex();
		}		
		break;
	}

	// 메쉬의 변경 내용이 처리 되었음을 기록한다.
	ModifiedFlag = MODIFIED_NONE;
}

/*!
*	\brief 선택된 객체를 렌더링 한다.
*/
void GMesh::RenderForEdit(float r, float g, float b)
{
	// 본 메쉬라면,
	if (SemanticType == MESH_BONE)
	{
		glDisable(GL_DEPTH_TEST);
		// 현재 좌표계: 월드(장면)
		glPushMatrix();
		{
			// 메쉬의 모델링 좌표계로 변환
			glMultMatrixd(cast_mat(MC).M);
			RenderMeshWire(r, g, b, 1.0);
		}
		glPopMatrix();
		glEnable(GL_DEPTH_TEST);

		int NumSubMesh = (int)SubMeshList.size();
		for (int i = 0; i < NumSubMesh; ++i)
			SubMeshList[i]->RenderForEdit(0.0, 0.0, 1.0);
	}
	else	// 일반 메쉬라면,
	{
		BBox.Render(r, g, b, 1.0);

		int NumSubMesh = (int)SubMeshList.size();
		for (int i = 0; i < NumSubMesh; ++i)
			SubMeshList[i]->RenderForEdit(1.0, 1.0, 1.0);
	}
}

/*!
*	\brief 메쉬의 생성자
*
*	\param _Name 생성할 메쉬의 이름
*	\param _MC 객체가 정의되는 모델링 좌표계
*	\param _FaceType 메쉬의 폴리곤 타입(FACE_TRI, FACE_QUAD, FACE_TRI_FAN)
*	\param _ParentName 생성할 메쉬와 연결된 부모 메쉬의 이름
*/
GMesh::GMesh(const std::string &_Name, GTransf _MC, TypeMeshFace _FaceType, const std::string &_ParentName)
{
	// 기본 데이터 멤버(19)
	Name = _Name;						// 메쉬의 이름
	MC = _MC;							// 메쉬가 정의된 모델링 좌표계
	S = GVector3(1.0, 1.0, 1.0);		// 메쉬의 크기 변환 정보
	FaceType = _FaceType;				// 메쉬의 폴리곤 타입(삼각형, 사각형) 정보
	Verts.clear();						// 메쉬를 구성하는 정점의 배열
	Texs.clear();						// 메쉬를 구성하는 텍스처 정점의 배열
	Faces.clear();						// 메쉬를 구성하는 삼각형의 배열
	GrpFaces.clear();					// 그룹핑된 메쉬의 삼각형 인덱스 배열
	BBox.pMesh = this;					// 메쉬를 둘러싸는 경계 상자의 양 끝점
	pBVH = NULL;						// 메쉬의 경계 볼률 계층 구조
	CollisionFaces.clear();				// 충돌이 생긴 삼각형의 인덱스 리스트
	Renderer = GRenderer();				// 메쉬의 렌더러
	MtlList.clear();					// 메쉬가 사용하는 재질 리스트
	DispTexMesh = 0;					// 텍스처 매핑된 메쉬의 디스플레이 리스트
	DispVertColMesh = 0;				// 정점 칼라 렌더링을 위한 디스플레이 리스트
	DispFaceColMesh = 0;				// 삼각형 컬러 렌더링을 위한 디스플레이 리스트 */
	VtxBuf.clear();						// 정점 버퍼를 초기화
	NorBuf.clear();						// 법선 버퍼를 초기화
	FaceIdxBuf.clear();					// 폴리곤 인덱스 버퍼를 초기화
	
	// 추가 데이터 멤버(11)
	GScene *pScene = NULL;				// 메쉬가 포함된 장면에 대한 포인터
	ParentMeshName = _ParentName;		// 메쉬와 연결된 부모 메쉬의 이름
	DelX = GTransf();					// 부모 메쉬에로부터의 변환 (MC = 부모 MC * DelX)
	SubMeshList.clear();				// 메쉬와 연결된 자식 메쉬 리스트
	SemanticType = MESH_STD;			// 메쉬의 시멘틱 타입 정보
	LinkedMeshNameList.clear();			// 메쉬의 시멘틱 타입에 따라 연결된 메쉬의 이름 리스트
	ClipPlaneNameList.clear();			// 절단 평면의 이름 리스트
	ModifiedFlag = MODIFIED_NONE;		// 메쉬의 업데이트 성분
	BndryEdgeList.clear();				// 메쉬의 경계 에지의 리스트
	Animation.clear();					// 애니메이션을 위한 변환 정보
	FrameIdx = 0;						// 애니메이션을 위한 프레임 인덱스
}

/*!
*	\brief 메쉬의 복사 생성자
*
*	\param cpy 복사될 객체
*/
GMesh::GMesh(const GMesh &cpy)
{
	// 기본 데이터 멤버(19)
	Name = cpy.Name;								// 메쉬의 이름
	MC = cpy.MC;									// 메쉬가 정의된 모델링 좌표계
	S = cpy.S;										// 메쉬의 크기 변환 정보
	FaceType = cpy.FaceType;						// 메쉬의 폴리곤 타입(삼각형, 사각형) 정보
	Verts = cpy.Verts;								// 메쉬를 구성하는 정점의 배열
	Texs = cpy.Texs;								// 메쉬를 구성하는 텍스처 정점의 배열
	Faces = cpy.Faces;								// 메쉬를 구성하는 삼각형의 배열
	GrpFaces = cpy.GrpFaces;						// 그룹핑된 메쉬의 삼각형 인덱스 배열
	BBox = cpy.BBox; BBox.pMesh = this;				// 메쉬를 둘러싸는 경계 상자의 양 끝점
	pBVH = NULL;
	CollisionFaces.clear();							// 충돌이 생긴 삼각형의 인덱스 리스트
	Renderer = cpy.Renderer;						// 메쉬의 렌더링 속성
	MtlList = cpy.MtlList;							// 메쉬가 사용하는 재질 리스트
	DispTexMesh = cpy.DispTexMesh;					// 텍스처 매핑된 메쉬의 디스플레이 리스트
	DispVertColMesh = cpy.DispVertColMesh;			// 컬러 정점 렌더링을 위한 디스플레이 리스트
	DispFaceColMesh = cpy.DispFaceColMesh;			// 삼각형 컬러 렌더링을 위한 디스플레이 리스트
	VtxBuf = cpy.VtxBuf;							// 정점 버퍼를 초기화
	NorBuf = cpy.NorBuf;							// 법선 버퍼를 초기화
	FaceIdxBuf = cpy.FaceIdxBuf;					// 폴리곤 인덱스 버퍼를 초기화

	
	// 추가 데이터 멤버(11)
	pScene = cpy.pScene;							// 메쉬가 포함된 장면에 대한 포인터
	ParentMeshName = cpy.ParentMeshName;			// 메쉬와 연결된 부모 메쉬의 이름
	DelX = cpy.DelX;								// 부모 메쉬에로부터의 변환 (MC = 부모 MC * DelX)
	SemanticType = cpy.SemanticType;				// 메쉬의 시멘틱 타입 정보
	LinkedMeshNameList = cpy.LinkedMeshNameList;	// 메쉬의 시멘틱 타입에 따라 연결된 메쉬의 이름 리스트
	ClipPlaneNameList = cpy.ClipPlaneNameList;		// 절단 평면의 이름 리스트
	ModifiedFlag = cpy.ModifiedFlag;				// 메쉬의 업데이트 성분
	BndryEdgeList = cpy.BndryEdgeList;				// 메쉬의 경계 에지의 리스트
	Animation = cpy.Animation;						// 애니메이션을 위한 변환 정보
	FrameIdx = cpy.FrameIdx;						// 애니메이션을 위한 프레임 인덱스
	if (!cpy.SubMeshList.empty())					// 서브 메쉬의 계층적 복사
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
*	\brief 메쉬의 소멸자
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
*	\brief 메쉬의 대입연산자
*
*	\param rhs 대입될 메쉬 객체
*
*	\return 대입된 자신을 반환한다.
*/
GMesh &GMesh::operator =(const GMesh &rhs)
{
	// 기본 데이터 멤버(19)
	Name = rhs.Name;								// 메쉬의 이름
	MC = rhs.MC;									// 메쉬가 정의된 모델링 좌표계
	S = rhs.S;										// 메쉬의 크기 변환 정보
	FaceType = rhs.FaceType;						// 메쉬의 폴리곤 타입(삼각형, 사각형) 정보
	Verts = rhs.Verts;								// 메쉬를 구성하는 정점의 배열
	Texs = rhs.Texs;								// 메쉬를 구성하는 텍스처 정점의 배열
	Faces = rhs.Faces;								// 메쉬를 구성하는 삼각형의 배열
	GrpFaces = rhs.GrpFaces;						// 그룹핑된 메쉬의 삼각형 인덱스 배열
	BBox = rhs.BBox; BBox.pMesh = this;				// 메쉬를 둘러싸는 경계 상자의 양 끝점
	pBVH = NULL;
	CollisionFaces.clear();							// 충돌이 생긴 삼각형의 인덱스 리스트
	Renderer = rhs.Renderer;						// 메쉬의 렌더링 속성
	MtlList = rhs.MtlList;							// 메쉬가 사용하는 재질 리스트
	DispTexMesh = rhs.DispTexMesh;					// 텍스처 매핑된 메쉬의 디스플레이 리스트
	DispVertColMesh = rhs.DispVertColMesh;			// 컬러 정점 렌더링을 위한 디스플레이 리스트
	DispFaceColMesh = rhs.DispFaceColMesh;			// 삼각형 컬러 렌더링을 위한 디스플레이 리스트
	VtxBuf = rhs.VtxBuf;							// 정점 버퍼를 초기화
	NorBuf = rhs.NorBuf;							// 법선 버퍼를 초기화
	FaceIdxBuf = rhs.FaceIdxBuf;					// 폴리곤 인덱스 버퍼를 초기화

	// 추가 데이터 멤버(11)
	pScene = rhs.pScene;							// 메쉬가 포함된 장면에 대한 포인터
	ParentMeshName = rhs.ParentMeshName;			// 메쉬와 연결된 부모 메쉬의 이름
	DelX = rhs.DelX;								// 부모 메쉬에로부터의 변환 (MC = 부모 MC * DelX)
	SemanticType = rhs.SemanticType;				// 메쉬의 시멘틱 타입 정보
	LinkedMeshNameList = rhs.LinkedMeshNameList;	// 메쉬의 시멘틱 타입에 따라 연결된 메쉬의 이름 리스트
	ClipPlaneNameList = rhs.ClipPlaneNameList;		// 절단 평면의 이름 리스트
	ModifiedFlag = rhs.ModifiedFlag;				// 메쉬의 업데이트 성분
	BndryEdgeList = rhs.BndryEdgeList;				// 메쉬의 경계 에지의 리스트
	Animation = rhs.Animation;						// 애니메이션을 위한 변환 정보
	FrameIdx = rhs.FrameIdx;						// 애니메이션을 위한 프레임 인덱스
	if (!rhs.SubMeshList.empty())					// 서브 메쉬의 계층적 복사
	{
		// 서브 메쉬 리스트 지우기
		int NumSubMesh = (int)SubMeshList.size();
		for (int i = 0; i < NumSubMesh; ++i)
			delete SubMeshList[i];
		SubMeshList.clear();

		// 서브 메쉬 리스트 복사하기
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
*	\brief 메쉬에 정점을 추가한다.
*
*	\param v 추가될 정점
*/
void GMesh::AddVertex(GVertex v)
{
	v.pMesh = this;
	v.Idx = GetNumVert();
	Verts.push_back(v);
}

/*!
*	\brief 메쉬에 삼각형을 추가한다.
*
*	\param f 추가될 삼각형
*	\param GroupName 삼각형의 그룹 이름
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
*	\brief 메쉬에 텍스처 정점을 추가한다.
*
*	\param t 추가될 텍스처 정점
*/
void GMesh::AddTexel(GVector2 t)
{
	Texs.push_back(t);
}

/*!
*	\brief 메쉬에 재질을 추가한다.
*
*	\param mtl 추가될 재질
*/
void GMesh::AddMtl(GMaterial mtl)
{
	MtlList.push_back(mtl);
}

/*!
*	\brief 메쉬에 6개의 절단면을 추가한다.
*/
void GMesh::AddClipPlanes()
{
	// 월드 좌표계에서 표현된 경계 상자의 코너 정점의 위치를 구하고,
	GPoint3 P[8];
	BBox.GetCornerPts(P, false);

	// 6개의 절단 평면의 변환을 정의한다.
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

	// 6개의 절단 평면을 생성한다.
	int vidx[] = {0, 2, 6, 4, 1, 5, 7, 3, 0, 4, 5, 1, 2, 3, 7, 6, 4, 6, 7, 5, 0, 1, 3, 2};
	for (int i = 0; i < 6; ++i)
	{
		GMesh *pPlane = new GMesh("", GTransf(), FACE_QUAD, Name);
		pPlane->Name = ::create_mesh_name("Clip Plane");
		pPlane->Renderer.RenderType = RENDER_WIRE;
		pPlane->MC = BBox.GetMC() * dX[i];

		// 정점을 추가한다.
		for (int j = 0; j < 4; ++j)
		{
			GPoint3 p = inv(pPlane->MC) * P[vidx[4 * i + j]];
			pPlane->AddVertex(GVertex(p, GVector3()));
		}

		// 삼각형을 추가한다.
		pPlane->AddFace(GFace(0, 1, 2, 0, 1, 2, -1));
		pPlane->AddFace(GFace(0, 2, 3, 0, 2, 3, -1));

		// 절단면을 장면에 추가한다.
		pScene->AddMesh(pPlane);
		ClipPlaneNameList.push_back(pPlane->Name);
	}
}

/*!
*	\brief 메쉬의 삼각형을 제거한다.
*
*	\param 제거할 삼각형의 인덱스
*/
void GMesh::DelFaces(std::vector<int> &FaceList)
{
	//////////////////////////////////////
	// 1단계: 선택된 삼각형을 삭제한다. //
	//////////////////////////////////////	

	// 삭제할 삼각형을 DelFaceFlag 배열에 true로 기록한다.
	std::vector<bool> DelFaceFlag(GetNumFace(), false);
	for (unsigned i = 0; i < FaceList.size(); ++i)
		DelFaceFlag[FaceList[i]] = true;

	// 남길 삼각형의 리스트와 삼각형 인덱스 맵을 생성한다.
	std::map<int, int> FaceIdxMap;
	std::vector<GFace> SavedFaces = Faces;
	Faces.clear();
	for (unsigned i = 0; i < SavedFaces.size(); ++i)
	{
		// 남길 삼각형이라면, 
		if (!DelFaceFlag[i])
		{
			// 추가하고 (이전 인덱스, 새로운 인덱스 + 1)를 기록한다.
			AddFace(SavedFaces[i]);
			FaceIdxMap[i] = GetNumFace();
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// 2단계: 사용되지 않는 정점들을 제거하고, 삼각형의 인덱스를 갱신한다. //
	/////////////////////////////////////////////////////////////////////////	

	// 삭제할 정점을 DelVertFlag 배열에 기록한다.
	std::vector<bool> DelVertFlag(GetNumVert(), true);
	for (int i = 0; i < GetNumFace(); ++i)
	{
		int *vidx = Faces[i].vIdx;
		DelVertFlag[vidx[0]] = false;
		DelVertFlag[vidx[1]] = false;
		DelVertFlag[vidx[2]] = false;
	}

	// 사용되는 정점 리스트와 정점 인덱스 (이전 인덱스, 새로운 인덱스 + 1) 맵을 생성한다.
	std::map<int, int> VertIdxMap;
	std::vector<GVertex> SavedVerts = Verts;
	Verts.clear();
	for (unsigned i = 0; i < SavedVerts.size(); ++i)
	{
		// 남길 정점이라면, 
		if (!DelVertFlag[i])
		{
			// 정점 리스트에 추가하고 (이전 인덱스, 새로운 인덱스 + 1)를 기록한다.
			AddVertex(SavedVerts[i]);
			VertIdxMap[i] = GetNumVert();
		}
	}

	// 남은 삼각형의 정점의 인덱스를 갱신한다.
	for (int i = 0; i < GetNumFace(); ++i)
	{
		Faces[i].vIdx[0] = VertIdxMap[Faces[i].vIdx[0]] - 1;
		Faces[i].vIdx[1] = VertIdxMap[Faces[i].vIdx[1]] - 1;
		Faces[i].vIdx[2] = VertIdxMap[Faces[i].vIdx[2]] - 1;
	}

	/////////////////////////////////////////////
	// 3단계: 삼각형 그룹의 인덱스를 갱신한다. //
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

	// 기하정보가 변경됨을 기록하고, 경계상자를 재계산한다.
	ModifiedFlag = MODIFIED_VERT_NUM;
	UpdateMesh();
}

/*!
*	\brief 메쉬의 정점을 제거한다.
*
*	\param 제거할 정점의 인덱스
*/
void GMesh::DelVerts(std::vector<int> &VertList)
{
	/////////////////////////////////////
	// 1 단계: 선택된 정점을 삭제한다. //
	/////////////////////////////////////	

	// 삭제할 정점을 SltVertFlag 배열에 기록한다.
	std::vector<bool> SltVertFlag(GetNumVert(), false);
	for (unsigned i = 0; i < VertList.size(); ++i)
		SltVertFlag[VertList[i]] = true;

	std::vector<GVertex> SavedVerts = Verts;
	Verts.clear();

	// 남길 정점 리스트와 정점 인덱스 맵(이전 인덱스, 새로운 인덱스 + 1)을 생성한다.
	std::map<int, int> VertIdxMap;
	for (unsigned i = 0; i < SavedVerts.size(); ++i)
	{
		// 남길 정점이라면,
		if (!SltVertFlag[i])
		{
			// 메쉬에 추가하고, (이전 인덱스, 새로운 인덱스 + 1)를 기록한다.
			AddVertex(SavedVerts[i]);
			VertIdxMap[i] = GetNumVert();
		}
	}

	///////////////////////////////////////////////////////////////////////
	// 2단계: 남은 정점으로 새로운 삼각형 리스트와 인덱스 맵을 생성한다. //
	///////////////////////////////////////////////////////////////////////
	std::vector<GFace> SavedFaces = Faces;
	Faces.clear();	

	std::map<int, int> FaceIdxMap;
	for (unsigned i = 0; i < SavedFaces.size(); ++i)
	{
		// 삼각형의 정점 인덱스를 구하여, 
		int vidx0 = VertIdxMap[SavedFaces[i].vIdx[0]] - 1;
		int vidx1 = VertIdxMap[SavedFaces[i].vIdx[1]] - 1;
		int vidx2 = VertIdxMap[SavedFaces[i].vIdx[2]] - 1;

		// 남길 삼각형이라면,
		if (vidx0 != -1 && vidx1 != -1 && vidx2 != -1)
		{
			// 삼각형 리스트에 추가하고,
			int *tidx = SavedFaces[i].tIdx;
			GFace f(vidx0, vidx1, vidx2, tidx[0], tidx[1], tidx[2], SavedFaces[i].MtlIdx);
			AddFace(f);

			// (이전 인덱스, 새로운 인덱스 + 1)를 기록한다.
			FaceIdxMap[i] = GetNumFace();
		}
	}

	/////////////////////////////////////////////
	// 3단계: 삼각형 그룹의 인덱스를 갱신한다. //
	/////////////////////////////////////////////
	std::map<std::string, std::vector<int>>::iterator it_group_name;
	for (it_group_name = GrpFaces.begin(); it_group_name != GrpFaces.end(); ++it_group_name)
	{
		// 그룹의 이전 삼각형 인덱스를 저장하고,
		std::vector<int> SavedFaceIdx = it_group_name->second;
		it_group_name->second.clear();

		// 그룹의 새로운 삼각형 인덱스를 생성한다.
		std::vector<int>::iterator it_face_idx;
		for (it_face_idx = SavedFaceIdx.begin(); it_face_idx != SavedFaceIdx.end(); ++it_face_idx)
		{
			// 남길 삼각형의 인덱스를 추가한다.
			if (FaceIdxMap[*it_face_idx] != 0)
				it_group_name->second.push_back(FaceIdxMap[*it_face_idx] - 1);
		}
	}

	// 기하정보가 변경됨을 기록하고 메쉬를 갱신한다.
	ModifiedFlag = MODIFIED_VERT_NUM;
	UpdateMesh();
}

/*!
*	\brief 메쉬의 정점의 개수를 반환한다.
*
*	\return 메쉬를 구성하는 정점의 개수
*/
int GMesh::GetNumVert() 
{ 
	return (int)Verts.size(); 
}

/*!
*	\brief 메쉬의 삼각형의 개수를 반환한다.
*
*	\return 메쉬를 구성하는 삼각형의 개수
*/
int GMesh::GetNumFace() 
{ 
	return (int)Faces.size(); 
}

/*!
*	\brief 메쉬의 텍스처 정점의 개수를 반환한다.
*
*	\return 메쉬를 텍스처 정점의 개수
*/
int GMesh::GetNumTex()	
{ 
	return (int)Texs.size(); 
}

/*!
*	\brief 메쉬의 재질의 개수를 반환한다.
*
*	\return 메쉬에 포함된 재질의 개수
*/
int GMesh::GetNumMtl() 
{ 
	return (int)MtlList.size(); 
}

/*!
*	\brief 주어진 정점과 동일한 위치를 갖는 정점의 인덱스를 구한다.
*
*	\param pVert 주어진 정점
*
*	\return pVert와 동일한 위치를 갖는 정점의 인덱스 (없으면 -1)를 반환한다.
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
*	\brief 삼각형 리스트에서 주어진 삼각형의 인덱스를 구한다.
*
*	\param pFace 주어진 삼각형
*
*	\return 삼각형 리스트에서 pFace의 인덱스 (없으면 -1)를 반환한다.
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
*	\brief 주어진 정점에 대응되는 텍스처의 좌표의 인덱스를 반환한다.
*
*	\param pVert 주어진 정점
*
*	\return 주어진 정점에 대응되는 텍스처의 좌표의 인덱스를 반환한다.
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
*	\brief 재질 리스트에서 이름이 \a Name인 재질을 찾아 인덱스를 반환한다.
*
*	\param Name 찾을 재질의 이름
*
*	\return 재질의 인덱스를 반환한다 (해당 이름의 재질이 없는 경우, -1을 반환)
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
*	\brief 객체의 재질을 설정한다.
*	\note 객체가 장면에 추가된 후에 호출해야 한다.
*
*	\param MtlName 객체의 재질 리스트에 존재하는 재질의 이름
*/
void GMesh::SetMtl(std::string MtlName)
{
	int MtlIdx = GetMtlIdx(MtlName);
	if (MtlIdx != -1)
	{
		// 전체 삼각형에 대하여 재질을 적용한다.
		int NumFace = GetNumFace();
		for (int i = 0; i < NumFace; ++i)
			Faces[i].MtlIdx = MtlIdx;
	}

	// 삼각형이 없는 곡선 객체에 대하여 재질을 적용한다.
	Renderer.CrvMtlIdx = MtlIdx;
	ModifiedFlag = MODIFIED_MTL;
	UpdateMesh();
}

/*!
*	\brief 메쉬에 새로운 삼각형 리스트를 설정한다.
*
*	\param NewFaces 새로운 삼각형 리스트
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
*	\brief (sidx, edix)를 정점의 인덱스로 갖는 삼각형을 찾는다.
*
*	\param sidx 시작 정점의 인덱스
*	\param eidx 끝 정점의 인덱스
*
*	\param (sidx, edix)를 정점의 인덱스로 갖는 삼각형의 포인터를 반환한다.
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
*	\brief 재질 리스트에서 이름이 \a Name인 재질을 찾아 포인터를 반환한다.
*
*	\param Name 찾을 재질의 이름
*
*	\return 재질의 포인터 (해당 이름의 재질이 없는 경우, 디폴트 재질의 포인터를 반환)
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
*	\brief 메쉬 삼각형의 앞/뒤 면을 뒤집는다.
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
*	\brief 메쉬 정점의 법선벡터를 갱신한다.
*
*	\param all 자식 메쉬의 갱신 여부
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
*	\brief 메쉬를 둘러싸는 경계상자를 갱신한다.
*
*	\param all 자식 메쉬의 갱신 여부
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
*	\brief 정점의 바인딩 정보를 업데이트 한다.
*/
void GMesh::InitRefBoneInfo()
{
	// 스킨 메쉬가 아니라면 리턴한다.
	if (SemanticType != MESH_SKIN)
		return;

	// 스킨 메쉬라면 정점의 개수를 구하고,
	int NumVert = GetNumVert();

	// 각각의 정점에 대하여	
	for (int i = 0; i < NumVert; ++i)
	{
		// 바인딩 정보를 얻어와서,
		std::vector<GRefBoneInfo> &InfoList = Verts[i].BoneInfoList;

		// 바인딩 정보가 없다면 다음 정점을 처리하고,
		if (InfoList.empty())
			continue;
		
		// 바인딩 정보가 있다면, 
		std::vector<GRefBoneInfo>::iterator it = InfoList.begin();

		// 각각의 바인딩 정보에 대하여,
		while (it != InfoList.end())
		{
			// 연결된 본 메쉬를 찾아서,
			GMesh *pBoneMesh = FindMesh(pScene->MeshList, it->BoneMeshName);

			// 연결된 본 메쉬가 있다면,
			if (pBoneMesh != NULL)
			{
				// 바인딩 정보를 초기화 하고,
				it->pRefMC = &(pBoneMesh->MC);
				it->P = inv(pBoneMesh->MC) * MC * Verts[i].P;
				it->N = inv(pBoneMesh->MC) * MC * Verts[i].N;

				// 본 메쉬에 스킨 메쉬 이름을 추가한다.
				if (pBoneMesh->LinkedMeshNameList.end() == find(pBoneMesh->LinkedMeshNameList.begin(), pBoneMesh->LinkedMeshNameList.end(), Name))
					pBoneMesh->LinkedMeshNameList.push_back(Name);
				
			}
			// 연결된 본 메쉬가 없다면,
			else
			{
				// 해당 바인딩 정보를 삭제한다.
				it = InfoList.erase(it);
				continue;
			}
			it++;
		}
	}

	// 모든 서브 메쉬에 대하여 작업을 반복한다.
	int NumSubMesh = (int)SubMeshList.size();
	for (int i = 0; i < NumSubMesh; ++i)
		SubMeshList[i]->InitRefBoneInfo();
}

/*!
*	\brief 메쉬의 애니메이션 정보를 이용하여 변환 정보를 갱신한다.
*
*	\param pParent 메쉬가 참조하는 부모 메쉬
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
*	\brief 본의 바인딩 정보를 이용하여 스키닝을 수행한다.
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
*	\brief 객체(메쉬/곡선/곡면/볼륨 등)를 렌더링 한다.
*
*	\param bSubMesh 서브 메쉬의 렌더링 여부(선택 모드에서는 false로 설정)
*/
void GMesh::Render(bool bSubMesh)
{
	// 보이는 메쉬라면,
	if (Renderer.IsVisibleMesh())
	{
		// 월드 좌표계에서
		glPushMatrix();
		{
			// 객체의 모델링 좌표계로의 변환하고,
			glMultMatrixd(cast_mat(MC).M);

			// 사용자 절단 평면을 설정하고,
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

			// 뒷면 제거 옵션을 설정하고,
			if (Renderer.bBackFaceCull)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
			}			

			// 쉐이딩 타입을 설정하고, 
			switch (Renderer.ShadeType)
			{
			case SHADE_FLAT:
				glShadeModel(GL_FLAT);
				break;

			case SHADE_SMOOTH:
				glShadeModel(GL_SMOOTH);
				break;
			}

			// 선으로 렌더링하는 곡선 객체라면,
			if (GetClassName() == "GNurbsCrv" && Renderer.bRenderCrvAsLine)
			{
				// 곡선의 두께와 색상을 설정한 후,
				glDisable(GL_LIGHTING);
				glLineWidth(2.0);
				int NumMtl = GetNumMtl();
				int MtlIdx = Renderer.CrvMtlIdx;
				if (MtlIdx >= 0 && MtlIdx < NumMtl)
					glColor3fv(MtlList[MtlIdx].Diffuse);
				else
					glColor3f(0.5f, 0.5f, 0.5f);

				// 곡선을 그려주고,
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
			else // 폴리곤으로 구성된 일반 객체라면,
			{
				// 폴리곤 렌더링이 적용된 경우,
				if (Renderer.RenderType & RENDER_POLYGON)
				{
					// 텍스처 렌더링 모드라면,
					if (Renderer.bTexture && !Texs.empty())
						RenderMeshTexture();
					else	// 일반 폴리곤 렌더링 모드라면,
						RenderMeshPolygon();
				}

				// 와이어 렌더링이 적용된 경우,
				if (Renderer.RenderType & RENDER_WIRE)
					RenderMeshWire(0.0, 0.0, 0.0, 1.0);

				// 포인트 렌더링이 적용된 경우,
				if (Renderer.RenderType & RENDER_POINT)
					RenderMeshPoint(1.0, 0.0, 0.0, 3.0);

				// 경계 렌더링이 적용된 경우,
				if (Renderer.RenderType & RENDER_BOUNDARY)
					RenderMeshBoundary(1.0, 0.0, 0.0, 2.0);

				// 쉐이딩 모드를 복원하고,
				glShadeModel(GL_SMOOTH);

				// 정점의 법선 벡터를 렌더링 옵션에 따라,
				if (Renderer.RenderType & RENDER_NORMAL)
				{
					double scale = dist(BBox.MaxPt, BBox.MinPt) * 0.03;
					// 조명을 끄고, 법선의 색과 두께를 지정한 후,
					glDisable(GL_LIGHTING);
					glColor3f(1.0, 0.0, 0.0);
					glLineWidth(1.5);

					// 법선 벡터를 렌더링한다.
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

					// 조명을 켜고, 직선의 두께를 초기화 한다.
					glLineWidth(1.0);
					glEnable(GL_LIGHTING);
				}

				// 충돌된 삼각형이 있다면,
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
				// 조명을 비활성화 하고,
				glDisable(GL_LIGHTING);

				// 곡선의 두께와 색상을 설정한 후,
				glLineWidth(2.0);
				int NumMtl = GetNumMtl();
				int MtlIdx = Renderer.CrvMtlIdx;
				if (MtlIdx >= 0 && MtlIdx < NumMtl)
					glColor3fv(MtlList[MtlIdx].Diffuse);
				else
					glColor3f(0.5f, 0.5f, 0.5f);

				// 곡선을 그려주고,
				glBegin(GL_LINE_STRIP);
				{
					int NumVert = (int)Verts.size();
					for (int i = 0; i < NumVert; ++i)
						glVertex3dv(Verts[i].P.V);
				}
				glEnd();

				// 선 두께를 기본 값으로 재 설정한 후,
				glLineWidth(1.0);

				// 조명을 활성화 한다.
				glEnable(GL_LIGHTING);
			}

			// 컬링이 활성화 되었다면 비활성화 하고,
			if (Renderer.bBackFaceCull)
				glDisable(GL_CULL_FACE);

			// 절단 평면을 가지고 있다면, 비활성화 한다.
			if (!ClipPlaneNameList.empty())
			{
				for (int i = 0; i < 6; ++i)
					glDisable(GL_CLIP_PLANE0 + i);
			}
		}
		glPopMatrix();	// 월드 좌표계로 복원한다.
	}

	// 조건에 따라 서브 메쉬를 렌더링 한다.
	if (bSubMesh)
	{
		int NumSubMesh = (int)SubMeshList.size();
		for (int i = 0; i < NumSubMesh; ++i)
			SubMeshList[i]->Render();
	}
}

/*!
*	\brief 메쉬를 생성한다.
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
*	\brief GMesh의 파생 클래스를 위한 가상함수
*/
GVector4 GMesh::Eval(double u, ...) 
{ 
	return GVector4(); 
}

/*!
*	\brief GMesh의 파생 클래스를 위한 가상함수
*/
void GMesh::GetDomain(double *umin, ...) 
{ 
	*umin = 0.0; 
}

/*!
*	\brief GMesh의 파생 클래스를 위한 가상함수
*/
void GMesh::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList) 
{
}

/*!
*	\brief 텍스처 메쉬를 렌더링 한다.
*/
void GMesh::RenderMeshTexture()
{
	// 텍스처 매핑 옵션을 설정하고,
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_TEXTURE_2D);
	
	// 텍스처링된 메쉬의 디스플레이 리스트가 있다면,
	if (DispTexMesh != 0)
		glCallList(DispTexMesh);
	else	// 텍스처링된 메쉬의 디스플레이 리스트가 없다면,
	{
		// 텍스처 폴리곤 형태로 렌더링된 디스플레이 리스트를 생성한다.
		DispTexMesh = glGenLists(1);
		glNewList(DispTexMesh, GL_COMPILE_AND_EXECUTE);
		{
			// 폴리곤 오프셋 활성화 하고,
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);

			// 재질의 인덱스를 초기화 하고,
			int PrevMtlIdx = -1;
			int NumFace = GetNumFace();
			int NumMtl = GetNumMtl();

			// 모든 삼각형을 렌더링 한다.
			glBegin(GL_TRIANGLES);
			for (int i = 0; i < NumFace; ++i)
			{
				int CurrMtlIdx = Faces[i].MtlIdx;
				if (CurrMtlIdx != PrevMtlIdx)
				{
					// 현재 재질을 종료하고,
					glEnd();

					// 새로운 재질을 설정한 후,
					if (CurrMtlIdx >= 0 && CurrMtlIdx < NumMtl)
						MtlList[CurrMtlIdx].Enable(true);
					PrevMtlIdx = CurrMtlIdx;

					// 삼각형 렌더링을 시작한다.
					glBegin(GL_TRIANGLES);				
				}

				// 텍스처와 정점의 인덱스를 구하여,
				int *tidx = Faces[i].tIdx;
				int *vidx = Faces[i].vIdx;

				// 삼각형을 렌더링한다.
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

			// 폴리곤 오프셋을 비활성화 한다.
			glDisable(GL_POLYGON_OFFSET_FILL);		
		}	
		glEndList();
	}

	// 텍스처 기능을 비활성화 한다.
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, NULL);
}

/*!
*	\brief 메쉬를 폴리곤 형태로 렌더링 한다.
*/
void GMesh::RenderMeshPolygon()
{
	// 조명 모델을 적용한다면,
	if (Renderer.VertColorType == VERTEX_COLOR_ILLUMINATION && !VtxBuf.empty() && !NorBuf.empty())
	{
		// 폴리곤 오프셋 활성화 하고,
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);

		// 정점과 노말 버퍼를 지정하고,
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, &(VtxBuf[0]));
		glNormalPointer(GL_FLOAT, 0, &(NorBuf[0]));

		// 재질에 따른 폴리곤 그룹을 렌더링 한다.
		std::map<int, std::vector<int>>::iterator it;
		for (it = FaceIdxBuf.begin(); it != FaceIdxBuf.end(); ++it)
		{
			// 재질을 적용하여
			int MtlIdx = it->first;
			if (MtlIdx != -1 && !MtlList.empty())
				MtlList[MtlIdx].Enable(false);

			// 폴리곤을 렌더링하고,
			if (FaceType == FACE_QUAD)
				glDrawElements(GL_QUADS, it->second.size(), GL_UNSIGNED_INT, &(it->second[0]));
			else
				glDrawElements(GL_TRIANGLES, it->second.size(), GL_UNSIGNED_INT, &(it->second[0]));

			// 재질을 비활성화 한다.
			if (MtlIdx != -1 && !MtlList.empty())
				MtlList[MtlIdx].Disable();
		}

		// 정점과 노말 버퍼를 해제하고 폴리곤 오프셋을 비활성화 한다.
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	// 정점의 색상이 할당 되었다면,
	else
	{
		// 컬러링된 메쉬의 디스플레이 리스트가 있다면,
		if (DispVertColMesh != 0)
			glCallList(DispVertColMesh);
		else	// 컬러링된 메쉬의 디스플레이 리스트가 없다면,,
		{
			// 디스플레이 리스트를 생성한다.
			DispVertColMesh = glGenLists(1);
			glNewList(DispVertColMesh, GL_COMPILE_AND_EXECUTE);
			{
				// 조명을 끄고 폴리곤 오프셋 활성화 하고,
				glDisable(GL_LIGHTING);
				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(1.0, 1.0);

				// 모든 삼각형을 렌더링 한다.
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

				// 폴리곤 오프셋을 비활성화, 조명을 활성화 한다.
				glDisable(GL_POLYGON_OFFSET_FILL);
				glEnable(GL_LIGHTING);
			}
			glEndList();
		}
	}	
}

/*!
*	\brief 메쉬를 와이어프레임 형태로 렌더링 한다.
*	
*	\param r 라인의 빨간색 성분
*	\param g 라인의 초록색 성분
*	\param b 라인의 파란색 성분
*	\param width 라인의 두께
*/
void GMesh::RenderMeshWire(float r, float g, float b, float width)
{
	// 폴리곤 렌더링 모드를 라인으로 지정하고, 조명을 끄고, 색상을 지정한 후,
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_LIGHTING);
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(r, g, b, 1.0);
	glLineWidth(0.00001f);

	// 폴리곤의 타입에 따라서,
	switch (FaceType)
	{
		// 삼각형이라면,
	case FACE_TRI:
		{			
			// 정점 버퍼와 인덱스 버퍼를 이용하여 렌더링 한다.
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

		// 삼각형 팬이라면,
	case FACE_TRI_FAN:
		{
			// 일반 모드로 렌더링 한다.
			glBegin(GL_LINE_LOOP);
			for (int i = 0; i < GetNumVert(); ++i)
				glVertex3dv(Verts[i].P.V);
			glEnd();
		}
		break;

		// 사각형이라면,
	case FACE_QUAD:
		{
			// 스윕 곡면의 스위핑 평면을 렌더링 한다면,
			if (Renderer.RenderType & RENDER_SWEEP_PLANE)
			{
				// 일반 모드로 렌더링 한다.
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
			else	// 일반적인 사각 메쉬라면,
			{
				// 정점 버퍼와 인덱스 버퍼를 이용하여 렌더링 한다.
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

	// 폴리곤 렌더링 모드를 재설정하고, 조명을 활성화 한다.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glDisable(GL_BLEND);
	//glDisable(GL_LINE_SMOOTH);
	glEnable(GL_LIGHTING);
}

/*!
*	\brief 메쉬를 포인트 형태로 렌더링 한다.
*
*	\param r 정점의 빨간색 성분
*	\param g 정점의 초록색 성분
*	\param b 정점의 파란색 성분
*	\param size 정점의 크기
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
*	\brief 메쉬의 가장자리 경계를 렌더링 한다.
*
*	\param r 정점의 빨간색 성분
*	\param g 정점의 초록색 성분
*	\param b 정점의 파란색 성분
*	\param width 경계선의 두께
*/
void GMesh::RenderMeshBoundary(float r, float g, float b, float width)
{
	// 메쉬의 경계 에지의 리스트가 찾아지지 않았다면,
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
*	\brief 메쉬의 정점 버퍼를 갱신한다.
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
*	\brief 메쉬의 법선 버퍼를 갱신한다.
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
*	\brief 메쉬의 폴리곤 인덱스 버퍼를 갱신한다.
*/
void GMesh::UpdateBufFace()
{
	// 기존의 인덱스 버퍼를 초기화하고,
	FaceIdxBuf.clear();

	int NumFace = GetNumFace();
	switch (FaceType)
	{
		// 삼각형 또는 삼각형 팬 타입이라면,
	case FACE_TRI:
	case FACE_TRI_FAN:
		{
			for (int i = 0; i < NumFace; ++i)
			{
				int MtlIdx = Faces[i].MtlIdx;
				int *vidx = Faces[i].vIdx;

				// 삼각형의 정점 인덱스를 추가한다.
				FaceIdxBuf[MtlIdx].push_back(vidx[0]);
				FaceIdxBuf[MtlIdx].push_back(vidx[1]);
				FaceIdxBuf[MtlIdx].push_back(vidx[2]);
			}
		}
		break;

		// 사각형 타입이라면,
	case FACE_QUAD:
		{
			for (int i = 0; i < NumFace - 1; i += 2)
			{
				int MtlIdx = Faces[i].MtlIdx;
				int *vidx = Faces[i].vIdx;
				int vidx3 = Faces[i + 1].vIdx[2];

				// 사각형의 정점 인덱스를 추가한다.
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
*	\brief 메쉬의 경계 에지의 리스트를 초기화 한다.
*/
void GMesh::InitBndryEdgeList()
{
	BndryEdgeList.clear();
}

/*!
*	\brief 메쉬의 모델링 좌표계와 크기를 정규화 한다.
*/
void GMesh::Normalize()
{
	// 월드 좌표계에서 표현된 정점의 좌표를 구하고,
	int NumVert = GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		Verts[i].P = MC * Verts[i].P;
		Verts[i].N = MC * Verts[i].N;
	}

	// 월드좌표계에서 표현된 경계상자의 변환을 계산하고,
	// 이 변환을 메쉬의 모델링 좌표계로 설정
	MC = BBox.GetMC();

	// 메쉬의 모델링 좌표계에서 표현된 정점의 좌표를 계산한다.
	for (int i = 0; i < NumVert; ++i)
	{
		Verts[i].P = inv(MC) * Verts[i].P;
		Verts[i].N = inv(MC) * Verts[i].N;
	}

	// 모델링 좌표계를 월드 좌표계와 일치시킨다.
	MC = GTransf();

	double scale = BBox.GetDiagonal();
	// 메쉬의 모델링 좌표계에서 표현된 정점의 좌표를 계산한다.
	for (int i = 0; i < NumVert; ++i)
	{
		Verts[i].P[0] /= scale;
		Verts[i].P[1] /= scale;
		Verts[i].P[2] /= scale;
	}

	// 메쉬를 업데이트 한다.
	ModifiedFlag = MODIFIED_VERT_POS_ALL;
	UpdateMesh();
}

/*!
*	\brief 렌더링 속성의 메쉬 렌더링을 활성화 한다.
*
*	\param bSubMesh 서브메쉬의 적용여부
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
*	\brief 렌더링 속성의 메쉬 렌더링을 비활성화 한다.
*
*	\param bSubMesh 서브메쉬의 적용여부
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
*	\brief 메쉬의 바운딩 볼륨 계층(BVH) 구조를 생성한다.
*	\note PQP 라이브러리를 이용한다.
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
*	\brief 메쉬의 바운딩 볼륨 계층(BVH) 구조를 제거한다.
*/
void GMesh::DeleteBVH()
{
	if (pBVH != NULL)
		delete pBVH;

	CollisionFaces.clear();
	pBVH = NULL;
}

/****************************/
/* GRefBoneInfo 클래스 구현 */
/****************************/

/*!
*	\brief 디폴트 생성자
*/
GRefBoneInfo::GRefBoneInfo()
{
	pRefMC = NULL;
	Wgt = 0.0;
}

/*!
*	\brief 생성자
*/
GRefBoneInfo::GRefBoneInfo(std::string &_BoneMeshName, double _Wgt, GPoint3 &_P, GVector3 &_N)
: BoneMeshName(_BoneMeshName), pRefMC(NULL), Wgt(_Wgt), P(_P), N(_N)
{
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사될 객체
*/
GRefBoneInfo::GRefBoneInfo(const GRefBoneInfo &cpy)
: BoneMeshName(cpy.BoneMeshName), pRefMC(cpy.pRefMC), Wgt(cpy.Wgt), P(cpy.P), N(cpy.N)
{
}

/*!
*	\brief 소멸자
*/
GRefBoneInfo::~GRefBoneInfo()
{
}

/*!
*	\brief 대입연산자
*
*	\param rhs 대입될 객체
*
*	\return 대입된 자신을 반환
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
/* GPathInfo 클래스 구현 */
/*************************/

/*!
*	\brief 생성자
*/
GPathInfo::GPathInfo(int _sidx, int _eidx, double _t)
{
	sidx = _sidx;
	eidx = _eidx;
	t = _t;
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사될 객체
*/
GPathInfo::GPathInfo(const GPathInfo &cpy)
{
	sidx = cpy.sidx;
	eidx = cpy.eidx;
	t = cpy.t;
}

/*!
*	\brief 소멸자
*/
GPathInfo::~GPathInfo()
{
}

/*!
*	\brief 대입연산자
*
*	\param rhs 대입될 객체
*
*	\return 대입된 자신을 반환
*/
GPathInfo &GPathInfo::operator =(const GPathInfo &rhs)
{
	sidx = rhs.sidx;
	eidx = rhs.eidx;
	t = rhs.t;

	return *this;
}

/***********************/
/* GCtrlPt 클래스 구현 */
/***********************/
IMPLEMENT_GEDIT_OBJECT(GCtrlPt)

/*!
*	\brief 제어점이 포함된 메쉬의 포인터를 반환한다.
*
*	\return 제어점이 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GCtrlPt::GetRefMesh()
{
	return pMesh;
}

/*!
*	\brief 제어점의 접평면 좌표계를 반환한다.
*
*	\return 월드 좌표계에서 표현된 제어점의 접평면 좌표계를 반환한다.
*/
GTransf GCtrlPt::GetTransf()
{
	return pMesh->MC * GTransf(GVector3(V[0], V[1], V[2]));
}

/*!
*	\brief 선택된 제어점을 편집한다.
*
*	\param EC 편집 좌표계
*	\param t 편집될 객체의 이동 변위
*	\param q 편집될 객체의 회전 변위
*	\param s 편집될 객체의 스케일 변위
*	\param pParentMesh 편집 객체의 부모 메쉬
*/
void GCtrlPt::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	// 편집 변환을 위한 변수를 선언하고,
	static GTransf MC2MC;

	// 편집되는 첫번째 정점이라면,
	if (pMesh->ModifiedFlag == MODIFIED_NONE)
	{
		// 편집 변환을 계산하고,
		MC2MC = inv(pMesh->MC) * EC * GTransf(t, q) * inv(EC) * pMesh->MC;

		// 정점의 편집을 기록하고,
		pMesh->ModifiedFlag = MODIFIED_CTLPT_POS;						
	}

	// 편집된 정점의 좌표를 계산하여,
	GPoint3 pt = MC2MC * GPoint3(V[0], V[1], V[2]);

	// 스케일 변환을 수행하고,
	if (s[0] != 1.0) pt[0] *= s[0];
	if (s[1] != 1.0) pt[1] *= s[1];
	if (s[2] != 1.0) pt[2] *= s[2];

	// 최종 정점의 위치를 수정한다.
	V[0] = pt[0];
	V[1] = pt[1];
	V[2] = pt[2];
}

/*!
*	\brief 편집 제어점을 렌더링 한다.
*/
void GCtrlPt::RenderForEdit(float r, float g, float b)
{
	// 현재 좌표계: 월드(장면)
	glPushMatrix();
	{
		// 객체의 모델링 좌표계로 변환
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
*	\brief 디폴트 생성자
*/
GCtrlPt::GCtrlPt()
: GVector4()
{
	pMesh = NULL;
}

/*!
*	\brief 생성자
*
*	\param _P 제어점의 좌표
*/
GCtrlPt::GCtrlPt(GVector4 _P)
: GVector4(_P)
{
	pMesh = NULL;
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사될 객체
*/
GCtrlPt::GCtrlPt(const GCtrlPt &cpy)
: GVector4(cpy)
{
	pMesh = cpy.pMesh;
}

/*!
*	\brief 소멸자
*/
GCtrlPt::~GCtrlPt()
{
}

/*!
*	\brief 대입연산자
*
*	\param rhs 대입될 객체
*	
*	\return 대입 후, 자신을 반환한다.
*/
GCtrlPt &GCtrlPt::operator =(const GVector4 &rhs)
{
	GVector4::operator =(rhs);

	return *this;
}

/*!
*	\brief 대입연산자
*
*	\param rhs 대입될 객체
*	
*	\return 대입 후, 자신을 반환한다.
*/
GCtrlPt &GCtrlPt::operator =(const GCtrlPt &rhs)
{
	GVector4::operator =(rhs);

	pMesh = rhs.pMesh;

	return *this;
}

/*!
*	\brief (+)연산자
*
*	\param lhs 왼쪽 피연산자
*	\param rhs 오른쪽 피연산자
*	
*	\return 연산의 결과를 반환한다.
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
*	\brief (+)연산자
*
*	\param lhs 왼쪽 피연산자
*	\param rhs 오른쪽 피연산자
*	
*	\return 연산의 결과를 반환한다.
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
*	\brief (+)연산자
*
*	\param lhs 왼쪽 피연산자
*	\param rhs 오른쪽 피연산자
*	
*	\return 연산의 결과를 반환한다.
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
*	\brief 점이 포함된 메쉬의 포인터를 반환한다.
*
*	\return 점이 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GCutPt::GetRefMesh()
{
	if (pFace == NULL)
		return NULL;
	else
		return pFace->pMesh;
}

/*!
*	\brief 절단점의 접평면 좌표계를 반환한다.
*
*	\return 월드 좌표계에서 표현된 절단점의 접평면 좌표계를 반환한다.
*/
GTransf GCutPt::GetTransf()
{
	GTransf ret;

	if (pFace != NULL)
		ret = pFace->pMesh->MC * GTransf(cast_vec3(P));

	return ret;
}

/*!
*	\brief 선택된 점을 편집한다.
*
*	\param EC 편집 좌표계
*	\param t 편집될 객체의 이동 변위
*	\param q 편집될 객체의 회전 변위
*	\param s 편집될 객체의 스케일 변위
*	\param pParentMesh 편집 객체의 부모 메쉬
*/
void GCutPt::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	// 제어점의 위치를 수정한다.
	P[0] += t[0];
	P[1] += t[1];
	P[2] += t[2];
}

/*!
*	\brief 점을 렌더링 한다.
*/
void GCutPt::RenderForEdit(float r, float g, float b)
{
	if (pFace == NULL)
		return;

	// 현재 좌표계: 월드(장면)
	glPushMatrix();
	{
		// 객체의 모델링 좌표계로 변환
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
*	\brief 디폴트 생성자
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
*	\brief 생성자
*
*	\param _pFace 점이 놓여 있는 모서리를 포함한 삼각형
*	\param vidx0 점이 놓여 있는 모서리의 시작 정점의 인덱스
*	\param vidx1 점이 놓여 있는 모서리의 끝 정점의 인덱스
*	\param _P 정점의 위치
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
*	\brief 복사 생성자
*
*	\param cpy 복사될 객체
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
*	\brief 소멸자
*/
GCutPt::~GCutPt()
{
}

/*!
*	\brief 대입연산자
*
*	\param rhs 대입될 객체
*	
*	\return 대입 후, 자신을 반환한다.
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
*	\brief 디폴트 생성자
*/
GCirArc::GCirArc()
{
	// 기본 데이터 멤버 (5)
	Center = GPoint3(0.0, 0.0, 0.0);	// 아크의 중심
	Radius = 1.0;						// 아크의 반지름
	AngleStart = 0.0;					// 아크의 시작 각도
	AngleEnd = 2 * M_PI;				// 아크의 끝 각도
	Direction = DIR_CCW;				// 아크의 방향
	LenTangent = 0.0;					// 아크의 끝부분 접선의 길이
}

/*!
*	\brief 생성자
*
*	\param _Center 아크의 중심
*	\param _Radius 아크의 반경
*	\param _AngleStart 아크의 시작 각도
*	\param _AngleEnd 아크의 종료 각도
*	\param _Direction 아크의 방향
*/
GCirArc::GCirArc(GPoint3 _Center, double _Radius, double _AngleStart, double _AngleEnd, TypeCirDir _Direction, double _LenTangent)
{
	Center = _Center;					// 아크의 중심
	Radius = _Radius;					// 아크의 반지름
	AngleStart = DEG2RAD(_AngleStart);	// 아크의 시작 각도
	AngleEnd = DEG2RAD(_AngleEnd);		// 아크의 끝 각도
	Direction = _Direction;				// 아크의 방향
	LenTangent = _LenTangent;			// 아크의 끝부분 접선의 길이

	// 아크의 방향에 따라 시작 각도와 끝 각도를 수정한다.
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
*	\brief 복사생성자
*
*	\param cpy 복사될 객체
*/
GCirArc::GCirArc(const GCirArc &cpy)
{
	Center = cpy.Center;			// 아크의 중심
	Radius = cpy.Radius;			// 아크의 반지름
	AngleStart = cpy.AngleStart;	// 아크의 시작 각도
	AngleEnd = cpy.AngleEnd;		// 아크의 끝 각도
	Direction = cpy.Direction;		// 아크의 방향
	LenTangent = cpy.LenTangent;	// 아크의 끝부분 접선의 길이
}

/*!
*	\brief 소멸자
*/
GCirArc::~GCirArc()
{
}

/*!
*	\brief 대입 연산자
*
*	\param rhs 오른쪽 피연산자
*
*	\return 대입된 자신을 반환한다.
*/
GCirArc &GCirArc::operator =(const GCirArc &rhs)
{
	Center = rhs.Center;			// 아크의 중심
	Radius = rhs.Radius;			// 아크의 반지름
	AngleStart = rhs.AngleStart;	// 아크의 시작 각도
	AngleEnd = rhs.AngleEnd;		// 아크의 끝 각도
	Direction = rhs.Direction;		// 아크의 방향
	LenTangent = rhs.LenTangent;	// 아크의 끝부분 접선의 길이

	return *this;
}

/*!
*	\brief 아크위의 점을 계산
*
*	\param t 매개변수의 값
*
*	\return 곡선위의 점 C(t)를 계산
*/
GVector4 GCirArc::Eval(double t)
{
	double theta = AngleStart + (AngleEnd - AngleStart) * t;
	double x = Radius * cos(theta) + Center[0];
	double y = Radius * sin(theta) + Center[1];

	return GVector4(x, y, 0.0, 0.0);
}

/*!
*	\brief 아크위의 접선의 방향을 계산한다.
*
*	\param t 매개변수의 값
*
*	\return 아크위의 접선의 방향 C'(t)를 계산하여 반환한다.
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

