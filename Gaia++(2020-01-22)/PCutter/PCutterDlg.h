#pragma once

#include "resource.h"
#include "PCutterView.h"

#include "..\usr\include\Eigen\Dense"
#include "..\usr\include\Eigen\SparseLU.h"

using namespace Eigen;

// PCutterDlg ��ȭ �����Դϴ�.

class PCutterDlg : public CDialog
{
	DECLARE_DYNAMIC(PCutterDlg)

public:
	PCutterDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~PCutterDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CUTTER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCutMesh();
	afx_msg void OnBnClickedRefineCrv();
	afx_msg void OnBnClickedSelectPivotVert();
	afx_msg void OnBnClickedEditCurve();
	afx_msg void OnBnClickedMapSltMesh();
	afx_msg void OnBnClickedMapSltFace();

public:
	void UpdateQuadTree();
	void Update3DCutCrv();

public:
	PCutterView *m_pCutterView;
	GMesh *pSubMesh;

	// �Ķ���� �������� ���ǵ� 2D ���� �
	GMesh *p2DCutCrv;	// pCutterScene->MeshList[0]�� �����

	// ���õ� �޽� Ȥ�� �ﰢ���� �Ű�ȭ ��� �޽�
	GMesh *pParamMesh;	// pCutterScene->MeshList[1]�� �����

	// ��ü �������� ���ǵ� 3D ���� �
	GMesh *p3DCutCrv;

	std::map<int, int> SubMeshVertIdxMap;

	GQTree *pQTree;
};
