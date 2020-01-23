// Src\GSceneTreeDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../Gaia.h"
#include "../MainFrm.h"
#include "GSceneTreeDlg.h"


// GSceneTreeDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(GSceneTreeDlg, CDialog)

GSceneTreeDlg::GSceneTreeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GSceneTreeDlg::IDD, pParent)
{
}

GSceneTreeDlg::~GSceneTreeDlg()
{
}

void GSceneTreeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MESH_TREE, m_SceneTree);
}


BEGIN_MESSAGE_MAP(GSceneTreeDlg, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_MESH_TREE, &GSceneTreeDlg::OnNMClickMeshTree)
	ON_WM_CONTEXTMENU()	
	ON_WM_CTLCOLOR()
	ON_NOTIFY(TVN_SELCHANGED, IDC_MESH_TREE, &GSceneTreeDlg::OnTvnSelchangedMeshTree)
	ON_NOTIFY(NM_KILLFOCUS, IDC_MESH_TREE, &GSceneTreeDlg::OnNMKillfocusMeshTree)
END_MESSAGE_MAP()


// GSceneTreeDlg 메시지 처리기입니다.

void GSceneTreeDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	static bool bInit = false;

	if (bInit)
		RecalcLayOut();
	else
		bInit = true;
}

BOOL GSceneTreeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	unsigned char *BC = theScene().UIBkgColor;
	unsigned char *TC = theScene().UItxtColor;
	m_SceneTree.SetBkColor(RGB(BC[0], BC[1], BC[2]));
	m_SceneTree.SetTextColor(RGB(TC[0], TC[1], TC[2]));
		
	// 장면 트리를 다시 그린다.
	m_SceneTree.Invalidate(false);

	return TRUE;
}

void GSceneTreeDlg::UpdateSceneTree()
{
	// 트리의 모든 아이템을 삭제하고,
	m_SceneTree.DeleteAllItems();

	// 3개의 루트 아이템을 추가한다.
	m_RootMesh = m_SceneTree.InsertItem(_T("Mesh"), TVI_ROOT);
	m_RootLight = m_SceneTree.InsertItem(_T("Light"), TVI_ROOT);
	m_RootCamera = m_SceneTree.InsertItem(_T("Camera"), TVI_ROOT);

	int NumMesh = (int)theScene().MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
		InsertMesh(theScene().MeshList[i], m_RootMesh);

	// 장면 트리를 다시 그린다.
	m_SceneTree.Invalidate(false);
}

void GSceneTreeDlg::RecalcLayOut()
{
	CRect rcClient;
	GetClientRect(rcClient);
	rcClient.DeflateRect(5, 5);
	m_SceneTree.MoveWindow(rcClient);
}

void GSceneTreeDlg::InsertMesh(const GMesh *pMesh, HTREEITEM hItem)
{
	CString Name = CString(pMesh->Name.c_str());
	HTREEITEM hNextItem = m_SceneTree.InsertItem(Name, hItem);

	// 재귀 호출을 통해 메쉬 객체를 장면 트리에 추가한다.
	int NumSubMesh = (int)pMesh->SubMeshList.size();
	for (int i = 0; i < NumSubMesh; ++i)
		InsertMesh(pMesh->SubMeshList[i], hNextItem);
}

void GSceneTreeDlg::DeleteItem()
{
	// 장면 트리에서 선택된 아이템의 핸들을 구하여,
	HTREEITEM item = m_SceneTree.GetSelectedItem();

	// 선택된 핸들이 루트 핸들이면 리턴하고,
	if (item == m_RootMesh || item == m_RootLight || item == m_RootCamera)
		return;

	// 아니라면, 해당 아이템을 장면 트리에서 삭제한다.
	m_SceneTree.DeleteItem(item);
}

BOOL GSceneTreeDlg::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN )
		if ( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )
			return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void GSceneTreeDlg::UpdateSelectedItems()
{
	// 트리의 모든 아이템에 대하여,
	HTREEITEM hItem = m_SceneTree.GetRootItem();
	while (hItem) 
	{
		// 아이템의 이름을 구하고,
		CString Name = m_SceneTree.GetItemText(hItem);

		// 선택된 아이템인지를 조사하여,
		bool bFlag = false;
		int NumObj = theEditor().GetNumEditObj();
		for (int i = 0; i < NumObj; ++i)
		{
			GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
			if (std::string((CStringA)(Name)) == pMesh->Name)
				bFlag = true;
		}

		// 선택 여부를 체크한다.
		if (bFlag)
			m_SceneTree.SetItemState(hItem, UINT(TVIS_SELECTED), UINT(TVIS_SELECTED));
		else
			m_SceneTree.SetItemState(hItem, UINT(~TVIS_SELECTED), UINT(TVIS_SELECTED));

		// 다음 아이템을 구한다.
		hItem = m_SceneTree.GetNextVisibleItem(hItem);
	}
}

// TreeCtrl의 아이템이 클릭되는 순간 호출된다 (이전 아이템의 정보가 전달)
void GSceneTreeDlg::OnNMClickMeshTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 현재 선택된 아이템의 정보를 구하여,
	CPoint pt ;
	GetCursorPos(&pt);
	m_SceneTree.ScreenToClient(&pt) ;
	UINT unFlags = 0 ;
	HTREEITEM hItem = m_SceneTree.HitTest(pt, &unFlags);
 	if ((unFlags & TVHT_ONITEMLABEL) && hItem != NULL)
	{
		// 아이템의 이름으로 메쉬를 선택한다.
 		CString Name = m_SceneTree.GetItemText(hItem);
		theEditor().theSelector.SelectMesh(std::string((CStringA)(Name)));
	}

	// 선택된 아이템의 표시를 갱신한다.
	UpdateSelectedItems();	


	int NumObj = theEditor().EditObjList.size();
	std::string msg = cast_str(NumObj) + "개의 객체가 선택되었습니다.";
	theDoc().PrintMessage(msg);

	// 메쉬의 선택 결과를 작업창에 반영한다.
	HWND hWnd = GetParent()->GetSafeHwnd();
	::PostMessage(hWnd, WM_UPDATE_SELECT, 0, 0);	
	theDoc().UpdateAllViews(NULL);
	*pResult = 0;
}

// 선택된 아이템이 변경될 때 호출된다(동일한 아이템을 선택할 경우 호출 되지 않음)
void GSceneTreeDlg::OnTvnSelchangedMeshTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	// 이전 선택된 아이템과 현재 선택된 아이템의 핸들을 구하여,
	HTREEITEM hFrom = pNMTreeView->itemOld.hItem;
	HTREEITEM hTo = pNMTreeView->itemNew.hItem;

	// SHIFT 키가 눌렸다면,
	if (::GetKeyState(VK_SHIFT) & 0x8000)
	{
		HTREEITEM hItem = m_SceneTree.GetRootItem();
		bool bSelect = false;
		while (hItem) 
		{
			if (hItem == hFrom || hItem == hTo)
				bSelect = !bSelect;

			if (bSelect || hItem == hFrom || hItem == hTo)
			{
				if (m_SceneTree.GetItemState(hItem, UINT(TVIS_SELECTED)) == 0)
				{
					CString Name = m_SceneTree.GetItemText(hItem);
					theEditor().theSelector.SelectMesh(std::string((CStringA)(Name)));
				}				
			}

			hItem = m_SceneTree.GetNextVisibleItem(hItem);
		}
	}
	
	// 선택된 아이템의 표시를 갱신한다.
	UpdateSelectedItems();


	int NumObj = theEditor().EditObjList.size();
	std::string msg = cast_str(NumObj) + "개의 객체가 선택되었습니다.";
	theDoc().PrintMessage(msg);
}

void GSceneTreeDlg::OnNMKillfocusMeshTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 선택된 아이템의 표시를 갱신한다.
	UpdateSelectedItems();	
	*pResult = 0;
}

void GSceneTreeDlg::OnContextMenu(CWnd * pWnd, CPoint point)
{
	// 팝업 메뉴가 출력될 위치를 계산하고,
	if (point.x == -1 && point.y == -1)
	{
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);
		point = rect.TopLeft();
		point.Offset(5, 5);
	}

	// 장면 트리의 팝업 메뉴를 로드하고,
	CMenu menu;
	menu.LoadMenu(IDR_SCENE_TREE_POPUP);

	// 팝업 메뉴를 출력한다.
	CMenu *pPopup = menu.GetSubMenu(0);
	CWnd *pWndPopupOwner = this;

	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pWndPopupOwner);	
}

HBRUSH GSceneTreeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	unsigned char *BC = theScene().UIBkgColor;
	unsigned char *TC = theScene().UItxtColor;
	pDC->SetBkColor(RGB(BC[0], BC[1], BC[2]));
	pDC->SetTextColor(RGB(TC[0], TC[1], TC[2]));
	pDC->SetBkMode(OPAQUE);
	static CBrush BkgBrush(RGB(BC[0], BC[1], BC[2]));
	hbr = (HBRUSH)BkgBrush;

	return hbr;
}


