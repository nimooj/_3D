// PCreatePlnDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PCreatePlnDlg.h"


// PCreatePlnDlg 대화 상자입니다.

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


// PCreatePlnDlg 메시지 처리기입니다.

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
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
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
