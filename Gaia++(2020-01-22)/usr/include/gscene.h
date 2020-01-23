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
*	\brief ����� ������ ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu) 
*	\date 02 Sept. 2012	
*/
class GLight
{
public:
	// ������ �� �Ҹ���
	GLight(GLenum _LightNum);
	GLight(const GLight &cpy);
	virtual ~GLight();

	// ���Կ�����
	GLight &operator =(const GLight &rhs);

	// ����Լ�
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
	// ������ ���
	/*! \brief ������ ��ȣ */
	GLenum LightNum;

	/*! \brief ������ ��ġ */
	GLfloat Position[4];

	/*! \brief ������ �ֺ��� ���� */
	GLfloat Ambient[3];

	/*! \brief ������ ���ݻ� ���� */
	GLfloat Diffuse[3];

	/*! \brief ������ ���ݻ� ���� */
	GLfloat Specular[3];

	/*! \brief ������ Ȱ��ȭ ���� */
	bool IsEnabled;

	/*! \brief ���߱��� ���� */
	bool IsSpotLight;

	/*! \brief ���߰����� ��� ������ ���� */
	GLfloat SpotDirection[3];

	/*! \brief ���߰����� ��� ������ ���� */
	GLfloat SpotCutOff;
};

/*! \brief ī�޶��� ���� Ÿ���� �����ϴ� ������ ���� */
enum TypeProjection
{
	PROJECTION_ORTHO = 0,
	PROJECTION_PERS,
};

/*! \brief ī�޶��� ���� Ÿ���� �����ϴ� ������ ���� */
enum TypeCameraManipulate
{ 
	CAMERA_MANIPULATE_NONE = 0,
	CAMERA_MANIPULATE_ROTATE = 1,
	CAMERA_MANIPULATE_PAN,
	CAMERA_MANIPULATE_ZOOM,
};

/*!
*	\class GCamera
*	\brief 3���� ���(GScene)�� �����ϴ� ī�޶� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu) 
*	\date 06 Sept. 2012	
*/
class GCamera
{
public:
	// ������ �� �Ҹ���
	GCamera();
	GCamera(GScene *_pScene, double _Fovy, double _Near, double _Far, int _ViewWidth, int _ViewHeight);
	GCamera(GScene *_pScene, GVector3 Eye, GVector3 Center, GVector3 Up);
	GCamera(GScene *_pScene, double _Left, double _Right, double _Bottom, double _Top);
	GCamera(const GCamera &cpy);
	virtual ~GCamera();

	// ���Կ�����
	GCamera &operator =(const GCamera &rhs);

	// ������ȯ, ��������, �亯ȯ ���� �Լ�
	void SetupViewFrustum();
	void SetupViewTransform();
	void SetupViewport(int w, int h);

	// ���� ��ȯ ���� �Լ�
	void Panning(int x, int y);
	void Rotating(int x, int y);
	void Zooming(int type);
	void SetArcBallType(TypeArcBall type);
	
	// ���콺 �̹�Ʈ ó�� �Լ�
	void LButtonDown(int x, int y);
	void MouseMove(int x, int y);

	// ������ ���� �Լ�
	void Render();

	// ī�޶� ���� ��/��� ���� �Լ�
	void Export(char *fname);
	void Import(char *fname);

	// ���� ó�� ���� �Լ�
	void GetWorldCoord(double wx, double wy, double depth, double *ox, double *oy, double *oz);
	void GetScreenCoord(GVertex *pVert, double *wx, double *wy, double *wz);
	void GetScreenCoord(GCtrlPt *pPt, double *wx, double *wy, double *wz);
	void GetScreenCoord(double ox, double oy, double oz, double *wx, double *wy, double *wz);
	GLine GetOrthoRay(double x, double y);
	GVector3 GetViewDirection();

public:
	// ������ ���
	/*! \brief ī�޶� ���Ե� ��鿡 ���� ������ */
	GScene *pScene;

	/*! \brief ���� ������ ���� ī�޶��� ȭ��(field of view) */
	double Fovy;

	/*! \brief ���� ������ ���� �������� �Ÿ�(>0) */
	double Near;

	/*! \brief ���� ������ ���� �������� �Ÿ�(>0) */
	double Far;

	/*! \brief ����Ʈ ��ȯ�� ���� �ʺ� */
	int ViewWidth;

	/*! \brief ����Ʈ ��ȯ�� ���� ���� */
	int ViewHeight;

	/*! \brief ī�޶� �߽ɿ��� ���� ��ǥ������� �Ÿ�(<0) */
	double ZoomDist;

	/*! \brief ���� ��ǥ���� ȸ�� ���� */
	GQuater Rot;

	/*! \brief ���� ��ǥ���� �̵� ���� */
	GVector3 Pan;

	/*! \brief �� ��ǥ�迡�� ǥ���� ���� ��ǥ�� (WC = ZoomDist * Rot * Pan) */
	GTransf WC;

	/*! \brief ������ ���콺 ��ġ ���� */
	int MousePt[2];

	/*! \brief ī�޶��� ���� Ÿ�� */
	TypeProjection CameraType;

	/*! \brief ī�޶��� ȸ�� ��ũ�� Ÿ�� */
	TypeArcBall ArcBallType;

	/*! \brief ī�޶��� ���� Ÿ�� */
	TypeCameraManipulate CameraManipulateType;

	/*! \brief ī�޶��� �𵨺� ��ȯ ��� */
	double ModelviewMat[16];

	/*! \brief ī�޶��� ���� ��ȯ ��� */
	double ProjectionMat[16];

	/*! \brief ī�޶��� ����Ʈ ��ȯ ���� */
	int Viewport[4];

	/*! \brief ����� ������ ����� �����ϴ� Į�� �迭 */
	unsigned char *pPixelBuf;
};

/*! \brief Head-up Display�� ǥ�� ������ ���� ������ */
#define HUD_VIEW_CUBE	0x0001
#define HUD_FRAME_RATE	0x0002
#define HUD_ORIGIN_AXIS	0x0004

/*!
*	\class GHud
*	\brief 3���� ����� Head-up Display�� ����ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu) 
*	\date 02 Sept. 2012	
*/
class GHud
{
public:
	// ������ �� �Ҹ���
	GHud();
	GHud(const GHud &cpy);
	virtual ~GHud() {}

	// ���Կ�����
	GHud &operator =(const GHud &rhs);

	// ����Լ�
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

	// ����� �ٶ󺸴� ������ ����
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
	// ������ ���
	/*! \brief �������� ���Ե� ��鿡 ���� ������ */
	GScene *pScene;

	/*! \brief �� ť�긦 �������ϱ� ���� �ؽ���� ���� */
	std::vector<GMaterial> TextureList;

	/*! \brief ������ ���������� xy, yz, zx ���� ���������� �Ǵ� */
	bool Parallel;

	/*! \brief ���������� ���� �������� */
	double ViewFrustum[6];

	/*! \brief HUD�� ǥ�� ���� */
	unsigned int DisplayInfo;

	/*! \brief HUD�� ����� �޽��� ���ڿ� */
	std::string Message;
};

/*! \brief ī�޶��� ���� Ÿ���� �����ϴ� ������ ���� */
enum TypeTimerEvent
{ 
	TIMER_EVENT_CAMERA_TRANSIT = 0,
};
/*!
*	\class GScene
*	\brief 3���� ����� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu) 
*	\date 02 Sept. 2012	
*/
class GScene
{
	// ������ �Լ�
	friend void GetPreOrderMeshList(GScene *pScene, std::vector<GMesh *> &ChildMeshList, GMesh *ParentMesh);
	friend GMesh *FindMesh(std::vector<GMesh *> &MeshList, const std::string &Name);
	friend void CALLBACK theTimer(HWND hWnd, UINT uMsg, UINT wParam, DWORD lParam);
	
public:
	// ������ �� �Ҹ���
	GScene(const std::string fname = "noname01.gff");
	GScene(const GScene &cpy);
	virtual ~GScene();

	// ���� ������ �ߺ�
	GScene &operator =(const GScene &rhs);
		
	// �Ļ� Ŭ������ �� ������ �����Լ�
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

	// ��鿡 ��ü�� �߰�/���� �ϱ� ���� ����Լ�
	void AddMesh(GMesh *pMesh);
	void DelMesh(std::vector<GMesh *> &MeshList, std::string Name);
	void AddLight(const GLight &Light);
	void AddCamera(const GCamera &Cam);

	// ����� �ִϸ��̼��� ���� ����Լ�
	void SetTimer(UINT nIDEvent, UINT uElapse);
	void KillTimer(UINT nIDEvent);
	void AnimateScene();

public:
	// ������ ���
	/*! \brief ��θ� ������ ����� ���ϸ� */
	std::string FileName;

	/*! \brief �������α׷��� ���۵� ���� ��� */
	std::string RootDir;

	/*! \brief ���� ���α׷��� �� �������� �ڵ� */
	HWND hWndView;

	/*! \brief ��鿡 ���Ե� �޽� ����Ʈ */
	std::vector<GMesh *> MeshList;

	/*! \brief ���� �ֱٿ� �߰��� �޽� */
	GMesh *pRecentMesh;

	/*! \brief ��鿡 ���Ե� ������ ����Ʈ */
	std::vector<GLight> LightList;

	/*! \brief ����� �� ī�޶� */
	GCamera MainCamera;

	/*! \brief ����� ���� ��ȯ�� ���� �ҽ� ī�޶� */
	GCamera SrcCamera;

	/*! \brief ����� ���� ��ȯ�� ���� Ÿ�� ī�޶� */
	GCamera TarCamera;

	/*! \brief ����� ���� ī�޶� ����Ʈ */
	std::vector<GCamera> AuxCameraList;

	/*! \brief ��鿡 ���Ե� Head-up Display */
	GHud HeadUpDisplay;

	/*! \brief ����� �ٴ� ����� ������ ���� */
	bool bShowFloor;

	/*! \brief ����� ���(�׷����̼�) ������ ���� */
	bool bShowBkg;

	/*! \brief ����� Ÿ�̸� �۵� ���� */
	bool bTimer;

	/*! \brief �޽����� ������ ����Ʈ */
	std::vector<GVector4> PtsOnMesh;

	/*! \brief UI�� ��� ���� */
	unsigned char UIBkgColor[3];

	/*! \brief UI�� �ؽ�Ʈ ���� */
	unsigned char UItxtColor[3];

	/*! \brief ����� �ҽ� ��ü�� ���Ǵ� �������� ũ�� */
	double CtrlPtSize;
};

#endif // _GSCENE_H