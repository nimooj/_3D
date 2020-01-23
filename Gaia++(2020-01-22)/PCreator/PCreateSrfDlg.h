#pragma once

// PCreateSrfDlg ��ȭ �����Դϴ�.
class AFX_EXT_CLASS PCreateSrfDlg : public CDialog
{
	DECLARE_DYNAMIC(PCreateSrfDlg)
public:
	PCreateSrfDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~PCreateSrfDlg();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CREATE_SRF_DLG };

public:
	virtual BOOL OnInitDialog();

protected:
	// �����Լ�
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	
public:

	// �޽��� ó���Լ�
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
	// ������ ���
	/*! \brief PCreator ��ü�� ���� ������ */
	PCreator *pCreator;

	/*! \brief ���� �����ǰ� �ִ� ����� �̸� */
	CString m_SrfName;

	/*! \brief ���� �����ǰ� �ִ� ����� U���� ���� */
	int m_SrfDegU;

	/*! \brief ���� �����ǰ� �ִ� ����� V���� ���� */
	int m_SrfDegV;

	/*! \brief ���� �����ǰ� �ִ� ����� U���� ������ ���� */
	int m_NumCtlPtU;

	/*! \brief ���� �����ǰ� �ִ� ����� V���� ������ ���� */
	int m_NumCtlPtV;

	/*! \brief U���� �� ������ Ÿ�� (0: clamped, 1: unclamped, 2: periodic) */
	int m_TypeKnotU;

	/*! \brief V���� �� ������ Ÿ�� (0: clamped, 1: unclamped, 2: periodic) */
	int m_TypeKnotV;

	/*! \brief ������ ��� ���� */
	int m_TypeRational;
};
