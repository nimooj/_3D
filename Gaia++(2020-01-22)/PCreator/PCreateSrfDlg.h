#pragma once

// PCreateSrfDlg 대화 상자입니다.
class AFX_EXT_CLASS PCreateSrfDlg : public CDialog
{
	DECLARE_DYNAMIC(PCreateSrfDlg)
public:
	PCreateSrfDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~PCreateSrfDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_CREATE_SRF_DLG };

public:
	virtual BOOL OnInitDialog();

protected:
	// 가상함수
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	
public:

	// 메시지 처리함수
	DECLARE_MESSAGE_MAP()
	afx_msg void OnEnKillfocusSrfName();
	afx_msg void OnEnKillfocusSrfDegU();
	afx_msg void OnEnSetfocusSrfDegV();
	afx_msg void OnEnKillfocusNumCtlptU();
	afx_msg void OnEnKillfocusNumCtlptV();
	afx_msg void OnBnClickedKnotClampedU();
	afx_msg void OnBnClickedKnotUnclampedU();
	afx_msg void OnBnClickedKnotClampedV();
	afx_msg void OnBnClickedKnotUnclampedV();
	afx_msg void OnBnClickedSrfRational();
	afx_msg void OnBnClickedSrfPolynomial();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

public:
	// 데이터 멤버
	/*! \brief PCreator 객체에 대한 포인터 */
	PCreator *pCreator;

	/*! \brief 현재 생성되고 있는 곡면의 이름 */
	CString m_SrfName;

	/*! \brief 현재 생성되고 있는 곡면의 U방향 차수 */
	int m_SrfDegU;

	/*! \brief 현재 생성되고 있는 곡면의 V방향 차수 */
	int m_SrfDegV;

	/*! \brief 현재 생성되고 있는 곡면의 U방향 제어점 개수 */
	int m_NumCtlPtU;

	/*! \brief 현재 생성되고 있는 곡면의 V방향 제어점 개수 */
	int m_NumCtlPtV;

	/*! \brief U방향 낫 벡터의 타입 (0: clamped, 1: unclamped, 2: periodic) */
	int m_TypeKnotU;

	/*! \brief V방향 낫 벡터의 타입 (0: clamped, 1: unclamped, 2: periodic) */
	int m_TypeKnotV;

	/*! \brief 유리식 곡면 여부 */
	int m_TypeRational;
};
