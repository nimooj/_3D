#ifndef _GCREATOR_H
#define _GCREATOR_H

#include "PCreateCrvDlg.h"
#include "PCreateSrfDlg.h"
#include "PCreatePlyDlg.h"
#include "PCreatePlnDlg.h"

#pragma comment(lib, "gmath.lib")
#pragma comment(lib, "gobject.lib")
#pragma comment(lib, "gscene.lib")
#ifdef _DEBUG
#pragma comment(lib, "pqpD.lib")
#else
#pragma comment(lib, "pqp.lib")
#endif // _DEBUG

#define  WM_EXIT_CREATOR (WM_USER + 1)

/*! \brief ��ü�� ���� ��带 �����ϴ� ������ Ÿ�� */
enum TypeCreate
{
	CREATE_INACTIVE = 0,

	// �ҽ� ��ü ����
	CREATE_CRV_CTL_PT,
	CREATE_CRV_INT_PT,
	CREATE_SRF_CTL_PT,
	CREATE_NURBS_SPHERE,
	CREATE_NURBS_TEAPOT,
	CREATE_CRV_ON_MESH_FROM_SKETCH,

	// ������ ��ü
	CREATE_POLYGON,
	CREATE_POLYGON_SPHERE,
	CREATE_POLYGON_ICOSAHEDRON,
	CREATE_POLYGON_TEAPOT,
	CREATE_PLANE,
	CREATE_CIRCLE,
	CREATE_CUT_LINE,
	CREATE_CUT_LINE_FROM_SKETCH,
	CREATE_POLYGON_DISK,

	// �޽����� ���ǵ� ��ü
	CREATE_GEODESIC_PATH,
	CREATE_GEODESIC_MESH,

	// �̿ϼ�....
	CREATE_PTS_ON_MESH,
};

/*!
*	\class PCreator
*	\brief �ҽ� ��� ����� �����ϴ� �������̽� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001	
*/
class AFX_EXT_CLASS PCreator
{
public:
	// ������ �� �Ҹ���
	PCreator();
	PCreator(const PCreator &cpy);
	virtual ~PCreator();

	// ���� ������
	PCreator &operator =(const PCreator &rhs);

	// �÷����� ������ ���� �Լ�
	void InitPlugIn(GScene *_pScene, CDialog *pParent, CDocument *_pDoc);

	// ���콺 �̺�Ʈ ���� ��� �Լ�
	void LButtonDown(int x, int y, unsigned flag);
	void MouseMove(int x, int y, unsigned flag);
	void MouseMoveForCutLine(int x, int y, unsigned flag);
	void MouseMoveForPtsOnMesh(int x, int y, unsigned flag);
	bool SetMouseOverInfo(int x, int y, unsigned flag);
	void MouseMoveForPtOnFloor(int x, int y, unsigned flag);
	void LButtonUp(int x, int y, unsigned flag);
	GVector3 GetWorldCoord(int x, int y);
	void GetPixelList(CPoint p, CPoint q, std::vector<CPoint> &PixelList);

	// �Ϲ� ��� �Լ�
	void InitBaseMesh(GMesh *pMesh);
	void Reset();
	void Render();
	void Finish();	
	GTransf GetMC();
	GFace *SelectVertOnFace(int x, int y, GPoint3 &ispt);
	void RenderForSelect();

	// �ҽ� ��ü ����
	void RenderNurbsCrv();
	void CreateNurbsCrv();
	void RenderNurbsSrf();
	void CreateNurbsSrf();
	void RenderNurbsCircle();
	void CreateNurbsCircle();
	void RenderNurbsTeapot();
	void CreateNurbsTeapot();
	void RenderNurbsSphere();
	void CreateNurbsSphere();

	// �⺻ ������ ��ü �����Լ�
	void RenderPolygon();
	void CreatePolygon();
	void RenderPolygonPlane();	
	void CreatePolygonPlane();
	void RenderPolygonSphere();
	void CreatePolygonSphere();
	void CreatePolygonIcosahedron();
	void RenderPolygonDisk();
	void CreatePolygonDisk();
	void RenderPolygonTeapot();
	void CreatePolygonTeapot();
	void RenderCutLine();
	void CreateCutLine();

	// ��Ÿ ��ü ���� �Լ�.
	void RenderMouseOverVert();
	void RenderGeodesicPath();
	void RenderQuadLayout();
	void RenderCoonsBndry();

	void RenderPtsOnMesh();
	void CreatePtsOnMesh();

	void RenderCrvOnMeshFromSketch();
	void CreateCrvOnMeshFromSketch();

	void RenderCutLineFromSketch();
	void CreateCutLineFromSketch();

public:
	// ������ ���
	/*! \brief ���� ���α׷��� ��ť��Ʈ�� ���� ������(���� �̻��) */
	CDocument *pDoc;

	/*! \brief ���� ���α׷��� ��鿡 ���� ������ */
	GScene *pScene;

	/*! \brief ��ü ���� Ÿ�� */
	TypeCreate CreateType;

	/*! \brief Ư�� ��ü(���ܼ�) ������ ���Ǵ� ���� �޽� */
	GMesh *pBaseMesh;

	/*! \brief ���� ���콺 �����Ϳ� ������ �ﰢ���� �ε��� */
	int MouseOverFaceIdx;

	/*! \brief ���� ���콺 �����Ϳ� ���� ����� ������ �ε��� */
	int MouseOverVertIdx;

	/*! \brief ���� ���콺 �����Ϳ� ������ �ﰢ������ ���� Barycentric ��ǥ */
	GVector3 MouseOverPtCoords;

	/*! \brief ���õ� ������ �ε��� */
	int PrevSltVertIdx;

	/*! \brief ��ü ������ ���� ������ ����Ʈ */
	std::vector<GVector4> PtList;

	/*! \brief ���ܼ� ������ ���� ������ ����Ʈ */
	std::vector<GCutPt> CutLinePtList;

	/*! \brief ��ũ�� ��ǥ�迡�� ǥ���� ����ġ ���� ����Ʈ */
	std::vector<GVector2> Sketch2DPtList;

	/*! \brief ���� ��ǥ�迡�� ǥ���� ����ġ ���� ����Ʈ ����Ʈ (�������� ���� ����) */
	std::vector<GVector3> Sketch3DPtList;

	/*! \brief � ���� ��ȭ���� */
	PCreateCrvDlg CreateCrvDlg;

	/*! \brief ��� ���� ��ȭ���� */
	PCreateSrfDlg CreateSrfDlg;

	/*! \brief ������ ���� ��ȭ���� */
	PCreatePlyDlg CreatePlyDlg;		

	/*! \brief ��� ���� ��ȭ���� */
	PCreatePlnDlg CreatePlnDlg;		
};

#endif // _GCREATOR_H

