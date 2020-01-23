#pragma once
#include "afxcmn.h"


// PEditBlendSrfDlg 대화 상자입니다.

class PEditBlendSrfDlg : public CDialog
{
	DECLARE_DYNAMIC(PEditBlendSrfDlg)

public:
	PEditBlendSrfDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~PEditBlendSrfDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EDIT_BLEND_SRF_DLG };

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnKillfocusSnumU();
	afx_msg void OnBnClickedPatchTypePn();
	afx_msg void OnBnClickedPatchTypeExtPn();
	afx_msg void OnBnClickedPatchTypeFlat();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()

public:
	// 데이터 멤버
	CDocument *pDoc;
	PEditor *pEditor;	
	int m_SNumU;
	CSpinButtonCtrl m_cSNumU;

	int m_PatchType;
	CSliderCtrl m_cBlendOffset;
	CSliderCtrl m_cBlendOffsetE0;
	CSliderCtrl m_cBlendOffsetE1;
	CSliderCtrl m_cBlendOffsetE2;
	int m_BlendOffset;
	int m_BlendOffsetE0;
	int m_BlendOffsetE1;
	int m_BlendOffsetE2;
};
