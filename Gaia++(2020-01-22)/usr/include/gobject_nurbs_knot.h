#ifndef _GOBJECT_NURBS_KNOT_H
#define _GOBJECT_NURBS_KNOT_H

class GCtrlPt;

// 낫 벡터를 동적으로 생성하여 반환하는 함수들
double *get_knot_uniform_clamped(int p, int n, double u0, double um);
double *get_knot_uniform_unclamped(int p, int n, double st, double ed);
double *get_knot_averaging_clamped(int p, int n, double *params);
double *get_knot_natural_unclamped(int p, int n, double *params);
double *get_knot_approx_clamped(int p, int n, double *params, int s);

// 데이터 보간을 위한 파라미터를 구하는 함수들
double *get_param_uniform(int n, double u0, double un);
double *get_param_chord_len(int n, GVector4 *Q, double u0, double un);
double *get_param_centripedal(int n, GVector4 *Q, double u0, double un);

// 주어진 파라미터의 반복수와 구간을 구하는 함수들
int get_knot_span_idx(int p, int n, double *knot, double u);
int get_knot_mult(int p, int n, double *knot, double u);

// Uniform Cubic B-spline의 기저 함수의 값들을 구하는 함수들
void get_ucbs_basis(double t, double *basis);
void get_ucbs_deriv_basis(double t, double *deriv_basis);

// 넙스 기저 함수의 값들을 구하는 함수들
void get_gnurbs_basis(double *knot, int p, double u, int idx, double *basis);
void get_gnurbs_deriv_basis(double *knot, int p, double u, int idx, int nth, double *deriv_basis);

// 낫 추가와 제거를 수행하는 함수들
void knot_insert(int p, int n, double *U, GCtrlPt *P, double u, int r, double *U1, GCtrlPt *P1, bool rational);
int	 knot_remove(int p, int n, double *U, GCtrlPt *P, double u, int r, bool rational);

// 낫 벡터를 변환하는 함수들
void transform_knot(double *U, int m, double u0, double um);
void translate_knot_with_scale(double *U, int m, double offset, double scale);
double *get_knot_merged_distinct(double *U0, int m0, double *U1, int m1, int *m);

#endif // _GOBJECT_NURBS_KNOT_H