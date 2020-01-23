#include "stdafx.h"

GScene *theScene = NULL;

/////////////////////////////////////////////////////////////////
/*!
*	\brief ������
*
*	\param _LightNum ������ ������ ��ȣ
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
*	\brief ���� ������
*
*	\param cpy ����� ��ü
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
*	\brief �Ҹ���
*/
GLight::~GLight()
{
}

/*!
*	\brief ���Կ�����
*
*	\param rhs �ǿ�����
*
*	\return ���Ե� �ڽ��� ��ȯ
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
*	\brief ������ Ȱ��ȭ �Ѵ�
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
*	\brief ������ ��Ȱ��ȭ �Ѵ�
*/
void GLight::Disable()
{
	glDisable(LightNum);

	IsEnabled = false;
}

/*!
*	\brief ������ ��ġ�� �����Ѵ�.
*
*	\param x ������ x��ǥ
*	\param y ������ y��ǥ
*	\param z ������ z��ǥ
*	\param w ������ w��ǥ(1: ������, 0: ���⼺����)
*/
void GLight::SetPosition(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	Position[0] = x;
	Position[1] = y;
	Position[2] = z;
	Position[3] = w;
}

/*!
*	\brief ������ �ֺ��� ������ �����Ѵ�.
*
*	\param r ������ ������ �ֺ��� ����
*	\param g ������ �ʷϻ� �ֺ��� ����
*	\param b ������ �Ķ��� �ֺ��� ����
*/
void GLight::SetAmbient(GLfloat r, GLfloat g, GLfloat b)
{
	Ambient[0] = r;
	Ambient[1] = g;
	Ambient[2] = b;
}

/*!
*	\brief ������ ���ݻ籤 ������ �����Ѵ�.
*
*	\param r ������ ������ ���ݻ籤 ����
*	\param g ������ �ʷϻ� ���ݻ籤 ����
*	\param b ������ �Ķ��� ���ݻ籤 ����
*/
void GLight::SetDiffuse(GLfloat r, GLfloat g, GLfloat b)
{
	Diffuse[0] = r;
	Diffuse[1] = g;
	Diffuse[2] = b;
}

/*!
*	\brief ������ ���ݻ籤 ������ �����Ѵ�.
*
*	\param r ������ ������ ���ݻ籤 ����
*	\param g ������ �ʷϻ� ���ݻ籤 ����
*	\param b ������ �Ķ��� ���ݻ籤 ����
*/
void GLight::SetSpecular(GLfloat r, GLfloat g, GLfloat b)
{
	Specular[0] = r;
	Specular[1] = g;
	Specular[2] = b;
}

/*!
*	\brief ������ ������ ���·� �����Ѵ�.
*/
void GLight::SetPointLight()
{
	Position[3] = 1.0f;
}

/*!
*	\brief ������ ���⼺ ���� ���·� �����Ѵ�.
*/
void GLight::SetDirectionalLight()
{
	Position[3] = 0.0f;
}

/*!
*	\brief ���߱����� ����� ������ �����Ѵ�.
*
*	\param vx ���߱����� ������ x����
*	\param vy ���߱����� ������ y����
*	\param vz ���߱����� ������ z����
*	\param cutoff ���߱����� ����
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
*	\brief ����Ʈ ������
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
*	\brief ������
*
*	\param _pScene ī�޶� ���ѵ� ��鿡 ���� ������
*	\param _Fovy ���� ������ ���� ī�޶��� ȭ��
*	\param _Near ���� ������ �����ϴ� ����� �������� �Ÿ�
*	\param _Far ���� ������ �����ϴ� �� �������� �Ÿ�
*	\param _ViewWidth ����Ʈ ��ȯ�� ���� �ʺ�
*	\param _ViewHeight ����Ʈ ��ȯ�� ���� ����
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
*	\brief ������
*
*	\param _pScene ī�޶� ���ѵ� ��鿡 ���� ������
*	\param Eye ���� ��ǥ�迡�� ǥ���� ī�޶� ������ ��ġ
*	\param Center ī�޶� �ٶ󺸴� ��� ��ǥ���� �� ��
*	\param Up ���� ��ǥ�迡�� ǥ���� ī�޶��� ���� ���� ����
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
*	\brief ������
*
*	\param _pScene ī�޶� ���ѵ� ��鿡 ���� ������
*	\param Left ���� clipping ���
*	\param Right ������clipping ���
*	\param Bottom �Ʒ��� clipping ���
*	\param Top ���� clipping ���
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
*	\brief ���� ������
*
*	\param cpy ����� ī�޶� ��ü
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
*	\brief �Ҹ���
*/
GCamera::~GCamera()
{
	if (pPixelBuf != NULL)
		delete [] pPixelBuf;
	pPixelBuf = NULL;
}

/*!
*	\brief ���Կ�����
*
*	\param rhs ���Ե� �ǿ�����
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
*	\brief ī�޶��� ���������� �����Ѵ�.
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

	// ���� ��� ���� ���
	glGetDoublev(GL_PROJECTION_MATRIX, ProjectionMat);
}

/*!
*	\brief ī�޶��� ������ȯ�� �����Ѵ�.
*/
void GCamera::SetupViewTransform()
{
	// ��ȯ ���� ���
	WC = GTransf(GVector3(0.0, 0.0, ZoomDist), Rot) * GTransf(Pan);
	
	// ��ȯ ���� ����
	glMultMatrixd(cast_arr(cast_mat(WC)));

	// �𵨺� ��� ���� ���
	glGetDoublev(GL_MODELVIEW_MATRIX, ModelviewMat);
}

/*!
*	\brief ī�޶��� ����Ʈ ��ȯ������ �����Ѵ�.
*
*	\param w ����Ʈ ��ȭ�� �ʺ� ����
*	\param h ����Ʈ ��ȯ�� ���� ����
*/
void GCamera::SetupViewport(int w, int h)
{
	ViewWidth = w;
	ViewHeight = h;

// 	if (pPixelBuf != NULL)
// 		delete pPixelBuf;
// 	pPixelBuf = new unsigned char [w * h * 3];

	glViewport(0, 0, ViewWidth, ViewHeight);

	// ����Ʈ ���� ���
	glGetIntegerv(GL_VIEWPORT, Viewport);
}

/*!
*	\brief ī�޶��� �� ����� �����Ѵ�.
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
*	\brief ī�޶��� ȸ�� ����� �����Ѵ�.
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
*	\brief ī�޶��� �� ��/�ƿ� ����� �����Ѵ�.
*
*	\param type ���: �� ��, ����: �� �Ƽ�
*/
void GCamera::Zooming(int type)
{
	double dz = (type > 0) ? 0.05 : -0.05;
	ZoomDist += ZoomDist * dz;
}

/*!
*	\brief ���� ��忡 ���� ī�޶��� ����� �����Ѵ�.
*
*	\param x ����Ʈ���� ���콺�� ���� ��ġ�� x��ǥ
*	\param y ����Ʈ���� ���콺�� ���� ��ġ�� y��ǥ
*/
void GCamera::LButtonDown(int x, int y)
{
	MousePt[0] = x;
	MousePt[1] = y;
}

/*!
*	\brief ���� ��忡 ���� ī�޶��� ����� �����Ѵ�.
*
*	\param x ����Ʈ���� ���콺�� ���� ��ġ�� x��ǥ
*	\param y ����Ʈ���� ���콺�� ���� ��ġ�� y��ǥ
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
*	\brief ī�޶� ���� �������̽��� ������ �Ѵ�.
*/
void GCamera::Render()
{
}

/*!
*	\brief ī�޶��� ��ȯ ������ ����Ѵ�.
*
*	\param fname ī�޶��� ��ȯ ������ ��µ� ���ϸ�
*/
void GCamera::Export(char *fname)
{
	FILE *fp = fopen(fname, "w");
	fprintf(fp, "%d\n", CameraType);	// ī�޶��� ���� Ÿ��
	fprintf(fp, "%lf\n", Fovy);			// ���� ������ ���� ī�޶��� ȭ��(field of view)
	fprintf(fp, "%lf\n", Near);			// ���� ������ ���� �������� �Ÿ�(>0)
	fprintf(fp, "%lf\n", Far);			// ���� ������ ���� �������� �Ÿ�(>0)
	fprintf(fp, "%lf\n", ZoomDist);		// ī�޶� �߽ɿ��� ���� ��ǥ������� �Ÿ�(<0)
	fprintf(fp, "%d\n", ViewWidth);		// ����Ʈ ��ȯ�� ���� �ʺ�
	fprintf(fp, "%d\n", ViewHeight);	// ����Ʈ ��ȯ�� ���� ����
	fprintf(fp, "%lf\t%lf\t%lf\t%lf\n", Rot.W, Rot.X, Rot.Y, Rot.Z);	// ���� ��ǥ���� ȸ�� ����
	fprintf(fp, "%lf\t%lf\t%lf\n", Pan[0], Pan[1], Pan[2]);				// ���� ��ǥ���� �̵� ����
	fclose(fp);
}

/*!
*	\brief ī�޶��� ��ȯ ������ �о�´�.
*
*	\param fname ī�޶��� ��ȯ ������ ���Ե� ���ϸ�
*/
void GCamera::Import(char *fname)
{
	FILE *fp = fopen(fname, "r");
	int tmp;

	fscanf(fp, "%d", &tmp);			// ī�޶��� ���� Ÿ��
	CameraType = (tmp == 0) ? PROJECTION_ORTHO : PROJECTION_PERS;
	fscanf(fp, "%lf", &Fovy);		// ���� ������ ���� ī�޶��� ȭ��(field of view)
	fscanf(fp, "%lf", &Near);		// ���� ������ ���� �������� �Ÿ�(>0)
	fscanf(fp, "%lf", &Far);		// ���� ������ ���� �������� �Ÿ�(>0)
	fscanf(fp, "%lf", &ZoomDist);	// ī�޶� �߽ɿ��� ���� ��ǥ������� �Ÿ�(<0)
	fscanf(fp, "%d", &ViewWidth);	// ����Ʈ ��ȯ�� ���� �ʺ�
	fscanf(fp, "%d", &ViewHeight);	// ����Ʈ ��ȯ�� ���� ����
	fscanf(fp, "%lf%lf%lf%lf", &Rot.W, &Rot.X, &Rot.Y, &Rot.Z);		// ���� ��ǥ���� ȸ�� ����
	fscanf(fp, "%lf%lf%lf", &Pan[0], &Pan[1], &Pan[2]);				// ���� ��ǥ���� �̵� ���� 
	Rot.Normalize();

	fclose(fp);
}

/*!
*	\brief ī�޶��� ȸ�� Ÿ���� �����Ѵ� (��۱���� �����Ѵ�).
*
*	\param fname ī�޶��� ������ ȸ�� Ÿ��
*/
void GCamera::SetArcBallType(TypeArcBall type)
{
	if (type == ArcBallType)
		ArcBallType = ARCBALL_FREE;
	else
		ArcBallType = type;
}

/*!
*	\brief ���� ��ǥ�迡�� ǥ���� �������� 3���� ��ǥ�� ����Ѵ�.
*
*	\param wx �������� ��ũ�� x��ǥ(in)
*	\param wy �������� ��ũ�� y��ǥ(in)
*	\param depth �������� ����(0.0 ~ 1.0) ����(in)
*	\param ox �����Ǳ���, ���� ��ǥ�迡���� ������ x��ǥ�� �����(out)
*	\param oy �����Ǳ���, ���� ��ǥ�迡���� ������ y��ǥ�� �����(out)
*	\param oz �����Ǳ���, ���� ��ǥ�迡���� ������ z��ǥ�� �����(out)
*
*	\note ������ ��ǥ�� �»���� (0, 0)
*/
void GCamera::GetWorldCoord(double wx, double wy, double depth, double *ox, double *oy, double *oz)
{
	::gluUnProject(wx, Viewport[3] - wy, depth, ModelviewMat, ProjectionMat, Viewport, ox, oy, oz);	
}

/*!
*	\brief ��ũ�� ��ǥ�迡�� ǥ���� �������� ��ǥ�踦 ����Ѵ�.
*
*	\param pVert ��ü�� ������ ���� ������(in)
*	\param wx ��ũ�� x��ǥ(out)
*	\param wy ��ũ�� y��ǥ(out)
*	\param wz ��ũ�� depth(0.0 ~ 1.0) ��ǥ(out)
*
*	\note ��ũ�� ��ǥ�� �»���� (0, 0)
*/
void GCamera::GetScreenCoord(GVertex *pVert, double *wx, double *wy, double *wz)
{
	// ������ ����ϱ�
	GPoint3 p = pVert->pMesh->MC * pVert->P;
	gluProject(p[0], p[1], p[2], ModelviewMat, ProjectionMat, Viewport, wx, wy, wz);
	*wy = Viewport[3] - *wy;
}

/*!
*	\brief ��ũ�� ��ǥ�迡�� ������ �������� ��ǥ�� ����Ѵ�.
*
*	\param pPt �������� ���� ������
*	\param wx ��ũ�� x��ǥ
*	\param wy ��ũ�� y��ǥ
*	\param wz ��ũ�� depth��ǥ
*
*	\note ��ũ�� ��ǥ�� �»���� (0, 0)
*/
void GCamera::GetScreenCoord(GCtrlPt *pPt, double *wx, double *wy, double *wz)
{
	// ������ ����ϱ�
	GPoint3 p = pPt->pMesh->MC * GPoint3((*pPt)[0], (*pPt)[1], (*pPt)[2]);
	gluProject(p[0], p[1], p[2], ModelviewMat, ProjectionMat, Viewport, wx, wy, wz);
	*wy = Viewport[3] - *wy;
}

/*!
*	\brief ��ũ�� ��ǥ�迡�� ǥ���� �������� ��ǥ�� ����Ѵ�.
*
*	\param ox 3D ������ x��ǥ
*	\param oy 3D ������ y��ǥ
*	\param oz 3D ������ z��ǥ
*	\param wx ��ũ�� x��ǥ
*	\param wy ��ũ�� y��ǥ
*	\param wz ��ũ�� depth��ǥ
*
*	\note ��ũ�� ��ǥ�� �»���� (0, 0)
*/
void GCamera::GetScreenCoord(double ox, double oy, double oz, double *wx, double *wy, double *wz)
{
	// ������ ����ϱ�
	gluProject(ox, oy, oz, ModelviewMat, ProjectionMat, Viewport, wx, wy, wz);
	*wy = Viewport[3] - *wy;
}

/*!
*	\brief ���� ��ǥ�迡�� ǥ���� ��ũ���� ������ ������ �������� ����Ѵ�.
*
*	\param x ������ x ��ǥ
*	\param y ������ y ��ǥ
*
*	\return ���� ��ǥ�迡�� ǥ���� ��ũ���� ������ ������ �������� ��ȯ�Ѵ�.
*/
GLine GCamera::GetOrthoRay(double x, double y)
{
	GPoint3 p, q;		
	GetWorldCoord(x, y, 0.0, &p[0], &p[1], &p[2]);
	GetWorldCoord(x, y, 0.5, &q[0], &q[1], &q[2]);
	
	return GLine(p, q);
}

/*!
*	\brief ���� ��ǥ�迡�� ǥ���� ���� ���� ���͸� ��ȯ�Ѵ�.
*
*	\return ���� ��ǥ�迡�� ǥ���� ���� ���� ���͸� ��ȯ�Ѵ�.
*/
GVector3 GCamera::GetViewDirection()
{
	return inv(Rot) * GVector3(0.0, 0.0, -1.0);
}

////////////////////////////////////////
/*!
*	\brief ������
*
*	\param fname ����� ���ϸ�
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
*	\brief ���������
*
*	\param cpy ����� �� ��ü
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

	// �޽� ����Ʈ ����
	int NumMesh = (int)cpy.MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
	{
		GMesh *pMesh = new GMesh((*cpy.MeshList[i]));
		MeshList.push_back(pMesh);
	}
}

/*!
*	\brief �Ҹ���
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
*	\brief ���Կ�����
*
*	\param rhs �ǿ�����
*
*	\return ���Ե� �ڽ��� ��ȯ
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

	// �޽� ����Ʈ ����
	int NumMesh = (int)MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
		delete MeshList[i];
	MeshList.clear();

	// �޽� ����Ʈ ����
	NumMesh = (int)rhs.MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
	{
		GMesh *pMesh = new GMesh((*rhs.MeshList[i]));
		MeshList.push_back(pMesh);
	}
	
	return *this;
}

/*!
*	\brief ���콺 ���� ��ư�� ���� ����� ȸ���� �غ��Ѵ�.
*
*	\param x ���콺 x��ǥ
*	\param y ���콺 y��ǥ
*
*	\note �信�� ALTŰ�� ������ ���¿��� ȣ���
*/
void GScene::LButtonDown(int x, int y)
{
	// ����� ȸ���� ���� �غ� �Ѵ�.
	MainCamera.CameraManipulateType = CAMERA_MANIPULATE_ROTATE;
	MainCamera.LButtonDown(x, y);	
}

/*!
*	\brief ���콺 �߰� ��ư�� ���� ����� �̵��� �غ��Ѵ�.
*
*	\param x ���콺 x��ǥ
*	\param y ���콺 y��ǥ
*
*	\note �信�� ALTŰ�� ������ ���¿��� ȣ���
*/
void GScene::MButtonDown(int x, int y)
{
	// ����� �̵��� �غ��Ѵ�.
	MainCamera.CameraManipulateType = CAMERA_MANIPULATE_PAN;
	MainCamera.LButtonDown(x, y);
}

/*!
*	\brief ���콺 �������� ���� ����� ����(ȸ��, �̵�)�� �����Ѵ�.
*
*	\param x ���콺 x��ǥ
*	\param y ���콺 y��ǥ
*/
void GScene::MouseMove(int x, int y)
{
	// ����� ����(ȸ��, �̵�)�� �����Ѵ�.
	MainCamera.MouseMove(x, y);
}

/*!
*	\brief ����� ȸ���� �����Ѵ�.
*
*	\param x ���콺 x��ǥ
*	\param y ���콺 y��ǥ
*/
void GScene::LButtonUp(int x, int y)
{
	MainCamera.CameraManipulateType = CAMERA_MANIPULATE_NONE;
}

/*!
*	\brief ����� �̵��� �����Ѵ�.
*
*	\param x ���콺 x��ǥ
*	\param y ���콺 y��ǥ
*/
void GScene::MButtonUp(int x, int y)
{
	MainCamera.CameraManipulateType = CAMERA_MANIPULATE_NONE;
}

/*!
*	\brief ����� �������� �����Ѵ�.
*
*	\param delta ���콺 ���� ����
*/
void GScene::MouseWheel(int delta)
{
	if (delta > 0)
		MainCamera.Zooming(-1);
	else
		MainCamera.Zooming(1);
}

/*!
*	\brief ��(ī�޶�) ��ǥ�迡�� ǥ���� ���� ��ǥ�踦 ��ȯ�Ѵ�.
*
*	\return ��(ī�޶�) ��ǥ��迡�� ǥ���� ���� ��ǥ�踦 ��ȯ�Ѵ�.
*/
GTransf GScene::GetWC()
{
	return MainCamera.WC;
}

/*!
*	\brief ��鿡 ���ο� �޽��� �߰��Ѵ�.
*
*	\param pMesh �߰��� �޽��� ���� ������
*/
void GScene::AddMesh(GMesh *pMesh)
{
	// �޽��� ���Ե� ����� �����͸� �����ϰ�,
	pMesh->pScene = this;

	// �������� ���� ����, ������ �� ��� ���� �����ϰ�,
	pMesh->CreateMesh();
		
	// �޽��� ��鿡 �߰��ϰ�,
	GMesh *pParent = FindMesh(MeshList, pMesh->ParentMeshName);
	if (pParent == NULL)
		MeshList.push_back(pMesh);
	else
	{
		pMesh->DelX = inv(pParent->MC) * pMesh->MC;
		pParent->SubMeshList.push_back(pMesh);
	}

	// �޽��� ���ǵ� ������ ���ٸ�,
	if (pMesh->MtlList.empty())
	{
		// ���� ������ �����Ͽ� �޽��� �߰��ϰ�, Ȱ��ȭ �Ѵ�.
		GMaterial mtl = ::get_material_random();
		pMesh->AddMtl(mtl);
		pMesh->SetMtl(mtl.Name);	
	}	

	// ���� �ֱ� �߰��� �޽����� ����Ѵ�.
	pRecentMesh = pMesh;
}

/*!
*	\brief ��鿡 ���ο� ���� ���� �߰��Ѵ�.
*
*	\param Map �߰��� ���� ��
*/
// void GScene::AddMapDisp(const GMapDisp &Map)
// {
// 	MapDispList.push_back(Map);
// }

/*!
*	\brief ��鿡 ���ο� ������ �߰��Ѵ�.
*
*	\param Light �߰��� ����
*/
void GScene::AddLight(const GLight &Light)
{
	LightList.push_back(Light);
}

/*!
*	\brief ��鿡 ���ο� ī�޶� �߰��Ѵ�.
*
*	\param Cam �߰��� ī�޶�
*/
void GScene::AddCamera(const GCamera &Cam)
{
	AuxCameraList.push_back(Cam);
}

/*!
*	\brief �޽� ����Ʈ���� �־��� �̸��� �޽��� �����Ѵ�.
*
*	\param MeshList �޽� ����Ʈ
*	\param Name ������ �޽��� �̸�
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
*	\brief ����� OpenGL������ ���¸� �ʱ�ȭ �Ѵ�.
*	\warning �ݵ�� OpenGL ���¿��� ȣ��Ǿ�� �Ѵ�.
*/
void GScene::InitScene()
{
	// ���� ������ �ʱ�ȭ �ϰ�,
	theScene = this;

	// ���� �׽�Ʈ�� Ȱ��ȭ �ϰ�,
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glDepthFunc(GL_LESS);

	// ���� ���� �ʱ�ȭ �ϰ�,
	glEnable(GL_NORMALIZE);	
	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, TRUE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, false);

	// ��Ƽ �ٸ������ Ȱ��ȭ �ϰ�,
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POINT_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_POLYGON_SMOOTH, GL_NICEST);

	// ��� ���� ����� �����г��� �ʱ�ȭ �ϰ�,
	HeadUpDisplay.pScene = this;

	// ������ ���丮 ������ �о�ͼ�,
	char path[256];
	_getcwd(path, 256);
	RootDir = std::string(path);
	std::cout<< RootDir.c_str() << std::endl;

	if (HeadUpDisplay.TextureList.empty())
	{
		// �ؽ�ó �ʱ�ȭ
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
	
	// ����� ������ ���� ī�޶� �ʱ�ȭ �ϰ�,
	AuxCameraList.clear();
	MainCamera = GCamera(this, GVector3(120.0, 80.0, 120.0), GVector3(0.0, 0.0, 0.0), GVector3(0.0, 1.0, 0.0));

	// ������ ��ġ ������ ���� �𵨺� ����� �ʱ�ȭ �ϰ�,
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
			
	// ������ �ʱ�ȭ�ϰ�,
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

	// �޽�����Ʈ�� �ʱ�ȭ �Ѵ�.
	int NumMesh = (int)MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
		delete MeshList[i];
	MeshList.clear();
}

/*!
*	\brief ����� ����Ʈ ��ȯ�� �����Ѵ�.
*/
void GScene::SetupViewport(int x, int y)
{
	MainCamera.SetupViewport(x, y);
	HeadUpDisplay.Resize(x, y);
	HeadUpDisplay.Parallel = false;
}

/*!
*	\brief ����� ���������� ��������� �����Ѵ�.
*/
void GScene::SetupViewFrustum()
{
	MainCamera.SetupViewFrustum();
}

/*!
*	\brief ����� ������ȯ�� �����Ѵ�.
*/
void GScene::SetupViewTransform()
{
	MainCamera.SetupViewTransform();
}

/*!
*	\brief \a Box�� �� �����ϵ��� ���������� �����Ѵ�.
*
*	\param Box ��ü�� ��� ����
*/
void GScene::AdjustViewFrustum(const GBndBox &Box)
{
	// �ҽ� ī�޶�� Ÿ�� ī�޶� �����ϰ�,
	SrcCamera = MainCamera;
	TarCamera = MainCamera;

	// �� ��ǥ�迡�� ǥ���� ��� ������ �糡���� ���ϰ�,
	GPoint3 p = MainCamera.WC * Box.pMesh->MC * Box.MinPt;
	GPoint3 q = MainCamera.WC * Box.pMesh->MC * Box.MaxPt;

	// ī�޶� ��ǥ�迡�� ǥ���� ��� ������ �߽��� ��ġ�� ���ϰ�,
	GPoint3 r = p + (q - p) * 0.5;

	// ȸ�� ��ȯ ���� ��, ���� ��ǥ���� �߽ɿ��� ��� ������ �߽������� ������ ����� ��,
	GVector3 v = r - GPoint3(0.0, 0.0, MainCamera.ZoomDist);

	// ���� ������ ȸ�� ��ȯ�� ���� ��ǥ���� ������ �ٲٰ�, 
	v = inv(MainCamera.WC) * v;

	// �� ������ ������ ��,
	TarCamera.Pan -= v;

	// ��� ������ ũ�⿡ ���� ������ �������� �����Ѵ�.
	double sz = dist(p, q); 
	TarCamera.ZoomDist = -sz / tan(TarCamera.Fovy * M_PI / 180.0);

	// Ÿ�̸Ӹ� �����Ͽ� ī�޶� ��ȯ�� �ǽ��Ѵ�.
	SetTimer(TIMER_EVENT_CAMERA_TRANSIT, 46);
}

/*!
*	\brief ����� ������ �Ѵ�.
*/
void GScene::Render()
{
	// ����� ���� ���� �� ���� ��� ����
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	SetupViewFrustum();
	
	// ����� ���� ��ȯ�� ����
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	SetupViewTransform();

	// HUD�� �������� ����
	HeadUpDisplay.Render();
	
	// ����� �ٴ� ���(ZX)�� ������
	RenderFloor();

	// ��鿡 ���Ե� �޽��� ������
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
*	\brief ����� ���� ����� ������ �Ѵ�.
*/
void GScene::RenderFloor()
{
	if (!bShowFloor)
		return;

	// Anti-aliasing�� Ȱ��ȭ �ϰ�,
	glDisable(GL_LIGHTING);
// 	glEnable(GL_BLEND);
// 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// 	glEnable(GL_LINE_SMOOTH);
// 	glLineWidth(0.5f);

	// �ٴ� ����� �׸���,
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

	// �߽ɼ��� �׸���.
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

	// Anti-aliasing�� ��Ȱ��ȭ �Ѵ�.
	glEnable(GL_LIGHTING);
//	glDisable(GL_BLEND);
//	glDisable(GL_LINE_SMOOTH);
}

/*!
*	\brief �ڵ����� ȣ��Ǵ� Ÿ�̸� �Լ�
*
*	\param hWnd ������ �ڵ�
*	\param uMsg Ÿ�̸� �޽��� (WM_TIMER)
*	\param nIDEvent Ÿ�̸� ��ȣ
*	\param dwTime ������ ���� �� ����ð�
*/
void CALLBACK theTimer(HWND hWnd, UINT uMsg, UINT nIDEvent, DWORD dwTime)
{
	switch (nIDEvent)
	{
	case TIMER_EVENT_CAMERA_TRANSIT:
		{
			static int frm_idx = 0;
			double t = frm_idx / 9.0;

			// ����� ���� ī�޶��� ��ȯ�� ����ϰ�,
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
*	\brief ��鿡 Ÿ�̸Ӹ� ��ġ�Ѵ�.
*
*	\param nIDEvent Ÿ�̸� ��ȣ
*	\param uElapse ������ �ð� ���� (ms ����)
*/
void GScene::SetTimer(UINT nIDEvent, UINT uElapse)
{
	::SetTimer(hWndView, nIDEvent, uElapse, (TIMERPROC)theTimer);
	bTimer = true;
}

/*!
*	\brief ��鿡 Ÿ�̸Ӹ� �����Ѵ�.
*
*	\param nIDEvent ������ Ÿ�̸� ��ȣ
*/
void GScene::KillTimer(UINT nIDEvent)
{
	::KillTimer(hWndView, nIDEvent);
	bTimer = false;
}

/*!
*	\brief ��鿡 ���Ե� �޽��� �ִϸ��̼��� �����Ѵ�.
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
*	\brief ��鿡 ���Ե� ���� �� ����Ʈ���� �̸��� \a Name�� ���� ã�´�.
*
*	\param Name ã�� ���� ���� �̸�
*
*	\return ���� ���� ������
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
*	\brief ParentMesh�� ���������� ���Ͽ� PreOrder�� �湮�� ���� ����Ʈ�� �����Ѵ�.
*
*	\param pScene ��鿡 ���� ������
*	\param MeshList �ڽ� �޽����� ����� ����Ʈ
*	\param ParentMesh �θ� �޽�, ���࿡ NULL�̸� ��鿡 ���Ե� ��� �޽��� ����
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
*	\brief �־��� �޽� ����Ʈ���� ���� �޽��� ��� �˻��ϸ� �޽��� ã�´�.
*
*	\param MeshList �޽� ����Ʈ
*	\param Name ã�� �޽��� �̸�
*
*	\return �޽��� ������
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
*	\brief ������
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
*	\brief ���� ������
*
*	\param cpy ����� ��ü
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
*	\brief ���Կ�����
*
*	\param rhs ���Ե� ��ü
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
*	\brief Control Panel�� ���������� ����Ѵ�.
*	
*	\param width �������� �ʺ�
*	\param height �������� ����
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
*	\brief HUD�� ������ ������ �Ѵ�.
*/
void GHud::Render()
{
	// ���� ������ȯ ����� �����ϰ�,
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// ���ο� ���������� �����Ѵ�.
	glLoadIdentity();	
	glOrtho(ViewFrustum[0], ViewFrustum[1], ViewFrustum[2], ViewFrustum[3], ViewFrustum[4], ViewFrustum[5] + 1.0);
	
	// ���� �𵨺� ����� �����ϰ�,
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	{
		// ���, FPS, ȸ����, ��ť���� �������ϰ�,
		RenderBackGround();
		RenderFPS();
		RenderLogo();
		RenderOriginAxis();
		RenderViewCube();
		RenderMessage();	
	}
	// �𵨺� ����� �����ϰ�,
	glPopMatrix();

	// ������ȯ ����� �����Ѵ�.
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// �𵨺� ����� �����Ѵ�.
	glMatrixMode(GL_MODELVIEW);
}

/*!
*	\brief ����� �׷����Ʈ ����� ������ �Ѵ�.
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
*	\brief ����� FPS�� ����Ѵ�.
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

		// FPS�� �������ϰ�,
		int h = pScene->MainCamera.ViewHeight;
		RenderText(ViewFrustum[0] * 0.98, ViewFrustum[3] * 0.95, ViewFrustum[4] * 0.9, buffer, 1.0, 1.0, 1.0);

		// �ΰ� �������Ѵ�.
		glEnable(GL_LIGHTING);
	}		
}

/*!
*	\brief ����� ȸ�� �߽����� �������Ѵ�.
*/
void GHud::RenderOriginAxis()
{
	if (DisplayInfo & HUD_ORIGIN_AXIS)
	{
		// ����� ���� �׽�Ʈ�� ����,
		glDisable(GL_LIGHTING);

		glPushMatrix();
		{
			//glTranslated(0.0, 0.0, ViewFrustum[4] * 0.9);
			glTranslated(ViewFrustum[0] + 1.2, ViewFrustum[2] + 1.2, ViewFrustum[4] * 0.9);
			// ȸ����ȯ ����
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
*	\brief ����� ��ť���� �������Ѵ�.
*/
void GHud::RenderViewCube()
{
	if (DisplayInfo & HUD_VIEW_CUBE)
	{
		// ������ ����,
		glDisable(GL_LIGHTING);

		glPushMatrix();
		{
			// ��ť�� �������� ���� ��ȯ
			//glTranslated(ViewFrustum[1] - 1.2, ViewFrustum[3] - 1.2, -ViewFrustum[5] * 0.8);
			glTranslated(ViewFrustum[1] - 1.2, ViewFrustum[3] - 1.2, ViewFrustum[4] * 0.9);

			glColor3d(0.8, 0.8, 0.8);
			if (Parallel)
			{
				// ���� ȭ��ǥ
				glBegin(GL_TRIANGLES);
				{
					glVertex2d(0.55, 0.0);
					glVertex2d(0.85, -0.15);
					glVertex2d(0.85, 0.15);
				}
				glEnd();

				// ���� ȭ��ǥ
				glBegin(GL_TRIANGLES);
				{
					glVertex2d(-0.55, 0.0);
					glVertex2d(-0.85, 0.15);
					glVertex2d(-0.85, -0.15);
				}
				glEnd();

				// ���� ȭ��ǥ
				glBegin(GL_TRIANGLES);
				{
					glVertex2d(0.0, -0.55);
					glVertex2d(-0.15, -0.85);
					glVertex2d(0.15, -0.85);
				}
				glEnd();

				// ���� ȭ��ǥ
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

			// Ȩ������
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

			// ȸ����ȯ ����
			glMultMatrixd(cast_mat(pScene->MainCamera.Rot).M);

			double cube_size = 0.5;
			// �ո�
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

			// ���ʸ�
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

			// ���ʸ�
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

			// �����ʸ�
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

			// �ٴڸ�
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

			// ����	
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
*	\brief �ΰ� �������Ѵ�.
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
*	\brief �߰� �޽����� ������ �Ѵ�.
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
*	\brief �ؽ�Ʈ�� ������ �Ѵ�.
*
*	\param x �������� �ؽ�Ʈ�� x ��ǥ
*	\param y �������� �ؽ�Ʈ�� y ��ǥ
*	\param z �������� �ؽ�Ʈ�� z ��ǥ
*	\param str �������� �ؽ��� �迭
*	\param r �������� �ؽ�Ʈ ������ R ����
*	\param g �������� �ؽ�Ʈ ������ G ����
*	\param b �������� �ؽ�Ʈ ������ B ����
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
*	\brief Control Panel�� �޴� ���� �� ����� �����Ѵ�.
*/
void GHud::LButtonDown(int x, int y)
{
	GLuint selectBuf[SELECT_BUFFER_SIZE];
	GLint hits;
	GLint viewport[4];
	double cube_size = 0.5;

	// �� ��Ʈ ������ ���ϰ�,
	glGetIntegerv(GL_VIEWPORT, viewport);

	// ���� ���۸� ������ ��,
	glSelectBuffer(SELECT_BUFFER_SIZE, selectBuf);

	// ������ ��带 ���� ���� �����Ѵ�.
	glRenderMode(GL_SELECT);

	// ���ӽ����� �ʱ�ȭ�ϰ� dummy(0)�� �ְ�,
	glInitNames();
	glPushName(0);

	// ��������� �����Ͽ�
	glMatrixMode(GL_PROJECTION);
	{
		// ���� ���¸� �����ϰ�, ������ķ� �ʱ�ȭ ����,
		glPushMatrix();
		glLoadIdentity();

		// ���� ����� �����Ѵ�.
		int offset = 1;
		gluPickMatrix((double)x, (GLdouble)(viewport[3] - y), offset, offset, viewport);
		glOrtho(ViewFrustum[0], ViewFrustum[1], ViewFrustum[2], ViewFrustum[3], ViewFrustum[4], ViewFrustum[5] + 1.0);

		// �𵨺� ����� �����Ͽ�,
		glMatrixMode(GL_MODELVIEW);
		{
			// ���� ���¸� �����ϰ�, ������ķ� �ʱ�ȭ ����, 
			glPushMatrix();
			glLoadIdentity();

			// Ȩ �������� �׸���,
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

			// ��ǥ ����� ���� ���⿡�� ���� �ִٸ�,
			if (Parallel)
			{
				// ���� ȭ��ǥ
				glLoadName(7);
				glBegin(GL_TRIANGLES);
				{
					glVertex2d(0.55, 0.0);
					glVertex2d(0.85, -0.15);
					glVertex2d(0.85, 0.15);
				}
				glEnd();

				// ���� ȭ��ǥ
				glLoadName(8);
				glBegin(GL_TRIANGLES);
				{
					glVertex2d(-0.55, 0.0);
					glVertex2d(-0.85, 0.15);
					glVertex2d(-0.85, -0.15);
				}
				glEnd();

				// ���� ȭ��ǥ��
				glLoadName(9);
				glBegin(GL_TRIANGLES);
				{
					glVertex2d(0.0, -0.55);
					glVertex2d(-0.15, -0.85);
					glVertex2d(0.15, -0.85);
				}
				glEnd();

				// ���� ȭ��ǥ�� �׸���,
				glLoadName(10);
				glBegin(GL_TRIANGLES);
				{
					glVertex2d(0.0, 0.55);
					glVertex2d(0.15, 0.85);
					glVertex2d(-0.15, 0.85);			
				}
				glEnd();
			}

			// ȸ����ȯ ������ ��,
			glMultMatrixd(cast_mat(pScene->MainCamera.Rot).M);

			// �� ť���� �ո�
			glLoadName(1);
			glBegin(GL_POLYGON);
			{
				glVertex3d(-cube_size, -cube_size, cube_size);
				glVertex3d(cube_size, -cube_size, cube_size);
				glVertex3d(cube_size, cube_size, cube_size);
				glVertex3d(-cube_size, cube_size, cube_size);
			}
			glEnd();

			// ���ʸ�
			glLoadName(2);
			glBegin(GL_POLYGON);
			{
				glVertex3d(-cube_size, -cube_size, -cube_size);
				glVertex3d(-cube_size, -cube_size, cube_size);
				glVertex3d(-cube_size, cube_size, cube_size);
				glVertex3d(-cube_size, cube_size, -cube_size);
			}
			glEnd();

			// ���ʸ�
			glLoadName(3);
			glBegin(GL_POLYGON);
			{
				glVertex3d(cube_size, -cube_size, -cube_size);
				glVertex3d(-cube_size, -cube_size, -cube_size);
				glVertex3d(-cube_size, cube_size, -cube_size);
				glVertex3d(cube_size, cube_size, -cube_size);
			}
			glEnd();

			// �����ʸ�
			glLoadName(4);
			glBegin(GL_POLYGON);
			{
				glVertex3d(cube_size, -cube_size, cube_size);
				glVertex3d(cube_size, -cube_size, -cube_size);
				glVertex3d(cube_size, cube_size, -cube_size);
				glVertex3d(cube_size, cube_size, cube_size);
			}
			glEnd();

			// �ٴڸ�
			glLoadName(5);
			glBegin(GL_POLYGON);
			{
				glVertex3d(-cube_size, -cube_size, -cube_size);
				glVertex3d(cube_size, -cube_size, -cube_size);
				glVertex3d(cube_size, -cube_size, cube_size);
				glVertex3d(-cube_size, -cube_size, cube_size);
			}
			glEnd();

			// ������ ���ʷ� �׸���.
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
		// �𵨺� ����� �����ϰ�,
		glPopMatrix();
	}
	// ���� ����� �ٽ� �����Ͽ�, ���� ���� ����� �����Ѵ�.
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();	
	
	// �ٽ� �𵨺� ����� ������ ��,
	glMatrixMode(GL_MODELVIEW);

	// ���õ� ��ü�� ���� ���Ͽ�,
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
*	\brief ����� ���������� �����Ѵ�.
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
*	\brief ����� ���������� �����Ѵ�.
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
*	\brief ����� ���������� �����Ѵ�.
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
*	\brief ����� ���������� �����Ѵ�.
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
*	\brief ����� ���������� �����Ѵ�.
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
*	\brief ����� ���������� �����Ѵ�.
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
*	\brief ����� ���������� �����Ѵ�.
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
*	\brief ����� ���������� �����Ѵ�.
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
*	\brief ����� ���������� �����Ѵ�.
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
*	\brief ����� ���������� �����Ѵ�.
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
*	\brief ����� ���������� �����Ѵ�.
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
