#pragma once
#include "afxcmn.h"


// PEditSrfDlg 대화 상자입니다.

class PEditSrfDlg : public CDialog
{
	DECLARE_DYNAMIC(PEditSrfDlg)

public:
	PEditSrfDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~PEditSrfDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EDIT_SRF_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedRefineU();
	afx_msg void OnBnClickedRefineV();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnKillfocusCtrlPtSize();
	afx_msg void OnEnKillfocusSnumU();
	afx_msg void OnEnKillfocusSnumV();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		
public:
	// 데이터 멤버
	CDocument *pDoc;
	PEditor *pEditor;

	int m_SNumU;
	int m_SNumV;
	CSpinButtonCtrl m_cSNumU;
	CSpinButtonCtrl m_cSNumV;
	double m_CtrlPtSize;	
	afx_msg void OnBnClickedElevateDegU();
	afx_msg void OnBnClickedElevateDegV();
	afx_msg void OnBnClickedCnvrtBzrForm();
	afx_msg void OnBnClickedCnvrtBspForm();
	afx_msg void OnBnClickedModifyKnotClampedU();
	afx_msg void OnBnClickedModifyKnotClampedV();
	afx_msg void OnBnClickedModifyKnotUnclampedU();
	afx_msg void OnBnClickedModifyKnotUnclampedV();
};
