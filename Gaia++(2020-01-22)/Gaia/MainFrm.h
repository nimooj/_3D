#pragma once

#include "Src/GDialogBar.h"
#include "Src/GToolBar.h"
#include "Src/GWorkSpaceDlg.h"
#include "Src/GSceneTreeDlg.h"
#include "Src/GAnimDlg.h"
#include "Src/GDentalCadDlg.h"
#include "Src/GOutputDlg.h"
#include "Src/GDepthViewDlg.h"
//#include "Src/GSplitter.h"

class CMainFrame : public CFrameWnd
{
	friend class GaiaView;
	friend class GaiaDoc;
protected:
	// ������
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

public:
	// �Ҹ���
	virtual ~CMainFrame();

public:
	// �����Լ� ������
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// ����Լ�
	bool CreateToolBar();
	bool CreateStatusBar();
	bool CreateWorkSpaceBar();
	bool CreateAnimBar();
	bool CreateOutputBar();
	void UpdateWorkSpaceDlg();
	void DockControlBarLeftOf(CToolBar* Bar, CToolBar* LeftOf);
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	// ������ ���
	// ����� ���� ����
	GToolBar m_wndToolBar;
	GToolBar m_wndToolEditBar;
	GToolBar m_wndToolRenderTypeBar;
	GToolBar m_wndToolNurbsCreatorBar;
	GToolBar m_wndToolPolygonCreatorBar;
	CBrush m_BackGroundBrush;

	GDialogBar m_wndWorkSpaceBar;	// �۾������� ���̾�α׹�
	GWorkSpaceDlg m_WorkSpaceDlg;	// �� ��Ʈ���� ���� ��ȭ���� (m_wndWorkSpaceBar�� ����)
	GSceneTreeDlg m_SceneTreeDlg;	// Ʈ���並 ���� ��ȭ���� (m_WorkSpaceDlg�� ����)
		
	// �ִϸ��̼� ���̾�α׹�
	GDialogBar m_wndAnimDlgBar;
	GAnimDlg m_AnimDlg;

	// �޽��� ��� ���̾�α׹�
	GDialogBar m_wndOutputDlgBar;
	GOutputDlg m_OutputDlg;
		
	// ���¹�
	CStatusBar m_wndStatusBar;	

	// FPS üũ ����
	BOOL m_CheckFPS;

	// �޽��� ó���Լ�
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	// �ҽ� ��ü ����
	afx_msg void OnCreateNurbsCrv();
	afx_msg void OnCreateNurbsSrf();
	afx_msg void OnCreateNurbsCircle();
	afx_msg void OnCreateNurbsTeapot();

	// �ҽ� Ȯ�� ��ü ����
	afx_msg void OnCreateCrvFromSketch();
	afx_msg void OnCreateCrvFromGeoPath();
	afx_msg void OnCreateSweepSrf();
	afx_msg void OnCreateTriBlendSrf();
	
	// ������ ��ü ����
	afx_msg void OnCreatePolygon();
	afx_msg void OnCreatePolygonSphere();
	afx_msg void OnCreatePolygonTeapot();
	afx_msg void OnCreatePolygonPlane();

	// ���ܼ� ����
	afx_msg void OnCreateCutLine();
	afx_msg void OnCreateCutLineFromSketch();

	afx_msg void OnCreateRagdoll();
	afx_msg void OnDeformBox();

	// ���� �� �Լ�
	afx_msg void OnSelectToolRect();
	afx_msg void OnUpdateSelectToolRect(CCmdUI *pCmdUI);

	afx_msg void OnSelectToolCircle();
	afx_msg void OnUpdateSelectToolCircle(CCmdUI *pCmdUI);

	afx_msg void OnSelectToolLasso();
	afx_msg void OnUpdateSelectToolLasso(CCmdUI *pCmdUI);

	afx_msg void OnSelectToolPoly();
	afx_msg void OnUpdateSelectToolPoly(CCmdUI *pCmdUI);

	afx_msg void OnSelectToolPaint();
	afx_msg void OnUpdateSelectToolPaint(CCmdUI *pCmdUI);

	// ���� �� �Լ�
	afx_msg void OnEditTra();
	afx_msg void OnUpdateEditTra(CCmdUI *pCmdUI);

	afx_msg void OnEditRot();
	afx_msg void OnUpdateEditRot(CCmdUI *pCmdUI);

	afx_msg void OnEditScl();
	afx_msg void OnUpdateEditScl(CCmdUI *pCmdUI);

	// ���� ��ü ���� �Լ�
	afx_msg void OnSelectMesh();
	afx_msg void OnUpdateSelectMesh(CCmdUI *pCmdUI);

	afx_msg void OnSelectVertex();
	afx_msg void OnUpdateSelectVertex(CCmdUI *pCmdUI);

	afx_msg void OnSelectFace();
	afx_msg void OnUpdateSelectFace(CCmdUI *pCmdUI);

	afx_msg void OnSelectFaceGroup();
	afx_msg void OnUpdateSelectFaceGroup(CCmdUI *pCmdUI);

	afx_msg void OnSelectFaceAndCut();
	afx_msg void OnUpdateSelectFaceAndCut(CCmdUI *pCmdUI);

	afx_msg void OnSelectPtOnFace();
	afx_msg void OnUpdateSelectPtOnFace(CCmdUI *pCmdUI);
	
	afx_msg void OnSelectCtrlPt();
	afx_msg void OnUpdateSelectCtrlPt(CCmdUI *pCmdUI);

	afx_msg void OnSelectBoneMesh();
	afx_msg void OnUpdateSelectBoneMesh(CCmdUI *pCmdUI);

	afx_msg void OnEditGeodesicPath();
	afx_msg void OnUpdateEditGeodesicPath(CCmdUI *pCmdUI);

	// ������ Ÿ�� ���� �Լ�
	afx_msg void OnRenderTypePolygon();
	afx_msg void OnUpdateRenderTypePolygon(CCmdUI *pCmdUI);

	afx_msg void OnRenderTypeWire();
	afx_msg void OnUpdateRenderTypeWire(CCmdUI *pCmdUI);

	afx_msg void OnRenderTypePoint();
	afx_msg void OnUpdateRenderTypePoint(CCmdUI *pCmdUI);

	afx_msg void OnShadeFlat();
	afx_msg void OnUpdateShadeFlat(CCmdUI *pCmdUI);

	afx_msg void OnShadeSmooth();
	afx_msg void OnUpdateShadeSmooth(CCmdUI *pCmdUI);

	afx_msg void OnVertexColorIllumination();
	afx_msg void OnUpdateVertexColorIllumination(CCmdUI *pCmdUI);

	afx_msg void OnVertexColorNormal();
	afx_msg void OnUpdateVertexColorNormal(CCmdUI *pCmdUI);

	afx_msg void OnVertexColorBoneWeights();
	afx_msg void OnUpdateVertexColorBoneWeights(CCmdUI *pCmdUI);

	afx_msg void OnRenderCtrlPt();
	afx_msg void OnUpdateRenderCtrlPt(CCmdUI *pCmdUI);

	afx_msg void OnRenderTexture();
	afx_msg void OnUpdateRenderTexture(CCmdUI *pCmdUI);

	afx_msg void OnRenderNormal();
	afx_msg void OnUpdateRenderNormal(CCmdUI *pCmdUI);

	afx_msg void OnRenderCullBackFace();
	afx_msg void OnUpdateRenderCullBackFace(CCmdUI *pCmdUI);

	afx_msg void OnRenderTypeBoundary();
	afx_msg void OnUpdateRenderTypeBoundary(CCmdUI *pCmdUI);

	afx_msg void OnDisplayBackground();
	afx_msg void OnUpdateDisplayBackground(CCmdUI *pCmdUI);

	// UI ������� �Լ�
	afx_msg void OnDisplayGrid();
	afx_msg void OnUpdateDisplayGrid(CCmdUI *pCmdUI);

	afx_msg void OnHeadsupdisplayFramerate();
	afx_msg void OnUpdateHeadsupdisplayFramerate(CCmdUI *pCmdUI);

	afx_msg void OnHeadsupdisplayOriginaxis();
	afx_msg void OnUpdateHeadsupdisplayOriginaxis(CCmdUI *pCmdUI);

	afx_msg void OnHeadsupdisplayViewcube();
	afx_msg void OnUpdateHeadsupdisplayViewcube(CCmdUI *pCmdUI);

	afx_msg void OnViewWorkspace();
	afx_msg void OnUpdateViewWorkspace(CCmdUI *pCmdUI);

	afx_msg void OnViewEditTool();
	afx_msg void OnUpdateViewEditTool(CCmdUI *pCmdUI);

	afx_msg void OnViewRenderTypeBar();
	afx_msg void OnUpdateViewRenderTypeBar(CCmdUI *pCmdUI);

	afx_msg void OnViewNurbsCreateBar();
	afx_msg void OnUpdateViewNurbsCreateBar(CCmdUI *pCmdUI);

	afx_msg void OnViewPolygonCreateBar();
	afx_msg void OnUpdateViewPolygonCreateBar(CCmdUI *pCmdUI);

	afx_msg void OnViewAnimateBar();
	afx_msg void OnUpdateViewAnimateBar(CCmdUI *pCmdUI);

	afx_msg void OnViewOutputBar();
	afx_msg void OnUpdateViewOutputBar(CCmdUI *pCmdUI);

	// �𵨸� ��ǥ�� ���� �Լ���
	afx_msg void OnUpdateMcFromWc();
	afx_msg void OnUpdateMcFromAabb();
	afx_msg void OnUpdateMcFromObb();

	// �޽� ����(clean) �Լ�
	afx_msg void OnFindNonManifoldFaces();
	afx_msg void OnFindDegeneratedFaces();
	afx_msg void OnFindIsolatedVerts();
	afx_msg void OnFindHoles();
	afx_msg void OnFillHoleDiskType();

	// ��Ÿ �Լ���
	afx_msg void OnSewBoundary();
	afx_msg void OnRepeatCommand();
	
	afx_msg void OnEditCutMesh();
	afx_msg void OnNormalizeSkinWeights();
	afx_msg void OnFillHole();
	
	afx_msg void OnCreateCharacterMale();
	afx_msg void OnCreateCharacterFemale();
	afx_msg void OnMergeMeshes();	

	// ���� ��� �Լ�
	afx_msg void OnSelectModeFrontOnly();
	afx_msg void OnSelectModeFrontAndBack();
	afx_msg void OnUpdateSelectModeFrontOnly(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSelectModeFrontAndBack(CCmdUI *pCmdUI);
	afx_msg void OnCheckFps();
	afx_msg void OnUpdateCheckFps(CCmdUI *pCmdUI);
	afx_msg void OnCreatePolygonDisk();
	afx_msg void OnCreateNurbsFromMesh();
	afx_msg void OnCreateTriangleStrip();
	afx_msg void OnCreatePtsOnMesh();
	afx_msg void OnCreateGeodesicPath();
	afx_msg void OnCreateMirrorMesh();
	afx_msg void OnCreateCoonsSrf();
	afx_msg void OnCreateGeodesicMesh();
	afx_msg void OnCreateCoonsMesh();
	afx_msg void OnCreateCoonsMeshList();
	afx_msg void OnFitMeshByOffset();
	afx_msg void OnComputeMeshArea();
	afx_msg void OnComputeCurvature();
	afx_msg void OnCreateRuledSrf();
	afx_msg void OnCreateDepthMap();
	afx_msg void OnOptimizeCutLine();	
	afx_msg void OnAddClippingPlane();
	afx_msg void OnCreateUnfoldedMeshCircle();
	afx_msg void OnCreateUnfoldedMeshSquare();
	afx_msg void OnCreatePqpBvh();
	afx_msg void OnDeletePqpBvh();
	afx_msg void OnEditSculpt();
	afx_msg void OnUpdateEditSculpt(CCmdUI *pCmdUI);
	afx_msg void OnEditSmooth();
	afx_msg void OnUpdateEditSmooth(CCmdUI *pCmdUI);
	afx_msg void OnEditWireframe();
	afx_msg void OnEditLocalRefine();
	afx_msg void OnSetBridgeStartCrv();
	afx_msg void OnSetBridgeEndCrv();
	afx_msg void OnCreateBridgeMesh();
	afx_msg void OnCreatePolygonIcosahedron();
};

