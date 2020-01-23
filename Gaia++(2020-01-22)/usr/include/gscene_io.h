#ifndef _GSCENE_IO_H
#define _GSCENE_IO_H

#include <io.h>
#include <string>
#include <time.h>

class GScene;
class GMesh;

// ASF 파일관련 함수
#define ASFTOKEN_EOF    0
#define ASFTOKEN_NULL   0
#define ASFTOKEN_TAG    1
#define ASFTOKEN_LPAREN 2
#define ASFTOKEN_RPAREN 3
#define ASFTOKEN_STRING 4
#define ASFTOKEN_RAW    5

/*!
*	\class ASF
*	\brief Ascii Scene File을 로드하기 위한 클래스
*
*	\author 윤승현 (shyun@dongguk.edu) 
*	\date 7 September 2012
*/
class ASF
{
public:
	// 데이터 멤버                       
	char  *text;		// 읽은 데이타를 보관한다. 
	int    size;		// ASF File 크기
	int    pos;			// 현재 읽고 있는 위치
	int    prev_pos;	// 이전 위치
	char   lexm[1024];	// 읽은 문자열
};

// GSF(Gaia Scene File) 파일로 장면을 저장하는 함수
bool save_scene_as_gsf_file(GScene *pScene, const char *fname);
//bool open_scene_from_gsf_file(GScene *pScene, const char *fname);

// ASE 파일 함수
bool load_scene_from_ase_file(GScene *pScene, const char *fname);
bool save_scene_as_ase_file(GScene *pScene, const char *fname);

// OBJ 파일 함수
bool load_mtl_file(GMesh *pMesh, const char *fname);
bool save_mtl_file(GMesh *pMesh, const char *fname);
bool load_mesh_from_obj_file(GScene *pScene, const char *fname);
bool save_mesh_as_obj_file(GMesh *pMesh, const char *fname);

// TRI 파일 함수
bool load_mesh_from_tri_file(GScene *pScene, const char *fname);
bool save_mesh_as_tri_file(GMesh *pMesh, const char *fname);
GMesh *load_mesh_from_tri_file(const char *fname);

// M 파일 함수
bool load_mesh_from_m_file(GScene *pScene, const char *fname);

// Path 파일 함수
bool save_geodesic_path(GGeoPath *pPath, const char *fname);
bool load_geodesic_path(GScene *pScene, char *fname);

#endif // _GSCENE_IO_H