#ifndef _GOBJECT_MESH_UTIL_H
#define _GOBJECT_MESH_UTIL_H

class GMesh;
class GVertex;
class GEditObj;
class GSweepSrf;
class GMaterial;
class PQP_Model;

/*! ���Ӱ� �����Ǵ� ��ü�� �̸��� �����ϴ� �Լ� */
std::string create_mesh_name(std::string TypeObj);

/*! ���Ӱ� �����Ǵ� ������ �̸��� �����ϴ� �Լ� */
std::string create_mtl_name(std::string Prefix);

/*! Cylinder �޽��� �����ϴ� �Լ� */
GMesh *create_mesh_from_cylinder(GNurbsCrv *pCrv);

/*! Sphere �޽��� �����ϴ� �Լ� */
GMesh *create_mesh_from_sphere(std::string Name, GTransf MC, double radius, int slices, int stacks);

/*! Sphere Icosahedron �޽��� �����ϴ� �Լ� */
GMesh *create_mesh_from_icosahedron(std::string Name, GTransf MC, double radius, int subdiv_depth);

/*! Teapot �޽��� �����ϴ� �Լ� */
GMesh *create_mesh_from_teapot(double size);

/*! Box �޽��� �����ϴ� �Լ� */
GMesh *create_mesh_from_box(GPoint3 p, GPoint3 q);

/*! �޽��� �κ� �޽��� �����ϴ� �Լ� */
GMesh *create_mesh_from_faces(std::string Name, GMesh *pParentMesh, std::vector<GFace *> &SltFaceList, std::map<int, int> &VertIdxMap);

/*! 2���� ������ �Ű�ȭ�� �޽��� �����ϴ� �Լ� */
GMesh *create_mesh_unfolded(std::string Name, GMesh *pMesh, int StartVertIdx = -1, int BndryType = 0);

/*! ��� �������� �޽��� �����ϴ� �Լ� */
GMesh *create_mesh_from_bndry_verts(std::string Name, std::vector<GVertex> &BndryVerts, int FillType);

/*! ���� ������κ��� �κ� �޽��� �����ϴ� �Լ� */
GMesh *create_mesh_from_sweep_srf(GSweepSrf *pSrf, double ContParam, double AuxParam, int snum_u, int snum_v);

/*! �� �޽��� �浹 ������ �����ϴ� �Լ� */
int collision_check(GMesh *pMesh1, GMesh *pMesh2);

/*! �κ� �޽��� BVH�� �޽����� �浹 ������ �����ϴ� �Լ� */
int collision_check(GTransf &MC1, PQP_Model *pBVH1, GMesh *pMesh2);

/*! Į��ʿ� �ش��ϴ� ������ ���ϴ� �Լ� */
GVector3 get_color_gradient_from_rainbow(double t);

/*! Į��ʿ� �ش��ϴ� ������ ���ϴ� �Լ� */
GVector3 get_color_gradient(double t, GVector3 C1, GVector3 C2, GVector3 C3);

/*! ������ ������ �����Ͽ� ��ȯ�ϴ� �Լ� */
GMaterial get_material_random();

/*! ����� ���� ������ �����ϴ� �Լ� */
GMaterial get_material(double diff_r, double diff_g, double diff_b, 
					   double ambi_r = 0.1, double ambi_g = 0.1, double ambi_b = 0.1, 
					   double spec_r = 0.7, double spec_g = 0.7, double spec_b = 0.7);

/*! �޽��� ���콺 ���(kG), ��� ���(kH), �ּ� ���(kMin) �׸��� �ִ� ���(kMax)�� ����ϴ� �Լ� */
void get_curvature(GMesh *pMesh, std::vector<double> &kG, std::vector<double> &kH, std::vector<double> &kMin, std::vector<double> &kMax);

/*! �޽��� �� ������ Gaussian ����� ����Ͽ� ��ȯ�ϴ� �Լ� */
void get_gaussian_curvature(GMesh *pMesh, std::vector<double> &Curvatures, double &MinK, double &MaxK);

/*! �޽��� �� ������ Mean ����� ����Ͽ� ��ȯ�ϴ� �Լ� */
void get_mean_curvature(GMesh *pMesh, std::vector<double> &Curvatures, double &MinK, double &MaxK);

/*! \brief ��ü�� ǥ������ ����ϴ� �Լ� */
double get_mesh_area(GMesh *pMesh);

/*! \brief �޽��� Laplacian ��źȭ ������ �����ϴ� �Լ� */
void edit_mesh_by_smoothing(GMesh *pMesh);

/*! \brief �޽��� ����ȭ ������ �����ϴ� �Լ� */
void edit_mesh_by_refine(GMesh *pMesh);

/*! \brief �޽��� �ߺ��� ������ ���� ��踦 ��޴� �Լ� */
void edit_mesh_by_stitching(GMesh *pMesh, double tol = 0.001);

/*! \brief �޽����� ������ �ʴ� ������ �����ϴ� �Լ� */
void edit_mesh_by_removing_unused_verts(GMesh *pMesh);

/*! \brief �޽��� ���� ������ ���� ���ͷ� �����ϴ� �Լ� */
void set_vertex_color_from_normal(GMesh *pTarMesh, GMesh *pSrcMesh = NULL);

/*! \brief �޽��� ���� ������ ���õ� �� �޽��� ���� ���� ����ġ�� �����ϴ� �Լ� */
void set_vertex_color_from_skin_wgt(GMesh *pSkinMesh, GMesh *pBoneMesh);

/*! \brief �� �������� �޽��� �� ���������� �����Ÿ��� ����ϴ� �Լ� */
void get_geodesic_dist_field(GMesh *pMesh, GVertex *pVert, std::vector<double> &Distances, double MaxRadius);

/*! \brief ���õ� �����鿡�� �޽��� �� ���������� �����Ÿ��� ����ϴ� �Լ� */
void get_geodesic_dist_field(GMesh *pMesh, std::vector<GEditObj *> &VertList, std::vector<double> &Distances, double MaxRadius);

/*! \brief �ﰢ�� ������ �� ������ �޽��� �� ���������� �����Ÿ��� ����ϴ� �Լ� */
void get_geodesic_dist_field(GMesh *pMesh, int fidx, GVector3 Coords, std::vector<double> &Distances, double MaxRadius);

/*! \brief �� �������� �޽��� �� ���������� Dijkstra �Ÿ��� ����ϴ� �Լ� */
void get_edge_dist_field(GMesh *pMesh, GVertex *pVert, std::vector<double> &Distances, double MaxRadius);

/*! \brief �ﰢ�� ������ �� ������ �޽��� �� ���������� edge�Ÿ��� ����ϴ� �Լ� */
void get_edge_dist_field(GMesh *pMesh, int fidx, GVector3 Coords, std::vector<double> &Distances, double MaxRadius);

/*!	\brief ���õ� �������� �޽��� �� ���������� �ִܰŸ��� �̿��� ����ǥ�� ����ϴ� �Լ� */
void get_geodesic_polar_coords(GMesh *pMesh, GVertex *pVert, std::vector<double> &Distances, std::vector<double> &Angles, double MaxRadius);

/*! \brief �޽� ������ �� ���� ������ �ִ� ��θ� ����ϴ� �Լ� */
void get_geodesic_paths(GMesh *pMesh, GVertex *P, GVertex *Q, std::vector<GPoint3> &Paths);

/*	\brief �޽��� �� ������ ������ �ﰢ�� �ε����� �����ϴ� �Լ� */
void get_neighbor_faces_of_vert(GMesh *pMesh, std::vector<std::vector<int>> &FaceList);

/*	\brief �޽��� �� ������ ������ ������ �ε����� �����ϴ� �Լ� */
void get_neighbor_verts_of_vert(GMesh *pMesh, std::vector<std::vector<int>> &VertList);

/*!	\brief �޽��� �� �ﰢ���� ������ �ﰢ�� �ε����� �����ϴ� �Լ� */
void get_neighbor_faces_of_face(GMesh *pMesh, std::vector<int> &FaceList);

/*!	\brief �־��� ������ K-ring Neighborhood ������ ���ϴ� �Լ� */
void get_neighbor_info_of_vert(GMesh *pMesh, int VertIdx, int K, std::vector<std::vector<int>> &VertList, std::vector<std::vector<int>> &FaceList);

/*!	\brief �޽��� �� ������ ��� �������� �����ϴ� �Լ� */
void get_bndry_vert_info(GMesh *pMesh, std::vector<bool> &InfoList);

/*!	\brief �޽��� ��� �𼭸��� ���ϴ� �Լ� */
void get_bndry_edges(GMesh *pMesh, std::map<int, int> &EdgeList);

/*!	\brief �ܺ� ���� ���� ���(����) ���� �ε����� ��ȯ�ϴ� �Լ� */
void get_bndry_verts(GMesh *pMesh, std::vector<std::vector<int>> &VertList);

/*!	\brief ���� ������ �ε����� ��ȯ�ϴ� �Լ� */
void get_intr_verts(GMesh *pMesh, std::vector<int> &VertList);

/*!	\brief �޽��� non-manifold �ﰢ���� ����Ʈ�� ���ϴ� �Լ� */
void find_non_manifold_faces(GMesh *pMesh, std::vector<GFace *> &FaceList);

/*!	\brief �޽��� degenerated �ﰢ���� ����Ʈ�� ���ϴ� �Լ� */
void find_degenerated_faces(GMesh *pMesh, std::vector<GFace *> &FaceList, double eps = 0.0001);

/*!	\brief �޽��� isolated ������ ����Ʈ�� ���ϴ� �Լ� */
void find_isolated_verts(GMesh *pMesh, std::vector<GVertex *> &VertList);

#else
#endif // _GOBJECT_MESH_UTIL_H