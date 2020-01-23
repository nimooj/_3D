#include "stdafx.h"

// 외부 변수 참조
extern double CBS_BASIS[1001][4];
extern double CBS_BASIS_DERIV[1001][4];

/*************************************/
/* Implementation of GUcbsSrf1D Class */
/*************************************/
IMPLEMENT_GMESH_OBJECT(GUcbsSrf1D)

/*!
*	\brief 객체가 포함된 메쉬의 포인터를 반환한다.
*
*	\return 객체가 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GUcbsSrf1D::GetRefMesh()
{
	return this;
}

/*!
*	\brief 월드 좌표계에서 표현된 객체의 모델링 좌표계를 반환한다.
*
*	\return 월드 좌표계에서 표현된 객체의 모델링 좌표계를 반환한다.
*/
GTransf GUcbsSrf1D::GetTransf()
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
void GUcbsSrf1D::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);	
}

/*!
*	\brief 선택된 객체를 렌더링 한다.
*/
void GUcbsSrf1D::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);
}

/*!
*	\brief	넙스 곡면의 생성을 위한 제어점 배열을 동적으로 할당
*	
*	\param _m 곡면의 U-방향 제어점의 마지막 인덱스
*	\param _n 곡면의 V-방향 제어점의 마지막 인덱스
*/
void GUcbsSrf1D::_alloc(int _m, int _n)
{
	m = _m; 
	n = _n;
	ARR_ALLOC(P, double, (m + 1) * (n + 1));
}

/*!
*	\brief	넙스곡면의 제어점의 동적 배열을 해제
*/
void GUcbsSrf1D::_free()
{
	ARR_FREE(P);
}

/*!
*	\brief	생성자
*
*	\param _Name 곡면의 이름
*	\param _MC 곡면의 모델링 좌표계
*	\param _m 곡면의 U-방향 제어점의 마지막 인덱스
*	\param _n 곡면의 V-방향 제어점의 마지막 인덱스
*	\param _P 곡면의 제어점 배열
*	\param _ParentName 부모 메쉬의 이름
*/
GUcbsSrf1D::GUcbsSrf1D(std::string _Name, GTransf _MC, int _m, int _n, double *_P, std::string _ParentName)
: GMesh(_Name, _MC, FACE_QUAD, _ParentName)
{
	assert(_m >= 3);
	assert(_n >= 3);

	_alloc(_m, _n);
	if (_P == NULL)
		ARR_ZERO(P, (m + 1) * (n + 1));
	else
		ARR_COPY(P, _P, (m + 1) * (n + 1));
}

/*!
*	\brief	복사 생성자
*
*	\param cpy 복사될 객체
*/
GUcbsSrf1D::GUcbsSrf1D(const GUcbsSrf1D &cpy)
: GMesh(cpy)
{
	_alloc(cpy.m, cpy.n);
	ARR_COPY(P, cpy.P, (m + 1) * (n + 1));
}

/*!
*	\brief	소멸자
*/
GUcbsSrf1D::~GUcbsSrf1D()
{
	_free();
}

/*!
*	\brief	곡면의 대입연산자 중복
*
*	\param rhs 대입될 넙스곡면 객체
*
*	\return 대입된 곡면을 반환한다
*/
GUcbsSrf1D &GUcbsSrf1D::operator =(const GUcbsSrf1D &rhs)
{
	GMesh::operator =(rhs);

	_free();
	_alloc(rhs.m, rhs.n);
	ARR_COPY(P, rhs.P, (m + 1) * (n + 1));

	return *this;
}

/*!
*	\brief	곡면을 렌더링 한다.
*
*	\param bSubMesh 서브 메쉬의 렌더링 여부(선택 모드에서는 false로 설정)
*/
void GUcbsSrf1D::Render(bool bSubMesh)
{
	GMesh::Render(bSubMesh);
}

/*!
*	\brief	곡면을 생성한다.
*/
void GUcbsSrf1D::CreateMesh()
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
*	\brief	곡면의 렌더링을 위한 정점의 리스트를 생성한다.
*/
void GUcbsSrf1D::CreateMeshVert()
{
	// 샘플링 정보를 얻어와서, 
	int NumSampU = Renderer.NumSampU;
	int NumSampV = Renderer.NumSampV;
	double umin = 0.0, umax = 1.0, vmin = 0.0, vmax = 1.0;

	// 기존의 정점을 지우고, 
	Verts.clear();

	// 정의역을 계산하고,
	GetDomain(&umin, &umax, &vmin, &vmax);

	// 곡며위의 정점을 계산한다.
	GVector4 pt;
	//GMapDisp *pMap = pScene->FindMapDisp(Renderer.map_disp_name);
	for (int i = 0; i < NumSampU; ++i)
	{
		double u = umin + (umax - umin) * (double)i / (double)(NumSampU - 1);
		double s = (double)i / (double)(NumSampU - 1);
		for (int j = 0; j < NumSampV; ++j)
		{
			double v = vmin + (vmax - vmin) * (double)j / (double)(NumSampV - 1);
			pt = Eval(u, v);

			GVertex vtx(GPoint3(u, v, pt[0]), GVector3(0.0, 0.0, 0.0));
			AddVertex(vtx);
		}
	}
}

/*!
*	\brief	곡면의 렌더링을 위한 텍스처 좌표를 생성한다.
*/
void GUcbsSrf1D::CreateMeshTex()
{
	// 샘플링 정보를 얻어와서, 
	int NumSampU = Renderer.NumSampU;
	int NumSampV = Renderer.NumSampV;

	// 텍스처 좌표를 생성하고 저장함
	Texs.clear();
	for (int i = 0; i < NumSampU; ++i)
	{
		double s = (double)i / (double)(NumSampU - 1);
		for (int j = 0; j < NumSampV; ++j)
		{
			double t = (double)j / (double)(NumSampV - 1);
			AddTexel(GVector2(s, t));
		}
	}
}

/*!
*	\brief	넙스곡면의 렌더링을 위한 삼각형 리스트를 생성한다.
*/
void GUcbsSrf1D::CreateMeshFace()
{
	// 삼각형 정보를 지우고,
	Faces.clear();

	// 샘플링 정보를 얻어와서, 
	int NumSampU = Renderer.NumSampU;
	int NumSampV = Renderer.NumSampV;

	// 삼각형을 생성하고 추가한다.
	for (int i = 0; i < NumSampU - 1; ++i)
	{
		for(int j = 0; j < NumSampV - 1; ++j)
		{
			int vidx0 = i * NumSampV + j;
			int vidx1 = vidx0 + NumSampV;
			int vidx2 = vidx1 + 1;
			int vidx3 = vidx0 + 1;

			GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
			GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);

			AddFace(f0);
			AddFace(f1);
		}
	}
}

/*!
*	\brief	곡면위의 점 S(u, v)를 계산
*
*	\param u U-방향 매개변수의 값
*
*	\return 곡면위의 점 S(u, v)의 위치를 계산
*/
GVector4 GUcbsSrf1D::Eval(double u, ...)
{
	double v;
	va_list ap;
	va_start(ap, u);
	v = va_arg(ap, double);
	va_end(ap);

	// get local parameter values for u and v
	double uu = u * (m - 2);
	double vv = v * (n - 2);
	int k = (int)uu;	
	int l = (int)vv;	// P(k,l) is the left bottum control points.
	uu = uu - k;	
	vv = vv - l;	// get the local parameter of u and v.

	if(k == m - 2)	// special case for u is 1.0. 
	{
		k = m - 3;
		uu = 1.0;
	}
	if(l == n - 2)  // special case for v is 1.0.
	{
		l = n - 3;
		vv = 1.0;
	}

	static double basis_u[4], basis_v[4];

	get_ucbs_basis(uu, basis_u);
	get_ucbs_basis(vv, basis_v);

	double f = 0;
	for (int i = 0; i < 4; ++i)
	{
		int idx = (k + i) * (n + 1) + l;
		f += basis_u[i] * (P[idx] * basis_v[0] + P[idx + 1] * basis_v[1] + P[idx + 2] * basis_v[2] + P[idx + 3] * basis_v[3]);
	}

	return GVector4(f, 0.0, 0.0, 0.0);
}

/*!
*	\brief	곡면위의 점 S(u, v)를 계산
*
*	\param u U-방향 매개변수의 값
*
*	\return 곡면위의 점 S(u, v)의 위치를 계산
*/
GVector4 GUcbsSrf1D::EvalFast(double u, ...)
{
	double v;
	va_list ap;
	va_start(ap, u);
	v = va_arg(ap, double);
	va_end(ap);

	// get local parameter values for u and v
	double uu = u * (m - 2);
	double vv = v * (n - 2);
	int k = (int)uu;	
	int l = (int)vv;	// P(k,l) is the left bottum control points.
	uu = uu - k;	
	vv = vv - l;	// get the local parameter of u and v.

	if(k == m - 2)	// special case for u is 1.0. 
	{
		k = m - 3;
		uu = 1.0;
	}
	if(l == n - 2)  // special case for v is 1.0.
	{
		l = n - 3;
		vv = 1.0;
	}

	static double basis_u[4], basis_v[4];

	int uidx = (int)(uu * 1000.0 + 0.5);
	int vidx = (int)(vv * 1000.0 + 0.5);
	ARR_COPY(basis_u, CBS_BASIS[uidx], 4);
	ARR_COPY(basis_v, CBS_BASIS[vidx], 4);

	double f = 0;
	for (int i = 0; i < 4; ++i)
	{
		int idx = (k + i) * (n + 1) + l;
		f += basis_u[i] * (P[idx] * basis_v[0] + P[idx + 1] * basis_v[1] + P[idx + 2] * basis_v[2] + P[idx + 3] * basis_v[3]);
	}

	return GVector4(f, 0.0, 0.0, 0.0);
}

/*!
*	\brief 곡면의 도메인을 반환한다.
*
*	\param min U 방향 도메인의 시작 값이 저장된다.
*/
void GUcbsSrf1D::GetDomain(double *umin, ...)
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
*	\brief	볼륨의 제어점 리스트를 반환한다.
*
*	\param CtrlPtList 볼륨의 제어점 리스트가 저장된다.
*/
void GUcbsSrf1D::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
// 	for (int i = 0; i < (m + 1) * (n + 1); ++i)
// 		CtrlPtList.push_back(&P[i]);
}

/*!
*	\brief	곡면의 refinement 를 수행한다.
*/
void GUcbsSrf1D::Refine()
{
	int mm = 2 * (m - 1);
	int nn = 2 * (n - 1);

	int num_ctlpt = (mm + 1) * (nn + 1);
	double *P0 = P;
	double *P1 = new double [num_ctlpt];
	ARR_ZERO(P1, num_ctlpt);

	// compute new control points.
	for(int i = 0; i < mm + 1; i += 2)
	{
		for(int j = 0; j < nn + 1; j += 2)
		{
			int idx0 = (n + 1) * (i / 2) + (j / 2);		// P0's index.
			int idx1 = (nn + 1) * i + j;				// P1's index.

			P1[idx1] = (P0[idx0] + P0[idx0 + 1] + P0[idx0 + n + 1] + P0[idx0 + n + 2]) / 4.0;

			if (j != nn)
				P1[idx1 + 1] = (P0[idx0] + P0[idx0 + 2] + P0[idx0 + n + 1] + 
				P0[idx0 + n + 3] + 6.0 * (P0[idx0 + 1] + P0[idx0 + n + 2])) / 16.0;

			if (i != mm)
				P1[idx1 + nn + 1] = (P0[idx0] + P0[idx0 + 1] + P0[idx0 + 2 * (n + 1)] + 
				P0[idx0 + 2 * (n + 1) + 1] + 6.0 * (P0[idx0 + n + 1] + P0[idx0 + n + 2])) / 16.0;

			if (i != mm && j != nn)
				P1[idx1 + nn + 2] = (P0[idx0] + P0[idx0 + 2] + P0[idx0 + 2 * (n + 1)] + 
				P0[idx0 + 2 * (n + 2)] + 6.0 * (P0[idx0 + 1] + P0[idx0 + n + 1] + P0[idx0 + n + 3] + 
				P0[idx0 + 2 * (n + 1) + 1]) + 36.0 * P0[idx0 + n + 2]) / 64.0;
		}
	}

	_free();
	m = mm;
	n = nn;
	P = P1;
}

/*!
*	\brief 함수 위의 한 점 S(u,v)가 주어진 df 만큼 변하도록 함수의 제어점들의 위치를 수정
*
*	\param u 곡면위의 점에 대한 U-방향 매개변수값
*	\param v 곡면위의 점에 대한 V-방향 매개변수값
*	\param df 함수값의 스칼라 변위
*/
void GUcbsSrf1D::Edit(double u, double v, double df)
{
	// get local parameter values for u and v
	double uu = u * (m - 2);
	double vv = v * (n - 2);
	int k = (int)uu;	
	int l = (int)vv;	// P(k,l) is the left bottum control points.

	uu = uu - k;	
	vv = vv - l;		// get the local parameter of u and v.
	if (k == m - 2)		// special case for u is 1.0. 
	{
		k = m - 3;
		uu = 1.0;
	}
	if (l == n - 2)		// special case for v is 1.0.
	{
		l = n - 3;
		vv = 1.0;
	}

	static double basis_u[4], basis_v[4], basis[4][4];
	get_ucbs_basis(uu, basis_u);
	get_ucbs_basis(vv, basis_v);

	double alpha = 0.0;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			basis[i][j] = basis_u[i] * basis_v[j];
			alpha += SQR(basis[i][j]);
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			int idx = (k + i) * (n + 1) + l + j;
			P[idx] += df * basis[i][j] / alpha;
		}
	}
}

/*!
*	\brief	메쉬의 모델링 좌표계를 월드 좌표계로 갱신한다.
*
*	\param RefType 모델링 좌표계의 갱신을 위한 참조 형태(0: 월드 좌표계, 1: 경계상자 좌표계)
*/
void GUcbsSrf1D::UpdateMC(int RefType)
{
}

/*!
*	\brief	곡면 메쉬의 정점 리스트를 다시 생성한다.
*/
void GUcbsSrf1D::UpdateMesh() 
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

/*************************************/
/* Implementation of GUcbsSrf Class */
/*************************************/
IMPLEMENT_GMESH_OBJECT(GUcbsSrf)

/*!
*	\brief 객체가 포함된 메쉬의 포인터를 반환한다.
*
*	\return 객체가 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GUcbsSrf::GetRefMesh()
{
	return this;
}

/*!
*	\brief 월드 좌표계에서 표현된 객체의 모델링 좌표계를 반환한다.
*
*	\return 월드 좌표계에서 표현된 객체의 모델링 좌표계를 반환한다.
*/
GTransf GUcbsSrf::GetTransf()
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
void GUcbsSrf::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);	
}

/*!
*	\brief	메쉬의 모델링 좌표계를 월드 좌표계로 갱신한다.
*
*	\param RefType 모델링 좌표계의 갱신을 위한 참조 형태(0: 월드 좌표계, 1: 경계상자 좌표계)
*/
void GUcbsSrf::UpdateMC(int RefType)
{
}

/*!
*	\brief	곡면 메쉬의 정점 리스트를 다시 생성한다.
*/
void GUcbsSrf::UpdateMesh() 
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
void GUcbsSrf::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);		
}

/*!
*	\brief	넙스 곡면의 생성을 위한 제어점 배열을 동적으로 할당
*	
*	\param _m 곡면의 U-방향 제어점의 마지막 인덱스
*	\param _n 곡면의 V-방향 제어점의 마지막 인덱스
*/
void GUcbsSrf::_alloc(int _m, int _n)
{
	m = _m; 
	n = _n;
	ARR_ALLOC(P, GCtrlPt, (m + 1) * (n + 1));
	for (int i = 0; i < (m + 1) * (n + 1); ++i)
		P[i].pMesh = this;
}

/*!
*	\brief	넙스곡면의 제어점의 동적 배열을 해제
*/
void GUcbsSrf::_free()
{
	ARR_FREE(P);
}

/*!
*	\brief	생성자
*
*	\param _Name 곡면의 이름
*	\param _MC 곡면의 모델링 좌표계
*	\param _m 곡면의 U-방향 제어점의 마지막 인덱스
*	\param _n 곡면의 V-방향 제어점의 마지막 인덱스
*	\param _P 곡면의 제어점 배열
*	\param _ParentName 부모 메쉬의 이름
*/
GUcbsSrf::GUcbsSrf(std::string _Name, GTransf _MC, int _m, int _n, GVector4 *_P, std::string _ParentName)
: GMesh(_Name, _MC, FACE_QUAD, _ParentName)
{
	assert(_m >= 3);
	assert(_n >= 3);

	_alloc(_m, _n);

	int num = (m + 1) * (n + 1);
	for (int i = 0; i < num; ++i)
	{
		if (_P == NULL)
			P[i].Set(0.0, 0.0, 0.0, 0.0);
		else
			P[i] = _P[i];
	}
}

/*!
*	\brief	복사 생성자
*
*	\param cpy 복사될 객체
*/
GUcbsSrf::GUcbsSrf(const GUcbsSrf &cpy)
: GMesh(cpy)
{
	_alloc(cpy.m, cpy.n);
	for (int i = 0; i < (m + 1) * (n + 1); ++i)
		P[i] = cpy.P[i];
}

/*!
*	\brief	소멸자
*/
GUcbsSrf::~GUcbsSrf()
{
	_free();
}

/*!
*	\brief	곡면의 대입연산자 중복
*
*	\param rhs 대입될 넙스곡면 객체
*
*	\return 대입된 곡면을 반환한다
*/
GUcbsSrf &GUcbsSrf::operator =(const GUcbsSrf &rhs)
{
	GMesh::operator =(rhs);

	_free();
	_alloc(rhs.m, rhs.n);

	for (int i = 0; i < (m + 1) * (n + 1); ++i)
		P[i] = rhs.P[i];
	
	return *this;
}

/*!
*	\brief	곡면을 렌더링 한다.
*
*	\param bSubMesh 서브 메쉬의 렌더링 여부(선택 모드에서는 false로 설정)
*/
void GUcbsSrf::Render(bool bSubMesh)
{
	GMesh::Render(bSubMesh);

	glPushMatrix();
	{
		// 월드 좌표계에서 객체의 모델링 좌표계로의 변환하고,
		glMultMatrixd(cast_mat(MC).M);

		// 곡면의 제어점을 렌더링할 경우,
		if (Renderer.RenderType & RENDER_CTRL_PT)
		{
			double size = Renderer.CtrlPtSize;
			
			// 재질을 설정하고,
			GMaterial Mat;
			Mat.SetAmbient(0.7f, 0.7f, 0.7f);
			Mat.SetDiffuse(0.7f, 0.7f, 0.7f);
			Mat.SetSpecular(0.7f, 0.7f, 0.7f);
			Mat.Enable(false);

			// 모든 제어점을 렌더링하고,
			for (int i = 0; i < m + 1; ++i)
			{
				for (int j = 0; j < n + 1; ++j)
				{
					if ((i == 0 && j == 0) || (i == m && j == n))
						continue;

					int idx = i * (n + 1) + j;
					glPushMatrix();
					glTranslated(P[idx][0], P[idx][1], P[idx][2]);
					glutSolidSphere(size, 10, 10);
					glPopMatrix();
				}
			}

			// 시작 제어점을 빨간색으로, 
			Mat.SetAmbient(0.7f, 0.0f, 0.0f);
			Mat.SetDiffuse(0.7f, 0.0f, 0.0f);
			Mat.SetSpecular(0.7f, 0.0f, 0.0f);
			Mat.Enable(false);
			glPushMatrix();
			glTranslated(P[0][0], P[0][1], P[0][2]);
			glutSolidSphere(size, 10, 10);
			glPopMatrix();

			// 끝 제어점을 파란색으로, 
			Mat.SetAmbient(0.0f, 0.0f, 0.7f);
			Mat.SetDiffuse(0.0f, 0.0f, 0.7f);
			Mat.SetSpecular(0.0f, 0.0f, 0.7f);
			Mat.Enable(false);
			glPushMatrix();
			glTranslated(P[m * (n + 1) + n][0], P[m * (n + 1) + n][1], P[m * (n + 1) + n][2]);
			glutSolidSphere(size, 10, 10);
			glPopMatrix();

			// 제어 메쉬도 렌더링 한다.
			glDisable(GL_LIGHTING);
			glLineWidth(2.0);
			glColor3d(0.0, 0.0, 1.0);
			for (int i = 0; i < n + 1; i++)
			{
				glBegin(GL_LINE_STRIP);
				for (int j = 0; j < m + 1; j++)
				{
					int idx = j * (n + 1) + i;
					glVertex3d(P[idx][0], P[idx][1], P[idx][2]);
				}
				glEnd();
			}
			glColor3d(0.0, 0.0, 1.0);
			for (int i = 0; i < m + 1; i++)
			{
				glBegin(GL_LINE_STRIP);
				for (int j = 0; j < n + 1; j++)
				{
					int idx = i * (n + 1) + j;
					glVertex3d(P[idx][0], P[idx][1], P[idx][2]);
				}
				glEnd();
			}
			glLineWidth(1.0);
			glEnable(GL_LIGHTING);
		}
	}
	glPopMatrix();
}

/*!
*	\brief	곡면을 생성한다.
*/
void GUcbsSrf::CreateMesh()
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
*	\brief	곡면의 렌더링을 위한 정점의 리스트를 생성한다.
*/
void GUcbsSrf::CreateMeshVert()
{
	// 샘플링 정보를 얻어와서, 
	int NumSampU = Renderer.NumSampU;
	int NumSampV = Renderer.NumSampV;
	double umin = 0.0, umax = 1.0, vmin = 0.0, vmax = 1.0;

	// 기존의 정점을 지우고, 
	Verts.clear();

	// 정의역을 계산하고,
	GetDomain(&umin, &umax, &vmin, &vmax);

	// 곡며위의 정점을 계산한다.
	GVector4 pt;
	//GMapDisp *pMap = pScene->FindMapDisp(Renderer.map_disp_name);
	for (int i = 0; i < NumSampU; ++i)
	{
		double u = umin + (umax - umin) * (double)i / (double)(NumSampU - 1);
		double s = (double)i / (double)(NumSampU - 1);
		for (int j = 0; j < NumSampV; ++j)
		{
			double v = vmin + (vmax - vmin) * (double)j / (double)(NumSampV - 1);
			pt = Eval(u, v);

			GVertex vtx(GPoint3(pt[0], pt[1], pt[2]), GVector3(0.0, 0.0, 0.0));
			AddVertex(vtx);
		}
	}
}

/*!
*	\brief	곡면의 렌더링을 위한 텍스처 좌표를 생성한다.
*/
void GUcbsSrf::CreateMeshTex()
{
	// 샘플링 정보를 얻어와서, 
	int NumSampU = Renderer.NumSampU;
	int NumSampV = Renderer.NumSampV;

	// 텍스처 좌표를 생성하고 저장함
	Texs.clear();
	for (int i = 0; i < NumSampU; ++i)
	{
		double s = (double)i / (double)(NumSampU - 1);
		for (int j = 0; j < NumSampV; ++j)
		{
			double t = (double)j / (double)(NumSampV - 1);
			AddTexel(GVector2(s, t));
		}
	}
}

/*!
*	\brief	넙스곡면의 렌더링을 위한 삼각형 리스트를 생성한다.
*/
void GUcbsSrf::CreateMeshFace()
{
	// 삼각형 정보를 지우고,
	Faces.clear();

	// 샘플링 정보를 얻어와서, 
	int NumSampU = Renderer.NumSampU;
	int NumSampV = Renderer.NumSampV;

	// 삼각형을 생성하고 추가한다.
	for (int i = 0; i < NumSampU - 1; ++i)
	{
		for(int j = 0; j < NumSampV - 1; ++j)
		{
			int vidx0 = i * NumSampV + j;
			int vidx1 = vidx0 + NumSampV;
			int vidx2 = vidx1 + 1;
			int vidx3 = vidx0 + 1;

			GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
			GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);

			AddFace(f0);
			AddFace(f1);
		}
	}
}

/*!
*	\brief	곡면위의 점 S(u, v)를 계산
*
*	\param u U-방향 매개변수의 값
*
*	\return 곡면위의 점 S(u, v)의 위치를 계산
*/
GVector4 GUcbsSrf::Eval(double u, ...)
{
	double v;
	va_list ap;
	va_start(ap, u);
	v = va_arg(ap, double);
	va_end(ap);

	// get local parameter values for u and v
	double uu = u * (m - 2);
	double vv = v * (n - 2);
	int k = (int)uu;	
	int l = (int)vv;	// P(k,l) is the left bottum control points.
	uu = uu - k;	
	vv = vv - l;	// get the local parameter of u and v.

	if (k == m - 2)	// special case for u is 1.0. 
	{
		k = m - 3;
		uu = 1.0;
	}
	if (l == n - 2)  // special case for v is 1.0.
	{
		l = n - 3;
		vv = 1.0;
	}

	static double basis_u[4], basis_v[4];
	get_ucbs_basis(uu, basis_u);
	get_ucbs_basis(vv, basis_v);

	GVector4 pt;
	for (int i = 0; i < 4; ++i)
	{
		int idx = (k + i) * (n + 1) + l;
		pt = pt + basis_u[i] * (P[idx] * basis_v[0] + P[idx + 1] * basis_v[1] + P[idx + 2] * basis_v[2] + P[idx + 3] * basis_v[3]);
	}

	return pt;
}

/*!
*	\brief	곡면위의 점 S(u, v)를 계산
*
*	\param u U-방향 매개변수의 값
*
*	\return 곡면위의 점 S(u, v)의 위치를 계산
*/
GVector4 GUcbsSrf::EvalFast(double u, ...)
{
	double v;
	va_list ap;
	va_start(ap, u);
	v = va_arg(ap, double);
	va_end(ap);

	// get local parameter values for u and v
	double uu = u * (m - 2);
	double vv = v * (n - 2);
	int k = (int)uu;	
	int l = (int)vv;	// P(k,l) is the left bottum control points.
	uu = uu - k;	
	vv = vv - l;	// get the local parameter of u and v.

	if (k == m - 2)	// special case for u is 1.0. 
	{
		k = m - 3;
		uu = 1.0;
	}
	if (l == n - 2)  // special case for v is 1.0.
	{
		l = n - 3;
		vv = 1.0;
	}

	int uidx = (int)(uu * 1000.0 + 0.5);
	int vidx = (int)(vv * 1000.0 + 0.5);

	static double basis_u[4], basis_v[4];
	ARR_COPY(basis_u, CBS_BASIS[uidx], 4);
	ARR_COPY(basis_v, CBS_BASIS[vidx], 4);

	GVector4 pt;
	for (int i = 0; i < 4; ++i)
	{
		int idx = (k + i) * (n + 1) + l;
		pt += basis_u[i] * (P[idx] * basis_v[0] + P[idx + 1] * basis_v[1] + P[idx + 2] * basis_v[2] + P[idx + 3] * basis_v[3]);
	}

	return pt;
}

/*!
*	\brief 곡면의 도메인을 반환한다.
*
*	\param min U 방향 도메인의 시작 값이 저장된다.
*/
void GUcbsSrf::GetDomain(double *umin, ...)
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
void GUcbsSrf::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
	for (int i = 0; i < (m + 1) * (n + 1); ++i)
		CtrlPtList.push_back(&P[i]);
}

/*!
*	\brief	곡면위의 점 S(u, v)의 접평면(Tangent Plane)의 기저 Su, Sv를 구함
*	
*	\param u U-방향 매개변수 값
*	\param v V-방향 매개변수 값
*	\param Su U-방향 1차 미분 벡터값이 저장됨
*	\param Sv V-방향 1차 미분 벡터값이 저장됨
*	\param N 곡면의 법선 벡터가 저장됨
*/
void GUcbsSrf::EvalTanPln(double u, double v, GVector3 &Su, GVector3 &Sv, GVector3 &N)
{
	// get local parameter values for u and v
	double uu = u * (m - 2);
	double vv = v * (n - 2);
	int k = (int)uu;	
	int l = (int)vv;	// P(k,l) is the left bottum control points.
	uu = uu - k;	
	vv = vv - l;	// get the local parameter of u and v.

	if (k == m - 2)	// special case for u is 1.0. 
	{
		k = m - 3;
		uu = 1.0;
	}
	if (l == n - 2)  // special case for v is 1.0.
	{
		l = n - 3;
		vv = 1.0;
	}

	int uidx = (int)(uu * 1000.0 + 0.5);
	int vidx = (int)(vv * 1000.0 + 0.5);

	static double basis_u[4], basis_v[4];
	static double basis_du[4], basis_dv[4];

	get_ucbs_basis(uu, basis_u);
	get_ucbs_basis(vv, basis_v);
	get_ucbs_deriv_basis(uu, basis_du);
	get_ucbs_deriv_basis(vv, basis_dv);

	// Su, Sv 계산
	GVector4 tmp1, tmp2;
	for (int i = 0; i < 4; ++i)
	{
		int idx = (k + i) * (n + 1) + l;
		tmp1 += basis_du[i] * (P[idx] * basis_v[0] + P[idx + 1] * basis_v[1] + P[idx + 2] * basis_v[2] + P[idx + 3] * basis_v[3]);
		tmp2 += basis_u[i] * (P[idx] * basis_dv[0] + P[idx + 1] * basis_dv[1] + P[idx + 2] * basis_dv[2] + P[idx + 3] * basis_dv[3]);
	}

	Su.Set(tmp1[0], tmp1[1], tmp1[2]);
	Sv.Set(tmp2[0], tmp2[1], tmp2[2]);
	N = Su ^ Sv;
}

/*!
*	\brief	곡면위의 점 S(u, v)의 접평면(Tangent Plane)의 기저 Su, Sv를 구함
*	
*	\param u U-방향 매개변수 값
*	\param v V-방향 매개변수 값
*	\param Su U-방향 1차 미분 벡터값이 저장됨
*	\param Sv V-방향 1차 미분 벡터값이 저장됨
*	\param N 곡면의 법선 벡터가 저장됨
*/
void GUcbsSrf::EvalTanPlnFast(double u, double v, GVector3 &Su, GVector3 &Sv, GVector3 &N)
{
	// get local parameter values for u and v
	double uu = u * (m - 2);
	double vv = v * (n - 2);
	int k = (int)uu;	
	int l = (int)vv;	// P(k,l) is the left bottum control points.
	uu = uu - k;	
	vv = vv - l;	// get the local parameter of u and v.

	if (k == m - 2)	// special case for u is 1.0. 
	{
		k = m - 3;
		uu = 1.0;
	}
	if (l == n - 2)  // special case for v is 1.0.
	{
		l = n - 3;
		vv = 1.0;
	}

	int uidx = (int)(uu * 1000.0 + 0.5);
	int vidx = (int)(vv * 1000.0 + 0.5);

	static double basis_u[4], basis_v[4];
	static double basis_du[4], basis_dv[4];

	ARR_COPY(basis_u, CBS_BASIS[uidx], 4);
	ARR_COPY(basis_v, CBS_BASIS[vidx], 4);
	ARR_COPY(basis_du, CBS_BASIS_DERIV[uidx], 4);	
	ARR_COPY(basis_dv, CBS_BASIS_DERIV[vidx], 4);

	// Su, Sv 계산
	GVector4 tmp1, tmp2;
	for (int i = 0; i < 4; ++i)
	{
		int idx = (k + i) * (n + 1) + l;
		tmp1 += basis_du[i] * (P[idx] * basis_v[0] + P[idx + 1] * basis_v[1] + P[idx + 2] * basis_v[2] + P[idx + 3] * basis_v[3]);
		tmp2 += basis_u[i] * (P[idx] * basis_dv[0] + P[idx + 1] * basis_dv[1] + P[idx + 2] * basis_dv[2] + P[idx + 3] * basis_dv[3]);
	}

	Su.Set(tmp1[0], tmp1[1], tmp1[2]);
	Sv.Set(tmp2[0], tmp2[1], tmp2[2]);
	N = Su ^ Sv;
}

/*!
*	\brief	곡면의 refinement(binary subdivision)를 수행한다.
*/
void GUcbsSrf::Refine()
{
	// 새로운 제어점 배열을 정의하고 초기화 한다.
	int mm = 2 * (m - 1);
	int nn = 2 * (n - 1);
	int num_ctlpt = (mm + 1) * (nn + 1);
	GCtrlPt *P1 = new GCtrlPt [num_ctlpt];
	for (int i = 0; i < num_ctlpt; ++i)
	{
		P1[i].pMesh = this;
		P1[i].Set(0.0, 0.0, 0.0, 0.0);
	}
	
	// 현재 제어점 배열을 P0로 하고, 새로운 제어점을 계산한다.
	GCtrlPt *P0 = P;	
	for(int i = 0; i < mm + 1; i += 2)
	{
		for(int j = 0; j < nn + 1; j += 2)
		{
			int idx0 = (n + 1) * (i / 2) + (j / 2);		// P0's index.
			int idx1 = (nn + 1) * i + j;				// P1's index.

			P1[idx1] = (P0[idx0] + P0[idx0 + 1] + P0[idx0 + n + 1] + P0[idx0 + n + 2]) / 4.0;

			if (j != nn)
				P1[idx1 + 1] = (P0[idx0] + P0[idx0 + 2] + P0[idx0 + n + 1] + 
				P0[idx0 + n + 3] + 6.0 * (P0[idx0 + 1] + P0[idx0 + n + 2])) / 16.0;

			if (i != mm)
				P1[idx1 + nn + 1] = (P0[idx0] + P0[idx0 + 1] + P0[idx0 + 2 * (n + 1)] + 
				P0[idx0 + 2 * (n + 1) + 1] + 6.0 * (P0[idx0 + n + 1] + P0[idx0 + n + 2])) / 16.0;

			if (i != mm && j != nn)
				P1[idx1 + nn + 2] = (P0[idx0] + P0[idx0 + 2] + P0[idx0 + 2 * (n + 1)] + 
				P0[idx0 + 2 * (n + 2)] + 6.0 * (P0[idx0 + 1] + P0[idx0 + n + 1] + P0[idx0 + n + 3] + 
				P0[idx0 + 2 * (n + 1) + 1]) + 36.0 * P0[idx0 + n + 2]) / 64.0;
		}
	}

	_free();
	m = mm;
	n = nn;

	_alloc(m, n);
	for (int i = 0; i < num_ctlpt; ++i)
	{
		P[i] = P1[i];
		P[i].pMesh = this;
	}
	delete [] P1;
}

/*************************************/
/* Implementation of GUcbsVol1D Class */
/*************************************/
IMPLEMENT_GMESH_OBJECT(GUcbsVol1D)

/*!
*	\brief 객체가 포함된 메쉬의 포인터를 반환한다.
*
*	\return 객체가 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GUcbsVol1D::GetRefMesh()
{
	return this;
}

/*!
*	\brief 월드 좌표계에서 표현된 객체의 모델링 좌표계를 반환한다.
*
*	\return 월드 좌표계에서 표현된 객체의 모델링 좌표계를 반환한다.
*/
GTransf GUcbsVol1D::GetTransf()
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
void GUcbsVol1D::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);	
}

/*!
*	\brief	메쉬의 모델링 좌표계를 월드 좌표계로 갱신한다.
*
*	\param RefType 모델링 좌표계의 갱신을 위한 참조 형태(0: 월드 좌표계, 1: 경계상자 좌표계)
*/
void GUcbsVol1D::UpdateMC(int RefType)
{
}

/*!
*	\brief	볼륨 메쉬의 정점 리스트를 다시 생성한다.
*/
void GUcbsVol1D::UpdateMesh() 
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
void GUcbsVol1D::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);	
}

/*!
*	\brief	넙스 볼륨의 생성을 위한 제어점 배열을 동적으로 할당
*	
*	\param _l 곡면의 U-방향 제어점의 마지막 인덱스
*	\param _m 곡면의 V-방향 제어점의 마지막 인덱스
*	\param _n 곡면의 W-방향 제어점의 마지막 인덱스
*/
void GUcbsVol1D::_alloc(int _l, int _m, int _n)
{
	l = _l;
	m = _m; 
	n = _n;
	ARR_ALLOC(P, double, (l + 1) * (m + 1) * (n + 1));
}

/*!
*	\brief	넙스곡면의 제어점의 동적 배열을 해제
*/
void GUcbsVol1D::_free()
{
	ARR_FREE(P);
}

/*!
*	\brief	생성자
*
*	\param _Name 볼륨의 이름
*	\param _MC 곡면의 모델링 좌표계
*	\param _l 볼륨의 U-방향 제어점의 마지막 인덱스
*	\param _m 볼륨의 V-방향 제어점의 마지막 인덱스
*	\param _n 볼륨의 W-방향 제어점의 마지막 인덱스
*	\param _P 볼륨의 제어점 배열
*	\param _ParentName 부모 볼륨의 이름
*/
GUcbsVol1D::GUcbsVol1D(std::string _Name, GTransf _MC, int _l, int _m, int _n, double *_P, std::string _ParentName)
: GMesh(_Name, _MC, FACE_TRI, _ParentName)
{
	assert(_l >= 3);
	assert(_m >= 3);
	assert(_n >= 3);

	_alloc(_l, _m, _n);
	if (_P == NULL)
		ARR_ZERO(P, (l + 1) * (m + 1) * (n + 1));
	else
		ARR_COPY(P, _P, (l + 1) * (m + 1) * (n + 1));
}

/*!
*	\brief	복사 생성자
*
*	\param cpy 복사될 객체
*/
GUcbsVol1D::GUcbsVol1D(const GUcbsVol1D &cpy)
: GMesh(cpy)
{
	_alloc(cpy.l, cpy.m, cpy.n);
	ARR_COPY(P, cpy.P, (l + 1) * (m + 1) * (n + 1));
}

/*!
*	\brief	소멸자
*/
GUcbsVol1D::~GUcbsVol1D()
{
	_free();
}

/*!
*	\brief	곡면의 대입연산자 중복
*
*	\param rhs 대입될 넙스곡면 객체
*
*	\return 대입된 곡면을 반환한다
*/
GUcbsVol1D &GUcbsVol1D::operator =(const GUcbsVol1D &rhs)
{
	GMesh::operator =(rhs);

	_free();
	_alloc(rhs.l, rhs.m, rhs.n);
	ARR_COPY(P, rhs.P, (l + 1) * (m + 1) * (n + 1));

	return *this;
}

/*!
*	\brief 볼륨의 표면을 렌더링 한다.
*
*	\param bSubMesh 서브 메쉬의 렌더링 여부(선택 모드에서는 false로 설정)
*/
void GUcbsVol1D::Render(bool bSubMesh)
{
	GMesh::Render(bSubMesh);
}

/*!
*	\brief 볼륨의 표면 메쉬를 생성한다.
*/
void GUcbsVol1D::CreateMesh()
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
*	\brief 볼륨의 표면 메쉬의 정점을 생성한다.
*/
void GUcbsVol1D::CreateMeshVert()
{
}

/*!
*	\brief 볼륨의 표면 메쉬의 텍스처 좌표를 생성한다.
*/
void GUcbsVol1D::CreateMeshTex()
{
}

/*!
*	\brief 볼륨의 표면 메쉬의 삼각형을 생성한다.
*/
void GUcbsVol1D::CreateMeshFace()
{
}

/*!
*	\brief	볼륨내의 점 V(u, v, w)를 계산
*
*	\param u U-방향 매개변수의 값
*
*	\return 볼륨내의 점 V(u, v, w)의 위치를 계산
*/
GVector4 GUcbsVol1D::Eval(double u, ...)
{
	double v, w;
	va_list ap;
	va_start(ap, u);
	v = va_arg(ap, double);
	w = va_arg(ap, double);
	va_end(ap);

	// get local parameter values for u and v
	double uu = u * (l - 2);
	double vv = v * (m - 2);
	double ww = w * (n - 2);
	int r = (int)uu;	
	int s = (int)vv;	
	int t = (int)ww;	// P(r, s, t) is the left bottum control points.
	uu = uu - r;	
	vv = vv - s;
	ww = ww - t;	// get the local parameter of u and v.

	if (r == l - 2)	// special case for u is 1.0. 
	{
		r = l - 3;
		uu = 1.0;
	}
	if (s == m - 2)  // special case for v is 1.0.
	{
		s = m - 3;
		vv = 1.0;
	}
	if (t == n - 2)  // special case for w is 1.0.
	{
		t = n - 3;
		ww = 1.0;
	}

	static double basis_u[4], basis_v[4], basis_w[4];
	get_ucbs_basis(uu, basis_u);
	get_ucbs_basis(vv, basis_v);
	get_ucbs_basis(ww, basis_w);

	// 	int uidx = (int)(uu * 1000.0 + 0.5);
	// 	int vidx = (int)(vv * 1000.0 + 0.5);
	// 	int widx = (int)(ww * 1000.0 + 0.5);
	// 	ARR_COPY(basis_u, CBS_BASIS[uidx], 4);
	// 	ARR_COPY(basis_v, CBS_BASIS[vidx], 4);
	// 	ARR_COPY(basis_w, CBS_BASIS[widx], 4);

	double f = 0;
	for (int i = 0; i < 4; ++i)
	{
		double tmp0 = 0;
		for (int j = 0; j < 4; ++j)
		{
			int idx = (r + i) * (m + 1) * (n + 1) + (s + j) * (n + 1) + t;
			tmp0 += basis_v[j] * (basis_w[0] * P[idx] + basis_w[1] * P[idx + 1] + basis_w[2] * P[idx + 2] + basis_w[3] * P[idx + 3]);
		}
		f += basis_u[i] * tmp0;
	}

	return GVector4(f, 0.0, 0.0, 0.0);
}

/*!
*	\brief 곡면의 도메인을 반환한다.
*
*	\param min U 방향 도메인의 시작 값이 저장된다.
*/
void GUcbsVol1D::GetDomain(double *umin, ...)
{
	double *umax, *vmin, *vmax, *wmin, *wmax;
	va_list ap;
	va_start(ap, umin);
	umax = va_arg(ap, double *);
	vmin = va_arg(ap, double *);
	vmax = va_arg(ap, double *);
	wmin = va_arg(ap, double *);
	wmax = va_arg(ap, double *);
	va_end(ap);

	*umin = 0.0;
	*umax = 1.0;
	*vmin = 0.0;
	*vmax = 1.0;
	*wmin = 0.0;
	*wmax = 1.0;
}

/*!
*	\brief	볼륨의 제어점 리스트를 반환한다.
*
*	\return 볼륨의 제어점 리스트를 반환한다.
*/
void GUcbsVol1D::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
	// 	for (int i = 0; i < (m + 1) * (n + 1); ++i)
	// 		CtrlPtList.push_back(&P[i]);
}

/*!
*	\brief	곡면의 refinement 를 수행한다.
*/
void GUcbsVol1D::Refine()
{
	int ll = 2 * (l - 1);
	int mm = 2 * (m - 1);
	int nn = 2 * (n - 1);

	int num_ctlpt = (ll + 1) * (mm + 1) * (nn + 1);
	double *Q = new double [num_ctlpt];	// refined new control points.
	ARR_ZERO(Q, num_ctlpt);

	// compute new control points.
	for (int i = 0; i < ll + 1; i += 2)	// U-방향
	{
		for (int j = 0; j < mm + 1; j += 2)	// V-방향
		{
			for (int k = 0; k < nn + 1; k += 2)	// W-방향
			{
				int nv = n + 1;					// offset to element in next v-direction
				int nu = (n + 1) * (m + 1);		// offset to element in next u-direction

				int idx0 = nu * (i / 2) + nv * (j / 2) + (k / 2);		// P's index
				int idx1 = (nn + 1) * (mm + 1) * i + (nn + 1) * j + k;	// Q's index

				// case 0:
				Q[idx1] = (P[idx0] + P[idx0 + 1] + P[idx0 + nv] + P[idx0 + nv + 1] +
					P[idx0 + nu] + P[idx0 + nu + 1] + P[idx0 + nu + nv] + P[idx0 + nu + nv + 1]) / 8.0;

				// case 1:
				if (k != nn)
				{
					Q[idx1 + 1] = (P[idx0] + P[idx0 + 2] + P[idx0 + nv] + P[idx0 + nv + 2] +
						P[idx0 + nu] + P[idx0 + nu + 2] + P[idx0 + nu + nv] + P[idx0 + nu + nv + 2] +
						6.0 * (P[idx0 + 1] + P[idx0 + nv + 1] + P[idx0 + nu + 1] + P[idx0 + nu + nv + 1])) / 32.0;
				}

				// case 2:
				if (j != mm)
				{
					Q[idx1 + nn + 1] = (P[idx0] + P[idx0 + nv * 2] + P[idx0 + 1] + P[idx0 + nv * 2 + 1] +
						P[idx0 + nu] + P[idx0 + nu + nv * 2] + P[idx0 + nu + 1] + P[idx0 + nu + nv * 2 + 1] +
						6.0 * (P[idx0 + nv] + P[idx0 + nv + 1] + P[idx0 + nu + nv] + P[idx0 + nu + nv + 1])) / 32.0;
				}

				// case 3:
				if (j != mm && k != nn)
				{
					Q[idx1 + nn + 2] = (P[idx0] + P[idx0 + 2] + P[idx0 + 2 * nv] + P[idx0 + 2 * nv + 2] +
						P[idx0 + nu] + P[idx0 + 2 + nu] + P[idx0 + 2 * nv + nu] + P[idx0 + 2 * nv + 2 + nu] +
						6.0 * (P[idx0 + 1] + P[idx0 + nv] + P[idx0 + 2 * nv + 1] + P[idx0 + nv + 2] +
						P[idx0 + 1 + nu] + P[idx0 + nv + nu] + P[idx0 + 2 * nv + 1 + nu] + P[idx0 + nv + 2 + nu]) +
						36.0 * (P[idx0 + nv + 1] + P[idx0 + nu + nv + 1])) / 128.0;
				}

				// case 4:
				if (i != ll)
				{
					Q[idx1 + (nn + 1) * (mm + 1)] = (P[idx0] + P[idx0 + 1] + P[idx0 + nv] + P[idx0 + nv + 1] +
						P[idx0 + 2 * nu] + P[idx0 + 1 + 2 * nu] + P[idx0 + nv + 2 * nu] + P[idx0 + nv + 1 + 2 * nu]+
						6.0 * (P[idx0 + nu] + P[idx0 + nu + 1] + P[idx0 + nu + nv] + P[idx0 + nu + nv + 1])) / 32.0;
				}

				// case 5:
				if (i != ll && k != nn)
				{
					Q[idx1 + (nn + 1) * (mm + 1) + 1] = (P[idx0] + P[idx0 + 2] + P[idx0 + nv] + P[idx0 + nv + 2] +
						P[idx0 + 2 * nu] + P[idx0 + 2 + 2 * nu] + P[idx0 + nv + 2 * nu] + P[idx0 + nv + 2 + 2 * nu] +
						6.0 * (P[idx0 + 1] + P[idx0 + nv + 1] + P[idx0 + nu] + P[idx0 + nu + 2] + P[idx0 + nu + nv] + P[idx0 + nu + nv + 2] + P[idx0 + 2 * nu + 1] + P[idx0 + 2 * nu + nv + 1]) + 
						36.0 * (P[idx0 + nu + 1] + P[idx0 + nu + nv + 1])) / 128.0;
				}

				// case 6:
				if (i != ll && j != mm)
				{
					Q[idx1 + (nn + 1) * (mm + 2)] = (P[idx0] + P[idx0 + 1] + P[idx0 + 2 * nv] + P[idx0 + 2 * nv + 1] +
						P[idx0 + 2 * nu] + P[idx0 + 1 + 2 * nu] + P[idx0 + 2 * nv + 2 * nu] + P[idx0 + 2 * nv + 1 + 2 * nu] +
						6.0 * (P[idx0 + nv] + P[idx0 + nv + 1] + P[idx0 + nu] + P[idx0 + nu + 1] + P[idx0 + nu + 2 * nv] + P[idx0 + nu + 2 * nv + 1] + P[idx0 + 2 * nu + nv] + P[idx0 + 2 * nu + nv + 1]) +
						36.0 * (P[idx0 + nu + nv] + P[idx0 + nu + nv + 1])) / 128.0;
				}

				// case 7:
				if (i != ll && j != mm && k != nn)
				{
					Q[idx1 + (nn + 1) * (mm + 2) + 1] = (P[idx0] + P[idx0 + 2] + P[idx0 + 2 * nv] + P[idx0 + 2 * nv + 2] +
						P[idx0 + 2 * nu] + P[idx0 + 2 + 2 * nu] + P[idx0 + 2 * nv + 2 * nu] + P[idx0 + 2 * nv + 2 + 2 * nu] +
						6.0 * (P[idx0 + 1] + P[idx0 + nv] + P[idx0 + 2 * nv + 1] + P[idx0 + nv + 2] +
						P[idx0 + nu] + P[idx0 + nu + 2] + P[idx0 + 2 * nv + nu] + P[idx0 + 2 * nv + 2 + nu] +
						P[idx0 + 1 + 2 * nu] + P[idx0 + nv + 2 * nu] + P[idx0 + 2 * nv + 1 + 2 * nu] + P[idx0 + nv + 2 + 2 * nu]) +
						36.0 * (P[idx0 + nv + 1] + P[idx0 + nu + 1] + P[idx0 + nu + nv] + P[idx0 + nu + 2 * nv + 1] + P[idx0 + nu + nv + 2] + P[idx0 + 2 * nu + nv + 1]) +
						216.0 * P[idx0 + nu + nv + 1]) / 512.0;
				}				
			}
		}
	}

	_free();
	l = ll;
	m = mm;
	n = nn;
	P = Q;
}

/*************************************/
/* Implementation of GUcbsVol Class */
/*************************************/
IMPLEMENT_GMESH_OBJECT(GUcbsVol)

/*!
*	\brief 객체가 포함된 메쉬의 포인터를 반환한다.
*
*	\return 객체가 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GUcbsVol::GetRefMesh()
{
	return this;
}

/*!
*	\brief 월드 좌표계에서 표현된 객체의 모델링 좌표계를 반환한다.
*
*	\return 월드 좌표계에서 표현된 객체의 모델링 좌표계를 반환한다.
*/
GTransf GUcbsVol::GetTransf()
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
void GUcbsVol::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);	
}

/*!
*	\brief	메쉬의 모델링 좌표계를 월드 좌표계로 갱신한다.
*
*	\param RefType 모델링 좌표계의 갱신을 위한 참조 형태(0: 월드 좌표계, 1: 경계상자 좌표계)
*/
void GUcbsVol::UpdateMC(int RefType)
{
}

/*!
*	\brief	볼륨 메쉬의 정점 리스트를 다시 생성한다.
*/
void GUcbsVol::UpdateMesh() 
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
void GUcbsVol::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);		
}

/*!
*	\brief	넙스 볼륨의 생성을 위한 제어점 배열을 동적으로 할당
*	
*	\param _l 곡면의 U-방향 제어점의 마지막 인덱스
*	\param _m 곡면의 V-방향 제어점의 마지막 인덱스
*	\param _n 곡면의 W-방향 제어점의 마지막 인덱스
*/
void GUcbsVol::_alloc(int _l, int _m, int _n)
{
	l = _l;
	m = _m; 
	n = _n;
	ARR_ALLOC(P, GCtrlPt, (l + 1) * (m + 1) * (n + 1));
	for (int i = 0; i < (l + 1) * (m + 1) * (n + 1); ++i)
		P[i].pMesh = this;
}

/*!
*	\brief	넙스곡면의 제어점의 동적 배열을 해제
*/
void GUcbsVol::_free()
{
	if (P != NULL)
		delete [] P;
	P = NULL;
}

/*!
*	\brief	생성자
*
*	\param _Name 볼륨의 이름
*	\param _MC 곡면의 모델링 좌표계
*	\param _l 볼륨의 U-방향 제어점의 마지막 인덱스
*	\param _m 볼륨의 V-방향 제어점의 마지막 인덱스
*	\param _n 볼륨의 W-방향 제어점의 마지막 인덱스
*	\param _P 볼륨의 제어점 배열
*	\param _ParentName 부모 볼륨의 이름
*/
GUcbsVol::GUcbsVol(std::string _Name, GTransf _MC, int _l, int _m, int _n, GVector4 *_P, std::string _ParentName)
: GMesh(_Name, _MC, FACE_QUAD, _ParentName)
{
	assert(_l >= 3);
	assert(_m >= 3);
	assert(_n >= 3);

	_alloc(_l, _m, _n);

	int num = (l + 1) * (m + 1) * (n + 1);
	if (_P != NULL)
	{
		for (int i = 0; i < num; ++i)
			P[i] = _P[i];
	}		
}

/*!
*	\brief	복사 생성자
*
*	\param cpy 복사될 객체
*/
GUcbsVol::GUcbsVol(const GUcbsVol &cpy)
: GMesh(cpy)
{
	_alloc(cpy.l, cpy.m, cpy.n);
	for (int i = 0; i < (l + 1) * (m + 1) * (n + 1); ++i)
		P[i] = cpy.P[i];
}

/*!
*	\brief	소멸자
*/
GUcbsVol::~GUcbsVol()
{
	_free();
}

/*!
*	\brief	볼륨의 대입연산자 중복
*
*	\param rhs 대입될 넙스볼륨 객체
*
*	\return 대입된 볼륨을 반환한다
*/
GUcbsVol &GUcbsVol::operator =(const GUcbsVol &rhs)
{
	GMesh::operator =(rhs);

	_free();
	_alloc(rhs.l, rhs.m, rhs.n);

	for (int i = 0; i < (l + 1) * (m + 1) * (n + 1); ++i)
		P[i] = rhs.P[i];

	return *this;
}

/*!
*	\brief 볼륨의 표면을 렌더링 한다.
*
*	\param bSubMesh 서브 메쉬의 렌더링 여부(선택 모드에서는 false로 설정)
*/
void GUcbsVol::Render(bool bSubMesh)
{
	GMesh::Render(bSubMesh);

	glPushMatrix();
	{
		// 월드 좌표계에서 객체의 모델링 좌표계로의 변환하고,
		glMultMatrixd(cast_mat(MC).M);

		// 곡면의 제어점을 렌더링할 경우,
		if (Renderer.RenderType & RENDER_CTRL_PT)
		{
			double size = Renderer.CtrlPtSize;

			// 재질을 설정하고,
			GMaterial Mat;
			Mat.SetAmbient(0.7f, 0.7f, 0.7f);
			Mat.SetDiffuse(0.7f, 0.7f, 0.7f);
			Mat.SetSpecular(0.7f, 0.7f, 0.7f);
			Mat.Enable(false);

			// 모든 제어점을 렌더링하고,
			for (int i = 0; i < l + 1; ++i)
			{
				for (int j = 0; j < m + 1; ++j)
				{
					for (int k = 0; k < n + 1; ++k)
					{
						if ((i == 0 && j == 0 && k == 0) || (i == l && j == m && k == n))
							continue;

						int idx = i * (m + 1) * (n + 1) + j * (n + 1) + k;
						glPushMatrix();
						glTranslated(P[idx][0], P[idx][1], P[idx][2]);
						glutSolidSphere(size, 10, 10);
						glPopMatrix();
					}					
				}
			}

			// 시작 제어점을 빨간색으로, 
			Mat.SetAmbient(0.7f, 0.0f, 0.0f);
			Mat.SetDiffuse(0.7f, 0.0f, 0.0f);
			Mat.SetSpecular(0.7f, 0.0f, 0.0f);
			Mat.Enable(false);
			glPushMatrix();
			glTranslated(P[0][0], P[0][1], P[0][2]);
			glutSolidSphere(size, 10, 10);
			glPopMatrix();

			// 끝 제어점을 파란색으로, 
			Mat.SetAmbient(0.0f, 0.0f, 0.7f);
			Mat.SetDiffuse(0.0f, 0.0f, 0.7f);
			Mat.SetSpecular(0.0f, 0.0f, 0.7f);
			Mat.Enable(false);
			glPushMatrix();
			glTranslated(P[(m + 1) * (n + 1) * (l + 1)][0], P[(m + 1) * (n + 1) * (l + 1)][1], P[(m + 1) * (n + 1) * (l + 1)][2]);
			glutSolidSphere(size, 10, 10);
			glPopMatrix();

			// 제어 메쉬도 렌더링 한다.
// 			glDisable(GL_LIGHTING);
// 			glLineWidth(2.0);
// 			glColor3d(0.0, 0.0, 1.0);
// 			for (int i = 0; i < n + 1; i++)
// 			{
// 				glBegin(GL_LINE_STRIP);
// 				for (int j = 0; j < m + 1; j++)
// 				{
// 					glVertex3d(P[j][i][0], P[j][i][1], P[j][i][2]);
// 				}
// 				glEnd();
// 			}
// 			glColor3d(0.0, 0.0, 1.0);
// 			for (int i = 0; i < m + 1; i++)
// 			{
// 				glBegin(GL_LINE_STRIP);
// 				for (int j = 0; j < n + 1; j++)
// 				{
// 					glVertex3d(P[i][j][0], P[i][j][1], P[i][j][2]);
// 				}
// 				glEnd();
// 			}
// 			glLineWidth(1.0);
// 			glEnable(GL_LIGHTING);			
		}
	}
	glPopMatrix();
}

/*!
*	\brief 볼륨의 표면 메쉬를 생성한다.
*/
void GUcbsVol::CreateMesh()
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
*	\brief 볼륨의 표면 메쉬의 정점을 생성한다.
*/
void GUcbsVol::CreateMeshVert()
{
	// 기존의 정점을 지우고, 
	Verts.clear();

	double u, v, w;
	double umin = Renderer.IsoSrfParam[0];
	double umax = Renderer.IsoSrfParam[1];
	double vmin = Renderer.IsoSrfParam[2];
	double vmax = Renderer.IsoSrfParam[3];
	double wmin = Renderer.IsoSrfParam[4];
	double wmax = Renderer.IsoSrfParam[5];

	int SNumU = Renderer.NumSampU;
	int SNumV = Renderer.NumSampV;
	int SNumW = Renderer.NumSampW;

	GVector4 pt;

	// u = umin 곡면의 정점 생성하고,
	u = umin;
	for (int i = 0; i < SNumW; ++i)
	{
		w = wmin + (wmax - wmin) * (double)i / (double)(SNumW - 1);
		for (int j = 0; j < SNumV; ++j)
		{
			v = vmin + (vmax - vmin) * (double)j / (double)(SNumV - 1);
			pt = Eval(u, v, w);
			GVertex vtx(GPoint3(pt[0], pt[1], pt[2]), GVector3(0.0, 0.0, 0.0));
			AddVertex(vtx);			
		}
	}

	// u = umax 곡면의 정점 생성하고,
	u = umax;
	for (int i = 0; i < SNumV; ++i)
	{
		v = vmin + (vmax - vmin) * (double)i / (double)(SNumV - 1);
		for (int j = 0; j < SNumW; ++j)
		{
			w = wmin + (wmax - wmin) * (double)j / (double)(SNumW - 1);
			pt = Eval(u, v, w);
			GVertex vtx(GPoint3(pt[0], pt[1], pt[2]), GVector3(0.0, 0.0, 0.0));
			AddVertex(vtx);			
		}
	}

	// v = vmin 곡면의 정점을 생성하고,
	v = vmin;
	for (int i = 0; i < SNumU; ++i)
	{
		u = umin + (umax - umin) * (double)i / (double)(SNumU - 1);
		for (int j = 0; j < SNumW; ++j)
		{
			w = wmin + (wmax - wmin) * (double)j / (double)(SNumW - 1);
			pt = Eval(u, v, w);
			GVertex vtx(GPoint3(pt[0], pt[1], pt[2]), GVector3(0.0, 0.0, 0.0));
			AddVertex(vtx);			
		}
	}

	// v = vmax 곡면의 정점을 생성하고,
	v = vmax;
	for (int i = 0; i < SNumW; ++i)
	{
		w = wmin + (wmax - wmin) * (double)i / (double)(SNumW - 1);
		for (int j = 0; j < SNumU; ++j)
		{
			u = umin + (umax - umin) * (double)j / (double)(SNumU - 1);
			pt = Eval(u, v, w);
			GVertex vtx(GPoint3(pt[0], pt[1], pt[2]), GVector3(0.0, 0.0, 0.0));
			AddVertex(vtx);			
		}
	}

	// w = wmin 곡면의 정점을 생성하고,
	w = wmin;
	for (int i = 0; i < SNumV; ++i)
	{
		v = vmin + (vmax - vmin) * (double)i / (double)(SNumV - 1);
		for (int j = 0; j < SNumU; ++j)
		{
			u = umin + (umax - umin) * (double)j / (double)(SNumU - 1);
			pt = Eval(u, v, w);
			GVertex vtx(GPoint3(pt[0], pt[1], pt[2]), GVector3(0.0, 0.0, 0.0));
			AddVertex(vtx);			
		}
	}

	// w = wmax 곡면의 정점을 생성하고,
	w = wmax;
	for (int i = 0; i < SNumU; ++i)
	{
		u = umin + (umax - umin) * (double)i / (double)(SNumU - 1);
		for (int j = 0; j < SNumV; ++j)
		{
			v = vmin + (vmax - vmin) * (double)j / (double)(SNumV - 1);
			pt = Eval(u, v, w);
			GVertex vtx(GPoint3(pt[0], pt[1], pt[2]), GVector3(0.0, 0.0, 0.0));
			AddVertex(vtx);			
		}
	}
}

/*!
*	\brief 볼륨의 표면 메쉬의 텍스처 좌표를 생성한다.
*/
void GUcbsVol::CreateMeshTex()
{
}

/*!
*	\brief 볼륨의 표면 메쉬의 삼각형을 생성한다.
*/
void GUcbsVol::CreateMeshFace()
{
	// 삼각형 정보를 지우고,
	Faces.clear();

	int BaseIdx = 0;
	int SNumU = Renderer.NumSampU;
	int SNumV = Renderer.NumSampV;
	int SNumW = Renderer.NumSampW;

	// u = umin 곡면의 삼각형을 생성하고,
	for (int i = 0; i < SNumW - 1; ++i)
	{
		for(int j = 0; j < SNumV - 1; ++j)
		{
			int vidx0 = BaseIdx + i * SNumV + j;
			int vidx1 = vidx0 + 1;
			int vidx2 = vidx1 + SNumV;
			int vidx3 = vidx2 - 1;

			GFace f0(vidx0, vidx2, vidx1, vidx0, vidx2, vidx1, 0);
			GFace f1(vidx0, vidx3, vidx2, vidx0, vidx3, vidx2, 0);

			AddFace(f0);
			AddFace(f1);
		}
	}

	// u = umax 곡면의 삼각형을 생성하고,
	BaseIdx += (SNumW * SNumV);
	for (int i = 0; i < SNumV - 1; ++i)
	{
		for(int j = 0; j < SNumW - 1; ++j)
		{
			int vidx0 = BaseIdx + i * SNumW + j;
			int vidx1 = vidx0 + 1;
			int vidx2 = vidx1 + SNumW;
			int vidx3 = vidx2 - 1;

			GFace f0(vidx0, vidx2, vidx1, vidx0, vidx2, vidx1, 0);
			GFace f1(vidx0, vidx3, vidx2, vidx0, vidx3, vidx2, 0);

			AddFace(f0);
			AddFace(f1);
		}
	}

	// v = vmin 곡면의 삼각형을 생성하고,
	BaseIdx += (SNumW * SNumV);
	for (int i = 0; i < SNumU - 1; ++i)
	{
		for(int j = 0; j < SNumW - 1; ++j)
		{
			int vidx0 = BaseIdx + i * SNumW + j;
			int vidx1 = vidx0 + 1;
			int vidx2 = vidx1 + SNumW;
			int vidx3 = vidx2 - 1;

			GFace f0(vidx0, vidx2, vidx1, vidx0, vidx2, vidx1, 0);
			GFace f1(vidx0, vidx3, vidx2, vidx0, vidx3, vidx2, 0);

			AddFace(f0);
			AddFace(f1);
		}
	}

	// v = vmax 곡면의 삼각형을 생성하고,
	BaseIdx += (SNumU * SNumW);
	for (int i = 0; i < SNumW - 1; ++i)
	{
		for(int j = 0; j < SNumU - 1; ++j)
		{
			int vidx0 = BaseIdx + i * SNumU + j;
			int vidx1 = vidx0 + 1;
			int vidx2 = vidx1 + SNumU;
			int vidx3 = vidx2 - 1;

			GFace f0(vidx0, vidx2, vidx1, vidx0, vidx2, vidx1, 0);
			GFace f1(vidx0, vidx3, vidx2, vidx0, vidx3, vidx2, 0);

			AddFace(f0);
			AddFace(f1);
		}
	}

	// w = wmin 곡면의 삼각형을 생성하고,
	BaseIdx += (SNumU * SNumW);
	for (int i = 0; i < SNumV - 1; ++i)
	{
		for(int j = 0; j < SNumU - 1; ++j)
		{
			int vidx0 = BaseIdx + i * SNumU + j;
			int vidx1 = vidx0 + 1;
			int vidx2 = vidx1 + SNumU;
			int vidx3 = vidx2 - 1;

			GFace f0(vidx0, vidx2, vidx1, vidx0, vidx2, vidx1, 0);
			GFace f1(vidx0, vidx3, vidx2, vidx0, vidx3, vidx2, 0);

			AddFace(f0);
			AddFace(f1);
		}
	}

	// w = wmax 곡면의 삼각형을 생성하고,
	BaseIdx += (SNumU * SNumV);
	for (int i = 0; i < SNumU - 1; ++i)
	{
		for(int j = 0; j < SNumV - 1; ++j)
		{
			int vidx0 = BaseIdx + i * SNumV + j;
			int vidx1 = vidx0 + 1;
			int vidx2 = vidx1 + SNumV;
			int vidx3 = vidx2 - 1;

			GFace f0(vidx0, vidx2, vidx1, vidx0, vidx2, vidx1, 0);
			GFace f1(vidx0, vidx3, vidx2, vidx0, vidx3, vidx2, 0);

			AddFace(f0);
			AddFace(f1);
		}
	}
}

/*!
*	\brief 곡면의 도메인을 반환한다.
*
*	\param min U 방향 도메인의 시작 값이 저장된다.
*/
void GUcbsVol::GetDomain(double *umin, ...)
{
	double *umax, *vmin, *vmax, *wmin, *wmax;
	va_list ap;
	va_start(ap, umin);
	umax = va_arg(ap, double *);
	vmin = va_arg(ap, double *);
	vmax = va_arg(ap, double *);
	wmin = va_arg(ap, double *);
	wmax = va_arg(ap, double *);
	va_end(ap);

	*umin = 0.0;
	*umax = 1.0;
	*vmin = 0.0;
	*vmax = 1.0;
	*wmin = 0.0;
	*wmax = 1.0;
}

/*!
*	\brief	볼륨의 제어점 리스트를 반환한다.
*
*	\return 볼륨의 제어점 리스트를 반환한다.
*/
void GUcbsVol::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
	for (int i = 0; i < (l + 1) * (m + 1) * (n + 1); ++i)
		CtrlPtList.push_back(&P[i]);
}

/*!
*	\brief	볼륨내의 점 V(u, v, w)를 계산
*
*	\param u U-방향 매개변수의 값
*
*	\return 볼륨내의 점 V(u, v, w)의 위치를 계산
*/
GVector4 GUcbsVol::Eval(double u, ...)
{
	double v, w;
	va_list ap;
	va_start(ap, u);
	v = va_arg(ap, double);
	w = va_arg(ap, double);
	va_end(ap);

	// get local parameter values for u and v
	double uu = u * (l - 2);
	double vv = v * (m - 2);
	double ww = w * (n - 2);
	int r = (int)uu;	
	int s = (int)vv;	
	int t = (int)ww;	// P(r, s, t) is the left bottum control points.
	uu = uu - r;	
	vv = vv - s;
	ww = ww - t;	// get the local parameter of u and v.

	if (r == l - 2)	// special case for u is 1.0. 
	{
		r = l - 3;
		uu = 1.0;
	}
	if (s == m - 2)  // special case for v is 1.0.
	{
		s = m - 3;
		vv = 1.0;
	}
	if (t == n - 2)  // special case for w is 1.0.
	{
		t = n - 3;
		ww = 1.0;
	}

	static double basis_u[4], basis_v[4], basis_w[4];

	get_ucbs_basis(uu, basis_u);
	get_ucbs_basis(vv, basis_v);
	get_ucbs_basis(ww, basis_w);

	// 	int uidx = (int)(uu * 1000.0 + 0.5);
	// 	int vidx = (int)(vv * 1000.0 + 0.5);
	// 	int widx = (int)(ww * 1000.0 + 0.5);
	// 	ARR_COPY(basis_u, CBS_BASIS[uidx], 4);
	// 	ARR_COPY(basis_v, CBS_BASIS[vidx], 4);
	// 	ARR_COPY(basis_w, CBS_BASIS[widx], 4);

	GCtrlPt pt;
	for (int i = 0; i < 4; ++i)
	{
		GCtrlPt tmp0;
		for (int j = 0; j < 4; ++j)
		{
			int idx = (r + i) * (m + 1) * (n + 1) + (s + j) * (n + 1) + t;
			tmp0 += basis_v[j] * (basis_w[0] * P[idx] + basis_w[1] * P[idx + 1] + basis_w[2] * P[idx + 2] + basis_w[3] * P[idx + 3]);
		}
		pt += basis_u[i] * tmp0;
	}

	return GVector4(pt[0], pt[1], pt[2], pt[3]);
}

/*!
*	\brief	곡면의 refinement 를 수행한다.
*/
void GUcbsVol::Refine()
{
	int ll = 2 * (l - 1);
	int mm = 2 * (m - 1);
	int nn = 2 * (n - 1);

	int num_ctlpt = (ll + 1) * (mm + 1) * (nn + 1);
	GCtrlPt *Q = new GCtrlPt [num_ctlpt];	// refined new control points.
	for (int i = 0; i < num_ctlpt; ++i)
		Q[i].pMesh = this;

	// compute new control points.
	for (int i = 0; i < ll + 1; i += 2)	// U-방향
	{
		for (int j = 0; j < mm + 1; j += 2)	// V-방향
		{
			for (int k = 0; k < nn + 1; k += 2)	// W-방향
			{
				int nv = n + 1;					// offset to element in next v-direction
				int nu = (n + 1) * (m + 1);		// offset to element in next u-direction

				int idx0 = nu * (i / 2) + nv * (j / 2) + (k / 2);		// P's index
				int idx1 = (nn + 1) * (mm + 1) * i + (nn + 1) * j + k;	// Q's index

				// case 0:
				Q[idx1] = (P[idx0] + P[idx0 + 1] + P[idx0 + nv] + P[idx0 + nv + 1] +
					P[idx0 + nu] + P[idx0 + nu + 1] + P[idx0 + nu + nv] + P[idx0 + nu + nv + 1]) / 8.0;

				// case 1:
				if (k != nn)
				{
					Q[idx1 + 1] = (P[idx0] + P[idx0 + 2] + P[idx0 + nv] + P[idx0 + nv + 2] +
						P[idx0 + nu] + P[idx0 + nu + 2] + P[idx0 + nu + nv] + P[idx0 + nu + nv + 2] +
						6.0 * (P[idx0 + 1] + P[idx0 + nv + 1] + P[idx0 + nu + 1] + P[idx0 + nu + nv + 1])) / 32.0;
				}

				// case 2:
				if (j != mm)
				{
					Q[idx1 + nn + 1] = (P[idx0] + P[idx0 + nv * 2] + P[idx0 + 1] + P[idx0 + nv * 2 + 1] +
						P[idx0 + nu] + P[idx0 + nu + nv * 2] + P[idx0 + nu + 1] + P[idx0 + nu + nv * 2 + 1] +
						6.0 * (P[idx0 + nv] + P[idx0 + nv + 1] + P[idx0 + nu + nv] + P[idx0 + nu + nv + 1])) / 32.0;
				}

				// case 3:
				if (j != mm && k != nn)
				{
					Q[idx1 + nn + 2] = (P[idx0] + P[idx0 + 2] + P[idx0 + 2 * nv] + P[idx0 + 2 * nv + 2] +
						P[idx0 + nu] + P[idx0 + 2 + nu] + P[idx0 + 2 * nv + nu] + P[idx0 + 2 * nv + 2 + nu] +
						6.0 * (P[idx0 + 1] + P[idx0 + nv] + P[idx0 + 2 * nv + 1] + P[idx0 + nv + 2] +
						P[idx0 + 1 + nu] + P[idx0 + nv + nu] + P[idx0 + 2 * nv + 1 + nu] + P[idx0 + nv + 2 + nu]) +
						36.0 * (P[idx0 + nv + 1] + P[idx0 + nu + nv + 1])) / 128.0;
				}

				// case 4:
				if (i != ll)
				{
					Q[idx1 + (nn + 1) * (mm + 1)] = (P[idx0] + P[idx0 + 1] + P[idx0 + nv] + P[idx0 + nv + 1] +
						P[idx0 + 2 * nu] + P[idx0 + 1 + 2 * nu] + P[idx0 + nv + 2 * nu] + P[idx0 + nv + 1 + 2 * nu]+
						6.0 * (P[idx0 + nu] + P[idx0 + nu + 1] + P[idx0 + nu + nv] + P[idx0 + nu + nv + 1])) / 32.0;
				}

				// case 5:
				if (i != ll && k != nn)
				{
					Q[idx1 + (nn + 1) * (mm + 1) + 1] = (P[idx0] + P[idx0 + 2] + P[idx0 + nv] + P[idx0 + nv + 2] +
						P[idx0 + 2 * nu] + P[idx0 + 2 + 2 * nu] + P[idx0 + nv + 2 * nu] + P[idx0 + nv + 2 + 2 * nu] +
						6.0 * (P[idx0 + 1] + P[idx0 + nv + 1] + P[idx0 + nu] + P[idx0 + nu + 2] + P[idx0 + nu + nv] + P[idx0 + nu + nv + 2] + P[idx0 + 2 * nu + 1] + P[idx0 + 2 * nu + nv + 1]) + 
						36.0 * (P[idx0 + nu + 1] + P[idx0 + nu + nv + 1])) / 128.0;
				}

				// case 6:
				if (i != ll && j != mm)
				{
					Q[idx1 + (nn + 1) * (mm + 2)] = (P[idx0] + P[idx0 + 1] + P[idx0 + 2 * nv] + P[idx0 + 2 * nv + 1] +
						P[idx0 + 2 * nu] + P[idx0 + 1 + 2 * nu] + P[idx0 + 2 * nv + 2 * nu] + P[idx0 + 2 * nv + 1 + 2 * nu] +
						6.0 * (P[idx0 + nv] + P[idx0 + nv + 1] + P[idx0 + nu] + P[idx0 + nu + 1] + P[idx0 + nu + 2 * nv] + P[idx0 + nu + 2 * nv + 1] + P[idx0 + 2 * nu + nv] + P[idx0 + 2 * nu + nv + 1]) +
						36.0 * (P[idx0 + nu + nv] + P[idx0 + nu + nv + 1])) / 128.0;
				}

				// case 7:
				if (i != ll && j != mm && k != nn)
				{
					Q[idx1 + (nn + 1) * (mm + 2) + 1] = (P[idx0] + P[idx0 + 2] + P[idx0 + 2 * nv] + P[idx0 + 2 * nv + 2] +
						P[idx0 + 2 * nu] + P[idx0 + 2 + 2 * nu] + P[idx0 + 2 * nv + 2 * nu] + P[idx0 + 2 * nv + 2 + 2 * nu] +
						6.0 * (P[idx0 + 1] + P[idx0 + nv] + P[idx0 + 2 * nv + 1] + P[idx0 + nv + 2] +
						P[idx0 + nu] + P[idx0 + nu + 2] + P[idx0 + 2 * nv + nu] + P[idx0 + 2 * nv + 2 + nu] +
						P[idx0 + 1 + 2 * nu] + P[idx0 + nv + 2 * nu] + P[idx0 + 2 * nv + 1 + 2 * nu] + P[idx0 + nv + 2 + 2 * nu]) +
						36.0 * (P[idx0 + nv + 1] + P[idx0 + nu + 1] + P[idx0 + nu + nv] + P[idx0 + nu + 2 * nv + 1] + P[idx0 + nu + nv + 2] + P[idx0 + 2 * nu + nv + 1]) +
						216.0 * P[idx0 + nu + nv + 1]) / 512.0;
				}				
			}
		}
	}

	_free();
	l = ll;
	m = mm;
	n = nn;
	P = Q;
}
