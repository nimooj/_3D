#include "stdafx.h"

/*************************************/
/* Implementation of GBiarcCrv Class */
/*************************************/
IMPLEMENT_GMESH_OBJECT(GBiarcCrv)

/*!
*	\brief ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GBiarcCrv::GetRefMesh()
{
	return this;
}

/*!
*	\brief ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�
*/
GTransf GBiarcCrv::GetTransf()
{
	return MC;
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
void GBiarcCrv::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);
}

/*!
*	\brief	�޽��� �𵨸� ��ǥ�踦 ���� ��ǥ��� �����Ѵ�.
*
*	\param RefType �𵨸� ��ǥ���� ������ ���� ���� ����(0: ���� ��ǥ��, 1: ������ ��ǥ��)
*/
void GBiarcCrv::UpdateMC(int RefType)
{
}

/*!
*	\brief	��� ������ ��ġ�� �����Ѵ�.
*/
void GBiarcCrv::UpdateMesh()
{
	switch (ModifiedFlag)
	{
	case MODIFIED_VERT_POS:
		break;

	case MODIFIED_VERT_NUM:
		break;

		// ������ ����Ǿ��ٸ�,
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
*	\brief ���õ� ��ü�� ������ �Ѵ�.
*/
void GBiarcCrv::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);
}

/*!
*	\brief	�⺻ ������
*/
GBiarcCrv::GBiarcCrv()
: GMesh("noname", GTransf(), FACE_TRI, "")
{
}

/*!
*	\brief	������
*
*	\param _Name ��� �̸�
*	\param _MC ��ü�� �𵨸� ��ǥ��
*	\param _ArcList ��� �����ϴ� ��ũ�� ����Ʈ
*	\param _ParentName ��� �θ� �޽��� �̸�
*/
GBiarcCrv::GBiarcCrv(std::string _Name, GTransf _MC, std::vector<GCirArc> &_ArcList, std::string _ParentName = "")
: GMesh(_Name, _MC, FACE_TRI, _ParentName)
{
	ArcList = _ArcList;
}

/*!
*	\brief	���� ������
*
*	\param cpy ����� ��ü
*/
GBiarcCrv::GBiarcCrv(const GBiarcCrv &cpy)
: GMesh(cpy)
{
	ArcList = cpy.ArcList;
}

/*!
*	\brief	�Ҹ���
*/
GBiarcCrv::~GBiarcCrv()
{
}

/*!
*	\brief	���� ������
*
*	\param rhs ���Ե� ��ü
*
*	\return ���Ե� �ڽ��� ��ȯ�Ѵ�.
*/
GBiarcCrv &GBiarcCrv::operator =(const GBiarcCrv &rhs)
{
	GMesh::operator =(rhs);

	ArcList = rhs.ArcList;

	return *this;
}


/*!
*	\brief	Biarc ��� ������ �Ѵ�.
*
*	\param bSubMesh ���� �޽��� ������ ����(���� ��忡���� false�� ����)
*/
void GBiarcCrv::Render(bool bSubMesh)
{
	// �θ� Ŭ���� �Լ��� ȣ���� ��,
	GMesh::Render(bSubMesh);

	glPushMatrix();
	{
		// ���� ��ǥ�迡�� ��ü�� �𵨸� ��ǥ��� ��ȯ�ϰ�,
		glMultMatrixd(cast_mat(MC).M);

		// ��� ������ �ϱ� ���� ������ ��Ȱ��ȭ �ϰ�,
		glDisable(GL_LIGHTING);

		// ���� �β��� ������ ������ ��,
		glLineWidth(2.0);
		int NumMtl = GetNumMtl();
		int MtlIdx = Renderer.CrvMtlIdx;
		if (MtlIdx >= 0 && MtlIdx < NumMtl)
		{
			glColor3fv(MtlList[MtlIdx].Diffuse);
			// ��� ������ �Ѵ�.
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
			// ��� ������ �Ѵ�.
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

		// ���� �β��� �����ϰ�,
		glLineWidth(1.0);

		// ������ Ȱ��ȭ �Ѵ�.
		glEnable(GL_LIGHTING);
	}
	glPopMatrix();

// 	glPushMatrix();
// 	{
// 		// ���� ��ǥ�迡�� ��ü�� �𵨸� ��ǥ��� ��ȯ�ϰ�,
// 		glMultMatrixd(cast_mat(MC).M);
// 
// 		// �������� ������ �� ���,
// 		if (Renderer.RenderType & RENDER_CTRL_PT)
// 		{
// 			// ������ �����ϰ�,
// 			GMaterial Mtl("mtl_gray");
// 			Mtl.SetAmbient(0.7f, 0.7f, 0.7f, 1.0f);
// 			Mtl.SetDiffuse(0.7f, 0.7f, 0.7f, 1.0f);
// 			Mtl.SetSpecular(0.7f, 0.7f, 0.7f, 1.0f);
// 			Mtl.Enable(false);
// 
// 			// �������� �������� ��,
// 			double size = Renderer.CtrlPtSize;
// 			for (int i = 0; i < n + 1; ++i)
// 			{
// 				glPushMatrix();
// 				glTranslated(P[i][0], P[i][1], P[i][2]);
// 				glutSolidSphere(size, 10, 10);
// 				glPopMatrix();
// 			}
// 
// 			// ������ ����, �������� �����Ͽ�
// 			glDisable(GL_LIGHTING);
// 			glColor3d(1.0, 0.0, 0.0);
// 
// 			// ���� �������� ������ �Ѵ�.
// 			glBegin(GL_LINE_STRIP);
// 			for (int i = 0; i < n + 1; ++i)
// 				glVertex3d(P[i][0], P[i][1], P[i][2]);
// 			glEnd();
// 
// 			// ���� ������ ������ �ִٸ�,  
// 			if (!Renderer.NodePts.empty())
// 			{
// 				// ���� ������ ũ�⸦ �����ϰ�,
// 				glPointSize(10.0);
// 
// 				// ���� ������ ������ �ϰ�,
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
// 				// ������ ũ�⸦ �����ϰ�,
// 				glPointSize(1.0);
// 			}
// 
// 			// ������ Ȱ��ȭ �Ѵ�.
// 			glEnable(GL_LIGHTING);
// 		}
// 
// 		// ��� ������ �ϱ� ���� ������ ��Ȱ��ȭ �ϰ�,
// 		glDisable(GL_LIGHTING);
// 
// 		// ���� �β��� ������ ������ ��,
// 		glLineWidth(2.0);
// 		int NumMtl = GetNumMtl();
// 		int MtlIdx = Renderer.MtlIdx;
// 		if (MtlIdx >= 0 && MtlIdx < NumMtl)
// 		{
// 			glColor3fv(MtlList[MtlIdx].Diffuse);
// 			// ��� ������ �Ѵ�.
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
// 			// ��� ������ �Ѵ�.
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
// 		// ���� �β��� �����ϰ�,
// 		glLineWidth(1.0);
// 
// 		// ������ Ȱ��ȭ �Ѵ�.
// 		glEnable(GL_LIGHTING);
// 	}
// 	glPopMatrix();
}

/*!
*	\brief	���� ��� ���ǵ� �������� ��ȯ�Ѵ�.
*
*	\param umin �������� ���� ���� ����ȴ�.
*	\param umax �������� �� ���� ����ȴ�.
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
*	\brief	��� ������ ����Ʈ�� ��ȯ�Ѵ�.
*
*	\return ��� ������ ����Ʈ�� ��ȯ�Ѵ�.
*/
void GBiarcCrv::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
}

/*!
*	\brief	Biarc ��� �������� ���� �޽�(������ ����Ʈ)�� �����Ѵ�.
*/
void GBiarcCrv::CreateMesh()
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
*	\brief	�ҽ���� �������� ���� ������ ����Ʈ�� �����Ѵ�.
*/
void GBiarcCrv::CreateMeshVert()
{
	// ������ ������ �����, 
	Verts.clear();

	// ���ø� ������ ���ͼ�, 
	int NumSamp = Renderer.NumSampU;

	// ��ũ�� ������ ���Ͽ�, ������ ��ũ�� ���Ͽ�,
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

		// ��ũ ���κ��� ������ ���̰� 0�� �ƴ϶��,
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
*	\brief	�ҽ���� �������� ���� �ؽ�ó ����Ʈ�� �����Ѵ�.
*/
void GBiarcCrv::CreateMeshTex()
{
	// ������ �ؼ� ������ �����,
	Texs.clear();
}

/*!
*	\brief	�ҽ���� �������� ���� �ﰢ�� ����Ʈ�� �����Ѵ�.
*/
void GBiarcCrv::CreateMeshFace()
{
	// �ﰢ�� ������ �����,
	Faces.clear();
}

/*!
*	\brief	����� ���� ���
*
*	\param u �Ű������� ��
*
*	\return ����� �� C(u)�� ���
*/
GVector4 GBiarcCrv::Eval(double u, ...)
{
	return GVector4();
}
