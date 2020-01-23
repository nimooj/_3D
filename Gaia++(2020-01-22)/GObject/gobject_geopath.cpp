#include "stdafx.h"

/************************/
/* GGeoPath Ŭ���� ���� */
/************************/
IMPLEMENT_GMESH_OBJECT(GGeoPath)

/*!
*	\brief ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GGeoPath::GetRefMesh()
{
	return this;
}

/*!
*	\brief ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�
*/
GTransf GGeoPath::GetTransf()
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
void GGeoPath::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);
}

/*!
*	\brief	�޽��� �𵨸� ��ǥ�踦 ���� ��ǥ��� �����Ѵ�.
*
*	\param RefType �𵨸� ��ǥ���� ������ ���� ���� ����(0: ���� ��ǥ��, 1: ������ ��ǥ��)
*/
void GGeoPath::UpdateMC(int RefType)
{
	GMesh::UpdateMC(RefType);
}

/*!
*	\brief ����� ������ ��ġ�� �����Ѵ�.
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
*	\brief ���õ� ��ü�� ������ �Ѵ�.
*/
void GGeoPath::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);
}

/*!
*	\brief	�⺻ ������
*/
GGeoPath::GGeoPath()
: GMesh("noname", GTransf(), FACE_TRI, "")
{
	pRefMesh = NULL;
	EditPathVertIdx = -1;
}

/*!
*	\brief	������
*
*	\param _Name ��ü�� �̸�
*	\param _MC ��ü�� �𵨸� ��ǥ��
*	\param _pRefMesh ��ΰ� ������ �޽� ��ü
*	\param _ParentName ��� �θ� �޽��� �̸�
*/
GGeoPath::GGeoPath(std::string _Name, GTransf _MC, GMesh *_pRefMesh, std::string _ParentName = "")
: GMesh(_Name, _MC, FACE_TRI, _ParentName)
{
	pRefMesh = _pRefMesh;
	EditPathVertIdx = -1;
}

/*!
*	\brief	���� ������
*
*	\param cpy ����� ��ü
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
*	\brief	�Ҹ���
*/
GGeoPath::~GGeoPath()
{
}

/*!
*	\brief	���� ������
*
*	\param rhs ���Ե� ��ü
*
*	\return ���Ե� �ڽ��� ��ȯ�Ѵ�.
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
*	\brief	��ü�� ������ �Ѵ�.
*
*	\param bSubMesh ���� �޽��� ������ ����(���� ��忡���� false�� ����)
*/
void GGeoPath::Render(bool bSubMesh)
{
	// �θ� Ŭ���� �Լ��� ȣ���� ��,
	GMesh::Render(bSubMesh);

	// ������ �޽���� �����Ѵ�.
	if (!Renderer.IsVisibleMesh())
		return;
	
	// ������� ã�� �ִ� ��θ� ������ �Ѵ�.
	glPushMatrix();
	{
		// ���� ��ǥ�迡�� ��ü�� �𵨸� ��ǥ��� ��ȯ�ϰ�,
		glMultMatrixd(cast_mat(MC).M);

		// ��� ������ ������ Ÿ�� (0: ����Ʈ, 1: ��)
		int RenderType = 0;
		if (RenderType == 0)
		{
			// Anti-aliasing ����� Ȱ��ȭ�ϰ�,
			glDisable(GL_LIGHTING);
			glEnable(GL_BLEND);
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POINT_SMOOTH);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glPointSize(9.0);
			glLineWidth(3.0);

			// ��� ������ �������Ѵ�.
			int NumIdx = (int)PathVertIdx.size();
			for (int i = 0; i < NumIdx; ++i)
			{
				GPoint3 p = pRefMesh->Verts[PathVertIdx[i]].P;
				glColor3f(0.0, 0.0, 0.0);

				// ���� ������ ������ ���,
				if (i == 0 || i == NumIdx - 1)
					glColor3f(1.0, 0.0, 0.0);
				
				// ���� ���� ������ ���,
				if (PathVertIdx[i] == EditPathVertIdx)
					glColor3f(1.0, 1.0, 0.0);

				// ��� ������ ������ �Ѵ�.
				glBegin(GL_POINTS);
				glVertex3f(p[0], p[1], p[2]);
				glEnd();
			}
						
			// ������ �ִ� ��θ� ������ �Ѵ�.
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

			// Anti-aliasing ��Ȱ��ȭ �Ѵ�		
			glEnable(GL_LIGHTING);
			glDisable(GL_BLEND);
			glDisable(GL_LINE_SMOOTH);
			glDisable(GL_POINT_SMOOTH);
			glPointSize(1.0);				
			glLineWidth(1.0);
		}
		else
		{
			// �� ���� ������ �����ϰ�,
			GMaterial mtl_red, mtl_yellow;
			mtl_red.SetAmbient(1.0, 0.0, 0.0);
			mtl_red.SetDiffuse(1.0, 0.0, 0.0);
			mtl_yellow.SetAmbient(1.0, 1.0, 0.0);
			mtl_yellow.SetDiffuse(1.0, 1.0, 0.0);
			mtl_red.Enable(false);

			// ��� ������ ũ�⸦ ���Ͽ�,
			double size = pRefMesh->BBox.GetDiagonal() * 0.007;

			// ��� ������ �������Ѵ�.
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

			// Anti-aliasing ����� Ȱ��ȭ�ϰ�,
			glDisable(GL_LIGHTING);
			glEnable(GL_BLEND);
			glEnable(GL_LINE_SMOOTH);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// ������ �ִ� ��θ� ������ �Ѵ�.
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

			// Anti-aliasing ��Ȱ��ȭ �Ѵ�		
			glEnable(GL_LIGHTING);
			glDisable(GL_BLEND);
			glDisable(GL_LINE_SMOOTH);
		}		
	}
	glPopMatrix();	
}

/*!
*	\brief	���� ��� ���ǵ� �������� ��ȯ�Ѵ�.
*
*	\param umin �������� ���� ���� ����ȴ�.
*	\param umax �������� �� ���� ����ȴ�.
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
*	\brief	��� ������ ����Ʈ�� ��ȯ�Ѵ�.
*
*	\return ��� ������ ����Ʈ�� ��ȯ�Ѵ�.
*/
void GGeoPath::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
}

/*!
*	\brief	Biarc ��� �������� ���� �޽�(������ ����Ʈ)�� �����Ѵ�.
*/
void GGeoPath::CreateMesh()
{
	// ��� ������ ����Ʈ�� �ʱ�ȭ �ϰ�,
	InitBndryEdgeList();

	// ������ ��ǥ�� �����ϰ�,
	CreateMeshVert();

	// �ؽ�ó ��ǥ�� �����ϰ�,
	CreateMeshTex();

	// �ﰢ�� ������ �����ϰ�, 
	CreateMeshFace();

	// ������ ������ ����ϰ�,
	UpdateNormal(false);

	// ����� �ٿ�� �ڽ��� ����Ѵ�.
	UpdateBndBox(false);

	// ���� ���ۿ� �ε��� ���۸� �����Ѵ�.
	UpdateBufVertex();
	UpdateBufNormal();
	UpdateBufFace();
}

/*!
*	\brief	��ü�� �������� ���� ������ ����Ʈ�� �����Ѵ�.
*/
void GGeoPath::CreateMeshVert()
{
	// ������ ��������Ʈ�� �ʱ�ȭ �ϰ�,
	Verts.clear();

	// ������ ���� ��θ���,
	int NumPath = Paths.size();
	for (int i = 0; i < NumPath; ++i)
	{
		// ��θ� �����ϴ� �������� ���� ����Ʈ�� �߰��Ѵ�.
		int NumVert = Paths[i].size();
		for (int j = (i == 0) ? 0 : 1; j < NumVert; ++j)
		{
			GVertex v(Paths[i][j], GVector3());
			AddVertex(v);
		}
	}
}

/*!
*	\brief	��ü�� �������� ���� �ؽ�ó ����Ʈ�� �����Ѵ�.
*/
void GGeoPath::CreateMeshTex()
{
	// ������ �ؼ� ������ �����,
	Texs.clear();
}

/*!
*	\brief	��ü�� �������� ���� �ﰢ�� ����Ʈ�� �����Ѵ�.
*/
void GGeoPath::CreateMeshFace()
{
	// �ﰢ�� ������ �����,
	Faces.clear();
}

/*!
*	\brief	��ü�� ���� ���
*
*	\param u �Ű������� ��
*
*	\return ��ü�� ���� ��ȯ�Ѵ�.
*/
GVector4 GGeoPath::Eval(double u, ...)
{
	return GVector4();
}

/*!
*	\brief	��θ� �߰��Ѵ�.
*
*	\param ToVertIdx �߰��� ������ �ε���
*/
void GGeoPath::AddPath(int ToVertIdx)
{
	// ����� ���� ������ �ε����� �߰��Ѵ�.
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
*	\brief	����� ������ �����Ѵ�.
*
*	\param VertIdx ������ ��� ������ �ε���
*/
void GGeoPath::DelPath(int VertIdx)
{
}

/*!
*	\brief ��θ� �����Ѵ�.
*
*	\param ToVertIdx �̵��� ������ �ε���
*/
void GGeoPath::EditPath(int ToVertIdx)
{
	// ���� ������ ���� ����� �ε��� ������ ���Ѵ�.
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

	// ���� ������ ����� ���� ��θ� �����ϰ�,
	if (CurrPathIdx > 0)
	{
		int PrevVertIdx = PathVertIdx[CurrPathIdx - 1];
		GVertex *P = &(pRefMesh->Verts[PrevVertIdx]);
		GVertex *Q = &(pRefMesh->Verts[ToVertIdx]);

		std::vector<GPoint3> NewPath;
		::get_geodesic_paths(pRefMesh, P, Q, NewPath);
		Paths[CurrPathIdx - 1] = NewPath;		
	}

	// ���� ������ ����� ���� ��θ� �����Ѵ�.
	if (CurrPathIdx < NumVert - 1)
	{
		int NextVertIdx = PathVertIdx[CurrPathIdx + 1];
		GVertex *P = &(pRefMesh->Verts[ToVertIdx]);
		GVertex *Q = &(pRefMesh->Verts[NextVertIdx]);

		std::vector<GPoint3> NewPath;
		::get_geodesic_paths(pRefMesh, P, Q, NewPath);
		Paths[CurrPathIdx] = NewPath;
	}

	// ��� ������ ��� �޽��� �����Ѵ�.
	PathVertIdx[CurrPathIdx] = ToVertIdx;
	EditPathVertIdx = ToVertIdx;
	ModifiedFlag = MODIFIED_SAMPLE_NUM;
	UpdateMesh();
}

/*!
*	\brief ����� ���� ������ �ε����� ��ȯ�Ѵ�.
*
*	\return ����� ���� ������ �ε����� ��ȯ�Ѵ�.
*/
int GGeoPath::GetStartVertIdx()
{
	if (PathVertIdx.empty())
		return -1;
	else
		return PathVertIdx[0];	
}

/*!
*	\brief ����� �� ������ �ε����� ��ȯ�Ѵ�.
*
*	\return ����� �� ������ �ε����� ��ȯ�Ѵ�.
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