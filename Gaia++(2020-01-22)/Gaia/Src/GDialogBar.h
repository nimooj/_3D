#ifndef _GDIALOGBAR_H
#define _GDIALOGBAR_H

class GDialogBar : public CControlBar
{
public:
	// 생성자 및 소멸자
	GDialogBar();
	virtual ~GDialogBar();

public:
	// 가상함수 재정의
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual BOOL Create(CWnd* pParentWnd, CDialog *pDialog, CString &pTitle, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_LEFT);
	virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );
	virtual CSize CalcDynamicLayout( int nLength, DWORD dwMode );
	
	// 멤버함수
	BOOL IsHorz() const;
	void StartTracking();
	void StopTracking(BOOL bAccept);
	void OnInvertTracker(const CRect& rect);
	void DrawGripper(CDC &dc);
	CPoint& ClientToWnd(CPoint& point);

	// 메시지 처리함수
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnNcPaint();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg HITTEST_RET OnNcHitTest(CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
		
public:
	// 데이터 멤버
	COLORREF	m_clrBtnHilight;
	COLORREF	m_clrBtnShadow;
	CSize   m_sizeMin;
	CSize   m_sizeHorz;
	CSize   m_sizeVert;
	CSize   m_sizeFloat;
	CRect   m_rectBorder;
	CRect   m_rectTracker;
	UINT    m_nDockBarID;
	CPoint  m_ptOld;
	BOOL    m_bTracking;
	BOOL    m_bInRecalcNC;
	int     m_cxEdge;
	int		m_cxBorder;
	CRect	m_rectUndock;
	CRect	m_rectClose;
	CRect	m_rectGripper;
	int		m_cxGripper;
	CDialog	*m_cDialog;
	CBrush	m_brushBkgd;
	int		m_cyBorder;
	int		m_cMinWidth;
	int		m_cMinHeight;
	int		m_cCaptionSize;	
};

#endif // _GDIALOGBAR_H
