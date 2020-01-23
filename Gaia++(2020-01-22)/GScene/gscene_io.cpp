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
*	\brief ASF ������ �����Ѵ�.
*
*	\param szFile ���ϸ�
*
*	\return ASF ���ϱ���ü�� ���� ������
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
*	\brief ASF ������ �ݴ´�.
*
*	\param asf ���µ� ASF ���� ����ü�� ���� ������
*/
void asfClose(ASF *asf)
{
	free(asf->text);
	free(asf);
}

/*!
*	\brief ������ ������ġ�� ���ڰ� �������� �����Ѵ�
*
*	\param asf ���µ� ASF ���� ����ü�� ���� ������
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
*	\brief ���Ͽ��� ������ �����ϰ�, ������ ��ū�� �д´�.
*	\note ���� ��ū�� '*' �ΰ��, �±׸� �߰������� �д´�.
*
*	\param asf ���µ� ASF ���� ����ü�� ���� ������
*
*	\return ���� ��ū�� ����� ��ȯ�Ѵ�. 
*	ASFTOKEN_TAG: *, 
*	ASFTOKEN_LPAREN: {, 
*	ASFTOKEN_RPAREN: }, 
*	ASFTOKEN_STRING: "***",
*	ASFTOKEN_RAW: ������
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
*	\brief ���Ͽ��� ���� ���({...})������ �־��� ��ũ�� ã�� �д´�.
*
*	\param asf ���µ� ASF ���� ����ü�� ���� ������
*	\param tag ã�� �±�
*
*	\return 1: ã�� ���, 0: �� ã�� ���.
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
			asfAscend(asf);		// ���� ����� �ǳʶڴ�.
			break;
		case ASFTOKEN_RPAREN:
			asf->pos--;			// ���� ��ϳ��� �������� �ʴ´�.
			return 0;
		}
	}
}

/*!
*	\brief ���Ͽ��� ���� ���({...})������ ���� ��ũ�� �д´�.
*
*	\param asf ���µ� ASF ���� ����ü�� ���� ������
*
*	\return 1: ���� �±׸� ���� ���, 0: ��ϳ��� �±װ� ���� ���.
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
*	\brief ���Ͽ��� ���� ��ġ���� ���� ������ �Է¹��ڿ��� ��ġ�ϴ��� �����Ѵ�.
*
*	\param asf ���µ� ASF ���� ����ü�� ���� ������
*	\param str ������ ���ڿ��� ���� ������
*
*	\return 1: ���� ���, 0: �ٸ� ���.
*/
int asfCompare(ASF *asf, char *str)
{
	if (strcmp(asf->lexm, str) == 0) 
		return 1;
	return 0;
}

/*!
*	\brief ���� ��ϳ����� �����±��� �о�� �Է¹��ڿ��� ��ġ�ϴ��� �����Ѵ�.
*
*	\param asf ���µ� ASF ���� ����ü�� ���� ������
*	\param str ������ ���ڿ��� ���� ������
*
*	\return 1: ���� ���, 0: �ٸ� ��� Ȥ�� ���� ��ϳ��� �±װ� ���� ���.
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
*	\brief ���� ��ϳ����� �����±��� �о�� �Է¹��ڿ��� ��ġ�ϴ��� �����Ѵ�.
*
*	\param asf ���µ� ASF ���� ����ü�� ���� ������
*	\param num �Է� ���ڿ��� ����
*	\param str1 ������ ù��° ���ڿ��� ���� ������
*
*	\return ��ġ�ϴ� �Է� ���ڿ��� �ε���, -1: �±װ� �Է¹��ڿ��� ��ġ���� �ʴ°��
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
*	\brief ���� ��ϳ����� �±��� ������ ����Ѵ�.
*
*	\param asf ���µ� ASF ���� ����ü�� ���� ������
*	\param tag ������ �±�
*
*	\return �����ϳ��� �±��� ������ ��ȯ�Ѵ� (��������� �ǳʶ�).
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
*	\brief ������ġ���� ������Ͽ� �����Ѵ�.
*
*	\param asf ���µ� ASF ���� ����ü�� ���� ������
*
*	\return 1: ������ ���, 0: ���� ����� ���� ���.
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
*	\brief ���� ��ϳ����� ����������� �ö󰣴�.
*
*	\param asf ���µ� ASF ���� ����ü�� ���� ������
*
*	\return 1: �ö� ���, 0: ������ ��
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
*	\brief ������ġ���� �������·� �����͸� �д´�.
*
*	\param asf ���µ� ASF ���� ����ü�� ���� ������
*
*	\return ���� �������� ��ȯ�Ѵ�.
*/
int asfParseInteger(ASF *asf)
{
	aseGetToken(asf);
	return atoi(asf->lexm);
}

/*!
*	\brief ������ġ���� �Ǽ����·� �����͸� �д´�.
*
*	\param asf ���µ� ASF ���� ����ü�� ���� ������
*
*	\return ���� �Ǽ����� ��ȯ�Ѵ�.
*/
float asfParseFloat(ASF *asf)
{
	aseGetToken(asf);
	return (float)atof(asf->lexm);
}

/*!
*	\brief ������ġ���� ���ڿ��� ���·� �����͸� �д´�.
*
*	\param asf ���µ� ASF ���� ����ü�� ���� ������
*
*	\return ���� ���ڿ��� ������.
*/
char *asfParseString(ASF *asf)
{
	aseGetToken(asf);
	return asf->lexm;
}

/*!
*	\brief ������ġ���� ���ڿ��� ���·� �����͸� �д´�.
*
*	\param asf ���µ� ASF ���� ����ü�� ���� ������
*
*	\return ���� ���ڿ��� ������.
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

// ���� �ε忡 �ʿ��� �Լ���
static bool load_ase_scene(GScene *pScene, ASF *asf);
static void load_ase_scene_info(GScene *pScene, ASF *asf);
static void load_ase_scene_material_list(GScene *pScene, std::vector<GMaterial> &MtrlList, ASF *asf);
static void load_ase_scene_material(GScene *pScene, std::vector<GMaterial> &MtrlList, ASF *asf);
static void load_ase_scene_geom_object(GScene *pScene, ASF *asf);
static void load_ase_scene_helper_object(GScene *pScene, ASF *asf);

// ���� ���忡 �ʿ��� �Լ���
static void save_ase_scene_material_list(FILE *fp, GScene *pScene);
static void save_ase_scene_geom_object_list(FILE *fp, GScene *pScene);

/*!
*	\brief ASE ������ ��� ������ ������ �ε��Ѵ�
*	
*	\param pScene ��� ��ü�� ������
*	\param fname �����̸�
*
*	\return true: ����, false: ����
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
*	\brief ASE ������ ������ �ε��Ѵ�.
*	
*	\param pScene �����Ͱ� ����� ����� ������
*	\param asf ASF ������ ������
*
*	\return true: ����, false: ����
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
	asfDescend(asf);	// SCENE { �������� �̵�.
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
	asfAscend(asf);		// SCENE {...} �������� �̵�.
}

/*!
*	\brief ASF ���Ͽ��� MATERIAL_LIST ��带 �д´�.
*	
*	\param MtrlList ���������� ����� ������ ����Ʈ
*	\param asf ASF ���Ͽ� ���� ����ü ������
*/
void load_ase_scene_material_list(GScene *pScene, std::vector<GMaterial> &MtrlList, ASF *asf)
{
	asfDescend(asf);	// MATERIAL_LIST { �������� �̵�
	asfFindTag(asf, "MATERIAL_COUNT");
	int NumMtrl = asfParseInteger(asf);

	if (NumMtrl > 0)
	{
		for (int i = 0; i < NumMtrl; i++)
		{
			asfFindTag(asf, "MATERIAL");
			asfParseInteger(asf);	// Material ���̵�

			asfDescend(asf);		// MATERIAL id { �������� �̵�.
			load_ase_scene_material(pScene, MtrlList, asf);
			asfAscend(asf);			// MATERIAL {} �������� �̵�.
		}
	}
	asfAscend(asf);		// MATERIAL_LIST {} �������� �̵�.
}

/*!
*	\brief ���� material ��ϳ����� ������ �о���δ�.
*
*	\param Mat ���� ������ ������ ������ ����Ʈ
*	\param asf ���µ� ASF ���� ����ü�� ���� ������
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
			asfDescend(asf);	// MAP_DIFFUSE { �������� �̵��Ѵ�.
			asfFindTag(asf, "BITMAP");

			std::string file_name = asfParseString(asf);
			std::string file_path = pScene->FileName;

			file_name = file_name.substr(file_name.rfind("\\") + 1, file_name.length() - file_name.rfind("\\"));
			file_path = file_path.substr(0, file_path.rfind("\\")) + "\\" + file_name;

			MtrlList[idx].LoadTexture(file_path.c_str());
			asfAscend(asf);		// MAP_DIFFUSE {} �������� �̵��Ѵ�.
		}

		if (asfCompare(asf, "NUMSUBMTLS"))
		{
// 			int NumSubMat = asfParseInteger(asf);
// 			for(int j = 0; j < NumSubMat; j++)
// 			{
// 				asfFindTag(asf, "SUBMATERIAL");
// 				asfParseInteger(asf);
// 				asfDescend(asf);	// SUBMATERIAL { �������� �̵�.
// 				load_ase_scene_material(pScene, MtrlList[idx].SubMtrlList, asf);
// 				asfAscend(asf);		// SUBMATERIAL {} �������� �̵�.
// 			}
		}
	}
}

/*!
*	\brief ASF ���Ͽ��� GEOMOBJECT ��带 �д´�.
*	
*	\param pScene �д� ������ ����� ��鿡 ���� ������.
*	\param asf ASF ���Ͽ� ���� ����ü ������.
*/
void load_ase_scene_geom_object(GScene *pScene, ASF *asf)
{
	asfDescend(asf);	// GEOMOBJECT { �������� �̵�.

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
			asfDescend(asf);	// NODE_TM { �������� �̵�
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

			if (ParentName == "")	// �θ�����Ʈ�� ���� ���.
			{
				pMesh->DelX = GTransf(pos, rot);
			}
			else				// �θ�����Ʈ�� �ִ� ���.
			{
				GMesh *pParent = FindMesh(pScene->MeshList, ParentName);
				pMesh->DelX = inv(pParent->MC) * pMesh->MC; // �θ� ��ǥ�迡 �°� ��ȯ				
			}
			asfAscend(asf);		// NOTE_TM {} �������� �̵�.
		}

		// *NODE_MESH
		if (asfCompare(asf, "MESH"))
		{
			asfDescend(asf);	// MESH { �������� �̵�

			asfFindTag(asf, "MESH_NUMVERTEX");
			int vNum = asfParseInteger(asf);

			asfFindTag(asf, "MESH_NUMFACES");
			int fNum = asfParseInteger(asf);

			asfFindTag(asf,"MESH_VERTEX_LIST");
			asfDescend(asf);		// MESH_VERTEX_LIST { �������� �̵�
			for (int i = 0; i < vNum; i++)
			{
				asfFindTag(asf, "MESH_VERTEX");
				asfParseInteger(asf); // ���ؽ� ��ȣ

				double x, y, z;
				x = asfParseFloat(asf);
				y = asfParseFloat(asf);
				z = asfParseFloat(asf);

				GPoint3 pt(x, y, z);
				pt = inv(pMesh->MC) * pt;

				pMesh->AddVertex(GVertex(pt, GVector3(0.0, 0.0, 0.0)));
			}
			asfAscend(asf);			// MESH_VERTEX_LIST {} �������� �̵�

			asfFindTag(asf,"MESH_FACE_LIST");
			asfDescend(asf);		// MESH_FACE_LIST { �������� �̵�
			for (int i = 0; i < fNum; i++)
			{
				asfFindTag(asf, "MESH_FACE");
				asfParseRaw(asf);	// ���̽� ��ȣ

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
			asfAscend(asf);			// MESH_FACE_LIST {} �������� �̵�

			if (asfNextTag(asf, "MESH_NUMTVERTEX"))
			{
				int tNum = asfParseInteger(asf);
				if (tNum)
				{
					asfFindTag(asf, "MESH_TVERTLIST");
					asfDescend(asf);	// MESH_TVERTLIST { �������� �̵�.
					for (int i = 0; i < tNum; i++)
					{
						asfFindTag(asf, "MESH_TVERT");
						asfParseInteger(asf);
						double u, v;
						u = asfParseFloat(asf);
						v = asfParseFloat(asf);
						pMesh->AddTexel(GVector2(u, v));
					}
					asfAscend(asf);		// MESH_TVERTLIST {} �������� �̵�.

					asfFindTag(asf, "MESH_NUMTVFACES");
					int fNum = asfParseInteger(asf);

					asfFindTag(asf, "MESH_TFACELIST");
					asfDescend(asf);	// MESH_TFACELIST { �������� �̵�.
					for (int i = 0; i < fNum; i++)
					{
						asfFindTag(asf, "MESH_TFACE");
						asfParseInteger(asf);
						pMesh->Faces[i].tIdx[0] = asfParseInteger(asf);
						pMesh->Faces[i].tIdx[1] = asfParseInteger(asf);
						pMesh->Faces[i].tIdx[2] = asfParseInteger(asf);
					}
					asfAscend(asf);		// MESH_TFACELIST {} �������� �̵�.
				}
			}
			asfAscend(asf);		// MESH {} �������� �̵�			
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
			asfDescend(asf);	// TM_ANIMATION { �������� �̵�

			int NumKeyPos = 0, NumKeyRot = 0;
			GVector3 *KeyPos = new GVector3 [AseNumFrames];
			GQuater *KeyRot = new GQuater [AseNumFrames];

			while (asfGetTag(asf))
			{
				if (asfCompare(asf, "CONTROL_POS_TRACK"))
				{
					asfDescend(asf);	// CONTROL_POS_TRACK { �������� �̵�
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
					asfAscend(asf);		// CONTROL_POS_TRACK {} �������� �̵�
				}
				else if (asfCompare(asf, "CONTROL_ROT_TRACK"))
				{
					asfDescend(asf);	// CONTROL_ROT_TRACK { �������� �̵�
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
					asfAscend(asf);		// CONTROL_ROT_TRACK {} �������� �̵�
				}
			}

			for (int i = 0; i < AseNumFrames; i++)
			{
				GVector3 t;
				if (NumKeyPos == 0)	// CONTROL_POS_TRACK�� ���� ���,
					t = part_trans(pMesh->DelX);
				else
					t = KeyPos[i];

				GQuater q;
				if (NumKeyRot == 0)		// CONTROL_ROT_TRACK�� ���� ���,
					q = part_rot(pMesh->DelX);
				else
					q = KeyRot[i];

				GTransf InX(t, q);
				InX = inv(pMesh->DelX) * InX;

				pMesh->Animation.push_back(InX);
			}

			delete [] KeyPos;
			delete [] KeyRot;

			asfAscend(asf);		// ANIMATION_TM {} �������� �̵�.
		}

		// *PHYSIQUE
		if (asfCompare(asf, "PHYSIQUE"))
		{
			pMesh->SemanticType = MESH_SKIN;
			asfDescend(asf);	// PHYSIQUE { �������� �̵�

			asfFindTag(asf, "PHYSIQUE_LIST");
			asfDescend(asf);	// PHYSIQUE_LIST { �������� �̵�

			std::vector<GVertex> &VertList = pMesh->Verts;
			while (asfGetTag(asf))
			{
				if (asfCompare(asf, "PHYSIQUE_NONBLENDED_RIGIDTYPE"))
				{
					int vidx = asfParseInteger(asf);			// vertex �ε���
					std::string MeshName(asfParseString(asf));		// ���ε� ����Ʈ�� �̸�
					VertList[vidx].AddRefBoneInfo(MeshName, 1.0);
				}

				if (asfCompare(asf, "PHYSIQUE_BLENDED_RIGIDTYPE"))
				{
					int vidx = asfParseInteger(asf);
					asfDescend(asf);		// PHYSIQUE_BLENDED_RIGIDTYPE vidx { �������� �̵�
					asfFindTag(asf, "PHYSIQUE_NUMNODE");
					int NumMesh = asfParseInteger(asf);

					asfFindTag(asf, "PHYSIQUE_NODELIST");
					asfDescend(asf);		// PHYSIQUE_NODELIST { �������� �̵�

					for (int i = 0; i < NumMesh; i++)
					{
						asfFindTag(asf, "PHYSIQUE_NODE");
						asfParseInteger(asf);
						double weight = asfParseFloat(asf);
						std::string MeshName(asfParseString(asf));
						VertList[vidx].AddRefBoneInfo(MeshName, weight);
					}

					asfAscend(asf);	// PHYSIQUE_NODELIST {} �������� �̵�
					asfAscend(asf);	// PHYSIQUE_BLENDED_RIGIDTYPE vidx {} �������� �̵�
				}
			}
			asfAscend(asf);		// PHYSIQUE_LIST {} �������� �̵�
			asfAscend(asf);		// PHYSIQUE {} �������� �̵�
		}

		// *SKIN
		if (asfCompare(asf, "SKIN"))
		{
			asfDescend(asf);	// SKIN { �������� �̵�

			asfAscend(asf);		// SKIN {} �������� �̵�.
		}
	}
	asfAscend(asf); // GEOMOBJECT {...} �������� �̵�.

	pScene->AddMesh(pMesh);
}

/*!
*	\brief ASF ���Ͽ��� HELPEROBJECT ��带 �д´�.
*	
*	\param pScene �д� ������ ����� ��鿡 ���� ������.
*	\param asf ASF ���Ͽ� ���� ����ü ������.
*/
void load_ase_scene_helper_object(GScene *pScene, ASF *asf)
{
// 	std::vector<GJoint *> &JointList = pModel->GetJointList();
// 
// 	asfDescend(asf);	// HELPEROBJECT { ��������...
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
// 				asfAscend(asf);		// HELPEROBJECT {} ��������...
// 				return;
// 			}
// 		}
// 
// 		if (pJoint != NULL && asfCompare(asf, "NODE_TM"))
// 		{
// 			asfDescend(asf);	// NODE_TM { ��������...
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
// 			asfAscend(asf);	// NOTD_TM {} ��������...
// 		}
// 	}
// 	asfAscend(asf);	// HELPEROBJECT {} ��������...
}

/*!
*	\brief ����� ASF ���Ϸ� �����Ѵ�
*	
*	\param pScene ��� ��ü�� ������
*	\param fname �����̸�
*
*	\return true: ����, false: ����
*/
bool save_scene_as_ase_file(GScene *pScene, const char *fname)
{
	std::string file_name(fname);
	FILE *fp = fopen(file_name.c_str(), "w");

	// ���� ����Ʈ ����
	//save_ase_scene_material_list(fp, pScene);

	// �޽� ����Ʈ ����
	save_ase_scene_geom_object_list(fp, pScene);

	// �ҽ� ��ü ����Ʈ ����
	//save_ase_scene_gnurbs_object_lise(fp, pScene);

	fclose(fp);
	return true;
}

/*!
*	\brief ��鿡 ���Ե� ������ ���Ϸ� �����Ѵ�.
*	
*	\param fp ���� ������ ����� ������ ������
*	\param pScene ���� ������ ������ ����� ������
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
*	\brief ��鿡 ���Ե� �޽��� ���Ϸ� �����Ѵ�.
*	
*	\param fp �޽� ������ ����� ������ ������
*	\param pScene �޽� ������ ������ ����� ������
*/
void save_ase_scene_geom_object_list(FILE *fp, GScene *pScene)
{
	// ��鿡 ���Ե� ��� �޽��� ���� ����Ʈ�� ����
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
*	\brief ����(*.mtl)�κ��� ���� ������ �о� �޽��� �߰��Ѵ�.
*
*	\param pMesh ������ �߰��� �޽��� ������
*	\param fname ���� ������ �̸�
*
*	\return ����: true, ����: flase
*/
bool load_mtl_file(GMesh *pMesh, const char *fname)
{
	// ������ ����, �����ϸ� false�� ��ȯ�Ѵ�.
	FILE *fp = fopen(fname, "r");
	if (!fp)
		return false;

	// �ε��� �ʿ��� �ӽ� �������� �����Ѵ�.
	char tag[256];
	GMaterial *pMat = NULL;

	// ������ ������ �� �ܾ �о�, tag �迭�� �����Ѵ�.
	while (fscanf(fp, "%s", tag) != EOF)
	{
		// newmtl (new material) �±׶��,
		if (!strcmp(tag, "newmtl"))
		{
			// ������ �̸��� �а�,
			char tmp[64];
			fscanf(fp, "%s", tmp);

			// ������ �����Ͽ�, �޽��� ���� ����Ʈ�� �߰��Ѵ�.
			GMaterial mtl(tmp);
			pMesh->AddMtl(mtl);

			// ��� �߰��� ������ �����͸� ��´�.
			pMat = pMesh->FindMtl(mtl.Name);
		}

		// Ka (ambient coefficients) �±׶��,
		if (!strcmp(tag, "Ka"))
		{
			// diffuse ������ �о ������ �����Ѵ�.
			double r, g, b;
			fscanf(fp, "%lf%lf%lf", &r, &g, &b);
			pMat->SetAmbient(r, g, b);
		}

		// Kd (diffuse coefficients) �±׶��,
		if (!strcmp(tag, "Kd"))
		{
			// diffuse ������ �о ������ �����Ѵ�.
			double r, g, b;
			fscanf(fp, "%lf%lf%lf", &r, &g, &b);
			pMat->SetDiffuse(r, g, b);
		}

		// Ks (specular coefficients) �±׶��,
		if (!strcmp(tag, "Ks"))
		{
			// diffuse ������ �о ������ �����Ѵ�.
			double r, g, b;
			fscanf(fp, "%lf%lf%lf", &r, &g, &b);
			pMat->SetSpecular(r, g, b);
		}

		// map_Kd (diffuse texture file) �±׶��,
		if (!strcmp(tag, "map_Kd"))
		{
			// �ý�ó ������ �д´�.
			char tex_name[64];
			fscanf(fp, "%s", tex_name);
			pMat->LoadTexture(tex_name);
		}
	}

	// ������ �ݴ´�.
	fclose(fp);

	return true;
}

/*!
*	\brief ����(*.obj)�κ��� �޽��� �����ϰ� ��鿡 �߰��Ѵ�.
*	\note ������ �븻 ������ ������
*
*	\param pScene �ε��� ��ü�� ����� ����� ������
*	\param fname �����ΰ� ���Ե� ������ �̸�
*
*	\return ����: true, ����: flase
*/
bool load_mesh_from_obj_file(GScene *pScene, const char *fname)
{
	// ������ ���� �����ϸ� false�� ��ȯ�Ѵ�.
	FILE *fp = fopen(fname, "r");
	if (!fp)
		return false;

	// ������ �̸��� ���´�.
	std::string file_name = get_file_name(fname);
	
	// ����Ÿ�� �޽��� �����Ѵ�.
	GMesh *pMesh = new GMesh(file_name, GTransf(), FACE_QUAD);
	
	// �ε��� �ʿ��� �ӽ� �������� �����Ѵ�.
	char tag[256];
	bool bTexture = false;
	bool bNormal = false;
	std::string GroupName;
	int MtlIdx = -1;

	// ������ ������ �� �ܾ �����鼭 tag �迭�� �����Ѵ�.
	while (fscanf(fp, "%s", tag) != EOF)
	{
		// mtllib (material library) �±׶��,
		if (!strcmp(tag, "mtllib"))
		{
			// ���� ������ �д´�.
			char mtl_fname[64];
			fscanf(fp, "%s", mtl_fname);			
			load_mtl_file(pMesh, mtl_fname);
		}

		// v (vertex) �±׶��,
		if (!strcmp(tag, "v"))
		{
			// ��ǥ(x, y, z)�� �о� ������ �����ϰ� �޽��� �߰��Ѵ�.
			double x, y, z;
			fscanf(fp, "%lf%lf%lf", &x, &y, &z);
			GVertex v(GPoint3(x, y, z), GVector3());
			pMesh->AddVertex(v);
		}

		// vt (vertex texture) �±׶��,
		if (!strcmp(tag, "vt"))
		{
			// �ؽ�ó ��ǥ(s, t)�� �о� �ؼ��� �����ϰ� �޽��� �߰��Ѵ�.
			bTexture = true;
			double s, t;
			fscanf(fp, "%lf%lf", &s, &t);
			pMesh->AddTexel(GVector2(s, t));
		}

		// vn (vertex normal) �±׶��,
		if (!strcmp(tag, "vn"))
		{
			bNormal = true;
		}

		// g (group) �±׶��,
		if (!strcmp(tag, "g"))
		{
			// �׷��� �̸��� �д´�.
			char tmp[64];
			fscanf(fp, "%s", tmp);
			GroupName = std::string(tmp);
		}

		// usemtl (use material) �±׶��,
		if (!strcmp(tag, "usemtl"))
		{
			// �޽��� ���� ����Ʈ���� �ش� ������ �ε����� ã�´�.
			char mtl_name[128];
			fscanf(fp, "%s", mtl_name);
			MtlIdx = pMesh->GetMtlIdx(mtl_name);
		}

		// f (face) �±׶��,
		if (!strcmp(tag, "f"))
		{
			// �������� �����ϴ� ����, �ؼ�, �븻�� �ε����� �����ϴ� ������ �����ϰ�,
			std::vector<int> vIdxs;
			std::vector<int> tIdxs;
			std::vector<int> nIdxs;

			// �� ���� �о,
			char line[128];
			fgets(line, 128, fp);
			char *pData = line;
			
			// �� ���� ��� ���� ������
			while (true)
			{
				// ���ʿ��� ����� �ٺ����� �����ϰ�,
				while (*pData == ' ' || *pData == '\n')
					pData++;

				// �� ���� ��� �о��ٸ� �ݺ����� ���� ���´�.
				if (!strcmp(pData, ""))
					break;

				//  �ε��� ������ buffer�� ���� ��,
				char buffer[64];
				sscanf(pData, "%s", buffer);
				pData += strlen(buffer);

				// �ε��� ������ ������ ������ �����ϰ�
				char sep;
				int vidx, tidx, nidx;

				// ����, �ؼ�, �븻 �ε����� ��� �ִ� ��� (f 1/1/1/ 2/2/2 3/3/3 ...)
				if (bTexture && bNormal)	
				{
					// ��� �ε����� �о� ����Ʈ�� �߰��Ѵ�.
					sscanf(buffer, "%d%c%d%c%d", &vidx, &sep, &tidx, &sep, &nidx);
					vIdxs.push_back(vidx);
					tIdxs.push_back(tidx);
					nIdxs.push_back(nidx);
					continue;
				}

				// ������ �ؼ� �ε����� �ִ� ��� (f 1/1 2/2 3/3 ...)
				if (bTexture && !bNormal)
				{
					// ������ �ؼ� �ε����� �о� ����Ʈ�� �߰��Ѵ�.
					sscanf(buffer, "%d%c%d", &vidx, &sep, &tidx);
					vIdxs.push_back(vidx);
					tIdxs.push_back(tidx);
					continue;
				}

				// ������ �븻 �ε����� �ִ� ��� (f 1//1 2//2 3//3 ...)
				if (!bTexture && bNormal)
				{
					// ������ �븻 �ε����� �о� ����Ʈ�� �߰��Ѵ�.
					sscanf(buffer, "%d%c%c%d", &vidx, &sep, &sep, &nidx);
					vIdxs.push_back(vidx);					
					nIdxs.push_back(nidx);
					continue;
				}

				// ������ �ε����� �ִ� ��� (f 1 2 3 ...)
				if (!bTexture && !bNormal)
				{
					// ������ �ε����� �о� ����Ʈ�� �߰��Ѵ�.
					sscanf(buffer, "%d", &vidx);
					vIdxs.push_back(vidx);
					continue;
				}
			}
			
			// �ﰢ���� �ϳ��� �����Ѵٸ� �޽��� ������Ƽ�� Ÿ���� FACE_TRI�� �����Ѵ�.
			int vnum = (int)vIdxs.size();
			if (vnum == 3)
				pMesh->FaceType = FACE_TRI;

			// �������� �ﰢ������ ������ �޽��� �߰��Ѵ�.
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

	// �޽��� ��鿡 �߰��Ѵ�.
	pScene->AddMesh(pMesh);

	// ������ �ݴ´�.
	fclose(fp);

	return true;
}

/*!
*	\brief ���� ������ ����(*.mtl)�� �����Ѵ�.
*
*	\param pMesh ������ �޽��� ������
*	\param fname ������ �̸�
*
*	\return ����: true, ����: flase
*/
bool save_mtl_file(GMesh *pMesh, const char *fname)
{
	// ������ ���� �����ϸ� false�� ��ȯ�Ѵ�.
	FILE *fp = fopen(fname, "w");
	if (!fp)
		return false;

	// ������ ���� ���� ����ϰ�,
	fprintf(fp, "%s", "# Wavefront MTL exported by Gaia\n");
	fprintf(fp, "%s", "# http://gaia.dongguk.ac.kr \n\n");

	// ������ ������ ���Ͽ�,
	int NumMtl = pMesh->GetNumMtl();
	for (int i = 0; i < NumMtl; ++i)
	{
		GMaterial *pMtl = &(pMesh->MtlList[i]);

		// ������ ������ ����ϰ�,
		fprintf(fp, "newmtl %s\n", pMtl->Name.c_str());		
		fprintf(fp, "Ka %f %f %f\n", pMtl->Ambient[0], pMtl->Ambient[1], pMtl->Ambient[2]);
		fprintf(fp, "Kd %f %f %f\n", pMtl->Diffuse[0], pMtl->Diffuse[1], pMtl->Diffuse[2]);
		fprintf(fp, "Ks %f %f %f\n", pMtl->Specular[0], pMtl->Specular[1], pMtl->Specular[2]);

		// �ؽ�ó�� �ִٸ� ���ϸ��� ����Ѵ�.
		if (!pMtl->TexFileName.empty())
			fprintf(fp, "map_Kd %s\n", pMtl->TexFileName.c_str());

		fprintf(fp, "\n");
	}

	// ������ �ݴ´�.
	fclose(fp);

	return true;
}

/*!
*	\brief �޽��� ����(*.obj)�� �����Ѵ�.
*
*	\param pMesh ������ �޽��� ������
*	\param fname ������ �̸�
*
*	\return ����: true, ����: flase
*/
bool save_mesh_as_obj_file(GMesh *pMesh, const char *fname)
{
	// �޽��� ���� ��ǥ�迡�� ǥ���Ѵ�.
	pMesh->UpdateMC(0);

	// ������ ���� �����ϸ� false�� ��ȯ�Ѵ�.
	FILE *fp = fopen(fname, "w");
	if (!fp)
		return false;

	// ������ ���� ���� ����ϰ�,
	fprintf(fp, "%s", "# Wavefront OBJ exported by Gaia\n");
	fprintf(fp, "%s", "# http://gaia.dongguk.edu \n\n");

	// ���� ���̺귯�� ������ ����ϰ�,
	int NumMtl = pMesh->GetNumMtl();
	if (NumMtl != 0)
	{
		// ���� ����(*.mtl)�� �̸��� ����ϰ�,
		std::string file_name(fname);
		file_name = get_file_name(fname) + ".mtl";
		fprintf(fp, "mtllib %s\n\n", file_name.c_str());

		// ���� ���� ����(*.mtl)�� �����Ͽ� ����Ѵ�.
		save_mtl_file(pMesh, file_name.c_str());
	}

	// ������ ������ ����ϰ�,
	int NumVert = (int)pMesh->Verts.size();
	for (int j = 0; j < NumVert; ++j)
	{
		GPoint3 p = pMesh->Verts[j].P;
		fprintf(fp, "%s %lf %lf %lf\n", "v", p[0], p[1], p[2]);
	}
	fprintf(fp, "\n");

	// �ؽ�ó ������ ����ϰ�,
	int NumTex = (int)pMesh->Texs.size();
	for (int j = 0; j < NumTex; ++j)
	{
		GVector2 t = pMesh->Texs[j];
		fprintf(fp, "%s %lf %lf\n", "vt", t[0], t[1]);
	}
	fprintf(fp, "\n");

	// �븻 ������ ����ϰ�,
	for (int j = 0; j < NumVert; ++j)
	{
		GVector3 n = pMesh->Verts[j].N;
		fprintf(fp, "%s %lf %lf %lf\n", "vn", n[0], n[1], n[2]);
	}
	fprintf(fp, "\n");

	// ������ ������ ����ϴµ�, ���� �׷� ������ ���ٸ�,
	if (pMesh->GrpFaces.empty())
	{
		// ���� ������ �ε����� ���� ������ �����ϰ�,
		int PrevMtlIdx = -1;

		// ����ô���� ��� �����￡ ���Ͽ�,
		int NumFace = pMesh->GetNumFace();
		for (int j = 0; j < NumFace; ++j)
		{
			// �ﰢ�� ������ ���,
			GFace *f = &(pMesh->Faces[j]);

			// �ﰢ���� ���� ������ �ִٸ� ���� ���������� ���Ͽ� �ٸ� ��� ����ϰ�,
			int MtlIdx = f->MtlIdx;
			if (MtlIdx != PrevMtlIdx && MtlIdx < NumMtl)
			{
				fprintf(fp, "usemtl %s\n", pMesh->MtlList[MtlIdx].Name.c_str());
				PrevMtlIdx = MtlIdx;
			}

			// ������ �ε����� ���ϰ�,
			int vidx0 = f->vIdx[0] + 1;
			int vidx1 = f->vIdx[1] + 1;
			int vidx2 = f->vIdx[2] + 1;

			// �簢�� �޽����,
			if (pMesh->FaceType == FACE_QUAD)
			{
				// �� ��° ������ �ε����� ���ϰ�,
				int vidx3 = pMesh->Faces[j + 1].vIdx[2] + 1;

				// �簢�� ������ ����Ѵ�.
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
			else	// �ﰢ�� �޽���� �ﰢ�� ������ ����Ѵ�.
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
	else	// �׷��� �����Ѵٸ�,
	{
		std::map<std::string, std::vector<int>>::iterator it;
		// ������ �׷쿡 ���Ͽ�,
		for (it = pMesh->GrpFaces.begin(); it != pMesh->GrpFaces.end(); ++it)
		{
			// �׷��� �̸��� ����ϰ�,
			std::string GroupName = it->first;
			fprintf(fp, "g %s\n", GroupName.c_str());

			// ���� ������ �ε����� ���� ������ �����ϰ�,
			int PrevMtlIdx = -1;

			// �׷쿡 ���� ������ �����￡ ���Ͽ�,
			int NumFace = (int)it->second.size();
			for (int i = 0; i < NumFace; ++i)
			{
				// �ﰢ�� ������ ���,
				GFace *f = &(pMesh->Faces[it->second[i]]);

				// �ﰢ���� ���� ������ �ִٸ� ���� ���������� ���Ͽ� �ٸ� ��� ����ϰ�,
				int MtlIdx = f->MtlIdx;
				if (MtlIdx != PrevMtlIdx && MtlIdx < NumMtl)
				{
					fprintf(fp, "usemtl %s\n", pMesh->MtlList[MtlIdx].Name.c_str());
					PrevMtlIdx = MtlIdx;
				}

				// ������ �ε����� ���ϰ�,
				int vidx0 = f->vIdx[0] + 1;
				int vidx1 = f->vIdx[1] + 1;
				int vidx2 = f->vIdx[2] + 1;

				// �簢�� �޽����,
				if (pMesh->FaceType == FACE_QUAD)
				{
					// �� ��° ������ �ε����� ���ϰ�,
					int vidx3 = pMesh->Faces[it->second[i + 1]].vIdx[2] + 1;

					// �簢�� ������ ����Ѵ�.
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
				else	// �ﰢ�� �޽���� �ﰢ�� ������ ����Ѵ�.
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

	// ������ �ݴ´�.
	fclose(fp);

	return true;
}

/*!
*	\brief TRI ������ �ε��Ѵ�.
*
*	\param pScene �ε��� ��ü�� ����� ����� ������
*	\param fname �ε��� ������ �̸�
*
*	\return ����: true, ����: flase
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

	// �޽��� ��鿡 �߰��Ѵ�.
	pScene->AddMesh(pMesh);

	fclose(fp);
	return true;
}

/*!
*	\brief ������ �޽��� *.tri ������ �����Ѵ�.
*
*	\param pMesh ������ �޽� ��ü�� ���� ������
*	\param fname �ε��� ������ �̸�
*
*	\return ����: true, ����: flase
*/
bool save_mesh_as_tri_file(GMesh *pMesh, const char *fname)
{
	if (pMesh == NULL)
		return false;

	FILE *fp;
	fp = fopen(fname, "w");

	// ����
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
*	\brief TRI ������ �ε��Ѵ�.
*
*	\param fname �ε��� ������ �̸�
*
*	\return ����: true, ����: flase
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
*	\brief ����� GSF ���Ϸ� �����Ѵ�
*	
*	\param pScene ��� ��ü�� ������
*	\param fname �����̸�
*
*	\return true: ����, false: ����
*/
bool save_scene_gsf_file(GScene *pScene, const char *fname)
{
	std::string file_name(fname);
	FILE *fp = fopen(file_name.c_str(), "w");

	// �޽� ����Ʈ ����
	std::vector<GMesh *> MeshList;

	
	fclose(fp);
	return true;
}

/*!
*	\brief M ������ �ε��Ѵ�.
*
*	\param pScene �ε��� ��ü�� ����� ����� ������
*	\param fname �ε��� ������ �̸�
*
*	\return ����: true, ����: flase
*/
bool load_mesh_from_m_file(GScene *pScene, const char *fname)
{
	// ������ ���� �����ϸ� false�� ��ȯ�Ѵ�.
	FILE *fp = fopen(fname, "r");
	if (!fp)
		return false;

	// ������ �̸��� ���´�.
	std::string file_name = get_file_name(fname);

	// ����Ÿ�� �޽��� �����Ͽ� ��鿡 �߰��Ѵ�.
	GMesh *pMesh = new GMesh(file_name, GTransf(), FACE_TRI);
	
	// �ε��� �ʿ��� �ӽ� �������� �����Ѵ�.
	char tag[256];

	// ������ ������ �� �ܾ �����鼭 tag �迭�� �����Ѵ�.
	while (fscanf(fp, "%s", tag) != EOF)
	{
		// V (vertex) �±׶��,
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

		// I (index) �±׶��,
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

	// �޽��� ��鿡 �߰��Ѵ�.
	pScene->AddMesh(pMesh);

	// ������ �ݴ´�.
	fclose(fp);

	return true;
}

/*!
*	\brief ���� ��θ� �����Ѵ�.
*
*	\param pPath ������ ���� ��� ��ü�� ���� ������
*	\param fname �ε��� ������ �̸�
*
*	\return ����: true, ����: flase
*/
bool save_geodesic_path(GGeoPath *pPath, const char *fname)
{
	if (pPath == NULL)
		return false;

	FILE *fp;
	fp = fopen(fname, "w");
	if (fp == NULL)
		return false;

	// ���� ����� �̸��� �����ϰ�,
	std::string Name = pPath->Name;
	fprintf(fp, "name\t%s\n", Name.c_str());

	// ���� �޽��� �̸��� �����ϰ�,
	std::string RefMeshName = pPath->pRefMesh->Name;
	fprintf(fp, "reference\t%s\n", RefMeshName.c_str());

	// ����� �������� �����Ѵ�.
	int NumIdx = pPath->PathVertIdx.size();
	for (int i = 0; i < NumIdx; ++i)
		fprintf(fp, "vidx\t%d\n", pPath->PathVertIdx[i]);
	
	fclose(fp);
	return true;
}

/*!
*	\brief ���� ��� (*.path) ������ �ε��Ѵ�.
*
*	\param pScene �ε��� ��ü�� ����� ����� ������
*	\param fname �ε��� ������ �̸�
*
*	\return ����: true, ����: flase
*/
bool load_geodesic_path(GScene *pScene, char *fname)
{
	// ������ ���� �����ϸ� false�� ��ȯ�Ѵ�.
	FILE *fp = fopen(fname, "r");
	if (!fp)
		return false;

	char buffer[256];
	char name[256];
	char reference[256];
	// ���� ����� �̸��� ���´�.
	fscanf(fp, "%s%s", buffer, name);

	// ���� �޽��� ���´�.
	fscanf(fp, "%s%s", buffer, reference);
	GMesh *pRefMesh = FindMesh(pScene->MeshList, reference);

	// ����Ÿ�� �޽��� �����Ͽ� ��鿡 �߰��Ѵ�.
	GGeoPath *pPath = new GGeoPath(name, GTransf(), pRefMesh, reference);

	// �ε��� �ʿ��� �ӽ� �������� �����Ѵ�.
	char tag[256];

	// ������ ������ �� �ܾ �����鼭 tag �迭�� �����Ѵ�.
	while (fscanf(fp, "%s", tag) != EOF)
	{
		// "vidx" �±׶��,
		if (!strcmp(tag, "vidx"))
		{
			int vidx;
			fscanf(fp, "%d", &vidx);
			pPath->AddPath(vidx);
		}
	}

	// �޽��� ��鿡 �߰��Ѵ�.
	pScene->AddMesh(pPath);

	// ������ �ݴ´�.
	fclose(fp);

	return true;
}