#include "stdafx.h"

/*!
*	\brief �� ���� (p + 1)���� multiplicity�� ���� clamped ������ uniform knot �迭�� ���Ѵ�.
*
*	\param p �ҽ� ������ ����
*	\param n �ҽ� �����Լ��� ������ �ε���
*	\param u0 �� ������ ���� ��
*	\param um �� ������ ����
*
*	\return �������� ������ �� ���� �迭�� ��ȯ
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
*	\brief unclamped ������ uniform knot �迭�� ���Ѵ�.
*
*	\param p �ҽ� ������ ����
*	\param n �ҽ� �����Լ��� ������ �ε���
*	\param st �������� ���۰� = knot[p]
*	\param ed �������� ���� = knot[m - p]
*
*	\return �������� ������ �� ���� �迭�� ��ȯ
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
*	\brief clamped ������ averaging knot �迭�� ���Ѵ�.
*
*	\param p knot �迭�� ����� ��� ����
*	\param n \a params�� ������ �ε��� (�������� ������ �ε���)
*	\param params �����Ķ���� �迭
*
*	\return �������� ������ �� ���� �迭�� ��ȯ
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
*	\brief unclamped ������ natural knot �迭�� ���Ѵ�.
*
*	\param p knot �迭�� ����� ��� ����
*	\param n \a params�� ������ �ε���
*	\param params �����Ķ���� �迭
*	\param knot ���� �� ���Ͱ� �����
*
*	\return �������� ������ �� ���� �迭�� ��ȯ
*/
double *get_knot_natural_unclamped(int p, int n, double *params)
{
	double *knot = new double [n + 2 * p + 1];

	if (p % 2)		// Ȧ�� ����
	{
		for (int i = 0; i < n + 1; ++i)
			knot[p + i] = params[i];

		for (int i = 1; i < p + 1; ++i)
		{
			knot[p - i] = knot[p] - (knot[n + p] - knot[n + p - i]); 
			knot[n + p + i] = knot[n + p] + (knot[p + i] - knot[p]);
		}
	}
	else		// ¦�� ����
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
*	\brief ������ �ٻ縦 ���� clamped ������ �� ���͸� ���Ѵ�.
*
*	\param p knot �迭�� ����� ��� ����
*	\param n �������� ������ �ε���
*	\param params �Ķ���� �迭
*	\param r �Ķ���� �迭�� ������ �ε���
*
*	\return �������� ������ �� ���� �迭�� ��ȯ
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
*	\brief [\a u0, \a un]���� \a (n + 1)���� �յ��� �����Ķ���͸� ����Ѵ�.
*
*	\param n ���� �Ķ������ ������ �ε���
*	\param u0 �����Ķ���� ���� ��
*	\param un �����Ķ������ �� ��
*
*	\return �������� ������ �Ķ���� �迭�� ��ȯ
*/
double *get_param_uniform(int n, double u0, double un)
{
	double *params = new double [n + 1];

	for (int i = 0; i < n + 1; ++i)
		params[i] = u0 + (un - u0) * i / n;

	return params;
}

/*!
*	\brief [\a u0, \a un]���� \a (n + 1)���� ������������ ���̸� �ݿ��� �����Ķ���͸� ����Ѵ�.
*
*	\param n ���� �Ķ������ ������ �ε���
*	\param Q ������ ���� �迭
*	\param u0 �����Ķ���� ���� ��
*	\param un �����Ķ������ �� ��
*
*	\return �������� ������ �Ķ���� �迭�� ��ȯ
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
*	\brief [\a u0, \a un]���� \a (n + 1)���� ������������ ���̸� �ݿ��� �����Ķ���͸� ����Ѵ�.
*
*	\param n ���� �Ķ������ ������ �ε���
*	\param Q ������ ���� �迭
*	\param u0 �����Ķ���� ���� ��
*	\param un �����Ķ������ �� ��
*
*	\return �������� ������ �Ķ���� �迭�� ��ȯ
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
*	\brief	�Ű����� \a u�� ���Ե� knot span�� �ε����� ��ȯ
*	
*	\param p �ҽ� ������ ����
*	\param n �ҽ� ������ ������ �ε���
*	\param u �Ű�����
*	\param knot �� �迭
*
*	\return knot[i] <= u < knot[i + 1]�� �����ϴ� �ε��� i�� ��ȯ
*/
int get_knot_span_idx(int p, int n, double *knot, double u)
{
	int m = n + p + 1;
	assert(u >= knot[p] && u <= knot[m - p]);

	int low, high, mid;
	if (u == knot[m - p]) // ���ǿ��� ������Ư���� ó��
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
*	\brief	knot ���Ϳ��� \a u�� �ݺ���(multiplicity)�� ����
*	
*	\param p �ҽ� ������ ����
*	\param n �ҽ� ������ ������ �ε���
*	\param u �Ű�����
*	\param knot �� �迭
*
*	\return �� ���Ϳ��� \a u�� �ݺ���(multiplicity)�� ��ȯ
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
*	\brief	Uniform Cubic B-spline�� �����Լ��� ���� ���
*	
*	\param t �Ű����� ��
*	\param basis ������ ����� �迭
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
*	\brief	Uniform Cubic B-spline�� �̺� �����Լ��� ���� ���
*	
*	\param t �Ű����� ��
*	\param deriv_basis �̺б����� ����� �迭
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
*	\brief	�ҽ� �����Լ��� ���� ���
*	
*	\param knot knot �迭
*	\param p �ҽ� ������ ����
*	\param u �Ű�����
*	\param idx �Ű����� \a u �� ���Ե� span �ε���, knot[idx] <= u < knot[idx+1]
*	\param basis \a (p+1)���� �ҽ� ������ ����� �迭
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
*	\brief	�ҽ��� basis �Լ��� �̺а��� ���
*	
*	\param knot knot �迭
*	\param p �ҽ� ������ ����
*	\param u �Ű�����
*	\param idx �Ű����� \a u �� ���Ե� span �ε���, knot[idx] <= u < knot[idx+1]
*	\param nth �̺�����.
*	\param deriv_basis \a 0 ~ nth ���� �̺а��� �����
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
*	\brief	�� ���Ϳ� ���ο� ���� �߰��ϰ� ���ο� �������� ���
*	
*	\param p �ҽ� ������ ����
*	\param n �������� ������ �ε���
*	\param U ���� �� ����
*	\param P ���� ������ �迭
*	\param u �߰��� ���ο� �� ��
*	\param r �߰��� ȸ��
*	\param nU ���� �߰��� ���ο� �� ���Ͱ� �����
*	\param nP ���� �߰��� ���� ���ο� �������� �����
*	\param rational ������ ����
*/
void knot_insert(int p, int n, double *U, GCtrlPt *P, double u, int r, double *nU, GCtrlPt *nP, bool rational)
{
	// ���� �ݺ����� �ε����� ����
	int idx = ::get_knot_span_idx(p, n, U, u);
	int mul = ::get_knot_mult(p, n, U, u);

	// �������� ���� (knot[m - p] = u_n+1)�� ���� Ư���� ó��
	if (u == U[n + 1]) 
		idx++;

	int m = n + p + 1;	// ���� �� ������ ������ �ε���
	int m1 = m + r;		// ���ο� �� ������ ������ �ε���
	int n1 = n + r;		// ���ο� �������� ������ �ε���

	// ���ο� �� ���Ϳ� ����
	for (int i = 0; i < idx + 1; nU[i] = U[i], ++i);
	for (int i = idx + 1; i < idx + r + 1; nU[i] = u, ++i);
	for (int i = idx + r + 1; i < m1 + 1; nU[i] = U[i - r], ++i);

	// ���ο� ������ �迭�� �����ϰ� ������ �ʴ� �������� ����
	for (int i = 0; i < idx - p + 1; nP[i] = P[i], ++i);		
	for (int i = idx - mul; i < n + 1; nP[i + r] = P[i], ++i);		

	// ���� ���� �������� ������ ����
	GVector4 *R = new GVector4 [p + 1];
	if (rational)
		for (int i = 0; i < p - mul + 1; R[i] = homogenize(P[idx - p + i]), ++i);
	else
		for (int i = 0; i < p - mul + 1; R[i] = P[idx - p + i], ++i);

	// ���ο� �������� ���
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

	// �޸� ����
	delete [] R;
}

/*!
*	\brief	�� ���Ϳ� ������ ���� �����ϰ� ���ο� �������� ���
*	
*	\param p �ҽ� ������ ����
*	\param n �������� ������ �ε���
*	\param U ���� �� ����(���� ���ŵ� ����� �����)
*	\param P ���� ������ �迭(���� ���ŵ� ����� �����)
*	\param u ������ �� ��
*	\param r ������ ȸ��
*	\param rational ������ ����
*
*	\return ������ ���� ���ŵ� ȸ���� ��ȯ
*/
int knot_remove(int p, int n, double *U, GCtrlPt *P, double u, int r, bool rational)
{
	// ���� �ݺ����� �ε����� ����
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
*	\brief	�� ���Ϳ� ��ȯ�ϴ� �Լ�
*	
*	\param U ��ȯ�� �� ����
*	\param m �� ������ ������ �ε���
*	\param u0 �� ������ ���� ��
*	\param um �� ������ ������ ��
*/
void transform_knot(double *U, int m, double u0, double um)
{
	double offset = u0 - U[0];
	double scale = (um - u0) / (U[m] - U[0]);
	translate_knot_with_scale(U, m, offset, scale);
}

/*!
*	\brief	�� ���͸� �̵��ϰ� ������ �ϴ� �Լ�
*	
*	\param U ��ȯ�� �� ����
*	\param m �� ������ ������ �ε���
*	\param offset �̵� ����
*	\param scale ������ ��
*/
void translate_knot_with_scale(double *U, int m, double offset, double scale)
{
	double u0 = U[0];
	for (int i = 0 ; i < m + 1; ++i)
		U[i] = u0 + offset + (U[i] - u0) * scale;
}

/*!
*	\brief ���յ� �� ���͸� ���Ѵ�.
*	\note ���յ� �� ���ʹ� �ߺ� ���� multiplicity 1�� ���´�.
*	
*	\param U0 ù ��° �� ����
*	\param m0 ù ��° �� ������ ������ �ε���
*	\param U1 �� ��° �� ����
*	\param m1 �� ��° �� ������ ������ �ε���
*	\param m  ���յ� �� ������ ������ �ε����� ����ȴ�.
*
*	\return ���յ� �� ������ �����͸� ��ȯ�Ѵ�.
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