#pragma once
#include "afxwin.h"


// GOutputDlg 대화 상자입니다.

class GOutputDlg : public CDialog
{
	DECLARE_DYNAMIC(GOutputDlg)

public:
	GOutputDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~GOutputDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_OUTPUT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

public:
	CEdit m_cOutputMessage;
	CString m_OutputMessage;
};
