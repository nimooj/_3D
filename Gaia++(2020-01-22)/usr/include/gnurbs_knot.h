#ifndef _GNURBS_KNOT_H
#define _GNURBS_KNOT_H

// �� ���͸� �������� �����Ͽ� ��ȯ�ϴ� �Լ���
double *get_knot_uniform_clamped(int p, int n, double u0, double um);
double *get_knot_uniform_unclamped(int p, int n, double st, double ed);
double *get_knot_averaging_clamped(int p, int n, double *params);
double *get_knot_natural_unclamped(int p, int n, double *params);

// �־��� �Ķ������ �ݺ����� ������ ���ϴ� �Լ���
int get_knot_span_idx(int p, int n, double *knot, double u);
int get_knot_mult(int p, int n, double *knot, double u);

// Uniform Cubic B-spline�� ���� �Լ��� ������ ���ϴ� �Լ���
void get_ucbs_basis(double t, double *basis);
void get_ucbs_deriv_basis(double t, double *deriv_basis);

// �ҽ� ���� �Լ��� ������ ���ϴ� �Լ���
void get_gnurbs_basis(double *knot, int p, double u, int idx, double *basis);
void get_gnurbs_deriv_basis(double *knot, int p, double u, int idx, int nth, double *deriv_basis);

// �� �߰��� ���Ÿ� �����ϴ� �Լ���
void knot_insert(int p, int n, double *U, GVector4 *P, double u, int r, double *U1, GVector4 *P1, bool rational);
int	 knot_remove(int p, int n, double *U, GVector4 *P, double u, int r, bool rational);

#endif // _GNURBS_KNOT_H
