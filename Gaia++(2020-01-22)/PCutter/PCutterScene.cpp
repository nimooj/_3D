#include "stdafx.h"
#include "PCutter.h"

PCutterScene::PCutterScene()
: GScene()
{
	m_Width = 0;
	m_Height = 0;
}

PCutterScene::PCutterScene(const PCutterScene &cpy)
: GScene(cpy)
{
	m_Width = cpy.m_Width;
	m_Height = cpy.m_Height;
}

PCutterScene::~PCutterScene()
{
}

PCutterScene &PCutterScene::operator = (const PCutterScene &rhs)
{
	GScene::operator =(rhs);

	m_Width = rhs.m_Width;
	m_Height = rhs.m_Height;

	return *this;
}

void PCutterScene::InitScene()
{
	// ����� �̸� �ʱ�ȭ
	FileName = "noname01.gff";

	// OpenGL �ʱ�ȭ
	//InitGL();

	// ī�޶� �ʱ�ȭ

	AuxCameraList.clear();
	MainCamera = GCamera(this, -0.1, 1.1, -0.1, 1.1);

	// ������ �ʱ�ȭ
	//InitLight();

	// ������ �ʱ�ȭ �ϰ�,
	MtrlList.clear();
	GMaterial mat_white("mat_white");
	mat_white.SetAmbient(1.0, 1.0, 1.0, 0.5);
	mat_white.SetDiffuse(1.0, 1.0, 1.0, 0.5);
	mat_white.SetSpecular(1.0, 1.0, 1.0, 0.5);

	GMaterial mat_red("mat_red");
	mat_red.SetAmbient(1.0, 0.0, 0.0, 0.5);
	mat_red.SetDiffuse(1.0, 0.0, 0.0, 0.5);
	mat_red.SetSpecular(1.0, 1.0, 1.0, 0.5);

	GMaterial mat_green("mat_green");
	mat_green.SetAmbient(0.0, 1.0, 0.0, 0.5);
	mat_green.SetDiffuse(0.0, 1.0, 0.0, 0.5);
	mat_green.SetSpecular(1.0, 1.0, 1.0, 0.5);

	GMaterial mat_blue("mat_blue");
	mat_blue.SetAmbient(0.0, 0.0, 1.0, 0.5);
	mat_blue.SetDiffuse(0.0, 0.0, 1.0, 0.5);
	mat_blue.SetSpecular(1.0, 1.0, 1.0, 0.5);

	GMaterial mat_black("mat_black");
	mat_black.SetAmbient(0.0, 0.0, 0.0, 0.5);
	mat_black.SetDiffuse(0.0, 0.0, 0.0, 0.5);
	mat_black.SetSpecular(1.0, 1.0, 1.0, 0.5);

	GMaterial mat_gray("mat_gray");
	mat_gray.SetAmbient(0.5, 0.5, 0.5, 0.5);
	mat_gray.SetDiffuse(0.5, 0.5, 0.5, 0.5);
	mat_gray.SetSpecular(0.5, 0.5, 0.5, 0.5);

	AddMaterial(mat_white);
	AddMaterial(mat_red);
	AddMaterial(mat_green);
	AddMaterial(mat_blue);
	AddMaterial(mat_black);
	AddMaterial(mat_gray);
	
	// �޽� ����Ʈ �ʱ�ȭ
	int NumMesh = (int)MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
		delete MeshList[i];
	MeshList.clear();

	// ����� �ٴ� ��� �ʱ�ȭ
	bShowFloor = false;
}

void PCutterScene::RenderFloor()
{
	glDisable(GL_LIGHTING);
	glColor3d(0.0, 0.0, 0.0);

	// �׵θ��� �׸���,
	glBegin(GL_LINE_LOOP);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(1.0, 0.0, 0.0);
	glVertex3f(1.0, 1.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	glEnd();

	// ��ǥ�踦 �׸���,
	glColor3d(0.6, 0.6, 0.6);
	for (int i = 1; i < 9; ++i)
	{
		glBegin(GL_LINES);
		glVertex3f(0.0, i / 9.0, 0.0);
		glVertex3f(1.0, i / 9.0, 0.0);
		glEnd();

		glBegin(GL_LINES);
		glVertex3f(i / 9.0, 0.0, 0.0);
		glVertex3f(i / 9.0, 1.0, 0.0);
		glEnd();
	}

	// x���� �׸���,
	glColor3d(1.0, 0.0, 0.0);
	glLineWidth(1.5);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(1.05, 0.0, 0.0);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex3f(1.03, 0.01, 0.0);
	glVertex3f(1.03, -0.01, 0.0);
	glVertex3f(1.05, 0.0, 0.0);
	glEnd();

	// y���� �׸���,
	glColor3d(0.0, 1.0, 0.0);
	glLineWidth(1.5);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 1.05, 0.0);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex3f(-0.01, 1.03, 0.0);
	glVertex3f(0.01, 1.03, 0.0);
	glVertex3f(0.0, 1.05, 0.0);
	glEnd();

	glLineWidth(1.0);	
	glEnable(GL_LIGHTING);
}

void PCutterScene::Render()
{
	// ����� ���� ���� �� ���� ��� ����
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	SetupViewFrustum();

	// ����� ���� ��ȯ�� ����
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	SetupViewTransform();

	RenderFloor();

	// ��鿡 ���Ե� �޽��� ������
	int NumMesh = (int)MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
		MeshList[i]->Render();
}
