#ifndef _GTOOLBAR_H
#define _GTOOLBAR_H

#include <afxtempl.h>

class GToolBar : public CToolBar
{
public:
	// ������ �� �Ҹ���
	GToolBar();
	virtual ~GToolBar();

public:
	// ����Լ�
	BOOL LoadTrueColorToolBar(int  nBtnWidth, UINT uToolBar, UINT uToolBarHot = 0, UINT uToolBarDisabled = 0);
	BOOL SetTrueColorToolBar(UINT uToolBarType, UINT uToolBar, int  nBtnWidth);
	void AddDropDownButton(CWnd* pParent, UINT uButtonID, UINT uMenuID);
		
	// �޽��� ó���Լ�
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNcPaint();

public:
	// ������ ���
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
