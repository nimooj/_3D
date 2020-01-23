// PEditMeshDlg.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "PEditMeshDlg.h"

// PEditMeshDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(PEditMeshDlg, CDialog)

PEditMeshDlg::PEditMeshDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PEditMeshDlg::IDD, pParent)
	, pDoc(NULL)
	, m_MeshName(_T(""))
	, m_NumVert(0)
	, m_NumFace(0)
	, m_MeshClsName(_T(""))
	, m_EditMeshTransX(0)
	, m_EditMeshTransY(0)
	, m_EditMeshTransZ(0)
	, m_EditMeshRotX(0)
	, m_EditMeshRotY(0)
	, m_EditMeshRotZ(0)
	, m_EditMeshSclX(0)
	, m_EditMeshSclY(0)
	, m_EditMeshSclZ(0)
	, m_BrushRadius(20)
	, m_EditRgnDist(0)
	, m_Transparency(0)
	, m_ShowEditBndryType(0)
	, m_CollisionMode(0)
{
}

PEditMeshDlg::~PEditMeshDlg()
{
}

void PEditMeshDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDemoPage)
	DDX_Control(pDX, IDC_MTRL_LIST, m_cMtlList);
	DDX_Text(pDX, IDC_MESH_NAME, m_MeshName);
	DDX_Control(pDX, IDC_EDIT_TOOL_LIST, m_cEditToolList);
	DDX_Text(pDX, IDC_MESH_NUM_VERT, m_NumVert);
	DDX_Text(pDX, IDC_MESH_NUM_FACE, m_NumFace);
	DDX_Text(pDX, IDC_MESH_TRANS_X, m_EditMeshTransX);
	DDX_Text(pDX, IDC_MESH_TRANS_Y, m_EditMeshTransY);
	DDX_Text(pDX, IDC_MESH_TRANS_Z, m_EditMeshTransZ);
	DDX_Text(pDX, IDC_MESH_ROT_X, m_EditMeshRotX);
	DDX_Text(pDX, IDC_MESH_ROT_Y, m_EditMeshRotY);
	DDX_Text(pDX, IDC_MESH_ROT_Z, m_EditMeshRotZ);
	DDX_Text(pDX, IDC_MESH_SCL_X, m_EditMeshSclX);
	DDX_Text(pDX, IDC_MESH_SCL_Y, m_EditMeshSclY);
	DDX_Text(pDX, IDC_MESH_SCL_Z, m_EditMeshSclZ);
	DDX_Text(pDX, IDC_MESH_CLASS_NAME, m_MeshClsName);
	DDX_Control(pDX, IDC_BRUSH_TOOL_RAD, m_cBrushRadius);
	DDX_Slider(pDX, IDC_BRUSH_TOOL_RAD, m_BrushRadius);
	DDX_Control(pDX, IDC_EDIT_RGN_DIST, m_cEditRgnDist);
	DDX_Slider(pDX, IDC_EDIT_RGN_DIST, m_EditRgnDist);
	DDV_MinMaxInt(pDX, m_EditRgnDist, 0, 1000);
	DDX_Control(pDX, IDC_TRANSPARENCY, m_cTransparency);
	DDX_Slider(pDX, IDC_TRANSPARENCY, m_Transparency);
	DDX_Radio(pDX, IDC_SHOW_EDIT_BNDRY_APPROX, m_ShowEditBndryType);
	DDX_Radio(pDX, IDC_COLLISION_CHECK, m_CollisionMode);
	//}}AFX_DATA_MAP

	COLORREF ThrowAwayColor;
	DDX_Control(pDX, IDC_AMBIENT_COLOR, m_cAmbiColor);
	DDX_Control(pDX, IDC_DIFFUSE_COLOR, m_cDiffColor);
	DDX_Control(pDX, IDC_SPECULAR_COLOR, m_cSpecColor);
	DDX_ColorButton(pDX, IDC_SPECULAR_COLOR, ThrowAwayColor);
	DDX_ColorButton(pDX, IDC_DIFFUSE_COLOR, ThrowAwayColor);
	DDX_ColorButton(pDX, IDC_AMBIENT_COLOR, ThrowAwayColor);
}


BEGIN_MESSAGE_MAP(PEditMeshDlg, CDialog)
	ON_EN_KILLFOCUS(IDC_MESH_NAME, &PEditMeshDlg::OnEnKillfocusMeshName)
	ON_CBN_CLOSEUP(IDC_MTRL_LIST, &PEditMeshDlg::OnCbnCloseupMtrlList)
	ON_BN_CLICKED(IDC_APPLY_EDIT_TOOL, &PEditMeshDlg::OnBnClickedApplyEditTool)	
	ON_WM_HSCROLL()
	ON_EN_KILLFOCUS(IDC_MESH_SCL_X, &PEditMeshDlg::OnEnKillfocusMeshSclX)
	ON_EN_KILLFOCUS(IDC_MESH_SCL_Y, &PEditMeshDlg::OnEnKillfocusMeshSclY)
	ON_EN_KILLFOCUS(IDC_MESH_SCL_Z, &PEditMeshDlg::OnEnKillfocusMeshSclZ)
	ON_EN_KILLFOCUS(IDC_MESH_ROT_X, &PEditMeshDlg::OnEnKillfocusMeshRotX)
	ON_EN_KILLFOCUS(IDC_MESH_ROT_Y, &PEditMeshDlg::OnEnKillfocusMeshRotY)
	ON_EN_KILLFOCUS(IDC_MESH_ROT_Z, &PEditMeshDlg::OnEnKillfocusMeshRotZ)
	ON_EN_KILLFOCUS(IDC_MESH_TRANS_X, &PEditMeshDlg::OnEnKillfocusMeshTransX)
	ON_EN_KILLFOCUS(IDC_MESH_TRANS_Y, &PEditMeshDlg::OnEnKillfocusMeshTransY)
	ON_EN_KILLFOCUS(IDC_MESH_TRANS_Z, &PEditMeshDlg::OnEnKillfocusMeshTransZ)
	ON_BN_CLICKED(IDC_ADD_MATERIAL, &PEditMeshDlg::OnBnClickedAddMaterial)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_SHOW_EDIT_BNDRY_APPROX, &PEditMeshDlg::OnBnClickedShowEditBndryApprox)
	ON_BN_CLICKED(IDC_SHOW_EDIT_BNDRY_EXACT, &PEditMeshDlg::OnBnClickedShowEditBndryExact)
	ON_BN_CLICKED(IDC_COLLISION_CHECK, &PEditMeshDlg::OnBnClickedCollisionCheck)
	ON_BN_CLICKED(IDC_COLLISION_FREE, &PEditMeshDlg::OnBnClickedCollisionFree)
	ON_MESSAGE(CPN_SELENDOK, OnSelEndOK)
    ON_MESSAGE(CPN_SELENDCANCEL, OnSelEndCancel)
    ON_MESSAGE(CPN_SELCHANGE, OnSelChange)
    ON_MESSAGE(CPN_CLOSEUP, OnCloseUp)
    ON_MESSAGE(CPN_DROPDOWN, OnDropDown)
END_MESSAGE_MAP()


// PEditMeshDlg 메시지 처리기입니다.
BOOL PEditMeshDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (pEditor == NULL || pDoc == NULL)
		return TRUE;

	// 대화상자가 처음 보여지는 경우라면,
	static bool bInit = false;
	if (bInit == false)
	{
		// 편집툴 리스트 박스 채우기
		m_cEditToolList.ResetContent();
		m_cEditToolList.AddString(_T("Smoothing"));
		m_cEditToolList.AddString(_T("Refine"));
		m_cEditToolList.AddString(_T("Sewing"));
		m_cEditToolList.AddString(_T("Flip Normals"));
		m_cEditToolList.AddString(_T("Normalize"));
		m_cEditToolList.AddString(_T("Double Size"));
		m_cEditToolList.SetCurSel(0);

		// 재질 리스트 박스 초기화 하고,
		m_cMtlList.ResetContent();

		// 투명도 조절 컨트롤을 설정하고,
 		m_cTransparency.SetRange(0, 100);
 		m_cTransparency.SetPos(0);
 		
		// 브러쉬 컨트롤 설정하고,
		m_cBrushRadius.SetRange(10, 1000);
		m_cBrushRadius.SetPos(0);

		// 편집 영역 컨트롤 설성한다.
		m_cEditRgnDist.SetRange(0, 1000);
		m_cEditRgnDist.SetPos(0);

		// 편집 영역의 경계를 정의하는 타입을 근사 경계 곡선으로 설정한다.
		m_ShowEditBndryType = 0;

		// 충돌 처리 모드를 설정한다.
		m_CollisionMode = 0;

		// 컨트롤의 초기화가 되었음을 설정한다.
		bInit = true;

		// 재질 색상 버튼을 초기화 한다.
		m_cAmbiColor.Color = RGB(127, 127, 127);
		m_cAmbiColor.DefaultColor = ::GetSysColor(COLOR_APPWORKSPACE);	
		m_cAmbiColor.TrackSelection = TRUE;

		m_cDiffColor.Color = RGB(127, 127, 127);
		m_cDiffColor.DefaultColor = ::GetSysColor(COLOR_APPWORKSPACE);	
		m_cDiffColor.TrackSelection = TRUE;

		m_cSpecColor.Color = RGB(127, 127, 127);
		m_cSpecColor.DefaultColor = ::GetSysColor(COLOR_APPWORKSPACE);	
		m_cSpecColor.TrackSelection = TRUE;	
	}

	// 선택된 객체를 구하여,
	GMesh *pMesh = pEditor->GetEditMesh();
	assert(pMesh != NULL);
	GRenderer Renderer = pMesh->Renderer;

	// 메쉬 정보와 변환정보를 설정하고,
	m_MeshName = pMesh->Name.c_str();
	m_NumVert = pMesh->GetNumVert();
	m_NumFace = pMesh->GetNumFace();
	m_MeshClsName = CString(pEditor->GetClsNameEditObj().c_str());		
	m_EditMeshTransX = pMesh->MC.t[0];
	m_EditMeshTransY = pMesh->MC.t[1];
	m_EditMeshTransZ = pMesh->MC.t[2];
	pMesh->MC.q.GetEulerAngle(m_EditMeshRotX, m_EditMeshRotY, m_EditMeshRotZ);
	m_EditMeshSclX = (int)(pMesh->S[0] * 10000.0 + 0.5) / 10000.0;
	m_EditMeshSclY = (int)(pMesh->S[1] * 10000.0 + 0.5) / 10000.0;
	m_EditMeshSclZ = (int)(pMesh->S[2] * 10000.0 + 0.5) / 10000.0;

	// 컨트롤의 값을 설정한다.
	m_cEditToolList.SetCurSel(0);
	m_BrushRadius = 0;
	m_EditRgnDist = 0;
	m_ShowEditBndryType = 1;
	m_CollisionMode = 0;
	
	// 재질 리스트를 초기화 하고 선택된 재질을 설정한다.
	m_cMtlList.ResetContent();
	int NumMtl = pMesh->GetNumMtl();
	for (int i = 0; i < NumMtl; ++i)
		m_cMtlList.AddString(pMesh->MtlList[i].Name.c_str());

	int MtlIdx = (pMesh->Faces.empty()) ? (Renderer.CrvMtlIdx) : (pMesh->Faces[0].MtlIdx);
	GMaterial Mtl = pMesh->MtlList[MtlIdx];
	m_cMtlList.SetCurSel(MtlIdx);
	m_Transparency = (int)((1.0 - Mtl.Diffuse[3]) * 100);
	m_cTransparency.SetPos(m_Transparency);
	UpdateData(false);

	// 선택된 재질을 색상 버튼에 반영한다.
	m_cAmbiColor.Color = RGB((int)(Mtl.Ambient[0] * 255), (int)(Mtl.Ambient[1] * 255), (int)(Mtl.Ambient[2] * 255));
	m_cDiffColor.Color = RGB((int)(Mtl.Diffuse[0] * 255), (int)(Mtl.Diffuse[1] * 255), (int)(Mtl.Diffuse[2] * 255));
	m_cSpecColor.Color = RGB((int)(Mtl.Specular[0] * 255), (int)(Mtl.Specular[1] * 255), (int)(Mtl.Specular[2] * 255));

	return TRUE;
}

BOOL PEditMeshDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void PEditMeshDlg::OnEnKillfocusMeshName()
{
 	UpdateData(true);
	if (pEditor->EditObjList.empty())
		return;

	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();
	pSltMesh->Name = (CStringA)(m_MeshName);
}

void PEditMeshDlg::OnCbnCloseupMtrlList()
{
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pEditor == NULL || pMesh == NULL)
	{
		AfxMessageBox("메쉬를 선택하세요!");
		return;
	}

	int idx = m_cMtlList.GetCurSel();
	if (idx == -1)
		return;

	CString Name;
	m_cMtlList.GetLBText(idx, Name);
	std::string MtlName = (CStringA)(Name);
	int MtlIdx = pMesh->GetMtlIdx(MtlName);

	// 곡선의 재질 인덱스를 갱신하고,
	pMesh->Renderer.CrvMtlIdx = MtlIdx;
	
	// 선택된 재질을 색상 버튼에 반영한다.
	GMaterial Mtl = pMesh->MtlList[MtlIdx];
	m_Transparency = (int)(100 * (1.0f - Mtl.Diffuse[3]));
	m_cTransparency.SetPos(m_Transparency);
	m_cAmbiColor.Color = RGB((int)(Mtl.Ambient[0] * 255), (int)(Mtl.Ambient[1] * 255), (int)(Mtl.Ambient[2] * 255));
	m_cDiffColor.Color = RGB((int)(Mtl.Diffuse[0] * 255), (int)(Mtl.Diffuse[1] * 255), (int)(Mtl.Diffuse[2] * 255));
	m_cSpecColor.Color = RGB((int)(Mtl.Specular[0] * 255), (int)(Mtl.Specular[1] * 255), (int)(Mtl.Specular[2] * 255));

	// 부분 삼각형 집합이 선택된 경우라면,
	if (pEditor->GetClsNameEditObj() == "GFace")
	{
		int NumFace = pEditor->GetNumEditObj();
		for (int i = 0; i < NumFace; ++i)
		{
			GFace *f = (GFace *)(pEditor->EditObjList[i]);
			f->MtlIdx = MtlIdx;
		}
	}
	else // 전체 메쉬가 선택된 경우라면,
	{
		// 전체 삼각형에 대하여 재질을 선택한다.
		int NumFace = pMesh->GetNumFace();
		for (int i = 0; i < NumFace; ++i)
			pMesh->Faces[i].MtlIdx = MtlIdx;
	}

	pMesh->ModifiedFlag = MODIFIED_MTL;
	pMesh->UpdateMesh();
	pDoc->UpdateAllViews(NULL);
}

void PEditMeshDlg::OnBnClickedApplyEditTool()
{
	if (pEditor == NULL || pEditor->EditObjList.empty())
		return;

	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();
	
	int EditMenu = m_cEditToolList.GetCurSel();
	switch (EditMenu)
	{
	case 0: // smoothing
		edit_mesh_by_smoothing(pSltMesh);
		break;

	case 1:	// Refine
		edit_mesh_by_refine(pSltMesh);
		break;

	case 2:	// Sewing
		edit_mesh_by_stitching(pSltMesh);
		break;

	case 3:	// Flip Normal
		pSltMesh->ReverseNormal();
		break;

	case 4:	// Normalize
		pSltMesh->Normalize();
		break;

	case 5:	// Double size
		pSltMesh->Edit(GTransf(), GVector3(), GQuater(), GVector3(2.0, 2.0, 2.0));
		pSltMesh->UpdateMesh();
		break;
	}
	pDoc->UpdateAllViews(NULL);
}

void PEditMeshDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// 컨트롤의 값을 변수에 대입하고,
	UpdateData(true);
	switch (pScrollBar->GetDlgCtrlID())
	{
	case IDC_TRANSPARENCY:
		{
			GMesh *pMesh = pEditor->GetEditMesh();
			float alpha = 1.0f - m_Transparency / 100.0f;
			int MtlIdx = m_cMtlList.GetCurSel();
			pMesh->MtlList[MtlIdx].SetAlpha(alpha);
		}
		break;

	case IDC_BRUSH_TOOL_RAD:
		break;

	case IDC_EDIT_RGN_DIST:
		pEditor->FindEditWgt();
		break;
	}
	pDoc->UpdateAllViews(NULL);	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void PEditMeshDlg::OnEnKillfocusMeshSclX()
{
	UpdateData(true);
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL)
		return;

	double ratio = m_EditMeshSclX / pMesh->S[0];

	int NumVert = pMesh->GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		pMesh->Verts[i].P[0] *= ratio;
	}
	
	pMesh->BBox.MinPt[0] *= ratio;
	pMesh->BBox.MaxPt[0] *= ratio;
	pMesh->S[0] *= ratio;

	pMesh->ModifiedFlag = MODIFIED_VERT_POS_ALL;
	pMesh->UpdateMesh();
	
	pDoc->UpdateAllViews(NULL);
}

void PEditMeshDlg::OnEnKillfocusMeshSclY()
{
	UpdateData(true);
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL)
		return;

	double ratio = m_EditMeshSclY / pMesh->S[1];
	int NumVert = pMesh->GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		pMesh->Verts[i].P[1] *= ratio;
	}
	
	pMesh->BBox.MinPt[1] *= ratio;
	pMesh->BBox.MaxPt[1] *= ratio;
	pMesh->S[1] *= ratio;

	pMesh->ModifiedFlag = MODIFIED_VERT_POS_ALL;
	pMesh->UpdateMesh();

	pDoc->UpdateAllViews(NULL);
}

void PEditMeshDlg::OnEnKillfocusMeshSclZ()
{
	UpdateData(true);
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL)
		return;

	double ratio = m_EditMeshSclZ / pMesh->S[2];
	int NumVert = pMesh->GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		pMesh->Verts[i].P[2] *= ratio;
	}
	
	pMesh->BBox.MinPt[2] *= ratio;
	pMesh->BBox.MaxPt[2] *= ratio;
	pMesh->S[2] *= ratio;

	pMesh->ModifiedFlag = MODIFIED_VERT_POS_ALL;
	pMesh->UpdateMesh();

	pDoc->UpdateAllViews(NULL);
}

void PEditMeshDlg::OnEnKillfocusMeshRotX()
{
	UpdateData(true);
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL)
		return;

	GQuater q;
	q.SetFromEulerAngle(m_EditMeshRotX, m_EditMeshRotY, m_EditMeshRotZ);
	pMesh->MC.q = q;

	pDoc->UpdateAllViews(NULL);
}

void PEditMeshDlg::OnEnKillfocusMeshRotY()
{
	UpdateData(true);
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL)
		return;

	GQuater q;
	q.SetFromEulerAngle(m_EditMeshRotX, m_EditMeshRotY, m_EditMeshRotZ);
	pMesh->MC.q = q;

	pDoc->UpdateAllViews(NULL);
}

void PEditMeshDlg::OnEnKillfocusMeshRotZ()
{
	UpdateData(true);
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL)
		return;

	GQuater q;
	q.SetFromEulerAngle(m_EditMeshRotX, m_EditMeshRotY, m_EditMeshRotZ);
	pMesh->MC.q = q;

	pDoc->UpdateAllViews(NULL);
}

void PEditMeshDlg::OnEnKillfocusMeshTransX()
{
	UpdateData(true);
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL)
		return;

	pMesh->MC.t[0] = m_EditMeshTransX;
	
	pDoc->UpdateAllViews(NULL);
}

void PEditMeshDlg::OnEnKillfocusMeshTransY()
{
	UpdateData(true);
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL)
		return;

	pMesh->MC.t[1] = m_EditMeshTransY;

	pDoc->UpdateAllViews(NULL);
}

void PEditMeshDlg::OnEnKillfocusMeshTransZ()
{
	UpdateData(true);
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL)
		return;

	pMesh->MC.t[2] = m_EditMeshTransZ;

	pDoc->UpdateAllViews(NULL);
}

void PEditMeshDlg::OnBnClickedAddMaterial()
{
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("메쉬를 선택하세요.");
		return;
	}

	// 재질 추가 대화상자를 출력한다.
	PAddMtlDlg dlg;
	dlg.pDoc = pDoc;
	dlg.pEditor = pEditor;

	// 현재 적용된 재질의 인덱스를 구하여,
	int idx = m_cMtlList.GetCurSel();

	// 현재 적용된 재질이 없다면,
	if (idx == -1)
	{
		// 디폴트 재질을 설정한다.
		dlg.m_AmbientR = 50;
		dlg.m_AmbientG = 50;
		dlg.m_AmbientB = 50;
		dlg.m_DiffuseR = 178;
		dlg.m_DiffuseG = 178;
		dlg.m_DiffuseB = 178;
		dlg.m_SpecularR = 178;
		dlg.m_SpecularG = 178;
		dlg.m_SpecularB = 178;
	}
	else	// 현재 적용된 재질이 있다면,
	{
		// 현재 적용된 재질을 구하여,
		CString Name;
		m_cMtlList.GetLBText(idx, Name);
		std::string MtlName = (CStringA)(Name);
		GMaterial *pMtl = pMesh->FindMtl(MtlName);

		// 동일한 재질을 설정한다.
		dlg.m_AmbientR = (int)(pMtl->Ambient[0] * 255);
		dlg.m_AmbientG = (int)(pMtl->Ambient[1] * 255);
		dlg.m_AmbientB = (int)(pMtl->Ambient[2] * 255);
		dlg.m_DiffuseR = (int)(pMtl->Diffuse[0] * 255);
		dlg.m_DiffuseG = (int)(pMtl->Diffuse[1] * 255);
		dlg.m_DiffuseB = (int)(pMtl->Diffuse[2] * 255);
		dlg.m_SpecularR = (int)(pMtl->Specular[0] * 255);
		dlg.m_SpecularG = (int)(pMtl->Specular[1] * 255);
		dlg.m_SpecularB = (int)(pMtl->Specular[2] * 255);
	}

	if (dlg.DoModal() != IDOK)
		return;
}

HBRUSH PEditMeshDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	unsigned char *BC = pEditor->pScene->UIBkgColor;
	unsigned char *TC = pEditor->pScene->UItxtColor;
	pDC->SetBkColor(RGB(BC[0], BC[1], BC[2]));
	pDC->SetTextColor(RGB(TC[0], TC[1], TC[2]));
	pDC->SetBkMode(OPAQUE);
	static CBrush BkgBrush(RGB(BC[0], BC[1], BC[2]));
	hbr = (HBRUSH)BkgBrush;

	return hbr;
}


void PEditMeshDlg::OnBnClickedShowEditBndryApprox()
{
	m_ShowEditBndryType = 0;
	pEditor->FindEditWgt();
	pDoc->UpdateAllViews(NULL);
}

void PEditMeshDlg::OnBnClickedShowEditBndryExact()
{
	m_ShowEditBndryType = 1;
	pEditor->FindEditWgt();
	pDoc->UpdateAllViews(NULL);
}

void PEditMeshDlg::OnBnClickedCollisionCheck()
{
	m_CollisionMode = 0;
	pDoc->UpdateAllViews(NULL);
}

void PEditMeshDlg::OnBnClickedCollisionFree()
{
	m_CollisionMode = 1;
	pDoc->UpdateAllViews(NULL);
}


LONG PEditMeshDlg::OnSelEndOK(UINT lParam, LONG wParam)
{
	return TRUE;
}

LONG PEditMeshDlg::OnSelEndCancel(UINT /*lParam*/, LONG /*wParam*/)
{
    return TRUE;
}

LONG PEditMeshDlg::OnSelChange(UINT /*lParam*/, LONG /*wParam*/)
{
    return TRUE;
}

LONG PEditMeshDlg::OnCloseUp(UINT lParam, LONG wParam)
{
   GMesh *pMesh = pEditor->GetEditMesh();

	int MtlIdx = m_cMtlList.GetCurSel();
	GMaterial &Mtl = pMesh->MtlList[MtlIdx];

	COLORREF Color = (COLORREF)lParam;
	float R = GetRValue(Color) / 255.0;
	float G = GetGValue(Color) / 255.0;
	float B = GetBValue(Color) / 255.0;
	
	switch (wParam)
	{
	case IDC_AMBIENT_COLOR:
		Mtl.SetAmbient(R, G, B);
		break;

	case IDC_DIFFUSE_COLOR:
		Mtl.SetDiffuse(R, G, B);
		break;

	case IDC_SPECULAR_COLOR:
		Mtl.SetSpecular(R, G, B);
		break;
	}
	
	pDoc->UpdateAllViews(NULL);
    return TRUE;
}

LONG PEditMeshDlg::OnDropDown(UINT /*lParam*/, LONG /*wParam*/)
{
    return TRUE;
}
