#pragma once
#include "afxwin.h"


// GOutputDlg ��ȭ �����Դϴ�.

class GOutputDlg : public CDialog
{
	DECLARE_DYNAMIC(GOutputDlg)

public:
	GOutputDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~GOutputDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_OUTPUT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

public:
	CEdit m_cOutputMessage;
	CString m_OutputMessage;
};
