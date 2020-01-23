#pragma once

#include "..\GView.h"

// GDepthViewDlg 대화 상자입니다.
class GDepthViewDlg : public CDialog
{
	DECLARE_DYNAMIC(GDepthViewDlg)

public:
	GDepthViewDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~GDepthViewDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DEPTH_VIEW_DLG };

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()

public:
	// 데이터 멤버
	GView *m_pView;	

	// 렌더링 할 객체의 리스트
	std::vector<GMesh *> m_MeshList;
};
