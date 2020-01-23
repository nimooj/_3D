#pragma once
#include "afxcmn.h"

// PAddMtlDlg 대화 상자입니다.
class PEditor;

class PAddMtlDlg : public CDialog
{
	DECLARE_DYNAMIC(PAddMtlDlg)

public:
	PAddMtlDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~PAddMtlDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ADD_MTL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
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

	// 콘트롤 값 저장 변수
	int m_AmbientR;
	int m_AmbientG;
	int m_AmbientB;
	int m_DiffuseR;
	int m_DiffuseG;
	int m_DiffuseB;
	int m_SpecularR;
	int m_SpecularG;
	int m_SpecularB;

	// 콘트롤 변수
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
