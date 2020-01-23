#include "stdafx.h"
#include "..\Gaia.h"
#include "..\MainFrm.h"
#include "GWorkSpaceDlg.h"

// GWorkSpaceDlg ��ȭ �����Դϴ�.
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

// GWorkSpaceDlg �޽��� ó�����Դϴ�.
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
*	\brief ���ο� �� ��ȭ���ڸ� �߰��Ѵ�.
*
*	\param pDlg �߰��� �� ��ȭ����
*	\param TabName ��ȭ������ �� �̸�
*	\param bSelect �߰��� ���� ����
*
*/
void GWorkSpaceDlg::InsertTabDlg(CDialog *pDlg, CString TabName, bool bSelect)
{
	// �߰��� ��ȭ���ڸ� �ʱ�ȭ �ϰ�,
	pDlg->OnInitDialog();

	// �߰��� ��ȭ���ڰ� �̹� �ִٸ�, �ʱ�ȭ�� �ϰ� �����Ѵ�.
	int NumDlg = (int)m_pTabDlgList.size();
	for (int i = 0; i < NumDlg; ++i)
	{
		if (m_pTabDlgList[i] == pDlg)
			return;
	}

	// ���ο� ��ȭ���ڶ�� �߰��Ѵ�.
	int idx = m_cWorkSpaceTab.GetItemCount();
	m_cWorkSpaceTab.InsertItem(idx, TabName);
	m_pTabDlgList.push_back(pDlg);

	// ���� ���ο� ���� �߰��� ��ȭ ���ڸ� �����ϰų� m_SceneTreeDlg�� �����Ѵ�.
	if (bSelect)
		SetCurrTab(idx);
	else
		SetCurrTab(0);

	// ��ȭ������ ���̾ƿ��� ������ �Ѵ�.
 	RecalcLayOut();
}

void GWorkSpaceDlg::DeleteTabDlg()
{
	// WorkSpaceTree ��ȭ���ڸ� ������ ��� ��ȭ���ڸ� �����Ѵ�.
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

	// ��ȭ ���ڸ� ��� ���� ��,
	int NumDlg = (int)m_pTabDlgList.size();
	for (int i = 0; i < NumDlg; i++)
		m_pTabDlgList[i]->ShowWindow(SW_HIDE);

	// ���õ� ��ȭ ���ڸ� ���δ�
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

	// �߰� ��ȭ���ڸ� ���ġ �Ѵ�.
	int Num = (int)m_pTabDlgList.size();
	for (int i = 0; i < Num; i++)
		m_pTabDlgList[i]->MoveWindow(&rcClient);
}

void GWorkSpaceDlg::UpdateWorkSpace()
{
	// ���� SceneTreeDlg�� ������ �����ϰ�,
	((GSceneTreeDlg *)m_pTabDlgList[0])->UpdateSceneTree();

	// SceneTreeDlg�� ������ ��� ��ȭ���ڸ� �����.
	DeleteTabDlg();

	// WorkSpaceTreeDlg ��ȭ ���ڸ� Ȱ��ȭ �Ѵ�.
	SetCurrTab(0);

	theDoc().UpdateAllViews(NULL);
}

void GWorkSpaceDlg::UpdateSelect()
{
	// SceneTreeDlg�� ������ ��� ��ȭ���ڸ� �����.
	DeleteTabDlg();

	// ���õ� ���� �޽��� �ִٸ�,
	if (!theEditor().EditObjList.empty())
	{
		// �޽� ���� ��ȭ���ڸ� �߰��ϰ�,
		InsertTabDlg(&theEditor().EditMeshDlg, _T("Mesh Attrib."), false);	

		// ���õ� ��ü�� ���� �޽��� ���ϰ�,
		GMesh *pMesh = theEditor().EditObjList[0]->GetRefMesh();

		// ���õ� ��ü�� Ŭ������ ���Ͽ�,
		std::string ClsName = pMesh->GetClassName();

		// ���õ� ��ü�� ��̶��, � ���� ��ȭ���ڸ� �߰��ϰ�,
		if (ClsName == "GNurbsCrv")
		{
			InsertTabDlg(&theEditor().EditCrvDlg, _T("Curve Attrib."), false);
		}

		// ���õ� ��ü�� ����̶��, ��� ���� ��ȭ���ڸ� �߰��ϰ�,
		if (ClsName == "GNurbsSrf" || ClsName == "GCoonsSrf" || ClsName == "GUcbsSrf")
		{
			InsertTabDlg(&theEditor().EditSrfDlg, _T("Surface Attrib."), false);
		}

		// ���õ� ��ü�� �����̶��, ���� ���� ��ȭ���ڸ� �߰��ϰ�,
		if (ClsName == "GUcbsVol")
		{
			InsertTabDlg(&theEditor().EditVolDlg, _T("Volume Attrib."), false);
		}

		// ���õ� ��ü�� �����̶��, ���� ���� ��ȭ���ڸ� �߰��ϰ�,
		if (ClsName == "GSweepSrf")
		{
			InsertTabDlg(&theEditor().EditSwpDlg, _T("Sweep Attrib."), false);
		}

		// ���õ� ��ü�� ��������̶��, ���� ��� ���� ��ȭ���ڸ� �߰��ϰ�,
		if (pMesh->SemanticType == MESH_CTRL || ClsName == "GBlendMesh")
		{
			InsertTabDlg(&theEditor().EditBlendSrfDlg, _T("BlendSrf Attrib."), false);
		}
	}
	else	// ���õ� ���� �޽��� ���ٸ� m_SceneTreeDlg�� Ȱ��ȭ �Ѵ�.
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
