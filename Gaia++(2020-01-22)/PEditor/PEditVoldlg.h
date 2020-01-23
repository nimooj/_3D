#pragma once
#include "afxcmn.h"


// PEditVolDlg 대화 상자입니다.

class PEditVolDlg : public CDialog
{
	DECLARE_DYNAMIC(PEditVolDlg)

public:
	PEditVolDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~PEditVolDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EDIT_VOL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	
	afx_msg void OnEnKillfocusSnumU();
	afx_msg void OnEnKillfocusSnumV();
	afx_msg void OnEnKillfocusSnumW();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

public:
	// 데이터 멤버
	CDocument *pDoc;
	PEditor *pEditor;
	int m_SNumU;
	int m_SNumV;
	int m_SNumW;	
	CSpinButtonCtrl m_cSNumU;
	CSpinButtonCtrl m_cSNumV;
	CSpinButtonCtrl m_cSNumW;
	CSliderCtrl m_cIsoUMin;
	CSliderCtrl m_cIsoUMax;
	CSliderCtrl m_cIsoVMin;
	CSliderCtrl m_cIsoVMax;
	CSliderCtrl m_cIsoWMin;
	CSliderCtrl m_cIsoWMax;
	int m_IsoUMin;
	int m_IsoUMax;
	int m_IsoVMin;
	int m_IsoVMax;
	int m_IsoWMin;
	int m_IsoWMax;
};
