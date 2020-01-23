#include "stdafx.h"

// �ܺ� ���� ����
extern double CBS_BASIS[1001][4];
extern double CBS_BASIS_DERIV[1001][4];

/*************************************/
/* Implementation of GUcbsSrf1D Class */
/*************************************/
IMPLEMENT_GMESH_OBJECT(GUcbsSrf1D)

/*!
*	\brief ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GUcbsSrf1D::GetRefMesh()
{
	return this;
}

/*!
*	\brief ���� ��ǥ�迡�� ǥ���� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ǥ���� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*/
GTransf GUcbsSrf1D::GetTransf()
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
void GUcbsSrf1D::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);	
}

/*!
*	\brief ���õ� ��ü�� ������ �Ѵ�.
*/
void GUcbsSrf1D::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);
}

/*!
*	\brief	�ҽ� ����� ������ ���� ������ �迭�� �������� �Ҵ�
*	
*	\param _m ����� U-���� �������� ������ �ε���
*	\param _n ����� V-���� �������� ������ �ε���
*/
void GUcbsSrf1D::_alloc(int _m, int _n)
{
	m = _m; 
	n = _n;
	ARR_ALLOC(P, double, (m + 1) * (n + 1));
}

/*!
*	\brief	�ҽ������ �������� ���� �迭�� ����
*/
void GUcbsSrf1D::_free()
{
	ARR_FREE(P);
}

/*!
*	\brief	������
*
*	\param _Name ����� �̸�
*	\param _MC ����� �𵨸� ��ǥ��
*	\param _m ����� U-���� �������� ������ �ε���
*	\param _n ����� V-���� �������� ������ �ε���
*	\param _P ����� ������ �迭
*	\param _ParentName �θ� �޽��� �̸�
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
*	\brief	���� ������
*
*	\param cpy ����� ��ü
*/
GUcbsSrf1D::GUcbsSrf1D(const GUcbsSrf1D &cpy)
: GMesh(cpy)
{
	_alloc(cpy.m, cpy.n);
	ARR_COPY(P, cpy.P, (m + 1) * (n + 1));
}

/*!
*	\brief	�Ҹ���
*/
GUcbsSrf1D::~GUcbsSrf1D()
{
	_free();
}

/*!
*	\brief	����� ���Կ����� �ߺ�
*
*	\param rhs ���Ե� �ҽ���� ��ü
*
*	\return ���Ե� ����� ��ȯ�Ѵ�
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
*	\brief	����� ������ �Ѵ�.
*
*	\param bSubMesh ���� �޽��� ������ ����(���� ��忡���� false�� ����)
*/
void GUcbsSrf1D::Render(bool bSubMesh)
{
	GMesh::Render(bSubMesh);
}

/*!
*	\brief	����� �����Ѵ�.
*/
void GUcbsSrf1D::CreateMesh()
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
*	\brief	����� �������� ���� ������ ����Ʈ�� �����Ѵ�.
*/
void GUcbsSrf1D::CreateMeshVert()
{
	// ���ø� ������ ���ͼ�, 
	int NumSampU = Renderer.NumSampU;
	int NumSampV = Renderer.NumSampV;
	double umin = 0.0, umax = 1.0, vmin = 0.0, vmax = 1.0;

	// ������ ������ �����, 
	Verts.clear();

	// ���ǿ��� ����ϰ�,
	GetDomain(&umin, &umax, &vmin, &vmax);

	// ������� ������ ����Ѵ�.
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
*	\brief	����� �������� ���� �ؽ�ó ��ǥ�� �����Ѵ�.
*/
void GUcbsSrf1D::CreateMeshTex()
{
	// ���ø� ������ ���ͼ�, 
	int NumSampU = Renderer.NumSampU;
	int NumSampV = Renderer.NumSampV;

	// �ؽ�ó ��ǥ�� �����ϰ� ������
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
*	\brief	�ҽ������ �������� ���� �ﰢ�� ����Ʈ�� �����Ѵ�.
*/
void GUcbsSrf1D::CreateMeshFace()
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
*	\brief	������� �� S(u, v)�� ���
*
*	\param u U-���� �Ű������� ��
*
*	\return ������� �� S(u, v)�� ��ġ�� ���
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
*	\brief	������� �� S(u, v)�� ���
*
*	\param u U-���� �Ű������� ��
*
*	\return ������� �� S(u, v)�� ��ġ�� ���
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
*	\brief ����� �������� ��ȯ�Ѵ�.
*
*	\param min U ���� �������� ���� ���� ����ȴ�.
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
*	\brief	������ ������ ����Ʈ�� ��ȯ�Ѵ�.
*
*	\param CtrlPtList ������ ������ ����Ʈ�� ����ȴ�.
*/
void GUcbsSrf1D::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
// 	for (int i = 0; i < (m + 1) * (n + 1); ++i)
// 		CtrlPtList.push_back(&P[i]);
}

/*!
*	\brief	����� refinement �� �����Ѵ�.
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
*	\brief �Լ� ���� �� �� S(u,v)�� �־��� df ��ŭ ���ϵ��� �Լ��� ���������� ��ġ�� ����
*
*	\param u ������� ���� ���� U-���� �Ű�������
*	\param v ������� ���� ���� V-���� �Ű�������
*	\param df �Լ����� ��Į�� ����
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
*	\brief	�޽��� �𵨸� ��ǥ�踦 ���� ��ǥ��� �����Ѵ�.
*
*	\param RefType �𵨸� ��ǥ���� ������ ���� ���� ����(0: ���� ��ǥ��, 1: ������ ��ǥ��)
*/
void GUcbsSrf1D::UpdateMC(int RefType)
{
}

/*!
*	\brief	��� �޽��� ���� ����Ʈ�� �ٽ� �����Ѵ�.
*/
void GUcbsSrf1D::UpdateMesh() 
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

/*************************************/
/* Implementation of GUcbsSrf Class */
/*************************************/
IMPLEMENT_GMESH_OBJECT(GUcbsSrf)

/*!
*	\brief ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GUcbsSrf::GetRefMesh()
{
	return this;
}

/*!
*	\brief ���� ��ǥ�迡�� ǥ���� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ǥ���� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*/
GTransf GUcbsSrf::GetTransf()
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
void GUcbsSrf::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);	
}

/*!
*	\brief	�޽��� �𵨸� ��ǥ�踦 ���� ��ǥ��� �����Ѵ�.
*
*	\param RefType �𵨸� ��ǥ���� ������ ���� ���� ����(0: ���� ��ǥ��, 1: ������ ��ǥ��)
*/
void GUcbsSrf::UpdateMC(int RefType)
{
}

/*!
*	\brief	��� �޽��� ���� ����Ʈ�� �ٽ� �����Ѵ�.
*/
void GUcbsSrf::UpdateMesh() 
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
void GUcbsSrf::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);		
}

/*!
*	\brief	�ҽ� ����� ������ ���� ������ �迭�� �������� �Ҵ�
*	
*	\param _m ����� U-���� �������� ������ �ε���
*	\param _n ����� V-���� �������� ������ �ε���
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
*	\brief	�ҽ������ �������� ���� �迭�� ����
*/
void GUcbsSrf::_free()
{
	ARR_FREE(P);
}

/*!
*	\brief	������
*
*	\param _Name ����� �̸�
*	\param _MC ����� �𵨸� ��ǥ��
*	\param _m ����� U-���� �������� ������ �ε���
*	\param _n ����� V-���� �������� ������ �ε���
*	\param _P ����� ������ �迭
*	\param _ParentName �θ� �޽��� �̸�
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
*	\brief	���� ������
*
*	\param cpy ����� ��ü
*/
GUcbsSrf::GUcbsSrf(const GUcbsSrf &cpy)
: GMesh(cpy)
{
	_alloc(cpy.m, cpy.n);
	for (int i = 0; i < (m + 1) * (n + 1); ++i)
		P[i] = cpy.P[i];
}

/*!
*	\brief	�Ҹ���
*/
GUcbsSrf::~GUcbsSrf()
{
	_free();
}

/*!
*	\brief	����� ���Կ����� �ߺ�
*
*	\param rhs ���Ե� �ҽ���� ��ü
*
*	\return ���Ե� ����� ��ȯ�Ѵ�
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
*	\brief	����� ������ �Ѵ�.
*
*	\param bSubMesh ���� �޽��� ������ ����(���� ��忡���� false�� ����)
*/
void GUcbsSrf::Render(bool bSubMesh)
{
	GMesh::Render(bSubMesh);

	glPushMatrix();
	{
		// ���� ��ǥ�迡�� ��ü�� �𵨸� ��ǥ����� ��ȯ�ϰ�,
		glMultMatrixd(cast_mat(MC).M);

		// ����� �������� �������� ���,
		if (Renderer.RenderType & RENDER_CTRL_PT)
		{
			double size = Renderer.CtrlPtSize;
			
			// ������ �����ϰ�,
			GMaterial Mat;
			Mat.SetAmbient(0.7f, 0.7f, 0.7f);
			Mat.SetDiffuse(0.7f, 0.7f, 0.7f);
			Mat.SetSpecular(0.7f, 0.7f, 0.7f);
			Mat.Enable(false);

			// ��� �������� �������ϰ�,
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

			// ���� �������� ����������, 
			Mat.SetAmbient(0.7f, 0.0f, 0.0f);
			Mat.SetDiffuse(0.7f, 0.0f, 0.0f);
			Mat.SetSpecular(0.7f, 0.0f, 0.0f);
			Mat.Enable(false);
			glPushMatrix();
			glTranslated(P[0][0], P[0][1], P[0][2]);
			glutSolidSphere(size, 10, 10);
			glPopMatrix();

			// �� �������� �Ķ�������, 
			Mat.SetAmbient(0.0f, 0.0f, 0.7f);
			Mat.SetDiffuse(0.0f, 0.0f, 0.7f);
			Mat.SetSpecular(0.0f, 0.0f, 0.7f);
			Mat.Enable(false);
			glPushMatrix();
			glTranslated(P[m * (n + 1) + n][0], P[m * (n + 1) + n][1], P[m * (n + 1) + n][2]);
			glutSolidSphere(size, 10, 10);
			glPopMatrix();

			// ���� �޽��� ������ �Ѵ�.
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
*	\brief	����� �����Ѵ�.
*/
void GUcbsSrf::CreateMesh()
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
*	\brief	����� �������� ���� ������ ����Ʈ�� �����Ѵ�.
*/
void GUcbsSrf::CreateMeshVert()
{
	// ���ø� ������ ���ͼ�, 
	int NumSampU = Renderer.NumSampU;
	int NumSampV = Renderer.NumSampV;
	double umin = 0.0, umax = 1.0, vmin = 0.0, vmax = 1.0;

	// ������ ������ �����, 
	Verts.clear();

	// ���ǿ��� ����ϰ�,
	GetDomain(&umin, &umax, &vmin, &vmax);

	// ������� ������ ����Ѵ�.
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
*	\brief	����� �������� ���� �ؽ�ó ��ǥ�� �����Ѵ�.
*/
void GUcbsSrf::CreateMeshTex()
{
	// ���ø� ������ ���ͼ�, 
	int NumSampU = Renderer.NumSampU;
	int NumSampV = Renderer.NumSampV;

	// �ؽ�ó ��ǥ�� �����ϰ� ������
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
*	\brief	�ҽ������ �������� ���� �ﰢ�� ����Ʈ�� �����Ѵ�.
*/
void GUcbsSrf::CreateMeshFace()
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
*	\brief	������� �� S(u, v)�� ���
*
*	\param u U-���� �Ű������� ��
*
*	\return ������� �� S(u, v)�� ��ġ�� ���
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
*	\brief	������� �� S(u, v)�� ���
*
*	\param u U-���� �Ű������� ��
*
*	\return ������� �� S(u, v)�� ��ġ�� ���
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
*	\brief ����� �������� ��ȯ�Ѵ�.
*
*	\param min U ���� �������� ���� ���� ����ȴ�.
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
*	\brief	����� ������ ����Ʈ�� ��ȯ�Ѵ�.
*
*	\return ����� ������ ����Ʈ�� ��ȯ�Ѵ�.
*/
void GUcbsSrf::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
	for (int i = 0; i < (m + 1) * (n + 1); ++i)
		CtrlPtList.push_back(&P[i]);
}

/*!
*	\brief	������� �� S(u, v)�� �����(Tangent Plane)�� ���� Su, Sv�� ����
*	
*	\param u U-���� �Ű����� ��
*	\param v V-���� �Ű����� ��
*	\param Su U-���� 1�� �̺� ���Ͱ��� �����
*	\param Sv V-���� 1�� �̺� ���Ͱ��� �����
*	\param N ����� ���� ���Ͱ� �����
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

	// Su, Sv ���
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
*	\brief	������� �� S(u, v)�� �����(Tangent Plane)�� ���� Su, Sv�� ����
*	
*	\param u U-���� �Ű����� ��
*	\param v V-���� �Ű����� ��
*	\param Su U-���� 1�� �̺� ���Ͱ��� �����
*	\param Sv V-���� 1�� �̺� ���Ͱ��� �����
*	\param N ����� ���� ���Ͱ� �����
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

	// Su, Sv ���
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
*	\brief	����� refinement(binary subdivision)�� �����Ѵ�.
*/
void GUcbsSrf::Refine()
{
	// ���ο� ������ �迭�� �����ϰ� �ʱ�ȭ �Ѵ�.
	int mm = 2 * (m - 1);
	int nn = 2 * (n - 1);
	int num_ctlpt = (mm + 1) * (nn + 1);
	GCtrlPt *P1 = new GCtrlPt [num_ctlpt];
	for (int i = 0; i < num_ctlpt; ++i)
	{
		P1[i].pMesh = this;
		P1[i].Set(0.0, 0.0, 0.0, 0.0);
	}
	
	// ���� ������ �迭�� P0�� �ϰ�, ���ο� �������� ����Ѵ�.
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
*	\brief ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GUcbsVol1D::GetRefMesh()
{
	return this;
}

/*!
*	\brief ���� ��ǥ�迡�� ǥ���� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ǥ���� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*/
GTransf GUcbsVol1D::GetTransf()
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
void GUcbsVol1D::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);	
}

/*!
*	\brief	�޽��� �𵨸� ��ǥ�踦 ���� ��ǥ��� �����Ѵ�.
*
*	\param RefType �𵨸� ��ǥ���� ������ ���� ���� ����(0: ���� ��ǥ��, 1: ������ ��ǥ��)
*/
void GUcbsVol1D::UpdateMC(int RefType)
{
}

/*!
*	\brief	���� �޽��� ���� ����Ʈ�� �ٽ� �����Ѵ�.
*/
void GUcbsVol1D::UpdateMesh() 
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
void GUcbsVol1D::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);	
}

/*!
*	\brief	�ҽ� ������ ������ ���� ������ �迭�� �������� �Ҵ�
*	
*	\param _l ����� U-���� �������� ������ �ε���
*	\param _m ����� V-���� �������� ������ �ε���
*	\param _n ����� W-���� �������� ������ �ε���
*/
void GUcbsVol1D::_alloc(int _l, int _m, int _n)
{
	l = _l;
	m = _m; 
	n = _n;
	ARR_ALLOC(P, double, (l + 1) * (m + 1) * (n + 1));
}

/*!
*	\brief	�ҽ������ �������� ���� �迭�� ����
*/
void GUcbsVol1D::_free()
{
	ARR_FREE(P);
}

/*!
*	\brief	������
*
*	\param _Name ������ �̸�
*	\param _MC ����� �𵨸� ��ǥ��
*	\param _l ������ U-���� �������� ������ �ε���
*	\param _m ������ V-���� �������� ������ �ε���
*	\param _n ������ W-���� �������� ������ �ε���
*	\param _P ������ ������ �迭
*	\param _ParentName �θ� ������ �̸�
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
*	\brief	���� ������
*
*	\param cpy ����� ��ü
*/
GUcbsVol1D::GUcbsVol1D(const GUcbsVol1D &cpy)
: GMesh(cpy)
{
	_alloc(cpy.l, cpy.m, cpy.n);
	ARR_COPY(P, cpy.P, (l + 1) * (m + 1) * (n + 1));
}

/*!
*	\brief	�Ҹ���
*/
GUcbsVol1D::~GUcbsVol1D()
{
	_free();
}

/*!
*	\brief	����� ���Կ����� �ߺ�
*
*	\param rhs ���Ե� �ҽ���� ��ü
*
*	\return ���Ե� ����� ��ȯ�Ѵ�
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
*	\brief ������ ǥ���� ������ �Ѵ�.
*
*	\param bSubMesh ���� �޽��� ������ ����(���� ��忡���� false�� ����)
*/
void GUcbsVol1D::Render(bool bSubMesh)
{
	GMesh::Render(bSubMesh);
}

/*!
*	\brief ������ ǥ�� �޽��� �����Ѵ�.
*/
void GUcbsVol1D::CreateMesh()
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
*	\brief ������ ǥ�� �޽��� ������ �����Ѵ�.
*/
void GUcbsVol1D::CreateMeshVert()
{
}

/*!
*	\brief ������ ǥ�� �޽��� �ؽ�ó ��ǥ�� �����Ѵ�.
*/
void GUcbsVol1D::CreateMeshTex()
{
}

/*!
*	\brief ������ ǥ�� �޽��� �ﰢ���� �����Ѵ�.
*/
void GUcbsVol1D::CreateMeshFace()
{
}

/*!
*	\brief	�������� �� V(u, v, w)�� ���
*
*	\param u U-���� �Ű������� ��
*
*	\return �������� �� V(u, v, w)�� ��ġ�� ���
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
*	\brief ����� �������� ��ȯ�Ѵ�.
*
*	\param min U ���� �������� ���� ���� ����ȴ�.
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
*	\brief	������ ������ ����Ʈ�� ��ȯ�Ѵ�.
*
*	\return ������ ������ ����Ʈ�� ��ȯ�Ѵ�.
*/
void GUcbsVol1D::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
	// 	for (int i = 0; i < (m + 1) * (n + 1); ++i)
	// 		CtrlPtList.push_back(&P[i]);
}

/*!
*	\brief	����� refinement �� �����Ѵ�.
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
	for (int i = 0; i < ll + 1; i += 2)	// U-����
	{
		for (int j = 0; j < mm + 1; j += 2)	// V-����
		{
			for (int k = 0; k < nn + 1; k += 2)	// W-����
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
*	\brief ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GUcbsVol::GetRefMesh()
{
	return this;
}

/*!
*	\brief ���� ��ǥ�迡�� ǥ���� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ǥ���� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*/
GTransf GUcbsVol::GetTransf()
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
void GUcbsVol::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);	
}

/*!
*	\brief	�޽��� �𵨸� ��ǥ�踦 ���� ��ǥ��� �����Ѵ�.
*
*	\param RefType �𵨸� ��ǥ���� ������ ���� ���� ����(0: ���� ��ǥ��, 1: ������ ��ǥ��)
*/
void GUcbsVol::UpdateMC(int RefType)
{
}

/*!
*	\brief	���� �޽��� ���� ����Ʈ�� �ٽ� �����Ѵ�.
*/
void GUcbsVol::UpdateMesh() 
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
void GUcbsVol::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);		
}

/*!
*	\brief	�ҽ� ������ ������ ���� ������ �迭�� �������� �Ҵ�
*	
*	\param _l ����� U-���� �������� ������ �ε���
*	\param _m ����� V-���� �������� ������ �ε���
*	\param _n ����� W-���� �������� ������ �ε���
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
*	\brief	�ҽ������ �������� ���� �迭�� ����
*/
void GUcbsVol::_free()
{
	if (P != NULL)
		delete [] P;
	P = NULL;
}

/*!
*	\brief	������
*
*	\param _Name ������ �̸�
*	\param _MC ����� �𵨸� ��ǥ��
*	\param _l ������ U-���� �������� ������ �ε���
*	\param _m ������ V-���� �������� ������ �ε���
*	\param _n ������ W-���� �������� ������ �ε���
*	\param _P ������ ������ �迭
*	\param _ParentName �θ� ������ �̸�
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
*	\brief	���� ������
*
*	\param cpy ����� ��ü
*/
GUcbsVol::GUcbsVol(const GUcbsVol &cpy)
: GMesh(cpy)
{
	_alloc(cpy.l, cpy.m, cpy.n);
	for (int i = 0; i < (l + 1) * (m + 1) * (n + 1); ++i)
		P[i] = cpy.P[i];
}

/*!
*	\brief	�Ҹ���
*/
GUcbsVol::~GUcbsVol()
{
	_free();
}

/*!
*	\brief	������ ���Կ����� �ߺ�
*
*	\param rhs ���Ե� �ҽ����� ��ü
*
*	\return ���Ե� ������ ��ȯ�Ѵ�
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
*	\brief ������ ǥ���� ������ �Ѵ�.
*
*	\param bSubMesh ���� �޽��� ������ ����(���� ��忡���� false�� ����)
*/
void GUcbsVol::Render(bool bSubMesh)
{
	GMesh::Render(bSubMesh);

	glPushMatrix();
	{
		// ���� ��ǥ�迡�� ��ü�� �𵨸� ��ǥ����� ��ȯ�ϰ�,
		glMultMatrixd(cast_mat(MC).M);

		// ����� �������� �������� ���,
		if (Renderer.RenderType & RENDER_CTRL_PT)
		{
			double size = Renderer.CtrlPtSize;

			// ������ �����ϰ�,
			GMaterial Mat;
			Mat.SetAmbient(0.7f, 0.7f, 0.7f);
			Mat.SetDiffuse(0.7f, 0.7f, 0.7f);
			Mat.SetSpecular(0.7f, 0.7f, 0.7f);
			Mat.Enable(false);

			// ��� �������� �������ϰ�,
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

			// ���� �������� ����������, 
			Mat.SetAmbient(0.7f, 0.0f, 0.0f);
			Mat.SetDiffuse(0.7f, 0.0f, 0.0f);
			Mat.SetSpecular(0.7f, 0.0f, 0.0f);
			Mat.Enable(false);
			glPushMatrix();
			glTranslated(P[0][0], P[0][1], P[0][2]);
			glutSolidSphere(size, 10, 10);
			glPopMatrix();

			// �� �������� �Ķ�������, 
			Mat.SetAmbient(0.0f, 0.0f, 0.7f);
			Mat.SetDiffuse(0.0f, 0.0f, 0.7f);
			Mat.SetSpecular(0.0f, 0.0f, 0.7f);
			Mat.Enable(false);
			glPushMatrix();
			glTranslated(P[(m + 1) * (n + 1) * (l + 1)][0], P[(m + 1) * (n + 1) * (l + 1)][1], P[(m + 1) * (n + 1) * (l + 1)][2]);
			glutSolidSphere(size, 10, 10);
			glPopMatrix();

			// ���� �޽��� ������ �Ѵ�.
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
*	\brief ������ ǥ�� �޽��� �����Ѵ�.
*/
void GUcbsVol::CreateMesh()
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
*	\brief ������ ǥ�� �޽��� ������ �����Ѵ�.
*/
void GUcbsVol::CreateMeshVert()
{
	// ������ ������ �����, 
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

	// u = umin ����� ���� �����ϰ�,
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

	// u = umax ����� ���� �����ϰ�,
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

	// v = vmin ����� ������ �����ϰ�,
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

	// v = vmax ����� ������ �����ϰ�,
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

	// w = wmin ����� ������ �����ϰ�,
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

	// w = wmax ����� ������ �����ϰ�,
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
*	\brief ������ ǥ�� �޽��� �ؽ�ó ��ǥ�� �����Ѵ�.
*/
void GUcbsVol::CreateMeshTex()
{
}

/*!
*	\brief ������ ǥ�� �޽��� �ﰢ���� �����Ѵ�.
*/
void GUcbsVol::CreateMeshFace()
{
	// �ﰢ�� ������ �����,
	Faces.clear();

	int BaseIdx = 0;
	int SNumU = Renderer.NumSampU;
	int SNumV = Renderer.NumSampV;
	int SNumW = Renderer.NumSampW;

	// u = umin ����� �ﰢ���� �����ϰ�,
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

	// u = umax ����� �ﰢ���� �����ϰ�,
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

	// v = vmin ����� �ﰢ���� �����ϰ�,
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

	// v = vmax ����� �ﰢ���� �����ϰ�,
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

	// w = wmin ����� �ﰢ���� �����ϰ�,
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

	// w = wmax ����� �ﰢ���� �����ϰ�,
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
*	\brief ����� �������� ��ȯ�Ѵ�.
*
*	\param min U ���� �������� ���� ���� ����ȴ�.
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
*	\brief	������ ������ ����Ʈ�� ��ȯ�Ѵ�.
*
*	\return ������ ������ ����Ʈ�� ��ȯ�Ѵ�.
*/
void GUcbsVol::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
	for (int i = 0; i < (l + 1) * (m + 1) * (n + 1); ++i)
		CtrlPtList.push_back(&P[i]);
}

/*!
*	\brief	�������� �� V(u, v, w)�� ���
*
*	\param u U-���� �Ű������� ��
*
*	\return �������� �� V(u, v, w)�� ��ġ�� ���
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
*	\brief	����� refinement �� �����Ѵ�.
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
	for (int i = 0; i < ll + 1; i += 2)	// U-����
	{
		for (int j = 0; j < mm + 1; j += 2)	// V-����
		{
			for (int k = 0; k < nn + 1; k += 2)	// W-����
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
