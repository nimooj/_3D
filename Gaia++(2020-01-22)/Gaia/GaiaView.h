#include "GaiaDoc.h"
#pragma once


class GaiaView : public CView
{
protected: 
	// ������
	GaiaView();
	DECLARE_DYNCREATE(GaiaView)

public:
	// �Ҹ���
	virtual ~GaiaView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	// �����Լ� ������
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// �Ϲݸ���Լ�
	GaiaDoc* GetDocument() const;
	void InitView();

	// �޽��� ó�� �Լ�
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnCameraPers();
	afx_msg void OnUpdateCameraPers(CCmdUI *pCmdUI);
	afx_msg void OnCameraOrtho();
	afx_msg void OnUpdateCameraOrtho(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

public:
	// ������ ���
	// OpenGL �ʱ�ȭ�� ���� ������
	CDC	*m_pDC;
	HDC	m_hDC;
	HGLRC m_hRC;
};

#ifndef _DEBUG  // GaiaView.cpp�� ����� ����
inline GaiaDoc* GaiaView::GetDocument() const { return reinterpret_cast<GaiaDoc *>(m_pDocument); }
#endif

