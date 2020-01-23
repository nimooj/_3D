#pragma once

#include "resource.h"
#include "PCutterView.h"

#include "..\usr\include\Eigen\Dense"
#include "..\usr\include\Eigen\SparseLU.h"

using namespace Eigen;

// PCutterDlg 대화 상자입니다.

class PCutterDlg : public CDialog
{
	DECLARE_DYNAMIC(PCutterDlg)

public:
	PCutterDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~PCutterDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CUTTER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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

	// 파라미터 공간에서 정의된 2D 절단 곡선
	GMesh *p2DCutCrv;	// pCutterScene->MeshList[0]에 저장됨

	// 선택된 메쉬 혹은 삼각형의 매개화 결과 메쉬
	GMesh *pParamMesh;	// pCutterScene->MeshList[1]에 저장됨

	// 객체 공간에서 정의된 3D 절단 곡선
	GMesh *p3DCutCrv;

	std::map<int, int> SubMeshVertIdxMap;

	GQTree *pQTree;
};
