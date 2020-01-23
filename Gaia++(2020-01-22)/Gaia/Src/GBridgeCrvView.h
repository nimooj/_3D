#pragma once

class GBridgeCrvView : public CWnd
{
	// Construction
public:
	GBridgeCrvView();
	virtual ~GBridgeCrvView();

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
	void InitOpenGL();
	void InitContCrv(int Type);
	void Render();

public: 
	// OpenGL �������� ���� ����
	CDC		*m_pDC;
	HDC		m_hDC;
	HGLRC	m_hRC;
	float	m_ZoomDist;

	// ������ ���� ����
	int MousePt[2];

	// ���� ���õ� �޽��� ���� ����
	int		m_SltMesh;
	GMesh	*m_pMesh;
	GMesh	*m_pUnfoldMesh;
	GNurbsCrv *m_pContCrv;
	double	m_CrvParam;
	int		m_CrvResolution;

	// �䰡 ���Ե� ��ȭ���ڿ� ���� ������
	CDialog *m_pParentDlg;
};