#pragma once

// PEditMeshDlg 대화 상자입니다.
#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "PAddMtlDlg.h"
#include "ColorButton.h"

class PEditor;

// PEditMeshDlg 대화 상자입니다.
class AFX_EXT_CLASS PEditMeshDlg : public CDialog
{
	DECLARE_DYNAMIC(PEditMeshDlg)
public:
	// 생성자 및 소멸자
	PEditMeshDlg(CWnd* pParent = NULL);
	virtual ~PEditMeshDlg();
	enum { IDD = IDD_EDIT_MESH_DLG };

public:
	// 가상함수 재정의
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// 메시지 처리 함수
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
	// 데이터 멤버
	/*! \brief 응용 프로그램의 도큐먼트 클래스에 대한 포인터 */
	CDocument *pDoc;

	/*! \brief 에디터에 대한 포인터 */
	PEditor *pEditor;

	/*! \brief 선택된 객체의 메쉬 이름 */
	CString m_MeshName;

	/*! \brief 선택된 객체의 정점의 개수 */
	int m_NumVert;

	/*! \brief 선택된 객체의 삼각형의 개수 */
	int m_NumFace;

	/*! \brief 선택된 객체의 클래스 이름 */
	CString m_MeshClsName;

	/*! \brief 재질 리스트를 저장하는 콤보 박스 */
	CComboBox m_cMtlList;

	/*! \brief 편집 툴을 이름을 저장하는 콤보 박스 */
	CComboBox m_cEditToolList;

	/*! \brief 선택된 객체의 모델링 좌표계의 이동 정보 */
	double m_EditMeshTransX;
	double m_EditMeshTransY;
	double m_EditMeshTransZ;

	/*! \brief 선택된 객체의 모델링 좌표계의 회전 정보 */
	double m_EditMeshRotX;
	double m_EditMeshRotY;
	double m_EditMeshRotZ;

	/*! \brief 선택된 객체의 모델링 좌표계의 스케일 정보 */
	double m_EditMeshSclX;
	double m_EditMeshSclY;
	double m_EditMeshSclZ;
	
	/*! \brief 선택된 객체의 투명도를 지정하는 컨트롤 및 변수 */
	CSliderCtrl m_cTransparency;
	int m_Transparency;

	/*! \brief 선택 브러쉬의 크기를 지정하는 컨트롤 및 변수 */
	CSliderCtrl m_cBrushRadius;
	int m_BrushRadius;

	/*! \brief 편집 영역의 크기를 조절하는 컨트롤 및 변수 */
	CSliderCtrl m_cEditRgnDist;
	int m_EditRgnDist;

	/*! \brief 편집 영역의 경계 표시 여부 */
	int m_ShowEditBndryType;
	int m_CollisionMode;
	afx_msg void OnBnClickedCollisionCheck();
	afx_msg void OnBnClickedCollisionFree();

	/*! \brief 재질의 색상을 나타내는 변수 */
	CColorButton m_cAmbiColor;
	CColorButton m_cDiffColor;
	CColorButton m_cSpecColor;
};
