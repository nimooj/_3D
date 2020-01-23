#include "stdafx.h"

extern double CBS_BASIS[1001][4];
extern double CBS_BASIS_DERIV[1001][4];

/*!
*	\brief 주어진 제어점으로 생성된 폐곡선을 생성
*
*	\param _p 곡선의 차수
*	\param _n 제어점의 마지막 인덱스
*	\param _P 제어점 배열
*	\param _rational 유리곡선 여부
*
*	\return 제어점 \a _P에 의해서 생성된 \a _p차 폐곡선를 반환
*/
GNurbsCrv *get_gnurbs_crv_closed(int _p, int _n, GVector4 *_P, bool _rational)
{
	int p = _p;
	int n = _n + p;
	int m = n + p + 1;

	GVector4 *P = new GVector4 [n + 1];
	for (int i = 0; i < n + 1; ++i)
	{
		int idx = i % (_n + 1);
		P[i] = _P[idx];
	}

	double *U = new double [m + 1];
	for (int i = 0; i < m + 1; ++i)
		U[i] = (double)i;

	double a = U[p], b = U[n + 1];
	for (int i = 0; i < m + 1; ++i)
		U[i] = (U[i] - a) / (b - a);

	GNurbsCrv *pCrv = new GNurbsCrv("", GTransf(), p, n, U, P, _rational);
	pCrv->closed = true;

	delete [] P;
	delete [] U;

	return pCrv;
}

/*!
*	\brief 주어진 데이터 정점을 보간하는 다항식 곡선을 생성
*	\note 주어진 데이터가 가중치를 갖는 것은 무의미 하므로, 4차원 공간의 다항식 곡선을 생성
*
*	\param p 곡선의 차수
*	\param n 보간될 정점의 마지막 인덱스
*	\param Q 보간될 데이터 정점의 배열
*	\param u0 Q0의 보간파라미터 값
*	\param un Qn의 보간파라미터 값
*	\param pType 보간될 데이터 정점의 파라미터 결정형태
*	\prarm kType 생성될 곡선의 knot 벡터의 생성형태
*
*	\return Q0, Q1, ... , Qn을 보간하는 \a p차 곡선을 반환
*/
GNurbsCrv *get_gnurbs_crv_inter(int p, int n, GVector4 *Q, double u0, double un, TypeParam pType, TypeKnot kType, double *Params, double *Knots)
{
	double *param;
	switch (pType)
	{
	case PARAM_UNIFORM:
		param = get_param_uniform(n, u0, un);
		break;
	case PARAM_CHORD_LEN:
		param = get_param_chord_len(n, Q, u0, un);
		break;
	case PARAM_CENTRIPETAL:
		param = get_param_centripedal(n, Q, u0, un);
		break;
	case PARAM_USER:
		param = new double [n + 1];
		ARR_COPY(param, Params, n + 1);
		break;
	}

	double *knot;
	switch (kType)
	{
	case KNOT_UNIFORM:
		knot = get_knot_uniform_clamped(p, n, u0, un);
		break;
	case KNOT_AVERAGING:
		knot = get_knot_averaging_clamped(p, n, param);
		break;
	case KNOT_USER:
		knot = new double [p + n + 2];
		ARR_COPY(knot, Knots, p + n + 2);
		break;
	}

	// Setup linear system and solve it.
	GVector4 *P = new GVector4 [n + 1];
	GMatrix A(n + 1, n + 1);
	GVector x(n + 1), b(n + 1), Idx(n + 1);
	double d;

	for (int i = 0; i < n + 1; ++i)
	{
		int idx = ::get_knot_span_idx(p, n, knot, param[i]);
		::get_gnurbs_basis(knot, p, param[i], idx, &A[i][idx - p]);
	}

	::dcmp_lu(A, Idx, d);

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < n + 1; ++j)
			b[j] = Q[j][i];

		::lin_sys_lu(A, x, b, Idx);
		for (int j = 0; j < n + 1; ++j)
			P[j][i] = x[j];
	}

	GNurbsCrv *pCrv = new GNurbsCrv("", GTransf(), p, n, knot, P, false);

	delete [] param;
	delete [] knot;
	delete [] P;

	return pCrv;
}

/*!
*	\brief 양끝 점과 주어진 법선 벡터를 보간하는 3차 Bezier곡선을 구한다.
*
*	\param p1 시작점의 위치
*	\param n1 시작점의 법선 벡터
*	\param p2 끝점의 위치
*	\param n2 끝점의 법선 벡터
*
*	\return 양끝 점과 주어진 법선벡터를 보간하는 3차 Bezier곡선을 반환한다.
*/
GNurbsCrv *get_gnurbs_crv_inter(GVector3 p1, GVector3 n1, GVector3 p2, GVector3 n2)
{
	n1.Normalize();
	n2.Normalize();

	GVector4 P[4];
	P[0].Set(p1[0], p1[1], p1[2], 1.0);
	P[3].Set(p2[0], p2[1], p2[2], 1.0);

	double s1 = 1.0 / 3.0;
	double s2 = 1.0 / 3.0;

	double omega1 = s1 * (p2 - p1) * n1;
	double omega2 = s2 * (p1 - p2) * n2;

	GVector3 tmp;
	tmp = (1.0 - s1) * p1 + s1 * p2 - omega1 * n1;
	P[1].Set(tmp[0], tmp[1], tmp[2], 1.0);
	tmp = s2 * p1 + (1.0 - s2) * p2 - omega2 * n2;
	P[2].Set(tmp[0], tmp[1], tmp[2], 1.0);

	double U[8] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};

	GNurbsCrv *pCrv = new GNurbsCrv("", GTransf(), 3, 3, U, P, true);

	return pCrv;
}

/*!
*	\brief 주어진 데이터 정점을 보간하는 폐곡선을 생성한다.
*
*	\param p 곡선의 차수
*	\param s 보간될 정점의 마지막 인덱스 (Q[0], Q[1], ... , Q[s])
*	\param Q 보간될 데이터 정점의 배열
*	\param pType 보간될 데이터 정점의 파라미터 결정형태
*	\prarm kType 생성될 곡선의 knot 벡터의 생성형태
*
*	\return Q0, Q1, ... , Qn, Q0을 보간하는 \a p차 닫힌 곡선을 생성한다.
*/
GNurbsCrv *get_gnurbs_crv_inter_closed(int p, int s, GVector4 *Q, TypeParam pType, TypeKnot kType)
{
	GVector4 *R = new GVector4 [s + 2];
	for (int i = 0; i < s + 1; ++i)
		R[i] = Q[i];
	R[s + 1] = Q[0];

	double *param;
	switch (pType)
	{
	case PARAM_UNIFORM:
		param = get_param_uniform(s + 1, 0.0, 1.0);
		break;

	case PARAM_CENTRIPETAL:
		param = get_param_centripedal(s + 1, R, 0.0, 1.0);
		break;

	case PARAM_CHORD_LEN:
		param = get_param_chord_len(s + 1, R, 0.0, 1.0);
		break;
	}

	int n = s + p, m = n + p + 1;
	double *knot;
	switch (kType)
	{
	case KNOT_UNIFORM:
		knot = get_knot_uniform_unclamped(p, n, 0.0, 1.0);
		break;
	case KNOT_NATURAL:
		knot = get_knot_natural_unclamped(p, s + 1, param);
		break;
	}

	GVector4 *P = new GVector4 [n + 1];
	// Setup linear system and solve it.
	GMatrix A(n + 1, n + 1);
	GVector x(n + 1), b(n + 1), Idx(n + 1);
	double d;

	for (int i = 0; i < n + 1; ++i)
	{
		if (i <= s)
		{
			int idx = ::get_knot_span_idx(p, n, knot, param[i]);
			::get_gnurbs_basis(knot, p, param[i], idx, &A[i][idx - p]);
		}
		else
		{
			A[i][i - s - 1] = 1.0;
			A[i][i] = -1.0;
		}
	}

	::dcmp_lu(A, Idx, d);

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < n + 1; ++j)
		{
			if (j <= s)
				b[j] = Q[j][i];
			else
				b[j] = 0.0;
		}

		::lin_sys_lu(A, x, b, Idx);
		for (int j = 0; j < n + 1; ++j)
			P[j][i] = x[j];
	}

	GNurbsCrv *pCrv = new GNurbsCrv("", GTransf(), p, n, knot, P, false);
	pCrv->closed = true;

	delete [] param;
	delete [] knot;
	delete [] P;
	delete [] R;
	return pCrv;
}

/*!
*	\brief 주어진 데이터 정점을 근사하는 곡선을 생성한다.
*
*	\param p 곡선의 차수
*	\param n 제어점의 마지막 인덱스
*	\param Q  근사될 데이터 정점의 배열
*	\param Qm 근사될 데이터 정점의 마지막 인덱스 (Q[0], Q[1], ... , Q[m])
*	\param pType 생성될 근사 곡선의 파라미터 타입
*	\prarm kType 생성될 근사 곡선의 낫 벡터 타입
*
*	\return Q0, Q1, ... , Qm 을 근사하는 \a p차 곡선을 반환한다.
*/
GNurbsCrv *get_gnurbs_crv_approx(int p, int n, GVector4 *Q, int Qm, TypeParam pType, TypeKnot kType)
{
	// 파라미터 값을 계산하고,
	double *param;
	switch (pType)
	{
	case PARAM_UNIFORM:
		param = get_param_uniform(Qm, 0.0, 1.0);
		break;
	case PARAM_CHORD_LEN:
		param = get_param_chord_len(Qm, Q, 0.0, 1.0);
		break;
	case PARAM_CENTRIPETAL:
		param = get_param_centripedal(Qm, Q, 0.0, 1.0);
		break;
	}

	// 낫 벡터를 생성한 후,
	double *knot;
	switch (kType)
	{
	case KNOT_UNIFORM:
		knot = get_knot_uniform_clamped(p, n, 0.0, 1.0);
		break;
	case KNOT_APPROX:
		knot = get_knot_approx_clamped(p, n, param, Qm);
		break;
	}

	// 선형 시스템을 생성하고, 해를 구한다.
	GVector4 *P = new GVector4 [n + 1];
	GMatrix A(Qm + 1, n + 1), At(n + 1, Qm + 1), B(n + 1, n + 1);
	GVector x(n + 1), b(Qm + 1), c(n + 1), Idx(n + 1);
	double d;

	for (int i = 0; i < Qm + 1; ++i)
	{
		int idx = ::get_knot_span_idx(p, n, knot, param[i]);
		::get_gnurbs_basis(knot, p, param[i], idx, &A[i][idx - p]);
	}

	At = tr(A);
	B = At * A;

	::dcmp_lu(B, Idx, d);

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < Qm + 1; ++j)
			b[j] = Q[j][i];

		c = At * b;

		::lin_sys_lu(B, x, c, Idx);
		for (int j = 0; j < n + 1; ++j)
			P[j][i] = x[j];
	}

	GNurbsCrv *pCrv = new GNurbsCrv("", GTransf(), p, n, knot, P, false);

	delete [] param;
	delete [] knot;
	delete [] P;

	return pCrv;
}


/*!
*	\brief 주어진 데이터 정점을 근사하는 곡선을 생성한다.
*
*	\param p 곡선의 차수
*	\param n 제어점의 마지막 인덱스
*	\param Pts 근사될 데이터 정점의 배열
*	\param pType 생성될 근사 곡선의 파라미터 타입
*	\prarm kType 생성될 근사 곡선의 낫 벡터 타입
*
*	\return \a p차 곡선을 반환한다.
*/
GNurbsCrv *get_gnurbs_crv_approx(int p, int n, std::vector<GPoint3> Pts, TypeParam pType, TypeKnot kType)
{
	// 근사할 정점을 배열에 옮기고,
	int Qm = Pts.size() - 1;
	GVector4 *Q = new GVector4 [Qm + 1];
	for (int i = 0; i < Qm + 1; ++i)
		Q[i] = cast_vec4(Pts[i], 1.0);

	// 파라미터 값을 계산하고,
	double *param;
	switch (pType)
	{
	case PARAM_UNIFORM:
		param = get_param_uniform(Qm, 0.0, 1.0);
		break;
	case PARAM_CHORD_LEN:
		param = get_param_chord_len(Qm, Q, 0.0, 1.0);
		break;
	case PARAM_CENTRIPETAL:
		param = get_param_centripedal(Qm, Q, 0.0, 1.0);
		break;
	}

	// 낫 벡터를 생성한 후,
	double *knot;
	switch (kType)
	{
	case KNOT_UNIFORM:
		knot = get_knot_uniform_clamped(p, n, 0.0, 1.0);
		break;
	case KNOT_APPROX:
		knot = get_knot_approx_clamped(p, n, param, Qm);
		break;
	}

	// 선형 시스템을 생성하고, 해를 구한다.
	GVector4 *P = new GVector4 [n + 1];
	GMatrix A(Qm + 1, n + 1), At(n + 1, Qm + 1), B(n + 1, n + 1);
	GVector x(n + 1), b(Qm + 1), c(n + 1), Idx(n + 1);
	double d;

	for (int i = 0; i < Qm + 1; ++i)
	{
		int idx = ::get_knot_span_idx(p, n, knot, param[i]);
		::get_gnurbs_basis(knot, p, param[i], idx, &A[i][idx - p]);
	}

	At = tr(A);
	B = At * A;

	::dcmp_lu(B, Idx, d);

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < Qm + 1; ++j)
			b[j] = Q[j][i];

		c = At * b;

		::lin_sys_lu(B, x, c, Idx);
		for (int j = 0; j < n + 1; ++j)
			P[j][i] = x[j];
	}

	P[0] = Q[0];
	P[n] = Q[Qm];
	GNurbsCrv *pCrv = new GNurbsCrv("", GTransf(), p, n, knot, P, false);

	delete [] param;
	delete [] knot;
	delete [] P;

	return pCrv;
}

/*!
*	\brief 제어점이 모두 동일한 퇴화된(degenerated) 곡선을 생성한다.
*
*	\param p 곡선의 차수
*	\param n 제어점의 마지막 인덱스
*	\param U 낫 벡터
*	\param P 제어점의 동일한 좌표
*
*	\return 제어점이 모두 동일한 퇴화된(degenerated) 곡선을 반환한다.
*/
GNurbsCrv *get_gnurbs_crv_degen(int p, int n, double *U, GVector4 p0, bool rational)
{
	GVector4 *P = new GVector4 [n + 1];
	for (int i = 0; i < n + 1; ++i)
		P[i] = p0;

	GNurbsCrv *pCrv = new GNurbsCrv("", GTransf(), p, n, U, P, rational);

	delete [] P;

	return pCrv;
}

/*!
*	\brief 측지 경로를 근사하는 3차 B-spline 곡선을 생성한다.
*
*	\param pPath 측지 경로
*
*	\return 측지 경로를 근사하는 3차 B-spline 곡선을 반환한다.
*/
GNurbsCrv *get_gnurbs_crv_from_geodesic_path(GGeoPath *pPath)
{
	// 측지 경로의 정점 수가 너무 적다면,
	int NumPt = pPath->GetNumVert();
	if (NumPt < 4)
		return NULL;
	
	// 측지 경로를 구성하는 정점들의 리스트를 구성한다.
	std::vector<GPoint3> Pts;
	for (int i = 0; i < NumPt; ++i)
		Pts.push_back(pPath->MC * pPath->Verts[i].P);

	// 근사 곡선을 생성하여 반환한다.
	int NumPath = pPath->Paths.size();
	int n = (NumPath * 2 < 3) ? 3 : (NumPath * 2 - 1);
	GNurbsCrv *pCrv = get_gnurbs_crv_approx(3, n, Pts, PARAM_CHORD_LEN, KNOT_APPROX);
	return pCrv;
}

/*!
*	\brief 주어진 곡선의 반대방향 곡선을 반환한다.
*
*	\param pCrv 입력 곡선
*
*	\return 입력 곡선의 반대방향 곡선을 반환한다.
*/
GNurbsCrv *get_gnurbs_crv_reverse(GNurbsCrv *pCrv)
{
	if (pCrv == NULL)
		return NULL;

	// 곡선의 정보를 얻어와서,
	int p = pCrv->p;
	int n = pCrv->n;
	int m = n + p + 1;
	GCtrlPt *P = pCrv->P;
	double *U = pCrv->U;

	// 새로운 제어점과 낫 벡터 배열를 생성하고,
	GCtrlPt *Q = new GCtrlPt [n + 1];
	double *knot = new double [m + 1];

	// 제어점을 거꾸로 복사하고,
	for (int i = 0; i < n + 1; ++i)
		Q[i] = P[n - i];

	// 새로운 낫 벡터를 생성한 후,
	for (int i = 0; i < m + 1; ++i)
		knot[m - i] = U[0] + U[m] - U[i];

	// 역방향 곡선을 생성하고,
	GNurbsCrv *rCrv = new GNurbsCrv("", pCrv->MC, p, n, knot, Q, pCrv->rational);
	rCrv->closed = pCrv->closed;

	// 동적할당된 메모리도 해제한 후,
	delete [] Q;
	delete [] knot;

	// 생성된 역방향 곡선을 반환한다.
	return rCrv;
}

/*!
*	\brief 평면에 포함된 2차 곡선(원)을 생성한다.
*
*	\param r 반지름
*	\param type 곡선이 포함된 평면 (0: xy 평면, 1: yz 평면, 2: zx 평면)
*
*	\return 생성된 평면 곡선을 반환
*/
GNurbsCrv *get_gnurbs_circle(double r, int type)
{
	double w0 = 1.0f;
	double w1 = SQRT(2.0) / 2.0;
	double U[12] = {0.0, 0.0, 0.0, 0.25, 0.25, 0.5, 0.5, 0.75, 0.75, 1.0, 1.0, 1.0};
	GVector4 P[9];

	switch (type)
	{
	case 0:
		P[0][0] = r;	P[0][1] = 0.0;	P[0][2] = 0.0;	P[0][3] = w0;
		P[1][0] = r;	P[1][1] = r;	P[1][2] = 0.0;	P[1][3] = w1;
		P[2][0] = 0.0;	P[2][1] = r;	P[2][2] = 0.0;	P[2][3] = w0;
		P[3][0] = -r;	P[3][1] = r;	P[3][2] = 0.0;	P[3][3] = w1;
		P[4][0] = -r;	P[4][1] = 0.0;	P[4][2] = 0.0;	P[4][3] = w0;
		P[5][0] = -r;	P[5][1] = -r;	P[5][2] = 0.0;	P[5][3] = w1;
		P[6][0] = 0.0; P[6][1] = -r;	P[6][2] = 0.0;	P[6][3] = w0;
		P[7][0] = r;	P[7][1] = -r;	P[7][2] = 0.0;	P[7][3] = w1;
		P[8][0] = r;	P[8][1] = 0.0;	P[8][2] = 0.0;	P[8][3] = w0;
		break;

	case 1:
		P[0][0] = 0.0; P[0][1] = r;		P[0][2] = 0;	P[0][3] = w0;
		P[1][0] = 0.0; P[1][1] = r;		P[1][2] = r;	P[1][3] = w1;
		P[2][0] = 0.0; P[2][1] = 0;		P[2][2] = r;    P[2][3] = w0;
		P[3][0] = 0.0; P[3][1] = -r;	P[3][2] = r;	P[3][3] = w1;
		P[4][0] = 0.0; P[4][1] = -r;	P[4][2] = 0;	P[4][3] = w0;
		P[5][0] = 0.0; P[5][1] = -r;	P[5][2] = -r;	P[5][3] = w1;
		P[6][0] = 0.0; P[6][1] = 0;		P[6][2] = -r;	P[6][3] = w0;
		P[7][0] = 0.0; P[7][1] = r;		P[7][2] = -r;	P[7][3] = w1;
		P[8][0] = 0.0; P[8][1] = r;		P[8][2] = 0;	P[8][3] = w0;
		break;

	case 2:
		P[0][0] = 0.0;	P[0][1] = 0.0;	P[0][2] = r;	P[0][3] = w0;
		P[1][0] = r;	P[1][1] = 0.0;	P[1][2] = r;	P[1][3] = w1;
		P[2][0] = r;	P[2][1] = 0.0;	P[2][2] = 0.0;	P[2][3] = w0;
		P[3][0] = r;	P[3][1] = 0.0;	P[3][2] = -r;	P[3][3] = w1;
		P[4][0] = 0.0;	P[4][1] = 0.0;	P[4][2] = -r;	P[4][3] = w0;
		P[5][0] = -r;	P[5][1] = 0.0;	P[5][2] = -r;	P[5][3] = w1;
		P[6][0] = -r;	P[6][1] = 0.0;	P[6][2] = 0.0;	P[6][3] = w0;
		P[7][0] = -r;	P[7][1] = 0.0;	P[7][2] = r;	P[7][3] = w1;
		P[8][0] = 0.0;	P[8][1] = 0.0;	P[8][2] = r;	P[8][3] = w0;
		break;
	}

	GNurbsCrv *pCrv = new GNurbsCrv("", GTransf(), 2, 8, U, P, true);
	return pCrv;
}

/*!
*	\brief 평면에서 원을 근사하는 3차 폐곡선을 생성한다.
*
*	\param r 반지름
*	\param type 곡선이 포함된 평면 (0: xy 평면, 1: yz 평면, 2: zx 평면)
*	\param ccw 반시계 방향 여부
*
*	\return 생성된 평면 곡선을 반환
*/
GNurbsCrv *get_gnurbs_approx_circle(double r, int type, bool ccw)
{
	GVector4 P[4];

	switch(type)
	{
	case 0:
		P[0][0] = r;	P[0][1] = r;	P[0][2] = 0.0;	P[0][3] = 1.0;
		P[1][0] = -r;	P[1][1] = r;	P[1][2] = 0.0;	P[1][3] = 1.0;
		P[2][0] = -r;	P[2][1] = -r;	P[2][2] = 0.0;	P[2][3] = 1.0;
		P[3][0] = r;	P[3][1] = -r;	P[3][2] = 0.0;	P[3][3] = 1.0;
		break;

	case 1:
		P[0][0] = 0.0;	P[0][1] = r;	P[0][2] = r;	P[0][3] = 1.0;
		P[1][0] = 0.0;	P[1][1] = -r;	P[1][2] = r;	P[1][3] = 1.0;
		P[2][0] = 0.0;	P[2][1] = -r;	P[2][2] = -r;	P[2][3] = 1.0;
		P[3][0] = 0.0;	P[3][1] = r;	P[3][2] = -r;	P[3][3] = 1.0;
		break;

	case 2:
		P[0][0] = r;	P[0][1] = 0.0;	P[0][2] = r;	P[0][3] = 1.0;
		P[1][0] = r;	P[1][1] = 0.0;	P[1][2] = -r;	P[1][3] = 1.0;
		P[2][0] = -r;	P[2][1] = 0.0;	P[2][2] = -r;	P[2][3] = 1.0;
		P[3][0] = -r;	P[3][1] = 0.0;	P[3][2] = r;	P[3][3] = 1.0;
		break;
	}

	// 시계 방향이라면,
	if (ccw == false)
		SWAP(GVector4, P[1], P[3]);
	
	GNurbsCrv *pCrv = ::get_gnurbs_crv_closed(3, 3, P, false);
	return pCrv;
}

/*!
*	\brief 평면에서 정의된 사각형을 생성한다.
*
*	\param w 사각형의 너비
*	\param h 사각형의 높이
*	\param type 곡선이 포함된 평면 (0: xy 평면, 1: yz 평면, 2: zx 평면)
*
*	\return 생성된 평면 사각형을 반환한다.
*/
GNurbsCrv *get_gnurbs_rect(double w, double h, int type)
{
	GVector4 P[4];

	switch(type)
	{
	case 0:
		P[0][0] = w / 2;	P[0][1] = h / 2;	P[0][2] = 0.0;	P[0][3] = 1.0;
		P[1][0] = -w / 2;	P[1][1] = h / 2;	P[1][2] = 0.0;	P[1][3] = 1.0;
		P[2][0] = -w / 2;	P[2][1] = -h / 2;	P[2][2] = 0.0;	P[2][3] = 1.0;
		P[3][0] = w / 2;	P[3][1] = -h / 2;	P[3][2] = 0.0;	P[3][3] = 1.0;
		break;

	case 1:
		P[0][0] = 0.0;	P[0][1] = w / 2;	P[0][2] = h / 2;	P[0][3] = 1.0;
		P[1][0] = 0.0;	P[1][1] = -w / 2;	P[1][2] = h / 2;	P[1][3] = 1.0;
		P[2][0] = 0.0;	P[2][1] = -w / 2;	P[2][2] = -h / 2;	P[2][3] = 1.0;
		P[3][0] = 0.0;	P[3][1] = w / 2;	P[3][2] = -h / 2;	P[3][3] = 1.0;
		break;

	case 2:
		P[0][0] = w / 2;	P[0][1] = 0.0;	P[0][2] = h / 2;	P[0][3] = 1.0;
		P[1][0] = w / 2;	P[1][1] = 0.0;	P[1][2] = -h / 2;	P[1][3] = 1.0;
		P[2][0] = -w / 2;	P[2][1] = 0.0;	P[2][2] = -h / 2;	P[2][3] = 1.0;
		P[3][0] = -w / 2;	P[3][1] = 0.0;	P[3][2] = h / 2;	P[3][3] = 1.0;
		break;
	}

	GNurbsCrv *pCrv = ::get_gnurbs_crv_closed(1, 3, P, false);
	return pCrv;
}

/*!
*	\brief 평면에서 2차 곡선(타원)을 생성한다.
*
*	\parmm a X-축 길이
*	\param b Y-축 길이
*
*	\return 생성된 평면 곡선을 반환
*/
GNurbsCrv *get_gnurbs_ellipse(double a, double b, int type)
{
	double w0 = 1.0f;
	double w1 = SQRT(2.0) / 2.0;
	double U[12] = {0.0, 0.0, 0.0, 0.25, 0.25, 0.5, 0.5, 0.75, 0.75, 1.0, 1.0, 1.0};
	GVector4 P[9];

	switch (type)
	{
	case 0:
		P[0][0] = a;	P[0][1] = 0.0;	P[0][2] = 0.0;	P[0][3] = w0;
		P[1][0] = a;	P[1][1] = b;	P[1][2] = 0.0;	P[1][3] = w1;
		P[2][0] = 0;	P[2][1] = b;	P[2][2] = 0.0;	P[2][3] = w0;
		P[3][0] = -a;	P[3][1] = b;	P[3][2] = 0.0;	P[3][3] = w1;
		P[4][0] = -a;	P[4][1] = 0.0;	P[4][2] = 0.0;	P[4][3] = w0;
		P[5][0] = -a;	P[5][1] = -b;	P[5][2] = 0.0;	P[5][3] = w1;
		P[6][0] = 0;	P[6][1] = -b;	P[6][2] = 0.0;	P[6][3] = w0;
		P[7][0] = a;	P[7][1] = -b;	P[7][2] = 0.0;	P[7][3] = w1;
		P[8][0] = a;	P[8][1] = 0.0;	P[8][2] = 0.0;	P[8][3] = w0;
		break;

	case 1:
		P[0][0] = 0.0;	P[0][1] = a;	P[0][2] = 0.0;		P[0][3] = w0;
		P[1][0] = 0.0;	P[1][1] = a;	P[1][2] = b;		P[1][3] = w1;
		P[2][0] = 0.0;	P[2][1] = 0.0;	P[2][2] = b;		P[2][3] = w0;
		P[3][0] = 0.0;	P[3][1] = -a;	P[3][2] = b;		P[3][3] = w1;
		P[4][0] = 0.0;	P[4][1] = -a;	P[4][2] = 0.0;		P[4][3] = w0;
		P[5][0] = 0.0;	P[5][1] = -a;	P[5][2] = -b;		P[5][3] = w1;
		P[6][0] = 0.0;	P[6][1] = 0.0;	P[6][2] = -b;		P[6][3] = w0;
		P[7][0] = 0.0;	P[7][1] = a;	P[7][2] = -b;		P[7][3] = w1;
		P[8][0] = 0.0;	P[8][1] = a;	P[8][2] = 0.0;		P[8][3] = w0;
		break;

	case 2:
		P[0][0] = 0.0;	P[0][1] = 0.0;	P[0][2] = a;	P[0][3] = w0;
		P[1][0] = b;	P[1][1] = 0.0;	P[1][2] = a;	P[1][3] = w1;
		P[2][0] = b;	P[2][1] = 0.0;	P[2][2] = 0.0;	P[2][3] = w0;
		P[3][0] = b;	P[3][1] = 0.0;	P[3][2] = -a;	P[3][3] = w1;
		P[4][0] = 0.0;	P[4][1] = 0.0;	P[4][2] = -a;	P[4][3] = w0;
		P[5][0] = -b;	P[5][1] = 0.0;	P[5][2] = -a;	P[5][3] = w1;
		P[6][0] = -b;	P[6][1] = 0.0;	P[6][2] = 0.0;	P[6][3] = w0;
		P[7][0] = -b;	P[7][1] = 0.0;	P[7][2] = a;	P[7][3] = w1;
		P[8][0] = 0.0;	P[8][1] = 0.0;	P[8][2] = a;	P[8][3] = w0;
		break;
	}
	
	GNurbsCrv *pCrv = new GNurbsCrv("", GTransf(), 2, 8, U, P, true);
	return pCrv;
}

/*!
*	\brief 주어진 제어점으로 생성된 폐곡면을 생성
*
*	\param p 곡면의 U-방향 차수
*	\param m 곡면의 U-방향 제어점의 마지막 인덱스
*	\param q 곡면의 V-방향 차수
*	\param n 곡면의 V-방향 제어점의 마지막 인덱스
*	\param P 제어점 배열
*	\param closed_u U-방향 폐곡선 여부
*	\param closed_v V-방향 폐곡선 여부
*
*	\return 제어점 \a P에 의해서 생성된 폐곡면를 반환
*/
GNurbsSrf *get_gnurbs_srf_closed(int _p, int _m, int _q, int _n, GVector4 *_P, bool _closed_u, bool _closed_v)
{
	int p = _p;
	int m = (_closed_u) ? _m + p : _m;
	int r = p + m + 1;
	int q = _q;
	int n = (_closed_v) ? _n + q : _n;
	int s = q + n + 1;

	GVector4 *P = new GVector4 [(m + 1) * (n + 1)];

	for (int i = 0; i < m + 1; ++i)
	{
		int idx0, idx1;
		idx0 = (_closed_u) ? i % (_m + 1) : i;

		for (int j = 0; j < n + 1; ++j)
		{
			idx1 = (_closed_v) ? j % (_n + 1) : j;
			P[i * (n + 1) + j] = _P[idx0 * (_n + 1) + idx1];
		}
	}

	double a, b;
	double *U = new double [r + 1];
	double *V = new double [s + 1];

	for (int i = 0; i < r + 1; ++i)
		U[i] = (double)i;

	for (int i = 0; i < s + 1; ++i)
		V[i] = (double)i;

	a = U[p];
	b = U[m + 1];
	for (int i = 0; i < r + 1; ++i)
		U[i] = (U[i] - a) / (b - a);

	a = V[q];
	b = V[n + 1];
	for (int i = 0; i < s + 1; ++i)
		V[i] = (V[i] - a) / (b - a);

	GNurbsSrf *pSrf = new GNurbsSrf("", GTransf(), p, m, U, q, n, V, P);
	pSrf->closed_u = _closed_u;
	pSrf->closed_v = _closed_v;

	delete [] P;
	delete [] U;
	delete [] V;

	return pSrf;
}

/*!
*	\brief 주어진 곡선들을 보간(interpolation)하는 곡면을 생성한다.
*	\waring 입력 곡선들은 동일한 차수와 낫벡터로 표현된다고 가정한다.
*	\waring 입력 곡선들은 U-방향 곡선들임을 가정한다.
*
*	\param q 곡면의 V-방향 차수
*	\param CrvList U-방향 곡선의 리스트
*	\param params 보간 파라미터
*	\param knot 낫 벡터 
*
*	\return 주어진 곡선들을 보간하는 곡면을 반환한다.
*/
GNurbsSrf *get_gnurbs_srf_inter(int q, std::vector<GNurbsCrv *> CrvList, double *Params, double *KnotV)
{
	// 곡면의 U-방향 차수(p)와 제어점 인덱스(m), 그리고 낫 벡터를 구하고,
	int p = CrvList[0]->p;
	int m = CrvList[0]->n;
	double *KnotU = CrvList[0]->U;

	// 곡면의 V-방향 제어점 인덱스(n)을 구한다.
	int n = (int)CrvList.size() - 1;
	
	// 보간 데이터를 저장할 변수를 할당하고,
	GVector4 *Q = new GVector4 [n + 1];

	// 결과 곡면의 제어점 배열을 생성하고,
	GVector4 **P = new GVector4 *[m + 1];
	for (int i = 0; i < m + 1; ++i)
		P[i] = new GVector4 [n + 1];
	
	// 각각의 곡선의 제어점에 대하여
	for (int i = 0; i < m + 1; ++i)
	{
		// 각각의 곡선에 대하여
		for (int j = 0; j < n + 1; ++j)
			Q[j] = CrvList[j]->P[i];
		
		// 구해진 제어점을 보간하는 임시 곡선을 생성하여, 
		GNurbsCrv *tmp = ::get_gnurbs_crv_inter(q, n, Q, 0.0, 1.0, PARAM_USER, KNOT_USER, Params, KnotV);

		// 결과 곡면의 제어점을 구하고,
		for (int j = 0; j < n + 1; ++j)
			P[i][j] = tmp->P[j];

		// 임시 곡선을 제거한다.
		delete tmp;	
	}

	// 계산된 제어점 배열을 이용하여 결과 곡면을 생성하고,
	GNurbsSrf *pRetSrf = new GNurbsSrf("srf", GTransf(), p, m, KnotU, q, n, KnotV, P, false, "");
	pRetSrf->closed_u = CrvList[0]->closed;

	// 동적으로 할당된 메모리를 해제하고,
	for (int i = 0; i < m + 1; ++i)
		delete [] P[i];
	delete [] P;
	delete [] Q;

	// 결과 곡면을 반환한다.
	return pRetSrf;
}

/*!
*	\brief 주어진 흩어진 점군(scattered data point)을 근사하는 넙스곡면을 생성한다.
*	\note  선형시스템을 이용하지 않고 빠르게 근사하지만 정확도는 다소 떨어짐
*
*	\param p 곡면의 U-방향 차수
*	\param m 곡면의 U-방향 제어점의 인덱스
*	\param U 곡면의 U-방향 낫 벡터
*	\param q 곡면의 V-방향 차수
*	\param n 곡면의 V-방향 제어점의 인덱스
*	\param V 곡면의 V-방향 낫 벡터
*	\param num_data 데이터의 개수
*	\param params 정점의 파라미터 배열, (u_i, v_i) = (params[i * 2], params[i * 2 + 1])
*	\param pts 근사할 정점의 배열
*	\param num_level 다단계 근사를 위한 단계수
*
*	\return 주어진 흩어진 점군(scattered data point) 근사하는 넙스곡면을 반환
*/
GNurbsSrf *get_gnurbs_srf_approx_ver1(int p, int m, double *U, int q, int n, double *V, int num_data, double *params, GVector4 *pts, int num_level)
{
	// static variable
	static double basis_u[20];
	static double basis_v[20];

	// Create surface to be returned
	GNurbsSrf *pSrf = ::get_gnurbs_srf_degen(p, m, U, q, n, V, GVector4(0.0, 0.0, 0.0, 0.0), false);

	// Set residual as sample values
	GVector4 *residual = new GVector4 [num_data];
	ARR_COPY(residual, pts, num_data);

	for (int i = 0; i < num_level; ++i)
	{
		m = pSrf->m;
		n = pSrf->n;
		U = pSrf->U;
		V = pSrf->V;

		// Initialize ctrlpt, omega, delta for computation
		int num_ctlpt = (m + 1) * (n + 1);

		GVector4 *ctlpt = new GVector4 [num_ctlpt];
		double *omega = new double [num_ctlpt];
		GVector4 *delta = new GVector4 [num_ctlpt];

		ARR_ZERO(ctlpt, num_ctlpt);
		ARR_ZERO(omega, num_ctlpt);
		ARR_ZERO(delta, num_ctlpt);

		// For each data points, compute omega and delta values.
		for (int j = 0; j < num_data; ++j)
		{
			double u = params[j * 2];
			double v = params[j * 2 + 1];
			GVector4 pt = residual[j];

			int idx_u = ::get_knot_span_idx(p, m, U, u);
			int idx_v = ::get_knot_span_idx(q, n, V, v);

			::get_gnurbs_basis(U, p, u, idx_u, basis_u);
			::get_gnurbs_basis(V, q, v, idx_v, basis_v);

			idx_u -= p;
			idx_v -= q;

			// Compute the sum of squared weight.
			double sum_sqr_wgt = 0.0;
			for (int k = 0; k < p + 1; ++k)
				for (int l = 0; l < q + 1; ++l)
					sum_sqr_wgt += SQR(basis_u[k] * basis_v[l]);

			// Compute delta and omega for each control points.
			for (int k = 0; k < p + 1; ++k)
			{
				for (int l = 0; l < q + 1; ++l)
				{
					double weight = basis_u[k] * basis_v[l];
					int idx = (n + 1) * (idx_u + k) + (idx_v + l);
					omega[idx] += SQR(weight);
					delta[idx] += SQR(weight) * weight * pt / sum_sqr_wgt;									
				}
			}
		}

		// Compute control points in object space.
		for (int j = 0; j < num_ctlpt; ++j)
		{
			if (omega[j] == 0.0)
				ctlpt[j].Set(0.0, 0.0, 0.0, 0.0);
			else
				ctlpt[j] = delta[j] / omega[j];

			pSrf->P[j / (n + 1)][j % (n + 1)] += ctlpt[j];
		}

		// Compute differnece between given data points and approximated data points.
		for (int j = 0; j < num_data; j++)
		{
			GVector4 pt;
			pt = pSrf->Eval(params[2 * j], params[2 * j + 1]);
			residual[j] = pts[j] - pt;
		}

		// Memory free
		delete [] ctlpt;
		delete [] omega;
		delete [] delta;

		// Refine surface's control point
		if (i != num_level - 1)
			pSrf->KnotRefine(true, true);
	}

	ARR_COPY(pts, residual, num_data);

	// Memory free
	delete [] residual;

	// Return approximated surface
	return pSrf;
}

/*!
*	\brief 주어진 흩어진 점군(scattered data point)을 근사하는 넙스곡면을 생성한다.
*	\note 선형시스템을 이용하여 근사곡면을 생성함
*
*	\param p 곡면의 U-방향 차수
*	\param m 곡면의 U-방향 제어점의 인덱스
*	\param U 곡면의 U-방향 낫 벡터
*	\param q 곡면의 V-방향 차수
*	\param n 곡면의 V-방향 제어점의 인덱스
*	\param V 곡면의 V-방향 낫 벡터
*	\param num_data 데이터의 개수
*	\param params 정점의 파라미터 배열, (u_i, v_i) = (params[i * 2], params[i * 2 + 1])
*	\param pts 근사할 정점의 배열
*	\param num_level 다단계 근사를 위한 단계수
*
*	\return 주어진 흩어진 점군(scattered data point) 근사하는 넙스곡면을 반환
*/
GNurbsSrf *get_gnurbs_srf_approx_ver2(int p, int m, double *U, int q, int n, double *V, int num_data, double *params, GVector4 *pts, int num_level)
{
	// static variable
	static double basis_u[20];
	static double basis_v[20];

	// Create surface to be returned
	GNurbsSrf *pSrf = ::get_gnurbs_srf_degen(p, m, U, q, n, V, GVector4(0.0, 0.0, 0.0, 0.0), false);

	// Set residual as sample values
	GVector4 *residual = new GVector4 [num_data];
	ARR_COPY(residual, pts, num_data);

	for (int i = 0; i < num_level; ++i)
	{
		m = pSrf->m;
		n = pSrf->n;
		U = pSrf->U;
		V = pSrf->V;

		int r = num_data;
		int c = (m + 1) * (n + 1);

		if (r < c)
			break;

		GMatrix A(r, c);
		GVector x(c), b(c);

		for (int j = 0; j < r; ++j)
		{
			double u = params[j * 2];
			double v = params[j * 2 + 1];
			GVector4 pt = residual[j];

			int idx_u = ::get_knot_span_idx(p, m, U, u);
			int idx_v = ::get_knot_span_idx(q, n, V, v);

			::get_gnurbs_basis(U, p, u, idx_u, basis_u);
			::get_gnurbs_basis(V, q, v, idx_v, basis_v);

			for (int k = 0; k < p + 1; ++k)
			{
				for (int l = 0; l < q + 1; ++l)
				{
					int idx = (n + 1) * (idx_u - p + k) + (idx_v - q + l);
					A[j][idx] = basis_u[k] * basis_v[l];
				}
			}
		}

		GMatrix At = tr(A);
		GMatrix B = At * A;

		GVector Idx(c);
		double d;

		::dcmp_lu(B, Idx, d);

		GVector4 *ctlpt = new GVector4 [c];
		for (int j = 0; j < 3; ++j)
		{
			GVector tmp(r);
			for (int k = 0; k < r; ++k)
			{
				tmp[k] = residual[k][j];
			}

			x.SetZeros();
			b = At * tmp;
			::lin_sys_lu(B, x, b, Idx);

			for (int k = 0; k < c; ++k)
			{
				ctlpt[k][j] = x[k];
			}
		}

		for (int j = 0; j < c; ++j)
		{
			pSrf->P[j / (n + 1)][j % (n + 1)] += ctlpt[j];
		}

		// Compute differnece between given data points and approximated data points.
		for (int j = 0; j < r; j++)
		{
			GVector4 pt;
			pt = pSrf->Eval(params[2 * j], params[2 * j + 1]);
			residual[j] = pts[j] - pt;
		}

		// Memory free
		delete [] ctlpt;

		// Refine surface's control point
		if (i != num_level - 1)
			pSrf->KnotRefine(true, true);
	}

	ARR_COPY(pts, residual, num_data);

	// Memory free
	delete [] residual;

	return pSrf;
}

/*!
*	\brief 제어점이 모두 동일한 퇴화된(degenerated) 곡면을 생성한다.
*
*	\param p 곡면의 U-방향 차수
*	\param m 곡면의 U-방향 제어점의 인덱스
*	\param U 곡면의 U-방향 낫 벡터
*	\param q 곡면의 V-방향 차수
*	\param n 곡면의 V-방향 제어점의 인덱스
*	\param V 곡면의 V-방향 낫 벡터
*	\param p0 곡면의 동일한 제어점
*	\param rational 유리곡면 여부
*
*	\return 제어점이 모두 동일한 퇴화된(degenerated) 곡면을 반환
*/
GNurbsSrf *get_gnurbs_srf_degen(int p, int m, double *U, int q, int n, double *V, GVector4 p0, bool rational)
{
	GVector4 **P;
	ARR_ALLOC_2D(P, GVector4, m + 1, n + 1);
	for (int i = 0; i < m + 1; ++i)
		for (int j = 0; j < n + 1; ++j)
			P[i][j] = p0;

	GNurbsSrf *pSrf = new GNurbsSrf("", GTransf(), p, m, U, q, n, V, P, rational);

	ARR_FREE_2D(P, m + 1);

	return pSrf;
}

/*!
*	\brief 넙스로 표현된 평면을 생성한다.
*
*	\param p U 방향 차수
*	\param q V 방향 차수
*	\param p0 평면위의 점 = S(0, 0)
*	\param p1 평면위의 점 = S(1, 0)
*	\param p2 평면위의 점 = S(0, 1)
*
*	\return p0, p1, p2를 지나는 넙스 평면을 생성하여 반환
*/
GNurbsSrf *get_gnurbs_plane(int p, int q, GPoint3 p0, GPoint3 p1, GPoint3 p2)
{
	double *U = ::get_knot_uniform_clamped(p, p, 0.0, 1.0);
	double *V = ::get_knot_uniform_clamped(q, q, 0.0, 1.0);

	GVector4 du = cast_vec4(p1) - cast_vec4(p0);
	GVector4 dv = cast_vec4(p2) - cast_vec4(p0);

	GVector4 **P;
	ARR_ALLOC_2D(P, GVector4, (p + 1), (q + 1));
	P[0][0].Set(p0[0], p0[1], p0[2], 1.0);

	for (int i = 0; i < p + 1; ++i)
	{
		double u = (double)i / (double)p;
		for (int j = 0; j < q + 1; ++j)
		{
			double v = (double)j / (double)q;
			P[i][j] = P[0][0] + u * du + v * dv;
		}
	}

	GNurbsSrf *pSrf = new GNurbsSrf("", GTransf(), p, p, U, q, q, V, P);

	delete [] U;
	delete [] V;
	ARR_FREE_2D(P, (p + 1));
	return pSrf;
}

/*!
*	\brief 넙스 곡선의 길이를 계산한다.
*	\warning 
*
*	\param pCrv 계산할 곡선의 포인터
*	\param u0 길이를 계산할 곡선의 시작 파라미터
*	\param u1 길이를 계산할 곡선의 끝 파라미터
*	\param snum 길이 계산을 위한 샘플링 정점의 갯수
*
*	\return C(u0) ~ C(u1)까지의 곡선의 길이를 계산하여 반환한다.
*/
double get_crv_len(GNurbsCrv *pCrv, double u0, double u1, int snum)
{
	double len = 0.0;
	GVector4 p0, p1;

	p0 = pCrv->Eval(u0);
	for (int i = 1; i < snum; ++i)
	{
		double u = u0 + (u1 - u0) * (double)i / (double)(snum - 1);
		p1 = pCrv->Eval(u);

		len += dist(p0, p1);
		p0 = p1;
	}

	return len;
}

/*!
*	\brief 주어진 곡선의 프레네 프레임을 계산한다.
*
*	\param pCrv 계산할 곡선의 포인터
*	\param T 계산된 단위 접벡터가 저장될 변수
*	\param N 계산된 단위 법벡터가 저장될 변수
*	\param B 계산된 단위 양법선 벡터가 저장될 변수
*/
void get_frenet_frame(GNurbsCrv *pCrv, double u, GVector3 &T, GVector3 &N, GVector3 &B)
{
	GVector3 Cu = cast_vec3(pCrv->EvalDeriv(u, 1));
	GVector3 Cuu = cast_vec3(pCrv->EvalDeriv(u, 2));
	GVector3 Cuuu = cast_vec3(pCrv->EvalDeriv(u, 3));

	(T = Cu).Normalize();
	(B = T ^ Cuu).Normalize();
	(N = B ^ T).Normalize();
}

/*!
*	\brief 주어진 곡선의 rotation minimizing frame 리스트를 계산한다.
*
*	\param pCrv 곡선의 포인터
*	\param snum 계산할 프레임의 개수
*	\param FrameList 계산된 프레임이 저장될 변수
*/
void get_rotation_minimizing_frames(GNurbsCrv *pCrv, int snum, std::vector<GTransf> &FrameList)
{
	// 좌표계 리스트를 초기화한다.
	FrameList.clear();

	// 정의역을 구하고, RMF 계산에 필요한 변수를 정의한다.
	double umin = 0.0, umax = 1.0;
	pCrv->GetDomain(&umin, &umax);	
	std::vector<GVector3> Ps, Xs, Ys, xL, yL;

	// 곡선의 시작 좌표계를 계산하여 추가한다.
	GVector3 p, x_axis, y_axis, z_axis;
	p = cast_vec3(pCrv->Eval(umin));
	y_axis = cast_vec3(pCrv->EvalDeriv(umin, 1));
	x_axis = ortho(y_axis);
	x_axis.Normalize();
	y_axis.Normalize();
	Ps.push_back(p);
	Xs.push_back(x_axis);
	Ys.push_back(y_axis);
	xL.push_back(GVector3());
	yL.push_back(GVector3());

	// 곡선의 정점과 접벡터를 구하여 좌표계 리스트에 추가한다.
	for (int i = 1; i < snum; ++i)
	{
		double u = umin + (umax - umin) * (double)i / (double)(snum - 1);
		p = cast_vec3(pCrv->Eval(u));
		y_axis = cast_vec3(pCrv->EvalDeriv(u, 1));
		y_axis.Normalize();
		Ps.push_back(p);
		Xs.push_back(GVector3());
		Ys.push_back(y_axis);
		xL.push_back(GVector3());
		yL.push_back(GVector3());
	}

	// 각 정점에서 RMF를 구한다.
	GTransf f(Ps[0], GQuater(Xs[0], Ys[0]));
	FrameList.push_back(f);
	for (int i = 0; i < snum - 1; i++)
	{
		GVector3 V1 = Ps[i + 1] - Ps[i];
		double tmp1 = V1 * V1;
		xL[i] = Xs[i] - (2.0 / tmp1) * (V1 * Xs[i]) * V1;
		yL[i] = Ys[i] - (2.0 / tmp1) * (V1 * Ys[i]) * V1;

		GVector3 V2 = Ys[i + 1] - yL[i];
		double tmp2 = V2 * V2;
		Xs[i + 1] = xL[i] - (2.0 / tmp2) * (V2 * xL[i]) * V2;
		Xs[i + 1].Normalize();

		GTransf f(Ps[i + 1], GQuater(Xs[i + 1], Ys[i + 1]));
		FrameList.push_back(f);
	}
}

/*!
*	\brief 삼항계수(= n! / (i! * j! * k!))를 계산한다.
*
*	\return n! / (i! * j! * k!)를 반환한다.
*/
double TriCoeff(int n, int i, int j, int k)
{
	int n_fact = 1;
	for (int idx = 0; idx < n; ++idx)
		n_fact = n_fact * (n - idx);

	int i_fact = 1;
	for (int idx = 0; idx < i; ++idx)
		i_fact = i_fact * (i - idx);

	int j_fact = 1;
	for (int idx = 0; idx < j; ++idx)
		j_fact = j_fact * (j - idx);

	int k_fact = 1;
	for (int idx = 0; idx < k; ++idx)
		k_fact = k_fact * (k - idx);

	double ret = (double)n_fact / ((double)i_fact * (double)j_fact * (double)k_fact);
	return ret;
}

/*!
*	\brief 주어진 흩어진 점군(scattered data point)을 근사하는 uniform cubic B-spline 곡면을 생성
*
*	\param m 곡면의 U-방향 제어점의 인덱스
*	\param n 곡면의 V-방향 제어점의 인덱스
*	\param num_data 데이터의 개수
*	\param params 정점의 파라미터 배열, (u_i, v_i) = (params[i * 2], params[i * 2 + 1])
*	\param pts 근사할 함수의 값
*	\param num_level 근사할 레벨의 수
*
*	\return 주어진 흩어진 점군(scattered data point) 근사하는 넙스곡면을 반환
*/
GUcbsSrf1D *get_gucbs_srf_approx(int m, int n, int num_data, double *params, double *pts, int num_level)
{
	// Static variables for cubic B-spline bases
	static double basis_u[4];
	static double basis_v[4];

	// Create uniform cubic B-spline function to be returned
	std::string name = create_mesh_name("approx_srf");
	GUcbsSrf1D *pSrf = new GUcbsSrf1D(name, GTransf(), m, n);

	// Set residual as sample values
	double *residual = new double [num_data];
	ARR_COPY(residual, pts, num_data);


	// Compute maximum number of contrl ponints.
	int max_level = (int)pow(2.0, num_level - 1);
	int num_ctlpt = (max_level * (m - 2) + 3) * (max_level * (n - 2) + 3);
	double *ctlpt = new double [num_ctlpt];
	double *omega = new double [num_ctlpt];
	double *delta = new double [num_ctlpt];

	for (int i = 0; i < num_level; ++i)
	{
		m = pSrf->m;
		n = pSrf->n;

		// Initialize ctrlpt, omega, delta for computation
		num_ctlpt = (m + 1) * (n + 1);
		ARR_ZERO(ctlpt, num_ctlpt);
		ARR_ZERO(omega, num_ctlpt);
		ARR_ZERO(delta, num_ctlpt);

		// For each data points, compute omega and delta values.
		for (int j = 0; j < num_data; ++j)
		{
			double u = params[j * 2] * (m - 2);
			double v = params[j * 2 + 1] * (n - 2);

			// P(k,l) will be the left bottom control points
			int k = (int)u;		
			int l = (int)v;		

			// Get the local parameter of u and v
			u = u - k;	
			v = v - l;	

			if (k == m - 2)	// special case for u is 1.0. 
			{
				k = m - 3;
				u = 1.0;
			}
			if (l == n - 2)  // special case for v = 1.0.
			{
				l = n - 3;
				v = 1.0;
			}

			// Get uniform cubic B-spline basis function values
			get_ucbs_basis(u, basis_u);
			get_ucbs_basis(v, basis_v);

			// 			int uidx = (int)(u * 1000.0 + 0.5);
			// 			int vidx = (int)(v * 1000.0 + 0.5);
			// 			ARR_COPY(basis_u, CBS_BASIS[uidx], 4);
			// 			ARR_COPY(basis_v, CBS_BASIS[vidx], 4);

			// Compute the sum of squared weight.
			double sum_sqr_wgt = 0.0;
			for (int p = 0; p < 4; p++)
				for (int q = 0; q < 4; q++)
					sum_sqr_wgt += (SQR(basis_u[p] * basis_v[q]));

			// Compute delta and omega for each control points.
			for (int p = 0; p < 4; p++)
			{
				for (int q = 0; q < 4; q++)
				{
					int pidx = (k + p) * (n + 1) + l + q;
					double wgt = basis_u[p] * basis_v[q];
					double sqr_wgt = SQR(wgt);

					omega[pidx] += sqr_wgt;
					delta[pidx] += (sqr_wgt * wgt * residual[j] / sum_sqr_wgt);
				}
			}
		}

		// Compute control points in object space.
		for (int j = 0; j < num_ctlpt; ++j)
		{
			if (omega[j] == 0.0)
				ctlpt[j] = 0.0;
			else
				ctlpt[j] = delta[j] / omega[j];

			pSrf->P[j] += ctlpt[j];
		}

		// 5. Compute differnece between given data points and approximated data points.
		for (int j = 0; j < num_data; j++)
		{
			GVector4 pt;
			pt = pSrf->Eval(params[2 * j], params[2 * j + 1]);
			residual[j] = pts[j] - pt[0];
		}

		// 6. Refine surface's control point
		if (i != num_level - 1)
			pSrf->Refine();		
	}

	// 7. Memory free
	delete [] ctlpt;
	delete [] omega;
	delete [] delta;
	delete [] residual;

	// 8. Return displacement function.
	return pSrf;
}

/*!
*	\brief 주어진 흩어진 점군(scattered data point)을 근사하는 uniform cubic B-spline 함수(높이맵)를 생성
*
*	\param m 곡면의 U-방향 제어점의 인덱스
*	\param n 곡면의 V-방향 제어점의 인덱스
*	\param num_data 데이터의 개수
*	\param params 정점의 파라미터 배열, (u_i, v_i) = (params[i * 2], params[i * 2 + 1])
*	\param pts 근사할 함수의 값
*	\param num_level 근사할 레벨의 수
*	\param srfs 각 단계의 변위곡면이 저장됨
*
*	\return 주어진 흩어진 점군(scattered data point) 근사하는 근사함수(높이맵)을 반환
*/
GUcbsSrf1D *get_gucbs_srf_approx_hiera(int m, int n, int num_data, double *params, double *pts, int num_level, std::vector<GUcbsSrf1D *> &srfs)
{
	// Static variables for cubic B-spline bases
	static double basis_u[4];
	static double basis_v[4];

	// Create uniform cubic B-spline function to be returned
	std::string name = create_mesh_name("approx_srf");
	GUcbsSrf1D *pSrf = new GUcbsSrf1D(name, GTransf(), m, n);

	// Set residual as sample values
	double *residual = new double [num_data];
	ARR_COPY(residual, pts, num_data);

	// Compute maximum number of contrl ponints.
	int max_level = (int)pow(2.0, num_level - 1);
	int num_ctlpt = (max_level * (m - 2) + 3) * (max_level * (n - 2) + 3);
	double *ctlpt = new double [num_ctlpt];
	double *omega = new double [num_ctlpt];
	double *delta = new double [num_ctlpt];

	for (int i = 0; i < num_level; ++i)
	{
		m = pSrf->m;
		n = pSrf->n;

		// Initialize ctrlpt, omega, delta for computation
		num_ctlpt = (m + 1) * (n + 1);
		ARR_ZERO(ctlpt, num_ctlpt);
		ARR_ZERO(omega, num_ctlpt);
		ARR_ZERO(delta, num_ctlpt);

		// For each data points, compute omega and delta values.
		for (int j = 0; j < num_data; ++j)
		{
			double u = params[j * 2] * (m - 2);
			double v = params[j * 2 + 1] * (n - 2);

			// P(k,l) will be the left bottom control points
			int k = (int)u;		
			int l = (int)v;		

			// Get the local parameter of u and v
			u = u - k;	
			v = v - l;	

			if (k == m - 2)	// special case for u is 1.0. 
			{
				k = m - 3;
				u = 1.0;
			}
			if (l == n - 2)  // special case for v = 1.0.
			{
				l = n - 3;
				v = 1.0;
			}

			// Get uniform cubic B-spline basis function values
			get_ucbs_basis(u, basis_u);
			get_ucbs_basis(v, basis_v);

			// Compute the sum of squared weight.
			double sum_sqr_wgt = 0.0;
			for (int p = 0; p < 4; p++)
				for (int q = 0; q < 4; q++)
					sum_sqr_wgt += (SQR(basis_u[p] * basis_v[q]));

			// Compute delta and omega for each control points.
			for (int p = 0; p < 4; p++)
			{
				for (int q = 0; q < 4; q++)
				{
					int pidx = (k + p) * (n + 1) + l + q;
					double wgt = basis_u[p] * basis_v[q];
					double sqr_wgt = SQR(wgt);

					omega[pidx] += sqr_wgt;
					delta[pidx] += (sqr_wgt * wgt * residual[j] / sum_sqr_wgt);
				}
			}
		}

		// Compute control points in object space.
		for (int j = 0; j < num_ctlpt; ++j)
		{
			if (omega[j] == 0.0)
				ctlpt[j] = 0.0;
			else
				ctlpt[j] = delta[j] / omega[j];

			pSrf->P[j] += ctlpt[j];
		}

		// Create displacement function 
		GUcbsSrf1D *pDispSrf = (i == 0) ? (new GUcbsSrf1D(*pSrf)) : (new GUcbsSrf1D(name, GTransf(), m, n, ctlpt));
		srfs.push_back(pDispSrf);

		// Compute differnece between given data points and approximated data points.
		for (int j = 0; j < num_data; ++j)
		{
			GVector4 pt;
			pt = pSrf->Eval(params[2 * j], params[2 * j + 1]);
			residual[j] = pts[j] - pt[0];
		}

		// Refine surface's control point
		if (i != num_level - 1)
			pSrf->Refine();	
	}

	ARR_COPY(pts, residual, num_data);

	// Memory free
	delete [] ctlpt;
	delete [] omega;
	delete [] delta;
	delete [] residual;

	// Return displacement function.
	return pSrf;
}

/*!
*	\brief 주어진 흩어진 점군(scattered data point)을 근사하는 uniform cubic B-spline 곡면을 생성
*	\note  기본적인 multi-level B-spline approximation 알고리즘
*
*	\param m 곡면의 U-방향 제어점의 인덱스
*	\param n 곡면의 V-방향 제어점의 인덱스
*	\param num_data 데이터의 개수
*	\param params 정점의 파라미터 배열, (u_i, v_i) = (params[i * 2], params[i * 2 + 1])
*	\param pts 근사할 함수의 값
*	\param num_level 근사할 레벨의 수
*
*	\return 주어진 흩어진 점군(scattered data point) 근사하는 넙스곡면을 반환
*/
GUcbsSrf *get_gucbs_srf_approx(int m, int n, int num_data, double *params, GVector4 *pts, int num_level)
{
	// Static variables for cubic B-spline bases
	static double basis_u[4];
	static double basis_v[4];

	// Create uniform cubic B-spline function to be returned
	std::string name = create_mesh_name("approx_srf");
	GUcbsSrf *pSrf = new GUcbsSrf(name, GTransf(), m, n);

	// Set residual as sample values
	GVector4 *residual = new GVector4 [num_data];
	ARR_COPY(residual, pts, num_data);

	// Compute maximum number of contrl ponints.
	int max_level = (int)pow(2.0, num_level - 1);
	int num_ctlpt = (max_level * (m - 2) + 3) * (max_level * (n - 2) + 3);
	GVector4 *ctlpt = new GVector4 [num_ctlpt];
	double *omega = new double [num_ctlpt];
	GVector4 *delta = new GVector4 [num_ctlpt];

	for (int i = 0; i < num_level; ++i)
	{
		// Initialize ctrlpt, omega, delta for computation
		m = pSrf->m;
		n = pSrf->n;
		num_ctlpt = (m + 1) * (n + 1);
		ARR_ZERO(ctlpt, num_ctlpt);
		ARR_ZERO(omega, num_ctlpt);
		ARR_ZERO(delta, num_ctlpt);

		// For each data points, compute omega and delta values.
		for (int j = 0; j < num_data; ++j)
		{
			double u = params[j * 2] * (m - 2);
			double v = params[j * 2 + 1] * (n - 2);

			// P(k,l) will be the left bottom control points
			int k = (int)u;		
			int l = (int)v;		

			// Get the local parameter of u and v
			u = u - k;	
			v = v - l;	

			if (k == m - 2)	// special case for u is 1.0. 
			{
				k = m - 3;
				u = 1.0;
			}
			if (l == n - 2)  // special case for v = 1.0.
			{
				l = n - 3;
				v = 1.0;
			}

			// Get uniform cubic B-spline basis function values
// 			get_ucbs_basis(u, basis_u);
// 			get_ucbs_basis(v, basis_v);

			int uidx = (int)(u * 1000.0 + 0.5);
			int vidx = (int)(v * 1000.0 + 0.5);
			ARR_COPY(basis_u, CBS_BASIS[uidx], 4);
			ARR_COPY(basis_v, CBS_BASIS[vidx], 4);

			// Compute the sum of squared weight.
			double sum_sqr_wgt = 0.0;
			for (int p = 0; p < 4; p++)
				for (int q = 0; q < 4; q++)
					sum_sqr_wgt += (SQR(basis_u[p] * basis_v[q]));

			// Compute delta and omega for each control points.
			for (int p = 0; p < 4; p++)
			{
				for (int q = 0; q < 4; q++)
				{
					int pidx = (k + p) * (n + 1) + l + q;
					double wgt = basis_u[p] * basis_v[q];
					double sqr_wgt = SQR(wgt);

					omega[pidx] += sqr_wgt;
					delta[pidx] += (sqr_wgt * wgt * residual[j] / sum_sqr_wgt);
				}
			}
		}

		// Compute control points in object space.
		for (int j = 0; j < num_ctlpt; ++j)
		{
			if (omega[j] == 0.0)
				ctlpt[j].Set(0.0, 0.0, 0.0, 0.0);
			else
				ctlpt[j] = delta[j] / omega[j];

			pSrf->P[j] = pSrf->P[j] + ctlpt[j];
		}

		// 5. Compute differnece between given data points and approximated data points.
		for (int j = 0; j < num_data; ++j)
		{
			GVector4 pt;
			pt = pSrf->Eval(params[2 * j], params[2 * j + 1]);
			residual[j] = pts[j] - pt;
		}

		// 6. Refine surface's control point
		if (i != num_level - 1)
			pSrf->Refine();		
	}

	ARR_COPY(pts, residual, num_data);

	// 7. Memory free
	delete [] ctlpt;
	delete [] omega;
	delete [] delta;
	delete [] residual;

	// 8. Return displacement function.
	return pSrf;
}

/*!
*	\brief 주어진 흩어진 점군(scattered data point)을 근사하는 uniform cubic B-spline 곡면을 생성
*	\note  초기 단계에서 4개의 가장자리 점들에 대한 근사곡면을 생성함
*
*	\param m 곡면의 U-방향 제어점의 인덱스
*	\param n 곡면의 V-방향 제어점의 인덱스
*	\param num_data 데이터의 개수
*	\param params 정점의 파라미터 배열, (u_i, v_i) = (params[i * 2], params[i * 2 + 1])
*	\param pts 근사할 함수의 값
*	\param num_level 근사할 레벨의 수
*
*	\return 주어진 흩어진 점군(scattered data point) 근사하는 넙스곡면을 반환
*/
GUcbsSrf *get_gucbs_srf_approx_ver2(int m, int n, int num_data, double *params, GVector4 *pts, int num_level)
{
	// Static variables for cubic B-spline bases
	static double basis_u[4];
	static double basis_v[4];

	// uniform sampling을 이용하여 3차 cubic B-spline 곡면으로 초기 피팅을 수행
	int idx = 0;
	double param1[32];
	GVector4 pts1[16];
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			param1[2 * idx] = (double)i / 3.0;
			param1[2 * idx + 1] = (double)j / 3.0;

			double len_min = 1000000.0;
			for (int k = 0; k < num_data; ++k)
			{
				double len = SQR(param1[2 * idx] - params[2 * k]) + SQR(param1[2 * idx + 1] - params[2 * k + 1]);
				if (len < len_min)
				{
					len_min = len;
					pts1[idx] = pts[k];
				}
			}
			idx++;
		}
	}

	double U[8] = {-3.0, -2.0, -1.0, 0.0, 1.0, 2.0, 3.0, 4.0};
	double V[8] = {-3.0, -2.0, -1.0, 0.0, 1.0, 2.0, 3.0, 4.0};
	GNurbsSrf *pBaseSrf = ::get_gnurbs_srf_approx_ver2(3, 3, U, 3, 3, V, 16, param1, pts1, 1);

	// Create uniform cubic B-spline function to be returned
	std::string name = create_mesh_name("approx_srf");
	GUcbsSrf *pSrf = new GUcbsSrf(name, GTransf(), m, n);
	for (int i = 0; i < m + 1; ++i)
		for (int j = 0; j < n + 1; ++j)
			pSrf->P[i * (n + 1) + j] = pBaseSrf->P[i][j];
	delete pBaseSrf;

	// Set residual as sample values
	GVector4 *residual = new GVector4 [num_data];
	for (int i = 0; i < num_data; ++i)
	{
		GVector4 pt;
		pt = pSrf->Eval(params[2 * i], params[2 * i + 1]);
		residual[i] = pts[i] - pt;
	}

	// Compute maximum number of contrl ponints.
	int max_level = (int)pow(2.0, num_level - 1);
	int num_ctlpt = (max_level * (m - 2) + 3) * (max_level * (n - 2) + 3);
	GVector4 *ctlpt = new GVector4 [num_ctlpt];
	double *omega = new double [num_ctlpt];
	GVector4 *delta = new GVector4 [num_ctlpt];

	for (int i = 0; i < num_level; ++i)
	{
		m = pSrf->m;
		n = pSrf->n;

		// Initialize ctrlpt, omega, delta for computation
		num_ctlpt = (m + 1) * (n + 1);
		ARR_ZERO(ctlpt, num_ctlpt);
		ARR_ZERO(omega, num_ctlpt);
		ARR_ZERO(delta, num_ctlpt);

		// For each data points, compute omega and delta values.
		for (int j = 0; j < num_data; ++j)
		{
			double u = params[j * 2] * (m - 2);
			double v = params[j * 2 + 1] * (n - 2);

			// P(k,l) will be the left bottom control points
			int k = (int)u;		
			int l = (int)v;		

			// Get the local parameter of u and v
			u = u - k;	
			v = v - l;	

			if (k == m - 2)	// special case for u is 1.0. 
			{
				k = m - 3;
				u = 1.0;
			}
			if (l == n - 2)  // special case for v = 1.0.
			{
				l = n - 3;
				v = 1.0;
			}

			// Get uniform cubic B-spline basis function values
			get_ucbs_basis(u, basis_u);
			get_ucbs_basis(v, basis_v);

			// 			int uidx = (int)(u * 1000.0 + 0.5);
			// 			int vidx = (int)(v * 1000.0 + 0.5);
			// 			ARR_COPY(basis_u, CBS_BASIS[uidx], 4);
			// 			ARR_COPY(basis_v, CBS_BASIS[vidx], 4);

			// Compute the sum of squared weight.
			double sum_sqr_wgt = 0.0;
			for (int p = 0; p < 4; p++)
				for (int q = 0; q < 4; q++)
					sum_sqr_wgt += (SQR(basis_u[p] * basis_v[q]));

			// Compute delta and omega for each control points.
			for (int p = 0; p < 4; p++)
			{
				for (int q = 0; q < 4; q++)
				{
					int pidx = (k + p) * (n + 1) + l + q;
					double wgt = basis_u[p] * basis_v[q];
					double sqr_wgt = SQR(wgt);

					omega[pidx] += sqr_wgt;
					delta[pidx] += (sqr_wgt * wgt * residual[j] / sum_sqr_wgt);
				}
			}
		}

		// Compute control points in object space.
		for (int j = 0; j < num_ctlpt; ++j)
		{
			if (omega[j] == 0.0)
				ctlpt[j].Set(0.0, 0.0, 0.0, 0.0);
			else
				ctlpt[j] = delta[j] / omega[j];

			pSrf->P[j] = pSrf->P[j] + ctlpt[j];
		}

		// 5. Compute differnece between given data points and approximated data points.
		for (int j = 0; j < num_data; ++j)
		{
			GVector4 pt;
			pt = pSrf->Eval(params[2 * j], params[2 * j + 1]);
			residual[j] = pts[j] - pt;
		}

		// 6. Refine surface's control point
		if (i != num_level - 1)
			pSrf->Refine();		
	}

	ARR_COPY(pts, residual, num_data);

	// 7. Memory free
	delete [] ctlpt;
	delete [] omega;
	delete [] delta;
	delete [] residual;

	// 8. Return displacement function.
	return pSrf;
}

/*!
*	\brief 주어진 흩어진 점군(scattered data point)을 근사하는 uniform cubic B-spline 곡면을 생성
*	\note  초기 단계에서 경계 상자를 이용한 근사곡면을 생성함
*
*	\param m 곡면의 U-방향 제어점의 인덱스
*	\param n 곡면의 V-방향 제어점의 인덱스
*	\param num_data 데이터의 개수
*	\param params 정점의 파라미터 배열, (u_i, v_i) = (params[i * 2], params[i * 2 + 1])
*	\param pts 근사할 함수의 값
*	\param num_level 근사할 레벨의 수
*	\param p 경계상자의 최소점
*	\param q 경계상자의 최대점
*
*	\return 주어진 흩어진 점군(scattered data point) 근사하는 넙스곡면을 반환
*/
GUcbsSrf *get_gucbs_srf_approx_ver3(int m, int n, int num_data, double *params, GVector4 *pts, int num_level, GPoint3 p, GPoint3 q)
{
	// Static variables for cubic B-spline bases
	static double basis_u[4];
	static double basis_v[4];

	// 경계상자의 정점을 이용하여 초기 피팅을 수행
	double U[8] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
	double V[8] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
	GVector4 P[16];
	double alpha = 1.0 / 3.0;
	P[0].Set(p[0], p[1], p[2], 0.0);
	P[3].Set(p[0], q[1], p[2], 0.0);
	P[1] = alpha * P[3] + (1.0 - alpha) * P[0];
	P[2] = alpha * P[0] + (1.0 - alpha) * P[3];

	P[4].Set(p[0], p[1], q[2], 0.0);
	P[7].Set(p[0], q[1], q[2], 0.0);
	P[5] = alpha * P[7] + (1.0 - alpha) * P[4];
	P[6] = alpha * P[4] + (1.0 - alpha) * P[7];

	P[8].Set(q[0], p[1], q[2], 0.0);
	P[11].Set(q[0], q[1], q[2], 0.0);
	P[9] = alpha * P[11] + (1.0 - alpha) * P[8];
	P[10] = alpha * P[8] + (1.0 - alpha) * P[11];

	P[12].Set(q[0], p[1], p[2], 0.0);
	P[15].Set(q[0], q[1], p[2], 0.0);
	P[13] = alpha * P[15] + (1.0 - alpha) * P[12];
	P[14] = alpha * P[12] + (1.0 - alpha) * P[15];

	GNurbsSrf *pBaseSrf = new GNurbsSrf("noname", GTransf(), 3, 3, U, 3, 3, V, P, false);
	pBaseSrf->KnotUnclamped(true, true);

	// Create uniform cubic B-spline function to be returned
	std::string name = create_mesh_name("approx_srf");
	GUcbsSrf *pSrf = new GUcbsSrf(name, GTransf(), m, n);
	for (int i = 0; i < m + 1; ++i)
		for (int j = 0; j < n + 1; ++j)
			pSrf->P[i * (n + 1) + j] = pBaseSrf->P[i][j];
	delete pBaseSrf;

	// Set residual as sample values
	GVector4 *residual = new GVector4 [num_data];
	for (int i = 0; i < num_data; ++i)
	{
		GVector4 pt;
		pt = pSrf->Eval(params[2 * i], params[2 * i + 1]);
		residual[i] = pts[i] - pt;
	}

	// Compute maximum number of contrl ponints.
	int max_level = (int)pow(2.0, num_level - 1);
	int num_ctlpt = (max_level * (m - 2) + 3) * (max_level * (n - 2) + 3);
	GVector4 *ctlpt = new GVector4 [num_ctlpt];
	double *omega = new double [num_ctlpt];
	GVector4 *delta = new GVector4 [num_ctlpt];

	for (int i = 0; i < num_level; ++i)
	{
		m = pSrf->m;
		n = pSrf->n;

		// Initialize ctrlpt, omega, delta for computation
		num_ctlpt = (m + 1) * (n + 1);
		ARR_ZERO(ctlpt, num_ctlpt);
		ARR_ZERO(omega, num_ctlpt);
		ARR_ZERO(delta, num_ctlpt);

		// For each data points, compute omega and delta values.
		for (int j = 0; j < num_data; ++j)
		{
			double u = params[j * 2] * (m - 2);
			double v = params[j * 2 + 1] * (n - 2);

			// P(k,l) will be the left bottom control points
			int k = (int)u;		
			int l = (int)v;		

			// Get the local parameter of u and v
			u = u - k;	
			v = v - l;	

			if (k == m - 2)	// special case for u is 1.0. 
			{
				k = m - 3;
				u = 1.0;
			}
			if (l == n - 2)  // special case for v = 1.0.
			{
				l = n - 3;
				v = 1.0;
			}

			// Get uniform cubic B-spline basis function values
			get_ucbs_basis(u, basis_u);
			get_ucbs_basis(v, basis_v);

			// 			int uidx = (int)(u * 1000.0 + 0.5);
			// 			int vidx = (int)(v * 1000.0 + 0.5);
			// 			ARR_COPY(basis_u, CBS_BASIS[uidx], 4);
			// 			ARR_COPY(basis_v, CBS_BASIS[vidx], 4);

			// Compute the sum of squared weight.
			double sum_sqr_wgt = 0.0;
			for (int p = 0; p < 4; p++)
				for (int q = 0; q < 4; q++)
					sum_sqr_wgt += (SQR(basis_u[p] * basis_v[q]));

			// Compute delta and omega for each control points.
			for (int p = 0; p < 4; p++)
			{
				for (int q = 0; q < 4; q++)
				{
					int pidx = (k + p) * (n + 1) + l + q;
					double wgt = basis_u[p] * basis_v[q];
					double sqr_wgt = SQR(wgt);

					omega[pidx] += sqr_wgt;
					delta[pidx] += (sqr_wgt * wgt * residual[j] / sum_sqr_wgt);
				}
			}
		}

		// Compute control points in object space.
		for (int j = 0; j < num_ctlpt; ++j)
		{
			if (omega[j] == 0.0)
				ctlpt[j].Set(0.0, 0.0, 0.0, 0.0);
			else
				ctlpt[j] = delta[j] / omega[j];

			pSrf->P[j] = pSrf->P[j] + ctlpt[j];
		}

		// 5. Compute differnece between given data points and approximated data points.
		for (int j = 0; j < num_data; ++j)
		{
			GVector4 pt;
			pt = pSrf->Eval(params[2 * j], params[2 * j + 1]);
			residual[j] = pts[j] - pt;
		}

		// 6. Refine surface's control point
		if (i != num_level - 1)
			pSrf->Refine();		
	}

	ARR_COPY(pts, residual, num_data);

	// 7. Memory free
	delete [] ctlpt;
	delete [] omega;
	delete [] delta;
	delete [] residual;

	// 8. Return displacement function.
	return pSrf;
}

/*!
*	\brief 주어진 흩어진 점군(scattered data point)을 근사하는 uniform cubic B-spline 곡면을 생성
*	\note 각 단계에서 최적화를 수행함
*
*	\param m 곡면의 U-방향 제어점의 인덱스
*	\param n 곡면의 V-방향 제어점의 인덱스
*	\param num_data 데이터의 개수
*	\param params 정점의 파라미터 배열, (u_i, v_i) = (params[i * 2], params[i * 2 + 1])
*	\param pts 근사할 함수의 값
*	\param num_level 근사할 레벨의 수
*
*	\return 주어진 흩어진 점군(scattered data point) 근사하는 넙스곡면을 반환
*/
GUcbsSrf *get_gucbs_srf_approx_ver4(int m, int n, int num_data, double *params, GVector4 *pts, int num_level)
{
	// Set residual as sample values
	GVector4 *residual = new GVector4 [num_data];
	ARR_COPY(residual, pts, num_data);

	// Create uniform cubic B-spline function to be returned
	std::string name = create_mesh_name("approx_srf");
	GUcbsSrf *pSrf = new GUcbsSrf(name, GTransf(), m, n);

	for (int i = 0; i < num_level; ++i)
	{
		m = pSrf->m;
		n = pSrf->n;

		double sum_sqr_err = 1.0e+15;
		while(true)
		{
			GUcbsSrf *pAuxSrf = ::get_gucbs_srf_approx(m, n, num_data, params, residual, 1);

			double new_err = 0.0;
			for (int j = 0; j < num_data; ++j)
				new_err += norm(residual[j]);

			if (sum_sqr_err - new_err < 0.1)
			{
				delete pAuxSrf;
				break;
			}

			int num_ctlpt = (m + 1) * (n + 1);
			for (int j = 0; j < num_ctlpt; ++j)
				pSrf->P[j] += pAuxSrf->P[j];

			sum_sqr_err = new_err;			
			delete pAuxSrf;
		}

		// Compute differnece between given data points and approximated data points.
		for (int j = 0; j < num_data; ++j)
		{
			GVector4 pt;
			pt = pSrf->Eval(params[2 * j], params[2 * j + 1]);
			residual[j] = pts[j] - pt;
		}

		// Refine surface's control point
		if (i != num_level - 1)
			pSrf->Refine();		
	}

	ARR_COPY(pts, residual, num_data);

	// Memory free
	delete [] residual;

	// Return displacement function.
	return pSrf;
}

/*!
*	\brief 주어진 흩어진 점군(scattered data point)을 근사하는 uniform cubic B-spline 곡면을 생성
*	\note  초기 단계에서 4개의 가장자리 점들에 대한 근사곡면을 생성함
*
*	\param m 곡면의 U-방향 제어점의 인덱스
*	\param n 곡면의 V-방향 제어점의 인덱스
*	\param num_data 데이터의 개수
*	\param params 정점의 파라미터 배열, (u_i, v_i) = (params[i * 2], params[i * 2 + 1])
*	\param pts 근사할 함수의 값
*	\param num_level 근사할 레벨의 수
*	\param srfs 각 단계의 변위곡면이 저장됨
*
*	\return 주어진 흩어진 점군(scattered data point) 근사하는 넙스곡면을 반환
*/
GUcbsSrf *get_gucbs_srf_approx_hiera(int m, int n, int num_data, double *params, GVector4 *pts, int num_level, std::vector<GUcbsSrf *> &srfs)
{
	// Static variables for cubic B-spline bases
	static double basis_u[4];
	static double basis_v[4];

	// uniform sampling을 이용하여 3차 cubic B-spline 곡면으로 초기 피팅을 수행
	int idx = 0;
	double param1[32];
	GVector4 pts1[16];
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			param1[2 * idx] = (double)i / 3.0;
			param1[2 * idx + 1] = (double)j / 3.0;

			double len_min = 1000000.0;
			for (int k = 0; k < num_data; ++k)
			{
				double len = SQR(param1[2 * idx] - params[2 * k]) + SQR(param1[2 * idx + 1] - params[2 * k + 1]);
				if (len < len_min)
				{
					len_min = len;
					pts1[idx] = pts[k];
				}
			}
			idx++;
		}
	}

	double U[8] = {-3.0, -2.0, -1.0, 0.0, 1.0, 2.0, 3.0, 4.0};
	double V[8] = {-3.0, -2.0, -1.0, 0.0, 1.0, 2.0, 3.0, 4.0};
	GNurbsSrf *pBaseSrf = ::get_gnurbs_srf_approx_ver2(3, 3, U, 3, 3, V, 16, param1, pts1, 1);

	// Create uniform cubic B-spline function to be returned
	std::string name = create_mesh_name("approx_srf");
	GUcbsSrf *pSrf = new GUcbsSrf(name, GTransf(), m, n);
	for (int i = 0; i < m + 1; ++i)
		for (int j = 0; j < n + 1; ++j)
			pSrf->P[i * (n + 1) + j] = pBaseSrf->P[i][j];
	delete pBaseSrf;

	// Set residual as sample values
	GVector4 *residual = new GVector4 [num_data];
	for (int i = 0; i < num_data; ++i)
	{
		GVector4 pt;
		pt = pSrf->Eval(params[2 * i], params[2 * i + 1]);
		residual[i] = pts[i] - pt;
	}

	// Compute maximum number of contrl ponints.
	int max_level = (int)pow(2.0, num_level - 1);
	int num_ctlpt = (max_level * (m - 2) + 3) * (max_level * (n - 2) + 3);
	GVector4 *ctlpt = new GVector4 [num_ctlpt];
	double *omega = new double [num_ctlpt];
	GVector4 *delta = new GVector4 [num_ctlpt];

	for (int i = 0; i < num_level; ++i)
	{
		m = pSrf->m;
		n = pSrf->n;

		// Initialize ctrlpt, omega, delta for computation
		num_ctlpt = (m + 1) * (n + 1);
		ARR_ZERO(ctlpt, num_ctlpt);
		ARR_ZERO(omega, num_ctlpt);
		ARR_ZERO(delta, num_ctlpt);

		// For each data points, compute omega and delta values.
		for (int j = 0; j < num_data; ++j)
		{
			double u = params[j * 2] * (m - 2);
			double v = params[j * 2 + 1] * (n - 2);

			// P(k,l) will be the left bottom control points
			int k = (int)u;		
			int l = (int)v;		

			// Get the local parameter of u and v
			u = u - k;	
			v = v - l;	

			if (k == m - 2)	// special case for u is 1.0. 
			{
				k = m - 3;
				u = 1.0;
			}
			if (l == n - 2)  // special case for v = 1.0.
			{
				l = n - 3;
				v = 1.0;
			}

			// Get uniform cubic B-spline basis function values
			get_ucbs_basis(u, basis_u);
			get_ucbs_basis(v, basis_v);

			// Compute the sum of squared weight.
			double sum_sqr_wgt = 0.0;
			for (int p = 0; p < 4; p++)
				for (int q = 0; q < 4; q++)
					sum_sqr_wgt += (SQR(basis_u[p] * basis_v[q]));

			// Compute delta and omega for each control points.
			for (int p = 0; p < 4; p++)
			{
				for (int q = 0; q < 4; q++)
				{
					int pidx = (k + p) * (n + 1) + l + q;
					double wgt = basis_u[p] * basis_v[q];
					double sqr_wgt = SQR(wgt);

					omega[pidx] += sqr_wgt;
					delta[pidx] += (sqr_wgt * wgt * residual[j] / sum_sqr_wgt);
				}
			}
		}

		// Compute control points in object space.
		for (int j = 0; j < num_ctlpt; ++j)
		{
			if (omega[j] == 0.0)
				ctlpt[j].Set(0.0, 0.0, 0.0, 0.0);
			else
				ctlpt[j] = delta[j] / omega[j];

			pSrf->P[j] += ctlpt[j];
		}

		// Create displacement function 
		GUcbsSrf *pDispSrf = (i == 0) ? (new GUcbsSrf(*pSrf)) : (new GUcbsSrf(name, GTransf(), m, n, ctlpt));
		srfs.push_back(pDispSrf);

		// Compute differnece between given data points and approximated data points.
		for (int j = 0; j < num_data; ++j)
		{
			GVector4 pt;
			pt = pSrf->Eval(params[2 * j], params[2 * j + 1]);
			residual[j] = pts[j] - pt;
		}

		// Refine surface's control point
		if (i != num_level - 1)
			pSrf->Refine();	
	}

	ARR_COPY(pts, residual, num_data);

	// Memory free
	delete [] ctlpt;
	delete [] omega;
	delete [] delta;
	delete [] residual;

	// Return displacement function.
	return pSrf;
}

/*!
*	\brief Overlayed된 곡면을 반환한다.
*
*	\param srf 변위 곡면 리스트
*	\param sidx 시작 인덱스
*	\param eidx 끝 인덱스
*
*	\return Overlayed된 곡면(srfs[sidx] + srfs[sidx + 1] + ... + srfs[eidx])을 반환한다.
*/
GUcbsSrf *get_gucbs_srf_overlayed(std::vector<GUcbsSrf *> &srfs, int sidx, int eidx)
{
	GUcbsSrf *pSrf = new GUcbsSrf(*srfs[sidx]);
	for (int i = sidx + 1; i < eidx + 1; ++i)
	{
		pSrf->Refine();
		int num = (pSrf->m + 1) * (pSrf->n + 1);
		for (int j = 0; j < num; ++j)
		{
			pSrf->P[j] += srfs[i]->P[j];
		}
	}

	return pSrf;
}

/*!
*	\brief 기반 곡면의 접평면 필드에서 정의된 변위 곡면을 계산
*
*	\param pBaseSrf 기반 곡면
*	\param pDispSrf 대상 변위 곡면 절대 좌표계에서 표현됨
*
*	\return pBaseSrf의 접평면 필드에서 정의된 pDispSrf를 반환한다.
*/
GUcbsSrf *get_gucbs_srf_tan_frm(GUcbsSrf *pBaseSrf, GUcbsSrf *pDispSrf)
{
	GUcbsSrf *pSrf = new GUcbsSrf(*pDispSrf);

	int m = pSrf->m;
	int n = pSrf->n;
	for (int i = 0; i < m + 1; ++i)
	{
		for (int j = 0; j < n + 1; ++j)
		{
			double u = (double)i / (double)(m);
			double v = (double)j / (double)(n);

			GVector3 Su, Sv, N;
			pBaseSrf->EvalTanPln(u, v, Su, Sv, N);

			Su.Normalize();
			Sv.Normalize();
			N.Normalize();

			int idx = i * (n + 1) + j;
			// 근사된 곡면위의 점 S(u, v)의 접평면에서 표현된 변위 벡터를 계산
			GMatrix M(3, 3);
			M.SetColVec(0, cast_vec(Su));
			M.SetColVec(1, cast_vec(Sv));
			M.SetColVec(2, cast_vec(N));

			GVector4 p = pSrf->P[idx];
			GVector dp(3, p[0], p[1], p[2]);
			dp = inv(M) * dp;
			pSrf->P[idx].Set(dp[0], dp[1], dp[2], 0.0);
		}
	}

	return pSrf;
}

/*!
*	\brief 기반 곡면의 접평면 필드에서 정의된 변위 곡면을 고정 좌표계로 변경
*
*	\param pBaseSrf 기반 곡면
*	\param pDispSrf 대상 곡면(반환 값으로 변환)
*
*	\return 고정 좌표계에서 표현된 변위 곡면을 반환한다.
*/
GUcbsSrf *get_gucbs_srf_fixed_frm(GUcbsSrf *pBaseSrf, GUcbsSrf *pDispSrf)
{
	GUcbsSrf *pSrf = new GUcbsSrf(*pDispSrf);

	int m = pSrf->m;
	int n = pSrf->n;
	for (int i = 0; i < m + 1; ++i)
	{
		for (int j = 0; j < n + 1; ++j)
		{
			double u = (double)i / (double)(m);
			double v = (double)j / (double)(n);

			GVector3 Su, Sv, N;
			pBaseSrf->EvalTanPln(u, v, Su, Sv, N);

			Su.Normalize();
			Sv.Normalize();
			N.Normalize();

			int idx = i * (n + 1) + j;
			// 근사된 곡면위의 점 S(u, v)의 접평면에서 표현된 변위 벡터를 계산
			GMatrix M(3, 3);
			M.SetColVec(0, cast_vec(Su));
			M.SetColVec(1, cast_vec(Sv));
			M.SetColVec(2, cast_vec(N));

			GVector4 p = pSrf->P[idx];
			GVector dp(3, p[0], p[1], p[2]);
			dp = M * dp;
			pSrf->P[idx].Set(dp[0], dp[1], dp[2], 0.0);
		}
	}

	return pSrf;
}

/*!
*	\brief 주어진 흩어진 점군(scattered data point)을 근사하는 uniform cubic B-spline 볼륨을 생성
*	\note  기본적인 multi-level B-spline approximation 알고리즘
*
*	\param l 곡면의 U-방향 제어점의 인덱스
*	\param m 곡면의 V-방향 제어점의 인덱스
*	\param n 곡면의 W-방향 제어점의 인덱스
*	\param num_data 데이터의 개수
*	\param params 정점의 파라미터 배열, (u_i, v_i, w_i) = (params[i * 2], params[i * 2 + 1], params[i * 2 + 2])
*	\param pts 근사할 함수의 값
*	\param num_level 근사할 레벨의 수
*
*	\return 주어진 흩어진 점군(scattered data point) 근사하는 넙스볼륨을 반환
*/
GUcbsVol *get_gucbs_vol_approx(int l, int m, int n, int num_data, double *params, GVector4 *pts, int num_level)
{
	// Static variables for cubic B-spline bases
	static double basis_u[4];
	static double basis_v[4];
	static double basis_w[4];

	// Create uniform cubic B-spline function to be returned
	std::string name = create_mesh_name("approx_vol");
	GUcbsVol *pVol = new GUcbsVol(name, GTransf(), l, m, n);

	// Set residual as sample values
	GVector4 *residual = new GVector4 [num_data];
	ARR_COPY(residual, pts, num_data);

	// Compute maximum number of contrl ponints.
	int max_level = (int)pow(2.0, num_level - 1);
	int num_ctlpt = (max_level * (l - 2) + 3) * (max_level * (m - 2) + 3) * (max_level * (n - 2) + 3);
	GVector4 *ctlpt = new GVector4 [num_ctlpt];
	double *omega = new double [num_ctlpt];
	GVector4 *delta = new GVector4 [num_ctlpt];

 	for (int i = 0; i < num_level; ++i)
 	{
 		l = pVol->l;
 		m = pVol->m;
 		n = pVol->n;
 
 		// Initialize ctrlpt, omega, delta for computation
 		num_ctlpt = (l + 1) * (m + 1) * (n + 1);
 		ARR_ZERO(ctlpt, num_ctlpt);
 		ARR_ZERO(omega, num_ctlpt);
 		ARR_ZERO(delta, num_ctlpt);

		// For each data points, compute omega and delta values.
		for (int j = 0; j < num_data; ++j)
		{
			double u = params[j * 3] * (l - 2);
			double v = params[j * 3 + 1] * (m - 2);
			double w = params[j * 3 + 2] * (n - 2);

			// P(p, q, r) will be the left bottom control points
			int p = (int)u;		
			int q = (int)v;		
			int r = (int)w;		

			// Get the local parameter of u, v, and w
			u = u - p;	
			v = v - q;
			w = w - r;

			if (p == l - 2)	// special case for u = 1.0. 
			{
				p = l - 3;
				u = 1.0;
			}
			if (q == m - 2) // special case for v = 1.0.
			{
				q = m - 3;
				v = 1.0;
			}
			if (r == n - 2) // special case for w = 1.0.
			{
				r = n - 3;
				w = 1.0;
			}

			// Get uniform cubic B-spline basis function values
			get_ucbs_basis(u, basis_u);
			get_ucbs_basis(v, basis_v);
			get_ucbs_basis(w, basis_w);

			// 			int uidx = (int)(u * 1000.0 + 0.5);
			// 			int vidx = (int)(v * 1000.0 + 0.5);
			// 			int widx = (int)(w * 1000.0 + 0.5);
			// 			ARR_COPY(basis_u, CBS_BASIS[uidx], 4);
			// 			ARR_COPY(basis_v, CBS_BASIS[vidx], 4);
			// 			ARR_COPY(basis_w, CBS_BASIS[widx], 4);

			// Compute the sum of squared weight.
			double sum_sqr_wgt = 0.0;
			for (int ii = 0; ii < 4; ++ii)
				for (int jj = 0; jj < 4; ++jj)
					for (int kk = 0; kk < 4; ++kk)
						sum_sqr_wgt += (SQR(basis_u[ii] * basis_v[jj] * basis_w[kk]));

			// Compute delta and omega for each control points.
			for (int ii = 0; ii < 4; ++ii)
			{
				for (int jj = 0; jj < 4; ++jj)
				{
					for (int kk = 0; kk < 4; ++kk)
					{
						int pidx = (p + ii) * (m + 1) * (n + 1) + (q + jj) * (n + 1) + (r + kk);
						double wgt = basis_u[ii] * basis_v[jj] * basis_w[kk];
						double sqr_wgt = SQR(wgt);

						omega[pidx] += sqr_wgt;
						delta[pidx] += (sqr_wgt * wgt * residual[j] / sum_sqr_wgt);
					}					
				}
			}
		}

		// Compute control points in object space.
		for (int j = 0; j < num_ctlpt; ++j)
		{
			if (omega[j] == 0.0)
				ctlpt[j].Set(0.0, 0.0, 0.0, 0.0);
			else
				ctlpt[j] = delta[j] / omega[j];

			pVol->P[j][0] += ctlpt[j][0];
			pVol->P[j][1] += ctlpt[j][1];
			pVol->P[j][2] += ctlpt[j][2];
			pVol->P[j][3] += ctlpt[j][3];
		}

		// 5. Compute differnece between given data points and approximated data points.
		for (int j = 0; j < num_data; ++j)
		{
			GVector4 pt;
			pt = pVol->Eval(params[3 * j], params[3 * j + 1], params[3 * j + 2]);
			residual[j] = pts[j] - pt;
		}

		// 6. Refine surface's control point
		if (i != num_level - 1)
			pVol->Refine();		
 	}

	ARR_COPY(pts, residual, num_data);

	// 7. Memory free
	delete [] ctlpt;
	delete [] omega;
	delete [] delta;
	delete [] residual;

	// 8. Return displacement function.
	return pVol;
}

/*!
*	\brief 측지 경계 경로를 근사하는 Coons 곡면을 생성한다.
*
*	\param BndryPaths 4개의 측지 경계 경로
*
*	\return 생성된 Coons 곡면을 반환한다.
*/
GCoonsSrf *get_coons_srf_from_geodesic_paths(std::vector<GGeoPath *> BndryPaths)
{
	GNurbsCrv *pCrv0 = get_gnurbs_crv_from_geodesic_path(BndryPaths[0]);
	GNurbsCrv *pCrv1 = get_gnurbs_crv_from_geodesic_path(BndryPaths[1]);
	GNurbsCrv *pCrv2 = get_gnurbs_crv_from_geodesic_path(BndryPaths[2]);
	GNurbsCrv *pCrv3 = get_gnurbs_crv_from_geodesic_path(BndryPaths[3]);
	
	GCoonsSrf *pSrf = new GCoonsSrf("", BndryPaths[0]->pRefMesh->MC, pCrv0, pCrv1, pCrv2, pCrv3, "");
	return pSrf;
}

/*!
*	\brief Teapot 객체를 구성하는 B-spline 곡면의 리스트를 생성함
*
*	\param srfs 생성된 곡면 객체의 포인터가 저장될 벡터리스트
*	\param size 생성될 객체의 크기
*/
void get_gnurbs_srf_teapot(std::vector<GNurbsSrf *> &srfs, double size)
{
	double pts[306][3] = {
		1.4, 0.0, 2.4, 1.4, -0.784, 2.4, 0.784, -1.4, 2.4, 0.0, -1.4, 2.4, 1.3375, 0.0, 2.53125, 1.3375, -0.749, 2.53125, 0.749, -1.3375, 2.53125, 0.0, -1.3375, 2.53125, 1.4375, 0.0, 2.53125, 1.4375, -0.805, 2.53125, 
		0.805, -1.4375, 2.53125, 0.0, -1.4375, 2.53125, 1.5, 0.0, 2.4, 1.5, -0.84, 2.4, 0.84, -1.5, 2.4, 0.0, -1.5, 2.4, -0.784, -1.4, 2.4, -1.4, -0.784, 2.4, -1.4, 0.0, 2.4, -0.749, -1.3375, 2.53125, 
		-1.3375, -0.749, 2.53125, -1.3375, 0.0, 2.53125, -0.805, -1.4375, 2.53125, -1.4375, -0.805, 2.53125, -1.4375, 0.0, 2.53125, -0.84, -1.5, 2.4, -1.5, -0.84, 2.4, -1.5, 0.0, 2.4, -1.4, 0.784, 2.4, -0.784, 1.4, 2.4,
		0.0, 1.4, 2.4, -1.3375, 0.749, 2.53125, -0.749, 1.3375, 2.53125, 0.0, 1.3375, 2.53125, -1.4375, 0.805, 2.53125, -0.805, 1.4375, 2.53125, 0.0, 1.4375, 2.53125, -1.5, 0.84, 2.4, -0.84, 1.5, 2.4, 0.0, 1.5, 2.4, 
		0.784, 1.4, 2.4, 1.4, 0.784, 2.4, 0.749, 1.3375, 2.53125, 1.3375, 0.749, 2.53125, 0.805, 1.4375, 2.53125, 1.4375, 0.805, 2.53125, 0.84, 1.5, 2.4, 1.5, 0.84, 2.4, 1.75, 0.0, 1.875, 1.75, -0.98, 1.875, 
		0.98, -1.75, 1.875, 0.0, -1.75, 1.875, 2.0, 0.0, 1.35, 2.0, -1.12, 1.35, 1.12, -2.0, 1.35, 0.0, -2.0, 1.35, 2.0, 0.0, 0.9, 2.0, -1.12, 0.9, 1.12, -2.0, 0.9, 0.0, -2.0, 0.9, 
		-0.98, -1.75, 1.875, -1.75, -0.98, 1.875, -1.75, 0.0, 1.875, -1.12, -2.0, 1.35, -2.0, -1.12, 1.35, -2.0, 0.0, 1.35, -1.12, -2.0, 0.9, -2.0, -1.12, 0.9, -2.0, 0.0, 0.9, -1.75, 0.98, 1.875, 
		-0.98, 1.75, 1.875, 0.0, 1.75, 1.875, -2.0, 1.12, 1.35, -1.12, 2.0, 1.35, 0.0, 2.0, 1.35, -2.0, 1.12, 0.9, -1.12, 2.0, 0.9, 0.0, 2.0, 0.9, 0.98, 1.75, 1.875, 1.75, 0.98, 1.875, 
		1.12, 2.0, 1.35, 2.0, 1.12, 1.35, 1.12, 2.0, 0.9, 2.0, 1.12, 0.9, 2.0, 0.0, 0.45, 2.0, -1.12, 0.45, 1.12, -2.0, 0.45, 0.0, -2.0, 0.45, 1.5, 0.0, 0.225, 1.5, -0.84, 0.225, 
		0.84, -1.5, 0.225, 0.0, -1.5, 0.225, 1.5, 0.0, 0.15, 1.5, -0.84, 0.15, 0.84, -1.5, 0.15, 0.0, -1.5, 0.15, -1.12, -2.0, 0.45, -2.0, -1.12, 0.45, -2.0, 0.0, 0.45, -0.84, -1.5, 0.225, 
		-1.5, -0.84, 0.225, -1.5, 0.0, 0.225, -0.84, -1.5, 0.15, -1.5, -0.84, 0.15, -1.5, 0.0, 0.15, -2.0, 1.12, 0.45, -1.12, 2.0, 0.45, 0.0, 2.0, 0.45, -1.5, 0.84, 0.225, -0.84, 1.5, 0.225, 
		0.0, 1.5, 0.225, -1.5, 0.84, 0.15, -0.84, 1.5, 0.15, 0.0, 1.5, 0.15, 1.12, 2.0, 0.45, 2.0, 1.12, 0.45, 0.84, 1.5, 0.225, 1.5, 0.84, 0.225, 0.84, 1.5, 0.15, 1.5, 0.84, 0.15, 
		-1.6, 0.0, 2.025, -1.6, -0.3, 2.025, -1.5, -0.3, 2.25, -1.5, 0.0, 2.25, -2.3, 0.0, 2.025, -2.3, -0.3, 2.025, -2.5, -0.3, 2.25, -2.5, 0.0, 2.25, -2.7, 0.0, 2.025, -2.7, -0.3, 2.025, 
		-3.0, -0.3, 2.25, -3.0, 0.0, 2.25, -2.7, 0.0, 1.8, -2.7, -0.3, 1.8, -3.0, -0.3, 1.8, -3.0, 0.0, 1.8, -1.5, 0.3, 2.25, -1.6, 0.3, 2.025, -2.5, 0.3, 2.25, -2.3, 0.3, 2.025, 
		-3.0, 0.3, 2.25, -2.7, 0.3, 2.025, -3.0, 0.3, 1.8, -2.7, 0.3, 1.8, -2.7, 0.0, 1.575, -2.7, -0.3, 1.575, -3.0, -0.3, 1.35, -3.0, 0.0, 1.35, -2.5, 0.0, 1.125, -2.5, -0.3, 1.125, 
		-2.65, -0.3, 0.9375, -2.65, 0.0, 0.9375, -2.0, -0.3, 0.9, -1.9, -0.3, 0.6, -1.9, 0.0, 0.6, -3.0, 0.3, 1.35, -2.7, 0.3, 1.575, -2.65, 0.3, 0.9375, -2.5, 0.3, 1.125, -1.9, 0.3, 0.6, 
		-2.0, 0.3, 0.9, 1.7, 0.0, 1.425, 1.7, -0.66, 1.425, 1.7, -0.66, 0.6, 1.7, 0.0, 0.6, 2.6, 0.0, 1.425, 2.6, -0.66, 1.425, 3.1, -0.66, 0.825, 3.1, 0.0, 0.825, 2.3, 0.0, 2.1, 
		2.3, -0.25, 2.1, 2.4, -0.25, 2.025, 2.4, 0.0, 2.025, 2.7, 0.0, 2.4, 2.7, -0.25, 2.4, 3.3, -0.25, 2.4, 3.3, 0.0, 2.4, 1.7, 0.66, 0.6, 1.7, 0.66, 1.425, 3.1, 0.66, 0.825, 
		2.6, 0.66, 1.425, 2.4, 0.25, 2.025, 2.3, 0.25, 2.1, 3.3, 0.25, 2.4, 2.7, 0.25, 2.4, 2.8, 0.0, 2.475, 2.8, -0.25, 2.475, 3.525, -0.25, 2.49375, 3.525, 0.0, 2.49375, 2.9, 0.0, 2.475, 
		2.9, -0.15, 2.475, 3.45, -0.15, 2.5125, 3.45, 0.0, 2.5125, 2.8, 0.0, 2.4, 2.8, -0.15, 2.4, 3.2, -0.15, 2.4, 3.2, 0.0, 2.4, 3.525, 0.25, 2.49375, 2.8, 0.25, 2.475, 3.45, 0.15, 2.5125, 
		2.9, 0.15, 2.475, 3.2, 0.15, 2.4, 2.8, 0.15, 2.4, 0.0, 0.0, 3.15, 0.0, -0.002, 3.15, 0.002, 0.0, 3.15, 0.8, 0.0, 3.15, 0.8, -0.45, 3.15, 0.45, -0.8, 3.15, 0.0, -0.8, 3.15, 
		0.0, 0.0, 2.85, 0.2, 0.0, 2.7, 0.2, -0.112, 2.7, 0.112, -0.2, 2.7, 0.0, -0.2, 2.7, -0.002, 0.0, 3.15, -0.45, -0.8, 3.15, -0.8, -0.45, 3.15, -0.8, 0.0, 3.15, -0.112, -0.2, 2.7, 
		-0.2, -0.112, 2.7, -0.2, 0.0, 2.7, 0.0, 0.002, 3.15, -0.8, 0.45, 3.15, -0.45, 0.8, 3.15, 0.0, 0.8, 3.15, -0.2, 0.112, 2.7, -0.112, 0.2, 2.7, 0.0, 0.2, 2.7, 0.45, 0.8, 3.15,
		0.8, 0.45, 3.15, 0.112, 0.2, 2.7, 0.2, 0.112, 2.7, 0.4, 0.0, 2.55, 0.4, -0.224, 2.55, 0.224, -0.4, 2.55, 0.0, -0.4, 2.55, 1.3, 0.0, 2.55, 1.3, -0.728, 2.55, 0.728, -1.3, 2.55, 
		0.0, -1.3, 2.55, 1.3, 0.0, 2.4, 1.3, -0.728, 2.4, 0.728, -1.3, 2.4, 0.0, -1.3, 2.4, -0.224, -0.4, 2.55, -0.4, -0.224, 2.55, -0.4, 0.0, 2.55, -0.728, -1.3, 2.55, -1.3, -0.728, 2.55, 
		-1.3, 0.0, 2.55, -0.728, -1.3, 2.4, -1.3, -0.728, 2.4, -1.3, 0.0, 2.4, -0.4, 0.224, 2.55, -0.224, 0.4, 2.55, 0.0, 0.4, 2.55, -1.3, 0.728, 2.55, -0.728, 1.3, 2.55, 0.0, 1.3, 2.55, 
		-1.3, 0.728, 2.4, -0.728, 1.3, 2.4, 0.0, 1.3, 2.4, 0.224, 0.4, 2.55, 0.4, 0.224, 2.55, 0.728, 1.3, 2.55, 1.3, 0.728, 2.55, 0.728, 1.3, 2.4, 1.3, 0.728, 2.4, 0.0, 0.0, 0.0, 
		1.5, 0.0, 0.15, 1.5, 0.84, 0.15, 0.84, 1.5, 0.15, 0.0, 1.5, 0.15, 1.5, 0.0, 0.075, 1.5, 0.84, 0.075, 0.84, 1.5, 0.075, 0.0, 1.5, 0.075, 1.425, 0.0, 0.0, 1.425, 0.798, 0.0, 
		0.798, 1.425, 0.0, 0.0, 1.425, 0.0, -0.84, 1.5, 0.15, -1.5, 0.84, 0.15, -1.5, 0.0, 0.15, -0.84, 1.5, 0.075, -1.5, 0.84, 0.075, -1.5, 0.0, 0.075, -0.798, 1.425, 0.0, -1.425, 0.798, 0.0, 
		-1.425, 0.0, 0.0, -1.5, -0.84, 0.15, -0.84, -1.5, 0.15, 0.0, -1.5, 0.15, -1.5, -0.84, 0.075, -0.84, -1.5, 0.075, 0.0, -1.5, 0.075, -1.425, -0.798, 0.0, -0.798, -1.425, 0.0, 0.0, -1.425, 0.0, 
		0.84, -1.5, 0.15, 1.5, -0.84, 0.15, 0.84, -1.5, 0.075, 1.5, -0.84, 0.075, 0.798, -1.425, 0.0, 1.425, -0.798, 0.0};

		int idxs[32][16] = {
			1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16,
			4, 8, 12, 16, 17, 20, 23, 26, 18, 21, 24, 27, 19, 22, 25, 28,
			19, 22, 25, 28, 29, 32, 35, 38, 30, 33, 36, 39, 31, 34, 37, 40,
			31, 34, 37, 40, 41, 43, 45, 47, 42, 44, 46, 48, 1, 5, 9, 13,
			//13, 49, 53, 57, 14, 50, 54, 58, 15, 51, 55, 59, 16, 52, 56, 60,
			60, 56, 52, 16, 59, 55, 51, 15, 58, 54, 50, 14, 57, 53, 49, 13,
			//16, 52, 56, 60, 26, 61, 64, 67, 27, 62, 65, 68, 28, 63, 66, 69,
			69, 66, 63, 28, 68, 65, 62, 27, 67, 64, 61, 26, 60, 56, 52, 16,		
			28, 63, 66, 69, 38, 70, 73, 76, 39, 71, 74, 77, 40, 72, 75, 78,
			40, 72, 75, 78, 47, 79, 81, 83, 48, 80, 82, 84, 13, 49, 53, 57,
			57, 85, 89, 93, 58, 86, 90, 94, 59, 87, 91, 95, 60, 88, 92, 96,
			60, 88, 92, 96, 67, 97, 100, 103, 68, 98, 101, 104, 69, 99, 102, 105,
			69, 99, 102, 105, 76, 106, 109, 112, 77, 107, 110, 113, 78, 108, 111, 114,
			78, 108, 111, 114, 83, 115, 117, 119, 84, 116, 118, 120, 57, 85, 89, 93,
			121, 125, 129, 133, 122, 126, 130, 134, 123, 127, 131, 135, 124, 128, 132, 136,
			124, 128, 132, 136, 137, 139, 141, 143, 138, 140, 142, 144, 121, 125, 129, 133,
			133, 145, 149, 69, 134, 146, 150, 153, 135, 147, 151, 154, 136, 148, 152, 155,
			136, 148, 152, 155, 143, 156, 158, 160, 144, 157, 159, 161, 133, 145, 149, 69,
			162, 166, 170, 174, 163, 167, 171, 175, 164, 168, 172, 176, 165, 169, 173, 177,
			165, 169, 173, 177, 178, 180, 182, 184, 179, 181, 183, 185, 162, 166, 170, 174,
			174, 186, 190, 194, 175, 187, 191, 195, 176, 188, 192, 196, 177, 189, 193, 197,
			177, 189, 193, 197, 184, 198, 200, 202, 185, 199, 201, 203, 174, 186, 190, 194,
			204, 207, 211, 212, 204, 208, 211, 213, 204, 209, 211, 214, 204, 210, 211, 215,
			204, 210, 211, 215, 204, 217, 211, 220, 204, 218, 211, 221, 204, 219, 211, 222,
			204, 219, 211, 222, 204, 224, 211, 227, 204, 225, 211, 228, 204, 226, 211, 229,
			204, 226, 211, 229, 204, 230, 211, 232, 204, 231, 211, 233, 204, 207, 211, 212,
			212, 234, 238, 242, 213, 235, 239, 243, 214, 236, 240, 244, 215, 237, 241, 245,
			215, 237, 241, 245, 220, 246, 249, 252, 221, 247, 250, 253, 222, 248, 251, 254,
			222, 248, 251, 254, 227, 255, 258, 261, 228, 256, 259, 262, 229, 257, 260, 263,
			229, 257, 260, 263, 232, 264, 266, 268, 233, 265, 267, 269, 212, 234, 238, 242,
			270, 279, 275, 271, 270, 280, 276, 272, 270, 281, 277, 273, 270, 282, 278, 274,
			270, 282, 278, 274, 270, 289, 286, 283, 270, 290, 287, 284, 270, 291, 288, 285,
			270, 291, 288, 285, 270, 298, 295, 292, 270, 299, 296, 293, 270, 300, 297, 294,
			270, 300, 297, 294, 270, 305, 303, 301, 270, 306, 304, 302, 270, 279, 275, 271};

	double U[8] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
	double V[8] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};

	for (int i = 0; i < 32; ++i)
	{
		GVector4 P[16];
		for (int j = 0; j < 16; ++j)
		{
			int idx = idxs[i][j] - 1;
			P[j] = GVector4(pts[idx][0], pts[idx][2], -pts[idx][1], 1.0) * size;
		}

		std::string name = create_mesh_name("teapot");
		GNurbsSrf *pSrf = new GNurbsSrf(name, GTransf(), 3, 3, U, 3, 3, V, P, false);
		srfs.push_back(pSrf);
	}
}

/*!
*	\brief 넙스로 표현된 Sphere 객체를 생성함
*
*	\param radius Sphere의 반지름
*
*	\return 생성된 Sphere 객체를 반환한다.
*/
GNurbsSrf *get_gnurbs_srf_approx_sphere(double radius)
{
	GNurbsSrf *pSphere = NULL;

	return pSphere;
}

/*!
*	\brief 4 개의 코너 정점으로 Bilinear B-spline 곡면을 생성한다.
*
*	\param p0 첫 번째 정점 S(0, 0)
*	\param p1 첫 번째 정점 S(1, 0)
*	\param p2 첫 번째 정점 S(1, 1)
*	\param p3 첫 번째 정점 S(0, 1)
*
*	\return 생성된 넙스 곡면을 반환한다.
*/
GNurbsSrf *get_gnurbs_srf_linear(GPoint3 p0, GPoint3 p1, GPoint3 p2, GPoint3 p3)
{
	double U[4] = {0.0, 0.0, 1.0, 1.0};
	double V[4] = {0.0, 0.0, 1.0, 1.0};
	GVector4 P[4];
	P[0] = cast_vec4(p0, 1.0);
	P[1] = cast_vec4(p3, 1.0);
	P[2] = cast_vec4(p1, 1.0);
	P[3] = cast_vec4(p2, 1.0);

	// Bilinear 곡면을 생성하고,
	GNurbsSrf *pSrf = new GNurbsSrf("", GTransf(), 1, 1, U, 1, 1, V, P, false, "");

	// 생성된 곡면을 반환한다.
	return pSrf;
}

/*!
*	\brief 두 개의 B-spline 경계 곡선을 보간하는 선직면(ruled surface)을 생성한다.
*	\note 두 개의 곡선은 같은 방향으로 지정되어야 한다.
*
*	\param pCrv0 첫 번째 경계곡선
*	\param pCrv1 두 번째 경계곡선
*	\prame dirType 곡선의 방향이 U-방향이면 0, V-방향이면 1
*
*	\return 생성된 넙스 곡면을 반환한다.
*/
GNurbsSrf *get_gnurbs_srf_ruled(GNurbsCrv *pCrv0, GNurbsCrv *pCrv1, int dirType)
{
	// 두 곡선의 차수, 낫 벡터를 일치시킨다.
	make_compatible(pCrv0, pCrv1);

	GNurbsSrf *pSrf = NULL;

	// 곡선의 방향이 U-방향일 경우,
	if (dirType == 0)
	{
		// 생성될 곡면의 차수, 낫 벡터 그리고 제어점의 정보를 구하여,
		int p = pCrv0->p;
		int q = 1;
		int m = pCrv0->n;
		int n = 1;
		double *U = pCrv0->U;
		double V[4] = {0.0, 0.0, 1.0, 1.0};
		GVector4 **P = new GVector4 *[m + 1];
		for (int i = 0; i < m + 1; ++i)
		{
			P[i] = new GVector4 [n + 1];
			GPoint3 p0 = cast_pt3(pCrv0->P[i]);
			GPoint3 p1 = cast_pt3(pCrv1->P[i]);
			p0 = pCrv0->MC * p0;
			p1 = pCrv1->MC * p1;
			P[i][0] = cast_vec4(p0, 1.0);
			P[i][1] = cast_vec4(p1, 1.0);
		}

		// Ruled 곡면을 생성하고,
		pSrf = new GNurbsSrf("", GTransf(), p, m, U, q, n, V, P, false, "");

		// 사용된 메모리를 해제하고,
		for (int i = 0; i < m + 1; ++i)
			delete [] P[i];
		delete [] P;
	}
	else	// 곡선의 방향이 V-방향일 경우,
	{
		// 생성될 곡면의 차수, 낫 벡터 그리고 제어점의 정보를 구하여,
		int p = 1;
		int q = pCrv0->p;
		int m = 1;
		int n = pCrv0->n;
		double U[4] = {0.0, 0.0, 1.0, 1.0};
		double *V = pCrv0->U;
		
		GVector4 **P = new GVector4 *[m + 1];
		for (int i = 0; i < m + 1; ++i)
			P[i] = new GVector4 [n + 1];

		for (int i = 0; i < n + 1; ++i)
		{
			GPoint3 p0 = cast_pt3(pCrv0->P[i]);
			GPoint3 p1 = cast_pt3(pCrv1->P[i]);
			p0 = pCrv0->MC * p0;
			p1 = pCrv1->MC * p1;
			P[0][i] = cast_vec4(p0, 1.0);
			P[1][i] = cast_vec4(p1, 1.0);
		}

		// Ruled 곡면을 생성하고,
		pSrf = new GNurbsSrf("", GTransf(), p, m, U, q, n, V, P, false, "");

		// 사용된 메모리를 해제하고,
		for (int i = 0; i < m + 1; ++i)
			delete [] P[i];
		delete [] P;
	}
	
	// 생성된 곡면을 반환한다.
	return pSrf;
}

/*!
*	\brief B-spline 경계 곡선을 보간하는 곡면을 생성한다.
*	\note 4개의 경계 곡선은 반시계 방향으로 지정되어야 한다.
*
*	\param pCrv0 첫 번째 경계곡선
*	\param pCrv1 두 번째 경계곡선
*	\param pCrv2 세 번째 경계곡선
*	\param pCrv3 네 번째 경계곡선
*
*	\return 생성된 넙스 곡면을 반환한다.
*/
GNurbsSrf *get_gnurbs_srf_from_bndry_crvs(GNurbsCrv *pCrv0, GNurbsCrv *pCrv1, GNurbsCrv *pCrv2, GNurbsCrv *pCrv3)
{
	// 곡선의 U-방향을 일치시키고,
	GNurbsCrv *C0 = pCrv0->CopyObject();
	GNurbsCrv *C2 = ::get_gnurbs_crv_reverse(pCrv2);

	// 곡선의 V-방향을 일치시킨 후,
	GNurbsCrv *C1 = pCrv1->CopyObject();
	GNurbsCrv *C3 = ::get_gnurbs_crv_reverse(pCrv3);

	// 두 개의 Ruled 곡면을 생성하고,
	GNurbsSrf *pSrf0 = get_gnurbs_srf_ruled(C0, C2, 0);
	GNurbsSrf *pSrf1 = get_gnurbs_srf_ruled(C3, C1, 1);

	// Bilinear 곡면을 생성하고,
	GPoint3 p0(C0->P[0][0], C0->P[0][1], C0->P[0][2]);
	GPoint3 p1(C1->P[0][0], C1->P[0][1], C1->P[0][2]);
	GPoint3 p2(pCrv2->P[0][0], pCrv2->P[0][1], pCrv2->P[0][2]);
	GPoint3 p3(pCrv3->P[0][0], pCrv3->P[0][1], pCrv3->P[0][2]);
	GNurbsSrf *pSrf2 = get_gnurbs_srf_linear(p0, p1, p2, p3);

	// 곡면들의 차수와 낫 벡터를 일치시키고,
	make_compatible(pSrf0, pSrf1);
	make_compatible(pSrf1, pSrf2);
	make_compatible(pSrf2, pSrf0);

	// 두 곡면의 합을 구하고,
	GNurbsSrf *tmp = add(pSrf0, pSrf1);

	// 두 곡면의 차를 구한다.
	GNurbsSrf *pSrf = sub(tmp, pSrf2);

	// 임시 객체를 해제한다.
	delete C0;
	delete C1;
	delete C2;
	delete C3;
	delete pSrf0;
	delete pSrf1;
	delete pSrf2;
	delete tmp;

	// 생성된 곡면을 반환한다.
	return pSrf;
}

/*!
*	\brief 3차 삼각형 베지에 곡면을 생성하는 함수
*
*	\param pFace 삼각형 베지에 곡면의 세 정점
*	\param bFlat 평면 베지에 삼각형 여부
*
*	\return 생성된 3차 삼각형 베지에 곡면을 반환한다.
*/
GTriBzrSrf *get_tri_bzr_srf(GFace *pFace, bool bFlat)
{
	if (pFace == NULL)
		return NULL;

	// 삼각형의 세 정점을 구해서,
	std::vector<GVertex> &Verts = pFace->pMesh->Verts;
	GVector4 p0 = cast_vec4(Verts[pFace->vIdx[0]].P);
	GVector4 p1 = cast_vec4(Verts[pFace->vIdx[1]].P);
	GVector4 p2 = cast_vec4(Verts[pFace->vIdx[2]].P);

	GVector4 n0 = cast_vec4(Verts[pFace->vIdx[0]].N);
	GVector4 n1 = cast_vec4(Verts[pFace->vIdx[1]].N);
	GVector4 n2 = cast_vec4(Verts[pFace->vIdx[2]].N);

	// 베지에 곡면의 제어점을 설정하고,
	GVector4 P[10];
	P[0] = p0;
	P[6] = p1;
	P[9] = p2;

	// 평면 베지에 삼각형이라면,
	if (bFlat)
	{
		P[1] = (2 * p0 + p1) / 3.0;
		P[3] = (2 * p1 + p0) / 3.0;

		P[7] = (2 * p1 + p2) / 3.0;
		P[8] = (2 * p2 + p1) / 3.0;

		P[5] = (2 * p2 + p0) / 3.0;
		P[2] = (2 * p0 + p2) / 3.0;

		P[4] = (p0 + p1 + p2) / 3.0;
	}
	// 평면 베지에 삼각형이 아니라면,
	else
	{
		double w01 = (p1 - p0) * n0;
		double w10 = (p0 - p1) * n1;
		double w02 = (p2 - p0) * n0;
		double w20 = (p0 - p2) * n2;
		double w12 = (p2 - p1) * n1;
		double w21 = (p1 - p2) * n2;

		P[1] = (2 * p0 + p1 - w01 * n0) / 3.0;
		P[3] = (2 * p1 + p0 - w10 * n1) / 3.0;

		P[7] = (2 * p1 + p2 - w12 * n1) / 3.0;
		P[8] = (2 * p2 + p1 - w21 * n2) / 3.0;

		P[5] = (2 * p2 + p0 - w20 * n2) / 3.0;
		P[2] = (2 * p0 + p2 - w02 * n0) / 3.0;

		GVector4 E = (P[1] + P[3] + P[7] + P[8] + P[2] + P[5]) / 6.0;
		GVector4 V = (p0 + p1 + p2) / 3.0;
		P[4] = E + (E - V) / 2.0;
	}

	// 반환할 삼각형 베지에 곡면을 생성
	std::string name = create_mesh_name("tri_bzr_srf");
	GTriBzrSrf *pSrf = new GTriBzrSrf(name, GTransf(), 3, P);

	return pSrf;
}

/*!
*	\brief 삼각형 베지에 곡면을 생성하는 함수
*
*	\param pFace 삼각형 베지에 곡면의 세 정점
*	\param q0, q1, q2 삼각형에 인접한 세 삼각형의 건너편 점
*
*	\return 생성된 삼각형 베지에 곡면을 반환한다.
*/
GTriBzrSrf *get_tri_bzr_srf(GFace *pFace, GVector4 q0, GVector4 q1, GVector4 q2, bool flag0, bool flag1, bool flag2)
{
	if (pFace == NULL)
		return NULL;

	// 삼각형의 세 정점을 구해서,
	std::vector<GVertex> &Verts = pFace->pMesh->Verts;
	GVector4 p0 = cast_vec4(Verts[pFace->vIdx[0]].P);
	GVector4 p1 = cast_vec4(Verts[pFace->vIdx[1]].P);
	GVector4 p2 = cast_vec4(Verts[pFace->vIdx[2]].P);

	GVector4 n0 = cast_vec4(Verts[pFace->vIdx[0]].N);
	GVector4 n1 = cast_vec4(Verts[pFace->vIdx[1]].N);
	GVector4 n2 = cast_vec4(Verts[pFace->vIdx[2]].N);

	// 베지에 곡면의 제어점을 설정하고,
	GVector4 P[10];
	P[0] = p0;
	P[6] = p1;
	P[9] = p2;

	double w01 = (p1 - p0) * n0;
	double w10 = (p0 - p1) * n1;
	double w02 = (p2 - p0) * n0;
	double w20 = (p0 - p2) * n2;
	double w12 = (p2 - p1) * n1;
	double w21 = (p1 - p2) * n2;

	P[1] = (2 * p0 + p1 - w01 * n0) / 3.0;
	P[3] = (2 * p1 + p0 - w10 * n1) / 3.0;

	P[7] = (2 * p1 + p2 - w12 * n1) / 3.0;
	P[8] = (2 * p2 + p1 - w21 * n2) / 3.0;

	P[5] = (2 * p2 + p0 - w20 * n2) / 3.0;
	P[2] = (2 * p0 + p2 - w02 * n0) / 3.0;

	// q0만 유효한 경우,
	if (flag0 && !flag1 && !flag2)
	{
		P[4] = -(q0 + P[0] - 3 * P[1] - 3 * P[2] + 3 * P[3] + 3 * P[5] - P[6] - 3 * P[7] - 3 * P[8] - P[9]) / 6.0;
	}

	// q1만 유효한 경우,
	if (!flag0 && flag1 && !flag2)
	{
		P[4] = -(q1 - P[0] + 3 * P[1] - 3 * P[2] - 3 * P[3] - 3 * P[5] + P[6] - 3 * P[7] + 3 * P[8] - P[9]) / 6.0;
	}

	// q2만 유효한 경우,
	if (!flag0 && !flag1 && flag2)
	{
		P[4] = -(q2 - P[0] - 3 * P[1] + 3 * P[2] - 3 * P[3] - 3 * P[5] - P[6] + 3 * P[7] - 3 * P[8] + P[9]) / 6.0;
	}

	// q0, q1이 유효한 경우,
	if (flag0 && flag1 && !flag2)
	{
		P[4] = -(q0 + q1 - 6 * P[2] - 6 * P[7] - 2 * P[9]) / 12.0;
	}

	// q1, q2이 유효한 경우,
	if (!flag0 && flag1 && flag2)
	{
		P[4] = -(q1 + q2 - 2 * P[0] - 6 * P[3] - 6 * P[5]) / 12.0;
	}

	// q2, q0이 유효한 경우,
	if (flag0 && !flag1 && flag2)
	{
		P[4] = -(q2 + q0 - 6 * P[1] - 2 * P[6] - 6 * P[8]) / 12.0;
	}

	// q0, q1, q2 모두 유효한 경우,
	if (flag0 && flag1 && flag2)
	{
		GVector4 E = (P[1] + P[3] + P[7] + P[8] + P[2] + P[5]) / 6.0;
		GVector4 V = (p0 + p1 + p2) / 3.0;
		GVector4 F = (q0 + q1 + q2) / 3.0;
		P[4] = E + (V - F) / 6.0;
	}

	// 반환할 삼각형 베지에 곡면을 생성
	std::string name = create_mesh_name("tri_bzr_srf");
	GTriBzrSrf *pSrf = new GTriBzrSrf(name, GTransf(), 3, P);

	return pSrf;
}

/*!
*	\brief 두 곡선의 차수와 낫 벡터를 일치시킨다.
*	\note 차수는 높은 차수로 일치되고, 낫 벡터는 clamped 형태의 [0,...,0, ..., 1,...,1] 구간으로 변형됨
*
*	\param pCrv0 첫 번째 곡선
*	\param pCrv1 두 번째 곡선
*/
void make_compatible(GNurbsCrv *pCrv0, GNurbsCrv *pCrv1)
{
	// 두 곡선의 차수를 일치시키고,
	make_compatible_deg(pCrv0, pCrv1);

	// 두 곡선의 낫 벡터를 일치시킨다.
	make_compatible_knot(pCrv0, pCrv1);
}

/*!
*	\brief 두 곡선의 차수를 일치시킨다.
*
*	\param pCrv0 첫 번째 곡선
*	\param pCrv1 두 번째 곡선
*/
void make_compatible_deg(GNurbsCrv *pCrv0, GNurbsCrv *pCrv1)
{
	// 두 곡선의 차수를 구하여,
	int p0 = pCrv0->p;
	int p1 = pCrv1->p;

	// 적은 차수의 곡선의 차수를 높인다.
	if (p0 > p1)
	{
		for (int i = 0; i < p0 - p1; ++i)
			pCrv1->ElevateDeg();
	}
	else
	{
		for (int i = 0; i < p1 - p0; ++i)
			pCrv0->ElevateDeg();
	}
}

/*!
*	\brief 두 곡선의 낫 벡터를 일치시킨다.
*	\note 낫 벡터를 일치시키기 전에 먼저 차수를 일치시켜야 함
*
*	\param pCrv0 첫 번째 곡선
*	\param pCrv1 두 번째 곡선
*/
void make_compatible_knot(GNurbsCrv *pCrv0, GNurbsCrv *pCrv1)
{
	// 두 곡선의 낫 벡터를 clamped 형태로 일치시키고,
	pCrv0->KnotClamped();
	pCrv1->KnotClamped();

	// 낫 벡터를 [0, 1] 구간으로 일치시키고,
	int m0 = pCrv0->p + pCrv0->n + 1;
	int m1 = pCrv1->p + pCrv1->n + 1;
	transform_knot(pCrv0->U, m0, 0.0, 1.0);
	transform_knot(pCrv1->U, m1, 0.0, 1.0);

	// 두 곡선의 공통된 낫 벡터를 구하여,
	int kidx;
	double *knot = get_knot_merged_distinct(pCrv0->U, m0, pCrv1->U, m1, &kidx);

	// 각각의 낫에 대하여, 동일한 반복수를 갖도록 낫을 추가한다.
	for (int i = 1; i < kidx; ++i)
	{
		int mult0 = get_knot_mult(pCrv0->p, pCrv0->n, pCrv0->U, knot[i]);
		int mult1 = get_knot_mult(pCrv1->p, pCrv1->n, pCrv1->U, knot[i]);

		if (mult0 > mult1)
			pCrv1->KnotInsert(knot[i], mult0 - mult1);
		else
			pCrv0->KnotInsert(knot[i], mult1 - mult0);
	}

	// 사용한 동적 메모리를 해제한다.
	delete [] knot;
}

/*!
*	\brief 두 곡면의 차수와 낫 벡터를 일치시킨다.
*	\note 차수는 높은 차수로 일치되고, 낫 벡터는 clamped 형태의 [0,...,0, ..., 1,...,1] 구간으로 변형됨
*
*	\param pSrf0 첫 번째 곡면
*	\param pSrf1 두 번째 곡면
*/
void make_compatible(GNurbsSrf *pSrf0, GNurbsSrf *pSrf1)
{
	// 두 곡면의 U-방향 차수를 일치시키고,
	// 두 곡면의 V-방향 차수를 일치시킨다.
	make_compatible_deg(pSrf0, pSrf1);

	// 두 곡면의 U-방향 낫 벡터를 일치시키고,
	// 두 곡면의 V-방향 낫 벡터를 일치시킨다.
	make_compatible_knot(pSrf0, pSrf1);
}

/*!
*	\brief 두 곡면의 차수를 일치시킨다.
*
*	\param pSrf0 첫 번째 곡면
*	\param pSrf1 두 번째 곡면
*/
void make_compatible_deg(GNurbsSrf *pSrf0, GNurbsSrf *pSrf1)
{
	// 두 곡면의 U-방향 차수를 구하여,
	int p0 = pSrf0->p;
	int p1 = pSrf1->p;

	// 적은 차수의 곡면의 U-방향 차수를 높인다.
	if (p0 > p1)
	{
		for (int i = 0; i < p0 - p1; ++i)
			pSrf1->ElevateDeg(true, false);
	}
	else
	{
		for (int i = 0; i < p1 - p0; ++i)
			pSrf0->ElevateDeg(true, false);
	}

	// 두 곡면의 V-방향 차수를 구하여,
	int q0 = pSrf0->q;
	int q1 = pSrf1->q;

	// 적은 차수의 곡면의 V-방향 차수를 높인다.
	if (q0 > q1)
	{
		for (int i = 0; i < q0 - q1; ++i)
			pSrf1->ElevateDeg(false, true);
	}
	else
	{
		for (int i = 0; i < q1 - q0; ++i)
			pSrf0->ElevateDeg(false, true);
	}
}

/*!
*	\brief 두 곡면의 낫 벡터를 일치시킨다.
*	\note 낫 벡터를 일치시키기 전에 먼저 차수를 일치시켜야 함
*
*	\param pSrf0 첫 번째 곡면
*	\param pSrf1 두 번째 곡면
*/
void make_compatible_knot(GNurbsSrf *pSrf0, GNurbsSrf *pSrf1)
{
	// 두 곡면의 낫 벡터를 clamped 형태로 일치시키고,
	pSrf0->KnotClamped(true, true);
	pSrf1->KnotClamped(true, true);

	// U-방향 낫 벡터를 [0, 1] 구간으로 일치시키고,
	int r0 = pSrf0->p + pSrf0->m + 1;
	int r1 = pSrf1->p + pSrf1->m + 1;
	transform_knot(pSrf0->U, r0, 0.0, 1.0);
	transform_knot(pSrf1->U, r1, 0.0, 1.0);

 	// 두 곡선의 공통된 낫 벡터를 구하여,
 	int kidx;
 	double *knot = get_knot_merged_distinct(pSrf0->U, r0, pSrf1->U, r1, &kidx);
 
 	// 각각의 낫에 대하여, 동일한 반복수를 갖도록 낫을 추가한다.
 	for (int i = 1; i < kidx; ++i)
 	{
 		int mult0 = get_knot_mult(pSrf0->p, pSrf0->m, pSrf0->U, knot[i]);
 		int mult1 = get_knot_mult(pSrf1->p, pSrf1->m, pSrf1->U, knot[i]);
 
 		if (mult0 > mult1)
 			pSrf1->KnotInsert(0, knot[i], mult0 - mult1);
		else
			pSrf0->KnotInsert(0, knot[i], mult1 - mult0);
 	}
 
 	// 사용한 동적 메모리를 해제한다
	delete [] knot;

	// V-방향 낫 벡터를 [0, 1] 구간으로 일치시키고,
	int s0 = pSrf0->q + pSrf0->n + 1;
	int s1 = pSrf1->q + pSrf1->n + 1;
	transform_knot(pSrf0->V, s0, 0.0, 1.0);
	transform_knot(pSrf1->V, s1, 0.0, 1.0);

	// 두 곡선의 공통된 낫 벡터를 구하여,
	knot = get_knot_merged_distinct(pSrf0->V, s0, pSrf1->V, s1, &kidx);

	// 각각의 낫에 대하여, 동일한 반복수를 갖도록 낫을 추가한다.
	for (int i = 1; i < kidx; ++i)
	{
		int mult0 = get_knot_mult(pSrf0->q, pSrf0->n, pSrf0->V, knot[i]);
		int mult1 = get_knot_mult(pSrf1->q, pSrf1->n, pSrf1->V, knot[i]);

		if (mult0 > mult1)
			pSrf1->KnotInsert(1, knot[i], mult0 - mult1);
		else
			pSrf0->KnotInsert(1, knot[i], mult1 - mult0);
	}

	// 사용한 동적 메모리를 해제한다
	delete [] knot;
}

/*!
*	\brief 정점에서 곡선까지의 최단 거리를 계산한다.
*	\note 단순한 샘플링을 통하여 최단 거리를 계산한다.
*
*	\param p 입력 정점
*	\param pCrv 입력 곡선
*	\param t 최단거리가 계산되는 곡선의 파라미터가 저장된다 (out)
*
*	\return 정점과 곡선의 최단 거리를 반환한다.
*/
double get_min_dist(GPoint3 p, GNurbsCrv *pCrv, double *t)
{
	// 정의역을 구하고,
	double min_dist = 1.0e+10;
	double min_u, max_u;
	pCrv->GetDomain(&min_u, &max_u);	

	// 곡선위의 점을 샘플링하여 최단 거리를 구한다.
	int NumSamp = pCrv->Renderer.NumSampU * 10;
	for (int i = 0; i < NumSamp; ++i)
	{
		double tt = min_u + (max_u - min_u) * i / (double)(NumSamp - 1);
		GPoint3 q = cast_pt3(pCrv->Eval(tt));
		double len = dist(p, q);
		if (len < min_dist)
		{
			min_dist = len;
			*t = tt;
		}
	}

	// 계산된 거리를 반환한다.
	return min_dist;
}

/*!
*	\brief 넙스 곡선을 두 개의 부분 곡선으로 나눈다.
*
*	\param pCrv 입력 곡선
*	\param u 파라미터
*	\param pLeftCrv 생성된 왼쪽 곡선의 포인터가 저장된다.
*	\param pRightCrv 생성된 오른쪽 곡선의 포인터가 저장된다.
*/
void subdivide(GNurbsCrv *pCrv, double u, GNurbsCrv **pLeftCrv, GNurbsCrv **pRightCrv)
{
	// 정의역을 구하여,
	double umin, umax;
	pCrv->GetDomain(&umin, &umax);

	// 곡선의 파라미터가 양 끝점이라면 예외 처리를 한다.
	if (EQ(u, umin, 1.0e-6))
	{
		*pLeftCrv = NULL;
		*pRightCrv = pCrv->CopyObject();
		return;
	}
	if (EQ(u, umax, 1.0e-6))
	{
		*pLeftCrv = pCrv->CopyObject();
		*pRightCrv = NULL;
		return;
	}

	GNurbsCrv *pCopyCrv = pCrv->CopyObject();
	int mul = ::get_knot_mult(pCopyCrv->p, pCopyCrv->n, pCopyCrv->U, u);
	pCopyCrv->KnotInsert(u, pCopyCrv->p - mul);

	int kidx = ::get_knot_span_idx(pCopyCrv->p, pCopyCrv->n, pCopyCrv->U, u);
	kidx = kidx - pCopyCrv->p + 1;
	GCtrlPt *lP = new GCtrlPt [kidx];
	GCtrlPt *rP = new GCtrlPt [pCopyCrv->n - kidx + 2];
	double *lKnot = new double [kidx + pCopyCrv->p + 1];
	double *rKnot = new double [pCopyCrv->n - kidx + pCopyCrv->p + 3];

	for (int i = 0; i < kidx; ++i)
		lP[i] = pCopyCrv->P[i];
	for (int i = 0; i < pCopyCrv->n - kidx + 2; ++i)
		rP[i] = pCopyCrv->P[kidx - 1 + i];

	for (int i = 0; i < kidx + pCopyCrv->p; ++i)
		lKnot[i] = pCopyCrv->U[i];
	for (int i = 0; i < pCopyCrv->n - kidx + pCopyCrv->p + 2; ++i)
		rKnot[1 + i] = pCopyCrv->U[kidx + i];

	lKnot[kidx + pCopyCrv->p] = u;
	rKnot[0] = u;

	transform_knot(lKnot, kidx + pCopyCrv->p, 0.0, 1.0);
	transform_knot(rKnot, pCopyCrv->n - kidx + pCopyCrv->p + 2, 0.0, 1.0);

	std::string LeftCrvName = ::create_mesh_name(pCrv->Name + "_L");
	std::string RightCrvName = ::create_mesh_name(pCrv->Name + "_R");
	*pLeftCrv = new GNurbsCrv(LeftCrvName, pCrv->MC, pCopyCrv->p, kidx - 1, lKnot, lP, pCopyCrv->rational);
	*pRightCrv = new GNurbsCrv(RightCrvName, pCrv->MC, pCopyCrv->p, pCopyCrv->n - kidx + 1, rKnot, rP, pCopyCrv->rational);

	delete [] lP;
	delete [] rP;
	delete [] lKnot;
	delete [] rKnot;
	delete pCopyCrv;
}