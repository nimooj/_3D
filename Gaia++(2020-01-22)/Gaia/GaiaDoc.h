#pragma once

#include "Src/GClothesSkinDlg.h"
#include "Src/GDentalCadDlg.h"
#include "Src/GCurvatureDlg.h"
#include "Src/GBridgeMeshDlg.h"

class GaiaDoc : public CDocument
{
	friend class GaiaView;
protected: 
	// 생성자
	GaiaDoc();
	DECLARE_DYNCREATE(GaiaDoc)

public:
	// 소멸자
	virtual ~GaiaDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	// 가상함수 재정의
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	void PrintMessage(std::string msg);

	// 메시지 처리 함수
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileImport();
	afx_msg void OnFileExport();
	afx_msg void OnDeleteMesh();
	afx_msg void OnShowMesh();
	afx_msg void OnHideMesh();
	afx_msg void OnUpdateShowMesh(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHideMesh(CCmdUI *pCmdUI);
	afx_msg void OnTestScene0();
	afx_msg void OnTestScene1();
	afx_msg void OnTestScene2();
	afx_msg void OnTestScene3();
	afx_msg void OnTestScene4();
	afx_msg void OnTestScene5();
	afx_msg void OnTestScene6();
	afx_msg void OnTestScene7A();
	afx_msg void OnTestScene7B();
	afx_msg void OnTestScene7C();
	afx_msg void OnTestScene7D();
	afx_msg void OnTestScene8();
	afx_msg void OnTestScene9();
	afx_msg void OnTestScene10();
	afx_msg void OnTestScene11();
	afx_msg void OnTestScene12();
	afx_msg void OnTestScene13();
	afx_msg void OnTestScene15();
	afx_msg void OnTestScene16();
	afx_msg void OnTestScene17();
	afx_msg void OnTestScene18();
	afx_msg void OnTestScene19();
	afx_msg void OnTestScene20();
	afx_msg void OnTestScene21();
	afx_msg void OnTestScene22();
	afx_msg void OnExportSelection();
	afx_msg void OnImportCamera();
	afx_msg void OnExportCamera();
	afx_msg void OnImportGeodesicPath();
	afx_msg void OnExportGeodesicPath();
	afx_msg void OnImportLayout();
	afx_msg void OnExportLayout();

public:
	/*******************************************************************************/
	/* 프로젝트명: 3D 스캔된 체험아이템의 디지털 편집 조작성 강화 기법에 대한 연구 */
	/* 지원기관: 한국전자통신연구원 (김호원, 김태준 박사님)                        */
	/* 수행기간: 2015년 6월 1일 ~ 2016년 1월 31                                    */
	/*******************************************************************************/
	GClothesSkinDlg *m_pClothesSkinDlg; /*! \brief 옷의 스키닝을 위한 모달리스 대화상자 */
	GMesh *m_pAvatar;					/*! \brief 아바타 모델 (스킨 + 본) */
	GMesh *m_pAvatarSkinMesh;			/*! \brief 아바타 스킨 메쉬 */
	GMesh *m_pAvatarBoneMesh;			/*! \brief 아바타 본 메쉬 */
	GMesh *m_pAvatarSkirtMesh;			/*! \brief 여자 아바타 스커트 메쉬 */
	GMesh *m_pAvatarClothesMesh;		/*! \brief 스키닝할 아바타의 옷 메쉬 */
	GMesh *m_pBindRefPlnLeftShoulder;	/*! \brief 옷의 왼쪽 어깨 부분의 스키닝을 위한 바인딩 참조 스윕 */
	GMesh *m_pBindRefPlnRightShoulder;	/*! \brief 옷의 오른쪽 어깨 부분의 스키닝을 위한 바인딩 참조 스윕 */
	afx_msg void OnEtri2015();
	void Etri2015CreateBindRefSwpPlnLS();
	void Etri2015CreateBindRefSwpPlnRS();
	
	/***********************************************************/
	/* 프로젝트명: 임플란트를 위한 크라운 설계 및 변형 툴 개발 */
	/* 지원기관: 서울대학교 (김명수 교수님)                    */
	/* 수행기간: 2015년 10월 1일 ~ 2016년 7월 31               */
	/***********************************************************/
	GDentalCadDlg *m_pDentalCadDlg;		/*! \brief 치아 데이터 관리를 위한 대화상자 */
	GCurvatureDlg *m_pCurvatureDlg;		/*! \brief 곡률 가시화를 위한 모달리스 대화상자 */
	afx_msg void OnImplantGuide();
	afx_msg void OnDentalModel1();
	afx_msg void OnDentalModel2();

	/***********************************************/
	/* 프로젝트명: 치과용 모델의 sculpting 툴 개발 */
	/* 지원기관: 서울대학교 (김명수 교수님)        */
	/* 수행기간: 2016년 10월 1일 ~ 2017년 7월 31   */
	/***********************************************/
	GMesh *m_pBridgeStMesh;	/*! \brief Bridge 메쉬 생성을 위한 변수들 */
	GVertex *m_pBridgeStVert;
	double m_BridgeStConRad;
	double m_BridgeStAuxRad;
	GMesh *m_pBridgeEdMesh;
	GVertex *m_pBridgeEdVert;
	double m_BridgeEdConRad;
	double m_BridgeEdAuxRad;

	GMesh *m_pPQPMesh1;
	GMesh *m_pPQPMesh2;
	GPoint3 m_MinDistPt1;		/*! \brief 최단거리가 생긴 정점의 정보 */
	GPoint3 m_MinDistPt2;		/*! \brief 최단거리가 생긴 정점의 정보 */


	GBridgeMeshDlg *m_pBridgeMeshDlg;	/*! \brief 브리지 생성 대화상자 */
};


