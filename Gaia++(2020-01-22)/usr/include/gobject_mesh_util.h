#ifndef _GOBJECT_MESH_UTIL_H
#define _GOBJECT_MESH_UTIL_H

class GMesh;
class GVertex;
class GEditObj;
class GSweepSrf;
class GMaterial;
class PQP_Model;

/*! 새롭게 생성되는 객체의 이름을 생성하는 함수 */
std::string create_mesh_name(std::string TypeObj);

/*! 새롭게 생성되는 재질의 이름을 생성하는 함수 */
std::string create_mtl_name(std::string Prefix);

/*! Cylinder 메쉬를 생성하는 함수 */
GMesh *create_mesh_from_cylinder(GNurbsCrv *pCrv);

/*! Sphere 메쉬를 생성하는 함수 */
GMesh *create_mesh_from_sphere(std::string Name, GTransf MC, double radius, int slices, int stacks);

/*! Sphere Icosahedron 메쉬를 생성하는 함수 */
GMesh *create_mesh_from_icosahedron(std::string Name, GTransf MC, double radius, int subdiv_depth);

/*! Teapot 메쉬를 생성하는 함수 */
GMesh *create_mesh_from_teapot(double size);

/*! Box 메쉬를 생성하는 함수 */
GMesh *create_mesh_from_box(GPoint3 p, GPoint3 q);

/*! 메쉬의 부분 메쉬를 생성하는 함수 */
GMesh *create_mesh_from_faces(std::string Name, GMesh *pParentMesh, std::vector<GFace *> &SltFaceList, std::map<int, int> &VertIdxMap);

/*! 2차원 공간에 매개화된 메쉬를 생성하는 함수 */
GMesh *create_mesh_unfolded(std::string Name, GMesh *pMesh, int StartVertIdx = -1, int BndryType = 0);

/*! 경계 정점으로 메쉬를 생성하는 함수 */
GMesh *create_mesh_from_bndry_verts(std::string Name, std::vector<GVertex> &BndryVerts, int FillType);

/*! 스윕 곡면으로부터 부분 메쉬를 생성하는 함수 */
GMesh *create_mesh_from_sweep_srf(GSweepSrf *pSrf, double ContParam, double AuxParam, int snum_u, int snum_v);

/*! 두 메쉬의 충돌 감지를 수행하는 함수 */
int collision_check(GMesh *pMesh1, GMesh *pMesh2);

/*! 부분 메쉬의 BVH와 메쉬와의 충돌 감지를 수행하는 함수 */
int collision_check(GTransf &MC1, PQP_Model *pBVH1, GMesh *pMesh2);

/*! 칼라맵에 해당하는 색상을 구하는 함수 */
GVector3 get_color_gradient_from_rainbow(double t);

/*! 칼라맵에 해당하는 색상을 구하는 함수 */
GVector3 get_color_gradient(double t, GVector3 C1, GVector3 C2, GVector3 C3);

/*! 임의의 재질을 생성하여 반환하는 함수 */
GMaterial get_material_random();

/*! 사용자 정의 재질을 생성하는 함수 */
GMaterial get_material(double diff_r, double diff_g, double diff_b, 
					   double ambi_r = 0.1, double ambi_g = 0.1, double ambi_b = 0.1, 
					   double spec_r = 0.7, double spec_g = 0.7, double spec_b = 0.7);

/*! 메쉬의 가우스 곡률(kG), 평균 곡률(kH), 최소 곡률(kMin) 그리고 최대 곡률(kMax)를 계산하는 함수 */
void get_curvature(GMesh *pMesh, std::vector<double> &kG, std::vector<double> &kH, std::vector<double> &kMin, std::vector<double> &kMax);

/*! 메쉬의 각 정점의 Gaussian 곡률의 계산하여 반환하는 함수 */
void get_gaussian_curvature(GMesh *pMesh, std::vector<double> &Curvatures, double &MinK, double &MaxK);

/*! 메쉬의 각 정점의 Mean 곡률의 계산하여 반환하는 함수 */
void get_mean_curvature(GMesh *pMesh, std::vector<double> &Curvatures, double &MinK, double &MaxK);

/*! \brief 객체의 표면적을 계산하는 함수 */
double get_mesh_area(GMesh *pMesh);

/*! \brief 메쉬에 Laplacian 평탄화 연산을 적용하는 함수 */
void edit_mesh_by_smoothing(GMesh *pMesh);

/*! \brief 메쉬에 세분화 연산을 적용하는 함수 */
void edit_mesh_by_refine(GMesh *pMesh);

/*! \brief 메쉬의 중복된 정점과 내부 경계를 꿰메는 함수 */
void edit_mesh_by_stitching(GMesh *pMesh, double tol = 0.001);

/*! \brief 메쉬에서 사용되지 않는 정점을 제거하는 함수 */
void edit_mesh_by_removing_unused_verts(GMesh *pMesh);

/*! \brief 메쉬의 정점 색상을 법선 벡터로 설정하는 함수 */
void set_vertex_color_from_normal(GMesh *pTarMesh, GMesh *pSrcMesh = NULL);

/*! \brief 메쉬의 정점 색상을 선택된 본 메쉬에 대한 변형 가중치로 설정하는 함수 */
void set_vertex_color_from_skin_wgt(GMesh *pSkinMesh, GMesh *pBoneMesh);

/*! \brief 한 정점에서 메쉬의 각 정점까지의 측지거리를 계산하는 함수 */
void get_geodesic_dist_field(GMesh *pMesh, GVertex *pVert, std::vector<double> &Distances, double MaxRadius);

/*! \brief 선택된 정점들에서 메쉬의 각 정점까지의 측지거리를 계산하는 함수 */
void get_geodesic_dist_field(GMesh *pMesh, std::vector<GEditObj *> &VertList, std::vector<double> &Distances, double MaxRadius);

/*! \brief 삼각형 내부의 한 점에서 메쉬의 각 정점까지의 측지거리를 계산하는 함수 */
void get_geodesic_dist_field(GMesh *pMesh, int fidx, GVector3 Coords, std::vector<double> &Distances, double MaxRadius);

/*! \brief 한 정점에서 메쉬의 각 정점까지의 Dijkstra 거리를 계산하는 함수 */
void get_edge_dist_field(GMesh *pMesh, GVertex *pVert, std::vector<double> &Distances, double MaxRadius);

/*! \brief 삼각형 내부의 한 점에서 메쉬의 각 정점까지의 edge거리를 계산하는 함수 */
void get_edge_dist_field(GMesh *pMesh, int fidx, GVector3 Coords, std::vector<double> &Distances, double MaxRadius);

/*!	\brief 선택된 정점에서 메쉬의 각 정점까지의 최단거리를 이용한 극좌표를 계산하는 함수 */
void get_geodesic_polar_coords(GMesh *pMesh, GVertex *pVert, std::vector<double> &Distances, std::vector<double> &Angles, double MaxRadius);

/*! \brief 메쉬 위에서 두 정점 사이의 최단 경로를 계산하는 함수 */
void get_geodesic_paths(GMesh *pMesh, GVertex *P, GVertex *Q, std::vector<GPoint3> &Paths);

/*	\brief 메쉬의 각 정점을 공유한 삼각형 인덱스를 생성하는 함수 */
void get_neighbor_faces_of_vert(GMesh *pMesh, std::vector<std::vector<int>> &FaceList);

/*	\brief 메쉬의 각 정점에 인접한 정점의 인덱스를 생성하는 함수 */
void get_neighbor_verts_of_vert(GMesh *pMesh, std::vector<std::vector<int>> &VertList);

/*!	\brief 메쉬의 각 삼각형에 인접한 삼각형 인덱스를 생성하는 함수 */
void get_neighbor_faces_of_face(GMesh *pMesh, std::vector<int> &FaceList);

/*!	\brief 주어진 정점의 K-ring Neighborhood 정보를 구하는 함수 */
void get_neighbor_info_of_vert(GMesh *pMesh, int VertIdx, int K, std::vector<std::vector<int>> &VertList, std::vector<std::vector<int>> &FaceList);

/*!	\brief 메쉬의 각 정점이 경계 정점인지 조사하는 함수 */
void get_bndry_vert_info(GMesh *pMesh, std::vector<bool> &InfoList);

/*!	\brief 메쉬의 경계 모서리를 구하는 함수 */
void get_bndry_edges(GMesh *pMesh, std::map<int, int> &EdgeList);

/*!	\brief 외부 경계와 내부 경계(구멍) 정점 인덱스를 반환하는 함수 */
void get_bndry_verts(GMesh *pMesh, std::vector<std::vector<int>> &VertList);

/*!	\brief 내부 정점의 인덱스를 반환하는 함수 */
void get_intr_verts(GMesh *pMesh, std::vector<int> &VertList);

/*!	\brief 메쉬의 non-manifold 삼각형의 리스트를 구하는 함수 */
void find_non_manifold_faces(GMesh *pMesh, std::vector<GFace *> &FaceList);

/*!	\brief 메쉬의 degenerated 삼각형의 리스트를 구하는 함수 */
void find_degenerated_faces(GMesh *pMesh, std::vector<GFace *> &FaceList, double eps = 0.0001);

/*!	\brief 메쉬의 isolated 정점의 리스트를 구하는 함수 */
void find_isolated_verts(GMesh *pMesh, std::vector<GVertex *> &VertList);

#else
#endif // _GOBJECT_MESH_UTIL_H