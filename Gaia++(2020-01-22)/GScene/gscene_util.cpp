#include "stdafx.h"

/*!
*	\brief ��θ��� ������ ���ϸ�(Ȯ��� ����)�� ��ȯ�Ѵ�.
*	
*	\param full_path ��θ��� ������ ������ �̸�
*
*	\return \a full_path���� ��θ��� ������ ���ϸ�(Ȯ���)�� ��ȯ�Ѵ�.
*/
std::string get_file_name_ext(std::string full_path)
{
	int idx = full_path.rfind("\\");
	std::string ret = full_path.substr(idx + 1, full_path.length() - idx - 1);

	return ret;
}

/*!
*	\brief ��θ��� ������ ���ϸ�(Ȯ��� ������)�� ��ȯ�Ѵ�.
*	
*	\param full_path ��θ��� ������ ������ �̸�
*
*	\return \a full_path���� ��θ��� ������ ���ϸ�(Ȯ��� ������)�� ��ȯ�Ѵ�.
*/
std::string get_file_name(std::string full_path)
{
	int idx0 = full_path.rfind("\\");
	int idx1 = full_path.rfind(".");
	std::string ret = full_path.substr(idx0 + 1, idx1 - idx0 - 1);

	return ret;
}

/*!
*	\brief ������ Ȯ���ڸ� ��ȯ�Ѵ�.
*	
*	\param full_path ��θ��� ������ ������ �̸�
*
*	\return \a full_path���� ���� Ȯ���ڸ� ��ȯ�Ѵ�.
*/
std::string get_file_ext(std::string full_path)
{
	int idx = full_path.rfind(".");
	std::string ret = full_path.substr(idx + 1, full_path.length() - idx - 1);

	return ret;
}

/*!
*	\brief ������ ���丮 ��θ��� ��ȯ�Ѵ�.
*	
*	\param full_path ��θ��� ������ ������ �̸�
*
*	\return \a full_path���� ���丮 ��θ��� ��ȯ�Ѵ�.
*/
std::string get_file_path(std::string full_path)
{
	int idx = full_path.rfind("\\");
	std::string ret = full_path.substr(0, idx);

	return ret;
}

/*!
*	\brief ������ ���ڿ��� ��ȯ�Ͽ� ��ȯ�Ѵ�.
*	
*	\param i ���ڿ��� ��ȯ�� ����
*
*	\return ���ڿ��� ��ȯ�� ����
*/
std::string cast_str(int i)
{
	char buffer[128];
	itoa(i, buffer, 10);

	return std::string(buffer);
}

/*!
*	\brief �Ǽ��� ���ڿ��� ��ȯ�Ͽ� ��ȯ�Ѵ�.
*	
*	\param v ���ڿ��� ��ȯ�� �Ǽ�
*
*	\return ���ڿ��� ��ȯ�� �Ǽ�
*/
std::string cast_str(float v)
{
	char buffer[128];
	sprintf(buffer, "%f", v);

	return std::string(buffer);
}

/*!
*	\brief �Ǽ��� ���ڿ��� ��ȯ�Ͽ� ��ȯ�Ѵ�.
*	
*	\param v ���ڿ��� ��ȯ�� �Ǽ�
*
*	\return ���ڿ��� ��ȯ�� �Ǽ�
*/
std::string cast_str(double v)
{
	char buffer[128];
	sprintf(buffer, "%lf", v);

	return std::string(buffer);
}

/*!
*	\brief Ray casting�� ���� scalar displacement�� ����Ѵ�.
*	
*	\param pBaseMesh ��� �޽�
*	\param pTarMesh ��� �޽�
*	\param Distances ���ø��� ������ �����
*	\param vIdxs ���ø������ ������ ���� ������ �ε����� �����
*	\param UpperBound ��� ������ ������ �ִ밪
*	\param bCullBackFace ������ �ﰢ���� �޸鿡�� ������ ��� ���� ����
*
*/
void get_scalar_displacement(GMesh *pBaseMesh, GMesh *pTarMesh, std::vector<double> &Distances, std::vector<int> &vIdxs, double UpperBound, bool bCullBackFace)
{
	int vnum = (int)pBaseMesh->Verts.size();
	int fnum = (int)pTarMesh->Faces.size();
	for (int i = 0; i < vnum; ++i)
	{
		Distances.push_back(UpperBound);
		GPoint3 p = pBaseMesh->Verts[i].P;
		GVector3 n = pBaseMesh->Verts[i].N;
		GLine l(p, n);

		bool flag = false;
		for (int j = 0; j < fnum; ++j)
		{
			GFace &f = pTarMesh->Faces[j];
			GPoint3 p1 = pTarMesh->Verts[f.vIdx[0]].P;
			GPoint3 p2 = pTarMesh->Verts[f.vIdx[1]].P;
			GPoint3 p3 = pTarMesh->Verts[f.vIdx[2]].P;

			GPoint3 q;
			double t, alpha, beta, gamma;
			bool ret = ::intersect_line_triangle(p1, p2, p3, l, t, q, alpha, beta, gamma, bCullBackFace);
			if (ret)
			{
				if (ABS(t) < ABS(Distances[i]))
				{
					flag = true;
					Distances[i] = t;
				}
//  				pDisp[i] = t;
// 				break;
			}
		}

		if (!flag)
		{
			vIdxs.push_back(i);
			Distances[i] = 0.0;
		}
	}
}