#include "stdafx.h"

/************************/
/* GGeoPath 클래스 구현 */
/************************/
IMPLEMENT_GMESH_OBJECT(GGeoPath)

/*!
*	\brief 객체가 포함된 메쉬의 포인터를 반환한다.
*
*	\return 객체가 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GGeoPath::GetRefMesh()
{
	return this;
}

/*!
*	\brief 객체의 모델링 좌표계를 반환한다.
*
*	\return 월드 좌표계에서 객체의 모델링 좌표계를 반환한다
*/
GTransf GGeoPath::GetTransf()
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
void GGeoPath::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);
}

/*!
*	\brief	메쉬의 모델링 좌표계를 월드 좌표계로 갱신한다.
*
*	\param RefType 모델링 좌표계의 갱신을 위한 참조 형태(0: 월드 좌표계, 1: 경계상자 좌표계)
*/
void GGeoPath::UpdateMC(int RefType)
{
	GMesh::UpdateMC(RefType);
}

/*!
*	\brief 경로의 정점의 위치를 갱신한다.
*/
void GGeoPath::UpdateMesh()
{
	switch (ModifiedFlag)
	{
	case MODIFIED_VERT_POS:
		break;

	case MODIFIED_MTL:
		break;

	case MODIFIED_INTPT_POS:
		break;

	case MODIFIED_CTLPT_POS:
		break;

	case MODIFIED_VERT_NUM:
	case MODIFIED_SAMPLE_NUM:
		CreateMesh();
		break;
	}

	ModifiedFlag = MODIFIED_NONE;
}

/*!
*	\brief 선택된 객체를 렌더링 한다.
*/
void GGeoPath::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);
}

/*!
*	\brief	기본 생성자
*/
GGeoPath::GGeoPath()
: GMesh("noname", GTransf(), FACE_TRI, "")
{
	pRefMesh = NULL;
	EditPathVertIdx = -1;
}

/*!
*	\brief	생성자
*
*	\param _Name 객체의 이름
*	\param _MC 객체의 모델링 좌표계
*	\param _pRefMesh 경로가 생성될 메쉬 객체
*	\param _ParentName 곡선의 부모 메쉬의 이름
*/
GGeoPath::GGeoPath(std::string _Name, GTransf _MC, GMesh *_pRefMesh, std::string _ParentName = "")
: GMesh(_Name, _MC, FACE_TRI, _ParentName)
{
	pRefMesh = _pRefMesh;
	EditPathVertIdx = -1;
}

/*!
*	\brief	복사 생성자
*
*	\param cpy 복사될 객체
*/
GGeoPath::GGeoPath(const GGeoPath &cpy)
: GMesh(cpy)
{
	pRefMesh = cpy.pRefMesh;
	PathVertIdx = cpy.PathVertIdx;
	Paths = cpy.Paths;
	EditPathVertIdx = cpy.EditPathVertIdx;
}

/*!
*	\brief	소멸자
*/
GGeoPath::~GGeoPath()
{
}

/*!
*	\brief	대입 연산자
*
*	\param rhs 대입될 객체
*
*	\return 대입된 자신을 반환한다.
*/
GGeoPath &GGeoPath::operator =(const GGeoPath &rhs)
{
	GMesh::operator =(rhs);

	pRefMesh = rhs.pRefMesh;
	PathVertIdx = rhs.PathVertIdx;
	Paths = rhs.Paths;
	EditPathVertIdx = rhs.EditPathVertIdx;

	return *this;
}

/*!
*	\brief	객체를 렌더링 한다.
*
*	\param bSubMesh 서브 메쉬의 렌더링 여부(선택 모드에서는 false로 설정)
*/
void GGeoPath::Render(bool bSubMesh)
{
	// 부모 클래스 함수를 호출한 후,
	GMesh::Render(bSubMesh);

	// 숨겨진 메쉬라면 리턴한다.
	if (!Renderer.IsVisibleMesh())
		return;
	
	// 현재까지 찾은 최단 경로를 렌더링 한다.
	glPushMatrix();
	{
		// 월드 좌표계에서 객체의 모델링 좌표계로 변환하고,
		glMultMatrixd(cast_mat(MC).M);

		// 경로 정점의 렌더링 타입 (0: 포인트, 1: 구)
		int RenderType = 0;
		if (RenderType == 0)
		{
			// Anti-aliasing 기능을 활성화하고,
			glDisable(GL_LIGHTING);
			glEnable(GL_BLEND);
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POINT_SMOOTH);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glPointSize(9.0);
			glLineWidth(3.0);

			// 경로 정점을 렌더링한다.
			int NumIdx = (int)PathVertIdx.size();
			for (int i = 0; i < NumIdx; ++i)
			{
				GPoint3 p = pRefMesh->Verts[PathVertIdx[i]].P;
				glColor3f(0.0, 0.0, 0.0);

				// 시작 정점과 끝점의 경우,
				if (i == 0 || i == NumIdx - 1)
					glColor3f(1.0, 0.0, 0.0);
				
				// 편집 중인 정점의 경우,
				if (PathVertIdx[i] == EditPathVertIdx)
					glColor3f(1.0, 1.0, 0.0);

				// 경로 정점을 렌더링 한다.
				glBegin(GL_POINTS);
				glVertex3f(p[0], p[1], p[2]);
				glEnd();
			}
						
			// 각각의 최단 경로를 렌더링 한다.
			glColor3f(0.25, 0.25, 0.25);
			int NumVert = Verts.size();
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < NumVert; ++i)
				glVertex3dv(Verts[i].P.V);
			glEnd();
// 			int NumPath = Paths.size();
// 			for (int i = 0; i < NumPath; ++i)
// 			{
// 				int NumVert = (int)Paths[i].size();
// 				glBegin(GL_LINE_STRIP);
// 				for (int j = 0; j < NumVert; ++j)
// 				{
// 					//double t = (double)j / (NumVert - 1);
// 					//unsigned char r, g, b;
// 					//get_color_gradient_from_rainbow(t, r, g, b);
// 					//glColor3ub(r, g, b);
// 					glVertex3dv(Paths[i][j].V);
// 				}
// 				glEnd();				
// 			}

			// Anti-aliasing 비활성화 한다		
			glEnable(GL_LIGHTING);
			glDisable(GL_BLEND);
			glDisable(GL_LINE_SMOOTH);
			glDisable(GL_POINT_SMOOTH);
			glPointSize(1.0);				
			glLineWidth(1.0);
		}
		else
		{
			// 두 개의 재질을 정의하고,
			GMaterial mtl_red, mtl_yellow;
			mtl_red.SetAmbient(1.0, 0.0, 0.0);
			mtl_red.SetDiffuse(1.0, 0.0, 0.0);
			mtl_yellow.SetAmbient(1.0, 1.0, 0.0);
			mtl_yellow.SetDiffuse(1.0, 1.0, 0.0);
			mtl_red.Enable(false);

			// 경로 정점의 크기를 구하여,
			double size = pRefMesh->BBox.GetDiagonal() * 0.007;

			// 경로 정점을 렌더링한다.
			int NumIdx = (int)PathVertIdx.size();
			for (int i = 0; i < NumIdx; ++i)
			{
				glPushMatrix();
				{
					GPoint3 p = pRefMesh->Verts[PathVertIdx[i]].P;
					glTranslated(p[0], p[1], p[2]);
					glutSolidSphere(size, 10, 10);

					if (PathVertIdx[i] == EditPathVertIdx)
					{
						mtl_yellow.Enable(false);
						glutWireSphere(size * 1.1, 10, 10);
						mtl_red.Enable(false);
					}
				}
				glPopMatrix();
			}

			// Anti-aliasing 기능을 활성화하고,
			glDisable(GL_LIGHTING);
			glEnable(GL_BLEND);
			glEnable(GL_LINE_SMOOTH);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// 각각의 최단 경로를 렌더링 한다.
			int NumPath = Paths.size();
			for (int i = 0; i < NumPath; ++i)
			{
				int NumVert = (int)Paths[i].size();
				glLineWidth(3.0);
				glBegin(GL_LINE_STRIP);
				for (int j = 0; j < NumVert; ++j)
				{
					double t = (double)j / (NumVert - 1);
					GVector3 C = get_color_gradient_from_rainbow(t);
					glColor3dv(C.V);
					glVertex3dv(Paths[i][j].V);
				}
				glEnd();
				glLineWidth(1.0);
			}

			// Anti-aliasing 비활성화 한다		
			glEnable(GL_LIGHTING);
			glDisable(GL_BLEND);
			glDisable(GL_LINE_SMOOTH);
		}		
	}
	glPopMatrix();	
}

/*!
*	\brief	실제 곡선이 정의된 도메인을 반환한다.
*
*	\param umin 도메인의 시작 값이 저장된다.
*	\param umax 도메인의 끝 값이 저장된다.
*/
void GGeoPath::GetDomain(double *umin, ...)
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
*	\brief	곡선의 제어점 리스트를 반환한다.
*
*	\return 곡선의 제어점 리스트를 반환한다.
*/
void GGeoPath::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
}

/*!
*	\brief	Biarc 곡선의 렌더링을 위한 메쉬(정점의 리스트)를 생성한다.
*/
void GGeoPath::CreateMesh()
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
void GGeoPath::CreateMeshVert()
{
	// 기존의 정점리스트를 초기화 하고,
	Verts.clear();

	// 각각의 측지 경로마다,
	int NumPath = Paths.size();
	for (int i = 0; i < NumPath; ++i)
	{
		// 경로를 구성하는 정점들을 정점 리스트에 추가한다.
		int NumVert = Paths[i].size();
		for (int j = (i == 0) ? 0 : 1; j < NumVert; ++j)
		{
			GVertex v(Paths[i][j], GVector3());
			AddVertex(v);
		}
	}
}

/*!
*	\brief	객체의 렌더링을 위한 텍스처 리스트를 생성한다.
*/
void GGeoPath::CreateMeshTex()
{
	// 기존의 텍셀 정보를 지우고,
	Texs.clear();
}

/*!
*	\brief	객체의 렌더링을 위한 삼각형 리스트를 생성한다.
*/
void GGeoPath::CreateMeshFace()
{
	// 삼각형 정보를 지우고,
	Faces.clear();
}

/*!
*	\brief	객체의 점을 계산
*
*	\param u 매개변수의 값
*
*	\return 객체의 점을 반환한다.
*/
GVector4 GGeoPath::Eval(double u, ...)
{
	return GVector4();
}

/*!
*	\brief	경로를 추가한다.
*
*	\param ToVertIdx 추가될 정점의 인덱스
*/
void GGeoPath::AddPath(int ToVertIdx)
{
	// 경로의 경유 정점의 인덱스를 추가한다.
	PathVertIdx.push_back(ToVertIdx);

	int NumIdx = (int)PathVertIdx.size();
	if (NumIdx < 2)
		return;
	
	int PrevVertIdx = PathVertIdx[NumIdx - 2];
	int CurrVertIdx = PathVertIdx[NumIdx - 1];

	GVertex *P = &(pRefMesh->Verts[PrevVertIdx]);
	GVertex *Q = &(pRefMesh->Verts[CurrVertIdx]);

	std::vector<GPoint3> NewPath;
	::get_geodesic_paths(pRefMesh, P, Q, NewPath);
	Paths.push_back(NewPath);

	ModifiedFlag = MODIFIED_VERT_NUM;
	UpdateMesh();
}

/*!
*	\brief	경로의 정점을 삭제한다.
*
*	\param VertIdx 삭제할 경로 정점의 인덱스
*/
void GGeoPath::DelPath(int VertIdx)
{
}

/*!
*	\brief 경로를 편집한다.
*
*	\param ToVertIdx 이동할 정점의 인덱스
*/
void GGeoPath::EditPath(int ToVertIdx)
{
	// 편집 정점이 속한 경로의 인덱스 정보를 구한다.
	int CurrPathIdx = -1;
	int NumVert = PathVertIdx.size();	
	for (int i = 0; i < NumVert; ++i)
	{
		if (PathVertIdx[i] == EditPathVertIdx)
		{
			CurrPathIdx = i;
			break;
		}
	}
	if (CurrPathIdx == -1)
		return;

	// 편집 정점과 연결된 이전 경로를 편집하고,
	if (CurrPathIdx > 0)
	{
		int PrevVertIdx = PathVertIdx[CurrPathIdx - 1];
		GVertex *P = &(pRefMesh->Verts[PrevVertIdx]);
		GVertex *Q = &(pRefMesh->Verts[ToVertIdx]);

		std::vector<GPoint3> NewPath;
		::get_geodesic_paths(pRefMesh, P, Q, NewPath);
		Paths[CurrPathIdx - 1] = NewPath;		
	}

	// 편집 정점과 연결된 다음 경로를 편집한다.
	if (CurrPathIdx < NumVert - 1)
	{
		int NextVertIdx = PathVertIdx[CurrPathIdx + 1];
		GVertex *P = &(pRefMesh->Verts[ToVertIdx]);
		GVertex *Q = &(pRefMesh->Verts[NextVertIdx]);

		std::vector<GPoint3> NewPath;
		::get_geodesic_paths(pRefMesh, P, Q, NewPath);
		Paths[CurrPathIdx] = NewPath;
	}

	// 경로 정점과 경로 메쉬를 갱신한다.
	PathVertIdx[CurrPathIdx] = ToVertIdx;
	EditPathVertIdx = ToVertIdx;
	ModifiedFlag = MODIFIED_SAMPLE_NUM;
	UpdateMesh();
}

/*!
*	\brief 경로의 시작 정점의 인덱스를 반환한다.
*
*	\return 경로의 시작 정점의 인덱스를 반환한다.
*/
int GGeoPath::GetStartVertIdx()
{
	if (PathVertIdx.empty())
		return -1;
	else
		return PathVertIdx[0];	
}

/*!
*	\brief 경로의 끝 정점의 인덱스를 반환한다.
*
*	\return 경로의 끝 정점의 인덱스를 반환한다.
*/
int GGeoPath::GetEndVertIdx()
{
	if (PathVertIdx.empty())
		return -1;
	else
	{
		int NumVert = (int)PathVertIdx.size();
		return PathVertIdx[NumVert - 1];
	}
}