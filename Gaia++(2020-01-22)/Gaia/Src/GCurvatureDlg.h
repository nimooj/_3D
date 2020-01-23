#pragma once
#include "afxwin.h"


// GCurvatureDlg 대화 상자입니다.

class GCurvatureDlg : public CDialog
{
	DECLARE_DYNAMIC(GCurvatureDlg)

public:
	GCurvatureDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~GCurvatureDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CURVATURE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	/*! \brief 곡률을 측정할 대상 메쉬 */
	GMesh *m_pMesh;	

	/*! \brief 메쉬 정점의 가우스, 평균, 최소, 최대 곡률의 정보를 저장하는 배열 */
	std::vector<double> m_kG;
	std::vector<double> m_kH;
	std::vector<double> m_kMin;
	std::vector<double> m_kMax;

	/*! \brief 곡률의 가시화 범위 */
	double m_MinVal;
	double m_MaxVal;

	CComboBox m_cCurvatureType;

	// 이벤트 처리 함수들
	afx_msg void OnEnKillfocusMinVal();
	afx_msg void OnEnKillfocusMaxVal();
	afx_msg void OnCbnCloseupCurvatureType();

	// 사용자 멤버함수
	void UpdateCurvatureRange();	
};
