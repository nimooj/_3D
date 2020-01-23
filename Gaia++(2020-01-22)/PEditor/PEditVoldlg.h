#pragma once
#include "afxcmn.h"


// PEditVolDlg ��ȭ �����Դϴ�.

class PEditVolDlg : public CDialog
{
	DECLARE_DYNAMIC(PEditVolDlg)

public:
	PEditVolDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~PEditVolDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_EDIT_VOL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
	// ������ ���
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
