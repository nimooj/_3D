#pragma once
#include "afxwin.h"


// GCurvatureDlg ��ȭ �����Դϴ�.

class GCurvatureDlg : public CDialog
{
	DECLARE_DYNAMIC(GCurvatureDlg)

public:
	GCurvatureDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~GCurvatureDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CURVATURE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	/*! \brief ����� ������ ��� �޽� */
	GMesh *m_pMesh;	

	/*! \brief �޽� ������ ���콺, ���, �ּ�, �ִ� ����� ������ �����ϴ� �迭 */
	std::vector<double> m_kG;
	std::vector<double> m_kH;
	std::vector<double> m_kMin;
	std::vector<double> m_kMax;

	/*! \brief ����� ����ȭ ���� */
	double m_MinVal;
	double m_MaxVal;

	CComboBox m_cCurvatureType;

	// �̺�Ʈ ó�� �Լ���
	afx_msg void OnEnKillfocusMinVal();
	afx_msg void OnEnKillfocusMaxVal();
	afx_msg void OnCbnCloseupCurvatureType();

	// ����� ����Լ�
	void UpdateCurvatureRange();	
};
