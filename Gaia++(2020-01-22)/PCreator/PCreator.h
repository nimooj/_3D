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

/*! \brief 객체의 생성 모드를 정의하는 열거형 타입 */
enum TypeCreate
{
	CREATE_INACTIVE = 0,

	// 넙스 객체 생성
	CREATE_CRV_CTL_PT,
	CREATE_CRV_INT_PT,
	CREATE_SRF_CTL_PT,
	CREATE_NURBS_SPHERE,
	CREATE_NURBS_TEAPOT,
	CREATE_CRV_ON_MESH_FROM_SKETCH,

	// 폴리곤 객체
	CREATE_POLYGON,
	CREATE_POLYGON_SPHERE,
	CREATE_POLYGON_ICOSAHEDRON,
	CREATE_POLYGON_TEAPOT,
	CREATE_PLANE,
	CREATE_CIRCLE,
	CREATE_CUT_LINE,
	CREATE_CUT_LINE_FROM_SKETCH,
	CREATE_POLYGON_DISK,

	// 메쉬위에 정의된 객체
	CREATE_GEODESIC_PATH,
	CREATE_GEODESIC_MESH,

	// 미완성....
	CREATE_PTS_ON_MESH,
};

/*!
*	\class PCreator
*	\brief 넙스 곡선과 곡면을 생성하는 인터페이스 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 01 Jan 2001	
*/
class AFX_EXT_CLASS PCreator
{
public:
	// 생성자 및 소멸자
	PCreator();
	PCreator(const PCreator &cpy);
	virtual ~PCreator();

	// 대입 연산자
	PCreator &operator =(const PCreator &rhs);

	// 플러그인 구동을 위한 함수
	void InitPlugIn(GScene *_pScene, CDialog *pParent, CDocument *_pDoc);

	// 마우스 이벤트 관련 멤버 함수
	void LButtonDown(int x, int y, unsigned flag);
	void MouseMove(int x, int y, unsigned flag);
	void MouseMoveForCutLine(int x, int y, unsigned flag);
	void MouseMoveForPtsOnMesh(int x, int y, unsigned flag);
	bool SetMouseOverInfo(int x, int y, unsigned flag);
	void MouseMoveForPtOnFloor(int x, int y, unsigned flag);
	void LButtonUp(int x, int y, unsigned flag);
	GVector3 GetWorldCoord(int x, int y);
	void GetPixelList(CPoint p, CPoint q, std::vector<CPoint> &PixelList);

	// 일반 멤버 함수
	void InitBaseMesh(GMesh *pMesh);
	void Reset();
	void Render();
	void Finish();	
	GTransf GetMC();
	GFace *SelectVertOnFace(int x, int y, GPoint3 &ispt);
	void RenderForSelect();

	// 넙스 객체 생성
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

	// 기본 폴리곤 객체 생성함수
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

	// 기타 객체 생성 함수.
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
	// 데이터 멤버
	/*! \brief 메인 프로그램의 다큐먼트에 대한 포인터(현재 미사용) */
	CDocument *pDoc;

	/*! \brief 메인 프로그램의 장면에 대한 포인터 */
	GScene *pScene;

	/*! \brief 객체 생성 타입 */
	TypeCreate CreateType;

	/*! \brief 특정 객체(절단선) 생성시 사용되는 기초 메쉬 */
	GMesh *pBaseMesh;

	/*! \brief 현재 마우스 포인터와 교차된 삼각형의 인덱스 */
	int MouseOverFaceIdx;

	/*! \brief 현재 마우스 포인터와 가장 가까운 정점의 인덱스 */
	int MouseOverVertIdx;

	/*! \brief 현재 마우스 포인터와 교차된 삼각형위의 점의 Barycentric 좌표 */
	GVector3 MouseOverPtCoords;

	/*! \brief 선택된 정점의 인덱스 */
	int PrevSltVertIdx;

	/*! \brief 객체 생성을 위한 정점의 리스트 */
	std::vector<GVector4> PtList;

	/*! \brief 절단선 생성을 위한 정점의 리스트 */
	std::vector<GCutPt> CutLinePtList;

	/*! \brief 스크린 좌표계에서 표현된 스케치 정점 리스트 */
	std::vector<GVector2> Sketch2DPtList;

	/*! \brief 월드 좌표계에서 표현된 스케치 정점 리스트 리스트 (렌더링을 위해 사용됨) */
	std::vector<GVector3> Sketch3DPtList;

	/*! \brief 곡선 생성 대화상자 */
	PCreateCrvDlg CreateCrvDlg;

	/*! \brief 곡면 생성 대화상자 */
	PCreateSrfDlg CreateSrfDlg;

	/*! \brief 폴리곤 생성 대화상자 */
	PCreatePlyDlg CreatePlyDlg;		

	/*! \brief 평면 생성 대화상자 */
	PCreatePlnDlg CreatePlnDlg;		
};

#endif // _GCREATOR_H

