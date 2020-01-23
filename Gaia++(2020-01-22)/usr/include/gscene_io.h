#ifndef _GSCENE_IO_H
#define _GSCENE_IO_H

#include <io.h>
#include <string>
#include <time.h>

class GScene;
class GMesh;

// ASF ���ϰ��� �Լ�
#define ASFTOKEN_EOF    0
#define ASFTOKEN_NULL   0
#define ASFTOKEN_TAG    1
#define ASFTOKEN_LPAREN 2
#define ASFTOKEN_RPAREN 3
#define ASFTOKEN_STRING 4
#define ASFTOKEN_RAW    5

/*!
*	\class ASF
*	\brief Ascii Scene File�� �ε��ϱ� ���� Ŭ����
*
*	\author ������ (shyun@dongguk.edu) 
*	\date 7 September 2012
*/
class ASF
{
public:
	// ������ ���                       
	char  *text;		// ���� ����Ÿ�� �����Ѵ�. 
	int    size;		// ASF File ũ��
	int    pos;			// ���� �а� �ִ� ��ġ
	int    prev_pos;	// ���� ��ġ
	char   lexm[1024];	// ���� ���ڿ�
};

// GSF(Gaia Scene File) ���Ϸ� ����� �����ϴ� �Լ�
bool save_scene_as_gsf_file(GScene *pScene, const char *fname);
//bool open_scene_from_gsf_file(GScene *pScene, const char *fname);

// ASE ���� �Լ�
bool load_scene_from_ase_file(GScene *pScene, const char *fname);
bool save_scene_as_ase_file(GScene *pScene, const char *fname);

// OBJ ���� �Լ�
bool load_mtl_file(GMesh *pMesh, const char *fname);
bool save_mtl_file(GMesh *pMesh, const char *fname);
bool load_mesh_from_obj_file(GScene *pScene, const char *fname);
bool save_mesh_as_obj_file(GMesh *pMesh, const char *fname);

// TRI ���� �Լ�
bool load_mesh_from_tri_file(GScene *pScene, const char *fname);
bool save_mesh_as_tri_file(GMesh *pMesh, const char *fname);
GMesh *load_mesh_from_tri_file(const char *fname);

// M ���� �Լ�
bool load_mesh_from_m_file(GScene *pScene, const char *fname);

// Path ���� �Լ�
bool save_geodesic_path(GGeoPath *pPath, const char *fname);
bool load_geodesic_path(GScene *pScene, char *fname);

#endif // _GSCENE_IO_H