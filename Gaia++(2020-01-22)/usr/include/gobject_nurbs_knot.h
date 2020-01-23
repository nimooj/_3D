#ifndef _GOBJECT_NURBS_KNOT_H
#define _GOBJECT_NURBS_KNOT_H

class GCtrlPt;

// �� ���͸� �������� �����Ͽ� ��ȯ�ϴ� �Լ���
double *get_knot_uniform_clamped(int p, int n, double u0, double um);
double *get_knot_uniform_unclamped(int p, int n, double st, double ed);
double *get_knot_averaging_clamped(int p, int n, double *params);
double *get_knot_natural_unclamped(int p, int n, double *params);
double *get_knot_approx_clamped(int p, int n, double *params, int s);

// ������ ������ ���� �Ķ���͸� ���ϴ� �Լ���
double *get_param_uniform(int n, double u0, double un);
double *get_param_chord_len(int n, GVector4 *Q, double u0, double un);
double *get_param_centripedal(int n, GVector4 *Q, double u0, double un);

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
void knot_insert(int p, int n, double *U, GCtrlPt *P, double u, int r, double *U1, GCtrlPt *P1, bool rational);
int	 knot_remove(int p, int n, double *U, GCtrlPt *P, double u, int r, bool rational);

// �� ���͸� ��ȯ�ϴ� �Լ���
void transform_knot(double *U, int m, double u0, double um);
void translate_knot_with_scale(double *U, int m, double offset, double scale);
double *get_knot_merged_distinct(double *U0, int m0, double *U1, int m1, int *m);

#endif // _GOBJECT_NURBS_KNOT_H