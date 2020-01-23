#ifndef _GSCENE_IMAGE_H
#define _GSCENE_IMAGE_H

#include <windows.h>
#include <wingdi.h>
#include <vfw.h>
#pragma comment(lib, "vfw32.lib")

// BMP
unsigned char *load_image_from_bmp(const char *filename, BITMAPINFO **info);
int save_image_to_bmp(const char *filename, BITMAPINFO *info, GLubyte *bits);

// TGA
unsigned char *load_image_from_tga(const char *, int *, int *, GLenum *);

/*!
*	\class AVI
*	\brief AVI 파일을 로드하기 위한 클래스
*
*	\author Gyu-Hyun Hwang 
*	\date 24 March 2010
*/
class AVI
{
public:
	// 생성자 및 소멸자
	AVI();
	virtual ~AVI();

public:
	// 멤버 함수
	bool InitAVI(LPCSTR _fileName, GLuint *TexId);
	bool OpenAVI(LPCSTR _fileName);
	void fliplt(void *_buffer);
	void BindNextTexture();

public:
	// 데이터 멤버
	HDC					m_hDC;
	HDRAWDIB			m_hDIB;
	HBITMAP				m_hBitmap;
	PGETFRAME			m_pFrame;
	PAVISTREAM			m_pStream;
	int					m_MPF;
	int	m_CurFrame;

	// AVI Rendering 관련	
	int m_TexWidth;
	int m_TexHeight;
	GLuint m_BindTexId;
	unsigned char *m_ImageData;
	long m_StreamLength;
};



#endif // _GSCENE_IMAGE_H