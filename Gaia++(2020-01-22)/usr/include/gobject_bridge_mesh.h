#ifndef _GOBJECT_BRIDGE_MESH_H
#define _GOBJECT_BRIDGE_MESH_H

class GMesh;
class GVertex;
class GEditObj;
class GSweepSrf;

/*! �޽� ������ �����Ÿ��� ���ǵ� �� ���� ä��� �޽��� �����ϴ� �Լ� */
GMesh *create_mesh_from_two_circles_on_mesh(GMesh *pMesh, GVertex *pVert, double r0, double r1, int snum_u, int snum_v);

/*! �޽� ������ �� ��� � ������ �޽��� �����ϴ� �Լ� */
GMesh *create_mesh_from_two_curves_on_mesh(GMesh *pMesh, GVertex *pVert, GNurbsCrv *pCrv0, GNurbsCrv *pCrv1, int snum_u, int snum_v, bool trim);

/*! �޽��� �߰� ������ �����ϴ� �ﰢ���� �޽��� �߰��ϴ� �Լ� */
void connect_mesh_with_verts(GMesh *pMesh, std::vector<GPoint3> &Verts);

/*! �� �޽��� �����ϴ� �긮�� �޽��� �����ϴ� �Լ� (�̻��) */
GMesh *create_mesh_bridge(GMesh *pStMesh, GVertex *pStVert, double st_cont_rad, double st_aux_rad, 
						  GMesh *pEdMesh, GVertex *pEdVert, double ed_cont_rad, double ed_aux_rad,
						  int snum_u, int snum_v);

/*! �� �޽��� �����ϴ� �긮�� �޽��� �����ϴ� �Լ� (�̻��) */
GMesh *create_mesh_bridge(GMesh *pStMesh, GVertex *pStVert, GNurbsCrv *pStContCrv, GNurbsCrv *pStAuxCrv,
						  GMesh *pEdMesh, GVertex *pEdVert, GNurbsCrv *pEdContCrv, GNurbsCrv *pEdAuxCrv,
						  double Mu, int snum_u, int snum_v, bool trim);

/*! �� �޽��� �����ϴ� �긮�� �޽��� �����ϴ� �Լ� */
GMesh *create_mesh_bridge(GMesh *pStMesh, GVertex *pStVert, GNurbsCrv *pStContCrv, double alpha,
						  GMesh *pEdMesh, GVertex *pEdVert, GNurbsCrv *pEdContCrv, double beta,
						  double mu, int snum_u, int snum_v, bool trim);

/*! ������ �긮�� �޽��� �����ϴ� �Լ� */
void update_mesh_bridge(GMesh *pBridgeMesh,
						GMesh *pStMesh, GVertex *pStVert, GNurbsCrv *pStContCrv, double alpha,
						GMesh *pEdMesh, GVertex *pEdVert, GNurbsCrv *pEdContCrv, double beta,
						double mu, int snum_u, int snum_v, bool trim);

/*! �޽��� ���� ����� �����ϴ� �긮�� �޽��� �����ϴ� �Լ� */
GMesh *create_mesh_bridge(GMesh *pMesh, GVertex *pVert, double ContRad, double AuxRad, 
						  GSweepSrf *pSrf, double ContParam, double AuxParam, 
						  int snum_u, int snum_v);


#endif // _GOBJECT_BRIDGE_MESH_H