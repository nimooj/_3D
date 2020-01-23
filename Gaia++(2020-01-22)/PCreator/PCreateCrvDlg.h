#pragma once
#include "resource.h"

class PCreator;

// PCreateCrvDlg 대화 상자입니다.
class AFX_EXT_CLASS PCreateCrvDlg : public CDialog
{
	DECLARE_DYNAMIC(PCreateCrvDlg)
public:
	// 생성자 및 소멸자
	PCreateCrvDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~PCreateCrvDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_CREATE_CRV_DLG };

public:
	// 가상 함수 재정의
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()
public:
	// 메시지 처리함수
	afx_msg void OnEnKillfocusCrvName();
	afx_msg void OnEnKillfocusCrvDegree();
	afx_msg void OnBnClickedCrvCtrlPt();
	afx_msg void OnBnClickedCrvInterPt();
	afx_msg void OnBnClickedKnotCalmped();
	afx_msg void OnBnClickedKnotUnclamped();
	afx_msg void OnBnClickedKnotPeriod();
	afx_msg void OnBnClickedCrvRational();
	afx_msg void OnBnClickedCrvPolynomial();
	afx_msg void OnBnClickedFillCrv();
	afx_msg void OnBnClickedViewPlnCrv();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

public:
	// 데이터 멤버
	/*! \brief  PCreator 객체에 대한 포인터 */
	PCreator *pCreator;

	/*! \brief  생성할 곡선의 이름 */
	CString m_Name;

	/*! \brief  생성할 곡선의 차수 */
	int m_Degree;

	/*! \brief  곡선 생성 방법 (0: 제어점, 1: 보간점) */
	int m_TypeCreate;

	/*! \brief  생성할 곡선의 낫 벡터 타입 (0: clamped, 1: unclamped, 2: periodic) */
	int m_TypeKnot;

	/*! \brief 생성할 곡선 타입 (0: 유리식 곡선, 1: 다항식 곡선) */
	int m_TypeRational;	

	/*! \brief 곡선의 내부 영역 채움 여부 */
	BOOL m_bFillCrv;

	/*! \brief 뷰평면 위의 곡선 생성 여부 */
	BOOL m_bViewPlnCrv;
};
