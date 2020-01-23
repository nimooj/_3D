// 필요한 헤더 파일을 포함한다.
#include <algorithm>
#include "..\usr\include\gmath.h"
#include "..\usr\include\gobject.h"
#include "..\usr\include\gscene.h"

// 링크할 외부 라이브러리를 지정한다.
#pragma comment(lib, "gmath.lib")
#pragma comment(lib, "gobject.lib")
#pragma comment(lib, "gscene.lib")	
#pragma comment(lib, "glut32.lib")

/*! \brief 객체의 표면적을 계산하는 함수 */
extern "C" __declspec(dllexport) double get_mesh_area2(GMesh *pMesh);
