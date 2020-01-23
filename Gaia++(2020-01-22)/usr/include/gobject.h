#ifndef _GOBJECT_H
#define _GOBJECT_H

#include "..\usr\include\gobject_nurbs_util.h"
#include "..\usr\include\gobject_nurbs_knot.h"
#include "..\usr\include\gobject_nurbs_symbolic.h"
#include "..\usr\include\gobject_mesh_util.h"
#include "..\usr\include\gobject_bridge_mesh.h"

// 클래스 선언
class GEditObj;
class GVertex;
class GFace;
class GFacePt;
class GRenderer;
class GMesh;
class GCtrlPt;
class GNurbsCrv;
class GNurbsSrf;
class GSweepSrf;
class GUcbsSrf;
class GUcbsSrf1D;
class GUcbsVol;
class GUcbsVol1D;
class GScene;
class GRefBoneInfo;
class GBndBox;
class GTransf;
class PQP_Model;

// 클래스 GEditObj를 상속 받는 클래스의 멤버 함수들 선언 매크로
#define DECLARE_GEDIT_OBJECT(Class) \
public: \
	virtual std::string GetClassName(); \
	virtual Class *CopyObject(); \
	virtual	GMesh *GetRefMesh(); \
	virtual GTransf GetTransf(); \
	virtual void Edit(GTransf EC, GVector3 t = GVector3(), GQuater q = GQuater(), GVector3 s = GVector3(1.0, 1.0, 1.0), GMesh *pParentMesh = NULL); \
	virtual void RenderForEdit(float r, float g, float b);

// 클래스 GEditObj를 상속 받는 클래스의 일부 멤버 함수들 구현 매크로
#define IMPLEMENT_GEDIT_OBJECT(Class) \
	std::string Class::GetClassName() {return std::string(#Class);} \
	Class *Class::CopyObject() {return new Class(*this);}

// 클래스 GMesh를 상속 받는 클래스의 멤버 함수들 선언 매크로
#define DECLARE_GMESH_OBJECT(Class) \
public: \
	virtual std::string GetClassName(); \
	virtual Class *CopyObject(); \
	virtual	GMesh *GetRefMesh(); \
	virtual GTransf GetTransf(); \
	virtual void Edit(GTransf EC, GVector3 t = GVector3(), GQuater q = GQuater(), GVector3 s = GVector3(1.0, 1.0, 1.0), GMesh *pParentMesh = NULL); \
	virtual void UpdateMesh(); \
	virtual void UpdateMC(int type); \
	virtual void RenderForEdit(float r, float g, float b); \
	virtual void Render(bool bSubMesh = true); \
	virtual void CreateMesh(); \
	virtual GVector4 Eval(double u, ...); \
	virtual void GetDomain(double *umin, ...); \
	virtual void GetCtrlPtList(std::vector<GCtrlPt *> &CtrlPtList);

// 클래스 GMesh를 상속 받는 클래스의 일부 멤버 함수들 구현 매크로
#define IMPLEMENT_GMESH_OBJECT(Class) \
	std::string Class::GetClassName() {return std::string(#Class);} \
	Class *Class::CopyObject() {return new Class(*this);}

/*!
*	\class GEditObj
*	\brief 편집 가능한 객체의 기본 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 01 Jan 2001	
*/
class GEditObj
{
	DECLARE_GEDIT_OBJECT(GEditObj)
public:
	// 생성자 및 소멸자
	GEditObj() {};
	virtual ~GEditObj() {};
};

/*!
*	\class GVertex
*	\brief 메쉬의 정점을 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu) 
*	\date 01 Jan 2001	
*/
class GVertex: public GEditObj
{
	DECLARE_GEDIT_OBJECT(GVertex)
	// 프렌드 함수	
	friend bool operator ==(const GVertex &lhs, const GVertex &rhs);

public:
	// 생성자 및 소멸자
	GVertex(GPoint3 _P = GPoint3(), GVector3 _N = GVector3(), GVector3 _C = GVector3(0.5, 0.5, 0.5));
	GVertex(const GVertex &cpy);
	virtual ~GVertex();

	// 대입 연산자 중복
	GVertex &operator =(const GVertex &rhs);

	// 멤버 함수
	void AddRefBoneInfo(std::string &BoneMeshName, double Wgt);

public:
	// 데이터 멤버(7)
	/*! \brief 정점이 속한 메쉬의 포인터 */
	GMesh *pMesh;

	/*! \brief 정점의 인덱스 (0 부터 시작) */
	int Idx;

	/*! \brief 정점의 3차원 좌표 */
	GPoint3 P;

	/*! \brief 정점의 법선 벡터 */
	GVector3 N;

	/*! \brief 정점의 색상 */
	GVector3 C;

	/*! \brief 정점의 편집 가중치 */
	double EditWgt;

	/*! \brief 정점이 참조하는 본(bone) 메쉬에 대한 정보 리스트 */
	std::vector<GRefBoneInfo> BoneInfoList;
};

/*!
*	\class GFace
*	\brief 메쉬의 삼각형 정보를 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu) 
*	\date 21 July 2008	
*/
class GFace: public GEditObj
{
	DECLARE_GEDIT_OBJECT(GFace)
public:
	// 생성자 및 소멸자
	GFace(int vidx0 = -1, int vidx1 = -1, int vidx2 = -1, int tidx0 = -1, int tidx1 = -1, int tidx2 = -1, int mtl_idx = -1);
	GFace(const GFace &cpy);
	virtual ~GFace();

	// 대입 연산자 중복
	GFace &operator =(const GFace &rhs);

	// 멤버 함수
	int GetPrevVertIdx(int vidx);
	int GetNextVertIdx(int vidx);
	int GetSharedVertIdx(GFace *pSharedFace, int &vidx0, int &vidx1);
	GPoint3 GetCOG(bool bLocal);
	GVector3 GetNormal(bool bLocal);

public:
	// 데이터 멤버(5)
	/*! \brief 삼각형이 속한 메쉬의 포인터 */
	GMesh *pMesh;

	/*! \brief 삼각형의 인덱스(0 부터 시작) */
	int Idx;

	/*! \brief 삼각형을 구성하는 정점의 인덱스 */
	int vIdx[3];

	/*! \brief 삼각형의 정점들이 참조하는 텍스처 좌표의 인덱스 */
	int tIdx[3];

	/*! \brief 삼각형에 적용되는 재질의 인덱스 */
	int MtlIdx;
};

/*!
*	\class GPtOnFace
*	\brief 삼각형 내부의 점을 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu) 
*	\date 8 December 2015	
*/
class GPtOnFace: public GEditObj
{
	DECLARE_GEDIT_OBJECT(GPtOnFace)
public:
	// 생성자 및 소멸자
	GPtOnFace(GFace *_pFace = NULL, GVector3 _Coords = GVector3());
	GPtOnFace(const GPtOnFace &cpy);
	virtual ~GPtOnFace();

	// 대입 연산자 중복
	GPtOnFace &operator =(const GPtOnFace &rhs);

	// 멤버 함수
	void Set(GFace *_pFace, GVector3 _Coords);

public:
	// 데이터 멤버(2)
	/*! \brief 선택된 점이 속한 삼각형의 포인터 */
	GFace *pFace;

	/*! \brief 삼각형의 세 정점을 기준으로 표현된 선택 점의 barycentric 좌표, P = V0 * C0 + V1 * C1 + V2 * C3 */
	GVector3 Coords;
};

/*! \brief 메쉬의 렌더링 타입 열거형 */
#define RENDER_POLYGON		0x0001
#define RENDER_WIRE			0x0002
#define RENDER_POINT		0x0004
#define RENDER_NORMAL		0x0008
#define RENDER_CTRL_PT		0x0010
#define RENDER_BOUNDARY		0X0020
#define RENDER_SWEEP_PLANE	0x0040

/*! \brief 메쉬의 쉐이딩 타입 열거형 */
enum TypeShade
{
	SHADE_SMOOTH = 0,
	SHADE_FLAT,
};

/*! \brief 메쉬 정점을 색상을 결정하는 타입 열거형 */
enum TypeVertexColor
{
	VERTEX_COLOR_ILLUMINATION,
	VERTEX_COLOR_NORMAL,
	VERTEX_COLOR_BONE_WEIGHT,
	VERTEX_COLOR_CURVATURE,
};

/*!
*	\class GRenderer
*	\brief 메쉬의 렌더링 속성을 정의하는 클래스
*	\note 마지막 수정일(20 Aug. 2016)
*
*	\author 윤승현(shyun@dongguk.edu) 
*	\date 02 Sept. 2012	
*/
class GRenderer
{
public:
	// 생성자 및 소멸자
	GRenderer();
	GRenderer(const GRenderer &cpy);
	virtual ~GRenderer();

	// 대입 연산자 중복
	GRenderer &operator =(const GRenderer &rhs);

	// 멤버함수
	bool IsVisibleMesh();
	void SetSampleNum(int NumU, int NumV, int NumW = 0);
	
public:
	/*! \brief 메쉬의 렌더링 타입: RENDER_POLYGON, RENDER_WIRE, ... */
	unsigned int RenderType;

	/*! \brief 메쉬의 쉐이딩 타입: SHADE_SMOOTH, SHADE_FLAT */
	TypeShade ShadeType;

	/*! \brief 정점의 색상 타입 */
	TypeVertexColor VertColorType;

	/*! \brief 뒷면 제거 여부 */
	bool bBackFaceCull;

	/*! \brief 텍스처 적용 여부 */
	bool bTexture;

	// 곡선/곡면/볼륨의 렌더링 속성을 표현하는 데이터 멤버
public:	
	/*! \brief 곡선/곡면/볼륨의 (U-방향) 샘플링 수 */
	int NumSampU;

	/*! \brief 곡선/곡면/볼륨의 (V-방향) 샘플링 수 */
	int NumSampV;

	/*! \brief 볼륨의 W-방향 샘플링 수 */
	int NumSampW;

	/*! \brief 곡선을 선으로 렌더링하는 여부 */
	bool bRenderCrvAsLine;

	/*! \brief 넙스 곡선/곡면/볼륨의 제어점의 크기 */
	double CtrlPtSize;

	/*! \brief 보간 곡선/곡면의 보간 정점 */
	std::vector<GVector3> NodePts;

	/*! \brief 볼륨의 표면 렌더링을 위한 파라미터 값 */
	double IsoSrfParam[6];

	/*! \brief 스윕 평면의 너비 */
	double SwpPlnWidth;

	/*! \brief 스윕 평면의 높이 */
	double SwpPlnHeight;

	/*! \brief GNurbsCrv, GBlendCrv, GBiArcCrv의 경우에만 사용되는 재질의 인덱스 */
	int CrvMtlIdx;
};

/*!
*	\class GMaterial
*	\brief 메쉬의 재질(material)을 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 21 July 2008	
*/
class GMaterial
{
public:
	// 생성자 및 소멸자
	GMaterial(const std::string &_Name = "mtl_default");
	GMaterial(const GMaterial &cpy);
	virtual ~GMaterial();

	// 연산자 중복
	GMaterial &operator =(const GMaterial &rhs);

	// 멤버함수
	void SetAmbient(GLfloat r, GLfloat g, GLfloat b);
	void SetDiffuse(GLfloat r, GLfloat g, GLfloat b);
	void SetSpecular(GLfloat r, GLfloat g, GLfloat b);
	void SetShiness(GLfloat shiness);
	void SetAlpha(GLfloat alpha);
	void LoadTexture(const char *fname);
	void Enable(bool bTexture);
	void Disable();

public:
	/*! \brief 재질의 이름 */	
	std::string Name;

	/*! \brief 재질의 주변광 반사계수 */
	GLfloat Ambient[4];

	/*! \brief 재질의 난반사 계수 */
	GLfloat Diffuse[4];

	/*! \brief 재질의 전반사 계수 */
	GLfloat Specular[4];

	/*! \brief 재질의 전반사 지수 */
	GLfloat Shiness;

	/*! \brief 재질의 텍스처 파일명 */	
	std::string TexFileName;

	/*! \brief 재질의 텍스처 아이디 */
	GLuint BindTexId;
};

/*!
*	\class GBndBox
*	\brief 3차원 장면에서 월드 좌표계의 축에 평행한 바운딩 박스를 표현하는 클래스
*	\note 마지막 수정일(21 Aug. 2016)
*
*	\author 윤승현(shyun@dongguk.edu) 
*	\date 29 Jan. 2015
*/
class GBndBox
{
public:
	// 생성자 및 소멸자
	GBndBox(double minx = 0, double miny = 0, double minz = 0, double maxx = 0, double maxy = 0, double maxz = 0);
	GBndBox(const GBndBox &cpy);
	~GBndBox() {}

	// 대입 연산자 
	GBndBox &operator =(const GBndBox &rhs);

	// 멤버 함수
	GTransf GetMC();
	double GetWidth();
	double GetHeight();
	double GetDepth();
	double GetDiagonal();
	void GetCornerPts(GPoint3 *Pt, bool bLocal = true);
	
	// 렌더링 관련 함수
	void Render(double r, double g, double b, double width);

public:
	// 데이터 멤버
	/*! \brief 경계 상자의 대상 메쉬 */
	GMesh *pMesh;

	/*! \brief 경계 상자의 최소점 (메쉬의 모델링 좌표계에서 표현) */
	GPoint3 MinPt;

	/*! \brief 경계 상자의 최대점 (메쉬의 모델링 좌표계에서 표현) */
	GPoint3 MaxPt;	
};

/*! \brief 메쉬를 구성하는 삼각형의 배열 위치에 따른 메쉬 타입 */
enum TypeMeshFace
{
	FACE_TRI = 0,	// 규칙성이 없는 삼각형 배열
	FACE_QUAD,		// 짝수, 홀수번째 삼각형이 인접해 있음, 일반적인 곡면에 해당됨
	FACE_TRI_FAN,	// 삼각형이 순서대로 인접해 있음
};

/*! \brief 메쉬의 시멘틱 타입을 나타내는 열거형 */
enum TypeMeshSemantic
{
	MESH_STD = 0,		// 표준(STANDARD) 메쉬
	MESH_BONE,			// 본(bone)으로 사용되는 메쉬
	MESH_SKIN,			// 본에 바인딩된 정보를 갖고, 스킨으로 사용되는 메쉬
	MESH_CTRL,			// 블렌딩 메쉬의 제어 메쉬
	MESH_KEY_CRV,		// 스윕곡면을 생성하는 키 곡선 메쉬
	MESH_KEY_FRAME,		// 넙스모션을 생성하기 키 메쉬
	MESH_PIVOT,			// 바인딩 참조 평면의 기준점 메쉬
};

/*! \brief 메쉬의 변경 정보를 나타내는 열거형 */
enum TypeModified
{
	MODIFIED_NONE = 0,		// 변경이 없음
	MODIFIED_VERT_POS,		// 메쉬의 일부 정점의 위치가 편집되었음
	MODIFIED_VERT_POS_ALL,	// 메쉬의 모든 정점의 위치가 편집되었음
	MODIFIED_VERT_NUM,		// 메쉬의 정점의 개수가 변경되었음(삭제, 분할 등)
	MODIFIED_FACE_POS,		// 메쉬의 일부 삼각형의 위치가 편집되었음
	MODIFIED_NORMAL,		// 메쉬의 법선이 변경되었음
	MODIFIED_MTL,			// 메쉬의 재질이 변경되었음
	MODIFIED_CTLPT_POS,		// 넙스 제어점의 위치가 변경되었음
	MODIFIED_INTPT_POS,		// 넙스 보간점의 위치가 변경되었음
	MODIFIED_SAMPLE_NUM,	// 넙스 객체의 샘플링 수가 변경되었음
	MODIFIED_CTLPT_SIZE,	// 넙스 객체의 제어점의 크기기 변경되었음
	MODIFIED_REF_BONE,		// 참조 본 메쉬가 변경되었음
};

/*!
*	\class GMesh
*	\brief 계층적 구조를 갖는 메쉬 데이터를 표현하는 클래스
*	\note 마지막 수정일(21 Aug. 2016)
*
*	\author 윤승현(shyun@dongguk.edu) 
*	\date 01 Jan 2001	
*/
class GMesh: public GEditObj
{
	DECLARE_GMESH_OBJECT(GMesh)
public:
	// 생성자 및 소멸자
	GMesh(const std::string &_Name, GTransf _MC, TypeMeshFace _FaceType, const std::string &_ParentMeshName = "");
	GMesh(const GMesh &cpy);
	virtual ~GMesh();

	// 대입 연산자
	GMesh &operator =(const GMesh &rhs);

	// 메쉬의 기하 정보를 추가/삭제하는 함수
	void AddVertex(GVertex v);
	void AddFace(GFace f, std::string GroupName = "");
	void AddTexel(GVector2 t);
	void AddMtl(GMaterial mtl);
	void AddClipPlanes();
	void DelFaces(std::vector<int> &FaceList);
	void DelVerts(std::vector<int> &VertList);

	// 메쉬의 기하 정보를 얻는 함수
	int GetNumVert();
	int GetNumFace();
	int GetNumTex();
	int GetNumMtl();
	int GetVertIdx(GVertex *pVert);
	int GetFaceIdx(GFace *pFace);
	int GetTexIdx(GVertex *pVert);
	int GetMtlIdx(std::string MtlName);
	GMaterial *FindMtl(std::string MtlName);
	void SetMtl(std::string MtlName);
	void SetNewFaces(std::vector<GFace> &NewFaces);
	GFace *FindFace(int sidx, int eidx);
			
	// 메쉬의 법선과 경계 상자 갱신 함수
	void UpdateNormal(bool all);
	void UpdateBndBox(bool all);
	void ReverseNormal();

	// 본 애니메이션과 관련된 함수
	void InitRefBoneInfo();
	void Animate(GMesh *pParent);
	void ApplySSD();

	// 메쉬의 렌더링 버퍼를 갱신하는 함수
	void UpdateBufVertex();
	void UpdateBufNormal();
	void UpdateBufFace();

	// 메쉬를 렌더링 하는 멤버함수
	void RenderMeshPolygon();
	void RenderMeshTexture();
	void RenderMeshWire(float r, float g, float b, float width);
	void RenderMeshPoint(float r, float g, float b, float size);
	void RenderMeshBoundary(float r, float g, float b, float width);
	void InitBndryEdgeList();

	// 메쉬 편집 함수
	void Normalize();
	void ShowMesh(bool bSubMesh);
	void HideMesh(bool bSubMesh);

	// 바운딩 볼률 계층 구조 생성 함수
	void CreateBVH();
	void DeleteBVH();

public:
	// 기본 데이터 멤버(19)
	/*! \brief 메쉬의 이름 */
	std::string Name;

	/*! \brief 메쉬가 정의된 모델링 좌표계 */
	GTransf MC;

	/*! \brief 메쉬의 크기 변환 정보 */
	GVector3 S;

	/*! \brief 메쉬의 폴리곤 타입(삼각형, 사각형) 정보 */
	TypeMeshFace FaceType;

	/*! \brief 메쉬를 구성하는 정점의 배열 */
	std::vector<GVertex> Verts;

	/*! \brief 메쉬를 구성하는 텍스처 정점의 배열 */
	std::vector<GVector2> Texs;

	/*! \brief 메쉬를 구성하는 삼각형의 배열 */
	std::vector<GFace> Faces;

	/*! \brief 메쉬의 그룹핑된 삼각형 인덱스 배열 */
	std::map<std::string, std::vector<int>> GrpFaces;

	/*! \brief 메쉬를 둘러싸는 경계 상자 */
	GBndBox BBox;

	/*! \brief 메쉬의 바운딩 볼률 계층 구조 */
	PQP_Model *pBVH;

	/*! \brief 충돌이 생긴 삼각형의 인덱스 리스트 */
	std::vector<int> CollisionFaces;

	/*! \brief 메쉬의 렌더링 속성 */
	GRenderer Renderer;

	/*! \brief 메쉬가 사용하는 재질 리스트 */
	std::vector<GMaterial> MtlList;

	/*! \brief 텍스처 모델을 위한 디스플레이 리스트 */
	GLuint DispTexMesh;

	/*! \brief 정점 칼라 렌더링을 위한 디스플레이 리스트 */
	GLuint DispVertColMesh;

	/*! \brief 삼각형 컬러 렌더링을 위한 디스플레이 리스트 */
	GLuint DispFaceColMesh;	

	/*! \brief 렌더링을 속도를 향상을 위한 버퍼 */
	std::vector<float> VtxBuf;
	std::vector<float> NorBuf;
	std::map<int, std::vector<int>> FaceIdxBuf;

public:
	// 추가 데이터 멤버(11)
	/*! \brief 메쉬가 포함된 장면에 대한 포인터 */
	GScene *pScene;

	/*! \brief 메쉬와 연결된 부모 메쉬의 이름 */
	std::string ParentMeshName;

	/*! \brief 부모 메쉬로부터의 변환 (MC = 부모 MC * DelX) */
	GTransf DelX;

	/*! \brief 메쉬와 연결된 자식 메쉬 리스트 */
	std::vector<GMesh *> SubMeshList;

	/*! \brief 메쉬의 시멘틱 타입 정보 */
	TypeMeshSemantic SemanticType;
	
	/*! \brief 메쉬의 시멘틱 타입에 따라 연결된 메쉬의 이름 리스트 */
	std::vector<std::string> LinkedMeshNameList;

	/*! \brief 메쉬의 절단 평면의 이름 리스트 */
	std::vector<std::string> ClipPlaneNameList;

	/*! \brief 메쉬의 편집된 정보를 기록하는 변수 */
	TypeModified ModifiedFlag;
	
	/*! \brief 메쉬의 경계 모서리 리스트 */
	std::map<int, int> BndryEdgeList;

	/*! \brief 애니메이션을 위한 변환 정보 */
	std::vector<GTransf> Animation;

	/*! \brief 애니메이션을 위한 프레임 인덱스 */
	int FrameIdx;
};

/*!
*	\class GRefBoneInfo
*	\brief 본 메쉬에 대한 정점의 바인딩 정보를 표현하는 클래스
*	\note 마지막 수정일(20 Aug. 2016)
*
*	\author 윤승현(shyun@dongguk.edu) 
*	\date 01 Jan 2001	
*/
class GRefBoneInfo
{
public:
	// 생성자 및 소멸자
	GRefBoneInfo();
	GRefBoneInfo(std::string &MeshName, double _Weight, GPoint3 &_P, GVector3 &_N);
	GRefBoneInfo(const GRefBoneInfo &cpy);
	~GRefBoneInfo();

	// 대입 연산자
	GRefBoneInfo &operator =(const GRefBoneInfo &rhs);

public:
	// 데이터 멤버(5)
	/*! \brief 정점이 바인딩된 본 메쉬의 이름 */
	std::string BoneMeshName;

	/*! \brief 정점이 바인딩된 본 메쉬의 모델링 좌표계(MC)에 대한 포인터 */
	GTransf *pRefMC;

	/*! \brief 본 메쉬의 모델링 좌표계에 대한 정점의 변형 가중치 */
	double Wgt;

	/*! \brief pRefMC에서 표현된 정점의 좌표 */
	GPoint3 P;

	/*! \brief pRefMC에서 표현된 법선의 좌표 */
	GVector3 N;
};

/*!
*	\class GPathInfo
*	\brief 최단 경로의 정점들이 삼각형 모서리와 교차하는 정보를 표현하는 클래스
*	\note 마지막 수정일(22 Aug. 2016)
*
*	\author 윤승현(shyun@dongguk.edu) 
*	\date 09 Mar. 2016
*/
class GPathInfo
{
public:
	// 생성자 및 소멸자
	GPathInfo(int _sidx, int _eidx, double _t);
	GPathInfo(const GPathInfo &cpy);
	~GPathInfo();

	// 대입 연산자
	GPathInfo &operator =(const GPathInfo &rhs);

public:
	// 데이터 멤버
	/*! \brief 경로와 교차하는 선분 정점의 시작 인덱스 */
	int sidx;

	/*! \brief 경로와 교차하는 선분 정점의 끝 인덱스 */
	int eidx;

	/*! \brief 교차점의 매개변수 */
	double t;
};

/*!
*	\class GGeoPath
*	\brief 메쉬의 정점들을 연결하는 측지 경로를 표현하는 클래스
*	\note 마지막 수정일(22 Aug. 2016)
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 11 March, 2016
*/
class GGeoPath: public GMesh
{
	DECLARE_GMESH_OBJECT(GGeoPath)
public:
	// 생성자 및 소멸자
	GGeoPath();
	GGeoPath(std::string _Name, GTransf _MC, GMesh *_pMesh, std::string _ParentName);
	GGeoPath(const GGeoPath &rhs);
	virtual ~GGeoPath();

	// 대입 연산자
	GGeoPath &operator =(const GGeoPath &rhs);

	// 메쉬 생성 함수
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// 경로 추가 함수
	void AddPath(int ToVertIdx);
	void DelPath(int VertIdx);
	void EditPath(int ToVertIdx);

	// 멤버 함수
	int GetStartVertIdx();
	int GetEndVertIdx();

public:
	/*! \brief 측지 경로가 정의되는 메쉬에 대한 포인터 */
	GMesh *pRefMesh;

	/*! \brief 경로 정점의 인덱스 리스트 */
	std::vector<int> PathVertIdx;

	/*! \brief 각각의 경로를 구성하는 교차점의 리스트 */
	std::vector<std::vector<GPoint3>> Paths;

	/*! \brief 현재 편집 중인 경로 정점의 인덱스 */
	int EditPathVertIdx;
};

/*!
*	\class GCutPt
*	\brief 메쉬의 절단선을 구성하는 삼각형 모서리위의 점을 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 05 Sep., 2015	
*/
class GCutPt: public GEditObj
{
	DECLARE_GEDIT_OBJECT(GCutPt);
public:
	// 생성자 및 소멸자
	GCutPt();
	GCutPt(GFace *_pFace, int vidx0, int vidx1, double t, GPoint3 _P);
	GCutPt(const GCutPt &cpy);
	virtual ~GCutPt();

	// 대입연산자
	GCutPt &operator =(const GCutPt &rhs);

public:
	/*! \brief 점이 놓여 있는 모서리를 포함한 삼각형 객체 */
	GFace *pFace;

	/*! \brief 점이 놓여 있는 모서리의 정점 인덱스: 0: 시작 인덱스, 1: 끝 인덱사, 2: 나머지 인덱스 */
	int vIdx[3];

	/*! \brief 모서리에서 점의 파라미터 */
	double t;

	/*! \brief 점의 3차원 좌표 */
	GPoint3 P;
};

/*!
*	\class GCtrlPt
*	\brief B-spline 객체의 제어점을 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 25 Jan, 2015	
*/
class GCtrlPt: public GEditObj, public GVector4
{
	DECLARE_GEDIT_OBJECT(GCtrlPt);
	// 일반 연산자
	friend GCtrlPt operator +(const GCtrlPt &lhs, const GCtrlPt &rhs);
	friend GCtrlPt operator +(const GVector4 &lhs, const GCtrlPt &rhs);
	friend GCtrlPt operator +(const GCtrlPt &lhs, const GVector4 &rhs);

public:
	// 생성자 및 소멸자
	GCtrlPt();
	GCtrlPt(GVector4 _P);
	GCtrlPt(const GCtrlPt &cpy);
	virtual ~GCtrlPt();

	// 대입연산자
	GCtrlPt &operator =(const GVector4 &rhs);
	GCtrlPt &operator =(const GCtrlPt &rhs);	

public:
	/*! \brief 제어점 속한 B-spline 객체 */
	GMesh *pMesh;
};

/*!
*	\class GNurbsCrv
*	\brief Non-Uniform Rational B-spline 곡선을 나타내는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 01 Jan, 2001	
*/
class GNurbsCrv: public GMesh
{
	DECLARE_GMESH_OBJECT(GNurbsCrv)
public:
	// 생성자 및 소멸자
	void _alloc(int _p, int _n);
	void _free();
	GNurbsCrv();
	GNurbsCrv(std::string _Name, GTransf _MC, int _p, int _n, double *_U, GVector4 *_P, bool _rational = false, std::string _ParentName = "", std::string _RefMeshName = "");
	GNurbsCrv(std::string _Name, GTransf _MC, int _p, int _n, double *_U, GCtrlPt *_P, bool _rational = false, std::string _ParentName = "", std::string _RefMeshName = "");
	GNurbsCrv(const GNurbsCrv &cpy);
	virtual ~GNurbsCrv();

	// 대입 연산자
	GNurbsCrv &operator =(const GNurbsCrv &rhs);

	// 메쉬 생성함수
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// 계산 함수들
	GVector4 EvalDeriv(double u, int nth);

	// 기하 연산 함수들
	void KnotClamped();
	void KnotUnclamped();
	int	 KnotInsert(double u, int r);
	int  KnotRemove(double u, int r);
	void KnotRefine();
	void ElevateDeg();
	void Reverse();
	void Scale(double alpha);
	void CnvrtBzrForm();
	void CnvrtBspForm();
	
	// 곡선 편집 함수들
	void Edit(int idx, GVector4 dp);
	void Edit(double u, GVector4 dp);
	void Edit(double u, GVector4 dp, int support);
	GVector4 GetCtrlPt(int idx);

public:
	/*! \brief 넙스 곡선이 정의된 메쉬의 이름 */
	std::string RefMeshName;

	/*! \brief 넙스 곡선의 차수 */
	int p;

	/*! \brief 넙스 곡선의 제어점의 마지막 인덱스 */
	int n;

	/*! \brief 넙스 곡선의 낫 벡터 */
	double *U;

	/*! \brief 넙스 곡선의 제어점 배열 */
	GCtrlPt *P;

	/*! \brief 유리식 곡선 여부 */
	bool rational;

	/*! \brief \a (p - 1)차 연속성을 갖는 폐곡선 여부 */
	bool closed;
};

/*!
*	\class GNurbsSrf
*	\brief 3차원 공간상의 넙스곡면을 나타내는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 01 Jan, 2001	
*/
class GNurbsSrf: public GMesh
{
	DECLARE_GMESH_OBJECT(GNurbsSrf)
public:
	// 생성자 및 소멸자
	void _alloc(int _p, int _m, int _q, int _n);
	void _free();
	GNurbsSrf();
	GNurbsSrf(std::string _Name, GTransf _MC, int _p, int _m, double *_U, int _q, int _n, double *_V, GVector4 **_P, bool _rational = false, std::string _ParentName = "");
	GNurbsSrf(std::string _Name, GTransf _MC, int _p, int _m, double *_U, int _q, int _n, double *_V, GVector4 *_P, bool _rational = false, std::string _ParentName = "");
	GNurbsSrf(const GNurbsSrf &cpy);
	virtual ~GNurbsSrf();

	// 대입 연산자
	GNurbsSrf &operator =(const GNurbsSrf &rhs);

	// 메쉬 생성함수
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// Evaluation 함수들
	GVector4 EvalDeriv(double u, double v, int kth, int lth);
	void EvalTanPln(double u, double v, GVector3 &Su, GVector3 &Sv, GVector3 &N);

	// Geometric 연산 함수들
	void KnotClamped(bool dir_u, bool dir_v);
	void KnotUnclamped(bool dir_u, bool dir_v);
	int KnotInsert(int dirType, double val, int r);
	int KnotRemove(int dirType, double val, int r);
	void KnotRefine(bool dir_u, bool dir_v);
	void ElevateDeg(bool dir_u, bool dir_v);
	void CnvrtBzrForm(bool dir_u, bool dir_v);
	void CnvrtBspForm(bool dir_u, bool dir_v);

	// 곡면 편집 함수들
	void Edit(int uidx, int vidx, GVector4 dp);
	void Edit(double u, double v, GVector4 dp);
	void Edit(double u, double v, GVector4 dp, int support_u, int support_v);
	void SetZeros();
	GVector4 GetCtrlPt(int idx);

public:
	/*! \brief 넙스 곡면의 U-방향 차수 */
	int p;

	/*! \brief 넙스 곡면의 U-방향 제어점의 마지막 인덱스 */
	int m;

	/*! \brief 넙스 곡면의 U-방향 낫 벡터 */
	double *U;

	/*! \brief 넙스 곡면의 V-방향 차수 */
	int q;

	/*! \brief 넙스 곡면의 V-방향 제어점의 마지막 인덱스 */
	int n;

	/*! \brief 넙스 곡면의 V-방향 낫 벡터 */
	double *V;

	/*! \brief 넙스 곡면의 제어점 배열 */
	GCtrlPt **P;

	/*! \brief 유리식 곡선 여부 */
	bool rational;

	/*! \brief \a (p - 1)차 연속성을 갖는 U-방향 폐곡선 여부 */
	bool closed_u;

	/*! \brief \a (q - 1)차 연속성을 갖는 V-방향 폐곡선 여부 */
	bool closed_v;
};

/*!
*	\class GSweepSrf
*	\brief 단면 곡선을 보간하여 생성되는 스윕곡면을 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 20 May, 2013	
*/
class GSweepSrf: public GMesh
{
	DECLARE_GMESH_OBJECT(GSweepSrf)
public:
	// 생성자 및 소멸자
	GSweepSrf(std::string _Name, GTransf _MC, std::vector<GNurbsCrv *> _KeyCrvList, int _DegT, int _DegR, std::string _ParentName);
	GSweepSrf(const GSweepSrf &cpy);
	virtual ~GSweepSrf();

	// 대입연사자
	GSweepSrf &operator =(const GSweepSrf &rhs);

	// 스윕 평면을 계산하는 함수
	GPlane EvalPlane(double t);

	// 메쉬 생성함수
	void CreateComponent();
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

public:
	// 데이터 멤버
	/*! \brief 스윕 곡면을 생성하기 위한 키곡선 리스트 */
	std::vector<GNurbsCrv *> KeyCrvList;

	/*! \brief 스윕 곡면의 궤적 성분 곡선의 차수 */
	int DegT;

	/*! \brief 스윕 곡면의 방향 성분 곡선의 차수 */
	int DegR;

	/*! \brief 스윕 곡면의 궤적 성분 곡선 */
	GNurbsCrv *pTraCrv;	

	/*! \brief 스윕 곡면의 방향 성분 곡선 */
	GNurbsCrv *pRotCrv;
	
	/*! \brief 스윕 곡면의 형상 성분 곡면 */
	GNurbsSrf *pSrcSrf;
};

/*!
*	\class GNurbsMot
*	\brief 키 좌표계를 보간하여 생성되는 연속 모션을 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 23 Feb., 2016	
*/
class GNurbsMot: public GMesh
{
	DECLARE_GMESH_OBJECT(GNurbsMot)
public:
	// 생성자 및 소멸자
	GNurbsMot(std::string _Name, GTransf _MC, std::vector<GMesh *> _KeyMeshList, int _DegT, int _DegR, std::string _ParentName);
	GNurbsMot(const GNurbsMot &cpy);
	virtual ~GNurbsMot();

	// 대입연사자
	GNurbsMot &operator =(const GNurbsMot &rhs);

	// 스윕 평면을 계산하는 함수
	GPlane EvalPlane(double u);
	GTransf EvalFrame(double u);
	int GetNumKeyMesh();

	// 메쉬 생성함수
	void CreateComponent();
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

public:
	// 데이터 멤버
	/*! \brief 넙스 모션을 생성하기 위한 키 좌표계의 리스트 */
	std::vector<GMesh *> KeyMeshList;

	/*! \brief 넙스 모션의 궤적 성분 곡선의 차수 */
	int DegT;

	/*! \brief 넙스 모션의 방향 성분 곡선의 차수 */
	int DegR;

	/*! \brief 넙스 모션의 궤적 성분 곡선 */
	GNurbsCrv *pTraCrv;	

	/*! \brief 넙스 모션의 방향 성분 곡선 */
	GNurbsCrv *pRotCrv;

	/*! \brief 넙스 모션의 스케일 성분 곡면 */
	GNurbsCrv *pSclCrv;
};

/*!
*	\class GBindRefPln
*	\brief Clothes 메쉬 정점의 바인딩 영역을 정의하는 매개화된 평면을 나타내는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 1 July, 2015	
*/
class GBindRefPln: public GMesh
{
	DECLARE_GMESH_OBJECT(GBindRefPln)

public:
	// 생성자 및 소멸자
	GBindRefPln(std::string _Name, GTransf _MC, GMesh *pPivot, std::vector<GMesh *> _KeyBoneList, std::string _ParentName);
	GBindRefPln(const GBindRefPln &cpy);
	virtual ~GBindRefPln();

	// 대입연사자
	GBindRefPln &operator =(const GBindRefPln &rhs);

	// 스윕 평면을 계산하는 함수
	GPlane EvalPlane(double t);

	// 메쉬 생성함수
	void CreateComponent();
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

public:
	/*! \brief 참조 평면을 생성하는 기준점 */
	GMesh *pPivot;

	/*! \brief 참조 평면을 생성하는 키 본(key bone) 메쉬 리스트 */
	std::vector<GMesh *> KeyBoneList;

	/*! \brief 평면 원점의 이동 성분 곡선 */
	GNurbsCrv *pTraCrv;

	/*! \brief 평면 원점의 방향 성분 곡선 */
	GNurbsCrv *pRotCrv;

	/*! \brief 평면의 너비 */
	double PlnWidth;
};

/*!
*	\class GBlendCrv
*	\brief 2차 라그랑주 보간곡선을 블렌딩한 보간 곡선을 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 10 Feb, 2013	
*/
class GBlendCrv: public GMesh
{
	DECLARE_GMESH_OBJECT(GBlendCrv)
public:
	// 생성자 및 소멸자
	GBlendCrv();
	GBlendCrv(std::string _Name, GTransf _MC, GVector4 *_P, int _n, std::string _ParentName = "");
	GBlendCrv(const GBlendCrv &cpy);
	virtual ~GBlendCrv();

	// 대입연산자 중복
	GBlendCrv &operator =(const GBlendCrv &rhs);

	// 메쉬 생성함수
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// 멤버 함수
	void Edit(int idx, GVector4 dp);

public:
	// 데이터 멤버
	/*! \brief 곡선의 보간점 */
	GVector4 *P;

	/*! \brief 곡선의 접선 벡터 */
	GVector4 *T;

	/*! \brief 곡선의 보간점의 마지막 인덱스 */
	int n;

	/*! \brief 곡선의 정의역 */
	double *Param;
};

/*!
*	\class GTriBzrSrf
*	\brief 삼각형 베지에 패치를 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 13 Jan, 2013	
*/
class GTriBzrSrf: public GMesh
{
	DECLARE_GMESH_OBJECT(GTriBzrSrf)
public:
	// 생성자 및 소멸자
	void _alloc(int _p);
	void _free();
	GTriBzrSrf(std::string _Name, GTransf _MC, int _p = 0, GVector4 *_P = NULL, std::string _ParentName = "");
	GTriBzrSrf(const GTriBzrSrf &cpy);
	virtual ~GTriBzrSrf();

	// 대입연산자
	GTriBzrSrf &operator =(const GTriBzrSrf &rhs);

	// 일반 멤버함수
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();
	GTriBzrSrf *SubPatch(double u, double v, double w, int idx);
	GVector3 EvalNormal(double u, double v, double w);

public:
	// 데이터 멤버
	/*! \brief 삼각형 베지에 곡면의 차수 */
	int p;

	/*! \brief 삼각형 베지에 곡면의 제어점 배열의 마지막 인덱스 */
	int n;

	/*! \brief 삼각형 베지에 곡면의 제어점 배열 */
	GCtrlPt *P;
};

/*! \brief 삼각형에서 정의되는 패치의 타입 열거형 */
#define PATCH_TYPE_MODIFIED		0x0001
#define PATCH_TYPE_BZR_TRI		0x0002
#define PATCH_TYPE_BZR_TRI_EXT	0x0004
#define PATCH_TYPE_BZR_TRI_PLN	0x0008

/*!
*	\class GBlendMesh
*	\brief 삼각형 베지에 패치를 블렌딩하여 생성된 곡면을 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 4 March, 2015
*/
class GBlendMesh: public GMesh
{
	DECLARE_GMESH_OBJECT(GBlendMesh)
public:
	// 생성자 및 소멸자
	GBlendMesh(std::string _Name, GTransf _MC, GMesh *_pCtrlMesh);
	GBlendMesh(const GBlendMesh &cpy);
	virtual ~GBlendMesh();

	// 대입연산자
	GBlendMesh &operator =(const GBlendMesh &rhs);

	// 일반 멤버함수
	void CreateLocalPatch();
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();
	
public:
	// 데이터 멤버
	/*! \brief 블렌딩 메쉬의 제어 메쉬 */
	GMesh *pCtrlMesh;

	/*! \brief 제어 메쉬의 각 삼각형에 정의된 삼각형 베지에 패치 */
	std::vector<GTriBzrSrf> PatchList;

	/*! \brief 제어 메쉬의 각 삼각형의 블렌딩 영역 오프셋 */
	std::vector<GVector3> BlendOffset;

	/*! \brief 제어 메쉬의 각 삼각형에 인접한 삼각형의 정보 */
	std::vector<int> AdjFaceListOfFace;

	/*! \brief 제어 메쉬의 각 삼각형의 패치 타입 */
	std::vector<int> PatchType;
};

/*!
*	\class GUcbsSrf1D
*	\brief Uniform cubic B-spline 곡면을 표현하는 클래스
*	\note [0, 1] x [0, 1] --> R 로의 스칼라 함수
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 9 Aug, 2013		
*/
class GUcbsSrf1D: public GMesh
{
	DECLARE_GMESH_OBJECT(GUcbsSrf1D)
public:
	// 생성자 및 소멸자
	void _alloc(int _m, int _n);
	void _free();
	GUcbsSrf1D(std::string _Name, GTransf _MC, int m = 3, int n = 3, double *_P = NULL, std::string _ParentName = "");
	GUcbsSrf1D(const GUcbsSrf1D &cpy);
	virtual ~GUcbsSrf1D();

	// 대입연산자
	GUcbsSrf1D &operator =(const GUcbsSrf1D &rhs);

	// 메쉬 생성함수
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// 멤버함수
	GVector4 EvalFast(double u, ...);
	void Refine();
	void Edit(double u, double v, double df);

public:
	// 데이터 멤버
	/*! \brief U-방향 제어점의 마지막 인덱스 (m >= 3) */
	int m;

	/*! \brief V-방향 제어점의 마지막 인덱스 (n >= 3) */
	int n; 

	/*! \brief 곡면의 제어점 배열, 크기 = (m + 1) * (n + 1) */
	double *P;
};

/*!
*	\class GUcbsSrf
*	\brief Uniform cubic B-spline 곡면을 표현하는 클래스
*	\note [0, 1] x [0, 1] --> R^4로의 함수
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 9 Aug, 2013		
*/
class GUcbsSrf: public GMesh
{
	DECLARE_GMESH_OBJECT(GUcbsSrf)
public:
	// 생성자 및 소멸자
	void _alloc(int _m, int _n);
	void _free();
	GUcbsSrf(std::string _Name, GTransf _MC, int m = 3, int n = 3, GVector4 *_P = NULL, std::string _ParentName = "");
	GUcbsSrf(const GUcbsSrf &cpy);
	virtual ~GUcbsSrf();

	// 대입연산자
	GUcbsSrf &operator =(const GUcbsSrf &rhs);

	// 메쉬 생성함수
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// 멤버함수
	GVector4 EvalFast(double u, ...);
	void EvalTanPln(double u, double v, GVector3 &Su, GVector3 &Sv, GVector3 &N);
	void EvalTanPlnFast(double u, double v, GVector3 &Su, GVector3 &Sv, GVector3 &N);
	void Refine();

public:
	// 데이터 멤버
	/*! \brief U-방향 제어점의 마지막 인덱스 (m >= 3) */
	int m;

	/*! \brief V-방향 제어점의 마지막 인덱스 (n >= 3) */
	int n; 

	/*! \brief 곡면의 제어점 배열, 크기 = (m + 1) * (n + 1) */
	GCtrlPt *P;
};

/*!
*	\class GUcbsVol1D
*	\brief Uniform cubic B-spline 볼륨을 표현하는 클래스
*	\note [0, 1] x [0, 1] x [0, 1] --> R로의 함수
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 5 Nov, 2013		
*/
class GUcbsVol1D: public GMesh
{
	DECLARE_GMESH_OBJECT(GUcbsVol1D)
public:
	// 생성자 및 소멸자
	void _alloc(int _l, int _m, int _n);
	void _free();
	GUcbsVol1D(std::string _Name, GTransf _MC, int l = 3, int m = 3, int n = 3, double *_P = NULL, std::string _ParentName = "");
	GUcbsVol1D(const GUcbsVol1D &cpy);
	virtual ~GUcbsVol1D();

	// 대입연산자
	GUcbsVol1D &operator =(const GUcbsVol1D &rhs);

	// 메쉬 생성함수
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// 멤버함수
	void Refine();

public:
	// 데이터 멤버
	/*! \brief U-방향 제어점의 마지막 인덱스 (l >= 3) */
	int l;

	/*! \brief V-방향 제어점의 마지막 인덱스 (m >= 3) */
	int m; 

	/*! \brief W-방향 제어점의 마지막 인덱스 (n >= 3) */
	int n; 

	/*! \brief 곡면의 제어점 배열, 크기 = (l + 1) * (m + 1) * (n + 1) */
	double *P;
};

/*!
*	\class GUcbsVol
*	\brief Uniform cubic B-spline 볼륨을 표현하는 클래스
*	\note [0, 1] x [0, 1] x [0, 1] --> R^4로의 함수
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 28 Aug, 2013		
*/
class GUcbsVol: public GMesh
{
	DECLARE_GMESH_OBJECT(GUcbsVol)
public:
	// 생성자 및 소멸자
	void _alloc(int _l, int _m, int _n);
	void _free();
	GUcbsVol(std::string _Name, GTransf _MC, int l = 3, int m = 3, int n = 3, GVector4 *_P = NULL, std::string _ParentName = "");
	GUcbsVol(const GUcbsVol &cpy);
	virtual ~GUcbsVol();

	// 대입연산자
	GUcbsVol &operator =(const GUcbsVol &rhs);

	// 메쉬 생성함수
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// 멤버함수
	void Refine();

public:
	// 데이터 멤버
	/*! \brief U-방향 제어점의 마지막 인덱스 (l >= 3) */
	int l;

	/*! \brief V-방향 제어점의 마지막 인덱스 (m >= 3) */
	int m; 

	/*! \brief W-방향 제어점의 마지막 인덱스 (n >= 3) */
	int n; 

	/*! \brief 곡면의 제어점 배열, 크기 = (l + 1) * (m + 1) * (n + 1) */
	GCtrlPt *P;
};

/*!
*	\class GCutLine
*	\brief 메쉬 위에서 정의된 절단선을 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 06 Sept., 2015		
*/
class GCutLine: public GMesh
{
	DECLARE_GMESH_OBJECT(GCutLine)
public:
	// 생성자 및 소멸자
	GCutLine();
	GCutLine(std::string _Name, GTransf _MC, std::vector<GCutPt> &CutPtList, std::string _ParentName = "");
	GCutLine(const GCutLine &cpy);
	virtual ~GCutLine();

	// 대입연산자
	GCutLine &operator =(const GCutLine &rhs);

	// 메쉬 생성함수
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// 멤버함수
	void GetLengthAndKappaSum(float *params, float *len_sum, float *Kappa_sum);
	
public:
	/*! \brief 절단선 정점의 리스트 */
	std::vector<GCutPt> PtList;

	/*! \brief 절단선 닫힘 여부 */
	bool bClosed;
};

/*! \brief Circular 아크의 방향성을 나타내는 열거형 */
enum TypeCirDir
{
	DIR_CW = 0,
	DIR_CCW,
};

/*!
*	\class GCirArc
*	\brief Circular arc segment 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 11 Feb., 2016
*/
class GCirArc
{
public:
	// 생성자 및 소멸자
	GCirArc();
	GCirArc(GPoint3 _Center, double _Radius, double _AngleStart, double _AngleEnd, TypeCirDir _Direction, double _LenTangent);
	GCirArc(const GCirArc &cpy);
	virtual ~GCirArc();

	// 대입 연산자
	GCirArc &operator =(const GCirArc &rhs);

	// 멤버 함수
	GVector4 Eval(double t);
	GVector4 EvalDeriv(double t);

public:
	/*! \brief 아크의 중심 */
	GPoint3 Center;

	/*! \brief 아크의 반지름 */
	double Radius;

	/*! \brief 아크의 시작 각도 */
	double AngleStart;

	/*! \brief 아크의 끝 각도 */
	double AngleEnd;

	/*! \brief 아크의 방향 */
	TypeCirDir Direction;

	/*! \brief 아크의 끝부분 접선의 길이 */
	double LenTangent;
};

/*!
*	\class GBiArcCrv
*	\brief Circular arc들로 연결된 biarc 곡선을 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 11 Feb., 2016
*/
class GBiarcCrv: public GMesh
{
	DECLARE_GMESH_OBJECT(GBiarcCrv)
public:
	// 생성자 및 소멸자
	GBiarcCrv();
	GBiarcCrv(std::string _Name, GTransf _MC, std::vector<GCirArc> &_ArcList, std::string _ParentName);
	GBiarcCrv(const GBiarcCrv &rhs);
	virtual ~GBiarcCrv();

	// 대입 연산자
	GBiarcCrv &operator =(const GBiarcCrv &rhs);

	// 메쉬 생성함수
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

public:
	/*! \brief 곡선을 구성하는 아크의 리스트 */
	std::vector<GCirArc> ArcList;
};

/*!
*	\class GCoonsSrf
*	\brief Coons 곡면을 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 1 April, 2016
*/
class GCoonsSrf: public GMesh
{
	DECLARE_GMESH_OBJECT(GCoonsSrf)
public:
	// 생성자 및 소멸자
	GCoonsSrf();
	GCoonsSrf(std::string _Name, GTransf _MC, int _p, int _m, double *_U, int _q, int _n, double *_V, GVector4 *_P, std::string _ParentName);
	GCoonsSrf(std::string _Name, GTransf _MC, GNurbsCrv *pCrv0, GNurbsCrv *pCrv1, GNurbsCrv *pCrv2, GNurbsCrv *pCrv3, std::string _ParentName);
	GCoonsSrf(const GCoonsSrf &rhs);
	virtual ~GCoonsSrf();

	// 대입 연산자
	GCoonsSrf &operator =(const GCoonsSrf &rhs);

	// 메쉬 생성함수
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// 넙스 연산
	void KnotRefineU();
	void KnotRefineV();

public:
	/*! \brief Coons 곡면을 구성하는 4개의 경계 곡선 */
	GNurbsCrv *pCrvs[4];
};

/*!
*	\class GCoonsMesh
*	\brief Coons 곡면들로 구성된 메쉬를 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 4 April, 2016
*/
class GCoonsMesh: public GMesh
{
	DECLARE_GMESH_OBJECT(GCoonsMesh)
public:
	// 생성자 및 소멸자
	GCoonsMesh();
	GCoonsMesh(std::string _Name, GMesh *_pRefMesh);
	GCoonsMesh(const GCoonsMesh &cpy);
	virtual ~GCoonsMesh();

	// 대입 연산자
	GCoonsMesh &operator =(const GCoonsMesh &rhs);

	// 메쉬 생성함수
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// 멤버함수
	void AddVertIdx(int vidx);
	GMesh *AddVertIdx2(int vidx);

public:
	/*! \brief Coons 메쉬가 정의된 참조 메쉬 */
	GMesh *pRefMesh;

	/*! \brief 생성 중인 Coons 패치의 코너 정점의 인덱스 리스트 (4개씩 그룹으로 하나의 패치를 생성) */
	std::vector<int> VertIdx;

	/*! \brief 생성된 Coons 패치의 리스트 */
	std::vector<GCoonsSrf *> SrfList;
};

/*!
*	\class GBridgeMesh
*	\brief 두 메쉬를 연결하는 브리지 메쉬를 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 31 July, 2017
*/
// class GBridgeMesh : public GMesh
// {
// 	DECLARE_GMESH_OBJECT(GBridgeMesh)
// public:
// 	// 생성자 및 소멸자
// 	GBridgeMesh(std::string _Name);
// 	GBridgeMesh(GBridgeMesh &cpy);
// 	virtual ~GBridgeMesh();
// 
// 	// 대입 연산자
// 	GBridgeMesh &operator =(const GBridgeMesh &rhs);
// 
// public:
// 	/*! \brief 시작 메쉬에 대한 포인터 */
// 	GMesh *pStMesh;
// 
// 	/*! \brief 시작 메쉬의 브리지 중심점 */
// 	GVertex *pStVert;
// 
// 	/*! \brief 시작 메쉬의 2D 매개화 메쉬 */
// 	GMesh *pStUnfoldMesh;
// 
// 	/*! \brief 시작 메쉬의 접촉 곡선 */
// 	GNurbsCrv *pStContCrv;
// 
// 	/*! \brief 시작 메쉬의 보조 곡선의 스케일 값 */
// 	double StAuxCrvScl;
// 
// 	/*! \brief 끝 메쉬에 대한 포인터 */
// 	GMesh *pEdMesh;
// 
// 	/*! \brief 끝 메쉬의 브리지 중심점 */
// 	GVertex *pEdVert;
// 
// 	/*! \brief 끝 메쉬의 2D 매개화 메쉬 */
// 	GMesh *pEdUnfoldMesh;
// 
// 	/*! \brief 끝 메쉬의 접촉 곡선 */
// 	GNurbsCrv *pEdContCrv;
// 
// 	/*! \brief 끝 메쉬의 보조 곡선의 스케일 값 */
// 	double EdAuxCrvScl;
// 
// 	/*! \brief 블렌딩 함수의 제어 상수 */
// 	double BlendFuncParam;
// };


#endif // _GOBJECT_H
