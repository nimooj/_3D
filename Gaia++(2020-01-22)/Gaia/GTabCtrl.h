#pragma once

enum
{
	BTC_NONE = 0,
	BTC_TABS = 1,
	BTC_ALL = 2,
};

class CBaseTabCtrl : public CTabCtrl
{
	// Construction
public:
	CBaseTabCtrl(int nType = BTC_NONE);
	BOOL EnableDraw(int nType = BTC_ALL);
	void SetBkgndColor(COLORREF color);

protected:
	COLORREF m_crBack;
	int m_nDrawType;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseTabCtrl)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CBaseTabCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBaseTabCtrl)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	virtual void DrawMainBorder(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DrawItemBorder(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual COLORREF GetTabColor(BOOL bSelected = FALSE);
	virtual COLORREF GetTabTextColor(BOOL bSelected = FALSE);
	virtual void PreSubclassWindow();
};


// custom look
enum
{
	ETC_FLAT = 1, 
	ETC_COLOR = 2, // draw tabs with color
	ETC_SELECTION = 4, // highlight the selected tab
	ETC_GRADIENT = 8, // draw colors with a gradient
	ETC_BACKTABS = 16,
};

class GTabCtrl : public CBaseTabCtrl
{
	// Construction
public:
	GTabCtrl();

	static void EnableCustomLook(BOOL bEnable = TRUE, DWORD dwStyle = ETC_FLAT | ETC_COLOR);

	// Attributes
public:

protected:
	static DWORD s_dwCustomLook;

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GTabCtrl)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~GTabCtrl();

	// Generated message map functions
protected:
	virtual void DrawMainBorder(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DrawItemBorder(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual COLORREF GetTabColor(BOOL bSelected = FALSE);
	virtual COLORREF GetTabTextColor(BOOL bSelected = FALSE);

};


