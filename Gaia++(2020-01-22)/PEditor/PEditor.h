#ifndef _PEDITOR_H
#define _PEDITOR_H

// 필요한 헤더 파일을 포함한다.
#include "PEditMeshDlg.h"
#include "PEditCrvDlg.h"
#include "PEditSrfDlg.h"
#include "PEditVolDlg.h"
#include "PEditSwpDlg.h"
#include "PEditBlendSrfDlg.h"

// 링크할 외부 라이브러리를 지정한다.
#pragma comment(lib, "gmath.lib")
#pragma comment(lib, "gobject.lib")
#pragma comment(lib, "gscene.lib")

#ifdef _DEBUG
#pragma comment(lib, "pqpD.lib")
#else
#pragma comment(lib, "pqp.lib")
#endif // _DEBUG

// 사용자 정의 메시지
#ifndef WM_UPDATE_SELECT
#define WM_UPDATE_SELECT (WM_USER + 2)
#endif

/*! \brief 선택 툴의 유형을 구별하는 열거형 타입 */
enum TypeSelectTool
{
	SELECT_TOOL_RECT = 0,
	SELECT_TOOL_CIRCLE,
	SELECT_TOOL_LASSO,
	SELECT_TOOL_POLY,
	SELECT_TOOL_BRUSH,
};

/*! \brief 선택 객체의 유형을 구별하는 열거형 타입 */
enum TypeSelectObj
{
	SELECT_MESH = 0,
	SELECT_BONE,
	SELECT_VERT,
	SELECT_FACE,
	SELECT_FACE_GROUP,
	SELECT_FACE_AND_CUT,	
	SELECT_PT_ON_FACE,
	SELECT_CTRL_PT,
};

/*! \brief 선택 모드를 구별하는 열거형 타입 */
enum TypeSelectMode
{
	SELECT_MODE_FRONT_ONLY = 0,
	SELECT_MODE_FRONT_AND_BACK,
};

/*! \brief 객체의 편집 모드를 구별하는 열거형 타입 */
enum TypeEditTool
{
	EDIT_INACTIVE = 0,
	EDIT_TRA,
	EDIT_TRA_X,
	EDIT_TRA_Y,
	EDIT_TRA_Z,
	EDIT_TRA_XY,
	EDIT_TRA_YZ,
	EDIT_TRA_ZX,
	EDIT_TRA_XYZ,
	EDIT_ROT,
	EDIT_ROT_X,
	EDIT_ROT_Y,
	EDIT_ROT_Z,
	EDIT_SCL, 
	EDIT_SCL_X,
	EDIT_SCL_Y,
	EDIT_SCL_Z,
	EDIT_SCL_XY,
	EDIT_SCL_YZ,
	EDIT_SCL_ZX,
	EDIT_SCL_XYZ,
	EDIT_GEODESIC_PATH,
	EDIT_SCULPT,
	EDIT_SMOOTH,
};

/*!
*	\class PSelector
*	\brief 객체의 선택 기능을 담당하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu) 
*	\date 12 May 2015	
*/
class AFX_EXT_CLASS PSelector
{
public:
	// 생성자 및 소멸자
	PSelector();
	PSelector(const PSelector &cpy);
	virtual ~PSelector();

	// 대입 연산자
	PSelector &operator =(const PSelector &rhs);

	// 객체를 선택하거나 편집하기 위한 마우스 이벤트 처리 가상함수
	virtual void LButtonDown(int x, int y, UINT nFlags);	// 확인
	virtual void MouseMove(int x, int y, UINT nFlags);		// 확인
	virtual void LButtonUp(int x, int y, UINT nFlags);		// 확인
	virtual void MouseWheel(int delta);						// 확인
	GVector3 GetWorldCoord(int x, int y);					// 확인

	// 초기화 및 렌더링 함수
	void Reset();	// 확인
	void Render();
	void RenderRectTool();
	void RenderCircleTool();
	void RenderLassoTool();
	void RenderPolyTool();
	void RenderBrushTool();
	void RenderMouseOverVert();
	void RenderMouseOverFace();
	void RenderMouseOverFaceGroup();
	void RenderMouseOverPtOnFace();
	void RenderSculptTool();

	// 객체 선택을 위한 렌더링 함수
	void RenderForSelect();
	void RenderForSelectFace();
	void RenderForSelectHandle();

	// 객체 선택 함수
	void EnterSelectMode(int x, int y);			// 확인
	int	 ExitSelectMode();						// 확인
	bool SelectMesh(std::string Name);			// 확인
	bool SelectMesh(int x, int y);				// 확인
	bool SelectBone(int x, int y);				// 확인

	void SelectFace(int x, int y);				// 확인
	void SelectFaceAndCutBndry(int x, int y);
	void SelectFaceByBrush(GMesh *pMesh);		// 확인

	void SelectPtOnFace(int x, int y);
	void SelectFaceGroup(int x, int y);			// 확인
	
	void SelectVert(int x, int y);				// 확인
	void SelectVertByBrush(GMesh *pMesh);		// 확인
	bool SelectCtrlPt(int x, int y);			// 확인
	bool SetMouseOverEditHandle(int x, int y);	// 확인
	bool SetMouseOverInfo(int x, int y);		// 확인 
	bool IsPtInPolygon(double x, double y, std::vector<GVector2> &PtList);	// 확인

	// 내부적으로 사용
	void SelectFaceByClick(GMesh *pMesh);		// 확인
	void SelectFaceByRgn(GMesh *pMesh);			// 확인
	void GetPixelList(CPoint p, CPoint q, std::vector<CPoint> &PixelList);
	void GetPixelColor(int x, int y, GLubyte *pixel);
	int FindCutPtList(GMesh *pMesh, std::vector<GCutPt> &CutPtList);	// 미사용

public:	// 데이터 멤버(10)
	/*! \brief 에디터에 대한 포인터 */
	PEditor *pEditor;

	/*! \brief 선택 툴 유형(사각형, 올가미, 다각형, 브러쉬) */
	TypeSelectTool SelectToolType;

	/*! \brief 선택 모드 (앞면, 앞면과 뒷면 모두) */
	TypeSelectMode SelectModeType;

	/*! \brief 선택 객체 유형(메쉬, 정점, 삼각형, 본, 제어점 중) */
	TypeSelectObj SelectObjType;

	/*! \brief 현재 마우스 포인터와 교차된 삼각형 그룹의 인덱스 */
	int MouseOverGroupIdx;

	/*! \brief 현재 마우스 포인터와 교차된 삼각형의 인덱스 */
	int MouseOverFaceIdx;

	/*! \brief 현재 마우스 포인터와 가장 가까운 정점의 인덱스 */
	int MouseOverVertIdx;

	/*! \brief 현재 마우스 포인터와 교차된 삼각형위의 점의 Barycentric 좌표 */
	GVector3 MouseOverPtCoords;

	/*! \brief 윈도우 좌표계에서 표현된 선택 영역의 가장자리 정점 리스트 */
	std::vector<GVector2> SltRgn2DPtList;

	/*! \brief 월드 좌표계에서 표현된 선택 영역의 가장자리 정점 리스트 (렌더링을 위해 사용됨) */
	std::vector<GVector3> SltRgn3DPtList;

	/*! \biref OpenGL 픽킹용 선택버퍼 */
	GLuint SelectBuf[SELECT_BUFFER_SIZE];
};

/*!
*	\class PEditor
*	\brief 3차원 장면내에 포함된 객체의 편집 기능을 담당하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu) 
*	\date 02 Sept. 2012	
*/
class AFX_EXT_CLASS PEditor
{
public:
	// 생성자 및 소멸자
	PEditor();
	PEditor(const PEditor &cpy);
	virtual ~PEditor();

	// 대입연산자
	PEditor &operator =(const PEditor &rhs);

	// 플러그인 구동을 위한 함수
	virtual void InitPlugIn(GScene *_pScene, CDialog *pParent, CDocument *pDoc);

	// 객체를 선택하거나 편집하기 위한 마우스 이벤트 처리 가상함수
	virtual void LButtonDown(int x, int y, UINT nFlags);
	virtual void MouseMove(int x, int y, UINT nFlags);
	virtual void LButtonUp(int x, int y, UINT nFlags);
	virtual void MouseWheel(int delta);

	// 편집 좌표계 관련 함수
	void SetEC();		//확인
	GTransf GetEC();

	// 편집 관련 함수
	void Reset();
	void Edit(GVector3 t, GQuater q, GVector3 s);
	bool EditTra(double dx, double dy, double dz);
	bool EditRot(double dx, double dy, double dz);
	bool EditScl(double dx, double dy, double dz);
	bool EditGeodesicPath(int x, int y);
	bool EditSculpt(int x, int y);
	bool EditSmooth(int x, int y);
	void EditFinish();	
	double GetEditToolSize();
	void ProjectCrvOnMesh(GNurbsCrv *pCrv);

	// 측지선 기반의 편집 함수들
	void FindEditWgt();
	void FindEditWgtForSculpt();
	void FindEditRgn(GMesh *pMesh, std::vector<double> &Distances, double EditDist);
	void FindEditBndry(GMesh *pMesh, std::vector<double> &Distances, double EditDist);
	void FindEditBndry(GMesh *pMesh, std::vector<double> &Distances, std::vector<double> &Angles, double EditDist);
	
	// 렌더링 함수
	void Render();
	void RenderEditRgn();
	
	// 이동 툴의 렌더링 함수
	void RenderTra(double len);
	void RenderTraHandleX(double len);
	void RenderTraHandleY(double len);
	void RenderTraHandleZ(double len);
	void RenderTraHandleXY(double len);
	void RenderTraHandleYZ(double len);
	void RenderTraHandleZX(double len);
	void RenderTraHandleXYZ(double len);

	// 회전 툴의 렌더링 함수
	void RenderRot(double len);
	void RenderRotHandleX(const GVector3 &view_aixs, double len);
	void RenderRotHandleY(const GVector3 &view_aixs, double len);
	void RenderRotHandleZ(const GVector3 &view_aixs, double len);

	// 회전 툴의 핸들 선택 함수
	bool SelectEditHandle(int x, int y);

	// 스케일 툴의 렌더링 함수
	void RenderScl(double len);
	void RenderSclHandleX(double len);
	void RenderSclHandleY(double len);
	void RenderSclHandleZ(double len);
	void RenderSclHandleXY(double len);
	void RenderSclHandleYZ(double len);
	void RenderSclHandleZX(double len);
	void RenderSclHandleXYZ(double len);

	// 편집 객체 관리 함수
	GMesh *GetEditMesh();
	std::string GetClsNameEditObj();
	int  GetNumEditObj();
	void GetRefMeshList(std::vector<GMesh *> &MeshList);
	void AddToEditObjList(GEditObj *pObj);
	bool DelFromEditObjList(GEditObj *pObj);
	void ToggleEditObject(GEditObj *pObj);
	void UniqueEditObjList();	// 확인
	void DeleteObject();
	void DeleteMeshVert();
	void DeleteMeshFace();
	void ShowMesh();
	void HideMesh();

	// 기본 데이터 멤버(11)
public:
	/*! \brief 편집 툴의 편집 좌표계 (객체 선택시 업데이트) */
	GTransf EC;

	/*! \brief 편집 툴 유형(이동, 회전, 스케일, Sculpt, Smooth 중) */
	TypeEditTool EditToolType;
	
	/*! \brief 선택된 편집 객체 리스트(정점, 삼각형, 기하 객체 등) */
	std::vector<GEditObj *> EditObjList;

	/*! \brief 부드러운 편집을 위해 추가로 선택된 정점의 리스트(편집 객체가 정점 또는 삼각형의 경우) */
	std::vector<GVertex *> EditVertRgn; 

	/*! \brief 부드러운 편집을 위해 추가로 선택된 삼각형의 리스트(렌더링 용) */
	std::vector<GFace *> EditFaceRgn;

	/*! \brief 부드러운 편집을 위해 추가로 선택된 삼각형의 리스트의 BVH */
	PQP_Model *pEditFaceRgnBvh;

	/*! \brief 편집 영역의 경계 정점의 리스트 */
	std::vector<GPoint3> EditBndryPts;

	/*! \brief 편집 영역의 2차원 매개화 메쉬 */
	GMesh *pUnfoldMesh;

	/*! \brief 2차원 매개화 메쉬와 3차원 편집 영역간의 삼각형 대응 정보 (2차원 삼각형 인덱스, 3차원 삼각형 인덱스) */
	std::map<int, int> UnfoldFaceMap;

	/*! \brief 선택 및 편집을 위한 마우스의 위치 */
	CPoint MousePt[2];

	/*! \brief 회전 편집을 할 경우, 최초 회전 핸들이 선택된 참조 좌표계 */
	GTransf RotRefX;

	/*! \brief 회전 편집을 할 경우, 회전 핸들의 선택된 지점 */
	GVector3 RotRefPos;	

	// 추가 데이터 멤버(8)
public:	
	/*! \brief 메인 프로그램의 장면에 대한 포인터 */
	GScene *pScene;

	/*! \brief 편집 객체 선택을 위한 객체 */
	PSelector theSelector;

	/*! \brief 메쉬 편집 대화상자 */
	PEditMeshDlg EditMeshDlg;

	/*! \brief 곡선 편집 대화상자 */
	PEditCrvDlg EditCrvDlg;

	/*! \brief 곡면 편집 대화상자 */
	PEditSrfDlg EditSrfDlg;

	/*! \brief 볼륨 편집 대화상자 */
	PEditVolDlg EditVolDlg;

	/*! \brief 스윕 편집 대화상자 */
	PEditSwpDlg EditSwpDlg;

	/*! \brief 블렌딩 곡면 편집 대화상자 */
	PEditBlendSrfDlg EditBlendSrfDlg;
};

#endif // _PEDITOR_H