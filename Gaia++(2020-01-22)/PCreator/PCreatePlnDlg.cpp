// PCreatePlnDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PCreatePlnDlg.h"


// PCreatePlnDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(PCreatePlnDlg, CDialog)

PCreatePlnDlg::PCreatePlnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PCreatePlnDlg::IDD, pParent)
	, m_Name(_T(""))
{

}

PCreatePlnDlg::~PCreatePlnDlg()
{
}

void PCreatePlnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PLN_NAME, m_Name);
}


BEGIN_MESSAGE_MAP(PCreatePlnDlg, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// PCreatePlnDlg �޽��� ó�����Դϴ�.

BOOL PCreatePlnDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (pCreator != NULL)
	{
		std::string name = create_mesh_name("plane");
		m_Name = name.c_str();
		UpdateData(false);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

HBRUSH PCreatePlnDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
