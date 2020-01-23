#ifndef _GTOOLBAR_H
#define _GTOOLBAR_H

#include <afxtempl.h>

class GToolBar : public CToolBar
{
public:
	// 생성자 및 소멸자
	GToolBar();
	virtual ~GToolBar();

public:
	// 멤버함수
	BOOL LoadTrueColorToolBar(int  nBtnWidth, UINT uToolBar, UINT uToolBarHot = 0, UINT uToolBarDisabled = 0);
	BOOL SetTrueColorToolBar(UINT uToolBarType, UINT uToolBar, int  nBtnWidth);
	void AddDropDownButton(CWnd* pParent, UINT uButtonID, UINT uMenuID);
		
	// 메시지 처리함수
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNcPaint();

public:
	// 데이터 멤버
	BOOL	m_bDropDown;
	struct stDropDownInfo 
	{
	public:
		UINT  uButtonID;
		UINT  uMenuID;
		CWnd* pParent;
	};
	CArray <stDropDownInfo, stDropDownInfo&> m_lstDropDownButton;
};

#else
#endif // _GTOOLBAR_H
