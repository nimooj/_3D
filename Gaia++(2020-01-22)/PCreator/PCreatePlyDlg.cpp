// PCreatePlyDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PCreatePlyDlg.h"

// PCreatePlyDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(PCreatePlyDlg, CDialog)

PCreatePlyDlg::PCreatePlyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PCreatePlyDlg::IDD, pParent)
	, m_PlyName(_T(""))
{
	pCreator = NULL;
}

PCreatePlyDlg::~PCreatePlyDlg()
{
}

void PCreatePlyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PLY_NAME, m_PlyName);
}


BEGIN_MESSAGE_MAP(PCreatePlyDlg, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// PCreatePlyDlg �޽��� ó�����Դϴ�.
BOOL PCreatePlyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (pCreator != NULL)
	{
		std::string name = create_mesh_name("polygon");
		m_PlyName = name.c_str();
		UpdateData(false);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
}


HBRUSH PCreatePlyDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	unsigned char *BC = pCreator->pScene->UIBkgColor;
	unsigned char *TC = pCreator->pScene->UItxtColor;
	pDC->SetBkColor(RGB(BC[0], BC[1], BC[2]));
	pDC->SetTextColor(RGB(TC[0], TC[1], TC[2]));
	pDC->SetBkMode(OPAQUE);
	static CBrush BkgBrush(RGB(BC[0], BC[1], BC[2]));
	hbr = (HBRUSH)BkgBrush;

	return hbr;
}
