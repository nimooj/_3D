#include "stdafx.h"
#include "GToolBar.h"

/////////////////////////////////////////////////////////////////////////////
// GToolBar
GToolBar::GToolBar()
{
	m_bDropDown = FALSE;
}

GToolBar::~GToolBar()
{
}


//ON_NOTIFY_REFLECT(TBN_DROPDOWN, OnToolbarDropDown)
BEGIN_MESSAGE_MAP(GToolBar, CToolBar)
	//{{AFX_MSG_MAP(GToolBar)
	ON_WM_NCPAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GToolBar message handlers
BOOL GToolBar::LoadTrueColorToolBar(int nBtnWidth, UINT uToolBar, UINT uToolBarHot, UINT uToolBarDisabled)
{
	if (!SetTrueColorToolBar(TB_SETIMAGELIST, uToolBar, nBtnWidth))
		return FALSE;
	
	if (uToolBarHot) 
	{
		if (!SetTrueColorToolBar(TB_SETHOTIMAGELIST, uToolBarHot, nBtnWidth))
			return FALSE;
	}

	if (uToolBarDisabled) 
	{
		if (!SetTrueColorToolBar(TB_SETDISABLEDIMAGELIST, uToolBarDisabled, nBtnWidth))
			return FALSE;
	}
	return TRUE;
}


BOOL GToolBar::SetTrueColorToolBar(UINT uToolBarType, UINT uToolBar, int  nBtnWidth)
{
	CImageList	cImageList;
	CBitmap		cBitmap;
	BITMAP		bmBitmap;
	
	if (!cBitmap.Attach(LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(uToolBar),
								  IMAGE_BITMAP, 0, 0,
								  LR_DEFAULTSIZE|LR_CREATEDIBSECTION)) ||
	    !cBitmap.GetBitmap(&bmBitmap))
		return FALSE;

	CSize		cSize(bmBitmap.bmWidth, bmBitmap.bmHeight); 
	int			nNbBtn	= cSize.cx/nBtnWidth;
	RGBTRIPLE*	rgb		= (RGBTRIPLE*)(bmBitmap.bmBits);
	COLORREF	rgbMask	= RGB(rgb[0].rgbtRed, rgb[0].rgbtGreen, rgb[0].rgbtBlue);
	
	if (!cImageList.Create(nBtnWidth, cSize.cy, ILC_COLOR24|ILC_MASK, nNbBtn, 0))
		return FALSE;
	
	if (cImageList.Add(&cBitmap, rgbMask) == -1)
		return FALSE;

	SendMessage(uToolBarType, 0, (LPARAM)cImageList.m_hImageList);
	cImageList.Detach(); 
	cBitmap.Detach();
	
	return TRUE;
}

void GToolBar::AddDropDownButton(CWnd* pParent, UINT uButtonID, UINT uMenuID)
{
	if (!m_bDropDown) {
		GetToolBarCtrl().SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_EX_DRAWDDARROWS);
		m_bDropDown = TRUE;
	}

	SetButtonStyle(CommandToIndex(uButtonID), TBSTYLE_DROPDOWN);

	stDropDownInfo DropDownInfo;
	DropDownInfo.pParent	= pParent;
	DropDownInfo.uButtonID	= uButtonID;
	DropDownInfo.uMenuID	= uMenuID;
	m_lstDropDownButton.Add(DropDownInfo);
}

/*
//void GToolBar::OnToolbarDropDown(NMTOOLBAR* pnmtb, LRESULT *plr)
void GToolBar::OnToolbarDropDown(NMHDR* pnmtb, LRESULT *plr)
{
	for (int i = 0; i < m_lstDropDownButton.GetSize(); i++) {
		
		stDropDownInfo DropDownInfo = m_lstDropDownButton.GetAt(i);

		if (DropDownInfo.uButtonID == UINT(pnmtb->iItem)) {

			CMenu menu;
			menu.LoadMenu(DropDownInfo.uMenuID);
			CMenu* pPopup = menu.GetSubMenu(0);
			
			CRect rc;
			SendMessage(TB_GETRECT, (WPARAM)pnmtb->iItem, (LPARAM)&rc);
			ClientToScreen(&rc);
			
			pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,
				                   rc.left, rc.bottom, DropDownInfo.pParent, &rc);
			break;
		}
	}
}
*/

void GToolBar::OnNcPaint()
{
	// get window DC that is clipped to the non-client area
    CWindowDC dc(this);
    CRect rectClient;
    GetClientRect(rectClient);
    CRect rectWindow;
    GetWindowRect(rectWindow);
    ScreenToClient(rectWindow);
    rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
    dc.ExcludeClipRect(rectClient);

    // draw borders in non-client area
    rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
    DrawBorders(&dc, rectWindow);
 
    // erase parts not drawn
    dc.IntersectClipRect(rectWindow);
    SendMessage(WM_ERASEBKGND, (WPARAM)dc.m_hDC);

    // draw gripper in non-client area
	dc.FillSolidRect(&rectWindow, GetSysColor(COLOR_3DFACE));
	//dc.FillSolidRect(&rectWindow, RGB(68, 68, 68));
    DrawGripper(&dc, rectWindow);
}
