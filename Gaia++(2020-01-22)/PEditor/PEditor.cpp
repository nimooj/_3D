// PEditor.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern double _sin_table[];
extern double _cos_table[];

/*!
*	\brief 디폴트 생성자
*/
PSelector::PSelector()
{
	// 데이터 멤버(9)
	pEditor = NULL;									// 에디터에 대한 포인터
	SelectToolType = SELECT_TOOL_RECT;				// 선택 툴 유형(사각형, 올가미, 다각형, 브러쉬)
	SelectModeType = SELECT_MODE_FRONT_ONLY;		// 선택 모드 (앞면, 앞면과 뒷면 모두)
	SelectObjType = SELECT_MESH;					// 선택 객체 유형(메쉬, 정점, 삼각형, 본, 제어점 중)
	MouseOverGroupIdx = -1;							// 현재 마우스 포인터와 교차된 삼각형 그룹의 인덱스
	MouseOverFaceIdx = -1;							// 현재 마우스 포인터와 교차된 삼각형의 인덱스
	MouseOverVertIdx = -1;							// 현재 마우스 포인터와 가장 가까운 정점의 인덱스
	MouseOverPtCoords.Set(0.0, 0.0, 0.0);			// 현재 마우스 포인터와 교차된 삼각형위의 점의 Barycentric 좌표
	SltRgn2DPtList.clear();							// 윈도우 좌표계에서 표현된 선택 영역의 가장자리 정점 리스트
	SltRgn3DPtList.clear();							// 월드 좌표계에서 표현된 선택 영역의 가장자리 정점 리스트 (렌더링을 위해 사용됨)
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사될 객체
*/
PSelector::PSelector(const PSelector &cpy)
{
	// 데이터 멤버(9)
	pEditor = cpy.pEditor;						// 에디터에 대한 포인터
	SelectToolType = cpy.SelectToolType;		// 선택 툴 유형(사각형, 올가미, 다각형, 브러쉬)
	SelectModeType = cpy.SelectModeType;		// 선택 모드 (앞면, 앞면과 뒷면 모두)
	SelectObjType = cpy.SelectObjType; 			// 선택 객체 유형(메쉬, 정점, 삼각형, 본, 제어점 중)
	MouseOverGroupIdx = cpy.MouseOverGroupIdx;	// 현재 마우스 포인터와 교차된 삼각형 그룹의 인덱스
	MouseOverFaceIdx = cpy.MouseOverFaceIdx;	// 현재 마우스 포인터와 교차된 삼각형의 인덱스
	MouseOverVertIdx = cpy.MouseOverVertIdx;	// 현재 마우스 포인터와 가장 가까운 정점의 인덱스
	MouseOverPtCoords = cpy.MouseOverPtCoords;	// 현재 마우스 포인터와 교차된 삼각형위의 점의 Barycentric 좌표
	SltRgn2DPtList = cpy.SltRgn2DPtList;		// 윈도우 좌표계에서 표현된 선택 영역의 가장자리 정점 리스트
	SltRgn3DPtList = cpy.SltRgn3DPtList;		// 월드 좌표계에서 표현된 선택 영역의 가장자리 정점 리스트 (렌더링을 위해 사용됨)
}

/*!
*	\brief 소멸자
*/
PSelector::~PSelector()
{
}

/*!
*	\brief 대입 연산자
*
*	\param rhs 대입될 객체
*/
PSelector &PSelector::operator =(const PSelector &rhs)
{
	// 데이터 멤버(9)
	pEditor = rhs.pEditor;						// 에디터에 대한 포인터
	SelectToolType = rhs.SelectToolType;		// 선택 툴 유형(사각형, 올가미, 다각형, 브러쉬)
	SelectModeType = rhs.SelectModeType;		// 선택 모드 (앞면, 앞면과 뒷면 모두)
	SelectObjType = rhs.SelectObjType; 			// 선택 객체 유형(메쉬, 정점, 삼각형, 본, 제어점 중)
	MouseOverGroupIdx = rhs.MouseOverGroupIdx;	// 현재 마우스 포인터와 교차된 삼각형 그룹의 인덱스
	MouseOverFaceIdx = rhs.MouseOverFaceIdx;	// 현재 마우스 포인터와 교차된 삼각형의 인덱스
	MouseOverVertIdx = rhs.MouseOverVertIdx;	// 현재 마우스 포인터와 가장 가까운 정점의 인덱스
	MouseOverPtCoords = rhs.MouseOverPtCoords;	// 현재 마우스 포인터와 교차된 삼각형위의 점의 Barycentric 좌표
	SltRgn2DPtList = rhs.SltRgn2DPtList;		// 윈도우 좌표계에서 표현된 선택 영역의 가장자리 정점 리스트
	SltRgn3DPtList = rhs.SltRgn3DPtList;		// 월드 좌표계에서 표현된 선택 영역의 가장자리 정점 리스트 (렌더링을 위해 사용됨)

	return *this;
}

/*!
*	\brief 현재 마우스의 포인터의 3차원 좌표를 반환한다.
*
*	\param x 마우스의 X 좌표
*	\param y 마우스의 Y 좌표
*
*	\return 마우스 포인터의 3차원 좌표를 반환한다.
*/
GVector3 PSelector::GetWorldCoord(int x, int y)
{
	double ox, oy, oz;

	if (pEditor->pScene->MainCamera.CameraType == PROJECTION_ORTHO)
		pEditor->pScene->MainCamera.GetWorldCoord(x, y, 0.0, &ox, &oy, &oz);
	else
		pEditor->pScene->MainCamera.GetWorldCoord(x, y, 0.5, &ox, &oy, &oz);

	return GVector3(ox, oy, oz);
}

/*!
*	\brief 객체 선택을 위한 경계 영역의 초기화 작업을 수행
*
*	\param x 마우스의 X 좌표
*	\param y 마우스의 Y 좌표
*	\param nFlags 마우스 왼쪽 버튼이 눌릴 때의 추가 정보 (MK_CONTROL...)
*/
void PSelector::LButtonDown(int x, int y, UINT nFlags)
{
	//std::cout << "PSelector::LButtonDown" << std::endl;

	// 선택 유형에 따라서, 선택 영역의 시작점을 리스트에 추가한다.
	switch (SelectToolType)
	{
		// 사각형 선택 툴이라면,
	case SELECT_TOOL_RECT:
		{
			// 선택 영역의 2D, 3D 경계점 리스트를 초기화 하고,
			SltRgn2DPtList.clear();
			SltRgn3DPtList.clear();

			// 2D 경계점을 리스트에 추가하고,
			SltRgn2DPtList.push_back(GVector2(x, y));
			SltRgn2DPtList.push_back(GVector2(x, y));
			SltRgn2DPtList.push_back(GVector2(x, y));
			SltRgn2DPtList.push_back(GVector2(x, y));

			// 3D 경계점을 리스트에 추가한다.
			GVector3 p = GetWorldCoord(x, y);
			SltRgn3DPtList.push_back(p);
			SltRgn3DPtList.push_back(p);
			SltRgn3DPtList.push_back(p);
			SltRgn3DPtList.push_back(p);
		}
		break;

		// 원 선택 툴이라면,
	case SELECT_TOOL_CIRCLE:
		{
			// 선택 영역의 2D, 3D 경계점 리스트를 초기화 한다.
			SltRgn2DPtList.clear();
			SltRgn3DPtList.clear();
		}
		break;

		// 올가미 선택 툴이라며,
	case SELECT_TOOL_LASSO:
		{
			// 선택 영역의 2D, 3D 경계점 리스트를 초기화 하고,
			SltRgn2DPtList.clear();
			SltRgn3DPtList.clear();

			// 2D 시작점을 경계점 리스트에 추가하고,
			SltRgn2DPtList.push_back(GVector2(x, y));

			// 3D 시작점을 경계점 리스트(렌더링 용)에 추가한다.
			GVector3 p = GetWorldCoord(x, y);
			SltRgn3DPtList.push_back(p);
		}
		break;

		// 다각형 선택 툴이라면,
	case SELECT_TOOL_POLY:
		{
			// 2D 선택점을 경계점 리스트를 추가하고,
			SltRgn2DPtList.push_back(GVector2(x, y));
			
			// 3D 선택점을 경계점 리스트를 추가하고,
			GVector3 p = GetWorldCoord(x, y);
			SltRgn3DPtList.push_back(p);

			// 다각형 경계의 시작점이라면 중복 추가한다.
			if (SltRgn2DPtList.size() == 1)
			{
				SltRgn2DPtList.push_back(GVector2(x, y));
				SltRgn3DPtList.push_back(p);
			}
		}
		break;

		// 브러쉬 툴이라면,
	case SELECT_TOOL_BRUSH:
		{
			// 마우스와 교차하는 삼각형, 정점, 교차점 등의 정보를 설정한다.
			SetMouseOverInfo(x, y);

			// 선택 메쉬를 찾아서,
			GMesh *pMesh = pEditor->GetEditMesh();
			if (pMesh != NULL && MouseOverVertIdx != -1)
			{
				// Shift와 Control중 어느 하나도 눌리지 않았다면, 이전 편집 객체 리스트를 지우고,
				if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
					pEditor->EditObjList.clear();	
								
				// 현재 선택 객체가 정점이라면 정점을 선택하고,
				if (SelectObjType == SELECT_VERT)
					SelectVertByBrush(pMesh);					
				
				// 현재 선택 객체가 삼각형이라면 삼각형을 선택하고,
				if (SelectObjType == SELECT_FACE)
					SelectFaceByBrush(pMesh);
			}
		}
		break;
	}
}

/*!
*	\brief 마우스가 움직일 때 호출되면 선택 영역을 정의한다.
*
*	\param x 마우스의 X 좌표
*	\param y 마우스의 Y 좌표
*	\param nFlags 마우스가 움직일 때, 눌려진 버튼의 정보 (MK_LBUTTON, MK_RBUTTON, MK_MBUTTON)
*/
void PSelector::MouseMove(int x, int y, UINT nFlags)
{
	//std::cout << "PSelector::MouseMove" << std::endl;

	CPoint MousePt[2];
	MousePt[0] = pEditor->MousePt[0];
	MousePt[1] = pEditor->MousePt[1];

	// 왼쪽 마우스 버튼이 눌린 상태라면,
	if (nFlags & MK_LBUTTON)
	{
		// 편집 툴의 핸들이 선택되지 않은 경우,
		switch (SelectToolType)
		{
			// 사각형 선택 툴을 사용할 경우,
		case SELECT_TOOL_RECT:	
			if (!SltRgn3DPtList.empty())
			{
				// 2D 경계점 리스트를 추가하고,
				SltRgn2DPtList[0] = GVector2(MousePt[0].x, MousePt[0].y);
				SltRgn2DPtList[1] = GVector2(MousePt[0].x, MousePt[1].y);
				SltRgn2DPtList[2] = GVector2(MousePt[1].x, MousePt[1].y);
				SltRgn2DPtList[3] = GVector2(MousePt[1].x, MousePt[0].y);

				// 3D 경계점 리스트를 추가한다.
				SltRgn3DPtList[0] = GetWorldCoord(MousePt[0].x, MousePt[0].y);
				SltRgn3DPtList[1] = GetWorldCoord(MousePt[0].x, MousePt[1].y);
				SltRgn3DPtList[2] = GetWorldCoord(MousePt[1].x, MousePt[1].y);
				SltRgn3DPtList[3] = GetWorldCoord(MousePt[1].x, MousePt[0].y);				
			}			
			break;

			// 원 선택 툴을 사용할 경우,
		case SELECT_TOOL_CIRCLE:
			{
				// 기존의 선택 영역을 지우고,
				SltRgn2DPtList.clear();
				SltRgn3DPtList.clear();

				// 원을 구성하는 2D/3D 경계점을 생성하여 리스트에 추가한다.
				int snum = 50;
				double r = dist(GVector2(MousePt[0].x, MousePt[0].y), GVector2(MousePt[1].x, MousePt[1].y));
				for (int i = 0; i < snum; ++i)
				{
					double theta = 2 * M_PI * (double) i / (snum - 1);
					int px = (int)(MousePt[0].x + r * cos(theta) + 0.5);
					int py = (int)(MousePt[0].y + r * sin(theta) + 0.5);

					SltRgn2DPtList.push_back(GVector2(px, py));
					SltRgn3DPtList.push_back(GetWorldCoord(px, py));
				}
			}
			break;

			// 올가미 선택 툴을 사용할 경우,
		case SELECT_TOOL_LASSO:	
			if (!SltRgn3DPtList.empty())
			{
				// 2D 경계점 리스트를 추가하고,
				SltRgn2DPtList.push_back(GVector2(MousePt[1].x, MousePt[1].y));

				// 3D 경계점 리스트를 추가한다.
				GVector3 p = GetWorldCoord(MousePt[1].x, MousePt[1].y);
				SltRgn3DPtList.push_back(p);
			}
			break;

			// 브러쉬 선택 툴이라면,
		case SELECT_TOOL_BRUSH:
			{
				// 마우스와 교차하는 삼각형, 정점, 교차점 등의 정보를 설정한다.
				SetMouseOverInfo(x, y);

				// 참조 메쉬를 구하고,
				GMesh *pMesh = pEditor->GetEditMesh();
				if (pMesh != NULL && MouseOverVertIdx != -1)
				{
					// 현재 선택 객체가 정점이라면, 정점을 선택하고,
					if (SelectObjType == SELECT_VERT)
						SelectVertByBrush(pMesh);					
					
					// 현재 선택 객체가 삼각형이라면, 삼각형을 선택하여 추가하고,
					if (SelectObjType == SELECT_FACE)
						SelectFaceByBrush(pMesh);
				}
			}
			break;			
		}		
	}
	// 왼쪽 마우스 버튼이 눌려지지 않은 상태라면,
	else
	{
		// 마우스 포인터와 교차하는 편집 핸들을 찾고,
		if (!SetMouseOverEditHandle(x, y))
			SetMouseOverInfo(x, y);	// 선택된 핸들이 없다면, 마우스와 교차하는 삼각형, 정점, 교차점 등의 정보를 설정한다.

		// (1) 다각형 선택 툴을 사용 중이고, (2) 선택과정이 진행 중이라면,
		if (SelectToolType == SELECT_TOOL_POLY && !SltRgn3DPtList.empty())
		{
			// 마지막 인덱스를 구하고,
			int LastIdx = (int)SltRgn2DPtList.size() - 1;

			// 2D 경계점 리스트를 변경하고,
			SltRgn2DPtList[LastIdx] = GVector2(MousePt[1].x, MousePt[1].y);

			// 3D 경계점 리스트를 변경한다.
			SltRgn3DPtList[LastIdx] = GetWorldCoord(MousePt[1].x, MousePt[1].y);
		}

		// 편집 메뉴에 따라서 
		switch (pEditor->EditToolType)
		{
			// 측지 경로를 편집 중이라면 
		case EDIT_GEODESIC_PATH:
			{
				// 편집 정점을 설정하고,
				std::vector<GMesh *> MeshList;
				GMesh *pMesh = pEditor->GetEditMesh();
				GetPreOrderMeshList(pEditor->pScene, MeshList, pMesh);

				// 측지 경로의 편집 정점을 설정한다.
				for (unsigned i = 0; i < MeshList.size(); ++i)
				{
					if (MeshList[i]->GetClassName() == "GGeoPath")
					{
						GGeoPath *pPath = (GGeoPath *)MeshList[i];
						pPath->EditPathVertIdx = -1;

						for (unsigned j = 0; j < pPath->PathVertIdx.size(); ++j)
						{
							if (MouseOverVertIdx == pPath->PathVertIdx[j])
							{
								pPath->EditPathVertIdx = MouseOverVertIdx;
								break;
							}
						}

						if (pPath->EditPathVertIdx != -1)
							break;
					}
				}
			}
			break;

			// Sculpting, smoothing 툴이 사용 중이라며,
		case EDIT_SCULPT:
		case EDIT_SMOOTH:
			{
				// 편집 영역을 찾는다.
				if (MouseOverVertIdx != -1)
					pEditor->FindEditWgtForSculpt();
			}
			break;
		}
	}	
}

/*!
*	\brief 새로운 객체를 선택한다.
*
*	\param x 마우스의 X 좌표
*	\param y 마우스의 Y 좌표
*	\param nFlags 마우스 왼쪽 버튼이 떼어질 때의 추가 정보 (MK_CONTROL...)
*/
void PSelector::LButtonUp(int x, int y, UINT nFlags)
{
	if (SltRgn2DPtList.empty())
	{
		// 브러신 선택 툴이라면,
		if (SelectToolType == SELECT_TOOL_BRUSH)
		{
			// 중복 객체를 제거하고, 편집 좌표계를 설정한다.
			pEditor->UniqueEditObjList();
			pEditor->SetEC();
		}
		return;
	}

	switch (SelectToolType)
	{
		// 선택 타입이 SELECT_TOOL_RECT 또는 SELECT_TOOL_LASSO 라면,
	case SELECT_TOOL_RECT:
	case SELECT_TOOL_CIRCLE:
	case SELECT_TOOL_LASSO:
		// 닫힌 선택 영역을 위해 시작점을 추가한다.
		SltRgn2DPtList.push_back(SltRgn2DPtList[0]);
		SltRgn3DPtList.push_back(SltRgn3DPtList[0]);
		break;

		// 선택 타입이 SELECT_TOOL_PLOY 이고
	case SELECT_TOOL_POLY:
		if (GetAsyncKeyState(VK_SPACE) & 0x8000) // 스페이스 키가 눌린 상태라면,
		{
			// 닫힌 선택 영역을 위해 현재점을 시작점으로 설정한다.
			int LastIdx = SltRgn2DPtList.size() - 1;
			SltRgn2DPtList[LastIdx] = SltRgn2DPtList[0];
			SltRgn3DPtList[LastIdx] = SltRgn3DPtList[0];
		}
		else
			return;
		break;
	}

	// 현재 선택된 메쉬를 구하고,
	GMesh *pPrevMesh = pEditor->GetEditMesh();

	// 선택 객체 유형에 따라(편집 툴의 핸들이 선택되지 않은 경우)
	switch (SelectObjType)
	{
	case SELECT_MESH:		// 메쉬를 선택하거나, 
		SelectMesh(x, y);
		break;

	case SELECT_BONE:		// 본 메쉬를 선택하거나,
		SelectBone(x, y);
		break;

	case SELECT_VERT:		// 메쉬의 정점을 선택하거나,
		SelectVert(x, y);
		break;

	case SELECT_FACE:		// 메쉬의 삼각형을 선택하거나, 
		SelectFace(x, y);
		break;

	case SELECT_FACE_AND_CUT:	// 메쉬의 삼각형을 선택하고 경계영역을 절단하거나,
		SelectFaceAndCutBndry(x, y);
		break;

	case SELECT_PT_ON_FACE:	// 메쉬의 삼각형과 마우스의 교차점을 선택하거나,
		SelectPtOnFace(x, y);
		break;

	case SELECT_FACE_GROUP:	// 삼각형 그룹을 선택하거나,
		SelectFaceGroup(x, y);
		break;

	case SELECT_CTRL_PT:	// 제어점을 선택하고,
		SelectCtrlPt(x, y);
		break;
	}

	// 새로 선택된 메쉬를 구하여, 서로 다르다면,
	GMesh *pCurrMesh = pEditor->GetEditMesh();
	if (pPrevMesh != pCurrMesh && pEditor->EditMeshDlg.m_hWnd != NULL)
	{
		// 작업창의 윈도우 핸들을 구하여,
		HWND hWnd = pEditor->EditMeshDlg.GetParent()->GetSafeHwnd();
		::PostMessage(hWnd, WM_UPDATE_SELECT, 0, 0);
	}

	// 선택된 객체를 이용하여 편집 툴의 편집 좌표계를 설정한다.
	pEditor->SetEC();

	// 선택 영역의 정점리스트를 초기화 한다.
	SltRgn2DPtList.clear();
	SltRgn3DPtList.clear();	
}

/*!
*	\brief 선택 툴의 선택 범위를 조절한다.
*
*	\param delta 마우스 휠의 변위
*/
void PSelector::MouseWheel(int delta)
{
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL)
		return;

	if (delta > 0)
	{
		pEditor->EditMeshDlg.m_BrushRadius += 10;
		if (pEditor->EditMeshDlg.m_BrushRadius > 1000)
			pEditor->EditMeshDlg.m_BrushRadius = 1000;
	}
	else
	{
		pEditor->EditMeshDlg.m_BrushRadius -= 10;
		if (pEditor->EditMeshDlg.m_BrushRadius < 1)
			pEditor->EditMeshDlg.m_BrushRadius = 2;		
	}

	pEditor->EditMeshDlg.UpdateData(FALSE);
	pEditor->EditMeshDlg.pDoc->UpdateAllViews(NULL);
}

/*!
*	\brief 선택 툴의 상태를 초기화한다.
*/
void PSelector::Reset()
{
	SelectObjType = SELECT_MESH;
	SelectToolType = SELECT_TOOL_RECT;
	SltRgn2DPtList.clear();
	SltRgn3DPtList.clear();
	MouseOverGroupIdx = -1;
	MouseOverFaceIdx = -1;
	MouseOverVertIdx = -1;
	MouseOverPtCoords.Set(0.0, 0.0, 0.0);
}

/*!
*	\brief 편집 객체, 에디터 그리고 선택 영역을 렌더링 한다.
*/
void PSelector::Render()
{
	switch (SelectToolType)
	{
		// 사각형 선택 툴이 진행 중이라면,
	case SELECT_TOOL_RECT:
		RenderRectTool();
		break;

		// 원 선택 툴이 진행 중이라면,
	case SELECT_TOOL_CIRCLE:
		RenderCircleTool();
		break;

		// 올가미 선택 툴이 진행 중이라면,
	case SELECT_TOOL_LASSO:
		RenderLassoTool();
		break;

		// 다각형 선택 툴이 진행 중이라면,
	case SELECT_TOOL_POLY:
		RenderPolyTool();
		break;

		// 브러쉬 선택 툴이 진행 중이라면,
	case SELECT_TOOL_BRUSH:
		RenderBrushTool();
		break;
	}
	
	// 정점을 선택 중이라면 마우스와 교차하는 정점을 표시한다.
	if (SelectObjType == SELECT_VERT || pEditor->EditToolType == EDIT_GEODESIC_PATH)
		RenderMouseOverVert();

	// 삼각형을 선택 중이라면 마우스와 교차하는 삼각형을 표시한다.
	if (SelectObjType == SELECT_FACE)
		RenderMouseOverFace();

	if (SelectObjType == SELECT_FACE_GROUP)
		RenderMouseOverFaceGroup();

	if (SelectObjType == SELECT_PT_ON_FACE)
		RenderMouseOverPtOnFace();	

	if (pEditor->EditToolType == EDIT_SCULPT || pEditor->EditToolType == EDIT_SMOOTH)
		RenderSculptTool();
}

/*!
*	\brief 사각형 선택 툴을 표시한다.
*/
void PSelector::RenderRectTool()
{
	if (SltRgn3DPtList.empty())
		return;

	// 조명을 끄고 경계 색상과 경계선의 패턴을 지정한다.
	glDisable(GL_LIGHTING);
	glColor3f(0.2627, 1.0, 0.64);
	glEnable(GL_LINE_STIPPLE); 
	glLineStipple(2, 0xf0f0);

	// 현재의 사각형 선택 영역을 표시한다.
	glBegin(GL_LINE_LOOP);
	{
		glVertex3f(SltRgn3DPtList[0][0], SltRgn3DPtList[0][1], SltRgn3DPtList[0][2]);
		glVertex3f(SltRgn3DPtList[1][0], SltRgn3DPtList[1][1], SltRgn3DPtList[1][2]);
		glVertex3f(SltRgn3DPtList[2][0], SltRgn3DPtList[2][1], SltRgn3DPtList[2][2]);
		glVertex3f(SltRgn3DPtList[3][0], SltRgn3DPtList[3][1], SltRgn3DPtList[3][2]);
	}			
	glEnd();

	// 조명을 활성화하고 경계선의 패턴을 비활성화 한다.
	glEnable(GL_LIGHTING);
	glDisable(GL_LINE_STIPPLE);
}

/*!
*	\brief 원 선택 툴을 표시한다.
*/
void PSelector::RenderCircleTool()
{
	if (SltRgn3DPtList.empty())
		return;

	// 조명을 끄고 경계 색상과 경계선의 패턴을 지정한다.
	glDisable(GL_LIGHTING);
	glColor3f(0.2627, 1.0, 0.64);
	// 	glEnable(GL_LINE_STIPPLE); 
	// 	glLineStipple(2, 0xf0f0);

	// 현재 원 선택 영역을 표시한다.
	glBegin(GL_LINE_LOOP);
	{
		int NumPt = SltRgn3DPtList.size();
		for (int i = 0; i < NumPt; ++i)
			glVertex3f(SltRgn3DPtList[i][0], SltRgn3DPtList[i][1], SltRgn3DPtList[i][2]);
	}			
	glEnd();

	// 원의 중심과 현재 마우스 위치를 연결하는 직선을 표시한다.
	GVector3 p = GetWorldCoord(pEditor->MousePt[0].x, pEditor->MousePt[0].y);
	GVector3 q = GetWorldCoord(pEditor->MousePt[1].x, pEditor->MousePt[1].y);

	glColor3f(1.0f, 0.0f, 0.0f);
	glPointSize(3.0);
	glBegin(GL_POINTS);
	glVertex3dv(p.V);
	glVertex3dv(q.V);
	glEnd();

	glColor3f(0.7f, 0.7f, 0.7f);
	glEnable(GL_LINE_STIPPLE); 
	glLineStipple(2, 0xf0f0);

	glBegin(GL_LINES);
	glVertex3dv(p.V);
	glVertex3dv(q.V);
	glEnd();

	glDisable(GL_LINE_STIPPLE);

	// 조명을 활성화하고 경계선의 패턴을 비활성화 한다.
	glEnable(GL_LIGHTING);	
}

/*!
*	\brief 올가미 선택 툴을 표시한다.
*/
void PSelector::RenderLassoTool()
{
	if (SltRgn3DPtList.empty())
		return;

	// 조명을 끄고 경계 색상과 경계선의 패턴을 지정한다.
	glDisable(GL_LIGHTING);
	glColor3f(0.2627, 1.0, 0.64);
// 	glEnable(GL_LINE_STIPPLE); 
// 	glLineStipple(2, 0xf0f0);

	// 현재 올가미 선택 영역을 표시한다.
	glBegin(GL_LINE_LOOP);
	{
		int NumPt = SltRgn3DPtList.size();
		for (int i = 0; i < NumPt; ++i)
			glVertex3f(SltRgn3DPtList[i][0], SltRgn3DPtList[i][1], SltRgn3DPtList[i][2]);
	}			
	glEnd();

	// 조명을 활성화하고 경계선의 패턴을 비활성화 한다.
	glEnable(GL_LIGHTING);
//	glDisable(GL_LINE_STIPPLE);
}

/*!
*	\brief 다각형 선택 툴을 표시한다.
*/
void PSelector::RenderPolyTool()
{
	if (SltRgn3DPtList.empty())
		return;

	// 조명을 끄고 경계 색상과 경계선의 패턴을 지정한다.
	glDisable(GL_LIGHTING);
	glColor3f(0.2627, 1.0, 0.64);
	glEnable(GL_LINE_STIPPLE); 
	glLineStipple(2, 0xf0f0);

	// 다각형 선택 영역을 표시한다.
	glBegin(GL_LINE_LOOP);
	{
		int ptnum = SltRgn3DPtList.size();
		for (int i = 0; i < ptnum; ++i)
			glVertex3f(SltRgn3DPtList[i][0], SltRgn3DPtList[i][1], SltRgn3DPtList[i][2]);
	}			
	glEnd();

	// 조명을 활성화하고 경계선의 패턴을 비활성화 한다.
	glEnable(GL_LIGHTING);
	glDisable(GL_LINE_STIPPLE);
}

/*!
*	\brief 브러시 선택 툴을 표시한다.
*/
void PSelector::RenderBrushTool()
{
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL || MouseOverVertIdx == -1)
		return;

	static GLUquadric *pCricle = NULL;
	if (pCricle == NULL)
	{
		pCricle = gluNewQuadric();
		gluQuadricDrawStyle(pCricle, GLU_LINE);
	}

	// 조명과 깊이 테스트를 비활성화하고 블렌딩을 활성화한다.
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	
	// 월드 좌표계에서
	glPushMatrix();
	{
		// 브러시의 크기를 구하고,
		double BrushRadius = pMesh->BBox.GetDiagonal() * (pEditor->EditMeshDlg.m_BrushRadius / 1000.0);

		// 마우스 포인터와 가장 가까운 정점의 좌표계를 구하고,
		GPoint3 p = pMesh->Verts[MouseOverVertIdx].P;
		GVector3 n = pMesh->Verts[MouseOverVertIdx].N;
		GQuater q;
		q.SetFromArcBall(GVector3(0.0, 1.0, 0.0), n);
		GTransf MC = pMesh->MC * GTransf(cast_vec3(p), q);

		// 그 좌표계로 변환한 후,
		glMultMatrixd(cast_mat(MC).M);

		// 원판을 그리고,
		glPushMatrix();
		{
			glRotated(-90.0, 1.0, 0.0, 0.0);
			glColor4f(0.0f, 0.0f, 1.0f, 0.6f);
			gluDisk(pCricle, BrushRadius * 0.3, BrushRadius, 30, 5);
		}				
		glPopMatrix();

		glLineWidth(2.0);
		glBegin(GL_LINES);
		{
			// x 축을 그리고,
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(-BrushRadius, 0.0, 0.0);
			glVertex3f(BrushRadius, 0.0, 0.0);

			// z 축을 그린다.
			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0.0, 0.0, -BrushRadius);
			glVertex3f(0.0, 0.0, BrushRadius);
		}				
		glEnd();
		glLineWidth(1.0);

		// 원점을 그린다.
		glPointSize(8.0);
		glBegin(GL_POINTS);
		{
			glVertex3f(0.0, 0.0, 0.0);
		}
		glEnd();
		glPointSize(1.0);
	}
	glPopMatrix();	// 월드 좌표계로 복원한다.


	// 조명을 활성화하고 경계선의 패턴을 비활성화 한다.
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

/*!
*	\brief 현재 마우스 포인터와 교차하는 삼각형(사각형)을 렌더링 한다.
*/
void PSelector::RenderMouseOverFace()
{
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL || MouseOverFaceIdx == -1)
		return;

	if ((GetAsyncKeyState(VK_MENU) & 0x8000))
		return;

	// 조명과 깊이 테스트를 끄고,
	glDisable(GL_LIGHTING);
	//glDisable(GL_DEPTH_TEST);

	// 월드 좌표계에서
	glPushMatrix();
	{
		// 선택 객체의 모델링 좌표계로 변환하고,
		glMultMatrixd(cast_mat(pMesh->MC).M);

		// 사각형 메쉬라면,
		if (pMesh->FaceType == FACE_QUAD)
		{
			// 짝수번째 삼각형과 홀수번째 삼각형을 구분하여,
			GFace *pFace0, *pFace1;
			if (MouseOverFaceIdx % 2 == 0)	// 짝수 번째 삼각형이라면,
			{
				pFace0 = &(pMesh->Faces[MouseOverFaceIdx]);
				pFace1 = &(pMesh->Faces[MouseOverFaceIdx + 1]);
			}
			else	// 홀수번째 삼각형 이라면,
			{
				pFace0 = &(pMesh->Faces[MouseOverFaceIdx - 1]);
				pFace1 = &(pMesh->Faces[MouseOverFaceIdx]);
			}

			// 사각형 정점의 인덱스를 구하고,
			int vidx0 = pFace0->vIdx[0];
			int vidx1 = pFace0->vIdx[1];
			int	vidx2 = pFace0->vIdx[2];
			int	vidx3 = pFace1->vIdx[2];

			// 선택된 사각형을 그린다.
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//glColor4f(0.2627, 1.0, 0.64, 1.0);
			glColor4f(1.0, 0.0, 0.0, 0.5);
			glBegin(GL_POLYGON);
			{
				glVertex3d(pMesh->Verts[vidx0].P[0], pMesh->Verts[vidx0].P[1], pMesh->Verts[vidx0].P[2]);
				glVertex3d(pMesh->Verts[vidx1].P[0], pMesh->Verts[vidx1].P[1], pMesh->Verts[vidx1].P[2]);
				glVertex3d(pMesh->Verts[vidx2].P[0], pMesh->Verts[vidx2].P[1], pMesh->Verts[vidx2].P[2]);
				glVertex3d(pMesh->Verts[vidx3].P[0], pMesh->Verts[vidx3].P[1], pMesh->Verts[vidx3].P[2]);
			}
			glEnd();
			glDisable(GL_BLEND);
		}
		else	// 삼각형 메쉬라면,
		{
			int vidx0 = pMesh->Faces[MouseOverFaceIdx].vIdx[0];
			int vidx1 = pMesh->Faces[MouseOverFaceIdx].vIdx[1];
			int vidx2 = pMesh->Faces[MouseOverFaceIdx].vIdx[2];

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//glColor4f(0.2627, 1.0, 0.64, 0.5);
			glColor4f(1.0, 0.0, 0.0, 0.5);
			glBegin(GL_POLYGON);
			{
				glVertex3d(pMesh->Verts[vidx0].P[0], pMesh->Verts[vidx0].P[1], pMesh->Verts[vidx0].P[2]);
				glVertex3d(pMesh->Verts[vidx1].P[0], pMesh->Verts[vidx1].P[1], pMesh->Verts[vidx1].P[2]);
				glVertex3d(pMesh->Verts[vidx2].P[0], pMesh->Verts[vidx2].P[1], pMesh->Verts[vidx2].P[2]);
			}
			glEnd();
			glDisable(GL_BLEND);
		}
	}
	glPopMatrix();		// 월드 좌표계로 복원한다.

	// 조명과 깊이 테스트를 다시 켠다.
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);	
}

/*!
*	\brief 현재 마우스 포인터와 교차(가장 가까운) 정점을 렌더링 한다.
*/
void PSelector::RenderMouseOverVert()
{
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL || MouseOverVertIdx == -1)
		return;

	if ((GetAsyncKeyState(VK_MENU) & 0x8000))
		return;

	// 블렌딩 모드로 진입하고,
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// 월드 좌표계에서
	glPushMatrix();
	{
		// 선택 객체의 모델링 좌표계로 변환하고,
		glMultMatrixd(cast_mat(pMesh->MC).M);

		// 선택된 정점을 그린다.
		GVertex *v = &(pMesh->Verts[MouseOverVertIdx]);
		
		glColor3f(1.0, 0.0, 0.0);
		glPointSize(8.0);
		glBegin(GL_POINTS);
			glVertex3dv(v->P.V);
		glEnd();
		glPointSize(1.0);	
	}
	glPopMatrix();		// 월드 좌표계로 복원한다.

	// 블렌딩 모드에서 나온다.
	glDisable(GL_BLEND);
	glDisable(GL_POINT_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

/*!
*	\brief 현재 마우스 포인터와 교차(가장 가까운) 삼각형 그룹을 렌더링 한다.
*/
void PSelector::RenderMouseOverFaceGroup()
{
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL || MouseOverGroupIdx == -1 || pMesh->GrpFaces.empty())
		return;

	if ((GetAsyncKeyState(VK_MENU) & 0x8000))
		return;

	// 조명과 깊이 테스트를 끄고,
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// 월드 좌표계에서
	glPushMatrix();
	{
		// 선택 객체의 모델링 좌표계로 변환하고,
		glMultMatrixd(cast_mat(pMesh->MC).M);

		std::map<std::string, std::vector<int>>::iterator it = pMesh->GrpFaces.begin();
		for (int i = 0; i < MouseOverGroupIdx; ++i)
			it++;

		glColor4f(0.2627, 1.0, 0.64, 0.5);
		glBegin(GL_TRIANGLES);
		int NumFace = (int)it->second.size();
		for (int i = 0; i < NumFace; ++i)
		{
			int fidx = it->second[i];
			int *vidx = pMesh->Faces[fidx].vIdx;
			glVertex3dv(pMesh->Verts[vidx[0]].P.V);
			glVertex3dv(pMesh->Verts[vidx[1]].P.V);
			glVertex3dv(pMesh->Verts[vidx[2]].P.V);
		}
		glEnd();		
	}
	glPopMatrix();		// 월드 좌표계로 복원한다.

	// 조명과 깊이 테스트를 다시 켠다.
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

/*!
*	\brief 현재 마우스 포인터와 교차하는 삼각형 위의 점을 렌더링 한다.
*/
void PSelector::RenderMouseOverPtOnFace()
{
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL || MouseOverFaceIdx == -1)
		return;

	if ((GetAsyncKeyState(VK_MENU) & 0x8000))
		return;

	if (MouseOverPtCoords == GVector3(0.0, 0.0, 0.0))
		return;

	// 월드 좌표계에서
	glPushMatrix();
	{
		// 선택 객체의 모델링 좌표계로 변환하고,
		glMultMatrixd(cast_mat(pMesh->MC).M);

		// 마우스와 교차하는 삼각형의 세 정점의 좌표를 구하고,
		int vidx0 = pMesh->Faces[MouseOverFaceIdx].vIdx[0];
		int vidx1 = pMesh->Faces[MouseOverFaceIdx].vIdx[1];
		int vidx2 = pMesh->Faces[MouseOverFaceIdx].vIdx[2];
		GPoint3 p0 = pMesh->Verts[vidx0].P;
		GPoint3 p1 = pMesh->Verts[vidx1].P;
		GPoint3 p2 = pMesh->Verts[vidx2].P;

		// 삼각형 위의 교차점을 계산하여,
		GPoint3 pt = p2 + (p0 - p2) * MouseOverPtCoords[0] + (p1 - p2) * MouseOverPtCoords[1];
		
		// 블렌딩 모드로 진입하고,
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(0.2627, 1.0, 0.64, 0.5);
		glPointSize(8.0);
		glBegin(GL_POINTS);
		{
			glVertex3dv(pt.V);
		}
		glEnd();

		// 블렌딩 모드에서 나온다.
		glDisable(GL_BLEND);
		glDisable(GL_POINT_SMOOTH);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
	}
	glPopMatrix();		// 월드 좌표계로 복원한다.
}

/*!
*	\brief Sculpt 툴을 렌더링한다.
*/
void PSelector::RenderSculptTool()
{
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL || MouseOverFaceIdx == -1)
		return;

	if ((GetAsyncKeyState(VK_MENU) & 0x8000))
		return;

	if (MouseOverPtCoords == GVector3(0.0, 0.0, 0.0))
		return;

	static GLUquadric *pCylinder = NULL;
	if (pCylinder == NULL)
		pCylinder = gluNewQuadric();

	static GLUquadric *pCricle = NULL;
	if (pCricle == NULL)
	{
		pCricle = gluNewQuadric();
		gluQuadricDrawStyle(pCricle, GLU_LINE);
	}

	// 마우스와 교차하는 삼각형의 세 정점의 좌표를 구하고,
	int *vidx = pMesh->Faces[MouseOverFaceIdx].vIdx;
	GPoint3 p0 = pMesh->Verts[vidx[0]].P;
	GPoint3 p1 = pMesh->Verts[vidx[1]].P;
	GPoint3 p2 = pMesh->Verts[vidx[2]].P;
	GVector3 n0 = pMesh->Verts[vidx[0]].N;
	GVector3 n1 = pMesh->Verts[vidx[1]].N;
	GVector3 n2 = pMesh->Verts[vidx[2]].N;

	// 삼각형 위의 교차점을 계산하여,
	GPoint3 p = p2 + (p0 - p2) * MouseOverPtCoords[0] + (p1 - p2) * MouseOverPtCoords[1];
	GVector3 n = n0 * MouseOverPtCoords[0] + n1 * MouseOverPtCoords[1] + n2 * MouseOverPtCoords[2];

	GQuater q;
	q.SetFromArcBall(GVector3(0.0, 1.0, 0.0), n);
	GTransf MC = pMesh->MC * GTransf(cast_vec3(p), q);

	// 월드 좌표계에서
	glPushMatrix();
	{
		// 선택 객체의 모델링 좌표계로 변환하고,
		glMultMatrixd(cast_mat(MC).M);

		double scl = pMesh->BBox.GetDiagonal() * 0.009;

		if (pEditor->EditToolType == EDIT_SCULPT)
		{
			if ((GetAsyncKeyState(VK_SHIFT) & 0x8000))
			{
				GMaterial mtl = get_material(1, 1, 0);
				mtl.Enable(false);

				glPushMatrix();
				glRotatef(90.0, 1.0, 0.0, 0.0);
				glTranslatef(0.0, 0.0, -1.0 * scl);
				glutSolidCone(0.5 * scl, 1.0 * scl, 8, 4);
				glTranslatef(0.0, 0.0, -1.0 * scl);
				gluCylinder(pCylinder, 0.3 * scl, 0.3 * scl, 1.0 * scl, 8, 2);			
				glPopMatrix();
			}
			else 
			{
				GMaterial mtl = get_material(0.2627, 1.0, 0.64);
				mtl.Enable(false);

				glPushMatrix();
				glRotatef(-90.0, 1.0, 0.0, 0.0);
				gluCylinder(pCylinder, 0.3 * scl, 0.3 * scl, 1.0 * scl, 8, 2);
				glTranslatef(0.0, 0.0, 1.0 * scl);
				glutSolidCone(0.5 * scl, 1.0 * scl, 8, 4);
				glPopMatrix();
			}
		}
		else if (pEditor->EditToolType == EDIT_SMOOTH)
		{
			// 조명과 깊이 테스트를 비활성화하고 블렌딩을 활성화한다.
			glDisable(GL_LIGHTING);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POINT_SMOOTH);

			// 브러시의 크기를 구하고,
			double Radius = pMesh->BBox.GetDiagonal() * (pEditor->EditMeshDlg.m_EditRgnDist / 1000.0);

			// 원판을 그리고,
			glPushMatrix();
			{
				glRotated(-90.0, 1.0, 0.0, 0.0);
				//glColor4f(0.0f, 0.0f, 1.0f, 0.6f);
				glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
				gluDisk(pCricle, Radius * 0.3, Radius, 30, 5);
			}				
			glPopMatrix();

			glLineWidth(2.0);
			glBegin(GL_LINES);
			{
				// x 축을 그리고,
				glColor3f(1.0, 0.0, 0.0);
				glVertex3f(-Radius, 0.0, 0.0);
				glVertex3f(Radius, 0.0, 0.0);

				// z 축을 그린다.
				glColor3f(0.0, 0.0, 1.0);
				glVertex3f(0.0, 0.0, -Radius);
				glVertex3f(0.0, 0.0, Radius);
			}				
			glEnd();
			glLineWidth(1.0);

			// 원점을 그린다.
			glPointSize(8.0);
			glColor3f(0.7, 0.7, 0.7);
			glBegin(GL_POINTS);
			{
				glVertex3f(0.0, 0.0, 0.0);
			}
			glEnd();
			glPointSize(1.0);


			// 조명을 활성화하고 경계선의 패턴을 비활성화 한다.
			glDisable(GL_LINE_SMOOTH);
			glDisable(GL_BLEND);
			glEnable(GL_LIGHTING);
			glEnable(GL_DEPTH_TEST);
		}
	}
	glPopMatrix();		// 월드 좌표계로 복원한다.
}

/*!
*	\brief 참조 메쉬의 정점과 삼각형 선택을 위해 렌더링 한다.
*/
void GetColorId(unsigned int &r, unsigned int &g, unsigned int &b)
{
	b++;
	if (b > 255)
	{
		b = 0;
		g++;
		if (g > 255)
		{
			g = 0;
			r++;
		}
	}
}

/*!
*	\brief 선택된 객체의 정점, 삼각형 등의 선택을 위해서 선택된 객체의 삼각형을 서로 다른 색상(아이디)으로 렌더링 한다.
*	\note 마지막 수정일(2016/01/31)
*/
void PSelector::RenderForSelect()
{
	// 선택된 참조 객체를 구하고,
	GMesh *pMesh = pEditor->GetEditMesh();

	// 선택된 객체가 없다면 리턴한다.
	if (pMesh == NULL)
		return;

	// 색상 버퍼와 깊이 버퍼를 초기화한 후,
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 장면의 관측 공간 및 투영 방법을 설정하고,
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	pEditor->pScene->SetupViewFrustum();

	// 장면의 관측 변환을 수행한다.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	pEditor->pScene->SetupViewTransform();

	// 현재 편집 중이라면,
	if ((pEditor->EditToolType >= EDIT_TRA_X && pEditor->EditToolType <= EDIT_TRA_XYZ) ||
		(pEditor->EditToolType >= EDIT_ROT_X && pEditor->EditToolType <= EDIT_ROT_Z) ||
		(pEditor->EditToolType >= EDIT_SCL_X && pEditor->EditToolType <= EDIT_SCL_XYZ))
	{
		// 편집 툴의 각 핸들을 서로 다른 색상 (255, 255, 핸들 아이디)으로 렌더링하고 리턴한다.
		RenderForSelectHandle();
		return;
	}

	// 삼각형을 선택을 위해 각 삼각형을 서로 다른 색상으로 렌더링 한다.
	RenderForSelectFace();

	// 편집 툴의 각 핸들을 서로 다른 색상으로 렌더링 한다. 색상 = (255, 255, 핸들 아이디)
	RenderForSelectHandle();
}

/*!
*	\brief 메쉬의 삼각형 선택을 위해 각 삼각형을 다른 색상으로 렌더링 한다.
*/
void PSelector::RenderForSelectFace()
{
	// 선택 객체 타입이 정점, 삼각형, 삼각형 그룹과 같지 않으면 리턴한다.
	if (SelectObjType != SELECT_VERT && 
		SelectObjType != SELECT_FACE && 
		SelectObjType != SELECT_FACE_AND_CUT && 
		SelectObjType != SELECT_PT_ON_FACE && 
		SelectObjType != SELECT_FACE_GROUP && 
		//////////////////////////////////////////////////////////////////////////////////////////////
		pEditor->EditToolType != EDIT_GEODESIC_PATH && 
		pEditor->EditToolType != EDIT_SCULPT &&
		pEditor->EditToolType != EDIT_SMOOTH)
		return;

	GMesh *pMesh = pEditor->GetEditMesh();

	// 조명을 비활성화 하고, 색상 삼각형 렌더링 시작.
	glDisable(GL_LIGHTING);

	// 선택된 메쉬의 삼각형을 서로다른 색상의 아이디를 렌더링 한다.
	unsigned int r = 0, g = 0, b = 0;

	// 월드 좌표계에서
	glPushMatrix();
	{
		// 메쉬의 모델링 좌표계로 변환하고,
		glMultMatrixd(cast_mat(pMesh->MC).M);

		// 삼각형의 개수를 구하고,
		int NumFace = pMesh->GetNumFace();

		// 삼각형 색상 디스플레이 리스트가 있다면, 
		if (pMesh->DispFaceColMesh != 0)
		{
			// 디스플레이 리스트를 호출하고,
			glCallList(pMesh->DispFaceColMesh);
		}
		else
		{
			if (pMesh->GrpFaces.empty())	// 삼각형 그룹이 정의되지 않았다면,
			{
				// 삼각형 색상으로 렌더링된 디스플레이 리스트를 생성한다.
				pMesh->DispFaceColMesh = glGenLists(1);
				glNewList(pMesh->DispFaceColMesh, GL_COMPILE_AND_EXECUTE);
				{
					// 삼각형을 서로 다른 색상(아이디)으로 렌더링한 후,
					glBegin(GL_TRIANGLES);
					for (int i = 0; i < NumFace; ++i)
					{
						int *vidx = pMesh->Faces[i].vIdx;
						GetColorId(r, g, b);	// (0, 0, 1) 부터 시작됨...
						glColor3ub(r, g, b);
						glVertex3dv(pMesh->Verts[vidx[0]].P.V);
						glVertex3dv(pMesh->Verts[vidx[1]].P.V);
						glVertex3dv(pMesh->Verts[vidx[2]].P.V);
					}
					glEnd();
				}
				glEndList();				
			}
			else	// 삼각형 그룹이 정의되었다면,
			{
				pMesh->DispFaceColMesh = glGenLists(1);
				glNewList(pMesh->DispFaceColMesh, GL_COMPILE_AND_EXECUTE);
				{
					// 삼각형을 그룹별로 렌더링 한다.
					std::map<std::string, std::vector<int>>::iterator it;
					for (it = pMesh->GrpFaces.begin(); it != pMesh->GrpFaces.end(); ++it)
					{
						int NumGrpFace = it->second.size();
						glBegin(GL_TRIANGLES);
						for (int i = 0; i < NumGrpFace; ++i)
						{
							int fidx = it->second[i];
							int *vidx = pMesh->Faces[fidx].vIdx;
							GetColorId(r, g, b);	// (0, 0, 1) 부터 시작됨...
							glColor3ub(r, g, b);
							glVertex3dv(pMesh->Verts[vidx[0]].P.V);
							glVertex3dv(pMesh->Verts[vidx[1]].P.V);
							glVertex3dv(pMesh->Verts[vidx[2]].P.V);
						}	
						glEnd();
					}
				}
				glEndList();				
			}
		}		
	}
	// 월드 좌표계로 복원한다.
	glPopMatrix();

	// 조명을 활성화 한다.
	glEnable(GL_LIGHTING);
}

/*!
*	\brief 편집 툴의 핸들 선택을 위해 각 툴의 핸들을 다른 색상으로 렌더링 한다.
*/
void PSelector::RenderForSelectHandle()
{
	// 편집 툴의 핸들의 렌더링을 위한 초기 값을 설정하고,
	unsigned char r = 255, g = 255;

	// 깊이 테스트를 비활성화 한다.
	glDisable(GL_DEPTH_TEST);

	// 조명을 비활성화 하고, 색상 핸들 렌더링 시작.
	glDisable(GL_LIGHTING);

	// 월드 좌표계에서
	glPushMatrix();
	{
		// 편집 툴의 모델링 좌표계로 변환하여,
		glMultMatrixd(cast_mat(pEditor->EC).M);

		// 편집 툴의 상대적 크기를 계산하고,
		double len = pEditor->GetEditToolSize();

		// 이동 툴의 핸들을 렌더링 하고,
		if (pEditor->EditToolType >= EDIT_TRA && pEditor->EditToolType <= EDIT_TRA_XYZ)
		{
			// x축과 핸들,
			glColor3ub(r, g, EDIT_TRA_X);
			glBegin(GL_LINES);
			glVertex3d(len * 0.4, 0.0, 0.0);
			glVertex3d(len, 0.0f, 0.0f);
			glEnd();
			glPushMatrix();
			glTranslated(len, 0.0, 0.0);
			glRotated(90.0, 0.0, 1.0, 0.0);
			glutSolidCone(len * 0.1, len * 0.3, 10, 10);
			glPopMatrix();

			// y축과 핸들,
			glColor3ub(r, g, EDIT_TRA_Y);
			glBegin(GL_LINES);
			glVertex3d(0.0, len * 0.4, 0.0);
			glVertex3d(0.0f, len, 0.0f);
			glEnd();
			glPushMatrix();
			glTranslated(0.0, len, 0.0);
			glRotated(-90.0, 1.0, 0.0, 0.0);
			glutSolidCone(len * 0.1, len * 0.3, 10, 10);
			glPopMatrix();

			// z축과 핸들을 렌더링한 후,
			glColor3ub(r, g, EDIT_TRA_Z);
			glBegin(GL_LINES);
			glVertex3d(0.0, 0.0, len * 0.4);
			glVertex3d(0.0, 0.0, len);
			glEnd();
			glPushMatrix();
			glTranslated(0.0, 0.0, len);
			glutSolidCone(len * 0.1, len * 0.3, 10, 10);
			glPopMatrix();

			// xy 핸들
			glColor3ub(r, g, EDIT_TRA_XY);
			glBegin(GL_POLYGON);
			{
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(len * 0.4, 0.0, 0.0);
				glVertex3d(len * 0.4, len * 0.4, 0.0);
				glVertex3d(0.0, len * 0.4, 0.0);
			}
			glEnd();

			// yz 핸들
			glColor3ub(r, g, EDIT_TRA_YZ);
			glBegin(GL_POLYGON);
			{
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(0.0, len * 0.4, 0.0);
				glVertex3d(0.0, len * 0.4, len * 0.4);
				glVertex3d(0.0, 0.0, len * 0.4);
			}
			glEnd();

			// zx 핸들
			glColor3ub(r, g, EDIT_TRA_ZX);
			glBegin(GL_POLYGON);
			{
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(0.0, 0.0, len * 0.4);
				glVertex3d(len * 0.4, 0.0, len * 0.4);
				glVertex3d(len * 0.4, 0.0, 0.0);			
			}
			glEnd();

			// xyz 핸들
			glColor3ub(r, g, EDIT_TRA_XYZ);
			glPushMatrix();
			{
				glTranslated(len * 0.1, len * 0.1, len * 0.1);
				glutSolidCube(len * 0.2);
			}
			glPopMatrix();
		}

		// 회전 툴의 핸들을 렌더링 하고,
		if (pEditor->EditToolType >= EDIT_ROT && pEditor->EditToolType <= EDIT_ROT_Z)
		{
			// 현재 좌표계: 편집 툴의 좌표계
			double modelview[16];
			glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
			
			// 관측 방향의 테두리
			GQuater qt;
			qt.SetFromMatrix(modelview, true);
			qt = inv(qt);

			// 관측방향의 테두리 뒤쪽 점들을 제거하기 위한 벡터 
			GVector3 view_axis(0.0, 0.0, 1.0);
			view_axis = qt * view_axis;

			glLineWidth(20.0);
			int snum = 50;

			// x축과 핸들,
			glColor3ub(r, g, EDIT_ROT_X);
			for (int i = 0; i < snum; ++i)
			{
				double theta1 = (double)i / (double)(snum - 1) * M_PI * 2.0;
				GVector3 p1(0.0, len * cos(theta1), len * sin(theta1));
				if (view_axis * p1 > 0)
				{
					double theta2 = (double)(i + 1) / (double)(snum - 1) * M_PI * 2.0;
					GVector3 p2(0.0, len * cos(theta2), len * sin(theta2));
					glBegin(GL_LINES);
					glVertex3d(p1[0], p1[1], p1[2]);
					glVertex3d(p2[0], p2[1], p2[2]);
					glEnd();
				}
			}	

			// y축과 핸들,
			glColor3ub(r, g, EDIT_ROT_Y);
			for (int i = 0; i < snum; ++i)
			{
				double theta1 = (double)i / (double)(snum - 1) * M_PI * 2.0;
				GVector3 p1(len * sin(theta1), 0.0, len * cos(theta1));
				if (view_axis * p1 > 0)
				{
					double theta2 = (double)(i + 1) / (double)(snum - 1) * M_PI * 2.0;
					GVector3 p2(len * sin(theta2), 0.0, len * cos(theta2));
					glBegin(GL_LINES);
					glVertex3d(p1[0], p1[1], p1[2]);
					glVertex3d(p2[0], p2[1], p2[2]);
					glEnd();
				}
			}

			// z축과 핸들을 렌더링한 후,
			glColor3ub(r, g, EDIT_ROT_Z);
			for (int i = 0; i < snum; ++i)
			{
				double theta1 = (double)i / (double)(snum - 1) * M_PI * 2.0;
				GVector3 p1(len * cos(theta1), len * sin(theta1), 0.0);
				if (view_axis * p1 > 0)
				{
					double theta2 = (double)(i + 1) / (double)(snum - 1) * M_PI * 2.0;
					GVector3 p2(len * cos(theta2), len * sin(theta2), 0.0);
					glBegin(GL_LINES);
					glVertex3d(p1[0], p1[1], p1[2]);
					glVertex3d(p2[0], p2[1], p2[2]);
					glEnd();
				}
			}				

			glLineWidth(1.0);
		}

		// 스케일 툴의 핸들을 렌더링 하고,
		if (pEditor->EditToolType >= EDIT_SCL && pEditor->EditToolType <= EDIT_SCL_XYZ)
		{
			// x축과 핸들,
			glColor3ub(r, g, EDIT_SCL_X);
			glBegin(GL_LINES);
			glVertex3d(len * 0.4, 0.0, 0.0);
			glVertex3d(len, 0.0f, 0.0f);
			glEnd();
			glPushMatrix();
			glTranslated(len, 0.0f, 0.0f);
			glutSolidCube((float)len * 0.2);
			glPopMatrix();

			// y축과 핸들,
			glColor3ub(r, g, EDIT_SCL_Y);
			glBegin(GL_LINES);
			glVertex3d(0.0, len * 0.4, 0.0);
			glVertex3d(0.0f, len, 0.0f);
			glEnd();
			glPushMatrix();
			glTranslated(0.0, len, 0.0);
			glutSolidCube(len * 0.2);
			glPopMatrix();

			// z축과 핸들을 렌더링한 후,
			glColor3ub(r, g, EDIT_SCL_Z);
			glBegin(GL_LINES);
			glVertex3d(0.0, 0.0, len * 0.4);
			glVertex3d(0.0, 0.0, len);
			glEnd();
			glPushMatrix();
			glTranslated(0.0, 0.0, len);
			glutSolidCube(len * 0.2);
			glPopMatrix();

			// xy 핸들
			glColor3ub(r, g, EDIT_SCL_XY);
			glBegin(GL_POLYGON);
			{
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(len * 0.4, 0.0, 0.0);
				glVertex3d(len * 0.4, len * 0.4, 0.0);
				glVertex3d(0.0, len * 0.4, 0.0);
			}
			glEnd();

			// yz 핸들
			glColor3ub(r, g, EDIT_SCL_YZ);
			glBegin(GL_POLYGON);
			{
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(0.0, len * 0.4, 0.0);
				glVertex3d(0.0, len * 0.4, len * 0.4);
				glVertex3d(0.0, 0.0, len * 0.4);
			}
			glEnd();

			// zx 핸들
			glColor3ub(r, g, EDIT_SCL_ZX);
			glBegin(GL_POLYGON);
			{
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(0.0, 0.0, len * 0.4);
				glVertex3d(len * 0.4, 0.0, len * 0.4);
				glVertex3d(len * 0.4, 0.0, 0.0);			
			}
			glEnd();

			// xyz 핸들
			glColor3ub(r, g, EDIT_SCL_XYZ);
			glPushMatrix();
			{
				glTranslated(len * 0.1, len * 0.1, len * 0.1);
				glutSolidCube(len * 0.2);
			}
			glPopMatrix();
		}
	}
	// 월드 좌표계로 복원한다.
	glPopMatrix();

	// 깊이 테스트와 조명을 활성화 한다.
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

/*!
*	\brief 픽킹을 위한 선택메커니즘으로 진입한다.
*
*	\param x 마우스의 X 좌표
*	\param y 마우스의 Y 좌표
*/
void PSelector::EnterSelectMode(int x, int y)
{
	// 뷰 포트 변환 정보를 얻고,
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// 선택 버퍼를 지정하고, 
	memset(SelectBuf, 0, sizeof(GLuint) * SELECT_BUFFER_SIZE);
	glSelectBuffer(SELECT_BUFFER_SIZE, SelectBuf);

	// 렌더링 모드 변경한 후,
	glRenderMode(GL_SELECT);

	// 네임 스택을 초기화하고 dummy(0)을 넣음
	glInitNames();
	glPushName(0);

	// 투영 행렬을 선택하고,
	glMatrixMode(GL_PROJECTION);

	// 현재 투영 행렬을 저장하고,
	glPushMatrix();

	// 투영 행렬을 단위행렬로 초기화 하고,
	glLoadIdentity();

	// 선택 행렬을 생성하고 투영 행렬에 곱한다.
	int offset = 10;
	gluPickMatrix((double)x, (GLdouble)(viewport[3] - y), offset, offset, viewport);
	pEditor->pScene->SetupViewFrustum();

	// 모델뷰 행렬을 선택하고,
	glMatrixMode(GL_MODELVIEW);

	// 현재 모델뷰 행렬을 저장하고,
	glPushMatrix();

	// 모델뷰 행렬을 단위 행렬로 초기화 하고,
	glLoadIdentity();

	// 월드(장면) 좌표계로 변환 하고,
	pEditor->pScene->SetupViewTransform();
}

/*!
*	\brief 선택 메커니즘을 종료하고, 일반 렌더링 모드로 돌아온다.
*
*	\return 선택된 객체의 개수
*/
int PSelector::ExitSelectMode()
{
	// 이전 모델뷰 행렬을 복원하고,
	glPopMatrix();

	// 투영 행렬을 선택하고,
	glMatrixMode(GL_PROJECTION);

	// 이전 투영 행렬을 복원하고,
	glPopMatrix();

	// 모델뷰 행렬을 선택하고,
	glMatrixMode(GL_MODELVIEW);

	// 선택 버퍼에 저장된 개수를 반환한다.
	return glRenderMode(GL_RENDER);
}

/*!
*	\brief 장면에 포함된 메쉬를 선택한다.
*
*	\param x 마우스의 x 좌표
*	\param y 마우스의 y 좌표
*
*	\return 메쉬가 선택된 경우 true, 그렇지 않으면 false.
*/
bool PSelector::SelectMesh(int x, int y)
{
	// Shift와 Control중 어느 하나도 눌리지 않았다면,
	if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
		// 이전 편집 객체 리스트를 지우고,
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
		pEditor->EditFaceRgn.clear();
		pEditor->EditBndryPts.clear();
	}

	// 장면의 순차적 메쉬 리스트를 구하고,
	std::vector<GMesh *> MeshList;
	GetPreOrderMeshList(pEditor->pScene, MeshList, NULL);

	// 각각의 메쉬에 대하여,
	for (unsigned i = 0; i < MeshList.size(); ++i)
	{
		// 본 메쉬이거나 숨겨진 메쉬라면 건너뛴다.
		if (!MeshList[i]->Renderer.IsVisibleMesh() || MeshList[i]->SemanticType == MESH_BONE)
			continue;

		// 각각의 정점에 대하여,
		int NumVert = MeshList[i]->GetNumVert();
		for (int j = 0; j < NumVert; ++j)
		{
			// 투영된 좌표가
			double wx, wy, wz;
			pEditor->pScene->MainCamera.GetScreenCoord(&(MeshList[i]->Verts[j]), &wx, &wy, &wz);

			// 선택 영역안에 있다면,
			if (IsPtInPolygon(wx, wy, SltRgn2DPtList))
			{
				// Control 키가 눌린 경우, 선택 객체를 삭제하고,
				if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
					pEditor->DelFromEditObjList(MeshList[i]);
				else	// 아닌 경우(아무 키도 눌리지 않거나, Shift 키가 눌린 경우) 객체를 추가한다.
					pEditor->AddToEditObjList(MeshList[i]);
				break;
			}
		}
	}

	// 선택된 메쉬가 없다면 false, 있다면 true를 리턴한다.
	if (pEditor->EditObjList.empty())
		return false;
	else
		return true;
}


/*!
*	\brief 장면에 포함된 메쉬를 선택한다.
*
*	\param Name 선택할 메쉬의 이름
*
*	\return 메쉬가 선택된 경우 true, 그렇지 않으면 false.
*/
bool PSelector::SelectMesh(std::string Name)
{
	// Shift와 Ctrl키의 눌린 상태를 구하여,
	bool bShift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
	bool bCtrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;

	// Shift와 Control중 어느 하나도 눌리지 않았다면,
	if (!bShift && !bCtrl)
	{
		// 선택 객체 리스트를 지우고,
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
		pEditor->EditFaceRgn.clear();
		pEditor->EditBndryPts.clear();
	}

	// 장면의 메쉬 리스트를 구하고,
	std::vector<GMesh *> MeshList;
	GetPreOrderMeshList(pEditor->pScene, MeshList, NULL);

	// 장면의 각각의 메쉬에 대하여,
	int NumMesh = (int)MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
	{
		// 전달 인자와 같은 이름의 메쉬를 찾아
		if (MeshList[i]->Name == Name)
		{
			// Control 키가 눌린 경우, 객체를 토글하고,
			if (bCtrl)
				pEditor->ToggleEditObject(MeshList[i]);
			else	// 아닌 경우(아무 키도 눌리지 않거나, Shift 키가 눌린 경우) 객체를 추가한다.
				pEditor->AddToEditObjList(MeshList[i]);

			pEditor->SetEC();
			break;
		}
	}

	// 중복 선택된 객체를 제거한다.
	pEditor->UniqueEditObjList();

	// 객체의 선택 결과를 반환한다.
	if (pEditor->EditObjList.empty())
		return false;
	else
		return true;
}

/*!
*	\brief 장면에 포함된 본 메쉬를 선택한다.
*
*	\param x 마우스의 x 좌표
*	\param y 마우스의 y 좌표
*
*	\return 메쉬가 선택된 경우 true, 그렇지 않으면 false.
*/
bool PSelector::SelectBone(int x, int y)
{
	// Shift와 Control중 어느 하나도 눌리지 않았다면,
	if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
		// 이전 편집 객체 리스트를 지우고,
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
		pEditor->EditFaceRgn.clear();
		pEditor->EditBndryPts.clear();
	}

	// 장면의 순차적 메쉬 리스트를 구하고,
	std::vector<GMesh *> MeshList;
	GetPreOrderMeshList(pEditor->pScene, MeshList, NULL);

	// 선택된 본 메쉬를 저장할 변수를 정의하고,
	GMesh *pSltBoneMesh = NULL;

	// 각각의 본 메쉬에 대하여,
	int NumMesh = (int)MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
	{
		// 보여지는 본 메쉬가 아니라면 건너띄고,
		if (!MeshList[i]->Renderer.IsVisibleMesh() || MeshList[i]->SemanticType != MESH_BONE)
			continue;

		// 각각의 정점에 대하여,
		int NumVert = MeshList[i]->GetNumVert();
		for (int j = 0; j < NumVert; ++j)
		{
			// 투영된 좌표가
			double wx, wy, wz;
			pEditor->pScene->MainCamera.GetScreenCoord(&(MeshList[i]->Verts[j]), &wx, &wy, &wz);

			// 선택 영역안에 있다면,
			if (IsPtInPolygon(wx, wy, SltRgn2DPtList))
			{
				// 현재 본 메쉬를 선택하고,
				pSltBoneMesh = MeshList[i];
				break;
			}
		}

		// 본 메쉬가 하나라도 선택 되었다면 빠져나온다. 
		if (pSltBoneMesh)
			break;
	}

	// 본 메쉬가 선택 되었다면,
	if (pSltBoneMesh)
	{
		// 기존에 선택되었던 본 메쉬의 개수를 구하고,
		int PrevNumBoneMesh = (int)(pEditor->EditObjList.size());

		// Control 키가 눌린 경우, 선택 객체를 삭제하고,
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			pEditor->DelFromEditObjList(pSltBoneMesh);
		// 아닌 경우(아무 키도 눌리지 않거나, Shift 키가 눌린 경우) 객체를 추가한다.
		else	
			pEditor->AddToEditObjList(pSltBoneMesh);

		// 선택 객체의 중복을 제거한 후,
		pEditor->UniqueEditObjList();

		// 기존에 선택되었던 본 메쉬의 개수를 구하고,
		int CurrNumBoneMesh = (int)(pEditor->EditObjList.size());

		// 새로운 본 메쉬가 추가 혹은 삭제 되었다면,
		if (CurrNumBoneMesh != PrevNumBoneMesh)
		{
			// 본 메쉬와 연결된 스킨 메쉬의 개수를 구하고,
			int NumSkinMesh = (int)pSltBoneMesh->LinkedMeshNameList.size();

			// 각각의 스킨 메쉬마다,
			for (int i = 0; i < NumSkinMesh; ++i)
			{
				// 본 메쉬와 연결된 스킨 메쉬를 검색해서,
				std::string SkinMeshName = pSltBoneMesh->LinkedMeshNameList[i];
				GMesh *pSkinMesh = FindMesh(pEditor->pScene->MeshList, SkinMeshName);

				// 스킨 메쉬가 존재하고, 정점의 색상이 변형 가중치로 설정되었다면,
				if (pSkinMesh != NULL && pSkinMesh->Renderer.VertColorType == VERTEX_COLOR_BONE_WEIGHT)
				{
					// 스켄 메쉬의 정점의 색상을 선택된 본 메쉬의 가중치로 설정한다.
					::set_vertex_color_from_skin_wgt(pSkinMesh, pSltBoneMesh);
					pSkinMesh->ModifiedFlag = MODIFIED_NORMAL;
					pSkinMesh->UpdateMesh();
				}
			}
		}
		return true;
	}

	return false;
}

/*!
*	\brief 선택된 메쉬에서 특정 정점을 선택한다.
*
*	\param x 마우스의 x 좌표
*	\param y 마우스의 y 좌표
*/
void PSelector::SelectVert(int x, int y)
{
	// 현재 선택된 메쉬가 없다면 리턴한다.
	GMesh *pMesh = pEditor->GetEditMesh();

	// 선택된 객체가 없었다면 리턴한다.
	if (pMesh == NULL)
	{
		// 메쉬를 하나 선택하고 리턴한다.
		SelectMesh(x, y);
		return;
	}

	// Shift와 Control중 어느 하나도 눌리지 않았다면,
	if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
		// 이전 편집 객체 리스트를 지우고,
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
		pEditor->EditFaceRgn.clear();
		pEditor->EditBndryPts.clear();
	}

	// 만약 영역 지정없이 클릭을 이용한 선택이라면,
	if (SltRgn2DPtList[0] == SltRgn2DPtList[1])
	{
		// Control 키가 눌린 경우, 선택 객체를 삭제하고,
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			if (MouseOverVertIdx != -1)
				pEditor->DelFromEditObjList(&(pMesh->Verts[MouseOverVertIdx]));
		}
		// 아닌 경우(아무 키도 눌리지 않거나, Shift 키가 눌린 경우) 객체를 추가한다.
		else	 
		{
			if (MouseOverVertIdx != -1)
				pEditor->AddToEditObjList(&(pMesh->Verts[MouseOverVertIdx]));
		}
		
		std::cout << "Selected Vertex Id: " << MouseOverVertIdx << std::endl;
	}
	// 경계 영역을 이용한 메쉬 선택이라면,
	else
	{
		// 관측방향에서 보이는 정점만을 선택하는 모드라면,
		if (SelectModeType == SELECT_MODE_FRONT_ONLY)
		{
			// 시선 벡터를 구하고,
			GVector3 dir = pEditor->pScene->MainCamera.GetViewDirection();
			for (unsigned j = 0; j < pMesh->Verts.size(); ++j)
			{
				// 각각의 정점의 법선 벡터가 시선 벡터와 90도 이상이라면,
				GVector3 n = pMesh->Verts[j].N;
				if (angle(dir, n) > 90.0)
				{
					// 정점의 스크린 좌표를 구하여 
					double wx, wy, wz;
					pEditor->pScene->MainCamera.GetScreenCoord(&(pMesh->Verts[j]), &wx, &wy, &wz);

					// 선택 영역 내에 포함되었다면
					if (IsPtInPolygon(wx, wy, SltRgn2DPtList))
					{
						// Control 키가 눌린 경우, 선택 객체를 삭제하고,
						if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
							pEditor->DelFromEditObjList(&(pMesh->Verts[j]));
						// 아닌 경우(아무 키도 눌리지 않거나, Shift 키가 눌린 경우) 객체를 추가한다.
						else	
							pEditor->AddToEditObjList(&(pMesh->Verts[j]));
					}
				}				
			}
		}
		else	// 영역내의 모든 정점을 선택하는 모드라면,
		{
			for (unsigned j = 0; j < pMesh->Verts.size(); ++j)
			{
				// 정점의 스크린 좌표를 구하여 
				double wx, wy, wz;
				pEditor->pScene->MainCamera.GetScreenCoord(&(pMesh->Verts[j]), &wx, &wy, &wz);

				// 선택 영역 내에 포함되었다면
				if (IsPtInPolygon(wx, wy, SltRgn2DPtList))
				{
					// Control 키가 눌린 경우, 선택 객체를 삭제하고,
					if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
						pEditor->DelFromEditObjList(&(pMesh->Verts[j]));
					// 아닌 경우(아무 키도 눌리지 않거나, Shift 키가 눌린 경우) 객체를 추가한다.
					else	
						pEditor->AddToEditObjList(&(pMesh->Verts[j]));
				}				
			}
		}
		
		// 선택된 편집 객체의 중복을 제거한다.
		pEditor->UniqueEditObjList();
	}

	// 선택 객체가 있으면 편집 가중치를 계산한다.
	pEditor->FindEditWgt();
}

/*!
*	\brief 삼각형과 마우스의 교차점을 선택한다.
*
*	\param x 마우스의 x 좌표
*	\param y 마우스의 y 좌표
*/
void PSelector::SelectPtOnFace(int x, int y)
{
	// 현재 선택된 메쉬를 구하고,
	GMesh *pMesh = pEditor->GetEditMesh();

	// 선택된 메쉬가 없다면 리턴한다.
	if (pMesh == NULL)
	{
		// 마우스와 교차하는 메쉬를 하나 선택하고 리턴한다.
		SelectMesh(x, y);
		return;
	}

	// 이전 편집 객체 리스트를 지우고,
	pEditor->EditObjList.clear();
	pEditor->EditVertRgn.clear();
	pEditor->EditFaceRgn.clear();
	pEditor->EditBndryPts.clear();

	// 교차점의 좌표가 계산되었다면,
	if (MouseOverPtCoords != GVector3(0.0, 0.0, 0.0))
	{
		static GPtOnFace SltPt;
		GFace *pFace = &(pMesh->Faces[MouseOverFaceIdx]);
		SltPt.Set(pFace, MouseOverPtCoords);
		pEditor->AddToEditObjList(&SltPt);
	}
}

/*!
*	\brief 브러시 선택 영역 내의 정점을 선택한다.
*
*	\param pMesh 선택된 객체
*/
void PSelector::SelectVertByBrush(GMesh *pMesh)
{
	// 현재 마우스 포인터와 교차한 정점 주변의 브러쉬 선택 영역을 계산하고,
	std::vector<double> Distances;
	double BrushRadius = pMesh->BBox.GetDiagonal() * (pEditor->EditMeshDlg.m_BrushRadius / 1000.0);
	get_geodesic_dist_field(pMesh, &(pMesh->Verts[MouseOverVertIdx]), Distances, BrushRadius);

	// 선택 영역의 정점들을 편집 리스트에 추가/삭제 한다.
	int NumVert = pMesh->GetNumVert();
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)	// Control 키가 눌린 경우, 선택 객체를 삭제하고,
	{
		for (int i = 0; i < NumVert; ++i)
			if (Distances[i] < BrushRadius)
				pEditor->DelFromEditObjList(&(pMesh->Verts[i]));					
	}
	else		// 아닌 경우(아무 키도 눌리지 않거나, Shift 키가 눌린 경우) 객체를 추가한다.
	{
		for (int i = 0; i < NumVert; ++i)
			if (Distances[i] < BrushRadius)
				pEditor->AddToEditObjList(&(pMesh->Verts[i]));
	}
}

/*!
*	\brief 선택된 메쉬에서 특정 삼각형을 선택한다.
*
*	\param x 마우스의 x 좌표
*	\param y 마우스의 y 좌표
*/
void PSelector::SelectFace(int x, int y)
{
	// 현재 선택된 메쉬를 구하고,
	GMesh *pMesh = pEditor->GetEditMesh();

	// 선택된 객체가 없었다면, 
	if (pMesh == NULL)
	{
		// 메쉬를 하나 선택하고 리턴한다.
		SelectMesh(x, y);
		return;
	}

	// Shift와 Control중 어느 하나도 눌리지 않았다면,
	if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
		// 이전 편집 객체 리스트를 지우고,
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
		pEditor->EditFaceRgn.clear();
		pEditor->EditBndryPts.clear();
	}

	// 마우스 클릭을 이용한 선택이라면,
	if (SltRgn2DPtList[0] == SltRgn2DPtList[1])
		SelectFaceByClick(pMesh);		
	else	// 경계 영역을 이용한 삼각형 선택이라면,
		SelectFaceByRgn(pMesh);		

	// 선택 객체가 있으면 편집 가중치를 계산한다.
	pEditor->FindEditWgt();
}

/*!
*	\brief 선택된 메쉬에서 특정 삼각형을 마우스 Click으로 선택한다.
*
*	\param pMesh 선택된 메쉬
*/
void PSelector::SelectFaceByClick(GMesh *pMesh)
{
	// 마우스 포인터와 교차하는 삼각형이 있다면,
	if (MouseOverFaceIdx != -1)
	{
		// Control 키가 눌린 경우, 선택 객체를 삭제하고,
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			pEditor->DelFromEditObjList(&(pMesh->Faces[MouseOverFaceIdx]));
			// Quad 메쉬라면 추가적으로,
			if (pMesh->FaceType == FACE_QUAD)
			{
				// 짝수번째 삼각형과 홀수번째 삼각형을 구분하여 인접한 삼각형을 삭제한다.
				if (MouseOverFaceIdx % 2 == 0)
					pEditor->DelFromEditObjList(&(pMesh->Faces[MouseOverFaceIdx + 1]));
				else
					pEditor->DelFromEditObjList(&(pMesh->Faces[MouseOverFaceIdx - 1]));
			}
		}
		// 아닌 경우(아무 키도 눌리지 않거나, Shift 키가 눌린 경우) 객체를 추가한다.
		else	 
		{
			pEditor->AddToEditObjList(&(pMesh->Faces[MouseOverFaceIdx]));
			// Quad 메쉬라면,
			if (pMesh->FaceType == FACE_QUAD)
			{
				// 짝수번째 삼각형과 홀수번째 삼각형을 구분하여 인접한 삼각형을 추가한다.
				if (MouseOverFaceIdx % 2 == 0)
					pEditor->AddToEditObjList(&(pMesh->Faces[MouseOverFaceIdx + 1]));
				else
					pEditor->AddToEditObjList(&(pMesh->Faces[MouseOverFaceIdx - 1]));
			}
		}
	}

	// 선택된 편집 객체의 중복을 제거한다.
	pEditor->UniqueEditObjList();
}

/*!
*	\brief 선택된 메쉬에서 특정 삼각형을 영역 지정으로 선택한다.
*
*	\param pMesh 선택된 메쉬
*/

void PSelector::SelectFaceByRgn(GMesh *pMesh)
{
	// 렌더링 속성에 폴리곤과 와이어속성을 추가하고,
	pMesh->Renderer.RenderType = (pMesh->Renderer.RenderType | RENDER_POLYGON | RENDER_WIRE);

	// 선택된 삼각형을 저장할 변수를 정의하고,
	std::vector<GFace *> SltFaceList;

	// 관측방향에서 보이는 삼각형만 선택하는 모드라면,
	if (SelectModeType == SELECT_MODE_FRONT_ONLY)
	{
		// 시선 벡터를 구하여,
		GVector3 dir = pEditor->pScene->MainCamera.GetViewDirection();

		// 각 삼각형 마다,
		int NumFace = pMesh->GetNumFace();
		for (int i = 0; i < NumFace; ++i)
		{
			// 삼각형의 법선을 구하여,
			GVector3 n = pMesh->Faces[i].GetNormal(false);

			// 앞면이 보인다면,
			if (angle(dir, n) > 90.0)
			{
				// 삼각형의 각 정점에 대하여,
				int *vidx = pMesh->Faces[i].vIdx;
				for (int j = 0; j < 3; ++j)
				{
					// 윈도우에 투영된 좌표를 구하고,
					double wx, wy, wz;
					pEditor->pScene->MainCamera.GetScreenCoord(&(pMesh->Verts[vidx[j]]), &wx, &wy, &wz);

					// 투영된 좌표가 영역 내에 존재한다면,
					if (IsPtInPolygon(wx, wy, SltRgn2DPtList))
					{
						// 선택리스트에 추가한다.
						SltFaceList.push_back(&(pMesh->Faces[i]));

						// Quad 메쉬라면 추가적으로 인접한 삼각형을 추가한다.
						if (pMesh->FaceType == FACE_QUAD)
						{
							// 짝수번째 삼각형과 홀수번째 삼각형을 구분하여,
							if (i % 2 == 0)
							{
								SltFaceList.push_back(&(pMesh->Faces[i + 1]));
								i++;
							}
							else
								SltFaceList.push_back(&(pMesh->Faces[i - 1]));
						}
						break;
					}
				}
			}							
		}
	}
	else	// 영역내의 모든 삼각형을 선택하는 모드라면,
	{
		int NumFace = pMesh->GetNumFace();
		for (int i = 0; i < NumFace; ++i)
		{
			for (int k = 0; k < 3; ++k)
			{
				int vidx = pMesh->Faces[i].vIdx[k];
				double wx, wy, wz;
				pEditor->pScene->MainCamera.GetScreenCoord(&(pMesh->Verts[vidx]), &wx, &wy, &wz);
				if (IsPtInPolygon(wx, wy, SltRgn2DPtList))
				{
					// 선택리스트에 추가한다.
					SltFaceList.push_back(&(pMesh->Faces[i]));

					// Quad 메쉬라면 추가적으로 인접한 삼각형을 추가한다.
					if (pMesh->FaceType == FACE_QUAD)
					{
						// 짝수번째 삼각형과 홀수번째 삼각형을 구분하여,
						if (i % 2 == 0)
						{
							SltFaceList.push_back(&(pMesh->Faces[i + 1]));
							i++;
						}
						else
							SltFaceList.push_back(&(pMesh->Faces[i - 1]));
					}
					break;
				}
			}
		}
	}

	int NumSltFace = (int)SltFaceList.size();
	// Control 키가 눌린 경우, 선택 객체를 삭제하고,
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{			
		for (int i = 0; i < NumSltFace; ++i)
			pEditor->DelFromEditObjList(SltFaceList[i]);
	}
	// 아닌 경우(아무 키도 눌리지 않거나, Shift 키가 눌린 경우) 객체를 추가한다.
	else
	{
		for (int i = 0; i < NumSltFace; ++i)
			pEditor->AddToEditObjList(SltFaceList[i]);
	}

	// 선택된 편집 객체의 중복을 제거한다.
	pEditor->UniqueEditObjList();
}

/*!
*	\brief 선택 영역의 경계와 교차하는 절단점 리스트를 생성한다.
*	\note 현재 미사용
*
*	\param pMesh 절단점을 계산할 메쉬
*	\param CutPtList 절단점이 저장될 리스트
*
*	\return 리스트에 저장된 절단점의 개수를 반환한다.
*/
int PSelector::FindCutPtList(GMesh *pMesh, std::vector<GCutPt> &CutPtList)
{
	// 선택 영역의 경계와 교차하는 픽셀의 리스트를 구한다.
	std::vector<CPoint> PixelList;
	int NumPt = (int)SltRgn2DPtList.size();
	for (int i = 0; i < NumPt - 1; ++i)
	{
		// 직선 (x0, y0)와 (x1, y1)을 교차하는 모든 픽셀의 좌표를 구한다.
		int x0 = SltRgn2DPtList[i][0];
		int y0 = SltRgn2DPtList[i][1];
		int x1 = SltRgn2DPtList[i + 1][0];
		int y1 = SltRgn2DPtList[i + 1][1];
		double m = (double)(y1 - y0) / (double)(x1 - x0);
		if (ABS(m) < 1.0)
		{
			if (x0 > x1)
			{
				SWAP(int, x0, x1);
				SWAP(int, y0, y1);
			}
			for (int x = x0; x <= x1; x++)
			{
				int y = (int)(y0 + m * (x - x0) + 0.5);
				PixelList.push_back(CPoint(x, y));
				//std::cout << "(" << x << ", " << y << ")" << std::endl;
			}
		}
		else
		{
			if (y0 > y1)
			{
				SWAP(int, x0, x1);
				SWAP(int, y0, y1);
			}
			for (int y = y0; y <= y1; y++)
			{
				int x = (int)(x0 + 1.0 / m * (y - y0) + 0.5);
				PixelList.push_back(CPoint(x, y));
				//std::cout << "(" << x << ", " << y << ")" << std::endl;
			}
		}		
	}

	// 이전 편집 객체 리스트를 지우고,
	pEditor->EditObjList.clear();
	pEditor->EditVertRgn.clear();
	pEditor->EditFaceRgn.clear();
	pEditor->EditBndryPts.clear();

	int NumPixel = (int)PixelList.size();
	static int PrevFaceIdx = -1;
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	for (int i = 0; i < NumPixel; ++i)
	{
		// 각 픽셀의 위치에서 색상 버퍼의 픽셀 값을 읽어서,
		GLubyte pixel[3];
		int x = PixelList[i].x;
		int y = PixelList[i].y;
		glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

		// 삼각형의 렌더링 인덱스(1, 2, 3, ...)를 구하고,
		int CurrFaceIdx = pixel[0] * 256 * 256 + pixel[1] * 256 + pixel[2] - 1;

		// 선택된 삼각형이 없다면 다음 픽셀로 넘어가고,
		if (CurrFaceIdx < 0 || CurrFaceIdx > pMesh->GetNumFace() - 1)
			continue;

		// 선택된 삼각형이 이전 삼각형과 다른 삼각형이라면,
		if (CurrFaceIdx != PrevFaceIdx && PrevFaceIdx != -1)
		{
			// 이전 삼각형과 현재 선택된 삼각형이 공유하는 정점의 인덱스를 찾는다.
			int sidx, eidx;
			GFace *pPrevFace = &(pMesh->Faces[PrevFaceIdx]);
			GFace *pCurrFace = &(pMesh->Faces[CurrFaceIdx]);			
			int result = pCurrFace->GetSharedVertIdx(pPrevFace, sidx, eidx);

			// 하나의 정점만 공유한 경우,
			if (result == 0)
			{
				// 공유점을 구하여 절단 정점의 리스트에 추가한다.
				GCutPt pt(pCurrFace, sidx, eidx, 0.0, pMesh->Verts[sidx].P);
				CutPtList.push_back(pt);
			}

			// 현재 선택된 삼각형과 이전 삼각형이 인접하면,
			if (result == 1)
			{
				GVertex *v0 = &(pMesh->Verts[sidx]);
				GVertex *v1 = &(pMesh->Verts[eidx]);
				
				double tmp;
				GVector2 w0, w1;
				pEditor->pScene->MainCamera.GetScreenCoord(v0, &w0[0], &w0[1], &tmp);
				pEditor->pScene->MainCamera.GetScreenCoord(v1, &w1[0], &w1[1], &tmp);

				GVector2 p(x, y);
				GVector2 v = w1 - w0;
				double t = ((p - w0) * v) / (v * v);

				GPoint3 p0 = pMesh->Verts[sidx].P;
				GPoint3 p1 = pMesh->Verts[eidx].P;
				GPoint3 ispt = p0 + t * (p1 - p0);

				// 절단 정점을 구하여 리스트에 추가한다.
				GCutPt pt = GCutPt(pCurrFace, sidx, eidx, t, ispt);
				CutPtList.push_back(pt);
			}	
			//pEditor->EditObjList.push_back(pCurrFace);
		}
		PrevFaceIdx = CurrFaceIdx;
	}

	return CutPtList.size();
}

/*!
*	\brief 두 점을 지나는 직선과 교차하는 픽셀의 리스트를 계산한다.
*
*	\param p 직선의 시작점
*	\param q 직선의 끝점
*/
void PSelector::GetPixelList(CPoint p, CPoint q, std::vector<CPoint> &PixelList)
{
	bool bSwap = false;
	std::vector<CPoint> TmpList;
	double m = (double)(q.y - p.y) / (double)(q.x - p.x);
	if (ABS(m) < 1.0)
	{
		if (p.x > q.x)
		{
			SWAP(CPoint, p, q);
			bSwap = true;
		}
		for (int x = p.x; x <= q.x; x++)
		{
			int y = (int)(p.y + m * (x - p.x) + 0.5);
			TmpList.push_back(CPoint(x, y));
// 			TmpList.push_back(CPoint(x, y - 1));
// 			TmpList.push_back(CPoint(x, y - 2));
// 			TmpList.push_back(CPoint(x, y + 1));
// 			TmpList.push_back(CPoint(x, y + 2));
		}
	}
	else
	{
		if (p.y > q.y)
		{
			SWAP(CPoint, p, q);
			bSwap = true;
		}
		for (int y = p.y; y <= q.y; y++)
		{
			int x = (int)(p.x + 1.0 / m * (y - p.y) + 0.5);
			TmpList.push_back(CPoint(x, y));
// 			TmpList.push_back(CPoint(x - 1, y));
// 			TmpList.push_back(CPoint(x - 2, y));
// 			TmpList.push_back(CPoint(x + 1, y));
// 			TmpList.push_back(CPoint(x + 2, y));
		}
	}

	if (bSwap)
	{
		int NumPixel = (int)TmpList.size();
		for (int i = 0; i < NumPixel; ++i)
			PixelList.push_back(TmpList[NumPixel - i - 1]);
	}
	else
	{
		PixelList = TmpList;
	}	
}

/*!
*	\brief 색상 버퍼에 저장된 (x, y) 픽셀의 색상을 읽는다.
*
*	\param x 마우스의 x 좌표
*	\param y 마우스의 y 좌표
*	\param color (R, G, B) 색상이 저장됨
*/
void PSelector::GetPixelColor(int x, int y, GLubyte *color)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, color);
}

/*!
*	\brief 선택된 메쉬에서 삼각형을 선택하고, 경계 영역을 절단한다.
*
*	\param x 마우스의 x 좌표
*	\param y 마우스의 y 좌표
*/
void PSelector::SelectFaceAndCutBndry(int x, int y)
{
	// 선택된 메쉬가 없다면 새로운 메쉬를 선택하고 리턴한다.
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL)
	{
		SelectMesh(x, y);
		return;
	}

	// 렌더링 속성에 폴리곤과 와이어속성을 추가한다.
	pMesh->Renderer.RenderType = (pMesh->Renderer.RenderType | RENDER_POLYGON | RENDER_WIRE);

	/////////////////////////////////////////////////////////////////
	// 1 단계: 선택 영역에 중첩된 삼각형의 정점의 좌표를 조절한다. //
	/////////////////////////////////////////////////////////////////

	// 움직인 정점의 정보를 저장하는 변수를 정의한다.
	std::map<int, double> ModifiedVertDist;
	std::vector<GVertex> OldVerts = pMesh->Verts;
	
	// 선택 영역의 경계와 교차하는 픽셀의 리스트를 구한다.
	for (unsigned i = 0; i < SltRgn2DPtList.size() - 1; ++i)
	{
		// 두 점 St와 Ed을 교차하는 모든 픽셀의 좌표를 구한다.
		std::vector<CPoint> PixelList;
		CPoint St(SltRgn2DPtList[i][0], SltRgn2DPtList[i][1]);
		CPoint Ed(SltRgn2DPtList[i + 1][0], SltRgn2DPtList[i + 1][1]);
		GetPixelList(St, Ed, PixelList);
		
		GFace *pPrevFace = NULL;
		for (unsigned j = 0; j < PixelList.size(); ++j)
		{
			// 각 픽셀의 위치에서 색상 버퍼의 픽셀 값을 읽어서,
			GLubyte color[3];
			GetPixelColor(PixelList[j].x, PixelList[j].y, color);

			// 선택된 삼각형을 구하고, 없다면 다음 픽셀로 넘어간다.
			int CurrFaceIdx = color[0] * 256 * 256 + color[1] * 256 + color[2] - 1;
			if (CurrFaceIdx < 0 || CurrFaceIdx > pMesh->GetNumFace() - 1)
				continue;

			// 선택된 삼각형이 이전 삼각형과 다른 삼각형이라면,
			GFace *pCurrFace = &(pMesh->Faces[CurrFaceIdx]);
			if (pCurrFace != pPrevFace && pPrevFace != NULL)
			{	
				// 변형 전 삼각형의 세 정점의 좌표를 구하고,
				int *vidx = pCurrFace->vIdx;
				GVertex *v0 = &(OldVerts[vidx[0]]);
				GVertex *v1 = &(OldVerts[vidx[1]]);
				GVertex *v2 = &(OldVerts[vidx[2]]);

				// 세 정점의 투영된 점이 선택 영역내에 포함되는지 조사한다.
				double tmp;
				GVector2 w0, w1, w2;
				pEditor->pScene->MainCamera.GetScreenCoord(v0, &w0[0], &w0[1], &tmp);
				pEditor->pScene->MainCamera.GetScreenCoord(v1, &w1[0], &w1[1], &tmp);
				pEditor->pScene->MainCamera.GetScreenCoord(v2, &w2[0], &w2[1], &tmp);
				bool inside0 = IsPtInPolygon(w0[0], w0[1], SltRgn2DPtList);
				bool inside1 = IsPtInPolygon(w1[0], w1[1], SltRgn2DPtList);
				bool inside2 = IsPtInPolygon(w2[0], w2[1], SltRgn2DPtList);

				// (v0, v1)이 경계선과 만난다면,
				if (inside0 && !inside1)
				{
					GPoint3 p = OldVerts[vidx[0]].P;
					GPoint3 q = OldVerts[vidx[1]].P;
					GPoint3 r;

					// (St, Ed)와 (w0, w1)의 교차점의 좌표를 구하여 v0의 좌표를 갱신한다.
					double a = w1[0] - w0[0];
					double c = w1[1] - w0[1];
					double b = -(Ed.x - St.x);
					double d = -(Ed.y - St.y);
					double e = St.x - w0[0];
					double f = St.y - w0[1];
					double t = (d * e - b * f) / (a * d - b * c);

					if (t >= 0.0 && t <= 1.0)
					{
						r = p + t * (q - p) * 0.98;
						double d = dist(p, r);
						if (ModifiedVertDist[vidx[0]] == 0.0 || d < ModifiedVertDist[vidx[0]])
						{
							ModifiedVertDist[vidx[0]] = d;
							pMesh->Verts[vidx[0]].P = r;
						}
					}					
				}

				// (v1, v2)이 경계선과 만난다면,
				if (inside1 && !inside2)
				{
					GPoint3 p = OldVerts[vidx[1]].P;
					GPoint3 q = OldVerts[vidx[2]].P;
					GPoint3 r;

					// (St, Ed)와 (w1, w2)의 교차점의 좌표를 구하여 v1의 좌표를 갱신한다.
					double a = w2[0] - w1[0];
					double c = w2[1] - w1[1];
					double b = -(Ed.x - St.x);
					double d = -(Ed.y - St.y);
					double e = St.x - w1[0];
					double f = St.y - w1[1];
					double t = (d * e - b * f) / (a * d - b * c);

					if (t >= 0.0 && t <= 1.0)
					{
						r = p + t * (q - p) * 0.98;
						double d = dist(p, r);
						if (ModifiedVertDist[vidx[1]] == 0.0 || d < ModifiedVertDist[vidx[1]])
						{
							ModifiedVertDist[vidx[1]] = d;
							pMesh->Verts[vidx[1]].P = r;
						}
					}	
				}

				// (v2, v0)이 경계선과 만난다면,
				if (inside2 && !inside0)
				{
					GPoint3 p = OldVerts[vidx[2]].P;
					GPoint3 q = OldVerts[vidx[0]].P;
					GPoint3 r;

					// (St, Ed)와 (w2, w0)의 교차점의 좌표를 구하여 v2의 좌표를 갱신한다.
					double a = w0[0] - w2[0];
					double c = w0[1] - w2[1];
					double b = -(Ed.x - St.x);
					double d = -(Ed.y - St.y);
					double e = St.x - w2[0];
					double f = St.y - w2[1];
					double t = (d * e - b * f) / (a * d - b * c);

					if (t >= 0.0 && t <= 1.0)
					{
						r = p + t * (q - p) * 0.98;
						double d = dist(p, r);
						if (ModifiedVertDist[vidx[2]] == 0.0 || d < ModifiedVertDist[vidx[2]])
						{
							ModifiedVertDist[vidx[2]] = d;
							pMesh->Verts[vidx[2]].P = r;
						}
					}	
				}
			}
			// 이전 삼각형을 갱신한다.
			pPrevFace = pCurrFace;
		}
	}

	pMesh->ModifiedFlag = MODIFIED_VERT_POS_ALL;
	pMesh->UpdateMesh();

	////////////////////////////////////////////////////////////////////
	// 2 단계: 선택 영역내에 세 점이 포함되는 보이는 삼각형을 선택한다. //
	////////////////////////////////////////////////////////////////////

	// Shift와 Control중 어느 하나도 눌리지 않았다면,
	if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
		// 이전 편집 객체 리스트를 지우고,
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
		pEditor->EditFaceRgn.clear();
		pEditor->EditBndryPts.clear();
	}

	// 선택된 삼각형을 저장할 변수를 정의하고,
	std::vector<GFace *> SltFaceList;

	// 시선 벡터를 구하고,
	GVector3 dir = pEditor->pScene->MainCamera.GetViewDirection();
	for (int i = 0; i < pMesh->GetNumFace(); ++i)
	{
		// 각 삼각형의 법선을 구하여, 앞면이 보인다면,
		GVector3 n = pMesh->Faces[i].GetNormal(false);
		if (angle(dir, n) > 90.0)
		{
			// 삼각형의 세 정점의 투영된 점이 선택 영역내에 포함되는지 조사한다.
			double tmp;
			GVector2 w0, w1, w2;
			int *vidx = pMesh->Faces[i].vIdx;
			pEditor->pScene->MainCamera.GetScreenCoord(&(pMesh->Verts[vidx[0]]), &w0[0], &w0[1], &tmp);
			pEditor->pScene->MainCamera.GetScreenCoord(&(pMesh->Verts[vidx[1]]), &w1[0], &w1[1], &tmp);
			pEditor->pScene->MainCamera.GetScreenCoord(&(pMesh->Verts[vidx[2]]), &w2[0], &w2[1], &tmp);
			bool inside0 = IsPtInPolygon(w0[0], w0[1], SltRgn2DPtList);
			bool inside1 = IsPtInPolygon(w1[0], w1[1], SltRgn2DPtList);
			bool inside2 = IsPtInPolygon(w2[0], w2[1], SltRgn2DPtList);

			// 새 정점 중 최소 두 점이 선택 영역 내에 포함되었다면,
			//if ((inside0 && inside1) || (inside1 && inside2) || (inside2 && inside0))
			if (inside0 && inside1 && inside2)
			{
				// 선택 리스트에 추가한다.
				SltFaceList.push_back(&(pMesh->Faces[i]));

				// Quad 메쉬라면 추가적으로 인접한 삼각형을 추가한다.
				if (pMesh->FaceType == FACE_QUAD)
				{
					// 짝수번째 삼각형과 홀수번째 삼각형을 구분하여,
					if (i % 2 == 0)
					{
						SltFaceList.push_back(&(pMesh->Faces[i + 1]));
						i++;
					}
					else
						SltFaceList.push_back(&(pMesh->Faces[i - 1]));
				}
			}
		}							
	}

	// Control 키가 눌린 경우, 선택 객체를 삭제하고,
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{			
		for (unsigned i = 0; i < SltFaceList.size(); ++i)
			pEditor->DelFromEditObjList(SltFaceList[i]);
	}
	// 아닌 경우(아무 키도 눌리지 않거나, Shift 키가 눌린 경우) 객체를 추가한다.
	else
	{
		for (unsigned i = 0; i < SltFaceList.size(); ++i)
			pEditor->AddToEditObjList(SltFaceList[i]);
	}

	// 선택된 편집 객체의 중복을 제거한다.
	pEditor->UniqueEditObjList();

	// 선택 객체가 있으면 편집 가중치를 계산한다.
	pEditor->FindEditWgt();
}

/*!
*	\brief 선택된 메쉬에서 삼각형 그룹을 선택한다.
*
*	\param x 마우스의 x 좌표
*	\param y 마우스의 y 좌표
*/
void PSelector::SelectFaceGroup(int x, int y)
{
	// 현재 선택된 메쉬를 구하고,
	GMesh *pMesh = pEditor->GetEditMesh();

	// 선택된 객체가 없다면, 
	if (pMesh == NULL)
	{
		// 메쉬를 하나 선택하고 리턴한다.
		SelectMesh(x, y);
		return;
	}

	// Shift와 Control중 어느 하나도 눌리지 않았다면,
	if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
		// 이전 편집 객체 리스트를 지우고,
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
		pEditor->EditFaceRgn.clear();
		pEditor->EditBndryPts.clear();
	}

	// 마우스 클릭을 이용한 선택이라면,
	if (SltRgn2DPtList[0] == SltRgn2DPtList[1])
	{
		// 마우스 포인터와 교차한 삼각형 그룹이 있다면,
		if (MouseOverGroupIdx != -1)
		{
			// Control 키가 눌린 경우, 선택 객체를 삭제하고,
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				std::map<std::string, std::vector<int>>::iterator it = pMesh->GrpFaces.begin();
				for (int i = 0; i < MouseOverGroupIdx; ++i)
					it++;

				int NumFace = (int)it->second.size();
				for (int i = 0; i < NumFace; ++i)
					pEditor->DelFromEditObjList(&(pMesh->Faces[it->second[i]]));			
			}
			// 아닌 경우(아무 키도 눌리지 않거나, Shift 키가 눌린 경우) 객체를 추가한다.
			else	 
			{
				std::map<std::string, std::vector<int>>::iterator it = pMesh->GrpFaces.begin();
				for (int i = 0; i < MouseOverGroupIdx; ++i)
					it++;

				int NumFace = (int)it->second.size();
				for (int i = 0; i < NumFace; ++i)
					pEditor->AddToEditObjList(&(pMesh->Faces[it->second[i]]));
			}
		}
		
		// 선택된 편집 객체의 중복을 제거한다.
		pEditor->UniqueEditObjList();
	}
	// 경계 영역을 이용한 메쉬 선택이라면,
	else
	{
		pMesh->Renderer.RenderType = (pMesh->Renderer.RenderType | RENDER_POLYGON | RENDER_WIRE);

		// 관측방향에서 보이는 삼각형 그룹만을 선택하기 위해서 시선 벡터를 구하고,
		GVector3 dir = pEditor->pScene->MainCamera.GetViewDirection();

		std::map<std::string, std::vector<int>>::iterator it;
		for (it = pMesh->GrpFaces.begin(); it != pMesh->GrpFaces.end(); ++it)
		{
			int NumFace = (int)it->second.size();
			for (int i = 0; i < NumFace; ++i)
			{
				int fidx = it->second[i];
				GVector3 n = pMesh->Faces[fidx].GetNormal(false);
				if (angle(dir, n) > 90.0)
				{
					for (int j = 0; j < 3; ++j)
					{
						int vidx = pMesh->Faces[fidx].vIdx[j];
						double wx, wy, wz;
						pEditor->pScene->MainCamera.GetScreenCoord(&(pMesh->Verts[vidx]), &wx, &wy, &wz);
						if (IsPtInPolygon(wx, wy, SltRgn2DPtList))
						{
							// Control 키가 눌린 경우, 선택 객체를 삭제하고,
							if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
							{
								for (int k = 0; k < NumFace; ++k)
									pEditor->DelFromEditObjList(&(pMesh->Faces[it->second[k]]));
							}
							// 아닌 경우(아무 키도 눌리지 않거나, Shift 키가 눌린 경우) 객체를 추가한다.
							else	 
							{
								for (int k = 0; k < NumFace; ++k)
									pEditor->AddToEditObjList(&(pMesh->Faces[it->second[k]]));
							}
							i = NumFace;
							break;
						}
					}
				}
			}
		}
		// 선택된 편집 객체의 중복을 제거한다.
		pEditor->UniqueEditObjList();
	}

	// 선택 객체가 있으면 편집 가중치를 계산한다.
	pEditor->FindEditWgt();
}

/*!
*	\brief 브러시 선택 영역내의 삼각형을 선택한다.
*
*	\param pMesh 선택된 객체
*/
void PSelector::SelectFaceByBrush(GMesh *pMesh)
{
	// 현재 마우스 포인터와 교차한 정점 주변의 브러쉬 선택 영역을 계산하고,
	std::vector<double> Distances;
	double BrushRadius = pMesh->BBox.GetDiagonal() * (pEditor->EditMeshDlg.m_BrushRadius / 1000.0);
	::get_geodesic_dist_field(pMesh, &(pMesh->Verts[MouseOverVertIdx]), Distances, BrushRadius);

	// 선택된 삼각형을 저장할 변수를 정의하고,
	std::vector<GFace *> SltFaceList;

	// 선택된 삼각형을 구하여,
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		int vidx0 = pMesh->Faces[i].vIdx[0];
		int vidx1 = pMesh->Faces[i].vIdx[1];
		int vidx2 = pMesh->Faces[i].vIdx[2];
		if (Distances[vidx0] < BrushRadius || Distances[vidx1] < BrushRadius || Distances[vidx2] < BrushRadius)
		{
			// 선택 삼각형 리스트에 추가하고,
			SltFaceList.push_back(&(pMesh->Faces[i]));

			// Quad 메쉬라면 추가적으로 인접한 삼각형을 추가한다.
			if (pMesh->FaceType == FACE_QUAD)
			{
				// 짝수번째 삼각형과 홀수번째 삼각형을 구분하여,
				if (i % 2 == 0)
				{
					SltFaceList.push_back(&(pMesh->Faces[i + 1]));
					i++;
				}
				else
					SltFaceList.push_back(&(pMesh->Faces[i - 1]));
			}
		}
	}

	// 중복 선택 객체를 제거한다.
	sort(SltFaceList.begin(), SltFaceList.end());
	SltFaceList.erase(unique(SltFaceList.begin(), SltFaceList.end()), SltFaceList.end());

	// Control 키가 눌린 경우, 선택 객체를 삭제하고,
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) 
	{
		for (unsigned i = 0; i < SltFaceList.size(); ++i)
			pEditor->DelFromEditObjList(SltFaceList[i]);
	}
	else	// 아닌 경우(아무 키도 눌리지 않거나, Shift 키가 눌린 경우) 객체를 추가한다.
	{
		for (unsigned i = 0; i < SltFaceList.size(); ++i)
			pEditor->AddToEditObjList(SltFaceList[i]);
	}
}

/*!
*	\brief 선택된 메쉬(넙스)의 제어점을 선택한다.
*
*	\param x 마우스의 x 좌표
*	\param y 마우스의 y 좌표
*
*	\return 선택여부
*/
bool PSelector::SelectCtrlPt(int x, int y)
{
	// 현재 선택된 메쉬를 구하고,
	GMesh *pMesh = pEditor->GetEditMesh();

	// 선택된 객체가 없었다면, 
	if (pMesh == NULL)
	{
		// 메쉬를 하나 선택하고, false를 리턴한다.
		SelectMesh(x, y);
		return false;
	}

	// 선택 객체의 클래스 이름을 얻어와, 
	std::string ClsName = pMesh->GetClassName();

	// 제어점을 가진 객체가 아니라면 false를 반환한다.
	if (ClsName != "GNurbsCrv" && ClsName != "GNurbsSrf" && ClsName != "GCoonsSrf"
		&& ClsName != "GTriBzrSrf" && ClsName != "GUcbsSrf" && ClsName != "GUcbsVol")
	{
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
		pEditor->EditFaceRgn.clear();
		pEditor->EditBndryPts.clear();
		return false;
	}

	// Shift와 Control중 어느 하나도 눌리지 않았다면,
	if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
		// 이전 편집 객체 리스트를 지우고,
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
	}

	// 제어점 리스트를 구하여,
	std::vector<GCtrlPt *> CtrlPtLlist;
	pMesh->GetCtrlPtList(CtrlPtLlist);
	pMesh->Renderer.RenderType = pMesh->Renderer.RenderType | RENDER_CTRL_PT;

	// 각각의 제어점에 대하여,
	int NumCtrlPt = (int)CtrlPtLlist.size();
	for (int j = 0; j < NumCtrlPt; ++j)
	{
		// 스크린 상의 좌표를 구하여,
		double wx, wy, wz;
		pEditor->pScene->MainCamera.GetScreenCoord(CtrlPtLlist[j], &wx, &wy, &wz);

		// 선택 영역내에 포함된다면,
		if (IsPtInPolygon(wx, wy, SltRgn2DPtList))
		{
			// Control 키가 눌린 경우, 선택 객체를 삭제하고,
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
				pEditor->DelFromEditObjList(CtrlPtLlist[j]);
			// 아닌 경우(아무 키도 눌리지 않거나, Shift 키가 눌린 경우) 객체를 추가한다.
			else	
				pEditor->AddToEditObjList(CtrlPtLlist[j]);
		}
	}

	// 선택된 편집 객체의 중복을 제거한다.
	pEditor->UniqueEditObjList();

	// 선택 객체가 있으면 true, 없으면 false를 반환한다.
	if (pEditor->EditObjList.empty())
		return false;
	else
		return true;	
}

/*!
*	\brief 정점이 폴리곤 내부에 존재하는지 여부를 판단
*	\note 폴리곤 경계에 위치한 정점은 false를 반환한다.
*
*	\param x 정점의 x좌표
*	\param y 정점의 y좌표
*	\param PtList 폴리곤을 구성하는 정점 리스트
*/
bool PSelector::IsPtInPolygon(double x, double y, std::vector<GVector2> &PtList)
{
	// 알고리즘 참조(Real-time Rendering, 2nd ed., p585)
	bool inside = false;
	GVector2 e0 = PtList[0];
	bool y0 = (e0[1] >= y);
	for (unsigned i = 1; i < PtList.size(); ++i)
	{
		GVector2 e1 = PtList[i];
		bool y1 = (e1[1] >= y);
		if (y0 != y1)
		{
			if (((e1[1] - y) * (e0[0] - e1[0]) >= (e1[0] - x) * (e0[1] - e1[1])) == y1)
				inside = !inside;
		}
		y0 = y1;
		e0 = e1;		
	}
	return inside;
}

/*!
*	\brief 마우스 포인터와 교차하는 편집 핸들을 찾는다.
*
*	\param x 마우스의 x좌표
*	\param y 마우스의 y좌표
*/
bool PSelector::SetMouseOverEditHandle(int x, int y)
{
	// 반환 값을 설정하고,
	bool ret = false;

	// 선택된 편집 객체가 없다면 flase를 반환한다.
	if (pEditor->GetEditMesh() == NULL)
		return ret;

	// 현재 마우스 포인터 위치에서 픽셀의 색상 값을 읽어서,
	GLubyte pixel[3];
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

	// 편집 핸들이 선택 되었다면,
	if (pixel[0] == 255 && pixel[1] == 255)
	{
		// 선택된 편집 핸들의 정보를 설정하고,
		ret = true;
		MouseOverGroupIdx = -1;
		MouseOverFaceIdx = -1;
		MouseOverVertIdx = -1;
		MouseOverPtCoords.Set(0.0, 0.0, 0.0);
		pEditor->EditToolType = (TypeEditTool)pixel[2];
	}
	else	// 편집 핸들이 선택되지 않았다면, 이전 편집 툴의 형태로 설정한다.
	{
		if (pEditor->EditToolType >= EDIT_TRA && pEditor->EditToolType <= EDIT_TRA_XYZ)
			pEditor->EditToolType = EDIT_TRA;

		if (pEditor->EditToolType >= EDIT_ROT && pEditor->EditToolType <= EDIT_ROT_Z)
			pEditor->EditToolType = EDIT_ROT;

		if (pEditor->EditToolType >= EDIT_SCL && pEditor->EditToolType <= EDIT_SCL_XYZ)
			pEditor->EditToolType = EDIT_SCL;
	}

	return ret;
}

/*!
*	\brief 현재 마우스 포인터와 교차하는 삼각형, 정점 등의 정보를 설정한다.
*	\note 최근 수정일(2017/02/07)
*
*	\param x 마우스의 x좌표
*	\param y 마우스의 y좌표
*
*	\return 마우스와 교차한 삼각형, 정점이 있으면 true, 없으면 false를 반환한다.
*/
bool PSelector::SetMouseOverInfo(int x, int y)
{
	// 선택된 메쉬 객체를 구하고,
	GMesh *pMesh = pEditor->GetEditMesh();

	// 선택된 편집 객체가 없다면 -1을 설정하고 리턴한다.
	if (pMesh == NULL)
	{
		MouseOverGroupIdx = -1;
		MouseOverFaceIdx = -1;
		MouseOverVertIdx = -1;
		MouseOverPtCoords.Set(0.0, 0.0, 0.0);
		return false;
	}
		
	// 현재 마우스 포인터 위치에서 색상 버퍼의 픽셀 값을 읽어서,
	GLubyte pixel[3];
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

// 	int w = pEditor->pScene->MainCamera.ViewWidth;
// 	int h = pEditor->pScene->MainCamera.ViewHeight;
// 	int idx = (h - y) * w * 3 + x * 3;
// 	pixel[0] = pEditor->pScene->MainCamera.pPixelBuf[idx];
// 	pixel[1] = pEditor->pScene->MainCamera.pPixelBuf[idx + 1];
// 	pixel[2] = pEditor->pScene->MainCamera.pPixelBuf[idx + 2];

	// 삼각형의 렌더링 인덱스(1, 2, 3, ...)를 구하고,
	int RenderIdx = pixel[0] * 256 * 256 + pixel[1] * 256 + pixel[2];

	// 선택된 삼각형이 없다면,
	if (RenderIdx < 1 || RenderIdx > pMesh->GetNumFace())
	{
		MouseOverGroupIdx = -1;
		MouseOverFaceIdx = -1;
		MouseOverVertIdx = -1;
		MouseOverPtCoords.Set(0.0, 0.0, 0.0);
		return false;
	}

	MouseOverGroupIdx = 0;
	if (pMesh->GrpFaces.empty())	// 삼각형 그룹이 없다면,
	{
		MouseOverFaceIdx = RenderIdx - 1;
	}
	else	// 삼각형 그룹이 있다면,
	{
		// 마우스와 중첩된 삼각형 그룹의 인덱스를 구하고,
		std::map<std::string, std::vector<int>>::iterator it;
		for (it = pMesh->GrpFaces.begin(); it != pMesh->GrpFaces.end(); ++it)
		{
			int NumFace = (int)it->second.size();
			if (RenderIdx - NumFace <= 0)
			{
				MouseOverFaceIdx = it->second[RenderIdx - 1];
				break;
			}
			else
			{
				RenderIdx -= NumFace;
				MouseOverGroupIdx++;
			}
		}
	}
	
	// 마우스와 중첩된 삼각형의 세 정점을 구하여,
	int vidx0 = pMesh->Faces[MouseOverFaceIdx].vIdx[0];
	int vidx1 = pMesh->Faces[MouseOverFaceIdx].vIdx[1];
	int vidx2 = pMesh->Faces[MouseOverFaceIdx].vIdx[2];
	GVertex *v0 = &(pMesh->Verts[vidx0]);
	GVertex *v1 = &(pMesh->Verts[vidx1]);
	GVertex *v2 = &(pMesh->Verts[vidx2]);

	// 세 정점이 투영된 스크린상의 좌표를 구하고,
	GPoint3 p0, p1, p2;
	pEditor->pScene->MainCamera.GetScreenCoord(v0, &p0.V[0], &p0.V[1], &p0.V[2]);
	pEditor->pScene->MainCamera.GetScreenCoord(v1, &p1.V[0], &p1.V[1], &p1.V[2]);
	pEditor->pScene->MainCamera.GetScreenCoord(v2, &p2.V[0], &p2.V[1], &p2.V[2]);

	// 현재 마우스 위치와 거리를 계산하여, 
	double dist0 = dist(GPoint3(x, y, 0.0), p0);
	double dist1 = dist(GPoint3(x, y, 0.0), p1);
	double dist2 = dist(GPoint3(x, y, 0.0), p2);

	// 마우스 포인터와 가장 가까운 정점의 인덱스를 찾는다.
	double min_dist = MIN(dist0, MIN(dist1, dist2));
	if (min_dist == dist0)
		MouseOverVertIdx = vidx0;
	else if (min_dist == dist1)
		MouseOverVertIdx = vidx1;
	else
		MouseOverVertIdx = vidx2;

	// 2D Baricentric 좌표를 계산한다.
	double a = p0[0] - p2[0];
	double b = p1[0] - p2[0];
	double c = p0[1] - p2[1];
	double d = p1[1] - p2[1];
	double e = x - p2[0];
	double f = y - p2[1];

	double det = a * d - b * c;
	double alpha = (d * e - b * f) / det;
	double beta = (-c * e + a * f) / det;
	MouseOverPtCoords.Set(alpha, beta, 1.0 - alpha - beta);

	return true;
}

///////////////////////////////////////////////
/*!
*	\brief 디폴트 생성자
*/
PEditor::PEditor()
{	
	// 기본 데이터 멤버(13)
	EC = GTransf();							// 편집 툴의 편집 좌표계 (객체 선택시 업데이트)
	EditToolType = EDIT_INACTIVE;			// 편집 툴 유형(이동, 회전, 스케일 중)
	EditObjList.clear();					// 선택된 편집 객체 리스트
	EditVertRgn.clear();					// 부드러운 편집을 위해 추가로 선택된 정점 리스트
	EditFaceRgn.clear();					// 부드러운 편집을 위해 추가로 선택된 삼각형 리스트(렌더링 용)
	pEditFaceRgnBvh = NULL;					// 부드러운 편집을 위해 추가로 선택된 삼각형 리스트의 BVH
	EditBndryPts.clear();					// 편집 영역의 경계 정점들
	pUnfoldMesh = NULL;						// 2차원에서 펼쳐진 선택 영역
	UnfoldFaceMap.clear();					// 2차원 매개화 메쉬와 3차원 편집 영역간의 삼각형 대응 정보
	MousePt[0] = CPoint(0, 0);				// 선택 및 편집을 위한 마우스의 위치
	MousePt[1] = CPoint(0, 0);				// 선택 및 편집을 위한 마우스의 위치
	RotRefX = GTransf();					// 회전 편집을 할 경우, 최초 회전 핸들이 선택된 참조 좌표계
	RotRefPos = GVector3();					// 회전 편집을 할 경우, 회전 핸들의 선택된 지점
	
	// 추가 데이터 멤버(2)
	pScene = NULL;					// 메인 프로그램의 장면에 대한 포인터
	theSelector.pEditor = this;		// 편집 객체 선택을 위한 객체
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사될 객체
*/
PEditor::PEditor(const PEditor &cpy)
{
	// 기본 데이터 멤버(13)
	EC = cpy.EC;							// 편집 툴의 편집 좌표계 (객체 선택시 업데이트)
	EditToolType = cpy.EditToolType;		// 편집 툴 유형(이동, 회전, 스케일 중)
	EditObjList = cpy.EditObjList;			// 선택된 편집 객체 리스트
	EditVertRgn = cpy.EditVertRgn;			// 부드러운 편집을 위해 추가로 선택된 편집 객체 리스트
	EditFaceRgn = cpy.EditFaceRgn;			// 편집될 영역에 포함된 삼각형들(렌더링 용)
	pEditFaceRgnBvh = NULL;					// 부드러운 편집을 위해 추가로 선택된 삼각형 리스트의 BVH
	EditBndryPts = cpy.EditBndryPts;		// 편집 영역의 경계 정점들
	pUnfoldMesh = NULL;						// 2차원에서 펼쳐진 선택 영역
	UnfoldFaceMap.clear();					// 2차원 매개화 메쉬와 3차원 편집 영역간의 삼각형 대응 정보
	MousePt[0] = cpy.MousePt[0];			// 선택 및 편집을 위한 마우스의 위치
	MousePt[1] = cpy.MousePt[1];			// 선택 및 편집을 위한 마우스의 위치
	RotRefX = cpy.RotRefX;					// 회전 편집을 할 경우, 최초 회전 핸들이 선택된 참조 좌표계
	RotRefPos = cpy.RotRefPos;				// 회전 편집을 할 경우, 회전 핸들의 선택된 지점
	
	// 추가 데이터 멤버(2)
	pScene = cpy.pScene; 							// 메인 프로그램의 장면에 대한 포인터
	theSelector.pEditor = cpy.theSelector.pEditor;	// 편집 객체 선택을 위한 객체
}

/*!
*	\brief 소멸자
*/
PEditor::~PEditor()
{
	if (pUnfoldMesh != NULL)
	{
		delete pUnfoldMesh;
		pUnfoldMesh = NULL;
		UnfoldFaceMap.clear();
	}

	if (pEditFaceRgnBvh != NULL)
	{
		delete pEditFaceRgnBvh;
		pEditFaceRgnBvh = NULL;
	}
}

/*!
*	\brief 대입 연산자
*
*	\param rhs 대입될 객체
*/
PEditor &PEditor::operator =(const PEditor &rhs)
{
	// 기본 데이터 멤버(11)
	EC = rhs.EC;							// 편집 툴의 편집 좌표계 (객체 선택시 업데이트)
	EditToolType = rhs.EditToolType;		// 편집 툴 유형(이동, 회전, 스케일 중)
	EditObjList = rhs.EditObjList;			// 선택된 편집 객체 리스트
	EditVertRgn = rhs.EditVertRgn;			// 부드러운 편집을 위해 추가로 선택된 편집 객체 리스트
	EditFaceRgn = rhs.EditFaceRgn;			// 편집될 영역에 포함된 삼각형들(렌더링 용)
	pEditFaceRgnBvh = NULL;					// 부드러운 편집을 위해 추가로 선택된 삼각형 리스트의 BVH
	EditBndryPts = rhs.EditBndryPts;		// 편집 영역의 경계 정점들
	pUnfoldMesh = NULL;						// 2차원에서 펼쳐진 선택 영역
	UnfoldFaceMap.clear();					// 2차원 매개화 메쉬와 3차원 편집 영역간의 삼각형 대응 정보
	MousePt[0] = rhs.MousePt[0];			// 선택 및 편집을 위한 마우스의 위치
	MousePt[1] = rhs.MousePt[1];			// 선택 및 편집을 위한 마우스의 위치
	RotRefX = rhs.RotRefX;					// 회전 편집을 할 경우, 최초 회전 핸들이 선택된 참조 좌표계
	RotRefPos = rhs.RotRefPos;				// 회전 편집을 할 경우, 회전 핸들의 선택된 지점
	
	// 추가 데이터 멤버(2)
	pScene = rhs.pScene; 							// 메인 프로그램의 장면에 대한 포인터
	theSelector.pEditor = rhs.theSelector.pEditor;	// 편집 객체 선택을 위한 객체	
	
	return *this;
}

/*!
*	\brief 에티터의 장면에 대한 포인터를 설정한다.
*/
void PEditor::InitPlugIn(GScene *_pScene, CDialog *_pParent, CDocument *_pDoc)
{
	pScene = _pScene;
	theSelector.pEditor = this;
	
	EditMeshDlg.Create(IDD_EDIT_MESH_DLG, _pParent);
	EditMeshDlg.pEditor = this;
	EditMeshDlg.pDoc = _pDoc;

	EditCrvDlg.Create(IDD_EDIT_CRV_DLG, _pParent);
	EditCrvDlg.pEditor = this;
	EditCrvDlg.pDoc = _pDoc;

	EditSrfDlg.Create(IDD_EDIT_SRF_DLG, _pParent);
	EditSrfDlg.pEditor = this;
	EditSrfDlg.pDoc = _pDoc;

	EditVolDlg.Create(IDD_EDIT_VOL_DLG, _pParent);
	EditVolDlg.pEditor = this;
	EditVolDlg.pDoc = _pDoc;

	EditSwpDlg.Create(IDD_EDIT_SWP_DLG, _pParent);
	EditSwpDlg.pEditor = this;
	EditSwpDlg.pDoc = _pDoc;

	EditBlendSrfDlg.Create(IDD_EDIT_BLEND_SRF_DLG, _pParent);
	EditBlendSrfDlg.pEditor = this;
	EditBlendSrfDlg.pDoc = _pDoc;
}

/*!
*	\brief 편집 객체, 에디터 그리고 선택 영역을 렌더링 한다.
*/
void PEditor::Render()
{
	GMesh *pMesh = GetEditMesh();
	if (pMesh != NULL)
	{
		// 월드 좌표계에서
		glPushMatrix();	 
		{
			// 부드러운 편집 영역을 렌더링 한다.
			RenderEditRgn();

			// 선택된 객체의 표시 경계 상자를 렌더링하고,
			pMesh->RenderForEdit(1.0, 0.0, 0.0);

			// 선택된 객체의 개수를 구하고,
			int NumObj = (int)EditObjList.size();

			// 각각의 선택된 객체에 대하여,
			for (int i = 0; i < NumObj; ++i)
			{
				// 선택 표시 경계 상자를 렌더링 한다.
				if (i == 0 && EditObjList[i])
					EditObjList[i]->RenderForEdit(1.0, 0.0, 0.0);
				else
					EditObjList[i]->RenderForEdit(1.0, 1.0, 1.0);
			}			

			// 편집 툴의 편집 좌표계로 변환하여,
			GTransf EC = GetEC();
			glMultMatrixd(cast_mat(EC).M);

			// 편집 툴의 유형에 따라, 해당 툴을 렌더링하고,
			double ToolSize = GetEditToolSize();
			switch (EditToolType)
			{
			case EDIT_TRA:
			case EDIT_TRA_X:
			case EDIT_TRA_Y:
			case EDIT_TRA_Z:
			case EDIT_TRA_XY:
			case EDIT_TRA_YZ:
			case EDIT_TRA_ZX:
			case EDIT_TRA_XYZ:
				RenderTra(ToolSize);
				break;

			case EDIT_ROT:
			case EDIT_ROT_X:
			case EDIT_ROT_Y:
			case EDIT_ROT_Z:
				RenderRot(ToolSize);
				break;

			case EDIT_SCL:
			case EDIT_SCL_X:
			case EDIT_SCL_Y:
			case EDIT_SCL_Z:
			case EDIT_SCL_XY:
			case EDIT_SCL_YZ:
			case EDIT_SCL_ZX:
			case EDIT_SCL_XYZ:
				RenderScl(ToolSize);
				break;
			}
		}
		glPopMatrix(); // 월드 좌표계로 복원한다.
	}	
		
	// 객체 선택 중이라면, 선택 유형에 따라 선택 영역을 렌더링 한다.
	theSelector.Render();
}

/*!
*	\brief 편집 영역을 렌더링 한다.
*/
void PEditor::RenderEditRgn()
{
	// 편집 메쉬를 구하고 없으면 리턴한다.
	GMesh *pMesh = GetEditMesh();
	if (pMesh == NULL)
		return;

	// 월드 좌표계에서
	glPushMatrix();
	{
		// 편집 메쉬의 모델링 좌표계로 변환하고,
		glMultMatrixd(cast_mat(pMesh->MC).M);

		// 조명을 비활성화 하고, 오프셋과 블렌딩을 활성화 하고,
		glDisable(GL_LIGHTING);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// 편집 영역의 삼각형을 렌더링 한다.
		if (pMesh->FaceType == FACE_QUAD)
		{
			glBegin(GL_QUADS);
			{
				int NumFace = (int)EditFaceRgn.size();
				for (int i = 0; i < NumFace - 1; i += 2)
				{
					int vidx0 = EditFaceRgn[i]->vIdx[0];
					int vidx1 = EditFaceRgn[i]->vIdx[1];
					int vidx2 = EditFaceRgn[i]->vIdx[2];
					int vidx3 = EditFaceRgn[i + 1]->vIdx[2];

					double w0 = pMesh->Verts[vidx0].EditWgt;
					double w1 = pMesh->Verts[vidx1].EditWgt;
					double w2 = pMesh->Verts[vidx2].EditWgt;
					double w3 = pMesh->Verts[vidx3].EditWgt;

					glColor4f(1.0, 0.0, 0.0, w0);
					glVertex3dv(pMesh->Verts[vidx0].P.V);

					glColor4f(1.0, 0.0, 0.0, w1);
					glVertex3dv(pMesh->Verts[vidx1].P.V);

					glColor4f(1.0, 0.0, 0.0, w2);
					glVertex3dv(pMesh->Verts[vidx2].P.V);

					glColor4f(1.0, 0.0, 0.0, w3);
					glVertex3dv(pMesh->Verts[vidx3].P.V);
				}			
			}
			glEnd();
		}
		else
		{
			glBegin(GL_TRIANGLES);
			{
				int NumFace = (int)EditFaceRgn.size();
				for (int i = 0; i < NumFace; ++i)
				{
					int vidx0 = EditFaceRgn[i]->vIdx[0];
					int vidx1 = EditFaceRgn[i]->vIdx[1];
					int vidx2 = EditFaceRgn[i]->vIdx[2];

					double w0 = pMesh->Verts[vidx0].EditWgt;
					double w1 = pMesh->Verts[vidx1].EditWgt;
					double w2 = pMesh->Verts[vidx2].EditWgt;

					//glColor4f(0.2627, 1.0, 0.64, w0);
					glColor4f(1.0, 0.0, 0.0, w0);
					glVertex3dv(pMesh->Verts[vidx0].P.V);

					//glColor4f(0.2627, 1.0, 0.64, w1);
					glColor4f(1.0, 0.0, 0.0, w1);
					glVertex3dv(pMesh->Verts[vidx1].P.V);

					//glColor4f(0.2627, 1.0, 0.64, w2);
					glColor4f(1.0, 0.0, 0.0, w2);
					glVertex3dv(pMesh->Verts[vidx2].P.V);
				}			
			}
			glEnd();
		}

		// 편집 영역의 경계를 렌더링한다.
		glEnable(GL_LINE_SMOOTH);

		// 근사된 편집 경계를 표시한다면
		if (EditMeshDlg.m_ShowEditBndryType == 0)
		{
			glLineWidth(2.5);
			//glDisable(GL_DEPTH_TEST);			
			glColor3f(1.0, 1.0, 0.0);
			glBegin(GL_LINES);
			for (unsigned i = 0; i < EditBndryPts.size(); i += 2)
			{
				glVertex3dv(EditBndryPts[i].V);
				glVertex3dv(EditBndryPts[i + 1].V);
			}
			glEnd();
			//glEnable(GL_DEPTH_TEST);
			glLineWidth(1.0);
			
		}
		else	// 정확한 편집 경계를 표시한다면,	
		{
			glLineWidth(2.5);
			//glDisable(GL_DEPTH_TEST);
			glBegin(GL_LINE_LOOP);
			int NumPt = (int)EditBndryPts.size();
			for (int i = 0; i < NumPt; ++i)
			{
				double t = (double) i / (NumPt - 1);
				GVector3 C = get_color_gradient_from_rainbow(t);
				glColor3dv(C.V);
				glVertex3dv(EditBndryPts[i].V);
			}
			glEnd();
			//glEnable(GL_DEPTH_TEST);
			glLineWidth(1.0);			
		}
		
		// 블렌딩을 비활성화하고 조명과 깊이 테스트를 활성화한다.
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_BLEND);
		glDisable(GL_LINE_SMOOTH);
		glEnable(GL_LIGHTING);
	}
	glPopMatrix();	// 월드 좌표계로 복원한다.

	// 펼쳐진 편집 영역을 표시한다.
// 	glTranslatef(-10.0, 00.0, 0.0);
// 	if (pUnfoldMesh != NULL)
// 	{
//  	pUnfoldMesh->Render();
// 		double t = EditMeshDlg.m_EditRgnDist / 1000.0;
// 		double EditDist = pMesh->BBox.GetDiagonal() * t;
// 		int NumSamp = 100;
// 		glDisable(GL_LIGHTING);
// 		glColor3f(1.0, 0.0, 0.0);
// 		glBegin(GL_LINE_LOOP);
// 		for (int i = 0; i < NumSamp; ++i)
// 		{
// 			double t = (double) i / (NumSamp - 1);
// 			double theta = t * 2.0 * M_PI;
// 			GVector3 C = get_color_gradient_from_rainbow(t);
// 			GVector2 p(EditDist * cos(theta), EditDist * sin(theta));
// 			glColor3dv(C.V);
// 			glVertex2dv(p.V);
// 		}
// 		glEnd();
// 		glEnable(GL_LIGHTING);	
// 	}
// 	glTranslatef(10.0, 0.0, 0.0);
}

/*!
*	\brief 에디터의 이동핸들을 렌더링 한다.
*/
void PEditor::RenderTra(double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	RenderTraHandleX(len);
	RenderTraHandleY(len);
	RenderTraHandleZ(len);
	RenderTraHandleXY(len);
	RenderTraHandleYZ(len);
	RenderTraHandleZX(len);
	RenderTraHandleXYZ(len);

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

/*!
*	\brief 에디터의 이동핸들을 렌더링 한다.
*
*	\param len 이동 핸들의 길이 
*/
void PEditor::RenderTraHandleX(double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	if (EditToolType == EDIT_TRA_X)
		glColor3d(1.0, 1.0, 0.0);
	else
		glColor3d(1.0, 0.0, 0.0);

	glBegin(GL_LINES);
	glVertex3d(len * 0.4, 0.0, 0.0);
	glVertex3d(len, 0.0f, 0.0f);
	glEnd();

	glPushMatrix();
	glTranslated(len, 0.0, 0.0);
	glRotated(90.0, 0.0, 1.0, 0.0);
	glutSolidCone(len * 0.1, len * 0.3, 10, 10);
	glPopMatrix();
}

/*!
*	\brief 에디터의 이동핸들을 렌더링 한다.
*
*	\param len 이동 핸들의 길이 
*/
void PEditor::RenderTraHandleY(double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	if (EditToolType == EDIT_TRA_Y)
		glColor3d(1.0, 1.0, 0.0);
	else
		glColor3d(0.0, 1.0, 0.0);

	glBegin(GL_LINES);
	glVertex3d(0.0, len * 0.4, 0.0);
	glVertex3d(0.0f, len, 0.0f);
	glEnd();

	glPushMatrix();
	glTranslated(0.0, len, 0.0);
	glRotated(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(len * 0.1, len * 0.3, 10, 10);
	glPopMatrix();
}

/*!
*	\brief 에디터의 이동핸들을 렌더링 한다.
*
*	\param len 이동 핸들의 길이
*/
void PEditor::RenderTraHandleZ(double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	if (EditToolType == EDIT_TRA_Z)
		glColor3d(1.0, 1.0, 0.0);
	else
		glColor3d(0.0, 0.0, 1.0);

	glBegin(GL_LINES);
	glVertex3d(0.0, 0.0, len * 0.4);
	glVertex3d(0.0, 0.0, len);
	glEnd();

	glPushMatrix();
	glTranslated(0.0, 0.0, len);
	glutSolidCone(len * 0.1, len * 0.3, 10, 10);
	glPopMatrix();
}

/*!
*	\brief 에디터의 이동핸들을 렌더링 한다.
*
*	\param len 이동 핸들의 길이
*/
void PEditor::RenderTraHandleXY(double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	if (EditToolType == EDIT_TRA_XY)
	{
		glColor3d(1.0, 1.0, 0.0);
		glBegin(GL_POLYGON);
		{
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d(len * 0.4, 0.0, 0.0);
			glVertex3d(len * 0.4, len * 0.4, 0.0);
			glVertex3d(0.0, len * 0.4, 0.0);
		}
		glEnd();
	}
	else
	{
		glBegin(GL_LINES);
		{
			glColor3d(1.0, 0.0, 0.0);
			glVertex3d(len * 0.4, 0.0, 0.0);
			glVertex3d(len * 0.4, len * 0.4, 0.0);
			glColor3d(0.0, 1.0, 0.0);
			glVertex3d(len * 0.4, len * 0.4, 0.0);
			glVertex3d(0.0, len * 0.4, 0.0);
		}
		glEnd();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4d(0.0, 0.0, 0.0, 0.0);
		glBegin(GL_POLYGON);
		{
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d(len * 0.4, 0.0, 0.0);
			glVertex3d(len * 0.4, len * 0.4, 0.0);
			glVertex3d(0.0, len * 0.4, 0.0);
		}
		glEnd();
		glDisable(GL_BLEND);
	}
}

/*!
*	\brief 에디터의 이동핸들을 렌더링 한다.
*
*	\param len 이동 핸들의 길이
*/
void PEditor::RenderTraHandleYZ(double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	if (EditToolType == EDIT_TRA_YZ)
	{
		glColor3d(1.0, 1.0, 0.0);
		glBegin(GL_POLYGON);
		{
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d(0.0, len * 0.4, 0.0);
			glVertex3d(0.0, len * 0.4, len * 0.4);
			glVertex3d(0.0, 0.0, len * 0.4);
		}
		glEnd();
	}
	else
	{
		glBegin(GL_LINES);
		{
			glColor3f(0.0, 1.0, 0.0);
			glVertex3d(0.0, len * 0.4, 0.0);
			glVertex3d(0.0, len * 0.4, len * 0.4);
			glColor3f(0.0, 0.0, 1.0);
			glVertex3d(0.0, len * 0.4, len * 0.4);
			glVertex3d(0.0, 0.0, len * 0.4);
		}
		glEnd();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4d(0.0, 0.0, 0.0, 0.0);
		glBegin(GL_POLYGON);
		{
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d(0.0, len * 0.4, 0.0);
			glVertex3d(0.0, len * 0.4, len * 0.4);
			glVertex3d(0.0, 0.0, len * 0.4);
		}
		glEnd();
		glDisable(GL_BLEND);
	}
}

/*!
*	\brief 에디터의 이동핸들을 렌더링 한다.
*
*	\param len 이동 핸들의 길이
*/
void PEditor::RenderTraHandleZX(double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	if (EditToolType == EDIT_TRA_ZX)
	{
		glColor3d(1.0, 1.0, 0.0);
		glBegin(GL_POLYGON);
		{
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d(0.0, 0.0, len * 0.4);
			glVertex3d(len * 0.4, 0.0, len * 0.4);
			glVertex3d(len * 0.4, 0.0, 0.0);			
		}
		glEnd();
	}
	else
	{
		glBegin(GL_LINES);
		{
			glColor3f(0.0, 0.0, 1.0);
			glVertex3d(0.0, 0.0, len * 0.4);
			glVertex3d(len * 0.4, 0.0, len * 0.4);

			glColor3f(1.0, 0.0, 0.0);
			glVertex3d(len * 0.4, 0.0, len * 0.4);
			glVertex3d(len * 0.4, 0.0, 0.0);
		}
		glEnd();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4d(0.0, 0.0, 0.0, 0.0);
		glBegin(GL_POLYGON);
		{
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d(0.0, 0.0, len * 0.4);
			glVertex3d(len * 0.4, 0.0, len * 0.4);
			glVertex3d(len * 0.4, 0.0, 0.0);
		}
		glEnd();
		glDisable(GL_BLEND);
	}
}

/*!
*	\brief 에디터의 이동핸들을 렌더링 한다.
*
*	\param len 이동 핸들의 길이
*/
void PEditor::RenderTraHandleXYZ(double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	if (EditToolType == EDIT_TRA_XYZ)
	{
		glColor3d(1.0, 1.0, 0.0);
		glPushMatrix();
		{
			glTranslated(len * 0.1, len * 0.1, len * 0.1);
			glutSolidCube(len * 0.2);
		}
		glPopMatrix();
	}
	else
	{
		glColor3d(0.0, 0.0, 0.0);
		glPushMatrix();
		{
			glTranslated(len * 0.1, len * 0.1, len * 0.1);
			glutWireCube(len * 0.2);
		}
		glPopMatrix();
	}
}

/*!
*	\brief 에디터의 회전 핸들을 렌더링 한다.
*
*	\param len 회전 핸들의 길이
*/
void PEditor::RenderRot(double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	double modelview[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

	// 관측 방향의 테두리
	GQuater qt;
	qt.SetFromMatrix(modelview, true);
	qt = inv(qt);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	int snum = 50;
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < snum; ++i)
	{
		double theta = (double)i / (double)(snum - 1) * M_PI * 2.0;
		GVector3 p = qt * GVector3(len * cos(theta), len * sin(theta), 0.0);
		glVertex3d(p[0], p[1], p[2]);
	}
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3d(0.0, 0.0, 0.0);
	for (int i = 0; i < snum; ++i)
	{
		double theta = (double)i / (double)(snum - 1) * M_PI * 2.0;
		GVector3 p = qt * GVector3(len * cos(theta) * 1.05, len * sin(theta) * 1.05, 0.0);
		glVertex3d(p[0], p[1], p[2]);
	}
	glEnd();

	// 관측방향의 테두리 뒤쪽 점들을 제거하기 위한 벡터 
	GVector3 view_axis(0.0, 0.0, 1.0);
	view_axis = qt * view_axis;

	glLineWidth(2.0);
	RenderRotHandleX(view_axis, len);
	RenderRotHandleY(view_axis, len);
	RenderRotHandleZ(view_axis, len);
	glLineWidth(1.0);

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

/*!
*	\brief 에디터의 회전 핸들을 렌더링 한다.
*
*	\param view_axis 뒤쪽 핸들을 지우기 위한 법선 벡터
*	\param len 회전 핸들의 길이
*/
void PEditor::RenderRotHandleX(const GVector3 &view_axis, double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	if (EditToolType == EDIT_ROT_X)
	{
		double modelview[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

		GTransf CurX(modelview, true);
		GTransf dX = inv(CurX) * RotRefX;
		GVector3 p = dX * RotRefPos;
		GVector3 q = RotRefPos;
		GVector3 axis = (p ^ q).Normalize();
		double theta = angle(p, q, false);
		GQuater qt;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0f, 1.0f, 0.0f, 0.5f);
		glBegin(GL_TRIANGLE_FAN);
		{
			glVertex3d(0.0, 0.0, 0.0);
			for (int i = 0; i < 20; ++i)
			{
				double phi = theta * (double)i / 19.0;
				qt.SetFromAngleAxis(phi, axis);
				GVector3 r = qt * p;
				glVertex3d(r[0], r[1], r[2]);
			}
		}
		glEnd();
		glDisable(GL_BLEND);
	}
	else
		glColor3f(1.0f, 0.0f, 0.0f);

	int snum = 50;
	for (int i = 0; i < snum; ++i)
	{
		double theta1 = (double)i / (double)(snum - 1) * M_PI * 2.0;
		GVector3 p1(0.0, len * cos(theta1), len * sin(theta1));
		if (view_axis * p1 > 0)
		{
			double theta2 = (double)(i + 1) / (double)(snum - 1) * M_PI * 2.0;
			GVector3 p2(0.0, len * cos(theta2), len * sin(theta2));
			glBegin(GL_LINES);
			glVertex3d(p1[0], p1[1], p1[2]);
			glVertex3d(p2[0], p2[1], p2[2]);
			glEnd();
		}
	}	
}

/*!
*	\brief 에디터의 회전 핸들을 렌더링 한다.
*
*	\param view_axis 뒤쪽 핸들을 지우기 위한 법선 벡터
*	\param len 회전 핸들의 길이
*/
void PEditor::RenderRotHandleY(const GVector3 &view_axis, double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	if (EditToolType == EDIT_ROT_Y)
	{
		double modelview[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

		GTransf CurX(modelview, true);
		GTransf dX = inv(CurX) * RotRefX;
		GVector3 p = dX * RotRefPos;
		GVector3 q = RotRefPos;
		GVector3 axis = (p ^ q).Normalize();
		double theta = angle(p, q, false);
		GQuater qt;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0f, 1.0f, 0.0f, 0.5f);
		glBegin(GL_TRIANGLE_FAN);
		{
			glVertex3d(0.0, 0.0, 0.0);
			for (int i = 0; i < 20; ++i)
			{
				double phi = theta * (double)i / 19.0;
				qt.SetFromAngleAxis(phi, axis);
				GVector3 r = qt * p;
				glVertex3d(r[0], r[1], r[2]);
			}
		}
		glEnd();
		glDisable(GL_BLEND);
	}
	else
		glColor3f(0.0f, 1.0f, 0.0f);

	int snum = 50;
	for (int i = 0; i < snum; ++i)
	{
		double theta1 = (double)i / (double)(snum - 1) * M_PI * 2.0;
		GVector3 p1(len * sin(theta1), 0.0, len * cos(theta1));
		if (view_axis * p1 > 0)
		{
			double theta2 = (double)(i + 1) / (double)(snum - 1) * M_PI * 2.0;
			GVector3 p2(len * sin(theta2), 0.0, len * cos(theta2));
			glBegin(GL_LINES);
			glVertex3d(p1[0], p1[1], p1[2]);
			glVertex3d(p2[0], p2[1], p2[2]);
			glEnd();
		}
	}
}

/*!
*	\brief 에디터의 회전 핸들을 렌더링 한다.
*
*	\param view_axis 뒤쪽 핸들을 지우기 위한 법선 벡터
*	\param len 회전 핸들의 길이
*/
void PEditor::RenderRotHandleZ(const GVector3 &view_axis, double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	if (EditToolType == EDIT_ROT_Z)
	{
		double modelview[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

		GTransf CurX(modelview, true);
		GTransf dX = inv(CurX) * RotRefX;
		GVector3 p = dX * RotRefPos;
		GVector3 q = RotRefPos;
		GVector3 axis = (p ^ q).Normalize();
		double theta = angle(p, q, false);
		GQuater qt;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0f, 1.0f, 0.0f, 0.5f);
		glBegin(GL_TRIANGLE_FAN);
		{
			glVertex3d(0.0, 0.0, 0.0);
			for (int i = 0; i < 20; ++i)
			{
				double phi = theta * (double)i / 19.0;
				qt.SetFromAngleAxis(phi, axis);
				GVector3 r = qt * p;
				glVertex3d(r[0], r[1], r[2]);
			}
		}
		glEnd();
		glDisable(GL_BLEND);
	}
	else
		glColor3f(0.0f, 0.0f, 1.0f);

	int snum = 50;
	for (int i = 0; i < snum; ++i)
	{
		double theta1 = (double)i / (double)(snum - 1) * M_PI * 2.0;
		GVector3 p1(len * cos(theta1), len * sin(theta1), 0.0);
		if (view_axis * p1 > 0)
		{
			double theta2 = (double)(i + 1) / (double)(snum - 1) * M_PI * 2.0;
			GVector3 p2(len * cos(theta2), len * sin(theta2), 0.0);
			glBegin(GL_LINES);
			glVertex3d(p1[0], p1[1], p1[2]);
			glVertex3d(p2[0], p2[1], p2[2]);
			glEnd();
		}
	}
}

/*!
*	\brief 에디터의 스케일 핸들을 렌더링 한다.
*
*	\param len 스케일 핸들의 길이
*/
void PEditor::RenderScl(double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	RenderSclHandleX(len);
	RenderSclHandleY(len);
	RenderSclHandleZ(len);
	RenderSclHandleXY(len);
	RenderSclHandleYZ(len);
	RenderSclHandleZX(len);
	RenderSclHandleXYZ(len);

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

/*!
*	\brief 에디터의 스케일 핸들을 렌더링 한다.
*
*	\param len 스케일 핸들의 길이
*/
void PEditor::RenderSclHandleX(double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	if (EditToolType == EDIT_SCL_X)
		glColor3d(1.0, 1.0, 0.0);
	else
		glColor3d(1.0, 0.0, 0.0);

	glBegin(GL_LINES);
	glVertex3d(len * 0.4, 0.0, 0.0);
	glVertex3d(len, 0.0f, 0.0f);
	glEnd();

	glPushMatrix();
	glTranslated(len, 0.0f, 0.0f);
	glutSolidCube((float)len * 0.2);
	glPopMatrix();
}

/*!
*	\brief 에디터의 스케일 핸들을 렌더링 한다.
*
*	\param len 스케일 핸들의 길이
*/
void PEditor::RenderSclHandleY(double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	if (EditToolType == EDIT_SCL_Y)
		glColor3d(1.0, 1.0, 0.0);
	else
		glColor3d(0.0, 1.0, 0.0);

	glBegin(GL_LINES);
	glVertex3d(0.0, len * 0.4, 0.0);
	glVertex3d(0.0f, len, 0.0f);
	glEnd();

	glPushMatrix();
	glTranslated(0.0, len, 0.0);
	glutSolidCube(len * 0.2);
	glPopMatrix();
}

/*!
*	\brief 에디터의 스케일 핸들을 렌더링 한다.
*
*	\param len 스케일 핸들의 길이
*/
void PEditor::RenderSclHandleZ(double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	if (EditToolType == EDIT_SCL_Z)
		glColor3d(1.0, 1.0, 0.0);
	else
		glColor3d(0.0, 0.0, 1.0);

	glBegin(GL_LINES);
	glVertex3d(0.0, 0.0, len * 0.4);
	glVertex3d(0.0, 0.0, len);
	glEnd();

	glPushMatrix();
	glTranslated(0.0, 0.0, len);
	glutSolidCube(len * 0.2);
	glPopMatrix();
}

/*!
*	\brief 에디터의 스케일 핸들을 렌더링 한다.
*
*	\param len 이동 핸들의 길이
*/
void PEditor::RenderSclHandleXY(double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	if (EditToolType == EDIT_SCL_XY)
	{
		glColor3d(1.0, 1.0, 0.0);
		glBegin(GL_POLYGON);
		{
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d(len * 0.4, 0.0, 0.0);
			glVertex3d(len * 0.4, len * 0.4, 0.0);
			glVertex3d(0.0, len * 0.4, 0.0);
		}
		glEnd();
	}
	else
	{
		glBegin(GL_LINES);
		{
			glColor3d(1.0, 0.0, 0.0);
			glVertex3d(len * 0.4, 0.0, 0.0);
			glVertex3d(len * 0.4, len * 0.4, 0.0);
			glColor3d(0.0, 1.0, 0.0);
			glVertex3d(len * 0.4, len * 0.4, 0.0);
			glVertex3d(0.0, len * 0.4, 0.0);
		}
		glEnd();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4d(0.0, 0.0, 0.0, 0.0);
		glBegin(GL_POLYGON);
		{
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d(len * 0.4, 0.0, 0.0);
			glVertex3d(len * 0.4, len * 0.4, 0.0);
			glVertex3d(0.0, len * 0.4, 0.0);
		}
		glEnd();
		glDisable(GL_BLEND);
	}
}

/*!
*	\brief 에디터의 스케일 핸들을 렌더링 한다.
*
*	\param len 이동 핸들의 길이
*/
void PEditor::RenderSclHandleYZ(double len)
{

	// 현재 좌표계: 편집 툴의 좌표계
	if (EditToolType == EDIT_SCL_YZ)
	{
		glColor3d(1.0, 1.0, 0.0);
		glBegin(GL_POLYGON);
		{
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d(0.0, len * 0.4, 0.0);
			glVertex3d(0.0, len * 0.4, len * 0.4);
			glVertex3d(0.0, 0.0, len * 0.4);
		}
		glEnd();
	}
	else
	{
		glBegin(GL_LINES);
		{
			glColor3f(0.0, 1.0, 0.0);
			glVertex3d(0.0, len * 0.4, 0.0);
			glVertex3d(0.0, len * 0.4, len * 0.4);
			glColor3f(0.0, 0.0, 1.0);
			glVertex3d(0.0, len * 0.4, len * 0.4);
			glVertex3d(0.0, 0.0, len * 0.4);
		}
		glEnd();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4d(0.0, 0.0, 0.0, 0.0);
		glBegin(GL_POLYGON);
		{
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d(0.0, len * 0.4, 0.0);
			glVertex3d(0.0, len * 0.4, len * 0.4);
			glVertex3d(0.0, 0.0, len * 0.4);
		}
		glEnd();
		glDisable(GL_BLEND);
	}
}

/*!
*	\brief 에디터의 스케일 핸들을 렌더링 한다.
*
*	\param len 이동 핸들의 길이
*/
void PEditor::RenderSclHandleZX(double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	if (EditToolType == EDIT_SCL_ZX)
	{
		glColor3d(1.0, 1.0, 0.0);
		glBegin(GL_POLYGON);
		{
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d(0.0, 0.0, len * 0.4);
			glVertex3d(len * 0.4, 0.0, len * 0.4);
			glVertex3d(len * 0.4, 0.0, 0.0);			
		}
		glEnd();
	}
	else
	{
		glBegin(GL_LINES);
		{
			glColor3f(0.0, 0.0, 1.0);
			glVertex3d(0.0, 0.0, len * 0.4);
			glVertex3d(len * 0.4, 0.0, len * 0.4);

			glColor3f(1.0, 0.0, 0.0);
			glVertex3d(len * 0.4, 0.0, len * 0.4);
			glVertex3d(len * 0.4, 0.0, 0.0);
		}
		glEnd();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4d(0.0, 0.0, 0.0, 0.0);
		glBegin(GL_POLYGON);
		{
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d(0.0, 0.0, len * 0.4);
			glVertex3d(len * 0.4, 0.0, len * 0.4);
			glVertex3d(len * 0.4, 0.0, 0.0);
		}
		glEnd();
		glDisable(GL_BLEND);
	}
}

/*!
*	\brief 에디터의 스케일 핸들을 렌더링 한다.
*
*	\param len 스케일 핸들의 길이
*/
void PEditor::RenderSclHandleXYZ(double len)
{
	// 현재 좌표계: 편집 툴의 좌표계
	if (EditToolType == EDIT_SCL_XYZ)
	{
		glColor3d(1.0, 1.0, 0.0);
		glPushMatrix();
		{
			glTranslated(len * 0.1, len * 0.1, len * 0.1);
			glutSolidCube(len * 0.2);
		}
		glPopMatrix();
	}
	else
	{
		glColor3d(0.0, 0.0, 0.0);
		glPushMatrix();
		{
			glTranslated(len * 0.1, len * 0.1, len * 0.1);
			glutWireCube(len * 0.2);
		}
		glPopMatrix();
	}
}

/*!
*	\brief 에디터의 핸들 선택하거나, 객체 선택을 위한 준비 작업을 한다.
*
*	\param x 마우스의 X 좌표
*	\param y 마우스의 Y 좌표
*	\param nFlags 마우스 왼쪽 버튼이 눌릴 때의 추가 정보 (MK_CONTROL...)
*/
void PEditor::LButtonDown(int x, int y, UINT nFlags)
{
	// 편집을 위해서 마우스 위치 저장하고,
	MousePt[0] = MousePt[1] = CPoint(x, y);

	// 편집 툴(이동, 회전, 스케일)의 핸들이 선택하고,
	bool bEditStart = SelectEditHandle(x, y);

	// 편집 툴이 선택되지 않고, 측지 경로를 편집하는 경우가 아니라면
	if (!bEditStart && EditToolType != EDIT_GEODESIC_PATH && EditToolType != EDIT_SCULPT && EditToolType != EDIT_SMOOTH)
		theSelector.LButtonDown(x, y, nFlags);	// 객체 선택을 준비한다.
}

/*!
*	\brief 선택 객체를 편집하거나, 선택 영역을 정의한다.
*
*	\param x 마우스의 X 좌표
*	\param y 마우스의 Y 좌표
*	\param nFlags 마우스가 움직일 때, 눌려진 버튼의 정보 (MK_LBUTTON, MK_RBUTTON, MK_MBUTTON)
*/
void PEditor::MouseMove(int x, int y, UINT nFlags)
{
	// 카메라 좌표계에서 표현된 마우스의 변위를 계산하고,
	double dx = (x - MousePt[1].x);
	double dy = -(y - MousePt[1].y);
	double dz = 0.0;
	
	// 마우스 현재 위치를 갱신한다.
	MousePt[1] = CPoint(x, y);

	// 왼쪽 마우스 버튼이 눌린 상태라면 편집 객체를 편집하고 리턴한다.
	if (nFlags & MK_LBUTTON)
	{
		// 객체가 이동 편집이 되었다면 리턴한다.
		if (EditTra(dx, dy, dz))
			return;

		// 객체가 회전 편집이 되었다면 리턴한다.
		if (EditRot(dx, dy, dz))
			return;

		// 객체가 스케일 편집이 되었다면 리턴한다.
		if (EditScl(dx, dy, dz))
			return;

		if (EditGeodesicPath(x, y))
			return;

		if (EditSculpt(x, y))
			return;

		if (EditSmooth(x, y))
			return;
	}
	
	// 객체의 편집이 이루어지지 않은 경우 또는 마우스 왼쪽 버튼이 눌러지지 않은 경우
	// 편집 영역을 정의한다.
	theSelector.MouseMove(x, y, nFlags);			
}

/*!
*	\brief 객체를 이동 편집한다.
*
*	\param dx X 축 이동변위
*	\param dy Y 축 이동변위
*	\param dz Z 축 이동변위
*
*	\return 편집이 된 경우 true, 편집이 되지 않은 겨우 false를 반환한다.
*/
bool PEditor::EditTra(double dx, double dy, double dz)
{
	// 편집 좌표계에서 표현된 변위를 구하여,
	double scale = -(pScene->MainCamera.ZoomDist * 0.0005);
	GVector3 t = inv(pScene->GetWC() * EC) * GVector3(dx, dy, dz) * scale;

	// 편집 툴의 핸들이 선택된 경우,
	switch (EditToolType)
	{
		// 객체 이동을 수행 후 리턴한다.
	case EDIT_TRA_X:
		Edit(GVector3(t[0], 0.0, 0.0), GQuater(), GVector3(1.0, 1.0, 1.0));
		return true;

	case EDIT_TRA_Y:
		Edit(GVector3(0.0, t[1], 0.0), GQuater(), GVector3(1.0, 1.0, 1.0));
		return true;

	case EDIT_TRA_Z:
		Edit(GVector3(0.0, 0.0, t[2]), GQuater(), GVector3(1.0, 1.0, 1.0));
		return true;

	case EDIT_TRA_XY:
		Edit(GVector3(t[0], t[1], 0.0), GQuater(), GVector3(1.0, 1.0, 1.0));
		return true;

	case EDIT_TRA_YZ:
		Edit(GVector3(0.0, t[1], t[2]), GQuater(), GVector3(1.0, 1.0, 1.0));
		return true;

	case EDIT_TRA_ZX:
		Edit(GVector3(t[0], 0.0, t[2]), GQuater(), GVector3(1.0, 1.0, 1.0));
		return true;

	case EDIT_TRA_XYZ:
		Edit(t, GQuater(), GVector3(1.0, 1.0, 1.0));
		return true;
	}

	return false;
}

/*!
*	\brief 객체를 회전 편집한다.
*
*	\param dx X 축 이동변위
*	\param dy Y 축 이동변위
*	\param dz Z 축 이동변위
*
*	\return 편집이 된 경우 true, 편집이 되지 않은 겨우 false를 반환한다.
*/
bool PEditor::EditRot(double dx, double dy, double dz)
{
	// 편집 툴의 핸들이 선택된 경우,
	switch (EditToolType)
	{
		// 객체 회전을 수행 후 리턴한다.
	case EDIT_ROT_X:
		{
			GVector3 v = inv(RotRefX) * GVector3(dx, dy, dz);
			double theta = (GVector3(1.0, 0.0, 0.0) ^ RotRefPos).Normalize() * v * 0.01;
			GQuater q(GVector3(1.0, 0.0, 0.0), theta, true);
			Edit(GVector3(), q, GVector3(1.0, 1.0, 1.0));
		}
		return true;

	case EDIT_ROT_Y:
		{
			GVector3 v = inv(RotRefX) * GVector3(dx, dy, dz);
			double theta = (GVector3(0.0, 1.0, 0.0) ^ RotRefPos).Normalize() * v * 0.01;
			GQuater q(GVector3(0.0, 1.0, 0.0), theta, true);
			Edit(GVector3(), q, GVector3(1.0, 1.0, 1.0));
		}
		return true;

	case EDIT_ROT_Z:
		{
			GVector3 v = inv(RotRefX) * GVector3(dx, dy, dz);
			double theta = (GVector3(0.0, 0.0, 1.0) ^ RotRefPos).Normalize() * v * 0.01;
			GQuater q(GVector3(0.0, 0.0, 1.0), theta, true);
			Edit(GVector3(), q, GVector3(1.0, 1.0, 1.0));
		}
		return true;
	}

	return false;
}

/*!
*	\brief 객체를 스케일 편집한다.
*
*	\param dx X 축 이동변위
*	\param dy Y 축 이동변위
*	\param dz Z 축 이동변위
*
*	\return 편집이 된 경우 true, 편집이 되지 않은 겨우 false를 반환한다.
*/
bool PEditor::EditScl(double dx, double dy, double dz)
{
	// 편집 좌표계에서 표현된 변위를 구하여,
	double scale = -(pScene->MainCamera.ZoomDist * 0.0005);
	GVector3 t = inv(pScene->GetWC() * EC) * GVector3(dx, dy, dz) * scale;

	// 편집 툴의 핸들이 선택된 경우,
	switch (EditToolType)
	{
		// 객체 스케일 수행 후 리턴한다.
	case EDIT_SCL_X:
		Edit(GVector3(0.0, 0.0, 0.0), GQuater(), GVector3(1.0 + t[0], 1.0, 1.0));
		return true;

	case EDIT_SCL_Y:
		Edit(GVector3(0.0, 0.0, 0.0), GQuater(), GVector3(1.0, 1.0 + t[1], 1.0));
		return true;

	case EDIT_SCL_Z:
		Edit(GVector3(0.0, 0.0, 0.0), GQuater(), GVector3(1.0, 1.0, 1.0 + t[2]));
		return true;

	case EDIT_SCL_XY:
		{
			double s = t * GVector3(1.0, 1.0, 0.0);
			Edit(GVector3(0.0, 0.0, 0.0), GQuater(), GVector3(1.0 + s, 1.0 + s, 1.0));
		}
		return true;

	case EDIT_SCL_YZ:
		{
			double s = t * GVector3(0.0, 1.0, 1.0);
			Edit(GVector3(0.0, 0.0, 0.0), GQuater(), GVector3(1.0, 1.0 + s, 1.0 + s));
		}
		return true;

	case EDIT_SCL_ZX:
		{
			double s = t * GVector3(1.0, 0.0, 1.0);
			Edit(GVector3(0.0, 0.0, 0.0), GQuater(), GVector3(1.0 + s, 1.0, 1.0 + s));
		}
		return true;

	case EDIT_SCL_XYZ:
		{
			double s = t * GVector3(1.0, 1.0, 1.0);
			Edit(GVector3(0.0, 0.0, 0.0), GQuater(), GVector3(1.0 + s, 1.0 + s, 1.0 + s));
		}
		return true;
	}

	return false;
}

/*!
*	\brief 객체위에 정의된 측지 경로를 편집한다.
*
*	\param x 마우스의 x 좌표
*	\param y 마우스의 y 좌표
*
*	\return 편집이 된 경우 true, 편집이 되지 않은 겨우 false를 반환한다.
*/
bool PEditor::EditGeodesicPath(int x, int y)
{
	if (EditToolType != EDIT_GEODESIC_PATH)
		return false;

	// 마우스와 교차한 삼각형, 정점의 정보를 설정하고,
	theSelector.SetMouseOverInfo(x, y);

	//  선택된 정점이 있다면,
	if (theSelector.MouseOverVertIdx != -1)
	{
		GMesh *pMesh = GetEditMesh();
		std::vector<GMesh *> MeshList;
		GetPreOrderMeshList(pScene, MeshList, pMesh);

		// 먼저 편집 중인 경로 메쉬를 찾고,
		GGeoPath *pPath = NULL;
		for (unsigned i = 0; i < MeshList.size(); ++i)
		{
			if (MeshList[i]->GetClassName() != "GGeoPath")
				continue;

			if (((GGeoPath *)MeshList[i])->EditPathVertIdx != -1)
			{
				pPath = (GGeoPath *)MeshList[i];
				break;
			}
		}

		// 편집 중이 경로 메쉬가 있다면 경로를 수정한다.
		if (pPath != NULL)
		{
			pPath->EditPath(theSelector.MouseOverVertIdx);
			return true;
		}
	}

	return false;
}

/*!
*	\brief 메쉬를 Sculpting 한다.
*
*	\param x 마우스의 x 좌표
*	\param y 마우스의 y 좌표
*
*	\return 편집이 된 경우 true, 편집이 되지 않은 겨우 false를 반환한다.
*/
bool PEditor::EditSculpt(int x, int y)
{
	if (EditToolType != EDIT_SCULPT)
		return false;
	
	// 마우스와 교차한 삼각형, 정점의 정보찾아 없다면 리턴한다.
	if (!theSelector.SetMouseOverInfo(x, y))
		return false;

	// Sclupting 을 위한 편집 영역 및 가중치를 계산하고,
	FindEditWgtForSculpt();

	GMesh *pEditMesh = GetEditMesh();
	if (!EditVertRgn.empty())
	{
		// 편집 비율을 구하여,
		double wgt = pEditMesh->BBox.GetDiagonal() * 0.0003;

		// 선택된 삼각형의 법선을 구하여,
		int *vidx = pEditMesh->Faces[theSelector.MouseOverFaceIdx].vIdx;
		GVector3 n0 = pEditMesh->Verts[vidx[0]].N;
		GVector3 n1 = pEditMesh->Verts[vidx[1]].N;
		GVector3 n2 = pEditMesh->Verts[vidx[2]].N;
		GVector3 C = theSelector.MouseOverPtCoords;
		GVector3 N = n0 * C[0] + n1 * C[1] + n2 * C[2];
		N.Normalize();
		GVector3 EditDir = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? -N : N;

		// 주변 영역을 편집한다.
		for (unsigned i = 0; i < EditVertRgn.size(); ++i)
		{
			EditVertRgn[i]->P = EditVertRgn[i]->P + EditDir * EditVertRgn[i]->EditWgt * wgt;
			EditVertRgn[i]->N.Set(0, 0, 0);
		}

		// 법선을 갱신한다.
		for (unsigned i = 0; i < EditFaceRgn.size(); ++i)
		{
			int *vidx = EditFaceRgn[i]->vIdx;
			GVector3 p, q, n;
			p = pEditMesh->Verts[vidx[1]].P - pEditMesh->Verts[vidx[0]].P;
			q = pEditMesh->Verts[vidx[2]].P - pEditMesh->Verts[vidx[0]].P;
			n = (p ^ q).Normalize();

			pEditMesh->Verts[vidx[0]].N += n;
			pEditMesh->Verts[vidx[1]].N += n;
			pEditMesh->Verts[vidx[2]].N += n;
		}
		for (unsigned i = 0; i < EditVertRgn.size(); ++i)
			EditVertRgn[i]->N.Normalize();

		// 경계 상자와 정점 버퍼를 갱신한다.
		pEditMesh->UpdateBndBox(false);
		pEditMesh->UpdateBufVertex();
		pEditMesh->UpdateBufNormal();
	}

	return true;
}

/*!
*	\brief 메쉬를 Smoothing 한다.
*
*	\param x 마우스의 x 좌표
*	\param y 마우스의 y 좌표
*
*	\return 편집이 된 경우 true, 편집이 되지 않은 겨우 false를 반환한다.
*/
bool PEditor::EditSmooth(int x, int y)
{
	if (EditToolType != EDIT_SMOOTH)
		return false;

	// 마우스와 교차한 삼각형, 정점의 정보찾아 없다면 리턴한다.
	if (!theSelector.SetMouseOverInfo(x, y))
		return false;

	// Smoothing 을 위한 편집 영역 및 가중치를 계산하고,
	FindEditWgtForSculpt();

	GMesh *pEditMesh = GetEditMesh();
	if (!EditVertRgn.empty())
	{
		int NumVert = pEditMesh->GetNumVert();
		std::vector<int> Deg;
		Deg.assign(NumVert, 0);

		std::vector<GVector3> COG;
		COG.assign(NumVert, GVector3(0.0, 0.0, 0.0));

		// 법선을 갱신한다.
		for (unsigned i = 0; i < EditFaceRgn.size(); ++i)
		{
			GVector3 g = cast_vec3(EditFaceRgn[i]->GetCOG(true));
			int *vidx = EditFaceRgn[i]->vIdx;
			COG[vidx[0]] += g;
			COG[vidx[1]] += g;
			COG[vidx[2]] += g;
			Deg[vidx[0]]++;
			Deg[vidx[1]]++;
			Deg[vidx[2]]++;
		}

		for (int i = 0; i < NumVert; ++i)
		{
			if (Deg[i] >= 3)
			{
				double wgt = pEditMesh->Verts[i].EditWgt;
				COG[i] = COG[i] / Deg[i];
				pEditMesh->Verts[i].P = pEditMesh->Verts[i].P + wgt * (cast_pt3(COG[i]) - pEditMesh->Verts[i].P);
			}		
		}

		// 법선을 갱신한다.
		for (unsigned i = 0; i < EditFaceRgn.size(); ++i)
		{
			int *vidx = EditFaceRgn[i]->vIdx;
			GVector3 p, q, n;
			p = pEditMesh->Verts[vidx[1]].P - pEditMesh->Verts[vidx[0]].P;
			q = pEditMesh->Verts[vidx[2]].P - pEditMesh->Verts[vidx[0]].P;
			n = (p ^ q).Normalize();

			pEditMesh->Verts[vidx[0]].N += n;
			pEditMesh->Verts[vidx[1]].N += n;
			pEditMesh->Verts[vidx[2]].N += n;
		}
		for (unsigned i = 0; i < EditVertRgn.size(); ++i)
			EditVertRgn[i]->N.Normalize();

		// 경계 상자와 정점 버퍼를 갱신한다.
		pEditMesh->UpdateBndBox(false);
		pEditMesh->UpdateBufVertex();
		pEditMesh->UpdateBufNormal();
	}
	
	return true;
}

/*!
*	\brief 객체 편집을 종료하거나, 새로운 객체를 선택한다.
*
*	\param x 마우스의 X 좌표
*	\param y 마우스의 Y 좌표
*	\param nFlags 마우스 왼쪽 버튼이 떼어질 때의 추가 정보 (MK_CONTROL...)
*/
void PEditor::LButtonUp(int x, int y, UINT nFlags)
{
	// 편집 툴의 핸들이 선택 된 경우,
	switch (EditToolType)
	{
		// 객체 이동을 종료하고 리턴한다.
	case EDIT_TRA_X:
	case EDIT_TRA_Y:
	case EDIT_TRA_Z:
	case EDIT_TRA_XY:
	case EDIT_TRA_YZ:
	case EDIT_TRA_ZX:
	case EDIT_TRA_XYZ:
		EditFinish();
		EditToolType = EDIT_TRA;
		return;

		// 객체 회전을 종료하고 리턴한다.
	case EDIT_ROT_X:
	case EDIT_ROT_Y:
	case EDIT_ROT_Z:
		EditFinish();
		EditToolType = EDIT_ROT;
		return;

		// 객체 스케일을 종료하고 리턴한다.
	case EDIT_SCL_X:
	case EDIT_SCL_Y:
	case EDIT_SCL_Z:
	case EDIT_SCL_XY:			
	case EDIT_SCL_YZ:
	case EDIT_SCL_ZX:
	case EDIT_SCL_XYZ:
		EditFinish();
		EditToolType = EDIT_SCL;
		return;

		// 측지선 편집이라면,
	case EDIT_GEODESIC_PATH:
		return;

		// Sculpting, smoothing 편집이라면,
	case EDIT_SCULPT:
	case EDIT_SMOOTH:
		EditFinish();
		return;
	}
	
	// 편집 툴의 핸들이 선택되지 않은 경우, 새로운 객체를 선택한다.
	theSelector.LButtonUp(x, y, nFlags);
}

/*!
*	\brief 편집 영역의 크기를 조절한다.
*
*	\param delta 마우스 휠의 변위
*/
void PEditor::MouseWheel(int delta)
{
	// 편집 메쉬를 구하여 없다면 리턴한다.
	GMesh *pMesh = GetEditMesh();
	if (pMesh == NULL)
		return;

	// 편집 영역을 확장하는 경우라면,
	if (delta > 0)
	{
		EditMeshDlg.m_EditRgnDist += 10;
		if (EditMeshDlg.m_EditRgnDist > 1000)
			EditMeshDlg.m_EditRgnDist = 1000;
	}
	else	// 편집 영역을 축소하는 경우라면,
	{
		EditMeshDlg.m_EditRgnDist -= 10;
		if (EditMeshDlg.m_EditRgnDist < 0)
			EditMeshDlg.m_EditRgnDist = 0;		
	}

	// 변경된 변수의 값을 콘트롤에 반영한다.
	EditMeshDlg.m_cEditRgnDist.SetPos(EditMeshDlg.m_EditRgnDist);

	// 현재 편집 툴이 sculpt 또는 smooth 타입이라면,
	if (EditToolType == EDIT_SCULPT || EditToolType == EDIT_SMOOTH)
		FindEditWgtForSculpt();
	else
		FindEditWgt();

	// 장면의 렌더링을 갱신한다.
	EditMeshDlg.pDoc->UpdateAllViews(NULL);
}

/*!
*	\brief 선택된 객체들 중 첫 번째 객체의 참조 메쉬를 반환한다.
*
*	\return 선택된 객체들 중 첫 번째 객체의 참조 메쉬를 반환한다.
*/
GMesh *PEditor::GetEditMesh()
{
	if (EditObjList.empty())
		return NULL;
	else
		return EditObjList[0]->GetRefMesh();
}

/*!
*	\brief 편집 객체의 클래스 이름을 반환한다.
*
*	\return 편집 객체의 클래스 이름
*/
std::string PEditor::GetClsNameEditObj()
{
	std::string name;
	if (!EditObjList.empty())
		name = EditObjList[0]->GetClassName();

	return name;
}

/*!
*	\brief 편집 객체의 개수를 반환한다.
*
*	\return 편집 객체의 개수
*/
int PEditor::GetNumEditObj()
{
	int NumEditObj = (int)EditObjList.size();

	return NumEditObj;
}

/*!
*	\brief 선택된 객체들이 참조하는 메쉬 객체만 선택하여 리스트를 생성한다.
*
*	\param MeshList 메쉬 객체가 저장될 리스트
*/
void PEditor::GetRefMeshList(std::vector<GMesh *> &MeshList)
{
	GMesh *pPrevMesh = NULL;
	int NumObj = (int)EditObjList.size();
	for (int i = 0; i < NumObj; ++i)
	{
		GMesh *pMesh = EditObjList[i]->GetRefMesh();
		if (pMesh != pPrevMesh)
		{
			MeshList.push_back(pMesh);
			pPrevMesh = pMesh;
		}
	}
}

/*!
*	\brief 편집 객체를 추가한다.
*
*	\param pObj 추가 또는 삭제할 객체
*/
void PEditor::AddToEditObjList(GEditObj *pObj)
{
	if (!EditObjList.empty())
	{
		std::string PrevClsName = EditObjList[0]->GetClassName();
		std::string CurrClsName = pObj->GetClassName();
		if (PrevClsName != CurrClsName)
			EditObjList.clear();
	}

	// 무조건 추가하고, 나중에 필요한 경우, UniqueEditObjList() 함수를 호출하여 일괄적으로 중복 제거한다.
	EditObjList.push_back(pObj);
}

/*!
*	\brief 편집 객체를 삭제한다.
*
*	\param pObj 삭제할 객체
*
*	\return 삭제된 경우 true, 삭제할 객체가 없는 경우 false를 반환한다.
*/
bool PEditor::DelFromEditObjList(GEditObj *pObj)
{
	bool bErased = false;
	std::vector<GEditObj *>::iterator it;
	for (it = EditObjList.begin(); it != EditObjList.end(); ++it)
	{
		if (*it == pObj)
		{
			EditObjList.erase(it);
			bErased = true;
			break;
		}
	}

	return bErased;
}

/*!
*	\brief 객체의 선택을 토글한다.
*
*	\param pObj 토글할 객체
*/
void PEditor::ToggleEditObject(GEditObj *pObj)
{
	bool bErased = false;

	// 모든 편집 객체에 대하여,
	std::vector<GEditObj *>::iterator it;
	for (it = EditObjList.begin(); it != EditObjList.end(); ++it)
	{
		// pObj가 이미 선택된 상태라면,
		if (*it == pObj)
		{
			// pObj의 선택을 해제하고,
			EditObjList.erase(it);
			bErased = true;
			break;
		}
	}

	// 새로운 객체라면 편집 리스트에 추가한다.
	if (!bErased)
		EditObjList.push_back(pObj);
}

/*!
*	\brief 선택된 편집 객체리스트의 중복을 제거한다.
*	\warning 선택된 객체의 리스트가 중복은 제거되지만 정렬된다는 단점이 있다.
*/
void PEditor::UniqueEditObjList()
{
	sort(EditObjList.begin(), EditObjList.end());
	EditObjList.erase(unique(EditObjList.begin(), EditObjList.end()), EditObjList.end());
}

/*!
*	\brief 편집 툴의 핸들을 선택한다.
*
*	\param x 마우스의 X 좌표
*	\param y 마우스의 Y 좌표
*/
bool PEditor::SelectEditHandle(int x, int y)
{
	// 이동/스케일 편집 툴의 핸들(X, Y, Z)이 선택 되었다면 true를 리턴한다.
	if ((EditToolType >= EDIT_TRA_X && EditToolType <= EDIT_TRA_XYZ) || (EditToolType >= EDIT_SCL_X && EditToolType <= EDIT_SCL_XYZ))
		return true;

	// 반환 값을 초기화 하고,
	bool ret = false;

	// 회전 편집 툴의 핸들(X, Y, Z)이 선택되었다면,
	if (EditToolType >= EDIT_ROT_X && EditToolType <= EDIT_ROT_Z)
	{
		// 편집 좌표계로의 변환을 구하고,
		GTransf X = pScene->GetWC() * EC;

		// 편집 좌표계에서 표현된 뷰 방향 벡터를 구하고,
		GVector3 view_axis(0.0, 0.0, 1.0);
		view_axis = inv(X) * view_axis;

		// 선택 모드로 진입하여,
		theSelector.EnterSelectMode(x, y);

		// 편집 좌표계로 변환하고,
		glMultMatrixd(cast_mat(EC).M);

		// 현재 선택된 회전 툴의 핸들에 따라서,
		int snum = 100;
		double len = GetEditToolSize();
		switch (EditToolType)
		{
			// X 축 회전 핸들이 선택된 경우,
		case EDIT_ROT_X:
			for (int i = 0; i < snum; ++i)
			{
				double theta1 = (double)i / (double)snum * M_PI * 2.0;
				GVector3 p1(0.0, len * cos(theta1), len * sin(theta1));
				if (view_axis * p1 > 0)
				{
					double theta2 = (double)(i + 1) / (double)snum * M_PI * 2.0;
					GVector3 p2(0.0, len * cos(theta2), len * sin(theta2));
					glLoadName(i);
					glBegin(GL_LINES);
					glVertex3d(p1[0], p1[1], p1[2]);
					glVertex3d(p2[0], p2[1], p2[2]);
					glEnd();
				}
			}	
			break;

			// Y 축 회전 핸들이 선택된 경우,
		case EDIT_ROT_Y:
			for (int i = 0; i < snum; ++i)
			{
				double theta1 = (double)i / (double)snum * M_PI * 2.0;
				GVector3 p1(len * sin(theta1), 0.0, len * cos(theta1));
				if (view_axis * p1 > 0)
				{
					double theta2 = (double)(i + 1) / (double)snum * M_PI * 2.0;
					GVector3 p2(len * sin(theta2), 0.0, len * cos(theta2));
					glLoadName(i);
					glBegin(GL_LINES);
					glVertex3d(p1[0], p1[1], p1[2]);
					glVertex3d(p2[0], p2[1], p2[2]);
					glEnd();
				}
			}
			break;

			// Z 축 회전 핸들이 선택된 경우,
		case EDIT_ROT_Z:
			for (int i = 0; i < snum; ++i)
			{
				double theta1 = (double)i / (double)snum * M_PI * 2.0;
				GVector3 p1(len * cos(theta1), len * sin(theta1), 0.0);
				if (view_axis * p1 > 0)
				{
					double theta2 = (double)(i + 1) / (double)snum * M_PI * 2.0;
					GVector3 p2(len * cos(theta2), len * sin(theta2), 0.0);
					glLoadName(i);
					glBegin(GL_LINES);
					glVertex3d(p1[0], p1[1], p1[2]);
					glVertex3d(p2[0], p2[1], p2[2]);
					glEnd();
				}
			}
			break;
		}

		// 렌더링 모드로 복귀하여, 
		GLint hits = theSelector.ExitSelectMode();

		// 선택된 객체가 있다면,
		if (hits > 0)
		{
			// 현재 편집 툴의 좌표계를 저장하고,
			ret = true;
			RotRefX = X;
			double theta = (double)(theSelector.SelectBuf[3]) / (double)snum * M_PI * 2.0;

			// 현재 선택된 회전 툴의 핸들에 따라서 선택 지점을 저장한다.
			switch (EditToolType)
			{
			case EDIT_ROT_X:
				RotRefPos.Set(0.0, len * cos(theta), len * sin(theta));			
				break;

			case EDIT_ROT_Y:
				RotRefPos.Set(len * sin(theta), 0.0, len * cos(theta));			
				break;

			case EDIT_ROT_Z:
				RotRefPos.Set(len * cos(theta), len * sin(theta), 0.0);			
				break;
			}
		}
	}

	// 결과 값을 반환한다.
	return ret;
}

/*!
*	\brief 선택된 객체들로부터 에디터의 편집 좌표계를 설정한다.
*/
void PEditor::SetEC()
{
	// 선택된 객체가 없다면, 
	if (EditObjList.empty())
	{
		EC = GTransf();
		return;
	}

	// 편집 객체와 이름를 구하여,
	GMesh *pMesh = GetEditMesh();
	std::string ClsName = GetClsNameEditObj();
	GVector3 C;

	// 만약 정점이 선택되었다면,
	if (ClsName == "GVertex")
	{
		for (unsigned i = 0; i < EditObjList.size(); ++i)
		{
			GVertex *v = (GVertex *)EditObjList[i];
			C = C + cast_vec3(pMesh->MC * v->P);
		}
		C = C / (int)EditObjList.size();
		EC = GTransf(C, pMesh->MC.q);
	}
	else if (ClsName == "GFace")	// 삼각형이 선택되었다면,
	{
		for (unsigned i = 0; i < EditObjList.size(); ++i)
		{
			GFace *f = (GFace *)EditObjList[i];
			C = C + cast_vec3(pMesh->MC * f->GetCOG(true));
		}
		C = C / (int)EditObjList.size();
		EC = GTransf(C, pMesh->MC.q);
	}
	else if (ClsName == "GCtrlPt")	// 제어점이 선택되었다면,
	{
		GVector3 C;
		int NumObj = (int)EditObjList.size();
		for (int i = 0; i < NumObj; ++i)
		{
			GCtrlPt *p = (GCtrlPt *)EditObjList[i];
			C = C + cast_vec3(pMesh->MC * GPoint3(p->V[0], p->V[1], p->V[2]));
		}
		C = C / (int)EditObjList.size();
		EC = GTransf(C, pMesh->MC.q);
	}
	else	// 정점, 삼각형, 제어점 이외의 객체가 선택되었다면,
	{
		// 첫 번째 편집 객체의 변환으로 설정한다.
		EC = EditObjList[0]->GetTransf();
	}	
}

/*!
*	\brief 편집 툴의 크기를 계산한다.
*
*	\return 편집 툴의 크기를 반환한다.
*/
double PEditor::GetEditToolSize()
{
	// 카메라 좌표계에서 편집 좌표계로의 변환을 구하고,
	GTransf X = pScene->GetWC() * EC;

	// 편집 좌표계의 원점까지의 길이의 5%를 구하여 리턴한다.
	double len = norm(X.t) * 0.05;

	//std::cout << "Edit Tool Size: " << len << std::endl;
	return len;
}

/*!
*	\brief 에티터의 편집 좌표계를 반환한다.
*/
GTransf PEditor::GetEC()
{
	return EC;
}

/*!
*	\brief 편집 객체의 편집 가중치를 계산한다.
*	\note 호출순서 FindEditWgt() -> FindEditRgn() -> FindEditBndry()
*/
void PEditor::FindEditWgt()
{
	// 참조 메쉬가 없다면 리턴하고,
	GMesh *pMesh = GetEditMesh();
	if (pMesh == NULL)
		return;

	// 편집 객체가 정점도 아니고 삼각형도 아니라면 리턴한다.
	std::string ClsName = GetClsNameEditObj();
	if (ClsName != "GVertex" && ClsName != "GFace")
		return;

	double t = EditMeshDlg.m_EditRgnDist / 1000.0;
	// 편집 영역이 지정되었다면,
	if (t > 0.0)
	{
		// 편집 메쉬 정점의 편집 가중치를 초기화 한다.
		int NumVert = pMesh->GetNumVert();
		for (int i = 0; i < NumVert; ++i)
			pMesh->Verts[i].EditWgt = 0.0;

		// 편집 거리를 구하고,
		double EditDist = pMesh->BBox.GetDiagonal() * t;

		// 근사 편집 경계 곡선이라면,
		if (EditMeshDlg.m_ShowEditBndryType == 0)
		{
			// 측지 거리장, 편집 영역, 근사 경계선을 구한다.
			std::vector<double> Distances;
			get_geodesic_dist_field(pMesh, EditObjList, Distances, EditDist);
			FindEditRgn(pMesh, Distances, EditDist);
			FindEditBndry(pMesh, Distances, EditDist);			
		}
		else	// 정확한 편집 경계 곡선이라면,
		{
			// 측지 좌표, 편집 영역, 경계선을 구한다.
			std::vector<double> Distances;
			std::vector<double> Angles;
			//double scale = (t < 0.1) ? 2.0 : 1.2;
			double scale = 1.2;
			get_geodesic_polar_coords(pMesh, (GVertex *)EditObjList[0], Distances, Angles, EditDist * scale);
			FindEditRgn(pMesh, Distances, EditDist);
			FindEditBndry(pMesh, Distances, Angles, EditDist);			
		}
	}
	else	// 편집 영역이 지정되지 않았다면, 
	{
		// 편집 영역과 확장된 편집 객체리스트를 초기화한다.
		EditFaceRgn.clear();
		EditVertRgn.clear();
		EditBndryPts.clear();
		
		// 편집 메쉬의 정점의 개수를 구하고,
		int NumVert = pMesh->GetNumVert();
		for (int i = 0; i < NumVert; ++i)
			pMesh->Verts[i].EditWgt = 1.0;		
	}
}

/*!
*	\brief 편집 객체의 편집 가중치를 계산한다.
*/
void PEditor::FindEditWgtForSculpt()
{
	// 참조 메쉬가 없다면 리턴하고,
	GMesh *pMesh = GetEditMesh();
	if (pMesh == NULL)
		return;

	double t = EditMeshDlg.m_EditRgnDist / 1000.0;
	// 편집 영역이 지정되었다면,
	if (t > 0.0)
	{
		// 편집 메쉬 정점의 편집 가중치를 초기화 한다.
		int NumVert = pMesh->GetNumVert();
		for (int i = 0; i < NumVert; ++i)
			pMesh->Verts[i].EditWgt = 0.0;
	
		// 측지 거리장, 편집 영역, 근사 경계선을 구한다.
		double EditDist = pMesh->BBox.GetDiagonal() * t;
		std::vector<double> Distances;
		get_geodesic_dist_field(pMesh, theSelector.MouseOverFaceIdx, theSelector.MouseOverPtCoords, Distances, EditDist);
		FindEditRgn(pMesh, Distances, EditDist);
		FindEditBndry(pMesh, Distances, EditDist);			
	}
	else	// 편집 영역이 지정되지 않았다면, 
	{
		// 편집 영역과 확장된 편집 객체리스트를 초기화한다.
		EditFaceRgn.clear();
		EditVertRgn.clear();
		EditBndryPts.clear();
	}
}

/*!
*	\brief 측지선 거리장을 이용하여 편집 영역을 구한다.
*
*	\param pMesh 대상 메쉬
*	\param Distances 대상 메쉬의 측지 거리장
*	\param EditDist 편집 측지 거리
*/
void PEditor::FindEditRgn(GMesh *pMesh, std::vector<double> &Distances, double EditDist)
{
	// 편집 영역과 확장된 편집 객체리스트를 초기화한다.
	EditFaceRgn.clear();
	EditVertRgn.clear();

	// 거리에 따라 각 정점의 편집 가중치를 구한다.
	int NumVert = pMesh->GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		double t = Distances[i] / EditDist;

		// 선택 정점들이라면,
		if (t == 0.0)
		{
			pMesh->Verts[i].EditWgt = 1.0;
		}
		else if (t > 0.0 && t < 1.0)	// 편집 영역내의 정점이라면,
		{
			pMesh->Verts[i].EditWgt = 2 * t * t * t - 3 * t * t + 1;
			EditVertRgn.push_back(&(pMesh->Verts[i]));
		}
	}

	// 편집 메쉬가 사각형 이라면,
	if (pMesh->FaceType == FACE_QUAD)
	{
		// 각각의 사각형에 대하여,
		for (unsigned i = 0; i < pMesh->Faces.size(); i += 2)
		{
			int *vidx = pMesh->Faces[i].vIdx;
			int vidx3 = pMesh->Faces[i + 1].vIdx[2];
			double w0 = pMesh->Verts[vidx[0]].EditWgt;
			double w1 = pMesh->Verts[vidx[1]].EditWgt;
			double w2 = pMesh->Verts[vidx[2]].EditWgt;
			double w3 = pMesh->Verts[vidx3].EditWgt;

			// 편집될 삼각형이라면 편집 삼각형 리스트에 추가한다. (렌더링 용)
			if (w0 > 0.0 || w1 > 0.0 || w2 > 0.0 || w3 > 0.0)
			{
				EditFaceRgn.push_back(&(pMesh->Faces[i]));
				EditFaceRgn.push_back(&(pMesh->Faces[i + 1]));
			}
		}
	}
	else if (pMesh->FaceType == FACE_TRI)	// 편집 메쉬가 삼각형 이라면,
	{
		// 각각의 삼각형에 대하여,
		for (unsigned i = 0; i < pMesh->Faces.size(); ++i)
		{
			// 편집될 삼각형이라면 편집 삼각형 리스트에 추가한다. (렌더링 용)
			int *vidx = pMesh->Faces[i].vIdx;
			double w0 = pMesh->Verts[vidx[0]].EditWgt;
			double w1 = pMesh->Verts[vidx[1]].EditWgt;
			double w2 = pMesh->Verts[vidx[2]].EditWgt;
			if (w0 > 0.0 || w1 > 0.0 || w2 > 0.0)
				EditFaceRgn.push_back(&(pMesh->Faces[i]));				
		}
	}
}

/*!
*	\brief 측지선 거리장을 이용하여 정확한 편집 경계를 구한다.
*
*	\param pMesh 대상 메쉬
*	\param Distances 대상 메쉬의 측지 거리장
*	\param Angles 대상 메쉬의 측지 거리장 (각도)
*	\param EditDist 편집 측지 거리
*/
void PEditor::FindEditBndry(GMesh *pMesh, std::vector<double> &Distances, std::vector<double> &Angles, double EditDist)
{
	// 편집 영역의 경계선을 초기화 한다.
	EditBndryPts.clear();

	// 편집 영역을 2차원 공간에 펼친 메쉬를 생성한다.
	if (pUnfoldMesh != NULL)
	{
		delete pUnfoldMesh;
		UnfoldFaceMap.clear();
	}
	pUnfoldMesh = new GMesh("", GTransf(), FACE_TRI, "");
	pUnfoldMesh->Renderer.RenderType = RENDER_WIRE;

	// 2차원 메쉬의 정점과 삼각형을 구성한다.
	for (unsigned i = 0; i < pMesh->Faces.size(); ++i)
	{
		// 각 삼각형 세 점의 좌표와 거리를 구하여,
		int *vidx = pMesh->Faces[i].vIdx;
		double d0 = Distances[vidx[0]];
		double d1 = Distances[vidx[1]];
		double d2 = Distances[vidx[2]];

		// 삼각형이 편집 영역내에 포함된다며,
		if (d0 != 1000000.0 && d1 != 1000000.0 && d2 != 1000000.0)	// 1000000.0 : default 값
		{
			// 펼쳐진 삼각형을 추가한다.
			GPoint3 p0(d0 * cos(Angles[vidx[0]]), d0 * sin(Angles[vidx[0]]), 0.0);
			GPoint3 p1(d1 * cos(Angles[vidx[1]]), d1 * sin(Angles[vidx[1]]), 0.0);
			GPoint3 p2(d2 * cos(Angles[vidx[2]]), d2 * sin(Angles[vidx[2]]), 0.0);
			pUnfoldMesh->AddVertex(GVertex(p0));
			pUnfoldMesh->AddVertex(GVertex(p1));
			pUnfoldMesh->AddVertex(GVertex(p2));
			int NumVert = pUnfoldMesh->GetNumVert();
			pUnfoldMesh->AddFace(GFace(NumVert - 3, NumVert - 2, NumVert - 1));

			// 삼각형의 대응 정보(2D 삼각형 인덱스, 3D 삼각형 인덱스)를 저장한다.
			UnfoldFaceMap[pUnfoldMesh->GetNumFace() - 1] = i;		
		}		
	}
	pUnfoldMesh->CreateMesh();
	GMaterial mtl = ::get_material(0.0, 0.0, 0.0);
	pUnfoldMesh->AddMtl(mtl);
	pUnfoldMesh->SetMtl(mtl.Name);

	// 3차원 메쉬의 편집 경계를 구한다.
	GQTree QuadTree(pUnfoldMesh, 7, 5);
	int NumSamp = MAX(20, pUnfoldMesh->GetNumFace() * 2);	// 최소 정점의 수: 20
	for (int i = 0; i < NumSamp; ++i)
	{
		double theta = (double) i / (NumSamp - 1) * 2.0 * M_PI;
		GVector2 p(EditDist * cos(theta), EditDist * sin(theta));
		
		// 계산된 정점 p가 포함된 삼각형의 인덱스와 barycentric 좌표를 계산하고,
		double alpha, beta;
		int fidx = QuadTree.FindFace(p, &alpha, &beta);
		if (fidx != -1)
		{
			// 3차원 객체의 좌표로 복원하여 측지선 메쉬에 추가한다.
			int *vidx = pMesh->Faces[UnfoldFaceMap[fidx]].vIdx;
			GPoint3 p0 = pMesh->Verts[vidx[0]].P;
			GPoint3 p1 = pMesh->Verts[vidx[1]].P;
			GPoint3 p2 = pMesh->Verts[vidx[2]].P;
			GPoint3 p3 = p0 + alpha * (p1 - p0) + beta * (p2 - p0);
			EditBndryPts.push_back(p3);
		}		
	}
}

/*!
*	\brief 측지선 거리장을 이용하여 대략적인 편집 경계를 구한다.
*
*	\param pMesh 대상 메쉬
*	\param Distances 대상 메쉬의 측지 거리장
*	\param EditDist 편집 측지 거리
*/
void PEditor::FindEditBndry(GMesh *pMesh, std::vector<double> &Distances, double EditDist)
{
	// 편집 영역의 경계선을 초기화 한다.
	EditBndryPts.clear();

	// 삼각형, 사각형 메쉬에 대하여 편집 경계를 구한다.
	for (unsigned i = 0; i < EditFaceRgn.size(); ++i)
	{
		// 삼각형 세 점의 좌표와 거리를 구하여,
		int *vidx = EditFaceRgn[i]->vIdx;
		double d0 = Distances[vidx[0]];
		double d1 = Distances[vidx[1]];
		double d2 = Distances[vidx[2]];

		GPoint3 v0 = pMesh->Verts[vidx[0]].P;
		GPoint3 v1 = pMesh->Verts[vidx[1]].P;
		GPoint3 v2 = pMesh->Verts[vidx[2]].P;
		GPoint3 p, q;

		// 선분 (v0, v1)과 (v2, v0)가 편집 경계와 만나는 경우,
		if (d0 < EditDist && d1 > EditDist && d2 > EditDist)
		{
			double s = (EditDist - d0) / (d1 - d0);
			double t = (EditDist - d0) / (d2 - d0);
			p = v0 + s * (v1 - v0);
			q = v0 + t * (v2 - v0);
		}
		// 선분 (v1, v2)과 (v0, v1)이 편집 경계와 만나는 경우,
		if (d1 < EditDist && d2 > EditDist && d0 > EditDist)
		{
			double s = (EditDist - d1) / (d2 - d1);
			double t = (EditDist - d1) / (d0 - d1);
			p = v1 + s * (v2 - v1);
			q = v1 + t * (v0 - v1);
		}
		// 선분 (v2, v0)와 (v1, v2)가 편집 경계와 만나는 경우,
		if (d2 < EditDist && d0 > EditDist && d1 > EditDist)
		{
			double s = (EditDist - d2) / (d0 - d2);
			double t = (EditDist - d2) / (d1 - d2);
			p = v2 + s * (v0 - v2);
			q = v2 + t * (v1 - v2);
		}
		// 선분 (v0, v1)과 (v1, v2)가 편집 경계와 만나는 경우,
		if (d2 < EditDist && d0 < EditDist && d1 > EditDist)
		{
			double s = (EditDist - d0) / (d1 - d0);
			double t = (EditDist - d2) / (d1 - d2);
			p = v0 + s * (v1 - v0);
			q = v2 + t * (v1 - v2);
		}
		// 선분 (v1, v2)과 (v2, v0)가 편집 경계와 만나는 경우,
		if (d0 < EditDist && d1 < EditDist && d2 > EditDist)
		{
			double s = (EditDist - d1) / (d2 - d1);
			double t = (EditDist - d0) / (d2 - d0);
			p = v1 + s * (v2 - v1);
			q = v0 + t * (v2 - v0);			
		}
		// 선분 (v2, v0)와 (v0, v1)이 편집 경계와 만나는 경우,
		if (d1 < EditDist && d2 < EditDist && d0 > EditDist)
		{
			double s = (EditDist - d2) / (d0 - d2);
			double t = (EditDist - d1) / (d0 - d1);
			p = v2 + s * (v0 - v2);
			q = v1 + t * (v0 - v1);
		}

		EditBndryPts.push_back(p);
		EditBndryPts.push_back(q);
	}	
}

/*!
*	\brief 넙스 곡선을 메쉬에 투영한다.
*
*	\param pCrv 투영될 넙스 곡선
*/
void PEditor::ProjectCrvOnMesh(GNurbsCrv *pCrv)
{
	// 곡선이 정의된 참조 메쉬가 없다면 리턴한다.
	if (pCrv->RefMeshName == "")
		return;

	// 참조 메쉬가 없다면 리턴한다.
	GMesh *pMesh = FindMesh(pScene->MeshList, pCrv->RefMeshName);
	if (pMesh == NULL)
	{
		pCrv->RefMeshName = "";
		return;
	}

	// 메쉬의 교차점을 저장하는 변수를 정의하고,
	std::vector<GVector4> IsPtList;

	GVector3 view_dir = pScene->MainCamera.GetViewDirection();
	double uMin, uMax;
	pCrv->GetDomain(&uMin, &uMax);
	int NumPt = pCrv->n + 1;
	for (int i = 0; i < NumPt; ++i)
	{
		double u = uMin + (uMax - uMin) * i / (double)(NumPt - 1);
		GPoint3 pt = cast_pt3(pCrv->Eval(u));
		GLine ray(pt, view_dir);

		double min_dist = 10000000.0;
		GPoint3 ispt;
		int NumFace = pMesh->GetNumFace();
		for (int j = 0; j < NumFace; ++j)
		{
			int vidx0 = pMesh->Faces[j].vIdx[0];
			int vidx1 = pMesh->Faces[j].vIdx[1];
			int vidx2 = pMesh->Faces[j].vIdx[2];
			GPoint3 p0 = pMesh->MC * pMesh->Verts[vidx0].P;
			GPoint3 p1 = pMesh->MC * pMesh->Verts[vidx1].P;
			GPoint3 p2 = pMesh->MC * pMesh->Verts[vidx2].P;

			// 현재 마우스 포인터와 교차된 삼각형위의 점의 Barycentric 좌표를 계산한다.
			GPoint3 curr_ispt;
			if (intersect_line_triangle(p0, p1, p2, ray, curr_ispt, false))
			{
				double curr_dist = dist(pt, curr_ispt);
				if (curr_dist < min_dist)
				{
					min_dist = curr_dist;
					ispt = curr_ispt;
				}							
			}
		}
		IsPtList.push_back(cast_vec4(ispt));
	}

	GNurbsCrv *pProjCrv = ::get_gnurbs_crv_inter(3, pCrv->n, &IsPtList[0], uMin, uMax, PARAM_CHORD_LEN, KNOT_AVERAGING);

	// 생성된 곡선의 시작 점과 끝점의 제어점을 구한다.
	// 	GPoint3 P0 = IsPtList[0];
	// 	GPoint3 Pn = IsPtList[IsPtList.size() - 1];
	// 
	// 	// 장면에 포함된 모든 메쉬를 구하고,
	// 	std::vector<GMesh *> MeshList;
	// 	GetPreOrderMeshList(pScene, MeshList, NULL);
	// 
	// 	// 생성된 곡선의 시작 점과 끝 점에 가장 가까운 곡선을 저장할 변수를 정의한다.
	// 	GNurbsCrv *pStProxiCrv, *pEdProxiCrv;
	// 	double StParam, EdParam;
	// 	double StMinDist = pCrv->Renderer.CtrlPtSize * 3.0;
	// 	double EdMinDist = pCrv->Renderer.CtrlPtSize * 3.0;
	// 	bool StFlag = false;
	// 	bool EdFlag = false;
	// 
	// 	// 장면의 모든 곡선 객체에 대하여,
	// 	int NumMesh = (int)MeshList.size();
	// 	for (int i = 0; i < NumMesh; ++i)
	// 	{
	// 		GMesh *pMesh = MeshList[i];
	// 		double d, tmp;
	// 		if (pMesh->GetClassName() == "GNurbsCrv")
	// 		{
	// 			GNurbsCrv *pOtherCrv = (GNurbsCrv *)pMesh;
	// 			d = ::get_min_dist(P0, pOtherCrv, &tmp);
	// 			if (d < StMinDist)
	// 			{
	// 				pStProxiCrv = pOtherCrv;
	// 				StMinDist = d;
	// 				StParam = tmp;
	// 				StFlag = true;
	// 			}
	// 
	// 			d = ::get_min_dist(Pn, pOtherCrv, &tmp);
	// 			if (d < EdMinDist)
	// 			{
	// 				pEdProxiCrv = pOtherCrv;
	// 				EdMinDist = d;
	// 				EdParam = tmp;
	// 				EdFlag = true;
	// 			}
	// 		}
	// 	}
	// 
	// 	if (StFlag)
	// 	{
	// 		double umin, umax;
	// 		pStProxiCrv->GetDomain(&umin, &umax);
	// 		StParam = ((StParam - umin) < (umax - umin) * 0.2) ? umin : StParam;
	// 		StParam = ((umax - StParam) < (umax - umin) * 0.2) ? umax : StParam;
	// 		GPoint3 StPt = cast_pt3(pStProxiCrv->Eval(StParam));
	// 		pProjCrv->P[0].Set(StPt[0], StPt[1], StPt[2], 1.0);
	// 	}
	// 
	// 	if (EdFlag)
	// 	{
	// 		double umin, umax;
	// 		pEdProxiCrv->GetDomain(&umin, &umax);
	// 		EdParam = ((EdParam - umin) < (umax - umin) * 0.2) ? umin : EdParam;
	// 		EdParam = ((umax - EdParam) < (umax - umin) * 0.2) ? umax : EdParam;
	// 		GPoint3 EdPt = cast_pt3(pEdProxiCrv->Eval(EdParam));
	// 		pProjCrv->P[pProjCrv->n].Set(EdPt[0], EdPt[1], EdPt[2], 1.0);
	// 	}

	ARR_COPY(pCrv->U, pProjCrv->U, pCrv->n + pCrv->p + 2);
	for (int i = 0; i < pCrv->n + 1; ++i)
	{
		pCrv->P[i] = pProjCrv->P[i];
	}
	delete pProjCrv;
}
/*
void PEditor::ProjectCrvOnMesh(GNurbsCrv *pCrv)
{
	// 곡선이 정의된 참조 메쉬가 없다면 리턴한다.
	if (pCrv->RefMeshName == "")
		return;

	// 참조 메쉬가 없다면 리턴한다.
	GMesh *pMesh = FindMesh(pScene->MeshList, pCrv->RefMeshName);
	if (pMesh == NULL)
	{
		pCrv->RefMeshName = "";
		return;
	}
	
	//////////////////////////////////////////////////////
	// 1 단계: 참조 메쉬를 후면 칼라 버퍼에 렌더링 한다. //
	//////////////////////////////////////////////////////

	// 색상 버퍼와 깊이 버퍼를 초기화한 후,
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 장면의 관측 공간 및 투영 방법을 설정하고,
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	pScene->SetupViewFrustum();

	// 장면의 관측 변환을 수행한다.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	pScene->SetupViewTransform();

	// 조명을 비활성화 하고, 색상 삼각형 렌더링 시작.
	glDisable(GL_LIGHTING);

	// 선택된 메쉬의 삼각형을 서로다른 색상의 아이디를 렌더링 한다.
	unsigned int r = 0, g = 0, b = 0;

	// 월드 좌표계에서
	glPushMatrix();
	{
		// 메쉬의 모델링 좌표계로 변환하고,
		glMultMatrixd(cast_mat(pMesh->MC).M);

		// 삼각형의 개수를 구하고,
		int NumFace = pMesh->GetNumFace();

		// 삼각형 색상 디스플레이 리스트가 있다면, 
		if (pMesh->DispFaceColMesh != 0)
		{
			// 디스플레이 리스트를 호출하고,
			glCallList(pMesh->DispFaceColMesh);
		}
		else
		{
			if (pMesh->GrpFaces.empty())	// 삼각형 그룹이 정의되지 않았다면,
			{
				// 삼각형 색상으로 렌더링된 디스플레이 리스트를 생성한다.
				pMesh->DispFaceColMesh = glGenLists(1);
				glNewList(pMesh->DispFaceColMesh, GL_COMPILE_AND_EXECUTE);
				{
					// 삼각형을 서로 다른 색상(아이디)으로 렌더링한 후,
					glBegin(GL_TRIANGLES);
					for (int i = 0; i < NumFace; ++i)
					{
						int *vidx = pMesh->Faces[i].vIdx;
						GetColorId(r, g, b);	// (0, 0, 1) 부터 시작됨...
						glColor3ub(r, g, b);
						glVertex3dv(pMesh->Verts[vidx[0]].P.V);
						glVertex3dv(pMesh->Verts[vidx[1]].P.V);
						glVertex3dv(pMesh->Verts[vidx[2]].P.V);
					}
					glEnd();
				}
				glEndList();				
			}
			else	// 삼각형 그룹이 정의되었다면,
			{
				pMesh->DispFaceColMesh = glGenLists(1);
				glNewList(pMesh->DispFaceColMesh, GL_COMPILE_AND_EXECUTE);
				{
					// 삼각형을 그룹별로 렌더링 한다.
					std::map<std::string, std::vector<int>>::iterator it;
					for (it = pMesh->GrpFaces.begin(); it != pMesh->GrpFaces.end(); ++it)
					{
						int NumGrpFace = it->second.size();
						glBegin(GL_TRIANGLES);
						for (int i = 0; i < NumGrpFace; ++i)
						{
							int fidx = it->second[i];
							int *vidx = pMesh->Faces[fidx].vIdx;
							GetColorId(r, g, b);	// (0, 0, 1) 부터 시작됨...
							glColor3ub(r, g, b);
							glVertex3dv(pMesh->Verts[vidx[0]].P.V);
							glVertex3dv(pMesh->Verts[vidx[1]].P.V);
							glVertex3dv(pMesh->Verts[vidx[2]].P.V);
						}	
						glEnd();
					}
				}
				glEndList();				
			}
		}		
	}
	// 월드 좌표계로 복원한다.
	glPopMatrix();

	// 조명을 활성화 한다.
	glEnable(GL_LIGHTING);	


	/////////////////////////////////////////////////
	// 2 단계: 곡선의 투영된 스크린 좌표를 구한다. //
	/////////////////////////////////////////////////
	double uMin, uMax;
	pCrv->GetDomain(&uMin, &uMax);
	int NumPt = pCrv->n + 1;

	// 메쉬의 교차점을 저장하는 변수를 정의하고,
	std::vector<GVector4> IsPtList;

	for (int i = 0; i < NumPt; ++i)
	{
		double u = uMin + (uMax - uMin) * i / (double)(NumPt - 1);
		GVector4 pt = pCrv->Eval(u);
		GVector2 p;
		double tmp;
		pScene->MainCamera.GetScreenCoord(pt[0], pt[1], pt[2], &p[0], &p[1], &tmp);

		int x = p[0];
		int y = p[1];
		// 현재 마우스 포인터 위치에서 색상 버퍼의 픽셀 값을 읽어서,
		GLubyte pixel[3];
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

		// 삼각형의 렌더링 인덱스(1, 2, 3, ...)를 구하고,
		int MouseOverFaceIdx = pixel[0] * 256 * 256 + pixel[1] * 256 + pixel[2] - 1;
		
		// 마우스와 중첩된 삼각형의 세 정점을 구하여,
		int vidx0 = pMesh->Faces[MouseOverFaceIdx].vIdx[0];
		int vidx1 = pMesh->Faces[MouseOverFaceIdx].vIdx[1];
		int vidx2 = pMesh->Faces[MouseOverFaceIdx].vIdx[2];
		GVertex *v0 = &(pMesh->Verts[vidx0]);
		GVertex *v1 = &(pMesh->Verts[vidx1]);
		GVertex *v2 = &(pMesh->Verts[vidx2]);

		// 월드 좌표계에서 표현된 삼각형의 정점을 좌표를 구하고,
		GPoint3 p0 = pMesh->MC * pMesh->Verts[vidx0].P;
		GPoint3 p1 = pMesh->MC * pMesh->Verts[vidx1].P;
		GPoint3 p2 = pMesh->MC * pMesh->Verts[vidx2].P;

		// 현재 마우스 위치에서 뷰에 수직한 직선의 방정식을 구하여,
		GLine ray = pScene->MainCamera.GetOrthoRay(x, y);

		// 현재 마우스 포인터와 교차된 삼각형위의 점의 Barycentric 좌표를 계산한다.
		double t, alpha, beta, gamma;
		GPoint3 ispt;
		if (intersect_line_triangle(p0, p1, p2, ray, t, ispt, alpha, beta, gamma, true))
		{
			IsPtList.push_back(cast_vec4(ispt, 1.0));
		}
	}

	////////////////////////////////////////////////////
	// 2 단계: 교차점을 근사하는 3차 곡선을 생성한다. //
	////////////////////////////////////////////////////
	int n = pCrv->n;
	GNurbsCrv *pProjCrv = ::get_gnurbs_crv_inter(3, n, &IsPtList[0], uMin, uMax, PARAM_CHORD_LEN, KNOT_AVERAGING);
	
	// 생성된 곡선의 시작 점과 끝점의 제어점을 구한다.
// 	GPoint3 P0 = IsPtList[0];
// 	GPoint3 Pn = IsPtList[IsPtList.size() - 1];
// 
// 	// 장면에 포함된 모든 메쉬를 구하고,
// 	std::vector<GMesh *> MeshList;
// 	GetPreOrderMeshList(pScene, MeshList, NULL);
// 
// 	// 생성된 곡선의 시작 점과 끝 점에 가장 가까운 곡선을 저장할 변수를 정의한다.
// 	GNurbsCrv *pStProxiCrv, *pEdProxiCrv;
// 	double StParam, EdParam;
// 	double StMinDist = pCrv->Renderer.CtrlPtSize * 3.0;
// 	double EdMinDist = pCrv->Renderer.CtrlPtSize * 3.0;
// 	bool StFlag = false;
// 	bool EdFlag = false;
// 
// 	// 장면의 모든 곡선 객체에 대하여,
// 	int NumMesh = (int)MeshList.size();
// 	for (int i = 0; i < NumMesh; ++i)
// 	{
// 		GMesh *pMesh = MeshList[i];
// 		double d, tmp;
// 		if (pMesh->GetClassName() == "GNurbsCrv")
// 		{
// 			GNurbsCrv *pOtherCrv = (GNurbsCrv *)pMesh;
// 			d = ::get_min_dist(P0, pOtherCrv, &tmp);
// 			if (d < StMinDist)
// 			{
// 				pStProxiCrv = pOtherCrv;
// 				StMinDist = d;
// 				StParam = tmp;
// 				StFlag = true;
// 			}
// 
// 			d = ::get_min_dist(Pn, pOtherCrv, &tmp);
// 			if (d < EdMinDist)
// 			{
// 				pEdProxiCrv = pOtherCrv;
// 				EdMinDist = d;
// 				EdParam = tmp;
// 				EdFlag = true;
// 			}
// 		}
// 	}
// 
// 	if (StFlag)
// 	{
// 		double umin, umax;
// 		pStProxiCrv->GetDomain(&umin, &umax);
// 		StParam = ((StParam - umin) < (umax - umin) * 0.2) ? umin : StParam;
// 		StParam = ((umax - StParam) < (umax - umin) * 0.2) ? umax : StParam;
// 		GPoint3 StPt = cast_pt3(pStProxiCrv->Eval(StParam));
// 		pProjCrv->P[0].Set(StPt[0], StPt[1], StPt[2], 1.0);
// 	}
// 
// 	if (EdFlag)
// 	{
// 		double umin, umax;
// 		pEdProxiCrv->GetDomain(&umin, &umax);
// 		EdParam = ((EdParam - umin) < (umax - umin) * 0.2) ? umin : EdParam;
// 		EdParam = ((umax - EdParam) < (umax - umin) * 0.2) ? umax : EdParam;
// 		GPoint3 EdPt = cast_pt3(pEdProxiCrv->Eval(EdParam));
// 		pProjCrv->P[pProjCrv->n].Set(EdPt[0], EdPt[1], EdPt[2], 1.0);
// 	}

	ARR_COPY(pCrv->U, pProjCrv->U, pCrv->n + pCrv->p + 2);
	for (int i = 0; i < pCrv->n + 1; ++i)
	{
		pCrv->P[i] = pProjCrv->P[i];
	}
	delete pProjCrv;

	pCrv->ModifiedFlag = MODIFIED_CTLPT_POS;
}
*/

/*!
*	\brief 선택된 객체를 편집한다 (변위는 모두 편집 좌표계에서 표현됨)
*
*	\param t 편집될 객체의 이동 변위
*	\param q 편집될 객체의 회전 변위
*	\param s 편집될 객체의 스케일 변위
*/
void PEditor::Edit(GVector3 t, GQuater q, GVector3 s)
{
	// 선택된 객체가 없다면 리턴한다.
	if (EditObjList.empty())
		return;

	// Shift가 눌렸다면,
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		// 편집 툴의 편집 좌표계를 편집하고 리턴한다.
		EC = EC * GTransf(t, q);
		return;
	}

	// 먼저, 선택된 모든 객체를 편집하고,
	for (unsigned i = 0; i < EditObjList.size(); ++i)
		EditObjList[i]->Edit(EC, t, q, s);

	// 편집 객체가 넙스 곡선의 정점이라면,
// 	GMesh *pMesh = GetEditMesh();
// 	if (pMesh->GetClassName() == "GNurbsCrv" && EditObjList[0]->GetClassName() == "GVertex")
// 	{
// 		// 곡선을 참조 메쉬에 뷰 방향으로 투영한다.
// 		GNurbsCrv *pCrv = (GNurbsCrv *)pMesh;
// 		ProjectCrvOnMesh(pCrv);
// 		pCrv->ModifiedFlag = MODIFIED_CTLPT_POS;
// 		pCrv->UpdateMesh();
// 	}

	// 편집 메쉬를 구하고,
	GMesh *pEditMesh = GetEditMesh();

	// 주변 영역을 편집할 경우 (선택 객체가 정점 또는 삼각형),
	if (!EditVertRgn.empty())
	{
		// 편집 객체의 주변 영역을 편집하고,
		for (unsigned i = 0; i < EditVertRgn.size(); ++i)
			EditVertRgn[i]->Edit(EC, t, q, s);

		if (pEditMesh->pBVH != NULL)
		{
			// 주변 영역에 대한 BVH를 구성한다.
			if (pEditFaceRgnBvh != NULL)
				delete pEditFaceRgnBvh;

			pEditFaceRgnBvh = new PQP_Model();
			pEditFaceRgnBvh->BeginModel();
			for (unsigned i = 0; i < EditFaceRgn.size(); ++i)
			{
				int *vidx = EditFaceRgn[i]->vIdx;
				GPoint3 p0 = pEditMesh->Verts[vidx[0]].P;
				GPoint3 p1 = pEditMesh->Verts[vidx[1]].P;
				GPoint3 p2 = pEditMesh->Verts[vidx[2]].P;
				pEditFaceRgnBvh->AddTri(p0.V, p1.V, p2.V, i);
			}
			pEditFaceRgnBvh->EndModel();
		}		
	}
		
	// 장면에 포함된 모든 메쉬의 리스트 구하여,
	std::vector<GMesh *> MeshList;
	GetPreOrderMeshList(pScene, MeshList, NULL);

	// 각각의 메쉬에 대하여 필요에 따라 업데이트를 수행한다.
	int NumMesh = (int)MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
		MeshList[i]->UpdateMesh();

	// 주변 영역을 편집할 경우 (선택 객체가 정점 또는 삼각형),
	if (!EditVertRgn.empty() && pEditFaceRgnBvh != NULL)
	{
		for (int i = 0; i < NumMesh; ++i)
		{
			GMesh *pTmpMesh = MeshList[i];
			if (pEditMesh == pTmpMesh)
				continue;

			// 편집 주변 영역과의 충돌 감지를 수행하고, 충돌 회피 모드라면 역 편집을 수행한다.
			if (collision_check(pEditMesh->MC, pEditFaceRgnBvh, pTmpMesh) > 0 && EditMeshDlg.m_CollisionMode == 1)
				Edit(-t, inv(q), GVector3(1, 1, 1));
		}
	}
	else // 객체를 편집할 경우
	{
		for (int i = 0; i < NumMesh; ++i)
		{
			GMesh *pTmpMesh = MeshList[i];
			if (pEditMesh == pTmpMesh)
				continue;

			// 다른 메쉬와 충돌 감지를 수행하고, 충돌 회피 모드라면 역 편집을 수행한다.
			if (collision_check(pEditMesh, pTmpMesh) > 0  && EditMeshDlg.m_CollisionMode == 1)
				Edit(-t, inv(q), GVector3(1, 1, 1));
		}
	}

	// 편집 툴의 편집 좌표계를 편집한다.
	EC = EC * GTransf(t, q);
}

/*!
*	\brief 선택된 객체를 편집을 종료한다.
*/
void PEditor::EditFinish()
{
	// 선택된 객체가 없다면 리턴한다.
	if (EditObjList.empty())
		return;

	// 편집된 메쉬를 구하여,
	GMesh *pMesh = GetEditMesh();

	// 메쉬 정점이나 삼각형이 편집된 경우, 메쉬의 법선 정보를 갱신한다.
	std::string ClsName = EditObjList[0]->GetClassName();
	if (ClsName == "GVertex" && pMesh->GetClassName() == "GNurbsCrv")
	{
		GNurbsCrv *pCrv = (GNurbsCrv *)pMesh;
		ProjectCrvOnMesh(pCrv);
		pCrv->ModifiedFlag = MODIFIED_SAMPLE_NUM;
		pCrv->UpdateMesh();
	}
	else if (ClsName == "GVertex" || ClsName == "GFace" || ClsName == "GCtrlPt")
	{
		// 정점의 위치나 삼각형의 위치가 편집 되었다면 메쉬를 업데이트 한다.
		pMesh->ModifiedFlag = MODIFIED_NORMAL;
		pMesh->UpdateMesh();

		// 편집 영역이 존재하고 기존 BVH가 존재한다면 BVH를 새로 갱신한다.
		if (!EditVertRgn.empty() && pMesh->pBVH != NULL)
			pMesh->CreateBVH();
	}

	// 회전 편집툴의 참조 변수를 초기화 한다.
	RotRefX = GTransf();
	RotRefPos.Set(0.0, 0.0, 0.0);

	// 선택된 첫번째 객체의 메쉬의 편집정보를 대화 상자에 표시한다.
	if (EditMeshDlg.m_hWnd != NULL)
	{
		EditMeshDlg.m_EditMeshTransX = (int)(pMesh->MC.t[0] * 10000.0 + 0.5) / 10000.0;
		EditMeshDlg.m_EditMeshTransY = (int)(pMesh->MC.t[1] * 10000.0 + 0.5) / 10000.0;
		EditMeshDlg.m_EditMeshTransZ = (int)(pMesh->MC.t[2] * 10000.0 + 0.5) / 10000.0;

		pMesh->MC.q.GetEulerAngle(EditMeshDlg.m_EditMeshRotX, EditMeshDlg.m_EditMeshRotY, EditMeshDlg.m_EditMeshRotZ);
		EditMeshDlg.m_EditMeshRotX = (int)(EditMeshDlg.m_EditMeshRotX * 10000.0 + 0.5) / 10000.0;
		EditMeshDlg.m_EditMeshRotY = (int)(EditMeshDlg.m_EditMeshRotY * 10000.0 + 0.5) / 10000.0;
		EditMeshDlg.m_EditMeshRotZ = (int)(EditMeshDlg.m_EditMeshRotZ * 10000.0 + 0.5) / 10000.0;

		GVector3 S = pMesh->S;
		EditMeshDlg.m_EditMeshSclX = (int)(S[0] * 10000.0 + 0.5) / 10000.0;
		EditMeshDlg.m_EditMeshSclY = (int)(S[1] * 10000.0 + 0.5) / 10000.0;
		EditMeshDlg.m_EditMeshSclZ = (int)(S[2] * 10000.0 + 0.5) / 10000.0;

		EditMeshDlg.UpdateData(FALSE);		
	}	
}

/*!
*	\brief 편집 후, 편집 툴의 상태를 초기화한다.
*/
void PEditor::Reset()
{
	EditToolType = EDIT_INACTIVE;
	EditObjList.clear();
	EditVertRgn.clear();
	EditFaceRgn.clear();
	EditBndryPts.clear();
	theSelector.Reset();
	if (pUnfoldMesh != NULL)
		delete pUnfoldMesh;
	pUnfoldMesh = NULL;
}

/*!
*	\brief 선택된 객체(정점, 삼각형, 메쉬 등)를 삭제한다.
*/
void PEditor::DeleteObject()
{
	// 선택된 편집 객체가 없다면 리턴한다.
	if (EditObjList.empty())
		return;

	// 선택된 객체의 클래스 이름을 구하여,
	std::string ClsName = EditObjList[0]->GetClassName();

	// 선택된 객체가 정점이라면,
	if (ClsName == "GVertex")
	{
		DeleteMeshVert();		
	}
	// 선택된 편집 객체가 삼각형이라면,
	else if (ClsName == "GFace")
	{
		DeleteMeshFace();		
	}
	// 선택된 편집 객체가 정점도 삼각형도 아니라며,
	else
	{
		// 참조 메쉬를 구하고, 메쉬를 삭제한다.
		std::vector<GMesh *> RefMeshList;
		GetRefMeshList(RefMeshList);

		std::vector<std::string> NameList;
		int NumMesh = (int)RefMeshList.size();
		for (int i = 0; i < NumMesh; ++i)
			NameList.push_back(RefMeshList[i]->Name);

		for (int i = 0; i < NumMesh; ++i)
			pScene->DelMesh(pScene->MeshList, NameList[i]);

		// 삭제된 메쉬에 바인딩된 메쉬의 바인딩 정보를 갱신한다.
		NumMesh = (int)pScene->MeshList.size();
		for (int i = 0; i < NumMesh; ++i)
			pScene->MeshList[i]->InitRefBoneInfo();

		// 에티터를 초기화 한다.
		Reset();
	}
}

/*!
*	\brief 선택된 정점을 삭제한다.
*/
void PEditor::DeleteMeshVert()
{
	// 편집 메쉬를 구하고,
	GMesh *pMesh = GetEditMesh();

	// 삭제할 정점의 인덱스를 구한다.
	std::vector<int> VertList;
	for (int i = 0; i < GetNumEditObj(); ++i)
	{
		GVertex *pVert = (GVertex *)EditObjList[i];
		VertList.push_back(pVert->Idx);
	}

	// 메쉬의 정점을 삭제한다.
	pMesh->DelVerts(VertList);

	// 에디터를 초기화하고, 참조 메쉬를 선택한다.
	Reset();
	EditObjList.push_back(pMesh);
}

/*!
*	\brief 선택된 삼각형을 삭제한다.
*/
void PEditor::DeleteMeshFace()
{
	// 참조 메쉬를 구하여,
	GMesh *pMesh = GetEditMesh();

	// 삭제할 삼각형의 인덱스를 구한다.
	std::vector<int> FaceList;
	for (int i = 0; i < GetNumEditObj(); ++i)
	{
		GFace *pFace = (GFace *)EditObjList[i];
		FaceList.push_back(pFace->Idx);
	}

	// 메쉬의 삼각형을 삭제한다.
	pMesh->DelFaces(FaceList);

	// 에디터를 초기화하고, 참조 메쉬를 선택한다.
	Reset();
	EditObjList.push_back(pMesh);
}

/*!
*	\brief 선택된 편집 객체를 렌더링 하도록 한다.
*/
void PEditor::ShowMesh()
{
	// 선택된 편집 객체가 없다면 리턴한다.
	if (EditObjList.empty())
		return;

	// 참조 메쉬를 구하고, 메쉬를 삭제한다.
	std::vector<GMesh *> RefMeshList;
	GetRefMeshList(RefMeshList);

	int NumMesh = (int)RefMeshList.size();
	for (int i = 0; i < NumMesh; ++i)
		RefMeshList[i]->ShowMesh(false);
}

/*!
*	\brief 선택된 편집 객체를 렌더링하지 않도록 한다.
*/
void PEditor::HideMesh()
{
	// 선택된 편집 객체가 없다면 리턴한다.
	if (EditObjList.empty())
		return;

	// 참조 메쉬를 구하고, 메쉬를 삭제한다.
	std::vector<GMesh *> RefMeshList;
	GetRefMeshList(RefMeshList);

	int NumMesh = (int)RefMeshList.size();
	for (int i = 0; i < NumMesh; ++i)
		RefMeshList[i]->HideMesh(false);
}