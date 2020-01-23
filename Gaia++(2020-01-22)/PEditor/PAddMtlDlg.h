#pragma once
#include "afxcmn.h"

// PAddMtlDlg ��ȭ �����Դϴ�.
class PEditor;

class PAddMtlDlg : public CDialog
{
	DECLARE_DYNAMIC(PAddMtlDlg)

public:
	PAddMtlDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~PAddMtlDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ADD_MTL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedMtlRandom();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
public:
	CDocument *pDoc;
	PEditor *pEditor;
	CString m_MtlName;
	CString m_TexFileName;
	int m_CurrMtlIdx;

	// ��Ʈ�� �� ���� ����
	int m_AmbientR;
	int m_AmbientG;
	int m_AmbientB;
	int m_DiffuseR;
	int m_DiffuseG;
	int m_DiffuseB;
	int m_SpecularR;
	int m_SpecularG;
	int m_SpecularB;

	// ��Ʈ�� ����
	CSliderCtrl m_cAmbientR;
	CSliderCtrl m_cAmbientG;
	CSliderCtrl m_cAmbientB;
	CSliderCtrl m_cDiffuseR;
	CSliderCtrl m_cDiffuseG;
	CSliderCtrl m_cDiffuseB;
	CSliderCtrl m_cSpecularR;
	CSliderCtrl m_cSpecularG;
	CSliderCtrl m_cSpecularB;
	afx_msg void OnBnClickedOpenDiffuseTexture();
};
