#pragma once
#include "resource.h"

class PCreator;
// PCreatePlyDlg ��ȭ �����Դϴ�.

class AFX_EXT_CLASS PCreatePlyDlg : public CDialog	
{
	DECLARE_DYNAMIC(PCreatePlyDlg)

public:
	PCreatePlyDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~PCreatePlyDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CREATE_PLY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

public:
	virtual BOOL OnInitDialog();

	// �޽��� ó���Լ�
	DECLARE_MESSAGE_MAP()

public:
	// ������ ���
	/* PCreator ��ü�� ���� ������ */
	PCreator *pCreator;

	/* ������ �������� �̸�*/
	CString m_PlyName;
};
