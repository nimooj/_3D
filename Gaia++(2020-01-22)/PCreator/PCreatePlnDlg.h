#pragma once
#include "resource.h"

class PCreator;

// PCreatePlnDlg 대화 상자입니다.

class PCreatePlnDlg : public CDialog
{
	DECLARE_DYNAMIC(PCreatePlnDlg)

public:
	PCreatePlnDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~PCreatePlnDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CREATE_PLN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	// 메시지 처리함수
	DECLARE_MESSAGE_MAP()

public:
	/* PCreator 객체에 대한 포인터 */
	PCreator *pCreator;

	/* 생성할 평면의 이름*/
	CString m_Name;
};
