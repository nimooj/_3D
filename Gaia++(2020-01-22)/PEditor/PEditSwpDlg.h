#pragma once
#include "afxcmn.h"


// PEditSwpDlg ��ȭ �����Դϴ�.

class PEditSwpDlg : public CDialog
{
	DECLARE_DYNAMIC(PEditSwpDlg)

public:
	PEditSwpDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~PEditSwpDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_EDIT_SWP_DLG };

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
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
	// ������ ���
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
