#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class GBridgeCrvView;

// GBridgeMeshDlg ��ȭ �����Դϴ�.

class GBridgeMeshDlg : public CDialog
{
	DECLARE_DYNAMIC(GBridgeMeshDlg)

public:
	GBridgeMeshDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~GBridgeMeshDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_BRIDGE_MESH_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();

	// �޽��� ó�� �Լ�
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedScreenCapture();
	afx_msg void OnBnClickedCreateBridgeMesh();
	afx_msg void OnBnClickedCompleteBridgeMesh();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnCloseupContactCrvType();
	afx_msg void OnBnClickedEndMesh();
	afx_msg void OnBnClickedStartMesh();
	
public:
	// ����� ���� �Լ�
	void UpdateGLView();
	void UpdateBridgeMesh();

public:
	// ������ ���
	/*! \brief �信 ���� ������ */
	GBridgeCrvView *m_pView;

	/*! \brief ���õ� �޽��� �ε��� (0: ���� �޽�, 1: �� �޽�) */
	int			m_SltMesh;
	double		m_BlendParam;
	CComboBox	m_cContactCrvType;
	
	/*! \brief �긮���� ���� �޽��� ���� ���� */
	GMesh		*m_pStartMesh;
	GVertex		*m_pStartVert;
	GMesh		*m_pStartUnfoldMesh;
	std::map<int, int> m_StartUnfoldFaceMap;
	GNurbsCrv	*m_pStartContCrv;
	double		m_StartCrvSize;
	double		m_StartCrvEditParam;
	int			m_StartCrvEditRes;
	double		m_StartAuxCrvSclParam;
		
	/*! \brief �긮���� �� �޽��� ���� ���� */
	GMesh		*m_pEndMesh;
	GVertex		*m_pEndVert;
	GMesh		*m_pEndUnfoldMesh;
	std::map<int, int> m_EndUnfoldFaceMap;
	GNurbsCrv	*m_pEndContCrv;
	double		m_EndCrvSize;
	double		m_EndCrvEditParam;
	int			m_EndCrvEditRes;
	double		m_EndAuxCrvSclParam;

	/*! \brief ������ �긮�� �޽��� ���� ���� */
	GMesh		*m_pBridgeMesh;
};
