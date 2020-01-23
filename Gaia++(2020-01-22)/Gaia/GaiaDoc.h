#pragma once

#include "Src/GClothesSkinDlg.h"
#include "Src/GDentalCadDlg.h"
#include "Src/GCurvatureDlg.h"
#include "Src/GBridgeMeshDlg.h"

class GaiaDoc : public CDocument
{
	friend class GaiaView;
protected: 
	// ������
	GaiaDoc();
	DECLARE_DYNCREATE(GaiaDoc)

public:
	// �Ҹ���
	virtual ~GaiaDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	// �����Լ� ������
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	void PrintMessage(std::string msg);

	// �޽��� ó�� �Լ�
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
	/* ������Ʈ��: 3D ��ĵ�� ü��������� ������ ���� ���ۼ� ��ȭ ����� ���� ���� */
	/* �������: �ѱ�������ſ����� (��ȣ��, ������ �ڻ��)                        */
	/* ����Ⱓ: 2015�� 6�� 1�� ~ 2016�� 1�� 31                                    */
	/*******************************************************************************/
	GClothesSkinDlg *m_pClothesSkinDlg; /*! \brief ���� ��Ű���� ���� ��޸��� ��ȭ���� */
	GMesh *m_pAvatar;					/*! \brief �ƹ�Ÿ �� (��Ų + ��) */
	GMesh *m_pAvatarSkinMesh;			/*! \brief �ƹ�Ÿ ��Ų �޽� */
	GMesh *m_pAvatarBoneMesh;			/*! \brief �ƹ�Ÿ �� �޽� */
	GMesh *m_pAvatarSkirtMesh;			/*! \brief ���� �ƹ�Ÿ ��ĿƮ �޽� */
	GMesh *m_pAvatarClothesMesh;		/*! \brief ��Ű���� �ƹ�Ÿ�� �� �޽� */
	GMesh *m_pBindRefPlnLeftShoulder;	/*! \brief ���� ���� ��� �κ��� ��Ű���� ���� ���ε� ���� ���� */
	GMesh *m_pBindRefPlnRightShoulder;	/*! \brief ���� ������ ��� �κ��� ��Ű���� ���� ���ε� ���� ���� */
	afx_msg void OnEtri2015();
	void Etri2015CreateBindRefSwpPlnLS();
	void Etri2015CreateBindRefSwpPlnRS();
	
	/***********************************************************/
	/* ������Ʈ��: ���ö�Ʈ�� ���� ũ��� ���� �� ���� �� ���� */
	/* �������: ������б� (���� ������)                    */
	/* ����Ⱓ: 2015�� 10�� 1�� ~ 2016�� 7�� 31               */
	/***********************************************************/
	GDentalCadDlg *m_pDentalCadDlg;		/*! \brief ġ�� ������ ������ ���� ��ȭ���� */
	GCurvatureDlg *m_pCurvatureDlg;		/*! \brief ��� ����ȭ�� ���� ��޸��� ��ȭ���� */
	afx_msg void OnImplantGuide();
	afx_msg void OnDentalModel1();
	afx_msg void OnDentalModel2();

	/***********************************************/
	/* ������Ʈ��: ġ���� ���� sculpting �� ���� */
	/* �������: ������б� (���� ������)        */
	/* ����Ⱓ: 2016�� 10�� 1�� ~ 2017�� 7�� 31   */
	/***********************************************/
	GMesh *m_pBridgeStMesh;	/*! \brief Bridge �޽� ������ ���� ������ */
	GVertex *m_pBridgeStVert;
	double m_BridgeStConRad;
	double m_BridgeStAuxRad;
	GMesh *m_pBridgeEdMesh;
	GVertex *m_pBridgeEdVert;
	double m_BridgeEdConRad;
	double m_BridgeEdAuxRad;

	GMesh *m_pPQPMesh1;
	GMesh *m_pPQPMesh2;
	GPoint3 m_MinDistPt1;		/*! \brief �ִܰŸ��� ���� ������ ���� */
	GPoint3 m_MinDistPt2;		/*! \brief �ִܰŸ��� ���� ������ ���� */


	GBridgeMeshDlg *m_pBridgeMeshDlg;	/*! \brief �긮�� ���� ��ȭ���� */
};


