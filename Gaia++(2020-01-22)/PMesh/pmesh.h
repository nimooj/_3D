// �ʿ��� ��� ������ �����Ѵ�.
#include <algorithm>
#include "..\usr\include\gmath.h"
#include "..\usr\include\gobject.h"
#include "..\usr\include\gscene.h"

// ��ũ�� �ܺ� ���̺귯���� �����Ѵ�.
#pragma comment(lib, "gmath.lib")
#pragma comment(lib, "gobject.lib")
#pragma comment(lib, "gscene.lib")	
#pragma comment(lib, "glut32.lib")

/*! \brief ��ü�� ǥ������ ����ϴ� �Լ� */
extern "C" __declspec(dllexport) double get_mesh_area2(GMesh *pMesh);
