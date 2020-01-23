#ifndef _GNURBS_UTIL_H
#define _GNURBS_UTIL_H

/*! \brief ������ �����Ķ���� Ÿ���� ��Ÿ���� ������ ���� */
enum TypeParam
{
	PARAM_UNIFORM = 0,
	PARAM_CHORD_LEN,
	PARAM_CENTRIPETAL,
	PARAM_USER,
};

/*! \brief ������ ������ ���� �� ���� Ÿ���� ��Ÿ���� ������ ���� */
enum TypeKnot
{
	KNOT_UNIFORM = 0,
	KNOT_AVERAGING,
	KNOT_NATURAL,
	KNOT_USER,
};

class GVector4;
class GNurbsCrv;
class GNurbsSrf;
class GUcbsSrf1D;
class GUcbsSrf;
class GUcbsVol;

// ��� �����ϴ� �Լ���
GNurbsCrv *get_gnurbs_crv_closed(int _p, int _n, GVector4 *_P, bool _rational);
GNurbsCrv *get_gnurbs_crv_inter(int _p, int _n, GVector4 *Q, double u0, double un, TypeParam pType, TypeKnot kType, double *Params = NULL, double *Knots = NULL);
GNurbsCrv *get_gnurbs_crv_inter_closed(int p, int s, GVector4 *Q, TypeParam pType, TypeKnot kType);
GNurbsCrv *get_gnurbs_crv_degen(int _p, int _n, double *_U, GVector4 p, bool _rational);

// �׽�Ʈ ��....
GNurbsCrv *get_gnurbs_crv_inter(GVector3 p1, GVector3 n1, GVector3 p2, GVector3 n2);

// �⺻ ����� �����ϴ� �Լ���
void get_gnurbs_srf_teapot(vector<GNurbsSrf *> &srfs);

// 2�� ��� ��� �����ϴ� �Լ���
GNurbsCrv *get_gnurbs_circle_xy(double r);
GNurbsCrv *get_gnurbs_circle_yz(double r);
GNurbsCrv *get_gnurbs_circle_zx(double r);
GNurbsCrv *get_gnurbs_ellipse_xy(double a, double b);
GNurbsCrv *get_gnurbs_ellipse_yz(double a, double b);
GNurbsCrv *get_gnurbs_ellipse_zx(double a, double b);

// ����� �����ϴ� �Լ���
GNurbsSrf *get_gnurbs_srf_closed(int p, int m, int q, int n, GVector4 *P, bool closed_u, bool closed_v);
GNurbsSrf *get_gnurbs_srf_approx_ver1(int p, int m, double *U, int q, int n, double *V, int num, double *params, GVector4 *pts, int num_level);
GNurbsSrf *get_gnurbs_srf_approx_ver2(int p, int m, double *U, int q, int n, double *V, int num, double *params, GVector4 *pts, int num_level);
GNurbsSrf *get_gnurbs_srf_degen(int p, int m, double *U, int q, int n, double *V, GVector4 p0, bool rational);
GNurbsSrf *get_gnurbs_plane(int p, int q, GPoint3 p0, GPoint3 p1, GPoint3 p2);
GUcbsSrf1D *get_gucbs_srf_approx(int m, int n, int num_data, double *params, double *pts, int num_level);
GUcbsSrf1D *get_gucbs_srf_approx_hiera(int m, int n, int num_data, double *params, double *pts, int num_level, vector<GUcbsSrf1D *> &srfs);
GUcbsSrf *get_gucbs_srf_approx(int m, int n, int num_data, double *params, GVector4 *pts, int num_level);
GUcbsSrf *get_gucbs_srf_approx_ver2(int m, int n, int num_data, double *params, GVector4 *pts, int num_level);
GUcbsSrf *get_gucbs_srf_approx_ver3(int m, int n, int num_data, double *params, GVector4 *pts, int num_level, GPoint3 p, GPoint3 q);
GUcbsSrf *get_gucbs_srf_approx_ver4(int m, int n, int num_data, double *params, GVector4 *pts, int num_level);
GUcbsSrf *get_gucbs_srf_approx_hiera(int m, int n, int num_data, double *params, GVector4 *pts, int num_level, vector<GUcbsSrf *> &srfs);
GUcbsSrf *get_gucbs_srf_overlayed(vector<GUcbsSrf *> &srfs, int sidx, int eidx);
GUcbsSrf *get_gucbs_srf_tan_frm(GUcbsSrf *pBaseSrf, GUcbsSrf *pTarSrf);
GUcbsSrf *get_gucbs_srf_fixed_frm(GUcbsSrf *pBaseSrf, GUcbsSrf *pTarSrf);
GUcbsVol *get_gucbs_vol_approx(int l, int m, int n, int num_data, double *params, GVector4 *pts, int num_level);

// ������ ������ ���� �Ķ���� �迭�� �������� �����ϴ� �Լ���
double *get_param_uniform(int n, double u0, double un);
double *get_param_chord_len(int n, GVector4 *Q, double u0, double un);
double *get_param_centripedal(int n, GVector4 *Q, double u0, double un);

// �ҽ� ��� ���̸� ����ϴ� �Լ�
double get_crv_len(GNurbsCrv *pCrv, double u0, double u1, int snum);
void get_frenet_frame(GNurbsCrv *pCrv, double u, GVector3 &T, GVector3 &N, GVector3 &B);

// ���װ���� ����ϴ� �Լ�
double TriCoeff(int n, int i, int j, int k);


#endif // _GNURBS_UTIL_H
