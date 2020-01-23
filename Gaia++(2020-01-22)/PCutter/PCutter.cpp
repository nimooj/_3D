// PCutter.cpp : �ش� DLL�� �ʱ�ȭ ��ƾ�� �����մϴ�.
//

#include "stdafx.h"
#include "PCutter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*! \brief ���� ���α׷��� ��ť��Ʈ�� ���� ������ */
CDocument *theAppDoc;

/*! \brief ���� ���α׷��� ��鿡 ���� ������ */
GScene *theAppScene;

/*! \brief ���� ���α׷��� ������ */
PEditor *theAppEditor;

/*! \brief theCutterScene ���� ��ü */
PCutterScene theCutterScene;

/*!	\brief ParamEditor ���� ��ü */
PEditor theCutterEditor;

/*!
*	\brief �÷������� �ʱ�ȭ �Ѵ�.
*/
void PCutter::InitPlugIn(GScene *_pScene, CDialog *_pParent, CDocument *_pDoc, PEditor *_pEditor)
{
	theAppDoc = _pDoc;
	theAppScene = _pScene;
	theAppEditor = _pEditor;
	m_CutterDlg.Create(IDD_CUTTER_DLG, _pParent);

	// ���� ��ü �ʱ�ȭ
	theCutterEditor.pScene = &theCutterScene;
	theCutterEditor.theSelector.SelectToolType = SELECT_TOOL_RECT;
	theCutterEditor.theSelector.SelectObjType = SELECT_CTRL_PT;
	theCutterEditor.EditType = EDIT_TRA;
}
