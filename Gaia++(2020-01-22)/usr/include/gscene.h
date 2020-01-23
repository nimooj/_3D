#ifndef _GSCENE_H
#define _GSCENE_H

#include "..\usr\include\gscene_io.h"
#include "..\usr\include\gscene_util.h"
#include "..\usr\include\gscene_image.h"
#include "..\usr\include\gscene_qtree.h"
#include "..\usr\include\gscene_heap.h"

class GMaterial;
class GLight;
class GCamera;
class GScene;
class GEditObj;
class GVertex;
class GCtrlPt;
class GBndBox;
class GHeap;

#ifndef SELECT_BUFFER_SIZE
#define SELECT_BUFFER_SIZE 256
#endif // SELECT_BUFFER_SIZE

/*!
*	\class GLight
*	\brief 장면의 광원을 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu) 
*	\date 02 Sept. 2012	
*/
class GLight
{
public:
	// 생성자 및 소멸자
	GLight(GLenum _LightNum);
	GLight(const GLight &cpy);
	virtual ~GLight();

	// 대입연산자
	GLight &operator =(const GLight &rhs);

	// 멤버함수
	void Enable();
	void Disable();
	void SetPosition(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void SetAmbient(GLfloat r, GLfloat g, GLfloat b);
	void SetDiffuse(GLfloat r, GLfloat g, GLfloat b);
	void SetSpecular(GLfloat r, GLfloat g, GLfloat b);
	void SetPointLight();
	void SetDirectionalLight();
	void SetSpotLight(GLfloat vx, GLfloat vy, GLfloat vz, GLfloat cutoff);

public:
	// 데이터 멤버
	/*! \brief 광원의 번호 */
	GLenum LightNum;

	/*! \brief 광원의 위치 */
	GLfloat Position[4];

	/*! \brief 광원의 주변광 성분 */
	GLfloat Ambient[3];

	/*! \brief 광원의 난반사 성분 */
	GLfloat Diffuse[3];

	/*! \brief 광원의 전반사 성분 */
	GLfloat Specular[3];

	/*! \brief 광원의 활성화 여부 */
	bool IsEnabled;

	/*! \brief 집중광원 여부 */
	bool IsSpotLight;

	/*! \brief 집중관원의 경우 광원의 방향 */
	GLfloat SpotDirection[3];

	/*! \brief 집중관원의 경우 광원의 범위 */
	GLfloat SpotCutOff;
};

/*! \brief 카메라의 투영 타입을 정의하는 열거형 변수 */
enum TypeProjection
{
	PROJECTION_ORTHO = 0,
	PROJECTION_PERS,
};

/*! \brief 카메라의 조작 타입을 정의하는 열거형 변수 */
enum TypeCameraManipulate
{ 
	CAMERA_MANIPULATE_NONE = 0,
	CAMERA_MANIPULATE_ROTATE = 1,
	CAMERA_MANIPULATE_PAN,
	CAMERA_MANIPULATE_ZOOM,
};

/*!
*	\class GCamera
*	\brief 3차원 장면(GScene)을 관측하는 카메라를 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu) 
*	\date 06 Sept. 2012	
*/
class GCamera
{
public:
	// 생성자 및 소멸자
	GCamera();
	GCamera(GScene *_pScene, double _Fovy, double _Near, double _Far, int _ViewWidth, int _ViewHeight);
	GCamera(GScene *_pScene, GVector3 Eye, GVector3 Center, GVector3 Up);
	GCamera(GScene *_pScene, double _Left, double _Right, double _Bottom, double _Top);
	GCamera(const GCamera &cpy);
	virtual ~GCamera();

	// 대입연산자
	GCamera &operator =(const GCamera &rhs);

	// 관측변환, 관측공간, 뷰변환 설정 함수
	void SetupViewFrustum();
	void SetupViewTransform();
	void SetupViewport(int w, int h);

	// 관측 변환 조작 함수
	void Panning(int x, int y);
	void Rotating(int x, int y);
	void Zooming(int type);
	void SetArcBallType(TypeArcBall type);
	
	// 마우스 이번트 처리 함수
	void LButtonDown(int x, int y);
	void MouseMove(int x, int y);

	// 렌더링 관련 함수
	void Render();

	// 카메라 정보 입/출력 관련 함수
	void Export(char *fname);
	void Import(char *fname);

	// 투영 처리 관련 함수
	void GetWorldCoord(double wx, double wy, double depth, double *ox, double *oy, double *oz);
	void GetScreenCoord(GVertex *pVert, double *wx, double *wy, double *wz);
	void GetScreenCoord(GCtrlPt *pPt, double *wx, double *wy, double *wz);
	void GetScreenCoord(double ox, double oy, double oz, double *wx, double *wy, double *wz);
	GLine GetOrthoRay(double x, double y);
	GVector3 GetViewDirection();

public:
	// 데이터 멤버
	/*! \brief 카메라가 포함된 장면에 대한 포인터 */
	GScene *pScene;

	/*! \brief 원근 투영을 위한 카메라의 화각(field of view) */
	double Fovy;

	/*! \brief 투영 공간의 앞쪽 평면까지의 거리(>0) */
	double Near;

	/*! \brief 투영 공간의 뒤쪽 평면까지의 거리(>0) */
	double Far;

	/*! \brief 뷰포트 변환에 사용될 너비 */
	int ViewWidth;

	/*! \brief 뷰포트 변환에 사용될 높이 */
	int ViewHeight;

	/*! \brief 카메라 중심에서 월드 좌표계까지의 거리(<0) */
	double ZoomDist;

	/*! \brief 월드 좌표계의 회전 정보 */
	GQuater Rot;

	/*! \brief 월드 좌표계의 이동 정보 */
	GVector3 Pan;

	/*! \brief 뷰 좌표계에서 표현된 월드 좌표계 (WC = ZoomDist * Rot * Pan) */
	GTransf WC;

	/*! \brief 현재의 마우스 위치 정보 */
	int MousePt[2];

	/*! \brief 카메라의 투영 타입 */
	TypeProjection CameraType;

	/*! \brief 카메라의 회전 아크볼 타입 */
	TypeArcBall ArcBallType;

	/*! \brief 카메라의 조작 타입 */
	TypeCameraManipulate CameraManipulateType;

	/*! \brief 카메라의 모델뷰 변환 행렬 */
	double ModelviewMat[16];

	/*! \brief 카메라의 투영 변환 행렬 */
	double ProjectionMat[16];

	/*! \brief 카메라의 뷰포트 변환 정보 */
	int Viewport[4];

	/*! \brief 장면의 렌더링 결과를 저장하는 칼라 배열 */
	unsigned char *pPixelBuf;
};

/*! \brief Head-up Display의 표시 정보에 대한 열거형 */
#define HUD_VIEW_CUBE	0x0001
#define HUD_FRAME_RATE	0x0002
#define HUD_ORIGIN_AXIS	0x0004

/*!
*	\class GHud
*	\brief 3차원 장면의 Head-up Display를 담당하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu) 
*	\date 02 Sept. 2012	
*/
class GHud
{
public:
	// 생성자 및 소멸자
	GHud();
	GHud(const GHud &cpy);
	virtual ~GHud() {}

	// 대입연산자
	GHud &operator =(const GHud &rhs);

	// 멤버함수
	void Resize(int width, int height);
	void Render();
	void RenderBackGround();
	void RenderFPS();
	void RenderOriginAxis();
	void RenderViewCube();
	void RenderLogo();
	void RenderMessage();
	void RenderText(double x, double y, double z, const char *str, double r, double g, double b);
	void LButtonDown(int x, int y);

	// 장면의 바라보는 방향을 설정
	void SetHomeView();
	void SetFrontView();
	void SetBackView();
	void SetLeftView();
	void SetRightView();
	void SetBottomView();
	void SetTopView();
	void SetEastView();
	void SetWestView();
	void SetSouthView();
	void SetNorthView();

public:
	// 데이터 멤버
	/*! \brief 제어판이 포함된 장면에 대한 포인터 */
	GScene *pScene;

	/*! \brief 뷰 큐브를 렌더링하기 위한 텍스춰와 재질 */
	std::vector<GMaterial> TextureList;

	/*! \brief 현재의 관측방향이 xy, yz, zx 평면과 평행인지를 판단 */
	bool Parallel;

	/*! \brief 직교투영을 위한 관측공간 */
	double ViewFrustum[6];

	/*! \brief HUD의 표시 정보 */
	unsigned int DisplayInfo;

	/*! \brief HUD에 출력할 메시지 문자열 */
	std::string Message;
};

/*! \brief 카메라의 조작 타입을 정의하는 열거형 변수 */
enum TypeTimerEvent
{ 
	TIMER_EVENT_CAMERA_TRANSIT = 0,
};
/*!
*	\class GScene
*	\brief 3차원 장면을 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu) 
*	\date 02 Sept. 2012	
*/
class GScene
{
	// 프렌드 함수
	friend void GetPreOrderMeshList(GScene *pScene, std::vector<GMesh *> &ChildMeshList, GMesh *ParentMesh);
	friend GMesh *FindMesh(std::vector<GMesh *> &MeshList, const std::string &Name);
	friend void CALLBACK theTimer(HWND hWnd, UINT uMsg, UINT wParam, DWORD lParam);
	
public:
	// 생성자 및 소멸자
	GScene(const std::string fname = "noname01.gff");
	GScene(const GScene &cpy);
	virtual ~GScene();

	// 대입 연산자 중복
	GScene &operator =(const GScene &rhs);
		
	// 파생 클래스가 재 정의할 가상함수
	virtual void InitScene();
	virtual void SetupViewport(int x, int y);
	virtual void SetupViewFrustum();
	virtual void SetupViewTransform();
	virtual void AdjustViewFrustum(const GBndBox &Box);	
	virtual void Render();
	virtual void RenderFloor();
	virtual void LButtonDown(int x, int y);
	virtual void LButtonUp(int x, int y);
	virtual void MButtonDown(int x, int y);
	virtual void MButtonUp(int x, int y);
	virtual void MouseMove(int x, int y);
	virtual void MouseWheel(int delta);
	virtual GTransf GetWC();

	// 장면에 객체를 추가/삭제 하기 위한 멤버함수
	void AddMesh(GMesh *pMesh);
	void DelMesh(std::vector<GMesh *> &MeshList, std::string Name);
	void AddLight(const GLight &Light);
	void AddCamera(const GCamera &Cam);

	// 장면의 애니메이션을 위한 멤버함수
	void SetTimer(UINT nIDEvent, UINT uElapse);
	void KillTimer(UINT nIDEvent);
	void AnimateScene();

public:
	// 데이터 멤버
	/*! \brief 경로를 포함한 장면의 파일명 */
	std::string FileName;

	/*! \brief 응용프로그램이 시작된 최초 경로 */
	std::string RootDir;

	/*! \brief 응용 프로그램의 뷰 윈도우의 핸들 */
	HWND hWndView;

	/*! \brief 장면에 포함된 메쉬 리스트 */
	std::vector<GMesh *> MeshList;

	/*! \brief 가장 최근에 추가된 메쉬 */
	GMesh *pRecentMesh;

	/*! \brief 장면에 포함된 광원의 리스트 */
	std::vector<GLight> LightList;

	/*! \brief 장면의 주 카메라 */
	GCamera MainCamera;

	/*! \brief 장면의 시점 전환을 위한 소스 카메라 */
	GCamera SrcCamera;

	/*! \brief 장면의 시점 전환을 위한 타겟 카메라 */
	GCamera TarCamera;

	/*! \brief 장면의 보조 카메라 리스트 */
	std::vector<GCamera> AuxCameraList;

	/*! \brief 장면에 포함된 Head-up Display */
	GHud HeadUpDisplay;

	/*! \brief 장면의 바닥 평면의 렌더링 여부 */
	bool bShowFloor;

	/*! \brief 장면의 배경(그래디에이션) 렌더링 여부 */
	bool bShowBkg;

	/*! \brief 장면의 타이머 작동 여부 */
	bool bTimer;

	/*! \brief 메쉬위의 점들의 리스트 */
	std::vector<GVector4> PtsOnMesh;

	/*! \brief UI의 배경 색상 */
	unsigned char UIBkgColor[3];

	/*! \brief UI의 텍스트 생상 */
	unsigned char UItxtColor[3];

	/*! \brief 장면의 넙스 객체에 사용되는 제어점의 크기 */
	double CtrlPtSize;
};

#endif // _GSCENE_H