#ifndef _GSCENE_UTIL_H
#define _GSCENE_UTIL_H

class GMesh;

/*!
*	\brief 경로명을 제외한 파일명(확장명 포함)을 반환한다.
*/	
std::string get_file_name_ext(std::string full_path);

/*!
*	\brief 경로명을 제외한 파일명(확장명 불포함)을 반환한다.
*/
std::string get_file_name(std::string full_path);

/*!
*	\brief 파일의 확장자를 반환한다.
*/
std::string get_file_ext(std::string full_path);

/*!
*	\brief 파일의 디렉토리 경로명을 반환한다.
*/	
std::string get_file_path(std::string full_path);

/*!
*	\brief 정수를 문자열로 변환하여 반환한다.
*/
std::string cast_str(int i);

/*!
*	\brief 실수를 문자열로 변환하여 반환한다.
*/
std::string cast_str(double v);

/*!
*	\brief 실수를 문자열로 변환하여 반환한다.
*/
std::string cast_str(float v);

/*!
*	\brief Ray casting을 통해 Scalar displacement map을 구한다.
*/
void get_scalar_displacement(GMesh *pBaseMesh, GMesh *pTarMesh, std::vector<double> &Distances, std::vector<int> &vIdxs, double UpperBound, bool bCullBackFace);

#endif // _GSCENE_UTIL_H
