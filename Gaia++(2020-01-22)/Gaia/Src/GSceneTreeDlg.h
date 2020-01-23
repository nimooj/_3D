#pragma once
#include "afxcmn.h"

#include "..\GaiaDoc.h"

// GSceneTreeDlg 대화 상자입니다.
class GSceneTreeDlg : public CDialog
{
	DECLARE_DYNAMIC(GSceneTreeDlg)

public:
	// 생성자 및 소멸자
	GSceneTreeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~GSceneTreeDlg();
	enum { IDD = IDD_WORKSPACE_TREE_DLG };

public:
	// 가상함수 재정의
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// 멤버함수
	void RecalcLayOut();
	void InsertMesh(const GMesh *pMesh, HTREEITEM hItem);
	void DeleteItem();
	void UpdateSceneTree();
	void UpdateSelectedItems();
		
	// 메시지 처리함수
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMClickMeshTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedMeshTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusMeshTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

public:
	/*! \brief 장면의 구성 객체를 나타내는 트리 컨트롤 */
	CTreeCtrl m_SceneTree;

	/*! \brief 장면의 구성 객체의 최상위 아이템에 대한 핸들 */
	HTREEITEM m_RootMesh;
	HTREEITEM m_RootLight;
	HTREEITEM m_RootCamera;
};
