#ifndef _PCUTTER_H
#define _PCUTTER_H

#include "..\usr\include\gmath.h"
#include "..\usr\include\gobject.h"
#include "..\usr\include\gscene.h"

#include "..\PEditor\PEditor.h"
#include "..\PCreator\PCreator.h"

#include "PCutterDlg.h"
#include "PCutterView.h"
#include "PCutterScene.h"

#pragma comment(lib, "gmath.lib")	
#pragma comment(lib, "gobject.lib")	
#pragma comment(lib, "gscene.lib")
#pragma comment(lib, "peditor.lib")	
#pragma comment(lib, "pcreator.lib")	
#pragma comment(lib, "glut32.lib")


/*! \brief 플러그인 프로그램에서 사용하는 전역변수 선언 */
extern PCutterScene theCutterScene;
extern PEditor theCutterEditor;
extern CDocument *theAppDoc;
extern GScene *theAppScene;
extern PEditor *theAppEditor;

/*!
*	\class PCutter
*	\brief Class for mesh cutter
*
*	\author Seung-Hyun Yun(shyun@dongguk.edu) 
*	\date 12 Mar. 2015	
*/
class AFX_EXT_CLASS PCutter
{
public:
	PCutter() {};
	virtual ~PCutter() {};

	// Member functions
	void InitPlugIn(GScene *_pScene, CDialog *_pParent, CDocument *_pDoc, PEditor *_pEditor);

public:
	/*! \brief Dialog for blending surfaces */
	PCutterDlg m_CutterDlg;
};

#endif // _PBLENDSRF_H
