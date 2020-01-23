// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "Gaia.h"
#include "GaiaView.h"
#include "MainFrm.h"

#include "GAvatar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_CREATE_NURBS_CRV, &CMainFrame::OnCreateNurbsCrv)
	ON_COMMAND(ID_CREATE_NURBS_SRF, &CMainFrame::OnCreateNurbsSrf)
	ON_COMMAND(ID_SELECT_TOOL_RECT, &CMainFrame::OnSelectToolRect)
	ON_COMMAND(ID_SELECT_TOOL_LASSO, &CMainFrame::OnSelectToolLasso)
	ON_UPDATE_COMMAND_UI(ID_SELECT_TOOL_RECT, &CMainFrame::OnUpdateSelectToolRect)
	ON_UPDATE_COMMAND_UI(ID_SELECT_TOOL_LASSO, &CMainFrame::OnUpdateSelectToolLasso)
	ON_COMMAND(ID_EDIT_TRA, &CMainFrame::OnEditTra)
	ON_COMMAND(ID_EDIT_ROT, &CMainFrame::OnEditRot)
	ON_COMMAND(ID_EDIT_SCL, &CMainFrame::OnEditScl)
	ON_UPDATE_COMMAND_UI(ID_EDIT_TRA, &CMainFrame::OnUpdateEditTra)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ROT, &CMainFrame::OnUpdateEditRot)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SCL, &CMainFrame::OnUpdateEditScl)
	ON_COMMAND(ID_CREATE_POLYGON, &CMainFrame::OnCreatePolygon)
	ON_COMMAND(ID_SELECT_MESH, &CMainFrame::OnSelectMesh)
	ON_COMMAND(ID_SELECT_VERTEX, &CMainFrame::OnSelectVertex)
	ON_COMMAND(ID_SELECT_FACE, &CMainFrame::OnSelectFace)
	ON_COMMAND(ID_SELECT_CTRL_PT, &CMainFrame::OnSelectCtrlPt)
	ON_UPDATE_COMMAND_UI(ID_SELECT_MESH, &CMainFrame::OnUpdateSelectMesh)
	ON_UPDATE_COMMAND_UI(ID_SELECT_VERTEX, &CMainFrame::OnUpdateSelectVertex)
	ON_UPDATE_COMMAND_UI(ID_SELECT_FACE, &CMainFrame::OnUpdateSelectFace)
	ON_UPDATE_COMMAND_UI(ID_SELECT_CTRL_PT, &CMainFrame::OnUpdateSelectCtrlPt)
	ON_COMMAND(ID_RENDER_TYPE_POLYGON, &CMainFrame::OnRenderTypePolygon)
	ON_COMMAND(ID_RENDER_TYPE_WIRE, &CMainFrame::OnRenderTypeWire)
	ON_COMMAND(ID_RENDER_TYPE_POINT, &CMainFrame::OnRenderTypePoint)
	ON_UPDATE_COMMAND_UI(ID_RENDER_TYPE_POLYGON, &CMainFrame::OnUpdateRenderTypePolygon)
	ON_UPDATE_COMMAND_UI(ID_RENDER_TYPE_WIRE, &CMainFrame::OnUpdateRenderTypeWire)
	ON_UPDATE_COMMAND_UI(ID_RENDER_TYPE_POINT, &CMainFrame::OnUpdateRenderTypePoint)
	ON_COMMAND(ID_SHADE_FLAT, &CMainFrame::OnShadeFlat)
	ON_COMMAND(ID_SHADE_SMOOTH, &CMainFrame::OnShadeSmooth)
	ON_UPDATE_COMMAND_UI(ID_SHADE_FLAT, &CMainFrame::OnUpdateShadeFlat)
	ON_UPDATE_COMMAND_UI(ID_SHADE_SMOOTH, &CMainFrame::OnUpdateShadeSmooth)
	ON_COMMAND(ID_VERTEX_COLOR_ILLUMINATION, &CMainFrame::OnVertexColorIllumination)
	ON_COMMAND(ID_VERTEX_COLOR_NORMAL, &CMainFrame::OnVertexColorNormal)
	ON_UPDATE_COMMAND_UI(ID_VERTEX_COLOR_ILLUMINATION, &CMainFrame::OnUpdateVertexColorIllumination)
	ON_UPDATE_COMMAND_UI(ID_VERTEX_COLOR_NORMAL, &CMainFrame::OnUpdateVertexColorNormal)
	ON_COMMAND(ID_RENDER_TEXTURE, &CMainFrame::OnRenderTexture)
	ON_COMMAND(ID_RENDER_NORMAL, &CMainFrame::OnRenderNormal)
	ON_COMMAND(ID_RENDER_CULL_BACK_FACE, &CMainFrame::OnRenderCullBackFace)
	ON_COMMAND(ID_RENDER_CTRL_PT, &CMainFrame::OnRenderCtrlPt)
	ON_UPDATE_COMMAND_UI(ID_RENDER_CTRL_PT, &CMainFrame::OnUpdateRenderCtrlPt)
	ON_UPDATE_COMMAND_UI(ID_RENDER_TEXTURE, &CMainFrame::OnUpdateRenderTexture)
	ON_UPDATE_COMMAND_UI(ID_RENDER_NORMAL, &CMainFrame::OnUpdateRenderNormal)
	ON_UPDATE_COMMAND_UI(ID_RENDER_CULL_BACK_FACE, &CMainFrame::OnUpdateRenderCullBackFace)
	ON_COMMAND(ID_CREATE_RAGDOLL, &CMainFrame::OnCreateRagdoll)
	ON_COMMAND(ID_VERTEX_COLOR_BONE_WEIGHTS, &CMainFrame::OnVertexColorBoneWeights)
	ON_UPDATE_COMMAND_UI(ID_VERTEX_COLOR_BONE_WEIGHTS, &CMainFrame::OnUpdateVertexColorBoneWeights)
	ON_COMMAND(ID_SELECT_BONE_MESH, &CMainFrame::OnSelectBoneMesh)
	ON_UPDATE_COMMAND_UI(ID_SELECT_BONE_MESH, &CMainFrame::OnUpdateSelectBoneMesh)
	ON_COMMAND(ID_DISPLAY_GRID, &CMainFrame::OnDisplayGrid)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_GRID, &CMainFrame::OnUpdateDisplayGrid)
	ON_COMMAND(ID_HEADSUPDISPLAY_FRAMERATE, &CMainFrame::OnHeadsupdisplayFramerate)
	ON_COMMAND(ID_HEADSUPDISPLAY_ORIGINAXIS, &CMainFrame::OnHeadsupdisplayOriginaxis)
	ON_COMMAND(ID_HEADSUPDISPLAY_VIEWCUBE, &CMainFrame::OnHeadsupdisplayViewcube)
	ON_UPDATE_COMMAND_UI(ID_HEADSUPDISPLAY_FRAMERATE, &CMainFrame::OnUpdateHeadsupdisplayFramerate)
	ON_UPDATE_COMMAND_UI(ID_HEADSUPDISPLAY_ORIGINAXIS, &CMainFrame::OnUpdateHeadsupdisplayOriginaxis)
	ON_UPDATE_COMMAND_UI(ID_HEADSUPDISPLAY_VIEWCUBE, &CMainFrame::OnUpdateHeadsupdisplayViewcube)
	ON_COMMAND(ID_VIEW_WORKSPACE, &CMainFrame::OnViewWorkspace)
	ON_COMMAND(ID_VIEW_EDIT_TOOL, &CMainFrame::OnViewEditTool)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WORKSPACE, &CMainFrame::OnUpdateViewWorkspace)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDIT_TOOL, &CMainFrame::OnUpdateViewEditTool)
	ON_COMMAND(ID_VIEW_RENDER_TYPE_BAR, &CMainFrame::OnViewRenderTypeBar)
	ON_COMMAND(ID_VIEW_NURBS_CREATE_BA, &CMainFrame::OnViewNurbsCreateBar)
	ON_COMMAND(ID_VIEW_POLYGON_CREATE_BAR, &CMainFrame::OnViewPolygonCreateBar)
	ON_COMMAND(ID_VIEW_ANIMATE_BAR, &CMainFrame::OnViewAnimateBar)
	ON_COMMAND(ID_VIEW_OUTPUT_BAR, &CMainFrame::OnViewOutputBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RENDER_TYPE_BAR, &CMainFrame::OnUpdateViewRenderTypeBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NURBS_CREATE_BA, &CMainFrame::OnUpdateViewNurbsCreateBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_POLYGON_CREATE_BAR, &CMainFrame::OnUpdateViewPolygonCreateBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ANIMATE_BAR, &CMainFrame::OnUpdateViewAnimateBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUT_BAR, &CMainFrame::OnUpdateViewOutputBar)
	ON_COMMAND(ID_RENDER_TYPE_BOUNDARY, &CMainFrame::OnRenderTypeBoundary)
	ON_UPDATE_COMMAND_UI(ID_RENDER_TYPE_BOUNDARY, &CMainFrame::OnUpdateRenderTypeBoundary)
	ON_WM_PAINT()
	ON_COMMAND(ID_CREATE_TRI_BLEND_SRF, &CMainFrame::OnCreateTriBlendSrf)
	ON_COMMAND(ID_SEW_BOUNDARY, &CMainFrame::OnSewBoundary)
	ON_COMMAND(ID_SELECT_TOOL_POLY, &CMainFrame::OnSelectToolPoly)
	ON_UPDATE_COMMAND_UI(ID_SELECT_TOOL_POLY, &CMainFrame::OnUpdateSelectToolPoly)
	ON_COMMAND(ID_CREATE_NURBS_CIRCLE, &CMainFrame::OnCreateNurbsCircle)
	ON_COMMAND(ID_CREATE_SWEEP_SRF, &CMainFrame::OnCreateSweepSrf)
	ON_COMMAND(ID_DISPLAY_BACKGROUND, &CMainFrame::OnDisplayBackground)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_BACKGROUND, &CMainFrame::OnUpdateDisplayBackground)
	ON_COMMAND(ID_REPEAT_COMMAND, &CMainFrame::OnRepeatCommand)	
	ON_COMMAND(ID_SELECT_TOOL_PAINT, &CMainFrame::OnSelectToolPaint)
	ON_UPDATE_COMMAND_UI(ID_SELECT_TOOL_PAINT, &CMainFrame::OnUpdateSelectToolPaint)
	ON_COMMAND(ID_CREATE_CUT_LINE, &CMainFrame::OnCreateCutLine)
	ON_COMMAND(ID_EDIT_CUT_MESH, &CMainFrame::OnEditCutMesh)
	ON_COMMAND(ID_NORMALIZE_SKIN_WEIGHTS, &CMainFrame::OnNormalizeSkinWeights)
	ON_COMMAND(ID_FILL_HOLE, &CMainFrame::OnFillHole)
	ON_COMMAND(ID_UPDATE_MC_FROM_WC, &CMainFrame::OnUpdateMcFromWc)
	ON_COMMAND(ID_UPDATE_MC_FROM_AABB, &CMainFrame::OnUpdateMcFromAabb)
	ON_COMMAND(ID_SELECT_FACE_GROUP, &CMainFrame::OnSelectFaceGroup)
	ON_UPDATE_COMMAND_UI(ID_SELECT_FACE_GROUP, &CMainFrame::OnUpdateSelectFaceGroup)
	ON_COMMAND(ID_FIND_NON_MANIFOLD_FACE, &CMainFrame::OnFindNonManifoldFaces)
	ON_COMMAND(ID_CREATE_CHARACTER_MALE, &CMainFrame::OnCreateCharacterMale)
	ON_COMMAND(ID_CREATE_CHARACTER_FEMALE, &CMainFrame::OnCreateCharacterFemale)
	ON_COMMAND(ID_UPDATE_MC_FROM_OBB, &CMainFrame::OnUpdateMcFromObb)
	ON_COMMAND(ID_SELECT_MODE_FRONT_ONLY, &CMainFrame::OnSelectModeFrontOnly)
	ON_COMMAND(ID_SELECT_MODE_FRONT_AND_BACK, &CMainFrame::OnSelectModeFrontAndBack)
	ON_UPDATE_COMMAND_UI(ID_SELECT_MODE_FRONT_ONLY, &CMainFrame::OnUpdateSelectModeFrontOnly)
	ON_UPDATE_COMMAND_UI(ID_SELECT_MODE_FRONT_AND_BACK, &CMainFrame::OnUpdateSelectModeFrontAndBack)
	ON_COMMAND(ID_CHECK_FPS, &CMainFrame::OnCheckFps)
	ON_UPDATE_COMMAND_UI(ID_CHECK_FPS, &CMainFrame::OnUpdateCheckFps)
	ON_COMMAND(ID_DEFORM_BOX, &CMainFrame::OnDeformBox)
	ON_COMMAND(ID_CREATE_NURBS_TEAPOT, &CMainFrame::OnCreateNurbsTeapot)
	ON_COMMAND(ID_CREATE_POLYGON_SPHERE, &CMainFrame::OnCreatePolygonSphere)
	ON_COMMAND(ID_CREATE_POLYGON_TEAPOT, &CMainFrame::OnCreatePolygonTeapot)
	ON_COMMAND(ID_CREATE_POLYGON_PLANE, &CMainFrame::OnCreatePolygonPlane)
	ON_COMMAND(ID_CREATE_POLYGON_DISK, &CMainFrame::OnCreatePolygonDisk)
	ON_COMMAND(ID_CREATE_NURBS_FROM_MESH, &CMainFrame::OnCreateNurbsFromMesh)
	ON_COMMAND(ID_CREATE_PTS_ON_MESH, &CMainFrame::OnCreatePtsOnMesh)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_CREATE_GEODESIC_PATH, &CMainFrame::OnCreateGeodesicPath)
	ON_COMMAND(ID_CREATE_MIRROR_MESH, &CMainFrame::OnCreateMirrorMesh)
	ON_COMMAND(ID_EDIT_GEODESIC_PATH, &CMainFrame::OnEditGeodesicPath)
	ON_COMMAND(ID_CREATE_CRV_FROM_GEO_PATH, &CMainFrame::OnCreateCrvFromGeoPath)
	ON_COMMAND(ID_CREATE_COONS_SRF, &CMainFrame::OnCreateCoonsSrf)
	ON_COMMAND(ID_CREATE_GEODESIC_MESH, &CMainFrame::OnCreateGeodesicMesh)
	ON_UPDATE_COMMAND_UI(ID_EDIT_GEODESIC_PATH, &CMainFrame::OnUpdateEditGeodesicPath)
	ON_COMMAND(ID_CREATE_COONS_MESH, &CMainFrame::OnCreateCoonsMesh)
	ON_COMMAND(ID_MERGE_MESHES, &CMainFrame::OnMergeMeshes)
	ON_COMMAND(ID_FIT_MESH_BY_OFFSET, &CMainFrame::OnFitMeshByOffset)
	ON_COMMAND(ID_CREATE_COONS_MESH_LIST, &CMainFrame::OnCreateCoonsMeshList)
	ON_COMMAND(ID_COMPUTE_MESH_AREA, &CMainFrame::OnComputeMeshArea)
	ON_COMMAND(ID_COMPUTE_CURVATURE, &CMainFrame::OnComputeCurvature)
	ON_COMMAND(ID_SELECT_PT_ON_FACE, &CMainFrame::OnSelectPtOnFace)
	ON_UPDATE_COMMAND_UI(ID_SELECT_PT_ON_FACE, &CMainFrame::OnUpdateSelectPtOnFace)
	ON_COMMAND(ID_CREATE_RULED_SRF, &CMainFrame::OnCreateRuledSrf)
	ON_COMMAND(ID_CREATE_DEPTH_MAP, &CMainFrame::OnCreateDepthMap)
	ON_COMMAND(ID_OPTIMIZE_CUT_LINE, &CMainFrame::OnOptimizeCutLine)
	ON_COMMAND(ID_SELECT_FACE_AND_CUT, &CMainFrame::OnSelectFaceAndCut)
	ON_UPDATE_COMMAND_UI(ID_SELECT_FACE_AND_CUT, &CMainFrame::OnUpdateSelectFaceAndCut)
	ON_COMMAND(ID_SELECT_TOOL_CIRCLE, &CMainFrame::OnSelectToolCircle)
	ON_UPDATE_COMMAND_UI(ID_SELECT_TOOL_CIRCLE, &CMainFrame::OnUpdateSelectToolCircle)
	ON_COMMAND(ID_ADD_CLIPPING_PLANE, &CMainFrame::OnAddClippingPlane)
	ON_COMMAND(ID_CREATE_UNFOLDED_MESH_CIRCLE, &CMainFrame::OnCreateUnfoldedMeshCircle)
	ON_COMMAND(ID_CREATE_UNFOLDED_MESH_SQUARE, &CMainFrame::OnCreateUnfoldedMeshSquare)
	ON_COMMAND(ID_CREATE_CRV_FROM_SKETCH, &CMainFrame::OnCreateCrvFromSketch)
	ON_COMMAND(ID_CREATE_CUT_LINE_FROM_SKETCH, &CMainFrame::OnCreateCutLineFromSketch)
	ON_COMMAND(ID_CREATE_PQP_BVH, &CMainFrame::OnCreatePqpBvh)
	ON_COMMAND(ID_DELETE_PQP_BVH, &CMainFrame::OnDeletePqpBvh)
	ON_COMMAND(ID_EDIT_SCULPT, &CMainFrame::OnEditSculpt)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SCULPT, &CMainFrame::OnUpdateEditSculpt)
	ON_COMMAND(ID_EDIT_SMOOTH, &CMainFrame::OnEditSmooth)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SMOOTH, &CMainFrame::OnUpdateEditSmooth)
	ON_COMMAND(ID_EDIT_WIREFRAME, &CMainFrame::OnEditWireframe)
	ON_COMMAND(ID_CREATE_TRIANGLE_STRIP, &CMainFrame::OnCreateTriangleStrip)
	ON_COMMAND(ID_FIND_DEGENERATED_FACES, &CMainFrame::OnFindDegeneratedFaces)
	ON_COMMAND(ID_FIND_ISOLATED_VERTS, &CMainFrame::OnFindIsolatedVerts)
	ON_COMMAND(ID_FIND_HOLES, &CMainFrame::OnFindHoles)
	ON_COMMAND(ID_EDIT_LOCAL_REFINE, &CMainFrame::OnEditLocalRefine)
	ON_COMMAND(ID_SET_BRIDGE_START_CRV, &CMainFrame::OnSetBridgeStartCrv)
	ON_COMMAND(ID_SET_BRIDGE_END_CRV, &CMainFrame::OnSetBridgeEndCrv)
	ON_COMMAND(ID_CREATE_BRIDGE_MESH, &CMainFrame::OnCreateBridgeMesh)
	ON_COMMAND(ID_CREATE_POLYGON_ICOSAHEDRON, &CMainFrame::OnCreatePolygonIcosahedron)
	ON_COMMAND(ID_FILL_HOLE_DISK_TYPE, &CMainFrame::OnFillHoleDiskType)
	END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

void (CMainFrame::*pRepeatFunc)() = NULL;

void CMainFrame::OnRepeatCommand()
{
	if (pRepeatFunc != NULL)
		(theMainFrm().*pRepeatFunc)();
}

// CMainFrame 생성/소멸
CMainFrame::CMainFrame()
{
	m_CheckFPS = FALSE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 기본 툴바를 생성하고,
	if (!CreateToolBar())
		return -1;

	// 작업공간바를 생성하고,
	if (!CreateWorkSpaceBar())
		return -1;

	// 상태바를 생성하고,
	if (!CreateStatusBar())
		return -1;

	// 애니메이션제어바를 생성하고,
	if (!CreateAnimBar())
		return -1;

	// 메시시출력바를 생성한다.
	if (!CreateOutputBar())
		return -1;

	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndWorkSpaceBar);
	DockControlBar(&m_wndToolBar);
	DockControlBarLeftOf(&m_wndToolNurbsCreatorBar, &m_wndToolBar);
	DockControlBarLeftOf(&m_wndToolPolygonCreatorBar, &m_wndToolNurbsCreatorBar);
	DockControlBarLeftOf(&m_wndToolRenderTypeBar, &m_wndToolPolygonCreatorBar);
	DockControlBar(&m_wndToolEditBar);
	DockControlBar(&m_wndAnimDlgBar);
	DockControlBar(&m_wndOutputDlgBar);
	
	// 상태바의 색상을 변경한다.
	unsigned char *BC = theScene().UIBkgColor;
	m_wndStatusBar.GetStatusBarCtrl().SetBkColor(RGB(BC[0], BC[1], BC[2]));

	return 0;
}

bool CMainFrame::CreateToolBar()
{
	// 표준 툴바 생성
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return false;      // fail to create
	}
	m_wndToolBar.LoadTrueColorToolBar(34, IDB_TOOLBAR);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	
	// 편집 툴바 생성
	if (!m_wndToolEditBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_LEFT
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolEditBar.LoadToolBar(IDR_TOOLBAR_EDIT))
	{
		TRACE0("Failed to create toolbar\n");
		return false;      // fail to create
	}
	m_wndToolEditBar.LoadTrueColorToolBar(34, IDB_TOOLBAR_EDIT, IDB_TOOLBAR_EDIT_HOT);
	m_wndToolEditBar.EnableDocking(CBRS_ALIGN_ANY);

	// 렌더링 타입 툴바 생성
	if (!m_wndToolRenderTypeBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_LEFT
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolRenderTypeBar.LoadToolBar(IDR_TOOLBAR_RENDER_TYPE))
	{
		TRACE0("Failed to create toolbar\n");
		return false;      // fail to create
	}
	m_wndToolRenderTypeBar.LoadTrueColorToolBar(34, IDB_TOOLBAR_RENDER_TYPE1, IDB_TOOLBAR_RENDER_TYPE2);
	m_wndToolRenderTypeBar.EnableDocking(CBRS_ALIGN_ANY);

	// 넙스 객체 생성 툴바 생성
	if (!m_wndToolNurbsCreatorBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolNurbsCreatorBar.LoadToolBar(IDR_TOOLBAR_CREATE_NURBS))
	{
		TRACE0("Failed to create toolbar\n");
		return false;      // fail to create
	}
	m_wndToolNurbsCreatorBar.LoadTrueColorToolBar(34, IDB_TOOLBAR_CREATE_NURBS, IDB_TOOLBAR_CREATE_NURBS_HOT);
	m_wndToolNurbsCreatorBar.EnableDocking(CBRS_ALIGN_ANY);

	// 폴리곤 객체 생성 툴바 생성
	if (!m_wndToolPolygonCreatorBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolPolygonCreatorBar.LoadToolBar(IDR_TOOLBAR_CREATE_POLYGON))
	{
		TRACE0("Failed to create toolbar\n");
		return false;      // fail to create
	}
	m_wndToolPolygonCreatorBar.LoadTrueColorToolBar(34, IDB_TOOLBAR_CREATE_POLYGON, IDB_TOOLBAR_CREATE_POLYGON_HOT);
	m_wndToolPolygonCreatorBar.EnableDocking(CBRS_ALIGN_ANY);

	return true;
}

void CMainFrame::DockControlBarLeftOf(CToolBar* Bar, CToolBar* LeftOf)
{
	CRect rect;
	DWORD dw;
	UINT n;

	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate
	RecalcLayout(TRUE);

	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw=LeftOf->GetBarStyle();
	n = 0;
	n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;

	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a separate line. By calculating a rectangle, we
	// are simulating a Toolbar being dragged to that location and docked.
	DockControlBar(Bar, n, &rect);
}

bool CMainFrame::CreateStatusBar()
{
	if (!m_wndStatusBar.Create(this) || 
		!m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return false;     // fail to create
	}
	return true;
}

bool CMainFrame::CreateWorkSpaceBar()
{
	// GWorkSpaceDlg를 포함한 도킹이 가능한 다이얼로그바를 생성하고,
 	if (!m_wndWorkSpaceBar.Create(this, &m_WorkSpaceDlg, CString("WorkSpace Bar"), IDD_WORKSPACE_DLG, WS_CHILD | WS_VISIBLE | CBRS_RIGHT))
 	{
 		TRACE0("Failed to create dialogbar\n");
 		return false;      // fail to create
 	}
	m_wndWorkSpaceBar.SetBarStyle(m_wndWorkSpaceBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndWorkSpaceBar.EnableDocking(CBRS_ALIGN_ANY);
 	
	// m_SceneTreeDlg 대화상자 생성하여,
	m_SceneTreeDlg.Create(IDD_WORKSPACE_TREE_DLG, &m_WorkSpaceDlg);

	// m_WorkSpaceDlg에 추가한다.
 	m_WorkSpaceDlg.InsertTabDlg(&m_SceneTreeDlg, _T("Scene"), false);

	return true;
}

bool CMainFrame::CreateAnimBar()
{
	// GAnimDlg를 포함한 도킹이 가능한 다이얼로그바를 생성
	if (!m_wndAnimDlgBar.Create(this, &m_AnimDlg, CString("Animation Control Bar"), IDD_ANIM_DLG, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM))
	{
		TRACE0("Failed to create dialogbar\n");
		return false;      // fail to create
	}
	m_wndAnimDlgBar.SetBarStyle(m_wndAnimDlgBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_BOTTOM);
	m_wndAnimDlgBar.EnableDocking(CBRS_ALIGN_ANY);

	return true;
}

bool CMainFrame::CreateOutputBar()
{
	// GOutputDlg를 포함한 도킹이 가능한 다이얼로그바를 생성
	if (!m_wndOutputDlgBar.Create(this, &m_OutputDlg, CString("Output Bar"), IDD_OUTPUT_DLG, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM))
	{
		TRACE0("Failed to create dialogbar\n");
		return false;      // fail to create
	}
	m_wndOutputDlgBar.SetBarStyle(m_wndOutputDlgBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_BOTTOM);
	m_wndOutputDlgBar.EnableDocking(CBRS_ALIGN_ANY);

	return true;
}

void CMainFrame::UpdateWorkSpaceDlg()
{
	// WorkSpaceDlg의 정보를 갱신한다.
	m_WorkSpaceDlg.UpdateWorkSpace();
	theDoc().UpdateAllViews(NULL);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	
	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		| WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 메시지 처리기
void CMainFrame::OnCreateNurbsCrv()
{
	m_WorkSpaceDlg.InsertTabDlg(&theCreator().CreateCrvDlg, _T("곡선생성"), true);
	theCreator().InitBaseMesh(NULL);
	theCreator().CreateType = CREATE_CRV_CTL_PT;
	theDoc().PrintMessage("Click control points to create a NURBS curve.");

	pRepeatFunc = &CMainFrame::OnCreateNurbsCrv;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateNurbsSrf()
{
	m_WorkSpaceDlg.InsertTabDlg(&theCreator().CreateSrfDlg, _T("곡면생성"), true);
	theCreator().InitBaseMesh(NULL);
	theCreator().CreateType = CREATE_SRF_CTL_PT;
	theDoc().PrintMessage("Click two corner points S(0, 0) and S(1, 1) to create a NURBS plane.");

	pRepeatFunc = &CMainFrame::OnCreateNurbsSrf;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateNurbsCircle()
{
	theCreator().InitBaseMesh(NULL);
	theCreator().CreateType = CREATE_CIRCLE;

	pRepeatFunc = &CMainFrame::OnCreateNurbsCircle;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateNurbsTeapot()
{
	theCreator().InitBaseMesh(NULL);
	theCreator().CreateType = CREATE_NURBS_TEAPOT;

	pRepeatFunc = &CMainFrame::OnCreateNurbsTeapot;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnSelectToolRect()
{
	theEditor().theSelector.SelectToolType = SELECT_TOOL_RECT;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateSelectToolRect(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().theSelector.SelectToolType == SELECT_TOOL_RECT);
}

void CMainFrame::OnSelectToolCircle()
{
	theEditor().theSelector.SelectToolType = SELECT_TOOL_CIRCLE;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateSelectToolCircle(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().theSelector.SelectToolType == SELECT_TOOL_CIRCLE);
}

void CMainFrame::OnSelectToolLasso()
{
	theEditor().theSelector.SelectToolType = SELECT_TOOL_LASSO;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateSelectToolLasso(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().theSelector.SelectToolType == SELECT_TOOL_LASSO);
}

void CMainFrame::OnSelectToolPoly()
{
	theEditor().theSelector.SelectToolType = SELECT_TOOL_POLY;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateSelectToolPoly(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().theSelector.SelectToolType == SELECT_TOOL_POLY);
}

void CMainFrame::OnSelectToolPaint()
{
	theEditor().theSelector.SelectToolType = SELECT_TOOL_BRUSH;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateSelectToolPaint(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().theSelector.SelectToolType == SELECT_TOOL_BRUSH);
}

void CMainFrame::OnUpdateEditScl(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().EditToolType >= EDIT_SCL && theEditor().EditToolType <= EDIT_SCL_XYZ);
}

void CMainFrame::OnEditTra()
{
	if (theEditor().EditToolType == EDIT_TRA)
		theEditor().EditToolType = EDIT_INACTIVE;
	else
		theEditor().EditToolType = EDIT_TRA;

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateEditTra(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().EditToolType >= EDIT_TRA && theEditor().EditToolType <= EDIT_TRA_XYZ);
}

void CMainFrame::OnEditRot()
{
	if (theEditor().EditToolType == EDIT_ROT)
		theEditor().EditToolType = EDIT_INACTIVE;
	else
		theEditor().EditToolType = EDIT_ROT;

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateEditRot(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().EditToolType >= EDIT_ROT && theEditor().EditToolType <= EDIT_ROT_Z);
}

void CMainFrame::OnEditScl()
{
	if (theEditor().EditToolType == EDIT_SCL)
		theEditor().EditToolType = EDIT_INACTIVE;
	else
		theEditor().EditToolType = EDIT_SCL;

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnSelectMesh()
{
	theEditor().theSelector.SelectObjType = SELECT_MESH;
	theEditor().theSelector.SelectToolType = SELECT_TOOL_RECT;

	if (theEditor().EditObjList.empty())
		return;

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateSelectMesh(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().theSelector.SelectObjType == SELECT_MESH);
}

void CMainFrame::OnSelectBoneMesh()
{
	theEditor().theSelector.SelectObjType = SELECT_BONE;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateSelectBoneMesh(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().theSelector.SelectObjType == SELECT_BONE);
}

void CMainFrame::OnSelectVertex()
{
	theEditor().theSelector.SelectObjType = SELECT_VERT;

	if (theEditor().EditObjList.empty())
		return;

	int NumObj = (int)theEditor().EditObjList.size();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
		pMesh->Renderer.RenderType = pMesh->Renderer.RenderType | RENDER_POLYGON;
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateSelectVertex(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().theSelector.SelectObjType == SELECT_VERT);
}

void CMainFrame::OnSelectFace()
{
	theEditor().theSelector.SelectObjType = SELECT_FACE;

	if (theEditor().EditObjList.empty())
		return;

	int NumObj = theEditor().GetNumEditObj();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
		pMesh->Renderer.RenderType = pMesh->Renderer.RenderType | RENDER_POLYGON;
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateSelectFace(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().theSelector.SelectObjType == SELECT_FACE);
}

void CMainFrame::OnSelectFaceGroup()
{
	theEditor().theSelector.SelectObjType = SELECT_FACE_GROUP;

	if (theEditor().EditObjList.empty())
		return;

	int NumObj = theEditor().GetNumEditObj();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
		pMesh->Renderer.RenderType = pMesh->Renderer.RenderType | RENDER_POLYGON | RENDER_WIRE;
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateSelectFaceGroup(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().theSelector.SelectObjType == SELECT_FACE_GROUP);
}

void CMainFrame::OnSelectFaceAndCut()
{
	theEditor().theSelector.SelectObjType = SELECT_FACE_AND_CUT;

	if (theEditor().EditObjList.empty())
		return;

	int NumObj = theEditor().GetNumEditObj();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
		pMesh->Renderer.RenderType = pMesh->Renderer.RenderType | RENDER_POLYGON;
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateSelectFaceAndCut(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().theSelector.SelectObjType == SELECT_FACE_AND_CUT);
}

void CMainFrame::OnSelectPtOnFace()
{
	theEditor().theSelector.SelectObjType = SELECT_PT_ON_FACE;
	//theEditor().theSelector.SelectToolType = SELECT_TOOL_BRUSH;

	if (theEditor().EditObjList.empty())
		return;

	int NumObj = theEditor().GetNumEditObj();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
		pMesh->Renderer.RenderType = pMesh->Renderer.RenderType | RENDER_POLYGON | RENDER_WIRE;
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateSelectPtOnFace(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().theSelector.SelectObjType == SELECT_PT_ON_FACE);
}

void CMainFrame::OnSelectCtrlPt()
{
	theEditor().theSelector.SelectObjType = SELECT_CTRL_PT;
}

void CMainFrame::OnUpdateSelectCtrlPt(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().theSelector.SelectObjType == SELECT_CTRL_PT);
}

void CMainFrame::OnEditGeodesicPath()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("메쉬를 선택하세요!");
		return;
	}

	theEditor().EditToolType = EDIT_GEODESIC_PATH;
}

void CMainFrame::OnUpdateEditGeodesicPath(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().EditToolType == EDIT_GEODESIC_PATH);
}

void CMainFrame::OnRenderTypePolygon()
{
	if (theEditor().EditObjList.empty())
		return;

	int NumObj = theEditor().GetNumEditObj();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
		if (pMesh->Renderer.RenderType & RENDER_POLYGON)
		{
			pMesh->Renderer.RenderType = pMesh->Renderer.RenderType & (~RENDER_POLYGON);
		}
		else
		{
			pMesh->Renderer.RenderType = pMesh->Renderer.RenderType | RENDER_POLYGON;
		}		
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnRenderTypeWire()
{
	if (theEditor().EditObjList.empty())
		return;

	int NumObj = theEditor().GetNumEditObj();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
		if (pMesh->Renderer.RenderType & RENDER_WIRE)
		{
			pMesh->Renderer.RenderType = pMesh->Renderer.RenderType & (~RENDER_WIRE);
		}
		else
		{
			pMesh->Renderer.RenderType = pMesh->Renderer.RenderType | RENDER_WIRE;
		}	
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnRenderTypePoint()
{
	if (theEditor().EditObjList.empty())
		return;

	int NumObj = theEditor().GetNumEditObj();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
		if (pMesh->Renderer.RenderType & RENDER_POINT)
			pMesh->Renderer.RenderType = pMesh->Renderer.RenderType & (~RENDER_POINT);
		else
			pMesh->Renderer.RenderType = pMesh->Renderer.RenderType | RENDER_POINT;
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateRenderTypePolygon(CCmdUI *pCmdUI)
{
	if (theEditor().EditObjList.empty())
	{
		pCmdUI->SetCheck(FALSE);
		return;
	}

	GMesh *pMesh = theEditor().EditObjList[0]->GetRefMesh();
	if (pMesh->Renderer.RenderType & RENDER_POLYGON)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnUpdateRenderTypeWire(CCmdUI *pCmdUI)
{
	if (theEditor().EditObjList.empty())
	{
		pCmdUI->SetCheck(FALSE);
		return;
	}

	GMesh *pMesh = theEditor().EditObjList[0]->GetRefMesh();
	if (pMesh->Renderer.RenderType & RENDER_WIRE)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnUpdateRenderTypePoint(CCmdUI *pCmdUI)
{
	if (theEditor().EditObjList.empty())
	{
		pCmdUI->SetCheck(FALSE);
		return;
	}

	GMesh *pMesh = theEditor().EditObjList[0]->GetRefMesh();
	if (pMesh->Renderer.RenderType & RENDER_POINT)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnShadeFlat()
{
	if (theEditor().EditObjList.empty())
		return;

	int NumObj = theEditor().GetNumEditObj();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
		pMesh->Renderer.ShadeType = SHADE_FLAT;
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnShadeSmooth()
{
	if (theEditor().EditObjList.empty())
		return;

	int NumObj = theEditor().GetNumEditObj();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
		pMesh->Renderer.ShadeType = SHADE_SMOOTH;
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateShadeFlat(CCmdUI *pCmdUI)
{
	if (theEditor().EditObjList.empty())
	{
		pCmdUI->SetCheck(FALSE);
		return;
	}

	GMesh *pMesh = theEditor().EditObjList[0]->GetRefMesh();
	pCmdUI->SetCheck(pMesh->Renderer.ShadeType == SHADE_FLAT);	
}

void CMainFrame::OnUpdateShadeSmooth(CCmdUI *pCmdUI)
{
	if (theEditor().EditObjList.empty())
	{
		pCmdUI->SetCheck(FALSE);
		return;
	}

	GMesh *pMesh = theEditor().EditObjList[0]->GetRefMesh();
	pCmdUI->SetCheck(pMesh->Renderer.ShadeType == SHADE_SMOOTH);	
}

void CMainFrame::OnVertexColorIllumination()
{
	if (theEditor().EditObjList.empty())
		return;

	int NumObj = (int)theEditor().EditObjList.size();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
		pMesh->Renderer.VertColorType = VERTEX_COLOR_ILLUMINATION;
		pMesh->UpdateNormal(false);
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnVertexColorNormal()
{
	if (theEditor().EditObjList.empty())
		return;

	int NumObj = (int)theEditor().EditObjList.size();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().GetEditMesh();
		pMesh->Renderer.VertColorType = VERTEX_COLOR_NORMAL;
		::set_vertex_color_from_normal(pMesh);
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnVertexColorBoneWeights()
{
	if (theEditor().EditObjList.empty())
		return;

	int NumObj = (int)theEditor().EditObjList.size();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
		pMesh->Renderer.VertColorType = VERTEX_COLOR_BONE_WEIGHT;
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateVertexColorIllumination(CCmdUI *pCmdUI)
{
	if (theEditor().EditObjList.empty())
	{
		pCmdUI->SetCheck(FALSE);
		return;
	}

	GMesh *pMesh = theEditor().EditObjList[0]->GetRefMesh();
	pCmdUI->SetCheck(pMesh->Renderer.VertColorType == VERTEX_COLOR_ILLUMINATION);	
}

void CMainFrame::OnUpdateVertexColorNormal(CCmdUI *pCmdUI)
{
	if (theEditor().EditObjList.empty())
	{
		pCmdUI->SetCheck(FALSE);
		return;
	}

	GMesh *pMesh = theEditor().EditObjList[0]->GetRefMesh();
	pCmdUI->SetCheck(pMesh->Renderer.VertColorType == VERTEX_COLOR_NORMAL);	
}

void CMainFrame::OnUpdateVertexColorBoneWeights(CCmdUI *pCmdUI)
{
	if (theEditor().EditObjList.empty())
	{
		pCmdUI->SetCheck(FALSE);
		return;
	}

	GMesh *pMesh = theEditor().EditObjList[0]->GetRefMesh();
	pCmdUI->SetCheck(pMesh->Renderer.VertColorType == VERTEX_COLOR_BONE_WEIGHT);	
}

void CMainFrame::OnRenderTexture()
{
	if (theEditor().EditObjList.empty())
		return;

	int NumObj = (int)theEditor().EditObjList.size();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
		pMesh->Renderer.bTexture = !pMesh->Renderer.bTexture;
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnRenderNormal()
{
	if (theEditor().EditObjList.empty())
		return;

	int NumObj = (int)theEditor().EditObjList.size();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
		if (pMesh->Renderer.RenderType & RENDER_NORMAL)
			pMesh->Renderer.RenderType = pMesh->Renderer.RenderType & (~RENDER_NORMAL);
		else
			pMesh->Renderer.RenderType = pMesh->Renderer.RenderType | RENDER_NORMAL;
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnRenderCullBackFace()
{
	if (theEditor().EditObjList.empty())
		return;

	int NumObj = (int)theEditor().EditObjList.size();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().GetEditMesh();
		pMesh->Renderer.bBackFaceCull = !pMesh->Renderer.bBackFaceCull;
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnRenderCtrlPt()
{
	if (theEditor().EditObjList.empty())
		return;

	int NumObj = (int)theEditor().EditObjList.size();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
		if (pMesh->Renderer.RenderType & RENDER_CTRL_PT)
			pMesh->Renderer.RenderType = pMesh->Renderer.RenderType & (~RENDER_CTRL_PT);
		else
			pMesh->Renderer.RenderType = pMesh->Renderer.RenderType | RENDER_CTRL_PT;
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateRenderCtrlPt(CCmdUI *pCmdUI)
{
	if (theEditor().EditObjList.empty())
	{
		pCmdUI->SetCheck(FALSE);
		return;
	}

	GMesh *pMesh = theEditor().EditObjList[0]->GetRefMesh();
	if (pMesh->Renderer.RenderType & RENDER_CTRL_PT)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnUpdateRenderTexture(CCmdUI *pCmdUI)
{
	if (theEditor().EditObjList.empty())
	{
		pCmdUI->SetCheck(FALSE);
		return;
	}

	GMesh *pMesh = theEditor().EditObjList[0]->GetRefMesh();
	pCmdUI->SetCheck(pMesh->Renderer.bTexture == TRUE);
}

void CMainFrame::OnUpdateRenderNormal(CCmdUI *pCmdUI)
{
	if (theEditor().EditObjList.empty())
	{
		pCmdUI->SetCheck(FALSE);
		return;
	}

	GMesh *pMesh = theEditor().EditObjList[0]->GetRefMesh();
	if (pMesh->Renderer.RenderType & RENDER_NORMAL)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnUpdateRenderCullBackFace(CCmdUI *pCmdUI)
{
	if (theEditor().EditObjList.empty())
	{
		pCmdUI->SetCheck(FALSE);
		return;
	}

	GMesh *pMesh = theEditor().EditObjList[0]->GetRefMesh();
	pCmdUI->SetCheck(pMesh->Renderer.bBackFaceCull == TRUE);
}

void CMainFrame::OnCreateRagdoll()
{
	if (theEditor().EditObjList.empty())
	{
		theScene().HeadUpDisplay.Message = "Select a joint mesh to create a ragdoll";
		theDoc().UpdateAllViews(NULL);
		return;
	}

	int NumObj = theEditor().EditObjList.size();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pParentMesh = theEditor().EditObjList[i]->GetRefMesh();

		double size = pParentMesh->BBox.GetDiagonal() * 0.2;
		std::string name = create_mesh_name("Ragdoll");
		GMesh *pRagdollMesh = ::create_mesh_from_sphere(name, pParentMesh->MC, size, 20, 20);
		pRagdollMesh->Renderer.RenderType = RENDER_WIRE;
		pRagdollMesh->ParentMeshName = pParentMesh->Name;
		theScene().AddMesh(pRagdollMesh);
	}
	
	UpdateWorkSpaceDlg();
}


void CMainFrame::OnDisplayGrid()
{
	theScene().bShowFloor = !theScene().bShowFloor;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateDisplayGrid(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theScene().bShowFloor == TRUE);
}

void CMainFrame::OnDisplayBackground()
{
	theScene().bShowBkg = !theScene().bShowBkg;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateDisplayBackground(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theScene().bShowBkg == TRUE);
}

void CMainFrame::OnHeadsupdisplayFramerate()
{
	if (theScene().HeadUpDisplay.DisplayInfo & HUD_FRAME_RATE)
		theScene().HeadUpDisplay.DisplayInfo = theScene().HeadUpDisplay.DisplayInfo & (~HUD_FRAME_RATE);
	else
		theScene().HeadUpDisplay.DisplayInfo = theScene().HeadUpDisplay.DisplayInfo | HUD_FRAME_RATE;

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnHeadsupdisplayOriginaxis()
{
	if (theScene().HeadUpDisplay.DisplayInfo & HUD_ORIGIN_AXIS)
		theScene().HeadUpDisplay.DisplayInfo = theScene().HeadUpDisplay.DisplayInfo & (~HUD_ORIGIN_AXIS);
	else
		theScene().HeadUpDisplay.DisplayInfo = theScene().HeadUpDisplay.DisplayInfo | HUD_ORIGIN_AXIS;

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnHeadsupdisplayViewcube()
{
	if (theScene().HeadUpDisplay.DisplayInfo & HUD_VIEW_CUBE)
		theScene().HeadUpDisplay.DisplayInfo = theScene().HeadUpDisplay.DisplayInfo & (~HUD_VIEW_CUBE);
	else
		theScene().HeadUpDisplay.DisplayInfo = theScene().HeadUpDisplay.DisplayInfo | HUD_VIEW_CUBE;

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateHeadsupdisplayFramerate(CCmdUI *pCmdUI)
{
	if (theScene().HeadUpDisplay.DisplayInfo & HUD_FRAME_RATE)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnUpdateHeadsupdisplayOriginaxis(CCmdUI *pCmdUI)
{
	if (theScene().HeadUpDisplay.DisplayInfo & HUD_ORIGIN_AXIS)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnUpdateHeadsupdisplayViewcube(CCmdUI *pCmdUI)
{
	if (theScene().HeadUpDisplay.DisplayInfo & HUD_VIEW_CUBE)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnViewWorkspace()
{
	if (m_wndWorkSpaceBar.IsWindowVisible())
		ShowControlBar(&m_wndWorkSpaceBar, false, true);
	else
		ShowControlBar(&m_wndWorkSpaceBar, true, true);
}

void CMainFrame::OnViewEditTool()
{
	if (m_wndToolEditBar.IsWindowVisible())
		ShowControlBar(&m_wndToolEditBar, false, true);
	else
		ShowControlBar(&m_wndToolEditBar, true, true);
}

void CMainFrame::OnUpdateViewWorkspace(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndWorkSpaceBar.IsWindowVisible());
}

void CMainFrame::OnUpdateViewEditTool(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndToolEditBar.IsWindowVisible());
}

void CMainFrame::OnViewRenderTypeBar()
{
	if (m_wndToolRenderTypeBar.IsWindowVisible())
		ShowControlBar(&m_wndToolRenderTypeBar, false, true);
	else
		ShowControlBar(&m_wndToolRenderTypeBar, true, true);
}

void CMainFrame::OnViewNurbsCreateBar()
{
	if (m_wndToolNurbsCreatorBar.IsWindowVisible())
		ShowControlBar(&m_wndToolNurbsCreatorBar, false, true);
	else
		ShowControlBar(&m_wndToolNurbsCreatorBar, true, true);
}

void CMainFrame::OnViewPolygonCreateBar()
{
	if (m_wndToolPolygonCreatorBar.IsWindowVisible())
		ShowControlBar(&m_wndToolPolygonCreatorBar, false, true);
	else
		ShowControlBar(&m_wndToolPolygonCreatorBar, true, true);
}

void CMainFrame::OnViewAnimateBar()
{
	if (m_wndAnimDlgBar.IsWindowVisible())
		ShowControlBar(&m_wndAnimDlgBar, false, true);
	else
		ShowControlBar(&m_wndAnimDlgBar, true, true);
}

void CMainFrame::OnViewOutputBar()
{
	if (m_wndOutputDlgBar.IsWindowVisible())
		ShowControlBar(&m_wndOutputDlgBar, false, true);
	else
		ShowControlBar(&m_wndOutputDlgBar, true, true);
}

void CMainFrame::OnUpdateViewRenderTypeBar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndToolRenderTypeBar.IsWindowVisible());
}

void CMainFrame::OnUpdateViewNurbsCreateBar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndToolNurbsCreatorBar.IsWindowVisible());
}

void CMainFrame::OnUpdateViewPolygonCreateBar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndToolPolygonCreatorBar.IsWindowVisible());
}

void CMainFrame::OnUpdateViewAnimateBar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndAnimDlgBar.IsWindowVisible());
}

void CMainFrame::OnUpdateViewOutputBar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndOutputDlgBar.IsWindowVisible());
}

void CMainFrame::OnRenderTypeBoundary()
{
	if (theEditor().EditObjList.empty())
		return;

	int NumObj = (int)theEditor().EditObjList.size();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
		if (pMesh->Renderer.RenderType & RENDER_BOUNDARY)
			pMesh->Renderer.RenderType = pMesh->Renderer.RenderType & (~RENDER_BOUNDARY);
		else
			pMesh->Renderer.RenderType = pMesh->Renderer.RenderType | RENDER_BOUNDARY;
	}

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateRenderTypeBoundary(CCmdUI *pCmdUI)
{
	if (theEditor().EditObjList.empty())
	{
		pCmdUI->SetCheck(FALSE);
		return;
	}

	GMesh *pMesh = theEditor().EditObjList[0]->GetRefMesh();
	if (pMesh->Renderer.RenderType & RENDER_BOUNDARY)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnCreateTriBlendSrf()
{
	if (theEditor().EditObjList.empty())
	{
		theDoc().PrintMessage("// Error: a control mesh is required for creating triangular blending surface.");
		return;
	}

	if (theEditor().EditObjList[0]->GetClassName() != "GMesh")
	{
		theDoc().PrintMessage("// Error: a control mesh is required for creating triangular blending surface.");
		return;
	}

	GMesh *pCtrlMesh = theEditor().EditObjList[0]->GetRefMesh();
	pCtrlMesh->Renderer.RenderType = RENDER_WIRE;

	std::string Name = create_mesh_name("BlendMesh");
	GBlendMesh *pSrf = new GBlendMesh(Name, pCtrlMesh->MC, pCtrlMesh);
	pSrf->Renderer.RenderType = RENDER_POLYGON | RENDER_WIRE;
	//pSrf->Renderer.NameMtrl = "mat_gray";
	//pSrf->Renderer.CtrlPtSize = 0.01;
	pSrf->Renderer.SetSampleNum(5, 5);
	theScene().AddMesh(pSrf);

	UpdateWorkSpaceDlg();
}

void CMainFrame::OnSewBoundary()
{
	if (theEditor().EditObjList.empty())
	{
		theDoc().PrintMessage("// Error: select a mesh to be sewed.");
		return;
	}

	if (theEditor().EditObjList[0]->GetClassName() != "GMesh" && 
		theEditor().EditObjList[0]->GetClassName() != "GBlendMesh" && 
		theEditor().EditObjList[0]->GetClassName() != "GTriPNSrf" &&
		theEditor().EditObjList[0]->GetClassName() != "GCoonsMesh")
	{
		theDoc().PrintMessage("// Error: select a mesh to be sewed.");
		return;
	}

	GMesh *pMesh = theEditor().GetEditMesh();
	edit_mesh_by_stitching(pMesh);

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateSweepSrf()
{
	if (theEditor().GetClsNameEditObj() != "GNurbsCrv" || theEditor().GetNumEditObj() < 4)
	{
		AfxMessageBox("4 개 이상의 단면 곡선을 선택하세요!");
		return;
	}
	
	std::vector<GNurbsCrv *> KeyCrvList;
	int NumCrv = theEditor().EditObjList.size();
	for (int i = 0; i < NumCrv; ++i)
		KeyCrvList.push_back((GNurbsCrv *)theEditor().EditObjList[i]);

	GSweepSrf *pSrf = new GSweepSrf("Sweep Surface", GTransf(), KeyCrvList, 3, 1, "");
	pSrf->Renderer.RenderType = RENDER_POLYGON | RENDER_WIRE;
	pSrf->Renderer.CtrlPtSize = (theScene().CtrlPtSize == 0.0) ? 0.01 : theScene().CtrlPtSize;
	theScene().AddMesh(pSrf);

	GMaterial mtl = ::get_material_random();
	pSrf->AddMtl(mtl);
	pSrf->SetMtl(mtl.Name);	

	UpdateWorkSpaceDlg();
}

void CMainFrame::OnEditCutMesh()
{
	// 절단선이 선택되지 않았다면 리턴한다.
	if (theEditor().EditObjList.empty() || theEditor().GetEditMesh()->GetClassName() != "GCutLine")
	{
		theDoc().PrintMessage("절단선을 선택하세요.");
		return;
	}

	// 선택된 절단선 객체와 절단할 메쉬 객체를 구한다.
	GCutLine *pCutLine = (GCutLine *)(theEditor().GetEditMesh());
	GMesh *pMesh = pCutLine->PtList[0].pFace->pMesh;

	if (pMesh->FaceType != FACE_TRI)
	{
		AfxMessageBox("삼각형 메쉬만 가능합니다.");
		return;
	}

	// 절단 메쉬의 텍스처와 그룹 정보를 제거한다.
	pMesh->Texs.clear();
	pMesh->GrpFaces.clear();

	// 추가할 절단점의 인덱스를 저장하는 맵을 선언하고,
	std::map<int, int> CutPtIdxMap;

	// 절단점으로 구성된 새로운 삼각형의 리스트를 생성한다.
	std::vector<GFace> NewFaces;

	// 절단점을 메쉬 객체에 추가한다.
	int NumPt = (int)(pCutLine->PtList.size());
	for (int i = 0; i < NumPt; ++i)
	{
		// 절단점이 메쉬의 정점이 아니라면 절단점을 메쉬의 정점리스트에 추가한다.
		if (pCutLine->PtList[i].t != 0.0)
		{
			pMesh->AddVertex(GVertex(pCutLine->PtList[i].P));
			CutPtIdxMap[i] = pMesh->GetNumVert() - 1;
		}
		else	// 절단점이 메쉬 정점이라면 인덱스만 기록한다.
		{
			CutPtIdxMap[i] = pCutLine->PtList[i].vIdx[0];
		}
	}

	// 시작 삼각형을 생성하고,
	GFace *pFace = pMesh->FindFace(pCutLine->PtList[0].vIdx[1], pCutLine->PtList[0].vIdx[0]);
	int tmp_idx = pFace->GetNextVertIdx(pCutLine->PtList[0].vIdx[0]);
	NewFaces.push_back(GFace(tmp_idx, pCutLine->PtList[0].vIdx[1], CutPtIdxMap[0]));
	NewFaces.push_back(GFace(CutPtIdxMap[0], pCutLine->PtList[0].vIdx[0], tmp_idx));

	// 절단선과 교차하는 삼각형을 식별할 맵을 구성하고,
	std::map<GFace *, bool> SltFaceMap;
	SltFaceMap[pFace] = true;

	for	(int i = 0; i < NumPt - 1; ++i)
	{
		int curr_sidx = pCutLine->PtList[i].vIdx[0];
		int curr_eidx = pCutLine->PtList[i].vIdx[1];
		int curr_aidx = pCutLine->PtList[i].vIdx[2];
		int next_sidx = pCutLine->PtList[i + 1].vIdx[0];
		int next_eidx = pCutLine->PtList[i + 1].vIdx[1];
		//int offset_idx = (pCutLine->bClosed) ? ((i + 1) % (NumPt - 1)) : 1;

		// i 번째 절단점이 삼각형의 정점을 지나는 경우
		if (pCutLine->PtList[i].t == 0.0)
		{
			// i + 1 번째 절단점이 삼각형의 정점을 지나는 경우
			if (pCutLine->PtList[i + 1].t == 0.0)
			{
				NewFaces.push_back(GFace(curr_sidx, curr_eidx, curr_aidx));
			}
			else // i + 1 번째 절단점이 삼각형의 모서리를 지나는 경우
			{
				if (curr_eidx == next_eidx)
				{
					NewFaces.push_back(GFace(CutPtIdxMap[i], CutPtIdxMap[i + 1], curr_aidx));
					NewFaces.push_back(GFace(CutPtIdxMap[i], curr_eidx, CutPtIdxMap[i + 1]));
				}

				if (curr_sidx == next_eidx)
				{
					NewFaces.push_back(GFace(CutPtIdxMap[i], CutPtIdxMap[i + 1], curr_aidx));
					NewFaces.push_back(GFace(CutPtIdxMap[i + 1], curr_eidx, curr_aidx));
				}

				if (curr_sidx == next_sidx)
				{
					NewFaces.push_back(GFace(curr_sidx, curr_eidx, CutPtIdxMap[i + 1]));
					NewFaces.push_back(GFace(curr_aidx, CutPtIdxMap[i + 1], curr_eidx));
				}
			}			
		}
		else // i 번째 절단점이 삼각형의 모서리를 지나지는 경우
		{
			// (i + 1) 번째 절단점이 삼각형의 정점을 지나는 경우
			if (pCutLine->PtList[i + 1].t == 0.0)
			{
				NewFaces.push_back(GFace(CutPtIdxMap[i], curr_eidx, curr_aidx));
				NewFaces.push_back(GFace(CutPtIdxMap[i], curr_aidx, curr_sidx));
			}
			else // (i + 1) 번째 절단점이 삼각형의 모서리를 지나지는 경우
			{
				// (i + 1) 번째 절단점이 i 번째 절단점과 같은 모서리를 지나지는 경우
				if (pCutLine->PtList[i].pFace == pCutLine->PtList[i + 1].pFace)
				{
					printf("oops...\n");
					if (pCutLine->PtList[i].t < 1.0 - pCutLine->PtList[i + 1].t)
					{
						NewFaces.push_back(GFace(CutPtIdxMap[i], curr_aidx, curr_sidx));
						NewFaces.push_back(GFace(CutPtIdxMap[i], CutPtIdxMap[i + 1], curr_aidx));
						NewFaces.push_back(GFace(CutPtIdxMap[i + 1], curr_eidx, curr_aidx));
					}
					else
					{
						NewFaces.push_back(GFace(CutPtIdxMap[i], curr_eidx, curr_aidx));
						NewFaces.push_back(GFace(CutPtIdxMap[i], curr_aidx, CutPtIdxMap[i + 1]));
						NewFaces.push_back(GFace(CutPtIdxMap[i + 1], curr_aidx, curr_sidx));
					}					
				}
				else // (i + 1) 번째 절단점이 i 번째 절단점과 다른 모서리를 지나지는 경우 (가장 일반적인 경우)
				{
					if (curr_sidx == next_sidx)	
					{
						NewFaces.push_back(GFace(CutPtIdxMap[i], CutPtIdxMap[i + 1], curr_sidx));
						NewFaces.push_back(GFace(CutPtIdxMap[i], curr_eidx, CutPtIdxMap[i + 1]));
						NewFaces.push_back(GFace(CutPtIdxMap[i + 1], curr_eidx, next_eidx));
					}
					else if (curr_eidx == next_eidx)
					{
						NewFaces.push_back(GFace(CutPtIdxMap[i], curr_eidx, CutPtIdxMap[i + 1]));
						NewFaces.push_back(GFace(CutPtIdxMap[i], CutPtIdxMap[i + 1], next_sidx));
						NewFaces.push_back(GFace(CutPtIdxMap[i], next_sidx, curr_sidx));
					}

					/*
					if (curr_sidx == next_sidx)	
					{
						pMesh->AddVertex(GVertex(pMesh->Verts[curr_eidx].P + 0.5 * (pMesh->Verts[next_eidx].P - pMesh->Verts[curr_eidx].P)));
						int last_idx = pMesh->GetNumVert() - 1;

						NewFaces.push_back(GFace(CutPtIdxMap[i], CutPtIdxMap[i + 1], curr_sidx));
						NewFaces.push_back(GFace(CutPtIdxMap[i], curr_eidx, last_idx));
						NewFaces.push_back(GFace(CutPtIdxMap[i], last_idx, CutPtIdxMap[i + 1]));
						NewFaces.push_back(GFace(CutPtIdxMap[i + 1], last_idx, next_eidx));
					}
					else if (curr_eidx == next_eidx)
					{
						pMesh->AddVertex(GVertex(pMesh->Verts[curr_sidx].P + 0.5 * (pMesh->Verts[next_sidx].P - pMesh->Verts[curr_sidx].P)));
						int last_idx = pMesh->GetNumVert() - 1;

						NewFaces.push_back(GFace(CutPtIdxMap[i], curr_eidx, CutPtIdxMap[i + 1]));
						NewFaces.push_back(GFace(CutPtIdxMap[i], CutPtIdxMap[i + 1], last_idx));
						NewFaces.push_back(GFace(CutPtIdxMap[i], last_idx, curr_sidx));
						NewFaces.push_back(GFace(CutPtIdxMap[i + 1], next_sidx, last_idx));
					}
					*/
				}
			}		
		}
	}

	// 마무리 삼각형을 생성하고,
	pFace = pMesh->FindFace(pCutLine->PtList[NumPt - 1].vIdx[0], pCutLine->PtList[NumPt - 1].vIdx[1]);
	tmp_idx = pFace->GetNextVertIdx(pCutLine->PtList[NumPt - 1].vIdx[1]);
	NewFaces.push_back(GFace(CutPtIdxMap[NumPt - 1], pCutLine->PtList[NumPt - 1].vIdx[1], tmp_idx));
	NewFaces.push_back(GFace(CutPtIdxMap[NumPt - 1], tmp_idx, pCutLine->PtList[NumPt - 1].vIdx[0]));

	// 절단선과 교차하는 삼각형을 추가한다.
	for (int i = 0; i < NumPt; ++i)
	{
		GFace *pFace = pCutLine->PtList[i].pFace;
		SltFaceMap[pFace] = true;
	}

	// 교차하지 않는 삼각형은 새로운 삼각형 리스트에 추가하여
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		GFace *pFace = &(pMesh->Faces[i]);
		if (!SltFaceMap[pFace])
			NewFaces.push_back(pMesh->Faces[i]);
	}

	// 참조 메쉬의 삼각형 리스트와 메쉬를 갱신한다.
	pMesh->SetNewFaces(NewFaces);
	pMesh->CreateMesh();
	
	// 절단선을 삭제하고, 작업창의 메쉬리스트를 갱신한다.
	theEditor().DeleteObject();
	theMainFrm().UpdateWorkSpaceDlg();

	// 추가된 정점을 선택하고, 편집 툴의 편집 좌표계를 설정한다.
	for (int i = 0; i < (int)CutPtIdxMap.size(); ++i)
	{
		int vidx = CutPtIdxMap[i];
		theEditor().EditObjList.push_back(&(pMesh->Verts[vidx]));
	}
	theEditor().SetEC();

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnNormalizeSkinWeights()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL || pMesh->SemanticType != MESH_SKIN)
	{
		theDoc().PrintMessage("정규화할 스킨 메쉬를 선택하세요.");
		return;
	}

	std::vector<std::vector<int>> VertList;
	::get_neighbor_verts_of_vert(pMesh, VertList);
	
	int NumVert = pMesh->GetNumVert();
	std::vector<std::vector<GRefBoneInfo>> NewBindInfoList;
	std::vector<GRefBoneInfo> tmp;
	NewBindInfoList.assign(NumVert, tmp);

	// 각각의 정점 v_i에 대하여.
	for (int i = 0; i < NumVert; ++i)
	{
		// 각각의 바인딩 정보 I_j에 대하여,
		int NumBindInfo = (int)(pMesh->Verts[i].BoneInfoList.size());
		for (int j = 0; j < NumBindInfo; ++j)
		{
			GRefBoneInfo Info = pMesh->Verts[i].BoneInfoList[j];
						
			double tot_wgt = 0.0;
			// 각각의 이웃한 정점 v_k에 대하여,
			int NumNgbVert = (int)VertList[i].size();
			for (int k = 0; k < NumNgbVert; ++k)
			{
				int vIdx = VertList[i][k];
				int NumBindInfoNgbVert = (int)(pMesh->Verts[vIdx].BoneInfoList.size());

				// 이웃한 정점의 각 바인딩 정보 I_l에 대하여
				for (int l = 0; l < NumBindInfoNgbVert; ++l)
				{
					GRefBoneInfo NgbInfo = pMesh->Verts[vIdx].BoneInfoList[l];

					// 정점 v_i의 j번째 바인딩 정보 I_j와 동일한 본에 연결되어 있다면, 가중치를 누적한다.
					if (Info.BoneMeshName == NgbInfo.BoneMeshName)
						tot_wgt = tot_wgt + NgbInfo.Wgt;
				}
			}
			Info.Wgt = tot_wgt / (double)NumNgbVert;
			NewBindInfoList[i].push_back(Info);
		}		
	}

	for (int i = 0; i < NumVert; ++i)
	{
		pMesh->Verts[i].BoneInfoList = NewBindInfoList[i];
	}

	for (int i = 0; i < NumVert; ++i)
	{
		int NumBindInfo = (int)(pMesh->Verts[i].BoneInfoList.size());
		double sum_wgt = 0.0;
		for (int j = 0; j < NumBindInfo; ++j)
		{
			double wgt = pMesh->Verts[i].BoneInfoList[j].Wgt;
			sum_wgt += wgt;
		}

		for (int j = 0; j < NumBindInfo; ++j)
		{
			pMesh->Verts[i].BoneInfoList[j].Wgt /= sum_wgt;
		}
	}

	pMesh->ModifiedFlag = MODIFIED_VERT_NUM;
	pMesh->UpdateMesh();

	pRepeatFunc = &CMainFrame::OnNormalizeSkinWeights;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnFillHole()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		theDoc().PrintMessage("메쉬를 선택하세요.");
		return;
	}

	std::vector<std::vector<int>> VertList;
	::get_bndry_verts(pMesh, VertList);

	if (VertList.empty())
	{
		theDoc().PrintMessage("홀이 존재하지 않는 메쉬입니다.");
		return;
	}

	// 각각의 홀에 대하여,
	int NumHoles = (int)VertList.size();
	for (int i = 0; i < NumHoles; ++i)
	{
		// 홀의 경계 정점의 리스트를 구하여,
		std::vector<GVertex> BndryVerts;
		int NumVerts = (int)VertList[i].size();
		for (int j = 0; j < NumVerts; ++j)
		{
			int vidx = VertList[i][j];
			BndryVerts.push_back(pMesh->Verts[vidx]);
		}

		// 경계 정점의 리스트로 메쉬를 생성하여 장면에 추가한다.
		GMesh *pHoleMesh = create_mesh_from_bndry_verts(create_mesh_name("Fill_Mesh"), BndryVerts, 0);
		theScene().AddMesh(pHoleMesh);
	}

	pRepeatFunc = &CMainFrame::OnFillHole;	
	theMainFrm().UpdateWorkSpaceDlg();
}

void CMainFrame::OnFillHoleDiskType()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		theDoc().PrintMessage("메쉬를 선택하세요.");
		return;
	}

	std::vector<std::vector<int>> VertList;
	::get_bndry_verts(pMesh, VertList);

	if (VertList.empty())
	{
		theDoc().PrintMessage("홀이 존재하지 않는 메쉬입니다.");
		return;
	}

	// 각각의 홀에 대하여,
	int NumHoles = (int)VertList.size();
	for (int i = 0; i < NumHoles; ++i)
	{
		// 홀의 경계 정점의 리스트를 구하여,
		std::vector<GVertex> BndryVerts;
		int NumVerts = (int)VertList[i].size();
		for (int j = 0; j < NumVerts; ++j)
		{
			int vidx = VertList[i][j];
			BndryVerts.push_back(pMesh->Verts[vidx]);
		}

		// 경계 정점의 리스트로 메쉬를 생성하여 장면에 추가한다.
		GMesh *pHoleMesh = create_mesh_from_bndry_verts(create_mesh_name("Fill_Mesh"), BndryVerts, 1);
		theScene().AddMesh(pHoleMesh);
	}

	pRepeatFunc = &CMainFrame::OnFillHole;	
	theMainFrm().UpdateWorkSpaceDlg();
}


void CMainFrame::OnUpdateMcFromWc()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("메쉬를 선택하세요.");
		return;
	}

	pMesh->UpdateMC(0);
	theEditor().SetEC();
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateMcFromAabb()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("메쉬를 선택하세요.");
		return;
	}

	pMesh->UpdateMC(1);
	theEditor().SetEC();
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateMcFromObb()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("메쉬를 선택하세요.");
		return;
	}

	pMesh->UpdateMC(2);
	theEditor().SetEC();
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnFindNonManifoldFaces()
{
	// 선택된 메쉬가 없다면 리턴한다.
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("메쉬를 선택하세요.");
		return;
	}

	// 메쉬의 non-manifold 삼각형을 찾아서
	std::vector<GFace *> FaceList;
	::find_non_manifold_faces(pMesh, FaceList);

	// 편집 리스트에 추가한다.
	for (unsigned i = 0; i < FaceList.size(); ++i)
		theEditor().AddToEditObjList(FaceList[i]);

	if (FaceList.empty())
		AfxMessageBox("Non-manifold 삼각형이 존재하지 않습니다.");

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnFindDegeneratedFaces()
{
	// 선택된 메쉬가 없다면 리턴한다.
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("메쉬를 선택하세요.");
		return;
	}

	// 메쉬의 degenerated 삼각형을 찾아서
	std::vector<GFace *> FaceList;
	::find_degenerated_faces(pMesh, FaceList, 0.0000001);

	// 편집 리스트에 추가한다.
	for (unsigned i = 0; i < FaceList.size(); ++i)
		theEditor().AddToEditObjList(FaceList[i]);

	if (FaceList.empty())
		AfxMessageBox("Degenerated 삼각형이 존재하지 않습니다.");

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnFindIsolatedVerts()
{
	// 선택된 메쉬가 없다면 리턴한다.
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("메쉬를 선택하세요.");
		return;
	}

	// 메쉬의 고립된 정점을 찾아서
	std::vector<GVertex *> VertList;
	::find_isolated_verts(pMesh, VertList);

	// 편집 리스트에 추가한다.
	for (unsigned i = 0; i < VertList.size(); ++i)
		theEditor().AddToEditObjList(VertList[i]);

	if (VertList.empty())
		AfxMessageBox("Isolated 정점이 존재하지 않습니다.");

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnFindHoles()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CMainFrame::OnCreateCharacterMale()
{
	// 남자 모델의 본와 스킨 메쉬를 로드한다.
	std::string file_name = theScene().RootDir + "\\res\\models\\avatar_man.dump";
	::loadAvatarDump(&theScene(), file_name.c_str());

	theMainFrm().UpdateWorkSpaceDlg();
}

void CMainFrame::OnCreateCharacterFemale()
{
	// 여자 모델의 본와 스킨 메쉬를 로드한다.
	std::string file_name = theScene().RootDir + "\\res\\models\\avatar_woman.dump";
	::loadAvatarDump(&theScene(), file_name.c_str());
	
	theMainFrm().UpdateWorkSpaceDlg();
}

void CMainFrame::OnMergeMeshes()
{
	int NumMesh = theEditor().GetNumEditObj();
	if (NumMesh == 1 || theEditor().EditObjList[0]->GetClassName() != "GMesh")
	{
		theDoc().PrintMessage("2개 이상의 메쉬를 선택하세요.");
		return;
	}

	TypeMeshFace FaceType = FACE_QUAD;
	for (int i = 0; i < NumMesh; ++i)
	{
		if (((GMesh *)theEditor().EditObjList[i])->FaceType == FACE_TRI)
			FaceType = FACE_TRI;
	}

	std::string Name = create_mesh_name("Merged Mesh");
	GMesh *pMergedMesh = new GMesh(Name, GTransf(), FaceType);

	int PrevNumVert = 0;
	for (int i = 0; i < NumMesh; ++i)
	{
		GMesh *pMesh = (GMesh *)theEditor().EditObjList[i];

		// 병합할 메쉬의 정점을 추가하고,
		int NumVert = pMesh->GetNumVert();
		for (int j = 0; j < NumVert; ++j)
		{
			GPoint3 p = pMesh->MC * pMesh->Verts[j].P;
			GVector3 n = pMesh->MC * pMesh->Verts[j].N;
			pMergedMesh->AddVertex(GVertex(p, n));
		}

		// 병합할 메쉬의 삼(사)각형을 추가한다.
		if (!pMesh->GrpFaces.empty())
		{
			std::map<std::string, std::vector<int>>::iterator it;
			for (it = pMesh->GrpFaces.begin(); it != pMesh->GrpFaces.end(); ++it)
			{
				std::string name = it->first;
				int NumFace = it->second.size();
				for (int j = 0; j < NumFace; ++j)
				{
					int fidx = it->second[j];
					GFace f = pMesh->Faces[fidx];
					f.vIdx[0] += PrevNumVert;
					f.vIdx[1] += PrevNumVert;
					f.vIdx[2] += PrevNumVert;
					pMergedMesh->AddFace(f, name);
				}
			}
		}
		else
		{
			int NumFace = pMesh->GetNumFace();
			for (int j = 0; j < NumFace; ++j)
			{
				GFace f = pMesh->Faces[j];
				f.vIdx[0] += PrevNumVert;
				f.vIdx[1] += PrevNumVert;
				f.vIdx[2] += PrevNumVert;
				pMergedMesh->AddFace(f, pMesh->Name);
			}
		}

		PrevNumVert = pMergedMesh->GetNumVert();
	}
	
	// 인접 부분의 정점의 중복성을 제거한다.
	edit_mesh_by_stitching(pMergedMesh);
	
	theScene().AddMesh(pMergedMesh);
	theMainFrm().UpdateWorkSpaceDlg();
}


void CMainFrame::OnSelectModeFrontOnly()
{
	theSelector().SelectModeType = SELECT_MODE_FRONT_ONLY;
}

void CMainFrame::OnSelectModeFrontAndBack()
{
	theSelector().SelectModeType = SELECT_MODE_FRONT_AND_BACK;
}

void CMainFrame::OnUpdateSelectModeFrontOnly(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theSelector().SelectModeType == SELECT_MODE_FRONT_ONLY);
}

void CMainFrame::OnUpdateSelectModeFrontAndBack(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theSelector().SelectModeType == SELECT_MODE_FRONT_AND_BACK);
}

void CMainFrame::OnCheckFps()
{
	m_CheckFPS = !m_CheckFPS;
}

void CMainFrame::OnUpdateCheckFps(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_CheckFPS == TRUE);
}

void CMainFrame::OnDeformBox()
{
}

void CMainFrame::OnCreatePolygon()
{
	m_WorkSpaceDlg.InsertTabDlg(&theCreator().CreatePlyDlg, _T("폴리곤생성"), true);
	theCreator().InitBaseMesh(NULL);
	theCreator().CreateType = CREATE_POLYGON;

	pRepeatFunc = &CMainFrame::OnCreatePolygon;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreatePolygonSphere()
{
	theCreator().InitBaseMesh(NULL);
	theCreator().CreateType = CREATE_POLYGON_SPHERE;

	pRepeatFunc = &CMainFrame::OnCreatePolygonSphere;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreatePolygonIcosahedron()
{
	theCreator().InitBaseMesh(NULL);
	theCreator().CreateType = CREATE_POLYGON_ICOSAHEDRON;

	pRepeatFunc = &CMainFrame::OnCreatePolygonIcosahedron;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreatePolygonTeapot()
{
	theCreator().InitBaseMesh(NULL);
	theCreator().CreateType = CREATE_POLYGON_TEAPOT;

	pRepeatFunc = &CMainFrame::OnCreatePolygonTeapot;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreatePolygonPlane()
{
	m_WorkSpaceDlg.InsertTabDlg(&theCreator().CreatePlnDlg, _T("평면생성"), true);
	theCreator().InitBaseMesh(NULL);
	theCreator().CreateType = CREATE_PLANE;

	pRepeatFunc = &CMainFrame::OnCreatePolygonPlane;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreatePolygonDisk()
{
	theCreator().InitBaseMesh(NULL);
	theCreator().CreateType = CREATE_POLYGON_DISK;

	pRepeatFunc = &CMainFrame::OnCreatePolygonDisk;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateNurbsFromMesh()
{
	if (theEditor().EditObjList.size() != 2)
	{
		AfxMessageBox("UV 메쉬와 대상 메쉬를 순서대로 선택하세요!");
		return;
	}

	std::string ClsName = theEditor().EditObjList[0]->GetClassName();
	if (ClsName != "GMesh")
	{
		AfxMessageBox("UV 메쉬와 대상 메쉬를 순서대로 선택하세요!");
		return;
	}

	GMesh *pParamMesh = (GMesh *)theEditor().EditObjList[0];
	GMesh *pTargetMesh = (GMesh *)theEditor().EditObjList[1];

	int NumVert = pParamMesh->GetNumVert();
	double *params = new double [NumVert * 2];
	GVector4 *pts = new GVector4 [NumVert];

	for (int i = 0; i < NumVert; ++i)
	{
		params[i * 2] = pParamMesh->Verts[i].P[0];
		params[i * 2 + 1] = pParamMesh->Verts[i].P[1];

		GPoint3 pt = pTargetMesh->Verts[i].P;
		pts[i].Set(pt[0], pt[1], pt[2], 1.0);
	}

	GUcbsSrf *pSrf = get_gucbs_srf_approx(3, 3, NumVert, params, pts, 8);
	//GUcbsSrf *pSrf = get_gucbs_srf_approx_ver3(3, 3, NumVert, params, pts, 9, pTargetMesh->BBox.MinPt, pTargetMesh->BBox.MaxPt);
	pSrf->Renderer.RenderType = RENDER_POLYGON | RENDER_WIRE;
	pSrf->Renderer.SetSampleNum(200, 200);
	pSrf->CreateMesh();
	theScene().AddMesh(pSrf);
	theMainFrm().UpdateWorkSpaceDlg();
	theDoc().UpdateAllViews(NULL);

	delete [] params;
	delete [] pts;
}

void CMainFrame::OnCreatePtsOnMesh()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("메쉬를 선택하세요!");
		return;
	}

	theCreator().InitBaseMesh(NULL);
	theCreator().CreateType = CREATE_PTS_ON_MESH;

	pRepeatFunc = &CMainFrame::OnCreatePtsOnMesh;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateGeodesicPath()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("메쉬를 선택하세요!");
		return;
	}

	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GMesh")
	{
		AfxMessageBox("GMesh 객체를 선택하세요!");
		return;
	}

	// 측지 경로 생성을 위해 생성 툴을 초기화 한다.
	theCreator().InitBaseMesh(pMesh);
	theCreator().CreateType = CREATE_GEODESIC_PATH;

	// 측지 경로 객체를 생성하고 장면에 추가한다.
	std::string Name = create_mesh_name("Geodesic Path");
	GGeoPath *pPath = new GGeoPath(Name, pMesh->MC, pMesh, pMesh->Name);
	theScene().AddMesh(pPath);

	// 작업창과 장면을 갱신한다.
	//theMainFrm().UpdateWorkSpaceDlg();
	pRepeatFunc = &CMainFrame::OnCreateGeodesicPath;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateMirrorMesh()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("미러링할 메쉬를 선택하세요!");
		return;
	}

	GMesh *pCpyMesh = pMesh->CopyObject();

	int NumVert = pCpyMesh->GetNumVert();
	for (int i = 0; i < NumVert; ++i)
		pCpyMesh->Verts[i].P[0] *= -1.0;
	pCpyMesh->ModifiedFlag = MODIFIED_VERT_NUM;
	pCpyMesh->UpdateMesh();

	pCpyMesh->Name = pMesh->Name + "_copy";
	theScene().AddMesh(pCpyMesh);
	theMainFrm().UpdateWorkSpaceDlg();
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateCrvFromSketch()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL || pMesh->GetClassName() != "GMesh")
	{
		AfxMessageBox("곡선이 생성될 메쉬를 선택하세요!");
		return;
	}

	// 곡선 생성을 위해 생성 툴을 초기화 한다.
	theCreator().InitBaseMesh(pMesh);
	theCreator().CreateType = CREATE_CRV_ON_MESH_FROM_SKETCH;
	theDoc().PrintMessage("메쉬위에서 생성할 곡선을 스케치 하세요.");

	// 장면과 작업창을 갱신한다.
	pRepeatFunc = &CMainFrame::OnCreateCrvFromSketch;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateCrvFromGeoPath()
{
	// 측지 경로를 선택하고,
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("메쉬를 선택하세요!");
		return;
	}

	// 선택된 객체가 측지 경로가 아니라며,
	std::string ClsName = pMesh->GetClassName();
	if (ClsName != "GGeoPath")
	{
		AfxMessageBox("측지 경로 (Geodesic Path)를 선택하세요!");
		return;
	}

	int NumObj = theEditor().GetNumEditObj();
	for (int i = 0; i < NumObj; ++i)
	{
		// 측지 경로의 정점 수가 너무 적다면,
		GGeoPath *pPath = (GGeoPath *)(theEditor().EditObjList[i]);
		int NumPt = pPath->GetNumVert();
		if (NumPt < 4)
		{
			AfxMessageBox("측지 경로의 정점 수가 너무 적습니다.");
			continue;
		}

		// 곡선을 생성하여 장면에 추가하고,
		GNurbsCrv *pCrv = ::get_gnurbs_crv_from_geodesic_path(pPath);
		pCrv->Name = ::create_mesh_name("InterCrv");
		pCrv->Renderer.SetSampleNum(20, pCrv->n * 10);
		pCrv->Renderer.RenderType = RENDER_POLYGON;
		double size = dist(pPath->pRefMesh->BBox.MinPt, pPath->pRefMesh->BBox.MaxPt) * 0.002;
		pCrv->Renderer.CtrlPtSize = (theScene().CtrlPtSize == 0.0) ? size : theScene().CtrlPtSize;
		theScene().AddMesh(pCrv);

		GMaterial mtl = ::get_material_random();
		pCrv->AddMtl(mtl);		
		pCrv->SetMtl(mtl.Name);
	}	

	// 장면과 작업창을 갱신한다.
	theMainFrm().UpdateWorkSpaceDlg();
	pRepeatFunc = &CMainFrame::OnCreateCrvFromGeoPath;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateRuledSrf()
{
	// 2개의 넙스 곡선이 선택되지 않았다면 리턴한다.
	if (theEditor().EditObjList.size() != 2)
	{
		AfxMessageBox("2개의 경계 곡선을 선택하세요!");
		return;
	}

	GMesh *pMesh0 = theEditor().EditObjList[0]->GetRefMesh();
	GMesh *pMesh1 = theEditor().EditObjList[1]->GetRefMesh();

	if (pMesh0->GetClassName() != "GNurbsCrv" || pMesh1->GetClassName() != "GNurbsCrv")
	{
		AfxMessageBox("2개의 경계 곡선을 선택하세요!");
		return;
	}

	GNurbsCrv *pCrv0 = (GNurbsCrv *)pMesh0;
	GNurbsCrv *pCrv1 = (GNurbsCrv *)pMesh1;

	GNurbsSrf *pSrf = ::get_gnurbs_srf_ruled(pCrv0, pCrv1, 0);
	pSrf->Name = create_mesh_name("Ruled Srf");

	theScene().AddMesh(pSrf);

	theMainFrm().UpdateWorkSpaceDlg();

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateCoonsSrf()
{
	// 4개의 측지 경로가 선택되지 않았다면, 
	if (theEditor().EditObjList.size() != 4)
	{
		AfxMessageBox("측지 경로상의 4개의 코너 정점를 선택하세요!");
		return;
	}

	for (int i = 0; i < 4; ++i)
	{
		if (theEditor().EditObjList[i]->GetClassName() != "GVertex")
		{
			AfxMessageBox("측지 경로상의 4개의 코너 정점를 선택하세요!");
			return;
		}
	}

	// 코너 정점의 인덱스를 구하여,
	int vidx[4];
	for (int i = 0; i < 4; ++i)
		vidx[i] = ((GVertex *)(theEditor().EditObjList[i]))->Idx;


	GNurbsCrv *pCrv[4] = {NULL};

	// 장면에 포함된 메쉬 리스트를 얻어와서,
	std::vector<GMesh *> MeshList;
	::GetPreOrderMeshList(&theScene(), MeshList, theEditor().GetEditMesh());
	int NumMesh = MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
	{
		if (MeshList[i]->GetClassName() == "GGeoPath")
		{
			GGeoPath *pPath = (GGeoPath *)MeshList[i];
			int sidx = pPath->PathVertIdx[0];
			int eidx = pPath->PathVertIdx[pPath->PathVertIdx.size() - 1];

			// 현재 객체가 첫 번째 경로라면,
			if (vidx[0] == sidx && vidx[1] == eidx)
			{
				pCrv[0] = get_gnurbs_crv_from_geodesic_path(pPath);
			}
			// 현재 객체가 뒤집힌 첫 번째 경로라면,
			if (vidx[0] == eidx && vidx[1] == sidx)
			{
				GNurbsCrv *tmp = get_gnurbs_crv_from_geodesic_path(pPath);
				pCrv[0] = get_gnurbs_crv_reverse(tmp);
				delete tmp;
			}

			// 현재 객체가 두 번째 경로라면,
			if (vidx[1] == sidx && vidx[2] == eidx)
			{
				pCrv[1] = get_gnurbs_crv_from_geodesic_path(pPath);
			}
			// 현재 객체가 뒤집힌 두 번째 경로라면,
			if (vidx[1] == eidx && vidx[2] == sidx)
			{
				GNurbsCrv *tmp = get_gnurbs_crv_from_geodesic_path(pPath);
				pCrv[1] = get_gnurbs_crv_reverse(tmp);
				delete tmp;
			}

			// 현재 객체가 세 번째 경로라면,
			if (vidx[2] == sidx && vidx[3] == eidx)
			{
				pCrv[2] = get_gnurbs_crv_from_geodesic_path(pPath);
			}
			// 현재 객체가 뒤집힌 세 번째 경로라면,
			if (vidx[2] == eidx && vidx[3] == sidx)
			{
				GNurbsCrv *tmp = get_gnurbs_crv_from_geodesic_path(pPath);
				pCrv[2] = get_gnurbs_crv_reverse(tmp);
				delete tmp;
			}

			// 현재 객체가 네 번째 경로라면,
			if (vidx[3] == sidx && vidx[0] == eidx)
			{
				pCrv[3] = get_gnurbs_crv_from_geodesic_path(pPath);
			}
			// 현재 객체가 뒤집힌 첫 번째 경로라면,
			if (vidx[3] == eidx && vidx[0] == sidx)
			{
				GNurbsCrv *tmp = get_gnurbs_crv_from_geodesic_path(pPath);
				pCrv[3] = get_gnurbs_crv_reverse(tmp);
				delete tmp;
			}
		}
	}

	// 4개의 경계 곡선이 모두 생성되었다면, Coons 패치를 생성한다.
	if (pCrv[0] != NULL && pCrv[1] != NULL && pCrv[2] != NULL && pCrv[3] != NULL)
	{
		//GCoonsSrf *pSrf = new GCoonsSrf("", theEditor().GetEditMesh()->MC, pCrv[0], pCrv[1], pCrv[2], pCrv[3], "");
		GNurbsSrf *pSrf = ::get_gnurbs_srf_from_bndry_crvs(pCrv[0], pCrv[1], pCrv[2], pCrv[3]);
		pSrf->Name = ::create_mesh_name("CoonsSrf");
		pSrf->MC = theEditor().GetEditMesh()->MC;
		pSrf->Renderer.SetSampleNum(10, 10);
		pSrf->Renderer.RenderType = RENDER_POLYGON | RENDER_WIRE;
		theScene().AddMesh(pSrf);
	}
	else
	{
		AfxMessageBox("4개의 코너 정점이 일치하지 않습니다.");
	}
	
	if (pCrv[0] != NULL) delete pCrv[0];
	if (pCrv[1] != NULL) delete pCrv[1];
	if (pCrv[2] != NULL) delete pCrv[2];
	if (pCrv[3] != NULL) delete pCrv[3];


	// 장면과 작업창을 갱신한다.
	theMainFrm().UpdateWorkSpaceDlg();
	pRepeatFunc = NULL;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateGeodesicMesh()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("참조 메쉬를 선택하세요.");
		return;
	}

	theCreator().InitBaseMesh(pMesh);
	theCreator().CreateType = CREATE_GEODESIC_MESH;

	std::string Name = create_mesh_name("GeodesicMesh");
	GCoonsMesh *pSrf = new GCoonsMesh(Name, pMesh);
	pSrf->Renderer.SetSampleNum(30, 30);
	pSrf->Renderer.RenderType |= RENDER_BOUNDARY;
	theScene().AddMesh(pSrf);
	theMainFrm().UpdateWorkSpaceDlg();

	pRepeatFunc = &CMainFrame::OnCreateGeodesicMesh;
	theDoc().UpdateAllViews(NULL);
}


void CMainFrame::OnCreateCoonsMesh()
{
	// 4개의 코너 정점이 선택되지 않았다면,
	if (theEditor().EditObjList.size() != 4)
	{
		AfxMessageBox("측지 경로상의 4개의 코너 정점를 선택하세요!");
		return;
	}
	for (int i = 0; i < 4; ++i)
	{
		if (theEditor().EditObjList[i]->GetClassName() != "GVertex")
		{
			AfxMessageBox("측지 경로상의 4개의 코너 정점를 선택하세요!");
			return;
		}
	}

	// 코너 정점의 인덱스를 구하여,
	int vidx[4];
	for (int i = 0; i < 4; ++i)
		vidx[i] = ((GVertex *)(theEditor().EditObjList[i]))->Idx;

	// 참조 메쉬를 구한다.
	GMesh *pRefMesh = theEditor().GetEditMesh();
	GNurbsCrv *pCrv[4] = {NULL};

	// 장면에 포함된 메쉬 리스트를 얻어와서,
	std::vector<GMesh *> MeshList;
	::GetPreOrderMeshList(&theScene(), MeshList, theEditor().GetEditMesh());
	int NumMesh = MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
	{
		// 측지 경로에 대해서,	
		if (MeshList[i]->GetClassName() == "GGeoPath")
		{
			GGeoPath *pPath = (GGeoPath *)MeshList[i];
			int sidx = pPath->GetStartVertIdx();
			int eidx = pPath->GetEndVertIdx();

			// 현재 객체가 첫 번째 경로라면,
			if (vidx[0] == sidx && vidx[1] == eidx)
			{
				pCrv[0] = get_gnurbs_crv_from_geodesic_path(pPath);
			}
			// 현재 객체가 뒤집힌 첫 번째 경로라면,
			if (vidx[0] == eidx && vidx[1] == sidx)
			{
				GNurbsCrv *tmp = get_gnurbs_crv_from_geodesic_path(pPath);
				pCrv[0] = get_gnurbs_crv_reverse(tmp);
				delete tmp;
			}

			// 현재 객체가 두 번째 경로라면,
			if (vidx[1] == sidx && vidx[2] == eidx)
			{
				pCrv[1] = get_gnurbs_crv_from_geodesic_path(pPath);
			}
			// 현재 객체가 뒤집힌 두 번째 경로라면,
			if (vidx[1] == eidx && vidx[2] == sidx)
			{
				GNurbsCrv *tmp = get_gnurbs_crv_from_geodesic_path(pPath);
				pCrv[1] = get_gnurbs_crv_reverse(tmp);
				delete tmp;
			}

			// 현재 객체가 세 번째 경로라면,
			if (vidx[2] == sidx && vidx[3] == eidx)
			{
				pCrv[2] = get_gnurbs_crv_from_geodesic_path(pPath);
			}
			// 현재 객체가 뒤집힌 세 번째 경로라면,
			if (vidx[2] == eidx && vidx[3] == sidx)
			{
				GNurbsCrv *tmp = get_gnurbs_crv_from_geodesic_path(pPath);
				pCrv[2] = get_gnurbs_crv_reverse(tmp);
				delete tmp;
			}

			// 현재 객체가 네 번째 경로라면,
			if (vidx[3] == sidx && vidx[0] == eidx)
			{
				pCrv[3] = get_gnurbs_crv_from_geodesic_path(pPath);
			}
			// 현재 객체가 뒤집힌 첫 번째 경로라면,
			if (vidx[3] == eidx && vidx[0] == sidx)
			{
				GNurbsCrv *tmp = get_gnurbs_crv_from_geodesic_path(pPath);
				pCrv[3] = get_gnurbs_crv_reverse(tmp);
				delete tmp;
			}
		}
	}

	// 4개의 경계 곡선이 모두 생성되었다면, Coons 패치를 생성한다.
	if (pCrv[0] != NULL && pCrv[1] != NULL && pCrv[2] != NULL && pCrv[3] != NULL)
	{
		// 임시적인 Coons 패치를 생성하여,
		GCoonsSrf *pSrf = new GCoonsSrf("CoonsSrf", theEditor().GetEditMesh()->MC, pCrv[0], pCrv[1], pCrv[2], pCrv[3], "");
		pSrf->Renderer.SetSampleNum(20, 20);
		pSrf->CreateMesh();
		
		// 참조 메쉬와의 변위를 구하고, 
		std::vector<int> vIdxs;
		std::vector<double> Distances;
		get_scalar_displacement(pSrf, pRefMesh, Distances, vIdxs, 20.0, false);

		// Coons 패치를 이용하여 참조 메쉬를 근사한다.
		int su = pSrf->Renderer.NumSampU;
		int sv = pSrf->Renderer.NumSampV;
		for (int i = 1; i < su - 1; ++i)
		{
			for (int j = 1; j < sv - 1; ++j)
			{
				int vidx = i * sv + j;
				double d = Distances[vidx];
				pSrf->Verts[vidx].P = pSrf->Verts[vidx].P + d * pSrf->Verts[vidx].N;
			}
		}
		// 정점의 법선을 갱신한다.
		pSrf->UpdateNormal(false);

		// Coons 패치를 이용하여 Coons 메쉬를 생성하여 장면에 추가한다.
		GMesh *pMesh = new GMesh("", theEditor().GetEditMesh()->MC, FACE_QUAD, "");
		pMesh->Name = ::create_mesh_name("CoonsMesh");
		pMesh->Renderer.RenderType = RENDER_POLYGON | RENDER_WIRE;
		pMesh->Verts = pSrf->Verts;
		pMesh->Texs = pSrf->Texs;
		pMesh->Faces = pSrf->Faces;
		theScene().AddMesh(pMesh);
	}
	else
	{
		AfxMessageBox("4개의 코너 정점이 일치하지 않습니다.");
	}

	if (pCrv[0] != NULL) delete pCrv[0];
	if (pCrv[1] != NULL) delete pCrv[1];
	if (pCrv[2] != NULL) delete pCrv[2];
	if (pCrv[3] != NULL) delete pCrv[3];


	// 장면과 작업창을 갱신한다.
	theMainFrm().UpdateWorkSpaceDlg();
	pRepeatFunc = NULL;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateCoonsMeshList()
{
	// 선택된 메쉬를 구하여,
	GMesh *pMesh = theEditor().GetEditMesh();

	// 선택된 메쉬가 없다면 리턴한다.
	if (pMesh == NULL || pMesh->GetClassName() != "GGeoPath")
	{
		AfxMessageBox("메쉬를 생성할 레이아웃 측지선들을 선택하세요!");
		return;
	}
	
	// 선택된 측지선의 개수를 구하여 PathList에 추가한다.
	int NumEdge = theEditor().GetNumEditObj();
	std::vector<GGeoPath *> PathList;
	for (int i = 0; i < NumEdge; ++i)
	{
		GGeoPath *pPath = (GGeoPath *)(theEditor().EditObjList[i]);
		PathList.push_back(pPath);
	}

	// 각 측지선의 시작점과 끝점에 중복없이 새로운 인덱스를 할당한다.
	std::map<int, int> MapVertIdx;
	for (int i = 0, vidx = 1; i < NumEdge; ++i)
	{
		// 경로의 시작점과 끝점의 인덱스를 구하여
		int sidx = PathList[i]->GetStartVertIdx();
		int eidx = PathList[i]->GetEndVertIdx();

		// 시작점이 새로운 점이라면, 새로운 인덱스를 할당한다.
		if (MapVertIdx[sidx] == 0)	
			MapVertIdx[sidx] = vidx++;

		// 끝점이 새로운 점 이라면, 새로운 인덱스를 할당한다.
		if (MapVertIdx[eidx] == 0)
			MapVertIdx[eidx] = vidx++;		
	}

	// 경로의 레이아웃을 표현하는 incidence 행렬 M을 정의한다.
	int NumVert = (int)MapVertIdx.size();
	GMatrix M(NumVert, NumEdge);

	// 각 경로에 대하여, 시작점을 -1, 끝점을 1로 설정하여
	for (int i = 0; i < NumEdge; ++i)
	{
		int sidx = MapVertIdx[PathList[i]->GetStartVertIdx()] - 1;
		int eidx = MapVertIdx[PathList[i]->GetEndVertIdx()] - 1;
		M[sidx][i] = -1.0;
		M[eidx][i] = 1.0;
	}
	// 계산된 incidence 행렬을 출력한다.
	std::cout << M << std::endl;

// 	clock_t st, ed;
// 	CString info;
// 	st = clock();

	// Incidence 행렬의 null 공간의 기저를 구하여 출력한다.
	GMatrix N = ::basis_null(M);

// 	std::cout << N << std::endl;
// 	ed = clock();
// 	info.Format("%d", ed - st);
// 	AfxMessageBox(info);

//	st = clock();
	// 사이클(Coons 메쉬)의 수를 구하여,
	int NumCycle = N.GetColNum();
	bool flag;
	int iter = 1;
	do
	{
		flag = false;
		for (int i = 0; i < NumCycle; ++i)
		{
			GVector ci = N.GetColVec(i);
			if (!flag && norm(ci) > 2.1)
				flag = true;

			for (int j = 0; j < NumCycle; ++j)
			{
				GVector cj = N.GetColVec(j);
				if (j == i || norm(cj) < 2.1)	
					continue;

				if (cj >= cj + ci)
					N.SetColVec(j, cj + ci);
				else if (cj >= cj - ci)
					N.SetColVec(j, cj - ci);
			}
		}

 		std::cout << iter << "-th iteration" << std::endl;
 		std::cout << N << std::endl;
		iter++;
		if (iter > 10)
			break;
	} while(flag);

// 	GMatrix ret = M * N;
// 	std::cout << ret << std::endl;
// 	ed = clock();
// 	info.Format("%d", ed - st);
// 	AfxMessageBox(info);
	
	// 각의 Cycle 마다,
 	for (int i = 0; i < NumCycle; ++i)
 	{
 		// Coons 메쉬를 구성할 4개의 경계선 정보(시작점, 끝점)를 저장할 변수를 정의하고,
 		std::map<int, int> EdgeList;
 		GVector x = N.GetColVec(i);
		for (int j = 0; j < NumEdge; ++j)
		{
			int sidx = PathList[j]->GetStartVertIdx();
			int eidx = PathList[j]->GetEndVertIdx();
			
			// x[j] == 1.0 과 같다면,
			if (EQ(x[j], 1.0, 1e-6))
				EdgeList[sidx] = eidx;				
			
			// x[j] == -1.0 과 같다면,
			if (EQ(x[j], -1.0, 1e-6))
				EdgeList[eidx] = sidx;
		}

		// 4개의 경로로 구성된 사이클이라면,
		if (EdgeList.size() == 4)
		{
			// 경계 정점을 이용하여 Coon 메쉬를 생성한다.
			int vidx[4];
			std::map<int, int>::iterator it = EdgeList.begin();
			vidx[0] = it->first;
			vidx[1] = it->second;
			vidx[2] = EdgeList[vidx[1]];
			vidx[3] = EdgeList[vidx[2]];

			pMesh = PathList[0]->pRefMesh;
			theEditor().Reset();

			GVector3 a = pMesh->Verts[vidx[1]].P - pMesh->Verts[vidx[0]].P;
			GVector3 b = pMesh->Verts[vidx[3]].P - pMesh->Verts[vidx[0]].P;
			GVector3 n = (a ^ b).Normalize();
			GVector3 n0 = pMesh->Verts[vidx[0]].N;

			if (angle(n, n0) <= 90.0)
			{
				theEditor().AddToEditObjList(&(pMesh->Verts[vidx[0]]));
				theEditor().AddToEditObjList(&(pMesh->Verts[vidx[1]]));
				theEditor().AddToEditObjList(&(pMesh->Verts[vidx[2]]));
				theEditor().AddToEditObjList(&(pMesh->Verts[vidx[3]]));
			}
			else
			{
				theEditor().AddToEditObjList(&(pMesh->Verts[vidx[3]]));
				theEditor().AddToEditObjList(&(pMesh->Verts[vidx[2]]));
				theEditor().AddToEditObjList(&(pMesh->Verts[vidx[1]]));
				theEditor().AddToEditObjList(&(pMesh->Verts[vidx[0]]));
			}
			OnCreateCoonsMesh();			
		}		
	}
}

void CMainFrame::OnFitMeshByOffset()
{
	if (theEditor().EditObjList.size() != 2)
	{
		AfxMessageBox("타겟 메쉬와 소스 메쉬를 선택하세요!");
		return;
	}
	for (int i = 0; i < 2; ++i)
	{
		if (theEditor().EditObjList[i]->GetClassName() != "GMesh")
		{
			AfxMessageBox("타겟 메쉬와 소스 메쉬를 선택하세요!");
			return;
		}
	}

	GMesh *pTarMesh = (GMesh *)theEditor().EditObjList[0];
	GMesh *pSrcMesh = (GMesh *)theEditor().EditObjList[1];

	std::vector<int> vIdxs;
	std::vector<double> Distances;
	get_scalar_displacement(pSrcMesh, pTarMesh, Distances, vIdxs, 20.0, false);

	int NumVert = pSrcMesh->GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		double d = Distances[i];
		pSrcMesh->Verts[i].P = pSrcMesh->Verts[i].P + d * pSrcMesh->Verts[i].N;
	}

	// 정점의 법선을 계산하고,
	pSrcMesh->UpdateNormal(false);

	// 곡면의 바운딩 박스를 계산한다.
	pSrcMesh->UpdateBndBox(false);

	// 정점 버퍼와 인덱스 버퍼를 갱신한다.
	pSrcMesh->UpdateBufVertex();
	pSrcMesh->UpdateBufNormal();
	pSrcMesh->UpdateBufFace();


	// 장면과 작업창을 갱신한다.
	theMainFrm().UpdateWorkSpaceDlg();
	pRepeatFunc = NULL;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnComputeMeshArea()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("면적을 측정할 메쉬를 선택하세요.");
		return;
	}

	double Area = get_mesh_area(pMesh);
	std::string msg = "메쉬의 총 면적은 " + cast_str(Area) + " 입니다";
	theDoc().PrintMessage(msg);
}

void CMainFrame::OnComputeCurvature()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("곡률을 측정할 메쉬를 선택하세요.");
		return;
	}

	pMesh->Renderer.VertColorType = VERTEX_COLOR_CURVATURE;

	// 대화 상자가 생성되지 않았다면, 생성하여 출력한다.
	if (theDoc().m_pCurvatureDlg == NULL)
	{
		theDoc().m_pCurvatureDlg = new GCurvatureDlg();
		theDoc().m_pCurvatureDlg->Create(IDD_CURVATURE_DLG);
		theDoc().m_pCurvatureDlg->m_pMesh = pMesh;
		theDoc().m_pCurvatureDlg->OnInitDialog();
		theDoc().m_pCurvatureDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		theDoc().m_pCurvatureDlg->m_pMesh = pMesh;
		theDoc().m_pCurvatureDlg->OnInitDialog();
		theDoc().m_pCurvatureDlg->ShowWindow(SW_SHOW);
	}
}

void CMainFrame::OnCreateDepthMap()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("메쉬를 선택하세요.");
		return;
	}

	GDepthViewDlg dlg;
	dlg.m_MeshList.push_back(pMesh);
	
	GaiaView *pView = (GaiaView *)GetActiveView();
	::wglMakeCurrent(NULL, NULL);
	{
		dlg.DoModal();		
	}	
	::wglMakeCurrent(pView->m_hDC, pView->m_hRC);
}

// GCutLine *pCutLine = NULL;
// float D0 = 0.0;	// 초기 길이

// float CostFunc(float *params)
// {
// 	float len_sum = 0.0f;
// 	float kappa_sum = 0.0f;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
// 	pCutLine->GetLengthAndKappaSum(params, &len_sum, &kappa_sum);
// 
// 	float cost = SQR(D0 - len_sum) + 0.1 * kappa_sum;
// 	//float cost = kappa_sum;
// 	return cost;
// }

// 
// void CMainFrame::OnOptimizeCutLine()
// {
// 	GMesh *pMesh = theEditor().GetEditMesh();
// 	if (pMesh == NULL)
// 	{
// 		AfxMessageBox("메쉬를 선택하세요.");
// 		return;
// 	}
// 	
// 	if (pMesh->GetClassName() != "GCutLine")
// 	{
// 		AfxMessageBox("절단선 메쉬를 선택하세요!");
// 		return;
// 	}
// 
// 	pCutLine = (GCutLine *)pMesh;
// 	int NumPt = (int)pCutLine->PtList.size();
// 
// 	float *sol = new float [NumPt + 1];
// 	sol[0] = 0.0f;
// 	for (int i = 1; i < NumPt + 1; ++i)
// 		sol[i] = pCutLine->PtList[i - 1].t;
// 
// 	float tmp;
// 	pCutLine->GetLengthAndKappaSum(sol, &D0, &tmp);
// 
// 	float **xi = new float *[NumPt + 1];
// 	for (int i = 0; i < NumPt + 1; ++i)
// 	{
// 		xi[i] = new float [NumPt + 1];
// 		for (int j = 0; j < NumPt + 1; ++j)
// 		{
// 			if (i == j)
// 				xi[i][j] = 1.0f;
// 			else
// 				xi[i][j] = 0.0f;
// 		}
// 	}
// 	xi[0][0] = 0.0f;
// 
// 	int iter;
// 	float ret;
// 	::powell(sol, xi, NumPt, 0.0001, &iter, &ret, CostFunc);
// 
// 	// 새로운 파라미터에 해당하는 절단점의 리스트를 구한다.
// 	for (int i = 0; i < NumPt; ++i)
// 	{
// 		int vidx0 = pCutLine->PtList[i].vIdx[0];
// 		int vidx1 = pCutLine->PtList[i].vIdx[1];
// 
// 		GPoint3 v0 = pCutLine->PtList[i].pFace->pMesh->Verts[vidx0].P;
// 		GPoint3 v1 = pCutLine->PtList[i].pFace->pMesh->Verts[vidx1].P;
// 
// 		float t = sol[i + 1];
// 	 	if (t > 1.0) t = 0.5;
//  		if (t < 0.0) t = 0.5;
// 
// 		pCutLine->PtList[i].P = v0 + t * (v1 - v0);
// 	}
// 
// 	pCutLine->ModifiedFlag = MODIFIED_VERT_POS;
// 	pCutLine->UpdateMesh();
// }

// using namespace alglib;
// GCutLine *pCutLine = NULL;
// double D0 = 0.0;	// 초기 길이
// void CostFunc(const real_1d_array &x, double &func, void *ptr)
// {
// 	// 절단점을 저장할 변수를 정의하고,
// 	std::vector<GPoint3> Pts;
// 
// 	// 새로운 파라미터에 해당하는 절단점의 리스트를 구한다.
// 	int NumPt = (int)pCutLine->PtList.size();
// 	for (int i = 0; i < NumPt; ++i)
// 	{
// 		int vidx0 = pCutLine->PtList[i].vIdx[0];
// 		int vidx1 = pCutLine->PtList[i].vIdx[1];
// 
// 		GPoint3 v0 = pCutLine->PtList[i].pFace->pMesh->Verts[vidx0].P;
// 		GPoint3 v1 = pCutLine->PtList[i].pFace->pMesh->Verts[vidx1].P;
// 
// 		float t = x[i];
// 		GPoint3 pt = v0 + t * (v1 - v0);
// 		Pts.push_back(pt);
// 	}
// 
// 	double len_sum = 0.0;
// 	double kappa_sum = 0.0;
// 
// 	for (int i = 0; i < NumPt - 1; ++i)
// 	{
// 		GPoint3 q = Pts[i];
// 		GPoint3 r = Pts[i + 1];
// 		len_sum += dist(q, r);
// 
// 		if (i > 0)
// 		{
// 			GPoint3 p = Pts[i - 1];
// 			GVector3 e = r - q;
// 			GVector3 d = q - p;
// 			double len_e = norm(e);
// 			double len_d = norm(d);
// 
// 			GVector3 Kappa = 2.0 * (e.Normalize() - d.Normalize()) / (len_e + len_d);
// 			kappa_sum += norm(Kappa);
// 		}
// 	}
// 	func = 0.5 * SQR(D0 - len_sum) + 0.1 * kappa_sum;
// }

void CMainFrame::OnOptimizeCutLine()
{
// 	GMesh *pMesh = theEditor().GetEditMesh();
// 	if (pMesh == NULL)
// 	{
// 		AfxMessageBox("메쉬를 선택하세요.");
// 		return;
// 	}
// 
// 	if (pMesh->GetClassName() != "GCutLine")
// 	{
// 		AfxMessageBox("절단선 메쉬를 선택하세요!");
// 		return;
// 	}
// 
// 	/////////////////////////////////////////////////////////////////
// 	pCutLine = (GCutLine *)pMesh;
// 	int NumPt = (int)pCutLine->PtList.size();
// 	D0 = 0.0;
// 	for (int i = 0; i < NumPt - 1; ++i)
// 	{
// 		GPoint3 p = pCutLine->PtList[i].P;
// 		GPoint3 q = pCutLine->PtList[i + 1].P;
// 		D0 += dist(p, q);
// 	}
// 
// 	///////////////////////////////////////////////////////////////////
// 	std::string str_x = "[";
// 	std::string str_bndl = "[";
// 	std::string str_bndu = "[";
// 	for (int i = 0; i < NumPt; ++i)
// 	{
// 		double t = pCutLine->PtList[i].t;
// 		if (i == NumPt - 1)
// 		{
// 			str_x += (CStringA)(t) + "]";
// 			str_bndl += "0]";
// 			str_bndu += "1]";
// 		}
// 		else
// 		{
// 			str_x += (CStringA)(t) + ",";
// 			str_bndl += "0,";
// 			str_bndu += "1,";
// 		}
// 	}
// 	real_1d_array x = str_x.c_str();
// 	real_1d_array bndl = str_bndl.c_str();
// 	real_1d_array bndu = str_bndu.c_str();
// 	minbleicstate state;
// 	minbleicreport rep;
// 
// 	double epsg = 0.00001;
// 	double epsf = 0;
// 	double epsx = 0;
// 	ae_int_t maxits = 0;
// 	double diffstep = 1.0e-5;
// 
// 	minbleiccreatef(x, diffstep, state);
// 	minbleicsetbc(state, bndl, bndu);
// 	minbleicsetcond(state, epsg, epsf, epsx, maxits);
// 	alglib::minbleicoptimize(state, CostFunc);
// 	minbleicresults(state, x, rep);
// 
// 	//printf("%d\n", int(rep.terminationtype)); // EXPECTED: 4
// 	//printf("%s\n", x.tostring(2).c_str()); // EXPECTED: [-1,1]
// 	////////////////////////////////////////////////////////////////////
// 
// 	// 새로운 파라미터에 해당하는 절단점의 리스트를 구한다.
// 	for (int i = 0; i < NumPt; ++i)
// 	{
// 		int vidx0 = pCutLine->PtList[i].vIdx[0];
// 		int vidx1 = pCutLine->PtList[i].vIdx[1];
// 
// 		GPoint3 v0 = pCutLine->PtList[i].pFace->pMesh->Verts[vidx0].P;
// 		GPoint3 v1 = pCutLine->PtList[i].pFace->pMesh->Verts[vidx1].P;
// 
// 		float t = x[i];
// 		GPoint3 pt = v0 + t * (v1 - v0);
// 		pCutLine->PtList[i].P = pt;
// 	}
// 
// 	pCutLine->ModifiedFlag = MODIFIED_VERT_POS;
// 	pCutLine->UpdateMesh();
}

void CMainFrame::OnAddClippingPlane()
{
	// 선택된 메쉬를 찾아 없으면 리턴한다.
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("메쉬를 선택하세요.");
		return;
	}

	// 6개의 절단면을 추가한다.
	pMesh->AddClipPlanes();

	theMainFrm().UpdateWorkSpaceDlg();
	theDoc().UpdateAllViews(NULL);
}


void CMainFrame::OnCreateUnfoldedMeshSquare()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("메쉬를 선택하세요.");
		return;
	}

	int vidx = -1;
	if (theEditor().GetClsNameEditObj() == "GVertex")
		vidx = ((GVertex *)theEditor().EditObjList[0])->Idx;

	GMesh *pUnfoldMesh = ::create_mesh_unfolded(create_mesh_name("Unfolded_Mesh"), pMesh, vidx, 0);
	theScene().AddMesh(pUnfoldMesh);

	theMainFrm().UpdateWorkSpaceDlg();
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateUnfoldedMeshCircle()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		theDoc().PrintMessage("메쉬를 선택하세요.");
		return;
	}

	int vidx = -1;
	if (theEditor().GetClsNameEditObj() == "GVertex")
		vidx = ((GVertex *)theEditor().EditObjList[0])->Idx;

	GMesh *pUnfoldMesh = ::create_mesh_unfolded(create_mesh_name("Unfolded_Mesh"), pMesh, vidx, 1);
	theScene().AddMesh(pUnfoldMesh);

	theMainFrm().UpdateWorkSpaceDlg();
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateCutLine()
{
	if (theEditor().EditObjList.empty())
	{
		AfxMessageBox("메쉬를 선택하세요.");
		return;
	}

	GMesh *pMesh = theEditor().EditObjList[0]->GetRefMesh();
	theCreator().InitBaseMesh(pMesh);
	theCreator().CreateType = CREATE_CUT_LINE;

	pRepeatFunc = &CMainFrame::OnCreateCutLine;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateCutLineFromSketch()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL || pMesh->GetClassName() != "GMesh")
	{
		AfxMessageBox("메쉬를 선택하세요.");
		return;
	}

	theCreator().InitBaseMesh(pMesh);
	theCreator().CreateType = CREATE_CUT_LINE_FROM_SKETCH;

	pRepeatFunc = &CMainFrame::OnCreateCutLineFromSketch;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreatePqpBvh()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL || pMesh->GetClassName() != "GMesh")
	{
		AfxMessageBox("메쉬를 선택하세요.");
		return;
	}

	pMesh->CreateBVH();
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnDeletePqpBvh()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL || pMesh->GetClassName() != "GMesh")
	{
		AfxMessageBox("메쉬를 선택하세요.");
		return;
	}

	pMesh->DeleteBVH();
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnEditSculpt()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL || pMesh->GetClassName() != "GMesh")
	{
		AfxMessageBox("메쉬를 선택하세요.");
		return;
	}

	theEditor().EditToolType = EDIT_SCULPT;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateEditSculpt(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().EditToolType == EDIT_SCULPT);
}

void CMainFrame::OnEditSmooth()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL || pMesh->GetClassName() != "GMesh")
	{
		AfxMessageBox("메쉬를 선택하세요.");
		return;
	}

	theEditor().EditToolType = EDIT_SMOOTH;
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateEditSmooth(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theEditor().EditToolType == EDIT_SMOOTH);
}

void CMainFrame::OnEditWireframe()
{
	// 절단선이 선택되지 않았다면 리턴한다.
	if (theEditor().EditObjList.empty() || theEditor().GetEditMesh()->GetClassName() != "GCutLine")
	{
		theDoc().PrintMessage("절단선을 선택하세요.");
		return;
	}

	// 선택된 절단선 객체와 절단할 메쉬 객체를 구한다.
	GCutLine *pCutLine = (GCutLine *)(theEditor().GetEditMesh());
	GMesh *pMesh = pCutLine->PtList[0].pFace->pMesh;

	if (pMesh->FaceType != FACE_TRI)
	{
		AfxMessageBox("삼각형 메쉬만 가능합니다.");
		return;
	}

	// 움직인 정점의 정보를 저장하는 변수를 정의한다.
	std::map<int, double> ModifiedVertDist;
	std::vector<GVertex> OldVerts = pMesh->Verts;

	int NumPt = (int)pCutLine->PtList.size();
	for	(int i = 0; i < NumPt; ++i)
	{
		int vidx0 = pCutLine->PtList[i].vIdx[0];
		int vidx1 = pCutLine->PtList[i].vIdx[1];
		double d0 = dist(OldVerts[vidx0].P, pCutLine->PtList[i].P);
		double d1 = dist(OldVerts[vidx1].P, pCutLine->PtList[i].P);

		if (d0 < d1)
		{
			if (ModifiedVertDist[vidx0] == 0.0 || d0 < ModifiedVertDist[vidx0])
			{
				ModifiedVertDist[vidx0] = d0;
				pMesh->Verts[vidx0].P = pCutLine->PtList[i].P;
			}
		}
		else
		{
			if (ModifiedVertDist[vidx1] == 0.0 || d1 < ModifiedVertDist[vidx1])
			{
				ModifiedVertDist[vidx1] = d1;
				pMesh->Verts[vidx1].P = pCutLine->PtList[i].P;
			}
		}		
	}

	pMesh->ModifiedFlag = MODIFIED_VERT_POS_ALL;
	pMesh->UpdateMesh();

	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateTriangleStrip()
{
	// 3개 이하의 정점이 선택되었거나, 선택된 객체가 정점이 아니라면 리턴한다.
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL || theEditor().EditObjList.size() < 3 || theEditor().EditObjList[0]->GetClassName() != "GVertex")
	{
		AfxMessageBox("삼각형을 생성할 세 정점을 반시계 방향으로 선택하세요.");
		return;
	}

	int NumVert = theEditor().GetNumEditObj();
	for (int i = 0; i < NumVert - 2; ++i)
	{
		if (i % 2 == 0)
		{
			GVertex *v0 = (GVertex *)(theEditor().EditObjList[i]);
			GVertex *v1 = (GVertex *)(theEditor().EditObjList[i + 1]);
			GVertex *v2 = (GVertex *)(theEditor().EditObjList[i + 2]);
			GFace f(v0->Idx, v1->Idx, v2->Idx, v0->Idx, v1->Idx, v2->Idx);
			pMesh->AddFace(f);
		}
		else
		{
			GVertex *v0 = (GVertex *)(theEditor().EditObjList[i + 1]);
			GVertex *v1 = (GVertex *)(theEditor().EditObjList[i]);
			GVertex *v2 = (GVertex *)(theEditor().EditObjList[i + 2]);
			GFace f(v0->Idx, v1->Idx, v2->Idx, v0->Idx, v1->Idx, v2->Idx);
			pMesh->AddFace(f);
		}		
	}

	pMesh->ModifiedFlag = MODIFIED_VERT_NUM;
	pMesh->UpdateMesh();
	theDoc().UpdateAllViews(NULL);
}


void CMainFrame::OnEditLocalRefine()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL || theEditor().EditObjList[0]->GetClassName() != "GFace")
	{
		AfxMessageBox("분할할 삼각형 영역을 선택하세요.");
		return;
	}
	if (pMesh->FaceType == FACE_QUAD || pMesh->FaceType == FACE_TRI_FAN)
	{
		AfxMessageBox("지원되지 않는 메쉬 타입입니다.");
		return;
	}

	// 필요한 변수를 정의한다.	
	std::vector<GFace> NewFaces;				// 메쉬의 새로운 삼각형 리스트를 저장할 변수
	std::map<std::string, int> EdgeFaceMap;		// 모든 삼각형의 (에지, 삼각형 인덱스)를 저장할 변수
	std::map<std::string, int> EdgeVertMap;		// 제거할 삼각형의 (에지, 중점의 인덱스)를 저장할 변수
	std::vector<bool> DelFaceFlag(pMesh->GetNumFace(), false);	// 삼각형의 제거 여부를 기록하는 변수
	pMesh->GrpFaces.clear();	

	// 모든 삼각형의 각각의 에지에 대하여,
	for (int i = 0; i < pMesh->GetNumFace(); ++i)
	{
		// (에지, 삼각형 인덱스) 맵을 생성한다.
		int *vidx = pMesh->Faces[i].vIdx;
		std::string edge01 = cast_str(vidx[0]) + std::string("-") + cast_str(vidx[1]);
		std::string edge12 = cast_str(vidx[1]) + std::string("-") + cast_str(vidx[2]);
		std::string edge20 = cast_str(vidx[2]) + std::string("-") + cast_str(vidx[0]);
		EdgeFaceMap[edge01] = EdgeFaceMap[edge12] = EdgeFaceMap[edge20] = pMesh->Faces[i].Idx;
	}	

	// 선택된 삼각형에 대하여, 1차적으로 삭제할 삼각형임을 기록한다.
	for (int i = 0; i < theEditor().GetNumEditObj(); ++i)		
		DelFaceFlag[((GFace *)theEditor().EditObjList[i])->Idx] = true;
	
	// 모든 삼각형에 대하여,
	for (int i = 0; i < pMesh->GetNumFace(); ++i)
	{
		// 이미 삭제 기록이 되었다면 건너띄고,
		if (DelFaceFlag[i])
			continue;

		GFace *pFace = &pMesh->Faces[i];
		int *vidx = pFace->vIdx;
		std::string edge10 = cast_str(vidx[1]) + std::string("-") + cast_str(vidx[0]);
		std::string edge21 = cast_str(vidx[2]) + std::string("-") + cast_str(vidx[1]);
		std::string edge02 = cast_str(vidx[0]) + std::string("-") + cast_str(vidx[2]);
		int fidx0 = EdgeFaceMap[edge10];
		int fidx1 = EdgeFaceMap[edge21];
		int fidx2 = EdgeFaceMap[edge02];
		
		// 외접한 삼각형 중 삭제할 삼각형의 개수를 구하여,
		int cnt = 0;
		if (DelFaceFlag[fidx0])
			cnt++;
		if (DelFaceFlag[fidx1])
			cnt++;
		if (DelFaceFlag[fidx2])
			cnt++;

		// 삭제할 삼각형과 두 에지가 인접했다면 삭제할 삼각형에 추가한다.
		if (cnt == 2)
		{
			theEditor().AddToEditObjList(pFace);
			DelFaceFlag[pFace->Idx] = true;
		}
	}	

	// 삭제할 삼각형에 대하여,
	for (int i = 0; i < theEditor().GetNumEditObj(); ++i)
	{
		// 정점의 인덱스를 구하여,
		GFace *pFace = (GFace *)(theEditor().EditObjList[i]);
		int vidx0 = pFace->vIdx[0];
		int vidx1 = pFace->vIdx[1];
		int vidx2 = pFace->vIdx[2];
		int mtl_idx = pFace->MtlIdx;		

		// 에지 문자열을 만들고,
		std::string edge01 = cast_str(vidx0) + std::string("-") + cast_str(vidx1);
		std::string edge10 = cast_str(vidx1) + std::string("-") + cast_str(vidx0);
		std::string edge12 = cast_str(vidx1) + std::string("-") + cast_str(vidx2);
		std::string edge21 = cast_str(vidx2) + std::string("-") + cast_str(vidx1);
		std::string edge20 = cast_str(vidx2) + std::string("-") + cast_str(vidx0);
		std::string edge02 = cast_str(vidx0) + std::string("-") + cast_str(vidx2);

		// 반대편 에지(vidx1, vidx0)의 중점이 추가되지 않았다면,
		if (EdgeVertMap[edge10] == 0)
		{
			// 에지의 중점을 구하여 정점 리스트에 추가하고,
			GVertex v;
			v.P = cast_pt3((cast_vec3(pMesh->Verts[vidx0].P) + cast_vec3(pMesh->Verts[vidx1].P)) * 0.5);
			pMesh->AddVertex(v);

			// 에지의 중점의 인덱스를 기록한다.
			EdgeVertMap[edge01] = pMesh->GetNumVert() - 1;
		}		
		else	// 반대편 에지(vidx1, vidx0)의 중점이 이미 추가된 상태라면, 중점의 인덱스를 저장한다.
			EdgeVertMap[edge01] = EdgeVertMap[edge10];

		// 반대편 에지 (vidx2, vidx1)의 중점이 추가되지 않았다면,
		if (EdgeVertMap[edge21] == 0)
		{
			// 에지의 중점을 구하여 정점 리스트에 추가하고,
			GVertex v;
			v.P = cast_pt3((cast_vec3(pMesh->Verts[vidx1].P) + cast_vec3(pMesh->Verts[vidx2].P)) * 0.5);
			pMesh->AddVertex(v);

			// 에지의 중점의 인덱스를 기록한다.
			EdgeVertMap[edge12] = pMesh->GetNumVert() - 1;
		}
		else
			EdgeVertMap[edge12] = EdgeVertMap[edge21];

		// 반대편 에지 (vidx0, vidx2)의 중점이 추가되지 않았다면,
		if (EdgeVertMap[edge02] == 0)
		{
			// 에지의 중점을 구하여 정점 리스트에 추가하고,
			GVertex v;
			v.P = cast_pt3((cast_vec3(pMesh->Verts[vidx2].P) + cast_vec3(pMesh->Verts[vidx0].P)) * 0.5);
			pMesh->AddVertex(v);

			// 에지의 중점의 인덱스를 기록한다.
			EdgeVertMap[edge20] = pMesh->GetNumVert() - 1;
		}
		else
			EdgeVertMap[edge20] = EdgeVertMap[edge02];

		// 에지 중점의 인덱스를 구하여,
		int vidx3 = EdgeVertMap[edge01];
		int vidx4 = EdgeVertMap[edge12];
		int vidx5 = EdgeVertMap[edge20];

		// 삼각형을 4개의 부분삼각형으로 분할하여 새로운 삼각형 리스트에 추가한다.
		GFace f0(vidx0, vidx3, vidx5, -1, -1, -1, mtl_idx); 
		GFace f1(vidx3, vidx1, vidx4, -1, -1, -1, mtl_idx); 
		GFace f2(vidx3, vidx4, vidx5, -1, -1, -1, mtl_idx); 
		GFace f3(vidx5, vidx4, vidx2, -1, -1, -1, mtl_idx); 
 		NewFaces.push_back(f0);
 		NewFaces.push_back(f1);
 		NewFaces.push_back(f2);
 		NewFaces.push_back(f3);
	}

	// 각각의 (에지, 정점)의 원소에 대하여,
 	std::map<std::string, int>::iterator it_edge;
 	for (it_edge = EdgeVertMap.begin(); it_edge != EdgeVertMap.end(); it_edge++)
 	{
 		if (it_edge->second == 0)
 		{
 			std::string edge01 = it_edge->first; 
 			int fidx = EdgeFaceMap[edge01];

			DelFaceFlag[fidx] = true;
			int mtl_idx = pMesh->Faces[fidx].MtlIdx;

			int vidx0 = atoi(edge01.substr(0, edge01.rfind("-")).c_str());
			int vidx1 = atoi(edge01.substr(edge01.rfind("-") + 1, edge01.length() - edge01.rfind("-")).c_str());
			int vidx2 = pMesh->Faces[fidx].GetNextVertIdx(vidx1);			
			
			std::string edge10 = cast_str(vidx1) + std::string("-") + cast_str(vidx0);
			int vidx3 = EdgeVertMap[edge10];

			if (vidx0 == -1 || vidx1 == -1 || vidx2 == -1)
				continue;

			GFace f0(vidx0, vidx3, vidx2, -1, -1, -1, mtl_idx); 
			GFace f1(vidx3, vidx1, vidx2, -1, -1, -1, mtl_idx); 
			NewFaces.push_back(f0);
			NewFaces.push_back(f1);
		}
	}

	// 남길 삼각형을 추가한다.
	for (int i = 0; i < pMesh->GetNumFace(); ++i)
	{
		if (!DelFaceFlag[i])
			NewFaces.push_back(pMesh->Faces[i]);
	}

	// 메쉬의 삼각형을 갱신한다.
	pMesh->SetNewFaces(NewFaces);
	pMesh->ModifiedFlag = MODIFIED_VERT_NUM;
	pMesh->UpdateMesh();	

	// 편집기를 초기화 한다.
	theEditor().Reset();
	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnSetBridgeStartCrv()
{
	if (theDoc().m_pBridgeMeshDlg == NULL)
	{
		theDoc().m_pBridgeMeshDlg = new GBridgeMeshDlg();
		theDoc().m_pBridgeMeshDlg->Create(IDD_BRIDGE_MESH_DLG);
	}

	GMesh *pStartMesh = theEditor().GetEditMesh();
	theDoc().m_pBridgeMeshDlg->m_pStartMesh = pStartMesh;
	theDoc().m_pBridgeMeshDlg->m_pStartVert = (GVertex *)(theEditor().EditObjList[0]);
	theDoc().m_pBridgeMeshDlg->m_pStartUnfoldMesh = theEditor().pUnfoldMesh->CopyObject();
	theDoc().m_pBridgeMeshDlg->m_StartUnfoldFaceMap = theEditor().UnfoldFaceMap;
	theDoc().m_pBridgeMeshDlg->m_StartCrvSize = pStartMesh->BBox.GetDiagonal() * theEditor().EditMeshDlg.m_EditRgnDist / 1000.0;

	GNurbsCrv *pCrv = ::get_gnurbs_approx_circle(theDoc().m_pBridgeMeshDlg->m_StartCrvSize, 0, true);
	GMaterial mat_blue = get_material(0.0, 0.0, 1.0);
	pCrv->KnotRefine();
	pCrv->KnotRefine();
	pCrv->KnotRefine();
	pCrv->KnotRefine();	
	pCrv->Renderer.RenderType = RENDER_POLYGON;
	pCrv->Renderer.SetSampleNum(20, 200);
	pCrv->Renderer.CtrlPtSize = 0.1;
	pCrv->Renderer.bRenderCrvAsLine = true;
	pCrv->AddMtl(mat_blue);
	pCrv->SetMtl(mat_blue.Name);
	pCrv->CreateMesh();
	theDoc().m_pBridgeMeshDlg->m_pStartContCrv = pCrv;

// 	theDoc().m_pBridgeStMesh = theEditor().GetEditMesh();
// 	theDoc().m_pBridgeStVert = (GVertex *)(theEditor().EditObjList[0]);
// 	theDoc().m_BridgeStConRad = theDoc().m_pBridgeStMesh->BBox.GetDiagonal() * theEditor().EditMeshDlg.m_EditRgnDist / 1000.0;
// 	GMesh *pMesh = new GMesh("", GTransf(), FACE_TRI, theDoc().m_pBridgeStMesh->Name);
// 	pMesh->Name = create_mesh_name("Contact Curve");
// 
// 	// 측지 극좌표를 이용하여 편집 경계선을 구하여 장면에 추가한다.
// 	std::vector<double> Distances;
// 	std::vector<double> Angles;
// 	get_geodesic_polar_coords(theDoc().m_pBridgeStMesh, theDoc().m_pBridgeStVert, Distances, Angles, theDoc().m_BridgeStConRad * 1.5);
// 	theEditor().FindEditBndry(theDoc().m_pBridgeStMesh, Distances, Angles, theDoc().m_BridgeStConRad);
// 	for (unsigned i = 0; i < theEditor().EditBndryPts.size(); ++i)
// 	{
// 		GPoint3 p = theDoc().m_pBridgeStMesh->MC * theEditor().EditBndryPts[i];
// 		pMesh->AddVertex(GVertex(p));
// 	}
// 	theEditor().EditBndryPts.clear();
// 	theScene().AddMesh(pMesh);
// 
// 	GMaterial mtl = ::get_material(1.0, 0.0, 0.0);
// 	pMesh->AddMtl(mtl);
// 	pMesh->SetMtl(mtl.Name);
// 
// 	UpdateWorkSpaceDlg();
// 	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnSetBridgeEndCrv()
{
	if (theDoc().m_pBridgeMeshDlg == NULL)
	{
		theDoc().m_pBridgeMeshDlg = new GBridgeMeshDlg();
		theDoc().m_pBridgeMeshDlg->Create(IDD_BRIDGE_MESH_DLG);
	}

	GMesh *pEndMesh = theEditor().GetEditMesh();
	theDoc().m_pBridgeMeshDlg->m_pEndMesh = pEndMesh;
	theDoc().m_pBridgeMeshDlg->m_pEndVert = (GVertex *)(theEditor().EditObjList[0]);
	theDoc().m_pBridgeMeshDlg->m_pEndUnfoldMesh = theEditor().pUnfoldMesh->CopyObject();
	theDoc().m_pBridgeMeshDlg->m_EndUnfoldFaceMap = theEditor().UnfoldFaceMap;
	theDoc().m_pBridgeMeshDlg->m_EndCrvSize = pEndMesh->BBox.GetDiagonal() * theEditor().EditMeshDlg.m_EditRgnDist / 1000.0;

	GNurbsCrv *pCrv = ::get_gnurbs_approx_circle(theDoc().m_pBridgeMeshDlg->m_EndCrvSize, 0, true);
	GMaterial mat_blue = get_material(0.0, 0.0, 1.0);
	pCrv->KnotRefine();
	pCrv->KnotRefine();
	pCrv->KnotRefine();
	pCrv->KnotRefine();	
	pCrv->Renderer.RenderType = RENDER_POLYGON;
	pCrv->Renderer.SetSampleNum(20, 200);
	pCrv->Renderer.CtrlPtSize = 0.1;
	pCrv->Renderer.bRenderCrvAsLine = true;
	pCrv->AddMtl(mat_blue);
	pCrv->SetMtl(mat_blue.Name);
	pCrv->CreateMesh();
	theDoc().m_pBridgeMeshDlg->m_pEndContCrv = pCrv;

	// 	theDoc().m_pBridgeEdMesh = theEditor().GetEditMesh();
	// 	theDoc().m_pBridgeEdVert = (GVertex *)(theEditor().EditObjList[0]);
	// 	theDoc().m_BridgeEdConRad = theDoc().m_pBridgeEdMesh->BBox.GetDiagonal() * theEditor().EditMeshDlg.m_EditRgnDist / 1000.0;
	// 
	// 	GMesh *pMesh = new GMesh("", GTransf(), FACE_TRI, theDoc().m_pBridgeEdMesh->Name);
	// 	pMesh->Name = create_mesh_name("Contact Curve");
	// 
	// 	// 측지 극좌표를 이용하여 편집 경계선을 구하여 장면에 추가한다.
	// 	std::vector<double> Distances;
	// 	std::vector<double> Angles;
	// 	get_geodesic_polar_coords(theDoc().m_pBridgeEdMesh, theDoc().m_pBridgeEdVert, Distances, Angles, theDoc().m_BridgeEdConRad * 1.5);
	// 	theEditor().FindEditBndry(theDoc().m_pBridgeEdMesh, Distances, Angles, theDoc().m_BridgeEdConRad);
	// 	for (unsigned i = 0; i < theEditor().EditBndryPts.size(); ++i)
	// 	{
	// 		GPoint3 p = theDoc().m_pBridgeEdMesh->MC * theEditor().EditBndryPts[i];
	// 		pMesh->AddVertex(GVertex(p));
	// 	}
	// 	theEditor().EditBndryPts.clear();
	// 	theScene().AddMesh(pMesh);
	// 
	// 	GMaterial mtl = ::get_material(1.0, 0.0, 0.0);
	// 	pMesh->AddMtl(mtl);
	// 	pMesh->SetMtl(mtl.Name);
	// 
	// 	UpdateWorkSpaceDlg();
	// 	theDoc().UpdateAllViews(NULL);
}

void CMainFrame::OnCreateBridgeMesh()
{
	if (theDoc().m_pBridgeMeshDlg == NULL || theDoc().m_pBridgeMeshDlg->m_pStartMesh == NULL || theDoc().m_pBridgeMeshDlg->m_pEndMesh == NULL)
	{
		AfxMessageBox("시작 메쉬, 끝 메쉬의 시작 곡선을 지정하세요!");
		return;
	}

	GMesh *pStartMesh = theDoc().m_pBridgeMeshDlg->m_pStartMesh;
	GVertex *pStartVert = theDoc().m_pBridgeMeshDlg->m_pStartVert;
	GNurbsCrv *pStartContCrv = theDoc().m_pBridgeMeshDlg->m_pStartContCrv;
	double Alpha = theDoc().m_pBridgeMeshDlg->m_StartAuxCrvSclParam;

	GMesh *pEndMesh = theDoc().m_pBridgeMeshDlg->m_pEndMesh;
	GVertex *pEndVert = theDoc().m_pBridgeMeshDlg->m_pEndVert;
	GNurbsCrv *pEndContCrv = theDoc().m_pBridgeMeshDlg->m_pEndContCrv;
	double Beta = theDoc().m_pBridgeMeshDlg->m_EndAuxCrvSclParam;
	double Mu = theDoc().m_pBridgeMeshDlg->m_BlendParam;

	GMesh *pBridgeMesh = ::create_mesh_bridge(
		pStartMesh, pStartVert, pStartContCrv, Alpha,
		pEndMesh, pEndVert, pEndContCrv, Beta,
		Mu, 100, 30, false);
	pBridgeMesh->Name = ::create_mesh_name("Bridge Mesh");
	theDoc().m_pBridgeMeshDlg->m_pBridgeMesh = pBridgeMesh;
	theScene().AddMesh(pBridgeMesh);
	theEditor().Reset();

	theDoc().m_pBridgeMeshDlg->ShowWindow(SW_SHOW);
	theDoc().m_pBridgeMeshDlg->SetFocus();
	theDoc().m_pBridgeMeshDlg->UpdateGLView();


	theMainFrm().UpdateWorkSpaceDlg();
	theDoc().UpdateAllViews(NULL);
}



