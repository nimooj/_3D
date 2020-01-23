// GDepthViewDlg.cpp : ���� �����Դϴ�.
//
#include "stdafx.h"
#include "..\Gaia.h"
#include "..\MainFrm.h"
#include "GDepthViewDlg.h"


// GDepthViewDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(GDepthViewDlg, CDialog)

GDepthViewDlg::GDepthViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GDepthViewDlg::IDD, pParent)
{
	m_pView = NULL;
}

GDepthViewDlg::~GDepthViewDlg()
{
	delete m_pView;
	m_pView = NULL;
}

void GDepthViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(GDepthViewDlg, CDialog)
END_MESSAGE_MAP()


// GDepthViewDlg �޽��� ó�����Դϴ�.

BOOL GDepthViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Create view for rendering control curves in parametric space
	if (m_pView == NULL)
	{
		// OpenGL �並 �����ϰ�,
		m_pView = new GView();
		m_pView->Create(NULL, "Depth View", WS_VISIBLE | WS_CHILD | WS_BORDER, CRect(12, 12, 524, 524), this, IDW_DEPTH_VIEW);
		m_pView->m_pDialog = this;
		m_pView->m_MeshList = m_MeshList;
	}
	return TRUE; 
}
