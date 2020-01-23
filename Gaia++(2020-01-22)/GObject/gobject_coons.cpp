#include "stdafx.h"

/*************************************/
/* Implementation of GCoonsSrf Class */
/*************************************/
IMPLEMENT_GMESH_OBJECT(GCoonsSrf)

/*!
*	\brief 객체가 포함된 메쉬의 포인터를 반환한다.
*
*	\return 객체가 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GCoonsSrf::GetRefMesh()
{
	return this;
}

/*!
*	\brief 객체의 모델링 좌표계를 반환한다.
*
*	\return 월드 좌표계에서 객체의 모델링 좌표계를 반환한다
*/
GTransf GCoonsSrf::GetTransf()
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
void GCoonsSrf::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);
}

/*!
*	\brief	메쉬의 모델링 좌표계를 월드 좌표계로 갱신한다.
*
*	\param RefType 모델링 좌표계의 갱신을 위한 참조 형태(0: 월드 좌표계, 1: 경계상자 좌표계)
*/
void GCoonsSrf::UpdateMC(int RefType)
{
}

/*!
*	\brief	곡선의 정점의 위치를 갱신한다.
*/
void GCoonsSrf::UpdateMesh()
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
		UpdateBndBox(false);
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
void GCoonsSrf::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);
}

/*!
*	\brief	기본 생성자
*/
GCoonsSrf::GCoonsSrf()
: GMesh("noname", GTransf(), FACE_QUAD, "")
{
	for (int i = 0; i < 4; ++i)
		pCrvs[i] = NULL;
}

/*!
*	\brief	Coons 곡면을 생성한다.
*	
*	\param _Name 곡면의 이름
*	\param _MC 곡면의 모델링 좌표계
*	\param _p 곡면의 U-방향 차수
*	\param _m 곡면의 U-방향 제어점의 마지막 인덱스
*	\param _U 곡면의 U-방향 knot 벡터
*	\param _q 곡면의 V-방향 차수
*	\param _n 곡면의 V-방향 제어점의 마지막 인덱스
*	\param _V 곡면의 V-방향 knot 벡터
*	\param _P 곡면의 경계 제어점
*	\param _ParentName 부모 메쉬 곡면의 이름
*/
GCoonsSrf::GCoonsSrf(std::string _Name, GTransf _MC, int _p, int _m, double *_U, int _q, int _n, double *_V, GVector4 *_P, std::string _ParentName)
: GMesh(_Name, _MC, FACE_QUAD, _ParentName)
{
	// 경계 곡선의 제어점 배열을 생성하고,
	GCtrlPt *P[4];
	P[0] = new GCtrlPt [_m + 1];
	P[1] = new GCtrlPt [_n + 1];
	P[2] = new GCtrlPt [_m + 1];
	P[3] = new GCtrlPt [_n + 1];

	// 제어점 배열을 초기화 한다.
	for (int i = 0; i < _m + 1; ++i)
	{
		P[0][i] = _P[i];
		P[0][i].pMesh = this;

		P[2][i] = _P[_m + _n + i];
		P[2][i].pMesh = this;
	}	
	for (int i = 0; i < _n + 1; ++i)
	{
		P[1][i] = _P[_m + i];
		P[1][i].pMesh = this;

		P[3][i] = (i == _n) ? _P[0] : _P[2 * _m + _n + i];
		P[3][i].pMesh = this;
	}

	// 경계 곡선을 생성한다.
	pCrvs[0] = new GNurbsCrv("Crv0", _MC, _p, _m, _U, P[0], false, Name);
	pCrvs[1] = new GNurbsCrv("Crv1", _MC, _q, _n, _V, P[1], false, Name);
	pCrvs[2] = new GNurbsCrv("Crv2", _MC, _p, _m, _U, P[2], false, Name);
	pCrvs[3] = new GNurbsCrv("Crv3", _MC, _q, _n, _V, P[3], false, Name);

	// 제어점 배열을 제거한다.
	for (int i = 0; i < 4; ++i)
		delete [] P[i];
}

/*!
*	\brief	경계 곡선을 이용하여 Coons 곡면을 생성한다.
*	\note 경계 곡선은 반시계방향으로 정렬된 것으로 가정한다.
*	
*	\param _Name 곡면의 이름
*	\param _MC 곡면의 모델링 좌표계
*	\param pCrv0 경계 곡선 S(u, 0)
*	\param pCrv1 경계 곡선 S(1, v)
*	\param pCrv2 경계 곡선 S(u, 1)
*	\param pCrv3 경계 곡선 S(0, v)
*	\param _ParentName 부모 메쉬 곡면의 이름
*/
GCoonsSrf::GCoonsSrf(std::string _Name, GTransf _MC, GNurbsCrv *pCrv0, GNurbsCrv *pCrv1, GNurbsCrv *pCrv2, GNurbsCrv *pCrv3, std::string _ParentName)
: GMesh(_Name, _MC, FACE_QUAD, _ParentName)
{
	pCrvs[0] = pCrv0->CopyObject();
	pCrvs[1] = pCrv1->CopyObject();
	pCrvs[2] = pCrv2->CopyObject();
	pCrvs[3] = pCrv3->CopyObject();
}

/*!
*	\brief	복사 생성자
*
*	\param cpy 복사될 객체
*/
GCoonsSrf::GCoonsSrf(const GCoonsSrf &cpy)
: GMesh(cpy)
{
	// 경계 곡선을 복사한다.
	pCrvs[0] = cpy.pCrvs[0]->CopyObject();
	pCrvs[1] = cpy.pCrvs[1]->CopyObject();
	pCrvs[2] = cpy.pCrvs[2]->CopyObject();
	pCrvs[3] = cpy.pCrvs[3]->CopyObject();
}

/*!
*	\brief	소멸자
*/
GCoonsSrf::~GCoonsSrf()
{
	// 경계 곡선을 제거한다.
	for (int i = 0; i < 4; ++i)
	{
		if (pCrvs[i] != NULL)
			delete pCrvs[i];
	}
}

/*!
*	\brief	대입 연산자
*
*	\param rhs 대입될 객체
*
*	\return 대입된 자신을 반환한다.
*/
GCoonsSrf &GCoonsSrf::operator =(const GCoonsSrf &rhs)
{
	// 메쉬를 복사하고,
	GMesh::operator =(rhs);

	// 기존의 경계 곡선을 제거하고,
	for (int i = 0; i < 4; ++i)
	{
		if (pCrvs[i] != NULL)
			delete pCrvs[i];
	}

	// 새로운 경계 곡선을 복사한다.
	pCrvs[0] = rhs.pCrvs[0]->CopyObject();
	pCrvs[1] = rhs.pCrvs[1]->CopyObject();
	pCrvs[2] = rhs.pCrvs[2]->CopyObject();
	pCrvs[3] = rhs.pCrvs[3]->CopyObject();

	// 대입된 자신을 반환한다.
	return *this;
}

/*!
*	\brief 객체를 렌더링한다.
*
*	\param bSubMesh 서브 메쉬의 렌더링 여부(선택 모드에서는 false로 설정)
*/
void GCoonsSrf::Render(bool bSubMesh)
{
	// 부모 클래스 함수를 호출한 후,
	GMesh::Render(bSubMesh);

	// 월드 좌표계에서
	glPushMatrix();
	{
		// 객체의 모델링 좌표계로의 변환하고,
		glMultMatrixd(cast_mat(MC).M);

		// 곡면의 제어점을 렌더링할 경우,
		if (Renderer.RenderType & RENDER_CTRL_PT)
		{
			double size = Renderer.CtrlPtSize;

			// 재질을 설정하고,
			GMaterial Mtl;
			Mtl.SetAmbient(0.7f, 0.7f, 0.7f);
			Mtl.SetDiffuse(0.7f, 0.7f, 0.7f);
			Mtl.SetSpecular(0.7f, 0.7f, 0.7f);
			Mtl.SetAlpha(1.0f);
			Mtl.Enable(false);

			// U 방향 곡선의 제어점을 렌더링하고,
			int m = pCrvs[0]->n;
			for (int i = 1; i < m; ++i)
			{
				glPushMatrix();
				glTranslated(pCrvs[0]->P[i][0], pCrvs[0]->P[i][1], pCrvs[0]->P[i][2]);
				glutSolidSphere(size, 10, 10);
				glPopMatrix();

				glPushMatrix();
				glTranslated(pCrvs[2]->P[i][0], pCrvs[2]->P[i][1], pCrvs[2]->P[i][2]);
				glutSolidSphere(size, 10, 10);
				glPopMatrix();
			}

			// V 방향 곡선의 제어점을 렌더링하고,
			int n = pCrvs[1]->n;
			for (int i = 0; i < n + 1; ++i)
			{
				glPushMatrix();
				glTranslated(pCrvs[1]->P[i][0], pCrvs[1]->P[i][1], pCrvs[1]->P[i][2]);
				glutSolidSphere(size, 10, 10);
				glPopMatrix();

				glPushMatrix();
				glTranslated(pCrvs[3]->P[i][0], pCrvs[3]->P[i][1], pCrvs[3]->P[i][2]);
				glutSolidSphere(size, 10, 10);
				glPopMatrix();
			}

			// 조명을 비활성화 하고,
			glDisable(GL_LIGHTING);
			glLineWidth(2.0);

			// 왼쪽 제어 메쉬를 렌더링하고,
			glColor3d(1.0, 0.0, 0.0);
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < m + 1; i++)
				glVertex3d(pCrvs[0]->P[i][0], pCrvs[0]->P[i][1], pCrvs[0]->P[i][2]);
			glEnd();

			// 오른쪽 제어 메쉬를 렌더링하고,
			glColor3d(0.0, 0.0, 1.0);
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < m + 1; i++)
				glVertex3d(pCrvs[2]->P[i][0], pCrvs[2]->P[i][1], pCrvs[2]->P[i][2]);
			glEnd();

			// 아래쪽 제어 메쉬를 렌더링하고,
			glColor3d(0.0, 1.0, 0.0);
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < n + 1; i++)
				glVertex3d(pCrvs[1]->P[i][0], pCrvs[1]->P[i][1], pCrvs[1]->P[i][2]);
			glEnd();

			// 위쪽 제어 메쉬를 렌더링하고,
			glColor3d(1.0, 1.0, 0.0);
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < n + 1; i++)
				glVertex3d(pCrvs[3]->P[i][0], pCrvs[3]->P[i][1], pCrvs[3]->P[i][2]);
			glEnd();

			// 조명을 활성화 한다.
			glLineWidth(1.0);
			glEnable(GL_LIGHTING);
		}
	}
	glPopMatrix();
}

/*!
*	\brief	객체가 정의된 파라미터 도메인을 반환한다.
*
*	\param umin 도메인의 시작 값이 저장된다.
*	\param umax 도메인의 끝 값이 저장된다.
*/
void GCoonsSrf::GetDomain(double *umin, ...)
{
	double *umax, *vmin, *vmax;
	va_list ap;
	va_start(ap, umin);
	umax = va_arg(ap, double *);
	vmin = va_arg(ap, double *);
	vmax = va_arg(ap, double *);
	va_end(ap);

	double u0, u1, v0, v1;
	pCrvs[0]->GetDomain(&u0, &u1);
	pCrvs[1]->GetDomain(&v0, &v1);

	*umin = u0;
	*umax = u1;
	*vmin = v0;
	*vmax = v1;
}

/*!
*	\brief	객체의 제어점 리스트를 반환한다.
*
*	\param CtrlPtList 객체의 제어점 리스트가 저장됨
*
*	\return 객체의 제어점 리스트를 반환한다.
*/
void GCoonsSrf::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
	CtrlPtList.clear();

	int m = pCrvs[0]->n;
	int n = pCrvs[1]->n;

	for (int i = 0; i < m + 1; ++i)
		CtrlPtList.push_back(&(pCrvs[0]->P[i]));

	for (int i = 0; i < n + 1; ++i)
		CtrlPtList.push_back(&(pCrvs[1]->P[i]));

	for (int i = 0; i < m + 1; ++i)
		CtrlPtList.push_back(&(pCrvs[2]->P[i]));

	for (int i = 0; i < n + 1; ++i)
		CtrlPtList.push_back(&(pCrvs[3]->P[i]));
}

/*!
*	\brief 객체의 렌더링을 위한 메쉬(정점의 리스트)를 생성한다.
*/
void GCoonsSrf::CreateMesh()
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
*	\brief	객체의 렌더링을 위한 정점의 리스트를 생성한다.
*/
void GCoonsSrf::CreateMeshVert()
{
	// 기존의 정점을 지우고,
	Verts.clear();

	// 정의역을 계산하고,
	double umin, umax, vmin, vmax;
	GetDomain(&umin, &umax, &vmin, &vmax);

	// 샘플링 정보를 얻어와서, 
	int NumSampU = Renderer.NumSampU;
	int NumSampV = Renderer.NumSampV;

	// 곡면위의 정점을 계산한다.
	GVector4 pt;
	for (int i = 0; i < NumSampU; ++i)
	{
		double u = umin + (umax - umin) * (double)i / (double)(NumSampU - 1);
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
*	\brief	객체의 렌더링을 위한 텍스처 리스트를 생성한다.
*/
void GCoonsSrf::CreateMeshTex()
{
	// 기존의 텍셀 정보를 지우고,
	Texs.clear();

	// 샘플링 정보를 얻어와서, 
	int NumSampU = Renderer.NumSampU;
	int NumSampV = Renderer.NumSampV;

	// 텍스처 좌표를 생성하고 저장함
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
*	\brief	객체의 렌더링을 위한 삼각형 리스트를 생성한다.
*/
void GCoonsSrf::CreateMeshFace()
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
*	\brief	객체를 구성하는 정점을 계산한다.
*
*	\param u 매개변수의 값
*
*	\return 계산된 점을 반환한다
*/
GVector4 GCoonsSrf::Eval(double u, ...)
{
	// 매개변수 (u, v)를 구하고,
	double v;
	va_list ap;
	va_start(ap, u);
	v = va_arg(ap, double);
	va_end(ap);

	GVector4 p, q;
	// U 방향 경계 곡선으로 정의되는 선직면(ruled surface)의 정점을 계산하고,
	p = (1.0 - v) * (pCrvs[0]->Eval(u)) + v * (pCrvs[2]->Eval(1.0 - u));

	// V 방향 경계 곡선으로 정의되는 선직면(ruled surface)의 정점을 계산하고,
	q = (1.0 - u) * (pCrvs[3]->Eval(1.0 - v)) + u * (pCrvs[1]->Eval(v));

	// 4개의 코너 정점으로 정의되는 Bilinear 곡면의 정점을 계산하고,
	GVector4 p0, p1, p2, p3, r;
	p0 = pCrvs[0]->P[0];
	p1 = pCrvs[1]->P[0];
	p2 = pCrvs[2]->P[0];
	p3 = pCrvs[3]->P[0];
	r = (1.0 - v) * ((1.0 - u) * p0 + u * p1) + v * ((1.0 - u) * p3 + u * p2);

	// 최종적인 Coons 곡면의 정점을 계산하여 반환한다.
	return p + q - r;
}

/*!
*	\brief U 방향 knot refinement를 수행함
*/
void GCoonsSrf::KnotRefineU()
{
	pCrvs[0]->KnotRefine();
	pCrvs[2]->KnotRefine();
}

/*!
*	\brief V 방향 knot refinement를 수행함
*/
void GCoonsSrf::KnotRefineV()
{
	pCrvs[1]->KnotRefine();
	pCrvs[3]->KnotRefine();
}

/*************************************/
/* Implementation of GCoonsMesh Class */
/*************************************/
IMPLEMENT_GMESH_OBJECT(GCoonsMesh)

/*!
*	\brief 객체가 포함된 메쉬의 포인터를 반환한다.
*
*	\return 객체가 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GCoonsMesh::GetRefMesh()
{
	return this;
}

/*!
*	\brief 객체의 모델링 좌표계를 반환한다.
*
*	\return 월드 좌표계에서 객체의 모델링 좌표계를 반환한다
*/
GTransf GCoonsMesh::GetTransf()
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
void GCoonsMesh::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);
}

/*!
*	\brief	메쉬의 모델링 좌표계를 월드 좌표계로 갱신한다.
*
*	\param RefType 모델링 좌표계의 갱신을 위한 참조 형태(0: 월드 좌표계, 1: 경계상자 좌표계)
*/
void GCoonsMesh::UpdateMC(int RefType)
{
}

/*!
*	\brief	곡선의 정점의 위치를 갱신한다.
*/
void GCoonsMesh::UpdateMesh()
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
		UpdateBndBox(false);
		break;

	case MODIFIED_CTLPT_POS:
		CreateMeshVert();
		UpdateBndBox(false);
		UpdateBufVertex();
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
void GCoonsMesh::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);
}

/*!
*	\brief	기본 생성자
*/
GCoonsMesh::GCoonsMesh()
: GMesh("noname", GTransf(), FACE_QUAD, "")
{
	pRefMesh = NULL;
}

/*!
*	\brief	Coons 메쉬를 생성한다.
*	
*	\param _Name 메쉬의 이름
*	\param _pRefMesh 메쉬가 정의된 참조 메쉬
*/
GCoonsMesh::GCoonsMesh(std::string _Name, GMesh *_pRefMesh)
: GMesh(_Name, _pRefMesh->MC, FACE_QUAD, _pRefMesh->Name)
{
	pRefMesh = _pRefMesh;	
}

/*!
*	\brief	복사 생성자
*
*	\param cpy 복사될 객체
*/
GCoonsMesh::GCoonsMesh(const GCoonsMesh &cpy)
: GMesh(cpy)
{
	// Coons 메쉬가 정의되는 참조 메쉬를 복사한다.
	pRefMesh = cpy.pRefMesh->CopyObject();

	// Coons 메쉬의 코너 정점들의 리스트를 복사한다.
	VertIdx = cpy.VertIdx;
}

/*!
*	\brief	소멸자
*/
GCoonsMesh::~GCoonsMesh()
{
}

/*!
*	\brief	대입 연산자
*
*	\param rhs 대입될 객체
*
*	\return 대입된 자신을 반환한다.
*/
GCoonsMesh &GCoonsMesh::operator =(const GCoonsMesh &rhs)
{
	// 메쉬를 복사하고,
	GMesh::operator =(rhs);

	// Coons 메쉬가 정의되는 참조 메쉬를 복사한다.
	pRefMesh = rhs.pRefMesh->CopyObject();

	// Coons 메쉬의 코너 정점들의 리스트를 복사한다.
	VertIdx = rhs.VertIdx;

	// 대입된 자신을 반환한다.
	return *this;
}

/*!
*	\brief 객체를 렌더링한다.
*
*	\param bSubMesh 서브 메쉬의 렌더링 여부(선택 모드에서는 false로 설정)
*/
void GCoonsMesh::Render(bool bSubMesh)
{
	// 부모 클래스 함수를 호출한 후,
	GMesh::Render(bSubMesh);

// 	// 월드 좌표계에서
// 	glPushMatrix();
// 	{
// 		// 객체의 모델링 좌표계로의 변환하고,
// 		glMultMatrixd(cast_mat(MC).M);
// 
// 		// 조명을 비활성화 하고,
// 		glDisable(GL_LIGHTING);
// 		glLineWidth(5.0);
// 
// 		// 생성 중인 Coons 패치의 경계 경로를 렌더링 한다.
// 		for (int i = 0; i < 4; ++i)
// 		{
// 			int NumPt = BndryPaths[i].size();			
// 			glBegin(GL_LINE_STRIP);
// 			for (int j = 0; j < NumPt; ++j)
// 			{
// 				double t = (double)j / (NumPt - 1);
// 				unsigned char r, g, b;
// 				get_color_gradient_from_rainbow(t, r, g, b);
// 				glColor3ub(r, g, b);
// 				glVertex3dv(BndryPaths[i][j].V);
// 			}
// 			glEnd();
// 		}
// 
// 		// 조명을 활성화 한다.
// 		glEnable(GL_LIGHTING);
// 		glLineWidth(1.0);
// 	}
// 	glPopMatrix();
}

/*!
*	\brief	객체가 정의된 파라미터 도메인을 반환한다.
*
*	\param umin 도메인의 시작 값이 저장된다.
*/
void GCoonsMesh::GetDomain(double *umin, ...)
{
	double *umax;
	va_list ap;
	va_start(ap, umin);
	umax = va_arg(ap, double *);
	va_end(ap);

	*umin = 0.0;
	*umax = 1.0;
}

/*!
*	\brief	객체의 제어점 리스트를 반환한다.
*
*	\param CtrlPtList 객체의 제어점 리스트가 저장됨
*
*	\return 객체의 제어점 리스트를 반환한다.
*/
void GCoonsMesh::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
}

/*!
*	\brief 객체의 렌더링을 위한 메쉬(정점의 리스트)를 생성한다.
*/
void GCoonsMesh::CreateMesh()
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
*	\brief	객체의 렌더링을 위한 정점의 리스트를 생성한다.
*/
void GCoonsMesh::CreateMeshVert()
{
	// 기존의 정점을 지우고, 
	Verts.clear();

	// 각 방향 샘플링 수를 구하고,
	int su = Renderer.NumSampU;
	int sv = Renderer.NumSampV;

	// 각각의 Coons 곡면에 대하여,
	int NumSrf = SrfList.size();
	for (int i = 0; i < NumSrf; ++i)
	{
		// 샘플링 정보를 얻어와서, 
		int NumSampU = Renderer.NumSampU;
		int NumSampV = Renderer.NumSampV;

		// 곡면위의 정점을 계산한다.
		GVector4 pt;
		for (int j = 0; j < su; ++j)
		{
			double u = (double)j / (double)(su - 1);
			for (int k = 0; k < sv; ++k)
			{
				double v = (double)k / (double)(sv - 1);
				pt = SrfList[i]->Eval(u, v);
				GVertex vtx(GPoint3(pt[0], pt[1], pt[2]), GVector3(0.0, 0.0, 0.0));
				AddVertex(vtx);
			}
		}
	}
}

/*!
*	\brief	객체의 렌더링을 위한 텍스처 리스트를 생성한다.
*/
void GCoonsMesh::CreateMeshTex()
{
	// 기존의 텍셀 정보를 지우고,
	Texs.clear();
}

/*!
*	\brief	객체의 렌더링을 위한 삼각형 리스트를 생성한다.
*/
void GCoonsMesh::CreateMeshFace()
{
	// 삼각형 정보를 지우고,
	Faces.clear();

	// 샘플링 정보를 얻어와서, 
	int NumSampU = Renderer.NumSampU;
	int NumSampV = Renderer.NumSampV;

	// 삼각형 정보를 생성한다.
	int NumSrf = SrfList.size();
	for (int k = 0; k < NumSrf; ++k)
	{
		int baseidx = k * NumSampU * NumSampV;
		for (int i = 0; i < NumSampU - 1; ++i)
		{
			for(int j = 0; j < NumSampV - 1; ++j)
			{
				int vidx0 = i * NumSampV + j + baseidx;
				int vidx1 = vidx0 + NumSampV;
				int vidx2 = vidx1 + 1;
				int vidx3 = vidx0 + 1;

				GFace f0(vidx0, vidx1, vidx2);
				GFace f1(vidx0, vidx2, vidx3);
				AddFace(f0);				
				AddFace(f1);
			}
		}
	}
}

/*!
*	\brief	객체를 구성하는 정점을 계산한다.
*
*	\param u 매개변수의 값
*
*	\return 계산된 점을 반환한다
*/
GVector4 GCoonsMesh::Eval(double u, ...)
{
	// 매개변수 (u, v)를 구하고,
	double v;
	va_list ap;
	va_start(ap, u);
	v = va_arg(ap, double);
	va_end(ap);

	return GVector4();
}

/*!
*	\brief Coons 패치를 구성하는 코너 정점의 인덱스를 추가한다.
*
*	\param vidx 코너 정점의 인덱스
*/
void GCoonsMesh::AddVertIdx(int vidx)
{
	// 선택된 경계 정점의 인덱스를 추가하고,
	VertIdx.push_back(vidx);

	if (VertIdx.size() == 4)
	{
		// 4정점의 인덱스와
		int vidx0 = VertIdx[0];
		int vidx1 = VertIdx[1];
		int vidx2 = VertIdx[2];
		int vidx3 = VertIdx[3];
		
		// 4개의 정점을 구하고,
		std::vector<GPoint3> Path;
		GVertex *P = &(pRefMesh->Verts[vidx0]);
		GVertex *Q = &(pRefMesh->Verts[vidx1]);
		GVertex *R = &(pRefMesh->Verts[vidx2]);
		GVertex *S = &(pRefMesh->Verts[vidx3]);

		GNurbsCrv *pCrv0, *pCrv1, *pCrv2, *pCrv3;
		if (vidx0 < vidx1)
		{
			::get_geodesic_paths(pRefMesh, P, Q, Path);
			int n = MAX(3, Path.size() / 3);
			pCrv0 = get_gnurbs_crv_approx(3, n, Path, PARAM_CHORD_LEN, KNOT_APPROX);
		}
		else
		{
			::get_geodesic_paths(pRefMesh, Q, P, Path);
			int n = MAX(3, Path.size() / 3);
			GNurbsCrv *tmp = get_gnurbs_crv_approx(3, n, Path, PARAM_CHORD_LEN, KNOT_APPROX);
			pCrv0 = get_gnurbs_crv_reverse(tmp);
			delete tmp;
		}
		
		if (vidx1 < vidx2)
		{
			::get_geodesic_paths(pRefMesh, Q, R, Path);
			int n = MAX(3, Path.size() / 3);
			pCrv1 = get_gnurbs_crv_approx(3, n, Path, PARAM_CHORD_LEN, KNOT_APPROX);
		}
		else
		{
			::get_geodesic_paths(pRefMesh, R, Q, Path);
			int n = MAX(3, Path.size() / 3);
			GNurbsCrv *tmp = get_gnurbs_crv_approx(3, n, Path, PARAM_CHORD_LEN, KNOT_APPROX);
			pCrv1 = get_gnurbs_crv_reverse(tmp);
			delete tmp;
		}

		if (vidx2 < vidx3)
		{
			::get_geodesic_paths(pRefMesh, R, S, Path);
			int n = MAX(3, Path.size() / 3);
			pCrv2 = get_gnurbs_crv_approx(3, n, Path, PARAM_CHORD_LEN, KNOT_APPROX);
		}
		else
		{
			::get_geodesic_paths(pRefMesh, S, R, Path);
			int n = MAX(3, Path.size() / 3);
			GNurbsCrv *tmp = get_gnurbs_crv_approx(3, n, Path, PARAM_CHORD_LEN, KNOT_APPROX);
			pCrv2 = get_gnurbs_crv_reverse(tmp);
			delete tmp;
		}		

		if (vidx3 < vidx0)
		{
			::get_geodesic_paths(pRefMesh, S, P, Path);
			int n = MAX(3, Path.size() / 3);
			pCrv3 = get_gnurbs_crv_approx(3, n, Path, PARAM_CHORD_LEN, KNOT_APPROX);
		}
		else
		{
			::get_geodesic_paths(pRefMesh, P, S, Path);
			int n = MAX(3, Path.size() / 3);
			GNurbsCrv *tmp = get_gnurbs_crv_approx(3, n, Path, PARAM_CHORD_LEN, KNOT_APPROX);
			pCrv3 = get_gnurbs_crv_reverse(tmp);
			delete tmp;
		}
		
		std::string Name = create_mesh_name("CoonsSrf");
		GCoonsSrf *pSrf = new GCoonsSrf(Name, GTransf(), pCrv0, pCrv1, pCrv2, pCrv3, this->Name);
		SrfList.push_back(pSrf);
		CreateMesh();
		
		delete pCrv0;
		delete pCrv1;
		delete pCrv2;
		delete pCrv3;

		// 선택된 정점의 인덱스를 삭제한다.
		VertIdx.clear();		
	}
}

/*!
*	\brief Coons 패치를 구성하는 코너 정점의 인덱스를 추가한다.
*
*	\param vidx 코너 정점의 인덱스
*
*	\return 생성된 GeoMesh를 반환한다.
*/
GMesh *GCoonsMesh::AddVertIdx2(int vidx)
{
	// 선택된 경계 정점의 인덱스를 추가하고,
	VertIdx.push_back(vidx);

	// 만약 마지막 정점의 인덱스가 추가되었다면,
	if (VertIdx.size() == 4)
	{
		// 4개의 정점의 인덱스와
		int vidx0 = VertIdx[0];
		int vidx1 = VertIdx[1];
		int vidx2 = VertIdx[2];
		int vidx3 = VertIdx[3];

		// 4개의 정점을 구하고,
		GVertex *P = &(pRefMesh->Verts[vidx0]);
		GVertex *Q = &(pRefMesh->Verts[vidx1]);
		GVertex *R = &(pRefMesh->Verts[vidx2]);
		GVertex *S = &(pRefMesh->Verts[vidx3]);

		// 정점 P를 기준으로 측지 극좌표를 계산한다.
		std::vector<double> Distances;
		std::vector<double> Angles;
		double MaxRadius = dist(P->P, R->P) * 1.5;
		::get_geodesic_polar_coords(pRefMesh, P, Distances, Angles, MaxRadius);

		// 영역내의 삼각형들로 2차원 메쉬를 생성한다.
		GMesh *pSubMesh = new GMesh("SubMesh", GTransf(), FACE_TRI, "");
		std::map<int, int> FaceIdxMap;

		int NumFace = pRefMesh->GetNumFace();
		for (int i = 0; i < NumFace; ++i)
		{
			int vidx0 = pRefMesh->Faces[i].vIdx[0];
			int vidx1 = pRefMesh->Faces[i].vIdx[1];
			int vidx2 = pRefMesh->Faces[i].vIdx[2];

			// 영역내의 삼각형이라면,
			if (Distances[vidx0] < 100000.0 && Distances[vidx1] < 100000.0 && Distances[vidx2] < 100000.0)
			{
				GPoint3 p0(Distances[vidx0] * cos(Angles[vidx0]), Distances[vidx0] * sin(Angles[vidx0]), 0.0);
				GPoint3 p1(Distances[vidx1] * cos(Angles[vidx1]), Distances[vidx1] * sin(Angles[vidx1]), 0.0);
				GPoint3 p2(Distances[vidx2] * cos(Angles[vidx2]), Distances[vidx2] * sin(Angles[vidx2]), 0.0);

				// 정점을 추가하고,
				pSubMesh->AddVertex(GVertex(p0));
				pSubMesh->AddVertex(GVertex(p1));
				pSubMesh->AddVertex(GVertex(p2));

				// 삼각형을 추가하고,
				int NumVert = pSubMesh->GetNumVert();
				pSubMesh->AddFace(GFace(NumVert - 3, NumVert - 2, NumVert - 1));

				// 삼각형의 대응 정보를 저장한다.
				FaceIdxMap[pSubMesh->GetNumFace() - 1] = i;
			}
		}
		pSubMesh->UpdateBndBox(false);

		// 2D 탐색을 효율적으로 하기 위한 QuadTree를 생성한다.
		GQTree QuadTree(pSubMesh, 7, 10);

		// 계산된 측지 극좌표를 이용하여, 2차원 평면에서 PQ, SR의 직선 식을 계산한다.
		double r0 = Distances[vidx0];
		double r1 = Distances[vidx1];
		double r2 = Distances[vidx2];
		double r3 = Distances[vidx3];
		double theta0 = Angles[vidx0];		
		double theta1 = Angles[vidx1];		
		double theta2 = Angles[vidx2];		
		double theta3 = Angles[vidx3];
		GLine l_PQ(GPoint3(r0 * cos(theta0), r0 * sin(theta0), 0.0), GPoint3(r1 * cos(theta1), r1 * sin(theta1), 0.0));
		GLine l_SR(GPoint3(r3 * cos(theta3), r3 * sin(theta3), 0.0), GPoint3(r2 * cos(theta2), r2 * sin(theta2), 0.0));

		// 측지선 메쉬를 생성한다.
		std::string Name = create_mesh_name("GeoMesh");
		GMesh *pGeoMesh = new GMesh(Name, GTransf(), FACE_QUAD, this->Name);
		pGeoMesh->Renderer.RenderType = RENDER_POLYGON | RENDER_WIRE | RENDER_BOUNDARY;
		
		// 정점의 리스트를 생성한다.
		int snumu = Renderer.NumSampU;
		int snumv = Renderer.NumSampV;
		for (int i = 0; i < snumu; ++i)
		{
			// U 방향으로 ISO 직선을 구하여,
			double u = (double) i / (double)(snumu - 1);
			GLine tmp(l_PQ.Eval(u), l_SR.Eval(u));

			for (int j = 0; j < snumv; ++j)
			{
				// V 방향으로 직선위의 정점을 계산하고,
				double v = (double) j / (double)(snumv - 1);
				GPoint3 pt = tmp.Eval(v);
				GVector2 p = GVector2(pt[0], pt[1]);

				// 계산된 정점 p가 포함된 삼각형의 인덱스와 barycentric 좌표를 계산하고,
				double alpha, beta;
				int fidx = QuadTree.FindFace(p, &alpha, &beta);
				if (fidx != -1)
				{
					fidx = FaceIdxMap[fidx];
					int vidx0 = pRefMesh->Faces[fidx].vIdx[0];
					int vidx1 = pRefMesh->Faces[fidx].vIdx[1];
					int vidx2 = pRefMesh->Faces[fidx].vIdx[2];

					// 3차원 객체의 좌표로 복원하여 측지선 메쉬에 추가한다.
					GPoint3 p0 = pRefMesh->Verts[vidx0].P;
					GPoint3 p1 = pRefMesh->Verts[vidx1].P;
					GPoint3 p2 = pRefMesh->Verts[vidx2].P;
					GPoint3 p3 = p0 + alpha * (p1 - p0) + beta * (p2 - p0);
					pGeoMesh->AddVertex(GVertex(p3, GVector3()));
				}
			}
		}

		// 삼각형 리스트를 생성한다.
		for (int i = 0; i < snumu - 1; ++i)
		{
			for (int j = 0; j < snumv - 1; ++j)
			{
				int vidx0 = i * snumv + j;
				int vidx1 = vidx0 + snumv;
				int vidx2 = vidx1 + 1;
				int vidx3 = vidx0 + 1;

				GFace f0(vidx0, vidx1, vidx2, vidx0, vidx1, vidx2, -1);
				GFace f1(vidx0, vidx2, vidx3, vidx0, vidx2, vidx3, -1);

				pGeoMesh->AddFace(f0);
				pGeoMesh->AddFace(f1);
			}
		}
		
		// 선택된 영역의 메쉬를 제거한다.
		delete pSubMesh;

		// 선택된 정점의 인덱스를 삭제한다.
		VertIdx.clear();

		// 생성된 측지 메쉬를 반환한다.
		return pGeoMesh;
	}
	else
		return NULL;
}

