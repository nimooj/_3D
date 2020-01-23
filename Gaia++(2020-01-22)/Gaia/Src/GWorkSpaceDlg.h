#pragma once
#include "afxcmn.h"
#include "..\resource.h"
#include "..\GTabCtrl.h"

// GWorkSpaceDlg ��ȭ �����Դϴ�.
class GWorkSpaceDlg : public CDialog
{
	DECLARE_DYNAMIC(GWorkSpaceDlg)
public:
	// ������ �� �Ҹ���
	GWorkSpaceDlg(CWnd* pParent = NULL);
	virtual ~GWorkSpaceDlg();
	enum { IDD = IDD_WORKSPACE_DLG };

public:
	// �����Լ� ������
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// �Ϲݸ�� �Լ�
	void InsertTabDlg(CDialog *pDlg, CString TabName, bool bSelect);
	void DeleteTabDlg();
	void SetCurrTab(int iTabIdx);
	void RecalcLayOut();
	void UpdateWorkSpace();
	void UpdateSelect();
	
	// �޽��� ó���Լ�
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTcnSelchangeWorkspaceTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnExitCreator(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateSelect(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

public:
	// ������ ���
	/*! \brief �۾����� ����� �� ��Ʈ�� */
	GTabCtrl m_cWorkSpaceTab;

	/*! \brief �۾������� �������� �߰�/�����Ǵ� ��ȭ������ ����Ʈ */
	std::vector<CDialog *> m_pTabDlgList;	
};
