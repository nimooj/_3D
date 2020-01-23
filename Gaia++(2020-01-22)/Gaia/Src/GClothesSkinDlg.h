#pragma once

// GClothesSkinDlg ��ȭ �����Դϴ�.
class GaiaDoc;

class GClothesSkinDlg : public CDialog
{
	DECLARE_DYNAMIC(GClothesSkinDlg)

public:
	GClothesSkinDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~GClothesSkinDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CLOTHES_SKIN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	GaiaDoc *GetDocument();
	afx_msg void OnBnClickedBindByMinDist();
	afx_msg void OnBnClickedBindBySwpPln();
	afx_msg void OnBnClickedShowBindRefPlnLs();
	afx_msg void OnBnClickedShowBindRefPlnRs();
	afx_msg void OnBnClickedHideBindRefPln();

	// ���� ���õ� ���ε� ���� ���
	GBindRefPln *m_pBindRefPln;
	int m_BindRefPlnIdx;
	virtual BOOL OnInitDialog();
};
