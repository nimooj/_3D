#include "stdafx.h"

static GLenum texFormat;

/*!
*	\brief 비트맵 파일을 로드한다.
*
*	\param filename 로드할 파일의 이름
*	\param info 비트맵 파일의 정보가 저장됨
*
*	\return 비트맵 파일의 이미지 데이터
*/
unsigned char *load_image_from_bmp(const char *filename, BITMAPINFO **info)
{
	FILE *fp;					/* Open file pointer */
    GLubyte *bits;				/* Bitmap pixel bits */
    unsigned int bitsize;		/* Size of bitmap */
    unsigned int infosize;		/* Size of header information */
    BITMAPFILEHEADER header;    /* File header */

    /* Try opening the file; use "rb" mode to read this *binary* file. */
    if ((fp = fopen(filename, "rb")) == NULL)
        return NULL;

    /* Read the file header and any following bitmap information... */
    if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1)
	{
        /* Couldn't read the file header - return NULL... */
		fclose(fp);
        return NULL;
	}

    if (header.bfType != 'MB')	/* Check for BM reversed... */
	{
		/* Not a bitmap file - return NULL... */
        fclose(fp);
        return NULL;
	}

    infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
    if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL)
	{
		/* Couldn't allocate memory for bitmap info - return NULL... */
        fclose(fp);
        return NULL;
	}

    if (fread(*info, 1, infosize, fp) < infosize)
	{
        /* Couldn't read the bitmap header - return NULL... */
        free(*info);
        fclose(fp);
        return NULL;
	}

    /* Now that we have all the header info read in, allocate memory for *
     * the bitmap and read *it* in...                                    */
    if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)  
		bitsize = ((*info)->bmiHeader.biWidth * (*info)->bmiHeader.biBitCount + 7) / 8 * abs((*info)->bmiHeader.biHeight);
	
	if ((bits = (GLubyte*)malloc(bitsize)) == NULL)
	{
        /* Couldn't allocate memory - return NULL! */
        free(*info);
        fclose(fp);
        return NULL;
	}

    if (fread(bits, 1, bitsize, fp) < bitsize)
	{
        /* Couldn't read bitmap - free memory and return NULL! */
        free(*info);
        free(bits);
        fclose(fp);
        return NULL;
	}

    /* OK, everything went fine - return the allocated bitmap... */
    fclose(fp);
    return bits;
}

/*!
*	\brief 비트맵 데이터를 파일로 저장한다.
*
*	\param filename 저장할 파일의 이름
*	\param info 비트맵 파일의 정보
*	\param bits 비트맵 데이터
*
*	\return 성공: 0, 실패: -1
*/
int save_image_to_bmp(const char *filename, BITMAPINFO *info, GLubyte *bits)
{
	FILE *fp;					/* Open file pointer */
    unsigned int size,			/* Size of file */
                 infosize,		/* Size of bitmap info */
                 bitsize;		/* Size of bitmap pixels */
    BITMAPFILEHEADER header;    /* File header */

    /* Try opening the file; use "wb" mode to write this *binary* file. */
    if ((fp = fopen(filename, "wb")) == NULL)
        return -1;

    /* Figure out the bitmap size */
    if (info->bmiHeader.biSizeImage == 0)
		bitsize = (info->bmiHeader.biWidth * info->bmiHeader.biBitCount + 7) / 8 * abs(info->bmiHeader.biHeight);
    else
		bitsize = info->bmiHeader.biSizeImage;

    /* Figure out the header size */
    infosize = sizeof(BITMAPINFOHEADER);
    switch (info->bmiHeader.biCompression)
	{
	case BI_BITFIELDS:
		infosize += 12; /* Add 3 RGB doubleword masks */
		if (info->bmiHeader.biClrUsed == 0)
			break;
	case BI_RGB:
		if (info->bmiHeader.biBitCount > 8 && info->bmiHeader.biClrUsed == 0)
			break;
	case BI_RLE8:
	case BI_RLE4:
		if (info->bmiHeader.biClrUsed == 0)
			infosize += (1 << info->bmiHeader.biBitCount) * 4;
	    else
			infosize += info->bmiHeader.biClrUsed * 4;
	    break;
	}

    size = sizeof(BITMAPFILEHEADER) + infosize + bitsize;

    /* Write the file header, bitmap information, and bitmap pixel data... */
    header.bfType      = 'MB'; /* Non-portable... sigh */
    header.bfSize      = size;
    header.bfReserved1 = 0;
    header.bfReserved2 = 0;
    header.bfOffBits   = sizeof(BITMAPFILEHEADER) + infosize;

    if (fwrite(&header, 1, sizeof(BITMAPFILEHEADER), fp) < sizeof(BITMAPFILEHEADER))
	{
        /* Couldn't write the file header - return... */
        fclose(fp);
        return -1;
	}

    if (fwrite(info, 1, infosize, fp) < infosize)
	{
        /* Couldn't write the bitmap header - return... */
        fclose(fp);
        return -1;
	}

    if (fwrite(bits, 1, bitsize, fp) < bitsize)
	{
        /* Couldn't write the bitmap - return... */
        fclose(fp);
        return -1;
	}

    /* OK, everything went fine - return... */
    fclose(fp);
    return 0;
}

unsigned char *getRGBA (FILE *s, int size)
{
	unsigned char *rgba;
	unsigned char temp;
	int bread;
	int i;

	rgba = (unsigned char*)malloc (size * 4); 

	if (rgba == NULL)
		return 0;

	bread = (int)fread(rgba, sizeof(unsigned char), size * 4, s); 

	/* TGA is stored in BGRA, make it RGBA */
	if (bread != size * 4)
	{
		free (rgba);
		return 0;
	}

	for (i = 0; i < size * 4; i += 4 )
	{
		temp = rgba[i];
		rgba[i] = rgba[i + 2];
		rgba[i + 2] = temp;
	}

	texFormat = GL_RGBA;
	return rgba;
}

unsigned char *getRGB (FILE *s, int size)
{
	unsigned char *rgb;
	unsigned char temp;
	int bread;
	int i;

	rgb = (unsigned char*)malloc (size * 3); 

	if (rgb == NULL)
		return 0;

	bread = (int)fread(rgb, sizeof (unsigned char), size * 3, s);

	if (bread != size * 3)
	{
		free (rgb);
		return 0;
	}

	/* TGA is stored in BGR, make it RGB */
	for (i = 0; i < size * 3; i += 3)
	{
		temp = rgb[i];
		rgb[i] = rgb[i + 2];
		rgb[i + 2] = temp;
	}

	texFormat = GL_RGB;

	return rgb;
}

unsigned char *getGray (FILE *s, int size)
{
	unsigned char *grayData;
	int bread;

	grayData = (unsigned char*)malloc (size);

	if (grayData == NULL)
		return 0;

	bread = (int)fread(grayData, sizeof (unsigned char), size, s);

	if (bread != size)
	{
		free (grayData);
		return 0;
	}

	texFormat = GL_ALPHA;

	return grayData;
}

unsigned char *getData (FILE *s, int sz, int iBits)
{
	if (iBits == 32)
		return getRGBA (s, sz);
	else if (iBits == 24)
		return getRGB (s, sz);	
	else if (iBits == 8)
		return getGray (s, sz);

	return getGray (s, sz);
}

/*!
*	\brief TGA 파일을 로드한다.
*
*	\param filename 로드할 파일의 이름
*
*	\return 비트맵 파일의 이미지 데이터
*/
unsigned char *load_image_from_tga(const char *name, int *width, int *height, GLenum *format)
{
	FILE *fp;
	fopen_s(&fp, name, "r+bt" );

	unsigned char type[4];
	unsigned char info[7];
	unsigned char *imageData = NULL;
	int imageWidth, imageHeight;
	int imageBits, size;

	fread(&type, sizeof (char), 3, fp); // read in colormap info and image type, byte 0 ignored
	fseek(fp, 12, SEEK_SET);			// seek past the header and useless info
	fread(&info, sizeof (char), 6, fp);

	imageWidth = info[0] + info[1] * 256; 
	imageHeight = info[2] + info[3] * 256;
	imageBits =	info[4];
	size = imageWidth * imageHeight; 

	/* make sure we are loading a supported type */
	if (imageBits != 32 && imageBits != 24 && imageBits != 8)
		return NULL;

	imageData = getData(fp, size, imageBits);

	fclose(fp);

	*width = imageWidth;
	*height = imageHeight;
	*format = texFormat;

	return imageData;
}

/////////////////////////////////////////////////////////////////////
AVI::AVI()
{
	m_hDC = CreateCompatibleDC(0);
	m_hBitmap = NULL;
	m_hDIB = NULL;
	m_pFrame = NULL;
	m_pStream = NULL;
	m_ImageData = NULL;
	m_BindTexId	= 0;
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