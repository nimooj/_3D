#include "stdafx.h"

/*************************************/
/* Implementation of GNurbsCrv Class */
/*************************************/
IMPLEMENT_GMESH_OBJECT(GBlendCrv)

/*!
*	\brief 객체가 포함된 메쉬의 포인터를 반환한다.
*
*	\return 객체가 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GBlendCrv::GetRefMesh()
{
	return this;
}

/*!
*	\brief 객체의 모델링 좌표계를 반환한다.
*
*	\return 월드 좌표계에서 객체의 모델링 좌표계를 반환한다
*/
GTransf GBlendCrv::GetTransf()
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
void GBlendCrv::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);
}

/*!
*	\brief	메쉬의 모델링 좌표계를 월드 좌표계로 갱신한다.
*
*	\param RefType 모델링 좌표계의 갱신을 위한 참조 형태(0: 월드 좌표계, 1: 경계상자 좌표계)
*/
void GBlendCrv::UpdateMC(int RefType)
{
}

/*!
*	\brief	곡선의 정점의 위치를 갱신한다.
*/
void GBlendCrv::UpdateMesh()
{
	switch (ModifiedFlag)
	{
	case MODIFIED_VERT_POS:
		break;

	case MODIFIED_VERT_NUM:
		break;

		// 재질이 변경되었다면,
	case MODIFIED_MTL:
		UpdateBufFace();
		break;

	case MODIFIED_INTPT_POS:
		CreateMeshVert();
		UpdateBndBox(false);
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
void GBlendCrv::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);
}

/*!
*	\brief	기본 생성자
*/
GBlendCrv::GBlendCrv()
: GMesh("noname", GTransf(), FACE_TRI, "")
{
	n = 0;
	P = new GVector4 [n + 1];
	T = new GVector4 [n + 1];
	P[0].Set(0.0, 0.0, 0.0, 0.0);
	T[0].Set(0.0, 0.0, 0.0, 0.0);
	Param = new double [n + 1];
	Param[0] = 0.0;
}

/*!
*	\brief	생성자
*
*	\param _Name 곡선의 이름
*	\param _MC 객체의 모델링 좌표계
*	\param _P 곡선의 보간점
*	\param _n 곡선의 보간점의 마지막 인덱스
*	\param _ParentName 곡선의 부모 메쉬의 이름
*/
GBlendCrv::GBlendCrv(std::string _Name, GTransf _MC, GVector4 *_P, int _n, std::string _ParentName)
: GMesh(_Name, _MC, FACE_TRI, _ParentName)
{
	n = _n;
	P = new GVector4 [n + 1];
	ARR_COPY(P, _P, n + 1);

	T = new GVector4 [n + 1];
	ARR_ZERO(T, n + 1);
	Param = ::get_param_chord_len(n, P, 0.0, 1.0);

	for (int i = 1; i < n; ++i)
	{
		int i0 = i - 1;
		int i1 = i;
		int i2 = i + 1;

		T[i] = P[i0] * (Param[i1] - Param[i2]) / (Param[i0] - Param[i1]) / (Param[i0] - Param[i2]) +
			P[i1] * (2 * Param[i1] - Param[i0] - Param[i2]) / (Param[i1] - Param[i0]) / (Param[i1] - Param[i2]) +
			P[i2] * (Param[i1] - Param[i0]) / (Param[i2] - Param[i0]) / (Param[i2] - Param[i1]);
	}
}

/*!
*	\brief	복사 생성자
*
*	\param cpy 복사될 객체
*/
GBlendCrv::GBlendCrv(const GBlendCrv &cpy)
: GMesh(cpy)
{
	n = cpy.n;
	P = new GVector4 [n + 1];
	T = new GVector4 [n + 1];
	Param = new double [n + 1];

	ARR_COPY(P, cpy.P, n + 1);
	ARR_COPY(T, cpy.T, n + 1);
	ARR_COPY(Param, cpy.Param, n + 1);
}

/*!
*	\brief	소멸자
*/
GBlendCrv::~GBlendCrv()
{
	if (P != NULL)
		ARR_FREE(P);

	if (T != NULL)
		ARR_FREE(T);

	if (Param != NULL)
		ARR_FREE(Param);
}

/*!
*	\brief	대입 연산자
*
*	\param rhs 대입될 객체
*
*	\return 대입된 자신을 반환한다.
*/
GBlendCrv &GBlendCrv::operator =(const GBlendCrv &rhs)
{
	GMesh::operator =(rhs);

	if (P != NULL)
		ARR_FREE(P);

	if (T != NULL)
		ARR_FREE(T);

	if (Param != NULL)
		ARR_FREE(Param);

	n = rhs.n;
	P = new GVector4 [n + 1];
	T = new GVector4 [n + 1];
	Param = new double [n + 1];

	ARR_COPY(P, rhs.P, n + 1);
	ARR_COPY(T, rhs.T, n + 1);
	ARR_COPY(Param, rhs.Param, n + 1);

	return *this;
}

/*!
*	\brief	실제 곡선이 정의된 도메인을 반환한다.
*
*	\param umin 도메인의 시작 값이 저장된다.
*	\param umax 도메인의 끝 값이 저장된다.
*/
void GBlendCrv::GetDomain(double *umin, ...)
{
	double *umax;
	va_list ap;
	va_start(ap, umin);
	umax = va_arg(ap, double *);
	va_end(ap);

	*umin = Param[0];
	*umax = Param[n];
}

/*!
*	\brief	곡선의 제어점 리스트를 반환한다.
*
*	\return 곡선의 제어점 리스트를 반환한다.
*/
void GBlendCrv::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
}

/*!
*	\brief	블렌딩 곡선의 렌더링을 위한 메쉬(정점의 리스트)를 생성한다.
*/
void GBlendCrv::CreateMesh()
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
	UpdateBufFace();
}

/*!
*	\brief	곡선의 렌더링을 위한 정점의 리스트를 생성한다.
*/
void GBlendCrv::CreateMeshVert()
{
	// 기존의 정점을 지우고, 
	Verts.clear();

	// 샘플링 정보를 얻어와서, 
	int NumSampU = Renderer.NumSampU;

	// 정의역을 계산하고,
	double umin = 0.0, umax = 1.0;
	GetDomain(&umin, &umax);

	// 곡면위의 정점을 계산하여, 
	for (int i = 0; i < NumSampU; ++i)
	{
		double u = umin + (umax - umin) * i / (NumSampU - 1);
		GVector4 pt = Eval(u);
		// 메쉬의 정점 리스트에 추가한다.
		AddVertex(GVertex(GPoint3(pt[0], pt[1], pt[2]), GVector3()));
	}
}

/*!
*	\brief	곡선의 렌더링을 위한 텍스처 리스트를 생성한다.
*/
void GBlendCrv::CreateMeshTex()
{
	// 기존의 텍셀 정보를 지우고,
	Texs.clear();

	// 샘플링 정보를 얻어와서, 
	int NumSampU = Renderer.NumSampU;

	// 텍스처 좌표를 생성하고 저장함
	for (int i = 0; i < NumSampU; ++i)
	{
		double s = (double)i / (double)(NumSampU - 1);
		AddTexel(GVector2(s, 0.0));
	}
}

/*!
*	\brief	곡선의 렌더링을 위한 삼각형 리스트를 생성한다.
*/
void GBlendCrv::CreateMeshFace()
{
	// 삼각형 정보를 지우고,
	Faces.clear();

	// FAN 타입 메쉬에 대해서만, 
	if (FaceType == FACE_TRI_FAN)
	{
		int vnum = (int)Verts.size();
		for (int i = 1; i < vnum - 1; ++i)
		{
			// 삼각형을 생성하고, 메쉬의 삼각형 리스트에 추가한다.
			GFace f0(0, i, i + 1);
			AddFace(f0);
		}
	}
}

/*!
*	\brief	블렌딩 곡선을 렌더링 한다.
*
*	\param bSubMesh 서브 메쉬의 렌더링 여부(선택 모드에서는 false로 설정)
*/
void GBlendCrv::Render(bool bSubMesh)
{
	// 부모 클래스 함수를 호출한 후,
	GMesh::Render(bSubMesh);

	glPushMatrix();
	{
		// 월드 좌표계에서 객체의 모델링 좌표계로의 변환하고,
		glMultMatrixd(cast_mat(MC).M);

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


		// 보간점을 렌더링 할 경우,
		if (Renderer.RenderType & RENDER_CTRL_PT)
		{
			// 재질을 설정하고,
			GMaterial Mtl("mtl_gray");
			Mtl.SetAmbient(0.7f, 0.7f, 0.7f);
			Mtl.SetDiffuse(0.7f, 0.7f, 0.7f);
			Mtl.SetSpecular(0.7f, 0.7f, 0.7f);
			Mtl.SetAlpha(1.0);
			Mtl.Enable(false);

			// 보간점을 렌더링하고,
			double size = Renderer.CtrlPtSize;
			for (int i = 0; i < n + 1; ++i)
			{
				glPushMatrix();
				glTranslated(P[i][0], P[i][1], P[i][2]);
				glutSolidSphere(size, 10, 10);
				glPopMatrix();
			}
		}
	}
	glPopMatrix();
}

/*!
*	\brief	곡선위의 점을 계산
*
*	\param u 매개변수의 값
*
*	\return 곡선위의 점 C(u)를 계산
*/
GVector4 GBlendCrv::Eval(double u, ...)
{
	int idx = -1;
	for (int i = 0; i < n + 1; ++i)
	{
		if (u >= Param[i] && u < Param[i + 1])
		{
			idx = i;
			break;
		}
	}

	if (u == Param[n])
		idx = n - 1;

	double t, h1, h2;
	GVector4 p0, p1;

	if (idx == 0)
	{
		p0.Set(0.0, 0.0, 0.0, 0.0);
		p1 = P[idx] * ((u - Param[idx + 1]) * (u - Param[idx + 2])) / ((Param[idx] - Param[idx + 1]) * (Param[idx] - Param[idx + 2])) + 
			P[idx + 1] * ((u - Param[idx]) * (u - Param[idx + 2])) / ((Param[idx + 1] - Param[idx]) * (Param[idx + 1] - Param[idx + 2])) +
			P[idx + 2] * ((u - Param[idx]) * (u - Param[idx + 1])) / ((Param[idx + 2] - Param[idx]) * (Param[idx + 2] - Param[idx + 1]));

		// 		int i0, i1, i2;
		// 		double a, b, w0, w1, w2, w3;
		// 
		// 		// p0
		// 		p0.Set(0.0, 0.0, 0.0, 0.0);
		// 
		// 		// p1
		// 		i0 = idx;
		// 		i1 = idx + 1;
		// 		i2 = idx + 2;
		// 		a = Param[i0] + Param[i2] - 2 * Param[i1];
		// 		b = 3 * SQR(Param[i1]) - 2 * Param[i0] * Param[i1] - 2 * Param[i1] * Param[i2] + Param[i0] * Param[i2];
		// 		w0 = SQR(u - Param[i1]) * (u - Param[i2]) / SQR(Param[i0] - Param[i1]) / (Param[i0] - Param[i2]);
		// 		w1 = (a * u + b) * (u - Param[i0]) * (u - Param[i2]) / SQR(Param[i1] - Param[i0]) / SQR(Param[i1] - Param[i2]);
		// 		w2 = (u - Param[i0]) * SQR(u - Param[i1]) / (Param[i2] - Param[i0]) / SQR(Param[i2] - Param[i1]);
		// 		w3 = (u - Param[i0]) * (u - Param[i1]) * (u - Param[i2]) / (Param[i1] - Param[i0]) / (Param[i1] - Param[i2]);
		// 		p1 = w0 * P[i0] + w1 * P[i1] + w2 * P[i2] + w3 * T[i1];

		h1 = 0.0;
		h2 = 1.0;
	}
	else if (idx == n - 1)
	{
		p0 = P[idx - 1] * ((u - Param[idx]) * (u - Param[idx + 1])) / ((Param[idx - 1] - Param[idx]) * (Param[idx - 1] - Param[idx + 1])) + 
			P[idx] * ((u - Param[idx - 1]) * (u - Param[idx + 1])) / ((Param[idx] - Param[idx - 1]) * (Param[idx] - Param[idx + 1])) +
			P[idx + 1] * ((u - Param[idx - 1]) * (u - Param[idx])) / ((Param[idx + 1] - Param[idx - 1]) * (Param[idx + 1] - Param[idx]));

		p1 = 0.0;

		// 		int i0, i1, i2;
		// 		double a, b, w0, w1, w2, w3;
		// 
		// 		// p0
		// 		i0 = idx - 1;
		// 		i1 = idx;
		// 		i2 = idx + 1;
		// 		a = Param[i0] + Param[i2] - 2 * Param[i1];
		// 		b = 3 * SQR(Param[i1]) - 2 * Param[i0] * Param[i1] - 2 * Param[i1] * Param[i2] + Param[i0] * Param[i2];
		// 		w0 = SQR(u - Param[i1]) * (u - Param[i2]) / SQR(Param[i0] - Param[i1]) / (Param[i0] - Param[i2]);
		// 		w1 = (a * u + b) * (u - Param[i0]) * (u - Param[i2]) / SQR(Param[i1] - Param[i0]) / SQR(Param[i1] - Param[i2]);
		// 		w2 = (u - Param[i0]) * SQR(u - Param[i1]) / (Param[i2] - Param[i0]) / SQR(Param[i2] - Param[i1]);
		// 		w3 = (u - Param[i0]) * (u - Param[i1]) * (u - Param[i2]) / (Param[i1] - Param[i0]) / (Param[i1] - Param[i2]);
		// 		p0 = w0 * P[i0] + w1 * P[i1] + w2 * P[i2] + w3 * T[i1];
		// 
		// 		// p1
		// 		p1.Set(0.0, 0.0, 0.0, 0.0);

		h1 = 1.0;
		h2 = 0.0;
	}
	else
	{
		p0 = P[idx - 1] * ((u - Param[idx]) * (u - Param[idx + 1])) / ((Param[idx - 1] - Param[idx]) * (Param[idx - 1] - Param[idx + 1])) + 
			P[idx] * ((u - Param[idx - 1]) * (u - Param[idx + 1])) / ((Param[idx] - Param[idx - 1]) * (Param[idx] - Param[idx + 1])) +
			P[idx + 1] * ((u - Param[idx - 1]) * (u - Param[idx])) / ((Param[idx + 1] - Param[idx - 1]) * (Param[idx + 1] - Param[idx]));


		p1 = P[idx] * ((u - Param[idx + 1]) * (u - Param[idx + 2])) / ((Param[idx] - Param[idx + 1]) * (Param[idx] - Param[idx + 2])) + 
			P[idx + 1] * ((u - Param[idx]) * (u - Param[idx + 2])) / ((Param[idx + 1] - Param[idx]) * (Param[idx + 1] - Param[idx + 2])) +
			P[idx + 2] * ((u - Param[idx]) * (u - Param[idx + 1])) / ((Param[idx + 2] - Param[idx]) * (Param[idx + 2] - Param[idx + 1]));

		// 		int i0, i1, i2;
		// 		double a, b, w0, w1, w2, w3;
		// 
		// 		// p0
		// 		i0 = idx - 1;
		// 		i1 = idx;
		// 		i2 = idx + 1;
		// 		a = Param[i0] + Param[i2] - 2 * Param[i1];
		// 		b = 3 * SQR(Param[i1]) - 2 * Param[i0] * Param[i1] - 2 * Param[i1] * Param[i2] + Param[i0] * Param[i2];
		// 		w0 = SQR(u - Param[i1]) * (u - Param[i2]) / SQR(Param[i0] - Param[i1]) / (Param[i0] - Param[i2]);
		// 		w1 = (a * u + b) * (u - Param[i0]) * (u - Param[i2]) / SQR(Param[i1] - Param[i0]) / SQR(Param[i1] - Param[i2]);
		// 		w2 = (u - Param[i0]) * SQR(u - Param[i1]) / (Param[i2] - Param[i0]) / SQR(Param[i2] - Param[i1]);
		// 		w3 = (u - Param[i0]) * (u - Param[i1]) * (u - Param[i2]) / (Param[i1] - Param[i0]) / (Param[i1] - Param[i2]);
		// 		p0 = w0 * P[i0] + w1 * P[i1] + w2 * P[i2] + w3 * T[i1];
		// 		
		// 		// p1
		// 		i0 = idx;
		// 		i1 = idx + 1;
		// 		i2 = idx + 2;
		// 		a = Param[i0] + Param[i2] - 2 * Param[i1];
		// 		b = 3 * SQR(Param[i1]) - 2 * Param[i0] * Param[i1] - 2 * Param[i1] * Param[i2] + Param[i0] * Param[i2];
		// 		w0 = SQR(u - Param[i1]) * (u - Param[i2]) / SQR(Param[i0] - Param[i1]) / (Param[i0] - Param[i2]);
		// 		w1 = (a * u + b) * (u - Param[i0]) * (u - Param[i2]) / SQR(Param[i1] - Param[i0]) / SQR(Param[i1] - Param[i2]);
		// 		w2 = (u - Param[i0]) * SQR(u - Param[i1]) / (Param[i2] - Param[i0]) / SQR(Param[i2] - Param[i1]);
		// 		w3 = (u - Param[i0]) * (u - Param[i1]) * (u - Param[i2]) / (Param[i1] - Param[i0]) / (Param[i1] - Param[i2]);
		// 		p1 = w0 * P[i0] + w1 * P[i1] + w2 * P[i2] + w3 * T[i1];

		t = (u - Param[idx]) / (Param[idx + 1] - Param[idx]);
		h1 = 2 * t * t * t - 3 * t * t + 1.0;
		h2 = 2 * (1.0 - t) * (1.0 - t) * (1.0 - t) - 3 * (1.0 - t) * (1.0 - t) + 1.0;

		h1 = 1.0 - t;
		h2 = t;

		// 		t = t * 3.141592;
		// 		h1 = cos(t) * 0.5 + 0.5;
		// 		h2 = sin(t - 3.141592 * 0.5) * 0.5 + 0.5;
	}	

	GVector4 ret = h1 * p0 + h2 * p1;

	return ret;
}

/*!
*	\brief	곡선의 보간점을 편집한다.
*
*	\param idx 편집될 보간점의 인덱스
*	\param dp 편집될 보간점의 편집 변위
*/
void GBlendCrv::Edit(int idx, GVector4 dp)
{
	P[idx] += dp;	
}
