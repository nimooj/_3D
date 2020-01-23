#pragma once
#include "resource.h"

class PCreator;
// PCreatePlyDlg 대화 상자입니다.

class AFX_EXT_CLASS PCreatePlyDlg : public CDialog	
{
	DECLARE_DYNAMIC(PCreatePlyDlg)

public:
	PCreatePlyDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~PCreatePlyDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CREATE_PLY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

public:
	virtual BOOL OnInitDialog();

	// 메시지 처리함수
	DECLARE_MESSAGE_MAP()

public:
	// 데이터 멤버
	/* PCreator 객체에 대한 포인터 */
	PCreator *pCreator;

	/* 생성할 폴리곤의 이름*/
	CString m_PlyName;
};
