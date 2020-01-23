#include "stdafx.h"

/****************************************/
/* Implementation of GBlendMesh Class */
/****************************************/
IMPLEMENT_GMESH_OBJECT(GBlendMesh)

/*!
*	\brief ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*
*	\return ��ü�� ���Ե� �޽��� �����͸� ��ȯ�Ѵ�.
*/
GMesh *GBlendMesh::GetRefMesh()
{
	return this;
}

/*!
*	\brief ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ǥ���� ��ü�� �𵨸� ��ǥ�踦 ��ȯ�Ѵ�.
*/
GTransf GBlendMesh::GetTransf()
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
void GBlendMesh::Edit(GTransf EC, GVector3 t, GQuater q, GVector3 s, GMesh *pParentMesh)
{
	GMesh::Edit(EC, t, q, s, pParentMesh);	
}

/*!
*	\brief	�޽��� �𵨸� ��ǥ�踦 ���� ��ǥ��� �����Ѵ�.
*
*	\param RefType �𵨸� ��ǥ���� ������ ���� ���� ����(0: ���� ��ǥ��, 1: ������ ��ǥ��)
*/
void GBlendMesh::UpdateMC(int RefType)
{
}

/*!
*	\brief	��� �޽��� ���� ����Ʈ�� �ٽ� �����Ѵ�.
*/
void GBlendMesh::UpdateMesh() 
{
	switch (ModifiedFlag)
	{
	case MODIFIED_VERT_POS:
		break;

	case MODIFIED_VERT_NUM:		// ������ ��ġ�� ��谡 ����� ���...
		InitBndryEdgeList();
		UpdateNormal(false);
		UpdateBufVertex();
		UpdateBufNormal();
		UpdateBufFace();
		break;

		// ������ ����Ǿ��ٸ�,
	case MODIFIED_MTL:
		UpdateBufFace();
		break;

	case MODIFIED_INTPT_POS:
		CreateMesh();
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
void GBlendMesh::RenderForEdit(float r, float g, float b)
{
	GMesh::RenderForEdit(r, g, b);
}

/*!
*	\brief ������
*
*	\param _Name �̸�
*	\param _MC ��ü�� �𵨸� ��ǥ��
*	\param _pCtrlMesh ���� �޽�
*/
GBlendMesh::GBlendMesh(std::string _Name, GTransf _MC, GMesh *_pCtrlMesh)
: GMesh(_Name, _MC, FACE_TRI, _pCtrlMesh->Name)
{
	// ���� �޽��� �����ϰ�,
	pCtrlMesh = _pCtrlMesh;
	pCtrlMesh->SemanticType = MESH_CTRL;
	pCtrlMesh->LinkedMeshNameList.push_back(Name);

	// �ﰢ���� ������ ���Ͽ�,
	int NumFace = pCtrlMesh->GetNumFace();

	// �� �ﰢ������ ���ǵǴ� ��ġ�� �ʱ�ȭ�Ѵ�.
	PatchList.assign(NumFace, GTriBzrSrf("", GTransf()));

	// �� �ﰢ������ ���ǵǴ� ���� �������� �ʱ�ȭ �Ѵ�.
	BlendOffset.assign(NumFace, GVector3(0.5, 0.5, 0.5));

	// �� �ﰢ������ ���ǵǴ� ��ġ�� Ÿ���� �ʱ�ȭ �Ѵ�.
	PatchType.assign(NumFace, (PATCH_TYPE_MODIFIED | PATCH_TYPE_BZR_TRI));

	// �� �ﰢ���� ������ �ﰢ�� ������ �����Ѵ�.
	get_neighbor_faces_of_face(pCtrlMesh, AdjFaceListOfFace);
}

/*!
*	\brief ������ ����� ���� ������
*
*	\param cpy ����� ��ü
*/
GBlendMesh::GBlendMesh(const GBlendMesh &cpy)
: GMesh(cpy)
{
	pCtrlMesh = cpy.pCtrlMesh;
	PatchList = cpy.PatchList;
	BlendOffset = cpy.BlendOffset;
	PatchType = cpy.PatchType;
	AdjFaceListOfFace = cpy.AdjFaceListOfFace;
}

/*!
*	\brief ������ ����� �Ҹ���
*/
GBlendMesh::~GBlendMesh()
{
}

/*!
*	\brief ���� ������
*
*	\param rhs ���Ե� ��ü
*
*	\return ���Ե� �ڽ��� ��ȯ�Ѵ�.
*/
GBlendMesh &GBlendMesh::operator =(const GBlendMesh &rhs)
{
	GMesh::operator =(rhs);

	pCtrlMesh = rhs.pCtrlMesh;
	PatchList = rhs.PatchList;
	BlendOffset = rhs.BlendOffset;
	PatchType = rhs.PatchType;
	AdjFaceListOfFace = rhs.AdjFaceListOfFace;

	return *this;
}

/*!
*	\brief ������ �Ѵ�.
*
*	\param bSubMesh ���� �޽��� ������ ����(���� ��忡���� false�� ����)
*/
void GBlendMesh::Render(bool bSubMesh)
{
	GMesh::Render(bSubMesh);
}

/*!
*	\brief �޽��� �����Ѵ�.
*/
void GBlendMesh::CreateMesh()
{
	// ��� ������ ����Ʈ�� �ʱ�ȭ �ϰ�,
	InitBndryEdgeList();

	// �ﰢ������ ���ǵǴ� �ﰢ�� ������ ��ġ�� �����ϰ�,
	CreateLocalPatch();

	// ������ ��ǥ�� �����ϰ�,
	CreateMeshVert();

	// �ؽ�ó ��ǥ�� �����ϰ�,
	CreateMeshTex();

	// �ﰢ�� ������ �����ϰ�, 
	CreateMeshFace();

	// ������ ������ ����ϰ�,
	//UpdateNormal(false);

	// ����� �ٿ�� �ڽ��� ����Ѵ�.
	UpdateBndBox(false);

	// ���� ���ۿ� �ε��� ���۸� �����Ѵ�.
	UpdateBufVertex();
	UpdateBufNormal();
	UpdateBufFace();
}

/*!
*	\brief ��ġ Ÿ�Կ� ���� ������ �ﰢ������ ���ǵǴ� ������ ��ġ�� �����Ѵ�.
*/
void GBlendMesh::CreateLocalPatch()
{
	// �ﰢ�� ������ ���ͼ�, 
	int NumFace = pCtrlMesh->GetNumFace();

	// ������ �ﰢ������ �ﰢ�� ������ ����� �����Ѵ�.
	for (int i = 0; i < NumFace; ++i)
	{
		// �� �ﰢ�� ����,
		GFace *pFace = &(pCtrlMesh->Faces[i]);

		// ��ġ�� Ÿ���� ����Ǿ��ٸ�, (Ȥ�� ���� ������)
		if (PatchType[i] & PATCH_TYPE_MODIFIED)
		{
			PatchType[i] &= ~(PATCH_TYPE_MODIFIED);

			// ��ġ Ÿ���� �Ϲ� PN �ﰢ���̶��,
			if (PatchType[i] & PATCH_TYPE_BZR_TRI)
			{
				GTriBzrSrf *pPatch = ::get_tri_bzr_srf(pFace, false);
				PatchList[i] = *pPatch;
				delete pPatch;
			}
			// ��ġ Ÿ���� Ȯ�� PN �ﰢ���̶��,
			else if (PatchType[i] & PATCH_TYPE_BZR_TRI_EXT)
			{
				// ������ �ﰢ���� �ε����� ���ؼ�,
				int fidx0 = AdjFaceListOfFace[i * 3];		// e0�� �����ϴ� �ﰢ���� �ε���
				int fidx1 = AdjFaceListOfFace[i * 3 + 1];	// e1�� �����ϴ� �ﰢ���� �ε���
				int fidx2 = AdjFaceListOfFace[i * 3 + 2];	// e2�� �����ϴ� �ﰢ���� �ε���

				GVector4 q0, q1, q2;
				bool flag0 = false;
				bool flag1 = false;
				bool flag2 = false;

				// e0�� ������ �ﰢ���� �ִٸ�,
				if (fidx0 != -1)
				{
					GFace *pFace0 = &(pCtrlMesh->Faces[fidx0]);
					int vidx = pFace0->GetNextVertIdx(pFace->vIdx[1]);
					q0 = cast_vec4(pCtrlMesh->Verts[vidx].P);
					flag0 = true;
				}
				// e1�� ������ �ﰢ���� �ִٸ�,
				if (fidx1 != -1)
				{
					GFace *pFace1 = &(pCtrlMesh->Faces[fidx1]);
					int vidx = pFace1->GetNextVertIdx(pFace->vIdx[2]);
					q1 = cast_vec4(pCtrlMesh->Verts[vidx].P);
					flag1 = true;
				}

				// e2�� ������ �ﰢ���� �ִٸ�,
				if (fidx2 != -1)
				{
					GFace *pFace2 = &(pCtrlMesh->Faces[fidx2]);
					int vidx = pFace2->GetNextVertIdx(pFace->vIdx[0]);
					q2 = cast_vec4(pCtrlMesh->Verts[vidx].P);
					flag2 = true;
				}

				GTriBzrSrf *pPatch = ::get_tri_bzr_srf(pFace, q0, q1, q2, flag0, flag1, flag2);
				PatchList[i] = *pPatch;
				delete pPatch;
			}
			// ��� �ﰢ���̶��,
			else if (PatchType[i] & PATCH_TYPE_BZR_TRI_PLN)
			{
				GTriBzrSrf *pPatch = ::get_tri_bzr_srf(pFace, true);
				PatchList[i] = *pPatch;
				delete pPatch;
			}
		}		
	}
}

double GetBlendingBound(double h, double s)
{
	double r = 0.0;

	if (0.0 <= s && s < 1.5 * h)
	{
		r = s / SQRT(3.0);
	}
	if (1.5 * h <= s && s < 2 * h)
	{
		r = SQRT(SQR(h) - SQR(2 * h - s));
	}
	if (2 * h <= s && s < SQRT(2.0) - 2 * h)
	{
		r = h;
	}
	if (SQRT(2.0) - 2 * h <= s && s < SQRT(2.0) - 1.5 * h)
	{
		r = SQRT(SQR(h) - SQR(s - SQRT(2.0) + 2 * h));
	}
	if (SQRT(2.0) - 1.5 * h <= s && s <= SQRT(2.0))
	{
		r = (SQRT(2.0) - s) / SQRT(3.0);
	}

	return r;	
}

void GetLocalParam(GFace *pSrcFace, double u, double v, double w, GFace *pTarFace, double &uu, double &vv, double &ww)
{
	// u ��ǥ�� ���� ���� ���,
	if (u <= v && u <= w)
	{
		if (pSrcFace->vIdx[1] == pTarFace->vIdx[1])
		{
			uu = 1.0 - v;
			vv = 1.0 - w;
			ww = v + w - 1.0;
		}
		if (pSrcFace->vIdx[1] == pTarFace->vIdx[0])
		{
			uu = 1.0 - w;
			vv = v + w - 1.0;
			ww = 1.0 - v;
		}
		if (pSrcFace->vIdx[1] == pTarFace->vIdx[2])
		{
			uu = v + w - 1.0;
			vv = 1.0 - v;
			ww = 1.0 - w;
		}
	}
	// v ��ǥ�� ���� ���� ���,
	else if (v <= u && v <= w)
	{
		if (pSrcFace->vIdx[2] == pTarFace->vIdx[1])
		{
			uu = 1.0 - w;
			vv = 1.0 - u;
			ww = u + w - 1.0;
		}
		if (pSrcFace->vIdx[2] == pTarFace->vIdx[0])
		{
			uu = 1.0 - u;
			vv = u + w - 1.0;
			ww = 1.0 - w;
		}
		if (pSrcFace->vIdx[2] == pTarFace->vIdx[2])
		{
			uu = u + w - 1.0;
			vv = 1.0 - w;
			ww = 1.0 - u;							
		}
	}
	// w ��ǥ�� ���� ���� ���,
	else
	{
		if (pSrcFace->vIdx[0] == pTarFace->vIdx[1])
		{
			uu = 1.0 - u;
			vv = 1.0 - v;
			ww = u + v - 1.0;
		}
		if (pSrcFace->vIdx[0] == pTarFace->vIdx[0])
		{
			uu = 1.0 - v;
			vv = u + v - 1.0;
			ww = 1.0 - u;
		}
		if (pSrcFace->vIdx[0] == pTarFace->vIdx[2])
		{
			uu = u + v - 1.0;
			vv = 1.0 - u;
			ww = 1.0 - v;
		}
	}
}

/*!
*	\brief ������ ��ǥ�� �����Ѵ�.
*/
void GBlendMesh::CreateMeshVert()
{
	// ������ ������ �����, 
	Verts.clear();

	// �ﰢ���� ������ ���ϰ�,
	int NumFace = pCtrlMesh->GetNumFace();

	for (int i = 0; i < NumFace; ++i)
	{
		// �� �ﰢ�� ����,
		GFace *pFace = &(pCtrlMesh->Faces[i]);

		// ������ �ﰢ���� �ε����� ���ؼ�,
		int fidx0 = AdjFaceListOfFace[i * 3];		// e0�� �����ϴ� �ﰢ���� �ε���
		int fidx1 = AdjFaceListOfFace[i * 3 + 1];	// e1�� �����ϴ� �ﰢ���� �ε���
		int fidx2 = AdjFaceListOfFace[i * 3 + 2];	// e2�� �����ϴ� �ﰢ���� �ε���

		// ������ �ﰢ���� ���ϰ�,
		GFace *pFace0 = (fidx0 == -1) ? NULL : &(pCtrlMesh->Faces[fidx0]);
		GFace *pFace1 = (fidx1 == -1) ? NULL : &(pCtrlMesh->Faces[fidx1]);
		GFace *pFace2 = (fidx2 == -1) ? NULL : &(pCtrlMesh->Faces[fidx2]);

		// ���� �ﰢ�� �� ������ �ﰢ������ ���ǵǴ� �ﰢ�� ������ ����� ���ϰ�,
		GTriBzrSrf *pSrf = &PatchList[i];
		GTriBzrSrf *pSrf0 = (fidx0 == -1) ? NULL : &PatchList[fidx0];
		GTriBzrSrf *pSrf1 = (fidx1 == -1) ? NULL : &PatchList[fidx1];
		GTriBzrSrf *pSrf2 = (fidx2 == -1) ? NULL : &PatchList[fidx2];

		// ���ø� ������ ���ͼ�, 
		int snum = Renderer.NumSampU;

		// �ﰢ�� ������ ����� ������ ����Ͽ�,
		for (int j = 0; j < snum; ++j)
		{
			double u = (double)j / (double)(snum - 1);
			for (int k = 0; k < snum - j; ++k)
			{
				double v = (double)k / (double)(snum - 1);
				double w = 1.0 - u - v;
				double uu, vv, ww, h = 0.0, s = 0.0, d = 0.0;
				
				// ���� �ﰢ������ ���ǵǴ� ����� ���� ����ϰ�,
				GVector4 p = pSrf->Eval(u, v, w);
				GVector4 q = p;

				GVector3 pn = pSrf->EvalNormal(u, v, w);
				GVector3 qn = pn;
				
				// u ��ǥ�� ���� ���� ���, pSrf0(u0, v0, w0)�� ������ �Ѵ�.
				if (u <= v && u <= w)
				{
					// ������ �ﰢ���� �ִٸ�, ������ �����Ѵ�.
					if (pFace0 != NULL)
					{
						GetLocalParam(pFace, u, v, w, pFace0, uu, vv, ww);
						q = pSrf0->Eval(uu, vv, ww);
						qn = pSrf0->EvalNormal(uu, vv, ww);

						h = BlendOffset[i][0] * SQRT(2.0) / 4.0;
						s = (1.0 - v + w) / SQRT(2.0);
						d = SQRT(SQR(u) + SQR(v - 1.0) + SQR(w) - SQR(s));
					}
				}
				// v ��ǥ�� ���� ���� ���, pSrf1(u1, v1, w1)�� ������ �Ѵ�.
				else if (v <= u && v <= w)
				{
					if (pFace1 != NULL)
					{
						GetLocalParam(pFace, u, v, w, pFace1, uu, vv, ww);
						q = pSrf1->Eval(uu, vv, ww);
						qn = pSrf1->EvalNormal(uu, vv, ww);

						h = BlendOffset[i][1] * SQRT(2.0) / 4.0;
						s = (u - w + 1.0) / SQRT(2.0);
						d = SQRT(SQR(u) + SQR(v) + SQR(w - 1.0) - SQR(s));						
					}					
				}
				// w ��ǥ�� ���� ���� ���, pSrf2(u2, v2, w2)�� ������ �Ѵ�.
				else
				{
					if (pFace2 != NULL)
					{
						GetLocalParam(pFace, u, v, w, pFace2, uu, vv, ww);
						q = pSrf2->Eval(uu, vv, ww);
						qn = pSrf2->EvalNormal(uu, vv, ww);

						h = BlendOffset[i][2] * SQRT(2.0) / 4.0;
						s = (v - u + 1.0) / SQRT(2.0);
						d = SQRT(SQR(u - 1.0) + SQR(v) + SQR(w) - SQR(s));						
					}					
				}

				double r = GetBlendingBound(h, s);
				double t = (r - d) / (2 * r);
				double alpha = (d >= r) ? 1.0 : (2 * t * t * t - 3 * t * t + 1);	// -6 * t * t * t * t * t + 15 * t * t * t * t - 10 * t * t * t + 1;												

				if (EQ_ZERO(u, 0.00001) || EQ_ZERO(v, 0.00001) || EQ_ZERO(w, 0.00001))
					alpha = 0.5;

				if (EQ_ZERO(r, 0.00001))
				{
					q = p;
					qn = pn;
				}

				// ���� ���� ����Ѵ�.
				p = (1.0 - alpha) * q + alpha * p;
				pn = (1.0 - alpha) * qn + alpha * pn;

				// ���� ����Ʈ�� �߰�
				GVertex vtx(GPoint3(p[0], p[1], p[2]), pn, GVector3(alpha, alpha, alpha));
				AddVertex(vtx);
			}
		}
	}
}

/*!
*	\brief �ؽ�ó ��ǥ�� �����Ѵ�.
*/ 
void GBlendMesh::CreateMeshTex()
{
}

/*!
*	\brief �ﰢ�� ������ �����Ѵ�.
*/ 
void GBlendMesh::CreateMeshFace()
{
	// �ﰢ�� ������ �����,
	Faces.clear();

	// ���ø� ������ ���ͼ�, 
	int snum = Renderer.NumSampU;

	// �ﰢ�� ������ �����Ѵ�.
	int vidx0 = 0;

	int fnum = pCtrlMesh->GetNumFace();
	for (int f = 0; f < fnum; ++f)
	{
		for (int i = 0; i < snum - 1; ++i)
		{
			for(int j = 0; j < snum - 1 - i; ++j)
			{
				int vidx1 = vidx0 + 1;
				int vidx2 = vidx1 + snum - i;
				int vidx3 = vidx2 - 1;

				GFace f0(vidx0, vidx3, vidx1);
				AddFace(f0);

				if (j != snum - 2 - i)
				{
					GFace f1(vidx1, vidx3, vidx2);
					AddFace(f1);
				}
				vidx0++;
			}
			vidx0++;
		}
		vidx0++;
	}
}

/*!
*	\brief ��ü ���� ���� ����Ѵ�.
*
*	\param u �Ķ����
*
*	\return ������ ������� �� S(u, v, w)�� ��ȯ�Ѵ�.
*/
GVector4 GBlendMesh::Eval(double u, ...)
{
	return GVector4();
}

/*!
*	\brief ������ ����� �Ķ���� ���� [umin, umax] x [vmin, vmax]�� ���Ѵ�.
*
*	\param umin U-���� �Ķ������ ���� ���� �����
*/
void GBlendMesh::GetDomain(double *umin, ...)
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
void GBlendMesh::GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList)
{
}