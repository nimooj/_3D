#pragma once
#include "afxcmn.h"

#include "..\GaiaDoc.h"

// GSceneTreeDlg ��ȭ �����Դϴ�.
class GSceneTreeDlg : public CDialog
{
	DECLARE_DYNAMIC(GSceneTreeDlg)

public:
	// ������ �� �Ҹ���
	GSceneTreeDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~GSceneTreeDlg();
	enum { IDD = IDD_WORKSPACE_TREE_DLG };

public:
	// �����Լ� ������
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// ����Լ�
	void RecalcLayOut();
	void InsertMesh(const GMesh *pMesh, HTREEITEM hItem);
	void DeleteItem();
	void UpdateSceneTree();
	void UpdateSelectedItems();
		
	// �޽��� ó���Լ�
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMClickMeshTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedMeshTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusMeshTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

public:
	/*! \brief ����� ���� ��ü�� ��Ÿ���� Ʈ�� ��Ʈ�� */
	CTreeCtrl m_SceneTree;

	/*! \brief ����� ���� ��ü�� �ֻ��� �����ۿ� ���� �ڵ� */
	HTREEITEM m_RootMesh;
	HTREEITEM m_RootLight;
	HTREEITEM m_RootCamera;
};
