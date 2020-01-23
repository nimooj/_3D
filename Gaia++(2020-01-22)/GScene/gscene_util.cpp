#include "stdafx.h"

/*!
*	\brief 경로명을 제외한 파일명(확장명 포함)을 반환한다.
*	
*	\param full_path 경로명을 포함한 파일의 이름
*
*	\return \a full_path에서 경로명을 제외한 파일명(확장명)을 반환한다.
*/
std::string get_file_name_ext(std::string full_path)
{
	int idx = full_path.rfind("\\");
	std::string ret = full_path.substr(idx + 1, full_path.length() - idx - 1);

	return ret;
}

/*!
*	\brief 경로명을 제외한 파일명(확장명 불포함)을 반환한다.
*	
*	\param full_path 경로명을 포함한 파일의 이름
*
*	\return \a full_path에서 경로명을 제외한 파일명(확장명 불포함)을 반환한다.
*/
std::string get_file_name(std::string full_path)
{
	int idx0 = full_path.rfind("\\");
	int idx1 = full_path.rfind(".");
	std::string ret = full_path.substr(idx0 + 1, idx1 - idx0 - 1);

	return ret;
}

/*!
*	\brief 파일의 확장자를 반환한다.
*	
*	\param full_path 경로명을 포함한 파일의 이름
*
*	\return \a full_path에서 파일 확장자를 반환한다.
*/
std::string get_file_ext(std::string full_path)
{
	int idx = full_path.rfind(".");
	std::string ret = full_path.substr(idx + 1, full_path.length() - idx - 1);

	return ret;
}

/*!
*	\brief 파일의 디렉토리 경로명을 반환한다.
*	
*	\param full_path 경로명을 포함한 파일의 이름
*
*	\return \a full_path에서 디렉토리 경로명을 반환한다.
*/
std::string get_file_path(std::string full_path)
{
	int idx = full_path.rfind("\\");
	std::string ret = full_path.substr(0, idx);

	return ret;
}

/*!
*	\brief 정수를 문자열로 변환하여 반환한다.
*	
*	\param i 문자열로 변환될 정수
*
*	\return 문자열로 변환된 정수
*/
std::string cast_str(int i)
{
	char buffer[128];
	itoa(i, buffer, 10);

	return std::string(buffer);
}

/*!
*	\brief 실수를 문자열로 변환하여 반환한다.
*	
*	\param v 문자열로 변환될 실수
*
*	\return 문자열로 변환된 실수
*/
std::string cast_str(float v)
{
	char buffer[128];
	sprintf(buffer, "%f", v);

	return std::string(buffer);
}

/*!
*	\brief 실수를 문자열로 변환하여 반환한다.
*	
*	\param v 문자열로 변환될 실수
*
*	\return 문자열로 변환된 실수
*/
std::string cast_str(double v)
{
	char buffer[128];
	sprintf(buffer, "%lf", v);

	return std::string(buffer);
}

/*!
*	\brief Ray casting을 통해 scalar displacement를 계산한다.
*	
*	\param pBaseMesh 기반 메쉬
*	\param pTarMesh 대상 메쉬
*	\param Distances 샘플링된 변위가 저장됨
*	\param vIdxs 샘플링결과를 구하진 못한 정점의 인덱스가 저장됨
*	\param UpperBound 허용 가능한 변위의 최대값
*	\param bCullBackFace 직선이 삼각형의 뒷면에서 만나는 경우 제외 여부
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