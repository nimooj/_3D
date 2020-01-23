#ifndef _GOBJECT_BRIDGE_MESH_H
#define _GOBJECT_BRIDGE_MESH_H

class GMesh;
class GVertex;
class GEditObj;
class GSweepSrf;

/*! 메쉬 위에서 측지거리로 정의된 두 원을 채우는 메쉬를 생성하는 함수 */
GMesh *create_mesh_from_two_circles_on_mesh(GMesh *pMesh, GVertex *pVert, double r0, double r1, int snum_u, int snum_v);

/*! 메쉬 위에서 두 경계 곡선 사이의 메쉬를 생성하는 함수 */
GMesh *create_mesh_from_two_curves_on_mesh(GMesh *pMesh, GVertex *pVert, GNurbsCrv *pCrv0, GNurbsCrv *pCrv1, int snum_u, int snum_v, bool trim);

/*! 메쉬와 추가 정점을 연결하는 삼각형을 메쉬에 추가하는 함수 */
void connect_mesh_with_verts(GMesh *pMesh, std::vector<GPoint3> &Verts);

/*! 두 메쉬를 연결하는 브리지 메쉬를 생성하는 함수 (미사용) */
GMesh *create_mesh_bridge(GMesh *pStMesh, GVertex *pStVert, double st_cont_rad, double st_aux_rad, 
						  GMesh *pEdMesh, GVertex *pEdVert, double ed_cont_rad, double ed_aux_rad,
						  int snum_u, int snum_v);

/*! 두 메쉬를 연결하는 브리지 메쉬를 생성하는 함수 (미사용) */
GMesh *create_mesh_bridge(GMesh *pStMesh, GVertex *pStVert, GNurbsCrv *pStContCrv, GNurbsCrv *pStAuxCrv,
						  GMesh *pEdMesh, GVertex *pEdVert, GNurbsCrv *pEdContCrv, GNurbsCrv *pEdAuxCrv,
						  double Mu, int snum_u, int snum_v, bool trim);

/*! 두 메쉬를 연결하는 브리지 메쉬를 생성하는 함수 */
GMesh *create_mesh_bridge(GMesh *pStMesh, GVertex *pStVert, GNurbsCrv *pStContCrv, double alpha,
						  GMesh *pEdMesh, GVertex *pEdVert, GNurbsCrv *pEdContCrv, double beta,
						  double mu, int snum_u, int snum_v, bool trim);

/*! 생성된 브리지 메쉬를 갱신하는 함수 */
void update_mesh_bridge(GMesh *pBridgeMesh,
						GMesh *pStMesh, GVertex *pStVert, GNurbsCrv *pStContCrv, double alpha,
						GMesh *pEdMesh, GVertex *pEdVert, GNurbsCrv *pEdContCrv, double beta,
						double mu, int snum_u, int snum_v, bool trim);

/*! 메쉬와 스윕 곡면을 연결하는 브리지 메쉬를 생성하는 함수 */
GMesh *create_mesh_bridge(GMesh *pMesh, GVertex *pVert, double ContRad, double AuxRad, 
						  GSweepSrf *pSrf, double ContParam, double AuxParam, 
						  int snum_u, int snum_v);


#endif // _GOBJECT_BRIDGE_MESH_H