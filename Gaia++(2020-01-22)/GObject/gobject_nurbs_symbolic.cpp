#include "stdafx.h"

/*!
*	\brief 두 개의 넙스 곡면의 합을 구한다.
*	\note 두 곡면은 동일한 차수와 낫 벡터로 구성되어야 계산이 가능하다.
*
*	\param pSrf0 첫 번째 넙스 곡면
*	\param pSrf1 두 번째 넙스 곡면
*
*	\return 두 개의 넙스 곡면의 합을 반환한다.
*/
GNurbsSrf *add(GNurbsSrf *pSrf0, GNurbsSrf *pSrf1)
{
	// 결과 곡면의 차수와 낫 벡터를 구하여,
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

	// 결과 곡면을 생성하고,
	GNurbsSrf *pSrf = new GNurbsSrf("", GTransf(), p, m, U, q, n, V, P, false, "");

	// 할당된 메모리를 해제한다.
	for (int i = 0; i < m + 1; ++i)
		delete [] P[i];
	delete [] P;

	// 생성된 곡면을 반환한다.
	return pSrf;
}

/*!
*	\brief 두 개의 넙스 곡면의 차를 구한다.
*	\note 두 곡면은 동일한 차수와 낫 벡터로 구성되어야 계산이 가능하다.
*
*	\param pSrf0 첫 번째 넙스 곡면
*	\param pSrf1 두 번째 넙스 곡면
*
*	\return 두 개의 넙스 곡면의 차를 반환한다.
*/
GNurbsSrf *sub(GNurbsSrf *pSrf0, GNurbsSrf *pSrf1)
{
	// 결과 곡면의 차수와 낫 벡터를 구하여,
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

	// 결과 곡면을 생성하고,
	GNurbsSrf *pSrf = new GNurbsSrf("", GTransf(), p, m, U, q, n, V, P, false, "");

	// 할당된 메모리를 해제한다.
	for (int i = 0; i < m + 1; ++i)
		delete [] P[i];
	delete [] P;

	// 생성된 곡면을 반환한다.
	return pSrf;
}
