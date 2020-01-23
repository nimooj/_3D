#ifndef _PEDITOR_H
#define _PEDITOR_H

// �ʿ��� ��� ������ �����Ѵ�.
#include "PEditMeshDlg.h"
#include "PEditCrvDlg.h"
#include "PEditSrfDlg.h"
#include "PEditVolDlg.h"
#include "PEditSwpDlg.h"
#include "PEditBlendSrfDlg.h"

// ��ũ�� �ܺ� ���̺귯���� �����Ѵ�.
#pragma comment(lib, "gmath.lib")
#pragma comment(lib, "gobject.lib")
#pragma comment(lib, "gscene.lib")

#ifdef _DEBUG
#pragma comment(lib, "pqpD.lib")
#else
#pragma comment(lib, "pqp.lib")
#endif // _DEBUG

// ����� ���� �޽���
#ifndef WM_UPDATE_SELECT
#define WM_UPDATE_SELECT (WM_USER + 2)
#endif

/*! \brief ���� ���� ������ �����ϴ� ������ Ÿ�� */
enum TypeSelectTool
{
	SELECT_TOOL_RECT = 0,
	SELECT_TOOL_CIRCLE,
	SELECT_TOOL_LASSO,
	SELECT_TOOL_POLY,
	SELECT_TOOL_BRUSH,
};

/*! \brief ���� ��ü�� ������ �����ϴ� ������ Ÿ�� */
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

/*! \brief ���� ��带 �����ϴ� ������ Ÿ�� */
enum TypeSelectMode
{
	SELECT_MODE_FRONT_ONLY = 0,
	SELECT_MODE_FRONT_AND_BACK,
};

/*! \brief ��ü�� ���� ��带 �����ϴ� ������ Ÿ�� */
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
*	\brief ��ü�� ���� ����� ����ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu) 
*	\date 12 May 2015	
*/
class AFX_EXT_CLASS PSelector
{
public:
	// ������ �� �Ҹ���
	PSelector();
	PSelector(const PSelector &cpy);
	virtual ~PSelector();

	// ���� ������
	PSelector &operator =(const PSelector &rhs);

	// ��ü�� �����ϰų� �����ϱ� ���� ���콺 �̺�Ʈ ó�� �����Լ�
	virtual void LButtonDown(int x, int y, UINT nFlags);	// Ȯ��
	virtual void MouseMove(int x, int y, UINT nFlags);		// Ȯ��
	virtual void LButtonUp(int x, int y, UINT nFlags);		// Ȯ��
	virtual void MouseWheel(int delta);						// Ȯ��
	GVector3 GetWorldCoord(int x, int y);					// Ȯ��

	// �ʱ�ȭ �� ������ �Լ�
	void Reset();	// Ȯ��
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

	// ��ü ������ ���� ������ �Լ�
	void RenderForSelect();
	void RenderForSelectFace();
	void RenderForSelectHandle();

	// ��ü ���� �Լ�
	void EnterSelectMode(int x, int y);			// Ȯ��
	int	 ExitSelectMode();						// Ȯ��
	bool SelectMesh(std::string Name);			// Ȯ��
	bool SelectMesh(int x, int y);				// Ȯ��
	bool SelectBone(int x, int y);				// Ȯ��

	void SelectFace(int x, int y);				// Ȯ��
	void SelectFaceAndCutBndry(int x, int y);
	void SelectFaceByBrush(GMesh *pMesh);		// Ȯ��

	void SelectPtOnFace(int x, int y);
	void SelectFaceGroup(int x, int y);			// Ȯ��
	
	void SelectVert(int x, int y);				// Ȯ��
	void SelectVertByBrush(GMesh *pMesh);		// Ȯ��
	bool SelectCtrlPt(int x, int y);			// Ȯ��
	bool SetMouseOverEditHandle(int x, int y);	// Ȯ��
	bool SetMouseOverInfo(int x, int y);		// Ȯ�� 
	bool IsPtInPolygon(double x, double y, std::vector<GVector2> &PtList);	// Ȯ��

	// ���������� ���
	void SelectFaceByClick(GMesh *pMesh);		// Ȯ��
	void SelectFaceByRgn(GMesh *pMesh);			// Ȯ��
	void GetPixelList(CPoint p, CPoint q, std::vector<CPoint> &PixelList);
	void GetPixelColor(int x, int y, GLubyte *pixel);
	int FindCutPtList(GMesh *pMesh, std::vector<GCutPt> &CutPtList);	// �̻��

public:	// ������ ���(10)
	/*! \brief �����Ϳ� ���� ������ */
	PEditor *pEditor;

	/*! \brief ���� �� ����(�簢��, �ð���, �ٰ���, �귯��) */
	TypeSelectTool SelectToolType;

	/*! \brief ���� ��� (�ո�, �ո�� �޸� ���) */
	TypeSelectMode SelectModeType;

	/*! \brief ���� ��ü ����(�޽�, ����, �ﰢ��, ��, ������ ��) */
	TypeSelectObj SelectObjType;

	/*! \brief ���� ���콺 �����Ϳ� ������ �ﰢ�� �׷��� �ε��� */
	int MouseOverGroupIdx;

	/*! \brief ���� ���콺 �����Ϳ� ������ �ﰢ���� �ε��� */
	int MouseOverFaceIdx;

	/*! \brief ���� ���콺 �����Ϳ� ���� ����� ������ �ε��� */
	int MouseOverVertIdx;

	/*! \brief ���� ���콺 �����Ϳ� ������ �ﰢ������ ���� Barycentric ��ǥ */
	GVector3 MouseOverPtCoords;

	/*! \brief ������ ��ǥ�迡�� ǥ���� ���� ������ �����ڸ� ���� ����Ʈ */
	std::vector<GVector2> SltRgn2DPtList;

	/*! \brief ���� ��ǥ�迡�� ǥ���� ���� ������ �����ڸ� ���� ����Ʈ (�������� ���� ����) */
	std::vector<GVector3> SltRgn3DPtList;

	/*! \biref OpenGL ��ŷ�� ���ù��� */
	GLuint SelectBuf[SELECT_BUFFER_SIZE];
};

/*!
*	\class PEditor
*	\brief 3���� ��鳻�� ���Ե� ��ü�� ���� ����� ����ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu) 
*	\date 02 Sept. 2012	
*/
class AFX_EXT_CLASS PEditor
{
public:
	// ������ �� �Ҹ���
	PEditor();
	PEditor(const PEditor &cpy);
	virtual ~PEditor();

	// ���Կ�����
	PEditor &operator =(const PEditor &rhs);

	// �÷����� ������ ���� �Լ�
	virtual void InitPlugIn(GScene *_pScene, CDialog *pParent, CDocument *pDoc);

	// ��ü�� �����ϰų� �����ϱ� ���� ���콺 �̺�Ʈ ó�� �����Լ�
	virtual void LButtonDown(int x, int y, UINT nFlags);
	virtual void MouseMove(int x, int y, UINT nFlags);
	virtual void LButtonUp(int x, int y, UINT nFlags);
	virtual void MouseWheel(int delta);

	// ���� ��ǥ�� ���� �Լ�
	void SetEC();		//Ȯ��
	GTransf GetEC();

	// ���� ���� �Լ�
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

	// ������ ����� ���� �Լ���
	void FindEditWgt();
	void FindEditWgtForSculpt();
	void FindEditRgn(GMesh *pMesh, std::vector<double> &Distances, double EditDist);
	void FindEditBndry(GMesh *pMesh, std::vector<double> &Distances, double EditDist);
	void FindEditBndry(GMesh *pMesh, std::vector<double> &Distances, std::vector<double> &Angles, double EditDist);
	
	// ������ �Լ�
	void Render();
	void RenderEditRgn();
	
	// �̵� ���� ������ �Լ�
	void RenderTra(double len);
	void RenderTraHandleX(double len);
	void RenderTraHandleY(double len);
	void RenderTraHandleZ(double len);
	void RenderTraHandleXY(double len);
	void RenderTraHandleYZ(double len);
	void RenderTraHandleZX(double len);
	void RenderTraHandleXYZ(double len);

	// ȸ�� ���� ������ �Լ�
	void RenderRot(double len);
	void RenderRotHandleX(const GVector3 &view_aixs, double len);
	void RenderRotHandleY(const GVector3 &view_aixs, double len);
	void RenderRotHandleZ(const GVector3 &view_aixs, double len);

	// ȸ�� ���� �ڵ� ���� �Լ�
	bool SelectEditHandle(int x, int y);

	// ������ ���� ������ �Լ�
	void RenderScl(double len);
	void RenderSclHandleX(double len);
	void RenderSclHandleY(double len);
	void RenderSclHandleZ(double len);
	void RenderSclHandleXY(double len);
	void RenderSclHandleYZ(double len);
	void RenderSclHandleZX(double len);
	void RenderSclHandleXYZ(double len);

	// ���� ��ü ���� �Լ�
	GMesh *GetEditMesh();
	std::string GetClsNameEditObj();
	int  GetNumEditObj();
	void GetRefMeshList(std::vector<GMesh *> &MeshList);
	void AddToEditObjList(GEditObj *pObj);
	bool DelFromEditObjList(GEditObj *pObj);
	void ToggleEditObject(GEditObj *pObj);
	void UniqueEditObjList();	// Ȯ��
	void DeleteObject();
	void DeleteMeshVert();
	void DeleteMeshFace();
	void ShowMesh();
	void HideMesh();

	// �⺻ ������ ���(11)
public:
	/*! \brief ���� ���� ���� ��ǥ�� (��ü ���ý� ������Ʈ) */
	GTransf EC;

	/*! \brief ���� �� ����(�̵�, ȸ��, ������, Sculpt, Smooth ��) */
	TypeEditTool EditToolType;
	
	/*! \brief ���õ� ���� ��ü ����Ʈ(����, �ﰢ��, ���� ��ü ��) */
	std::vector<GEditObj *> EditObjList;

	/*! \brief �ε巯�� ������ ���� �߰��� ���õ� ������ ����Ʈ(���� ��ü�� ���� �Ǵ� �ﰢ���� ���) */
	std::vector<GVertex *> EditVertRgn; 

	/*! \brief �ε巯�� ������ ���� �߰��� ���õ� �ﰢ���� ����Ʈ(������ ��) */
	std::vector<GFace *> EditFaceRgn;

	/*! \brief �ε巯�� ������ ���� �߰��� ���õ� �ﰢ���� ����Ʈ�� BVH */
	PQP_Model *pEditFaceRgnBvh;

	/*! \brief ���� ������ ��� ������ ����Ʈ */
	std::vector<GPoint3> EditBndryPts;

	/*! \brief ���� ������ 2���� �Ű�ȭ �޽� */
	GMesh *pUnfoldMesh;

	/*! \brief 2���� �Ű�ȭ �޽��� 3���� ���� �������� �ﰢ�� ���� ���� (2���� �ﰢ�� �ε���, 3���� �ﰢ�� �ε���) */
	std::map<int, int> UnfoldFaceMap;

	/*! \brief ���� �� ������ ���� ���콺�� ��ġ */
	CPoint MousePt[2];

	/*! \brief ȸ�� ������ �� ���, ���� ȸ�� �ڵ��� ���õ� ���� ��ǥ�� */
	GTransf RotRefX;

	/*! \brief ȸ�� ������ �� ���, ȸ�� �ڵ��� ���õ� ���� */
	GVector3 RotRefPos;	

	// �߰� ������ ���(8)
public:	
	/*! \brief ���� ���α׷��� ��鿡 ���� ������ */
	GScene *pScene;

	/*! \brief ���� ��ü ������ ���� ��ü */
	PSelector theSelector;

	/*! \brief �޽� ���� ��ȭ���� */
	PEditMeshDlg EditMeshDlg;

	/*! \brief � ���� ��ȭ���� */
	PEditCrvDlg EditCrvDlg;

	/*! \brief ��� ���� ��ȭ���� */
	PEditSrfDlg EditSrfDlg;

	/*! \brief ���� ���� ��ȭ���� */
	PEditVolDlg EditVolDlg;

	/*! \brief ���� ���� ��ȭ���� */
	PEditSwpDlg EditSwpDlg;

	/*! \brief ���� ��� ���� ��ȭ���� */
	PEditBlendSrfDlg EditBlendSrfDlg;
};

#endif // _PEDITOR_H