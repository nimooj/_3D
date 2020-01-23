// GaiaView.cpp : GaiaView Ŭ������ ����
//

#include "stdafx.h"
#include "Gaia.h"
#include "MainFrm.h"
#include "GaiaDoc.h"
#include "GaiaView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// GaiaView

IMPLEMENT_DYNCREATE(GaiaView, CView)

BEGIN_MESSAGE_MAP(GaiaView, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CAMERA_PERS, &GaiaView::OnCameraPers)
	ON_COMMAND(ID_CAMERA_ORTHO, &GaiaView::OnCameraOrtho)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_PERS, &GaiaView::OnUpdateCameraPers)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_ORTHO, &GaiaView::OnUpdateCameraOrtho)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_EDIT_COPY, &GaiaView::OnEditCopy)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// GaiaView ����/�Ҹ�
GaiaView::GaiaView()
{
}

GaiaView::~GaiaView()
{
}

BOOL GaiaView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;	

	return CView::PreCreateWindow(cs);
}

// GaiaView �׸���
void GaiaView::OnDraw(CDC *pDC)
{
	GaiaDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// OpenGL ��� ����
	wglMakeCurrent(m_hDC, m_hRC);

	// ���� ���ۿ� ���� ���۸� �ʱ�ȭ �Ѵ�.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ����� �������ϰ�,
	theScene().Render();

	// ���� �� ���� ������ ������ �ϰ�,
	theEditor().Render();

	// ���� ������ ������ �ϰ�,
	theCreator().Render();

	if (pDoc->m_MinDistPt1 != GPoint3(0, 0, 0) && pDoc->m_MinDistPt2 != GPoint3(0, 0, 0))
	{
		glDisable(GL_LIGHTING);
		glLineWidth(3.0);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES);
		glVertex3dv(pDoc->m_MinDistPt1.V);
		glVertex3dv(pDoc->m_MinDistPt2.V);
		glEnd();
		glLineWidth(1.0);
		glEnable(GL_LIGHTING);
	}

	// �������� �ĸ� ���۸� ���� ���۷� ��ȯ�Ѵ�.
	glFinish();
	SwapBuffers(m_hDC);

	// �ĸ� ���ۿ� ������ ���� �������� �����Ѵ�.
	if (theCreator().CreateType != CREATE_INACTIVE)
		theCreator().RenderForSelect();
	else
		theSelector().RenderForSelect();
		
	// OpenGL ��� Ż��
	wglMakeCurrent(NULL, NULL);

	CView::OnDraw(pDC);
}


// GaiaView ����
#ifdef _DEBUG
void GaiaView::AssertValid() const
{
	CView::AssertValid();
}

void GaiaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

GaiaDoc* GaiaView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(GaiaDoc)));
	return (GaiaDoc*)m_pDocument;
}
#endif //_DEBUG


// GaiaView �޽��� ó����

int GaiaView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pDC = new CClientDC(this); 
	m_hDC = m_pDC->GetSafeHdc();

	PIXELFORMATDESCRIPTOR PDF = {
		sizeof(PIXELFORMATDESCRIPTOR), 
		1, 
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0,
		0, 
		0, 
		0,
		0, 0, 0, 0,
		32, 
		0, 
		0, 
		PFD_MAIN_PLANE,
		0, 
		0, 0, 0 
	};

	int PixelFormat = ::ChoosePixelFormat(m_hDC, &PDF);
	::SetPixelFormat(m_hDC, PixelFormat, &PDF);

	m_hRC = ::wglCreateContext(m_hDC);
	::wglMakeCurrent(m_hDC, m_hRC);
	theScene().InitScene();
	::wglMakeCurrent(NULL, NULL);

	return 0;
}

void GaiaView::InitView()
{
	theScene().FileName = "Noname01.gff";
	//theScene().InitScene();
	theScene().MeshList.clear();
	theEditor().Reset();

	// OnSize() �Լ��� ȣ���ϱ� ���ؼ�...
	CRect rc;
	GetClientRect(&rc);
	SendMessage(WM_SIZE, 0, MAKELPARAM(rc.Width(), rc.Height()));		
}

void GaiaView::OnDestroy()
{
	CView::OnDestroy();

	::wglMakeCurrent(NULL, NULL);
	::wglDeleteContext(m_hRC);
	if (m_pDC)
		delete m_pDC;
}

BOOL GaiaView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}

void GaiaView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	::wglMakeCurrent(m_hDC, m_hRC);
	theScene().SetupViewport(cx, cy);
	::wglMakeCurrent(NULL, NULL);
}
/////////////////////////////////////////////////////////////////////////
void GaiaView::OnLButtonDown(UINT nFlags, CPoint point)
{
	::wglMakeCurrent(m_hDC, m_hRC);

	// ����� ȸ���� ���� ����ڰ� ALTŰ�� ���� ���¶��,
	if ((GetAsyncKeyState(VK_MENU) & 0x8000))
	{
		// ����� ȸ���� ���� �غ� �Ѵ�.
		theScene().LButtonDown(point.x, point.y);
	}
	// ����� ȸ�� ������ �ƴ϶��,
	else
	{
		// �ϴ�, ���� ����� ���(Ȩ ��, ���� ��, ������ �� ��)�� �����Ѵ�.
		theScene().HeadUpDisplay.LButtonDown(point.x, point.y);

		// ��ü�� �����ϰų�, ���ο� ��ü�� �����Ѵ�.
		theEditor().LButtonDown(point.x, point.y, nFlags);

		// ���ο� ��ü�� �����Ѵ�.
		theCreator().LButtonDown(point.x, point.y, nFlags);
	}

	Invalidate(false);

	wglMakeCurrent(NULL, NULL);
	CView::OnLButtonDown(nFlags, point);
}

void GaiaView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// ����� Ÿ�̸Ӱ� �� �ִٸ� �����Ѵ�.
	if (theScene().bTimer)
		return;

	wglMakeCurrent(m_hDC, m_hRC);

	// ����� �̵��� ���� ����ڰ� ALTŰ�� ���� ���¶��,
	if ((GetAsyncKeyState(VK_MENU) & 0x8000))
	{
		// ����� �̵��� �غ��Ѵ�.
		theScene().MButtonDown(point.x, point.y);			
	}

	Invalidate(false);

	wglMakeCurrent(0, 0);
	CView::OnMButtonDown(nFlags, point);
}

void GaiaView::OnMouseMove(UINT nFlags, CPoint point)
{
	// ����� Ÿ�̸Ӱ� �� �ִٸ� �����Ѵ�.
	if (theScene().bTimer)
		return;

	wglMakeCurrent(m_hDC, m_hRC);

	// ����� ����(ȸ��, �̵�)�� ���� ����ڰ� ALTŰ�� ���� ���¶��,
	if (GetAsyncKeyState(VK_MENU) & 0x8000)
	{
		// ����� ȸ���� �����Ѵ�.
		theScene().MouseMove(point.x, point.y);			
	}
	// ����� ������ �ƴ϶��,
	else
	{
		// ���ο� ��ü�� ���� ���̶��, 
		if (theCreator().CreateType != CREATE_INACTIVE)
		{
			// ���ο� ��ü�� �����ϴ� �۾��� 
			theCreator().MouseMove(point.x, point.y, nFlags);
		}
		// ��ü ���� ���� �ƴ϶��, 
		else
		{
			// ���� ��ü�� �����ϰų�, ���� ������ �����Ѵ�.
			theEditor().MouseMove(point.x, point.y, nFlags);			
		}
	}

	Invalidate(false);

	wglMakeCurrent(0, 0);
	CView::OnMouseMove(nFlags, point);
}

void GaiaView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// ����� Ÿ�̸Ӱ� �� �ִٸ� �����Ѵ�.
	if (theScene().bTimer)
		return;

	wglMakeCurrent(m_hDC, m_hRC);

	// ����� ȸ���� �����ϰ�,
	theScene().LButtonUp(point.x, point.y);

	// ��ü�� ���� ���̶��, 
	if (theCreator().CreateType != CREATE_INACTIVE)
	{
		// ��ü�� ������ �Ϸ��Ѵ�.
		theCreator().LButtonUp(point.x, point.y, nFlags);
	}
	// ��ü ���� ���� �ƴ϶��,
	else
	{
		// ��ü�� ���� �Ǵ� ��ü�� ������ �����Ѵ�.
		theEditor().LButtonUp(point.x, point.y, nFlags);
	}

	Invalidate(false);

	wglMakeCurrent(0, 0);
	CView::OnLButtonUp(nFlags, point);
}

void GaiaView::OnMButtonUp(UINT nFlags, CPoint point)
{
	wglMakeCurrent(m_hDC, m_hRC);

	// ����� �̵��� �����Ѵ�.
	theScene().MButtonUp(point.x, point.y);

	Invalidate(false);

	wglMakeCurrent(0, 0);
	CView::OnMButtonUp(nFlags, point);
}

BOOL GaiaView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	wglMakeCurrent(m_hDC, m_hRC);

	// Control Ű�� ���� ���¶��,
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		// ���� ���� ���� ������ �����Ѵ�.
		theEditor().MouseWheel(zDelta);
	}
	else if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		// ���� �귯���� ���� ������ �����Ѵ�.
		theSelector().MouseWheel(zDelta);
	}
	else
	{
		// ����� �������� �����Ѵ�.
		theScene().MouseWheel(zDelta);
	}

	Invalidate(false);

	wglMakeCurrent(0, 0);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

//////////////////////////////////////////////////////////////////////////////////////
void GaiaView::OnContextMenu(CWnd *pWnd, CPoint point)
{
	// �˾� �޴��� ��µ� ��ġ�� ����ϰ�,
	if (point.x == -1 && point.y == -1)
	{
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);
		point = rect.TopLeft();
		point.Offset(5, 5);
	}

	CMenu menu;
	std::string ClsName = theEditor().GetClsNameEditObj();
	if (ClsName == "GNurbsCrv" || ClsName == "GNurbsSrf")
	{
		// ��� Ʈ���� �˾� �޴��� �ε��ϰ�,
		menu.LoadMenu(IDR_NURBS_POPUP);		
	}
	else if (ClsName == "GVertex")
	{
		// ��� Ʈ���� �˾� �޴��� �ε��ϰ�,
		menu.LoadMenu(IDR_VERTEX_POPUP);
	}
	else
	{
		// ��� Ʈ���� �˾� �޴��� �ε��ϰ�,
		menu.LoadMenu(IDR_SCENE_POPUP);
	}

	// �˾� �޴��� ����Ѵ�.
	CMenu *pPopup = menu.GetSubMenu(0);
	CWnd *pWndPopupOwner = this;

	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pWndPopupOwner);	
}

void GaiaView::OnCameraPers()
{
	theScene().MainCamera.CameraType = PROJECTION_PERS;
}

void GaiaView::OnCameraOrtho()
{
	theScene().MainCamera.CameraType = PROJECTION_ORTHO;
}

void GaiaView::OnUpdateCameraPers(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theScene().MainCamera.CameraType == PROJECTION_PERS);
}

void GaiaView::OnUpdateCameraOrtho(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theScene().MainCamera.CameraType == PROJECTION_ORTHO);
}

void GaiaView::OnEditCopy()
{
	wglMakeCurrent(m_hDC, m_hRC);

	BeginWaitCursor();
	// Get client geometry 
	CRect rect; 
	GetClientRect(&rect); 
	CSize size(rect.Width(), rect.Height()); 
	TRACE("  client zone : (%d;%d)\n",size.cx,size.cy); 
	// Lines have to be 32 bytes aligned, suppose 24 bits per pixel 
	// I just cropped it 
	size.cx -= size.cx % 4; 
	TRACE("  final client zone : (%d;%d)\n",size.cx,size.cy); 

	// Create a bitmap and select it in the device context 
	// Note that this will never be used ;-) but no matter 

	CBitmap bitmap; 
	CDC *pDC = GetDC();
	CDC MemDC; 
	ASSERT(MemDC.CreateCompatibleDC(NULL)); 
	ASSERT(bitmap.CreateCompatibleBitmap(pDC,size.cx,size.cy)); 
	MemDC.SelectObject(&bitmap); 

	// Alloc pixel bytes 
	int NbBytes = 3 * size.cx * size.cy; 
	unsigned char *pPixelData = new unsigned char[NbBytes]; 
	// Copy from OpenGL 
	//::glFinish();
	::glReadPixels(0, 0, size.cx, size.cy, GL_BGR_EXT, GL_UNSIGNED_BYTE, pPixelData); 


	// Fill header 
	BITMAPINFOHEADER header; 
	header.biWidth = size.cx; 
	header.biHeight = size.cy; 
	header.biSizeImage = NbBytes; 
	header.biSize = 40; 
	header.biPlanes = 1; 
	header.biBitCount =  3 * 8; // RGB 
	header.biCompression = 0; 
	header.biXPelsPerMeter = 0; 
	header.biYPelsPerMeter = 0; 
	header.biClrUsed = 0; 
	header.biClrImportant = 0; 
	// Generate handle 
	HANDLE handle = (HANDLE)::GlobalAlloc(GHND,sizeof(BITMAPINFOHEADER) + NbBytes); 

	if(handle != NULL) 
	{ 
		// Lock handle 
		char *pData = (char *) ::GlobalLock((HGLOBAL)handle); 
		// Copy header and data 
		memcpy(pData,&header,sizeof(BITMAPINFOHEADER)); 
		memcpy(pData+sizeof(BITMAPINFOHEADER),pPixelData,NbBytes); 
		// Unlock 
		::GlobalUnlock((HGLOBAL)handle); 
		// Push DIB in clipboard 
		OpenClipboard(); 
		EmptyClipboard(); 
		SetClipboardData(CF_DIB,handle); 
		CloseClipboard(); 
	} 
	// Cleanup 
	MemDC.DeleteDC(); 
	bitmap.DeleteObject(); 
	delete [] pPixelData; 
	EndWaitCursor(); 

	wglMakeCurrent(0, 0);
}


void GaiaView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		// Home ��
	case 'h':
	case 'H':
		theScene().HeadUpDisplay.SetHomeView();
		break;

		// Front ��
	case 'f':
	case 'F':
		theScene().HeadUpDisplay.SetFrontView();
		break;

		// Left ��
	case 'l':
	case 'L':
		theScene().HeadUpDisplay.SetLeftView();
		break;

		// Right ��
	case 'r':
	case 'R':
		theScene().HeadUpDisplay.SetRightView();
		break;

		// Back ��
	case 'b':
		theScene().HeadUpDisplay.SetBackView();
		break;

		// Top ��
	case 't':
	case 'T':
		theScene().HeadUpDisplay.SetTopView();
		break;

		// Bottom ��
	case 'B':
		theScene().HeadUpDisplay.SetBottomView();
		break;

		// ���� ����
	case 'O':
	case 'o':
		theScene().MainCamera.CameraType = PROJECTION_ORTHO;
		break;

		// ���� ����
	case 'p':
	case 'P':
		theScene().MainCamera.CameraType = PROJECTION_PERS;
		break;

		// �ٴ� ��� ������ ����
	case 'g':	
	case 'G':
		theScene().bShowFloor = !theScene().bShowFloor;
		break;

		// ��� ȸ�� ��� x-axis
	case 'x':
	case 'X':
		theScene().MainCamera.SetArcBallType(ARCBALL_X);
		break;

		// ��� ȸ�� ��� y-axis
	case 'y':
	case 'Y':
		theScene().MainCamera.SetArcBallType(ARCBALL_Y);
		break;

		// ��� ȸ�� ��� z-axis
	case 'z':
	case 'Z':
		theScene().MainCamera.SetArcBallType(ARCBALL_Z);
		break;

	case 'a':
	case 'A':
		{
			if (theEditor().EditObjList.empty())
				break;

			GMesh *pMesh = theEditor().EditObjList[0]->GetRefMesh();
			theScene().AdjustViewFrustum(pMesh->BBox);
		}
		break;

	case 'w':
	case 'W':
		{
			std::cout << "Key w is pressed" << std::endl;
			if (theEditor().EditObjList.empty())
				break;

			int NumObj = theEditor().GetNumEditObj();
			for (int i = 0; i < NumObj; ++i)
			{
				GMesh *pMesh = theEditor().EditObjList[i]->GetRefMesh();
				if (pMesh->Renderer.RenderType & RENDER_WIRE)
					pMesh->Renderer.RenderType = pMesh->Renderer.RenderType & (~RENDER_WIRE);
				else
					pMesh->Renderer.RenderType = pMesh->Renderer.RenderType | RENDER_WIRE;
			}
		}
		break;		
	}

	Invalidate(FALSE);

	CView::OnChar(nChar, nRepCnt, nFlags);
}

void GaiaView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_F1:
		theMainFrm().SendMessage(WM_COMMAND, ID_EDIT_TRA);
		break;

	case VK_F2:
		theMainFrm().SendMessage(WM_COMMAND, ID_EDIT_ROT);
		break;

	case VK_F3:
		theMainFrm().SendMessage(WM_COMMAND, ID_EDIT_SCL);
		break;

	case VK_F4:
		theMainFrm().OnRepeatCommand();
		break;

	case VK_DELETE:
		theDoc().OnDeleteMesh();
		break;

	case VK_ESCAPE:
		theEditor().Reset();
		theCreator().Reset();
		break;

	case VK_LEFT:
		theScene().HeadUpDisplay.SetWestView();
		break;

	case VK_RIGHT:
		theScene().HeadUpDisplay.SetEastView();
		break;

	case VK_UP:
		theScene().HeadUpDisplay.SetNorthView();
		break;

	case VK_DOWN:
		theScene().HeadUpDisplay.SetSouthView();
		break;
	}

	Invalidate(FALSE);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL GaiaView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	if (theScene().MainCamera.CameraManipulateType == CAMERA_MANIPULATE_ROTATE)
	{
		::SetCursor(theApp.LoadCursor(IDC_ROTATE_CURSOR));
		return true;
	}

	if (theScene().MainCamera.CameraManipulateType == CAMERA_MANIPULATE_PAN)
	{
		::SetCursor(theApp.LoadCursor(IDC_MOVE_CURSOR));
		return true;
	}

	if (theEditor().EditToolType >= EDIT_TRA && theEditor().EditToolType <= EDIT_SCL_XYZ)
	{
		::SetCursor(theApp.LoadCursor(IDC_EDIT_CURSOR));
		return true;
	}

	if (theEditor().theSelector.SelectToolType == SELECT_TOOL_LASSO)
	{
		::SetCursor(theApp.LoadCursor(IDC_LASSO_CURSOR));
		return true;
	}

	if (theEditor().theSelector.SelectToolType == SELECT_TOOL_POLY)
	{
		::SetCursor(theApp.LoadCursor(IDC_POLY_CURSOR));
		return true;
	}

	if (theEditor().theSelector.SelectToolType == SELECT_TOOL_BRUSH)
	{
		::SetCursor(theApp.LoadCursor(IDC_PAINT_CURSOR));
		return true;
	}

	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void GaiaView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
// 	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
// 	pFrame->m_wndSplitter.ChangeViewStatus();

	CView::OnLButtonDblClk(nFlags, point);
}
