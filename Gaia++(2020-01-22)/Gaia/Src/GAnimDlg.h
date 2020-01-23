#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "..\GaiaDoc.h"
// GAnimDlg ��ȭ �����Դϴ�.

class GAnimDlg : public CDialog
{
	DECLARE_DYNAMIC(GAnimDlg)

public:
	GAnimDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~GAnimDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ANIM_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	// ������ ���
	bool m_bPlayBackward;
	bool m_bPlayForward;
	int m_CurrFrameIdx;
	int m_NumFrame;
	int m_FrameStep;

	CSliderCtrl m_cFrameSlider;
	CBitmapButton m_GoToStart;
	CBitmapButton m_StepBackOneFrame;
	CBitmapButton m_StepBackOneKey;
	CBitmapButton m_PlayBackward;
	CBitmapButton m_PlayForward;
	CBitmapButton m_StepForwardOneKey;
	CBitmapButton m_StepForwardOneFrame;
	CBitmapButton m_GoToEnd;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedGoToStart();
	afx_msg void OnBnClickedStepBackOneFrame();
	afx_msg void OnBnClickedStepBackOneKey();
	afx_msg void OnBnClickedPlayBackward();
	afx_msg void OnBnClickedPlayForward();
	afx_msg void OnBnClickedStepForwardOneKey();
	afx_msg void OnBnClickedStepForwardOneFrame();
	afx_msg void OnBnClickedGoToEnd();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnKillfocusNumFrame();
	afx_msg void OnEnKillfocusCurrFrameIdx();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
