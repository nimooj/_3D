#pragma once
#include "resource.h"

class PCreator;

// PCreatePlnDlg ��ȭ �����Դϴ�.

class PCreatePlnDlg : public CDialog
{
	DECLARE_DYNAMIC(PCreatePlnDlg)

public:
	PCreatePlnDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~PCreatePlnDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CREATE_PLN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	// �޽��� ó���Լ�
	DECLARE_MESSAGE_MAP()

public:
	/* PCreator ��ü�� ���� ������ */
	PCreator *pCreator;

	/* ������ ����� �̸�*/
	CString m_Name;
};
