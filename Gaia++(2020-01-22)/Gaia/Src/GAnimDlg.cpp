// GAnimDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "..\Gaia.h"
#include "GAnimDlg.h"


// GAnimDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(GAnimDlg, CDialog)

GAnimDlg::GAnimDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GAnimDlg::IDD, pParent)
	, m_CurrFrameIdx(0)
	, m_NumFrame(0)
{
}

GAnimDlg::~GAnimDlg()
{
}

void GAnimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FRAME_SLIDER, m_cFrameSlider);
	DDX_Control(pDX, IDC_GO_TO_START, m_GoToStart);
	DDX_Control(pDX, IDC_STEP_BACK_ONE_FRAME, m_StepBackOneFrame);
	DDX_Control(pDX, IDC_STEP_BACK_ONE_KEY, m_StepBackOneKey);
	DDX_Control(pDX, IDC_PLAY_BACKWARD, m_PlayBackward);
	DDX_Control(pDX, IDC_PLAY_FORWARD, m_PlayForward);
	DDX_Control(pDX, IDC_STEP_FORWARD_ONE_KEY, m_StepForwardOneKey);
	DDX_Control(pDX, IDC_STEP_FORWARD_ONE_FRAME, m_StepForwardOneFrame);
	DDX_Control(pDX, IDC_GO_TO_END, m_GoToEnd);
	DDX_Text(pDX, IDC_CURR_FRAME_IDX, m_CurrFrameIdx);
	DDX_Text(pDX, IDC_NUM_FRAME, m_NumFrame);
}


BEGIN_MESSAGE_MAP(GAnimDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_GO_TO_START, &GAnimDlg::OnBnClickedGoToStart)
	ON_BN_CLICKED(IDC_STEP_BACK_ONE_FRAME, &GAnimDlg::OnBnClickedStepBackOneFrame)
	ON_BN_CLICKED(IDC_STEP_BACK_ONE_KEY, &GAnimDlg::OnBnClickedStepBackOneKey)
	ON_BN_CLICKED(IDC_PLAY_BACKWARD, &GAnimDlg::OnBnClickedPlayBackward)
	ON_BN_CLICKED(IDC_PLAY_FORWARD, &GAnimDlg::OnBnClickedPlayForward)
	ON_BN_CLICKED(IDC_STEP_FORWARD_ONE_KEY, &GAnimDlg::OnBnClickedStepForwardOneKey)
	ON_BN_CLICKED(IDC_STEP_FORWARD_ONE_FRAME, &GAnimDlg::OnBnClickedStepForwardOneFrame)
	ON_BN_CLICKED(IDC_GO_TO_END, &GAnimDlg::OnBnClickedGoToEnd)
	ON_WM_TIMER()
	ON_EN_KILLFOCUS(IDC_NUM_FRAME, &GAnimDlg::OnEnKillfocusNumFrame)
	ON_EN_KILLFOCUS(IDC_CURR_FRAME_IDX, &GAnimDlg::OnEnKillfocusCurrFrameIdx)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// GAnimDlg 메시지 처리기입니다.

void GAnimDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	static bool bInit = false;
	if (bInit)
	{
		CRect sz;
		GetClientRect(sz);
		//sz.DeflateRect(5, 5);
		//sz.top += 6;
		sz.left += 330;
		sz.right = sz.right;
		sz.bottom -= 6;

		m_cFrameSlider.MoveWindow(sz, false);	
	}
	else
		bInit = true;
}

BOOL GAnimDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bPlayBackward = false;
	m_bPlayForward = false;
	m_FrameStep = 1;
	m_CurrFrameIdx = 0;
	m_NumFrame = 100;
	m_cFrameSlider.SetRange(0, m_NumFrame - 1);

	m_GoToStart.LoadBitmaps(IDB_GO_TO_START_OFF, IDB_GO_TO_START_ON, NULL, NULL);
	m_GoToStart.SizeToContent();

	m_StepBackOneFrame.LoadBitmaps(IDB_STEP_BACK_ONE_FRAME_OFF, IDB_STEP_BACK_ONE_FRAME_ON, NULL, NULL);
	m_StepBackOneFrame.SizeToContent();

	m_StepBackOneKey.LoadBitmaps(IDB_STEP_BACK_ONE_KEY_OFF, IDB_STEP_BACK_ONE_KEY_ON, NULL, NULL);
	m_StepBackOneKey.SizeToContent();

	m_PlayBackward.LoadBitmaps(IDB_PLAY_BACKWARD_OFF, IDB_PLAY_BACKWARD_ON, NULL, NULL);
	m_PlayBackward.SizeToContent();

	m_PlayForward.LoadBitmaps(IDB_PLAY_FORWARD_OFF, IDB_PLAY_FORWARD_ON, NULL, NULL);
	m_PlayForward.SizeToContent();

	m_StepForwardOneKey.LoadBitmaps(IDB_STEP_FORWARD_ONE_KEY_OFF, IDB_STEP_FORWARD_ONE_KEY_ON, NULL, NULL);
	m_StepForwardOneKey.SizeToContent();

	m_StepForwardOneFrame.LoadBitmaps(IDB_STEP_FORWARD_ONE_FRAME_OFF, IDB_STEP_FORWARD_ONE_FRAME_ON, NULL, NULL);
	m_StepForwardOneFrame.SizeToContent();

	m_GoToEnd.LoadBitmaps(IDB_GO_TO_END_OFF, IDB_GO_TO_END_ON, NULL, NULL);
	m_GoToEnd.SizeToContent();

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL GAnimDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN )
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

HBRUSH GAnimDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void GAnimDlg::OnBnClickedGoToStart()
{
	if (m_bPlayBackward || m_bPlayForward)
		return;

	m_CurrFrameIdx = 0;
	m_cFrameSlider.SetPos(m_CurrFrameIdx);
	UpdateData(FALSE);
}

void GAnimDlg::OnBnClickedStepBackOneFrame()
{
	if (m_bPlayBackward || m_bPlayForward)
		return;

	m_CurrFrameIdx--;
	if (m_CurrFrameIdx == m_NumFrame)
		m_CurrFrameIdx = 0;
	if (m_CurrFrameIdx == -1)
		m_CurrFrameIdx = m_NumFrame - 1;

	m_cFrameSlider.SetPos(m_CurrFrameIdx);
	UpdateData(FALSE);
}

void GAnimDlg::OnBnClickedStepBackOneKey()
{
	if (m_bPlayBackward || m_bPlayForward)
		return;

	m_CurrFrameIdx--;
	if (m_CurrFrameIdx == m_NumFrame)
		m_CurrFrameIdx = 0;
	if (m_CurrFrameIdx == -1)
		m_CurrFrameIdx = m_NumFrame - 1;

	m_cFrameSlider.SetPos(m_CurrFrameIdx);
	UpdateData(FALSE);
}

void GAnimDlg::OnBnClickedPlayBackward()
{
	m_bPlayBackward = !m_bPlayBackward;
	m_bPlayForward = false;

	if (m_bPlayBackward)
	{
		m_PlayBackward.LoadBitmaps(IDB_PLAY_STOP_OFF, IDB_PLAY_STOP_ON, NULL, NULL);
		m_PlayForward.LoadBitmaps(IDB_PLAY_FORWARD_OFF, IDB_PLAY_FORWARD_ON, NULL, NULL);
		m_PlayBackward.Invalidate(TRUE);
		m_PlayForward.Invalidate(TRUE);	

		SetTimer(0, 42, NULL);	// 24 fps
		m_FrameStep = -1;
		theDoc().PrintMessage("Play backward...");
	}
	else
	{
		m_PlayBackward.LoadBitmaps(IDB_PLAY_BACKWARD_OFF, IDB_PLAY_BACKWARD_ON, NULL, NULL);
		m_PlayForward.LoadBitmaps(IDB_PLAY_FORWARD_OFF, IDB_PLAY_FORWARD_ON, NULL, NULL);
		m_PlayBackward.Invalidate(TRUE);
		m_PlayForward.Invalidate(TRUE);	

		KillTimer(0);
	}
}

void GAnimDlg::OnBnClickedPlayForward()
{
	m_bPlayForward = !m_bPlayForward;
	m_bPlayBackward = false;

	if (m_bPlayForward)
	{
		m_PlayForward.LoadBitmaps(IDB_PLAY_STOP_OFF, IDB_PLAY_STOP_ON, NULL, NULL);
		m_PlayBackward.LoadBitmaps(IDB_PLAY_BACKWARD_OFF, IDB_PLAY_BACKWARD_ON, NULL, NULL);
		m_PlayBackward.Invalidate(TRUE);
		m_PlayForward.Invalidate(TRUE);

		SetTimer(0, 42, NULL);	// 24 fps
		m_FrameStep = 1;
		theDoc().PrintMessage("Play forward...");
	}
	else
	{
		m_PlayForward.LoadBitmaps(IDB_PLAY_FORWARD_OFF, IDB_PLAY_FORWARD_ON, NULL, NULL);
		m_PlayBackward.LoadBitmaps(IDB_PLAY_BACKWARD_OFF, IDB_PLAY_BACKWARD_ON, NULL, NULL);
		m_PlayBackward.Invalidate(TRUE);
		m_PlayForward.Invalidate(TRUE);

		KillTimer(0);
	}	
}

void GAnimDlg::OnBnClickedStepForwardOneKey()
{
	if (m_bPlayBackward || m_bPlayForward)
		return;

	m_CurrFrameIdx++;
	if (m_CurrFrameIdx == m_NumFrame)
		m_CurrFrameIdx = 0;
	if (m_CurrFrameIdx == -1)
		m_CurrFrameIdx = m_NumFrame - 1;

	m_cFrameSlider.SetPos(m_CurrFrameIdx);
	UpdateData(FALSE);
}

void GAnimDlg::OnBnClickedStepForwardOneFrame()
{
	if (m_bPlayBackward || m_bPlayForward)
		return;

	m_CurrFrameIdx++;
	if (m_CurrFrameIdx == m_NumFrame)
		m_CurrFrameIdx = 0;
	if (m_CurrFrameIdx == -1)
		m_CurrFrameIdx = m_NumFrame - 1;

	m_cFrameSlider.SetPos(m_CurrFrameIdx);
	UpdateData(FALSE);
}

void GAnimDlg::OnBnClickedGoToEnd()
{
	if (m_bPlayBackward || m_bPlayForward)
		return;

	m_CurrFrameIdx = m_NumFrame - 1;
	m_cFrameSlider.SetPos(m_CurrFrameIdx);
	UpdateData(FALSE);
}

void GAnimDlg::OnTimer(UINT_PTR nIDEvent)
{
	m_CurrFrameIdx += m_FrameStep;
	if (m_CurrFrameIdx == m_NumFrame)
		m_CurrFrameIdx = 0;
	if (m_CurrFrameIdx == -1)
		m_CurrFrameIdx = m_NumFrame - 1;
	
	m_cFrameSlider.SetPos(m_CurrFrameIdx);

	theDoc().UpdateAllViews(NULL);

	UpdateData(FALSE);
	CDialog::OnTimer(nIDEvent);
}

void GAnimDlg::OnEnKillfocusNumFrame()
{
	UpdateData(TRUE);
	m_cFrameSlider.SetPos(m_CurrFrameIdx);
}

void GAnimDlg::OnEnKillfocusCurrFrameIdx()
{
	UpdateData(TRUE);
	m_cFrameSlider.SetRange(0, m_NumFrame - 1);
}

void GAnimDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	m_CurrFrameIdx = m_cFrameSlider.GetPos();
	UpdateData(FALSE);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
