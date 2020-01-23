#include "stdafx.h"
#include "PCutter.h"


PCutterView::PCutterView()
{
	// Variables for OpenGL Setting.
	m_pDC = 0;
	m_hDC = 0;
	m_hRC = 0;
	m_pCutterDlg = NULL;
}

PCutterView::~PCutterView()
{
}

BEGIN_MESSAGE_MAP(PCutterView, CWnd)
	//{{AFX_MSG_MAP(PCutterView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// PCutterView message handlers
BOOL PCutterView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	return CWnd::PreCreateWindow(cs);
}

int PCutterView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	m_pDC = new CClientDC(this); 
	m_hDC = m_pDC->GetSafeHdc();

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof( PIXELFORMATDESCRIPTOR ), 
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
	m_hRC = ::wglCreateContext(m_hDC); // basic operations are finished...

	::wglMakeCurrent(m_hDC, m_hRC);
	theCutterScene.InitScene();
	::wglMakeCurrent(NULL, NULL);
	return 0;
}

void PCutterView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	::wglMakeCurrent(m_hDC, m_hRC);
	{
		theCutterScene.SetupViewport(cx, cy);
	}	
	::wglMakeCurrent(NULL, NULL);
}


BOOL PCutterView::OnEraseBkgnd(CDC* pDC) 
{
	return true;
}

void PCutterView::OnPaint() 
{
	SetFocus();
	::wglMakeCurrent(m_hDC, m_hRC);
	{
		glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		theCutterScene.Render();
		theCutterEditor.Render();

		glFinish();
		SwapBuffers(m_hDC);

	}
	::wglMakeCurrent(NULL, NULL);	

	CWnd::OnPaint();
}

void PCutterView::OnDestroy() 
{
	CWnd::OnDestroy();

	::wglMakeCurrent(NULL, NULL);
	::wglDeleteContext(m_hRC);
	if (m_pDC)
		delete m_pDC;
	m_pDC = NULL;
}

void PCutterView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	::wglMakeCurrent(m_hDC, m_hRC);
	{
		theCutterEditor.LButtonDown(point.x, point.y, nFlags);
	}
	::wglMakeCurrent(NULL, NULL);

	Invalidate(false);
	CWnd::OnLButtonDown(nFlags, point);
}

void PCutterView::OnMouseMove(UINT nFlags, CPoint point) 
{
	::wglMakeCurrent(m_hDC, m_hRC);
	{
		theCutterScene.MouseMove(point.x, point.y);

		if (nFlags & MK_LBUTTON)
		{
			theCutterEditor.MouseMove(point.x, point.y, nFlags);
			if (m_pCutterDlg != NULL)
				m_pCutterDlg->Update3DCutCrv();
		}
	}
	::wglMakeCurrent(NULL, NULL);

	Invalidate(false);
	CWnd::OnMouseMove(nFlags, point);
}
void PCutterView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	::wglMakeCurrent(m_hDC, m_hRC);
	{
		theCutterEditor.LButtonUp(point.x, point.y, nFlags);
	}
	::wglMakeCurrent(NULL, NULL);

	Invalidate();
	CWnd::OnLButtonUp(nFlags, point);
}

void PCutterView::OnMButtonDown(UINT nFlags, CPoint point)
{
	::wglMakeCurrent(m_hDC, m_hRC);
	{
		theCutterScene.MButtonDown(point.x, point.y);
	}	
	::wglMakeCurrent(NULL, NULL);

	CWnd::OnMButtonDown(nFlags, point);
}

void PCutterView::OnMButtonUp(UINT nFlags, CPoint point)
{
	::wglMakeCurrent(m_hDC, m_hRC);
	{
		theCutterScene.MButtonUp(point.x, point.y);
	}
	::wglMakeCurrent(NULL, NULL);

	CWnd::OnMButtonUp(nFlags, point);
}

BOOL PCutterView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	::wglMakeCurrent(m_hDC, m_hRC);
	{
		theCutterScene.MouseWheel(zDelta);
	}
	::wglMakeCurrent(NULL, NULL);

	Invalidate(false);
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void PCutterView::OnEditCopy()
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
