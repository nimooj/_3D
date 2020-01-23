#ifndef _PCUTTER_VIEW_H
#define _PCUTTER_VIEW_H

class PCutterDlg;

class PCutterView : public CWnd
{
	// Construction
public:
	PCutterView();
	virtual ~PCutterView();

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	DECLARE_MESSAGE_MAP()

public:
	void OnEditCopy();

public: 
	// Variables for OpenGL Setting.
	CDC*	m_pDC;
	HDC		m_hDC;
	HGLRC	m_hRC;

	PCutterDlg *m_pCutterDlg;
};

#endif // _PCUTTER_VIEW_H
