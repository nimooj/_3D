// GTabCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Gaia.h"
#include "GTabCtrl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CBaseTabCtrl::CBaseTabCtrl(int nType)
{
	m_crBack = (COLORREF)-1; // use default color
	m_nDrawType = BTC_NONE;

	EnableDraw(nType);
}

CBaseTabCtrl::~CBaseTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CBaseTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CBaseTabCtrl)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBaseTabCtrl message handlers

void CBaseTabCtrl::SetBkgndColor(COLORREF color)
{
	// set new color
	m_crBack = color;

	// redraw
	if (GetSafeHwnd())
		Invalidate();
}

BOOL CBaseTabCtrl::EnableDraw(int nType)
{
	ASSERT (nType >= BTC_NONE && nType <= BTC_ALL);

	if (nType < BTC_NONE || nType > BTC_ALL)
		return FALSE;

	m_nDrawType = nType;

	if (GetSafeHwnd())
	{
		if (m_nDrawType != BTC_NONE) 
			ModifyStyle(0, TCS_OWNERDRAWFIXED);
		else
			ModifyStyle(TCS_OWNERDRAWFIXED, 0);

		Invalidate();
	}

	return TRUE;
}

void CBaseTabCtrl::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
	ASSERT (0);
}

void CBaseTabCtrl::DrawItemBorder(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
	ASSERT (0);
}

void CBaseTabCtrl::DrawMainBorder(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
	ASSERT (0);
}

COLORREF CBaseTabCtrl::GetTabColor(BOOL /*bSelected*/)
{
	return ::GetSysColor(COLOR_3DFACE);
}

COLORREF CBaseTabCtrl::GetTabTextColor(BOOL /*bSelected*/)
{
	return ::GetSysColor(COLOR_WINDOWTEXT);
}

void CBaseTabCtrl::PreSubclassWindow() 
{
	CTabCtrl::PreSubclassWindow();

	if (m_nDrawType != BTC_NONE) 
		ModifyStyle(0, TCS_OWNERDRAWFIXED);
}

BOOL CBaseTabCtrl::OnEraseBkgnd(CDC* pDC) 
{
	CRect rClient, rTab, rTotalTab, rBkgnd, rEdge;
	COLORREF crBack;
	int nTab, nTabHeight = 0;

	CTabCtrl::OnEraseBkgnd(pDC);

	// calc total tab width
	GetClientRect(rClient);
	nTab = GetItemCount();
	rTotalTab.SetRectEmpty();

	while (nTab--)
	{
		GetItemRect(nTab, rTab);
		rTotalTab.UnionRect(rTab, rTotalTab);
	}

	nTabHeight = rTotalTab.Height();

	// add a bit
	rTotalTab.InflateRect(2, 3);
	rEdge = rTotalTab;

	// then if background color is set, paint the visible background
	// area of the tabs in the bkgnd color
	// note: the mfc code for drawing the tabs makes all sorts of assumptions
	// about the background color of the tab control being the same as the page
	// color - in some places the background color shows thru' the pages!!
	// so we must only paint the background color where we need to, which is that
	// portion of the tab area not excluded by the tabs themselves
	crBack = (m_crBack == -1) ? ::GetSysColor(COLOR_3DFACE) : m_crBack;

	// full width of tab ctrl above top of tabs
	rBkgnd = rClient;
	rBkgnd.bottom = rTotalTab.top + 3;
	pDC->SetBkColor(crBack);
	pDC->ExtTextOut(rBkgnd.left, rBkgnd.top, ETO_CLIPPED | ETO_OPAQUE, rBkgnd, "", NULL);

	// width of tab ctrl visible bkgnd including bottom pixel of tabs to left of tabs
	rBkgnd = rClient;
	rBkgnd.right = 2;
	rBkgnd.bottom = rBkgnd.top + (nTabHeight + 2);
	pDC->ExtTextOut(rBkgnd.left, rBkgnd.top, ETO_CLIPPED | ETO_OPAQUE, rBkgnd, "", NULL);

	// to right of tabs
	rBkgnd = rClient;
	rBkgnd.left += rTotalTab.Width() - 2;
	rBkgnd.bottom = rBkgnd.top + (nTabHeight + 2);
	pDC->ExtTextOut(rBkgnd.left, rBkgnd.top, ETO_CLIPPED | ETO_OPAQUE, rBkgnd, "", NULL);

	return TRUE;
}

void CBaseTabCtrl::OnPaint() 
{
	if (m_nDrawType == BTC_NONE)
	{
		Default();
	}
	else if (m_nDrawType == BTC_TABS)
	{
		ASSERT (GetStyle() & TCS_OWNERDRAWFIXED);
		Default();
	}
	else // all
	{
		CPaintDC dc(this); // device context for painting

		// prepare dc
		dc.SelectObject(GetFont());

		DRAWITEMSTRUCT dis;
		dis.CtlType = ODT_TAB;
		dis.CtlID = GetDlgCtrlID();
		dis.hwndItem = GetSafeHwnd();
		dis.hDC = dc.GetSafeHdc();
		dis.itemAction = ODA_DRAWENTIRE;

		// draw the rest of the border
		CRect rClient, rPage;
		GetClientRect(&dis.rcItem);
		rPage = dis.rcItem;
		AdjustRect(FALSE, rPage);
		dis.rcItem.top = rPage.top - 2;

		DrawMainBorder(&dis);

		// paint the tabs first and then the borders
		int nTab = GetItemCount();
		int nSel = GetCurSel();

		if (!nTab) // no pages added
			return;

		while (nTab--)
		{
			if (nTab != nSel)
			{
				dis.itemID = nTab;
				dis.itemState = 0;

				VERIFY(GetItemRect(nTab, &dis.rcItem));

				dis.rcItem.bottom -= 2;
				DrawItem(&dis);
				DrawItemBorder(&dis);
			}
		}

		// now selected tab
		dis.itemID = nSel;
		dis.itemState = ODS_SELECTED;

		VERIFY(GetItemRect(nSel, &dis.rcItem));

		dis.rcItem.bottom += 2;
		dis.rcItem.top -= 2;
		DrawItem(&dis);
		DrawItemBorder(&dis);
	}
}


DWORD GTabCtrl::s_dwCustomLook = 0;
enum { PADDING = 3, EDGE = 20};

//////////////////////////////////////////////////////////////////////////////
// helpers
COLORREF Darker(COLORREF crBase, float fFactor)
{
	ASSERT (fFactor < 1.0f && fFactor > 0.0f);

	fFactor = min(fFactor, 1.0f);
	fFactor = max(fFactor, 0.0f);

	BYTE bRed, bBlue, bGreen;
	BYTE bRedShadow, bBlueShadow, bGreenShadow;

	bRed = GetRValue(crBase);
	bBlue = GetBValue(crBase);
	bGreen = GetGValue(crBase);

	bRedShadow = (BYTE)(bRed * fFactor);
	bBlueShadow = (BYTE)(bBlue * fFactor);
	bGreenShadow = (BYTE)(bGreen * fFactor);

	return RGB(bRedShadow, bGreenShadow, bBlueShadow);
}

COLORREF Lighter(COLORREF crBase, float fFactor) 
{
	ASSERT (fFactor > 1.0f);

	fFactor = max(fFactor, 1.0f);

	BYTE bRed, bBlue, bGreen;
	BYTE bRedHilite, bBlueHilite, bGreenHilite;

	bRed = GetRValue(crBase);
	bBlue = GetBValue(crBase);
	bGreen = GetGValue(crBase);

	bRedHilite = (BYTE)min((int)(bRed * fFactor), 255);
	bBlueHilite = (BYTE)min((int)(bBlue * fFactor), 255);
	bGreenHilite = (BYTE)min((int)(bGreen * fFactor), 255);

	return RGB(bRedHilite, bGreenHilite, bBlueHilite);
}

CSize FormatText(CString& sText, CDC* pDC, int nWidth)
{
	CRect rect(0, 0, nWidth, 20);
	UINT uFlags = DT_CALCRECT | DT_SINGLELINE | DT_MODIFYSTRING | DT_END_ELLIPSIS;

	::DrawText(pDC->GetSafeHdc(), sText.GetBuffer(sText.GetLength() + 4), -1, rect, uFlags);
	sText.ReleaseBuffer();

	return pDC->GetTextExtent(sText);
}

////////////////////////////////////////////////////////////////////////////////////////

GTabCtrl::GTabCtrl()
{
}

GTabCtrl::~GTabCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// GTabCtrl message handlers

void GTabCtrl::DrawItem(LPDRAWITEMSTRUCT lpdis)
{
	TC_ITEM     tci;
	CDC* pDC = CDC::FromHandle(lpdis->hDC);
	HIMAGELIST hilTabs = (HIMAGELIST)TabCtrl_GetImageList(GetSafeHwnd());

	BOOL bSelected = (lpdis->itemID == (UINT)GetCurSel());
	BOOL bColor = (s_dwCustomLook & ETC_COLOR);

	CRect rItem(lpdis->rcItem);

	if (bSelected)
		rItem.bottom -= 1;
	else
		rItem.bottom += 2;

	// tab
	// blend from back color to COLOR_3DFACE if 16 bit mode or better
	COLORREF crFrom = GetTabColor(bSelected);

	if (s_dwCustomLook & ETC_GRADIENT && pDC->GetDeviceCaps(BITSPIXEL) >= 16)
	{
		COLORREF crTo = bSelected ? ::GetSysColor(COLOR_3DFACE) : Darker(!bColor || m_crBack == -1 ? ::GetSysColor(COLOR_3DFACE) : m_crBack, 0.7f);

		int nROrg = GetRValue(crFrom);
		int nGOrg = GetGValue(crFrom);
		int nBOrg = GetBValue(crFrom);
		int nRDiff = GetRValue(crTo) - nROrg;
		int nGDiff = GetGValue(crTo) - nGOrg;
		int nBDiff = GetBValue(crTo) - nBOrg;

		int nHeight = rItem.Height();

		for (int nLine = 0; nLine < nHeight; nLine += 2)
		{
			int nRed = nROrg + (nLine * nRDiff) / nHeight;
			int nGreen = nGOrg + (nLine * nGDiff) / nHeight;
			int nBlue = nBOrg + (nLine * nBDiff) / nHeight;

			pDC->FillSolidRect(CRect(rItem.left, rItem.top + nLine, rItem.right, rItem.top + nLine + 2), 
				RGB(nRed, nGreen, nBlue));
		}
	}
	else // simple solid fill
		pDC->FillSolidRect(rItem, crFrom);

	// text & icon
	rItem.left += PADDING;
	rItem.top += PADDING + (bSelected ? 1 : 0);

	pDC->SetBkMode(TRANSPARENT);

	CString sTemp;
	tci.mask        = TCIF_TEXT | TCIF_IMAGE;
	tci.pszText     = sTemp.GetBuffer(100);
	tci.cchTextMax  = 99;
	GetItem(lpdis->itemID, &tci);
	sTemp.ReleaseBuffer();

	// icon
	if (hilTabs)
	{
		ImageList_Draw(hilTabs, tci.iImage, *pDC, rItem.left, rItem.top, ILD_TRANSPARENT);
		rItem.left += 16 + PADDING;
	}

	// text
	rItem.right -= PADDING;
	FormatText(sTemp, pDC, rItem.Width());

	pDC->SetTextColor(GetTabTextColor(bSelected));
	pDC->DrawText(sTemp, rItem, DT_NOPREFIX | DT_CENTER);
}

void GTabCtrl::DrawItemBorder(LPDRAWITEMSTRUCT lpdis)
{
	ASSERT (s_dwCustomLook & ETC_FLAT);

	BOOL bSelected = (lpdis->itemID == (UINT)GetCurSel());
	BOOL bBackTabs = (s_dwCustomLook & ETC_BACKTABS);

	CRect rItem(lpdis->rcItem);
	CDC* pDC = CDC::FromHandle(lpdis->hDC); 

	COLORREF crTab = GetTabColor(bSelected);
	COLORREF crHighlight = Lighter(crTab, 1.5f);
	COLORREF crShadow = Darker(crTab, 0.75f);

	if (bSelected || bBackTabs)
	{
		rItem.bottom += bSelected ? -1 : 1;

		// edges
		pDC->FillSolidRect(CRect(rItem.left, rItem.top, rItem.left + 1, rItem.bottom), crHighlight);
		pDC->FillSolidRect(CRect(rItem.left, rItem.top, rItem.right, rItem.top + 1), crHighlight);
		pDC->FillSolidRect(CRect(rItem.right - 1, rItem.top, rItem.right, rItem.bottom), crShadow);
	}
	else // draw simple dividers
	{
		pDC->FillSolidRect(CRect(rItem.left - 1, rItem.top, rItem.left, rItem.bottom), crShadow);
		pDC->FillSolidRect(CRect(rItem.right - 1, rItem.top, rItem.right, rItem.bottom), crShadow);
	}
}

void GTabCtrl::DrawMainBorder(LPDRAWITEMSTRUCT lpdis)
{
	CRect rBorder(lpdis->rcItem);
	CDC* pDC = CDC::FromHandle(lpdis->hDC); 

	COLORREF crTab = GetTabColor();
	COLORREF crHighlight = Lighter(crTab, 1.5f);
	COLORREF crShadow = Darker(crTab, 0.75f);

	pDC->Draw3dRect(rBorder, crHighlight, crShadow);
}

COLORREF GTabCtrl::GetTabColor(BOOL bSelected)
{
	return RGB(theScene().UIBkgColor[0], theScene().UIBkgColor[1], theScene().UIBkgColor[2]);

	BOOL bColor = (s_dwCustomLook & ETC_COLOR);
	BOOL bHiliteSel = (s_dwCustomLook & ETC_SELECTION);
	BOOL bBackTabs = (s_dwCustomLook & ETC_BACKTABS);
	BOOL bFlat = (s_dwCustomLook & ETC_FLAT);

	if (bSelected && bHiliteSel)
	{
		if (bColor)
			return Lighter((m_crBack == -1) ? ::GetSysColor(COLOR_3DFACE) : m_crBack, 1.4f);
		else
			return Lighter(::GetSysColor(COLOR_3DFACE), 1.4f);
	}
	else if (!bSelected)
	{
		if (bBackTabs || !bFlat)
		{
			if (bColor)
				return Darker((m_crBack == -1) ? ::GetSysColor(COLOR_3DFACE) : m_crBack, 0.9f);
			else
				return Darker(::GetSysColor(COLOR_3DFACE), 0.9f);
		}
		else
			return (m_crBack == -1) ? ::GetSysColor(COLOR_3DFACE) : m_crBack;
	}

	// else
	return ::GetSysColor(COLOR_3DFACE);	
}

COLORREF GTabCtrl::GetTabTextColor(BOOL bSelected)
{
	if (bSelected)
		return RGB(255, 255, 255);
	else
		return RGB(180, 180, 180);

	BOOL bColor = (s_dwCustomLook & ETC_COLOR);
	BOOL bFlat = (s_dwCustomLook & ETC_FLAT);

	if (bSelected)
	{
		return ::GetSysColor(COLOR_WINDOWTEXT);
	}
	else
	{
		if (bColor || bFlat)
			return Darker((m_crBack == -1) ? ::GetSysColor(COLOR_3DFACE) : m_crBack, 0.5f);
		else
			return Darker(::GetSysColor(COLOR_3DFACE), 0.5f);
	}

	// else
	return Darker(::GetSysColor(COLOR_3DFACE), 0.5f);
}

void GTabCtrl::EnableCustomLook(BOOL bEnable, DWORD dwStyle)
{
	if (!bEnable)
		dwStyle = 0;

	s_dwCustomLook = dwStyle;
}

void GTabCtrl::PreSubclassWindow() 
{
	CBaseTabCtrl::PreSubclassWindow();

	if (s_dwCustomLook)
		ModifyStyle(0, TCS_OWNERDRAWFIXED);
}
