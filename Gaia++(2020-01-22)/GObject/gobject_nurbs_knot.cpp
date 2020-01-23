#include "stdafx.h"

/*!
*	\brief 양 끝에 (p + 1)개의 multiplicity를 갖는 clamped 형태의 uniform knot 배열을 구한다.
*
*	\param p 넙스 기저의 차수
*	\param n 넙스 기저함수의 마지막 인덱스
*	\param u0 낫 벡터의 시작 값
*	\param um 낫 벡터의 끝값
*
*	\return 동적으로 생성된 낫 벡터 배열을 반환
*/
double *get_knot_uniform_clamped(int p, int n, double u0, double um)
{
	int m = p + n + 1;
	double *knot = new double [m + 1];

	for (int i = 0; i < p + 1; ++i)
		knot[i] = u0;

	for (int i = m - p; i < m + 1; ++i)
		knot[i] = um; 

	double segnum = m - 2 * p;
	for (int i = 1; i < segnum; ++i)
		knot[p + i] = u0 + (um - u0) * i / segnum;

	return knot;
}

/*!
*	\brief unclamped 형태의 uniform knot 배열을 구한다.
*
*	\param p 넙스 기저의 차수
*	\param n 넙스 기저함수의 마지막 인덱스
*	\param st 도메인의 시작값 = knot[p]
*	\param ed 도메인의 끝값 = knot[m - p]
*
*	\return 동적으로 생성된 낫 벡터 배열을 반환
*/
double *get_knot_uniform_unclamped(int p, int n, double st, double ed)
{
	int m = p + n + 1;
	double *knot = new double [m + 1];
	double *tmp = new double [m + 1];

	for (int i = 0; i < m + 1; ++i)
		tmp[i] = (double)i / (double)m;

	for (int i = 0; i < m + 1; ++i)
		knot[i] = st + (tmp[i] - tmp[p]) * (ed - st) / (tmp[m - p] - tmp[p]);

	delete [] tmp;
	return knot;
}

/*!
*	\brief clamped 형태의 averaging knot 배열을 구한다.
*
*	\param p knot 배열을 사용할 곡선의 차수
*	\param n \a params의 마지막 인덱스 (제어점의 마지막 인덱스)
*	\param params 보간파라미터 배열
*
*	\return 동적으로 생성된 낫 벡터 배열을 반환
*/
double *get_knot_averaging_clamped(const int p, const int n, double *params)
{
	int m = n + p + 1;
	double *knot = new double [m + 1];

	for (int i = 0; i < p + 1; ++i)
		knot[i] = params[0];

	for (int i = m - p; i < m + 1; ++i)
		knot[i] = params[n];

	for (int i = 1; i < n - p + 1; ++i)
	{
		double u = 0.0;
		for (int j = i; j < i + p; ++j)
			u += params[j];

		knot[p + i] = u / p;
	}

	return knot;
}

/*!
*	\brief unclamped 형태의 natural knot 배열을 구한다.
*
*	\param p knot 배열을 사용할 곡선의 차수
*	\param n \a params의 마지막 인덱스
*	\param params 보간파라미터 배열
*	\param knot 계산된 낫 벡터가 저장됨
*
*	\return 동적으로 생성된 낫 벡터 배열을 반환
*/
double *get_knot_natural_unclamped(int p, int n, double *params)
{
	double *knot = new double [n + 2 * p + 1];

	if (p % 2)		// 홀수 차수
	{
		for (int i = 0; i < n + 1; ++i)
			knot[p + i] = params[i];

		for (int i = 1; i < p + 1; ++i)
		{
			knot[p - i] = knot[p] - (knot[n + p] - knot[n + p - i]); 
			knot[n + p + i] = knot[n + p] + (knot[p + i] - knot[p]);
		}
	}
	else		// 짝수 차수
	{
		knot[p] = params[0] - (params[n] - params[n - 1]) * 0.5;
		for (int i = 0; i < n; ++i)
			knot[p + i + 1] = (params[i] + params[i + 1]) * 0.5;

		for (int i = 1; i < p + 1; ++i)
		{
			knot[p - i] = knot[p] - (knot[n + p] - knot[n + p - i]); 
			knot[n + p + i] = knot[n + p] + (knot[p + i] - knot[p]);
		}
	}

	return knot;
}

/*!
*	\brief 데이터 근사를 위한 clamped 형태의 낫 벡터를 구한다.
*
*	\param p knot 배열을 사용할 곡선의 차수
*	\param n 제어점의 마지막 인덱스
*	\param params 파라미터 배열
*	\param r 파라미터 배열의 마지막 인덱스
*
*	\return 동적으로 생성된 낫 벡터 배열을 반환
*/
double *get_knot_approx_clamped(int p, int n, double *params, int r)
{
	int m = n + p + 1;
	double *knot = new double [m + 1];

	for (int i = 0; i < p + 1; ++i)
	{
		knot[i] = 0.0;
		knot[m - i] = 1.0;
	}

	double d = (1.0 + r) / (1.0 + n - p);
	for (int j = 1; j < n - p + 1; ++j)
	{
		int i = (int)(j * d);
		double alpha = j * d - i;
		knot[p + j] = (1.0 - alpha) * params[i - 1] + alpha * params[i];
	}

	return knot;
}

/*!
*	\brief [\a u0, \a un]에서 \a (n + 1)개의 균등한 보간파라미터를 계산한다.
*
*	\param n 보간 파라미터의 마지막 인덱스
*	\param u0 보간파라미터 시작 값
*	\param un 보간파라미터의 끝 값
*
*	\return 동적으로 생성된 파라미터 배열을 반환
*/
double *get_param_uniform(int n, double u0, double un)
{
	double *params = new double [n + 1];

	for (int i = 0; i < n + 1; ++i)
		params[i] = u0 + (un - u0) * i / n;

	return params;
}

/*!
*	\brief [\a u0, \a un]에서 \a (n + 1)개의 보간정점들의 길이를 반영한 보간파라미터를 계산한다.
*
*	\param n 보간 파라미터의 마지막 인덱스
*	\param Q 보간될 정점 배열
*	\param u0 보간파라미터 시작 값
*	\param un 보간파라미터의 끝 값
*
*	\return 동적으로 생성된 파라미터 배열을 반환
*/
double *get_param_chord_len(int n, GVector4 *Q, double u0, double un)
{
	double *params = new double [n + 1];

	double d = 0.0;
	for (int i = 1; i < n + 1; ++i)
		d += dist(Q[i - 1], Q[i]);

	params[0] = u0;
	params[n] = un;
	for (int i = 1; i < n; ++i)
		params[i] = params[i - 1] + (un - u0) * dist(Q[i - 1], Q[i]) / d;

	return params;
}

/*!
*	\brief [\a u0, \a un]에서 \a (n + 1)개의 보간정점들의 길이를 반영한 보간파라미터를 계산한다.
*
*	\param n 보간 파라미터의 마지막 인덱스
*	\param Q 보간될 정점 배열
*	\param u0 보간파라미터 시작 값
*	\param un 보간파라미터의 끝 값
*
*	\return 동적으로 생성된 파라미터 배열을 반환
*/
double *get_param_centripedal(int n, GVector4 *Q, double u0, double un)
{
	double *params = new double [n + 1];

	double d = 0.0;
	for (int i = 1; i < n + 1; ++i)
		d += SQRT(dist(Q[i - 1], Q[i]));

	params[0] = u0;
	params[n] = un;
	for (int i = 1; i < n; ++i)
		params[i] = params[i - 1] + (un - u0) * SQRT(dist(Q[i - 1], Q[i])) / d;

	return params;
}

/*!
*	\brief	매개변수 \a u가 포함된 knot span의 인덱스를 반환
*	
*	\param p 넙스 기저의 차수
*	\param n 넙스 기저의 마지막 인덱스
*	\param u 매개변수
*	\param knot 낫 배열
*
*	\return knot[i] <= u < knot[i + 1]을 만족하는 인덱스 i를 반환
*/
int get_knot_span_idx(int p, int n, double *knot, double u)
{
	int m = n + p + 1;
	assert(u >= knot[p] && u <= knot[m - p]);

	int low, high, mid;
	if (u == knot[m - p]) // 정의역의 마지막특별한 처리
		return n;

	low = p;
	high = n + 1;
	mid = (low + high) / 2;
	while(u < knot[mid] || u >= knot[mid+1])
	{
		if (u < knot[mid])
			high = mid;
		else
			low = mid;
		mid = (low + high) / 2;
	}
	return mid;
}

/*!
*	\brief	knot 벡터에서 \a u의 반복수(multiplicity)를 구함
*	
*	\param p 넙스 기저의 차수
*	\param n 넙스 기저의 마지막 인덱스
*	\param u 매개변수
*	\param knot 낫 배열
*
*	\return 낫 벡터에서 \a u의 반복수(multiplicity)를 반환
*/
int get_knot_mult(int p, int n, double *knot, double u)
{
	int mult = 0, m = p + n + 1;
	for (int i = 0; i < m + 1; ++i)
	{
		if (EQ(u, knot[i], MTYPE_EPS))
			mult++;
	}
	return mult;
}

/*!
*	\brief	Uniform Cubic B-spline의 기저함수의 값을 계산
*	
*	\param t 매개변수 값
*	\param basis 기저가 저장될 배열
*/
void get_ucbs_basis(double t, double *basis)
{
	double tt = t * t;
	double ttt = tt * t;
	basis[0] = (1.0 - 3 * t + 3 * tt - ttt) / 6.0;
	basis[1] = (3 * ttt - 6 * tt + 4) / 6.0;
	basis[2] = (-3 * ttt + 3 * tt + 3 * t + 1) / 6.0;
	basis[3] = ttt / 6.0;
}

/*!
*	\brief	Uniform Cubic B-spline의 미분 기저함수의 값을 계산
*	
*	\param t 매개변수 값
*	\param deriv_basis 미분기저가 저장될 배열
*/
void get_ucbs_deriv_basis(double t, double *deriv_basis)
{
	double tt = t * t;
	double ttt = tt * t;
	deriv_basis[0] = (-1 + 2 * t - tt) / 2.0;
	deriv_basis[1] = (3 * tt - 4 * t) / 2.0;
	deriv_basis[2] = (-3 * tt + 2 * t + 1) / 2.0;
	deriv_basis[3] = tt / 2.0;
}

/*!
*	\brief	넙스 기저함수의 값을 계산
*	
*	\param knot knot 배열
*	\param p 넙스 기저의 차수
*	\param u 매개변수
*	\param idx 매개변수 \a u 가 포함된 span 인덱스, knot[idx] <= u < knot[idx+1]
*	\param basis \a (p+1)개의 넙스 기저가 저장될 배열
*
*/
void get_gnurbs_basis(double *knot, int p, double u, int idx, double *basis)
{
	basis[0] = 1.0;
	for (int i = 1; i <= p; ++i)
	{
		double saved = 0.0;
		for (int j = 0; j < i; ++j)
		{
			double tmp = basis[j] / (knot[idx + j + 1] - knot[idx + j + 1 - i]);
			basis[j] = saved + (knot[idx + j + 1] - u ) * tmp;
			saved = (u - knot[idx + 1 - i + j]) * tmp;
		}
		basis[i] = saved;
	}
}

/*!
*	\brief	넙스의 basis 함수의 미분값을 계산
*	
*	\param knot knot 배열
*	\param p 넙스 기저의 차수
*	\param u 매개변수
*	\param idx 매개변수 \a u 가 포함된 span 인덱스, knot[idx] <= u < knot[idx+1]
*	\param nth 미분차수.
*	\param deriv_basis \a 0 ~ nth 차의 미분값이 저장됨
*/
void get_gnurbs_deriv_basis(double *knot, int p, double u, int idx, int nth, double *deriv_basis)
{
	double *ndu = new double [(p + 1) * (p + 1)];
	double *left = new double [p + 1];
	double *right = new double [p + 1];
	double *a = new double [2 * (p + 1)];

	double saved, tmp, dd; 
	int j, r, k, s1, s2, j1, j2, rk, pk;
	ndu[0] = 1.0;
	for (j = 1; j < p + 1; j++)
	{
		left[j] = u - knot[idx + 1 - j];
		right[j] = knot[idx + j] - u;
		saved = 0.0;
		for (r = 0; r < j; r++)
		{
			ndu[j * (p + 1) + r] = right[r + 1] + left[j - r];
			tmp = ndu[r * (p + 1) + j - 1] / ndu[j * (p + 1) + r];
			ndu[r * (p + 1) + j] = saved + right[r + 1] * tmp;
			saved = left[j - r] * tmp;
		}
		ndu[j * (p + 1) + j] = saved;
	}

	for (j = 0; j < p + 1; j++)
		deriv_basis[j] = ndu[j * (p + 1) + p];

	for (r = 0; r < p + 1; r++)
	{
		s1 = 0; 
		s2 = 1;
		a[0] = 1.0;
		for (k = 1; k < p + 1; k++)
		{
			dd = 0.0;
			rk = r - k, pk = p - k;
			if (r >= k)
			{
				a[s2 * (p + 1)] = a[s1 * (p + 1)] / ndu[(pk + 1) * (p + 1) + rk];
				dd = a[s2 * (p + 1)] * ndu[rk * (p + 1) + pk];
			}
			if (rk >= -1)
				j1 = 1;
			else
				j1 = -rk;
			if (r - 1 <= pk)
				j2 = k - 1;
			else
				j2 = p - r;
			for (j = j1; j <= j2; j++)
			{
				a[s2 * (p + 1) + j] = (a[s1 * (p + 1) + j] - a[s1 * (p + 1) + j - 1]) / ndu[(pk + 1) * (p + 1) + rk + j];
				dd += a[s2 * (p + 1) + j] * ndu[(rk + j) * (p + 1) + pk];
			}
			if (r <= pk)
			{
				a[s2 * (p + 1) + k] = -a[s1 * (p + 1) + k - 1] / ndu[(pk + 1) * (p + 1) + r];
				dd += a[s2 * (p + 1) + k] * ndu[r * (p + 1) + pk];
			}
			deriv_basis[k * (p + 1) + r] = dd;
			j = s1; s1 = s2; s2 = j;
		}
	}
	r = p;
	for (k = 1; k < p + 1; k++)
	{
		for (j = 0; j < p + 1; j++)
		{
			deriv_basis[k * (p + 1) + j] *= r;
		}
		r *= (p - k);
	}

	delete [] ndu;
	delete [] left;
	delete [] right;
	delete [] a;
}

/*!
*	\brief	낫 벡터에 새로운 낫을 추가하고 새로운 제어점을 계산
*	
*	\param p 넙스 기저의 차수
*	\param n 제어점의 마지막 인덱스
*	\param U 이전 낫 벡터
*	\param P 이전 제어점 배열
*	\param u 추가할 새로운 낫 값
*	\param r 추가할 회수
*	\param nU 낫이 추가된 새로운 낫 벡터가 저장됨
*	\param nP 낫이 추가에 따른 새로운 제어점이 저장됨
*	\param rational 유리식 여부
*/
void knot_insert(int p, int n, double *U, GCtrlPt *P, double u, int r, double *nU, GCtrlPt *nP, bool rational)
{
	// 낫의 반복수와 인덱스를 구함
	int idx = ::get_knot_span_idx(p, n, U, u);
	int mul = ::get_knot_mult(p, n, U, u);

	// 도메인의 끝점 (knot[m - p] = u_n+1)에 대한 특별한 처리
	if (u == U[n + 1]) 
		idx++;

	int m = n + p + 1;	// 이전 낫 벡터의 마지막 인덱스
	int m1 = m + r;		// 새로운 낫 벡터의 마지막 인덱스
	int n1 = n + r;		// 새로운 제어점의 마지막 인덱스

	// 새로운 낫 벡터에 복사
	for (int i = 0; i < idx + 1; nU[i] = U[i], ++i);
	for (int i = idx + 1; i < idx + r + 1; nU[i] = u, ++i);
	for (int i = idx + r + 1; i < m1 + 1; nU[i] = U[i - r], ++i);

	// 새로운 제어점 배열을 생성하고 변하지 않는 제어점을 복사
	for (int i = 0; i < idx - p + 1; nP[i] = P[i], ++i);		
	for (int i = idx - mul; i < n + 1; nP[i + r] = P[i], ++i);		

	// 새로 계산될 제어점을 저장할 변수
	GVector4 *R = new GVector4 [p + 1];
	if (rational)
		for (int i = 0; i < p - mul + 1; R[i] = homogenize(P[idx - p + i]), ++i);
	else
		for (int i = 0; i < p - mul + 1; R[i] = P[idx - p + i], ++i);

	// 새로운 제어점을 계산
	for (int j = 1; j <= r; j++)
	{
		int l = idx - p + j;
		for (int i = 0; i <= p - j - mul; i++)
		{
			double alpha = (u - U[l + i]) / (U[i + idx + 1] - U[l + i]);
			R[i] = alpha * R[i + 1] + (1.0 - alpha) * R[i];
		}
		nP[l] = R[0];
		nP[idx + r - j - mul] = R[p - j - mul];
	}
	for (int i = idx - p + r + 1; i < idx - mul; ++i)
		nP[i] = R[i - idx + p - r];		

	if (rational)
	{
		for (int i = idx - p + 1; i < idx + r - mul; ++i)
			nP[i] = rationalize(nP[i]);
	}

	// 메모리 해제
	delete [] R;
}

/*!
*	\brief	낫 벡터에 기존의 낫을 제거하고 새로운 제어점을 계산
*	
*	\param p 넙스 기저의 차수
*	\param n 제어점의 마지막 인덱스
*	\param U 이전 낫 벡터(낫이 제거된 결과가 저장됨)
*	\param P 이전 제어점 배열(낫이 제거된 결과가 저장됨)
*	\param u 제거할 낫 값
*	\param r 제거할 회수
*	\param rational 유리식 여부
*
*	\return 실제로 낫이 제거된 회수를 반환
*/
int knot_remove(int p, int n, double *U, GCtrlPt *P, double u, int r, bool rational)
{
	// 낫의 반복수와 인덱스를 구함
	int idx = ::get_knot_span_idx(p, n, U, u);
	int mul = ::get_knot_mult(p, n, U, u);

	if (mul <= 0 || idx <= p || idx >= n + 1)
		return 0;

	int m = p + n + 1;
	int order = p + 1;
	int fout = (2 * idx - mul - p) / 2;
	int last = idx - mul;
	int first = idx - p;
	int t, i, j, ii, jj, off, remflag;
	double alfi, alfj;

	if (rational)
	{
		for (int i = 0; i < n + 1; ++i)
			P[i] = homogenize(P[i]);
	}

	GVector4 *tmp = new GVector4 [2 * p + 1];
	for (t = 0; t < r; ++t)
	{
		off = first - 1;
		tmp[0] = P[off];
		tmp[last + 1 - off] = P[last + 1];

		i = first;
		j = last;
		ii = 1;
		jj = last - off;
		remflag = 0;

		while (j - i > t)
		{
			alfi = (u - U[i]) / (U[i + order + t] - U[i]);
			alfj = (u - U[j - t]) / (U[j + order] - U[j - t]);

			tmp[ii] = (P[i] - (1.0 - alfi) * tmp[ii - 1]) / alfi;
			tmp[jj] = (P[j] - alfj * tmp[jj+1]) / (1.0 - alfj);

			i++; ii++;
			j--; jj--;
		}

		if (j - i < t)
		{
			if (tmp[ii - 1] == tmp[jj + 1])
				remflag = 1;
		}
		else
		{
			alfi = (u - U[i]) / (U[i + order + t] - U[i]);
			GVector4 X;
			X = (1.0 - alfi) * tmp[ii - 1] + alfi * tmp[ii + t + 1];
			if (P[i] == X)
				remflag = 1;
		}

		if (remflag == 0)
			break;
		else
		{
			i = first;
			j = last;
			while (j - i > t)
			{
				P[i] = tmp[i - off];
				P[j] = tmp[j - off];
				i++; j--;
			}
		}
		first--;
		last++;
	}

	if (t == 0)
	{
		delete [] tmp;
		if (rational)
		{
			for (int i = 0; i < n + 1; ++i)
				P[i] = rationalize(P[i]);
		}
		return t;
	}

	for (int k = idx + 1; k <= m; ++k)
		U[k - t] = U[k];

	j = fout; 
	i = j;
	for (int k = 1; k < t; ++k)
	{
		if (k % 2 == 1)
			i++;
		else
			j--;
	}

	for (int k = i + 1; k <= n; ++k)
	{
		P[j] = P[k];	
		j++;
	}

	delete [] tmp;

	if (rational)
	{
		for (int i = 0; i < n + 1; ++i)
			P[i] = rationalize(P[i]);
	}

	return t;
}

/*!
*	\brief	낫 벡터에 변환하는 함수
*	
*	\param U 변환될 낫 벡터
*	\param m 낫 벡터의 마지막 인덱스
*	\param u0 낫 벡터의 시작 값
*	\param um 낫 벡터의 마지막 값
*/
void transform_knot(double *U, int m, double u0, double um)
{
	double offset = u0 - U[0];
	double scale = (um - u0) / (U[m] - U[0]);
	translate_knot_with_scale(U, m, offset, scale);
}

/*!
*	\brief	낫 벡터를 이동하고 스케일 하는 함수
*	
*	\param U 변환될 낫 벡터
*	\param m 낫 벡터의 마지막 인덱스
*	\param offset 이동 변위
*	\param scale 스케일 값
*/
void translate_knot_with_scale(double *U, int m, double offset, double scale)
{
	double u0 = U[0];
	for (int i = 0 ; i < m + 1; ++i)
		U[i] = u0 + offset + (U[i] - u0) * scale;
}

/*!
*	\brief 병합된 낫 벡터를 구한다.
*	\note 병합된 낫 벡터는 중복 없이 multiplicity 1을 갖는다.
*	
*	\param U0 첫 번째 낫 벡터
*	\param m0 첫 번째 낫 벡터의 마지막 인덱스
*	\param U1 두 번째 낫 벡터
*	\param m1 두 번째 낫 벡터의 마지막 인덱스
*	\param m  병합된 낫 벡터의 마지막 인덱스가 저장된다.
*
*	\return 병합된 낫 벡터의 포인터를 반환한다.
*/
double *get_knot_merged_distinct(double *U0, int m0, double *U1, int m1, int *m)
{
	std::vector<double> tmp;

	for (int i = 0; i < m0 + 1; ++i)
		tmp.push_back(U0[i]);
	for (int i = 0; i < m1 + 1; ++i)
		tmp.push_back(U1[i]);

	sort(tmp.begin(), tmp.end());
	tmp.erase(unique(tmp.begin(), tmp.end()), tmp.end());

	*m = tmp.size() - 1;
	double *U = new double [*m + 1];
	for (int i = 0; i < *m + 1; ++i)
		U[i] = tmp[i];

	return U;
}