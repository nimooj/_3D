#include "GaiaDoc.h"
#pragma once

class GView : public CWnd
{
	// Construction
public:
	GView();
	virtual ~GView();

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
	// OpenGL 렌더링을 위한 변수
	CDC		*m_pDC;
	HDC		m_hDC;
	HGLRC	m_hRC;

	// 렌더링 할 객체의 리스트
	std::vector<GMesh *> m_MeshList;

	// 뷰가 포함된 대화상자에 대한 포인터
	CDialog *m_pDialog;
};