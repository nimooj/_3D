#include "stdafx.h"

/*************************************/
/* Implementation of GBiarcCrv Class */
/*************************************/
IMPLEMENT_GMESH_OBJECT(GBiarcCrv)

/*!
*	\brief 객체가 포함된 메쉬의 포인터를 반환한다.
*
*	\return 객체가 포함된 메쉬의 포인터를 반환한다.
*/
GMesh *GBiarcCrv::GetRefMesh()
{
	return this;
}

/*!
*	\brief 객체의 모델링 좌표계를 반환한다.
*
*	\return 월드 좌표계에서 객체의 모델링 좌표계를 반환한다
*/
GTransf GBiarcCrv::GetTransf()
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
void GBiarcCrv::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);
}

/*!
*	\brief	메쉬의 모델링 좌표계를 월드 좌표계로 갱신한다.
*
*	\param RefType 모델링 좌표계의 갱신을 위한 참조 형태(0: 월드 좌표계, 1: 경계상자 좌표계)
*/
void GBiarcCrv::UpdateMC(int RefType)
{
}

/*!
*	\brief	곡선의 정점의 위치를 갱신한다.
*/
void GBiarcCrv::UpdateMesh()
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
		//CreateMeshVert();
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
void GBiarcCrv::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);
}

/*!
*	\brief	기본 생성자
*/
GBiarcCrv::GBiarcCrv()
: GMesh("noname", GTransf(), FACE_TRI, "")
{
}

/*!
*	\brief	생성자
*
*	\param _Name 곡선의 이름
*	\param _MC 객체의 모델링 좌표계
*	\param _ArcList 곡선을 구성하는 아크의 리스트
*	\param _ParentName 곡선의 부모 메쉬의 이름
*/
GBiarcCrv::GBiarcCrv(std::string _Name, GTransf _MC, std::vector<GCirArc> &_ArcList, std::string _ParentName = "")
: GMesh(_Name, _MC, FACE_TRI, _ParentName)
{
	ArcList = _ArcList;
}

/*!
*	\brief	복사 생성자
*
*	\param cpy 복사될 객체
*/
GBiarcCrv::GBiarcCrv(const GBiarcCrv &cpy)
: GMesh(cpy)
{
	ArcList = cpy.ArcList;
}

/*!
*	\brief	소멸자
*/
GBiarcCrv::~GBiarcCrv()
{
}

/*!
*	\brief	대입 연산자
*
*	\param rhs 대입될 객체
*
*	\return 대입된 자신을 반환한다.
*/
GBiarcCrv &GBiarcCrv::operator =(const GBiarcCrv &rhs)
{
	GMesh::operator =(rhs);

	ArcList = rhs.ArcList;

	return *this;
}


/*!
*	\brief	Biarc 곡선을 렌더링 한다.
*
*	\param bSubMesh 서브 메쉬의 렌더링 여부(선택 모드에서는 false로 설정)
*/
void GBiarcCrv::Render(bool bSubMesh)
{
	// 부모 클래스 함수를 호출한 후,
	GMesh::Render(bSubMesh);

	glPushMatrix();
	{
		// 월드 좌표계에서 객체의 모델링 좌표계로 변환하고,
		glMultMatrixd(cast_mat(MC).M);

		// 곡선을 렌더링 하기 위해 조명을 비활성화 하고,
		glDisable(GL_LIGHTING);

		// 선의 두께와 색상을 설정한 후,
		glLineWidth(2.0);
		int NumMtl = GetNumMtl();
		int MtlIdx = Renderer.CrvMtlIdx;
		if (MtlIdx >= 0 && MtlIdx < NumMtl)
		{
			glColor3fv(MtlList[MtlIdx].Diffuse);
			// 곡선을 렌더링 한다.
			glBegin(GL_LINE_STRIP);
			{
				int NumVert = (int)Verts.size();
				for (int i = 0; i < NumVert; ++i)
					glVertex3dv(Verts[i].P.V);
			}
			glEnd();
		}
		else
		{
			// 곡선을 렌더링 한다.
			glBegin(GL_LINE_STRIP);
			{
				int NumVert = (int)Verts.size();
				for (int i = 0; i < NumVert; ++i)
				{
					double t = (double)i / (NumVert - 1);
					GVector3 color = get_color_gradient_from_rainbow(t);
					glColor3d(color[0], color[1], color[2]);
					glVertex3dv(Verts[i].P.V);
				}
			}
			glEnd();
		}

		// 선의 두께를 복원하고,
		glLineWidth(1.0);

		// 조명을 활성화 한다.
		glEnable(GL_LIGHTING);
	}
	glPopMatrix();

// 	glPushMatrix();
// 	{
// 		// 월드 좌표계에서 객체의 모델링 좌표계로 변환하고,
// 		glMultMatrixd(cast_mat(MC).M);
// 
// 		// 제어점을 렌더링 할 경우,
// 		if (Renderer.RenderType & RENDER_CTRL_PT)
// 		{
// 			// 재질을 설정하고,
// 			GMaterial Mtl("mtl_gray");
// 			Mtl.SetAmbient(0.7f, 0.7f, 0.7f, 1.0f);
// 			Mtl.SetDiffuse(0.7f, 0.7f, 0.7f, 1.0f);
// 			Mtl.SetSpecular(0.7f, 0.7f, 0.7f, 1.0f);
// 			Mtl.Enable(false);
// 
// 			// 제어점을 렌더링한 후,
// 			double size = Renderer.CtrlPtSize;
// 			for (int i = 0; i < n + 1; ++i)
// 			{
// 				glPushMatrix();
// 				glTranslated(P[i][0], P[i][1], P[i][2]);
// 				glutSolidSphere(size, 10, 10);
// 				glPopMatrix();
// 			}
// 
// 			// 조명을 끄고, 붉은색을 설정하여
// 			glDisable(GL_LIGHTING);
// 			glColor3d(1.0, 0.0, 0.0);
// 
// 			// 제어 폴리곤을 렌더링 한다.
// 			glBegin(GL_LINE_STRIP);
// 			for (int i = 0; i < n + 1; ++i)
// 				glVertex3d(P[i][0], P[i][1], P[i][2]);
// 			glEnd();
// 
// 			// 보간 정점을 가지고 있다면,  
// 			if (!Renderer.NodePts.empty())
// 			{
// 				// 보간 정점의 크기를 설정하고,
// 				glPointSize(10.0);
// 
// 				// 보간 정점을 렌더링 하고,
// 				std::vector<GVector3> &Pts = Renderer.NodePts;
// 				int num = (int)Pts.size();
// 				for (int i = 0; i < num; ++i)
// 				{
// 					if (i == 0)
// 						glColor3f(1.0, 0.0, 0.0);
// 					else if (i == num - 1)
// 						glColor3f(0.0, 0.0, 1.0);
// 					else
// 						glColor3f(0.0, 1.0, 0.0);					
// 
// 					glBegin(GL_POINTS);
// 					glVertex3f(Pts[i][0], Pts[i][1], Pts[i][2]);
// 					glEnd();
// 				}
// 
// 				// 정점의 크기를 복원하고,
// 				glPointSize(1.0);
// 			}
// 
// 			// 조명을 활성화 한다.
// 			glEnable(GL_LIGHTING);
// 		}
// 
// 		// 곡선을 렌더링 하기 위해 조명을 비활성화 하고,
// 		glDisable(GL_LIGHTING);
// 
// 		// 선의 두께와 색상을 설정한 후,
// 		glLineWidth(2.0);
// 		int NumMtl = GetNumMtl();
// 		int MtlIdx = Renderer.MtlIdx;
// 		if (MtlIdx >= 0 && MtlIdx < NumMtl)
// 		{
// 			glColor3fv(MtlList[MtlIdx].Diffuse);
// 			// 곡선을 렌더링 한다.
// 			glBegin(GL_LINE_STRIP);
// 			{
// 				int NumVert = (int)Verts.size();
// 				for (int i = 0; i < NumVert; ++i)
// 					glVertex3dv(Verts[i].P.V);
// 			}
// 			glEnd();
// 		}
// 		else
// 		{
// 			// 곡선을 렌더링 한다.
// 			glBegin(GL_LINE_STRIP);
// 			{
// 				int NumVert = (int)Verts.size();
// 				for (int i = 0; i < NumVert; ++i)
// 				{
// 					double t = (double)i / (NumVert - 1);
// 					unsigned char r, g, b;
// 					get_color_gradient_from_rainbow(t, r, g, b);
// 					glColor3ub(r, g, b);
// 					glVertex3dv(Verts[i].P.V);
// 				}
// 			}
// 			glEnd();
// 		}
// 
// 		// 선의 두께를 복원하고,
// 		glLineWidth(1.0);
// 
// 		// 조명을 활성화 한다.
// 		glEnable(GL_LIGHTING);
// 	}
// 	glPopMatrix();
}

/*!
*	\brief	실제 곡선이 정의된 도메인을 반환한다.
*
*	\param umin 도메인의 시작 값이 저장된다.
*	\param umax 도메인의 끝 값이 저장된다.
*/
void GBiarcCrv::GetDomain(double *umin, ...)
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
void GBiarcCrv::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
}

/*!
*	\brief	Biarc 곡선의 렌더링을 위한 메쉬(정점의 리스트)를 생성한다.
*/
void GBiarcCrv::CreateMesh()
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
*	\brief	넙스곡선의 렌더링을 위한 정점의 리스트를 생성한다.
*/
void GBiarcCrv::CreateMeshVert()
{
	// 기존의 정점을 지우고, 
	Verts.clear();

	// 샘플링 정보를 얻어와서, 
	int NumSamp = Renderer.NumSampU;

	// 아크의 개수를 구하여, 각각의 아크에 대하여,
	int NumArc = (int)ArcList.size();
	for (int i = 0; i < NumArc; ++i)
	{
		GCirArc *pArc = &ArcList[i];
		for (int j = 0; j < NumSamp; ++j)
		{
			double t = (double)j / (double)(NumSamp - 1);
			GVector4 pt = pArc->Eval(t);			
			AddVertex(GVertex(cast_pt3(pt)));
		}

		// 아크 끝부분의 접선의 길이가 0이 아니라면,
		if (pArc->LenTangent != 0.0)
		{
			GVector4 pt = pArc->Eval(1.0);
			GVector4 v = pArc->EvalDeriv(1.0);
			pt = pt + pArc->LenTangent * v;
			AddVertex(GVertex(cast_pt3(pt)));
		}		
	}
}

/*!
*	\brief	넙스곡선의 렌더링을 위한 텍스처 리스트를 생성한다.
*/
void GBiarcCrv::CreateMeshTex()
{
	// 기존의 텍셀 정보를 지우고,
	Texs.clear();
}

/*!
*	\brief	넙스곡선의 렌더링을 위한 삼각형 리스트를 생성한다.
*/
void GBiarcCrv::CreateMeshFace()
{
	// 삼각형 정보를 지우고,
	Faces.clear();
}

/*!
*	\brief	곡선위의 점을 계산
*
*	\param u 매개변수의 값
*
*	\return 곡선위의 점 C(u)를 계산
*/
GVector4 GBiarcCrv::Eval(double u, ...)
{
	return GVector4();
}
