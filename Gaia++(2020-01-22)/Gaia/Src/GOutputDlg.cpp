#include "stdafx.h"
#include "..\Gaia.h"
#include "GOutputDlg.h"


// GOutputDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(GOutputDlg, CDialog)

GOutputDlg::GOutputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GOutputDlg::IDD, pParent)
	, m_OutputMessage(_T(""))
{
}

GOutputDlg::~GOutputDlg()
{
}

void GOutputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MESSAGE_BOX, m_cOutputMessage);
	DDX_Text(pDX, IDC_MESSAGE_BOX, m_OutputMessage);
}


BEGIN_MESSAGE_MAP(GOutputDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// GOutputDlg 메시지 처리기입니다.

BOOL GOutputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL GOutputDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN )
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

HBRUSH GOutputDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	unsigned char *BC = theScene().UIBkgColor;
	unsigned char *TC = theScene().UItxtColor;
	pDC->SetBkColor(RGB(BC[0] / 2, BC[1] / 2, BC[2] / 2));
	pDC->SetTextColor(RGB(TC[0], TC[1], TC[2]));
	pDC->SetBkMode(OPAQUE);

	static CBrush BkgBrush(RGB(BC[0], BC[1], BC[2]));
	hbr = (HBRUSH)BkgBrush;

	return hbr;
}

void GOutputDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	static bool bInit = false;
	if (bInit)
	{
		CRect sz;
		GetClientRect(sz);
		sz.DeflateRect(5, 5);
		// 	sz.top += 6;
		// 	sz.left += 330;
		// 	sz.right = sz.right;
		// 	sz.bottom -= 6;
		m_cOutputMessage.MoveWindow(sz, false);	
	}
	else
		bInit = true;
}
