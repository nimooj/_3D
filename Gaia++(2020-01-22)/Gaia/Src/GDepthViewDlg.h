#pragma once

#include "..\GView.h"

// GDepthViewDlg ��ȭ �����Դϴ�.
class GDepthViewDlg : public CDialog
{
	DECLARE_DYNAMIC(GDepthViewDlg)

public:
	GDepthViewDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~GDepthViewDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DEPTH_VIEW_DLG };

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	DECLARE_MESSAGE_MAP()

public:
	// ������ ���
	GView *m_pView;	

	// ������ �� ��ü�� ����Ʈ
	std::vector<GMesh *> m_MeshList;
};
