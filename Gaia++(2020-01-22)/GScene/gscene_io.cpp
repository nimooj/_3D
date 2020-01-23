#include "stdafx.h"

static ASF *asfOpen(const char *szFile);
static void asfClose(ASF *ase);
static int asfFindTag(ASF *ase,char *tag);
static int asfGetTag(ASF *ase);
static int asfCompare(ASF *ase,char *str);
static int asfNextTag(ASF *ase,char *str);
static int asfSwitch(ASF *ase,int num,char *str1,...);
static int asfCountTag(ASF *ase,char *tag);
static int asfDescend(ASF *ase);
static int asfAscend(ASF *ase);
static int asfParseInteger(ASF *ase);
static float asfParseFloat(ASF *ase);
static char *asfParseString(ASF *ase);
static char *asfParseRaw(ASF *ase);

/*!
*	\brief ASF 파일을 오픈한다.
*
*	\param szFile 파일명
*
*	\return ASF 파일구조체에 대한 포인터
*/
ASF *asfOpen(const char *szFile)
{
	ASF *asf;
	FILE *fp;

	fopen_s(&fp, szFile, "rb");
	if (!fp) 
		return NULL;

	asf = (ASF *)malloc(sizeof(ASF));
	memset(asf, 0, sizeof(asf));
	asf->size = _filelength(_fileno(fp)) + 1;
	asf->text = (char *)malloc(asf->size);
	fread(asf->text, asf->size, 1, fp);

	asf->text[asf->size - 1] = 0;
	asf->pos = 0;
	asf->prev_pos = 0;
	asf->lexm[0] = 0;

	fclose(fp);
	return asf;
}

/*!
*	\brief ASF 파일을 닫는다.
*
*	\param asf 오픈된 ASF 파일 구조체에 대한 포인터
*/
void asfClose(ASF *asf)
{
	free(asf->text);
	free(asf);
}

/*!
*	\brief 파일의 현재위치의 문자가 공백인지 조사한다
*
*	\param asf 오픈된 ASF 파일 구조체에 대한 포인터
*/
int aseIsWhiteSpace(ASF *asf)
{
	char c = asf->text[asf->pos];
	if (c == ' ' || c == '\r' || c == '\n' || c == '\t') 
		return 1;
	else 
		return 0;
}

/*!
*	\brief 파일에서 공백을 제거하고, 다음의 토큰을 읽는다.
*	\note 읽은 토큰이 '*' 인경우, 태그를 추가적으로 읽는다.
*
*	\param asf 오픈된 ASF 파일 구조체에 대한 포인터
*
*	\return 읽은 토큰의 결과를 반환한다. 
*	ASFTOKEN_TAG: *, 
*	ASFTOKEN_LPAREN: {, 
*	ASFTOKEN_RPAREN: }, 
*	ASFTOKEN_STRING: "***",
*	ASFTOKEN_RAW: 데이터
*/
int aseGetToken(ASF *asf)
{
	int i;
	asf->prev_pos = asf->pos;
	while (1)
	{
		while (aseIsWhiteSpace(asf)) 
			asf->pos++;

		switch (asf->text[asf->pos])
		{
		case 0: // EOF
			return ASFTOKEN_EOF;
		case '*': // tag
			asf->pos++;
			for (i = 0; !aseIsWhiteSpace(asf) && asf->text[asf->pos] != 0; asf->pos++)
			{
				asf->lexm[i++] = asf->text[asf->pos];
			}
			asf->lexm[i] = 0;
			return ASFTOKEN_TAG;
		case '{':
			asf->lexm[0] = asf->text[asf->pos++];
			asf->lexm[1] = 0;
			return ASFTOKEN_LPAREN;
		case '}':
			asf->lexm[0] = asf->text[asf->pos++];
			asf->lexm[1] = 0;
			return ASFTOKEN_RPAREN;
		case '"':
			asf->pos++;
			for (i = 0; asf->text[asf->pos] != '"'; asf->pos++)
			{
				asf->lexm[i++] = asf->text[asf->pos];
			}
			asf->lexm[i] = 0;
			asf->pos++;
			return ASFTOKEN_STRING;
		default:
			for (i = 0; !aseIsWhiteSpace(asf) && asf->text[asf->pos] != 0; asf->pos++)
			{
				asf->lexm[i++] = asf->text[asf->pos];
			}
			asf->lexm[i]=0;
			return ASFTOKEN_RAW;
		}
	}
}

/*!
*	\brief 파일에서 현재 블록({...})내에서 주어진 태크를 찾아 읽는다.
*
*	\param asf 오픈된 ASF 파일 구조체에 대한 포인터
*	\param tag 찾을 태그
*
*	\return 1: 찾은 경우, 0: 못 찾은 경우.
*/
int asfFindTag(ASF *asf, char *tag)
{
	while (1)
	{
		switch (aseGetToken(asf))
		{
		case ASFTOKEN_EOF:
			return 0;
		case ASFTOKEN_TAG:
			if (strcmp(tag, asf->lexm) == 0) 
				return 1;
			break;
		case ASFTOKEN_LPAREN:
			asfAscend(asf);		// 하위 블록을 건너뛴다.
			break;
		case ASFTOKEN_RPAREN:
			asf->pos--;			// 현재 블록내에 존재하지 않는다.
			return 0;
		}
	}
}

/*!
*	\brief 파일에서 현재 블록({...})내에서 다음 태크를 읽는다.
*
*	\param asf 오픈된 ASF 파일 구조체에 대한 포인터
*
*	\return 1: 다음 태그를 읽은 경우, 0: 블록내에 태그가 없는 경우.
*/
int asfGetTag(ASF *asf)
{
	while (1)
	{
		switch (aseGetToken(asf))
		{
		case ASFTOKEN_EOF:
			return 0;
		case ASFTOKEN_TAG:
			return 1;
		case ASFTOKEN_LPAREN:
			asfAscend(asf);
			break;
		case ASFTOKEN_RPAREN:
			asf->pos--;
			return 0;
		}
	}
}

/*!
*	\brief 파일에서 현재 위치에서 읽은 내용이 입력문자열과 일치하는지 조사한다.
*
*	\param asf 오픈된 ASF 파일 구조체에 대한 포인터
*	\param str 조사할 문자열에 대한 포인터
*
*	\return 1: 같은 경우, 0: 다른 경우.
*/
int asfCompare(ASF *asf, char *str)
{
	if (strcmp(asf->lexm, str) == 0) 
		return 1;
	return 0;
}

/*!
*	\brief 현재 블록내에서 다음태그을 읽어와 입력문자열과 일치하는지 조사한다.
*
*	\param asf 오픈된 ASF 파일 구조체에 대한 포인터
*	\param str 조사할 문자열에 대한 포인터
*
*	\return 1: 같은 경우, 0: 다른 경우 혹은 현재 블록내에 태그가 없는 경우.
*/
int asfNextTag(ASF *asf, char *str)
{
	if (asfGetTag(asf))
	{
		if (asfCompare(asf, str))
		{
			return 1;
		}
		else
		{
			asf->pos = asf->prev_pos;
			return 0;
		}
	}
	return 0;
}

/*!
*	\brief 현재 블록내에서 다음태그을 읽어와 입력문자열과 일치하는지 조사한다.
*
*	\param asf 오픈된 ASF 파일 구조체에 대한 포인터
*	\param num 입력 문자열의 개수
*	\param str1 조사할 첫번째 문자열에 대한 포인터
*
*	\return 일치하는 입력 문자열의 인덱스, -1: 태그가 입력문자열과 일치하지 않는경우
*/
int asfSwitch(ASF *asf, int num, char *str1, ...)
{
	va_list	args;
	int i;
	char *keys[100];

	va_start(args, str1);

	for (i = 0; i<num; i++) 
		keys[i] = va_arg(args, char *);

	while (asfGetTag(asf))
	{
		for (i = 0; i < num; i++)
			if (asfCompare(asf,keys[i])) 
				return i;
	}

	va_end(args);
	return -1;
}

/*!
*	\brief 현재 블록내에서 태그의 개수를 계산한다.
*
*	\param asf 오픈된 ASF 파일 구조체에 대한 포인터
*	\param tag 조사할 태그
*
*	\return 현재블록내의 태그의 개수를 반환한다 (하위블록은 건너뜀).
*/
int asfCountTag(ASF *asf, char *tag)
{
	int count = 0;

	int pos = asf->pos;
	int prev_pos = asf->prev_pos;

	while (asfFindTag(asf, tag))
	{
		count++;
	}

	asf->pos = pos;
	asf->prev_pos = prev_pos;

	return count;
}

/*!
*	\brief 현재위치에서 하위블록에 진입한다.
*
*	\param asf 오픈된 ASF 파일 구조체에 대한 포인터
*
*	\return 1: 성공한 경우, 0: 하위 블록이 없는 경우.
*/
int asfDescend(ASF *asf)
{
	while (1)
	{
		switch (aseGetToken(asf))
		{
		case ASFTOKEN_EOF:
			return 0;
		case ASFTOKEN_LPAREN:
			return 1;
		case ASFTOKEN_RPAREN:
			asf->pos--;
			return 0;
		}
	}	
}

/*!
*	\brief 현재 블록내에서 상위블록으로 올라간다.
*
*	\param asf 오픈된 ASF 파일 구조체에 대한 포인터
*
*	\return 1: 올라간 경우, 0: 파일의 끝
*/
int asfAscend(ASF *asf)
{
	while (1)
	{
		switch (aseGetToken(asf))
		{
		case ASFTOKEN_EOF:
			return 0;
		case ASFTOKEN_LPAREN:
			asfAscend(asf);
			break;
		case ASFTOKEN_RPAREN:
			return 1;
		}
	}
}

/*!
*	\brief 현재위치에서 정수형태로 데이터를 읽는다.
*
*	\param asf 오픈된 ASF 파일 구조체에 대한 포인터
*
*	\return 읽은 정수값을 반환한다.
*/
int asfParseInteger(ASF *asf)
{
	aseGetToken(asf);
	return atoi(asf->lexm);
}

/*!
*	\brief 현재위치에서 실수형태로 데이터를 읽는다.
*
*	\param asf 오픈된 ASF 파일 구조체에 대한 포인터
*
*	\return 읽은 실수값을 반환한다.
*/
float asfParseFloat(ASF *asf)
{
	aseGetToken(asf);
	return (float)atof(asf->lexm);
}

/*!
*	\brief 현재위치에서 문자열의 형태로 데이터를 읽는다.
*
*	\param asf 오픈된 ASF 파일 구조체에 대한 포인터
*
*	\return 읽은 문자열의 포인터.
*/
char *asfParseString(ASF *asf)
{
	aseGetToken(asf);
	return asf->lexm;
}

/*!
*	\brief 현재위치에서 문자열의 형태로 데이터를 읽는다.
*
*	\param asf 오픈된 ASF 파일 구조체에 대한 포인터
*
*	\return 읽은 문자열의 포인터.
*/
char *asfParseRaw(ASF *asf)
{
	aseGetToken(asf);
	return asf->lexm;
}

//////////////////////////
// gscene_io_ase.cpp	//
//////////////////////////
#define MAX_JOINT_NUM 100

static int AseStartFrame = 0;
static int AseLastFrame = 0;
static int AseNumFrames = 1;
static int AseTicksPerFrame = 0;
static clock_t AseFrameRate = 30L;
static GTransf MAX2GL(GVector3(0, 0, 0), GQuater(SQRT(2.0) / 2.0, -SQRT(2.0) / 2.0, 0.0, 0.0));

// 파일 로드에 필요한 함수들
static bool load_ase_scene(GScene *pScene, ASF *asf);
static void load_ase_scene_info(GScene *pScene, ASF *asf);
static void load_ase_scene_material_list(GScene *pScene, std::vector<GMaterial> &MtrlList, ASF *asf);
static void load_ase_scene_material(GScene *pScene, std::vector<GMaterial> &MtrlList, ASF *asf);
static void load_ase_scene_geom_object(GScene *pScene, ASF *asf);
static void load_ase_scene_helper_object(GScene *pScene, ASF *asf);

// 파일 저장에 필요한 함수들
static void save_ase_scene_material_list(FILE *fp, GScene *pScene);
static void save_ase_scene_geom_object_list(FILE *fp, GScene *pScene);

/*!
*	\brief ASE 형식의 장면 데이터 파일을 로드한다
*	
*	\param pScene 장면 객체의 포인터
*	\param fname 파일이름
*
*	\return true: 성공, false: 실패
*/
bool load_scene_from_ase_file(GScene *pScene, const char *fname)
{
	ASF *asf = asfOpen(fname);
	if (!asf) 
		return false;

	load_ase_scene(pScene, asf);
	asfClose(asf);

	int NumMesh = (int)pScene->MeshList.size();
	for (int i = 0; i < NumMesh; ++i)
	{
		pScene->MeshList[i]->UpdateNormal(true);
		pScene->MeshList[i]->UpdateBndBox(true);
		pScene->MeshList[i]->UpdateBufVertex();
		pScene->MeshList[i]->UpdateBufFace();
		pScene->MeshList[i]->InitRefBoneInfo();
	}

	return true;
}

/*!
*	\brief ASE 형식의 파일을 로드한다.
*	
*	\param pScene 데이터가 저장될 장면의 포인터
*	\param asf ASF 파일의 포인터
*
*	\return true: 성공, false: 실패
*/
bool load_ase_scene(GScene *pScene, ASF *asf)
{
	while (asfGetTag(asf))
	{
		if (asfCompare(asf, "SCENE"))
			load_ase_scene_info(pScene, asf);

// 		if (asfCompare(asf, "MATERIAL_LIST"))
// 			load_ase_scene_material_list(pScene, pScene->MtrlList, asf);

		if (asfCompare(asf, "GEOMOBJECT"))
			load_ase_scene_geom_object(pScene, asf);

		if (asfCompare(asf, "HELPEROBJECT"))
			load_ase_scene_helper_object(pScene, asf);
	}

	// 	pModel->FKSolver();
	// 	pModel->InitBasePose();
	// 	pModel->InitJointBox();
	// 	pModel->InitInfoBindJoint();
	// 	pModel->CalcBndBox();

	// 	if (AseNumFrames > 1)
	// 	{
	// 		int JointNum = (int)pModel->GetJointList().size();
	// 		std::string MotName = pModel->GetName();
	// 		GMotion *pMotion = new GMotion(MotName, AseNumFrames, JointNum);
	// 		pModel->GetMotionList().push_back(pMotion);
	// 
	// 		for (int i = 0; i < AseNumFrames; i++)
	// 			for (int j = 0; j < JointNum; j++)
	// 				(*pMotion)[i][j] = AseMotions[j][i];
	// 
	// 		pModel->SetCurMotion(MotName);
	// 
	// 		for (int i = 0; i < MAX_JOINT_NUM; i++)
	// 			delete [] AseMotions[i];
	// 	}
	return true;
}

void load_ase_scene_info(GScene *pScene, ASF *asf)
{
	asfDescend(asf);	// SCENE { 다음으로 이동.
	while (asfGetTag(asf))
	{
		if (asfCompare(asf, "SCENE_FIRSTFRAME"))
			AseStartFrame = asfParseInteger(asf);

		if (asfCompare(asf, "SCENE_LASTFRAME"))
			AseLastFrame = asfParseInteger(asf);

		if (asfCompare(asf, "SCENE_FRAMESPEED"))
			AseFrameRate = asfParseInteger(asf);

		if (asfCompare(asf, "SCENE_TICKSPERFRAME"))
			AseTicksPerFrame = asfParseInteger(asf);

		AseNumFrames = AseLastFrame - AseStartFrame + 1;

		clock_t ClockPerFrm = CLOCKS_PER_SEC / AseFrameRate;
	}
	asfAscend(asf);		// SCENE {...} 다음으로 이동.
}

/*!
*	\brief ASF 파일에서 MATERIAL_LIST 노드를 읽는다.
*	
*	\param MtrlList 재질정보가 저장될 재질의 리스트
*	\param asf ASF 파일에 대한 구조체 포인터
*/
void load_ase_scene_material_list(GScene *pScene, std::vector<GMaterial> &MtrlList, ASF *asf)
{
	asfDescend(asf);	// MATERIAL_LIST { 다음으로 이동
	asfFindTag(asf, "MATERIAL_COUNT");
	int NumMtrl = asfParseInteger(asf);

	if (NumMtrl > 0)
	{
		for (int i = 0; i < NumMtrl; i++)
		{
			asfFindTag(asf, "MATERIAL");
			asfParseInteger(asf);	// Material 아이디

			asfDescend(asf);		// MATERIAL id { 다음으로 이동.
			load_ase_scene_material(pScene, MtrlList, asf);
			asfAscend(asf);			// MATERIAL {} 다음으로 이동.
		}
	}
	asfAscend(asf);		// MATERIAL_LIST {} 다음으로 이동.
}

/*!
*	\brief 현재 material 블록내에서 정보를 읽어들인다.
*
*	\param Mat 읽은 정보를 저장할 재질의 리스트
*	\param asf 오픈된 ASF 파일 구조체에 대한 포인터
*/
void load_ase_scene_material(GScene *pScene, std::vector<GMaterial> &MtrlList, ASF *asf)
{
	GMaterial Mtrl;
	int idx = (int)MtrlList.size();
	MtrlList.push_back(Mtrl);

	float r, g, b;
	while (asfGetTag(asf))
	{
		if (asfCompare(asf, "MATERIAL_NAME"))
		{
			char *name = asfParseString(asf);
			MtrlList[idx].Name = std::string(name);
		}

		if (asfCompare(asf, "MATERIAL_AMBIENT"))
		{
			r = asfParseFloat(asf);
			g = asfParseFloat(asf);
			b = asfParseFloat(asf);
			MtrlList[idx].SetAmbient(r, g, b);
		}

		if (asfCompare(asf, "MATERIAL_DIFFUSE"))
		{
			r = asfParseFloat(asf);
			g = asfParseFloat(asf);
			b = asfParseFloat(asf);
			MtrlList[idx].SetDiffuse(r, g, b);
		}

		if (asfCompare(asf, "MATERIAL_SPECULAR"))
		{
			r = asfParseFloat(asf);
			g = asfParseFloat(asf);
			b = asfParseFloat(asf);
			MtrlList[idx].SetSpecular(r, g, b);
		}

		if (asfCompare(asf, "MAP_DIFFUSE"))
		{
			asfDescend(asf);	// MAP_DIFFUSE { 다음으로 이동한다.
			asfFindTag(asf, "BITMAP");

			std::string file_name = asfParseString(asf);
			std::string file_path = pScene->FileName;

			file_name = file_name.substr(file_name.rfind("\\") + 1, file_name.length() - file_name.rfind("\\"));
			file_path = file_path.substr(0, file_path.rfind("\\")) + "\\" + file_name;

			MtrlList[idx].LoadTexture(file_path.c_str());
			asfAscend(asf);		// MAP_DIFFUSE {} 다음으로 이동한다.
		}

		if (asfCompare(asf, "NUMSUBMTLS"))
		{
// 			int NumSubMat = asfParseInteger(asf);
// 			for(int j = 0; j < NumSubMat; j++)
// 			{
// 				asfFindTag(asf, "SUBMATERIAL");
// 				asfParseInteger(asf);
// 				asfDescend(asf);	// SUBMATERIAL { 다음으로 이동.
// 				load_ase_scene_material(pScene, MtrlList[idx].SubMtrlList, asf);
// 				asfAscend(asf);		// SUBMATERIAL {} 다음으로 이동.
// 			}
		}
	}
}

/*!
*	\brief ASF 파일에서 GEOMOBJECT 노드를 읽는다.
*	
*	\param pScene 읽는 정보가 저장될 장면에 대한 포이터.
*	\param asf ASF 파일에 대한 구조체 포인터.
*/
void load_ase_scene_geom_object(GScene *pScene, ASF *asf)
{
	asfDescend(asf);	// GEOMOBJECT { 다음으로 이동.

	std::string NodeName, ParentName;
	asfFindTag(asf, "NODE_NAME");
	NodeName = asfParseString(asf);

	if (asfNextTag(asf, "NODE_PARENT"))
		ParentName = asfParseString(asf);

	std::vector<GMesh *> &MeshList = pScene->MeshList;
	//std::vector<GMaterial> &MtrlList = pScene->MtrlList;

	GMesh *pMesh = new GMesh(NodeName, GTransf(), FACE_TRI, ParentName);
	
	while (asfGetTag(asf))
	{
		// *NODE_TM
		if (asfCompare(asf, "NODE_TM"))
		{
			asfDescend(asf);	// NODE_TM { 다음으로 이동
			GVector3 x_axis, y_axis;

			asfFindTag(asf, "TM_ROW0");
			x_axis[0] = asfParseFloat(asf);
			x_axis[1] = asfParseFloat(asf);
			x_axis[2] = asfParseFloat(asf);

			asfFindTag(asf, "TM_ROW1");
			y_axis[0] = asfParseFloat(asf);
			y_axis[1] = asfParseFloat(asf);
			y_axis[2] = asfParseFloat(asf);

			GVector3 pos;
			asfFindTag(asf, "TM_ROW3");
			pos[0] = asfParseFloat(asf);
			pos[1] = asfParseFloat(asf);
			pos[2] = asfParseFloat(asf);

			GQuater rot;
			rot.SetFromFrameXY(x_axis, y_axis);
			pMesh->MC = GTransf(pos, rot);

			if (ParentName == "")	// 부모조인트가 없는 경우.
			{
				pMesh->DelX = GTransf(pos, rot);
			}
			else				// 부모조인트가 있는 경우.
			{
				GMesh *pParent = FindMesh(pScene->MeshList, ParentName);
				pMesh->DelX = inv(pParent->MC) * pMesh->MC; // 부모 좌표계에 맞게 변환				
			}
			asfAscend(asf);		// NOTE_TM {} 다음으로 이동.
		}

		// *NODE_MESH
		if (asfCompare(asf, "MESH"))
		{
			asfDescend(asf);	// MESH { 다음으로 이동

			asfFindTag(asf, "MESH_NUMVERTEX");
			int vNum = asfParseInteger(asf);

			asfFindTag(asf, "MESH_NUMFACES");
			int fNum = asfParseInteger(asf);

			asfFindTag(asf,"MESH_VERTEX_LIST");
			asfDescend(asf);		// MESH_VERTEX_LIST { 다음으로 이동
			for (int i = 0; i < vNum; i++)
			{
				asfFindTag(asf, "MESH_VERTEX");
				asfParseInteger(asf); // 버텍스 번호

				double x, y, z;
				x = asfParseFloat(asf);
				y = asfParseFloat(asf);
				z = asfParseFloat(asf);

				GPoint3 pt(x, y, z);
				pt = inv(pMesh->MC) * pt;

				pMesh->AddVertex(GVertex(pt, GVector3(0.0, 0.0, 0.0)));
			}
			asfAscend(asf);			// MESH_VERTEX_LIST {} 다음으로 이동

			asfFindTag(asf,"MESH_FACE_LIST");
			asfDescend(asf);		// MESH_FACE_LIST { 다음으로 이동
			for (int i = 0; i < fNum; i++)
			{
				asfFindTag(asf, "MESH_FACE");
				asfParseRaw(asf);	// 페이스 번호

				int idx0, idx1, idx2, mtrl_id;
				asfParseRaw(asf);	// A:
				idx0 = asfParseInteger(asf);

				asfParseRaw(asf);	// B:
				idx1 = asfParseInteger(asf);

				asfParseRaw(asf);	// C:
				idx2 = asfParseInteger(asf);

				asfFindTag(asf, "MESH_MTLID");
				mtrl_id = asfParseInteger(asf);

				pMesh->AddFace(GFace(idx0, idx1, idx2, -1, -1, -1, mtrl_id));
			}
			asfAscend(asf);			// MESH_FACE_LIST {} 다음으로 이동

			if (asfNextTag(asf, "MESH_NUMTVERTEX"))
			{
				int tNum = asfParseInteger(asf);
				if (tNum)
				{
					asfFindTag(asf, "MESH_TVERTLIST");
					asfDescend(asf);	// MESH_TVERTLIST { 다음으로 이동.
					for (int i = 0; i < tNum; i++)
					{
						asfFindTag(asf, "MESH_TVERT");
						asfParseInteger(asf);
						double u, v;
						u = asfParseFloat(asf);
						v = asfParseFloat(asf);
						pMesh->AddTexel(GVector2(u, v));
					}
					asfAscend(asf);		// MESH_TVERTLIST {} 다음으로 이동.

					asfFindTag(asf, "MESH_NUMTVFACES");
					int fNum = asfParseInteger(asf);

					asfFindTag(asf, "MESH_TFACELIST");
					asfDescend(asf);	// MESH_TFACELIST { 다음으로 이동.
					for (int i = 0; i < fNum; i++)
					{
						asfFindTag(asf, "MESH_TFACE");
						asfParseInteger(asf);
						pMesh->Faces[i].tIdx[0] = asfParseInteger(asf);
						pMesh->Faces[i].tIdx[1] = asfParseInteger(asf);
						pMesh->Faces[i].tIdx[2] = asfParseInteger(asf);
					}
					asfAscend(asf);		// MESH_TFACELIST {} 다음으로 이동.
				}
			}
			asfAscend(asf);		// MESH {} 다음으로 이동			
		}

		// *MATERIAL_REF
		if (asfCompare(asf, "MATERIAL_REF"))
		{
// 			int mtrl_id = asfParseInteger(asf);
// 			pMesh->Renderer.MtlName = MtrlList[mtrl_id].Name;
		}			

		// *TM_ANIMATION
		if (asfCompare(asf, "TM_ANIMATION"))
		{
			asfDescend(asf);	// TM_ANIMATION { 다음으로 이동

			int NumKeyPos = 0, NumKeyRot = 0;
			GVector3 *KeyPos = new GVector3 [AseNumFrames];
			GQuater *KeyRot = new GQuater [AseNumFrames];

			while (asfGetTag(asf))
			{
				if (asfCompare(asf, "CONTROL_POS_TRACK"))
				{
					asfDescend(asf);	// CONTROL_POS_TRACK { 다음으로 이동
					NumKeyPos = asfCountTag(asf, "CONTROL_POS_SAMPLE");
					if (NumKeyPos != 0)
					{
						int *KeyIdx = new int [NumKeyPos];

						for (int i = 0; i < NumKeyPos; i++)
						{
							asfFindTag(asf, "CONTROL_POS_SAMPLE");
							KeyIdx[i] = asfParseInteger(asf) / AseTicksPerFrame - AseStartFrame;
							KeyPos[KeyIdx[i]][0] = asfParseFloat(asf);
							KeyPos[KeyIdx[i]][1] = asfParseFloat(asf);
							KeyPos[KeyIdx[i]][2] = asfParseFloat(asf);
						}

						for (int i = 0; i < KeyIdx[0]; i++)
							KeyPos[i] = KeyPos[KeyIdx[0]];

						int from = KeyIdx[0], to;
						for (int i = 1; i < NumKeyPos; i++)
						{
							to = KeyIdx[i];
							for (int j = from; j < to; j++)
							{
								double t = (double)(j - from) / (double)(to - from);
								KeyPos[j] = t * KeyPos[to] + (1.0 - t) * KeyPos[from];
							}
							from = to;
						}

						for (int i = KeyIdx[NumKeyPos - 1]; i < AseNumFrames; i++)
							KeyPos[i] = KeyPos[KeyIdx[NumKeyPos - 1]];

						delete [] KeyIdx;
					}
					asfAscend(asf);		// CONTROL_POS_TRACK {} 다음으로 이동
				}
				else if (asfCompare(asf, "CONTROL_ROT_TRACK"))
				{
					asfDescend(asf);	// CONTROL_ROT_TRACK { 다음으로 이동
					NumKeyRot = asfCountTag(asf, "CONTROL_ROT_SAMPLE");
					if (NumKeyRot != 0)
					{
						int *KeyIdx = new int [NumKeyRot];

						for (int i = 0; i < NumKeyRot; i++)
						{
							asfFindTag(asf, "CONTROL_ROT_SAMPLE");
							KeyIdx[i] = asfParseInteger(asf) / AseTicksPerFrame - AseStartFrame;

							GVector3 axis;
							axis[0] = asfParseFloat(asf);
							axis[1] = asfParseFloat(asf);
							axis[2] = asfParseFloat(asf);
							axis.Normalize();

							double angle;
							angle = asfParseFloat(asf);

							GQuater q;
							q.SetFromAngleAxis(angle, axis, true);
							q = inv(q);

							if (i == 0)
								KeyRot[KeyIdx[i]] = q;
							else
							{
								KeyRot[KeyIdx[i]] = q * KeyRot[KeyIdx[i - 1]];
								if (quat_dot(KeyRot[KeyIdx[i]], KeyRot[KeyIdx[i - 1]]) < 0.0)
									KeyRot[KeyIdx[i]].SetNegate();
							}
							KeyRot[KeyIdx[i]].Normalize();
						}

						for (int i = 0; i < KeyIdx[0]; i++)
							KeyRot[i] = KeyRot[KeyIdx[0]];

						int from = KeyIdx[0], to;
						for (int i = 1; i < NumKeyRot; i++)
						{
							to = KeyIdx[i];
							for (int j = from; j < to; j++)
							{
								double t = (double)(j - from) / (double)(to - from);
								KeyRot[j] = slerp(KeyRot[from], KeyRot[to], t);							
							}
							from = to;
						}

						for (int i = KeyIdx[NumKeyRot - 1]; i < AseNumFrames; i++)
							KeyRot[i] = KeyRot[KeyIdx[NumKeyRot - 1]];

						delete [] KeyIdx;
					}
					asfAscend(asf);		// CONTROL_ROT_TRACK {} 다음으로 이동
				}
			}

			for (int i = 0; i < AseNumFrames; i++)
			{
				GVector3 t;
				if (NumKeyPos == 0)	// CONTROL_POS_TRACK이 없는 경우,
					t = part_trans(pMesh->DelX);
				else
					t = KeyPos[i];

				GQuater q;
				if (NumKeyRot == 0)		// CONTROL_ROT_TRACK이 없는 경우,
					q = part_rot(pMesh->DelX);
				else
					q = KeyRot[i];

				GTransf InX(t, q);
				InX = inv(pMesh->DelX) * InX;

				pMesh->Animation.push_back(InX);
			}

			delete [] KeyPos;
			delete [] KeyRot;

			asfAscend(asf);		// ANIMATION_TM {} 다음으로 이동.
		}

		// *PHYSIQUE
		if (asfCompare(asf, "PHYSIQUE"))
		{
			pMesh->SemanticType = MESH_SKIN;
			asfDescend(asf);	// PHYSIQUE { 다음으로 이동

			asfFindTag(asf, "PHYSIQUE_LIST");
			asfDescend(asf);	// PHYSIQUE_LIST { 다음으로 이동

			std::vector<GVertex> &VertList = pMesh->Verts;
			while (asfGetTag(asf))
			{
				if (asfCompare(asf, "PHYSIQUE_NONBLENDED_RIGIDTYPE"))
				{
					int vidx = asfParseInteger(asf);			// vertex 인덱스
					std::string MeshName(asfParseString(asf));		// 바인딩 조인트의 이름
					VertList[vidx].AddRefBoneInfo(MeshName, 1.0);
				}

				if (asfCompare(asf, "PHYSIQUE_BLENDED_RIGIDTYPE"))
				{
					int vidx = asfParseInteger(asf);
					asfDescend(asf);		// PHYSIQUE_BLENDED_RIGIDTYPE vidx { 다음으로 이동
					asfFindTag(asf, "PHYSIQUE_NUMNODE");
					int NumMesh = asfParseInteger(asf);

					asfFindTag(asf, "PHYSIQUE_NODELIST");
					asfDescend(asf);		// PHYSIQUE_NODELIST { 다음으로 이동

					for (int i = 0; i < NumMesh; i++)
					{
						asfFindTag(asf, "PHYSIQUE_NODE");
						asfParseInteger(asf);
						double weight = asfParseFloat(asf);
						std::string MeshName(asfParseString(asf));
						VertList[vidx].AddRefBoneInfo(MeshName, weight);
					}

					asfAscend(asf);	// PHYSIQUE_NODELIST {} 다음으로 이동
					asfAscend(asf);	// PHYSIQUE_BLENDED_RIGIDTYPE vidx {} 다음으로 이동
				}
			}
			asfAscend(asf);		// PHYSIQUE_LIST {} 다음으로 이동
			asfAscend(asf);		// PHYSIQUE {} 다음으로 이동
		}

		// *SKIN
		if (asfCompare(asf, "SKIN"))
		{
			asfDescend(asf);	// SKIN { 다음으로 이동

			asfAscend(asf);		// SKIN {} 다음으로 이동.
		}
	}
	asfAscend(asf); // GEOMOBJECT {...} 다음으로 이동.

	pScene->AddMesh(pMesh);
}

/*!
*	\brief ASF 파일에서 HELPEROBJECT 노드를 읽는다.
*	
*	\param pScene 읽는 정보가 저장될 장면에 대한 포이터.
*	\param asf ASF 파일에 대한 구조체 포인터.
*/
void load_ase_scene_helper_object(GScene *pScene, ASF *asf)
{
// 	std::vector<GJoint *> &JointList = pModel->GetJointList();
// 
// 	asfDescend(asf);	// HELPEROBJECT { 다음으로...
// 
// 	GJoint *pJoint = NULL;
// 
// 	while (asfGetTag(asf))
// 	{
// 		if (asfCompare(asf, "NODE_PARENT"))
// 		{
// 			std::string Parent(asfParseString(asf));
// 
// 			pJoint = pModel->GetJoint(Parent);
// 			if (pJoint == NULL)
// 			{
// 				asfAscend(asf);		// HELPEROBJECT {} 다음으로...
// 				return;
// 			}
// 		}
// 
// 		if (pJoint != NULL && asfCompare(asf, "NODE_TM"))
// 		{
// 			asfDescend(asf);	// NODE_TM { 다음으로...
// 
// 			GVector3 x_axis, y_axis;
// 			asfFindTag(asf, "TM_ROW0");
// 			x_axis[0] = asfParseFloat(asf);
// 			x_axis[1] = asfParseFloat(asf);
// 			x_axis[2] = asfParseFloat(asf);
// 
// 			asfFindTag(asf, "TM_ROW1");
// 			y_axis[0] = asfParseFloat(asf);
// 			y_axis[1] = asfParseFloat(asf);
// 			y_axis[2] = asfParseFloat(asf);
// 
// 			GVector3 pos;
// 			asfFindTag(asf, "TM_ROW3");
// 			pos[0] = asfParseFloat(asf);
// 			pos[1] = asfParseFloat(asf);
// 			pos[2] = asfParseFloat(asf);
// 
// 			GQuater rot;
// 			rot.SetFromFrameXY(x_axis, y_axis);
// 
// 			GTransf x1, x2(pos, rot);
// 			x1 = pJoint->GetTransf();
// 
// 			GVector3 dir = part_trans(x2) - part_trans(x1);
// 			pJoint->SetLink(inv(x1) * dir);
// 
// 			asfAscend(asf);	// NOTD_TM {} 다음으로...
// 		}
// 	}
// 	asfAscend(asf);	// HELPEROBJECT {} 다음으로...
}

/*!
*	\brief 장면을 ASF 파일로 저장한다
*	
*	\param pScene 장면 객체의 포인터
*	\param fname 파일이름
*
*	\return true: 성공, false: 실패
*/
bool save_scene_as_ase_file(GScene *pScene, const char *fname)
{
	std::string file_name(fname);
	FILE *fp = fopen(file_name.c_str(), "w");

	// 재질 리스트 저장
	//save_ase_scene_material_list(fp, pScene);

	// 메쉬 리스트 저장
	save_ase_scene_geom_object_list(fp, pScene);

	// 넙스 객체 리스트 저장
	//save_ase_scene_gnurbs_object_lise(fp, pScene);

	fclose(fp);
	return true;
}

/*!
*	\brief 장면에 포함된 재질을 파일로 저장한다.
*	
*	\param fp 재질 정보가 저장될 파일의 포인터
*	\param pScene 재질 정보를 포함한 장면의 포인터
*/
void save_ase_scene_material_list(FILE *fp, GScene *pScene)
{
// 	int NumMtrl = (int)pScene->MtrlList.size();
// 
// 	fprintf(fp, "*MATERIAL_LIST {\n");
// 	fprintf(fp, "\t*MATERIAL_COUNT %d\n", NumMtrl);
// 
// 	for (int i = 0; i < NumMtrl; ++i)
// 	{
// 		GMaterial *pMtrl = &(pScene->MtrlList[i]);
// 		fprintf(fp, "\t*MATERIAL %d {\n", i);
// 		fprintf(fp, "\t\t*MATERIAL_NAME \"%s\"\n", pMtrl->Name.c_str());
// 		fprintf(fp, "\t\t*MATERIAL_AMBIENT %1.4f %1.4f %1.4f\n", pMtrl->Ambient[0], pMtrl->Ambient[1], pMtrl->Ambient[2]);
// 		fprintf(fp, "\t\t*MATERIAL_DIFFUSE %1.4f %1.4f %1.4f\n", pMtrl->Diffuse[0], pMtrl->Diffuse[1], pMtrl->Diffuse[2]);
// 		fprintf(fp, "\t\t*MATERIAL_SPECULAR %1.4f %1.4f %1.4f\n", pMtrl->Specular[0], pMtrl->Specular[1], pMtrl->Specular[2]);
// 		fprintf(fp, "\t\t*MATERIAL_SHINE %1.4f\n", pMtrl->Shiness);
// 
// 		int NumSubMtrl = (int)pMtrl->SubMtrlList.size();
// 		fprintf(fp, "\t\t*NUMSUBMTRLS %d\n", NumSubMtrl);
// 		if (NumSubMtrl != 0)
// 		{
// 			for (int j = 0; j < NumSubMtrl; ++j)
// 			{
// 				GMaterial *pSubMtrl = &(pMtrl->SubMtrlList[j]);
// 				fprintf(fp, "\t\t*SUBMATERIAL %d {\n", j);
// 				fprintf(fp, "\t\t\t*MATERIAL_NAME \"%s\"\n", pSubMtrl->Name.c_str());
// 				fprintf(fp, "\t\t\t*MATERIAL_AMBIENT %1.4f %1.4f %1.4f\n", pSubMtrl->Ambient[0], pSubMtrl->Ambient[1], pSubMtrl->Ambient[2]);
// 				fprintf(fp, "\t\t\t*MATERIAL_DIFFUSE %1.4f %1.4f %1.4f\n", pSubMtrl->Diffuse[0], pSubMtrl->Diffuse[1], pSubMtrl->Diffuse[2]);
// 				fprintf(fp, "\t\t\t*MATERIAL_SPECULAR %1.4f %1.4f %1.4f\n", pSubMtrl->Specular[0], pSubMtrl->Specular[1], pSubMtrl->Specular[2]);
// 				fprintf(fp, "\t\t\t*MATERIAL_SHINE %1.4f\n", pSubMtrl->Shiness);
// 
// 				if (pSubMtrl->BindTexId != 0)
// 				{
// 					fprintf(fp, "\t\t\t*MAP_DIFFUSE {\n");
// 					fprintf(fp, "\t\t\t\t*BITMAP \"%s\"\n", pSubMtrl->MapTexName.c_str());
// 					fprintf(fp, "\t\t\t}\n");
// 				}
// 
// 				// MAP_SPECULAR
// 				// MAP_SHINE
// 				// MAP_BUMP
// 
// 				fprintf(fp, "\t\t}\n");
// 			}
// 		}
// 		fprintf(fp, "\t}\n");
// 	}
// 	fprintf(fp, "}\n");
}

/*!
*	\brief 장면에 포함된 메쉬를 파일로 저장한다.
*	
*	\param fp 메쉬 정보가 저장될 파일의 포인터
*	\param pScene 메쉬 정보를 포함한 장면의 포인터
*/
void save_ase_scene_geom_object_list(FILE *fp, GScene *pScene)
{
	// 장면에 포함된 모든 메쉬의 선형 리스트를 생성
	std::vector<GMesh *> pMeshList;
	GetPreOrderMeshList(pScene, pMeshList, NULL);

	int NumMesh = (int)pMeshList.size();
	for (int i = 0; i < NumMesh; ++i)
	{
		GMesh *pMesh = pMeshList[i];

		// *GEOMOBJECT { ...
		fprintf(fp, "*GEOMOBJECT {\n");
		fprintf(fp, "\t*NODE_NAME \"%s\"\n", pMesh->Name.c_str());
		if (pMesh->ParentMeshName != "")
			fprintf(fp, "\t*NODE_PARENT \"%s\"\n", pMesh->ParentMeshName.c_str());

		// *NOTE_TM {...}
		GMatrix Mat = cast_mat(pMesh->MC, false);
		fprintf(fp, "\t*NODE_TM {\n");
		fprintf(fp, "\t\t*NODE_NAME \"%s\"\n", pMesh->Name.c_str());
		fprintf(fp, "\t\t*INHERIT_POS %f %f %f\n", 0, 0, 0);
		fprintf(fp, "\t\t*INHERIT_ROT %f %f %f\n", 0, 0, 0);
		fprintf(fp, "\t\t*INHERIT_SCL %f %f %f\n", 0, 0, 0);
		fprintf(fp, "\t\t*TM_ROW0 %f %f %f\n", Mat[0][0], Mat[1][0], Mat[2][0]);
		fprintf(fp, "\t\t*TM_ROW1 %f %f %f\n", Mat[0][1], Mat[1][1], Mat[2][1]);
		fprintf(fp, "\t\t*TM_ROW2 %f %f %f\n", Mat[0][2], Mat[1][2], Mat[2][2]);
		fprintf(fp, "\t\t*TM_ROW3 %f %f %f\n", Mat[0][3], Mat[1][3], Mat[2][3]);
		fprintf(fp, "\t\t*TM_POS %f %f %f\n", Mat[0][3], Mat[1][3], Mat[2][3]);
		fprintf(fp, "\t}\n");

		// *MESH {
		int NumVert = (int)pMesh->Verts.size();
		int NumFace = (int)pMesh->Faces.size();

		fprintf(fp, "\t*MESH {\n");
		fprintf(fp, "\t\t*MESH_NUMVERTEX %d\n", NumVert);
		fprintf(fp, "\t\t*MESH_NUMFACES %d\n", NumFace);

		// *MESH_VERTEX_LIST {...}
		fprintf(fp, "\t\t*MESH_VERTEX_LIST {\n");
		for (int j = 0; j < NumVert; ++j)
		{
			GPoint3 p = pMesh->Verts[j].P;
			p = pMesh->MC * p;
			fprintf(fp, "\t\t\t*MESH_VERTEX\t%d\t%lf\t%lf\t%lf\n", j, p[0], p[1], p[2]);
		}
		fprintf(fp, "\t\t}\n");

		// *MESH_FACE_LIST {...}
		fprintf(fp, "\t\t*MESH_FACE_LIST {\n");
		for (int j = 0; j < NumFace; ++j)
		{
			GFace *f = &(pMesh->Faces[j]);
			fprintf(fp, "\t\t\t*MESH_FACE\t%d:\tA:\t%d\tB:\t%d\tC:\t%d\t*MESH_MTLID %d\n", j, f->vIdx[0], f->vIdx[1], f->vIdx[2], f->MtlIdx); 
		}
		fprintf(fp, "\t\t}\n");

		if (!pMesh->Texs.empty())
		{
			int NumTex = (int)pMesh->Texs.size();
			// *MESH_NUMTVERTEX
			fprintf(fp, "\t\t*MESH_NUMTVERTEX %d\n", NumTex);

			// *MESH_TVERTLIST {...}
			fprintf(fp, "\t\t*MESH_TVERTLIST {\n");
			for (int j = 0; j < NumTex; ++j)
			{
				GVector2 t = pMesh->Texs[j];
				fprintf(fp, "\t\t\t*MESH_TVERT %d %f %f %f\n", j, t[0], t[1], 0.0);
			}
			fprintf(fp, "\t\t}\n");

			// *MESH_NUMTVFACES
			fprintf(fp, "\t\t*MESH_NUMTVFACES %d\n", NumFace);

			// *MESH_TFACELIST {...}
			fprintf(fp, "\t\t*MESH_TFACELIST {\n");
			for (int j = 0; j < NumFace; ++j)
			{
				GFace *f = &(pMesh->Faces[j]);
				fprintf(fp, "\t\t\t*MESH_TFACE %d %d %d %d\n", j, f->tIdx[0], f->tIdx[1], f->tIdx[2]);
			}
			fprintf(fp, "\t\t}\n");
		}

		// } of *MESH {}
		fprintf(fp, "\t}\n");

		// *MATERIAL_REF
// 		if (!pMesh->Renderer.MtlName.empty())
// 		{
// 			int MtlIdx;
// 			FindMtrl(pScene->MtrlList, pMesh->Renderer.MtlName, &MtlIdx);
// 			fprintf(fp, "\t*MATERIAL_REF %d\n", MtlIdx);
// 		}

		// } of *GEOMOBJECT {}
		fprintf(fp, "}\n");
	}
}

/*!
*	\brief 파일(*.mtl)로부터 재질 정보를 읽어 메쉬에 추가한다.
*
*	\param pMesh 재질을 추가할 메쉬의 포인터
*	\param fname 재질 파일의 이름
*
*	\return 성공: true, 실패: flase
*/
bool load_mtl_file(GMesh *pMesh, const char *fname)
{
	// 파일을 열고, 실패하면 false를 반환한다.
	FILE *fp = fopen(fname, "r");
	if (!fp)
		return false;

	// 로딩에 필요한 임시 변수들을 선언한다.
	char tag[256];
	GMaterial *pMat = NULL;

	// 파일의 끝까지 한 단어씩 읽어, tag 배열에 저장한다.
	while (fscanf(fp, "%s", tag) != EOF)
	{
		// newmtl (new material) 태그라면,
		if (!strcmp(tag, "newmtl"))
		{
			// 재질의 이름을 읽고,
			char tmp[64];
			fscanf(fp, "%s", tmp);

			// 재질을 생성하여, 메쉬의 재질 리스트에 추가한다.
			GMaterial mtl(tmp);
			pMesh->AddMtl(mtl);

			// 방금 추가된 재질의 포인터를 얻는다.
			pMat = pMesh->FindMtl(mtl.Name);
		}

		// Ka (ambient coefficients) 태그라면,
		if (!strcmp(tag, "Ka"))
		{
			// diffuse 성분을 읽어서 재질을 설정한다.
			double r, g, b;
			fscanf(fp, "%lf%lf%lf", &r, &g, &b);
			pMat->SetAmbient(r, g, b);
		}

		// Kd (diffuse coefficients) 태그라면,
		if (!strcmp(tag, "Kd"))
		{
			// diffuse 성분을 읽어서 재질을 설정한다.
			double r, g, b;
			fscanf(fp, "%lf%lf%lf", &r, &g, &b);
			pMat->SetDiffuse(r, g, b);
		}

		// Ks (specular coefficients) 태그라면,
		if (!strcmp(tag, "Ks"))
		{
			// diffuse 성분을 읽어서 재질을 설정한다.
			double r, g, b;
			fscanf(fp, "%lf%lf%lf", &r, &g, &b);
			pMat->SetSpecular(r, g, b);
		}

		// map_Kd (diffuse texture file) 태그라면,
		if (!strcmp(tag, "map_Kd"))
		{
			// 택스처 파일을 읽는다.
			char tex_name[64];
			fscanf(fp, "%s", tex_name);
			pMat->LoadTexture(tex_name);
		}
	}

	// 파일을 닫는다.
	fclose(fp);

	return true;
}

/*!
*	\brief 파일(*.obj)로부터 메쉬를 생성하고 장면에 추가한다.
*	\note 파일의 노말 정보는 무시함
*
*	\param pScene 로딩할 객체가 저장될 장면의 포인터
*	\param fname 절대경로가 포함된 파일의 이름
*
*	\return 성공: true, 실패: flase
*/
bool load_mesh_from_obj_file(GScene *pScene, const char *fname)
{
	// 파일을 열고 실패하면 false를 반환한다.
	FILE *fp = fopen(fname, "r");
	if (!fp)
		return false;

	// 파일의 이름을 얻어온다.
	std::string file_name = get_file_name(fname);
	
	// 쿼드타입 메쉬를 생성한다.
	GMesh *pMesh = new GMesh(file_name, GTransf(), FACE_QUAD);
	
	// 로딩에 필요한 임시 변수들을 선언한다.
	char tag[256];
	bool bTexture = false;
	bool bNormal = false;
	std::string GroupName;
	int MtlIdx = -1;

	// 파일의 끝까지 한 단어씩 읽으면서 tag 배열에 저장한다.
	while (fscanf(fp, "%s", tag) != EOF)
	{
		// mtllib (material library) 태그라면,
		if (!strcmp(tag, "mtllib"))
		{
			// 재질 파일을 읽는다.
			char mtl_fname[64];
			fscanf(fp, "%s", mtl_fname);			
			load_mtl_file(pMesh, mtl_fname);
		}

		// v (vertex) 태그라면,
		if (!strcmp(tag, "v"))
		{
			// 좌표(x, y, z)를 읽어 정점을 생성하고 메쉬에 추가한다.
			double x, y, z;
			fscanf(fp, "%lf%lf%lf", &x, &y, &z);
			GVertex v(GPoint3(x, y, z), GVector3());
			pMesh->AddVertex(v);
		}

		// vt (vertex texture) 태그라면,
		if (!strcmp(tag, "vt"))
		{
			// 텍스처 좌표(s, t)를 읽어 텍셀을 생성하고 메쉬에 추가한다.
			bTexture = true;
			double s, t;
			fscanf(fp, "%lf%lf", &s, &t);
			pMesh->AddTexel(GVector2(s, t));
		}

		// vn (vertex normal) 태그라면,
		if (!strcmp(tag, "vn"))
		{
			bNormal = true;
		}

		// g (group) 태그라면,
		if (!strcmp(tag, "g"))
		{
			// 그룹의 이름을 읽는다.
			char tmp[64];
			fscanf(fp, "%s", tmp);
			GroupName = std::string(tmp);
		}

		// usemtl (use material) 태그라면,
		if (!strcmp(tag, "usemtl"))
		{
			// 메쉬의 재질 리스트에서 해당 재질의 인덱스를 찾는다.
			char mtl_name[128];
			fscanf(fp, "%s", mtl_name);
			MtlIdx = pMesh->GetMtlIdx(mtl_name);
		}

		// f (face) 태그라면,
		if (!strcmp(tag, "f"))
		{
			// 폴리곤을 구성하는 정점, 텍셀, 노말의 인덱스를 저장하는 변수를 선언하고,
			std::vector<int> vIdxs;
			std::vector<int> tIdxs;
			std::vector<int> nIdxs;

			// 한 줄을 읽어서,
			char line[128];
			fgets(line, 128, fp);
			char *pData = line;
			
			// 한 줄을 모두 읽을 때까지
			while (true)
			{
				// 불필요한 공백과 줄빠꿈을 제거하고,
				while (*pData == ' ' || *pData == '\n')
					pData++;

				// 한 줄을 모두 읽었다면 반복문을 빠져 나온다.
				if (!strcmp(pData, ""))
					break;

				//  인덱스 정보를 buffer로 읽은 후,
				char buffer[64];
				sscanf(pData, "%s", buffer);
				pData += strlen(buffer);

				// 인덱스 정보를 저장할 변수를 선언하고
				char sep;
				int vidx, tidx, nidx;

				// 정점, 텍셀, 노말 인덱스가 모두 있는 경우 (f 1/1/1/ 2/2/2 3/3/3 ...)
				if (bTexture && bNormal)	
				{
					// 모든 인덱스를 읽어 리스트에 추가한다.
					sscanf(buffer, "%d%c%d%c%d", &vidx, &sep, &tidx, &sep, &nidx);
					vIdxs.push_back(vidx);
					tIdxs.push_back(tidx);
					nIdxs.push_back(nidx);
					continue;
				}

				// 정점과 텍셀 인덱스만 있는 경우 (f 1/1 2/2 3/3 ...)
				if (bTexture && !bNormal)
				{
					// 정점과 텍셀 인덱스만 읽어 리스트에 추가한다.
					sscanf(buffer, "%d%c%d", &vidx, &sep, &tidx);
					vIdxs.push_back(vidx);
					tIdxs.push_back(tidx);
					continue;
				}

				// 정점과 노말 인덱스만 있는 경우 (f 1//1 2//2 3//3 ...)
				if (!bTexture && bNormal)
				{
					// 정점과 노말 인덱스만 읽어 리스트에 추가한다.
					sscanf(buffer, "%d%c%c%d", &vidx, &sep, &sep, &nidx);
					vIdxs.push_back(vidx);					
					nIdxs.push_back(nidx);
					continue;
				}

				// 정점의 인덱스만 있는 경우 (f 1 2 3 ...)
				if (!bTexture && !bNormal)
				{
					// 정점의 인덱스만 읽어 리스트에 추가한다.
					sscanf(buffer, "%d", &vidx);
					vIdxs.push_back(vidx);
					continue;
				}
			}
			
			// 삼각형이 하나라도 존재한다면 메쉬의 프리미티브 타입을 FACE_TRI로 변경한다.
			int vnum = (int)vIdxs.size();
			if (vnum == 3)
				pMesh->FaceType = FACE_TRI;

			// 폴리곤을 삼각형으로 나누어 메쉬에 추가한다.
			for (int i = 0; i < vnum - 2; ++i)
			{
				if (bTexture)
				{
					GFace f(vIdxs[0] - 1, vIdxs[i + 1] - 1, vIdxs[i + 2] - 1, tIdxs[0] - 1, tIdxs[i + 1] - 1, tIdxs[i + 2] - 1, MtlIdx);
					pMesh->AddFace(f, GroupName);
				}
				else
				{
					GFace f(vIdxs[0] - 1, vIdxs[i + 1] - 1, vIdxs[i + 2] - 1, vIdxs[0] - 1, vIdxs[i + 1] - 1, vIdxs[i + 2] - 1, MtlIdx);
					pMesh->AddFace(f, GroupName);
				}	
			}
		}
	}

	// 메쉬를 장면에 추가한다.
	pScene->AddMesh(pMesh);

	// 파일을 닫는다.
	fclose(fp);

	return true;
}

/*!
*	\brief 재질 정보를 파일(*.mtl)에 저장한다.
*
*	\param pMesh 저장할 메쉬의 포인터
*	\param fname 파일의 이름
*
*	\return 성공: true, 실패: flase
*/
bool save_mtl_file(GMesh *pMesh, const char *fname)
{
	// 파일을 열고 실패하면 false를 반환한다.
	FILE *fp = fopen(fname, "w");
	if (!fp)
		return false;

	// 파일의 생성 정보 출력하고,
	fprintf(fp, "%s", "# Wavefront MTL exported by Gaia\n");
	fprintf(fp, "%s", "# http://gaia.dongguk.ac.kr \n\n");

	// 각각의 재질에 대하여,
	int NumMtl = pMesh->GetNumMtl();
	for (int i = 0; i < NumMtl; ++i)
	{
		GMaterial *pMtl = &(pMesh->MtlList[i]);

		// 재질의 정보를 출력하고,
		fprintf(fp, "newmtl %s\n", pMtl->Name.c_str());		
		fprintf(fp, "Ka %f %f %f\n", pMtl->Ambient[0], pMtl->Ambient[1], pMtl->Ambient[2]);
		fprintf(fp, "Kd %f %f %f\n", pMtl->Diffuse[0], pMtl->Diffuse[1], pMtl->Diffuse[2]);
		fprintf(fp, "Ks %f %f %f\n", pMtl->Specular[0], pMtl->Specular[1], pMtl->Specular[2]);

		// 텍스처가 있다면 파일명을 출력한다.
		if (!pMtl->TexFileName.empty())
			fprintf(fp, "map_Kd %s\n", pMtl->TexFileName.c_str());

		fprintf(fp, "\n");
	}

	// 파일을 닫는다.
	fclose(fp);

	return true;
}

/*!
*	\brief 메쉬를 파일(*.obj)로 저장한다.
*
*	\param pMesh 저장할 메쉬의 포인터
*	\param fname 파일의 이름
*
*	\return 성공: true, 실패: flase
*/
bool save_mesh_as_obj_file(GMesh *pMesh, const char *fname)
{
	// 메쉬를 월드 좌표계에서 표현한다.
	pMesh->UpdateMC(0);

	// 파일을 열고 실패하면 false를 반환한다.
	FILE *fp = fopen(fname, "w");
	if (!fp)
		return false;

	// 파일의 생성 정보 출력하고,
	fprintf(fp, "%s", "# Wavefront OBJ exported by Gaia\n");
	fprintf(fp, "%s", "# http://gaia.dongguk.edu \n\n");

	// 재질 라이브러리 파일을 출력하고,
	int NumMtl = pMesh->GetNumMtl();
	if (NumMtl != 0)
	{
		// 재질 파일(*.mtl)의 이름을 출력하고,
		std::string file_name(fname);
		file_name = get_file_name(fname) + ".mtl";
		fprintf(fp, "mtllib %s\n\n", file_name.c_str());

		// 재질 정보 파일(*.mtl)을 생성하여 출력한다.
		save_mtl_file(pMesh, file_name.c_str());
	}

	// 정점의 정보를 출력하고,
	int NumVert = (int)pMesh->Verts.size();
	for (int j = 0; j < NumVert; ++j)
	{
		GPoint3 p = pMesh->Verts[j].P;
		fprintf(fp, "%s %lf %lf %lf\n", "v", p[0], p[1], p[2]);
	}
	fprintf(fp, "\n");

	// 텍스처 정보를 출력하고,
	int NumTex = (int)pMesh->Texs.size();
	for (int j = 0; j < NumTex; ++j)
	{
		GVector2 t = pMesh->Texs[j];
		fprintf(fp, "%s %lf %lf\n", "vt", t[0], t[1]);
	}
	fprintf(fp, "\n");

	// 노말 정보를 출력하고,
	for (int j = 0; j < NumVert; ++j)
	{
		GVector3 n = pMesh->Verts[j].N;
		fprintf(fp, "%s %lf %lf %lf\n", "vn", n[0], n[1], n[2]);
	}
	fprintf(fp, "\n");

	// 폴리곤 정보를 출력하는데, 만약 그룹 정보가 없다면,
	if (pMesh->GrpFaces.empty())
	{
		// 이전 재질의 인덱스를 위한 변수를 선언하고,
		int PrevMtlIdx = -1;

		// 순자척으로 모든 폴리곤에 대하여,
		int NumFace = pMesh->GetNumFace();
		for (int j = 0; j < NumFace; ++j)
		{
			// 삼각형 정보를 얻고,
			GFace *f = &(pMesh->Faces[j]);

			// 삼각형의 재질 정보가 있다면 이전 재질정보와 비교하여 다를 경우 출력하고,
			int MtlIdx = f->MtlIdx;
			if (MtlIdx != PrevMtlIdx && MtlIdx < NumMtl)
			{
				fprintf(fp, "usemtl %s\n", pMesh->MtlList[MtlIdx].Name.c_str());
				PrevMtlIdx = MtlIdx;
			}

			// 정점의 인덱스를 구하고,
			int vidx0 = f->vIdx[0] + 1;
			int vidx1 = f->vIdx[1] + 1;
			int vidx2 = f->vIdx[2] + 1;

			// 사각형 메쉬라면,
			if (pMesh->FaceType == FACE_QUAD)
			{
				// 네 번째 정점의 인덱스를 구하고,
				int vidx3 = pMesh->Faces[j + 1].vIdx[2] + 1;

				// 사각형 정보를 출력한다.
				if (pMesh->Texs.empty())
				{
					fprintf(fp, "%s %d//%d %d//%d %d//%d %d//%d\n", "f", vidx0, vidx0, vidx1, vidx1, vidx2, vidx2, vidx3, vidx3);
				}
				else
				{
					int tidx0 = f->tIdx[0] + 1;
					int tidx1 = f->tIdx[1] + 1;
					int tidx2 = f->tIdx[2] + 1;
					int tidx3 = pMesh->Faces[j + 1].tIdx[2] + 1;
					fprintf(fp, "%s %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", "f", vidx0, tidx0, vidx0, vidx1, tidx1, vidx1, vidx2, tidx2, vidx2, vidx3, tidx3, vidx3);
				}
				j++;
			}
			else	// 삼각형 메쉬라면 삼각형 정보를 출력한다.
			{
				if (pMesh->Texs.empty())
				{
					fprintf(fp, "%s %d//%d %d//%d %d//%d\n", "f", vidx0, vidx0, vidx1, vidx1, vidx2, vidx2);
				}
				else
				{
					int tidx0 = f->tIdx[0] + 1;
					int tidx1 = f->tIdx[1] + 1;
					int tidx2 = f->tIdx[2] + 1;
					fprintf(fp, "%s %d/%d/%d %d/%d/%d %d/%d/%d\n", "f", vidx0, tidx0, vidx0, vidx1, tidx1, vidx1, vidx2, tidx2, vidx2);
				}
			}		
		}
		fprintf(fp, "\n");
	}
	else	// 그룹이 존재한다면,
	{
		std::map<std::string, std::vector<int>>::iterator it;
		// 각각의 그룹에 대하여,
		for (it = pMesh->GrpFaces.begin(); it != pMesh->GrpFaces.end(); ++it)
		{
			// 그룹의 이름을 출력하고,
			std::string GroupName = it->first;
			fprintf(fp, "g %s\n", GroupName.c_str());

			// 이전 재질의 인덱스를 위한 변수를 선언하고,
			int PrevMtlIdx = -1;

			// 그룹에 속한 각각의 폴리곤에 대하여,
			int NumFace = (int)it->second.size();
			for (int i = 0; i < NumFace; ++i)
			{
				// 삼각형 정보를 얻고,
				GFace *f = &(pMesh->Faces[it->second[i]]);

				// 삼각형의 재질 정보가 있다면 이전 재질정보와 비교하여 다를 경우 출력하고,
				int MtlIdx = f->MtlIdx;
				if (MtlIdx != PrevMtlIdx && MtlIdx < NumMtl)
				{
					fprintf(fp, "usemtl %s\n", pMesh->MtlList[MtlIdx].Name.c_str());
					PrevMtlIdx = MtlIdx;
				}

				// 정점의 인덱스를 구하고,
				int vidx0 = f->vIdx[0] + 1;
				int vidx1 = f->vIdx[1] + 1;
				int vidx2 = f->vIdx[2] + 1;

				// 사각형 메쉬라면,
				if (pMesh->FaceType == FACE_QUAD)
				{
					// 네 번째 정점의 인덱스를 구하고,
					int vidx3 = pMesh->Faces[it->second[i + 1]].vIdx[2] + 1;

					// 사각형 정보를 출력한다.
					if (pMesh->Texs.empty())
					{
						fprintf(fp, "%s %d//%d %d//%d %d//%d %d//%d\n", "f", vidx0, vidx0, vidx1, vidx1, vidx2, vidx2, vidx3, vidx3);
					}
					else
					{
						int tidx0 = f->tIdx[0] + 1;
						int tidx1 = f->tIdx[1] + 1;
						int tidx2 = f->tIdx[2] + 1;
						int tidx3 = pMesh->Faces[it->second[i + 1]].tIdx[2] + 1;
						fprintf(fp, "%s %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", "f", vidx0, tidx0, vidx0, vidx1, tidx1, vidx1, vidx2, tidx2, vidx2, vidx3, tidx3, vidx3);
					}
					i++;
				}
				else	// 삼각형 메쉬라면 삼각형 정보를 출력한다.
				{
					if (pMesh->Texs.empty())
					{
						fprintf(fp, "%s %d//%d %d//%d %d//%d\n", "f", vidx0, vidx0, vidx1, vidx1, vidx2, vidx2);
					}
					else
					{
						int tidx0 = f->tIdx[0] + 1;
						int tidx1 = f->tIdx[1] + 1;
						int tidx2 = f->tIdx[2] + 1;
						fprintf(fp, "%s %d/%d/%d %d/%d/%d %d/%d/%d\n", "f", vidx0, tidx0, vidx0, vidx1, tidx1, vidx1, vidx2, tidx2, vidx2);
					}
				}
			}
			fprintf(fp, "\n");
		}
	}

	// 파일을 닫는다.
	fclose(fp);

	return true;
}

/*!
*	\brief TRI 파일을 로딩한다.
*
*	\param pScene 로딩할 객체가 저장될 장면의 포인터
*	\param fname 로딩할 파일의 이름
*
*	\return 성공: true, 실패: flase
*/
bool load_mesh_from_tri_file(GScene *pScene, const char *fname)
{
	FILE *fp;
	fp = fopen(fname, "r");
	if (!fp)
		return false;

	std::string file_name = get_file_name(fname);
	GMesh *pMesh = new GMesh(file_name, GTransf(), FACE_TRI);
	
	int vnum, fnum;
	fscanf(fp, "%d%d", &vnum, &fnum);
	for (int i = 0; i < vnum; ++i)
	{
		double x, y, z;
		fscanf(fp, "%lf%lf%lf", &x, &y, &z);
		GVertex v(GPoint3(x, y, z), GVector3());
		pMesh->AddVertex(v);
	}

	for (int i = 0; i < fnum; ++i)
	{
		int idx0, idx1, idx2;
		fscanf(fp, "%d%d%d", &idx0, &idx1, &idx2);
		GFace f(idx0, idx1, idx2);
		pMesh->AddFace(f);
	}

	// 메쉬를 장면에 추가한다.
	pScene->AddMesh(pMesh);

	fclose(fp);
	return true;
}

/*!
*	\brief 선택한 메쉬를 *.tri 파일을 저장한다.
*
*	\param pMesh 저장할 메쉬 객체에 대한 포인터
*	\param fname 로딩할 파일의 이름
*
*	\return 성공: true, 실패: flase
*/
bool save_mesh_as_tri_file(GMesh *pMesh, const char *fname)
{
	if (pMesh == NULL)
		return false;

	FILE *fp;
	fp = fopen(fname, "w");

	// 정점
	int NumVert = (int)pMesh->Verts.size();
	int NumFace = (int)pMesh->Faces.size();

	fprintf(fp, "%d %d\n", NumVert, NumFace);
	for (int i = 0; i < NumVert; ++i)
	{
		GPoint3 p = pMesh->Verts[i].P;
		fprintf(fp, "%lf %lf %lf\n", p[0], p[1], p[2]);
	}

	for (int i = 0; i < NumFace; ++i)
	{
		GFace f = pMesh->Faces[i];

		int vidx0 = f.vIdx[0];
		int vidx1 = f.vIdx[1];
		int vidx2 = f.vIdx[2];

		fprintf(fp, "%d %d %d\n", vidx0, vidx1, vidx2);
	}

	fclose(fp);

	return true;
}


/*!
*	\brief TRI 파일을 로딩한다.
*
*	\param fname 로딩할 파일의 이름
*
*	\return 성공: true, 실패: flase
*/
GMesh *load_mesh_from_tri_file(const char *fname)
{
	FILE *fp;
	fp = fopen(fname, "r");
	if (!fp)
		return false;

	std::string file_name = get_file_name(fname);
	GMesh *pMesh = new GMesh(file_name, GTransf(), FACE_TRI);

	int vnum, fnum;
	fscanf(fp, "%d%d", &vnum, &fnum);
	for (int i = 0; i < vnum; ++i)
	{
		double x, y, z;
		fscanf(fp, "%lf%lf%lf", &x, &y, &z);
		GVertex v(GPoint3(x, y, z), GVector3());
		pMesh->AddVertex(v);
	}

	for (int i = 0; i < fnum; ++i)
	{
		int idx0, idx1, idx2;
		fscanf(fp, "%d%d%d", &idx0, &idx1, &idx2);
		GFace f(idx0, idx1, idx2);
		pMesh->AddFace(f);
	}

	fclose(fp);

	pMesh->ModifiedFlag = MODIFIED_VERT_NUM;
	pMesh->UpdateMesh();

	return pMesh;
}

/*!
*	\brief 장면을 GSF 파일로 저장한다
*	
*	\param pScene 장면 객체의 포인터
*	\param fname 파일이름
*
*	\return true: 성공, false: 실패
*/
bool save_scene_gsf_file(GScene *pScene, const char *fname)
{
	std::string file_name(fname);
	FILE *fp = fopen(file_name.c_str(), "w");

	// 메쉬 리스트 저장
	std::vector<GMesh *> MeshList;

	
	fclose(fp);
	return true;
}

/*!
*	\brief M 파일을 로딩한다.
*
*	\param pScene 로딩할 객체가 저장될 장면의 포인터
*	\param fname 로딩할 파일의 이름
*
*	\return 성공: true, 실패: flase
*/
bool load_mesh_from_m_file(GScene *pScene, const char *fname)
{
	// 파일을 열고 실패하면 false를 반환한다.
	FILE *fp = fopen(fname, "r");
	if (!fp)
		return false;

	// 파일의 이름을 얻어온다.
	std::string file_name = get_file_name(fname);

	// 쿼드타입 메쉬를 생성하여 장면에 추가한다.
	GMesh *pMesh = new GMesh(file_name, GTransf(), FACE_TRI);
	
	// 로딩에 필요한 임시 변수들을 선언한다.
	char tag[256];

	// 파일의 끝까지 한 단어씩 읽으면서 tag 배열에 저장한다.
	while (fscanf(fp, "%s", tag) != EOF)
	{
		// V (vertex) 태그라면,
		if (!strcmp(tag, "V"))
		{
			double x, y, z;
			double nx, ny, nz;
			double s, t;
			fscanf(fp, "%lf%lf%lf%lf%lf%lf%lf%lf", &x, &y, &z, &nx, &ny, &nz, &s, &t);
			GVertex v(GPoint3(x, y, z), GVector3(nx, ny, nz));
			pMesh->AddVertex(v);
			pMesh->AddTexel(GVector2(s, t));
		}

		// I (index) 태그라면,
		if (!strcmp(tag, "I"))
		{
			static std::vector<int> idxlist;

			int vidx;
			fscanf(fp, "%d", &vidx);
			idxlist.push_back(vidx);
			
			if (idxlist.size() == 3)
			{
				GFace f(idxlist[0], idxlist[1], idxlist[2], idxlist[0], idxlist[1], idxlist[2], -1);
				pMesh->AddFace(f);
				idxlist.clear();
			}			
		}		
	}

	// 메쉬를 장면에 추가한다.
	pScene->AddMesh(pMesh);

	// 파일을 닫는다.
	fclose(fp);

	return true;
}

/*!
*	\brief 측지 경로를 저장한다.
*
*	\param pPath 저장할 측지 경로 객체에 대한 포인터
*	\param fname 로딩할 파일의 이름
*
*	\return 성공: true, 실패: flase
*/
bool save_geodesic_path(GGeoPath *pPath, const char *fname)
{
	if (pPath == NULL)
		return false;

	FILE *fp;
	fp = fopen(fname, "w");
	if (fp == NULL)
		return false;

	// 측지 경로의 이름을 저장하고,
	std::string Name = pPath->Name;
	fprintf(fp, "name\t%s\n", Name.c_str());

	// 참조 메쉬의 이름을 저장하고,
	std::string RefMeshName = pPath->pRefMesh->Name;
	fprintf(fp, "reference\t%s\n", RefMeshName.c_str());

	// 경로의 정점들을 저정한다.
	int NumIdx = pPath->PathVertIdx.size();
	for (int i = 0; i < NumIdx; ++i)
		fprintf(fp, "vidx\t%d\n", pPath->PathVertIdx[i]);
	
	fclose(fp);
	return true;
}

/*!
*	\brief 측지 경로 (*.path) 파일을 로딩한다.
*
*	\param pScene 로딩할 객체가 저장될 장면의 포인터
*	\param fname 로딩할 파일의 이름
*
*	\return 성공: true, 실패: flase
*/
bool load_geodesic_path(GScene *pScene, char *fname)
{
	// 파일을 열고 실패하면 false를 반환한다.
	FILE *fp = fopen(fname, "r");
	if (!fp)
		return false;

	char buffer[256];
	char name[256];
	char reference[256];
	// 측지 경로의 이름을 얻어온다.
	fscanf(fp, "%s%s", buffer, name);

	// 참조 메쉬를 얻어온다.
	fscanf(fp, "%s%s", buffer, reference);
	GMesh *pRefMesh = FindMesh(pScene->MeshList, reference);

	// 쿼드타입 메쉬를 생성하여 장면에 추가한다.
	GGeoPath *pPath = new GGeoPath(name, GTransf(), pRefMesh, reference);

	// 로딩에 필요한 임시 변수들을 선언한다.
	char tag[256];

	// 파일의 끝까지 한 단어씩 읽으면서 tag 배열에 저장한다.
	while (fscanf(fp, "%s", tag) != EOF)
	{
		// "vidx" 태그라면,
		if (!strcmp(tag, "vidx"))
		{
			int vidx;
			fscanf(fp, "%d", &vidx);
			pPath->AddPath(vidx);
		}
	}

	// 메쉬를 장면에 추가한다.
	pScene->AddMesh(pPath);

	// 파일을 닫는다.
	fclose(fp);

	return true;
}