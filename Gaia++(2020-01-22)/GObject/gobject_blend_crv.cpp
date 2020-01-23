#include "stdafx.h"

/*************************************/
/* Implementation of GNurbsCrv Class */
/*************************************/
IMPLEMENT_GMESH_OBJECT(GBlendCrv)

/*!
*	\brief ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GBlendCrv::GetRefMesh()
{
	return this;
}

/*!
*	\brief ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�
*/
GTransf GBlendCrv::GetTransf()
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
void GBlendCrv::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);
}

/*!
*	\brief	�޽��� �𵨸� ��ǥ�踦 ���� ��ǥ��� �����Ѵ�.
*
*	\param RefType �𵨸� ��ǥ���� ������ ���� ���� ����(0: ���� ��ǥ��, 1: ������ ��ǥ��)
*/
void GBlendCrv::UpdateMC(int RefType)
{
}

/*!
*	\brief	��� ������ ��ġ�� �����Ѵ�.
*/
void GBlendCrv::UpdateMesh()
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
*	\brief ���õ� ��ü�� ������ �Ѵ�.
*/
void GBlendCrv::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);
}

/*!
*	\brief	�⺻ ������
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
*	\brief	������
*
*	\param _Name ��� �̸�
*	\param _MC ��ü�� �𵨸� ��ǥ��
*	\param _P ��� ������
*	\param _n ��� �������� ������ �ε���
*	\param _ParentName ��� �θ� �޽��� �̸�
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
*	\brief	���� ������
*
*	\param cpy ����� ��ü
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
*	\brief	�Ҹ���
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
*	\brief	���� ������
*
*	\param rhs ���Ե� ��ü
*
*	\return ���Ե� �ڽ��� ��ȯ�Ѵ�.
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
*	\brief	���� ��� ���ǵ� �������� ��ȯ�Ѵ�.
*
*	\param umin �������� ���� ���� ����ȴ�.
*	\param umax �������� �� ���� ����ȴ�.
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
*	\brief	��� ������ ����Ʈ�� ��ȯ�Ѵ�.
*
*	\return ��� ������ ����Ʈ�� ��ȯ�Ѵ�.
*/
void GBlendCrv::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
}

/*!
*	\brief	���� ��� �������� ���� �޽�(������ ����Ʈ)�� �����Ѵ�.
*/
void GBlendCrv::CreateMesh()
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
	UpdateBufFace();
}

/*!
*	\brief	��� �������� ���� ������ ����Ʈ�� �����Ѵ�.
*/
void GBlendCrv::CreateMeshVert()
{
	// ������ ������ �����, 
	Verts.clear();

	// ���ø� ������ ���ͼ�, 
	int NumSampU = Renderer.NumSampU;

	// ���ǿ��� ����ϰ�,
	double umin = 0.0, umax = 1.0;
	GetDomain(&umin, &umax);

	// ������� ������ ����Ͽ�, 
	for (int i = 0; i < NumSampU; ++i)
	{
		double u = umin + (umax - umin) * i / (NumSampU - 1);
		GVector4 pt = Eval(u);
		// �޽��� ���� ����Ʈ�� �߰��Ѵ�.
		AddVertex(GVertex(GPoint3(pt[0], pt[1], pt[2]), GVector3()));
	}
}

/*!
*	\brief	��� �������� ���� �ؽ�ó ����Ʈ�� �����Ѵ�.
*/
void GBlendCrv::CreateMeshTex()
{
	// ������ �ؼ� ������ �����,
	Texs.clear();

	// ���ø� ������ ���ͼ�, 
	int NumSampU = Renderer.NumSampU;

	// �ؽ�ó ��ǥ�� �����ϰ� ������
	for (int i = 0; i < NumSampU; ++i)
	{
		double s = (double)i / (double)(NumSampU - 1);
		AddTexel(GVector2(s, 0.0));
	}
}

/*!
*	\brief	��� �������� ���� �ﰢ�� ����Ʈ�� �����Ѵ�.
*/
void GBlendCrv::CreateMeshFace()
{
	// �ﰢ�� ������ �����,
	Faces.clear();

	// FAN Ÿ�� �޽��� ���ؼ���, 
	if (FaceType == FACE_TRI_FAN)
	{
		int vnum = (int)Verts.size();
		for (int i = 1; i < vnum - 1; ++i)
		{
			// �ﰢ���� �����ϰ�, �޽��� �ﰢ�� ����Ʈ�� �߰��Ѵ�.
			GFace f0(0, i, i + 1);
			AddFace(f0);
		}
	}
}

/*!
*	\brief	���� ��� ������ �Ѵ�.
*
*	\param bSubMesh ���� �޽��� ������ ����(���� ��忡���� false�� ����)
*/
void GBlendCrv::Render(bool bSubMesh)
{
	// �θ� Ŭ���� �Լ��� ȣ���� ��,
	GMesh::Render(bSubMesh);

	glPushMatrix();
	{
		// ���� ��ǥ�迡�� ��ü�� �𵨸� ��ǥ����� ��ȯ�ϰ�,
		glMultMatrixd(cast_mat(MC).M);

		// ������ ��Ȱ��ȭ �ϰ�,
		glDisable(GL_LIGHTING);

		// ��� �β��� ������ ������ ��,
		glLineWidth(2.0);
		int NumMtl = GetNumMtl();
		int MtlIdx = Renderer.CrvMtlIdx;
		if (MtlIdx >= 0 && MtlIdx < NumMtl)
			glColor3fv(MtlList[MtlIdx].Diffuse);
		else
			glColor3f(0.5f, 0.5f, 0.5f);

		// ��� �׷��ְ�,
		glBegin(GL_LINE_STRIP);
		{
			int NumVert = (int)Verts.size();
			for (int i = 0; i < NumVert; ++i)
				glVertex3dv(Verts[i].P.V);
		}
		glEnd();

		// �� �β��� �⺻ ������ �� ������ ��,
		glLineWidth(1.0);

		// ������ Ȱ��ȭ �Ѵ�.
		glEnable(GL_LIGHTING);


		// �������� ������ �� ���,
		if (Renderer.RenderType & RENDER_CTRL_PT)
		{
			// ������ �����ϰ�,
			GMaterial Mtl("mtl_gray");
			Mtl.SetAmbient(0.7f, 0.7f, 0.7f);
			Mtl.SetDiffuse(0.7f, 0.7f, 0.7f);
			Mtl.SetSpecular(0.7f, 0.7f, 0.7f);
			Mtl.SetAlpha(1.0);
			Mtl.Enable(false);

			// �������� �������ϰ�,
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
*	\brief	����� ���� ���
*
*	\param u �Ű������� ��
*
*	\return ����� �� C(u)�� ���
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
*	\brief	��� �������� �����Ѵ�.
*
*	\param idx ������ �������� �ε���
*	\param dp ������ �������� ���� ����
*/
void GBlendCrv::Edit(int idx, GVector4 dp)
{
	P[idx] += dp;	
}
