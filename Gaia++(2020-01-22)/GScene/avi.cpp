/************************************************************************

@ Program title : VFX Tool
@ Project Name  : dgVFX_Solution
@ Program name  : AVI.cpp
@ Author        : Gyu-Hyun Hwang
@ Position      : Dongguk Univ. Graphics Lab. 

@ Last Update   : 2010. 3. 24
@ Last Version  : 0.1b

************************************************************************/
#include "..\usr\include\avi.h"

AVI::AVI()
{
	m_hDC = CreateCompatibleDC(0);
	m_hBitmap = NULL;
	m_hDIB = NULL;
	m_pFrame = NULL;
	m_pStream = NULL;
	m_ImageData = NULL;
	m_BindTexId	= -1;
	m_StreamLength = 0;
	m_TexWidth = 0;
	m_TexHeight = 0;
	m_CurFrame = 0;
}

AVI::~AVI()
{
	if (m_hBitmap != NULL)	
		DeleteObject(m_hBitmap);

	if (m_hDIB != NULL)	
		DrawDibClose(m_hDIB);

	if (m_pFrame != NULL)
		AVIStreamGetFrameClose(m_pFrame);

	if (m_pStream != NULL)
		AVIStreamRelease(m_pStream);

	//if (m_BindTexId != NULL)
	//	glDeleteTextures(1, &m_BindTexId);

	AVIFileExit(); // Exit the AVI Files		
}

void AVI::fliplt(void *_buffer)
{
	void* b = _buffer;	// Pointer To The Buffer
	__asm				// Assembler Code To Follow
	{
		mov ecx, m_TexWidth * m_TexHeight	// Counter Set To Dimensions Of Our Memory Block
		mov ebx, b		// Points ebx To Our Data (b)
label:				// Label Used For Looping
		mov al,[ebx+0]	// Loads Value At ebx Into al
		mov ah,[ebx+2]	// Loads Value At ebx+2 Into ah
		mov [ebx+2],al	// Stores Value In al At ebx+2
			mov [ebx+0],ah	// Stores Value In ah At ebx

			add ebx,3		// Moves Through The Data By 3 Bytes
			dec ecx			// Decreases Our Loop Counter
			jnz label		// If Not Zero Jump Back To Label
	}	
}

bool AVI::InitAVI(LPCSTR _fileName, GLuint *TexId)
{
	m_hDIB = DrawDibOpen();

	// Open AVI
	if (!OpenAVI(_fileName)) 
		return false;
		
	// Enable Texture Mapping
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &m_BindTexId);
	glBindTexture(GL_TEXTURE_2D, m_BindTexId);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_TexWidth, m_TexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glDisable(GL_TEXTURE_2D);

	BindNextTexture();
	*TexId = m_BindTexId;

	return true;
}

bool AVI::OpenAVI(LPCSTR _fileName)
{
	// AVI File initialize
	AVIFileInit(); 

	// Open the AVI Stream
	if (AVIStreamOpenFromFile(&m_pStream, _fileName, streamtypeVIDEO, 0, OF_READ, NULL) != 0) 
		return false;
	
	AVISTREAMINFO StreamInfo;
	BITMAPINFOHEADER BitmapInfo;		

	AVIStreamInfo(m_pStream, &StreamInfo, sizeof(StreamInfo));
	m_TexWidth = StreamInfo.rcFrame.right - StreamInfo.rcFrame.left;
	m_TexHeight = StreamInfo.rcFrame.bottom - StreamInfo.rcFrame.top;
	m_StreamLength = AVIStreamLength(m_pStream);
	m_MPF = AVIStreamSampleToTime(m_pStream, m_StreamLength) / m_StreamLength; // calculate rough milliseconds per frame

	BitmapInfo.biSize = sizeof(BITMAPINFOHEADER);
	BitmapInfo.biPlanes = 1;
	BitmapInfo.biBitCount = 24;
	BitmapInfo.biWidth = m_TexWidth;
	BitmapInfo.biHeight = m_TexHeight;
	BitmapInfo.biCompression = BI_RGB;

	m_hBitmap = CreateDIBSection(m_hDC, (BITMAPINFO *)(&BitmapInfo), DIB_RGB_COLORS, (void **)(&m_ImageData), NULL, NULL);
	SelectObject(m_hDC, m_hBitmap);

	m_pFrame = AVIStreamGetFrameOpen(m_pStream, NULL);
	if (m_pFrame == NULL)
		return false;

	return true;
}

void AVI::BindNextTexture()
{
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER) AVIStreamGetFrame(m_pFrame, m_CurFrame); // Grab data from avi stream
	char *m_pData = (char *)lpbi + lpbi->biSize + lpbi->biClrUsed * sizeof(RGBQUAD);	// Pointer to data returned by avi stream

	// convert data to requested bitmap format
	DrawDibDraw(m_hDIB, m_hDC, 0, 0, m_TexWidth, m_TexHeight, lpbi, m_pData, 0, 0, m_TexWidth, m_TexHeight, 0);
	fliplt(m_ImageData);

	// Update Scene
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_BindTexId);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_TexWidth, m_TexHeight, GL_RGB, GL_UNSIGNED_BYTE, m_ImageData);
	glDisable(GL_TEXTURE_2D);

	m_CurFrame++;
	if (m_CurFrame >= m_StreamLength)
		m_CurFrame = 0;
}