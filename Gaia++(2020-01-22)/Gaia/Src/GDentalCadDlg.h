#pragma once
#include "afxcmn.h"

// GDentalCadDlg ��ȭ �����Դϴ�.

class GDentalCadDlg : public CDialog
{
	DECLARE_DYNAMIC(GDentalCadDlg)

public:
	GDentalCadDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~GDentalCadDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DENTAL_CAD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedImportUpperJaw();
	afx_msg void OnBnClickedImportLowerJaw();
	afx_msg void OnBnClickedImportAbutment();
	afx_msg void OnBnClickedCreateSwpPlane();
	afx_msg void OnBnClickedCreateSplint();
	afx_msg void OnBnClickedShowUpperJaw();
	afx_msg void OnBnClickedShowLowerJaw();
	afx_msg void OnBnClickedShowKeyPlane();
	afx_msg void OnBnClickedShowSwpPlane();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnKillfocusSplintOffset();

protected:
	void GroupFacesByPlane(GMesh *pMesh, GNurbsMot *pSwpPlanes, int NumGrp, std::vector<std::vector<int>> &FaceIdxList);
	void CreateUpperSplint(GMesh *pMesh);
	void CreateLowerSplint(GMesh *pMesh);

	// ������ ���
public:
	/* \brief */
	// ũ��� �������� ���� ������
	GMesh *m_pUpperJaw;
	GMesh *m_pLowerJaw;
	GMesh *m_pAbutment;

	// ���ø�Ʈ ������
	GMesh *m_pSplintInner;
	GMesh *m_pSplintOuter;
	GMesh *m_pSplintSide0;
	GMesh *m_pSplintSide1;
	GMesh *m_pSplintSide2;
	GMesh *m_pSplintSide3;
	std::vector<GMesh *> m_KeyPlaneList;

	// ���ø�Ʈ ������ ���� ��� ����
	GNurbsMot *m_pMovingPlanes;	

	CSliderCtrl m_cSplintWidth;
	CSliderCtrl m_cSplintWidthByNormal;

	int m_SplintWidth;
	int m_SplintWidthByNormal;
	BOOL m_bShowUpperJaw;
	BOOL m_bShowLowerJaw;
	BOOL m_bShowKeyPlane;
	BOOL m_bShowSwpPlane;
	double m_SplintOffset;
};
