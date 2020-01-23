#include "stdafx.h"

/****************************************/
/* Implementation of GBlendMesh Class */
/****************************************/
IMPLEMENT_GMESH_OBJECT(GBlendMesh)

/*!
*	\brief 객체가 포함된 메쉬의 포인터를 반환한다.
*
*	\return 객체가 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GBlendMesh::GetRefMesh()
{
	return this;
}

/*!
*	\brief 객체의 모델링 좌표계를 반환한다.
*
*	\return 월드 좌표계에서 표현된 객체의 모델링 좌표계를 반환한다.
*/
GTransf GBlendMesh::GetTransf()
{
	return MC;
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
void GBlendMesh::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);	
}

/*!
*	\brief	메쉬의 모델링 좌표계를 월드 좌표계로 갱신한다.
*
*	\param RefType 모델링 좌표계의 갱신을 위한 참조 형태(0: 월드 좌표계, 1: 경계상자 좌표계)
*/
void GBlendMesh::UpdateMC(int RefType)
{
}

/*!
*	\brief	곡면 메쉬의 정점 리스트를 다시 생성한다.
*/
void GBlendMesh::UpdateMesh() 
{
	switch (ModifiedFlag)
	{
	case MODIFIED_VERT_POS:
		break;

	case MODIFIED_VERT_NUM:		// 베지에 패치의 경계가 꿰메진 경우...
		InitBndryEdgeList();
		UpdateNormal(false);
		UpdateBufVertex();
		UpdateBufNormal();
		UpdateBufFace();
		break;

		// 재질이 변경되었다면,
	case MODIFIED_MTL:
		UpdateBufFace();
		break;

	case MODIFIED_INTPT_POS:
		CreateMesh();
		break;

	case MODIFIED_CTLPT_POS:
		break;

	case MODIFIED_SAMPLE_NUM:
		CreateMesh();
		break;
	}

	ModifiedFlag = MODIFIED_NONE;
}	

/*!
*	\brief 선택된 객체를 렌더링 한다.
*/
void GBlendMesh::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);
}

/*!
*	\brief 생성자
*
*	\param _Name 이름
*	\param _MC 객체의 모델링 좌표계
*	\param _pCtrlMesh 제어 메쉬
*/
GBlendMesh::GBlendMesh(std::string _Name, GTransf _MC, GMesh *_pCtrlMesh)
: GMesh(_Name, _MC, FACE_TRI, _pCtrlMesh->Name)
{
	// 제어 메쉬를 지정하고,
	pCtrlMesh = _pCtrlMesh;
	pCtrlMesh->SemanticType = MESH_CTRL;
	pCtrlMesh->LinkedMeshNameList.push_back(Name);

	// 삼각형의 개수를 구하여,
	int NumFace = pCtrlMesh->GetNumFace();

	// 각 삼각형에서 정의되는 패치를 초기화한다.
	PatchList.assign(NumFace, GTriBzrSrf("", GTransf()));

	// 각 삼각형에서 정의되는 블렌딩 오프셋을 초기화 한다.
	BlendOffset.assign(NumFace, GVector3(0.5, 0.5, 0.5));

	// 각 삼각형에서 정의되는 패치의 타입을 초기화 한다.
	PatchType.assign(NumFace, (PATCH_TYPE_MODIFIED | PATCH_TYPE_BZR_TRI));

	// 각 삼각형에 인접한 삼각형 정보를 생성한다.
	get_neighbor_faces_of_face(pCtrlMesh, AdjFaceListOfFace);
}

/*!
*	\brief 베지에 곡면의 복사 생성자
*
*	\param cpy 복사될 객체
*/
GBlendMesh::GBlendMesh(const GBlendMesh &cpy)
: GMesh(cpy)
{
	pCtrlMesh = cpy.pCtrlMesh;
	PatchList = cpy.PatchList;
	BlendOffset = cpy.BlendOffset;
	PatchType = cpy.PatchType;
	AdjFaceListOfFace = cpy.AdjFaceListOfFace;
}

/*!
*	\brief 베지에 곡면의 소멸자
*/
GBlendMesh::~GBlendMesh()
{
}

/*!
*	\brief 대입 연산자
*
*	\param rhs 대입될 객체
*
*	\return 대입된 자신을 반환한다.
*/
GBlendMesh &GBlendMesh::operator =(const GBlendMesh &rhs)
{
	GMesh::operator =(rhs);

	pCtrlMesh = rhs.pCtrlMesh;
	PatchList = rhs.PatchList;
	BlendOffset = rhs.BlendOffset;
	PatchType = rhs.PatchType;
	AdjFaceListOfFace = rhs.AdjFaceListOfFace;

	return *this;
}

/*!
*	\brief 렌더링 한다.
*
*	\param bSubMesh 서브 메쉬의 렌더링 여부(선택 모드에서는 false로 설정)
*/
void GBlendMesh::Render(bool bSubMesh)
{
	GMesh::Render(bSubMesh);
}

/*!
*	\brief 메쉬를 생성한다.
*/
void GBlendMesh::CreateMesh()
{
	// 경계 정점의 리스트를 초기화 하고,
	InitBndryEdgeList();

	// 삼각형에서 정의되는 삼각형 베지에 패치를 생성하고,
	CreateLocalPatch();

	// 정점의 좌표를 생성하고,
	CreateMeshVert();

	// 텍스처 좌표를 생성하고,
	CreateMeshTex();

	// 삼각형 정보를 생성하고, 
	CreateMeshFace();

	// 정점의 법선을 계산하고,
	//UpdateNormal(false);

	// 곡면의 바운딩 박스를 계산한다.
	UpdateBndBox(false);

	// 정점 버퍼와 인덱스 버퍼를 갱신한다.
	UpdateBufVertex();
	UpdateBufNormal();
	UpdateBufFace();
}

/*!
*	\brief 패치 타입에 따라 각각의 삼각형에서 정의되는 베지에 패치를 생성한다.
*/
void GBlendMesh::CreateLocalPatch()
{
	// 삼각형 개수를 얻어와서, 
	int NumFace = pCtrlMesh->GetNumFace();

	// 각각의 삼각형마다 삼각형 베지에 곡면을 생성한다.
	for (int i = 0; i < NumFace; ++i)
	{
		// 각 삼각형 마다,
		GFace *pFace = &(pCtrlMesh->Faces[i]);

		// 패치의 타입이 변경되었다면, (혹은 최초 생성시)
		if (PatchType[i] & PATCH_TYPE_MODIFIED)
		{
			PatchType[i] &= ~(PATCH_TYPE_MODIFIED);

			// 패치 타입이 일반 PN 삼각형이라면,
			if (PatchType[i] & PATCH_TYPE_BZR_TRI)
			{
				GTriBzrSrf *pPatch = ::get_tri_bzr_srf(pFace, false);
				PatchList[i] = *pPatch;
				delete pPatch;
			}
			// 패치 타입이 확장 PN 삼각형이라면,
			else if (PatchType[i] & PATCH_TYPE_BZR_TRI_EXT)
			{
				// 인접한 삼각형의 인덱스를 통해서,
				int fidx0 = AdjFaceListOfFace[i * 3];		// e0을 공유하는 삼각형의 인덱스
				int fidx1 = AdjFaceListOfFace[i * 3 + 1];	// e1을 공유하는 삼각형의 인덱스
				int fidx2 = AdjFaceListOfFace[i * 3 + 2];	// e2을 공유하는 삼각형의 인덱스

				GVector4 q0, q1, q2;
				bool flag0 = false;
				bool flag1 = false;
				bool flag2 = false;

				// e0를 공유한 삼각형이 있다면,
				if (fidx0 != -1)
				{
					GFace *pFace0 = &(pCtrlMesh->Faces[fidx0]);
					int vidx = pFace0->GetNextVertIdx(pFace->vIdx[1]);
					q0 = cast_vec4(pCtrlMesh->Verts[vidx].P);
					flag0 = true;
				}
				// e1를 공유한 삼각형이 있다면,
				if (fidx1 != -1)
				{
					GFace *pFace1 = &(pCtrlMesh->Faces[fidx1]);
					int vidx = pFace1->GetNextVertIdx(pFace->vIdx[2]);
					q1 = cast_vec4(pCtrlMesh->Verts[vidx].P);
					flag1 = true;
				}

				// e2를 공유한 삼각형이 있다면,
				if (fidx2 != -1)
				{
					GFace *pFace2 = &(pCtrlMesh->Faces[fidx2]);
					int vidx = pFace2->GetNextVertIdx(pFace->vIdx[0]);
					q2 = cast_vec4(pCtrlMesh->Verts[vidx].P);
					flag2 = true;
				}

				GTriBzrSrf *pPatch = ::get_tri_bzr_srf(pFace, q0, q1, q2, flag0, flag1, flag2);
				PatchList[i] = *pPatch;
				delete pPatch;
			}
			// 평면 삼각형이라면,
			else if (PatchType[i] & PATCH_TYPE_BZR_TRI_PLN)
			{
				GTriBzrSrf *pPatch = ::get_tri_bzr_srf(pFace, true);
				PatchList[i] = *pPatch;
				delete pPatch;
			}
		}		
	}
}

double GetBlendingBound(double h, double s)
{
	double r = 0.0;

	if (0.0 <= s && s < 1.5 * h)
	{
		r = s / SQRT(3.0);
	}
	if (1.5 * h <= s && s < 2 * h)
	{
		r = SQRT(SQR(h) - SQR(2 * h - s));
	}
	if (2 * h <= s && s < SQRT(2.0) - 2 * h)
	{
		r = h;
	}
	if (SQRT(2.0) - 2 * h <= s && s < SQRT(2.0) - 1.5 * h)
	{
		r = SQRT(SQR(h) - SQR(s - SQRT(2.0) + 2 * h));
	}
	if (SQRT(2.0) - 1.5 * h <= s && s <= SQRT(2.0))
	{
		r = (SQRT(2.0) - s) / SQRT(3.0);
	}

	return r;	
}

void GetLocalParam(GFace *pSrcFace, double u, double v, double w, GFace *pTarFace, double &uu, double &vv, double &ww)
{
	// u 좌표가 제일 작은 경우,
	if (u <= v && u <= w)
	{
		if (pSrcFace->vIdx[1] == pTarFace->vIdx[1])
		{
			uu = 1.0 - v;
			vv = 1.0 - w;
			ww = v + w - 1.0;
		}
		if (pSrcFace->vIdx[1] == pTarFace->vIdx[0])
		{
			uu = 1.0 - w;
			vv = v + w - 1.0;
			ww = 1.0 - v;
		}
		if (pSrcFace->vIdx[1] == pTarFace->vIdx[2])
		{
			uu = v + w - 1.0;
			vv = 1.0 - v;
			ww = 1.0 - w;
		}
	}
	// v 좌표가 제일 작은 경우,
	else if (v <= u && v <= w)
	{
		if (pSrcFace->vIdx[2] == pTarFace->vIdx[1])
		{
			uu = 1.0 - w;
			vv = 1.0 - u;
			ww = u + w - 1.0;
		}
		if (pSrcFace->vIdx[2] == pTarFace->vIdx[0])
		{
			uu = 1.0 - u;
			vv = u + w - 1.0;
			ww = 1.0 - w;
		}
		if (pSrcFace->vIdx[2] == pTarFace->vIdx[2])
		{
			uu = u + w - 1.0;
			vv = 1.0 - w;
			ww = 1.0 - u;							
		}
	}
	// w 좌표가 제일 작은 경우,
	else
	{
		if (pSrcFace->vIdx[0] == pTarFace->vIdx[1])
		{
			uu = 1.0 - u;
			vv = 1.0 - v;
			ww = u + v - 1.0;
		}
		if (pSrcFace->vIdx[0] == pTarFace->vIdx[0])
		{
			uu = 1.0 - v;
			vv = u + v - 1.0;
			ww = 1.0 - u;
		}
		if (pSrcFace->vIdx[0] == pTarFace->vIdx[2])
		{
			uu = u + v - 1.0;
			vv = 1.0 - u;
			ww = 1.0 - v;
		}
	}
}

/*!
*	\brief 정점의 좌표를 생성한다.
*/
void GBlendMesh::CreateMeshVert()
{
	// 기존의 정점을 지우고, 
	Verts.clear();

	// 삼각형의 개수를 구하고,
	int NumFace = pCtrlMesh->GetNumFace();

	for (int i = 0; i < NumFace; ++i)
	{
		// 각 삼각형 마다,
		GFace *pFace = &(pCtrlMesh->Faces[i]);

		// 인접한 삼각형의 인덱스를 통해서,
		int fidx0 = AdjFaceListOfFace[i * 3];		// e0를 공유하는 삼각형의 인덱스
		int fidx1 = AdjFaceListOfFace[i * 3 + 1];	// e1를 공유하는 삼각형의 인덱스
		int fidx2 = AdjFaceListOfFace[i * 3 + 2];	// e2를 공유하는 삼각형의 인덱스

		// 인접한 삼각형을 구하고,
		GFace *pFace0 = (fidx0 == -1) ? NULL : &(pCtrlMesh->Faces[fidx0]);
		GFace *pFace1 = (fidx1 == -1) ? NULL : &(pCtrlMesh->Faces[fidx1]);
		GFace *pFace2 = (fidx2 == -1) ? NULL : &(pCtrlMesh->Faces[fidx2]);

		// 현재 삼각형 및 인접한 삼각형에서 정의되는 삼각형 베지에 곡면을 구하고,
		GTriBzrSrf *pSrf = &PatchList[i];
		GTriBzrSrf *pSrf0 = (fidx0 == -1) ? NULL : &PatchList[fidx0];
		GTriBzrSrf *pSrf1 = (fidx1 == -1) ? NULL : &PatchList[fidx1];
		GTriBzrSrf *pSrf2 = (fidx2 == -1) ? NULL : &PatchList[fidx2];

		// 샘플링 정보를 얻어와서, 
		int snum = Renderer.NumSampU;

		// 삼각형 베지에 곡면의 정점을 계산하여,
		for (int j = 0; j < snum; ++j)
		{
			double u = (double)j / (double)(snum - 1);
			for (int k = 0; k < snum - j; ++k)
			{
				double v = (double)k / (double)(snum - 1);
				double w = 1.0 - u - v;
				double uu, vv, ww, h = 0.0, s = 0.0, d = 0.0;
				
				// 현재 삼각형에서 정의되는 곡면의 점을 계산하고,
				GVector4 p = pSrf->Eval(u, v, w);
				GVector4 q = p;

				GVector3 pn = pSrf->EvalNormal(u, v, w);
				GVector3 qn = pn;
				
				// u 좌표가 제일 작은 경우, pSrf0(u0, v0, w0)와 블렌딩을 한다.
				if (u <= v && u <= w)
				{
					// 인접한 삼각형이 있다면, 블렌딩을 수행한다.
					if (pFace0 != NULL)
					{
						GetLocalParam(pFace, u, v, w, pFace0, uu, vv, ww);
						q = pSrf0->Eval(uu, vv, ww);
						qn = pSrf0->EvalNormal(uu, vv, ww);

						h = BlendOffset[i][0] * SQRT(2.0) / 4.0;
						s = (1.0 - v + w) / SQRT(2.0);
						d = SQRT(SQR(u) + SQR(v - 1.0) + SQR(w) - SQR(s));
					}
				}
				// v 좌표가 제일 작은 경우, pSrf1(u1, v1, w1)와 블렌딩을 한다.
				else if (v <= u && v <= w)
				{
					if (pFace1 != NULL)
					{
						GetLocalParam(pFace, u, v, w, pFace1, uu, vv, ww);
						q = pSrf1->Eval(uu, vv, ww);
						qn = pSrf1->EvalNormal(uu, vv, ww);

						h = BlendOffset[i][1] * SQRT(2.0) / 4.0;
						s = (u - w + 1.0) / SQRT(2.0);
						d = SQRT(SQR(u) + SQR(v) + SQR(w - 1.0) - SQR(s));						
					}					
				}
				// w 좌표가 제일 작은 경우, pSrf2(u2, v2, w2)와 블렌딩을 한다.
				else
				{
					if (pFace2 != NULL)
					{
						GetLocalParam(pFace, u, v, w, pFace2, uu, vv, ww);
						q = pSrf2->Eval(uu, vv, ww);
						qn = pSrf2->EvalNormal(uu, vv, ww);

						h = BlendOffset[i][2] * SQRT(2.0) / 4.0;
						s = (v - u + 1.0) / SQRT(2.0);
						d = SQRT(SQR(u - 1.0) + SQR(v) + SQR(w) - SQR(s));						
					}					
				}

				double r = GetBlendingBound(h, s);
				double t = (r - d) / (2 * r);
				double alpha = (d >= r) ? 1.0 : (2 * t * t * t - 3 * t * t + 1);	// -6 * t * t * t * t * t + 15 * t * t * t * t - 10 * t * t * t + 1;												

				if (EQ_ZERO(u, 0.00001) || EQ_ZERO(v, 0.00001) || EQ_ZERO(w, 0.00001))
					alpha = 0.5;

				if (EQ_ZERO(r, 0.00001))
				{
					q = p;
					qn = pn;
				}

				// 블렌딩 점을 계산한다.
				p = (1.0 - alpha) * q + alpha * p;
				pn = (1.0 - alpha) * qn + alpha * pn;

				// 정점 리스트에 추가
				GVertex vtx(GPoint3(p[0], p[1], p[2]), pn, GVector3(alpha, alpha, alpha));
				AddVertex(vtx);
			}
		}
	}
}

/*!
*	\brief 텍스처 좌표를 생성한다.
*/ 
void GBlendMesh::CreateMeshTex()
{
}

/*!
*	\brief 삼각형 정보를 생성한다.
*/ 
void GBlendMesh::CreateMeshFace()
{
	// 삼각형 정보를 지우고,
	Faces.clear();

	// 샘플링 정보를 얻어와서, 
	int snum = Renderer.NumSampU;

	// 삼각형 정보를 생성한다.
	int vidx0 = 0;

	int fnum = pCtrlMesh->GetNumFace();
	for (int f = 0; f < fnum; ++f)
	{
		for (int i = 0; i < snum - 1; ++i)
		{
			for(int j = 0; j < snum - 1 - i; ++j)
			{
				int vidx1 = vidx0 + 1;
				int vidx2 = vidx1 + snum - i;
				int vidx3 = vidx2 - 1;

				GFace f0(vidx0, vidx3, vidx1);
				AddFace(f0);

				if (j != snum - 2 - i)
				{
					GFace f1(vidx1, vidx3, vidx2);
					AddFace(f1);
				}
				vidx0++;
			}
			vidx0++;
		}
		vidx0++;
	}
}

/*!
*	\brief 객체 위의 점을 계산한다.
*
*	\param u 파라미터
*
*	\return 베지에 곡면위의 점 S(u, v, w)을 반환한다.
*/
GVector4 GBlendMesh::Eval(double u, ...)
{
	return GVector4();
}

/*!
*	\brief 베지에 곡면의 파라미터 영역 [umin, umax] x [vmin, vmax]을 구한다.
*
*	\param umin U-방향 파라미터의 시작 값이 저장됨
*/
void GBlendMesh::GetDomain(double *umin, ...)
{
	double *umax, *vmin, *vmax;
	va_list ap;

	va_start(ap, umin);
	umax = va_arg(ap, double *);
	vmin = va_arg(ap, double *);
	vmax = va_arg(ap, double *);
	va_end(ap);

	*umin = 0.0;
	*umax = 1.0;

	*vmin = 0.0;
	*vmax = 1.0;
}

/*!
*	\brief	곡면의 제어점 리스트를 반환한다.
*
*	\return 곡면의 제어점 리스트를 반환한다.
*/
void GBlendMesh::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
}