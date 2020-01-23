// Src\GSceneTreeDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../Gaia.h"
#include "../MainFrm.h"
#include "GSceneTreeDlg.h"


// GSceneTreeDlg ��ȭ �����Դϴ�.

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


// GSceneTreeDlg �޽��� ó�����Դϴ�.

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
		
	// ��� Ʈ���� �ٽ� �׸���.
	m_SceneTree.Invalidate(false);

	return TRUE;
}

void GSceneTreeDlg::UpdateSceneTree()
{
	// Ʈ���� ��� �������� �����ϰ�,
	m_SceneTree.DeleteAllItems();

	// 3���� ��Ʈ �������� �߰��Ѵ�.
	m_RootMesh = m_SceneTree.InsertItem(_T("Mesh"), TVI_ROOT);
	m_RootLight = m_SceneTree.InsertItem(_T("Light"), TVI_ROOT);
	m_RootCamera = m_SceneTree.InsertItem(_T("Camera"), TVI_ROOT);

	int NumMesh = (int)theScene().MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
		InsertMesh(theScene().MeshList[i], m_RootMesh);

	// ��� Ʈ���� �ٽ� �׸���.
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

	// ��� ȣ���� ���� �޽� ��ü�� ��� Ʈ���� �߰��Ѵ�.
	int NumSubMesh = (int)pMesh->SubMeshList.size();
	for (int i = 0; i < NumSubMesh; ++i)
		InsertMesh(pMesh->SubMeshList[i], hNextItem);
}

void GSceneTreeDlg::DeleteItem()
{
	// ��� Ʈ������ ���õ� �������� �ڵ��� ���Ͽ�,
	HTREEITEM item = m_SceneTree.GetSelectedItem();

	// ���õ� �ڵ��� ��Ʈ �ڵ��̸� �����ϰ�,
	if (item == m_RootMesh || item == m_RootLight || item == m_RootCamera)
		return;

	// �ƴ϶��, �ش� �������� ��� Ʈ������ �����Ѵ�.
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
	// Ʈ���� ��� �����ۿ� ���Ͽ�,
	HTREEITEM hItem = m_SceneTree.GetRootItem();
	while (hItem) 
	{
		// �������� �̸��� ���ϰ�,
		CString Name = m_SceneTree.GetItemText(hItem);

		// ���õ� ������������ �����Ͽ�,
		bool bFlag = false;
		int NumObj = theEditor().GetNumEditObj();
		for (int i = 0; i < NumObj; ++i)
		{
			GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
			if (std::string((CStringA)(Name)) == pMesh->Name)
				bFlag = true;
		}

		// ���� ���θ� üũ�Ѵ�.
		if (bFlag)
			m_SceneTree.SetItemState(hItem, UINT(TVIS_SELECTED), UINT(TVIS_SELECTED));
		else
			m_SceneTree.SetItemState(hItem, UINT(~TVIS_SELECTED), UINT(TVIS_SELECTED));

		// ���� �������� ���Ѵ�.
		hItem = m_SceneTree.GetNextVisibleItem(hItem);
	}
}

// TreeCtrl�� �������� Ŭ���Ǵ� ���� ȣ��ȴ� (���� �������� ������ ����)
void GSceneTreeDlg::OnNMClickMeshTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// ���� ���õ� �������� ������ ���Ͽ�,
	CPoint pt ;
	GetCursorPos(&pt);
	m_SceneTree.ScreenToClient(&pt) ;
	UINT unFlags = 0 ;
	HTREEITEM hItem = m_SceneTree.HitTest(pt, &unFlags);
 	if ((unFlags & TVHT_ONITEMLABEL) && hItem != NULL)
	{
		// �������� �̸����� �޽��� �����Ѵ�.
 		CString Name = m_SceneTree.GetItemText(hItem);
		theEditor().theSelector.SelectMesh(std::string((CStringA)(Name)));
	}

	// ���õ� �������� ǥ�ø� �����Ѵ�.
	UpdateSelectedItems();	


	int NumObj = theEditor().EditObjList.size();
	std::string msg = cast_str(NumObj) + "���� ��ü�� ���õǾ����ϴ�.";
	theDoc().PrintMessage(msg);

	// �޽��� ���� ����� �۾�â�� �ݿ��Ѵ�.
	HWND hWnd = GetParent()->GetSafeHwnd();
	::PostMessage(hWnd, WM_UPDATE_SELECT, 0, 0);	
	theDoc().UpdateAllViews(NULL);
	*pResult = 0;
}

// ���õ� �������� ����� �� ȣ��ȴ�(������ �������� ������ ��� ȣ�� ���� ����)
void GSceneTreeDlg::OnTvnSelchangedMeshTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	// ���� ���õ� �����۰� ���� ���õ� �������� �ڵ��� ���Ͽ�,
	HTREEITEM hFrom = pNMTreeView->itemOld.hItem;
	HTREEITEM hTo = pNMTreeView->itemNew.hItem;

	// SHIFT Ű�� ���ȴٸ�,
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
	
	// ���õ� �������� ǥ�ø� �����Ѵ�.
	UpdateSelectedItems();


	int NumObj = theEditor().EditObjList.size();
	std::string msg = cast_str(NumObj) + "���� ��ü�� ���õǾ����ϴ�.";
	theDoc().PrintMessage(msg);
}

void GSceneTreeDlg::OnNMKillfocusMeshTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// ���õ� �������� ǥ�ø� �����Ѵ�.
	UpdateSelectedItems();	
	*pResult = 0;
}

void GSceneTreeDlg::OnContextMenu(CWnd * pWnd, CPoint point)
{
	// �˾� �޴��� ��µ� ��ġ�� ����ϰ�,
	if (point.x == -1 && point.y == -1)
	{
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);
		point = rect.TopLeft();
		point.Offset(5, 5);
	}

	// ��� Ʈ���� �˾� �޴��� �ε��ϰ�,
	CMenu menu;
	menu.LoadMenu(IDR_SCENE_TREE_POPUP);

	// �˾� �޴��� ����Ѵ�.
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


