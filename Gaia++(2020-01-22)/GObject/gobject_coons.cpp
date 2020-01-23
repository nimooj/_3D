#include "stdafx.h"

/*************************************/
/* Implementation of GCoonsSrf Class */
/*************************************/
IMPLEMENT_GMESH_OBJECT(GCoonsSrf)

/*!
*	\brief ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GCoonsSrf::GetRefMesh()
{
	return this;
}

/*!
*	\brief ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�
*/
GTransf GCoonsSrf::GetTransf()
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
void GCoonsSrf::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);
}

/*!
*	\brief	�޽��� �𵨸� ��ǥ�踦 ���� ��ǥ��� �����Ѵ�.
*
*	\param RefType �𵨸� ��ǥ���� ������ ���� ���� ����(0: ���� ��ǥ��, 1: ������ ��ǥ��)
*/
void GCoonsSrf::UpdateMC(int RefType)
{
}

/*!
*	\brief	��� ������ ��ġ�� �����Ѵ�.
*/
void GCoonsSrf::UpdateMesh()
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
*	\brief ���õ� ��ü�� ������ �Ѵ�.
*/
void GCoonsSrf::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);
}

/*!
*	\brief	�⺻ ������
*/
GCoonsSrf::GCoonsSrf()
: GMesh("noname", GTransf(), FACE_QUAD, "")
{
	for (int i = 0; i < 4; ++i)
		pCrvs[i] = NULL;
}

/*!
*	\brief	Coons ����� �����Ѵ�.
*	
*	\param _Name ����� �̸�
*	\param _MC ����� �𵨸� ��ǥ��
*	\param _p ����� U-���� ����
*	\param _m ����� U-���� �������� ������ �ε���
*	\param _U ����� U-���� knot ����
*	\param _q ����� V-���� ����
*	\param _n ����� V-���� �������� ������ �ε���
*	\param _V ����� V-���� knot ����
*	\param _P ����� ��� ������
*	\param _ParentName �θ� �޽� ����� �̸�
*/
GCoonsSrf::GCoonsSrf(std::string _Name, GTransf _MC, int _p, int _m, double *_U, int _q, int _n, double *_V, GVector4 *_P, std::string _ParentName)
: GMesh(_Name, _MC, FACE_QUAD, _ParentName)
{
	// ��� ��� ������ �迭�� �����ϰ�,
	GCtrlPt *P[4];
	P[0] = new GCtrlPt [_m + 1];
	P[1] = new GCtrlPt [_n + 1];
	P[2] = new GCtrlPt [_m + 1];
	P[3] = new GCtrlPt [_n + 1];

	// ������ �迭�� �ʱ�ȭ �Ѵ�.
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

	// ��� ��� �����Ѵ�.
	pCrvs[0] = new GNurbsCrv("Crv0", _MC, _p, _m, _U, P[0], false, Name);
	pCrvs[1] = new GNurbsCrv("Crv1", _MC, _q, _n, _V, P[1], false, Name);
	pCrvs[2] = new GNurbsCrv("Crv2", _MC, _p, _m, _U, P[2], false, Name);
	pCrvs[3] = new GNurbsCrv("Crv3", _MC, _q, _n, _V, P[3], false, Name);

	// ������ �迭�� �����Ѵ�.
	for (int i = 0; i < 4; ++i)
		delete [] P[i];
}

/*!
*	\brief	��� ��� �̿��Ͽ� Coons ����� �����Ѵ�.
*	\note ��� ��� �ݽð�������� ���ĵ� ������ �����Ѵ�.
*	
*	\param _Name ����� �̸�
*	\param _MC ����� �𵨸� ��ǥ��
*	\param pCrv0 ��� � S(u, 0)
*	\param pCrv1 ��� � S(1, v)
*	\param pCrv2 ��� � S(u, 1)
*	\param pCrv3 ��� � S(0, v)
*	\param _ParentName �θ� �޽� ����� �̸�
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
*	\brief	���� ������
*
*	\param cpy ����� ��ü
*/
GCoonsSrf::GCoonsSrf(const GCoonsSrf &cpy)
: GMesh(cpy)
{
	// ��� ��� �����Ѵ�.
	pCrvs[0] = cpy.pCrvs[0]->CopyObject();
	pCrvs[1] = cpy.pCrvs[1]->CopyObject();
	pCrvs[2] = cpy.pCrvs[2]->CopyObject();
	pCrvs[3] = cpy.pCrvs[3]->CopyObject();
}

/*!
*	\brief	�Ҹ���
*/
GCoonsSrf::~GCoonsSrf()
{
	// ��� ��� �����Ѵ�.
	for (int i = 0; i < 4; ++i)
	{
		if (pCrvs[i] != NULL)
			delete pCrvs[i];
	}
}

/*!
*	\brief	���� ������
*
*	\param rhs ���Ե� ��ü
*
*	\return ���Ե� �ڽ��� ��ȯ�Ѵ�.
*/
GCoonsSrf &GCoonsSrf::operator =(const GCoonsSrf &rhs)
{
	// �޽��� �����ϰ�,
	GMesh::operator =(rhs);

	// ������ ��� ��� �����ϰ�,
	for (int i = 0; i < 4; ++i)
	{
		if (pCrvs[i] != NULL)
			delete pCrvs[i];
	}

	// ���ο� ��� ��� �����Ѵ�.
	pCrvs[0] = rhs.pCrvs[0]->CopyObject();
	pCrvs[1] = rhs.pCrvs[1]->CopyObject();
	pCrvs[2] = rhs.pCrvs[2]->CopyObject();
	pCrvs[3] = rhs.pCrvs[3]->CopyObject();

	// ���Ե� �ڽ��� ��ȯ�Ѵ�.
	return *this;
}

/*!
*	\brief ��ü�� �������Ѵ�.
*
*	\param bSubMesh ���� �޽��� ������ ����(���� ��忡���� false�� ����)
*/
void GCoonsSrf::Render(bool bSubMesh)
{
	// �θ� Ŭ���� �Լ��� ȣ���� ��,
	GMesh::Render(bSubMesh);

	// ���� ��ǥ�迡��
	glPushMatrix();
	{
		// ��ü�� �𵨸� ��ǥ����� ��ȯ�ϰ�,
		glMultMatrixd(cast_mat(MC).M);

		// ����� �������� �������� ���,
		if (Renderer.RenderType & RENDER_CTRL_PT)
		{
			double size = Renderer.CtrlPtSize;

			// ������ �����ϰ�,
			GMaterial Mtl;
			Mtl.SetAmbient(0.7f, 0.7f, 0.7f);
			Mtl.SetDiffuse(0.7f, 0.7f, 0.7f);
			Mtl.SetSpecular(0.7f, 0.7f, 0.7f);
			Mtl.SetAlpha(1.0f);
			Mtl.Enable(false);

			// U ���� ��� �������� �������ϰ�,
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

			// V ���� ��� �������� �������ϰ�,
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

			// ������ ��Ȱ��ȭ �ϰ�,
			glDisable(GL_LIGHTING);
			glLineWidth(2.0);

			// ���� ���� �޽��� �������ϰ�,
			glColor3d(1.0, 0.0, 0.0);
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < m + 1; i++)
				glVertex3d(pCrvs[0]->P[i][0], pCrvs[0]->P[i][1], pCrvs[0]->P[i][2]);
			glEnd();

			// ������ ���� �޽��� �������ϰ�,
			glColor3d(0.0, 0.0, 1.0);
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < m + 1; i++)
				glVertex3d(pCrvs[2]->P[i][0], pCrvs[2]->P[i][1], pCrvs[2]->P[i][2]);
			glEnd();

			// �Ʒ��� ���� �޽��� �������ϰ�,
			glColor3d(0.0, 1.0, 0.0);
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < n + 1; i++)
				glVertex3d(pCrvs[1]->P[i][0], pCrvs[1]->P[i][1], pCrvs[1]->P[i][2]);
			glEnd();

			// ���� ���� �޽��� �������ϰ�,
			glColor3d(1.0, 1.0, 0.0);
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < n + 1; i++)
				glVertex3d(pCrvs[3]->P[i][0], pCrvs[3]->P[i][1], pCrvs[3]->P[i][2]);
			glEnd();

			// ������ Ȱ��ȭ �Ѵ�.
			glLineWidth(1.0);
			glEnable(GL_LIGHTING);
		}
	}
	glPopMatrix();
}

/*!
*	\brief	��ü�� ���ǵ� �Ķ���� �������� ��ȯ�Ѵ�.
*
*	\param umin �������� ���� ���� ����ȴ�.
*	\param umax �������� �� ���� ����ȴ�.
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
*	\brief	��ü�� ������ ����Ʈ�� ��ȯ�Ѵ�.
*
*	\param CtrlPtList ��ü�� ������ ����Ʈ�� �����
*
*	\return ��ü�� ������ ����Ʈ�� ��ȯ�Ѵ�.
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
*	\brief ��ü�� �������� ���� �޽�(������ ����Ʈ)�� �����Ѵ�.
*/
void GCoonsSrf::CreateMesh()
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
void GCoonsSrf::CreateMeshVert()
{
	// ������ ������ �����,
	Verts.clear();

	// ���ǿ��� ����ϰ�,
	double umin, umax, vmin, vmax;
	GetDomain(&umin, &umax, &vmin, &vmax);

	// ���ø� ������ ���ͼ�, 
	int NumSampU = Renderer.NumSampU;
	int NumSampV = Renderer.NumSampV;

	// ������� ������ ����Ѵ�.
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
*	\brief	��ü�� �������� ���� �ؽ�ó ����Ʈ�� �����Ѵ�.
*/
void GCoonsSrf::CreateMeshTex()
{
	// ������ �ؼ� ������ �����,
	Texs.clear();

	// ���ø� ������ ���ͼ�, 
	int NumSampU = Renderer.NumSampU;
	int NumSampV = Renderer.NumSampV;

	// �ؽ�ó ��ǥ�� �����ϰ� ������
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
*	\brief	��ü�� �������� ���� �ﰢ�� ����Ʈ�� �����Ѵ�.
*/
void GCoonsSrf::CreateMeshFace()
{
	// �ﰢ�� ������ �����,
	Faces.clear();

	// ���ø� ������ ���ͼ�, 
	int NumSampU = Renderer.NumSampU;
	int NumSampV = Renderer.NumSampV;

	// �ﰢ���� �����ϰ� �߰��Ѵ�.
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
*	\brief	��ü�� �����ϴ� ������ ����Ѵ�.
*
*	\param u �Ű������� ��
*
*	\return ���� ���� ��ȯ�Ѵ�
*/
GVector4 GCoonsSrf::Eval(double u, ...)
{
	// �Ű����� (u, v)�� ���ϰ�,
	double v;
	va_list ap;
	va_start(ap, u);
	v = va_arg(ap, double);
	va_end(ap);

	GVector4 p, q;
	// U ���� ��� ����� ���ǵǴ� ������(ruled surface)�� ������ ����ϰ�,
	p = (1.0 - v) * (pCrvs[0]->Eval(u)) + v * (pCrvs[2]->Eval(1.0 - u));

	// V ���� ��� ����� ���ǵǴ� ������(ruled surface)�� ������ ����ϰ�,
	q = (1.0 - u) * (pCrvs[3]->Eval(1.0 - v)) + u * (pCrvs[1]->Eval(v));

	// 4���� �ڳ� �������� ���ǵǴ� Bilinear ����� ������ ����ϰ�,
	GVector4 p0, p1, p2, p3, r;
	p0 = pCrvs[0]->P[0];
	p1 = pCrvs[1]->P[0];
	p2 = pCrvs[2]->P[0];
	p3 = pCrvs[3]->P[0];
	r = (1.0 - v) * ((1.0 - u) * p0 + u * p1) + v * ((1.0 - u) * p3 + u * p2);

	// �������� Coons ����� ������ ����Ͽ� ��ȯ�Ѵ�.
	return p + q - r;
}

/*!
*	\brief U ���� knot refinement�� ������
*/
void GCoonsSrf::KnotRefineU()
{
	pCrvs[0]->KnotRefine();
	pCrvs[2]->KnotRefine();
}

/*!
*	\brief V ���� knot refinement�� ������
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
*	\brief ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GCoonsMesh::GetRefMesh()
{
	return this;
}

/*!
*	\brief ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�
*/
GTransf GCoonsMesh::GetTransf()
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
void GCoonsMesh::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);
}

/*!
*	\brief	�޽��� �𵨸� ��ǥ�踦 ���� ��ǥ��� �����Ѵ�.
*
*	\param RefType �𵨸� ��ǥ���� ������ ���� ���� ����(0: ���� ��ǥ��, 1: ������ ��ǥ��)
*/
void GCoonsMesh::UpdateMC(int RefType)
{
}

/*!
*	\brief	��� ������ ��ġ�� �����Ѵ�.
*/
void GCoonsMesh::UpdateMesh()
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
*	\brief ���õ� ��ü�� ������ �Ѵ�.
*/
void GCoonsMesh::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);
}

/*!
*	\brief	�⺻ ������
*/
GCoonsMesh::GCoonsMesh()
: GMesh("noname", GTransf(), FACE_QUAD, "")
{
	pRefMesh = NULL;
}

/*!
*	\brief	Coons �޽��� �����Ѵ�.
*	
*	\param _Name �޽��� �̸�
*	\param _pRefMesh �޽��� ���ǵ� ���� �޽�
*/
GCoonsMesh::GCoonsMesh(std::string _Name, GMesh *_pRefMesh)
: GMesh(_Name, _pRefMesh->MC, FACE_QUAD, _pRefMesh->Name)
{
	pRefMesh = _pRefMesh;	
}

/*!
*	\brief	���� ������
*
*	\param cpy ����� ��ü
*/
GCoonsMesh::GCoonsMesh(const GCoonsMesh &cpy)
: GMesh(cpy)
{
	// Coons �޽��� ���ǵǴ� ���� �޽��� �����Ѵ�.
	pRefMesh = cpy.pRefMesh->CopyObject();

	// Coons �޽��� �ڳ� �������� ����Ʈ�� �����Ѵ�.
	VertIdx = cpy.VertIdx;
}

/*!
*	\brief	�Ҹ���
*/
GCoonsMesh::~GCoonsMesh()
{
}

/*!
*	\brief	���� ������
*
*	\param rhs ���Ե� ��ü
*
*	\return ���Ե� �ڽ��� ��ȯ�Ѵ�.
*/
GCoonsMesh &GCoonsMesh::operator =(const GCoonsMesh &rhs)
{
	// �޽��� �����ϰ�,
	GMesh::operator =(rhs);

	// Coons �޽��� ���ǵǴ� ���� �޽��� �����Ѵ�.
	pRefMesh = rhs.pRefMesh->CopyObject();

	// Coons �޽��� �ڳ� �������� ����Ʈ�� �����Ѵ�.
	VertIdx = rhs.VertIdx;

	// ���Ե� �ڽ��� ��ȯ�Ѵ�.
	return *this;
}

/*!
*	\brief ��ü�� �������Ѵ�.
*
*	\param bSubMesh ���� �޽��� ������ ����(���� ��忡���� false�� ����)
*/
void GCoonsMesh::Render(bool bSubMesh)
{
	// �θ� Ŭ���� �Լ��� ȣ���� ��,
	GMesh::Render(bSubMesh);

// 	// ���� ��ǥ�迡��
// 	glPushMatrix();
// 	{
// 		// ��ü�� �𵨸� ��ǥ����� ��ȯ�ϰ�,
// 		glMultMatrixd(cast_mat(MC).M);
// 
// 		// ������ ��Ȱ��ȭ �ϰ�,
// 		glDisable(GL_LIGHTING);
// 		glLineWidth(5.0);
// 
// 		// ���� ���� Coons ��ġ�� ��� ��θ� ������ �Ѵ�.
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
// 		// ������ Ȱ��ȭ �Ѵ�.
// 		glEnable(GL_LIGHTING);
// 		glLineWidth(1.0);
// 	}
// 	glPopMatrix();
}

/*!
*	\brief	��ü�� ���ǵ� �Ķ���� �������� ��ȯ�Ѵ�.
*
*	\param umin �������� ���� ���� ����ȴ�.
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
*	\brief	��ü�� ������ ����Ʈ�� ��ȯ�Ѵ�.
*
*	\param CtrlPtList ��ü�� ������ ����Ʈ�� �����
*
*	\return ��ü�� ������ ����Ʈ�� ��ȯ�Ѵ�.
*/
void GCoonsMesh::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
}

/*!
*	\brief ��ü�� �������� ���� �޽�(������ ����Ʈ)�� �����Ѵ�.
*/
void GCoonsMesh::CreateMesh()
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
void GCoonsMesh::CreateMeshVert()
{
	// ������ ������ �����, 
	Verts.clear();

	// �� ���� ���ø� ���� ���ϰ�,
	int su = Renderer.NumSampU;
	int sv = Renderer.NumSampV;

	// ������ Coons ��鿡 ���Ͽ�,
	int NumSrf = SrfList.size();
	for (int i = 0; i < NumSrf; ++i)
	{
		// ���ø� ������ ���ͼ�, 
		int NumSampU = Renderer.NumSampU;
		int NumSampV = Renderer.NumSampV;

		// ������� ������ ����Ѵ�.
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
*	\brief	��ü�� �������� ���� �ؽ�ó ����Ʈ�� �����Ѵ�.
*/
void GCoonsMesh::CreateMeshTex()
{
	// ������ �ؼ� ������ �����,
	Texs.clear();
}

/*!
*	\brief	��ü�� �������� ���� �ﰢ�� ����Ʈ�� �����Ѵ�.
*/
void GCoonsMesh::CreateMeshFace()
{
	// �ﰢ�� ������ �����,
	Faces.clear();

	// ���ø� ������ ���ͼ�, 
	int NumSampU = Renderer.NumSampU;
	int NumSampV = Renderer.NumSampV;

	// �ﰢ�� ������ �����Ѵ�.
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
*	\brief	��ü�� �����ϴ� ������ ����Ѵ�.
*
*	\param u �Ű������� ��
*
*	\return ���� ���� ��ȯ�Ѵ�
*/
GVector4 GCoonsMesh::Eval(double u, ...)
{
	// �Ű����� (u, v)�� ���ϰ�,
	double v;
	va_list ap;
	va_start(ap, u);
	v = va_arg(ap, double);
	va_end(ap);

	return GVector4();
}

/*!
*	\brief Coons ��ġ�� �����ϴ� �ڳ� ������ �ε����� �߰��Ѵ�.
*
*	\param vidx �ڳ� ������ �ε���
*/
void GCoonsMesh::AddVertIdx(int vidx)
{
	// ���õ� ��� ������ �ε����� �߰��ϰ�,
	VertIdx.push_back(vidx);

	if (VertIdx.size() == 4)
	{
		// 4������ �ε�����
		int vidx0 = VertIdx[0];
		int vidx1 = VertIdx[1];
		int vidx2 = VertIdx[2];
		int vidx3 = VertIdx[3];
		
		// 4���� ������ ���ϰ�,
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

		// ���õ� ������ �ε����� �����Ѵ�.
		VertIdx.clear();		
	}
}

/*!
*	\brief Coons ��ġ�� �����ϴ� �ڳ� ������ �ε����� �߰��Ѵ�.
*
*	\param vidx �ڳ� ������ �ε���
*
*	\return ������ GeoMesh�� ��ȯ�Ѵ�.
*/
GMesh *GCoonsMesh::AddVertIdx2(int vidx)
{
	// ���õ� ��� ������ �ε����� �߰��ϰ�,
	VertIdx.push_back(vidx);

	// ���� ������ ������ �ε����� �߰��Ǿ��ٸ�,
	if (VertIdx.size() == 4)
	{
		// 4���� ������ �ε�����
		int vidx0 = VertIdx[0];
		int vidx1 = VertIdx[1];
		int vidx2 = VertIdx[2];
		int vidx3 = VertIdx[3];

		// 4���� ������ ���ϰ�,
		GVertex *P = &(pRefMesh->Verts[vidx0]);
		GVertex *Q = &(pRefMesh->Verts[vidx1]);
		GVertex *R = &(pRefMesh->Verts[vidx2]);
		GVertex *S = &(pRefMesh->Verts[vidx3]);

		// ���� P�� �������� ���� ����ǥ�� ����Ѵ�.
		std::vector<double> Distances;
		std::vector<double> Angles;
		double MaxRadius = dist(P->P, R->P) * 1.5;
		::get_geodesic_polar_coords(pRefMesh, P, Distances, Angles, MaxRadius);

		// �������� �ﰢ����� 2���� �޽��� �����Ѵ�.
		GMesh *pSubMesh = new GMesh("SubMesh", GTransf(), FACE_TRI, "");
		std::map<int, int> FaceIdxMap;

		int NumFace = pRefMesh->GetNumFace();
		for (int i = 0; i < NumFace; ++i)
		{
			int vidx0 = pRefMesh->Faces[i].vIdx[0];
			int vidx1 = pRefMesh->Faces[i].vIdx[1];
			int vidx2 = pRefMesh->Faces[i].vIdx[2];

			// �������� �ﰢ���̶��,
			if (Distances[vidx0] < 100000.0 && Distances[vidx1] < 100000.0 && Distances[vidx2] < 100000.0)
			{
				GPoint3 p0(Distances[vidx0] * cos(Angles[vidx0]), Distances[vidx0] * sin(Angles[vidx0]), 0.0);
				GPoint3 p1(Distances[vidx1] * cos(Angles[vidx1]), Distances[vidx1] * sin(Angles[vidx1]), 0.0);
				GPoint3 p2(Distances[vidx2] * cos(Angles[vidx2]), Distances[vidx2] * sin(Angles[vidx2]), 0.0);

				// ������ �߰��ϰ�,
				pSubMesh->AddVertex(GVertex(p0));
				pSubMesh->AddVertex(GVertex(p1));
				pSubMesh->AddVertex(GVertex(p2));

				// �ﰢ���� �߰��ϰ�,
				int NumVert = pSubMesh->GetNumVert();
				pSubMesh->AddFace(GFace(NumVert - 3, NumVert - 2, NumVert - 1));

				// �ﰢ���� ���� ������ �����Ѵ�.
				FaceIdxMap[pSubMesh->GetNumFace() - 1] = i;
			}
		}
		pSubMesh->UpdateBndBox(false);

		// 2D Ž���� ȿ�������� �ϱ� ���� QuadTree�� �����Ѵ�.
		GQTree QuadTree(pSubMesh, 7, 10);

		// ���� ���� ����ǥ�� �̿��Ͽ�, 2���� ��鿡�� PQ, SR�� ���� ���� ����Ѵ�.
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

		// ������ �޽��� �����Ѵ�.
		std::string Name = create_mesh_name("GeoMesh");
		GMesh *pGeoMesh = new GMesh(Name, GTransf(), FACE_QUAD, this->Name);
		pGeoMesh->Renderer.RenderType = RENDER_POLYGON | RENDER_WIRE | RENDER_BOUNDARY;
		
		// ������ ����Ʈ�� �����Ѵ�.
		int snumu = Renderer.NumSampU;
		int snumv = Renderer.NumSampV;
		for (int i = 0; i < snumu; ++i)
		{
			// U �������� ISO ������ ���Ͽ�,
			double u = (double) i / (double)(snumu - 1);
			GLine tmp(l_PQ.Eval(u), l_SR.Eval(u));

			for (int j = 0; j < snumv; ++j)
			{
				// V �������� �������� ������ ����ϰ�,
				double v = (double) j / (double)(snumv - 1);
				GPoint3 pt = tmp.Eval(v);
				GVector2 p = GVector2(pt[0], pt[1]);

				// ���� ���� p�� ���Ե� �ﰢ���� �ε����� barycentric ��ǥ�� ����ϰ�,
				double alpha, beta;
				int fidx = QuadTree.FindFace(p, &alpha, &beta);
				if (fidx != -1)
				{
					fidx = FaceIdxMap[fidx];
					int vidx0 = pRefMesh->Faces[fidx].vIdx[0];
					int vidx1 = pRefMesh->Faces[fidx].vIdx[1];
					int vidx2 = pRefMesh->Faces[fidx].vIdx[2];

					// 3���� ��ü�� ��ǥ�� �����Ͽ� ������ �޽��� �߰��Ѵ�.
					GPoint3 p0 = pRefMesh->Verts[vidx0].P;
					GPoint3 p1 = pRefMesh->Verts[vidx1].P;
					GPoint3 p2 = pRefMesh->Verts[vidx2].P;
					GPoint3 p3 = p0 + alpha * (p1 - p0) + beta * (p2 - p0);
					pGeoMesh->AddVertex(GVertex(p3, GVector3()));
				}
			}
		}

		// �ﰢ�� ����Ʈ�� �����Ѵ�.
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
		
		// ���õ� ������ �޽��� �����Ѵ�.
		delete pSubMesh;

		// ���õ� ������ �ε����� �����Ѵ�.
		VertIdx.clear();

		// ������ ���� �޽��� ��ȯ�Ѵ�.
		return pGeoMesh;
	}
	else
		return NULL;
}

