#include "stdafx.h"

/*************************************/
/* Implementation of GTriBzrSrf Class */
/*************************************/
IMPLEMENT_GMESH_OBJECT(GTriBzrSrf)

/*!
*	\brief ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GTriBzrSrf::GetRefMesh()
{
	return this;
}

/*!
*	\brief ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ǥ���� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*/
GTransf GTriBzrSrf::GetTransf()
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
void GTriBzrSrf::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);	
}

/*!
*	\brief	�޽��� �𵨸� ��ǥ�踦 ���� ��ǥ��� �����Ѵ�.
*
*	\param RefType �𵨸� ��ǥ���� ������ ���� ���� ����(0: ���� ��ǥ��, 1: ������ ��ǥ��)
*/
void GTriBzrSrf::UpdateMC(int RefType)
{
}

/*!
*	\brief	��� �޽��� ���� ����Ʈ�� �ٽ� �����Ѵ�.
*/
void GTriBzrSrf::UpdateMesh() 
{
	// ���� ������ ����,
	switch (ModifiedFlag)
	{
		// ������ ����Ǿ��ٸ�,
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
*	\brief ���õ� ��ü�� ������ �Ѵ�.
*/
void GTriBzrSrf::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);
}

/*!
*	\brief ������ ����� ������ �迭�� �Ҵ��Ѵ�.
*
*	\param _p ����
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
*	\brief ������ ����� ������ �迭�� �����Ѵ�.
*/
void GTriBzrSrf::_free()
{
	if (P != NULL)
		delete [] P;
	P = NULL;
}

/*!
*	\brief ������ ����� ������
*
*	\param _Name ����� �̸�
*	\param _MC ����� �𵨸� ��ǥ��
*	\param _p ����
*	\param _P ������ �迭
*	\param _ParentName �θ� ����� �̸�
*/
GTriBzrSrf::GTriBzrSrf(std::string _Name, GTransf _MC, int _p, GVector4 *_P, std::string _ParentName)
: GMesh(_Name, _MC, FACE_TRI, _ParentName)
{
	// ���� 4������ ����
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
*	\brief ������ ����� ���� ������
*
*	\param cpy ����� ��ü
*/
GTriBzrSrf::GTriBzrSrf(const GTriBzrSrf &cpy)
: GMesh(cpy)
{
	_alloc(cpy.p);
	for (int i = 0; i < n + 1; ++i)
		P[i] = cpy.P[i];
}

/*!
*	\brief ������ ����� �Ҹ���
*/
GTriBzrSrf::~GTriBzrSrf()
{
	_free();
}

/*!
*	\brief ������ ����� ���� ������
*
*	\param rhs ���Ե� ��ü
*
*	\return ���Ե� �ڽ��� ��ȯ�Ѵ�.
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
*	\brief ����� ������ �Ѵ�.
*
*	\param bSubMesh ���� �޽��� ������ ����(���� ��忡���� false�� ����)
*/
void GTriBzrSrf::Render(bool bSubMesh)
{
	GMesh::Render(bSubMesh);

	glPushMatrix();
	{
		// ���� ��ǥ�迡�� ��ü�� �𵨸� ��ǥ����� ��ȯ�ϰ�,
		glMultMatrixd(cast_mat(MC).M);

		// �������� ������ �Ѵٸ�,
		if (Renderer.RenderType & RENDER_CTRL_PT)
		{
			// ������ �����ϰ�,
			GMaterial Mat;
			Mat.SetAmbient(0.7f, 0.7f, 0.7f);
			Mat.SetDiffuse(0.7f, 0.7f, 0.7f);
			Mat.SetSpecular(0.7f, 0.7f, 0.7f);
			Mat.Enable(false);

			// �������� ũ�⸦ ���ϰ�,
			double size = Renderer.CtrlPtSize;
			
			// �������� ������ �Ѵ�.
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

			// ������ ������ �����Ͽ�,
			Mat.SetAmbient(0.7f, 0.0f, 0.0f);
			Mat.SetDiffuse(0.7f, 0.0f, 0.0f);
			Mat.SetSpecular(0.7f, 0.0f, 0.0f);
			Mat.Enable(false);

			// ���� �������� ǥ���Ѵ�.
			glPushMatrix();
			glTranslated(P[0][0], P[0][1], P[0][2]);
			glutSolidSphere(size, 10, 10);
			glPopMatrix();

			// ������ ����, �� �β��� 2.0����, ������ �Ķ����� �����ϰ�,
			glDisable(GL_LIGHTING);
			glLineWidth(2.0);
			glColor3d(0.0, 0.0, 1.0);

			// ���� �޽��� �������Ѵ�.
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
			
			// ������ �Ѱ�, �� �β��� 1.0���� �缳���Ѵ�.
			glLineWidth(1.0);
			glEnable(GL_LIGHTING);
		}
	}
	glPopMatrix();
}

/*!
*	\brief �޽��� �����Ѵ�.
*/
void GTriBzrSrf::CreateMesh()
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
*	\brief ������ ��ǥ�� �����Ѵ�.
*/
void GTriBzrSrf::CreateMeshVert()
{
	// ������ ������ ���ϰ�,
	double umin = 0.0, umax = 1.0, vmin = 0.0, vmax = 1.0;
	GetDomain(&umin, &umax, &vmin, &vmax);
	
	// ���ø� ������ ���ͼ�, 
	int snum = Renderer.NumSampU;

	// ������ ������ �����, 
	Verts.clear();
	
	// �ﰢ�� ������ ����� ���� ����
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
*	\brief �ؽ�ó ��ǥ�� �����Ѵ�.
*/ 
void GTriBzrSrf::CreateMeshTex()
{
	// ���ø� ������ ���ͼ�, 
	int snum = Renderer.NumSampU;

	// �ﰢ�� ������ ����� �ؽ�ó ��ǥ�� �����Ѵ�.
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
*	\brief �ﰢ�� ������ �����Ѵ�.
*/ 
void GTriBzrSrf::CreateMeshFace()
{
	// �ﰢ�� ������ �����,
	Faces.clear();

	// ���ø� ������ ���ͼ�, 
	int snum = Renderer.NumSampU;

	// �ﰢ�� ������ �����Ѵ�.
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
*	\brief ������ ������� ���� ����Ѵ�.
*
*	\param u �Ķ����
*
*	\return ������ ������� �� S(u, v, w)�� ��ȯ�Ѵ�.
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
*	\brief ������ ������� �������͸� ����Ѵ�.
*
*	\param (u, v, w) �Ķ����
*
*	\return ������ ������� ���� ���� N(u, v, w)�� ��ȯ�Ѵ�.
*/
GVector3 GTriBzrSrf::EvalNormal(double u, double v, double w)
{
	GVector4 Du, Dv;

	static GCtrlPt tmp[66];
	
	if (p == 3)
	{
		// ������ ����
		ARR_COPY(tmp, P, n + 1);

		// U-���� ���� ���� ���ϰ�,
		tmp[0] = tmp[0] - tmp[2];
		tmp[2] = tmp[2] - tmp[5];
		tmp[5] = tmp[5] - tmp[9];
		
		tmp[1] = tmp[1] - tmp[4];
		tmp[4] = tmp[4] - tmp[8];
		
		tmp[3] = tmp[3] - tmp[7];

		// ù��° �ݺ�
		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
		tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];

		// �ι�° �ݺ�
		Du = u * tmp[0] + v * tmp[1] + w * tmp[2];

		// ������ ����
		ARR_COPY(tmp, P, n + 1);

		// V-���� ���� ���� ���ϰ�,
		tmp[0] = tmp[1] - tmp[2];
		tmp[1] = tmp[3] - tmp[4];
		tmp[2] = tmp[4] - tmp[5];

		tmp[3] = tmp[6] - tmp[7];
		tmp[4] = tmp[7] - tmp[8];

		tmp[5] = tmp[8] - tmp[9];

		// ù��° �ݺ�
		tmp[0] = u * tmp[0] + v * tmp[1] + w * tmp[2];
		tmp[1] = u * tmp[1] + v * tmp[3] + w * tmp[4];
		tmp[2] = u * tmp[2] + v * tmp[4] + w * tmp[5];

		// �ι�° �ݺ�
		Dv = u * tmp[0] + v * tmp[1] + w * tmp[2];
	}

	GVector3 ret = cast_vec3(Du) ^ cast_vec3(Dv);
	ret.Normalize();

	return ret;
}

/*!
*	\brief ������ ����� �Ķ���� ���� [umin, umax] x [vmin, vmax]�� ���Ѵ�.
*
*	\param umin U-���� �Ķ������ ���� ���� �����
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
*	\brief	����� ������ ����Ʈ�� ��ȯ�Ѵ�.
*
*	\return ����� ������ ����Ʈ�� ��ȯ�Ѵ�.
*/
void GTriBzrSrf::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
	for (int i = 0; i < n + 1; ++i)
		CtrlPtList.push_back(&P[i]);
}

/*!
*	\brief ������ ����� �� �κ����� �����Ѵ�.
*
*	\param u �Ķ����
*	\param v �Ķ����
*	\param w �Ķ����
*
*	\return ����� �� �������� �� S(u, v, w)�� �������� ���ҵ� �� ����� ��ȯ�Ѵ�.
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

