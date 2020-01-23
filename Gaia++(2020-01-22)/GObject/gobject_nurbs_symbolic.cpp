#include "stdafx.h"

/*!
*	\brief �� ���� �ҽ� ����� ���� ���Ѵ�.
*	\note �� ����� ������ ������ �� ���ͷ� �����Ǿ�� ����� �����ϴ�.
*
*	\param pSrf0 ù ��° �ҽ� ���
*	\param pSrf1 �� ��° �ҽ� ���
*
*	\return �� ���� �ҽ� ����� ���� ��ȯ�Ѵ�.
*/
GNurbsSrf *add(GNurbsSrf *pSrf0, GNurbsSrf *pSrf1)
{
	// ��� ����� ������ �� ���͸� ���Ͽ�,
	int p = pSrf0->p;
	int q = pSrf0->q;
	int m = pSrf0->m;
	int n = pSrf0->n;
	double *U = pSrf0->U;
	double *V = pSrf0->V;

	GVector4 **P = new GVector4 *[m + 1];
	for (int i = 0; i < m + 1; ++i)
	{
		P[i] = new GVector4 [n + 1];
		for (int j = 0; j < n + 1; ++j)
		{
			GPoint3 p0 = pSrf0->MC * GPoint3(pSrf0->P[i][j][0], pSrf0->P[i][j][1], pSrf0->P[i][j][2]);
			GPoint3 p1 = pSrf1->MC * GPoint3(pSrf1->P[i][j][0], pSrf1->P[i][j][1], pSrf1->P[i][j][2]);
			P[i][j] = cast_vec4(cast_vec3(p0) + cast_vec3(p1), 1.0);
		}
	}

	// ��� ����� �����ϰ�,
	GNurbsSrf *pSrf = new GNurbsSrf("", GTransf(), p, m, U, q, n, V, P, false, "");

	// �Ҵ�� �޸𸮸� �����Ѵ�.
	for (int i = 0; i < m + 1; ++i)
		delete [] P[i];
	delete [] P;

	// ������ ����� ��ȯ�Ѵ�.
	return pSrf;
}

/*!
*	\brief �� ���� �ҽ� ����� ���� ���Ѵ�.
*	\note �� ����� ������ ������ �� ���ͷ� �����Ǿ�� ����� �����ϴ�.
*
*	\param pSrf0 ù ��° �ҽ� ���
*	\param pSrf1 �� ��° �ҽ� ���
*
*	\return �� ���� �ҽ� ����� ���� ��ȯ�Ѵ�.
*/
GNurbsSrf *sub(GNurbsSrf *pSrf0, GNurbsSrf *pSrf1)
{
	// ��� ����� ������ �� ���͸� ���Ͽ�,
	int p = pSrf0->p;
	int q = pSrf0->q;
	int m = pSrf0->m;
	int n = pSrf0->n;
	double *U = pSrf0->U;
	double *V = pSrf0->V;

	GVector4 **P = new GVector4 *[m + 1];
	for (int i = 0; i < m + 1; ++i)
	{
		P[i] = new GVector4 [n + 1];
		for (int j = 0; j < n + 1; ++j)
		{
			GPoint3 p0 = pSrf0->MC * GPoint3(pSrf0->P[i][j][0], pSrf0->P[i][j][1], pSrf0->P[i][j][2]);
			GPoint3 p1 = pSrf1->MC * GPoint3(pSrf1->P[i][j][0], pSrf1->P[i][j][1], pSrf1->P[i][j][2]);
			P[i][j] = cast_vec4(cast_vec3(p0) - cast_vec3(p1), 1.0);
		}
	}

	// ��� ����� �����ϰ�,
	GNurbsSrf *pSrf = new GNurbsSrf("", GTransf(), p, m, U, q, n, V, P, false, "");

	// �Ҵ�� �޸𸮸� �����Ѵ�.
	for (int i = 0; i < m + 1; ++i)
		delete [] P[i];
	delete [] P;

	// ������ ����� ��ȯ�Ѵ�.
	return pSrf;
}
