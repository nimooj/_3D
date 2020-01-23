#pragma once
#include "afxcmn.h"
#include "..\resource.h"
#include "..\GTabCtrl.h"

// GWorkSpaceDlg 대화 상자입니다.
class GWorkSpaceDlg : public CDialog
{
	DECLARE_DYNAMIC(GWorkSpaceDlg)
public:
	// 생성자 및 소멸자
	GWorkSpaceDlg(CWnd* pParent = NULL);
	virtual ~GWorkSpaceDlg();
	enum { IDD = IDD_WORKSPACE_DLG };

public:
	// 가상함수 재정의
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// 일반멤버 함수
	void InsertTabDlg(CDialog *pDlg, CString TabName, bool bSelect);
	void DeleteTabDlg();
	void SetCurrTab(int iTabIdx);
	void RecalcLayOut();
	void UpdateWorkSpace();
	void UpdateSelect();
	
	// 메시지 처리함수
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTcnSelchangeWorkspaceTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnExitCreator(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateSelect(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

public:
	// 데이터 멤버
	/*! \brief 작업공간 상단의 탭 컨트롤 */
	GTabCtrl m_cWorkSpaceTab;

	/*! \brief 작업공간에 동적으로 추가/삭제되는 대화상자의 리스트 */
	std::vector<CDialog *> m_pTabDlgList;	
};
