/************************************************************************

@ Program title : VFX Tool
@ Project Name  : dgVFX_Solution
@ Program name  : AVI.h
@ Author        : 
@ Position      : Dongguk Univ. Graphics Lab. 

@ Last Update   : 2010. 3. 24
@ Last Version  : 0.1b

************************************************************************/
#ifndef _AVI_H
#define _AVI_H

#pragma comment(lib, "vfw32.lib")

// dgCommon.h �� �߰��Ǿ� �ִ� ������� �� �Ϻ�
#include <windows.h>
#include <string>
#include <time.h>
#include <vector>
#include <cmath>
#include <tchar.h>
#include <vfw.h>
#include <glut.h>

/*!
*	\class AVI
*	\brief AVI ������ �ε��ϱ� ���� Ŭ����
*
*	\author Gyu-Hyun Hwang 
*	\date 24 March 2010
*/
class AVI
{
public:
	// ������ �� �Ҹ���
	AVI();
	virtual ~AVI();

public:
	// ��� �Լ�
	bool InitAVI(LPCSTR _fileName, GLuint *TexId);
	bool OpenAVI(LPCSTR _fileName);
	void fliplt(void *_buffer);
	void BindNextTexture();

public:
	// ������ ���
	HDC					m_hDC;
	HDRAWDIB			m_hDIB;
	HBITMAP				m_hBitmap;
	PGETFRAME			m_pFrame;
	PAVISTREAM			m_pStream;
	int					m_MPF;
	int	m_CurFrame;
		
	// AVI Rendering ����	
	int m_TexWidth;
	int m_TexHeight;
	GLuint	m_BindTexId;
	unsigned char	*m_ImageData;
	long			m_StreamLength;
};

#endif	// _AVI_H