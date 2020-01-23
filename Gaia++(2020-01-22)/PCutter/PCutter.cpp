// PCutter.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "stdafx.h"
#include "PCutter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*! \brief 메인 프로그램의 도큐먼트에 대한 포인터 */
CDocument *theAppDoc;

/*! \brief 메인 프로그램의 장면에 대한 포인터 */
GScene *theAppScene;

/*! \brief 메인 프로그램의 편집기 */
PEditor *theAppEditor;

/*! \brief theCutterScene 전역 객체 */
PCutterScene theCutterScene;

/*!	\brief ParamEditor 전역 객체 */
PEditor theCutterEditor;

/*!
*	\brief 플러그인을 초기화 한다.
*/
void PCutter::InitPlugIn(GScene *_pScene, CDialog *_pParent, CDocument *_pDoc, PEditor *_pEditor)
{
	theAppDoc = _pDoc;
	theAppScene = _pScene;
	theAppEditor = _pEditor;
	m_CutterDlg.Create(IDD_CUTTER_DLG, _pParent);

	// 전역 객체 초기화
	theCutterEditor.pScene = &theCutterScene;
	theCutterEditor.theSelector.SelectToolType = SELECT_TOOL_RECT;
	theCutterEditor.theSelector.SelectObjType = SELECT_CTRL_PT;
	theCutterEditor.EditType = EDIT_TRA;
}
