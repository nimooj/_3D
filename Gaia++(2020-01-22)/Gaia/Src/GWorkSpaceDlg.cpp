#include "stdafx.h"
#include "..\Gaia.h"
#include "..\MainFrm.h"
#include "GWorkSpaceDlg.h"

// GWorkSpaceDlg 대화 상자입니다.
IMPLEMENT_DYNAMIC(GWorkSpaceDlg, CDialog)

GWorkSpaceDlg::GWorkSpaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GWorkSpaceDlg::IDD, pParent)
{
}

GWorkSpaceDlg::~GWorkSpaceDlg()
{
}

void GWorkSpaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WORKSPACE_TAB, m_cWorkSpaceTab);
}

BEGIN_MESSAGE_MAP(GWorkSpaceDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_WORKSPACE_TAB, &GWorkSpaceDlg::OnTcnSelchangeWorkspaceTab)
	ON_WM_SIZE()
	ON_MESSAGE(WM_EXIT_CREATOR, OnExitCreator)
	ON_MESSAGE(WM_UPDATE_SELECT, OnUpdateSelect)
	ON_WM_CTLCOLOR()
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()

// GWorkSpaceDlg 메시지 처리기입니다.
BOOL GWorkSpaceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	unsigned char *BC = theScene().UIBkgColor;
	unsigned char *TC = theScene().UItxtColor;
	m_cWorkSpaceTab.SetBkgndColor(RGB(BC[0], BC[1], BC[2]));
	m_cWorkSpaceTab.EnableDraw(BTC_ALL); 
	
	GTabCtrl::EnableCustomLook();
	m_cWorkSpaceTab.Invalidate();
	Invalidate(false);

	return TRUE;  
}

/*!
*	\brief 새로운 탭 대화상자를 추가한다.
*
*	\param pDlg 추가할 탭 대화상자
*	\param TabName 대화상자의 탭 이름
*	\param bSelect 추가후 선택 여부
*
*/
void GWorkSpaceDlg::InsertTabDlg(CDialog *pDlg, CString TabName, bool bSelect)
{
	// 추가할 대화상자를 초기화 하고,
	pDlg->OnInitDialog();

	// 추가할 대화상자가 이미 있다면, 초기화만 하고 리턴한다.
	int NumDlg = (int)m_pTabDlgList.size();
	for (int i = 0; i < NumDlg; ++i)
	{
		if (m_pTabDlgList[i] == pDlg)
			return;
	}

	// 새로운 대화상자라면 추가한다.
	int idx = m_cWorkSpaceTab.GetItemCount();
	m_cWorkSpaceTab.InsertItem(idx, TabName);
	m_pTabDlgList.push_back(pDlg);

	// 선택 여부에 따라서 추가된 대화 상자를 선택하거나 m_SceneTreeDlg를 선택한다.
	if (bSelect)
		SetCurrTab(idx);
	else
		SetCurrTab(0);

	// 대화상자의 레이아웃을 재정렬 한다.
 	RecalcLayOut();
}

void GWorkSpaceDlg::DeleteTabDlg()
{
	// WorkSpaceTree 대화상자를 제외한 모든 대화상자를 제거한다.
	int NumDlg = (int)m_pTabDlgList.size();
	for (int i = NumDlg - 1; i > 0; --i)
	{
		m_pTabDlgList[i]->SendMessage(WM_COMMAND, IDOK);
		m_cWorkSpaceTab.DeleteItem(i);
		m_pTabDlgList.pop_back();
	}
}

void GWorkSpaceDlg::SetCurrTab(int iTabIdx)
{
	m_cWorkSpaceTab.SetCurFocus(iTabIdx);

	// 대화 상자를 모두 숨긴 후,
	int NumDlg = (int)m_pTabDlgList.size();
	for (int i = 0; i < NumDlg; i++)
		m_pTabDlgList[i]->ShowWindow(SW_HIDE);

	// 선택된 대화 상자만 보인다
	m_pTabDlgList[iTabIdx]->ShowWindow(SW_SHOW);
}

void GWorkSpaceDlg::RecalcLayOut()
{
	CRect rcClient;
	GetClientRect(rcClient);
	rcClient.DeflateRect(5,5);
	m_cWorkSpaceTab.MoveWindow(rcClient);

	rcClient.DeflateRect(2,2);
	rcClient.top += 20;

	// 추가 대화상자를 재배치 한다.
	int Num = (int)m_pTabDlgList.size();
	for (int i = 0; i < Num; i++)
		m_pTabDlgList[i]->MoveWindow(&rcClient);
}

void GWorkSpaceDlg::UpdateWorkSpace()
{
	// 먼저 SceneTreeDlg의 정보를 갱신하고,
	((GSceneTreeDlg *)m_pTabDlgList[0])->UpdateSceneTree();

	// SceneTreeDlg를 제외한 모든 대화상자를 지운다.
	DeleteTabDlg();

	// WorkSpaceTreeDlg 대화 상자를 활성화 한다.
	SetCurrTab(0);

	theDoc().UpdateAllViews(NULL);
}

void GWorkSpaceDlg::UpdateSelect()
{
	// SceneTreeDlg를 제외한 모든 대화상자를 지운다.
	DeleteTabDlg();

	// 선택된 참조 메쉬가 있다면,
	if (!theEditor().EditObjList.empty())
	{
		// 메쉬 편집 대화상자를 추가하고,
		InsertTabDlg(&theEditor().EditMeshDlg, _T("Mesh Attrib."), false);	

		// 선택된 객체의 참조 메쉬를 구하고,
		GMesh *pMesh = theEditor().EditObjList[0]->GetRefMesh();

		// 선택된 객체의 클래스명 구하여,
		std::string ClsName = pMesh->GetClassName();

		// 선택된 객체가 곡선이라면, 곡선 편집 대화상자를 추가하고,
		if (ClsName == "GNurbsCrv")
		{
			InsertTabDlg(&theEditor().EditCrvDlg, _T("Curve Attrib."), false);
		}

		// 선택된 객체가 곡면이라면, 곡면 편집 대화상자를 추가하고,
		if (ClsName == "GNurbsSrf" || ClsName == "GCoonsSrf" || ClsName == "GUcbsSrf")
		{
			InsertTabDlg(&theEditor().EditSrfDlg, _T("Surface Attrib."), false);
		}

		// 선택된 객체가 볼륨이라면, 볼륨 편집 대화상자를 추가하고,
		if (ClsName == "GUcbsVol")
		{
			InsertTabDlg(&theEditor().EditVolDlg, _T("Volume Attrib."), false);
		}

		// 선택된 객체가 스윕이라면, 스윕 편집 대화상자를 추가하고,
		if (ClsName == "GSweepSrf")
		{
			InsertTabDlg(&theEditor().EditSwpDlg, _T("Sweep Attrib."), false);
		}

		// 선택된 객체가 블렌딩곡면이라면, 블렌딩 곡면 편집 대화상자를 추가하고,
		if (pMesh->SemanticType == MESH_CTRL || ClsName == "GBlendMesh")
		{
			InsertTabDlg(&theEditor().EditBlendSrfDlg, _T("BlendSrf Attrib."), false);
		}
	}
	else	// 선택된 참조 메쉬가 없다면 m_SceneTreeDlg를 활성화 한다.
		SetCurrTab(0);

	theDoc().UpdateAllViews(NULL);
}

LRESULT GWorkSpaceDlg::OnUpdateSelect(WPARAM wParam, LPARAM lParam)
{
	UpdateSelect();
	return 0;
}

LRESULT GWorkSpaceDlg::OnExitCreator(WPARAM wParam, LPARAM lParam)
{
	UpdateWorkSpace();
	theDoc().UpdateAllViews(NULL);
	return 0;
}

BOOL GWorkSpaceDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN )
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void GWorkSpaceDlg::OnTcnSelchangeWorkspaceTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iCurSel = m_cWorkSpaceTab.GetCurSel();  
	SetCurrTab(iCurSel);
	*pResult = 0;
}

void GWorkSpaceDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	static bool bInit = false;
	if (bInit)
		RecalcLayOut();
	else
		bInit = true;
}

HBRUSH GWorkSpaceDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO: Change any attributes of the DC here
	unsigned char *BC = theScene().UIBkgColor;
	unsigned char *TC = theScene().UItxtColor;
	pDC->SetBkColor(RGB(BC[0], BC[1], BC[2]));
	pDC->SetTextColor(RGB(TC[0], TC[1], TC[2]));
	pDC->SetBkMode(OPAQUE);
	static CBrush BkgBrush(RGB(BC[0], BC[1], BC[2]));
	hbr = (HBRUSH)BkgBrush;

	// TODO: Return a different brush if the default is not desired
	return hbr;
}
