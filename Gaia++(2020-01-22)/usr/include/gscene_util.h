#ifndef _GSCENE_UTIL_H
#define _GSCENE_UTIL_H

class GMesh;

/*!
*	\brief ��θ��� ������ ���ϸ�(Ȯ��� ����)�� ��ȯ�Ѵ�.
*/	
std::string get_file_name_ext(std::string full_path);

/*!
*	\brief ��θ��� ������ ���ϸ�(Ȯ��� ������)�� ��ȯ�Ѵ�.
*/
std::string get_file_name(std::string full_path);

/*!
*	\brief ������ Ȯ���ڸ� ��ȯ�Ѵ�.
*/
std::string get_file_ext(std::string full_path);

/*!
*	\brief ������ ���丮 ��θ��� ��ȯ�Ѵ�.
*/	
std::string get_file_path(std::string full_path);

/*!
*	\brief ������ ���ڿ��� ��ȯ�Ͽ� ��ȯ�Ѵ�.
*/
std::string cast_str(int i);

/*!
*	\brief �Ǽ��� ���ڿ��� ��ȯ�Ͽ� ��ȯ�Ѵ�.
*/
std::string cast_str(double v);

/*!
*	\brief �Ǽ��� ���ڿ��� ��ȯ�Ͽ� ��ȯ�Ѵ�.
*/
std::string cast_str(float v);

/*!
*	\brief Ray casting�� ���� Scalar displacement map�� ���Ѵ�.
*/
void get_scalar_displacement(GMesh *pBaseMesh, GMesh *pTarMesh, std::vector<double> &Distances, std::vector<int> &vIdxs, double UpperBound, bool bCullBackFace);

#endif // _GSCENE_UTIL_H
