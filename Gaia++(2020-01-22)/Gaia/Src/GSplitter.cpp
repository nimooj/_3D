#include "stdafx.h"
#include "..\resource.h"
#include "GSplitter.h"
#include "../GaiaDoc.h"
#include "../GaiaView.h"


IMPLEMENT_DYNCREATE(GSplitter, CSplitterWnd)


BEGIN_MESSAGE_MAP(GSplitter, CSplitterWnd)
	//{{AFX_MSG_MAP(GSplitter)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


#define FOCUS_HILIGHT_COLOR_ULO		RGB(75, 25, 180)
#define FOCUS_HILIGHT_COLOR_LRO		RGB(5, 25, 245)
#define FOCUS_HILIGHT_COLOR_ULI		RGB(95, 75, 145)
#define FOCUS_HILIGHT_COLOR_LRI		RGB(65, 40, 220)
#define FOCUS_HILIGHT_SHOW			TRUE
#define SPLITTER_CX					4
#define SPLITTER_CY					4
#define SPLITTER_GAPX				4
#define SPLITTER_GAPY				4

GSplitter::GSplitter()
{
	m_cRow = 0;	
	m_cCol = 0; 	
	m_nHidedCol = -1;
	m_nHidedRow = -1;
		
	m_cxSplitter = SPLITTER_CX;	
	m_cySplitter = SPLITTER_CY;
	m_cxSplitterGap = SPLITTER_GAPX;	
	m_cySplitterGap = SPLITTER_GAPY;
}
GSplitter::~GSplitter()
{
}
void GSplitter::RefreshSplitBars(void)
{
	CRect	rectInside;	
	GetInsideRect(rectInside);
	DrawAllSplitBars(NULL, rectInside.right, rectInside.bottom);
}

void GSplitter::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg)
{
	if((FOCUS_HILIGHT_SHOW) && ((GetRowCount()>1) || 
	   (GetColumnCount()>1)) && (nType == splitBorder))
	{		
		int	cRow, cCol, pRow = 0, pCol = 0;

		GetActivePane(&cRow, &cCol);
		if(rectArg.top)	pRow = 1;		
		if(rectArg.left) pCol = 1;
		if((cRow == pRow) && (cCol == pCol))		
		{
			if (pDC == NULL)			
			{
				RedrawWindow(rectArg, NULL, RDW_INVALIDATE|RDW_NOCHILDREN);				
				return; 			
			}
			ASSERT_VALID(pDC);			
			CRect	rect = rectArg;
			pDC->Draw3dRect(rect, FOCUS_HILIGHT_COLOR_ULO, FOCUS_HILIGHT_COLOR_LRO);
			rect.InflateRect(-GetSystemMetrics(SM_CXBORDER), -GetSystemMetrics(SM_CYBORDER));
			pDC->Draw3dRect(rect, FOCUS_HILIGHT_COLOR_ULI, FOCUS_HILIGHT_COLOR_LRI);
			return; 		
		}	
	}		
	CSplitterWnd::OnDrawSplitter(pDC,nType,rectArg); 
}

BOOL GSplitter::CreateView( int row, int col, CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext )
{
	CSplitterWnd::CreateView(row, col, pViewClass, sizeInit, pContext);
	GaiaView *pView = (GaiaView *)GetPane(row, col);
// 	if (row == 0 && col == 0)
// 	{
// 		pView->SetCameraPos(GSceneView::CameraPos::TOP);
// 		pView->SetCameraMode(GSceneView::CameraMode::ORTHOGONAL);
// 	}
// 	if (row == 0 && col == 1)
// 	{
// 		pView->SetCameraPos(GSceneView::CameraPos::FREE);
// 	}
// 	if (row == 1 && col == 0)
// 	{
// 		pView->SetCameraPos(GSceneView::CameraPos::FRONT);
// 		pView->SetCameraMode(GSceneView::CameraMode::ORTHOGONAL);
// 	}
// 	if (row == 1 && col == 1)
// 	{
// 		pView->SetCameraPos(GSceneView::CameraPos::RIGHT);
// 		pView->SetCameraMode(GSceneView::CameraMode::ORTHOGONAL);
// 	}
	pView->SendMessage(WM_SIZE, 0, MAKELPARAM(sizeInit.cx, sizeInit.cy));
	return true;
}

void GSplitter::ChangeViewStatus()
{
	int		aRow, aCol;
	CWnd* pActiveWnd = GetActivePane(&aRow, &aCol);

	if (m_nHidedRow == -1)
	{
		HideRow((aRow+1)%2);
		HideColumn((aCol+1)%2);
	}
	else
	{
		ShowColumn();
		ShowRow();
	}
	RecalcLayout();
}

void GSplitter::ShowColumn()
{
     ASSERT_VALID(this);
     ASSERT(m_nCols < m_nMaxCols);
     ASSERT(m_nHidedCol != -1);

     int colNew = m_nHidedCol;
     m_nHidedCol = -1;
     int cxNew = m_pColInfo[m_nCols].nCurSize;
     m_nCols++;  // add a column
     ASSERT(m_nCols == m_nMaxCols);

    // fill the hided column
     int col;
     for (int row = 0; row < m_nRows; row++)
     {
          CWnd* pPaneShow = GetDlgItem(
               AFX_IDW_PANE_FIRST + row * 16 + m_nCols);
          ASSERT(pPaneShow != NULL);
          pPaneShow->ShowWindow(SW_SHOWNA);

          for (col = m_nCols - 2; col >= colNew; col--)
          {
               CWnd* pPane = GetPane(row, col);
               ASSERT(pPane != NULL);
               pPane->SetDlgCtrlID(IdFromRowCol(row, col + 1));
          }

         pPaneShow->SetDlgCtrlID(IdFromRowCol(row, colNew));
     }

    // new panes have been created -- recalculate layout
     for (col = colNew + 1; col < m_nCols; col++)
         m_pColInfo[col].nIdealSize = m_pColInfo[col - 1].nCurSize;
     m_pColInfo[colNew].nIdealSize = cxNew;
     //RecalcLayout();
}

void GSplitter::HideColumn(int colHide)
{
     ASSERT_VALID(this);
     ASSERT(m_nCols > 1);
     ASSERT(colHide < m_nCols);
     ASSERT(m_nHidedCol == -1);
     m_nHidedCol = colHide;

    // if the column has an active window -- change it
     int rowActive, colActive;
     if (GetActivePane(&rowActive, &colActive) != NULL &&
         colActive == colHide)
     {
          if (++colActive >= m_nCols)
              colActive = 0;
          SetActivePane(rowActive, colActive);
     }

    // hide all column panes
     for (int row = 0; row < m_nRows; row++)
     {
          CWnd* pPaneHide = GetPane(row, colHide);
          ASSERT(pPaneHide != NULL);
          pPaneHide->ShowWindow(SW_HIDE);
          pPaneHide->SetDlgCtrlID(
               AFX_IDW_PANE_FIRST + row * 16 + m_nCols);

          for (int col = colHide + 1; col < m_nCols; col++)
          {
               CWnd* pPane = GetPane(row, col);
               ASSERT(pPane != NULL);
               pPane->SetDlgCtrlID(IdFromRowCol(row, col - 1));
          }
     }
     m_nCols--;
     m_pColInfo[m_nCols].nCurSize = m_pColInfo[colHide].nCurSize;
     //RecalcLayout();
}

void GSplitter::ShowRow()
{
     ASSERT_VALID(this);
     ASSERT(m_nRows < m_nMaxRows);
     ASSERT(m_nHidedRow != -1);

     int rowNew = m_nHidedRow;
     m_nHidedRow = -1;
     int cyNew = m_pRowInfo[m_nRows].nCurSize;
     m_nRows++;  // add a column
     ASSERT(m_nRows == m_nMaxRows);

    // fill the hided column
     int row;
     for (int col = 0; col < m_nCols; col++)
     {
          CWnd* pPaneShow = GetDlgItem(
               AFX_IDW_PANE_FIRST + m_nRows * 16 + col);
          ASSERT(pPaneShow != NULL);
          pPaneShow->ShowWindow(SW_SHOWNA);

          for (row = m_nRows - 2; row >= rowNew; row--)
          {
               CWnd* pPane = GetPane(row, col);
               ASSERT(pPane != NULL);
               pPane->SetDlgCtrlID(IdFromRowCol(row+1, col));
          }

         pPaneShow->SetDlgCtrlID(IdFromRowCol(rowNew, col));
     }

    // new panes have been created -- recalculate layout
     for (row = rowNew + 1; row < m_nRows; row++)
         m_pRowInfo[row].nIdealSize = m_pRowInfo[row - 1].nCurSize;
     m_pRowInfo[rowNew].nIdealSize = cyNew;
     //RecalcLayout();
}

void GSplitter::HideRow(int rowHide)
{
     ASSERT_VALID(this);
     ASSERT(m_nRows > 1);
     ASSERT(rowHide < m_nRows);
     ASSERT(m_nHidedRow == -1);
     m_nHidedRow = rowHide;

    // if the column has an active window -- change it
     int rowActive, colActive;
     if (GetActivePane(&rowActive, &colActive) != NULL &&
         rowActive == rowHide)
     {
          if (++rowActive >= m_nRows)
              rowActive = 0;
          SetActivePane(rowActive, colActive);
     }

    // hide all column panes
     for (int col = 0; col < m_nCols; col++)
     {
          CWnd* pPaneHide = GetPane(rowHide, col);
          ASSERT(pPaneHide != NULL);
          pPaneHide->ShowWindow(SW_HIDE);
          pPaneHide->SetDlgCtrlID(
               AFX_IDW_PANE_FIRST + m_nRows * 16 + col);

          for (int row = rowHide + 1; row < m_nRows; row++)
          {
               CWnd* pPane = GetPane(row, col);
               ASSERT(pPane != NULL);
               pPane->SetDlgCtrlID(IdFromRowCol(row-1, col));
          }
     }
     m_nRows--;
     m_pRowInfo[m_nRows].nCurSize = m_pRowInfo[rowHide].nCurSize;
     //RecalcLayout();
}
void GSplitter::OnSize( UINT nType, int cx, int cy )
{
	if ( cx != 0 && cy != 0 )
	{
		int w = ( cx / 2 ) - SPLITTER_GAPX;
		int h = ( cy / 2 ) - SPLITTER_GAPY;
		SetRowInfo(0, h, 1);
		SetRowInfo(1, h, 1);
		SetColumnInfo(0, w, 1);
		SetColumnInfo(1, w, 1);
	}
	CSplitterWnd::OnSize( nType, cx, cy );
}





