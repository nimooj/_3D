#pragma once

// PEditMeshDlg ��ȭ �����Դϴ�.
#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "PAddMtlDlg.h"
#include "ColorButton.h"

class PEditor;

// PEditMeshDlg ��ȭ �����Դϴ�.
class AFX_EXT_CLASS PEditMeshDlg : public CDialog
{
	DECLARE_DYNAMIC(PEditMeshDlg)
public:
	// ������ �� �Ҹ���
	PEditMeshDlg(CWnd* pParent = NULL);
	virtual ~PEditMeshDlg();
	enum { IDD = IDD_EDIT_MESH_DLG };

public:
	// �����Լ� ������
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// �޽��� ó�� �Լ�
	DECLARE_MESSAGE_MAP()
	afx_msg void OnEnKillfocusMeshName();
	afx_msg void OnEnKillfocusMeshSclX();
	afx_msg void OnEnKillfocusMeshSclY();
	afx_msg void OnEnKillfocusMeshSclZ();
	afx_msg void OnEnKillfocusMeshRotX();
	afx_msg void OnEnKillfocusMeshRotY();
	afx_msg void OnEnKillfocusMeshRotZ();
	afx_msg void OnEnKillfocusMeshTransX();
	afx_msg void OnEnKillfocusMeshTransY();
	afx_msg void OnEnKillfocusMeshTransZ();
	afx_msg void OnBnClickedAddMaterial();
	afx_msg void OnCbnCloseupMtrlList();
	afx_msg void OnBnClickedApplyEditTool();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedShowEditBndryApprox();
	afx_msg void OnBnClickedShowEditBndryExact();
	afx_msg LONG OnSelEndOK(UINT lParam, LONG wParam);
    afx_msg LONG OnSelEndCancel(UINT lParam, LONG wParam);
    afx_msg LONG OnSelChange(UINT lParam, LONG wParam);
    afx_msg LONG OnCloseUp(UINT lParam, LONG wParam);
    afx_msg LONG OnDropDown(UINT lParam, LONG wParam);
	
public:
	// ������ ���
	/*! \brief ���� ���α׷��� ��ť��Ʈ Ŭ������ ���� ������ */
	CDocument *pDoc;

	/*! \brief �����Ϳ� ���� ������ */
	PEditor *pEditor;

	/*! \brief ���õ� ��ü�� �޽� �̸� */
	CString m_MeshName;

	/*! \brief ���õ� ��ü�� ������ ���� */
	int m_NumVert;

	/*! \brief ���õ� ��ü�� �ﰢ���� ���� */
	int m_NumFace;

	/*! \brief ���õ� ��ü�� Ŭ���� �̸� */
	CString m_MeshClsName;

	/*! \brief ���� ����Ʈ�� �����ϴ� �޺� �ڽ� */
	CComboBox m_cMtlList;

	/*! \brief ���� ���� �̸��� �����ϴ� �޺� �ڽ� */
	CComboBox m_cEditToolList;

	/*! \brief ���õ� ��ü�� �𵨸� ��ǥ���� �̵� ���� */
	double m_EditMeshTransX;
	double m_EditMeshTransY;
	double m_EditMeshTransZ;

	/*! \brief ���õ� ��ü�� �𵨸� ��ǥ���� ȸ�� ���� */
	double m_EditMeshRotX;
	double m_EditMeshRotY;
	double m_EditMeshRotZ;

	/*! \brief ���õ� ��ü�� �𵨸� ��ǥ���� ������ ���� */
	double m_EditMeshSclX;
	double m_EditMeshSclY;
	double m_EditMeshSclZ;
	
	/*! \brief ���õ� ��ü�� ������ �����ϴ� ��Ʈ�� �� ���� */
	CSliderCtrl m_cTransparency;
	int m_Transparency;

	/*! \brief ���� �귯���� ũ�⸦ �����ϴ� ��Ʈ�� �� ���� */
	CSliderCtrl m_cBrushRadius;
	int m_BrushRadius;

	/*! \brief ���� ������ ũ�⸦ �����ϴ� ��Ʈ�� �� ���� */
	CSliderCtrl m_cEditRgnDist;
	int m_EditRgnDist;

	/*! \brief ���� ������ ��� ǥ�� ���� */
	int m_ShowEditBndryType;
	int m_CollisionMode;
	afx_msg void OnBnClickedCollisionCheck();
	afx_msg void OnBnClickedCollisionFree();

	/*! \brief ������ ������ ��Ÿ���� ���� */
	CColorButton m_cAmbiColor;
	CColorButton m_cDiffColor;
	CColorButton m_cSpecColor;
};
