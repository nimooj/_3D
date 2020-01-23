#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class GBridgeCrvView;

// GBridgeMeshDlg 대화 상자입니다.

class GBridgeMeshDlg : public CDialog
{
	DECLARE_DYNAMIC(GBridgeMeshDlg)

public:
	GBridgeMeshDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~GBridgeMeshDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_BRIDGE_MESH_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	// 메시지 처리 함수
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedScreenCapture();
	afx_msg void OnBnClickedCreateBridgeMesh();
	afx_msg void OnBnClickedCompleteBridgeMesh();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnCloseupContactCrvType();
	afx_msg void OnBnClickedEndMesh();
	afx_msg void OnBnClickedStartMesh();
	
public:
	// 사용자 정의 함수
	void UpdateGLView();
	void UpdateBridgeMesh();

public:
	// 데이터 멤버
	/*! \brief 뷰에 대한 포인터 */
	GBridgeCrvView *m_pView;

	/*! \brief 선택된 메쉬의 인덱스 (0: 시작 메쉬, 1: 끝 메쉬) */
	int			m_SltMesh;
	double		m_BlendParam;
	CComboBox	m_cContactCrvType;
	
	/*! \brief 브리지의 시작 메쉬에 대한 정보 */
	GMesh		*m_pStartMesh;
	GVertex		*m_pStartVert;
	GMesh		*m_pStartUnfoldMesh;
	std::map<int, int> m_StartUnfoldFaceMap;
	GNurbsCrv	*m_pStartContCrv;
	double		m_StartCrvSize;
	double		m_StartCrvEditParam;
	int			m_StartCrvEditRes;
	double		m_StartAuxCrvSclParam;
		
	/*! \brief 브리지의 끝 메쉬에 대한 정보 */
	GMesh		*m_pEndMesh;
	GVertex		*m_pEndVert;
	GMesh		*m_pEndUnfoldMesh;
	std::map<int, int> m_EndUnfoldFaceMap;
	GNurbsCrv	*m_pEndContCrv;
	double		m_EndCrvSize;
	double		m_EndCrvEditParam;
	int			m_EndCrvEditRes;
	double		m_EndAuxCrvSclParam;

	/*! \brief 생성된 브리지 메쉬에 대한 정보 */
	GMesh		*m_pBridgeMesh;
};
