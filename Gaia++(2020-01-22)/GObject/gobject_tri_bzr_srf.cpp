#include "stdafx.h"

/*************************************/
/* Implementation of GTriBzrSrf Class */
/*************************************/
IMPLEMENT_GMESH_OBJECT(GTriBzrSrf)

/*!
*	\brief 객체가 포함된 메쉬의 포인터를 반환한다.
*
*	\return 객체가 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GTriBzrSrf::GetRefMesh()
{
	return this;
}

/*!
*	\brief 객체의 모델링 좌표계를 반환한다.
*
*	\return 월드 좌표계에서 표현된 객체의 모델링 좌표계를 반환한다.
*/
GTransf GTriBzrSrf::GetTransf()
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
void GTriBzrSrf::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);	
}

/*!
*	\brief	메쉬의 모델링 좌표계를 월드 좌표계로 갱신한다.
*
*	\param RefType 모델링 좌표계의 갱신을 위한 참조 형태(0: 월드 좌표계, 1: 경계상자 좌표계)
*/
void GTriBzrSrf::UpdateMC(int RefType)
{
}

/*!
*	\brief	곡면 메쉬의 정점 리스트를 다시 생성한다.
*/
void GTriBzrSrf::UpdateMesh() 
{
	// 갱신 정보에 따라서,
	switch (ModifiedFlag)
	{
		// 재질이 변경되었다면,
	case MODIFIED_MTL:
		UpdateBufFace();
		break;

	case MODIFIED_INTPT_POS:
		CreateMeshVert();
		UpdateBndBox(false);
		UpdateBufVertex();
		break;

	case MODIFIED_CTLPT_POS:
		CreateMeshVert();
		UpdateBndBox(false);
		UpdateBufVertex();
		break;

	case MODIFIED_NORMAL:
		UpdateNormal(false);
		UpdateBufNormal();
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
void GTriBzrSrf::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);
}

/*!
*	\brief 베지에 곡면의 제어점 배열을 할당한다.
*
*	\param _p 차수
*/
void GTriBzrSrf::_alloc(int _p)
{
	p = _p;
	n = (p + 1) * (p + 2) / 2 - 1;
	P = new GCtrlPt [n + 1];
	for (int i = 0; i < n + 1; ++i)
		P[i].pMesh = this;
}

/*!
*	\brief 베지에 곡면의 제어점 배열을 해제한다.
*/
void GTriBzrSrf::_free()
{
	if (P != NULL)
		delete [] P;
	P = NULL;
}

/*!
*	\brief 베지에 곡면의 생성자
*
*	\param _Name 곡면의 이름
*	\param _MC 곡면의 모델링 좌표계
*	\param _p 차수
*	\param _P 제어점 배열
*	\param _ParentName 부모 곡면의 이름
*/
GTriBzrSrf::GTriBzrSrf(std::string _Name, GTransf _MC, int _p, GVector4 *_P, std::string _ParentName)
: GMesh(_Name, _MC, FACE_TRI, _ParentName)
{
	// 현재 4차까지 지원
	assert(_p <= 4);
	_alloc(_p);

	for (int i = 0; i < n + 1; ++i)
	{
		if (_P == NULL)
			P[i].Set(0.0, 0.0, 0.0, 0.0);
		else
			P[i] = _P[i];
	}
}

/*!
*	\brief 베지에 곡면의 복사 생성자
*
*	\param cpy 복사될 객체
*/
GTriBzrSrf::GTriBzrSrf(const GTriBzrSrf &cpy)
: GMesh(cpy)
{
	_alloc(cpy.p);
	for (int i = 0; i < n + 1; ++i)
		P[i] = cpy.P[i];
}

/*!
*	\brief 베지에 곡면의 소멸자
*/
GTriBzrSrf::~GTriBzrSrf()
{
	_free();
}

/*!
*	\brief 베지에 곡면의 대입 연산자
*
*	\param rhs 대입될 객체
*
*	\return 대입된 자신을 반환한다.
*/
GTriBzrSrf &GTriBzrSrf::operator =(const GTriBzrSrf &rhs)
{
	GMesh::operator =(rhs);

	_free();
	_alloc(rhs.p);
	for (int i = 0; i < n + 1; ++i)
		P[i] = rhs.P[i];

	return *this;
}

/*!
*	\brief 곡면을 렌더링 한다.
*
*	\param bSubMesh 서브 메쉬의 렌더링 여부(선택 모드에서는 false로 설정)
*/
void GTriBzrSrf::Render(bool bSubMesh)
{
	GMesh::Render(bSubMesh);

	glPushMatrix();
	{
		// 월드 좌표계에서 객체의 모델링 좌표계로의 변환하고,
		glMultMatrixd(cast_mat(MC).M);

		// 제어점을 렌더링 한다면,
		if (Renderer.RenderType & RENDER_CTRL_PT)
		{
			// 재질을 설정하고,
			GMaterial Mat;
			Mat.SetAmbient(0.7f, 0.7f, 0.7f);
			Mat.SetDiffuse(0.7f, 0.7f, 0.7f);
			Mat.SetSpecular(0.7f, 0.7f, 0.7f);
			Mat.Enable(false);

			// 제어점의 크기를 구하고,
			double size = Renderer.CtrlPtSize;
			
			// 제어점을 렌더링 한다.
			int m = -1;
			for (int i = 0; i < p; ++i)
			{
				for (int j = 0; j < i + 1; ++j)
				{
					m++;
					glPushMatrix();
					glTranslated(P[m][0], P[m][1], P[m][2]);
					glutSolidSphere(size, 10, 10);
					glPopMatrix();

					glPushMatrix();
					glTranslated(P[m + i + 1][0], P[m + i + 1][1], P[m + i + 1][2]);
					glutSolidSphere(size, 10, 10);
					glPopMatrix();

					glPushMatrix();
					glTranslated(P[m + i + 2][0], P[m + i + 2][1], P[m + i + 2][2]);
					glutSolidSphere(size, 10, 10);
					glPopMatrix();
				}
			}

			// 붉은색 재질을 설정하여,
			Mat.SetAmbient(0.7f, 0.0f, 0.0f);
			Mat.SetDiffuse(0.7f, 0.0f, 0.0f);
			Mat.SetSpecular(0.7f, 0.0f, 0.0f);
			Mat.Enable(false);

			// 시작 제어점을 표시한다.
			glPushMatrix();
			glTranslated(P[0][0], P[0][1], P[0][2]);
			glutSolidSphere(size, 10, 10);
			glPopMatrix();

			// 조명을 끄고, 선 두께를 2.0으로, 색상을 파랑으로 설정하고,
			glDisable(GL_LIGHTING);
			glLineWidth(2.0);
			glColor3d(0.0, 0.0, 1.0);

			// 제어 메쉬를 렌더랑한다.
			m = -1;
			for (int i = 0; i < p; ++i)
			{
				for (int j = 0; j < i + 1; ++j)
				{
					m++;
					glBegin(GL_LINE_LOOP);
					glVertex3d(P[m][0], P[m][1], P[m][2]);
					glVertex3d(P[m + i + 1][0], P[m + i + 1][1], P[m + i + 1][2]);
					glVertex3d(P[m + i + 2][0], P[m + i + 2][1], P[m + i + 2][2]);
					glEnd();
				}
			}
			
			// 조명을 켜고, 선 두께들 1.0으로 재설정한다.
			glLineWidth(1.0);
			glEnable(GL_LIGHTING);
		}
	}
	glPopMatrix();
}

/*!
*	\brief 메쉬를 생성한다.
*/
void GTriBzrSrf::CreateMesh()
{
	// 경계 정점의 리스트를 초기화 하고,
	InitBndryEdgeList();

	// 정점의 좌표를 생성하고,
	CreateMeshVert();

	// 텍스처 좌표를 생성하고,
	CreateMeshTex();

	// 삼각형 정보를 생성하고, 
	CreateMeshFace();

	// 정점의 법선을 계산하고,
	UpdateNormal(false);

	// 곡면의 바운딩 박스를 계산한다.
	UpdateBndBox(false);

	// 정점 버퍼와 인덱스 버퍼를 갱신한다.
	UpdateBufVertex();
	UpdateBufNormal();
	UpdateBufFace();
}

/*!
*	\brief 정점의 좌표를 생성한다.
*/
void GTriBzrSrf::CreateMeshVert()
{
	// 도메인 정보를 구하고,
	double umin = 0.0, umax = 1.0, vmin = 0.0, vmax = 1.0;
	GetDomain(&umin, &umax, &vmin, &vmax);
	
	// 샘플링 정보를 얻어와서, 
	int snum = Renderer.NumSampU;

	// 기존의 정점을 지우고, 
	Verts.clear();
	
	// 삼각형 베지에 곡면의 정점 생성
	GVector4 pt;
	for (int i = 0; i < snum; ++i)
	{
		double u = umin + (umax - umin) * (double)i / (double)(snum - 1);
		for (int j = 0; j < snum - i; ++j)
		{
			double v = vmin + (vmax - vmin) * (double)j / (double)(snum - 1);
			double w = 1.0 - u - v;
			pt = Eval(u, v, w);

			GVertex vtx(GPoint3(pt[0], pt[1], pt[2]), GVector3(0.0, 0.0, 0.0));
			AddVertex(vtx);
		}
	}
}

/*!
*	\brief 텍스처 좌표를 생성한다.
*/ 
void GTriBzrSrf::CreateMeshTex()
{
	// 샘플링 정보를 얻어와서, 
	int snum = Renderer.NumSampU;

	// 삼각형 베지에 곡면의 텍스처 좌표를 생성한다.
	for (int i = 0; i < snum; ++i)
	{
		double s = (double)i / (double)(snum - 1);
		for (int j = 0; j < snum - i; ++j)
		{
			double t = (double)j / (double)(snum - 1);
			AddTexel(GVector2(s, t));
		}
	}
}


/*!
*	\brief 삼각형 정보를 생성한다.
*/ 
void GTriBzrSrf::CreateMeshFace()
{
	// 삼각형 정보를 지우고,
	Faces.clear();

	// 샘플링 정보를 얻어와서, 
	int snum = Renderer.NumSampU;

	// 삼각형 정보를 생성한다.
	int vidx0 = 0;
	for (int i = 0; i < snum - 1; ++i)
	{
		for(int j = 0; j < snum - 1 - i; ++j)
		{
			int vidx1 = vidx0 + 1;
			int vidx2 = vidx1 + snum - i;
			int vidx3 = vidx2 - 1;

			GFace f0(vidx0, vidx3, vidx1, vidx0, vidx3, vidx1, -1);
			AddFace(f0);

			if (j != snum - 2 - i)
			{
				GFace f1(vidx1, vidx3, vidx2, vidx1, vidx3, vidx2, 0);
				AddFace(f1);
			}
			vidx0++;
		}
		vidx0++;
	}
}

/*!
*	\brief 베지에 곡면위의 점을 계산한다.
*
*	\param u 파라미터
*
*	\return 베지에 곡면위의 점 S(u, v, w)을 반환한다.
*/
GVector4 GTriBzrSrf::Eval(double u, ...)
{
	double v, w;
	va_list ap;
	va_start(ap, u);
	v = va_arg(ap, double);
	va_end(ap);
	w = 1.0 - u - v;

	static GCtrlPt tmp[66];
	ARR_COPY(tmp, P, n + 1);

	switch (p)
	{
	case 1:	// linear
		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		break;

	case 2:	// quadratic
		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
		tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];

		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		break;

	case 3:	// cubic
		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
		tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
		tmp[3] = u * tmp[3] + v * tmp[6] + w * tmp[7];
		tmp[4] = u * tmp[4] + v * tmp[7] + w * tmp[8];
		tmp[5] = u * tmp[5] + v * tmp[8] + w * tmp[9];

		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
		tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];

		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		break;

	case 4:	// quartic
		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
		tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
		tmp[3] = u * tmp[3] + v * tmp[6] + w * tmp[7];
		tmp[4] = u * tmp[4] + v * tmp[7] + w * tmp[8];
		tmp[5] = u * tmp[5] + v * tmp[8] + w * tmp[9];
		tmp[6] = u * tmp[6] + v * tmp[10] + w * tmp[11];
		tmp[7] = u * tmp[7] + v * tmp[11] + w * tmp[12];
		tmp[8] = u * tmp[8] + v * tmp[12] + w * tmp[13];
		tmp[9] = u * tmp[9] + v * tmp[13] + w * tmp[14];

		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
		tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
		tmp[3] = u * tmp[3] + v * tmp[6] + w * tmp[7];
		tmp[4] = u * tmp[4] + v * tmp[7] + w * tmp[8];
		tmp[5] = u * tmp[5] + v * tmp[8] + w * tmp[9];

		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
		tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];

		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		break;

	case 5:	// quintic
		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
		tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
		tmp[3] = u * tmp[3] + v * tmp[6] + w * tmp[7];
		tmp[4] = u * tmp[4] + v * tmp[7] + w * tmp[8];
		tmp[5] = u * tmp[5] + v * tmp[8] + w * tmp[9];
		tmp[6] = u * tmp[6] + v * tmp[10] + w * tmp[11];
		tmp[7] = u * tmp[7] + v * tmp[11] + w * tmp[12];
		tmp[8] = u * tmp[8] + v * tmp[12] + w * tmp[13];
		tmp[9] = u * tmp[9] + v * tmp[13] + w * tmp[14];
		tmp[10] = u * tmp[10] + v * tmp[15] + w * tmp[16];
		tmp[11] = u * tmp[11] + v * tmp[16] + w * tmp[17];
		tmp[12] = u * tmp[12] + v * tmp[17] + w * tmp[18];
		tmp[13] = u * tmp[13] + v * tmp[18] + w * tmp[19];
		tmp[14] = u * tmp[14] + v * tmp[19] + w * tmp[20];

		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
		tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
		tmp[3] = u * tmp[3] + v * tmp[6] + w * tmp[7];
		tmp[4] = u * tmp[4] + v * tmp[7] + w * tmp[8];
		tmp[5] = u * tmp[5] + v * tmp[8] + w * tmp[9];
		tmp[6] = u * tmp[6] + v * tmp[10] + w * tmp[11];
		tmp[7] = u * tmp[7] + v * tmp[11] + w * tmp[12];
		tmp[8] = u * tmp[8] + v * tmp[12] + w * tmp[13];
		tmp[9] = u * tmp[9] + v * tmp[13] + w * tmp[14];

		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
		tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
		tmp[3] = u * tmp[3] + v * tmp[6] + w * tmp[7];
		tmp[4] = u * tmp[4] + v * tmp[7] + w * tmp[8];
		tmp[5] = u * tmp[5] + v * tmp[8] + w * tmp[9];

		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
		tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];

		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		break;

	default:	// sextic septic octic ......
		for (int r = 1; r < p + 1; ++r)
		{
			int m = -1;
			for (int i = 0; i < p - r + 1; ++i)
			{
				for (int j = 0; j < i + 1; ++j)
				{
					m++;
					tmp[m] = u * tmp[m] + v * tmp[m + i + 1] + w * tmp[m + i + 2];
				}
			}
		}
		break;
	}

	return tmp[0];
}

/*!
*	\brief 베지에 곡면위의 법선벡터를 계산한다.
*
*	\param (u, v, w) 파라미터
*
*	\return 베지에 곡면위의 법선 벡터 N(u, v, w)을 반환한다.
*/
GVector3 GTriBzrSrf::EvalNormal(double u, double v, double w)
{
	GVector4 Du, Dv;

	static GCtrlPt tmp[66];
	
	if (p == 3)
	{
		// 제어점 복사
		ARR_COPY(tmp, P, n + 1);

		// U-방향 차이 벡터 구하고,
		tmp[0] = tmp[0] - tmp[2];
		tmp[2] = tmp[2] - tmp[5];
		tmp[5] = tmp[5] - tmp[9];
		
		tmp[1] = tmp[1] - tmp[4];
		tmp[4] = tmp[4] - tmp[8];
		
		tmp[3] = tmp[3] - tmp[7];

		// 첫번째 반복
		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
		tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];

		// 두번째 반복
		Du = u * tmp[0] + v * tmp[1] + w * tmp[2];

		// 제어점 복사
		ARR_COPY(tmp, P, n + 1);

		// V-방향 차이 벡터 구하고,
		tmp[0] = tmp[1] - tmp[2];
		tmp[1] = tmp[3] - tmp[4];
		tmp[2] = tmp[4] - tmp[5];

		tmp[3] = tmp[6] - tmp[7];
		tmp[4] = tmp[7] - tmp[8];

		tmp[5] = tmp[8] - tmp[9];

		// 첫번째 반복
		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
		tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];

		// 두번째 반복
		Dv = u * tmp[0] + v * tmp[1] + w * tmp[2];
	}

	GVector3 ret = cast_vec3(Du) ^ cast_vec3(Dv);
	ret.Normalize();

	return ret;
}

/*!
*	\brief 베지에 곡면의 파라미터 영역 [umin, umax] x [vmin, vmax]을 구한다.
*
*	\param umin U-방향 파라미터의 시작 값이 저장됨
*/
void GTriBzrSrf::GetDomain(double *umin, ...)
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
void GTriBzrSrf::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
	for (int i = 0; i < n + 1; ++i)
		CtrlPtList.push_back(&P[i]);
}

/*!
*	\brief 베지에 곡면을 세 부분으로 분할한다.
*
*	\param u 파라미터
*	\param v 파라미터
*	\param w 파라미터
*
*	\return 곡면의 세 꼭지점과 점 S(u, v, w)를 기준으로 분할된 세 곡면을 반환한다.
*/
GTriBzrSrf *GTriBzrSrf::SubPatch(double u, double v, double w, int idx)
{
	// linear quadratic cubic quartic quintic sextic septic octic 
	static GCtrlPt tmp[66];
	ARR_COPY(tmp, P, n + 1);

	GTriBzrSrf *pSrf = new GTriBzrSrf("", GTransf(), p, NULL);

	// Linear triangular Bezier patch
	if (p == 1)
	{
		switch (idx)
		{
		case 0:
			pSrf->P[1] = tmp[1];
			pSrf->P[2] = tmp[2];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			pSrf->P[0] = tmp[0];
			break;

		case 1:
			pSrf->P[1] = tmp[2];
			pSrf->P[2] = tmp[0];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			pSrf->P[0] = tmp[0];
			break;

		case 2:
			pSrf->P[1] = tmp[0];
			pSrf->P[2] = tmp[1];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			pSrf->P[0] = tmp[0];
			break;
		}
	}

	// Quadratic triangular Bezier patch
	if (p == 2)
	{
		switch (idx)
		{
		case 0:
			pSrf->P[3] = tmp[3];
			pSrf->P[4] = tmp[4];
			pSrf->P[5] = tmp[5];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			pSrf->P[1] = tmp[1];
			pSrf->P[2] = tmp[2];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			pSrf->P[0] = tmp[0];
			break;

		case 1:
			pSrf->P[3] = tmp[5];
			pSrf->P[4] = tmp[2];
			pSrf->P[5] = tmp[0];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			pSrf->P[1] = tmp[2];
			pSrf->P[2] = tmp[0];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			pSrf->P[0] = tmp[0];
			break;

		case 2:
			pSrf->P[3] = tmp[0];
			pSrf->P[4] = tmp[1];
			pSrf->P[5] = tmp[3];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			pSrf->P[1] = tmp[0];
			pSrf->P[2] = tmp[1];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			pSrf->P[0] = tmp[0];
			break;
		}
	}

	// Cubic triangular Bezier patch
	if (p == 3)
	{
		switch (idx)
		{
		case 0:
			pSrf->P[6] = tmp[6];
			pSrf->P[7] = tmp[7];
			pSrf->P[8] = tmp[8];
			pSrf->P[9] = tmp[9];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			tmp[3] = u * tmp[3] + v * tmp[6] + w * tmp[7];
			tmp[4] = u * tmp[4] + v * tmp[7] + w * tmp[8];
			tmp[5] = u * tmp[5] + v * tmp[8] + w * tmp[9];
			pSrf->P[3] = tmp[3];
			pSrf->P[4] = tmp[4];
			pSrf->P[5] = tmp[5];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			pSrf->P[1] = tmp[1];
			pSrf->P[2] = tmp[2];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			pSrf->P[0] = tmp[0];
			break;

		case 1:
			pSrf->P[6] = tmp[9];
			pSrf->P[7] = tmp[5];
			pSrf->P[8] = tmp[2];
			pSrf->P[9] = tmp[0];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			tmp[3] = u * tmp[3] + v * tmp[6] + w * tmp[7];
			tmp[4] = u * tmp[4] + v * tmp[7] + w * tmp[8];
			tmp[5] = u * tmp[5] + v * tmp[8] + w * tmp[9];
			pSrf->P[3] = tmp[5];
			pSrf->P[4] = tmp[2];
			pSrf->P[5] = tmp[0];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			pSrf->P[1] = tmp[2];
			pSrf->P[2] = tmp[0];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			pSrf->P[0] = tmp[0];
			break;

		case 2:
			pSrf->P[6] = tmp[0];
			pSrf->P[7] = tmp[1];
			pSrf->P[8] = tmp[3];
			pSrf->P[9] = tmp[6];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			tmp[3] = u * tmp[3] + v * tmp[6] + w * tmp[7];
			tmp[4] = u * tmp[4] + v * tmp[7] + w * tmp[8];
			tmp[5] = u * tmp[5] + v * tmp[8] + w * tmp[9];
			pSrf->P[3] = tmp[0];
			pSrf->P[4] = tmp[1];
			pSrf->P[5] = tmp[3];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			pSrf->P[1] = tmp[0];
			pSrf->P[2] = tmp[1];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			pSrf->P[0] = tmp[0];
			break;
		}
	}

	// Quartic triangular Bezier patch
	if (p == 4)
	{
		switch (idx)
		{
		case 0:
			pSrf->P[10] = tmp[10];
			pSrf->P[11] = tmp[11];
			pSrf->P[12] = tmp[12];
			pSrf->P[13] = tmp[13];
			pSrf->P[14] = tmp[14];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			tmp[3] = u * tmp[3] + v * tmp[6] + w * tmp[7];
			tmp[4] = u * tmp[4] + v * tmp[7] + w * tmp[8];
			tmp[5] = u * tmp[5] + v * tmp[8] + w * tmp[9];
			tmp[6] = u * tmp[6] + v * tmp[10] + w * tmp[11];
			tmp[7] = u * tmp[7] + v * tmp[11] + w * tmp[12];
			tmp[8] = u * tmp[8] + v * tmp[12] + w * tmp[13];
			tmp[9] = u * tmp[9] + v * tmp[13] + w * tmp[14];
			pSrf->P[6] = tmp[6];
			pSrf->P[7] = tmp[7];
			pSrf->P[8] = tmp[8];
			pSrf->P[9] = tmp[9];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			tmp[3] = u * tmp[3] + v * tmp[6] + w * tmp[7];
			tmp[4] = u * tmp[4] + v * tmp[7] + w * tmp[8];
			tmp[5] = u * tmp[5] + v * tmp[8] + w * tmp[9];
			pSrf->P[3] = tmp[3];
			pSrf->P[4] = tmp[4];
			pSrf->P[5] = tmp[5];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			pSrf->P[1] = tmp[1];
			pSrf->P[2] = tmp[2];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			pSrf->P[0] = tmp[0];
			break;

		case 1:
			pSrf->P[10] = tmp[14];
			pSrf->P[11] = tmp[9];
			pSrf->P[12] = tmp[5];
			pSrf->P[13] = tmp[2];
			pSrf->P[14] = tmp[0];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			tmp[3] = u * tmp[3] + v * tmp[6] + w * tmp[7];
			tmp[4] = u * tmp[4] + v * tmp[7] + w * tmp[8];
			tmp[5] = u * tmp[5] + v * tmp[8] + w * tmp[9];
			tmp[6] = u * tmp[6] + v * tmp[10] + w * tmp[11];
			tmp[7] = u * tmp[7] + v * tmp[11] + w * tmp[12];
			tmp[8] = u * tmp[8] + v * tmp[12] + w * tmp[13];
			tmp[9] = u * tmp[9] + v * tmp[13] + w * tmp[14];
			pSrf->P[6] = tmp[9];
			pSrf->P[7] = tmp[5];
			pSrf->P[8] = tmp[2];
			pSrf->P[9] = tmp[0];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			tmp[3] = u * tmp[3] + v * tmp[6] + w * tmp[7];
			tmp[4] = u * tmp[4] + v * tmp[7] + w * tmp[8];
			tmp[5] = u * tmp[5] + v * tmp[8] + w * tmp[9];
			pSrf->P[3] = tmp[5];
			pSrf->P[4] = tmp[2];
			pSrf->P[5] = tmp[0];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			pSrf->P[1] = tmp[2];
			pSrf->P[2] = tmp[0];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			pSrf->P[0] = tmp[0];
			break;

		case 2:
			pSrf->P[10] = tmp[0];
			pSrf->P[11] = tmp[1];
			pSrf->P[12] = tmp[3];
			pSrf->P[13] = tmp[6];
			pSrf->P[14] = tmp[10];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			tmp[3] = u * tmp[3] + v * tmp[6] + w * tmp[7];
			tmp[4] = u * tmp[4] + v * tmp[7] + w * tmp[8];
			tmp[5] = u * tmp[5] + v * tmp[8] + w * tmp[9];
			tmp[6] = u * tmp[6] + v * tmp[10] + w * tmp[11];
			tmp[7] = u * tmp[7] + v * tmp[11] + w * tmp[12];
			tmp[8] = u * tmp[8] + v * tmp[12] + w * tmp[13];
			tmp[9] = u * tmp[9] + v * tmp[13] + w * tmp[14];
			pSrf->P[6] = tmp[0];
			pSrf->P[7] = tmp[1];
			pSrf->P[8] = tmp[3];
			pSrf->P[9] = tmp[6];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			tmp[3] = u * tmp[3] + v * tmp[6] + w * tmp[7];
			tmp[4] = u * tmp[4] + v * tmp[7] + w * tmp[8];
			tmp[5] = u * tmp[5] + v * tmp[8] + w * tmp[9];
			pSrf->P[3] = tmp[0];
			pSrf->P[4] = tmp[1];
			pSrf->P[5] = tmp[3];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
			tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];
			pSrf->P[1] = tmp[0];
			pSrf->P[2] = tmp[1];

			tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
			pSrf->P[0] = tmp[0];
			break;
		}
	}

	return pSrf;
}

