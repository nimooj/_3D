// PEditor.cpp : �ش� DLL�� �ʱ�ȭ ��ƾ�� �����մϴ�.
//
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern double _sin_table[];
extern double _cos_table[];

/*!
*	\brief ����Ʈ ������
*/
PSelector::PSelector()
{
	// ������ ���(9)
	pEditor = NULL;									// �����Ϳ� ���� ������
	SelectToolType = SELECT_TOOL_RECT;				// ���� �� ����(�簢��, �ð���, �ٰ���, �귯��)
	SelectModeType = SELECT_MODE_FRONT_ONLY;		// ���� ��� (�ո�, �ո�� �޸� ���)
	SelectObjType = SELECT_MESH;					// ���� ��ü ����(�޽�, ����, �ﰢ��, ��, ������ ��)
	MouseOverGroupIdx = -1;							// ���� ���콺 �����Ϳ� ������ �ﰢ�� �׷��� �ε���
	MouseOverFaceIdx = -1;							// ���� ���콺 �����Ϳ� ������ �ﰢ���� �ε���
	MouseOverVertIdx = -1;							// ���� ���콺 �����Ϳ� ���� ����� ������ �ε���
	MouseOverPtCoords.Set(0.0, 0.0, 0.0);			// ���� ���콺 �����Ϳ� ������ �ﰢ������ ���� Barycentric ��ǥ
	SltRgn2DPtList.clear();							// ������ ��ǥ�迡�� ǥ���� ���� ������ �����ڸ� ���� ����Ʈ
	SltRgn3DPtList.clear();							// ���� ��ǥ�迡�� ǥ���� ���� ������ �����ڸ� ���� ����Ʈ (�������� ���� ����)
}

/*!
*	\brief ���� ������
*
*	\param cpy ����� ��ü
*/
PSelector::PSelector(const PSelector &cpy)
{
	// ������ ���(9)
	pEditor = cpy.pEditor;						// �����Ϳ� ���� ������
	SelectToolType = cpy.SelectToolType;		// ���� �� ����(�簢��, �ð���, �ٰ���, �귯��)
	SelectModeType = cpy.SelectModeType;		// ���� ��� (�ո�, �ո�� �޸� ���)
	SelectObjType = cpy.SelectObjType; 			// ���� ��ü ����(�޽�, ����, �ﰢ��, ��, ������ ��)
	MouseOverGroupIdx = cpy.MouseOverGroupIdx;	// ���� ���콺 �����Ϳ� ������ �ﰢ�� �׷��� �ε���
	MouseOverFaceIdx = cpy.MouseOverFaceIdx;	// ���� ���콺 �����Ϳ� ������ �ﰢ���� �ε���
	MouseOverVertIdx = cpy.MouseOverVertIdx;	// ���� ���콺 �����Ϳ� ���� ����� ������ �ε���
	MouseOverPtCoords = cpy.MouseOverPtCoords;	// ���� ���콺 �����Ϳ� ������ �ﰢ������ ���� Barycentric ��ǥ
	SltRgn2DPtList = cpy.SltRgn2DPtList;		// ������ ��ǥ�迡�� ǥ���� ���� ������ �����ڸ� ���� ����Ʈ
	SltRgn3DPtList = cpy.SltRgn3DPtList;		// ���� ��ǥ�迡�� ǥ���� ���� ������ �����ڸ� ���� ����Ʈ (�������� ���� ����)
}

/*!
*	\brief �Ҹ���
*/
PSelector::~PSelector()
{
}

/*!
*	\brief ���� ������
*
*	\param rhs ���Ե� ��ü
*/
PSelector &PSelector::operator =(const PSelector &rhs)
{
	// ������ ���(9)
	pEditor = rhs.pEditor;						// �����Ϳ� ���� ������
	SelectToolType = rhs.SelectToolType;		// ���� �� ����(�簢��, �ð���, �ٰ���, �귯��)
	SelectModeType = rhs.SelectModeType;		// ���� ��� (�ո�, �ո�� �޸� ���)
	SelectObjType = rhs.SelectObjType; 			// ���� ��ü ����(�޽�, ����, �ﰢ��, ��, ������ ��)
	MouseOverGroupIdx = rhs.MouseOverGroupIdx;	// ���� ���콺 �����Ϳ� ������ �ﰢ�� �׷��� �ε���
	MouseOverFaceIdx = rhs.MouseOverFaceIdx;	// ���� ���콺 �����Ϳ� ������ �ﰢ���� �ε���
	MouseOverVertIdx = rhs.MouseOverVertIdx;	// ���� ���콺 �����Ϳ� ���� ����� ������ �ε���
	MouseOverPtCoords = rhs.MouseOverPtCoords;	// ���� ���콺 �����Ϳ� ������ �ﰢ������ ���� Barycentric ��ǥ
	SltRgn2DPtList = rhs.SltRgn2DPtList;		// ������ ��ǥ�迡�� ǥ���� ���� ������ �����ڸ� ���� ����Ʈ
	SltRgn3DPtList = rhs.SltRgn3DPtList;		// ���� ��ǥ�迡�� ǥ���� ���� ������ �����ڸ� ���� ����Ʈ (�������� ���� ����)

	return *this;
}

/*!
*	\brief ���� ���콺�� �������� 3���� ��ǥ�� ��ȯ�Ѵ�.
*
*	\param x ���콺�� X ��ǥ
*	\param y ���콺�� Y ��ǥ
*
*	\return ���콺 �������� 3���� ��ǥ�� ��ȯ�Ѵ�.
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
*	\brief ��ü ������ ���� ��� ������ �ʱ�ȭ �۾��� ����
*
*	\param x ���콺�� X ��ǥ
*	\param y ���콺�� Y ��ǥ
*	\param nFlags ���콺 ���� ��ư�� ���� ���� �߰� ���� (MK_CONTROL...)
*/
void PSelector::LButtonDown(int x, int y, UINT nFlags)
{
	//std::cout << "PSelector::LButtonDown" << std::endl;

	// ���� ������ ����, ���� ������ �������� ����Ʈ�� �߰��Ѵ�.
	switch (SelectToolType)
	{
		// �簢�� ���� ���̶��,
	case SELECT_TOOL_RECT:
		{
			// ���� ������ 2D, 3D ����� ����Ʈ�� �ʱ�ȭ �ϰ�,
			SltRgn2DPtList.clear();
			SltRgn3DPtList.clear();

			// 2D ������� ����Ʈ�� �߰��ϰ�,
			SltRgn2DPtList.push_back(GVector2(x, y));
			SltRgn2DPtList.push_back(GVector2(x, y));
			SltRgn2DPtList.push_back(GVector2(x, y));
			SltRgn2DPtList.push_back(GVector2(x, y));

			// 3D ������� ����Ʈ�� �߰��Ѵ�.
			GVector3 p = GetWorldCoord(x, y);
			SltRgn3DPtList.push_back(p);
			SltRgn3DPtList.push_back(p);
			SltRgn3DPtList.push_back(p);
			SltRgn3DPtList.push_back(p);
		}
		break;

		// �� ���� ���̶��,
	case SELECT_TOOL_CIRCLE:
		{
			// ���� ������ 2D, 3D ����� ����Ʈ�� �ʱ�ȭ �Ѵ�.
			SltRgn2DPtList.clear();
			SltRgn3DPtList.clear();
		}
		break;

		// �ð��� ���� ���̶��,
	case SELECT_TOOL_LASSO:
		{
			// ���� ������ 2D, 3D ����� ����Ʈ�� �ʱ�ȭ �ϰ�,
			SltRgn2DPtList.clear();
			SltRgn3DPtList.clear();

			// 2D �������� ����� ����Ʈ�� �߰��ϰ�,
			SltRgn2DPtList.push_back(GVector2(x, y));

			// 3D �������� ����� ����Ʈ(������ ��)�� �߰��Ѵ�.
			GVector3 p = GetWorldCoord(x, y);
			SltRgn3DPtList.push_back(p);
		}
		break;

		// �ٰ��� ���� ���̶��,
	case SELECT_TOOL_POLY:
		{
			// 2D �������� ����� ����Ʈ�� �߰��ϰ�,
			SltRgn2DPtList.push_back(GVector2(x, y));
			
			// 3D �������� ����� ����Ʈ�� �߰��ϰ�,
			GVector3 p = GetWorldCoord(x, y);
			SltRgn3DPtList.push_back(p);

			// �ٰ��� ����� �������̶�� �ߺ� �߰��Ѵ�.
			if (SltRgn2DPtList.size() == 1)
			{
				SltRgn2DPtList.push_back(GVector2(x, y));
				SltRgn3DPtList.push_back(p);
			}
		}
		break;

		// �귯�� ���̶��,
	case SELECT_TOOL_BRUSH:
		{
			// ���콺�� �����ϴ� �ﰢ��, ����, ������ ���� ������ �����Ѵ�.
			SetMouseOverInfo(x, y);

			// ���� �޽��� ã�Ƽ�,
			GMesh *pMesh = pEditor->GetEditMesh();
			if (pMesh != NULL && MouseOverVertIdx != -1)
			{
				// Shift�� Control�� ��� �ϳ��� ������ �ʾҴٸ�, ���� ���� ��ü ����Ʈ�� �����,
				if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
					pEditor->EditObjList.clear();	
								
				// ���� ���� ��ü�� �����̶�� ������ �����ϰ�,
				if (SelectObjType == SELECT_VERT)
					SelectVertByBrush(pMesh);					
				
				// ���� ���� ��ü�� �ﰢ���̶�� �ﰢ���� �����ϰ�,
				if (SelectObjType == SELECT_FACE)
					SelectFaceByBrush(pMesh);
			}
		}
		break;
	}
}

/*!
*	\brief ���콺�� ������ �� ȣ��Ǹ� ���� ������ �����Ѵ�.
*
*	\param x ���콺�� X ��ǥ
*	\param y ���콺�� Y ��ǥ
*	\param nFlags ���콺�� ������ ��, ������ ��ư�� ���� (MK_LBUTTON, MK_RBUTTON, MK_MBUTTON)
*/
void PSelector::MouseMove(int x, int y, UINT nFlags)
{
	//std::cout << "PSelector::MouseMove" << std::endl;

	CPoint MousePt[2];
	MousePt[0] = pEditor->MousePt[0];
	MousePt[1] = pEditor->MousePt[1];

	// ���� ���콺 ��ư�� ���� ���¶��,
	if (nFlags & MK_LBUTTON)
	{
		// ���� ���� �ڵ��� ���õ��� ���� ���,
		switch (SelectToolType)
		{
			// �簢�� ���� ���� ����� ���,
		case SELECT_TOOL_RECT:	
			if (!SltRgn3DPtList.empty())
			{
				// 2D ����� ����Ʈ�� �߰��ϰ�,
				SltRgn2DPtList[0] = GVector2(MousePt[0].x, MousePt[0].y);
				SltRgn2DPtList[1] = GVector2(MousePt[0].x, MousePt[1].y);
				SltRgn2DPtList[2] = GVector2(MousePt[1].x, MousePt[1].y);
				SltRgn2DPtList[3] = GVector2(MousePt[1].x, MousePt[0].y);

				// 3D ����� ����Ʈ�� �߰��Ѵ�.
				SltRgn3DPtList[0] = GetWorldCoord(MousePt[0].x, MousePt[0].y);
				SltRgn3DPtList[1] = GetWorldCoord(MousePt[0].x, MousePt[1].y);
				SltRgn3DPtList[2] = GetWorldCoord(MousePt[1].x, MousePt[1].y);
				SltRgn3DPtList[3] = GetWorldCoord(MousePt[1].x, MousePt[0].y);				
			}			
			break;

			// �� ���� ���� ����� ���,
		case SELECT_TOOL_CIRCLE:
			{
				// ������ ���� ������ �����,
				SltRgn2DPtList.clear();
				SltRgn3DPtList.clear();

				// ���� �����ϴ� 2D/3D ������� �����Ͽ� ����Ʈ�� �߰��Ѵ�.
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

			// �ð��� ���� ���� ����� ���,
		case SELECT_TOOL_LASSO:	
			if (!SltRgn3DPtList.empty())
			{
				// 2D ����� ����Ʈ�� �߰��ϰ�,
				SltRgn2DPtList.push_back(GVector2(MousePt[1].x, MousePt[1].y));

				// 3D ����� ����Ʈ�� �߰��Ѵ�.
				GVector3 p = GetWorldCoord(MousePt[1].x, MousePt[1].y);
				SltRgn3DPtList.push_back(p);
			}
			break;

			// �귯�� ���� ���̶��,
		case SELECT_TOOL_BRUSH:
			{
				// ���콺�� �����ϴ� �ﰢ��, ����, ������ ���� ������ �����Ѵ�.
				SetMouseOverInfo(x, y);

				// ���� �޽��� ���ϰ�,
				GMesh *pMesh = pEditor->GetEditMesh();
				if (pMesh != NULL && MouseOverVertIdx != -1)
				{
					// ���� ���� ��ü�� �����̶��, ������ �����ϰ�,
					if (SelectObjType == SELECT_VERT)
						SelectVertByBrush(pMesh);					
					
					// ���� ���� ��ü�� �ﰢ���̶��, �ﰢ���� �����Ͽ� �߰��ϰ�,
					if (SelectObjType == SELECT_FACE)
						SelectFaceByBrush(pMesh);
				}
			}
			break;			
		}		
	}
	// ���� ���콺 ��ư�� �������� ���� ���¶��,
	else
	{
		// ���콺 �����Ϳ� �����ϴ� ���� �ڵ��� ã��,
		if (!SetMouseOverEditHandle(x, y))
			SetMouseOverInfo(x, y);	// ���õ� �ڵ��� ���ٸ�, ���콺�� �����ϴ� �ﰢ��, ����, ������ ���� ������ �����Ѵ�.

		// (1) �ٰ��� ���� ���� ��� ���̰�, (2) ���ð����� ���� ���̶��,
		if (SelectToolType == SELECT_TOOL_POLY && !SltRgn3DPtList.empty())
		{
			// ������ �ε����� ���ϰ�,
			int LastIdx = (int)SltRgn2DPtList.size() - 1;

			// 2D ����� ����Ʈ�� �����ϰ�,
			SltRgn2DPtList[LastIdx] = GVector2(MousePt[1].x, MousePt[1].y);

			// 3D ����� ����Ʈ�� �����Ѵ�.
			SltRgn3DPtList[LastIdx] = GetWorldCoord(MousePt[1].x, MousePt[1].y);
		}

		// ���� �޴��� ���� 
		switch (pEditor->EditToolType)
		{
			// ���� ��θ� ���� ���̶�� 
		case EDIT_GEODESIC_PATH:
			{
				// ���� ������ �����ϰ�,
				std::vector<GMesh *> MeshList;
				GMesh *pMesh = pEditor->GetEditMesh();
				GetPreOrderMeshList(pEditor->pScene, MeshList, pMesh);

				// ���� ����� ���� ������ �����Ѵ�.
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

			// Sculpting, smoothing ���� ��� ���̶��,
		case EDIT_SCULPT:
		case EDIT_SMOOTH:
			{
				// ���� ������ ã�´�.
				if (MouseOverVertIdx != -1)
					pEditor->FindEditWgtForSculpt();
			}
			break;
		}
	}	
}

/*!
*	\brief ���ο� ��ü�� �����Ѵ�.
*
*	\param x ���콺�� X ��ǥ
*	\param y ���콺�� Y ��ǥ
*	\param nFlags ���콺 ���� ��ư�� ������ ���� �߰� ���� (MK_CONTROL...)
*/
void PSelector::LButtonUp(int x, int y, UINT nFlags)
{
	if (SltRgn2DPtList.empty())
	{
		// �귯�� ���� ���̶��,
		if (SelectToolType == SELECT_TOOL_BRUSH)
		{
			// �ߺ� ��ü�� �����ϰ�, ���� ��ǥ�踦 �����Ѵ�.
			pEditor->UniqueEditObjList();
			pEditor->SetEC();
		}
		return;
	}

	switch (SelectToolType)
	{
		// ���� Ÿ���� SELECT_TOOL_RECT �Ǵ� SELECT_TOOL_LASSO ���,
	case SELECT_TOOL_RECT:
	case SELECT_TOOL_CIRCLE:
	case SELECT_TOOL_LASSO:
		// ���� ���� ������ ���� �������� �߰��Ѵ�.
		SltRgn2DPtList.push_back(SltRgn2DPtList[0]);
		SltRgn3DPtList.push_back(SltRgn3DPtList[0]);
		break;

		// ���� Ÿ���� SELECT_TOOL_PLOY �̰�
	case SELECT_TOOL_POLY:
		if (GetAsyncKeyState(VK_SPACE) & 0x8000) // �����̽� Ű�� ���� ���¶��,
		{
			// ���� ���� ������ ���� �������� ���������� �����Ѵ�.
			int LastIdx = SltRgn2DPtList.size() - 1;
			SltRgn2DPtList[LastIdx] = SltRgn2DPtList[0];
			SltRgn3DPtList[LastIdx] = SltRgn3DPtList[0];
		}
		else
			return;
		break;
	}

	// ���� ���õ� �޽��� ���ϰ�,
	GMesh *pPrevMesh = pEditor->GetEditMesh();

	// ���� ��ü ������ ����(���� ���� �ڵ��� ���õ��� ���� ���)
	switch (SelectObjType)
	{
	case SELECT_MESH:		// �޽��� �����ϰų�, 
		SelectMesh(x, y);
		break;

	case SELECT_BONE:		// �� �޽��� �����ϰų�,
		SelectBone(x, y);
		break;

	case SELECT_VERT:		// �޽��� ������ �����ϰų�,
		SelectVert(x, y);
		break;

	case SELECT_FACE:		// �޽��� �ﰢ���� �����ϰų�, 
		SelectFace(x, y);
		break;

	case SELECT_FACE_AND_CUT:	// �޽��� �ﰢ���� �����ϰ� ��迵���� �����ϰų�,
		SelectFaceAndCutBndry(x, y);
		break;

	case SELECT_PT_ON_FACE:	// �޽��� �ﰢ���� ���콺�� �������� �����ϰų�,
		SelectPtOnFace(x, y);
		break;

	case SELECT_FACE_GROUP:	// �ﰢ�� �׷��� �����ϰų�,
		SelectFaceGroup(x, y);
		break;

	case SELECT_CTRL_PT:	// �������� �����ϰ�,
		SelectCtrlPt(x, y);
		break;
	}

	// ���� ���õ� �޽��� ���Ͽ�, ���� �ٸ��ٸ�,
	GMesh *pCurrMesh = pEditor->GetEditMesh();
	if (pPrevMesh != pCurrMesh && pEditor->EditMeshDlg.m_hWnd != NULL)
	{
		// �۾�â�� ������ �ڵ��� ���Ͽ�,
		HWND hWnd = pEditor->EditMeshDlg.GetParent()->GetSafeHwnd();
		::PostMessage(hWnd, WM_UPDATE_SELECT, 0, 0);
	}

	// ���õ� ��ü�� �̿��Ͽ� ���� ���� ���� ��ǥ�踦 �����Ѵ�.
	pEditor->SetEC();

	// ���� ������ ��������Ʈ�� �ʱ�ȭ �Ѵ�.
	SltRgn2DPtList.clear();
	SltRgn3DPtList.clear();	
}

/*!
*	\brief ���� ���� ���� ������ �����Ѵ�.
*
*	\param delta ���콺 ���� ����
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
*	\brief ���� ���� ���¸� �ʱ�ȭ�Ѵ�.
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
*	\brief ���� ��ü, ������ �׸��� ���� ������ ������ �Ѵ�.
*/
void PSelector::Render()
{
	switch (SelectToolType)
	{
		// �簢�� ���� ���� ���� ���̶��,
	case SELECT_TOOL_RECT:
		RenderRectTool();
		break;

		// �� ���� ���� ���� ���̶��,
	case SELECT_TOOL_CIRCLE:
		RenderCircleTool();
		break;

		// �ð��� ���� ���� ���� ���̶��,
	case SELECT_TOOL_LASSO:
		RenderLassoTool();
		break;

		// �ٰ��� ���� ���� ���� ���̶��,
	case SELECT_TOOL_POLY:
		RenderPolyTool();
		break;

		// �귯�� ���� ���� ���� ���̶��,
	case SELECT_TOOL_BRUSH:
		RenderBrushTool();
		break;
	}
	
	// ������ ���� ���̶�� ���콺�� �����ϴ� ������ ǥ���Ѵ�.
	if (SelectObjType == SELECT_VERT || pEditor->EditToolType == EDIT_GEODESIC_PATH)
		RenderMouseOverVert();

	// �ﰢ���� ���� ���̶�� ���콺�� �����ϴ� �ﰢ���� ǥ���Ѵ�.
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
*	\brief �簢�� ���� ���� ǥ���Ѵ�.
*/
void PSelector::RenderRectTool()
{
	if (SltRgn3DPtList.empty())
		return;

	// ������ ���� ��� ����� ��輱�� ������ �����Ѵ�.
	glDisable(GL_LIGHTING);
	glColor3f(0.2627, 1.0, 0.64);
	glEnable(GL_LINE_STIPPLE); 
	glLineStipple(2, 0xf0f0);

	// ������ �簢�� ���� ������ ǥ���Ѵ�.
	glBegin(GL_LINE_LOOP);
	{
		glVertex3f(SltRgn3DPtList[0][0], SltRgn3DPtList[0][1], SltRgn3DPtList[0][2]);
		glVertex3f(SltRgn3DPtList[1][0], SltRgn3DPtList[1][1], SltRgn3DPtList[1][2]);
		glVertex3f(SltRgn3DPtList[2][0], SltRgn3DPtList[2][1], SltRgn3DPtList[2][2]);
		glVertex3f(SltRgn3DPtList[3][0], SltRgn3DPtList[3][1], SltRgn3DPtList[3][2]);
	}			
	glEnd();

	// ������ Ȱ��ȭ�ϰ� ��輱�� ������ ��Ȱ��ȭ �Ѵ�.
	glEnable(GL_LIGHTING);
	glDisable(GL_LINE_STIPPLE);
}

/*!
*	\brief �� ���� ���� ǥ���Ѵ�.
*/
void PSelector::RenderCircleTool()
{
	if (SltRgn3DPtList.empty())
		return;

	// ������ ���� ��� ����� ��輱�� ������ �����Ѵ�.
	glDisable(GL_LIGHTING);
	glColor3f(0.2627, 1.0, 0.64);
	// 	glEnable(GL_LINE_STIPPLE); 
	// 	glLineStipple(2, 0xf0f0);

	// ���� �� ���� ������ ǥ���Ѵ�.
	glBegin(GL_LINE_LOOP);
	{
		int NumPt = SltRgn3DPtList.size();
		for (int i = 0; i < NumPt; ++i)
			glVertex3f(SltRgn3DPtList[i][0], SltRgn3DPtList[i][1], SltRgn3DPtList[i][2]);
	}			
	glEnd();

	// ���� �߽ɰ� ���� ���콺 ��ġ�� �����ϴ� ������ ǥ���Ѵ�.
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

	// ������ Ȱ��ȭ�ϰ� ��輱�� ������ ��Ȱ��ȭ �Ѵ�.
	glEnable(GL_LIGHTING);	
}

/*!
*	\brief �ð��� ���� ���� ǥ���Ѵ�.
*/
void PSelector::RenderLassoTool()
{
	if (SltRgn3DPtList.empty())
		return;

	// ������ ���� ��� ����� ��輱�� ������ �����Ѵ�.
	glDisable(GL_LIGHTING);
	glColor3f(0.2627, 1.0, 0.64);
// 	glEnable(GL_LINE_STIPPLE); 
// 	glLineStipple(2, 0xf0f0);

	// ���� �ð��� ���� ������ ǥ���Ѵ�.
	glBegin(GL_LINE_LOOP);
	{
		int NumPt = SltRgn3DPtList.size();
		for (int i = 0; i < NumPt; ++i)
			glVertex3f(SltRgn3DPtList[i][0], SltRgn3DPtList[i][1], SltRgn3DPtList[i][2]);
	}			
	glEnd();

	// ������ Ȱ��ȭ�ϰ� ��輱�� ������ ��Ȱ��ȭ �Ѵ�.
	glEnable(GL_LIGHTING);
//	glDisable(GL_LINE_STIPPLE);
}

/*!
*	\brief �ٰ��� ���� ���� ǥ���Ѵ�.
*/
void PSelector::RenderPolyTool()
{
	if (SltRgn3DPtList.empty())
		return;

	// ������ ���� ��� ����� ��輱�� ������ �����Ѵ�.
	glDisable(GL_LIGHTING);
	glColor3f(0.2627, 1.0, 0.64);
	glEnable(GL_LINE_STIPPLE); 
	glLineStipple(2, 0xf0f0);

	// �ٰ��� ���� ������ ǥ���Ѵ�.
	glBegin(GL_LINE_LOOP);
	{
		int ptnum = SltRgn3DPtList.size();
		for (int i = 0; i < ptnum; ++i)
			glVertex3f(SltRgn3DPtList[i][0], SltRgn3DPtList[i][1], SltRgn3DPtList[i][2]);
	}			
	glEnd();

	// ������ Ȱ��ȭ�ϰ� ��輱�� ������ ��Ȱ��ȭ �Ѵ�.
	glEnable(GL_LIGHTING);
	glDisable(GL_LINE_STIPPLE);
}

/*!
*	\brief �귯�� ���� ���� ǥ���Ѵ�.
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

	// ����� ���� �׽�Ʈ�� ��Ȱ��ȭ�ϰ� ������ Ȱ��ȭ�Ѵ�.
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	
	// ���� ��ǥ�迡��
	glPushMatrix();
	{
		// �귯���� ũ�⸦ ���ϰ�,
		double BrushRadius = pMesh->BBox.GetDiagonal() * (pEditor->EditMeshDlg.m_BrushRadius / 1000.0);

		// ���콺 �����Ϳ� ���� ����� ������ ��ǥ�踦 ���ϰ�,
		GPoint3 p = pMesh->Verts[MouseOverVertIdx].P;
		GVector3 n = pMesh->Verts[MouseOverVertIdx].N;
		GQuater q;
		q.SetFromArcBall(GVector3(0.0, 1.0, 0.0), n);
		GTransf MC = pMesh->MC * GTransf(cast_vec3(p), q);

		// �� ��ǥ��� ��ȯ�� ��,
		glMultMatrixd(cast_mat(MC).M);

		// ������ �׸���,
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
			// x ���� �׸���,
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(-BrushRadius, 0.0, 0.0);
			glVertex3f(BrushRadius, 0.0, 0.0);

			// z ���� �׸���.
			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0.0, 0.0, -BrushRadius);
			glVertex3f(0.0, 0.0, BrushRadius);
		}				
		glEnd();
		glLineWidth(1.0);

		// ������ �׸���.
		glPointSize(8.0);
		glBegin(GL_POINTS);
		{
			glVertex3f(0.0, 0.0, 0.0);
		}
		glEnd();
		glPointSize(1.0);
	}
	glPopMatrix();	// ���� ��ǥ��� �����Ѵ�.


	// ������ Ȱ��ȭ�ϰ� ��輱�� ������ ��Ȱ��ȭ �Ѵ�.
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

/*!
*	\brief ���� ���콺 �����Ϳ� �����ϴ� �ﰢ��(�簢��)�� ������ �Ѵ�.
*/
void PSelector::RenderMouseOverFace()
{
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL || MouseOverFaceIdx == -1)
		return;

	if ((GetAsyncKeyState(VK_MENU) & 0x8000))
		return;

	// ����� ���� �׽�Ʈ�� ����,
	glDisable(GL_LIGHTING);
	//glDisable(GL_DEPTH_TEST);

	// ���� ��ǥ�迡��
	glPushMatrix();
	{
		// ���� ��ü�� �𵨸� ��ǥ��� ��ȯ�ϰ�,
		glMultMatrixd(cast_mat(pMesh->MC).M);

		// �簢�� �޽����,
		if (pMesh->FaceType == FACE_QUAD)
		{
			// ¦����° �ﰢ���� Ȧ����° �ﰢ���� �����Ͽ�,
			GFace *pFace0, *pFace1;
			if (MouseOverFaceIdx % 2 == 0)	// ¦�� ��° �ﰢ���̶��,
			{
				pFace0 = &(pMesh->Faces[MouseOverFaceIdx]);
				pFace1 = &(pMesh->Faces[MouseOverFaceIdx + 1]);
			}
			else	// Ȧ����° �ﰢ�� �̶��,
			{
				pFace0 = &(pMesh->Faces[MouseOverFaceIdx - 1]);
				pFace1 = &(pMesh->Faces[MouseOverFaceIdx]);
			}

			// �簢�� ������ �ε����� ���ϰ�,
			int vidx0 = pFace0->vIdx[0];
			int vidx1 = pFace0->vIdx[1];
			int	vidx2 = pFace0->vIdx[2];
			int	vidx3 = pFace1->vIdx[2];

			// ���õ� �簢���� �׸���.
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
		else	// �ﰢ�� �޽����,
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
	glPopMatrix();		// ���� ��ǥ��� �����Ѵ�.

	// ����� ���� �׽�Ʈ�� �ٽ� �Ҵ�.
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);	
}

/*!
*	\brief ���� ���콺 �����Ϳ� ����(���� �����) ������ ������ �Ѵ�.
*/
void PSelector::RenderMouseOverVert()
{
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL || MouseOverVertIdx == -1)
		return;

	if ((GetAsyncKeyState(VK_MENU) & 0x8000))
		return;

	// ���� ���� �����ϰ�,
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// ���� ��ǥ�迡��
	glPushMatrix();
	{
		// ���� ��ü�� �𵨸� ��ǥ��� ��ȯ�ϰ�,
		glMultMatrixd(cast_mat(pMesh->MC).M);

		// ���õ� ������ �׸���.
		GVertex *v = &(pMesh->Verts[MouseOverVertIdx]);
		
		glColor3f(1.0, 0.0, 0.0);
		glPointSize(8.0);
		glBegin(GL_POINTS);
			glVertex3dv(v->P.V);
		glEnd();
		glPointSize(1.0);	
	}
	glPopMatrix();		// ���� ��ǥ��� �����Ѵ�.

	// ���� ��忡�� ���´�.
	glDisable(GL_BLEND);
	glDisable(GL_POINT_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

/*!
*	\brief ���� ���콺 �����Ϳ� ����(���� �����) �ﰢ�� �׷��� ������ �Ѵ�.
*/
void PSelector::RenderMouseOverFaceGroup()
{
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL || MouseOverGroupIdx == -1 || pMesh->GrpFaces.empty())
		return;

	if ((GetAsyncKeyState(VK_MENU) & 0x8000))
		return;

	// ����� ���� �׽�Ʈ�� ����,
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// ���� ��ǥ�迡��
	glPushMatrix();
	{
		// ���� ��ü�� �𵨸� ��ǥ��� ��ȯ�ϰ�,
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
	glPopMatrix();		// ���� ��ǥ��� �����Ѵ�.

	// ����� ���� �׽�Ʈ�� �ٽ� �Ҵ�.
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

/*!
*	\brief ���� ���콺 �����Ϳ� �����ϴ� �ﰢ�� ���� ���� ������ �Ѵ�.
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

	// ���� ��ǥ�迡��
	glPushMatrix();
	{
		// ���� ��ü�� �𵨸� ��ǥ��� ��ȯ�ϰ�,
		glMultMatrixd(cast_mat(pMesh->MC).M);

		// ���콺�� �����ϴ� �ﰢ���� �� ������ ��ǥ�� ���ϰ�,
		int vidx0 = pMesh->Faces[MouseOverFaceIdx].vIdx[0];
		int vidx1 = pMesh->Faces[MouseOverFaceIdx].vIdx[1];
		int vidx2 = pMesh->Faces[MouseOverFaceIdx].vIdx[2];
		GPoint3 p0 = pMesh->Verts[vidx0].P;
		GPoint3 p1 = pMesh->Verts[vidx1].P;
		GPoint3 p2 = pMesh->Verts[vidx2].P;

		// �ﰢ�� ���� �������� ����Ͽ�,
		GPoint3 pt = p2 + (p0 - p2) * MouseOverPtCoords[0] + (p1 - p2) * MouseOverPtCoords[1];
		
		// ���� ���� �����ϰ�,
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

		// ���� ��忡�� ���´�.
		glDisable(GL_BLEND);
		glDisable(GL_POINT_SMOOTH);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
	}
	glPopMatrix();		// ���� ��ǥ��� �����Ѵ�.
}

/*!
*	\brief Sculpt ���� �������Ѵ�.
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

	// ���콺�� �����ϴ� �ﰢ���� �� ������ ��ǥ�� ���ϰ�,
	int *vidx = pMesh->Faces[MouseOverFaceIdx].vIdx;
	GPoint3 p0 = pMesh->Verts[vidx[0]].P;
	GPoint3 p1 = pMesh->Verts[vidx[1]].P;
	GPoint3 p2 = pMesh->Verts[vidx[2]].P;
	GVector3 n0 = pMesh->Verts[vidx[0]].N;
	GVector3 n1 = pMesh->Verts[vidx[1]].N;
	GVector3 n2 = pMesh->Verts[vidx[2]].N;

	// �ﰢ�� ���� �������� ����Ͽ�,
	GPoint3 p = p2 + (p0 - p2) * MouseOverPtCoords[0] + (p1 - p2) * MouseOverPtCoords[1];
	GVector3 n = n0 * MouseOverPtCoords[0] + n1 * MouseOverPtCoords[1] + n2 * MouseOverPtCoords[2];

	GQuater q;
	q.SetFromArcBall(GVector3(0.0, 1.0, 0.0), n);
	GTransf MC = pMesh->MC * GTransf(cast_vec3(p), q);

	// ���� ��ǥ�迡��
	glPushMatrix();
	{
		// ���� ��ü�� �𵨸� ��ǥ��� ��ȯ�ϰ�,
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
			// ����� ���� �׽�Ʈ�� ��Ȱ��ȭ�ϰ� ������ Ȱ��ȭ�Ѵ�.
			glDisable(GL_LIGHTING);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POINT_SMOOTH);

			// �귯���� ũ�⸦ ���ϰ�,
			double Radius = pMesh->BBox.GetDiagonal() * (pEditor->EditMeshDlg.m_EditRgnDist / 1000.0);

			// ������ �׸���,
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
				// x ���� �׸���,
				glColor3f(1.0, 0.0, 0.0);
				glVertex3f(-Radius, 0.0, 0.0);
				glVertex3f(Radius, 0.0, 0.0);

				// z ���� �׸���.
				glColor3f(0.0, 0.0, 1.0);
				glVertex3f(0.0, 0.0, -Radius);
				glVertex3f(0.0, 0.0, Radius);
			}				
			glEnd();
			glLineWidth(1.0);

			// ������ �׸���.
			glPointSize(8.0);
			glColor3f(0.7, 0.7, 0.7);
			glBegin(GL_POINTS);
			{
				glVertex3f(0.0, 0.0, 0.0);
			}
			glEnd();
			glPointSize(1.0);


			// ������ Ȱ��ȭ�ϰ� ��輱�� ������ ��Ȱ��ȭ �Ѵ�.
			glDisable(GL_LINE_SMOOTH);
			glDisable(GL_BLEND);
			glEnable(GL_LIGHTING);
			glEnable(GL_DEPTH_TEST);
		}
	}
	glPopMatrix();		// ���� ��ǥ��� �����Ѵ�.
}

/*!
*	\brief ���� �޽��� ������ �ﰢ�� ������ ���� ������ �Ѵ�.
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
*	\brief ���õ� ��ü�� ����, �ﰢ�� ���� ������ ���ؼ� ���õ� ��ü�� �ﰢ���� ���� �ٸ� ����(���̵�)���� ������ �Ѵ�.
*	\note ������ ������(2016/01/31)
*/
void PSelector::RenderForSelect()
{
	// ���õ� ���� ��ü�� ���ϰ�,
	GMesh *pMesh = pEditor->GetEditMesh();

	// ���õ� ��ü�� ���ٸ� �����Ѵ�.
	if (pMesh == NULL)
		return;

	// ���� ���ۿ� ���� ���۸� �ʱ�ȭ�� ��,
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ����� ���� ���� �� ���� ����� �����ϰ�,
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	pEditor->pScene->SetupViewFrustum();

	// ����� ���� ��ȯ�� �����Ѵ�.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	pEditor->pScene->SetupViewTransform();

	// ���� ���� ���̶��,
	if ((pEditor->EditToolType >= EDIT_TRA_X && pEditor->EditToolType <= EDIT_TRA_XYZ) ||
		(pEditor->EditToolType >= EDIT_ROT_X && pEditor->EditToolType <= EDIT_ROT_Z) ||
		(pEditor->EditToolType >= EDIT_SCL_X && pEditor->EditToolType <= EDIT_SCL_XYZ))
	{
		// ���� ���� �� �ڵ��� ���� �ٸ� ���� (255, 255, �ڵ� ���̵�)���� �������ϰ� �����Ѵ�.
		RenderForSelectHandle();
		return;
	}

	// �ﰢ���� ������ ���� �� �ﰢ���� ���� �ٸ� �������� ������ �Ѵ�.
	RenderForSelectFace();

	// ���� ���� �� �ڵ��� ���� �ٸ� �������� ������ �Ѵ�. ���� = (255, 255, �ڵ� ���̵�)
	RenderForSelectHandle();
}

/*!
*	\brief �޽��� �ﰢ�� ������ ���� �� �ﰢ���� �ٸ� �������� ������ �Ѵ�.
*/
void PSelector::RenderForSelectFace()
{
	// ���� ��ü Ÿ���� ����, �ﰢ��, �ﰢ�� �׷�� ���� ������ �����Ѵ�.
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

	// ������ ��Ȱ��ȭ �ϰ�, ���� �ﰢ�� ������ ����.
	glDisable(GL_LIGHTING);

	// ���õ� �޽��� �ﰢ���� ���δٸ� ������ ���̵� ������ �Ѵ�.
	unsigned int r = 0, g = 0, b = 0;

	// ���� ��ǥ�迡��
	glPushMatrix();
	{
		// �޽��� �𵨸� ��ǥ��� ��ȯ�ϰ�,
		glMultMatrixd(cast_mat(pMesh->MC).M);

		// �ﰢ���� ������ ���ϰ�,
		int NumFace = pMesh->GetNumFace();

		// �ﰢ�� ���� ���÷��� ����Ʈ�� �ִٸ�, 
		if (pMesh->DispFaceColMesh != 0)
		{
			// ���÷��� ����Ʈ�� ȣ���ϰ�,
			glCallList(pMesh->DispFaceColMesh);
		}
		else
		{
			if (pMesh->GrpFaces.empty())	// �ﰢ�� �׷��� ���ǵ��� �ʾҴٸ�,
			{
				// �ﰢ�� �������� �������� ���÷��� ����Ʈ�� �����Ѵ�.
				pMesh->DispFaceColMesh = glGenLists(1);
				glNewList(pMesh->DispFaceColMesh, GL_COMPILE_AND_EXECUTE);
				{
					// �ﰢ���� ���� �ٸ� ����(���̵�)���� �������� ��,
					glBegin(GL_TRIANGLES);
					for (int i = 0; i < NumFace; ++i)
					{
						int *vidx = pMesh->Faces[i].vIdx;
						GetColorId(r, g, b);	// (0, 0, 1) ���� ���۵�...
						glColor3ub(r, g, b);
						glVertex3dv(pMesh->Verts[vidx[0]].P.V);
						glVertex3dv(pMesh->Verts[vidx[1]].P.V);
						glVertex3dv(pMesh->Verts[vidx[2]].P.V);
					}
					glEnd();
				}
				glEndList();				
			}
			else	// �ﰢ�� �׷��� ���ǵǾ��ٸ�,
			{
				pMesh->DispFaceColMesh = glGenLists(1);
				glNewList(pMesh->DispFaceColMesh, GL_COMPILE_AND_EXECUTE);
				{
					// �ﰢ���� �׷캰�� ������ �Ѵ�.
					std::map<std::string, std::vector<int>>::iterator it;
					for (it = pMesh->GrpFaces.begin(); it != pMesh->GrpFaces.end(); ++it)
					{
						int NumGrpFace = it->second.size();
						glBegin(GL_TRIANGLES);
						for (int i = 0; i < NumGrpFace; ++i)
						{
							int fidx = it->second[i];
							int *vidx = pMesh->Faces[fidx].vIdx;
							GetColorId(r, g, b);	// (0, 0, 1) ���� ���۵�...
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
	// ���� ��ǥ��� �����Ѵ�.
	glPopMatrix();

	// ������ Ȱ��ȭ �Ѵ�.
	glEnable(GL_LIGHTING);
}

/*!
*	\brief ���� ���� �ڵ� ������ ���� �� ���� �ڵ��� �ٸ� �������� ������ �Ѵ�.
*/
void PSelector::RenderForSelectHandle()
{
	// ���� ���� �ڵ��� �������� ���� �ʱ� ���� �����ϰ�,
	unsigned char r = 255, g = 255;

	// ���� �׽�Ʈ�� ��Ȱ��ȭ �Ѵ�.
	glDisable(GL_DEPTH_TEST);

	// ������ ��Ȱ��ȭ �ϰ�, ���� �ڵ� ������ ����.
	glDisable(GL_LIGHTING);

	// ���� ��ǥ�迡��
	glPushMatrix();
	{
		// ���� ���� �𵨸� ��ǥ��� ��ȯ�Ͽ�,
		glMultMatrixd(cast_mat(pEditor->EC).M);

		// ���� ���� ����� ũ�⸦ ����ϰ�,
		double len = pEditor->GetEditToolSize();

		// �̵� ���� �ڵ��� ������ �ϰ�,
		if (pEditor->EditToolType >= EDIT_TRA && pEditor->EditToolType <= EDIT_TRA_XYZ)
		{
			// x��� �ڵ�,
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

			// y��� �ڵ�,
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

			// z��� �ڵ��� �������� ��,
			glColor3ub(r, g, EDIT_TRA_Z);
			glBegin(GL_LINES);
			glVertex3d(0.0, 0.0, len * 0.4);
			glVertex3d(0.0, 0.0, len);
			glEnd();
			glPushMatrix();
			glTranslated(0.0, 0.0, len);
			glutSolidCone(len * 0.1, len * 0.3, 10, 10);
			glPopMatrix();

			// xy �ڵ�
			glColor3ub(r, g, EDIT_TRA_XY);
			glBegin(GL_POLYGON);
			{
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(len * 0.4, 0.0, 0.0);
				glVertex3d(len * 0.4, len * 0.4, 0.0);
				glVertex3d(0.0, len * 0.4, 0.0);
			}
			glEnd();

			// yz �ڵ�
			glColor3ub(r, g, EDIT_TRA_YZ);
			glBegin(GL_POLYGON);
			{
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(0.0, len * 0.4, 0.0);
				glVertex3d(0.0, len * 0.4, len * 0.4);
				glVertex3d(0.0, 0.0, len * 0.4);
			}
			glEnd();

			// zx �ڵ�
			glColor3ub(r, g, EDIT_TRA_ZX);
			glBegin(GL_POLYGON);
			{
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(0.0, 0.0, len * 0.4);
				glVertex3d(len * 0.4, 0.0, len * 0.4);
				glVertex3d(len * 0.4, 0.0, 0.0);			
			}
			glEnd();

			// xyz �ڵ�
			glColor3ub(r, g, EDIT_TRA_XYZ);
			glPushMatrix();
			{
				glTranslated(len * 0.1, len * 0.1, len * 0.1);
				glutSolidCube(len * 0.2);
			}
			glPopMatrix();
		}

		// ȸ�� ���� �ڵ��� ������ �ϰ�,
		if (pEditor->EditToolType >= EDIT_ROT && pEditor->EditToolType <= EDIT_ROT_Z)
		{
			// ���� ��ǥ��: ���� ���� ��ǥ��
			double modelview[16];
			glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
			
			// ���� ������ �׵θ�
			GQuater qt;
			qt.SetFromMatrix(modelview, true);
			qt = inv(qt);

			// ���������� �׵θ� ���� ������ �����ϱ� ���� ���� 
			GVector3 view_axis(0.0, 0.0, 1.0);
			view_axis = qt * view_axis;

			glLineWidth(20.0);
			int snum = 50;

			// x��� �ڵ�,
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

			// y��� �ڵ�,
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

			// z��� �ڵ��� �������� ��,
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

		// ������ ���� �ڵ��� ������ �ϰ�,
		if (pEditor->EditToolType >= EDIT_SCL && pEditor->EditToolType <= EDIT_SCL_XYZ)
		{
			// x��� �ڵ�,
			glColor3ub(r, g, EDIT_SCL_X);
			glBegin(GL_LINES);
			glVertex3d(len * 0.4, 0.0, 0.0);
			glVertex3d(len, 0.0f, 0.0f);
			glEnd();
			glPushMatrix();
			glTranslated(len, 0.0f, 0.0f);
			glutSolidCube((float)len * 0.2);
			glPopMatrix();

			// y��� �ڵ�,
			glColor3ub(r, g, EDIT_SCL_Y);
			glBegin(GL_LINES);
			glVertex3d(0.0, len * 0.4, 0.0);
			glVertex3d(0.0f, len, 0.0f);
			glEnd();
			glPushMatrix();
			glTranslated(0.0, len, 0.0);
			glutSolidCube(len * 0.2);
			glPopMatrix();

			// z��� �ڵ��� �������� ��,
			glColor3ub(r, g, EDIT_SCL_Z);
			glBegin(GL_LINES);
			glVertex3d(0.0, 0.0, len * 0.4);
			glVertex3d(0.0, 0.0, len);
			glEnd();
			glPushMatrix();
			glTranslated(0.0, 0.0, len);
			glutSolidCube(len * 0.2);
			glPopMatrix();

			// xy �ڵ�
			glColor3ub(r, g, EDIT_SCL_XY);
			glBegin(GL_POLYGON);
			{
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(len * 0.4, 0.0, 0.0);
				glVertex3d(len * 0.4, len * 0.4, 0.0);
				glVertex3d(0.0, len * 0.4, 0.0);
			}
			glEnd();

			// yz �ڵ�
			glColor3ub(r, g, EDIT_SCL_YZ);
			glBegin(GL_POLYGON);
			{
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(0.0, len * 0.4, 0.0);
				glVertex3d(0.0, len * 0.4, len * 0.4);
				glVertex3d(0.0, 0.0, len * 0.4);
			}
			glEnd();

			// zx �ڵ�
			glColor3ub(r, g, EDIT_SCL_ZX);
			glBegin(GL_POLYGON);
			{
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(0.0, 0.0, len * 0.4);
				glVertex3d(len * 0.4, 0.0, len * 0.4);
				glVertex3d(len * 0.4, 0.0, 0.0);			
			}
			glEnd();

			// xyz �ڵ�
			glColor3ub(r, g, EDIT_SCL_XYZ);
			glPushMatrix();
			{
				glTranslated(len * 0.1, len * 0.1, len * 0.1);
				glutSolidCube(len * 0.2);
			}
			glPopMatrix();
		}
	}
	// ���� ��ǥ��� �����Ѵ�.
	glPopMatrix();

	// ���� �׽�Ʈ�� ������ Ȱ��ȭ �Ѵ�.
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

/*!
*	\brief ��ŷ�� ���� ���ø�Ŀ�������� �����Ѵ�.
*
*	\param x ���콺�� X ��ǥ
*	\param y ���콺�� Y ��ǥ
*/
void PSelector::EnterSelectMode(int x, int y)
{
	// �� ��Ʈ ��ȯ ������ ���,
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// ���� ���۸� �����ϰ�, 
	memset(SelectBuf, 0, sizeof(GLuint) * SELECT_BUFFER_SIZE);
	glSelectBuffer(SELECT_BUFFER_SIZE, SelectBuf);

	// ������ ��� ������ ��,
	glRenderMode(GL_SELECT);

	// ���� ������ �ʱ�ȭ�ϰ� dummy(0)�� ����
	glInitNames();
	glPushName(0);

	// ���� ����� �����ϰ�,
	glMatrixMode(GL_PROJECTION);

	// ���� ���� ����� �����ϰ�,
	glPushMatrix();

	// ���� ����� ������ķ� �ʱ�ȭ �ϰ�,
	glLoadIdentity();

	// ���� ����� �����ϰ� ���� ��Ŀ� ���Ѵ�.
	int offset = 10;
	gluPickMatrix((double)x, (GLdouble)(viewport[3] - y), offset, offset, viewport);
	pEditor->pScene->SetupViewFrustum();

	// �𵨺� ����� �����ϰ�,
	glMatrixMode(GL_MODELVIEW);

	// ���� �𵨺� ����� �����ϰ�,
	glPushMatrix();

	// �𵨺� ����� ���� ��ķ� �ʱ�ȭ �ϰ�,
	glLoadIdentity();

	// ����(���) ��ǥ��� ��ȯ �ϰ�,
	pEditor->pScene->SetupViewTransform();
}

/*!
*	\brief ���� ��Ŀ������ �����ϰ�, �Ϲ� ������ ���� ���ƿ´�.
*
*	\return ���õ� ��ü�� ����
*/
int PSelector::ExitSelectMode()
{
	// ���� �𵨺� ����� �����ϰ�,
	glPopMatrix();

	// ���� ����� �����ϰ�,
	glMatrixMode(GL_PROJECTION);

	// ���� ���� ����� �����ϰ�,
	glPopMatrix();

	// �𵨺� ����� �����ϰ�,
	glMatrixMode(GL_MODELVIEW);

	// ���� ���ۿ� ����� ������ ��ȯ�Ѵ�.
	return glRenderMode(GL_RENDER);
}

/*!
*	\brief ��鿡 ���Ե� �޽��� �����Ѵ�.
*
*	\param x ���콺�� x ��ǥ
*	\param y ���콺�� y ��ǥ
*
*	\return �޽��� ���õ� ��� true, �׷��� ������ false.
*/
bool PSelector::SelectMesh(int x, int y)
{
	// Shift�� Control�� ��� �ϳ��� ������ �ʾҴٸ�,
	if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
		// ���� ���� ��ü ����Ʈ�� �����,
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
		pEditor->EditFaceRgn.clear();
		pEditor->EditBndryPts.clear();
	}

	// ����� ������ �޽� ����Ʈ�� ���ϰ�,
	std::vector<GMesh *> MeshList;
	GetPreOrderMeshList(pEditor->pScene, MeshList, NULL);

	// ������ �޽��� ���Ͽ�,
	for (unsigned i = 0; i < MeshList.size(); ++i)
	{
		// �� �޽��̰ų� ������ �޽���� �ǳʶڴ�.
		if (!MeshList[i]->Renderer.IsVisibleMesh() || MeshList[i]->SemanticType == MESH_BONE)
			continue;

		// ������ ������ ���Ͽ�,
		int NumVert = MeshList[i]->GetNumVert();
		for (int j = 0; j < NumVert; ++j)
		{
			// ������ ��ǥ��
			double wx, wy, wz;
			pEditor->pScene->MainCamera.GetScreenCoord(&(MeshList[i]->Verts[j]), &wx, &wy, &wz);

			// ���� �����ȿ� �ִٸ�,
			if (IsPtInPolygon(wx, wy, SltRgn2DPtList))
			{
				// Control Ű�� ���� ���, ���� ��ü�� �����ϰ�,
				if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
					pEditor->DelFromEditObjList(MeshList[i]);
				else	// �ƴ� ���(�ƹ� Ű�� ������ �ʰų�, Shift Ű�� ���� ���) ��ü�� �߰��Ѵ�.
					pEditor->AddToEditObjList(MeshList[i]);
				break;
			}
		}
	}

	// ���õ� �޽��� ���ٸ� false, �ִٸ� true�� �����Ѵ�.
	if (pEditor->EditObjList.empty())
		return false;
	else
		return true;
}


/*!
*	\brief ��鿡 ���Ե� �޽��� �����Ѵ�.
*
*	\param Name ������ �޽��� �̸�
*
*	\return �޽��� ���õ� ��� true, �׷��� ������ false.
*/
bool PSelector::SelectMesh(std::string Name)
{
	// Shift�� CtrlŰ�� ���� ���¸� ���Ͽ�,
	bool bShift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
	bool bCtrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;

	// Shift�� Control�� ��� �ϳ��� ������ �ʾҴٸ�,
	if (!bShift && !bCtrl)
	{
		// ���� ��ü ����Ʈ�� �����,
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
		pEditor->EditFaceRgn.clear();
		pEditor->EditBndryPts.clear();
	}

	// ����� �޽� ����Ʈ�� ���ϰ�,
	std::vector<GMesh *> MeshList;
	GetPreOrderMeshList(pEditor->pScene, MeshList, NULL);

	// ����� ������ �޽��� ���Ͽ�,
	int NumMesh = (int)MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
	{
		// ���� ���ڿ� ���� �̸��� �޽��� ã��
		if (MeshList[i]->Name == Name)
		{
			// Control Ű�� ���� ���, ��ü�� ����ϰ�,
			if (bCtrl)
				pEditor->ToggleEditObject(MeshList[i]);
			else	// �ƴ� ���(�ƹ� Ű�� ������ �ʰų�, Shift Ű�� ���� ���) ��ü�� �߰��Ѵ�.
				pEditor->AddToEditObjList(MeshList[i]);

			pEditor->SetEC();
			break;
		}
	}

	// �ߺ� ���õ� ��ü�� �����Ѵ�.
	pEditor->UniqueEditObjList();

	// ��ü�� ���� ����� ��ȯ�Ѵ�.
	if (pEditor->EditObjList.empty())
		return false;
	else
		return true;
}

/*!
*	\brief ��鿡 ���Ե� �� �޽��� �����Ѵ�.
*
*	\param x ���콺�� x ��ǥ
*	\param y ���콺�� y ��ǥ
*
*	\return �޽��� ���õ� ��� true, �׷��� ������ false.
*/
bool PSelector::SelectBone(int x, int y)
{
	// Shift�� Control�� ��� �ϳ��� ������ �ʾҴٸ�,
	if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
		// ���� ���� ��ü ����Ʈ�� �����,
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
		pEditor->EditFaceRgn.clear();
		pEditor->EditBndryPts.clear();
	}

	// ����� ������ �޽� ����Ʈ�� ���ϰ�,
	std::vector<GMesh *> MeshList;
	GetPreOrderMeshList(pEditor->pScene, MeshList, NULL);

	// ���õ� �� �޽��� ������ ������ �����ϰ�,
	GMesh *pSltBoneMesh = NULL;

	// ������ �� �޽��� ���Ͽ�,
	int NumMesh = (int)MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
	{
		// �������� �� �޽��� �ƴ϶�� �ǳʶ��,
		if (!MeshList[i]->Renderer.IsVisibleMesh() || MeshList[i]->SemanticType != MESH_BONE)
			continue;

		// ������ ������ ���Ͽ�,
		int NumVert = MeshList[i]->GetNumVert();
		for (int j = 0; j < NumVert; ++j)
		{
			// ������ ��ǥ��
			double wx, wy, wz;
			pEditor->pScene->MainCamera.GetScreenCoord(&(MeshList[i]->Verts[j]), &wx, &wy, &wz);

			// ���� �����ȿ� �ִٸ�,
			if (IsPtInPolygon(wx, wy, SltRgn2DPtList))
			{
				// ���� �� �޽��� �����ϰ�,
				pSltBoneMesh = MeshList[i];
				break;
			}
		}

		// �� �޽��� �ϳ��� ���� �Ǿ��ٸ� �������´�. 
		if (pSltBoneMesh)
			break;
	}

	// �� �޽��� ���� �Ǿ��ٸ�,
	if (pSltBoneMesh)
	{
		// ������ ���õǾ��� �� �޽��� ������ ���ϰ�,
		int PrevNumBoneMesh = (int)(pEditor->EditObjList.size());

		// Control Ű�� ���� ���, ���� ��ü�� �����ϰ�,
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			pEditor->DelFromEditObjList(pSltBoneMesh);
		// �ƴ� ���(�ƹ� Ű�� ������ �ʰų�, Shift Ű�� ���� ���) ��ü�� �߰��Ѵ�.
		else	
			pEditor->AddToEditObjList(pSltBoneMesh);

		// ���� ��ü�� �ߺ��� ������ ��,
		pEditor->UniqueEditObjList();

		// ������ ���õǾ��� �� �޽��� ������ ���ϰ�,
		int CurrNumBoneMesh = (int)(pEditor->EditObjList.size());

		// ���ο� �� �޽��� �߰� Ȥ�� ���� �Ǿ��ٸ�,
		if (CurrNumBoneMesh != PrevNumBoneMesh)
		{
			// �� �޽��� ����� ��Ų �޽��� ������ ���ϰ�,
			int NumSkinMesh = (int)pSltBoneMesh->LinkedMeshNameList.size();

			// ������ ��Ų �޽�����,
			for (int i = 0; i < NumSkinMesh; ++i)
			{
				// �� �޽��� ����� ��Ų �޽��� �˻��ؼ�,
				std::string SkinMeshName = pSltBoneMesh->LinkedMeshNameList[i];
				GMesh *pSkinMesh = FindMesh(pEditor->pScene->MeshList, SkinMeshName);

				// ��Ų �޽��� �����ϰ�, ������ ������ ���� ����ġ�� �����Ǿ��ٸ�,
				if (pSkinMesh != NULL && pSkinMesh->Renderer.VertColorType == VERTEX_COLOR_BONE_WEIGHT)
				{
					// ���� �޽��� ������ ������ ���õ� �� �޽��� ����ġ�� �����Ѵ�.
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
*	\brief ���õ� �޽����� Ư�� ������ �����Ѵ�.
*
*	\param x ���콺�� x ��ǥ
*	\param y ���콺�� y ��ǥ
*/
void PSelector::SelectVert(int x, int y)
{
	// ���� ���õ� �޽��� ���ٸ� �����Ѵ�.
	GMesh *pMesh = pEditor->GetEditMesh();

	// ���õ� ��ü�� �����ٸ� �����Ѵ�.
	if (pMesh == NULL)
	{
		// �޽��� �ϳ� �����ϰ� �����Ѵ�.
		SelectMesh(x, y);
		return;
	}

	// Shift�� Control�� ��� �ϳ��� ������ �ʾҴٸ�,
	if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
		// ���� ���� ��ü ����Ʈ�� �����,
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
		pEditor->EditFaceRgn.clear();
		pEditor->EditBndryPts.clear();
	}

	// ���� ���� �������� Ŭ���� �̿��� �����̶��,
	if (SltRgn2DPtList[0] == SltRgn2DPtList[1])
	{
		// Control Ű�� ���� ���, ���� ��ü�� �����ϰ�,
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			if (MouseOverVertIdx != -1)
				pEditor->DelFromEditObjList(&(pMesh->Verts[MouseOverVertIdx]));
		}
		// �ƴ� ���(�ƹ� Ű�� ������ �ʰų�, Shift Ű�� ���� ���) ��ü�� �߰��Ѵ�.
		else	 
		{
			if (MouseOverVertIdx != -1)
				pEditor->AddToEditObjList(&(pMesh->Verts[MouseOverVertIdx]));
		}
		
		std::cout << "Selected Vertex Id: " << MouseOverVertIdx << std::endl;
	}
	// ��� ������ �̿��� �޽� �����̶��,
	else
	{
		// �������⿡�� ���̴� �������� �����ϴ� �����,
		if (SelectModeType == SELECT_MODE_FRONT_ONLY)
		{
			// �ü� ���͸� ���ϰ�,
			GVector3 dir = pEditor->pScene->MainCamera.GetViewDirection();
			for (unsigned j = 0; j < pMesh->Verts.size(); ++j)
			{
				// ������ ������ ���� ���Ͱ� �ü� ���Ϳ� 90�� �̻��̶��,
				GVector3 n = pMesh->Verts[j].N;
				if (angle(dir, n) > 90.0)
				{
					// ������ ��ũ�� ��ǥ�� ���Ͽ� 
					double wx, wy, wz;
					pEditor->pScene->MainCamera.GetScreenCoord(&(pMesh->Verts[j]), &wx, &wy, &wz);

					// ���� ���� ���� ���ԵǾ��ٸ�
					if (IsPtInPolygon(wx, wy, SltRgn2DPtList))
					{
						// Control Ű�� ���� ���, ���� ��ü�� �����ϰ�,
						if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
							pEditor->DelFromEditObjList(&(pMesh->Verts[j]));
						// �ƴ� ���(�ƹ� Ű�� ������ �ʰų�, Shift Ű�� ���� ���) ��ü�� �߰��Ѵ�.
						else	
							pEditor->AddToEditObjList(&(pMesh->Verts[j]));
					}
				}				
			}
		}
		else	// �������� ��� ������ �����ϴ� �����,
		{
			for (unsigned j = 0; j < pMesh->Verts.size(); ++j)
			{
				// ������ ��ũ�� ��ǥ�� ���Ͽ� 
				double wx, wy, wz;
				pEditor->pScene->MainCamera.GetScreenCoord(&(pMesh->Verts[j]), &wx, &wy, &wz);

				// ���� ���� ���� ���ԵǾ��ٸ�
				if (IsPtInPolygon(wx, wy, SltRgn2DPtList))
				{
					// Control Ű�� ���� ���, ���� ��ü�� �����ϰ�,
					if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
						pEditor->DelFromEditObjList(&(pMesh->Verts[j]));
					// �ƴ� ���(�ƹ� Ű�� ������ �ʰų�, Shift Ű�� ���� ���) ��ü�� �߰��Ѵ�.
					else	
						pEditor->AddToEditObjList(&(pMesh->Verts[j]));
				}				
			}
		}
		
		// ���õ� ���� ��ü�� �ߺ��� �����Ѵ�.
		pEditor->UniqueEditObjList();
	}

	// ���� ��ü�� ������ ���� ����ġ�� ����Ѵ�.
	pEditor->FindEditWgt();
}

/*!
*	\brief �ﰢ���� ���콺�� �������� �����Ѵ�.
*
*	\param x ���콺�� x ��ǥ
*	\param y ���콺�� y ��ǥ
*/
void PSelector::SelectPtOnFace(int x, int y)
{
	// ���� ���õ� �޽��� ���ϰ�,
	GMesh *pMesh = pEditor->GetEditMesh();

	// ���õ� �޽��� ���ٸ� �����Ѵ�.
	if (pMesh == NULL)
	{
		// ���콺�� �����ϴ� �޽��� �ϳ� �����ϰ� �����Ѵ�.
		SelectMesh(x, y);
		return;
	}

	// ���� ���� ��ü ����Ʈ�� �����,
	pEditor->EditObjList.clear();
	pEditor->EditVertRgn.clear();
	pEditor->EditFaceRgn.clear();
	pEditor->EditBndryPts.clear();

	// �������� ��ǥ�� ���Ǿ��ٸ�,
	if (MouseOverPtCoords != GVector3(0.0, 0.0, 0.0))
	{
		static GPtOnFace SltPt;
		GFace *pFace = &(pMesh->Faces[MouseOverFaceIdx]);
		SltPt.Set(pFace, MouseOverPtCoords);
		pEditor->AddToEditObjList(&SltPt);
	}
}

/*!
*	\brief �귯�� ���� ���� ���� ������ �����Ѵ�.
*
*	\param pMesh ���õ� ��ü
*/
void PSelector::SelectVertByBrush(GMesh *pMesh)
{
	// ���� ���콺 �����Ϳ� ������ ���� �ֺ��� �귯�� ���� ������ ����ϰ�,
	std::vector<double> Distances;
	double BrushRadius = pMesh->BBox.GetDiagonal() * (pEditor->EditMeshDlg.m_BrushRadius / 1000.0);
	get_geodesic_dist_field(pMesh, &(pMesh->Verts[MouseOverVertIdx]), Distances, BrushRadius);

	// ���� ������ �������� ���� ����Ʈ�� �߰�/���� �Ѵ�.
	int NumVert = pMesh->GetNumVert();
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)	// Control Ű�� ���� ���, ���� ��ü�� �����ϰ�,
	{
		for (int i = 0; i < NumVert; ++i)
			if (Distances[i] < BrushRadius)
				pEditor->DelFromEditObjList(&(pMesh->Verts[i]));					
	}
	else		// �ƴ� ���(�ƹ� Ű�� ������ �ʰų�, Shift Ű�� ���� ���) ��ü�� �߰��Ѵ�.
	{
		for (int i = 0; i < NumVert; ++i)
			if (Distances[i] < BrushRadius)
				pEditor->AddToEditObjList(&(pMesh->Verts[i]));
	}
}

/*!
*	\brief ���õ� �޽����� Ư�� �ﰢ���� �����Ѵ�.
*
*	\param x ���콺�� x ��ǥ
*	\param y ���콺�� y ��ǥ
*/
void PSelector::SelectFace(int x, int y)
{
	// ���� ���õ� �޽��� ���ϰ�,
	GMesh *pMesh = pEditor->GetEditMesh();

	// ���õ� ��ü�� �����ٸ�, 
	if (pMesh == NULL)
	{
		// �޽��� �ϳ� �����ϰ� �����Ѵ�.
		SelectMesh(x, y);
		return;
	}

	// Shift�� Control�� ��� �ϳ��� ������ �ʾҴٸ�,
	if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
		// ���� ���� ��ü ����Ʈ�� �����,
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
		pEditor->EditFaceRgn.clear();
		pEditor->EditBndryPts.clear();
	}

	// ���콺 Ŭ���� �̿��� �����̶��,
	if (SltRgn2DPtList[0] == SltRgn2DPtList[1])
		SelectFaceByClick(pMesh);		
	else	// ��� ������ �̿��� �ﰢ�� �����̶��,
		SelectFaceByRgn(pMesh);		

	// ���� ��ü�� ������ ���� ����ġ�� ����Ѵ�.
	pEditor->FindEditWgt();
}

/*!
*	\brief ���õ� �޽����� Ư�� �ﰢ���� ���콺 Click���� �����Ѵ�.
*
*	\param pMesh ���õ� �޽�
*/
void PSelector::SelectFaceByClick(GMesh *pMesh)
{
	// ���콺 �����Ϳ� �����ϴ� �ﰢ���� �ִٸ�,
	if (MouseOverFaceIdx != -1)
	{
		// Control Ű�� ���� ���, ���� ��ü�� �����ϰ�,
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			pEditor->DelFromEditObjList(&(pMesh->Faces[MouseOverFaceIdx]));
			// Quad �޽���� �߰�������,
			if (pMesh->FaceType == FACE_QUAD)
			{
				// ¦����° �ﰢ���� Ȧ����° �ﰢ���� �����Ͽ� ������ �ﰢ���� �����Ѵ�.
				if (MouseOverFaceIdx % 2 == 0)
					pEditor->DelFromEditObjList(&(pMesh->Faces[MouseOverFaceIdx + 1]));
				else
					pEditor->DelFromEditObjList(&(pMesh->Faces[MouseOverFaceIdx - 1]));
			}
		}
		// �ƴ� ���(�ƹ� Ű�� ������ �ʰų�, Shift Ű�� ���� ���) ��ü�� �߰��Ѵ�.
		else	 
		{
			pEditor->AddToEditObjList(&(pMesh->Faces[MouseOverFaceIdx]));
			// Quad �޽����,
			if (pMesh->FaceType == FACE_QUAD)
			{
				// ¦����° �ﰢ���� Ȧ����° �ﰢ���� �����Ͽ� ������ �ﰢ���� �߰��Ѵ�.
				if (MouseOverFaceIdx % 2 == 0)
					pEditor->AddToEditObjList(&(pMesh->Faces[MouseOverFaceIdx + 1]));
				else
					pEditor->AddToEditObjList(&(pMesh->Faces[MouseOverFaceIdx - 1]));
			}
		}
	}

	// ���õ� ���� ��ü�� �ߺ��� �����Ѵ�.
	pEditor->UniqueEditObjList();
}

/*!
*	\brief ���õ� �޽����� Ư�� �ﰢ���� ���� �������� �����Ѵ�.
*
*	\param pMesh ���õ� �޽�
*/

void PSelector::SelectFaceByRgn(GMesh *pMesh)
{
	// ������ �Ӽ��� ������� ���̾�Ӽ��� �߰��ϰ�,
	pMesh->Renderer.RenderType = (pMesh->Renderer.RenderType | RENDER_POLYGON | RENDER_WIRE);

	// ���õ� �ﰢ���� ������ ������ �����ϰ�,
	std::vector<GFace *> SltFaceList;

	// �������⿡�� ���̴� �ﰢ���� �����ϴ� �����,
	if (SelectModeType == SELECT_MODE_FRONT_ONLY)
	{
		// �ü� ���͸� ���Ͽ�,
		GVector3 dir = pEditor->pScene->MainCamera.GetViewDirection();

		// �� �ﰢ�� ����,
		int NumFace = pMesh->GetNumFace();
		for (int i = 0; i < NumFace; ++i)
		{
			// �ﰢ���� ������ ���Ͽ�,
			GVector3 n = pMesh->Faces[i].GetNormal(false);

			// �ո��� ���δٸ�,
			if (angle(dir, n) > 90.0)
			{
				// �ﰢ���� �� ������ ���Ͽ�,
				int *vidx = pMesh->Faces[i].vIdx;
				for (int j = 0; j < 3; ++j)
				{
					// �����쿡 ������ ��ǥ�� ���ϰ�,
					double wx, wy, wz;
					pEditor->pScene->MainCamera.GetScreenCoord(&(pMesh->Verts[vidx[j]]), &wx, &wy, &wz);

					// ������ ��ǥ�� ���� ���� �����Ѵٸ�,
					if (IsPtInPolygon(wx, wy, SltRgn2DPtList))
					{
						// ���ø���Ʈ�� �߰��Ѵ�.
						SltFaceList.push_back(&(pMesh->Faces[i]));

						// Quad �޽���� �߰������� ������ �ﰢ���� �߰��Ѵ�.
						if (pMesh->FaceType == FACE_QUAD)
						{
							// ¦����° �ﰢ���� Ȧ����° �ﰢ���� �����Ͽ�,
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
	else	// �������� ��� �ﰢ���� �����ϴ� �����,
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
					// ���ø���Ʈ�� �߰��Ѵ�.
					SltFaceList.push_back(&(pMesh->Faces[i]));

					// Quad �޽���� �߰������� ������ �ﰢ���� �߰��Ѵ�.
					if (pMesh->FaceType == FACE_QUAD)
					{
						// ¦����° �ﰢ���� Ȧ����° �ﰢ���� �����Ͽ�,
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
	// Control Ű�� ���� ���, ���� ��ü�� �����ϰ�,
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{			
		for (int i = 0; i < NumSltFace; ++i)
			pEditor->DelFromEditObjList(SltFaceList[i]);
	}
	// �ƴ� ���(�ƹ� Ű�� ������ �ʰų�, Shift Ű�� ���� ���) ��ü�� �߰��Ѵ�.
	else
	{
		for (int i = 0; i < NumSltFace; ++i)
			pEditor->AddToEditObjList(SltFaceList[i]);
	}

	// ���õ� ���� ��ü�� �ߺ��� �����Ѵ�.
	pEditor->UniqueEditObjList();
}

/*!
*	\brief ���� ������ ���� �����ϴ� ������ ����Ʈ�� �����Ѵ�.
*	\note ���� �̻��
*
*	\param pMesh �������� ����� �޽�
*	\param CutPtList �������� ����� ����Ʈ
*
*	\return ����Ʈ�� ����� �������� ������ ��ȯ�Ѵ�.
*/
int PSelector::FindCutPtList(GMesh *pMesh, std::vector<GCutPt> &CutPtList)
{
	// ���� ������ ���� �����ϴ� �ȼ��� ����Ʈ�� ���Ѵ�.
	std::vector<CPoint> PixelList;
	int NumPt = (int)SltRgn2DPtList.size();
	for (int i = 0; i < NumPt - 1; ++i)
	{
		// ���� (x0, y0)�� (x1, y1)�� �����ϴ� ��� �ȼ��� ��ǥ�� ���Ѵ�.
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

	// ���� ���� ��ü ����Ʈ�� �����,
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
		// �� �ȼ��� ��ġ���� ���� ������ �ȼ� ���� �о,
		GLubyte pixel[3];
		int x = PixelList[i].x;
		int y = PixelList[i].y;
		glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

		// �ﰢ���� ������ �ε���(1, 2, 3, ...)�� ���ϰ�,
		int CurrFaceIdx = pixel[0] * 256 * 256 + pixel[1] * 256 + pixel[2] - 1;

		// ���õ� �ﰢ���� ���ٸ� ���� �ȼ��� �Ѿ��,
		if (CurrFaceIdx < 0 || CurrFaceIdx > pMesh->GetNumFace() - 1)
			continue;

		// ���õ� �ﰢ���� ���� �ﰢ���� �ٸ� �ﰢ���̶��,
		if (CurrFaceIdx != PrevFaceIdx && PrevFaceIdx != -1)
		{
			// ���� �ﰢ���� ���� ���õ� �ﰢ���� �����ϴ� ������ �ε����� ã�´�.
			int sidx, eidx;
			GFace *pPrevFace = &(pMesh->Faces[PrevFaceIdx]);
			GFace *pCurrFace = &(pMesh->Faces[CurrFaceIdx]);			
			int result = pCurrFace->GetSharedVertIdx(pPrevFace, sidx, eidx);

			// �ϳ��� ������ ������ ���,
			if (result == 0)
			{
				// �������� ���Ͽ� ���� ������ ����Ʈ�� �߰��Ѵ�.
				GCutPt pt(pCurrFace, sidx, eidx, 0.0, pMesh->Verts[sidx].P);
				CutPtList.push_back(pt);
			}

			// ���� ���õ� �ﰢ���� ���� �ﰢ���� �����ϸ�,
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

				// ���� ������ ���Ͽ� ����Ʈ�� �߰��Ѵ�.
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
*	\brief �� ���� ������ ������ �����ϴ� �ȼ��� ����Ʈ�� ����Ѵ�.
*
*	\param p ������ ������
*	\param q ������ ����
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
*	\brief ���� ���ۿ� ����� (x, y) �ȼ��� ������ �д´�.
*
*	\param x ���콺�� x ��ǥ
*	\param y ���콺�� y ��ǥ
*	\param color (R, G, B) ������ �����
*/
void PSelector::GetPixelColor(int x, int y, GLubyte *color)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, color);
}

/*!
*	\brief ���õ� �޽����� �ﰢ���� �����ϰ�, ��� ������ �����Ѵ�.
*
*	\param x ���콺�� x ��ǥ
*	\param y ���콺�� y ��ǥ
*/
void PSelector::SelectFaceAndCutBndry(int x, int y)
{
	// ���õ� �޽��� ���ٸ� ���ο� �޽��� �����ϰ� �����Ѵ�.
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL)
	{
		SelectMesh(x, y);
		return;
	}

	// ������ �Ӽ��� ������� ���̾�Ӽ��� �߰��Ѵ�.
	pMesh->Renderer.RenderType = (pMesh->Renderer.RenderType | RENDER_POLYGON | RENDER_WIRE);

	/////////////////////////////////////////////////////////////////
	// 1 �ܰ�: ���� ������ ��ø�� �ﰢ���� ������ ��ǥ�� �����Ѵ�. //
	/////////////////////////////////////////////////////////////////

	// ������ ������ ������ �����ϴ� ������ �����Ѵ�.
	std::map<int, double> ModifiedVertDist;
	std::vector<GVertex> OldVerts = pMesh->Verts;
	
	// ���� ������ ���� �����ϴ� �ȼ��� ����Ʈ�� ���Ѵ�.
	for (unsigned i = 0; i < SltRgn2DPtList.size() - 1; ++i)
	{
		// �� �� St�� Ed�� �����ϴ� ��� �ȼ��� ��ǥ�� ���Ѵ�.
		std::vector<CPoint> PixelList;
		CPoint St(SltRgn2DPtList[i][0], SltRgn2DPtList[i][1]);
		CPoint Ed(SltRgn2DPtList[i + 1][0], SltRgn2DPtList[i + 1][1]);
		GetPixelList(St, Ed, PixelList);
		
		GFace *pPrevFace = NULL;
		for (unsigned j = 0; j < PixelList.size(); ++j)
		{
			// �� �ȼ��� ��ġ���� ���� ������ �ȼ� ���� �о,
			GLubyte color[3];
			GetPixelColor(PixelList[j].x, PixelList[j].y, color);

			// ���õ� �ﰢ���� ���ϰ�, ���ٸ� ���� �ȼ��� �Ѿ��.
			int CurrFaceIdx = color[0] * 256 * 256 + color[1] * 256 + color[2] - 1;
			if (CurrFaceIdx < 0 || CurrFaceIdx > pMesh->GetNumFace() - 1)
				continue;

			// ���õ� �ﰢ���� ���� �ﰢ���� �ٸ� �ﰢ���̶��,
			GFace *pCurrFace = &(pMesh->Faces[CurrFaceIdx]);
			if (pCurrFace != pPrevFace && pPrevFace != NULL)
			{	
				// ���� �� �ﰢ���� �� ������ ��ǥ�� ���ϰ�,
				int *vidx = pCurrFace->vIdx;
				GVertex *v0 = &(OldVerts[vidx[0]]);
				GVertex *v1 = &(OldVerts[vidx[1]]);
				GVertex *v2 = &(OldVerts[vidx[2]]);

				// �� ������ ������ ���� ���� �������� ���ԵǴ��� �����Ѵ�.
				double tmp;
				GVector2 w0, w1, w2;
				pEditor->pScene->MainCamera.GetScreenCoord(v0, &w0[0], &w0[1], &tmp);
				pEditor->pScene->MainCamera.GetScreenCoord(v1, &w1[0], &w1[1], &tmp);
				pEditor->pScene->MainCamera.GetScreenCoord(v2, &w2[0], &w2[1], &tmp);
				bool inside0 = IsPtInPolygon(w0[0], w0[1], SltRgn2DPtList);
				bool inside1 = IsPtInPolygon(w1[0], w1[1], SltRgn2DPtList);
				bool inside2 = IsPtInPolygon(w2[0], w2[1], SltRgn2DPtList);

				// (v0, v1)�� ��輱�� �����ٸ�,
				if (inside0 && !inside1)
				{
					GPoint3 p = OldVerts[vidx[0]].P;
					GPoint3 q = OldVerts[vidx[1]].P;
					GPoint3 r;

					// (St, Ed)�� (w0, w1)�� �������� ��ǥ�� ���Ͽ� v0�� ��ǥ�� �����Ѵ�.
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

				// (v1, v2)�� ��輱�� �����ٸ�,
				if (inside1 && !inside2)
				{
					GPoint3 p = OldVerts[vidx[1]].P;
					GPoint3 q = OldVerts[vidx[2]].P;
					GPoint3 r;

					// (St, Ed)�� (w1, w2)�� �������� ��ǥ�� ���Ͽ� v1�� ��ǥ�� �����Ѵ�.
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

				// (v2, v0)�� ��輱�� �����ٸ�,
				if (inside2 && !inside0)
				{
					GPoint3 p = OldVerts[vidx[2]].P;
					GPoint3 q = OldVerts[vidx[0]].P;
					GPoint3 r;

					// (St, Ed)�� (w2, w0)�� �������� ��ǥ�� ���Ͽ� v2�� ��ǥ�� �����Ѵ�.
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
			// ���� �ﰢ���� �����Ѵ�.
			pPrevFace = pCurrFace;
		}
	}

	pMesh->ModifiedFlag = MODIFIED_VERT_POS_ALL;
	pMesh->UpdateMesh();

	////////////////////////////////////////////////////////////////////
	// 2 �ܰ�: ���� �������� �� ���� ���ԵǴ� ���̴� �ﰢ���� �����Ѵ�. //
	////////////////////////////////////////////////////////////////////

	// Shift�� Control�� ��� �ϳ��� ������ �ʾҴٸ�,
	if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
		// ���� ���� ��ü ����Ʈ�� �����,
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
		pEditor->EditFaceRgn.clear();
		pEditor->EditBndryPts.clear();
	}

	// ���õ� �ﰢ���� ������ ������ �����ϰ�,
	std::vector<GFace *> SltFaceList;

	// �ü� ���͸� ���ϰ�,
	GVector3 dir = pEditor->pScene->MainCamera.GetViewDirection();
	for (int i = 0; i < pMesh->GetNumFace(); ++i)
	{
		// �� �ﰢ���� ������ ���Ͽ�, �ո��� ���δٸ�,
		GVector3 n = pMesh->Faces[i].GetNormal(false);
		if (angle(dir, n) > 90.0)
		{
			// �ﰢ���� �� ������ ������ ���� ���� �������� ���ԵǴ��� �����Ѵ�.
			double tmp;
			GVector2 w0, w1, w2;
			int *vidx = pMesh->Faces[i].vIdx;
			pEditor->pScene->MainCamera.GetScreenCoord(&(pMesh->Verts[vidx[0]]), &w0[0], &w0[1], &tmp);
			pEditor->pScene->MainCamera.GetScreenCoord(&(pMesh->Verts[vidx[1]]), &w1[0], &w1[1], &tmp);
			pEditor->pScene->MainCamera.GetScreenCoord(&(pMesh->Verts[vidx[2]]), &w2[0], &w2[1], &tmp);
			bool inside0 = IsPtInPolygon(w0[0], w0[1], SltRgn2DPtList);
			bool inside1 = IsPtInPolygon(w1[0], w1[1], SltRgn2DPtList);
			bool inside2 = IsPtInPolygon(w2[0], w2[1], SltRgn2DPtList);

			// �� ���� �� �ּ� �� ���� ���� ���� ���� ���ԵǾ��ٸ�,
			//if ((inside0 && inside1) || (inside1 && inside2) || (inside2 && inside0))
			if (inside0 && inside1 && inside2)
			{
				// ���� ����Ʈ�� �߰��Ѵ�.
				SltFaceList.push_back(&(pMesh->Faces[i]));

				// Quad �޽���� �߰������� ������ �ﰢ���� �߰��Ѵ�.
				if (pMesh->FaceType == FACE_QUAD)
				{
					// ¦����° �ﰢ���� Ȧ����° �ﰢ���� �����Ͽ�,
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

	// Control Ű�� ���� ���, ���� ��ü�� �����ϰ�,
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{			
		for (unsigned i = 0; i < SltFaceList.size(); ++i)
			pEditor->DelFromEditObjList(SltFaceList[i]);
	}
	// �ƴ� ���(�ƹ� Ű�� ������ �ʰų�, Shift Ű�� ���� ���) ��ü�� �߰��Ѵ�.
	else
	{
		for (unsigned i = 0; i < SltFaceList.size(); ++i)
			pEditor->AddToEditObjList(SltFaceList[i]);
	}

	// ���õ� ���� ��ü�� �ߺ��� �����Ѵ�.
	pEditor->UniqueEditObjList();

	// ���� ��ü�� ������ ���� ����ġ�� ����Ѵ�.
	pEditor->FindEditWgt();
}

/*!
*	\brief ���õ� �޽����� �ﰢ�� �׷��� �����Ѵ�.
*
*	\param x ���콺�� x ��ǥ
*	\param y ���콺�� y ��ǥ
*/
void PSelector::SelectFaceGroup(int x, int y)
{
	// ���� ���õ� �޽��� ���ϰ�,
	GMesh *pMesh = pEditor->GetEditMesh();

	// ���õ� ��ü�� ���ٸ�, 
	if (pMesh == NULL)
	{
		// �޽��� �ϳ� �����ϰ� �����Ѵ�.
		SelectMesh(x, y);
		return;
	}

	// Shift�� Control�� ��� �ϳ��� ������ �ʾҴٸ�,
	if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
		// ���� ���� ��ü ����Ʈ�� �����,
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
		pEditor->EditFaceRgn.clear();
		pEditor->EditBndryPts.clear();
	}

	// ���콺 Ŭ���� �̿��� �����̶��,
	if (SltRgn2DPtList[0] == SltRgn2DPtList[1])
	{
		// ���콺 �����Ϳ� ������ �ﰢ�� �׷��� �ִٸ�,
		if (MouseOverGroupIdx != -1)
		{
			// Control Ű�� ���� ���, ���� ��ü�� �����ϰ�,
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				std::map<std::string, std::vector<int>>::iterator it = pMesh->GrpFaces.begin();
				for (int i = 0; i < MouseOverGroupIdx; ++i)
					it++;

				int NumFace = (int)it->second.size();
				for (int i = 0; i < NumFace; ++i)
					pEditor->DelFromEditObjList(&(pMesh->Faces[it->second[i]]));			
			}
			// �ƴ� ���(�ƹ� Ű�� ������ �ʰų�, Shift Ű�� ���� ���) ��ü�� �߰��Ѵ�.
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
		
		// ���õ� ���� ��ü�� �ߺ��� �����Ѵ�.
		pEditor->UniqueEditObjList();
	}
	// ��� ������ �̿��� �޽� �����̶��,
	else
	{
		pMesh->Renderer.RenderType = (pMesh->Renderer.RenderType | RENDER_POLYGON | RENDER_WIRE);

		// �������⿡�� ���̴� �ﰢ�� �׷츸�� �����ϱ� ���ؼ� �ü� ���͸� ���ϰ�,
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
							// Control Ű�� ���� ���, ���� ��ü�� �����ϰ�,
							if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
							{
								for (int k = 0; k < NumFace; ++k)
									pEditor->DelFromEditObjList(&(pMesh->Faces[it->second[k]]));
							}
							// �ƴ� ���(�ƹ� Ű�� ������ �ʰų�, Shift Ű�� ���� ���) ��ü�� �߰��Ѵ�.
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
		// ���õ� ���� ��ü�� �ߺ��� �����Ѵ�.
		pEditor->UniqueEditObjList();
	}

	// ���� ��ü�� ������ ���� ����ġ�� ����Ѵ�.
	pEditor->FindEditWgt();
}

/*!
*	\brief �귯�� ���� �������� �ﰢ���� �����Ѵ�.
*
*	\param pMesh ���õ� ��ü
*/
void PSelector::SelectFaceByBrush(GMesh *pMesh)
{
	// ���� ���콺 �����Ϳ� ������ ���� �ֺ��� �귯�� ���� ������ ����ϰ�,
	std::vector<double> Distances;
	double BrushRadius = pMesh->BBox.GetDiagonal() * (pEditor->EditMeshDlg.m_BrushRadius / 1000.0);
	::get_geodesic_dist_field(pMesh, &(pMesh->Verts[MouseOverVertIdx]), Distances, BrushRadius);

	// ���õ� �ﰢ���� ������ ������ �����ϰ�,
	std::vector<GFace *> SltFaceList;

	// ���õ� �ﰢ���� ���Ͽ�,
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		int vidx0 = pMesh->Faces[i].vIdx[0];
		int vidx1 = pMesh->Faces[i].vIdx[1];
		int vidx2 = pMesh->Faces[i].vIdx[2];
		if (Distances[vidx0] < BrushRadius || Distances[vidx1] < BrushRadius || Distances[vidx2] < BrushRadius)
		{
			// ���� �ﰢ�� ����Ʈ�� �߰��ϰ�,
			SltFaceList.push_back(&(pMesh->Faces[i]));

			// Quad �޽���� �߰������� ������ �ﰢ���� �߰��Ѵ�.
			if (pMesh->FaceType == FACE_QUAD)
			{
				// ¦����° �ﰢ���� Ȧ����° �ﰢ���� �����Ͽ�,
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

	// �ߺ� ���� ��ü�� �����Ѵ�.
	sort(SltFaceList.begin(), SltFaceList.end());
	SltFaceList.erase(unique(SltFaceList.begin(), SltFaceList.end()), SltFaceList.end());

	// Control Ű�� ���� ���, ���� ��ü�� �����ϰ�,
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) 
	{
		for (unsigned i = 0; i < SltFaceList.size(); ++i)
			pEditor->DelFromEditObjList(SltFaceList[i]);
	}
	else	// �ƴ� ���(�ƹ� Ű�� ������ �ʰų�, Shift Ű�� ���� ���) ��ü�� �߰��Ѵ�.
	{
		for (unsigned i = 0; i < SltFaceList.size(); ++i)
			pEditor->AddToEditObjList(SltFaceList[i]);
	}
}

/*!
*	\brief ���õ� �޽�(�ҽ�)�� �������� �����Ѵ�.
*
*	\param x ���콺�� x ��ǥ
*	\param y ���콺�� y ��ǥ
*
*	\return ���ÿ���
*/
bool PSelector::SelectCtrlPt(int x, int y)
{
	// ���� ���õ� �޽��� ���ϰ�,
	GMesh *pMesh = pEditor->GetEditMesh();

	// ���õ� ��ü�� �����ٸ�, 
	if (pMesh == NULL)
	{
		// �޽��� �ϳ� �����ϰ�, false�� �����Ѵ�.
		SelectMesh(x, y);
		return false;
	}

	// ���� ��ü�� Ŭ���� �̸��� ����, 
	std::string ClsName = pMesh->GetClassName();

	// �������� ���� ��ü�� �ƴ϶�� false�� ��ȯ�Ѵ�.
	if (ClsName != "GNurbsCrv" && ClsName != "GNurbsSrf" && ClsName != "GCoonsSrf"
		&& ClsName != "GTriBzrSrf" && ClsName != "GUcbsSrf" && ClsName != "GUcbsVol")
	{
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
		pEditor->EditFaceRgn.clear();
		pEditor->EditBndryPts.clear();
		return false;
	}

	// Shift�� Control�� ��� �ϳ��� ������ �ʾҴٸ�,
	if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
		// ���� ���� ��ü ����Ʈ�� �����,
		pEditor->EditObjList.clear();
		pEditor->EditVertRgn.clear();
	}

	// ������ ����Ʈ�� ���Ͽ�,
	std::vector<GCtrlPt *> CtrlPtLlist;
	pMesh->GetCtrlPtList(CtrlPtLlist);
	pMesh->Renderer.RenderType = pMesh->Renderer.RenderType | RENDER_CTRL_PT;

	// ������ �������� ���Ͽ�,
	int NumCtrlPt = (int)CtrlPtLlist.size();
	for (int j = 0; j < NumCtrlPt; ++j)
	{
		// ��ũ�� ���� ��ǥ�� ���Ͽ�,
		double wx, wy, wz;
		pEditor->pScene->MainCamera.GetScreenCoord(CtrlPtLlist[j], &wx, &wy, &wz);

		// ���� �������� ���Եȴٸ�,
		if (IsPtInPolygon(wx, wy, SltRgn2DPtList))
		{
			// Control Ű�� ���� ���, ���� ��ü�� �����ϰ�,
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
				pEditor->DelFromEditObjList(CtrlPtLlist[j]);
			// �ƴ� ���(�ƹ� Ű�� ������ �ʰų�, Shift Ű�� ���� ���) ��ü�� �߰��Ѵ�.
			else	
				pEditor->AddToEditObjList(CtrlPtLlist[j]);
		}
	}

	// ���õ� ���� ��ü�� �ߺ��� �����Ѵ�.
	pEditor->UniqueEditObjList();

	// ���� ��ü�� ������ true, ������ false�� ��ȯ�Ѵ�.
	if (pEditor->EditObjList.empty())
		return false;
	else
		return true;	
}

/*!
*	\brief ������ ������ ���ο� �����ϴ��� ���θ� �Ǵ�
*	\note ������ ��迡 ��ġ�� ������ false�� ��ȯ�Ѵ�.
*
*	\param x ������ x��ǥ
*	\param y ������ y��ǥ
*	\param PtList �������� �����ϴ� ���� ����Ʈ
*/
bool PSelector::IsPtInPolygon(double x, double y, std::vector<GVector2> &PtList)
{
	// �˰��� ����(Real-time Rendering, 2nd ed., p585)
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
*	\brief ���콺 �����Ϳ� �����ϴ� ���� �ڵ��� ã�´�.
*
*	\param x ���콺�� x��ǥ
*	\param y ���콺�� y��ǥ
*/
bool PSelector::SetMouseOverEditHandle(int x, int y)
{
	// ��ȯ ���� �����ϰ�,
	bool ret = false;

	// ���õ� ���� ��ü�� ���ٸ� flase�� ��ȯ�Ѵ�.
	if (pEditor->GetEditMesh() == NULL)
		return ret;

	// ���� ���콺 ������ ��ġ���� �ȼ��� ���� ���� �о,
	GLubyte pixel[3];
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

	// ���� �ڵ��� ���� �Ǿ��ٸ�,
	if (pixel[0] == 255 && pixel[1] == 255)
	{
		// ���õ� ���� �ڵ��� ������ �����ϰ�,
		ret = true;
		MouseOverGroupIdx = -1;
		MouseOverFaceIdx = -1;
		MouseOverVertIdx = -1;
		MouseOverPtCoords.Set(0.0, 0.0, 0.0);
		pEditor->EditToolType = (TypeEditTool)pixel[2];
	}
	else	// ���� �ڵ��� ���õ��� �ʾҴٸ�, ���� ���� ���� ���·� �����Ѵ�.
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
*	\brief ���� ���콺 �����Ϳ� �����ϴ� �ﰢ��, ���� ���� ������ �����Ѵ�.
*	\note �ֱ� ������(2017/02/07)
*
*	\param x ���콺�� x��ǥ
*	\param y ���콺�� y��ǥ
*
*	\return ���콺�� ������ �ﰢ��, ������ ������ true, ������ false�� ��ȯ�Ѵ�.
*/
bool PSelector::SetMouseOverInfo(int x, int y)
{
	// ���õ� �޽� ��ü�� ���ϰ�,
	GMesh *pMesh = pEditor->GetEditMesh();

	// ���õ� ���� ��ü�� ���ٸ� -1�� �����ϰ� �����Ѵ�.
	if (pMesh == NULL)
	{
		MouseOverGroupIdx = -1;
		MouseOverFaceIdx = -1;
		MouseOverVertIdx = -1;
		MouseOverPtCoords.Set(0.0, 0.0, 0.0);
		return false;
	}
		
	// ���� ���콺 ������ ��ġ���� ���� ������ �ȼ� ���� �о,
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

	// �ﰢ���� ������ �ε���(1, 2, 3, ...)�� ���ϰ�,
	int RenderIdx = pixel[0] * 256 * 256 + pixel[1] * 256 + pixel[2];

	// ���õ� �ﰢ���� ���ٸ�,
	if (RenderIdx < 1 || RenderIdx > pMesh->GetNumFace())
	{
		MouseOverGroupIdx = -1;
		MouseOverFaceIdx = -1;
		MouseOverVertIdx = -1;
		MouseOverPtCoords.Set(0.0, 0.0, 0.0);
		return false;
	}

	MouseOverGroupIdx = 0;
	if (pMesh->GrpFaces.empty())	// �ﰢ�� �׷��� ���ٸ�,
	{
		MouseOverFaceIdx = RenderIdx - 1;
	}
	else	// �ﰢ�� �׷��� �ִٸ�,
	{
		// ���콺�� ��ø�� �ﰢ�� �׷��� �ε����� ���ϰ�,
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
	
	// ���콺�� ��ø�� �ﰢ���� �� ������ ���Ͽ�,
	int vidx0 = pMesh->Faces[MouseOverFaceIdx].vIdx[0];
	int vidx1 = pMesh->Faces[MouseOverFaceIdx].vIdx[1];
	int vidx2 = pMesh->Faces[MouseOverFaceIdx].vIdx[2];
	GVertex *v0 = &(pMesh->Verts[vidx0]);
	GVertex *v1 = &(pMesh->Verts[vidx1]);
	GVertex *v2 = &(pMesh->Verts[vidx2]);

	// �� ������ ������ ��ũ������ ��ǥ�� ���ϰ�,
	GPoint3 p0, p1, p2;
	pEditor->pScene->MainCamera.GetScreenCoord(v0, &p0.V[0], &p0.V[1], &p0.V[2]);
	pEditor->pScene->MainCamera.GetScreenCoord(v1, &p1.V[0], &p1.V[1], &p1.V[2]);
	pEditor->pScene->MainCamera.GetScreenCoord(v2, &p2.V[0], &p2.V[1], &p2.V[2]);

	// ���� ���콺 ��ġ�� �Ÿ��� ����Ͽ�, 
	double dist0 = dist(GPoint3(x, y, 0.0), p0);
	double dist1 = dist(GPoint3(x, y, 0.0), p1);
	double dist2 = dist(GPoint3(x, y, 0.0), p2);

	// ���콺 �����Ϳ� ���� ����� ������ �ε����� ã�´�.
	double min_dist = MIN(dist0, MIN(dist1, dist2));
	if (min_dist == dist0)
		MouseOverVertIdx = vidx0;
	else if (min_dist == dist1)
		MouseOverVertIdx = vidx1;
	else
		MouseOverVertIdx = vidx2;

	// 2D Baricentric ��ǥ�� ����Ѵ�.
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
*	\brief ����Ʈ ������
*/
PEditor::PEditor()
{	
	// �⺻ ������ ���(13)
	EC = GTransf();							// ���� ���� ���� ��ǥ�� (��ü ���ý� ������Ʈ)
	EditToolType = EDIT_INACTIVE;			// ���� �� ����(�̵�, ȸ��, ������ ��)
	EditObjList.clear();					// ���õ� ���� ��ü ����Ʈ
	EditVertRgn.clear();					// �ε巯�� ������ ���� �߰��� ���õ� ���� ����Ʈ
	EditFaceRgn.clear();					// �ε巯�� ������ ���� �߰��� ���õ� �ﰢ�� ����Ʈ(������ ��)
	pEditFaceRgnBvh = NULL;					// �ε巯�� ������ ���� �߰��� ���õ� �ﰢ�� ����Ʈ�� BVH
	EditBndryPts.clear();					// ���� ������ ��� ������
	pUnfoldMesh = NULL;						// 2�������� ������ ���� ����
	UnfoldFaceMap.clear();					// 2���� �Ű�ȭ �޽��� 3���� ���� �������� �ﰢ�� ���� ����
	MousePt[0] = CPoint(0, 0);				// ���� �� ������ ���� ���콺�� ��ġ
	MousePt[1] = CPoint(0, 0);				// ���� �� ������ ���� ���콺�� ��ġ
	RotRefX = GTransf();					// ȸ�� ������ �� ���, ���� ȸ�� �ڵ��� ���õ� ���� ��ǥ��
	RotRefPos = GVector3();					// ȸ�� ������ �� ���, ȸ�� �ڵ��� ���õ� ����
	
	// �߰� ������ ���(2)
	pScene = NULL;					// ���� ���α׷��� ��鿡 ���� ������
	theSelector.pEditor = this;		// ���� ��ü ������ ���� ��ü
}

/*!
*	\brief ���� ������
*
*	\param cpy ����� ��ü
*/
PEditor::PEditor(const PEditor &cpy)
{
	// �⺻ ������ ���(13)
	EC = cpy.EC;							// ���� ���� ���� ��ǥ�� (��ü ���ý� ������Ʈ)
	EditToolType = cpy.EditToolType;		// ���� �� ����(�̵�, ȸ��, ������ ��)
	EditObjList = cpy.EditObjList;			// ���õ� ���� ��ü ����Ʈ
	EditVertRgn = cpy.EditVertRgn;			// �ε巯�� ������ ���� �߰��� ���õ� ���� ��ü ����Ʈ
	EditFaceRgn = cpy.EditFaceRgn;			// ������ ������ ���Ե� �ﰢ����(������ ��)
	pEditFaceRgnBvh = NULL;					// �ε巯�� ������ ���� �߰��� ���õ� �ﰢ�� ����Ʈ�� BVH
	EditBndryPts = cpy.EditBndryPts;		// ���� ������ ��� ������
	pUnfoldMesh = NULL;						// 2�������� ������ ���� ����
	UnfoldFaceMap.clear();					// 2���� �Ű�ȭ �޽��� 3���� ���� �������� �ﰢ�� ���� ����
	MousePt[0] = cpy.MousePt[0];			// ���� �� ������ ���� ���콺�� ��ġ
	MousePt[1] = cpy.MousePt[1];			// ���� �� ������ ���� ���콺�� ��ġ
	RotRefX = cpy.RotRefX;					// ȸ�� ������ �� ���, ���� ȸ�� �ڵ��� ���õ� ���� ��ǥ��
	RotRefPos = cpy.RotRefPos;				// ȸ�� ������ �� ���, ȸ�� �ڵ��� ���õ� ����
	
	// �߰� ������ ���(2)
	pScene = cpy.pScene; 							// ���� ���α׷��� ��鿡 ���� ������
	theSelector.pEditor = cpy.theSelector.pEditor;	// ���� ��ü ������ ���� ��ü
}

/*!
*	\brief �Ҹ���
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
*	\brief ���� ������
*
*	\param rhs ���Ե� ��ü
*/
PEditor &PEditor::operator =(const PEditor &rhs)
{
	// �⺻ ������ ���(11)
	EC = rhs.EC;							// ���� ���� ���� ��ǥ�� (��ü ���ý� ������Ʈ)
	EditToolType = rhs.EditToolType;		// ���� �� ����(�̵�, ȸ��, ������ ��)
	EditObjList = rhs.EditObjList;			// ���õ� ���� ��ü ����Ʈ
	EditVertRgn = rhs.EditVertRgn;			// �ε巯�� ������ ���� �߰��� ���õ� ���� ��ü ����Ʈ
	EditFaceRgn = rhs.EditFaceRgn;			// ������ ������ ���Ե� �ﰢ����(������ ��)
	pEditFaceRgnBvh = NULL;					// �ε巯�� ������ ���� �߰��� ���õ� �ﰢ�� ����Ʈ�� BVH
	EditBndryPts = rhs.EditBndryPts;		// ���� ������ ��� ������
	pUnfoldMesh = NULL;						// 2�������� ������ ���� ����
	UnfoldFaceMap.clear();					// 2���� �Ű�ȭ �޽��� 3���� ���� �������� �ﰢ�� ���� ����
	MousePt[0] = rhs.MousePt[0];			// ���� �� ������ ���� ���콺�� ��ġ
	MousePt[1] = rhs.MousePt[1];			// ���� �� ������ ���� ���콺�� ��ġ
	RotRefX = rhs.RotRefX;					// ȸ�� ������ �� ���, ���� ȸ�� �ڵ��� ���õ� ���� ��ǥ��
	RotRefPos = rhs.RotRefPos;				// ȸ�� ������ �� ���, ȸ�� �ڵ��� ���õ� ����
	
	// �߰� ������ ���(2)
	pScene = rhs.pScene; 							// ���� ���α׷��� ��鿡 ���� ������
	theSelector.pEditor = rhs.theSelector.pEditor;	// ���� ��ü ������ ���� ��ü	
	
	return *this;
}

/*!
*	\brief ��Ƽ���� ��鿡 ���� �����͸� �����Ѵ�.
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
*	\brief ���� ��ü, ������ �׸��� ���� ������ ������ �Ѵ�.
*/
void PEditor::Render()
{
	GMesh *pMesh = GetEditMesh();
	if (pMesh != NULL)
	{
		// ���� ��ǥ�迡��
		glPushMatrix();	 
		{
			// �ε巯�� ���� ������ ������ �Ѵ�.
			RenderEditRgn();

			// ���õ� ��ü�� ǥ�� ��� ���ڸ� �������ϰ�,
			pMesh->RenderForEdit(1.0, 0.0, 0.0);

			// ���õ� ��ü�� ������ ���ϰ�,
			int NumObj = (int)EditObjList.size();

			// ������ ���õ� ��ü�� ���Ͽ�,
			for (int i = 0; i < NumObj; ++i)
			{
				// ���� ǥ�� ��� ���ڸ� ������ �Ѵ�.
				if (i == 0 && EditObjList[i])
					EditObjList[i]->RenderForEdit(1.0, 0.0, 0.0);
				else
					EditObjList[i]->RenderForEdit(1.0, 1.0, 1.0);
			}			

			// ���� ���� ���� ��ǥ��� ��ȯ�Ͽ�,
			GTransf EC = GetEC();
			glMultMatrixd(cast_mat(EC).M);

			// ���� ���� ������ ����, �ش� ���� �������ϰ�,
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
		glPopMatrix(); // ���� ��ǥ��� �����Ѵ�.
	}	
		
	// ��ü ���� ���̶��, ���� ������ ���� ���� ������ ������ �Ѵ�.
	theSelector.Render();
}

/*!
*	\brief ���� ������ ������ �Ѵ�.
*/
void PEditor::RenderEditRgn()
{
	// ���� �޽��� ���ϰ� ������ �����Ѵ�.
	GMesh *pMesh = GetEditMesh();
	if (pMesh == NULL)
		return;

	// ���� ��ǥ�迡��
	glPushMatrix();
	{
		// ���� �޽��� �𵨸� ��ǥ��� ��ȯ�ϰ�,
		glMultMatrixd(cast_mat(pMesh->MC).M);

		// ������ ��Ȱ��ȭ �ϰ�, �����°� ������ Ȱ��ȭ �ϰ�,
		glDisable(GL_LIGHTING);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// ���� ������ �ﰢ���� ������ �Ѵ�.
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

		// ���� ������ ��踦 �������Ѵ�.
		glEnable(GL_LINE_SMOOTH);

		// �ٻ�� ���� ��踦 ǥ���Ѵٸ�
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
		else	// ��Ȯ�� ���� ��踦 ǥ���Ѵٸ�,	
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
		
		// ������ ��Ȱ��ȭ�ϰ� ����� ���� �׽�Ʈ�� Ȱ��ȭ�Ѵ�.
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_BLEND);
		glDisable(GL_LINE_SMOOTH);
		glEnable(GL_LIGHTING);
	}
	glPopMatrix();	// ���� ��ǥ��� �����Ѵ�.

	// ������ ���� ������ ǥ���Ѵ�.
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
*	\brief �������� �̵��ڵ��� ������ �Ѵ�.
*/
void PEditor::RenderTra(double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� �̵��ڵ��� ������ �Ѵ�.
*
*	\param len �̵� �ڵ��� ���� 
*/
void PEditor::RenderTraHandleX(double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� �̵��ڵ��� ������ �Ѵ�.
*
*	\param len �̵� �ڵ��� ���� 
*/
void PEditor::RenderTraHandleY(double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� �̵��ڵ��� ������ �Ѵ�.
*
*	\param len �̵� �ڵ��� ����
*/
void PEditor::RenderTraHandleZ(double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� �̵��ڵ��� ������ �Ѵ�.
*
*	\param len �̵� �ڵ��� ����
*/
void PEditor::RenderTraHandleXY(double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� �̵��ڵ��� ������ �Ѵ�.
*
*	\param len �̵� �ڵ��� ����
*/
void PEditor::RenderTraHandleYZ(double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� �̵��ڵ��� ������ �Ѵ�.
*
*	\param len �̵� �ڵ��� ����
*/
void PEditor::RenderTraHandleZX(double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� �̵��ڵ��� ������ �Ѵ�.
*
*	\param len �̵� �ڵ��� ����
*/
void PEditor::RenderTraHandleXYZ(double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� ȸ�� �ڵ��� ������ �Ѵ�.
*
*	\param len ȸ�� �ڵ��� ����
*/
void PEditor::RenderRot(double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
	double modelview[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

	// ���� ������ �׵θ�
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

	// ���������� �׵θ� ���� ������ �����ϱ� ���� ���� 
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
*	\brief �������� ȸ�� �ڵ��� ������ �Ѵ�.
*
*	\param view_axis ���� �ڵ��� ����� ���� ���� ����
*	\param len ȸ�� �ڵ��� ����
*/
void PEditor::RenderRotHandleX(const GVector3 &view_axis, double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� ȸ�� �ڵ��� ������ �Ѵ�.
*
*	\param view_axis ���� �ڵ��� ����� ���� ���� ����
*	\param len ȸ�� �ڵ��� ����
*/
void PEditor::RenderRotHandleY(const GVector3 &view_axis, double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� ȸ�� �ڵ��� ������ �Ѵ�.
*
*	\param view_axis ���� �ڵ��� ����� ���� ���� ����
*	\param len ȸ�� �ڵ��� ����
*/
void PEditor::RenderRotHandleZ(const GVector3 &view_axis, double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� ������ �ڵ��� ������ �Ѵ�.
*
*	\param len ������ �ڵ��� ����
*/
void PEditor::RenderScl(double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� ������ �ڵ��� ������ �Ѵ�.
*
*	\param len ������ �ڵ��� ����
*/
void PEditor::RenderSclHandleX(double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� ������ �ڵ��� ������ �Ѵ�.
*
*	\param len ������ �ڵ��� ����
*/
void PEditor::RenderSclHandleY(double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� ������ �ڵ��� ������ �Ѵ�.
*
*	\param len ������ �ڵ��� ����
*/
void PEditor::RenderSclHandleZ(double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� ������ �ڵ��� ������ �Ѵ�.
*
*	\param len �̵� �ڵ��� ����
*/
void PEditor::RenderSclHandleXY(double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� ������ �ڵ��� ������ �Ѵ�.
*
*	\param len �̵� �ڵ��� ����
*/
void PEditor::RenderSclHandleYZ(double len)
{

	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� ������ �ڵ��� ������ �Ѵ�.
*
*	\param len �̵� �ڵ��� ����
*/
void PEditor::RenderSclHandleZX(double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� ������ �ڵ��� ������ �Ѵ�.
*
*	\param len ������ �ڵ��� ����
*/
void PEditor::RenderSclHandleXYZ(double len)
{
	// ���� ��ǥ��: ���� ���� ��ǥ��
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
*	\brief �������� �ڵ� �����ϰų�, ��ü ������ ���� �غ� �۾��� �Ѵ�.
*
*	\param x ���콺�� X ��ǥ
*	\param y ���콺�� Y ��ǥ
*	\param nFlags ���콺 ���� ��ư�� ���� ���� �߰� ���� (MK_CONTROL...)
*/
void PEditor::LButtonDown(int x, int y, UINT nFlags)
{
	// ������ ���ؼ� ���콺 ��ġ �����ϰ�,
	MousePt[0] = MousePt[1] = CPoint(x, y);

	// ���� ��(�̵�, ȸ��, ������)�� �ڵ��� �����ϰ�,
	bool bEditStart = SelectEditHandle(x, y);

	// ���� ���� ���õ��� �ʰ�, ���� ��θ� �����ϴ� ��찡 �ƴ϶��
	if (!bEditStart && EditToolType != EDIT_GEODESIC_PATH && EditToolType != EDIT_SCULPT && EditToolType != EDIT_SMOOTH)
		theSelector.LButtonDown(x, y, nFlags);	// ��ü ������ �غ��Ѵ�.
}

/*!
*	\brief ���� ��ü�� �����ϰų�, ���� ������ �����Ѵ�.
*
*	\param x ���콺�� X ��ǥ
*	\param y ���콺�� Y ��ǥ
*	\param nFlags ���콺�� ������ ��, ������ ��ư�� ���� (MK_LBUTTON, MK_RBUTTON, MK_MBUTTON)
*/
void PEditor::MouseMove(int x, int y, UINT nFlags)
{
	// ī�޶� ��ǥ�迡�� ǥ���� ���콺�� ������ ����ϰ�,
	double dx = (x - MousePt[1].x);
	double dy = -(y - MousePt[1].y);
	double dz = 0.0;
	
	// ���콺 ���� ��ġ�� �����Ѵ�.
	MousePt[1] = CPoint(x, y);

	// ���� ���콺 ��ư�� ���� ���¶�� ���� ��ü�� �����ϰ� �����Ѵ�.
	if (nFlags & MK_LBUTTON)
	{
		// ��ü�� �̵� ������ �Ǿ��ٸ� �����Ѵ�.
		if (EditTra(dx, dy, dz))
			return;

		// ��ü�� ȸ�� ������ �Ǿ��ٸ� �����Ѵ�.
		if (EditRot(dx, dy, dz))
			return;

		// ��ü�� ������ ������ �Ǿ��ٸ� �����Ѵ�.
		if (EditScl(dx, dy, dz))
			return;

		if (EditGeodesicPath(x, y))
			return;

		if (EditSculpt(x, y))
			return;

		if (EditSmooth(x, y))
			return;
	}
	
	// ��ü�� ������ �̷������ ���� ��� �Ǵ� ���콺 ���� ��ư�� �������� ���� ���
	// ���� ������ �����Ѵ�.
	theSelector.MouseMove(x, y, nFlags);			
}

/*!
*	\brief ��ü�� �̵� �����Ѵ�.
*
*	\param dx X �� �̵�����
*	\param dy Y �� �̵�����
*	\param dz Z �� �̵�����
*
*	\return ������ �� ��� true, ������ ���� ���� �ܿ� false�� ��ȯ�Ѵ�.
*/
bool PEditor::EditTra(double dx, double dy, double dz)
{
	// ���� ��ǥ�迡�� ǥ���� ������ ���Ͽ�,
	double scale = -(pScene->MainCamera.ZoomDist * 0.0005);
	GVector3 t = inv(pScene->GetWC() * EC) * GVector3(dx, dy, dz) * scale;

	// ���� ���� �ڵ��� ���õ� ���,
	switch (EditToolType)
	{
		// ��ü �̵��� ���� �� �����Ѵ�.
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
*	\brief ��ü�� ȸ�� �����Ѵ�.
*
*	\param dx X �� �̵�����
*	\param dy Y �� �̵�����
*	\param dz Z �� �̵�����
*
*	\return ������ �� ��� true, ������ ���� ���� �ܿ� false�� ��ȯ�Ѵ�.
*/
bool PEditor::EditRot(double dx, double dy, double dz)
{
	// ���� ���� �ڵ��� ���õ� ���,
	switch (EditToolType)
	{
		// ��ü ȸ���� ���� �� �����Ѵ�.
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
*	\brief ��ü�� ������ �����Ѵ�.
*
*	\param dx X �� �̵�����
*	\param dy Y �� �̵�����
*	\param dz Z �� �̵�����
*
*	\return ������ �� ��� true, ������ ���� ���� �ܿ� false�� ��ȯ�Ѵ�.
*/
bool PEditor::EditScl(double dx, double dy, double dz)
{
	// ���� ��ǥ�迡�� ǥ���� ������ ���Ͽ�,
	double scale = -(pScene->MainCamera.ZoomDist * 0.0005);
	GVector3 t = inv(pScene->GetWC() * EC) * GVector3(dx, dy, dz) * scale;

	// ���� ���� �ڵ��� ���õ� ���,
	switch (EditToolType)
	{
		// ��ü ������ ���� �� �����Ѵ�.
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
*	\brief ��ü���� ���ǵ� ���� ��θ� �����Ѵ�.
*
*	\param x ���콺�� x ��ǥ
*	\param y ���콺�� y ��ǥ
*
*	\return ������ �� ��� true, ������ ���� ���� �ܿ� false�� ��ȯ�Ѵ�.
*/
bool PEditor::EditGeodesicPath(int x, int y)
{
	if (EditToolType != EDIT_GEODESIC_PATH)
		return false;

	// ���콺�� ������ �ﰢ��, ������ ������ �����ϰ�,
	theSelector.SetMouseOverInfo(x, y);

	//  ���õ� ������ �ִٸ�,
	if (theSelector.MouseOverVertIdx != -1)
	{
		GMesh *pMesh = GetEditMesh();
		std::vector<GMesh *> MeshList;
		GetPreOrderMeshList(pScene, MeshList, pMesh);

		// ���� ���� ���� ��� �޽��� ã��,
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

		// ���� ���� ��� �޽��� �ִٸ� ��θ� �����Ѵ�.
		if (pPath != NULL)
		{
			pPath->EditPath(theSelector.MouseOverVertIdx);
			return true;
		}
	}

	return false;
}

/*!
*	\brief �޽��� Sculpting �Ѵ�.
*
*	\param x ���콺�� x ��ǥ
*	\param y ���콺�� y ��ǥ
*
*	\return ������ �� ��� true, ������ ���� ���� �ܿ� false�� ��ȯ�Ѵ�.
*/
bool PEditor::EditSculpt(int x, int y)
{
	if (EditToolType != EDIT_SCULPT)
		return false;
	
	// ���콺�� ������ �ﰢ��, ������ ����ã�� ���ٸ� �����Ѵ�.
	if (!theSelector.SetMouseOverInfo(x, y))
		return false;

	// Sclupting �� ���� ���� ���� �� ����ġ�� ����ϰ�,
	FindEditWgtForSculpt();

	GMesh *pEditMesh = GetEditMesh();
	if (!EditVertRgn.empty())
	{
		// ���� ������ ���Ͽ�,
		double wgt = pEditMesh->BBox.GetDiagonal() * 0.0003;

		// ���õ� �ﰢ���� ������ ���Ͽ�,
		int *vidx = pEditMesh->Faces[theSelector.MouseOverFaceIdx].vIdx;
		GVector3 n0 = pEditMesh->Verts[vidx[0]].N;
		GVector3 n1 = pEditMesh->Verts[vidx[1]].N;
		GVector3 n2 = pEditMesh->Verts[vidx[2]].N;
		GVector3 C = theSelector.MouseOverPtCoords;
		GVector3 N = n0 * C[0] + n1 * C[1] + n2 * C[2];
		N.Normalize();
		GVector3 EditDir = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? -N : N;

		// �ֺ� ������ �����Ѵ�.
		for (unsigned i = 0; i < EditVertRgn.size(); ++i)
		{
			EditVertRgn[i]->P = EditVertRgn[i]->P + EditDir * EditVertRgn[i]->EditWgt * wgt;
			EditVertRgn[i]->N.Set(0, 0, 0);
		}

		// ������ �����Ѵ�.
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

		// ��� ���ڿ� ���� ���۸� �����Ѵ�.
		pEditMesh->UpdateBndBox(false);
		pEditMesh->UpdateBufVertex();
		pEditMesh->UpdateBufNormal();
	}

	return true;
}

/*!
*	\brief �޽��� Smoothing �Ѵ�.
*
*	\param x ���콺�� x ��ǥ
*	\param y ���콺�� y ��ǥ
*
*	\return ������ �� ��� true, ������ ���� ���� �ܿ� false�� ��ȯ�Ѵ�.
*/
bool PEditor::EditSmooth(int x, int y)
{
	if (EditToolType != EDIT_SMOOTH)
		return false;

	// ���콺�� ������ �ﰢ��, ������ ����ã�� ���ٸ� �����Ѵ�.
	if (!theSelector.SetMouseOverInfo(x, y))
		return false;

	// Smoothing �� ���� ���� ���� �� ����ġ�� ����ϰ�,
	FindEditWgtForSculpt();

	GMesh *pEditMesh = GetEditMesh();
	if (!EditVertRgn.empty())
	{
		int NumVert = pEditMesh->GetNumVert();
		std::vector<int> Deg;
		Deg.assign(NumVert, 0);

		std::vector<GVector3> COG;
		COG.assign(NumVert, GVector3(0.0, 0.0, 0.0));

		// ������ �����Ѵ�.
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

		// ������ �����Ѵ�.
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

		// ��� ���ڿ� ���� ���۸� �����Ѵ�.
		pEditMesh->UpdateBndBox(false);
		pEditMesh->UpdateBufVertex();
		pEditMesh->UpdateBufNormal();
	}
	
	return true;
}

/*!
*	\brief ��ü ������ �����ϰų�, ���ο� ��ü�� �����Ѵ�.
*
*	\param x ���콺�� X ��ǥ
*	\param y ���콺�� Y ��ǥ
*	\param nFlags ���콺 ���� ��ư�� ������ ���� �߰� ���� (MK_CONTROL...)
*/
void PEditor::LButtonUp(int x, int y, UINT nFlags)
{
	// ���� ���� �ڵ��� ���� �� ���,
	switch (EditToolType)
	{
		// ��ü �̵��� �����ϰ� �����Ѵ�.
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

		// ��ü ȸ���� �����ϰ� �����Ѵ�.
	case EDIT_ROT_X:
	case EDIT_ROT_Y:
	case EDIT_ROT_Z:
		EditFinish();
		EditToolType = EDIT_ROT;
		return;

		// ��ü �������� �����ϰ� �����Ѵ�.
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

		// ������ �����̶��,
	case EDIT_GEODESIC_PATH:
		return;

		// Sculpting, smoothing �����̶��,
	case EDIT_SCULPT:
	case EDIT_SMOOTH:
		EditFinish();
		return;
	}
	
	// ���� ���� �ڵ��� ���õ��� ���� ���, ���ο� ��ü�� �����Ѵ�.
	theSelector.LButtonUp(x, y, nFlags);
}

/*!
*	\brief ���� ������ ũ�⸦ �����Ѵ�.
*
*	\param delta ���콺 ���� ����
*/
void PEditor::MouseWheel(int delta)
{
	// ���� �޽��� ���Ͽ� ���ٸ� �����Ѵ�.
	GMesh *pMesh = GetEditMesh();
	if (pMesh == NULL)
		return;

	// ���� ������ Ȯ���ϴ� �����,
	if (delta > 0)
	{
		EditMeshDlg.m_EditRgnDist += 10;
		if (EditMeshDlg.m_EditRgnDist > 1000)
			EditMeshDlg.m_EditRgnDist = 1000;
	}
	else	// ���� ������ ����ϴ� �����,
	{
		EditMeshDlg.m_EditRgnDist -= 10;
		if (EditMeshDlg.m_EditRgnDist < 0)
			EditMeshDlg.m_EditRgnDist = 0;		
	}

	// ����� ������ ���� ��Ʈ�ѿ� �ݿ��Ѵ�.
	EditMeshDlg.m_cEditRgnDist.SetPos(EditMeshDlg.m_EditRgnDist);

	// ���� ���� ���� sculpt �Ǵ� smooth Ÿ���̶��,
	if (EditToolType == EDIT_SCULPT || EditToolType == EDIT_SMOOTH)
		FindEditWgtForSculpt();
	else
		FindEditWgt();

	// ����� �������� �����Ѵ�.
	EditMeshDlg.pDoc->UpdateAllViews(NULL);
}

/*!
*	\brief ���õ� ��ü�� �� ù ��° ��ü�� ���� �޽��� ��ȯ�Ѵ�.
*
*	\return ���õ� ��ü�� �� ù ��° ��ü�� ���� �޽��� ��ȯ�Ѵ�.
*/
GMesh *PEditor::GetEditMesh()
{
	if (EditObjList.empty())
		return NULL;
	else
		return EditObjList[0]->GetRefMesh();
}

/*!
*	\brief ���� ��ü�� Ŭ���� �̸��� ��ȯ�Ѵ�.
*
*	\return ���� ��ü�� Ŭ���� �̸�
*/
std::string PEditor::GetClsNameEditObj()
{
	std::string name;
	if (!EditObjList.empty())
		name = EditObjList[0]->GetClassName();

	return name;
}

/*!
*	\brief ���� ��ü�� ������ ��ȯ�Ѵ�.
*
*	\return ���� ��ü�� ����
*/
int PEditor::GetNumEditObj()
{
	int NumEditObj = (int)EditObjList.size();

	return NumEditObj;
}

/*!
*	\brief ���õ� ��ü���� �����ϴ� �޽� ��ü�� �����Ͽ� ����Ʈ�� �����Ѵ�.
*
*	\param MeshList �޽� ��ü�� ����� ����Ʈ
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
*	\brief ���� ��ü�� �߰��Ѵ�.
*
*	\param pObj �߰� �Ǵ� ������ ��ü
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

	// ������ �߰��ϰ�, ���߿� �ʿ��� ���, UniqueEditObjList() �Լ��� ȣ���Ͽ� �ϰ������� �ߺ� �����Ѵ�.
	EditObjList.push_back(pObj);
}

/*!
*	\brief ���� ��ü�� �����Ѵ�.
*
*	\param pObj ������ ��ü
*
*	\return ������ ��� true, ������ ��ü�� ���� ��� false�� ��ȯ�Ѵ�.
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
*	\brief ��ü�� ������ ����Ѵ�.
*
*	\param pObj ����� ��ü
*/
void PEditor::ToggleEditObject(GEditObj *pObj)
{
	bool bErased = false;

	// ��� ���� ��ü�� ���Ͽ�,
	std::vector<GEditObj *>::iterator it;
	for (it = EditObjList.begin(); it != EditObjList.end(); ++it)
	{
		// pObj�� �̹� ���õ� ���¶��,
		if (*it == pObj)
		{
			// pObj�� ������ �����ϰ�,
			EditObjList.erase(it);
			bErased = true;
			break;
		}
	}

	// ���ο� ��ü��� ���� ����Ʈ�� �߰��Ѵ�.
	if (!bErased)
		EditObjList.push_back(pObj);
}

/*!
*	\brief ���õ� ���� ��ü����Ʈ�� �ߺ��� �����Ѵ�.
*	\warning ���õ� ��ü�� ����Ʈ�� �ߺ��� ���ŵ����� ���ĵȴٴ� ������ �ִ�.
*/
void PEditor::UniqueEditObjList()
{
	sort(EditObjList.begin(), EditObjList.end());
	EditObjList.erase(unique(EditObjList.begin(), EditObjList.end()), EditObjList.end());
}

/*!
*	\brief ���� ���� �ڵ��� �����Ѵ�.
*
*	\param x ���콺�� X ��ǥ
*	\param y ���콺�� Y ��ǥ
*/
bool PEditor::SelectEditHandle(int x, int y)
{
	// �̵�/������ ���� ���� �ڵ�(X, Y, Z)�� ���� �Ǿ��ٸ� true�� �����Ѵ�.
	if ((EditToolType >= EDIT_TRA_X && EditToolType <= EDIT_TRA_XYZ) || (EditToolType >= EDIT_SCL_X && EditToolType <= EDIT_SCL_XYZ))
		return true;

	// ��ȯ ���� �ʱ�ȭ �ϰ�,
	bool ret = false;

	// ȸ�� ���� ���� �ڵ�(X, Y, Z)�� ���õǾ��ٸ�,
	if (EditToolType >= EDIT_ROT_X && EditToolType <= EDIT_ROT_Z)
	{
		// ���� ��ǥ����� ��ȯ�� ���ϰ�,
		GTransf X = pScene->GetWC() * EC;

		// ���� ��ǥ�迡�� ǥ���� �� ���� ���͸� ���ϰ�,
		GVector3 view_axis(0.0, 0.0, 1.0);
		view_axis = inv(X) * view_axis;

		// ���� ���� �����Ͽ�,
		theSelector.EnterSelectMode(x, y);

		// ���� ��ǥ��� ��ȯ�ϰ�,
		glMultMatrixd(cast_mat(EC).M);

		// ���� ���õ� ȸ�� ���� �ڵ鿡 ����,
		int snum = 100;
		double len = GetEditToolSize();
		switch (EditToolType)
		{
			// X �� ȸ�� �ڵ��� ���õ� ���,
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

			// Y �� ȸ�� �ڵ��� ���õ� ���,
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

			// Z �� ȸ�� �ڵ��� ���õ� ���,
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

		// ������ ���� �����Ͽ�, 
		GLint hits = theSelector.ExitSelectMode();

		// ���õ� ��ü�� �ִٸ�,
		if (hits > 0)
		{
			// ���� ���� ���� ��ǥ�踦 �����ϰ�,
			ret = true;
			RotRefX = X;
			double theta = (double)(theSelector.SelectBuf[3]) / (double)snum * M_PI * 2.0;

			// ���� ���õ� ȸ�� ���� �ڵ鿡 ���� ���� ������ �����Ѵ�.
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

	// ��� ���� ��ȯ�Ѵ�.
	return ret;
}

/*!
*	\brief ���õ� ��ü��κ��� �������� ���� ��ǥ�踦 �����Ѵ�.
*/
void PEditor::SetEC()
{
	// ���õ� ��ü�� ���ٸ�, 
	if (EditObjList.empty())
	{
		EC = GTransf();
		return;
	}

	// ���� ��ü�� �̸��� ���Ͽ�,
	GMesh *pMesh = GetEditMesh();
	std::string ClsName = GetClsNameEditObj();
	GVector3 C;

	// ���� ������ ���õǾ��ٸ�,
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
	else if (ClsName == "GFace")	// �ﰢ���� ���õǾ��ٸ�,
	{
		for (unsigned i = 0; i < EditObjList.size(); ++i)
		{
			GFace *f = (GFace *)EditObjList[i];
			C = C + cast_vec3(pMesh->MC * f->GetCOG(true));
		}
		C = C / (int)EditObjList.size();
		EC = GTransf(C, pMesh->MC.q);
	}
	else if (ClsName == "GCtrlPt")	// �������� ���õǾ��ٸ�,
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
	else	// ����, �ﰢ��, ������ �̿��� ��ü�� ���õǾ��ٸ�,
	{
		// ù ��° ���� ��ü�� ��ȯ���� �����Ѵ�.
		EC = EditObjList[0]->GetTransf();
	}	
}

/*!
*	\brief ���� ���� ũ�⸦ ����Ѵ�.
*
*	\return ���� ���� ũ�⸦ ��ȯ�Ѵ�.
*/
double PEditor::GetEditToolSize()
{
	// ī�޶� ��ǥ�迡�� ���� ��ǥ����� ��ȯ�� ���ϰ�,
	GTransf X = pScene->GetWC() * EC;

	// ���� ��ǥ���� ���������� ������ 5%�� ���Ͽ� �����Ѵ�.
	double len = norm(X.t) * 0.05;

	//std::cout << "Edit Tool Size: " << len << std::endl;
	return len;
}

/*!
*	\brief ��Ƽ���� ���� ��ǥ�踦 ��ȯ�Ѵ�.
*/
GTransf PEditor::GetEC()
{
	return EC;
}

/*!
*	\brief ���� ��ü�� ���� ����ġ�� ����Ѵ�.
*	\note ȣ����� FindEditWgt() -> FindEditRgn() -> FindEditBndry()
*/
void PEditor::FindEditWgt()
{
	// ���� �޽��� ���ٸ� �����ϰ�,
	GMesh *pMesh = GetEditMesh();
	if (pMesh == NULL)
		return;

	// ���� ��ü�� ������ �ƴϰ� �ﰢ���� �ƴ϶�� �����Ѵ�.
	std::string ClsName = GetClsNameEditObj();
	if (ClsName != "GVertex" && ClsName != "GFace")
		return;

	double t = EditMeshDlg.m_EditRgnDist / 1000.0;
	// ���� ������ �����Ǿ��ٸ�,
	if (t > 0.0)
	{
		// ���� �޽� ������ ���� ����ġ�� �ʱ�ȭ �Ѵ�.
		int NumVert = pMesh->GetNumVert();
		for (int i = 0; i < NumVert; ++i)
			pMesh->Verts[i].EditWgt = 0.0;

		// ���� �Ÿ��� ���ϰ�,
		double EditDist = pMesh->BBox.GetDiagonal() * t;

		// �ٻ� ���� ��� ��̶��,
		if (EditMeshDlg.m_ShowEditBndryType == 0)
		{
			// ���� �Ÿ���, ���� ����, �ٻ� ��輱�� ���Ѵ�.
			std::vector<double> Distances;
			get_geodesic_dist_field(pMesh, EditObjList, Distances, EditDist);
			FindEditRgn(pMesh, Distances, EditDist);
			FindEditBndry(pMesh, Distances, EditDist);			
		}
		else	// ��Ȯ�� ���� ��� ��̶��,
		{
			// ���� ��ǥ, ���� ����, ��輱�� ���Ѵ�.
			std::vector<double> Distances;
			std::vector<double> Angles;
			//double scale = (t < 0.1) ? 2.0 : 1.2;
			double scale = 1.2;
			get_geodesic_polar_coords(pMesh, (GVertex *)EditObjList[0], Distances, Angles, EditDist * scale);
			FindEditRgn(pMesh, Distances, EditDist);
			FindEditBndry(pMesh, Distances, Angles, EditDist);			
		}
	}
	else	// ���� ������ �������� �ʾҴٸ�, 
	{
		// ���� ������ Ȯ��� ���� ��ü����Ʈ�� �ʱ�ȭ�Ѵ�.
		EditFaceRgn.clear();
		EditVertRgn.clear();
		EditBndryPts.clear();
		
		// ���� �޽��� ������ ������ ���ϰ�,
		int NumVert = pMesh->GetNumVert();
		for (int i = 0; i < NumVert; ++i)
			pMesh->Verts[i].EditWgt = 1.0;		
	}
}

/*!
*	\brief ���� ��ü�� ���� ����ġ�� ����Ѵ�.
*/
void PEditor::FindEditWgtForSculpt()
{
	// ���� �޽��� ���ٸ� �����ϰ�,
	GMesh *pMesh = GetEditMesh();
	if (pMesh == NULL)
		return;

	double t = EditMeshDlg.m_EditRgnDist / 1000.0;
	// ���� ������ �����Ǿ��ٸ�,
	if (t > 0.0)
	{
		// ���� �޽� ������ ���� ����ġ�� �ʱ�ȭ �Ѵ�.
		int NumVert = pMesh->GetNumVert();
		for (int i = 0; i < NumVert; ++i)
			pMesh->Verts[i].EditWgt = 0.0;
	
		// ���� �Ÿ���, ���� ����, �ٻ� ��輱�� ���Ѵ�.
		double EditDist = pMesh->BBox.GetDiagonal() * t;
		std::vector<double> Distances;
		get_geodesic_dist_field(pMesh, theSelector.MouseOverFaceIdx, theSelector.MouseOverPtCoords, Distances, EditDist);
		FindEditRgn(pMesh, Distances, EditDist);
		FindEditBndry(pMesh, Distances, EditDist);			
	}
	else	// ���� ������ �������� �ʾҴٸ�, 
	{
		// ���� ������ Ȯ��� ���� ��ü����Ʈ�� �ʱ�ȭ�Ѵ�.
		EditFaceRgn.clear();
		EditVertRgn.clear();
		EditBndryPts.clear();
	}
}

/*!
*	\brief ������ �Ÿ����� �̿��Ͽ� ���� ������ ���Ѵ�.
*
*	\param pMesh ��� �޽�
*	\param Distances ��� �޽��� ���� �Ÿ���
*	\param EditDist ���� ���� �Ÿ�
*/
void PEditor::FindEditRgn(GMesh *pMesh, std::vector<double> &Distances, double EditDist)
{
	// ���� ������ Ȯ��� ���� ��ü����Ʈ�� �ʱ�ȭ�Ѵ�.
	EditFaceRgn.clear();
	EditVertRgn.clear();

	// �Ÿ��� ���� �� ������ ���� ����ġ�� ���Ѵ�.
	int NumVert = pMesh->GetNumVert();
	for (int i = 0; i < NumVert; ++i)
	{
		double t = Distances[i] / EditDist;

		// ���� �������̶��,
		if (t == 0.0)
		{
			pMesh->Verts[i].EditWgt = 1.0;
		}
		else if (t > 0.0 && t < 1.0)	// ���� �������� �����̶��,
		{
			pMesh->Verts[i].EditWgt = 2 * t * t * t - 3 * t * t + 1;
			EditVertRgn.push_back(&(pMesh->Verts[i]));
		}
	}

	// ���� �޽��� �簢�� �̶��,
	if (pMesh->FaceType == FACE_QUAD)
	{
		// ������ �簢���� ���Ͽ�,
		for (unsigned i = 0; i < pMesh->Faces.size(); i += 2)
		{
			int *vidx = pMesh->Faces[i].vIdx;
			int vidx3 = pMesh->Faces[i + 1].vIdx[2];
			double w0 = pMesh->Verts[vidx[0]].EditWgt;
			double w1 = pMesh->Verts[vidx[1]].EditWgt;
			double w2 = pMesh->Verts[vidx[2]].EditWgt;
			double w3 = pMesh->Verts[vidx3].EditWgt;

			// ������ �ﰢ���̶�� ���� �ﰢ�� ����Ʈ�� �߰��Ѵ�. (������ ��)
			if (w0 > 0.0 || w1 > 0.0 || w2 > 0.0 || w3 > 0.0)
			{
				EditFaceRgn.push_back(&(pMesh->Faces[i]));
				EditFaceRgn.push_back(&(pMesh->Faces[i + 1]));
			}
		}
	}
	else if (pMesh->FaceType == FACE_TRI)	// ���� �޽��� �ﰢ�� �̶��,
	{
		// ������ �ﰢ���� ���Ͽ�,
		for (unsigned i = 0; i < pMesh->Faces.size(); ++i)
		{
			// ������ �ﰢ���̶�� ���� �ﰢ�� ����Ʈ�� �߰��Ѵ�. (������ ��)
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
*	\brief ������ �Ÿ����� �̿��Ͽ� ��Ȯ�� ���� ��踦 ���Ѵ�.
*
*	\param pMesh ��� �޽�
*	\param Distances ��� �޽��� ���� �Ÿ���
*	\param Angles ��� �޽��� ���� �Ÿ��� (����)
*	\param EditDist ���� ���� �Ÿ�
*/
void PEditor::FindEditBndry(GMesh *pMesh, std::vector<double> &Distances, std::vector<double> &Angles, double EditDist)
{
	// ���� ������ ��輱�� �ʱ�ȭ �Ѵ�.
	EditBndryPts.clear();

	// ���� ������ 2���� ������ ��ģ �޽��� �����Ѵ�.
	if (pUnfoldMesh != NULL)
	{
		delete pUnfoldMesh;
		UnfoldFaceMap.clear();
	}
	pUnfoldMesh = new GMesh("", GTransf(), FACE_TRI, "");
	pUnfoldMesh->Renderer.RenderType = RENDER_WIRE;

	// 2���� �޽��� ������ �ﰢ���� �����Ѵ�.
	for (unsigned i = 0; i < pMesh->Faces.size(); ++i)
	{
		// �� �ﰢ�� �� ���� ��ǥ�� �Ÿ��� ���Ͽ�,
		int *vidx = pMesh->Faces[i].vIdx;
		double d0 = Distances[vidx[0]];
		double d1 = Distances[vidx[1]];
		double d2 = Distances[vidx[2]];

		// �ﰢ���� ���� �������� ���Եȴٸ�,
		if (d0 != 1000000.0 && d1 != 1000000.0 && d2 != 1000000.0)	// 1000000.0 : default ��
		{
			// ������ �ﰢ���� �߰��Ѵ�.
			GPoint3 p0(d0 * cos(Angles[vidx[0]]), d0 * sin(Angles[vidx[0]]), 0.0);
			GPoint3 p1(d1 * cos(Angles[vidx[1]]), d1 * sin(Angles[vidx[1]]), 0.0);
			GPoint3 p2(d2 * cos(Angles[vidx[2]]), d2 * sin(Angles[vidx[2]]), 0.0);
			pUnfoldMesh->AddVertex(GVertex(p0));
			pUnfoldMesh->AddVertex(GVertex(p1));
			pUnfoldMesh->AddVertex(GVertex(p2));
			int NumVert = pUnfoldMesh->GetNumVert();
			pUnfoldMesh->AddFace(GFace(NumVert - 3, NumVert - 2, NumVert - 1));

			// �ﰢ���� ���� ����(2D �ﰢ�� �ε���, 3D �ﰢ�� �ε���)�� �����Ѵ�.
			UnfoldFaceMap[pUnfoldMesh->GetNumFace() - 1] = i;		
		}		
	}
	pUnfoldMesh->CreateMesh();
	GMaterial mtl = ::get_material(0.0, 0.0, 0.0);
	pUnfoldMesh->AddMtl(mtl);
	pUnfoldMesh->SetMtl(mtl.Name);

	// 3���� �޽��� ���� ��踦 ���Ѵ�.
	GQTree QuadTree(pUnfoldMesh, 7, 5);
	int NumSamp = MAX(20, pUnfoldMesh->GetNumFace() * 2);	// �ּ� ������ ��: 20
	for (int i = 0; i < NumSamp; ++i)
	{
		double theta = (double) i / (NumSamp - 1) * 2.0 * M_PI;
		GVector2 p(EditDist * cos(theta), EditDist * sin(theta));
		
		// ���� ���� p�� ���Ե� �ﰢ���� �ε����� barycentric ��ǥ�� ����ϰ�,
		double alpha, beta;
		int fidx = QuadTree.FindFace(p, &alpha, &beta);
		if (fidx != -1)
		{
			// 3���� ��ü�� ��ǥ�� �����Ͽ� ������ �޽��� �߰��Ѵ�.
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
*	\brief ������ �Ÿ����� �̿��Ͽ� �뷫���� ���� ��踦 ���Ѵ�.
*
*	\param pMesh ��� �޽�
*	\param Distances ��� �޽��� ���� �Ÿ���
*	\param EditDist ���� ���� �Ÿ�
*/
void PEditor::FindEditBndry(GMesh *pMesh, std::vector<double> &Distances, double EditDist)
{
	// ���� ������ ��輱�� �ʱ�ȭ �Ѵ�.
	EditBndryPts.clear();

	// �ﰢ��, �簢�� �޽��� ���Ͽ� ���� ��踦 ���Ѵ�.
	for (unsigned i = 0; i < EditFaceRgn.size(); ++i)
	{
		// �ﰢ�� �� ���� ��ǥ�� �Ÿ��� ���Ͽ�,
		int *vidx = EditFaceRgn[i]->vIdx;
		double d0 = Distances[vidx[0]];
		double d1 = Distances[vidx[1]];
		double d2 = Distances[vidx[2]];

		GPoint3 v0 = pMesh->Verts[vidx[0]].P;
		GPoint3 v1 = pMesh->Verts[vidx[1]].P;
		GPoint3 v2 = pMesh->Verts[vidx[2]].P;
		GPoint3 p, q;

		// ���� (v0, v1)�� (v2, v0)�� ���� ���� ������ ���,
		if (d0 < EditDist && d1 > EditDist && d2 > EditDist)
		{
			double s = (EditDist - d0) / (d1 - d0);
			double t = (EditDist - d0) / (d2 - d0);
			p = v0 + s * (v1 - v0);
			q = v0 + t * (v2 - v0);
		}
		// ���� (v1, v2)�� (v0, v1)�� ���� ���� ������ ���,
		if (d1 < EditDist && d2 > EditDist && d0 > EditDist)
		{
			double s = (EditDist - d1) / (d2 - d1);
			double t = (EditDist - d1) / (d0 - d1);
			p = v1 + s * (v2 - v1);
			q = v1 + t * (v0 - v1);
		}
		// ���� (v2, v0)�� (v1, v2)�� ���� ���� ������ ���,
		if (d2 < EditDist && d0 > EditDist && d1 > EditDist)
		{
			double s = (EditDist - d2) / (d0 - d2);
			double t = (EditDist - d2) / (d1 - d2);
			p = v2 + s * (v0 - v2);
			q = v2 + t * (v1 - v2);
		}
		// ���� (v0, v1)�� (v1, v2)�� ���� ���� ������ ���,
		if (d2 < EditDist && d0 < EditDist && d1 > EditDist)
		{
			double s = (EditDist - d0) / (d1 - d0);
			double t = (EditDist - d2) / (d1 - d2);
			p = v0 + s * (v1 - v0);
			q = v2 + t * (v1 - v2);
		}
		// ���� (v1, v2)�� (v2, v0)�� ���� ���� ������ ���,
		if (d0 < EditDist && d1 < EditDist && d2 > EditDist)
		{
			double s = (EditDist - d1) / (d2 - d1);
			double t = (EditDist - d0) / (d2 - d0);
			p = v1 + s * (v2 - v1);
			q = v0 + t * (v2 - v0);			
		}
		// ���� (v2, v0)�� (v0, v1)�� ���� ���� ������ ���,
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
*	\brief �ҽ� ��� �޽��� �����Ѵ�.
*
*	\param pCrv ������ �ҽ� �
*/
void PEditor::ProjectCrvOnMesh(GNurbsCrv *pCrv)
{
	// ��� ���ǵ� ���� �޽��� ���ٸ� �����Ѵ�.
	if (pCrv->RefMeshName == "")
		return;

	// ���� �޽��� ���ٸ� �����Ѵ�.
	GMesh *pMesh = FindMesh(pScene->MeshList, pCrv->RefMeshName);
	if (pMesh == NULL)
	{
		pCrv->RefMeshName = "";
		return;
	}

	// �޽��� �������� �����ϴ� ������ �����ϰ�,
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

			// ���� ���콺 �����Ϳ� ������ �ﰢ������ ���� Barycentric ��ǥ�� ����Ѵ�.
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

	// ������ ��� ���� ���� ������ �������� ���Ѵ�.
	// 	GPoint3 P0 = IsPtList[0];
	// 	GPoint3 Pn = IsPtList[IsPtList.size() - 1];
	// 
	// 	// ��鿡 ���Ե� ��� �޽��� ���ϰ�,
	// 	std::vector<GMesh *> MeshList;
	// 	GetPreOrderMeshList(pScene, MeshList, NULL);
	// 
	// 	// ������ ��� ���� ���� �� ���� ���� ����� ��� ������ ������ �����Ѵ�.
	// 	GNurbsCrv *pStProxiCrv, *pEdProxiCrv;
	// 	double StParam, EdParam;
	// 	double StMinDist = pCrv->Renderer.CtrlPtSize * 3.0;
	// 	double EdMinDist = pCrv->Renderer.CtrlPtSize * 3.0;
	// 	bool StFlag = false;
	// 	bool EdFlag = false;
	// 
	// 	// ����� ��� � ��ü�� ���Ͽ�,
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
	// ��� ���ǵ� ���� �޽��� ���ٸ� �����Ѵ�.
	if (pCrv->RefMeshName == "")
		return;

	// ���� �޽��� ���ٸ� �����Ѵ�.
	GMesh *pMesh = FindMesh(pScene->MeshList, pCrv->RefMeshName);
	if (pMesh == NULL)
	{
		pCrv->RefMeshName = "";
		return;
	}
	
	//////////////////////////////////////////////////////
	// 1 �ܰ�: ���� �޽��� �ĸ� Į�� ���ۿ� ������ �Ѵ�. //
	//////////////////////////////////////////////////////

	// ���� ���ۿ� ���� ���۸� �ʱ�ȭ�� ��,
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ����� ���� ���� �� ���� ����� �����ϰ�,
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	pScene->SetupViewFrustum();

	// ����� ���� ��ȯ�� �����Ѵ�.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	pScene->SetupViewTransform();

	// ������ ��Ȱ��ȭ �ϰ�, ���� �ﰢ�� ������ ����.
	glDisable(GL_LIGHTING);

	// ���õ� �޽��� �ﰢ���� ���δٸ� ������ ���̵� ������ �Ѵ�.
	unsigned int r = 0, g = 0, b = 0;

	// ���� ��ǥ�迡��
	glPushMatrix();
	{
		// �޽��� �𵨸� ��ǥ��� ��ȯ�ϰ�,
		glMultMatrixd(cast_mat(pMesh->MC).M);

		// �ﰢ���� ������ ���ϰ�,
		int NumFace = pMesh->GetNumFace();

		// �ﰢ�� ���� ���÷��� ����Ʈ�� �ִٸ�, 
		if (pMesh->DispFaceColMesh != 0)
		{
			// ���÷��� ����Ʈ�� ȣ���ϰ�,
			glCallList(pMesh->DispFaceColMesh);
		}
		else
		{
			if (pMesh->GrpFaces.empty())	// �ﰢ�� �׷��� ���ǵ��� �ʾҴٸ�,
			{
				// �ﰢ�� �������� �������� ���÷��� ����Ʈ�� �����Ѵ�.
				pMesh->DispFaceColMesh = glGenLists(1);
				glNewList(pMesh->DispFaceColMesh, GL_COMPILE_AND_EXECUTE);
				{
					// �ﰢ���� ���� �ٸ� ����(���̵�)���� �������� ��,
					glBegin(GL_TRIANGLES);
					for (int i = 0; i < NumFace; ++i)
					{
						int *vidx = pMesh->Faces[i].vIdx;
						GetColorId(r, g, b);	// (0, 0, 1) ���� ���۵�...
						glColor3ub(r, g, b);
						glVertex3dv(pMesh->Verts[vidx[0]].P.V);
						glVertex3dv(pMesh->Verts[vidx[1]].P.V);
						glVertex3dv(pMesh->Verts[vidx[2]].P.V);
					}
					glEnd();
				}
				glEndList();				
			}
			else	// �ﰢ�� �׷��� ���ǵǾ��ٸ�,
			{
				pMesh->DispFaceColMesh = glGenLists(1);
				glNewList(pMesh->DispFaceColMesh, GL_COMPILE_AND_EXECUTE);
				{
					// �ﰢ���� �׷캰�� ������ �Ѵ�.
					std::map<std::string, std::vector<int>>::iterator it;
					for (it = pMesh->GrpFaces.begin(); it != pMesh->GrpFaces.end(); ++it)
					{
						int NumGrpFace = it->second.size();
						glBegin(GL_TRIANGLES);
						for (int i = 0; i < NumGrpFace; ++i)
						{
							int fidx = it->second[i];
							int *vidx = pMesh->Faces[fidx].vIdx;
							GetColorId(r, g, b);	// (0, 0, 1) ���� ���۵�...
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
	// ���� ��ǥ��� �����Ѵ�.
	glPopMatrix();

	// ������ Ȱ��ȭ �Ѵ�.
	glEnable(GL_LIGHTING);	


	/////////////////////////////////////////////////
	// 2 �ܰ�: ��� ������ ��ũ�� ��ǥ�� ���Ѵ�. //
	/////////////////////////////////////////////////
	double uMin, uMax;
	pCrv->GetDomain(&uMin, &uMax);
	int NumPt = pCrv->n + 1;

	// �޽��� �������� �����ϴ� ������ �����ϰ�,
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
		// ���� ���콺 ������ ��ġ���� ���� ������ �ȼ� ���� �о,
		GLubyte pixel[3];
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

		// �ﰢ���� ������ �ε���(1, 2, 3, ...)�� ���ϰ�,
		int MouseOverFaceIdx = pixel[0] * 256 * 256 + pixel[1] * 256 + pixel[2] - 1;
		
		// ���콺�� ��ø�� �ﰢ���� �� ������ ���Ͽ�,
		int vidx0 = pMesh->Faces[MouseOverFaceIdx].vIdx[0];
		int vidx1 = pMesh->Faces[MouseOverFaceIdx].vIdx[1];
		int vidx2 = pMesh->Faces[MouseOverFaceIdx].vIdx[2];
		GVertex *v0 = &(pMesh->Verts[vidx0]);
		GVertex *v1 = &(pMesh->Verts[vidx1]);
		GVertex *v2 = &(pMesh->Verts[vidx2]);

		// ���� ��ǥ�迡�� ǥ���� �ﰢ���� ������ ��ǥ�� ���ϰ�,
		GPoint3 p0 = pMesh->MC * pMesh->Verts[vidx0].P;
		GPoint3 p1 = pMesh->MC * pMesh->Verts[vidx1].P;
		GPoint3 p2 = pMesh->MC * pMesh->Verts[vidx2].P;

		// ���� ���콺 ��ġ���� �信 ������ ������ �������� ���Ͽ�,
		GLine ray = pScene->MainCamera.GetOrthoRay(x, y);

		// ���� ���콺 �����Ϳ� ������ �ﰢ������ ���� Barycentric ��ǥ�� ����Ѵ�.
		double t, alpha, beta, gamma;
		GPoint3 ispt;
		if (intersect_line_triangle(p0, p1, p2, ray, t, ispt, alpha, beta, gamma, true))
		{
			IsPtList.push_back(cast_vec4(ispt, 1.0));
		}
	}

	////////////////////////////////////////////////////
	// 2 �ܰ�: �������� �ٻ��ϴ� 3�� ��� �����Ѵ�. //
	////////////////////////////////////////////////////
	int n = pCrv->n;
	GNurbsCrv *pProjCrv = ::get_gnurbs_crv_inter(3, n, &IsPtList[0], uMin, uMax, PARAM_CHORD_LEN, KNOT_AVERAGING);
	
	// ������ ��� ���� ���� ������ �������� ���Ѵ�.
// 	GPoint3 P0 = IsPtList[0];
// 	GPoint3 Pn = IsPtList[IsPtList.size() - 1];
// 
// 	// ��鿡 ���Ե� ��� �޽��� ���ϰ�,
// 	std::vector<GMesh *> MeshList;
// 	GetPreOrderMeshList(pScene, MeshList, NULL);
// 
// 	// ������ ��� ���� ���� �� ���� ���� ����� ��� ������ ������ �����Ѵ�.
// 	GNurbsCrv *pStProxiCrv, *pEdProxiCrv;
// 	double StParam, EdParam;
// 	double StMinDist = pCrv->Renderer.CtrlPtSize * 3.0;
// 	double EdMinDist = pCrv->Renderer.CtrlPtSize * 3.0;
// 	bool StFlag = false;
// 	bool EdFlag = false;
// 
// 	// ����� ��� � ��ü�� ���Ͽ�,
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
*	\brief ���õ� ��ü�� �����Ѵ� (������ ��� ���� ��ǥ�迡�� ǥ����)
*
*	\param t ������ ��ü�� �̵� ����
*	\param q ������ ��ü�� ȸ�� ����
*	\param s ������ ��ü�� ������ ����
*/
void PEditor::Edit(GVector3 t, GQuater q, GVector3 s)
{
	// ���õ� ��ü�� ���ٸ� �����Ѵ�.
	if (EditObjList.empty())
		return;

	// Shift�� ���ȴٸ�,
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		// ���� ���� ���� ��ǥ�踦 �����ϰ� �����Ѵ�.
		EC = EC * GTransf(t, q);
		return;
	}

	// ����, ���õ� ��� ��ü�� �����ϰ�,
	for (unsigned i = 0; i < EditObjList.size(); ++i)
		EditObjList[i]->Edit(EC, t, q, s);

	// ���� ��ü�� �ҽ� ��� �����̶��,
// 	GMesh *pMesh = GetEditMesh();
// 	if (pMesh->GetClassName() == "GNurbsCrv" && EditObjList[0]->GetClassName() == "GVertex")
// 	{
// 		// ��� ���� �޽��� �� �������� �����Ѵ�.
// 		GNurbsCrv *pCrv = (GNurbsCrv *)pMesh;
// 		ProjectCrvOnMesh(pCrv);
// 		pCrv->ModifiedFlag = MODIFIED_CTLPT_POS;
// 		pCrv->UpdateMesh();
// 	}

	// ���� �޽��� ���ϰ�,
	GMesh *pEditMesh = GetEditMesh();

	// �ֺ� ������ ������ ��� (���� ��ü�� ���� �Ǵ� �ﰢ��),
	if (!EditVertRgn.empty())
	{
		// ���� ��ü�� �ֺ� ������ �����ϰ�,
		for (unsigned i = 0; i < EditVertRgn.size(); ++i)
			EditVertRgn[i]->Edit(EC, t, q, s);

		if (pEditMesh->pBVH != NULL)
		{
			// �ֺ� ������ ���� BVH�� �����Ѵ�.
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
		
	// ��鿡 ���Ե� ��� �޽��� ����Ʈ ���Ͽ�,
	std::vector<GMesh *> MeshList;
	GetPreOrderMeshList(pScene, MeshList, NULL);

	// ������ �޽��� ���Ͽ� �ʿ信 ���� ������Ʈ�� �����Ѵ�.
	int NumMesh = (int)MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
		MeshList[i]->UpdateMesh();

	// �ֺ� ������ ������ ��� (���� ��ü�� ���� �Ǵ� �ﰢ��),
	if (!EditVertRgn.empty() && pEditFaceRgnBvh != NULL)
	{
		for (int i = 0; i < NumMesh; ++i)
		{
			GMesh *pTmpMesh = MeshList[i];
			if (pEditMesh == pTmpMesh)
				continue;

			// ���� �ֺ� �������� �浹 ������ �����ϰ�, �浹 ȸ�� ����� �� ������ �����Ѵ�.
			if (collision_check(pEditMesh->MC, pEditFaceRgnBvh, pTmpMesh) > 0 && EditMeshDlg.m_CollisionMode == 1)
				Edit(-t, inv(q), GVector3(1, 1, 1));
		}
	}
	else // ��ü�� ������ ���
	{
		for (int i = 0; i < NumMesh; ++i)
		{
			GMesh *pTmpMesh = MeshList[i];
			if (pEditMesh == pTmpMesh)
				continue;

			// �ٸ� �޽��� �浹 ������ �����ϰ�, �浹 ȸ�� ����� �� ������ �����Ѵ�.
			if (collision_check(pEditMesh, pTmpMesh) > 0  && EditMeshDlg.m_CollisionMode == 1)
				Edit(-t, inv(q), GVector3(1, 1, 1));
		}
	}

	// ���� ���� ���� ��ǥ�踦 �����Ѵ�.
	EC = EC * GTransf(t, q);
}

/*!
*	\brief ���õ� ��ü�� ������ �����Ѵ�.
*/
void PEditor::EditFinish()
{
	// ���õ� ��ü�� ���ٸ� �����Ѵ�.
	if (EditObjList.empty())
		return;

	// ������ �޽��� ���Ͽ�,
	GMesh *pMesh = GetEditMesh();

	// �޽� �����̳� �ﰢ���� ������ ���, �޽��� ���� ������ �����Ѵ�.
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
		// ������ ��ġ�� �ﰢ���� ��ġ�� ���� �Ǿ��ٸ� �޽��� ������Ʈ �Ѵ�.
		pMesh->ModifiedFlag = MODIFIED_NORMAL;
		pMesh->UpdateMesh();

		// ���� ������ �����ϰ� ���� BVH�� �����Ѵٸ� BVH�� ���� �����Ѵ�.
		if (!EditVertRgn.empty() && pMesh->pBVH != NULL)
			pMesh->CreateBVH();
	}

	// ȸ�� �������� ���� ������ �ʱ�ȭ �Ѵ�.
	RotRefX = GTransf();
	RotRefPos.Set(0.0, 0.0, 0.0);

	// ���õ� ù��° ��ü�� �޽��� ���������� ��ȭ ���ڿ� ǥ���Ѵ�.
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
*	\brief ���� ��, ���� ���� ���¸� �ʱ�ȭ�Ѵ�.
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
*	\brief ���õ� ��ü(����, �ﰢ��, �޽� ��)�� �����Ѵ�.
*/
void PEditor::DeleteObject()
{
	// ���õ� ���� ��ü�� ���ٸ� �����Ѵ�.
	if (EditObjList.empty())
		return;

	// ���õ� ��ü�� Ŭ���� �̸��� ���Ͽ�,
	std::string ClsName = EditObjList[0]->GetClassName();

	// ���õ� ��ü�� �����̶��,
	if (ClsName == "GVertex")
	{
		DeleteMeshVert();		
	}
	// ���õ� ���� ��ü�� �ﰢ���̶��,
	else if (ClsName == "GFace")
	{
		DeleteMeshFace();		
	}
	// ���õ� ���� ��ü�� ������ �ﰢ���� �ƴ϶��,
	else
	{
		// ���� �޽��� ���ϰ�, �޽��� �����Ѵ�.
		std::vector<GMesh *> RefMeshList;
		GetRefMeshList(RefMeshList);

		std::vector<std::string> NameList;
		int NumMesh = (int)RefMeshList.size();
		for (int i = 0; i < NumMesh; ++i)
			NameList.push_back(RefMeshList[i]->Name);

		for (int i = 0; i < NumMesh; ++i)
			pScene->DelMesh(pScene->MeshList, NameList[i]);

		// ������ �޽��� ���ε��� �޽��� ���ε� ������ �����Ѵ�.
		NumMesh = (int)pScene->MeshList.size();
		for (int i = 0; i < NumMesh; ++i)
			pScene->MeshList[i]->InitRefBoneInfo();

		// ��Ƽ�͸� �ʱ�ȭ �Ѵ�.
		Reset();
	}
}

/*!
*	\brief ���õ� ������ �����Ѵ�.
*/
void PEditor::DeleteMeshVert()
{
	// ���� �޽��� ���ϰ�,
	GMesh *pMesh = GetEditMesh();

	// ������ ������ �ε����� ���Ѵ�.
	std::vector<int> VertList;
	for (int i = 0; i < GetNumEditObj(); ++i)
	{
		GVertex *pVert = (GVertex *)EditObjList[i];
		VertList.push_back(pVert->Idx);
	}

	// �޽��� ������ �����Ѵ�.
	pMesh->DelVerts(VertList);

	// �����͸� �ʱ�ȭ�ϰ�, ���� �޽��� �����Ѵ�.
	Reset();
	EditObjList.push_back(pMesh);
}

/*!
*	\brief ���õ� �ﰢ���� �����Ѵ�.
*/
void PEditor::DeleteMeshFace()
{
	// ���� �޽��� ���Ͽ�,
	GMesh *pMesh = GetEditMesh();

	// ������ �ﰢ���� �ε����� ���Ѵ�.
	std::vector<int> FaceList;
	for (int i = 0; i < GetNumEditObj(); ++i)
	{
		GFace *pFace = (GFace *)EditObjList[i];
		FaceList.push_back(pFace->Idx);
	}

	// �޽��� �ﰢ���� �����Ѵ�.
	pMesh->DelFaces(FaceList);

	// �����͸� �ʱ�ȭ�ϰ�, ���� �޽��� �����Ѵ�.
	Reset();
	EditObjList.push_back(pMesh);
}

/*!
*	\brief ���õ� ���� ��ü�� ������ �ϵ��� �Ѵ�.
*/
void PEditor::ShowMesh()
{
	// ���õ� ���� ��ü�� ���ٸ� �����Ѵ�.
	if (EditObjList.empty())
		return;

	// ���� �޽��� ���ϰ�, �޽��� �����Ѵ�.
	std::vector<GMesh *> RefMeshList;
	GetRefMeshList(RefMeshList);

	int NumMesh = (int)RefMeshList.size();
	for (int i = 0; i < NumMesh; ++i)
		RefMeshList[i]->ShowMesh(false);
}

/*!
*	\brief ���õ� ���� ��ü�� ���������� �ʵ��� �Ѵ�.
*/
void PEditor::HideMesh()
{
	// ���õ� ���� ��ü�� ���ٸ� �����Ѵ�.
	if (EditObjList.empty())
		return;

	// ���� �޽��� ���ϰ�, �޽��� �����Ѵ�.
	std::vector<GMesh *> RefMeshList;
	GetRefMeshList(RefMeshList);

	int NumMesh = (int)RefMeshList.size();
	for (int i = 0; i < NumMesh; ++i)
		RefMeshList[i]->HideMesh(false);
}