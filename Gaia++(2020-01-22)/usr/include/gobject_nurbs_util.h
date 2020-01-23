#ifndef _GOBJECT_NURBS_UTIL_H
#define _GOBJECT_NURBS_UTIL_H

/*! \brief 정점의 보간파라미터 타입을 나타내는 열거형 변수 */
enum TypeParam
{
	PARAM_UNIFORM = 0,
	PARAM_CHORD_LEN,
	PARAM_CENTRIPETAL,
	PARAM_USER,
};

/*! \brief 정점의 보간에 사용될 낫 벡터 타입을 나타내는 열거형 변수 */
enum TypeKnot
{
	KNOT_UNIFORM = 0,
	KNOT_AVERAGING,
	KNOT_NATURAL,
	KNOT_APPROX,
	KNOT_USER,
};

class GVector4;
class GNurbsCrv;
class GNurbsSrf;
class GUcbsSrf1D;
class GUcbsSrf;
class GUcbsVol;
class GGeoPath;
class GCoonsSrf;
class GFace;
class GTriBzrSrf;
class GMesh;

// 기본 곡선을 생성하는 함수들
GNurbsCrv *get_gnurbs_crv_closed(int _p, int _n, GVector4 *_P, bool _rational);
GNurbsCrv *get_gnurbs_crv_inter(int _p, int _n, GVector4 *Q, double u0, double un, TypeParam pType, TypeKnot kType, double *Params = NULL, double *Knots = NULL);
GNurbsCrv *get_gnurbs_crv_inter_closed(int p, int s, GVector4 *Q, TypeParam pType, TypeKnot kType);
GNurbsCrv *get_gnurbs_crv_approx(int _p, int _n, GVector4 *Q, int Qm, TypeParam pType, TypeKnot kType);
GNurbsCrv *get_gnurbs_crv_approx(int _p, int _n, std::vector<GPoint3> Pts, TypeParam pType, TypeKnot kType);
GNurbsCrv *get_gnurbs_crv_degen(int _p, int _n, double *_U, GVector4 p, bool _rational);

// 응용 곡선을 생성하는 함수들
GNurbsCrv *get_gnurbs_crv_from_geodesic_path(GGeoPath *pPath);
GNurbsCrv *get_gnurbs_crv_reverse(GNurbsCrv *pCrv);
GNurbsCrv *get_gnurbs_circle(double r, int type);
GNurbsCrv *get_gnurbs_approx_circle(double r, int type, bool ccw);
GNurbsCrv *get_gnurbs_rect(double w, double h, int type);
GNurbsCrv *get_gnurbs_ellipse(double a, double b, int type);
GNurbsCrv *get_gnurbs_crv_inter(GVector3 p1, GVector3 n1, GVector3 p2, GVector3 n2);

// 기본 곡면을 생성하는 함수들
GNurbsSrf *get_gnurbs_srf_closed(int p, int m, int q, int n, GVector4 *P, bool closed_u, bool closed_v);
GNurbsSrf *get_gnurbs_srf_inter(int q, std::vector<GNurbsCrv *> CrvList, double *Params, double *KnotV);
GNurbsSrf *get_gnurbs_srf_approx_ver1(int p, int m, double *U, int q, int n, double *V, int num, double *params, GVector4 *pts, int num_level);
GNurbsSrf *get_gnurbs_srf_approx_ver2(int p, int m, double *U, int q, int n, double *V, int num, double *params, GVector4 *pts, int num_level);
GNurbsSrf *get_gnurbs_srf_degen(int p, int m, double *U, int q, int n, double *V, GVector4 p0, bool rational);
GNurbsSrf *get_gnurbs_plane(int p, int q, GPoint3 p0, GPoint3 p1, GPoint3 p2);
GUcbsSrf1D *get_gucbs_srf_approx(int m, int n, int num_data, double *params, double *pts, int num_level);
GUcbsSrf1D *get_gucbs_srf_approx_hiera(int m, int n, int num_data, double *params, double *pts, int num_level, std::vector<GUcbsSrf1D *> &srfs);
GUcbsSrf *get_gucbs_srf_approx(int m, int n, int num_data, double *params, GVector4 *pts, int num_level);
GUcbsSrf *get_gucbs_srf_approx_ver2(int m, int n, int num_data, double *params, GVector4 *pts, int num_level);
GUcbsSrf *get_gucbs_srf_approx_ver3(int m, int n, int num_data, double *params, GVector4 *pts, int num_level, GPoint3 p, GPoint3 q);
GUcbsSrf *get_gucbs_srf_approx_ver4(int m, int n, int num_data, double *params, GVector4 *pts, int num_level);
GUcbsSrf *get_gucbs_srf_approx_hiera(int m, int n, int num_data, double *params, GVector4 *pts, int num_level, std::vector<GUcbsSrf *> &srfs);
GUcbsSrf *get_gucbs_srf_overlayed(std::vector<GUcbsSrf *> &srfs, int sidx, int eidx);
GUcbsSrf *get_gucbs_srf_tan_frm(GUcbsSrf *pBaseSrf, GUcbsSrf *pTarSrf);
GUcbsSrf *get_gucbs_srf_fixed_frm(GUcbsSrf *pBaseSrf, GUcbsSrf *pTarSrf);
GUcbsVol *get_gucbs_vol_approx(int l, int m, int n, int num_data, double *params, GVector4 *pts, int num_level);
GTriBzrSrf *get_tri_bzr_srf(GFace *pFace, bool bFlat);
GTriBzrSrf *get_tri_bzr_srf(GFace *pFace, GVector4 q0, GVector4 q1, GVector4 q2, bool flag0, bool flag1, bool flag2);
GCoonsSrf *get_coons_srf_from_geodesic_paths(std::vector<GGeoPath *> BndryPaths);

// 응용 곡면을 생성하는 함수들
void get_gnurbs_srf_teapot(std::vector<GNurbsSrf *> &srfs, double size);
GNurbsSrf *get_gnurbs_srf_approx_sphere(double radius);
GNurbsSrf *get_gnurbs_srf_linear(GPoint3 p0, GPoint3 p1, GPoint3 p2, GPoint3 p3);
GNurbsSrf *get_gnurbs_srf_ruled(GNurbsCrv *pCrv0, GNurbsCrv *pCrv1, int dirType);
GNurbsSrf *get_gnurbs_srf_from_bndry_crvs(GNurbsCrv *pCrv0, GNurbsCrv *pCrv1, GNurbsCrv *pCrv2, GNurbsCrv *pCrv3);

// 곡선/곡면의 심볼릭 연산을 위한 함수들
void make_compatible(GNurbsCrv *pCrv0, GNurbsCrv *pCrv1);
void make_compatible_knot(GNurbsCrv *pCrv0, GNurbsCrv *pCrv1);
void make_compatible_deg(GNurbsCrv *pCrv0, GNurbsCrv *pCrv1);
void make_compatible(GNurbsSrf *pSrf0, GNurbsSrf *pSrf1);
void make_compatible_knot(GNurbsSrf *pSrf0, GNurbsSrf *pSrf1);
void make_compatible_deg(GNurbsSrf *pSrf0, GNurbsSrf *pSrf1);

// 곡선/곡면의 처리를 위한 응용 함수들
double get_crv_len(GNurbsCrv *pCrv, double u0, double u1, int snum);
void get_frenet_frame(GNurbsCrv *pCrv, double u, GVector3 &T, GVector3 &N, GVector3 &B);
void get_rotation_minimizing_frames(GNurbsCrv *pCrv, int snum, std::vector<GTransf> &FrameList);
double TriCoeff(int n, int i, int j, int k);

// 기하 객체간의 최단 거리를 계산하는 함수들
double get_min_dist(GPoint3 p, GNurbsCrv *pCrv, double *t);

// 곡선은 두개의 부분 곡선으로 나눈는 함수
void subdivide(GNurbsCrv *pCrv, double u, GNurbsCrv **pLeftCrv, GNurbsCrv **pRightCrv);

#endif // _GOBJECT_NURBS_UTIL_H
