#pragma once
#include "resource.h"

class PCreator;

// PCreateCrvDlg ��ȭ �����Դϴ�.
class AFX_EXT_CLASS PCreateCrvDlg : public CDialog
{
	DECLARE_DYNAMIC(PCreateCrvDlg)
public:
	// ������ �� �Ҹ���
	PCreateCrvDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~PCreateCrvDlg();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CREATE_CRV_DLG };

public:
	// ���� �Լ� ������
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()
public:
	// �޽��� ó���Լ�
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
	// ������ ���
	/*! \brief  PCreator ��ü�� ���� ������ */
	PCreator *pCreator;

	/*! \brief  ������ ��� �̸� */
	CString m_Name;

	/*! \brief  ������ ��� ���� */
	int m_Degree;

	/*! \brief  � ���� ��� (0: ������, 1: ������) */
	int m_TypeCreate;

	/*! \brief  ������ ��� �� ���� Ÿ�� (0: clamped, 1: unclamped, 2: periodic) */
	int m_TypeKnot;

	/*! \brief ������ � Ÿ�� (0: ������ �, 1: ���׽� �) */
	int m_TypeRational;	

	/*! \brief ��� ���� ���� ä�� ���� */
	BOOL m_bFillCrv;

	/*! \brief ����� ���� � ���� ���� */
	BOOL m_bViewPlnCrv;
};
