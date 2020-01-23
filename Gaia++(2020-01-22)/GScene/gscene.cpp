#include "stdafx.h"

GScene *theScene = NULL;

/////////////////////////////////////////////////////////////////
/*!
*	\brief 생성자
*
*	\param _LightNum 생성할 광원의 번호
*/
GLight::GLight(GLenum _LightNum)
{
	LightNum = _LightNum;
	ARR_ZERO(Position, 4);
	ARR_ZERO(Ambient, 3);
	ARR_ZERO(Diffuse, 3);
	ARR_ZERO(Specular, 3);
	ARR_ZERO(SpotDirection, 3);
	IsEnabled = true;
	IsSpotLight = false;
	SpotCutOff = 45.0f;
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사될 객체
*/
GLight::GLight(const GLight &cpy)
{
	LightNum = cpy.LightNum;
	ARR_COPY(Position, cpy.Position, 4);
	ARR_COPY(Ambient, cpy.Ambient, 3);
	ARR_COPY(Diffuse, cpy.Diffuse, 3);
	ARR_COPY(Specular, cpy.Specular, 3);
	ARR_COPY(SpotDirection, cpy.SpotDirection, 3);
	IsEnabled = cpy.IsEnabled;
	IsSpotLight = cpy.IsSpotLight;
	SpotCutOff = cpy.SpotCutOff;
}

/*!
*	\brief 소멸자
*/
GLight::~GLight()
{
}

/*!
*	\brief 대입연산자
*
*	\param rhs 피연산자
*
*	\return 대입된 자신을 반환
*/
GLight &GLight::operator =(const GLight &rhs)
{
	LightNum = rhs.LightNum;
	ARR_COPY(Position, rhs.Position, 4);
	ARR_COPY(Ambient, rhs.Ambient, 3);
	ARR_COPY(Diffuse, rhs.Diffuse, 3);
	ARR_COPY(Specular, rhs.Specular, 3);
	ARR_COPY(SpotDirection, rhs.SpotDirection, 3);
	IsEnabled = rhs.IsEnabled;
	IsSpotLight = rhs.IsSpotLight;
	SpotCutOff = rhs.SpotCutOff;

	return *this;
}

/*!
*	\brief 광원을 활성화 한다
*/
void GLight::Enable()
{
	glEnable(LightNum);
	glLightfv(LightNum, GL_POSITION, Position);
	glLightfv(LightNum, GL_AMBIENT, Ambient);
	glLightfv(LightNum, GL_DIFFUSE, Diffuse);
	glLightfv(LightNum, GL_SPECULAR, Specular);

	if (IsSpotLight)
	{
		glLightf(LightNum, GL_SPOT_CUTOFF, SpotCutOff);
		glLightfv(LightNum, GL_SPOT_DIRECTION, SpotDirection);
	}

	IsEnabled = true;
}

/*!
*	\brief 광원을 비활성화 한다
*/
void GLight::Disable()
{
	glDisable(LightNum);

	IsEnabled = false;
}

/*!
*	\brief 광원의 위치를 설정한다.
*
*	\param x 광원의 x좌표
*	\param y 광원의 y좌표
*	\param z 광원의 z좌표
*	\param w 광원의 w좌표(1: 점광원, 0: 방향성광원)
*/
void GLight::SetPosition(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	Position[0] = x;
	Position[1] = y;
	Position[2] = z;
	Position[3] = w;
}

/*!
*	\brief 광원의 주변광 성분을 설정한다.
*
*	\param r 광원의 붉은색 주변광 성분
*	\param g 광원의 초록색 주변광 성분
*	\param b 광원의 파란색 주변광 성분
*/
void GLight::SetAmbient(GLfloat r, GLfloat g, GLfloat b)
{
	Ambient[0] = r;
	Ambient[1] = g;
	Ambient[2] = b;
}

/*!
*	\brief 광원의 난반사광 성분을 설정한다.
*
*	\param r 광원의 붉은색 난반사광 성분
*	\param g 광원의 초록색 난반사광 성분
*	\param b 광원의 파란색 난반사광 성분
*/
void GLight::SetDiffuse(GLfloat r, GLfloat g, GLfloat b)
{
	Diffuse[0] = r;
	Diffuse[1] = g;
	Diffuse[2] = b;
}

/*!
*	\brief 광원의 전반사광 성분을 설정한다.
*
*	\param r 광원의 붉은색 전반사광 성분
*	\param g 광원의 초록색 전반사광 성분
*	\param b 광원의 파란색 전반사광 성분
*/
void GLight::SetSpecular(GLfloat r, GLfloat g, GLfloat b)
{
	Specular[0] = r;
	Specular[1] = g;
	Specular[2] = b;
}

/*!
*	\brief 광원을 점광원 형태로 설정한다.
*/
void GLight::SetPointLight()
{
	Position[3] = 1.0f;
}

/*!
*	\brief 광원을 방향성 광원 형태로 설정한다.
*/
void GLight::SetDirectionalLight()
{
	Position[3] = 0.0f;
}

/*!
*	\brief 집중광원의 방향과 범위를 설정한다.
*
*	\param vx 집중광원의 방향의 x성분
*	\param vy 집중광원의 방향의 y성분
*	\param vz 집중광원의 방향의 z성분
*	\param cutoff 집중광원의 범위
*/
void GLight::SetSpotLight(GLfloat vx, GLfloat vy, GLfloat vz, GLfloat cutoff)
{
	IsSpotLight = true;
	SpotDirection[0] = vx;
	SpotDirection[1] = vy;
	SpotDirection[2] = vz;
	SpotCutOff = cutoff;
}

////////////////////////////////////////////////////////////////////////
/*!
*	\brief 디폴트 생성자
*/
GCamera::GCamera()
{
	pScene = NULL;
	Fovy = 30.0;
	Near = 0.1;
	Far = 1000000.0;
	ViewWidth = 1024;
	ViewHeight = 768;
	ZoomDist = -100.0;	
	MousePt[0] = 0;
	MousePt[1] = 0;
	CameraType = PROJECTION_PERS;
	ArcBallType = ARCBALL_FREE;
	CameraManipulateType = CAMERA_MANIPULATE_NONE;
	memset(ModelviewMat, 0, sizeof(double) * 16);
	memset(ProjectionMat, 0, sizeof(double) * 16);
	memset(Viewport, 0, sizeof(int) * 4);
	pPixelBuf = NULL;
}

/*!
*	\brief 생성자
*
*	\param _pScene 카메라가 포한된 장면에 대한 포인터
*	\param _Fovy 원근 투영을 위한 카메라의 화각
*	\param _Near 투영 공간을 정의하는 가까운 평면까지의 거리
*	\param _Far 투영 공간을 정의하는 먼 평면까지의 거리
*	\param _ViewWidth 뷰포트 변환에 사용될 너비
*	\param _ViewHeight 뷰포트 변환에 사용될 높이
*/
GCamera::GCamera(GScene *_pScene, double _Fovy, double _Near, double _Far, int _ViewWidth, int _ViewHeight)
{
	pScene = _pScene;
	Fovy = _Fovy;
	Near = _Near;
	Far = _Far;
	ViewWidth = _ViewWidth;
	ViewHeight = _ViewHeight;
	ZoomDist = -100.0;
	MousePt[0] = 0;
	MousePt[1] = 0;
	CameraType = PROJECTION_PERS;
	ArcBallType = ARCBALL_FREE;
	CameraManipulateType = CAMERA_MANIPULATE_NONE;
	memset(ModelviewMat, 0, sizeof(double) * 16);
	memset(ProjectionMat, 0, sizeof(double) * 16);
	memset(Viewport, 0, sizeof(int) * 4);
	pPixelBuf = NULL;
}

/*!
*	\brief 생성자
*
*	\param _pScene 카메라가 포한된 장면에 대한 포인터
*	\param Eye 월드 좌표계에서 표현된 카메라 원점의 위치
*	\param Center 카메라가 바라보는 장면 좌표계의 한 점
*	\param Up 월드 좌표계에서 표현된 카메라의 위쪽 방향 벡터
*/
GCamera::GCamera(GScene *_pScene, GVector3 Eye, GVector3 Center, GVector3 Up)
{
	pScene = _pScene;
	Fovy = 30.0;
	Near = 0.1;
	Far = 1000000.0;
	ViewWidth = 1024;
	ViewHeight = 768;
	MousePt[0] = 0;
	MousePt[1] = 0;
	CameraType = PROJECTION_PERS;
	ArcBallType = ARCBALL_FREE;
	CameraManipulateType = CAMERA_MANIPULATE_NONE;

	Up.Normalize();		// Y-axis
	GVector3 z_axis = (Eye - Center).Normalize(); // Z-axis
	GVector3 x_axis = Up ^ z_axis;

	GQuater q;
	q.SetFromFrameZX(z_axis, x_axis);

	GTransf X(Eye, q);
	X = inv(X);

	ZoomDist = -norm(part_trans(X));
	Rot = part_rot(X);
	Pan.Set(0.0, 0.0, 0.0);

	memset(ModelviewMat, 0, sizeof(double) * 16);
	memset(ProjectionMat, 0, sizeof(double) * 16);
	memset(Viewport, 0, sizeof(int) * 4);
	pPixelBuf = NULL;
}

/*!
*	\brief 생성자
*
*	\param _pScene 카메라가 포한된 장면에 대한 포인터
*	\param Left 왼쪽 clipping 평면
*	\param Right 오른쪽clipping 평면
*	\param Bottom 아래쪽 clipping 평면
*	\param Top 위쪽 clipping 평면
*/
GCamera::GCamera(GScene *_pScene, double _Left, double _Right, double _Bottom, double _Top)
{
	pScene = _pScene;
	Fovy = 30.0;
	Near = 0.1;
	Far = 1000000.0;
	ViewWidth = 100;
	ViewHeight = 100;
	ZoomDist = -(_Top - _Bottom) / (2.0 * (GLdouble)tan(Fovy * M_PI / 360.0));
	Pan.Set(-(_Left + _Right) / 2.0, -(_Bottom + _Top) / 2.0, 0.0);
	MousePt[0] = 0;
	MousePt[1] = 0;
	CameraType = PROJECTION_ORTHO;
	ArcBallType = ARCBALL_FREE;
	CameraManipulateType = CAMERA_MANIPULATE_NONE;
	memset(ModelviewMat, 0, sizeof(double) * 16);
	memset(ProjectionMat, 0, sizeof(double) * 16);
	memset(Viewport, 0, sizeof(int) * 4);
	pPixelBuf = NULL;
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사될 카메라 객체
*/
GCamera::GCamera(const GCamera &cpy)
{
	pScene = cpy.pScene;
	Fovy = cpy.Fovy;
	Near = cpy.Near;
	Far = cpy.Far;
	ViewWidth = cpy.ViewWidth;
	ViewHeight = cpy.ViewHeight;
	ZoomDist = cpy.ZoomDist;
	ARR_COPY(MousePt, cpy.MousePt, 2);
	Rot = cpy.Rot;
	Pan = cpy.Pan;
	WC = cpy.WC;
	CameraType = cpy.CameraType;
	ArcBallType = cpy.ArcBallType;
	CameraManipulateType = cpy.CameraManipulateType;

	memcpy(ModelviewMat, cpy.ModelviewMat, sizeof(double) * 16);
	memcpy(ProjectionMat, cpy.ProjectionMat, sizeof(double) * 16);
	memcpy(Viewport, cpy.Viewport, sizeof(int) * 4);
	pPixelBuf = NULL;
}

/*!
*	\brief 소멸자
*/
GCamera::~GCamera()
{
	if (pPixelBuf != NULL)
		delete [] pPixelBuf;
	pPixelBuf = NULL;
}

/*!
*	\brief 대입연산자
*
*	\param rhs 대입될 피연산자
*/
GCamera &GCamera::operator =(const GCamera &rhs)
{
	pScene = rhs.pScene;
	Fovy = rhs.Fovy;
	Near = rhs.Near;
	Far = rhs.Far;
	ViewWidth = rhs.ViewWidth;
	ViewHeight = rhs.ViewHeight;
	ZoomDist = rhs.ZoomDist;
	ARR_COPY(MousePt, rhs.MousePt, 2);
	Rot = rhs.Rot;
	Pan = rhs.Pan;
	WC = rhs.WC;
	CameraType = rhs.CameraType;
	ArcBallType = rhs.ArcBallType;
	CameraManipulateType = rhs.CameraManipulateType;
	memcpy(ModelviewMat, rhs.ModelviewMat, sizeof(double) * 16);
	memcpy(ProjectionMat, rhs.ProjectionMat, sizeof(double) * 16);
	memcpy(Viewport, rhs.Viewport, sizeof(int) * 4);
	pPixelBuf = NULL;

	return *this;
}

/*!
*	\brief 카메라의 투영공간을 설정한다.
*/
void GCamera::SetupViewFrustum()
{
	double Aspect = (double)ViewWidth / (double)ViewHeight;
	if (CameraType == PROJECTION_PERS)
	{
		gluPerspective(Fovy, Aspect, Near, Far);
	}
	else
	{
		GLdouble ViewSize = -ZoomDist * (GLdouble)tan(Fovy * M_PI / 360.0); 
		if (Aspect > 1.0)
			glOrtho(-ViewSize * Aspect, ViewSize * Aspect, -ViewSize, ViewSize, -10.0, 10000.0);
		else
			glOrtho(-ViewSize, ViewSize, -ViewSize / Aspect, ViewSize / Aspect, -10.0, 10000.0);
	}

	// 투영 행렬 정보 얻기
	glGetDoublev(GL_PROJECTION_MATRIX, ProjectionMat);
}

/*!
*	\brief 카메라의 관측변환을 설정한다.
*/
void GCamera::SetupViewTransform()
{
	// 변환 정보 계산
	WC = GTransf(GVector3(0.0, 0.0, ZoomDist), Rot) * GTransf(Pan);
	
	// 변환 정보 적용
	glMultMatrixd(cast_arr(cast_mat(WC)));

	// 모델뷰 행렬 정보 얻기
	glGetDoublev(GL_MODELVIEW_MATRIX, ModelviewMat);
}

/*!
*	\brief 카메라의 뷰포트 변환정보를 설정한다.
*
*	\param w 뷰포트 변화의 너비 정보
*	\param h 뷰포트 변환의 높이 정보
*/
void GCamera::SetupViewport(int w, int h)
{
	ViewWidth = w;
	ViewHeight = h;

// 	if (pPixelBuf != NULL)
// 		delete pPixelBuf;
// 	pPixelBuf = new unsigned char [w * h * 3];

	glViewport(0, 0, ViewWidth, ViewHeight);

	// 뷰포트 정보 얻기
	glGetIntegerv(GL_VIEWPORT, Viewport);
}

/*!
*	\brief 카메라의 팬 기능을 수행한다.
*/
void GCamera::Panning(int x, int y)
{
	GVector3 off((MousePt[0] - x) * ZoomDist * 0.001, (y - MousePt[1]) * ZoomDist * 0.001, 0.0);
	off = inv(Rot) * off;
	Pan += off;

	MousePt[0] = x;
	MousePt[1] = y;
}

/*!
*	\brief 카메라의 회전 기능을 수행한다.
*/
void GCamera::Rotating(int x, int y)
{
	GVector3 p((2.0 * MousePt[0] - ViewWidth) / ViewWidth, (-2.0 * MousePt[1] + ViewHeight) / ViewHeight, 0.0);

	double r = norm(p);
	if (r >= 1.0)
		p.Normalize();
	else
		p[2] = SQRT(1.0 - r);

	GVector3 q((2.0 * x - ViewWidth) / ViewWidth, (-2.0 * y + ViewHeight) / ViewHeight, 0.0);

	r = norm(q);
	if (r >= 1.0)
		q.Normalize();
	else
		q[2] = SQRT(1.0 - r);

	GQuater dq;
	dq.SetFromArcBall(p, q, ArcBallType);
	Rot = dq * Rot;
	Rot = dq * Rot;

	MousePt[0] = x;
	MousePt[1] = y;

	//theCtrlPanel.Parallel = false;
}

/*!
*	\brief 카메라의 줌 인/아웃 기능을 수행한다.
*
*	\param type 양수: 줌 인, 음수: 줌 아수
*/
void GCamera::Zooming(int type)
{
	double dz = (type > 0) ? 0.05 : -0.05;
	ZoomDist += ZoomDist * dz;
}

/*!
*	\brief 현재 모드에 따라 카메라의 기능을 제어한다.
*
*	\param x 뷰포트에서 마우스의 현재 위치의 x좌표
*	\param y 뷰포트에서 마우스의 현재 위치의 y좌표
*/
void GCamera::LButtonDown(int x, int y)
{
	MousePt[0] = x;
	MousePt[1] = y;
}

/*!
*	\brief 현재 모드에 따라 카메라의 기능을 제어한다.
*
*	\param x 뷰포트에서 마우스의 현재 위치의 x좌표
*	\param y 뷰포트에서 마우스의 현재 위치의 y좌표
*/
void GCamera::MouseMove(int x, int y)
{
	switch (CameraManipulateType)
	{
	case CAMERA_MANIPULATE_ROTATE:
		Rotating(x, y);
		pScene->HeadUpDisplay.Parallel = false;
		break;

	case CAMERA_MANIPULATE_PAN:
		Panning(x, y);
		break;
	}
}

/*!
*	\brief 카메라 제어 인터페이스를 렌더링 한다.
*/
void GCamera::Render()
{
}

/*!
*	\brief 카메라의 변환 정보를 출력한다.
*
*	\param fname 카메라의 변환 정보가 출력될 파일명
*/
void GCamera::Export(char *fname)
{
	FILE *fp = fopen(fname, "w");
	fprintf(fp, "%d\n", CameraType);	// 카메라의 투영 타입
	fprintf(fp, "%lf\n", Fovy);			// 원근 투영을 위한 카메라의 화각(field of view)
	fprintf(fp, "%lf\n", Near);			// 투영 공간의 앞쪽 평면까지의 거리(>0)
	fprintf(fp, "%lf\n", Far);			// 투영 공간의 뒤쪽 평면까지의 거리(>0)
	fprintf(fp, "%lf\n", ZoomDist);		// 카메라 중심에서 월드 좌표계까지의 거리(<0)
	fprintf(fp, "%d\n", ViewWidth);		// 뷰포트 변환에 사용될 너비
	fprintf(fp, "%d\n", ViewHeight);	// 뷰포트 변환에 사용될 높이
	fprintf(fp, "%lf\t%lf\t%lf\t%lf\n", Rot.W, Rot.X, Rot.Y, Rot.Z);	// 월드 좌표계의 회전 정보
	fprintf(fp, "%lf\t%lf\t%lf\n", Pan[0], Pan[1], Pan[2]);				// 월드 좌표계의 이동 정보
	fclose(fp);
}

/*!
*	\brief 카메라의 변환 정보를 읽어온다.
*
*	\param fname 카메라의 변환 정보가 포함된 파일명
*/
void GCamera::Import(char *fname)
{
	FILE *fp = fopen(fname, "r");
	int tmp;

	fscanf(fp, "%d", &tmp);			// 카메라의 투영 타입
	CameraType = (tmp == 0) ? PROJECTION_ORTHO : PROJECTION_PERS;
	fscanf(fp, "%lf", &Fovy);		// 원근 투영을 위한 카메라의 화각(field of view)
	fscanf(fp, "%lf", &Near);		// 투영 공간의 앞쪽 평면까지의 거리(>0)
	fscanf(fp, "%lf", &Far);		// 투영 공간의 뒤쪽 평면까지의 거리(>0)
	fscanf(fp, "%lf", &ZoomDist);	// 카메라 중심에서 월드 좌표계까지의 거리(<0)
	fscanf(fp, "%d", &ViewWidth);	// 뷰포트 변환에 사용될 너비
	fscanf(fp, "%d", &ViewHeight);	// 뷰포트 변환에 사용될 높이
	fscanf(fp, "%lf%lf%lf%lf", &Rot.W, &Rot.X, &Rot.Y, &Rot.Z);		// 월드 좌표계의 회전 정보
	fscanf(fp, "%lf%lf%lf", &Pan[0], &Pan[1], &Pan[2]);				// 월드 좌표계의 이동 정보 
	Rot.Normalize();

	fclose(fp);
}

/*!
*	\brief 카메라의 회전 타입을 설정한다 (토글기능을 포함한다).
*
*	\param fname 카메라의 설정될 회전 타입
*/
void GCamera::SetArcBallType(TypeArcBall type)
{
	if (type == ArcBallType)
		ArcBallType = ARCBALL_FREE;
	else
		ArcBallType = type;
}

/*!
*	\brief 월드 좌표계에서 표현된 투영점의 3차원 좌표를 계산한다.
*
*	\param wx 투영점의 스크린 x좌표(in)
*	\param wy 투영점의 스크린 y좌표(in)
*	\param depth 투영점의 깊이(0.0 ~ 1.0) 정보(in)
*	\param ox 투영되기전, 월드 좌표계에서의 정점의 x좌표가 저장됨(out)
*	\param oy 투영되기전, 월드 좌표계에서의 정점의 y좌표가 저장됨(out)
*	\param oz 투영되기전, 월드 좌표계에서의 정점의 z좌표가 저장됨(out)
*
*	\note 윈도우 좌표는 좌상단이 (0, 0)
*/
void GCamera::GetWorldCoord(double wx, double wy, double depth, double *ox, double *oy, double *oz)
{
	::gluUnProject(wx, Viewport[3] - wy, depth, ModelviewMat, ProjectionMat, Viewport, ox, oy, oz);	
}

/*!
*	\brief 스크린 좌표계에서 표현된 투영점의 좌표계를 계산한다.
*
*	\param pVert 객체의 정점에 대한 포인터(in)
*	\param wx 스크린 x좌표(out)
*	\param wy 스크린 y좌표(out)
*	\param wz 스크린 depth(0.0 ~ 1.0) 좌표(out)
*
*	\note 스크린 좌표는 좌상단이 (0, 0)
*/
void GCamera::GetScreenCoord(GVertex *pVert, double *wx, double *wy, double *wz)
{
	// 투영점 계산하기
	GPoint3 p = pVert->pMesh->MC * pVert->P;
	gluProject(p[0], p[1], p[2], ModelviewMat, ProjectionMat, Viewport, wx, wy, wz);
	*wy = Viewport[3] - *wy;
}

/*!
*	\brief 스크린 좌표계에서 투영된 제어점의 좌표를 계산한다.
*
*	\param pPt 제어점에 대한 포인터
*	\param wx 스크린 x좌표
*	\param wy 스크린 y좌표
*	\param wz 스크린 depth좌표
*
*	\note 스크린 좌표는 좌상단이 (0, 0)
*/
void GCamera::GetScreenCoord(GCtrlPt *pPt, double *wx, double *wy, double *wz)
{
	// 투영점 계산하기
	GPoint3 p = pPt->pMesh->MC * GPoint3((*pPt)[0], (*pPt)[1], (*pPt)[2]);
	gluProject(p[0], p[1], p[2], ModelviewMat, ProjectionMat, Viewport, wx, wy, wz);
	*wy = Viewport[3] - *wy;
}

/*!
*	\brief 스크린 좌표계에서 표현된 투영점의 좌표를 계산한다.
*
*	\param ox 3D 정점의 x좌표
*	\param oy 3D 정점의 y좌표
*	\param oz 3D 정점의 z좌표
*	\param wx 스크린 x좌표
*	\param wy 스크린 y좌표
*	\param wz 스크린 depth좌표
*
*	\note 스크린 좌표는 좌상단이 (0, 0)
*/
void GCamera::GetScreenCoord(double ox, double oy, double oz, double *wx, double *wy, double *wz)
{
	// 투영점 계산하기
	gluProject(ox, oy, oz, ModelviewMat, ProjectionMat, Viewport, wx, wy, wz);
	*wy = Viewport[3] - *wy;
}

/*!
*	\brief 월드 좌표계에서 표현된 스크린에 수직한 직선의 방정식을 계산한다.
*
*	\param x 윈도우 x 좌표
*	\param y 윈도우 y 좌표
*
*	\return 월드 좌표계에서 표현된 스크린에 수직한 직선의 방정식을 반환한다.
*/
GLine GCamera::GetOrthoRay(double x, double y)
{
	GPoint3 p, q;		
	GetWorldCoord(x, y, 0.0, &p[0], &p[1], &p[2]);
	GetWorldCoord(x, y, 0.5, &q[0], &q[1], &q[2]);
	
	return GLine(p, q);
}

/*!
*	\brief 월드 좌표계에서 표현된 관측 방향 벡터를 반환한다.
*
*	\return 월드 좌표계에서 표현된 관측 방향 벡터를 반환한다.
*/
GVector3 GCamera::GetViewDirection()
{
	return inv(Rot) * GVector3(0.0, 0.0, -1.0);
}

////////////////////////////////////////
/*!
*	\brief 생성자
*
*	\param fname 장면의 파일명
*/
GScene::GScene(const std::string fname)
{	
	FileName = fname;
	hWndView = NULL;
	bShowFloor = true;
	bShowBkg = true;
	bTimer = false;
	pRecentMesh = NULL;
	CtrlPtSize = 0.0;
}

/*!
*	\brief 복사생성자
*
*	\param cpy 복사될 모델 객체
*/
GScene::GScene(const GScene &cpy)
{
	FileName = cpy.FileName;
	RootDir = cpy.RootDir;
	hWndView = cpy.hWndView;
	LightList = cpy.LightList;

	MainCamera = cpy.MainCamera;
	SrcCamera = cpy.SrcCamera;
	TarCamera = cpy.TarCamera;
	pRecentMesh = cpy.pRecentMesh;
	AuxCameraList = cpy.AuxCameraList;

	bShowFloor = cpy.bShowFloor;
	bShowBkg = cpy.bShowBkg;
	bTimer = cpy.bTimer;
	CtrlPtSize = cpy.CtrlPtSize;

	// 메쉬 리스트 복사
	int NumMesh = (int)cpy.MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
	{
		GMesh *pMesh = new GMesh((*cpy.MeshList[i]));
		MeshList.push_back(pMesh);
	}
}

/*!
*	\brief 소멸자
*/
GScene::~GScene()
{
	int NumMesh = (int)MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
	{
		delete MeshList[i];
	}
	MeshList.clear();
}

/*!
*	\brief 대입연산자
*
*	\param rhs 피연산자
*
*	\return 대입된 자신을 반환
*/
GScene &GScene::operator =(const GScene &rhs)
{
	FileName = rhs.FileName;
	RootDir = rhs.RootDir;
	hWndView = rhs.hWndView;
	LightList = rhs.LightList;

	MainCamera = rhs.MainCamera;
	SrcCamera = rhs.SrcCamera;
	TarCamera = rhs.TarCamera;
	pRecentMesh = rhs.pRecentMesh;
	AuxCameraList = rhs.AuxCameraList;

	bShowFloor = rhs.bShowFloor;
	bShowBkg = rhs.bShowBkg;
	bTimer = rhs.bTimer;
	CtrlPtSize = rhs.CtrlPtSize;

	// 메쉬 리스트 제거
	int NumMesh = (int)MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
		delete MeshList[i];
	MeshList.clear();

	// 메쉬 리스트 복사
	NumMesh = (int)rhs.MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
	{
		GMesh *pMesh = new GMesh((*rhs.MeshList[i]));
		MeshList.push_back(pMesh);
	}
	
	return *this;
}

/*!
*	\brief 마우스 왼쪽 버튼을 통한 장면의 회전을 준비한다.
*
*	\param x 마우스 x좌표
*	\param y 마우스 y좌표
*
*	\note 뷰에서 ALT키가 눌려진 상태에서 호출됨
*/
void GScene::LButtonDown(int x, int y)
{
	// 장면의 회전을 위한 준비를 한다.
	MainCamera.CameraManipulateType = CAMERA_MANIPULATE_ROTATE;
	MainCamera.LButtonDown(x, y);	
}

/*!
*	\brief 마우스 중간 버튼을 통한 장면의 이동을 준비한다.
*
*	\param x 마우스 x좌표
*	\param y 마우스 y좌표
*
*	\note 뷰에서 ALT키가 눌려진 상태에서 호출됨
*/
void GScene::MButtonDown(int x, int y)
{
	// 장면의 이동을 준비한다.
	MainCamera.CameraManipulateType = CAMERA_MANIPULATE_PAN;
	MainCamera.LButtonDown(x, y);
}

/*!
*	\brief 마우스 움직임을 통한 장면의 조작(회전, 이동)을 수행한다.
*
*	\param x 마우스 x좌표
*	\param y 마우스 y좌표
*/
void GScene::MouseMove(int x, int y)
{
	// 장면의 조작(회전, 이동)을 수행한다.
	MainCamera.MouseMove(x, y);
}

/*!
*	\brief 장면의 회전을 중지한다.
*
*	\param x 마우스 x좌표
*	\param y 마우스 y좌표
*/
void GScene::LButtonUp(int x, int y)
{
	MainCamera.CameraManipulateType = CAMERA_MANIPULATE_NONE;
}

/*!
*	\brief 장면의 이동을 중지한다.
*
*	\param x 마우스 x좌표
*	\param y 마우스 y좌표
*/
void GScene::MButtonUp(int x, int y)
{
	MainCamera.CameraManipulateType = CAMERA_MANIPULATE_NONE;
}

/*!
*	\brief 장면의 스케일을 수행한다.
*
*	\param delta 마우스 휠의 변위
*/
void GScene::MouseWheel(int delta)
{
	if (delta > 0)
		MainCamera.Zooming(-1);
	else
		MainCamera.Zooming(1);
}

/*!
*	\brief 뷰(카메라) 좌표계에서 표현된 월드 좌표계를 반환한다.
*
*	\return 뷰(카메라) 좌표계계에서 표현된 월드 좌표계를 반환한다.
*/
GTransf GScene::GetWC()
{
	return MainCamera.WC;
}

/*!
*	\brief 장면에 새로운 메쉬를 추가한다.
*
*	\param pMesh 추가될 메쉬에 대한 포인터
*/
void GScene::AddMesh(GMesh *pMesh)
{
	// 메쉬가 포함된 장면의 포인터를 설정하고,
	pMesh->pScene = this;

	// 렌더링을 위한 버퍼, 경계상자 등 모든 것을 생성하고,
	pMesh->CreateMesh();
		
	// 메쉬를 장면에 추가하고,
	GMesh *pParent = FindMesh(MeshList, pMesh->ParentMeshName);
	if (pParent == NULL)
		MeshList.push_back(pMesh);
	else
	{
		pMesh->DelX = inv(pParent->MC) * pMesh->MC;
		pParent->SubMeshList.push_back(pMesh);
	}

	// 메쉬에 정의된 재질이 없다면,
	if (pMesh->MtlList.empty())
	{
		// 임의 재질을 생성하여 메쉬에 추가하고, 활성화 한다.
		GMaterial mtl = ::get_material_random();
		pMesh->AddMtl(mtl);
		pMesh->SetMtl(mtl.Name);	
	}	

	// 가장 최근 추가된 메쉬임을 기록한다.
	pRecentMesh = pMesh;
}

/*!
*	\brief 장면에 새로운 변위 맵을 추가한다.
*
*	\param Map 추가될 변위 맵
*/
// void GScene::AddMapDisp(const GMapDisp &Map)
// {
// 	MapDispList.push_back(Map);
// }

/*!
*	\brief 장면에 새로운 광원을 추가한다.
*
*	\param Light 추가될 광원
*/
void GScene::AddLight(const GLight &Light)
{
	LightList.push_back(Light);
}

/*!
*	\brief 장면에 새로운 카메라를 추가한다.
*
*	\param Cam 추가될 카메라
*/
void GScene::AddCamera(const GCamera &Cam)
{
	AuxCameraList.push_back(Cam);
}

/*!
*	\brief 메쉬 리스트에서 주어진 이름의 메쉬를 제거한다.
*
*	\param MeshList 메쉬 리스트
*	\param Name 제거할 메쉬의 이름
*/
void GScene::DelMesh(std::vector<GMesh *> &MeshList, std::string Name)
{
	std::vector<GMesh *>::iterator it;
	for (it = MeshList.begin(); it != MeshList.end(); it++)
	{
		if ((*it)->Name == Name)
		{
			delete (*it);
			MeshList.erase(it);
			return;
		}

		if (!((*it)->SubMeshList.empty()))
			DelMesh((*it)->SubMeshList, Name);
	}
}

/*!
*	\brief 장면의 OpenGL렌더링 상태를 초기화 한다.
*	\warning 반드시 OpenGL 상태에서 호출되어야 한다.
*/
void GScene::InitScene()
{
	// 전역 변수를 초기화 하고,
	theScene = this;

	// 깊이 테스트를 활성화 하고,
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glDepthFunc(GL_LESS);

	// 조명 모델을 초기화 하고,
	glEnable(GL_NORMALIZE);	
	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, TRUE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, false);

	// 안티 앨리어싱을 활성화 하고,
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POINT_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_POLYGON_SMOOTH, GL_NICEST);

	// 장면 우측 상단의 제어패널을 초기화 하고,
	HeadUpDisplay.pScene = this;

	// 현재의 디렉토리 정보를 읽어와서,
	char path[256];
	_getcwd(path, 256);
	RootDir = std::string(path);
	std::cout<< RootDir.c_str() << std::endl;

	if (HeadUpDisplay.TextureList.empty())
	{
		// 텍스처 초기화
		GMaterial Mat[8];
		std::string file_name;

		file_name = RootDir + "\\res\\textures\\front.tga";
		Mat[0].LoadTexture(file_name.c_str());

		file_name = RootDir + "\\res\\textures\\left.tga";
		Mat[1].LoadTexture(file_name.c_str());

		file_name = RootDir + "\\res\\textures\\back.tga";
		Mat[2].LoadTexture(file_name.c_str());

		file_name = RootDir + "\\res\\textures\\right.tga";
		Mat[3].LoadTexture(file_name.c_str());

		file_name = RootDir + "\\res\\textures\\bottom.tga";
		Mat[4].LoadTexture(file_name.c_str());

		file_name = RootDir + "\\res\\textures\\top.tga";
		Mat[5].LoadTexture(file_name.c_str());

		file_name = RootDir + "\\res\\textures\\home.tga";
		Mat[6].LoadTexture(file_name.c_str());

		file_name = RootDir + "\\res\\textures\\logo-gray.tga";
		Mat[7].LoadTexture(file_name.c_str());

		HeadUpDisplay.TextureList.push_back(Mat[0]);
		HeadUpDisplay.TextureList.push_back(Mat[1]);
		HeadUpDisplay.TextureList.push_back(Mat[2]);
		HeadUpDisplay.TextureList.push_back(Mat[3]);
		HeadUpDisplay.TextureList.push_back(Mat[4]);
		HeadUpDisplay.TextureList.push_back(Mat[5]);
		HeadUpDisplay.TextureList.push_back(Mat[6]);
		HeadUpDisplay.TextureList.push_back(Mat[7]);
	}	
	
	// 장면의 관측을 위한 카메라를 초기화 하고,
	AuxCameraList.clear();
	MainCamera = GCamera(this, GVector3(120.0, 80.0, 120.0), GVector3(0.0, 0.0, 0.0), GVector3(0.0, 1.0, 0.0));

	// 광원의 위치 지정을 위해 모델뷰 행렬을 초기화 하고,
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
			
	// 광원을 초기화하고,
	LightList.clear();
	GLight Light0(GL_LIGHT0);
	Light0.SetPosition(100, 100, 100, 0);
	Light0.SetAmbient(0.1f, 0.1f, 0.1f);
	Light0.SetDiffuse(0.6f, 0.6f, 0.6f);
	Light0.SetSpecular(0.4f, 0.4f, 0.4f);
	AddLight(Light0);
	LightList[0].Enable();

	GLight Light1(GL_LIGHT1);
	Light1.SetPosition(-100,  100, 100, 0);
	Light1.SetAmbient(0.1f, 0.1f, 0.1f);
	Light1.SetDiffuse(0.6f, 0.6f, 0.6f);
	Light1.SetSpecular(0.4f, 0.4f, 0.4f);
	AddLight(Light1);
	LightList[1].Enable();

	// 메쉬리스트를 초기화 한다.
	int NumMesh = (int)MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
		delete MeshList[i];
	MeshList.clear();
}

/*!
*	\brief 장면의 뷰포트 변환을 설정한다.
*/
void GScene::SetupViewport(int x, int y)
{
	MainCamera.SetupViewport(x, y);
	HeadUpDisplay.Resize(x, y);
	HeadUpDisplay.Parallel = false;
}

/*!
*	\brief 장면의 관측공간과 투영방법을 설정한다.
*/
void GScene::SetupViewFrustum()
{
	MainCamera.SetupViewFrustum();
}

/*!
*	\brief 장면의 관측변환을 수행한다.
*/
void GScene::SetupViewTransform()
{
	MainCamera.SetupViewTransform();
}

/*!
*	\brief \a Box를 잘 포함하도록 관측공간을 조정한다.
*
*	\param Box 객체의 경계 상자
*/
void GScene::AdjustViewFrustum(const GBndBox &Box)
{
	// 소스 카메라와 타겟 카메라를 설정하고,
	SrcCamera = MainCamera;
	TarCamera = MainCamera;

	// 뷰 좌표계에서 표현된 경계 상자의 양끝점을 구하고,
	GPoint3 p = MainCamera.WC * Box.pMesh->MC * Box.MinPt;
	GPoint3 q = MainCamera.WC * Box.pMesh->MC * Box.MaxPt;

	// 카메라 좌표계에서 표현된 경계 상자의 중심의 위치를 구하고,
	GPoint3 r = p + (q - p) * 0.5;

	// 회전 변환 적용 전, 월드 좌표계의 중심에서 경계 상자의 중심으로의 변위를 계산한 후,
	GVector3 v = r - GPoint3(0.0, 0.0, MainCamera.ZoomDist);

	// 계산된 변위를 회전 변환된 월드 좌표계의 변위로 바꾸고, 
	v = inv(MainCamera.WC) * v;

	// 역 변위를 적용한 후,
	TarCamera.Pan -= v;

	// 경계 상자의 크기에 따라 적당한 스케일을 적용한다.
	double sz = dist(p, q); 
	TarCamera.ZoomDist = -sz / tan(TarCamera.Fovy * M_PI / 180.0);

	// 타이머를 생성하여 카메라 전환을 실시한다.
	SetTimer(TIMER_EVENT_CAMERA_TRANSIT, 46);
}

/*!
*	\brief 장면을 렌더링 한다.
*/
void GScene::Render()
{
	// 장면의 관측 공간 및 투영 방법 지정
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	SetupViewFrustum();
	
	// 장면의 관측 변환을 수행
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	SetupViewTransform();

	// HUD의 런더링을 수행
	HeadUpDisplay.Render();
	
	// 장면의 바닥 평면(ZX)을 렌더링
	RenderFloor();

	// 장면에 포함된 메쉬를 렌더링
	int NumMesh = (int)MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
		MeshList[i]->Render();

	if (!PtsOnMesh.empty())
	{
		glDisable(GL_LIGHTING);
		glColor3d(0.0, 0.0, 1.0);
		glLineWidth(5.0f);

		int NumPt = PtsOnMesh.size();
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < NumPt; ++i)
			glVertex3f(PtsOnMesh[i][0], PtsOnMesh[i][1], PtsOnMesh[i][2]);
		glEnd();

		glLineWidth(1.0f);
		glEnable(GL_LIGHTING);
	}
}

/*!
*	\brief 장면을 기준 평면을 렌더링 한다.
*/
void GScene::RenderFloor()
{
	if (!bShowFloor)
		return;

	// Anti-aliasing을 활성화 하고,
	glDisable(GL_LIGHTING);
// 	glEnable(GL_BLEND);
// 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// 	glEnable(GL_LINE_SMOOTH);
// 	glLineWidth(0.5f);

	// 바닥 평면을 그리고,
	for (int i = -50; i <= 50; i += 5)
	{
		if (i == 0)
			continue;

		double x, y;
		x = y = (double)i;

		glColor3f(0.7f, 0.7f, 0.7f);
		glBegin(GL_LINES);
		glVertex3d(x, 0.0, -50.0);
		glVertex3d(x, 0.0, 50.0);
		glEnd();
		
		glBegin(GL_LINES);
		glVertex3d(-50.0, 0.0, y);
		glVertex3d(50.0, 0.0, y);
		glEnd();
	}

	// 중심선을 그린다.
	glLineWidth(2.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	{
		glVertex3d(-50.0, 0.0, 0.0);
		glVertex3d(50.0, 0.0, 0.0);
		glVertex3d(0.0, 0.0, -50.0);
		glVertex3d(0.0, 0.0, 50.0);
	}
	glEnd();
	glLineWidth(1.0f);

	// Anti-aliasing을 비활성화 한다.
	glEnable(GL_LIGHTING);
//	glDisable(GL_BLEND);
//	glDisable(GL_LINE_SMOOTH);
}

/*!
*	\brief 자동으로 호출되는 타이머 함수
*
*	\param hWnd 윈도우 핸들
*	\param uMsg 타이머 메시지 (WM_TIMER)
*	\param nIDEvent 타이머 번호
*	\param dwTime 윈도우 실행 후 경과시간
*/
void CALLBACK theTimer(HWND hWnd, UINT uMsg, UINT nIDEvent, DWORD dwTime)
{
	switch (nIDEvent)
	{
	case TIMER_EVENT_CAMERA_TRANSIT:
		{
			static int frm_idx = 0;
			double t = frm_idx / 9.0;

			// 장면의 메인 카메라의 변환을 계산하고,
			theScene->MainCamera.ZoomDist = (1.0 - t) * theScene->SrcCamera.ZoomDist + t * theScene->TarCamera.ZoomDist;
			theScene->MainCamera.Pan = (1.0 - t) * theScene->SrcCamera.Pan + t * theScene->TarCamera.Pan;
			theScene->MainCamera.Rot = slerp(theScene->SrcCamera.Rot, theScene->TarCamera.Rot, t);

			if (frm_idx == 9)
			{
				theScene->KillTimer(TIMER_EVENT_CAMERA_TRANSIT);
				frm_idx = 0;
			}
			else
				frm_idx++;
		}
		break;
	}
	
	SendMessage(theScene->hWndView, WM_PAINT, 0, 0);
}

/*!
*	\brief 장면에 타이머를 설치한다.
*
*	\param nIDEvent 타이머 번호
*	\param uElapse 설정된 시간 간격 (ms 단위)
*/
void GScene::SetTimer(UINT nIDEvent, UINT uElapse)
{
	::SetTimer(hWndView, nIDEvent, uElapse, (TIMERPROC)theTimer);
	bTimer = true;
}

/*!
*	\brief 장면에 타이머를 해제한다.
*
*	\param nIDEvent 해제할 타이머 번호
*/
void GScene::KillTimer(UINT nIDEvent)
{
	::KillTimer(hWndView, nIDEvent);
	bTimer = false;
}

/*!
*	\brief 장면에 포함된 메쉬의 애니메이션을 수행한다.
*/
void GScene::AnimateScene()
{
// 	int NumMesh = (int)MeshList.size();
// 	for (int i = 0; i < NumMesh; ++i)
// 		MeshList[i]->Animate(NULL);
// 
// 	for (int i = 0; i < NumMesh; ++i)
// 		MeshList[i]->UpdateSkinedMesh();	
}

/*!
*	\brief 장면에 포함된 변위 맵 리스트에서 이름이 \a Name인 맵을 찾는다.
*
*	\param Name 찾을 변위 맵의 이름
*
*	\return 변위 맵의 포인터
*/
// GMapDisp *GScene::FindMapDisp(const std::string &Name)
// {
// 	int NumMap = (int)MapDispList.size();
// 	for (int i = 0; i < NumMap; ++i)
// 	{
// 		if (MapDispList[i].Name == Name)
// 			return &MapDispList[i];
// 	}
// 
// 	return NULL;
// }

/*!
*	\brief ParentMesh의 하위계층에 대하여 PreOrder로 방문된 선형 리스트를 생성한다.
*
*	\param pScene 장면에 대한 포인터
*	\param MeshList 자식 메쉬들이 저장될 리스트
*	\param ParentMesh 부모 메쉬, 만약에 NULL이면 장면에 모함된 모든 메쉬에 적용
*/
void GetPreOrderMeshList(GScene *pScene, std::vector<GMesh *> &MeshList, GMesh *ParentMesh)
{
	if (ParentMesh == NULL)
	{
		int NumMesh = (int)pScene->MeshList.size();
		for (int i = 0; i < NumMesh; ++i)
			GetPreOrderMeshList(pScene, MeshList, pScene->MeshList[i]);
	}
	else
	{
		MeshList.push_back(ParentMesh);
		int Num = (int)(ParentMesh->SubMeshList.size());
		for (int i = 0; i < Num; ++i)
			GetPreOrderMeshList(pScene, MeshList, ParentMesh->SubMeshList[i]);
	}	
}

/*!
*	\brief 주어진 메쉬 리스트에서 하위 메쉬를 모두 검색하며 메쉬를 찾는다.
*
*	\param MeshList 메쉬 리스트
*	\param Name 찾을 메쉬의 이름
*
*	\return 메쉬의 포인터
*/
GMesh *FindMesh(std::vector<GMesh *> &MeshList, const std::string &Name)
{
	static GMesh *pMesh = NULL;
	pMesh = NULL;

	int NumMesh = (int)MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
	{
		if (MeshList[i]->Name == Name)
		{
			pMesh = MeshList[i];
			return pMesh;
		}

		if (!MeshList[i]->SubMeshList.empty())
		{
			FindMesh(MeshList[i]->SubMeshList, Name);
			if (pMesh != NULL)
				return pMesh;
		}
	}
	return pMesh;
}

////////////////////////////////////////////////////////////
/*!
*	\brief 생성자
*/
GHud::GHud()
{
	pScene = NULL;
	Parallel = false;
	ViewFrustum[0] = -10.0;
	ViewFrustum[1] = 10.0;
	ViewFrustum[2] = -10.0;
	ViewFrustum[3] = 10.0;
	ViewFrustum[4] = -1000.0;
	ViewFrustum[5] = 1000000.0;
	DisplayInfo = HUD_VIEW_CUBE | HUD_FRAME_RATE | HUD_ORIGIN_AXIS;
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사될 객체
*/
GHud::GHud(const GHud &cpy)
{
	pScene = cpy.pScene;
	TextureList = cpy.TextureList;
	Parallel = cpy.Parallel;
	ARR_COPY(ViewFrustum, cpy.ViewFrustum, 6);
	DisplayInfo = cpy.DisplayInfo;
	Message = cpy.Message;
}

/*!
*	\brief 대입연산자
*
*	\param rhs 대입될 객체
*/
GHud &GHud::operator =(const GHud &rhs)
{
	pScene = rhs.pScene;
	TextureList = rhs.TextureList;
	Parallel = rhs.Parallel;
	ARR_COPY(ViewFrustum, rhs.ViewFrustum, 6);
	DisplayInfo = rhs.DisplayInfo;
	Message = rhs.Message;

	return *this;
}

/*!
*	\brief Control Panel의 관측공간을 계산한다.
*	
*	\param width 윈도우의 너비
*	\param height 윈도우의 높이
*/
void GHud::Resize(int width, int height)
{
	double ratio = (double)(width) / (double)(height);
	ViewFrustum[0] = -10.0;
	ViewFrustum[1] = 10.0;
	ViewFrustum[2] = -10.0;
	ViewFrustum[3] = 10.0;
	
	if (ratio > 1.0)
	{
		ViewFrustum[0] *= ratio;
		ViewFrustum[1] *= ratio;
	}
	else
	{
		ViewFrustum[2] /= ratio;
		ViewFrustum[3] /= ratio;
	}
}

/*!
*	\brief HUD의 정보를 렌더링 한다.
*/
void GHud::Render()
{
	// 현재 관측변환 행렬을 저장하고,
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// 새로운 관측공간을 설정한다.
	glLoadIdentity();	
	glOrtho(ViewFrustum[0], ViewFrustum[1], ViewFrustum[2], ViewFrustum[3], ViewFrustum[4], ViewFrustum[5] + 1.0);
	
	// 현재 모델뷰 행렬을 저장하고,
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	{
		// 배경, FPS, 회전축, 뷰큐브을 렌더링하고,
		RenderBackGround();
		RenderFPS();
		RenderLogo();
		RenderOriginAxis();
		RenderViewCube();
		RenderMessage();	
	}
	// 모델뷰 행렬을 복원하고,
	glPopMatrix();

	// 관측변환 행렬을 복원한다.
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// 모델뷰 행렬을 선택한다.
	glMatrixMode(GL_MODELVIEW);
}

/*!
*	\brief 장면의 그래디언트 배경을 렌더링 한다.
*/
void GHud::RenderBackGround()
{
	if (pScene->bShowBkg)
	{
		glDisable(GL_LIGHTING);
		glBegin(GL_POLYGON);
		{
			double R = 0.49, G = 0.565, B= 0.64;
			//double R = 0.823, G = 0.682, B= 0.1414;
			glColor3d(R, G, B);
			glVertex3d(ViewFrustum[0], ViewFrustum[2], -ViewFrustum[5]);
			glColor3d(R, G, B);
			glVertex3d(ViewFrustum[1], ViewFrustum[2], -ViewFrustum[5]);
			glColor3d(0.1, 0.1, 0.1);
			glVertex3d(ViewFrustum[1], ViewFrustum[3], -ViewFrustum[5]);
			glColor3d(0.1, 0.1, 0.1);
			glVertex3d(ViewFrustum[0], ViewFrustum[3], -ViewFrustum[5]);		
		}
		glEnd();
		glEnable(GL_LIGHTING);
	}	
}

/*!
*	\brief 장면의 FPS를 출력한다.
*/
void GHud::RenderFPS()
{
	if (DisplayInfo & HUD_FRAME_RATE)
	{
		glDisable(GL_LIGHTING);

		static unsigned long FrameCnt = 0L;
		static double fps = 0.0;
		static clock_t start = clock();

		clock_t end = clock();
		clock_t elapsed_time = end - start;

		FrameCnt++;
		if (elapsed_time > 1000)
		{
			start = end;
			fps = (double)FrameCnt / (double)elapsed_time * 1000.0;
			FrameCnt = 0L;
		}

		char buffer[50];
		sprintf_s(buffer, "%s%5.2lf", "FPS: ", fps);

		// FPS를 렌더링하고,
		int h = pScene->MainCamera.ViewHeight;
		RenderText(ViewFrustum[0] * 0.98, ViewFrustum[3] * 0.95, ViewFrustum[4] * 0.9, buffer, 1.0, 1.0, 1.0);

		// 로고를 렌더링한다.
		glEnable(GL_LIGHTING);
	}		
}

/*!
*	\brief 장면의 회전 중심축을 렌더링한다.
*/
void GHud::RenderOriginAxis()
{
	if (DisplayInfo & HUD_ORIGIN_AXIS)
	{
		// 조명과 깊이 테스트를 끄고,
		glDisable(GL_LIGHTING);

		glPushMatrix();
		{
			//glTranslated(0.0, 0.0, ViewFrustum[4] * 0.9);
			glTranslated(ViewFrustum[0] + 1.2, ViewFrustum[2] + 1.2, ViewFrustum[4] * 0.9);
			// 회전변환 적용
			glMultMatrixd(cast_mat(pScene->MainCamera.Rot).M);

			float len = 0.5;
			glColor3f(1.0, 0.0, 0.0);
			glBegin(GL_LINES);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(len, 0.0, 0.0);
			glVertex3f(len, len / 5.0, -len / 5.0);
			glVertex3f(len, -len / 5.0, len / 5.0);
			glVertex3f(len, len / 5.0, len / 5.0);
			glVertex3f(len, -len / 5.0, -len / 5.0);
			glEnd();

			glColor3f(0.0, 1.0, 0.0);
			glBegin(GL_LINES);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, len, 0.0);
			glVertex3f(-len / 5.0, len, -len / 5.0);
			glVertex3f(0.0, len, 0.0);
			glVertex3f(len / 5.0, len, -len / 5.0);
			glVertex3f(0.0, len, 0.0);
			glVertex3f(0.0, len, 0.0);
			glVertex3f(0.0, len, len / 5.0);
			glEnd();

			glColor3f(0.0, 0.0, 1.0);
			glBegin(GL_LINES);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0, len);
			glVertex3f(len / 5.0, len / 5.0, len);
			glVertex3f(-len / 5.0, -len / 5.0, len);
			glVertex3f(-len / 5.0, len / 5.0, len);
			glVertex3f(len / 5.0, len / 5.0, len);
			glVertex3f(-len / 5.0, -len / 5.0, len);
			glVertex3f(len / 5.0, -len / 5.0, len);
			glEnd();
		}
		glPopMatrix();

		glEnable(GL_LIGHTING);
	}
}

/*!
*	\brief 장면의 뷰큐브을 렌더링한다.
*/
void GHud::RenderViewCube()
{
	if (DisplayInfo & HUD_VIEW_CUBE)
	{
		// 조명을 끄고,
		glDisable(GL_LIGHTING);

		glPushMatrix();
		{
			// 뷰큐브 렌더링을 위한 변환
			//glTranslated(ViewFrustum[1] - 1.2, ViewFrustum[3] - 1.2, -ViewFrustum[5] * 0.8);
			glTranslated(ViewFrustum[1] - 1.2, ViewFrustum[3] - 1.2, ViewFrustum[4] * 0.9);

			glColor3d(0.8, 0.8, 0.8);
			if (Parallel)
			{
				// 동쪽 화살표
				glBegin(GL_TRIANGLES);
				{
					glVertex2d(0.55, 0.0);
					glVertex2d(0.85, -0.15);
					glVertex2d(0.85, 0.15);
				}
				glEnd();

				// 서쪽 화살표
				glBegin(GL_TRIANGLES);
				{
					glVertex2d(-0.55, 0.0);
					glVertex2d(-0.85, 0.15);
					glVertex2d(-0.85, -0.15);
				}
				glEnd();

				// 남쪽 화살표
				glBegin(GL_TRIANGLES);
				{
					glVertex2d(0.0, -0.55);
					glVertex2d(-0.15, -0.85);
					glVertex2d(0.15, -0.85);
				}
				glEnd();

				// 북쪽 화살표
				glBegin(GL_TRIANGLES);
				{
					glVertex2d(0.0, 0.55);
					glVertex2d(0.15, 0.85);
					glVertex2d(-0.15, 0.85);			
				}
				glEnd();
			}

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glEnable(GL_TEXTURE_2D);

			// 홈아이콘
			glBindTexture(GL_TEXTURE_2D, TextureList[6].BindTexId);
			glBegin(GL_QUADS);
			{
				glTexCoord2d(0.0, 0.0);
				glVertex3d(-0.9, 0.7, 0.0);
				glTexCoord2d(1.0, 0.0);
				glVertex3d(-0.6, 0.7, 0.0);
				glTexCoord2d(1.0, 1.0);
				glVertex3d(-0.6, 1.0, 0.0);
				glTexCoord2d(0.0, 1.0);
				glVertex3d(-0.9, 1.0, 0.0);
			}
			glEnd();

			// 회전변환 적용
			glMultMatrixd(cast_mat(pScene->MainCamera.Rot).M);

			double cube_size = 0.5;
			// 앞면
			glBindTexture(GL_TEXTURE_2D, TextureList[0].BindTexId);
			glBegin(GL_QUADS);
			{
				glTexCoord2d(0.0, 0.0);
				glVertex3d(-cube_size, -cube_size, cube_size);
				glTexCoord2d(1.0, 0.0);
				glVertex3d(cube_size, -cube_size, cube_size);
				glTexCoord2d(1.0, 1.0);
				glVertex3d(cube_size, cube_size, cube_size);
				glTexCoord2d(0.0, 1.0);
				glVertex3d(-cube_size, cube_size, cube_size);
			}
			glEnd();

			// 왼쪽면
			glBindTexture(GL_TEXTURE_2D, TextureList[1].BindTexId);
			glBegin(GL_QUADS);
			{
				glTexCoord2d(0.0, 0.0);
				glVertex3d(-cube_size, -cube_size, -cube_size);
				glTexCoord2d(1.0, 0.0);
				glVertex3d(-cube_size, -cube_size, cube_size);
				glTexCoord2d(1.0, 1.0);
				glVertex3d(-cube_size, cube_size, cube_size);
				glTexCoord2d(0.0, 1.0);
				glVertex3d(-cube_size, cube_size, -cube_size);
			}
			glEnd();

			// 뒤쪽면
			glBindTexture(GL_TEXTURE_2D, TextureList[2].BindTexId);
			glBegin(GL_QUADS);
			{
				glTexCoord2d(0.0, 0.0);
				glVertex3d(cube_size, -cube_size, -cube_size);
				glTexCoord2d(1.0, 0.0);
				glVertex3d(-cube_size, -cube_size, -cube_size);
				glTexCoord2d(1.0, 1.0);
				glVertex3d(-cube_size, cube_size, -cube_size);
				glTexCoord2d(0.0, 1.0);
				glVertex3d(cube_size, cube_size, -cube_size);
			}
			glEnd();

			// 오른쪽면
			glBindTexture(GL_TEXTURE_2D, TextureList[3].BindTexId);
			glBegin(GL_QUADS);
			{
				glTexCoord2d(0.0, 0.0);
				glVertex3d(cube_size, -cube_size, cube_size);
				glTexCoord2d(1.0, 0.0);
				glVertex3d(cube_size, -cube_size, -cube_size);
				glTexCoord2d(1.0, 1.0);
				glVertex3d(cube_size, cube_size, -cube_size);
				glTexCoord2d(0.0, 1.0);
				glVertex3d(cube_size, cube_size, cube_size);
			}
			glEnd();

			// 바닥면
			glBindTexture(GL_TEXTURE_2D, TextureList[4].BindTexId);
			glBegin(GL_QUADS);
			{
				glTexCoord2d(0.0, 0.0);
				glVertex3d(-cube_size, -cube_size, -cube_size);
				glTexCoord2d(1.0, 0.0);
				glVertex3d(cube_size, -cube_size, -cube_size);
				glTexCoord2d(1.0, 1.0);
				glVertex3d(cube_size, -cube_size, cube_size);
				glTexCoord2d(0.0, 1.0);
				glVertex3d(-cube_size, -cube_size, cube_size);
			}
			glEnd();

			// 윗면	
			glBindTexture(GL_TEXTURE_2D, TextureList[5].BindTexId);
			glBegin(GL_QUADS);
			{	
				glTexCoord2d(0.0, 0.0);
				glVertex3d(-cube_size, cube_size, cube_size);
				glTexCoord2d(1.0, 0.0);
				glVertex3d(cube_size, cube_size, cube_size);
				glTexCoord2d(1.0, 1.0);
				glVertex3d(cube_size, cube_size, -cube_size);
				glTexCoord2d(0.0, 1.0);
				glVertex3d(-cube_size, cube_size, -cube_size);				
			}
			glEnd();

			glBindTexture(GL_TEXTURE_2D, NULL);
			glDisable(GL_TEXTURE_2D);
		}
		glPopMatrix();		
		glEnable(GL_LIGHTING);
	}
}

/*!
*	\brief 로고를 렌더링한다.
*/
void GHud::RenderLogo()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, TextureList[7].BindTexId);

	glPushMatrix();
	{
		glTranslated(ViewFrustum[1] - 1.0, ViewFrustum[2] + 0.5, -ViewFrustum[5]);
		glBegin(GL_QUADS);
		{
			glTexCoord2d(0.0, 0.0);
			glVertex3d(-1.5, -0.5, 1.0);
			glTexCoord2d(1.0, 0.0);
			glVertex3d(1.5, -0.5, 1.0);
			glTexCoord2d(1.0, 1.0);
			glVertex3d(1.5, 0.5, 1.0);
			glTexCoord2d(0.0, 1.0);
			glVertex3d(-1.5, 0.5, 1.0);
		}
		glEnd();
	}
	glPopMatrix();	

	glBindTexture(GL_TEXTURE_2D, NULL);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
}

/*!
*	\brief 추가 메시지를 렌더링 한다.
*/
void GHud::RenderMessage()
{
	if (!Message.empty())
	{
		glDisable(GL_LIGHTING);
		RenderText(ViewFrustum[0] * 0.98, ViewFrustum[3] * 0.90, ViewFrustum[4] * 0.9, Message.c_str(), 1.0, 1.0, 1.0);
		glEnable(GL_LIGHTING);
	}
}

/*!
*	\brief 텍스트를 렌더링 한다.
*
*	\param x 렌더링될 텍스트의 x 좌표
*	\param y 렌더링될 텍스트의 y 좌표
*	\param z 렌더링될 텍스트의 z 좌표
*	\param str 렌더링될 텍스를 배열
*	\param r 렌더링될 텍스트 색상의 R 성분
*	\param g 렌더링될 텍스트 색상의 G 성분
*	\param b 렌더링될 텍스트 색상의 B 성분
*/
void GHud::RenderText(double x, double y, double z, const char *str, double r, double g, double b)
{
	glColor3d(r, g, b);
	glRasterPos3d(x, y, z);
	int len = (int)strlen(str);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
}

/*!
*	\brief Control Panel의 메뉴 선택 및 기능을 수행한다.
*/
void GHud::LButtonDown(int x, int y)
{
	GLuint selectBuf[SELECT_BUFFER_SIZE];
	GLint hits;
	GLint viewport[4];
	double cube_size = 0.5;

	// 뷰 포트 정보를 구하고,
	glGetIntegerv(GL_VIEWPORT, viewport);

	// 선택 버퍼를 지정한 후,
	glSelectBuffer(SELECT_BUFFER_SIZE, selectBuf);

	// 렌더링 모드를 선택 모드로 변경한다.
	glRenderMode(GL_SELECT);

	// 네임스택을 초기화하고 dummy(0)을 넣고,
	glInitNames();
	glPushName(0);

	// 투영행렬을 선택하여
	glMatrixMode(GL_PROJECTION);
	{
		// 현재 상태를 저장하고, 단위행렬로 초기화 한후,
		glPushMatrix();
		glLoadIdentity();

		// 선택 행렬을 생성한다.
		int offset = 1;
		gluPickMatrix((double)x, (GLdouble)(viewport[3] - y), offset, offset, viewport);
		glOrtho(ViewFrustum[0], ViewFrustum[1], ViewFrustum[2], ViewFrustum[3], ViewFrustum[4], ViewFrustum[5] + 1.0);

		// 모델뷰 행렬을 선택하여,
		glMatrixMode(GL_MODELVIEW);
		{
			// 현재 상태를 저장하고, 단위행렬로 초기화 한후, 
			glPushMatrix();
			glLoadIdentity();

			// 홈 아이콘을 그리고,
			glTranslated(ViewFrustum[1] - 1.2, ViewFrustum[3] - 1.2, -ViewFrustum[5] * 0.8);
			glLoadName(0);
			glBegin(GL_POLYGON);
			{
				glVertex3d(-0.9, 0.7, 0.0);
				glVertex3d(-0.6, 0.7, 0.0);
				glVertex3d(-0.6, 1.0, 0.0);
				glVertex3d(-0.9, 1.0, 0.0);
			}
			glEnd();

			// 좌표 평면들과 직각 방향에서 보고 있다면,
			if (Parallel)
			{
				// 동쪽 화살표
				glLoadName(7);
				glBegin(GL_TRIANGLES);
				{
					glVertex2d(0.55, 0.0);
					glVertex2d(0.85, -0.15);
					glVertex2d(0.85, 0.15);
				}
				glEnd();

				// 서쪽 화살표
				glLoadName(8);
				glBegin(GL_TRIANGLES);
				{
					glVertex2d(-0.55, 0.0);
					glVertex2d(-0.85, 0.15);
					glVertex2d(-0.85, -0.15);
				}
				glEnd();

				// 남쪽 화살표와
				glLoadName(9);
				glBegin(GL_TRIANGLES);
				{
					glVertex2d(0.0, -0.55);
					glVertex2d(-0.15, -0.85);
					glVertex2d(0.15, -0.85);
				}
				glEnd();

				// 북쪽 화살표를 그리고,
				glLoadName(10);
				glBegin(GL_TRIANGLES);
				{
					glVertex2d(0.0, 0.55);
					glVertex2d(0.15, 0.85);
					glVertex2d(-0.15, 0.85);			
				}
				glEnd();
			}

			// 회전변환 적용한 후,
			glMultMatrixd(cast_mat(pScene->MainCamera.Rot).M);

			// 뷰 큐브의 앞면
			glLoadName(1);
			glBegin(GL_POLYGON);
			{
				glVertex3d(-cube_size, -cube_size, cube_size);
				glVertex3d(cube_size, -cube_size, cube_size);
				glVertex3d(cube_size, cube_size, cube_size);
				glVertex3d(-cube_size, cube_size, cube_size);
			}
			glEnd();

			// 왼쪽면
			glLoadName(2);
			glBegin(GL_POLYGON);
			{
				glVertex3d(-cube_size, -cube_size, -cube_size);
				glVertex3d(-cube_size, -cube_size, cube_size);
				glVertex3d(-cube_size, cube_size, cube_size);
				glVertex3d(-cube_size, cube_size, -cube_size);
			}
			glEnd();

			// 뒤쪽면
			glLoadName(3);
			glBegin(GL_POLYGON);
			{
				glVertex3d(cube_size, -cube_size, -cube_size);
				glVertex3d(-cube_size, -cube_size, -cube_size);
				glVertex3d(-cube_size, cube_size, -cube_size);
				glVertex3d(cube_size, cube_size, -cube_size);
			}
			glEnd();

			// 오른쪽면
			glLoadName(4);
			glBegin(GL_POLYGON);
			{
				glVertex3d(cube_size, -cube_size, cube_size);
				glVertex3d(cube_size, -cube_size, -cube_size);
				glVertex3d(cube_size, cube_size, -cube_size);
				glVertex3d(cube_size, cube_size, cube_size);
			}
			glEnd();

			// 바닥면
			glLoadName(5);
			glBegin(GL_POLYGON);
			{
				glVertex3d(-cube_size, -cube_size, -cube_size);
				glVertex3d(cube_size, -cube_size, -cube_size);
				glVertex3d(cube_size, -cube_size, cube_size);
				glVertex3d(-cube_size, -cube_size, cube_size);
			}
			glEnd();

			// 윗면을 차례로 그린다.
			glLoadName(6);
			glBegin(GL_POLYGON);
			{	
				glVertex3d(-cube_size, cube_size, cube_size);
				glVertex3d(cube_size, cube_size, cube_size);
				glVertex3d(cube_size, cube_size, -cube_size);
				glVertex3d(-cube_size, cube_size, -cube_size);			
			}
			glEnd();
		}
		// 모델뷰 행렬을 복원하고,
		glPopMatrix();
	}
	// 투영 행렬을 다시 선택하여, 이전 투영 행렬을 복원한다.
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();	
	
	// 다시 모델뷰 행렬을 선택한 후,
	glMatrixMode(GL_MODELVIEW);

	// 선택된 객체의 수를 구하여,
	hits = glRenderMode(GL_RENDER);
	if (hits > 0)
	{
		int name;
		double max_z = -10000.0;
		GQuater q = pScene->MainCamera.Rot;
		GVector3 v;

		for (int i = 0; i < hits; ++i)
		{
			int tmp_name = selectBuf[4 * i + 3];
			switch (tmp_name)
			{
			case 1:
				v = q * GVector3(0.0, 0.0, cube_size);
				break;

			case 2:
				v = q * GVector3(-cube_size, 0.0, 0.0);
				break;

			case 3:
				v = q * GVector3(0.0, 0.0, -cube_size);
				break;

			case 4:
				v = q * GVector3(cube_size, 0.0, 0.0);
				break;

			case 5:
				v = q * GVector3(0.0, -cube_size, 0.0);
				break;

			case 6:
				v = q * GVector3(0.0, cube_size, 0.0);
				break;
			}

			if (v[2] > max_z)
			{
				max_z = v[2];
				name = tmp_name;
			}
		}
				
		switch (name)
		{
		case 0:	// home
			SetHomeView();
			break;
		case 1: // front
			SetFrontView();			
			break;
		case 2: // left
			SetLeftView();
			break;
		case 3:	// back
			SetBackView();			
			break;
		case 4: // right
			SetRightView();
			break;
		case 5: // bottom
			SetBottomView();
			break;
		case 6: // top
			SetTopView();
			break;
		case 7: // east
			SetEastView();
			break;
		case 8: // west
			SetWestView();
			break;
		case 9: // south
			SetSouthView();
			break;
		case 10: // north
			SetNorthView();
			break;
		}
	}
}

/*!
*	\brief 장면의 관측방향을 설정한다.
*/
void GHud::SetHomeView()
{
	int w = pScene->MainCamera.ViewWidth;
	int h = pScene->MainCamera.ViewHeight;

	pScene->SrcCamera = pScene->MainCamera;
	pScene->TarCamera = GCamera(pScene, GVector3(120.0, 80.0, 120.0), GVector3(0.0, 0.0, 0.0), GVector3(0.0, 1.0, 0.0));
	pScene->SetTimer(TIMER_EVENT_CAMERA_TRANSIT, 46);

	pScene->SetupViewport(w, h);
}

/*!
*	\brief 장면의 관측방향을 설정한다.
*/
void GHud::SetFrontView()
{
	pScene->SrcCamera = pScene->MainCamera;
	pScene->TarCamera = pScene->MainCamera;
	pScene->TarCamera.Rot.Set(1.0, 0.0, 0.0, 0.0);
	pScene->SetTimer(TIMER_EVENT_CAMERA_TRANSIT, 46);

	Parallel = true;
}

/*!
*	\brief 장면의 관측방향을 설정한다.
*/
void GHud::SetBackView()
{
	GQuater q;
	q.SetFromAngleAxis(180.0, GVector3(0.0, 1.0, 0.0));

	pScene->SrcCamera = pScene->MainCamera;
	pScene->TarCamera = pScene->MainCamera;
	pScene->TarCamera.Rot = q;
	pScene->SetTimer(TIMER_EVENT_CAMERA_TRANSIT, 46);

	Parallel = true;
}

/*!
*	\brief 장면의 관측방향을 설정한다.
*/
void GHud::SetLeftView()
{
	GQuater q;
	q.SetFromAngleAxis(90.0, GVector3(0.0, 1.0, 0.0));

	pScene->SrcCamera = pScene->MainCamera;
	pScene->TarCamera = pScene->MainCamera;
	pScene->TarCamera.Rot = q;
	pScene->SetTimer(TIMER_EVENT_CAMERA_TRANSIT, 46);

	Parallel = true;
}

/*!
*	\brief 장면의 관측방향을 설정한다.
*/
void GHud::SetRightView()
{
	GQuater q;
	q.SetFromAngleAxis(-90.0, GVector3(0.0, 1.0, 0.0));

	pScene->SrcCamera = pScene->MainCamera;
	pScene->TarCamera = pScene->MainCamera;
	pScene->TarCamera.Rot = q;
	pScene->SetTimer(TIMER_EVENT_CAMERA_TRANSIT, 46);

	Parallel = true;
}

/*!
*	\brief 장면의 관측방향을 설정한다.
*/
void GHud::SetBottomView()
{
	GQuater q;
	q.SetFromAngleAxis(-90.0, GVector3(1.0, 0.0, 0.0));

	pScene->SrcCamera = pScene->MainCamera;
	pScene->TarCamera = pScene->MainCamera;
	pScene->TarCamera.Rot = q;
	pScene->SetTimer(TIMER_EVENT_CAMERA_TRANSIT, 46);

	Parallel = true;
}

/*!
*	\brief 장면의 관측방향을 설정한다.
*/
void GHud::SetTopView()
{
	GQuater q;
	q.SetFromAngleAxis(90.0, GVector3(1.0, 0.0, 0.0));

	pScene->SrcCamera = pScene->MainCamera;
	pScene->TarCamera = pScene->MainCamera;
	pScene->TarCamera.Rot = q;
	pScene->SetTimer(TIMER_EVENT_CAMERA_TRANSIT, 46);

	Parallel = true;
}

/*!
*	\brief 장면의 관측방향을 설정한다.
*/
void GHud::SetEastView()
{
	GQuater q;
	q.SetFromAngleAxis(-90.0, GVector3(0.0, 1.0, 0.0));

	pScene->SrcCamera = pScene->MainCamera;
	pScene->TarCamera = pScene->MainCamera;
	pScene->TarCamera.Rot = q * pScene->TarCamera.Rot;
	pScene->SetTimer(TIMER_EVENT_CAMERA_TRANSIT, 46);

	Parallel = true;
}

/*!
*	\brief 장면의 관측방향을 설정한다.
*/
void GHud::SetWestView()
{
	GQuater q;
	q.SetFromAngleAxis(90.0, GVector3(0.0, 1.0, 0.0));

	pScene->SrcCamera = pScene->MainCamera;
	pScene->TarCamera = pScene->MainCamera;
	pScene->TarCamera.Rot = q * pScene->TarCamera.Rot;
	pScene->SetTimer(TIMER_EVENT_CAMERA_TRANSIT, 46);

	Parallel = true;
}


/*!
*	\brief 장면의 관측방향을 설정한다.
*/
void GHud::SetSouthView()
{
	GQuater q;
	q.SetFromAngleAxis(-90.0, GVector3(1.0, 0.0, 0.0));

	pScene->SrcCamera = pScene->MainCamera;
	pScene->TarCamera = pScene->MainCamera;
	pScene->TarCamera.Rot = q * pScene->TarCamera.Rot;
	pScene->SetTimer(TIMER_EVENT_CAMERA_TRANSIT, 46);

	Parallel = true;
}

/*!
*	\brief 장면의 관측방향을 설정한다.
*/
void GHud::SetNorthView()
{
	GQuater q;
	q.SetFromAngleAxis(90.0, GVector3(1.0, 0.0, 0.0));

	pScene->SrcCamera = pScene->MainCamera;
	pScene->TarCamera = pScene->MainCamera;
	pScene->TarCamera.Rot = q * pScene->TarCamera.Rot;
	pScene->SetTimer(TIMER_EVENT_CAMERA_TRANSIT, 46);

	Parallel = true;
}
