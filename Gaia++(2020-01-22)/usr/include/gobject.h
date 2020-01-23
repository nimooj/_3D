#ifndef _GOBJECT_H
#define _GOBJECT_H

#include "..\usr\include\gobject_nurbs_util.h"
#include "..\usr\include\gobject_nurbs_knot.h"
#include "..\usr\include\gobject_nurbs_symbolic.h"
#include "..\usr\include\gobject_mesh_util.h"
#include "..\usr\include\gobject_bridge_mesh.h"

// Ŭ���� ����
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

// Ŭ���� GEditObj�� ��� �޴� Ŭ������ ��� �Լ��� ���� ��ũ��
#define DECLARE_GEDIT_OBJECT(Class) \
public: \
	virtual std::string GetClassName(); \
	virtual Class *CopyObject(); \
	virtual	GMesh *GetRefMesh(); \
	virtual GTransf GetTransf(); \
	virtual void Edit(GTransf EC, GVector3 t = GVector3(), GQuater q = GQuater(), GVector3 s = GVector3(1.0, 1.0, 1.0), GMesh *pParentMesh = NULL); \
	virtual void RenderForEdit(float r, float g, float b);

// Ŭ���� GEditObj�� ��� �޴� Ŭ������ �Ϻ� ��� �Լ��� ���� ��ũ��
#define IMPLEMENT_GEDIT_OBJECT(Class) \
	std::string Class::GetClassName() {return std::string(#Class);} \
	Class *Class::CopyObject() {return new Class(*this);}

// Ŭ���� GMesh�� ��� �޴� Ŭ������ ��� �Լ��� ���� ��ũ��
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

// Ŭ���� GMesh�� ��� �޴� Ŭ������ �Ϻ� ��� �Լ��� ���� ��ũ��
#define IMPLEMENT_GMESH_OBJECT(Class) \
	std::string Class::GetClassName() {return std::string(#Class);} \
	Class *Class::CopyObject() {return new Class(*this);}

/*!
*	\class GEditObj
*	\brief ���� ������ ��ü�� �⺻ Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001	
*/
class GEditObj
{
	DECLARE_GEDIT_OBJECT(GEditObj)
public:
	// ������ �� �Ҹ���
	GEditObj() {};
	virtual ~GEditObj() {};
};

/*!
*	\class GVertex
*	\brief �޽��� ������ ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu) 
*	\date 01 Jan 2001	
*/
class GVertex: public GEditObj
{
	DECLARE_GEDIT_OBJECT(GVertex)
	// ������ �Լ�	
	friend bool operator ==(const GVertex &lhs, const GVertex &rhs);

public:
	// ������ �� �Ҹ���
	GVertex(GPoint3 _P = GPoint3(), GVector3 _N = GVector3(), GVector3 _C = GVector3(0.5, 0.5, 0.5));
	GVertex(const GVertex &cpy);
	virtual ~GVertex();

	// ���� ������ �ߺ�
	GVertex &operator =(const GVertex &rhs);

	// ��� �Լ�
	void AddRefBoneInfo(std::string &BoneMeshName, double Wgt);

public:
	// ������ ���(7)
	/*! \brief ������ ���� �޽��� ������ */
	GMesh *pMesh;

	/*! \brief ������ �ε��� (0 ���� ����) */
	int Idx;

	/*! \brief ������ 3���� ��ǥ */
	GPoint3 P;

	/*! \brief ������ ���� ���� */
	GVector3 N;

	/*! \brief ������ ���� */
	GVector3 C;

	/*! \brief ������ ���� ����ġ */
	double EditWgt;

	/*! \brief ������ �����ϴ� ��(bone) �޽��� ���� ���� ����Ʈ */
	std::vector<GRefBoneInfo> BoneInfoList;
};

/*!
*	\class GFace
*	\brief �޽��� �ﰢ�� ������ ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu) 
*	\date 21 July 2008	
*/
class GFace: public GEditObj
{
	DECLARE_GEDIT_OBJECT(GFace)
public:
	// ������ �� �Ҹ���
	GFace(int vidx0 = -1, int vidx1 = -1, int vidx2 = -1, int tidx0 = -1, int tidx1 = -1, int tidx2 = -1, int mtl_idx = -1);
	GFace(const GFace &cpy);
	virtual ~GFace();

	// ���� ������ �ߺ�
	GFace &operator =(const GFace &rhs);

	// ��� �Լ�
	int GetPrevVertIdx(int vidx);
	int GetNextVertIdx(int vidx);
	int GetSharedVertIdx(GFace *pSharedFace, int &vidx0, int &vidx1);
	GPoint3 GetCOG(bool bLocal);
	GVector3 GetNormal(bool bLocal);

public:
	// ������ ���(5)
	/*! \brief �ﰢ���� ���� �޽��� ������ */
	GMesh *pMesh;

	/*! \brief �ﰢ���� �ε���(0 ���� ����) */
	int Idx;

	/*! \brief �ﰢ���� �����ϴ� ������ �ε��� */
	int vIdx[3];

	/*! \brief �ﰢ���� �������� �����ϴ� �ؽ�ó ��ǥ�� �ε��� */
	int tIdx[3];

	/*! \brief �ﰢ���� ����Ǵ� ������ �ε��� */
	int MtlIdx;
};

/*!
*	\class GPtOnFace
*	\brief �ﰢ�� ������ ���� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu) 
*	\date 8 December 2015	
*/
class GPtOnFace: public GEditObj
{
	DECLARE_GEDIT_OBJECT(GPtOnFace)
public:
	// ������ �� �Ҹ���
	GPtOnFace(GFace *_pFace = NULL, GVector3 _Coords = GVector3());
	GPtOnFace(const GPtOnFace &cpy);
	virtual ~GPtOnFace();

	// ���� ������ �ߺ�
	GPtOnFace &operator =(const GPtOnFace &rhs);

	// ��� �Լ�
	void Set(GFace *_pFace, GVector3 _Coords);

public:
	// ������ ���(2)
	/*! \brief ���õ� ���� ���� �ﰢ���� ������ */
	GFace *pFace;

	/*! \brief �ﰢ���� �� ������ �������� ǥ���� ���� ���� barycentric ��ǥ, P = V0 * C0 + V1 * C1 + V2 * C3 */
	GVector3 Coords;
};

/*! \brief �޽��� ������ Ÿ�� ������ */
#define RENDER_POLYGON		0x0001
#define RENDER_WIRE			0x0002
#define RENDER_POINT		0x0004
#define RENDER_NORMAL		0x0008
#define RENDER_CTRL_PT		0x0010
#define RENDER_BOUNDARY		0X0020
#define RENDER_SWEEP_PLANE	0x0040

/*! \brief �޽��� ���̵� Ÿ�� ������ */
enum TypeShade
{
	SHADE_SMOOTH = 0,
	SHADE_FLAT,
};

/*! \brief �޽� ������ ������ �����ϴ� Ÿ�� ������ */
enum TypeVertexColor
{
	VERTEX_COLOR_ILLUMINATION,
	VERTEX_COLOR_NORMAL,
	VERTEX_COLOR_BONE_WEIGHT,
	VERTEX_COLOR_CURVATURE,
};

/*!
*	\class GRenderer
*	\brief �޽��� ������ �Ӽ��� �����ϴ� Ŭ����
*	\note ������ ������(20 Aug. 2016)
*
*	\author ������(shyun@dongguk.edu) 
*	\date 02 Sept. 2012	
*/
class GRenderer
{
public:
	// ������ �� �Ҹ���
	GRenderer();
	GRenderer(const GRenderer &cpy);
	virtual ~GRenderer();

	// ���� ������ �ߺ�
	GRenderer &operator =(const GRenderer &rhs);

	// ����Լ�
	bool IsVisibleMesh();
	void SetSampleNum(int NumU, int NumV, int NumW = 0);
	
public:
	/*! \brief �޽��� ������ Ÿ��: RENDER_POLYGON, RENDER_WIRE, ... */
	unsigned int RenderType;

	/*! \brief �޽��� ���̵� Ÿ��: SHADE_SMOOTH, SHADE_FLAT */
	TypeShade ShadeType;

	/*! \brief ������ ���� Ÿ�� */
	TypeVertexColor VertColorType;

	/*! \brief �޸� ���� ���� */
	bool bBackFaceCull;

	/*! \brief �ؽ�ó ���� ���� */
	bool bTexture;

	// �/���/������ ������ �Ӽ��� ǥ���ϴ� ������ ���
public:	
	/*! \brief �/���/������ (U-����) ���ø� �� */
	int NumSampU;

	/*! \brief �/���/������ (V-����) ���ø� �� */
	int NumSampV;

	/*! \brief ������ W-���� ���ø� �� */
	int NumSampW;

	/*! \brief ��� ������ �������ϴ� ���� */
	bool bRenderCrvAsLine;

	/*! \brief �ҽ� �/���/������ �������� ũ�� */
	double CtrlPtSize;

	/*! \brief ���� �/����� ���� ���� */
	std::vector<GVector3> NodePts;

	/*! \brief ������ ǥ�� �������� ���� �Ķ���� �� */
	double IsoSrfParam[6];

	/*! \brief ���� ����� �ʺ� */
	double SwpPlnWidth;

	/*! \brief ���� ����� ���� */
	double SwpPlnHeight;

	/*! \brief GNurbsCrv, GBlendCrv, GBiArcCrv�� ��쿡�� ���Ǵ� ������ �ε��� */
	int CrvMtlIdx;
};

/*!
*	\class GMaterial
*	\brief �޽��� ����(material)�� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 21 July 2008	
*/
class GMaterial
{
public:
	// ������ �� �Ҹ���
	GMaterial(const std::string &_Name = "mtl_default");
	GMaterial(const GMaterial &cpy);
	virtual ~GMaterial();

	// ������ �ߺ�
	GMaterial &operator =(const GMaterial &rhs);

	// ����Լ�
	void SetAmbient(GLfloat r, GLfloat g, GLfloat b);
	void SetDiffuse(GLfloat r, GLfloat g, GLfloat b);
	void SetSpecular(GLfloat r, GLfloat g, GLfloat b);
	void SetShiness(GLfloat shiness);
	void SetAlpha(GLfloat alpha);
	void LoadTexture(const char *fname);
	void Enable(bool bTexture);
	void Disable();

public:
	/*! \brief ������ �̸� */	
	std::string Name;

	/*! \brief ������ �ֺ��� �ݻ��� */
	GLfloat Ambient[4];

	/*! \brief ������ ���ݻ� ��� */
	GLfloat Diffuse[4];

	/*! \brief ������ ���ݻ� ��� */
	GLfloat Specular[4];

	/*! \brief ������ ���ݻ� ���� */
	GLfloat Shiness;

	/*! \brief ������ �ؽ�ó ���ϸ� */	
	std::string TexFileName;

	/*! \brief ������ �ؽ�ó ���̵� */
	GLuint BindTexId;
};

/*!
*	\class GBndBox
*	\brief 3���� ��鿡�� ���� ��ǥ���� �࿡ ������ �ٿ�� �ڽ��� ǥ���ϴ� Ŭ����
*	\note ������ ������(21 Aug. 2016)
*
*	\author ������(shyun@dongguk.edu) 
*	\date 29 Jan. 2015
*/
class GBndBox
{
public:
	// ������ �� �Ҹ���
	GBndBox(double minx = 0, double miny = 0, double minz = 0, double maxx = 0, double maxy = 0, double maxz = 0);
	GBndBox(const GBndBox &cpy);
	~GBndBox() {}

	// ���� ������ 
	GBndBox &operator =(const GBndBox &rhs);

	// ��� �Լ�
	GTransf GetMC();
	double GetWidth();
	double GetHeight();
	double GetDepth();
	double GetDiagonal();
	void GetCornerPts(GPoint3 *Pt, bool bLocal = true);
	
	// ������ ���� �Լ�
	void Render(double r, double g, double b, double width);

public:
	// ������ ���
	/*! \brief ��� ������ ��� �޽� */
	GMesh *pMesh;

	/*! \brief ��� ������ �ּ��� (�޽��� �𵨸� ��ǥ�迡�� ǥ��) */
	GPoint3 MinPt;

	/*! \brief ��� ������ �ִ��� (�޽��� �𵨸� ��ǥ�迡�� ǥ��) */
	GPoint3 MaxPt;	
};

/*! \brief �޽��� �����ϴ� �ﰢ���� �迭 ��ġ�� ���� �޽� Ÿ�� */
enum TypeMeshFace
{
	FACE_TRI = 0,	// ��Ģ���� ���� �ﰢ�� �迭
	FACE_QUAD,		// ¦��, Ȧ����° �ﰢ���� ������ ����, �Ϲ����� ��鿡 �ش��
	FACE_TRI_FAN,	// �ﰢ���� ������� ������ ����
};

/*! \brief �޽��� �ø�ƽ Ÿ���� ��Ÿ���� ������ */
enum TypeMeshSemantic
{
	MESH_STD = 0,		// ǥ��(STANDARD) �޽�
	MESH_BONE,			// ��(bone)���� ���Ǵ� �޽�
	MESH_SKIN,			// ���� ���ε��� ������ ����, ��Ų���� ���Ǵ� �޽�
	MESH_CTRL,			// ���� �޽��� ���� �޽�
	MESH_KEY_CRV,		// ��������� �����ϴ� Ű � �޽�
	MESH_KEY_FRAME,		// �ҽ������ �����ϱ� Ű �޽�
	MESH_PIVOT,			// ���ε� ���� ����� ������ �޽�
};

/*! \brief �޽��� ���� ������ ��Ÿ���� ������ */
enum TypeModified
{
	MODIFIED_NONE = 0,		// ������ ����
	MODIFIED_VERT_POS,		// �޽��� �Ϻ� ������ ��ġ�� �����Ǿ���
	MODIFIED_VERT_POS_ALL,	// �޽��� ��� ������ ��ġ�� �����Ǿ���
	MODIFIED_VERT_NUM,		// �޽��� ������ ������ ����Ǿ���(����, ���� ��)
	MODIFIED_FACE_POS,		// �޽��� �Ϻ� �ﰢ���� ��ġ�� �����Ǿ���
	MODIFIED_NORMAL,		// �޽��� ������ ����Ǿ���
	MODIFIED_MTL,			// �޽��� ������ ����Ǿ���
	MODIFIED_CTLPT_POS,		// �ҽ� �������� ��ġ�� ����Ǿ���
	MODIFIED_INTPT_POS,		// �ҽ� �������� ��ġ�� ����Ǿ���
	MODIFIED_SAMPLE_NUM,	// �ҽ� ��ü�� ���ø� ���� ����Ǿ���
	MODIFIED_CTLPT_SIZE,	// �ҽ� ��ü�� �������� ũ��� ����Ǿ���
	MODIFIED_REF_BONE,		// ���� �� �޽��� ����Ǿ���
};

/*!
*	\class GMesh
*	\brief ������ ������ ���� �޽� �����͸� ǥ���ϴ� Ŭ����
*	\note ������ ������(21 Aug. 2016)
*
*	\author ������(shyun@dongguk.edu) 
*	\date 01 Jan 2001	
*/
class GMesh: public GEditObj
{
	DECLARE_GMESH_OBJECT(GMesh)
public:
	// ������ �� �Ҹ���
	GMesh(const std::string &_Name, GTransf _MC, TypeMeshFace _FaceType, const std::string &_ParentMeshName = "");
	GMesh(const GMesh &cpy);
	virtual ~GMesh();

	// ���� ������
	GMesh &operator =(const GMesh &rhs);

	// �޽��� ���� ������ �߰�/�����ϴ� �Լ�
	void AddVertex(GVertex v);
	void AddFace(GFace f, std::string GroupName = "");
	void AddTexel(GVector2 t);
	void AddMtl(GMaterial mtl);
	void AddClipPlanes();
	void DelFaces(std::vector<int> &FaceList);
	void DelVerts(std::vector<int> &VertList);

	// �޽��� ���� ������ ��� �Լ�
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
			
	// �޽��� ������ ��� ���� ���� �Լ�
	void UpdateNormal(bool all);
	void UpdateBndBox(bool all);
	void ReverseNormal();

	// �� �ִϸ��̼ǰ� ���õ� �Լ�
	void InitRefBoneInfo();
	void Animate(GMesh *pParent);
	void ApplySSD();

	// �޽��� ������ ���۸� �����ϴ� �Լ�
	void UpdateBufVertex();
	void UpdateBufNormal();
	void UpdateBufFace();

	// �޽��� ������ �ϴ� ����Լ�
	void RenderMeshPolygon();
	void RenderMeshTexture();
	void RenderMeshWire(float r, float g, float b, float width);
	void RenderMeshPoint(float r, float g, float b, float size);
	void RenderMeshBoundary(float r, float g, float b, float width);
	void InitBndryEdgeList();

	// �޽� ���� �Լ�
	void Normalize();
	void ShowMesh(bool bSubMesh);
	void HideMesh(bool bSubMesh);

	// �ٿ�� ���� ���� ���� ���� �Լ�
	void CreateBVH();
	void DeleteBVH();

public:
	// �⺻ ������ ���(19)
	/*! \brief �޽��� �̸� */
	std::string Name;

	/*! \brief �޽��� ���ǵ� �𵨸� ��ǥ�� */
	GTransf MC;

	/*! \brief �޽��� ũ�� ��ȯ ���� */
	GVector3 S;

	/*! \brief �޽��� ������ Ÿ��(�ﰢ��, �簢��) ���� */
	TypeMeshFace FaceType;

	/*! \brief �޽��� �����ϴ� ������ �迭 */
	std::vector<GVertex> Verts;

	/*! \brief �޽��� �����ϴ� �ؽ�ó ������ �迭 */
	std::vector<GVector2> Texs;

	/*! \brief �޽��� �����ϴ� �ﰢ���� �迭 */
	std::vector<GFace> Faces;

	/*! \brief �޽��� �׷��ε� �ﰢ�� �ε��� �迭 */
	std::map<std::string, std::vector<int>> GrpFaces;

	/*! \brief �޽��� �ѷ��δ� ��� ���� */
	GBndBox BBox;

	/*! \brief �޽��� �ٿ�� ���� ���� ���� */
	PQP_Model *pBVH;

	/*! \brief �浹�� ���� �ﰢ���� �ε��� ����Ʈ */
	std::vector<int> CollisionFaces;

	/*! \brief �޽��� ������ �Ӽ� */
	GRenderer Renderer;

	/*! \brief �޽��� ����ϴ� ���� ����Ʈ */
	std::vector<GMaterial> MtlList;

	/*! \brief �ؽ�ó ���� ���� ���÷��� ����Ʈ */
	GLuint DispTexMesh;

	/*! \brief ���� Į�� �������� ���� ���÷��� ����Ʈ */
	GLuint DispVertColMesh;

	/*! \brief �ﰢ�� �÷� �������� ���� ���÷��� ����Ʈ */
	GLuint DispFaceColMesh;	

	/*! \brief �������� �ӵ��� ����� ���� ���� */
	std::vector<float> VtxBuf;
	std::vector<float> NorBuf;
	std::map<int, std::vector<int>> FaceIdxBuf;

public:
	// �߰� ������ ���(11)
	/*! \brief �޽��� ���Ե� ��鿡 ���� ������ */
	GScene *pScene;

	/*! \brief �޽��� ����� �θ� �޽��� �̸� */
	std::string ParentMeshName;

	/*! \brief �θ� �޽��κ����� ��ȯ (MC = �θ� MC * DelX) */
	GTransf DelX;

	/*! \brief �޽��� ����� �ڽ� �޽� ����Ʈ */
	std::vector<GMesh *> SubMeshList;

	/*! \brief �޽��� �ø�ƽ Ÿ�� ���� */
	TypeMeshSemantic SemanticType;
	
	/*! \brief �޽��� �ø�ƽ Ÿ�Կ� ���� ����� �޽��� �̸� ����Ʈ */
	std::vector<std::string> LinkedMeshNameList;

	/*! \brief �޽��� ���� ����� �̸� ����Ʈ */
	std::vector<std::string> ClipPlaneNameList;

	/*! \brief �޽��� ������ ������ ����ϴ� ���� */
	TypeModified ModifiedFlag;
	
	/*! \brief �޽��� ��� �𼭸� ����Ʈ */
	std::map<int, int> BndryEdgeList;

	/*! \brief �ִϸ��̼��� ���� ��ȯ ���� */
	std::vector<GTransf> Animation;

	/*! \brief �ִϸ��̼��� ���� ������ �ε��� */
	int FrameIdx;
};

/*!
*	\class GRefBoneInfo
*	\brief �� �޽��� ���� ������ ���ε� ������ ǥ���ϴ� Ŭ����
*	\note ������ ������(20 Aug. 2016)
*
*	\author ������(shyun@dongguk.edu) 
*	\date 01 Jan 2001	
*/
class GRefBoneInfo
{
public:
	// ������ �� �Ҹ���
	GRefBoneInfo();
	GRefBoneInfo(std::string &MeshName, double _Weight, GPoint3 &_P, GVector3 &_N);
	GRefBoneInfo(const GRefBoneInfo &cpy);
	~GRefBoneInfo();

	// ���� ������
	GRefBoneInfo &operator =(const GRefBoneInfo &rhs);

public:
	// ������ ���(5)
	/*! \brief ������ ���ε��� �� �޽��� �̸� */
	std::string BoneMeshName;

	/*! \brief ������ ���ε��� �� �޽��� �𵨸� ��ǥ��(MC)�� ���� ������ */
	GTransf *pRefMC;

	/*! \brief �� �޽��� �𵨸� ��ǥ�迡 ���� ������ ���� ����ġ */
	double Wgt;

	/*! \brief pRefMC���� ǥ���� ������ ��ǥ */
	GPoint3 P;

	/*! \brief pRefMC���� ǥ���� ������ ��ǥ */
	GVector3 N;
};

/*!
*	\class GPathInfo
*	\brief �ִ� ����� �������� �ﰢ�� �𼭸��� �����ϴ� ������ ǥ���ϴ� Ŭ����
*	\note ������ ������(22 Aug. 2016)
*
*	\author ������(shyun@dongguk.edu) 
*	\date 09 Mar. 2016
*/
class GPathInfo
{
public:
	// ������ �� �Ҹ���
	GPathInfo(int _sidx, int _eidx, double _t);
	GPathInfo(const GPathInfo &cpy);
	~GPathInfo();

	// ���� ������
	GPathInfo &operator =(const GPathInfo &rhs);

public:
	// ������ ���
	/*! \brief ��ο� �����ϴ� ���� ������ ���� �ε��� */
	int sidx;

	/*! \brief ��ο� �����ϴ� ���� ������ �� �ε��� */
	int eidx;

	/*! \brief �������� �Ű����� */
	double t;
};

/*!
*	\class GGeoPath
*	\brief �޽��� �������� �����ϴ� ���� ��θ� ǥ���ϴ� Ŭ����
*	\note ������ ������(22 Aug. 2016)
*
*	\author ������(shyun@dongguk.edu)
*	\date 11 March, 2016
*/
class GGeoPath: public GMesh
{
	DECLARE_GMESH_OBJECT(GGeoPath)
public:
	// ������ �� �Ҹ���
	GGeoPath();
	GGeoPath(std::string _Name, GTransf _MC, GMesh *_pMesh, std::string _ParentName);
	GGeoPath(const GGeoPath &rhs);
	virtual ~GGeoPath();

	// ���� ������
	GGeoPath &operator =(const GGeoPath &rhs);

	// �޽� ���� �Լ�
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// ��� �߰� �Լ�
	void AddPath(int ToVertIdx);
	void DelPath(int VertIdx);
	void EditPath(int ToVertIdx);

	// ��� �Լ�
	int GetStartVertIdx();
	int GetEndVertIdx();

public:
	/*! \brief ���� ��ΰ� ���ǵǴ� �޽��� ���� ������ */
	GMesh *pRefMesh;

	/*! \brief ��� ������ �ε��� ����Ʈ */
	std::vector<int> PathVertIdx;

	/*! \brief ������ ��θ� �����ϴ� �������� ����Ʈ */
	std::vector<std::vector<GPoint3>> Paths;

	/*! \brief ���� ���� ���� ��� ������ �ε��� */
	int EditPathVertIdx;
};

/*!
*	\class GCutPt
*	\brief �޽��� ���ܼ��� �����ϴ� �ﰢ�� �𼭸����� ���� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 05 Sep., 2015	
*/
class GCutPt: public GEditObj
{
	DECLARE_GEDIT_OBJECT(GCutPt);
public:
	// ������ �� �Ҹ���
	GCutPt();
	GCutPt(GFace *_pFace, int vidx0, int vidx1, double t, GPoint3 _P);
	GCutPt(const GCutPt &cpy);
	virtual ~GCutPt();

	// ���Կ�����
	GCutPt &operator =(const GCutPt &rhs);

public:
	/*! \brief ���� ���� �ִ� �𼭸��� ������ �ﰢ�� ��ü */
	GFace *pFace;

	/*! \brief ���� ���� �ִ� �𼭸��� ���� �ε���: 0: ���� �ε���, 1: �� �ε���, 2: ������ �ε��� */
	int vIdx[3];

	/*! \brief �𼭸����� ���� �Ķ���� */
	double t;

	/*! \brief ���� 3���� ��ǥ */
	GPoint3 P;
};

/*!
*	\class GCtrlPt
*	\brief B-spline ��ü�� �������� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 25 Jan, 2015	
*/
class GCtrlPt: public GEditObj, public GVector4
{
	DECLARE_GEDIT_OBJECT(GCtrlPt);
	// �Ϲ� ������
	friend GCtrlPt operator +(const GCtrlPt &lhs, const GCtrlPt &rhs);
	friend GCtrlPt operator +(const GVector4 &lhs, const GCtrlPt &rhs);
	friend GCtrlPt operator +(const GCtrlPt &lhs, const GVector4 &rhs);

public:
	// ������ �� �Ҹ���
	GCtrlPt();
	GCtrlPt(GVector4 _P);
	GCtrlPt(const GCtrlPt &cpy);
	virtual ~GCtrlPt();

	// ���Կ�����
	GCtrlPt &operator =(const GVector4 &rhs);
	GCtrlPt &operator =(const GCtrlPt &rhs);	

public:
	/*! \brief ������ ���� B-spline ��ü */
	GMesh *pMesh;
};

/*!
*	\class GNurbsCrv
*	\brief Non-Uniform Rational B-spline ��� ��Ÿ���� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan, 2001	
*/
class GNurbsCrv: public GMesh
{
	DECLARE_GMESH_OBJECT(GNurbsCrv)
public:
	// ������ �� �Ҹ���
	void _alloc(int _p, int _n);
	void _free();
	GNurbsCrv();
	GNurbsCrv(std::string _Name, GTransf _MC, int _p, int _n, double *_U, GVector4 *_P, bool _rational = false, std::string _ParentName = "", std::string _RefMeshName = "");
	GNurbsCrv(std::string _Name, GTransf _MC, int _p, int _n, double *_U, GCtrlPt *_P, bool _rational = false, std::string _ParentName = "", std::string _RefMeshName = "");
	GNurbsCrv(const GNurbsCrv &cpy);
	virtual ~GNurbsCrv();

	// ���� ������
	GNurbsCrv &operator =(const GNurbsCrv &rhs);

	// �޽� �����Լ�
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// ��� �Լ���
	GVector4 EvalDeriv(double u, int nth);

	// ���� ���� �Լ���
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
	
	// � ���� �Լ���
	void Edit(int idx, GVector4 dp);
	void Edit(double u, GVector4 dp);
	void Edit(double u, GVector4 dp, int support);
	GVector4 GetCtrlPt(int idx);

public:
	/*! \brief �ҽ� ��� ���ǵ� �޽��� �̸� */
	std::string RefMeshName;

	/*! \brief �ҽ� ��� ���� */
	int p;

	/*! \brief �ҽ� ��� �������� ������ �ε��� */
	int n;

	/*! \brief �ҽ� ��� �� ���� */
	double *U;

	/*! \brief �ҽ� ��� ������ �迭 */
	GCtrlPt *P;

	/*! \brief ������ � ���� */
	bool rational;

	/*! \brief \a (p - 1)�� ���Ӽ��� ���� �� ���� */
	bool closed;
};

/*!
*	\class GNurbsSrf
*	\brief 3���� �������� �ҽ������ ��Ÿ���� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan, 2001	
*/
class GNurbsSrf: public GMesh
{
	DECLARE_GMESH_OBJECT(GNurbsSrf)
public:
	// ������ �� �Ҹ���
	void _alloc(int _p, int _m, int _q, int _n);
	void _free();
	GNurbsSrf();
	GNurbsSrf(std::string _Name, GTransf _MC, int _p, int _m, double *_U, int _q, int _n, double *_V, GVector4 **_P, bool _rational = false, std::string _ParentName = "");
	GNurbsSrf(std::string _Name, GTransf _MC, int _p, int _m, double *_U, int _q, int _n, double *_V, GVector4 *_P, bool _rational = false, std::string _ParentName = "");
	GNurbsSrf(const GNurbsSrf &cpy);
	virtual ~GNurbsSrf();

	// ���� ������
	GNurbsSrf &operator =(const GNurbsSrf &rhs);

	// �޽� �����Լ�
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// Evaluation �Լ���
	GVector4 EvalDeriv(double u, double v, int kth, int lth);
	void EvalTanPln(double u, double v, GVector3 &Su, GVector3 &Sv, GVector3 &N);

	// Geometric ���� �Լ���
	void KnotClamped(bool dir_u, bool dir_v);
	void KnotUnclamped(bool dir_u, bool dir_v);
	int KnotInsert(int dirType, double val, int r);
	int KnotRemove(int dirType, double val, int r);
	void KnotRefine(bool dir_u, bool dir_v);
	void ElevateDeg(bool dir_u, bool dir_v);
	void CnvrtBzrForm(bool dir_u, bool dir_v);
	void CnvrtBspForm(bool dir_u, bool dir_v);

	// ��� ���� �Լ���
	void Edit(int uidx, int vidx, GVector4 dp);
	void Edit(double u, double v, GVector4 dp);
	void Edit(double u, double v, GVector4 dp, int support_u, int support_v);
	void SetZeros();
	GVector4 GetCtrlPt(int idx);

public:
	/*! \brief �ҽ� ����� U-���� ���� */
	int p;

	/*! \brief �ҽ� ����� U-���� �������� ������ �ε��� */
	int m;

	/*! \brief �ҽ� ����� U-���� �� ���� */
	double *U;

	/*! \brief �ҽ� ����� V-���� ���� */
	int q;

	/*! \brief �ҽ� ����� V-���� �������� ������ �ε��� */
	int n;

	/*! \brief �ҽ� ����� V-���� �� ���� */
	double *V;

	/*! \brief �ҽ� ����� ������ �迭 */
	GCtrlPt **P;

	/*! \brief ������ � ���� */
	bool rational;

	/*! \brief \a (p - 1)�� ���Ӽ��� ���� U-���� �� ���� */
	bool closed_u;

	/*! \brief \a (q - 1)�� ���Ӽ��� ���� V-���� �� ���� */
	bool closed_v;
};

/*!
*	\class GSweepSrf
*	\brief �ܸ� ��� �����Ͽ� �����Ǵ� ��������� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 20 May, 2013	
*/
class GSweepSrf: public GMesh
{
	DECLARE_GMESH_OBJECT(GSweepSrf)
public:
	// ������ �� �Ҹ���
	GSweepSrf(std::string _Name, GTransf _MC, std::vector<GNurbsCrv *> _KeyCrvList, int _DegT, int _DegR, std::string _ParentName);
	GSweepSrf(const GSweepSrf &cpy);
	virtual ~GSweepSrf();

	// ���Կ�����
	GSweepSrf &operator =(const GSweepSrf &rhs);

	// ���� ����� ����ϴ� �Լ�
	GPlane EvalPlane(double t);

	// �޽� �����Լ�
	void CreateComponent();
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

public:
	// ������ ���
	/*! \brief ���� ����� �����ϱ� ���� Ű� ����Ʈ */
	std::vector<GNurbsCrv *> KeyCrvList;

	/*! \brief ���� ����� ���� ���� ��� ���� */
	int DegT;

	/*! \brief ���� ����� ���� ���� ��� ���� */
	int DegR;

	/*! \brief ���� ����� ���� ���� � */
	GNurbsCrv *pTraCrv;	

	/*! \brief ���� ����� ���� ���� � */
	GNurbsCrv *pRotCrv;
	
	/*! \brief ���� ����� ���� ���� ��� */
	GNurbsSrf *pSrcSrf;
};

/*!
*	\class GNurbsMot
*	\brief Ű ��ǥ�踦 �����Ͽ� �����Ǵ� ���� ����� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 23 Feb., 2016	
*/
class GNurbsMot: public GMesh
{
	DECLARE_GMESH_OBJECT(GNurbsMot)
public:
	// ������ �� �Ҹ���
	GNurbsMot(std::string _Name, GTransf _MC, std::vector<GMesh *> _KeyMeshList, int _DegT, int _DegR, std::string _ParentName);
	GNurbsMot(const GNurbsMot &cpy);
	virtual ~GNurbsMot();

	// ���Կ�����
	GNurbsMot &operator =(const GNurbsMot &rhs);

	// ���� ����� ����ϴ� �Լ�
	GPlane EvalPlane(double u);
	GTransf EvalFrame(double u);
	int GetNumKeyMesh();

	// �޽� �����Լ�
	void CreateComponent();
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

public:
	// ������ ���
	/*! \brief �ҽ� ����� �����ϱ� ���� Ű ��ǥ���� ����Ʈ */
	std::vector<GMesh *> KeyMeshList;

	/*! \brief �ҽ� ����� ���� ���� ��� ���� */
	int DegT;

	/*! \brief �ҽ� ����� ���� ���� ��� ���� */
	int DegR;

	/*! \brief �ҽ� ����� ���� ���� � */
	GNurbsCrv *pTraCrv;	

	/*! \brief �ҽ� ����� ���� ���� � */
	GNurbsCrv *pRotCrv;

	/*! \brief �ҽ� ����� ������ ���� ��� */
	GNurbsCrv *pSclCrv;
};

/*!
*	\class GBindRefPln
*	\brief Clothes �޽� ������ ���ε� ������ �����ϴ� �Ű�ȭ�� ����� ��Ÿ���� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 1 July, 2015	
*/
class GBindRefPln: public GMesh
{
	DECLARE_GMESH_OBJECT(GBindRefPln)

public:
	// ������ �� �Ҹ���
	GBindRefPln(std::string _Name, GTransf _MC, GMesh *pPivot, std::vector<GMesh *> _KeyBoneList, std::string _ParentName);
	GBindRefPln(const GBindRefPln &cpy);
	virtual ~GBindRefPln();

	// ���Կ�����
	GBindRefPln &operator =(const GBindRefPln &rhs);

	// ���� ����� ����ϴ� �Լ�
	GPlane EvalPlane(double t);

	// �޽� �����Լ�
	void CreateComponent();
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

public:
	/*! \brief ���� ����� �����ϴ� ������ */
	GMesh *pPivot;

	/*! \brief ���� ����� �����ϴ� Ű ��(key bone) �޽� ����Ʈ */
	std::vector<GMesh *> KeyBoneList;

	/*! \brief ��� ������ �̵� ���� � */
	GNurbsCrv *pTraCrv;

	/*! \brief ��� ������ ���� ���� � */
	GNurbsCrv *pRotCrv;

	/*! \brief ����� �ʺ� */
	double PlnWidth;
};

/*!
*	\class GBlendCrv
*	\brief 2�� ��׶��� ������� ������ ���� ��� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 10 Feb, 2013	
*/
class GBlendCrv: public GMesh
{
	DECLARE_GMESH_OBJECT(GBlendCrv)
public:
	// ������ �� �Ҹ���
	GBlendCrv();
	GBlendCrv(std::string _Name, GTransf _MC, GVector4 *_P, int _n, std::string _ParentName = "");
	GBlendCrv(const GBlendCrv &cpy);
	virtual ~GBlendCrv();

	// ���Կ����� �ߺ�
	GBlendCrv &operator =(const GBlendCrv &rhs);

	// �޽� �����Լ�
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// ��� �Լ�
	void Edit(int idx, GVector4 dp);

public:
	// ������ ���
	/*! \brief ��� ������ */
	GVector4 *P;

	/*! \brief ��� ���� ���� */
	GVector4 *T;

	/*! \brief ��� �������� ������ �ε��� */
	int n;

	/*! \brief ��� ���ǿ� */
	double *Param;
};

/*!
*	\class GTriBzrSrf
*	\brief �ﰢ�� ������ ��ġ�� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 13 Jan, 2013	
*/
class GTriBzrSrf: public GMesh
{
	DECLARE_GMESH_OBJECT(GTriBzrSrf)
public:
	// ������ �� �Ҹ���
	void _alloc(int _p);
	void _free();
	GTriBzrSrf(std::string _Name, GTransf _MC, int _p = 0, GVector4 *_P = NULL, std::string _ParentName = "");
	GTriBzrSrf(const GTriBzrSrf &cpy);
	virtual ~GTriBzrSrf();

	// ���Կ�����
	GTriBzrSrf &operator =(const GTriBzrSrf &rhs);

	// �Ϲ� ����Լ�
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();
	GTriBzrSrf *SubPatch(double u, double v, double w, int idx);
	GVector3 EvalNormal(double u, double v, double w);

public:
	// ������ ���
	/*! \brief �ﰢ�� ������ ����� ���� */
	int p;

	/*! \brief �ﰢ�� ������ ����� ������ �迭�� ������ �ε��� */
	int n;

	/*! \brief �ﰢ�� ������ ����� ������ �迭 */
	GCtrlPt *P;
};

/*! \brief �ﰢ������ ���ǵǴ� ��ġ�� Ÿ�� ������ */
#define PATCH_TYPE_MODIFIED		0x0001
#define PATCH_TYPE_BZR_TRI		0x0002
#define PATCH_TYPE_BZR_TRI_EXT	0x0004
#define PATCH_TYPE_BZR_TRI_PLN	0x0008

/*!
*	\class GBlendMesh
*	\brief �ﰢ�� ������ ��ġ�� �����Ͽ� ������ ����� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 4 March, 2015
*/
class GBlendMesh: public GMesh
{
	DECLARE_GMESH_OBJECT(GBlendMesh)
public:
	// ������ �� �Ҹ���
	GBlendMesh(std::string _Name, GTransf _MC, GMesh *_pCtrlMesh);
	GBlendMesh(const GBlendMesh &cpy);
	virtual ~GBlendMesh();

	// ���Կ�����
	GBlendMesh &operator =(const GBlendMesh &rhs);

	// �Ϲ� ����Լ�
	void CreateLocalPatch();
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();
	
public:
	// ������ ���
	/*! \brief ���� �޽��� ���� �޽� */
	GMesh *pCtrlMesh;

	/*! \brief ���� �޽��� �� �ﰢ���� ���ǵ� �ﰢ�� ������ ��ġ */
	std::vector<GTriBzrSrf> PatchList;

	/*! \brief ���� �޽��� �� �ﰢ���� ���� ���� ������ */
	std::vector<GVector3> BlendOffset;

	/*! \brief ���� �޽��� �� �ﰢ���� ������ �ﰢ���� ���� */
	std::vector<int> AdjFaceListOfFace;

	/*! \brief ���� �޽��� �� �ﰢ���� ��ġ Ÿ�� */
	std::vector<int> PatchType;
};

/*!
*	\class GUcbsSrf1D
*	\brief Uniform cubic B-spline ����� ǥ���ϴ� Ŭ����
*	\note [0, 1] x [0, 1] --> R ���� ��Į�� �Լ�
*
*	\author ������(shyun@dongguk.edu)
*	\date 9 Aug, 2013		
*/
class GUcbsSrf1D: public GMesh
{
	DECLARE_GMESH_OBJECT(GUcbsSrf1D)
public:
	// ������ �� �Ҹ���
	void _alloc(int _m, int _n);
	void _free();
	GUcbsSrf1D(std::string _Name, GTransf _MC, int m = 3, int n = 3, double *_P = NULL, std::string _ParentName = "");
	GUcbsSrf1D(const GUcbsSrf1D &cpy);
	virtual ~GUcbsSrf1D();

	// ���Կ�����
	GUcbsSrf1D &operator =(const GUcbsSrf1D &rhs);

	// �޽� �����Լ�
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// ����Լ�
	GVector4 EvalFast(double u, ...);
	void Refine();
	void Edit(double u, double v, double df);

public:
	// ������ ���
	/*! \brief U-���� �������� ������ �ε��� (m >= 3) */
	int m;

	/*! \brief V-���� �������� ������ �ε��� (n >= 3) */
	int n; 

	/*! \brief ����� ������ �迭, ũ�� = (m + 1) * (n + 1) */
	double *P;
};

/*!
*	\class GUcbsSrf
*	\brief Uniform cubic B-spline ����� ǥ���ϴ� Ŭ����
*	\note [0, 1] x [0, 1] --> R^4���� �Լ�
*
*	\author ������(shyun@dongguk.edu)
*	\date 9 Aug, 2013		
*/
class GUcbsSrf: public GMesh
{
	DECLARE_GMESH_OBJECT(GUcbsSrf)
public:
	// ������ �� �Ҹ���
	void _alloc(int _m, int _n);
	void _free();
	GUcbsSrf(std::string _Name, GTransf _MC, int m = 3, int n = 3, GVector4 *_P = NULL, std::string _ParentName = "");
	GUcbsSrf(const GUcbsSrf &cpy);
	virtual ~GUcbsSrf();

	// ���Կ�����
	GUcbsSrf &operator =(const GUcbsSrf &rhs);

	// �޽� �����Լ�
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// ����Լ�
	GVector4 EvalFast(double u, ...);
	void EvalTanPln(double u, double v, GVector3 &Su, GVector3 &Sv, GVector3 &N);
	void EvalTanPlnFast(double u, double v, GVector3 &Su, GVector3 &Sv, GVector3 &N);
	void Refine();

public:
	// ������ ���
	/*! \brief U-���� �������� ������ �ε��� (m >= 3) */
	int m;

	/*! \brief V-���� �������� ������ �ε��� (n >= 3) */
	int n; 

	/*! \brief ����� ������ �迭, ũ�� = (m + 1) * (n + 1) */
	GCtrlPt *P;
};

/*!
*	\class GUcbsVol1D
*	\brief Uniform cubic B-spline ������ ǥ���ϴ� Ŭ����
*	\note [0, 1] x [0, 1] x [0, 1] --> R���� �Լ�
*
*	\author ������(shyun@dongguk.edu)
*	\date 5 Nov, 2013		
*/
class GUcbsVol1D: public GMesh
{
	DECLARE_GMESH_OBJECT(GUcbsVol1D)
public:
	// ������ �� �Ҹ���
	void _alloc(int _l, int _m, int _n);
	void _free();
	GUcbsVol1D(std::string _Name, GTransf _MC, int l = 3, int m = 3, int n = 3, double *_P = NULL, std::string _ParentName = "");
	GUcbsVol1D(const GUcbsVol1D &cpy);
	virtual ~GUcbsVol1D();

	// ���Կ�����
	GUcbsVol1D &operator =(const GUcbsVol1D &rhs);

	// �޽� �����Լ�
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// ����Լ�
	void Refine();

public:
	// ������ ���
	/*! \brief U-���� �������� ������ �ε��� (l >= 3) */
	int l;

	/*! \brief V-���� �������� ������ �ε��� (m >= 3) */
	int m; 

	/*! \brief W-���� �������� ������ �ε��� (n >= 3) */
	int n; 

	/*! \brief ����� ������ �迭, ũ�� = (l + 1) * (m + 1) * (n + 1) */
	double *P;
};

/*!
*	\class GUcbsVol
*	\brief Uniform cubic B-spline ������ ǥ���ϴ� Ŭ����
*	\note [0, 1] x [0, 1] x [0, 1] --> R^4���� �Լ�
*
*	\author ������(shyun@dongguk.edu)
*	\date 28 Aug, 2013		
*/
class GUcbsVol: public GMesh
{
	DECLARE_GMESH_OBJECT(GUcbsVol)
public:
	// ������ �� �Ҹ���
	void _alloc(int _l, int _m, int _n);
	void _free();
	GUcbsVol(std::string _Name, GTransf _MC, int l = 3, int m = 3, int n = 3, GVector4 *_P = NULL, std::string _ParentName = "");
	GUcbsVol(const GUcbsVol &cpy);
	virtual ~GUcbsVol();

	// ���Կ�����
	GUcbsVol &operator =(const GUcbsVol &rhs);

	// �޽� �����Լ�
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// ����Լ�
	void Refine();

public:
	// ������ ���
	/*! \brief U-���� �������� ������ �ε��� (l >= 3) */
	int l;

	/*! \brief V-���� �������� ������ �ε��� (m >= 3) */
	int m; 

	/*! \brief W-���� �������� ������ �ε��� (n >= 3) */
	int n; 

	/*! \brief ����� ������ �迭, ũ�� = (l + 1) * (m + 1) * (n + 1) */
	GCtrlPt *P;
};

/*!
*	\class GCutLine
*	\brief �޽� ������ ���ǵ� ���ܼ��� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 06 Sept., 2015		
*/
class GCutLine: public GMesh
{
	DECLARE_GMESH_OBJECT(GCutLine)
public:
	// ������ �� �Ҹ���
	GCutLine();
	GCutLine(std::string _Name, GTransf _MC, std::vector<GCutPt> &CutPtList, std::string _ParentName = "");
	GCutLine(const GCutLine &cpy);
	virtual ~GCutLine();

	// ���Կ�����
	GCutLine &operator =(const GCutLine &rhs);

	// �޽� �����Լ�
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// ����Լ�
	void GetLengthAndKappaSum(float *params, float *len_sum, float *Kappa_sum);
	
public:
	/*! \brief ���ܼ� ������ ����Ʈ */
	std::vector<GCutPt> PtList;

	/*! \brief ���ܼ� ���� ���� */
	bool bClosed;
};

/*! \brief Circular ��ũ�� ���⼺�� ��Ÿ���� ������ */
enum TypeCirDir
{
	DIR_CW = 0,
	DIR_CCW,
};

/*!
*	\class GCirArc
*	\brief Circular arc segment ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 11 Feb., 2016
*/
class GCirArc
{
public:
	// ������ �� �Ҹ���
	GCirArc();
	GCirArc(GPoint3 _Center, double _Radius, double _AngleStart, double _AngleEnd, TypeCirDir _Direction, double _LenTangent);
	GCirArc(const GCirArc &cpy);
	virtual ~GCirArc();

	// ���� ������
	GCirArc &operator =(const GCirArc &rhs);

	// ��� �Լ�
	GVector4 Eval(double t);
	GVector4 EvalDeriv(double t);

public:
	/*! \brief ��ũ�� �߽� */
	GPoint3 Center;

	/*! \brief ��ũ�� ������ */
	double Radius;

	/*! \brief ��ũ�� ���� ���� */
	double AngleStart;

	/*! \brief ��ũ�� �� ���� */
	double AngleEnd;

	/*! \brief ��ũ�� ���� */
	TypeCirDir Direction;

	/*! \brief ��ũ�� ���κ� ������ ���� */
	double LenTangent;
};

/*!
*	\class GBiArcCrv
*	\brief Circular arc��� ����� biarc ��� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 11 Feb., 2016
*/
class GBiarcCrv: public GMesh
{
	DECLARE_GMESH_OBJECT(GBiarcCrv)
public:
	// ������ �� �Ҹ���
	GBiarcCrv();
	GBiarcCrv(std::string _Name, GTransf _MC, std::vector<GCirArc> &_ArcList, std::string _ParentName);
	GBiarcCrv(const GBiarcCrv &rhs);
	virtual ~GBiarcCrv();

	// ���� ������
	GBiarcCrv &operator =(const GBiarcCrv &rhs);

	// �޽� �����Լ�
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

public:
	/*! \brief ��� �����ϴ� ��ũ�� ����Ʈ */
	std::vector<GCirArc> ArcList;
};

/*!
*	\class GCoonsSrf
*	\brief Coons ����� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 1 April, 2016
*/
class GCoonsSrf: public GMesh
{
	DECLARE_GMESH_OBJECT(GCoonsSrf)
public:
	// ������ �� �Ҹ���
	GCoonsSrf();
	GCoonsSrf(std::string _Name, GTransf _MC, int _p, int _m, double *_U, int _q, int _n, double *_V, GVector4 *_P, std::string _ParentName);
	GCoonsSrf(std::string _Name, GTransf _MC, GNurbsCrv *pCrv0, GNurbsCrv *pCrv1, GNurbsCrv *pCrv2, GNurbsCrv *pCrv3, std::string _ParentName);
	GCoonsSrf(const GCoonsSrf &rhs);
	virtual ~GCoonsSrf();

	// ���� ������
	GCoonsSrf &operator =(const GCoonsSrf &rhs);

	// �޽� �����Լ�
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// �ҽ� ����
	void KnotRefineU();
	void KnotRefineV();

public:
	/*! \brief Coons ����� �����ϴ� 4���� ��� � */
	GNurbsCrv *pCrvs[4];
};

/*!
*	\class GCoonsMesh
*	\brief Coons ����� ������ �޽��� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 4 April, 2016
*/
class GCoonsMesh: public GMesh
{
	DECLARE_GMESH_OBJECT(GCoonsMesh)
public:
	// ������ �� �Ҹ���
	GCoonsMesh();
	GCoonsMesh(std::string _Name, GMesh *_pRefMesh);
	GCoonsMesh(const GCoonsMesh &cpy);
	virtual ~GCoonsMesh();

	// ���� ������
	GCoonsMesh &operator =(const GCoonsMesh &rhs);

	// �޽� �����Լ�
	void CreateMeshVert();
	void CreateMeshTex();
	void CreateMeshFace();

	// ����Լ�
	void AddVertIdx(int vidx);
	GMesh *AddVertIdx2(int vidx);

public:
	/*! \brief Coons �޽��� ���ǵ� ���� �޽� */
	GMesh *pRefMesh;

	/*! \brief ���� ���� Coons ��ġ�� �ڳ� ������ �ε��� ����Ʈ (4���� �׷����� �ϳ��� ��ġ�� ����) */
	std::vector<int> VertIdx;

	/*! \brief ������ Coons ��ġ�� ����Ʈ */
	std::vector<GCoonsSrf *> SrfList;
};

/*!
*	\class GBridgeMesh
*	\brief �� �޽��� �����ϴ� �긮�� �޽��� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 31 July, 2017
*/
// class GBridgeMesh : public GMesh
// {
// 	DECLARE_GMESH_OBJECT(GBridgeMesh)
// public:
// 	// ������ �� �Ҹ���
// 	GBridgeMesh(std::string _Name);
// 	GBridgeMesh(GBridgeMesh &cpy);
// 	virtual ~GBridgeMesh();
// 
// 	// ���� ������
// 	GBridgeMesh &operator =(const GBridgeMesh &rhs);
// 
// public:
// 	/*! \brief ���� �޽��� ���� ������ */
// 	GMesh *pStMesh;
// 
// 	/*! \brief ���� �޽��� �긮�� �߽��� */
// 	GVertex *pStVert;
// 
// 	/*! \brief ���� �޽��� 2D �Ű�ȭ �޽� */
// 	GMesh *pStUnfoldMesh;
// 
// 	/*! \brief ���� �޽��� ���� � */
// 	GNurbsCrv *pStContCrv;
// 
// 	/*! \brief ���� �޽��� ���� ��� ������ �� */
// 	double StAuxCrvScl;
// 
// 	/*! \brief �� �޽��� ���� ������ */
// 	GMesh *pEdMesh;
// 
// 	/*! \brief �� �޽��� �긮�� �߽��� */
// 	GVertex *pEdVert;
// 
// 	/*! \brief �� �޽��� 2D �Ű�ȭ �޽� */
// 	GMesh *pEdUnfoldMesh;
// 
// 	/*! \brief �� �޽��� ���� � */
// 	GNurbsCrv *pEdContCrv;
// 
// 	/*! \brief �� �޽��� ���� ��� ������ �� */
// 	double EdAuxCrvScl;
// 
// 	/*! \brief ���� �Լ��� ���� ��� */
// 	double BlendFuncParam;
// };


#endif // _GOBJECT_H
