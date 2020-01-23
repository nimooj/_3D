#pragma once

// GClothesSkinDlg 대화 상자입니다.
class GaiaDoc;

class GClothesSkinDlg : public CDialog
{
	DECLARE_DYNAMIC(GClothesSkinDlg)

public:
	GClothesSkinDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~GClothesSkinDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CLOTHES_SKIN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	GaiaDoc *GetDocument();
	afx_msg void OnBnClickedBindByMinDist();
	afx_msg void OnBnClickedBindBySwpPln();
	afx_msg void OnBnClickedShowBindRefPlnLs();
	afx_msg void OnBnClickedShowBindRefPlnRs();
	afx_msg void OnBnClickedHideBindRefPln();

	// 현재 선택된 바인딩 참조 평면
	GBindRefPln *m_pBindRefPln;
	int m_BindRefPlnIdx;
	virtual BOOL OnInitDialog();
};
