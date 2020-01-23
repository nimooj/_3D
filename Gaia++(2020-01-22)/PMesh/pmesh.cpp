#include "pmesh.h"

/*!
*	\brief ��ü�� ǥ������ ����Ѵ�.
*
*	\pram pMesh ǥ������ ����� �޽�
*
*	\return �ﰢ���� ������ ������ ���� ��ü�� ǥ������ ��ȯ�Ѵ�.
*/
extern "C" __declspec(dllexport) 
double get_mesh_area2(GMesh *pMesh)
{
	double Area = 0.0;
	int NumFace = pMesh->GetNumFace();
	for (int i = 0; i < NumFace; ++i)
	{
		int *vidx = pMesh->Faces[i].vIdx;
		GPoint3 p = pMesh->Verts[vidx[0]].P;
		GPoint3 q = pMesh->Verts[vidx[1]].P;
		GPoint3 r = pMesh->Verts[vidx[2]].P;

		Area += norm((q - p) ^ (r - p)) * 0.5;
	}

	return Area;
}