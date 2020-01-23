#ifndef _GSplitter_H
#define _GSplitter_H

class GSplitter : public CSplitterWnd
{
public:
//protected:
	GSplitter();
	DECLARE_DYNCREATE(GSplitter)
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GSplitter)
	public:
	//}}AFX_VIRTUAL

public:
	virtual	~GSplitter();
public:
	int		m_cRow, m_cCol; 
	int		m_nHidedCol, m_nHidedRow;  // hided column number, -1 if all columns are shown
	
	BOOL	CreateView( int row, int col, CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext );
	void	ChangeViewStatus();
	void	OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg);	
	void	RefreshSplitBars(void);

    void	ShowColumn();
    void	HideColumn(int colHide);
	void	ShowRow();
	void	HideRow(int rowHide);
protected:
	//{{AFX_MSG(GSplitter)
	afx_msg void OnSize( UINT nType, int cx, int cy );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#else

#endif // _GSplitter_H
