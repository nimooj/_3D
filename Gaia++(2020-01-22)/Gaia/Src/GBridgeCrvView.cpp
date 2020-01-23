#include "stdafx.h"
#include "..\Gaia.h"
#include "..\MainFrm.h"
#include "..\GaiaDoc.h"
#include "..\GaiaView.h"
#include "GBridgeCrvView.h"

BEGIN_MESSAGE_MAP(GBridgeCrvView, CWnd)
	//{{AFX_MSG_MAP(GBridgeCrvView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
END_MESSAGE_MAP()

GBridgeCrvView::GBridgeCrvView()
{
	// Variables for OpenGL Setting.
	m_pDC = 0;
	m_hDC = 0;
	m_hRC = 0;
	m_pParentDlg = NULL;
	m_ZoomDist = -30.0;
	
	m_SltMesh = -1;
	m_pMesh = NULL;
	m_pUnfoldMesh = NULL;
	m_pContCrv = NULL;
	m_CrvParam = 0.0;
	m_CrvResolution = 0;
}

GBridgeCrvView::~GBridgeCrvView()
{
}

/////////////////////////////////////////////////////////////////////////////
// GBridgeCrvView message handlers
BOOL GBridgeCrvView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	return CWnd::PreCreateWindow(cs);
}

int GBridgeCrvView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pDC = new CClientDC(this); 
	m_hDC = m_pDC->GetSafeHdc();

	PIXELFORMATDESCRIPTOR pfd = {
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
	int pixelformat;
	pixelformat = ::ChoosePixelFormat(m_hDC, &pfd);
	::SetPixelFormat(m_hDC, pixelformat, &pfd);
	m_hRC = ::wglCreateContext(m_hDC);

	::wglMakeCurrent(m_hDC, m_hRC);
	InitOpenGL();
	::wglMakeCurrent(NULL, NULL);
	return 0;
}

void GBridgeCrvView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	::wglMakeCurrent(m_hDC, m_hRC);
	glViewport(0, 0, cx, cy);
	::wglMakeCurrent(NULL, NULL);
}

BOOL GBridgeCrvView::OnEraseBkgnd(CDC* pDC) 
{
	return true;
}

void GBridgeCrvView::InitOpenGL()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void GBridgeCrvView::InitContCrv(int Type)
{
	if (m_pContCrv != NULL)
		delete m_pContCrv;

	switch (Type)
	{
	case 0:	// Circle
		m_pContCrv = ::get_gnurbs_approx_circle(3.0, 0, true);
		break;

	case 1:	// Square
		m_pContCrv = ::get_gnurbs_rect(6.0, 6.0, 0);
		break;
	}

	GMaterial mat_blue = get_material(0.0, 0.0, 1.0);	
	m_pContCrv->KnotRefine();
	m_pContCrv->KnotRefine();
	m_pContCrv->KnotRefine();
	m_pContCrv->KnotRefine();
	m_pContCrv->Renderer.RenderType = RENDER_POLYGON;
	m_pContCrv->Renderer.SetSampleNum(20, 200);
	m_pContCrv->Renderer.CtrlPtSize = 0.1;
	m_pContCrv->Renderer.bRenderCrvAsLine = true;
	m_pContCrv->AddMtl(mat_blue);
	m_pContCrv->SetMtl(mat_blue.Name);
	m_pContCrv->CreateMesh();

	Invalidate(false);
}

void GBridgeCrvView::Render()
{
	// 조명을 비활성화 한다.
	glDisable(GL_LIGHTING);

	// X, Y 축을 그린다.
	glColor3f(0.5, 0.5, 0.5);
	glBegin(GL_LINES);	
	glVertex3f(-100.0f, 0.0f, 0.0f);
	glVertex3f(100.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, -100.0f, 0.0f);
	glVertex3f(0.0f, 100.0f, 0.0f);
	glEnd();

	if (m_SltMesh == -1 || m_pUnfoldMesh == NULL || m_pContCrv == NULL)
		return;

	// 선택점을 렌더링한다.
 	GVector4 pt = m_pContCrv->Eval(m_CrvParam);
 	glPointSize(7.0);
 	glColor3f(1.0f, 0.0f, 0.0f);
 	glBegin(GL_POINTS);
 	glVertex3d(pt[0], pt[1], pt[2]);
 	glEnd();
 	glPointSize(1.0);
 
 	// 접촉 곡선을 렌더링 한다.
 	m_pContCrv->Render();

	// 보조 곡선을 렌더링 한다.
	GNurbsCrv *pAuxCrv = m_pContCrv->CopyObject();
	GBridgeMeshDlg *pDlg = (GBridgeMeshDlg *)m_pParentDlg;

	if (pDlg->m_SltMesh == 0)
		pAuxCrv->Scale(pDlg->m_StartAuxCrvSclParam);
	else
		pAuxCrv->Scale(pDlg->m_EndAuxCrvSclParam);

	pAuxCrv->CreateMesh();
	pAuxCrv->Render();
	
 	
 	// 조명을 활성화 한다.
 	glEnable(GL_LIGHTING);
 
 	// 현재 선택된 메쉬의 포인터를 구하여,	
 	glColor3f(0.0, 0.0, 0.0);
 	glPushMatrix();
		m_pUnfoldMesh->Render();
 	glPopMatrix();	
}

void GBridgeCrvView::OnPaint() 
{
	//SetFocus();
	::wglMakeCurrent(m_hDC, m_hRC);
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		GLdouble ViewSize = -m_ZoomDist * (GLdouble)tan(30.0 * M_PI / 360.0); 
		glOrtho(-ViewSize, ViewSize, -ViewSize, ViewSize, -100.0, 100.0);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Render();
		
		glFinish();
		SwapBuffers(m_hDC);
	}
	::wglMakeCurrent(NULL, NULL);	

	CWnd::OnPaint();
}

void GBridgeCrvView::OnDestroy() 
{
	CWnd::OnDestroy();

	::wglMakeCurrent(NULL, NULL);
	::wglDeleteContext(m_hRC);
	if (m_pDC)
		delete m_pDC;
	m_pDC = NULL;
}

void GBridgeCrvView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	::wglMakeCurrent(m_hDC, m_hRC);
	{
		MousePt[0] = point.x;
		MousePt[1] = point.y;
	}
	::wglMakeCurrent(NULL, NULL);

	Invalidate(false);
	CWnd::OnLButtonDown(nFlags, point);
}

void GBridgeCrvView::OnMouseMove(UINT nFlags, CPoint point) 
{
	::wglMakeCurrent(m_hDC, m_hRC);
	{
		//theCutterScene.MouseMove(point.x, point.y);

		if (nFlags & MK_LBUTTON)
		{
			//theCutterEditor.MouseMove(point.x, point.y, nFlags);
			double dx = (point.x - MousePt[0]) * 0.05;
			double dy = -(point.y - MousePt[1]) * 0.05;
			if (m_pContCrv != NULL)
				m_pContCrv->Edit(m_CrvParam, GVector4(dx, dy, 0.0, 0.0), m_CrvResolution);
			MousePt[0] = point.x;
			MousePt[1] = point.y;			
		}
	}
	::wglMakeCurrent(NULL, NULL);

	Invalidate(false);
	CWnd::OnMouseMove(nFlags, point);
}
void GBridgeCrvView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	::wglMakeCurrent(m_hDC, m_hRC);
	{
	}
	::wglMakeCurrent(NULL, NULL);

	Invalidate();
	CWnd::OnLButtonUp(nFlags, point);
}

void GBridgeCrvView::OnMButtonDown(UINT nFlags, CPoint point)
{
	::wglMakeCurrent(m_hDC, m_hRC);
	{
	}	
	::wglMakeCurrent(NULL, NULL);

	CWnd::OnMButtonDown(nFlags, point);
}

void GBridgeCrvView::OnMButtonUp(UINT nFlags, CPoint point)
{
	::wglMakeCurrent(m_hDC, m_hRC);
	{
	}
	::wglMakeCurrent(NULL, NULL);

	CWnd::OnMButtonUp(nFlags, point);
}

BOOL GBridgeCrvView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	::wglMakeCurrent(m_hDC, m_hRC);
	{
		double dz = (zDelta > 0) ? -0.1 : 0.1;
		m_ZoomDist += m_ZoomDist * dz;
	}
	::wglMakeCurrent(NULL, NULL);

	Invalidate(false);
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void GBridgeCrvView::OnEditCopy()
{
	::wglMakeCurrent(m_hDC, m_hRC);
	{
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
		::glReadPixels( 0, 0, size.cx, size.cy, GL_BGR_EXT, GL_UNSIGNED_BYTE, pPixelData ); 

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
		HANDLE handle = (HANDLE)::GlobalAlloc (GHND,sizeof(BITMAPINFOHEADER) + NbBytes); 

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
	}
	::wglMakeCurrent(NULL, NULL);
}


