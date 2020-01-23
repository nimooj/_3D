#pragma once
#include "afxcmn.h"


// PEditSwpDlg 대화 상자입니다.

class PEditSwpDlg : public CDialog
{
	DECLARE_DYNAMIC(PEditSwpDlg)

public:
	PEditSwpDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~PEditSwpDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EDIT_SWP_DLG };

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnKillfocusSnumU();
	afx_msg void OnEnKillfocusSnumV();
	afx_msg void OnEnKillfocusSwpPlnWidth();
	afx_msg void OnEnKillfocusSwpPlnHeight();
	afx_msg void OnBnClickedSwpRenderTypeSrf();
	afx_msg void OnBnClickedSwpRenderTypePln();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()

public:
	// 데이터 멤버
	CDocument *pDoc;
	PEditor *pEditor;
	int m_SNumU;
	int m_SNumV;
	CSpinButtonCtrl m_cSNumU;
	CSpinButtonCtrl m_cSNumV;
	int m_SwpRenderType;
	double m_SwpPlnWidth;
	double m_SwpPlnHeight;	
};
