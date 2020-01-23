#pragma once
#include "afxcmn.h"


// PEditCrvDlg ��ȭ �����Դϴ�.

class PEditCrvDlg : public CDialog
{
	DECLARE_DYNAMIC(PEditCrvDlg)

public:
	PEditCrvDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~PEditCrvDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_EDIT_CRV_DLG };

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	afx_msg void OnBnClickedRefineU();
	afx_msg void OnEnKillfocusCtrlPtSize();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnKillfocusSnumU();
	afx_msg void OnEnKillfocusSnumV();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedCnvrtBzrForm();
	afx_msg void OnBnClickedCnvrtBspForm();
	afx_msg void OnBnClickedElevateDegree();
	afx_msg void OnBnClickedModifyKnotClamped();
	afx_msg void OnBnClickedModifyKnotUnclamped();
	afx_msg void OnBnClickedReverseDirection();
	afx_msg void OnBnClickedRenderAsLine();
	DECLARE_MESSAGE_MAP()

public:
	CDocument *pDoc;
	PEditor *pEditor;
	int m_SNumU;
	int m_SNumV;
	CSpinButtonCtrl m_cSNumU;
	CSpinButtonCtrl m_cSNumV;
	double m_CtrlPtSize;
	BOOL m_RenderAsLine;	
};
