// GaiaDoc.cpp : GaiaDoc Ŭ������ ����
//
#include "stdafx.h"
#include "Gaia.h"
#include "MainFrm.h"
#include "GaiaDoc.h"
#include "GaiaView.h"
#include <direct.h>
#include <string>

// ETRI
#include "GAvatar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::string WorkingDirectory = "E:\\Data\\Research\\GaiaProjectData\\";

// GaiaDoc
IMPLEMENT_DYNCREATE(GaiaDoc, CDocument)

BEGIN_MESSAGE_MAP(GaiaDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, &GaiaDoc::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, &GaiaDoc::OnFileSave)
	ON_COMMAND(ID_FILE_IMPORT, &GaiaDoc::OnFileImport)
	ON_COMMAND(ID_FILE_EXPORT, &GaiaDoc::OnFileExport)
	ON_COMMAND(ID_DELETE_MESH, &GaiaDoc::OnDeleteMesh)
	ON_COMMAND(ID_SHOW_MESH, &GaiaDoc::OnShowMesh)
	ON_UPDATE_COMMAND_UI(ID_SHOW_MESH, &GaiaDoc::OnUpdateShowMesh)
	ON_COMMAND(ID_HIDE_MESH, &GaiaDoc::OnHideMesh)
	ON_UPDATE_COMMAND_UI(ID_HIDE_MESH, &GaiaDoc::OnUpdateHideMesh)
	ON_COMMAND(ID_TEST_SCENE0, &GaiaDoc::OnTestScene0)
	ON_COMMAND(ID_TEST_SCENE1, &GaiaDoc::OnTestScene1)
	ON_COMMAND(ID_TEST_SCENE2, &GaiaDoc::OnTestScene2)
	ON_COMMAND(ID_TEST_SCENE3, &GaiaDoc::OnTestScene3)
	ON_COMMAND(ID_TEST_SCENE4, &GaiaDoc::OnTestScene4)
	ON_COMMAND(ID_TEST_SCENE5, &GaiaDoc::OnTestScene5)
	ON_COMMAND(ID_TEST_SCENE6, &GaiaDoc::OnTestScene6)
	ON_COMMAND(ID_TEST_SCENE8, &GaiaDoc::OnTestScene8)
	ON_COMMAND(ID_TEST_SCENE9, &GaiaDoc::OnTestScene9)
	ON_COMMAND(ID_TEST_SCENE10, &GaiaDoc::OnTestScene10)
	ON_COMMAND(ID_TEST_SCENE11, &GaiaDoc::OnTestScene11)
	ON_COMMAND(ID_TEST_SCENE12, &GaiaDoc::OnTestScene12)
	ON_COMMAND(ID_TEST_SCENE13, &GaiaDoc::OnTestScene13)	
	ON_COMMAND(ID_TEST_SCENE15, &GaiaDoc::OnTestScene15)
	ON_COMMAND(ID_TEST_SCENE16, &GaiaDoc::OnTestScene16)
	ON_COMMAND(ID_TEST_SCENE17, &GaiaDoc::OnTestScene17)
	ON_COMMAND(ID_TEST_SCENE18, &GaiaDoc::OnTestScene18)
	ON_COMMAND(ID_TEST_SCENE19, &GaiaDoc::OnTestScene19)
	ON_COMMAND(ID_TEST_SCENE20, &GaiaDoc::OnTestScene20)
	ON_COMMAND(ID_TEST_SCENE21, &GaiaDoc::OnTestScene21)
	ON_COMMAND(ID_TEST_SCENE22, &GaiaDoc::OnTestScene22)	
	ON_COMMAND(ID_EXPORT_SELECTION, &GaiaDoc::OnExportSelection)
	ON_COMMAND(ID_IMPORT_CAMERA, &GaiaDoc::OnImportCamera)
	ON_COMMAND(ID_EXPORT_CAMERA, &GaiaDoc::OnExportCamera)
	ON_COMMAND(ID_ETRI2015, &GaiaDoc::OnEtri2015)
	ON_COMMAND(ID_IMPORT_GEODESIC_PATH, &GaiaDoc::OnImportGeodesicPath)
	ON_COMMAND(ID_EXPORT_GEODESIC_PATH, &GaiaDoc::OnExportGeodesicPath)
	ON_COMMAND(ID_IMPORT_LAYOUT, &GaiaDoc::OnImportLayout)
	ON_COMMAND(ID_EXPORT_LAYOUT, &GaiaDoc::OnExportLayout)
	ON_COMMAND(ID_IMPLANT_GUIDE, &GaiaDoc::OnImplantGuide)
	ON_COMMAND(ID_DENTAL_MODEL_1, &GaiaDoc::OnDentalModel1)
	ON_COMMAND(ID_DENTAL_MODEL_2, &GaiaDoc::OnDentalModel2)
	ON_COMMAND(ID_TEST_SCENE7_A, &GaiaDoc::OnTestScene7A)
	ON_COMMAND(ID_TEST_SCENE7_B, &GaiaDoc::OnTestScene7B)
	ON_COMMAND(ID_TEST_SCENE7_C, &GaiaDoc::OnTestScene7C)
	ON_COMMAND(ID_TEST_SCENE7_D, &GaiaDoc::OnTestScene7D)
END_MESSAGE_MAP()


// GaiaDoc ����/�Ҹ�
GaiaDoc::GaiaDoc()
{
	m_pClothesSkinDlg = NULL;
	m_pAvatar = NULL;
	m_pAvatarSkinMesh = NULL;
	m_pAvatarBoneMesh = NULL;
	m_pAvatarSkirtMesh = NULL;
	m_pAvatarClothesMesh = NULL;

	m_pBindRefPlnLeftShoulder = NULL;
	m_pBindRefPlnRightShoulder = NULL;

	// ����� ��Ź����
	m_pDentalCadDlg = NULL;
	m_pCurvatureDlg = NULL;

	// Bridge �޽� ������ ���� ���� �ʱ�ȭ
	m_pBridgeStMesh = NULL;
	m_pBridgeStVert = NULL;
	m_BridgeStConRad = 0.0;
	m_BridgeStAuxRad = 0.0;
	m_pBridgeEdMesh = NULL;
	m_pBridgeEdVert = NULL;
	m_BridgeEdConRad = 0.0;
	m_BridgeEdAuxRad = 0.0;

	m_pBridgeMeshDlg = NULL;
}

GaiaDoc::~GaiaDoc()
{
	if (m_pClothesSkinDlg != NULL)
		delete m_pClothesSkinDlg;

	if (m_pDentalCadDlg != NULL)
		delete m_pDentalCadDlg;

	if (m_pCurvatureDlg != NULL)
		delete m_pCurvatureDlg;

	if (m_pBridgeMeshDlg != NULL)
		delete m_pBridgeMeshDlg;
}

BOOL GaiaDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	static bool bInit = false;
	if (!bInit)
	{
		bInit = true;
		return TRUE;
	}

	GaiaView *pView = (GaiaView *)theMainFrm().GetActiveView();
	pView->InitView();

	theMainFrm().UpdateWorkSpaceDlg();
	UpdateAllViews(NULL);
	
	return TRUE;
}

// GaiaDoc serialization
void GaiaDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}

// GaiaDoc ����
#ifdef _DEBUG
void GaiaDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void GaiaDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


void GaiaDoc::PrintMessage(std::string msg)
{
	CString tmp;
	tmp.Format("%s\r\n", msg.c_str());
 	theMainFrm().m_OutputDlg.m_OutputMessage += tmp;
	int len = theMainFrm().m_OutputDlg.m_cOutputMessage.GetWindowTextLength();
	theMainFrm().m_OutputDlg.UpdateData(false);
	theMainFrm().m_OutputDlg.m_cOutputMessage.SetSel(len, len);
}

void GaiaDoc::OnFileOpen()
{
	// ������ �̸��� ������ ��ο� ���� �̸� ������ �����ϰ�,
	CString FilePath, FileName;

	// ������ Ȯ���� ���Ϳ� �ε����� �����ϰ�,
	LPCTSTR filter = _T("ASE file(*.ase)|*.ase||");
	int idxFilter = 0;

	// ���� ���� ��ȭ���ڸ� ����ϰ�,
	CFileDialog FileDlg(true, NULL, NULL, OFN_HIDEREADONLY, filter);
	if(FileDlg.DoModal() != IDOK) 
		return;

	// ����ڰ� ������ ������ ��θ�, ���� �̸� �� ���� �ε����� ���ϰ�,
	FilePath = FileDlg.GetPathName(); 
	FileName = FileDlg.GetFileName();
	idxFilter = FileDlg.m_ofn.nFilterIndex; 

	// ���� ������ ��θ� ���ͼ�, �۾� ���丮�� �����Ѵ�.
	std::string file_path = (CStringA)(FilePath);
	file_path = get_file_path(file_path);
	_chdir(file_path.c_str());

	AfxGetApp()->BeginWaitCursor();
	switch (idxFilter)
	{
	case 1: // GFF ���� �б�
		OnNewDocument();
		theScene().FileName = (CStringA)(FileName);
		::load_scene_from_ase_file(&theScene(), (CStringA)(FilePath));
		theMainFrm().UpdateWorkSpaceDlg();
		break;
	}
	AfxGetApp()->EndWaitCursor();

	UpdateAllViews(NULL);
}

void GaiaDoc::OnFileSave()
{
	// ������ �̸��� ������ ��ο� ���� �̸� ������ �����ϰ�,
	CString FilePath, FileName;

	// ������ Ȯ���� ���Ϳ� �ε����� �����ϰ�,
	LPCTSTR filter = _T("ASE file(*.ase)|*.ase||");
	int idxFilter = 0;

	// ���� ���� ��ȭ���ڸ� ����ϰ�,
	CFileDialog FileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, filter);
	if(FileDlg.DoModal() != IDOK) 
		return;

	// ����ڰ� ������ ������ ��θ�, ���� �̸� �� ���� �ε����� ���ϰ�,
	FilePath = FileDlg.GetPathName(); 
	FileName = FileDlg.GetFileName();
	idxFilter = FileDlg.m_ofn.nFilterIndex; 

	// ���� ������ ��θ� ���ͼ�, �۾� ���丮�� �����Ѵ�.
	std::string file_path = (CStringA)(FilePath);
	file_path = get_file_path(file_path);
	_chdir(file_path.c_str());
	
	AfxGetApp()->BeginWaitCursor();
	switch (idxFilter)
	{
	case 1: // ASE ���� ����
		if(FilePath.Find(".ase") == -1)
			FilePath += ".ase";
		::save_scene_as_ase_file(&theScene(), (CStringA)(FilePath));
		break;
	}
	AfxGetApp()->EndWaitCursor();

	UpdateAllViews(NULL);
}

void GaiaDoc::OnFileImport()
{
	// ������ �̸��� ������ ��ο� ���� �̸� ������ �����ϰ�,
	CString FilePath, FileName;

	// ������ Ȯ���� ���Ϳ� �ε����� �����ϰ�,
	LPCTSTR filter = _T("OBJ file(*.obj)|*.obj|ASE file(*.ase)|*.ase|TRI file(*.tri)|*.tri|avataDump file(*.Dump)|*.Dump|itemDump file(*.itemDump)|*.itemDump|Model file(*.m)|*.m||");
	int idxFilter = 0;

	// ���� ���� ��ȭ���ڸ� ����ϰ�,
	CFileDialog FileDlg(true, NULL, NULL, OFN_HIDEREADONLY, filter);
	if(FileDlg.DoModal() != IDOK) 
		return;

	// ����ڰ� ������ ������ ��θ�, ���� �̸� �� ���� �ε����� ���ϰ�,
	FilePath = FileDlg.GetPathName();	// ���� ��� �� �̸����� ����
	FileName = FileDlg.GetFileName();	// ���� �̸��� ����
	idxFilter = FileDlg.m_ofn.nFilterIndex; 

	// ���� ������ ��θ� ���ͼ�, �۾� ���丮�� �����Ѵ�.
	std::string file_path = (CStringA)(FilePath);
	file_path = get_file_path(file_path);
 	_chdir(file_path.c_str());

	AfxGetApp()->BeginWaitCursor();

	GaiaView *pView = (GaiaView *)theMainFrm().GetActiveView();
	::wglMakeCurrent(pView->m_hDC, pView->m_hRC);

	switch (idxFilter)
	{
	case 1: // OBJ ���� �б�
		::load_mesh_from_obj_file(&theScene(), (CStringA)(FilePath));
		break;

	case 2: // ASE ���� �б�
		::load_scene_from_ase_file(&theScene(), (CStringA)(FilePath));
		break;

	case 3: // TRI ���� �б�
		::load_mesh_from_tri_file(&theScene(), (CStringA)(FilePath));
		break;

	case 4: // ETRI
		::loadAvatarDump(&theScene(), (CStringA)(FilePath));
		break;

	case 5: // ETRI
		::loadItemDump(&theScene(), (CStringA)(FilePath));
		break;

	case 6: // M ���� �б�
		::load_mesh_from_m_file(&theScene(), (CStringA)(FilePath));
		break;
	}

	::wglMakeCurrent(NULL, NULL);
	AfxGetApp()->EndWaitCursor();

	theMainFrm().UpdateWorkSpaceDlg();
	UpdateAllViews(NULL);
}

void GaiaDoc::OnFileExport()
{
	// ������ �̸��� ������ ��ο� ���� �̸� ������ �����ϰ�,
	CString FilePath, FileName;

	// ������ Ȯ���� ���Ϳ� �ε����� �����ϰ�,
	LPCTSTR filter = _T("OBJ file(*.obj)|*.obj|ASE file(*.ase)|*.ase||");
	int idxFilter = 0;

	// ���� ���� ��ȭ���ڸ� ����ϰ�,
	CFileDialog FileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, filter);
	if(FileDlg.DoModal() != IDOK) 
		return;

	// ����ڰ� ������ ������ ��θ�, ���� �̸� �� ���� �ε����� ���ϰ�,
	FilePath = FileDlg.GetPathName(); // ���� ��� �� �̸����� ����
	FileName = FileDlg.GetFileName();	// ���� �̸��� ����
	idxFilter = FileDlg.m_ofn.nFilterIndex; 

	// ���� ������ ��θ� ���ͼ�, �۾� ���丮�� �����Ѵ�.
	std::string file_path = (CStringA)(FilePath);
	file_path = get_file_path(file_path);
	_chdir(file_path.c_str());

	AfxGetApp()->BeginWaitCursor();
	switch (idxFilter)
	{
	case 1: // OBJ ���� ����
		{
			GMesh *pMesh = theEditor().GetEditMesh();
			if (pMesh == NULL)
			{
				AfxMessageBox("������ �޽��� �����ϼ���!");
				break;
			}
			if(FilePath.Find(".obj") == -1)
				FilePath += ".obj";
			::save_mesh_as_obj_file(pMesh, (CStringA)(FilePath));
		}		
		break;

	case 2: // ASE ���� ����
		if(FilePath.Find(".ase") == -1)
			FilePath += ".ase";
		::save_scene_as_ase_file(&theScene(), (CStringA)(FilePath));
		break;	
	}
	AfxGetApp()->EndWaitCursor();

	UpdateAllViews(NULL);
}

void GaiaDoc::OnImportCamera()
{
	// �����̸��� ������ ������ �����ϰ�,
	CString FileName;

	// ���� ���͸� �����ϰ�,
	LPCTSTR filter = "cam file(*.cam)|*.cam||";

	// ���� ��ȭ���ڸ� �����,
	CFileDialog FileDlg(true, NULL, NULL, OFN_HIDEREADONLY, filter);
	if(FileDlg.DoModal() != IDOK) return;

	// ������ �н��� ������,
	FileName = FileDlg.GetPathName(); 

	// Ȯ���ڸ� �������� �ʾҴٸ�, Ȯ���ڸ� �ٿ��ְ�,
	if(FileName.Find(".cam") == -1)
		FileName += ".cam";

	// ���Ͽ� ����� ī�޶� ������ �ε��Ѵ�.
	char *fname = (LPSTR)((LPCTSTR)FileName);
	theScene().MainCamera.Import(fname);
}

void GaiaDoc::OnExportCamera()
{
	// �����̸��� ������ ������ �����ϰ�,
	CString FileName;

	// ���� ���͸� �����ϰ�,
	LPCTSTR filter = "cam file(*.cam)|*.cam||";

	// ���� ��ȭ���ڸ� �����,
	CFileDialog FileDlg(false, NULL, NULL, OFN_HIDEREADONLY, filter);
	if(FileDlg.DoModal() != IDOK) return;

	// ������ �н��� ������,
	FileName = FileDlg.GetPathName(); 

	// Ȯ���ڸ� �������� �ʾҴٸ�, Ȯ���ڸ� �ٿ��ְ�,
	if(FileName.Find(".cam") == -1)
		FileName += ".cam";

	// ����� ī�޶� ������ ���Ϸ� �����Ѵ�.
	char *fname = (LPSTR)((LPCTSTR)FileName);
	theScene().MainCamera.Export(fname);
}

void GaiaDoc::OnImportGeodesicPath()
{
	// �����̸��� ������ ������ �����ϰ�,
	CString FileName;

	// ���� ���͸� �����ϰ�,
	LPCTSTR filter = "Geodesic path file(*.path)|*.path||";

	// ���� ��ȭ���ڸ� �����,
	CFileDialog FileDlg(true, NULL, NULL, OFN_HIDEREADONLY, filter);
	if(FileDlg.DoModal() != IDOK) return;

	// ������ �н��� ������,
	FileName = FileDlg.GetPathName(); 

	// Ȯ���ڸ� �������� �ʾҴٸ�, Ȯ���ڸ� �ٿ��ְ�,
	if(FileName.Find(".path") == -1)
		FileName += ".path";

	// ���Ͽ� ����� ���� ��� ������ �ε��Ѵ�.
	char *fname = (LPSTR)((LPCTSTR)FileName);
	::load_geodesic_path(&theScene(), fname);

	theMainFrm().UpdateWorkSpaceDlg();
	UpdateAllViews(NULL);
}

void GaiaDoc::OnExportGeodesicPath()
{
	// �����̸��� ������ ������ �����ϰ�,
	CString FileName;

	// ���� ���͸� �����ϰ�,
	LPCTSTR filter = "Geodesic path file(*.path)|*.path||";

	// ���� ��ȭ���ڸ� �����,
	CFileDialog FileDlg(false, NULL, NULL, OFN_HIDEREADONLY, filter);
	if(FileDlg.DoModal() != IDOK) return;

	// ������ �н��� ������,
	FileName = FileDlg.GetPathName(); 

	// Ȯ���ڸ� �������� �ʾҴٸ�, Ȯ���ڸ� �ٿ��ְ�,
	if(FileName.Find(".path") == -1)
		FileName += ".path";

	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("������ ���� ��θ� �����ϼ���!");
		return;
	}
	if (pMesh->GetClassName() != "GGeoPath")
	{
		AfxMessageBox("������ ���� ��θ� �����ϼ���!");
		return;
	}

	// ����� ī�޶� ������ ���Ϸ� �����Ѵ�.
	char *fname = (LPSTR)((LPCTSTR)FileName);
	::save_geodesic_path((GGeoPath *)pMesh, fname);
}


void GaiaDoc::OnImportLayout()
{
	// �����̸��� ������ ������ �����ϰ�,
	CString FileName;

	// ���� ���͸� �����ϰ�,
	LPCTSTR filter = "Layout file(*.layout)|*.layout||";
	
	// ���� ��ȭ���ڸ� �����,
	CFileDialog FileDlg(true, NULL, NULL, OFN_HIDEREADONLY, filter);
	if(FileDlg.DoModal() != IDOK) return;

	// ������ �н��� ������,
	FileName = FileDlg.GetPathName(); 

	// Ȯ���ڸ� �������� �ʾҴٸ�, Ȯ���ڸ� �ٿ��ְ�,
	if(FileName.Find(".layout") == -1)
		FileName += ".layout";

	// ���̾ƿ� ������ �б���� �����ϰ�,
	char *fname = (LPSTR)((LPCTSTR)FileName);
	FILE *fp;
	fopen_s(&fp, fname, "r");
	if (fp == NULL) return;

	// �ε��� �ʿ��� �ӽ� �������� �����Ѵ�.
	char tag[256];
	char name[256];
	char reference[256];
	GGeoPath *pPath = NULL;

	// ������ ������ �� �ܾ �����鼭 tag �迭�� �����Ѵ�.
	while (fscanf(fp, "%s", tag) != EOF)
	{
		// "name" �±׶��,
		if (!strcmp(tag, "name"))
		{
			// ���� ����� �̸��� ���´�.
			fscanf(fp, "%s", name);
		}

		// "reference" �±׶��,
		if (!strcmp(tag, "reference"))
		{
			// ���� �޽��� ���´�.
			fscanf(fp, "%s", reference);

			// ���� �޽��� �����͸� ���Ͽ�,
			GMesh *pRefMesh = FindMesh(theScene().MeshList, reference);

			// ���� �̸��� �����ϰ�,
			std::string Name = create_mesh_name("GeoPath");

			// ���� ��θ� �����Ͽ� ��鿡 �߰��Ѵ�.
			pPath = new GGeoPath(Name, GTransf(), pRefMesh, reference);
			theScene().AddMesh(pPath);
		}

		// "vidx" �±׶��,
		if (!strcmp(tag, "vidx"))
		{
			int vidx;
			fscanf(fp, "%d", &vidx);
			pPath->AddPath(vidx);
		}
	}

	// ������ �ݴ´�.
	fclose(fp);

	// �۾�â�� 3���� ����� �����Ѵ�.
	theMainFrm().UpdateWorkSpaceDlg();
	UpdateAllViews(NULL);
}

void GaiaDoc::OnExportLayout()
{
	// ���� �̸��� ������ ������ �����ϰ�,
	CString FileName;

	// ���� ���͸� �����ϰ�,
	LPCTSTR filter = "Layout file(*.layout)|*.layout||";

	// ���� ��ȭ���ڸ� ����ϰ�,
	CFileDialog FileDlg(false, NULL, NULL, OFN_HIDEREADONLY, filter);
	if(FileDlg.DoModal() != IDOK) return;

	// ������ �н��� ������,
	FileName = FileDlg.GetPathName(); 

	// Ȯ���ڸ� �������� �ʾҴٸ�, Ȯ���ڸ� �ٿ��ְ�,
	if(FileName.Find(".layout") == -1)
		FileName += ".layout";

	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh == NULL)
	{
		AfxMessageBox("������ ���� ��θ� �����ϼ���!");
		return;
	}
	if (pMesh->GetClassName() != "GGeoPath")
	{
		AfxMessageBox("������ ���� ��θ� �����ϼ���!");
		return;
	}

	// ������ �����ϰ�,
	char *fname = (LPSTR)((LPCTSTR)FileName);
	FILE *fp;
	fopen_s(&fp, fname, "w");
	if (fp == NULL) return;

	// ���õ� ���� ����� ������ ���Ͽ�,
	int NumPath = theEditor().GetNumEditObj();
	for (int i = 0; i < NumPath; ++i)
	{
		// ���õ� ��ü�� ���� ��ΰ� �ƴϸ� �ǳʶ��,		
		if (theEditor().EditObjList[i]->GetClassName() != "GGeoPath")
			continue;

		// ������ ���� ��θ� �ϳ��� layout ���Ͽ� �����Ѵ�.
		GGeoPath *pPath = (GGeoPath *)(theEditor().EditObjList[i]);
		
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
	}

	// ������ �ݴ´�.
	fclose(fp);
}

void GaiaDoc::OnDeleteMesh()
{
	// ���õ� ��ü�� �����,
	theEditor().DeleteObject();

	// �޽� ��ü�� ���� ���ٸ�,
	if (theEditor().EditObjList.empty())
	{
		// WorkSpace�� ������ �����Ѵ�.
		theMainFrm().UpdateWorkSpaceDlg();
	}
	// �����̳� �ﰢ���� �������ٸ�, 
	else
	{
		// ���� ��ü�� �����Ѵ�.
		theMainFrm().m_WorkSpaceDlg.UpdateSelect();
	}	
	
	UpdateAllViews(NULL);
}

void GaiaDoc::OnShowMesh()
{
	theEditor().ShowMesh();
	UpdateAllViews(NULL);
}

void GaiaDoc::OnUpdateShowMesh(CCmdUI *pCmdUI)
{
	if (theEditor().EditObjList.empty())
		pCmdUI->SetCheck(FALSE);
	else
	{
		GMesh *pRefMesh = theEditor().EditObjList[0]->GetRefMesh();
		pCmdUI->SetCheck(pRefMesh->Renderer.IsVisibleMesh());
	}
}

void GaiaDoc::OnHideMesh()
{
	theEditor().HideMesh();
	UpdateAllViews(NULL);
}

void GaiaDoc::OnUpdateHideMesh(CCmdUI *pCmdUI)
{
	if (theEditor().EditObjList.empty())
		pCmdUI->SetCheck(FALSE);
	else
	{
		GMesh *pRefMesh = theEditor().EditObjList[0]->GetRefMesh();
		pCmdUI->SetCheck(!pRefMesh->Renderer.IsVisibleMesh());
	}
}

void GaiaDoc::OnTestScene0()
{
	// �ﰢ�� ������ ���� �׽�Ʈ
	OnNewDocument();
	theScene().FileName = "scene0.gff";

	int TestId = 1;
	switch (TestId)
	{
	case 0:
		{
			std::string fname1 = WorkingDirectory + "manifold-mesh\\local-patch.obj";
			::load_mesh_from_obj_file(&theScene(), fname1.c_str());
			GMesh *pCtrlMesh = theScene().MeshList[0];
			pCtrlMesh->Renderer.RenderType = RENDER_WIRE;

			GBlendMesh *pSrf = new GBlendMesh("BlendSrf",pCtrlMesh->MC, pCtrlMesh);
			pSrf->Renderer.RenderType = RENDER_POLYGON | RENDER_WIRE;
			pSrf->Renderer.CtrlPtSize = 0.01;
			pSrf->Renderer.SetSampleNum(5, 5);
			theScene().AddMesh(pSrf);
		}
		break;

	case 1:
		{
			GMesh *pCtrlMesh = new GMesh("Ctrl Mesh", GTransf(), FACE_TRI);
			pCtrlMesh->AddVertex(GVertex(GPoint3(5.0, 0.0, 0.0), GVector3()));
			pCtrlMesh->AddVertex(GVertex(GPoint3(0.0, 5.0, 0.0), GVector3()));
			pCtrlMesh->AddVertex(GVertex(GPoint3(-5.0, 0.0, 0.0), GVector3()));
			pCtrlMesh->AddVertex(GVertex(GPoint3(0.0, -5.0, 0.0), GVector3()));
			
			pCtrlMesh->AddFace(GFace(0, 1, 2));
			pCtrlMesh->AddFace(GFace(0, 2, 3));
			theScene().AddMesh(pCtrlMesh);

			GBlendMesh *pSrf = new GBlendMesh("BlendSrf", pCtrlMesh->MC, pCtrlMesh);
			pSrf->Renderer.RenderType = RENDER_POLYGON | RENDER_WIRE;
			pSrf->Renderer.CtrlPtSize = 0.01;
			pSrf->Renderer.SetSampleNum(5, 5);
			theScene().AddMesh(pSrf);
		}
		break;

	case 2:
		{
			GMesh *pCtrlMesh = new GMesh("Ctrl Mesh", GTransf(), FACE_TRI);
			pCtrlMesh->AddVertex(GVertex(GPoint3(0, 0, 1), GVector3()));
			pCtrlMesh->AddVertex(GVertex(GPoint3(0.943, 0.0, -0.333), GVector3()));
			pCtrlMesh->AddVertex(GVertex(GPoint3(-0.471, 0.816, -0.333), GVector3()));
			pCtrlMesh->AddVertex(GVertex(GPoint3(-0.471, -0.816, -0.333), GVector3()));

			pCtrlMesh->AddFace(GFace(0, 1, 2));
			pCtrlMesh->AddFace(GFace(0, 2, 3));
			pCtrlMesh->AddFace(GFace(0, 3, 1));
			pCtrlMesh->AddFace(GFace(1, 3, 2));
			theScene().AddMesh(pCtrlMesh);

			GBlendMesh *pSrf = new GBlendMesh("BlendSrf", pCtrlMesh->MC, pCtrlMesh);
			pSrf->Renderer.RenderType = RENDER_POLYGON | RENDER_WIRE;
			pSrf->Renderer.CtrlPtSize = 0.01;
			pSrf->Renderer.SetSampleNum(5, 5);
			theScene().AddMesh(pSrf);
		}
		break;
	}
 
	theMainFrm().UpdateWorkSpaceDlg();
}

void GaiaDoc::OnTestScene1()
{
	// B-spline � �׽�Ʈ
 	OnNewDocument();
 	theScene().FileName = "Scene1.gff";

	// �����
	GVector4 P[12];
	P[0].Set(-10.0, 2.0, 0.0, 1.0);
	P[1].Set(-8.0, 3.0, 0.0, 1.0);
	P[2].Set(-6.0, 1.0, 0.0, 1.0);
	P[3].Set(-4.0, 0.0, 0.0, 1.0);
	P[4].Set(-2.0, -2.0, 0.0, 1.0);
	P[5].Set(0.0, 1.0, 0.0, 1.0);
	P[6].Set(2.0, 3.0, 0.0, 1.0);
	P[7].Set(4.0, 0.0, 0.0, 1.0);
	P[8].Set(6.0, -4.0, 0.0, 1.0);
	P[9].Set(8.0, 2.0, 0.0, 1.0);
	P[10].Set(10.0, 0.0, 0.0, 1.0);
	P[11].Set(12.0, 2.0, 0.0, 1.0);

	double *U = ::get_knot_uniform_clamped(2, 11, 0.0, 1.0);
	GNurbsCrv *pCrv = new GNurbsCrv("test_crv", GTransf(), 2, 11, U, P, false, "");
	pCrv->Renderer.RenderType = RENDER_POLYGON | RENDER_CTRL_PT;
	pCrv->Renderer.SetSampleNum(20, 100);
	pCrv->Renderer.CtrlPtSize = 0.1;
	theScene().AddMesh(pCrv);
	delete [] U;

	theMainFrm().UpdateWorkSpaceDlg();
}

void GaiaDoc::OnTestScene2()
{
	// B-spline ��� �׽�Ʈ
	OnNewDocument();
	theScene().FileName = "Scene2.gff";

	// ������
	GVector4 P[4][4];
	P[0][0].Set(-3.0, -3.0, 0.0, 1.0);	P[0][1].Set(-3.0, -1.0, 0.0, 1.0);	P[0][2].Set(-3.0, 1.0, 0.0, 1.0);	P[0][3].Set(-3.0, 3.0, 0.0, 1.0);
	P[1][0].Set(-1.0, -3.0, 0.0, 1.0);	P[1][1].Set(-1.0, -1.0, 5.0, 5.0);	P[1][2].Set(-1.0, 1.0, 5.0, 1.0);	P[1][3].Set(-1.0, 3.0, 0.0, 1.0);
	P[2][0].Set(1.0, -3.0, 0.0, 1.0);	P[2][1].Set(1.0, -1.0, 5.0, 1.0);	P[2][2].Set(1.0, 1.0, 5.0, 1.0);	P[2][3].Set(1.0, 3.0, 0.0, 1.0);
	P[3][0].Set(3.0, -3.0, 0.0, 1.0);	P[3][1].Set(3.0, -1.0, 0.0, 1.0);	P[3][2].Set(3.0, 1.0, 0.0, 1.0);	P[3][3].Set(3.0, 3.0, 0.0, 1.0);
	double *U = ::get_knot_uniform_clamped(3, 3, 0.0, 1.0);
	double *V = ::get_knot_uniform_clamped(3, 3, 0.0, 1.0);
	
	GNurbsSrf *pSrf = new GNurbsSrf("test_srf", GTransf(), 3, 3, U, 3, 3, V, &P[0][0], false);
	pSrf->Renderer.SetSampleNum(50, 50);
	pSrf->Renderer.CtrlPtSize = 0.1;
	theScene().AddMesh(pSrf);
	
	// �޽�����
	delete [] U;
	delete [] V;

	theMainFrm().UpdateWorkSpaceDlg();
}

void GaiaDoc::OnTestScene3()
{
	// �޽� Ȧ ���� �׽�Ʈ
	OnNewDocument();
	theScene().FileName = "Scene3.gff";

	// ������
	GVector4 P[12];
	P[0].Set(-5.0, 5.0, 0.0, 0.0);
	P[1].Set(-5.0, 1.0, 5.0, 0.0);
	P[2].Set(-5.0, -1.0, -5.0, 0.0);
	P[3].Set(-5.0, -5.0, 0.0, 0.0);
	P[4].Set(-1.0, -5.0, 0.0, 0.0);
	P[5].Set(1.0, -5.0, 3.0, 0.0);
	P[6].Set(5.0, -5.0, 0.0, 0.0);
	P[7].Set(5.0, -1.0, -3.0, 0.0);
	P[8].Set(5.0, 1.0, 0.0, 0.0);
	P[9].Set(5.0, 5.0, 5.0, 0.0);
	P[10].Set(1.0, 5.0, 0.0, 0.0);
	P[11].Set(-1.0, 5.0, -5.0, 0.0);

	double *U = ::get_knot_uniform_clamped(3, 3, 0.0, 1.0);
	double *V = ::get_knot_uniform_clamped(2, 3, 0.0, 1.0);
	GCoonsSrf *pSrf = new GCoonsSrf("Coons Patch", GTransf(), 3, 3, U, 2, 3, V, P, "");
	pSrf->Renderer.SetSampleNum(50, 50);
	pSrf->Renderer.RenderType = RENDER_POLYGON | RENDER_CTRL_PT;
	theScene().AddMesh(pSrf);
	
	delete [] U;
	delete [] V;
	theMainFrm().UpdateWorkSpaceDlg();
}

void GaiaDoc::OnTestScene4()
{
	// �޽� Ȧ ���� �׽�Ʈ
	OnNewDocument();
	theScene().FileName = "Scene4.gff";

	GMesh *pMesh = new GMesh("hole_mesh", GTransf(), FACE_TRI);
	pMesh->AddVertex(GVertex(GPoint3(0, 0, 0)));
	pMesh->AddVertex(GVertex(GPoint3(5, 0, 0)));
	pMesh->AddVertex(GVertex(GPoint3(2, 5, 0)));
	pMesh->AddVertex(GVertex(GPoint3(0, 10, 0)));
	pMesh->AddVertex(GVertex(GPoint3(-2, 5, 0)));
	pMesh->AddVertex(GVertex(GPoint3(-5, 0, 0)));
	
	pMesh->AddFace(GFace(0, 1, 2));
	pMesh->AddFace(GFace(2, 3, 4));
	pMesh->AddFace(GFace(0, 4, 5));

	theScene().AddMesh(pMesh);
	pMesh->Renderer.RenderType |= RENDER_BOUNDARY;
	
	theMainFrm().UpdateWorkSpaceDlg();
}

void GaiaDoc::OnTestScene5()
{
}

void GaiaDoc::OnTestScene6()
{
	OnNewDocument();
	m_MinDistPt1.Set(0, 0, 0);
	m_MinDistPt2.Set(0, 0, 0);

	std::string st_mesh_name = "C:\\Data\\Research\\GaiaData\\Bunny-obj\\start_bunny.obj";
	std::string ed_mesh_name = "C:\\Data\\Research\\GaiaData\\Bunny-obj\\end_bunny.obj";

	::load_mesh_from_obj_file(&theScene(), st_mesh_name.c_str());
	m_pPQPMesh1 = theScene().pRecentMesh;
	m_pPQPMesh1->CreateBVH();


	::load_mesh_from_obj_file(&theScene(), ed_mesh_name.c_str());
	m_pPQPMesh2 = theScene().pRecentMesh;
	m_pPQPMesh2->CreateBVH();

	PQP_DistanceResult dres;
	PQP_REAL R1[3][3];
	PQP_REAL T1[3];
	PQP_REAL R2[3][3];
	PQP_REAL T2[3];

	GMatrix M1 = cast_mat(m_pPQPMesh1->MC, false);
	R1[0][0] = M1[0][0];	R1[0][1] = M1[0][1];	R1[0][2] = M1[0][2];	T1[0] = M1[0][3];
	R1[1][0] = M1[1][0];	R1[1][1] = M1[1][1];	R1[1][2] = M1[1][2];	T1[1] = M1[1][3];
	R1[2][0] = M1[2][0];	R1[2][1] = M1[2][1];	R1[2][2] = M1[2][2];	T1[2] = M1[2][3];

	GMatrix M2 = cast_mat(m_pPQPMesh2->MC, false);
	R2[0][0] = M2[0][0];	R2[0][1] = M2[0][1];	R2[0][2] = M2[0][2];	T2[0] = M2[0][3];
	R2[1][0] = M2[1][0];	R2[1][1] = M2[1][1];	R2[1][2] = M2[1][2];	T2[1] = M2[1][3];
	R2[2][0] = M2[2][0];	R2[2][1] = M2[2][1];	R2[2][2] = M2[2][2];	T2[2] = M2[2][3];

	PQP_Distance(&dres, R1, T1, m_pPQPMesh1->pBVH, R2, T2, m_pPQPMesh2->pBVH, 0.0, 0.0);
	m_MinDistPt1 = GPoint3(dres.p1[0], dres.p1[1], dres.p1[2]);
	m_MinDistPt2 = GPoint3(dres.p2[0], dres.p2[1], dres.p2[2]);

	theMainFrm().UpdateWorkSpaceDlg();
	UpdateAllViews(NULL);
}

void GaiaDoc::OnTestScene7A()
{
	OnNewDocument();

	int place = 0; // 0: �б�, 1:��
	std::string st_mesh_name;
	std::string ed_mesh_name;
	if (place == 0)
	{
		st_mesh_name = "D:\\Data\\Research\\GaiaData\\Bunny-obj\\start_bunny.obj";
		ed_mesh_name = "D:\\Data\\Research\\GaiaData\\Bunny-obj\\end_bunny.obj";
	}
	else
	{
		st_mesh_name = "C:\\Data\\Research\\GaiaData\\Bunny-obj\\start_bunny.obj";
		ed_mesh_name = "C:\\Data\\Research\\GaiaData\\Bunny-obj\\end_bunny.obj";
	}

	::load_mesh_from_obj_file(&theScene(), st_mesh_name.c_str());
	GMesh *pStMesh = theScene().pRecentMesh;
	::load_mesh_from_obj_file(&theScene(), ed_mesh_name.c_str());
	GMesh *pEdMesh = theScene().pRecentMesh;

	int snum_u = 100;
	int snum_v = 30;

	GNurbsCrv *pStContCrv = ::get_gnurbs_approx_circle(3.0, 0, true);		
	GNurbsCrv *pStAuxCrv = ::get_gnurbs_approx_circle(1.0, 0, true);
	pStContCrv->CreateMesh();
	pStAuxCrv->CreateMesh();
	pStContCrv->KnotRefine();
	pStContCrv->KnotRefine();
	pStContCrv->Edit(0.1, GVector4(1.0, 1.0, 0.0, 0.0));
	pStContCrv->Edit(0.3, GVector4(1.0, 0.0, 0.0, 0.0));
	pStContCrv->Edit(0.5, GVector4(0.0, 1.0, 0.0, 0.0));
	pStContCrv->Edit(0.7, GVector4(-1.0, 1.0, 0.0, 0.0));
	
	GNurbsCrv *pEdContCrv = ::get_gnurbs_approx_circle(3.0, 0, false);
	GNurbsCrv *pEdAuxCrv = ::get_gnurbs_approx_circle(1.0, 0, false);
	pEdContCrv->CreateMesh();
	pEdAuxCrv->CreateMesh();
	pEdContCrv->Edit(0.3, GVector4(-1.0, -1.0, 0.0, 0.0));

	GMesh *pStSubMesh = create_mesh_from_two_curves_on_mesh(pStMesh, &(pStMesh->Verts[532]), pStContCrv, pStAuxCrv, snum_u, snum_v, false);
	GMesh *pEdSubMesh = create_mesh_from_two_curves_on_mesh(pEdMesh, &(pStMesh->Verts[540]), pEdContCrv, pEdAuxCrv, snum_u, snum_v, false);
	pStSubMesh->Name = create_mesh_name("test");
	pEdSubMesh->Name = create_mesh_name("test");
	theScene().AddMesh(pStSubMesh);
	theScene().AddMesh(pEdSubMesh);
	
	delete pStContCrv;
	delete pStAuxCrv;
	delete pEdContCrv;
	delete pEdAuxCrv;

	theMainFrm().UpdateWorkSpaceDlg();
	UpdateAllViews(NULL);
}

void GaiaDoc::OnTestScene7B()
{
	OnNewDocument();
	int place = 0; // 0: �б�, 1:��
	std::string st_mesh_name;
	std::string ed_mesh_name;
	if (place == 0)
	{
		st_mesh_name = "D:\\Data\\Research\\GaiaData\\Bunny-obj\\start_bunny.obj";
		ed_mesh_name = "D:\\Data\\Research\\GaiaData\\Bunny-obj\\end_bunny.obj";
	}
	else
	{
		st_mesh_name = "C:\\Data\\Research\\GaiaData\\Bunny-obj\\start_bunny.obj";
		ed_mesh_name = "C:\\Data\\Research\\GaiaData\\Bunny-obj\\end_bunny.obj";
	}

	::load_mesh_from_obj_file(&theScene(), st_mesh_name.c_str());
	GMesh *pStMesh = theScene().pRecentMesh;
	::load_mesh_from_obj_file(&theScene(), ed_mesh_name.c_str());
	GMesh *pEdMesh = theScene().pRecentMesh;

	GNurbsCrv *pStContCrv = ::get_gnurbs_approx_circle(2.0, 0, true);
	GNurbsCrv *pEdContCrv = ::get_gnurbs_approx_circle(2.0, 0, true);

	pStContCrv->KnotRefine();
	pStContCrv->KnotRefine();
	pStContCrv->Edit(0.0, GVector4(0.0, 0.5, 0.0));
	pStContCrv->Edit(0.5, GVector4(0.0, -0.5, 0.0));
	
	GMesh *pBridgeMesh = ::create_mesh_bridge(
		pStMesh, &(pStMesh->Verts[532]), pStContCrv, 0.5, 
		pEdMesh, &(pEdMesh->Verts[540]), pEdContCrv, 0.5,
		0.5, 100, 30, true);

	pBridgeMesh->Name = ::create_mesh_name("Bridge Mesh");
	theScene().AddMesh(pBridgeMesh);

	delete pStContCrv;
	delete pEdContCrv;

	theMainFrm().UpdateWorkSpaceDlg();
	UpdateAllViews(NULL);
}

void GaiaDoc::OnTestScene7C()
{
	OnNewDocument();

	int place = 0; // 0: �б�, 1:��
	std::string st_mesh_name;
	std::string ed_mesh_name;
	if (place == 0)
	{
		st_mesh_name = "D:\\Data\\Research\\������\\MeshBlending\\Data\\tooth3.obj";
		ed_mesh_name = "D:\\Data\\Research\\������\\MeshBlending\\Data\\tooth5.obj";
	}
	else
	{
		st_mesh_name = "C:\\Data\\Research\\������\\MeshBlending\\Data\\tooth3.obj";
		ed_mesh_name = "C:\\Data\\Research\\������\\MeshBlending\\Data\\tooth5.obj";
	}
	

	::load_mesh_from_obj_file(&theScene(), st_mesh_name.c_str());
	GMesh *pStMesh = theScene().pRecentMesh;
	::load_mesh_from_obj_file(&theScene(), ed_mesh_name.c_str());
	GMesh *pEdMesh = theScene().pRecentMesh;

	GNurbsCrv *pStContCrv = ::get_gnurbs_approx_circle(3.0, 0, true);		
	GNurbsCrv *pEdContCrv = ::get_gnurbs_approx_circle(3.0, 0, true);
	//GNurbsCrv *pEdContCrv = ::get_gnurbs_rect(5.0, 3.0, 0);

	GMesh *pBridgeMesh = ::create_mesh_bridge(
		pStMesh, &(pStMesh->Verts[1007]), pStContCrv, 0.1, 
		pEdMesh, &(pEdMesh->Verts[2075]), pEdContCrv, 0.1,
		0.5, 100, 30, true);

	pBridgeMesh->Name = ::create_mesh_name("Bridge Mesh");
	theScene().AddMesh(pBridgeMesh);

	delete pStContCrv;
	delete pEdContCrv;
	
	theEditor().Reset();
	theMainFrm().UpdateWorkSpaceDlg();
	UpdateAllViews(NULL);
}

void GaiaDoc::OnTestScene7D()
{
	OnNewDocument();

	GNurbsCrv *pCrv0 = get_gnurbs_approx_circle(3.0, 0, 0);
	GNurbsCrv *pCrv1 = get_gnurbs_approx_circle(5.0, 0, 0);
	pCrv1->Reverse();

	pCrv0->Name = "Forward Crv";
	pCrv1->Name = "Reverse Crv";
	theScene().AddMesh(pCrv0);
	theScene().AddMesh(pCrv1);

	theMainFrm().UpdateWorkSpaceDlg();
	UpdateAllViews(NULL);
}

void GaiaDoc::OnTestScene8()
{
	GMesh *pMesh = theEditor().GetEditMesh();
	if (pMesh->GetClassName() != "GSweepSrf")
		return;

	GSweepSrf *pSrf = (GSweepSrf *)pMesh;

	int snum_u = 100;
	int snum_v = 25;
	
	GMesh *pBridgeMesh = ::create_mesh_bridge(
		m_pBridgeStMesh, m_pBridgeStVert, m_BridgeStConRad, m_BridgeStAuxRad, 
		pSrf, 0.5, 0.0, 
		snum_u, snum_v);

	pBridgeMesh->Name = ::create_mesh_name("Bridge Mesh");
	theScene().AddMesh(pBridgeMesh);

	theMainFrm().UpdateWorkSpaceDlg();
	UpdateAllViews(NULL);
}

void GaiaDoc::OnTestScene9()
{
	// ASE �δ� �׽�Ʈ
	OnNewDocument();

	// ���ҷ�����
	std::string fname = "C:\\Data\\Research\\GaiaData\\ase\\gorilla.ase";
	theScene().FileName = fname;

	::load_scene_from_ase_file(&theScene(), fname.c_str());
	theScene().MeshList[0]->Renderer.bTexture = true;
	theMainFrm().UpdateWorkSpaceDlg();
}


void GaiaDoc::OnTestScene10()
{
	OnNewDocument();
	GMatrix T(3, 3);
	T[0][0] = 1;	T[0][1] = 1;	T[0][2] = 2.0;	
	T[1][0] = 0;	T[1][1] = -1;	T[1][2] = -1.0;	
	T[2][0] = 0;	T[2][1] = 1;	T[2][2] = 0;	

	GVector v0, v1, v2, v3;
	v0.Set(1, -1, 1);
	v1.Set(2.0, -0.5, -0.5);
	v2.Set(1, 0, 0);
	v3.Set(1.5, 0.0, -0.5);

	std::cout << inv(T) << std::endl;
// 	std::cout << inv(T) * v0 << std::endl << std::endl;
// 	std::cout << inv(T) * v1<< std::endl << std::endl;
// 	std::cout << inv(T) * v2 << std::endl << std::endl;
// 	std::cout << inv(T) * v3 << std::endl << std::endl;

	// �����
// 	GVector4 Q[5];
//  Q[0].Set(-2.5,   5.0, -10.0, 1.0);
//  Q[1].Set(-7.5,   0.0, -5.0, 1.0);
//  Q[2].Set(   0,  -5.0,   0.0, 1.0);
//  Q[3].Set( 7.5,   0.0,  5.0, 1.0);
// 	Q[4].Set( 2.5,   5.0,  10.0, 1.0);

//  Q[0].Set(-2.0, 0.0, 0, 1.0);
//  Q[1].Set(-1.0, 1.0, 0, 1.0);
//  Q[2].Set(0.0, 1.0, 0, 1.0);
//  Q[3].Set(1.0, 0.0, 0.3, 1.0);
//  Q[4].Set(2.0, 0.0, 0.5, 1.0);

// 	double *UU = ::get_knot_uniform_clamped(3, 4, 0.0, 1.0);
// 	GNurbsCrv *pCrv = new GNurbsCrv(3, 4, UU, Q, true);

// 	GVector3 p1(0.0, 0.0, 0.0), n1(0.0, 1.0, -1.0), p2(5.0, 1.0, 5.0), n2(0.0, 1.0, 1.0);
// 
// 	GNurbsCrv *pCrv = ::get_gnurbs_crv_inter(p1, n1, p2, n2);
// 	GDevelSrf *pSrf = new GDevelSrf(*pCrv, 5);
// 
// 	// ����� ������ �޽�����
// 	GMesh *pMesh0 = new GMesh("Developable Srf", FACE_TRI, pSrf);
// 	pMesh0->Renderer.NumSampU = 3;
// 	pMesh0->Renderer.NumSampV = 100;
// 	pMesh0->Renderer.bShow = false;
// 	pMesh0->Renderer.bTexture = true;
// 	pMesh0->Renderer.CtrlPtSize = 0.05;
// 	pMesh0->Renderer.wire_frame = true;
// 	pMesh0->GenerateMeshFromNurbs();
// 	pMesh0->Renderer.NameMtrl = "mat01";
// 	delete pSrf;
// 
// 	GMesh *pMesh1 = new GMesh("Geodescic", FACE_TRI, pCrv);
// 	pMesh1->Renderer.NumSampU = 30;
// 	pMesh1->Renderer.NumSampV = 30;
// 	pMesh1->Renderer.shade_flat = false;
// 	pMesh1->Renderer.bShowCtrlPt = true;
// 	pMesh1->Renderer.CtrlPtSize = 0.1;
// 	pMesh1->Renderer.wire_frame = false;
// 	pMesh1->Renderer.bTransparent = false;
// 	pMesh1->GenerateMeshFromNurbs();
// 	pMesh1->Renderer.NameMtrl = "mat01";
// 
// 	//delete [] UU;
// 	delete pCrv;
// 
// 	// ���� ����
// 	GLight light0(GL_LIGHT0);
// 	light0.SetPosition(0, 0, 10, 1);
// 	light0.SetAmbient(0.5, 0.5, 0.5);
// 	light0.SetDiffuse(0.5, 0.5, 0.5);
// 	light0.SetSpecular(1.0, 1.0, 1.0); 
// 
// 	GMaterial Mtrl("mat01");
// 	Mtrl.SetDiffuse(0.8, 0.8, 0.8, 0.5);
// 	Mtrl.SetAmbient(0.8, 0.8, 0.8, 0.5);
// 	Mtrl.LoadTexture("blackwhite.tga");
// 
// 	theScene().AddMesh(pMesh0);
// 	theScene().AddMesh(pMesh1);
// 	theScene().AddMaterial(Mtrl);
// 	theMainFrm().UpdateWorkSpaceDlg();
}

void GaiaDoc::OnTestScene11()
{
	OnNewDocument();

// 	GMaterial Mtrl("mat01");
// 	Mtrl.SetDiffuse(0.5, 0.5, 0.5, 0.5);
// 	Mtrl.SetAmbient(0.5, 0.5, 0.5, 0.5);
// 	Mtrl.LoadTexture("wood-3.tga");
// 	theScene().AddMaterial(Mtrl);

	// ���ҷ�����
	char path[256];
	_getcwd(path, 256);
	std::string file_name(path);
	file_name += "\\obj\\buddha.obj";
	theScene().FileName = file_name;


//	::load_mesh_from_obj_file(&theScene(), "obj\\gargoyle.obj");
//	::load_mesh_from_obj_file(&theScene(), "obj\\chinese_lion.obj");
	::load_mesh_from_obj_file(&theScene(), "obj\\buddha.obj");

	theMainFrm().UpdateWorkSpaceDlg();
}

void GaiaDoc::OnTestScene12()
{
	//double U[12] = {0.0, 0.0, 0.0, 0.0, 0.3, 0.5, 0.5, 0.7, 1.0, 1.0, 1.0, 1.0};
	double U0[12] = {0.0, 1.0, 2.0, 4.0, 5.0, 7.0, 8.0, 10.0, 10.0, 11.0, 12.0, 12.0};
	double U1[12] = {3.0, 4.0, 4.0, 5.0, 5.0, 6.0, 7.0, 7.0, 10.0, 10.0, 12.0, 12.0};


	::transform_knot(U0, 11, 0.0, 1.0);
	::transform_knot(U1, 11, 0.0, 1.0);
	
	int k = 0;
	int m = 0;
	double *U = ::get_knot_merged_distinct(U0, 11, U1, 11, &m);

	k = 0;
// 	OnNewDocument();
// 
// 	// ���� ���� �� �߰�
// 	GMaterial Mtrl("mat01");
// 	Mtrl.SetDiffuse(0.5, 0.5, 0.5, 0.5);
// 	Mtrl.SetAmbient(0.5, 0.5, 0.5, 0.5);
// 	Mtrl.LoadTexture("blackwhite.tga");
// 	theScene().AddMaterial(Mtrl);
// 
// 	// ������
// 	GVector4 P[2][2];
// 	//P[0][0].Set(-5.0, 2.0, 0.0, 1.0);	P[0][1].Set(-5.0, 15.0, 0.0, 1.0);
// 	//P[1][0].Set(5.0, 2.0, 0.0, 1.0);	P[1][1].Set(5.0, 15.0, 0.0, 1.0);
// 	P[0][0].Set(-5.0, 3.0, 0.0, 1.0);	P[0][1].Set(-5.0, 10.0, 0.0, 1.0);
// 	P[1][0].Set(5.0, 3.0, 0.0, 1.0);	P[1][1].Set(5.0, 10.0, 0.0, 1.0);
// 
// 	double *U = ::get_knot_uniform_clamped(1, 1, 0.0, 1.0);
// 	double *V = ::get_knot_uniform_clamped(1, 1, 0.0, 1.0);
// 	GNurbsObj *pSrf = new GNurbsSrf(1, 1, U, 1, 1, V, &P[0][0], false);
// 	delete [] U;
// 	delete [] V;
// 
// 	// ����� ������ �޽�����
// 	GMesh *pMesh0 = new GMesh("mesh_0", FACE_TRI, pSrf);
// 	pMesh0->Renderer.NumSampU = 100;
// 	pMesh0->Renderer.NumSampV = 100;
// 	pMesh0->Renderer.shade_flat = false;
// 	pMesh0->Renderer.bShowCtrlPt = true;
// 	pMesh0->Renderer.CtrlPtSize = 0.1;
// 	pMesh0->Renderer.wire_frame = false;
// 	pMesh0->Renderer.wire_frame = false;
// 	pMesh0->Renderer.bBackFaceCull = false;
// 	pMesh0->Renderer.bTransparent = false;
// 	pMesh0->GenerateMeshFromNurbs();
// 	pMesh0->Renderer.NameMtrl = "mat01";
// 	delete pSrf;
// 
// 	// ������ �̿��� ���� ��
// 	static double theta = 15 * M_PI / 180.0;
// 	double alpha0 = -20.0 * M_PI / 180.0;
// 	double alpha1 = 45.0 * M_PI / 180.0;
// 
// 	int VertNum = (int)pMesh0->Verts.size();
// 	for (int i = 0; i < VertNum; ++i)
// 	{
// 		double x = pMesh0->Verts[i].P[0];
// 		double y = pMesh0->Verts[i].P[1];
// 		double z = pMesh0->Verts[i].P[2];
// 
// 		double pi = atan2(x, y);
// 		double beta = pi / sin(theta);
// 
// 		if (alpha0 <= pi && pi <= alpha1)
// 		{
// 			double R = SQRT(SQR(x) + SQR(y));
// 			x = R * sin(theta) * sin(beta);
// 			y = R * (SQR(cos(theta)) + SQR(sin(theta)) * cos(beta));
// 			z = R * cos(theta) * sin(theta) * (1.0 - cos(beta));
// 		}
// 		else if (pi < alpha0)
// 		{
// 			beta = alpha0 / sin(theta);
// 
// 			GQuater q;
// 			q.SetFromAngleAxis(alpha0, GVector3(0.0, 0.0, 1.0), true);
// 
// 			GVector3 p(x, y, z);
// 			p = q * p;
// 
// 			q.SetFromAngleAxis(-theta, GVector3(1.0, 0.0, 0.0), true);
// 			p = q * p;
// 
// 			q.SetFromAngleAxis(-beta, GVector3(0.0, 1.0, 0.0), true);
// 			p = q * p;
// 
// 			q.SetFromAngleAxis(theta, GVector3(1.0, 0.0, 0.0), true);
// 			p = q * p;
// 
// 			x = p[0];
// 			y = p[1];
// 			z = p[2];
// 		}
// 		else if (pi > alpha1)
// 		{
// 			beta = alpha1 / sin(theta);
// 
// 			GQuater q;
// 			q.SetFromAngleAxis(alpha1, GVector3(0.0, 0.0, 1.0), true);
// 
// 			GVector3 p(x, y, z);
// 			p = q * p;
// 
// 			q.SetFromAngleAxis(-theta, GVector3(1.0, 0.0, 0.0), true);
// 			p = q * p;
// 
// 			q.SetFromAngleAxis(-beta, GVector3(0.0, 1.0, 0.0), true);
// 			p = q * p;
// 
// 			q.SetFromAngleAxis(theta, GVector3(1.0, 0.0, 0.0), true);
// 			p = q * p;
// 
// 			x = p[0];
// 			y = p[1];
// 			z = p[2];
// 		}
// 
// 		pMesh0->Verts[i].P.Set(x, y, z);
// 	}
// 	pMesh0->UpdateNormal(true);
// 
// 	theScene().AddMesh(pMesh0);
// 	theMainFrm().UpdateWorkSpaceDlg();
}

void GaiaDoc::OnTestScene13()
{
	OnNewDocument();

	// ���� ���� �� �߰�
// 	GMaterial Mtrl("mat01");
// 	Mtrl.SetDiffuse(0.5, 0.5, 0.5, 0.5);
// 	Mtrl.SetAmbient(0.5, 0.5, 0.5, 0.5);
// 	theScene().AddMaterial(Mtrl);

	// ������
	GVector4 P[10];
	P[0].Set(3.0, 0.0, 1.0, 1.0);
	P[1].Set(2.0, 1.0, 2.0, 1.0);
	P[2].Set(1.0, 0.0, 1.0, 1.0);
	P[3].Set(1.0, 2.0, 3.0, 1.0);
	P[4].Set(0.0, 1.0, 2.0, 1.0);
	P[5].Set(-1.0, 0.0, 1.0, 1.0);
	P[6].Set(0.0, 3.0, 0.0, 1.0);
	P[7].Set(-1.0, 2.0, 2.0, 1.0);
	P[8].Set(-2.0, 1.0, 4.0, 1.0);
	P[9].Set(-3.0, 0.0, -1.0, 1.0);

	GTriBzrSrf *pSrf = new GTriBzrSrf("BzrSrf", GTransf(), 3, P);
	
	// ����� ������ �޽�����
	pSrf->Renderer.RenderType = RENDER_POLYGON | RENDER_CTRL_PT;
	pSrf->Renderer.SetSampleNum(50, 50);
	pSrf->Renderer.CtrlPtSize = 0.2;
	pSrf->CreateMesh();
	//pSrf->Renderer.NameMtrl = "mat01";
	theScene().AddMesh(pSrf);

	theMainFrm().UpdateWorkSpaceDlg();
}


void GaiaDoc::OnTestScene15()	// ������� ����
{
	OnNewDocument();

	
}


void GaiaDoc::OnTestScene16()
{
// 	OnNewDocument();
// 
// 	::load_mesh_from_tri_file(&theScene(), ".\\face-victoria\\v2500\\face-vic.par");
// 	::load_mesh_from_tri_file(&theScene(), ".\\face-victoria\\v2500\\face-vic.tri");
// 
// 	GMesh *pMesh0 = theScene().MeshList[0];
// 	GMesh *pMesh1 = theScene().MeshList[1];
// 
// 	int vnum = (int)pMesh0->Verts.size();
// 	int fnum = (int)pMesh0->Faces.size();
// 
// 	// �޽�����
// 	GMesh *pMesh2 = new GMesh("uniform sampling mesh", FACE_TRI);
// 
// 	std::vector<std::vector<int>> Info;
// 	pMesh0->GetIncidentInfo(Info);
// 
// 	int res = 100;
// 	for (int i = 0; i < res; ++i)
// 	{
// 		for (int j = 0; j < res; ++j)
// 		{		
// 			double u = (double)i / (double)(res - 1);
// 			double v = (double)j / (double)(res - 1);
// 
// 			static GFace *prev_f = NULL;
// 			if (prev_f != NULL)
// 			{
// 				int idx0 = prev_f->vIdx[0];
// 				int idx1 = prev_f->vIdx[1];
// 				int idx2 = prev_f->vIdx[2];
// 
// 				double x1 = pMesh0->Verts[idx0].P[0];
// 				double y1 = pMesh0->Verts[idx0].P[1];
// 				double x2 = pMesh0->Verts[idx1].P[0];
// 				double y2 = pMesh0->Verts[idx1].P[1];
// 				double x3 = pMesh0->Verts[idx2].P[0];
// 				double y3 = pMesh0->Verts[idx2].P[1];
// 
// 				double min_x = MIN(MIN(x1, x2), x3);
// 				double max_x = MAX(MAX(x1, x2), x3);
// 				double min_y = MIN(MIN(y1, y2), y3);
// 				double max_y = MAX(MAX(y1, y2), y3);
// 
// 				if (u >= min_x && u <= max_x && v >= min_y && v <= max_y)
// 				{
// 					double a, b, c, d, e, f;
// 					a = x2 - x1;
// 					b = x3 - x1;
// 					c = y2 - y1;
// 					d = y3 - y1;
// 					e = u - x1;
// 					f = v - y1;
// 
// 					double alpha = (d * e - b * f) / (a * d - b * c);
// 					double beta = (-c * e + a * f) / (a * d - b * c);
// 
// 					if (alpha >= 0.0 && alpha <= 1.0 && beta >= 0.0 && beta <= 1.0)
// 					{
// 						GPoint3 p1 = pMesh1->Verts[idx0].P;
// 						GPoint3 p2 = pMesh1->Verts[idx1].P;
// 						GPoint3 p3 = pMesh1->Verts[idx2].P;
// 						GPoint3 p = p1 + (p2 - p1) * alpha + (p3 - p1) * beta;
// 						pMesh2->Verts.AddVertex(GVertex(p, GVector3()));
// 						continue;
// 					}
// 				}
// 				
// 				bool bfind = false;
// 				for (int k = 0; k < 3; ++k)
// 				{
// 					int idx = prev_f->vIdx[k];
// 					int fnum = (int)Info[idx].size();
// 					for (int l = 0; l < fnum; ++l)
// 					{
// 						prev_f = &(pMesh0->Faces[Info[idx][l]]);
// 
// 						int idx0 = prev_f->vIdx[0];
// 						int idx1 = prev_f->vIdx[1];
// 						int idx2 = prev_f->vIdx[2];
// 
// 						double x1 = pMesh0->Verts[idx0].P[0];
// 						double y1 = pMesh0->Verts[idx0].P[1];
// 						double x2 = pMesh0->Verts[idx1].P[0];
// 						double y2 = pMesh0->Verts[idx1].P[1];
// 						double x3 = pMesh0->Verts[idx2].P[0];
// 						double y3 = pMesh0->Verts[idx2].P[1];
// 
// 						double min_x = MIN(MIN(x1, x2), x3);
// 						double max_x = MAX(MAX(x1, x2), x3);
// 						double min_y = MIN(MIN(y1, y2), y3);
// 						double max_y = MAX(MAX(y1, y2), y3);
// 
// 						if (u >= min_x && u <= max_x && v >= min_y && v <= max_y)
// 						{
// 							double a, b, c, d, e, f;
// 							a = x2 - x1;
// 							b = x3 - x1;
// 							c = y2 - y1;
// 							d = y3 - y1;
// 							e = u - x1;
// 							f = v - y1;
// 
// 							double alpha = (d * e - b * f) / (a * d - b * c);
// 							double beta = (-c * e + a * f) / (a * d - b * c);
// 
// 							if (alpha >= 0.0 && alpha <= 1.0 && beta >= 0.0 && beta <= 1.0)
// 							{
// 								GPoint3 p1 = pMesh1->Verts[idx0].P;
// 								GPoint3 p2 = pMesh1->Verts[idx1].P;
// 								GPoint3 p3 = pMesh1->Verts[idx2].P;
// 								GPoint3 p = p1 + (p2 - p1) * alpha + (p3 - p1) * beta;
// 								pMesh2->Verts.AddVertex(GVertex(p, GVector3()));
// 								bfind = true;
// 								break;
// 							}
// 						}
// 					}
// 					if (bfind)
// 						break;
// 				}
// 				if (bfind)
// 					continue;
// 			}
// 
// 			for (int k = 0; k < fnum; ++k)
// 			{
// 				GFace *f0 = &(pMesh0->Faces[k]);
// 				int idx0 = f0->vIdx[0];
// 				int idx1 = f0->vIdx[1];
// 				int idx2 = f0->vIdx[2];
// 
// 				double x1 = pMesh0->Verts[idx0].P[0];
// 				double y1 = pMesh0->Verts[idx0].P[1];
// 				double x2 = pMesh0->Verts[idx1].P[0];
// 				double y2 = pMesh0->Verts[idx1].P[1];
// 				double x3 = pMesh0->Verts[idx2].P[0];
// 				double y3 = pMesh0->Verts[idx2].P[1];
// 
// 				double min_x = MIN(MIN(x1, x2), x3);
// 				double max_x = MAX(MAX(x1, x2), x3);
// 				double min_y = MIN(MIN(y1, y2), y3);
// 				double max_y = MAX(MAX(y1, y2), y3);
// 
// 				if (u < min_x)
// 					continue;
// 				if (u > max_x)
// 					continue;
// 				if (v < min_y)
// 					continue;
// 				if (v > max_y)
// 					continue;
// 
// 				double a, b, c, d, e, f;
// 				a = x2 - x1;
// 				b = x3 - x1;
// 				c = y2 - y1;
// 				d = y3 - y1;
// 				e = u - x1;
// 				f = v - y1;
// 
// 				double alpha = (d * e - b * f) / (a * d - b * c);
// 				double beta = (-c * e + a * f) / (a * d - b * c);
// 
// 				if (alpha >= 0.0 && alpha <= 1.0 && beta >= 0.0 && beta <= 1.0)
// 				{
// 					GPoint3 p1 = pMesh1->Verts[idx0].P;
// 					GPoint3 p2 = pMesh1->Verts[idx1].P;
// 					GPoint3 p3 = pMesh1->Verts[idx2].P;
// 					GPoint3 p = p1 + (p2 - p1) * alpha + (p3 - p1) * beta;
// 					pMesh2->Verts.AddVertex(GVertex(p, GVector3()));
// 					prev_f = f0;
// 					break;
// 				}
// 			}
// 		}
// 	}
// 
// 	pMesh2->Renderer.shade_flat = false;
// 	pMesh2->Renderer.bShowCtrlPt = true;
// 	pMesh2->Renderer.CtrlPtSize = 0.01;
// 	pMesh2->Renderer.wire_frame = false;
// 	pMesh2->Renderer.bTransparent = false;
// 
// 	// �Ϲ����� ����� �ﰢ�� ����Ʈ ����
// 	for (int i = 0; i < res - 1; ++i)
// 	{
// 		for(int j = 0; j < res - 1; ++j)
// 		{
// 			int vidx0 = i * (res) + j;
// 			int vidx1 = vidx0 + 1;
// 			int vidx2 = vidx1 + (res);
// 			int vidx3 = vidx2 - 1;
// 
// 			GFace f0(vidx0, vidx2, vidx1, vidx0, vidx2, vidx1, 0);
// 			GFace f1(vidx0, vidx3, vidx2, vidx0, vidx3, vidx2, 0);
// 
// 			pMesh2->Faces.AddFace(f0);
// 			pMesh2->Faces.AddFace(f1);
// 		}
// 	}
// 	pMesh2->UpdateNormal(true);
// 	theScene().AddMesh(pMesh2);
// 
// 
// 	theMainFrm().UpdateWorkSpaceDlg();

}

/*
void GaiaDoc::OnTestScene16()
{
	OnNewDocument();

	::load_mesh_from_tri_file(&theScene(), ".\\face-victoria\\v2500\\face-vic.par");
	::load_mesh_from_tri_file(&theScene(), ".\\face-victoria\\v2500\\face-vic.tri");

	GMesh *pMesh0 = theScene().MeshList[0];
	GMesh *pMesh1 = theScene().MeshList[1];

	int vnum = (int)pMesh0->Verts.size();
	int fnum = (int)pMesh0->Faces.size();

	// �޽�����
	GMesh *pMesh2 = new GMesh("uniform sampling mesh");

	std::vector<int> Info;
	pMesh0->GetAdjacentInfo(Info);

	int res = 100;
	for (int i = 0; i < res; ++i)
	{
		for (int j = 0; j < res; ++j)
		{		
			double u = (double)i / (double)(res - 1);
			double v = (double)j / (double)(res - 1);

			static int face_idx = -1;
			if (face_idx != -1)
			{
				int idx0 = pMesh0->Faces[face_idx].vIdx[0];
				int idx1 = pMesh0->Faces[face_idx].vIdx[1];
				int idx2 = pMesh0->Faces[face_idx].vIdx[2];

				double x1 = pMesh0->Verts[idx0].P[0];
				double y1 = pMesh0->Verts[idx0].P[1];
				double x2 = pMesh0->Verts[idx1].P[0];
				double y2 = pMesh0->Verts[idx1].P[1];
				double x3 = pMesh0->Verts[idx2].P[0];
				double y3 = pMesh0->Verts[idx2].P[1];

				double min_x = MIN(MIN(x1, x2), x3);
				double max_x = MAX(MAX(x1, x2), x3);
				double min_y = MIN(MIN(y1, y2), y3);
				double max_y = MAX(MAX(y1, y2), y3);

				if (u >= min_x && u <= max_x && v >= min_y && v <= max_y)
				{
					double a, b, c, d, e, f;
					a = x2 - x1;
					b = x3 - x1;
					c = y2 - y1;
					d = y3 - y1;
					e = u - x1;
					f = v - y1;

					double alpha = (d * e - b * f) / (a * d - b * c);
					double beta = (-c * e + a * f) / (a * d - b * c);

					if (alpha >= 0.0 && alpha <= 1.0 && beta >= 0.0 && beta <= 1.0)
					{
						GPoint3 p1 = pMesh1->Verts[idx0].P;
						GPoint3 p2 = pMesh1->Verts[idx1].P;
						GPoint3 p3 = pMesh1->Verts[idx2].P;
						GPoint3 p = p1 + (p2 - p1) * alpha + (p3 - p1) * beta;
						pMesh2->Verts.AddVertex(GVertex(p, GVector3()));
						continue;
					}
				}

				bool bfind = false;
				for (int k = 0; k < 3; ++k)
				{
					int tmp_idx = Info[3 * face_idx + k];
					if (tmp_idx == -1)
						continue;

					int idx0 = pMesh0->Faces[tmp_idx].vIdx[0];
					int idx1 = pMesh0->Faces[tmp_idx].vIdx[1];
					int idx2 = pMesh0->Faces[tmp_idx].vIdx[2];

					double x1 = pMesh0->Verts[idx0].P[0];
					double y1 = pMesh0->Verts[idx0].P[1];
					double x2 = pMesh0->Verts[idx1].P[0];
					double y2 = pMesh0->Verts[idx1].P[1];
					double x3 = pMesh0->Verts[idx2].P[0];
					double y3 = pMesh0->Verts[idx2].P[1];

					double min_x = MIN(MIN(x1, x2), x3);
					double max_x = MAX(MAX(x1, x2), x3);
					double min_y = MIN(MIN(y1, y2), y3);
					double max_y = MAX(MAX(y1, y2), y3);

					if (u >= min_x && u <= max_x && v >= min_y && v <= max_y)
					{
						double a, b, c, d, e, f;
						a = x2 - x1;
						b = x3 - x1;
						c = y2 - y1;
						d = y3 - y1;
						e = u - x1;
						f = v - y1;

						double alpha = (d * e - b * f) / (a * d - b * c);
						double beta = (-c * e + a * f) / (a * d - b * c);

						if (alpha >= 0.0 && alpha <= 1.0 && beta >= 0.0 && beta <= 1.0)
						{
							GPoint3 p1 = pMesh1->Verts[idx0].P;
							GPoint3 p2 = pMesh1->Verts[idx1].P;
							GPoint3 p3 = pMesh1->Verts[idx2].P;
							GPoint3 p = p1 + (p2 - p1) * alpha + (p3 - p1) * beta;
							pMesh2->Verts.AddVertex(GVertex(p, GVector3()));
							face_idx = tmp_idx;
							bfind = true;
							break;
						}
					}
				}

				if (bfind)
					continue;
			}

			for (int k = 0; k < fnum; ++k)
			{
				GFace *f0 = &(pMesh0->Faces[k]);
				int idx0 = f0->vIdx[0];
				int idx1 = f0->vIdx[1];
				int idx2 = f0->vIdx[2];

				double x1 = pMesh0->Verts[idx0].P[0];
				double y1 = pMesh0->Verts[idx0].P[1];
				double x2 = pMesh0->Verts[idx1].P[0];
				double y2 = pMesh0->Verts[idx1].P[1];
				double x3 = pMesh0->Verts[idx2].P[0];
				double y3 = pMesh0->Verts[idx2].P[1];

				double min_x = MIN(MIN(x1, x2), x3);
				double max_x = MAX(MAX(x1, x2), x3);
				double min_y = MIN(MIN(y1, y2), y3);
				double max_y = MAX(MAX(y1, y2), y3);

				if (u < min_x)
					continue;
				if (u > max_x)
					continue;
				if (v < min_y)
					continue;
				if (v > max_y)
					continue;

				double a, b, c, d, e, f;
				a = x2 - x1;
				b = x3 - x1;
				c = y2 - y1;
				d = y3 - y1;
				e = u - x1;
				f = v - y1;

				double alpha = (d * e - b * f) / (a * d - b * c);
				double beta = (-c * e + a * f) / (a * d - b * c);

				if (alpha >= 0.0 && alpha <= 1.0 && beta >= 0.0 && beta <= 1.0)
				{
					GPoint3 p1 = pMesh1->Verts[idx0].P;
					GPoint3 p2 = pMesh1->Verts[idx1].P;
					GPoint3 p3 = pMesh1->Verts[idx2].P;
					GPoint3 p = p1 + (p2 - p1) * alpha + (p3 - p1) * beta;
					pMesh2->Verts.AddVertex(GVertex(p, GVector3()));
					face_idx = k;
					break;
				}
			}
		}
	}

	pMesh2->Renderer.shade_flat = false;
	pMesh2->Renderer.bShowCtrlPt = true;
	pMesh2->Renderer.CtrlPtSize = 0.01;
	pMesh2->Renderer.wire_frame = false;
	pMesh2->Renderer.bTransparent = false;

	// �Ϲ����� ����� �ﰢ�� ����Ʈ ����
	for (int i = 0; i < res - 1; ++i)
	{
		for(int j = 0; j < res - 1; ++j)
		{
			int vidx0 = i * (res) + j;
			int vidx1 = vidx0 + 1;
			int vidx2 = vidx1 + (res);
			int vidx3 = vidx2 - 1;

			GFace f0(vidx0, vidx2, vidx1, vidx0, vidx2, vidx1, 0);
			GFace f1(vidx0, vidx3, vidx2, vidx0, vidx3, vidx2, 0);

			pMesh2->Faces.AddFace(f0);
			pMesh2->Faces.AddFace(f1);
		}
	}
	pMesh2->UpdateNormal(true);
	theScene().AddMesh(pMesh2);


	theMainFrm().UpdateWorkSpaceDlg();

}
*/

void GaiaDoc::OnTestScene17()
{
	
}

void GaiaDoc::OnTestScene18()
{
 	OnNewDocument();
}

void GaiaDoc::OnTestScene19()
{
 	OnNewDocument();
}

void GaiaDoc::OnTestScene20()
{
	OnNewDocument();

	theMainFrm().UpdateWorkSpaceDlg();
}

void GaiaDoc::OnTestScene21()
{
	OnNewDocument();

	// ������
// 	GVector4 P[4][4];
// 	P[0][0].Set(-3.0, -3.0, 0.0, 1.0);	P[0][1].Set(-3.0, -1.0, 0.0, 1.0);	P[0][2].Set(-3.0, 1.0, 0.0, 1.0);	P[0][3].Set(-3.0, 3.0, 0.0, 1.0);
// 	P[1][0].Set(-1.0, -3.0, 0.0, 1.0);	P[1][1].Set(-1.0, -1.0, 5.0, 5.0);	P[1][2].Set(-1.0, 1.0, 5.0, 1.0);	P[1][3].Set(-1.0, 3.0, 0.0, 1.0);
// 	P[2][0].Set(1.0, -3.0, 0.0, 1.0);	P[2][1].Set(1.0, -1.0, 5.0, 1.0);	P[2][2].Set(1.0, 1.0, 5.0, 1.0);	P[2][3].Set(1.0, 3.0, 0.0, 1.0);
// 	P[3][0].Set(3.0, -3.0, 0.0, 1.0);	P[3][1].Set(3.0, -1.0, 0.0, 1.0);	P[3][2].Set(3.0, 1.0, 0.0, 1.0);	P[3][3].Set(3.0, 3.0, 0.0, 1.0);
// 	GNurbsSrf *pSrf = ::get_gnurbs_srf_closed(3, 3, 3, 3, &P[0][0], false, false);
// 	
// 	GMesh *pMesh0 = new GMesh("test", pSrf);
// 	theScene().AddMesh(pMesh0);
// 
// 	pMesh0->Renderer.NumSampU = 100;
// 	pMesh0->Renderer.NumSampV = 100;
// 	pMesh0->Renderer.shade_flat = false;
// 	pMesh0->Renderer.bShowCtrlPt = false;
// 	pMesh0->Renderer.CtrlPtSize = 0.01;
// 	pMesh0->Renderer.wire_frame = false;
// 	pMesh0->Renderer.bTransparent = false;
// 	pMesh0->GenerateMeshFromNurbs();
	
	
// 	std::vector<GNurbsSrf *> SrfList;
// 	::get_gnurbs_srf_teapot(SrfList);
// 	
// 	for (int i = 0; i < 32; ++i)
// 	{
// 		// �޽�����
// 		std::string name = std::string("teapot-srf") + (CStringA)(i);
// 
// 		GMesh *pMesh0 = new GMesh(name.c_str(), FACE_TRI, SrfList[i]);
// 		theScene().AddMesh(pMesh0);
// 	
// 		pMesh0->Renderer.NumSampU = 20;
// 		pMesh0->Renderer.NumSampV = 20;
// 		pMesh0->Renderer.shade_flat = false;
// 		pMesh0->Renderer.bShowCtrlPt = false;
// 		pMesh0->Renderer.CtrlPtSize = 0.01;
// 		pMesh0->Renderer.wire_frame = false;
// 		pMesh0->Renderer.bTransparent = false;
// 		pMesh0->GenerateMeshFromNurbs();
// 	}
// 
// 	std::string MapName = ".\\face-victoria\\v2500\\displacement image1.bmp";
// 	GMapDisp Map(MapName.c_str());
// 	Map.Scale = 0.5;
// 	Map.BlendBoundary = true;
// 	theScene().AddMapDisp(Map);
// 
// 	GMesh *pMesh0 = theScene().MeshList[5];
// 	pMesh0->Renderer.NumSampU = 100;
// 	pMesh0->Renderer.NumSampV = 100;
// 	pMesh0->Renderer.shade_flat = false;
// 	pMesh0->Renderer.bShowCtrlPt = true;
// 	pMesh0->Renderer.CtrlPtSize = 0.01;
// 	pMesh0->Renderer.wire_frame = false;
// 	pMesh0->Renderer.bTransparent = false;
// 	pMesh0->Renderer.displacment = true;
// 	pMesh0->Renderer.map_disp_name = MapName;
// 	pMesh0->GenerateMeshFromNurbs();
// 	pMesh0->UpdateNormal(true);
// 
// 	theMainFrm().UpdateWorkSpaceDlg();
}

void GaiaDoc::OnTestScene22()
{
	std::string fname = WorkingDirectory + ".\\avm\\skull_vp.avm";
	//char *fname = ".\\avm\\fandisk_vp.avm";
	//char *fname = ".\\avm\\lion_vp.avm";
	
	int vnum, fnum, cnum;
	char str[64], Line[1024];

	FILE *fp = fopen(fname.c_str(), "r");

	//# 20768 vertices
	fgets (Line, 1024, fp);
	sscanf (Line, "%s %d %s", str, &vnum, str);

	//# 166579 faces
	fgets (Line, 1024, fp);
	sscanf (Line, "%s %d %s", str, &fnum, str);

	//# 75345 cells
	fgets (Line, 1024, fp);
	sscanf (Line, "%s %d %s", str, &cnum, str);

	// empty line
	fgets (Line, 1024, fp);

	double *Param = new double [vnum * 3];
	GVector4 *Point = new GVector4 [vnum];
	
	for (int i = 0; i < vnum; ++i)
	{
		float x, y, z;
		fgets (Line, 1024, fp);
		sscanf (Line, "%s %f %f %f", str, &x, &y, &z);
		Point[i].Set(x, y, z, 0.0);
	}

	for (int i = 0; i < vnum; ++i)
	{
		float u, v, w;
		fgets (Line, 1024, fp);
		sscanf (Line, "%s %f %f %f", str, &u, &v, &w);
		Param[i * 3] = u;
		Param[i * 3 + 1] = v;
		Param[i * 3 + 2] = w;	
	}

	fgets (Line, 1024, fp);
	fclose(fp);

	GUcbsVol *pVol = ::get_gucbs_vol_approx(3, 3, 3, vnum, Param, Point, 5);
	pVol->Renderer.RenderType = RENDER_POLYGON | RENDER_CTRL_PT;
	pVol->Renderer.CtrlPtSize = 0.1;
	pVol->Renderer.SetSampleNum(13, 17, 19);
	pVol->Renderer.IsoSrfParam[0] = 0.2;
	pVol->Renderer.IsoSrfParam[1] = 0.8;
	pVol->CreateMesh();
	theScene().AddMesh(pVol);

	theMainFrm().UpdateWorkSpaceDlg();

	delete [] Param;
	delete [] Point;
}

void GaiaDoc::OnExportSelection()
{
	// ������ �̸��� ������ ��ο� ���� �̸� ������ �����ϰ�,
	CString FilePath, FileName;

	// ������ Ȯ���� ���Ϳ� �ε����� �����ϰ�,
	LPCTSTR filter = _T("OBJ file(*.obj)|*.obj|ASE file(*.ase)|*.ase||");
	int idxFilter = 0;

	// ���� ���� ��ȭ���ڸ� ����ϰ�,
	CFileDialog FileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, filter);
	if(FileDlg.DoModal() != IDOK) 
		return;

	// ����ڰ� ������ ������ ��θ�, ���� �̸� �� ���� �ε����� ���ϰ�,
	FilePath = FileDlg.GetPathName(); // ���� ��� �� �̸����� ����
	FileName = FileDlg.GetFileName();	// ���� �̸��� ����
	idxFilter = FileDlg.m_ofn.nFilterIndex; 

	// ���� ������ ��θ� ���ͼ�, �۾� ���丮�� �����Ѵ�.
	std::string file_path = std::string((CStringA)(FilePath));
	file_path = get_file_path(file_path);
	_chdir(file_path.c_str());

	AfxGetApp()->BeginWaitCursor();
	switch (idxFilter)
	{
	case 1: // OBJ ���� ����
		if(FilePath.Find(".obj") == -1)
			FilePath += ".obj";
		::save_mesh_as_obj_file(theEditor().GetEditMesh(), (CStringA)(FilePath));
		break;

	case 2: // ASE ���� ����
		if(FilePath.Find(".ase") == -1)
			FilePath += ".ase";
		::save_scene_as_ase_file(&theScene(), (CStringA)(FilePath));
		break;	
	}
	AfxGetApp()->EndWaitCursor();

	UpdateAllViews(NULL);
}

void GaiaDoc::OnEtri2015()
{
	// ��ȭ���ڰ� �������� �ʾҴٸ�,
	if (m_pClothesSkinDlg == NULL)
	{
		// ��Ʈ�� ��Ź����
		OnNewDocument();
		theScene().FileName = "EtriProject2015.gff";

		// ��ȭ���ڸ� �����ϰ� ����� ��,
		m_pClothesSkinDlg = new GClothesSkinDlg();
		m_pClothesSkinDlg->Create(IDD_CLOTHES_SKIN_DLG);
		m_pClothesSkinDlg->OnInitDialog();
		m_pClothesSkinDlg->ShowWindow(SW_SHOW);

		// ���� ���� ��, ��Ų �޽�, ��ĿƮ �޽��� �ҷ��� MeshList[0]�� �����ϰ�,
		std::string file_path = "C:\\Data\\Project\\ETRI\\2015\\data\\Avatar\\";
		_chdir(file_path.c_str());
		::loadAvatarDump(&theScene(), "avatar_woman.dump");

		// ���� ���� ���� ��Ų �޽��� �ҷ��� MeshList[1]�� �����ϰ�,
		//::loadAvatarDump(&theScene(), "C:\\Data\\Project\\ETRI\\2015\\data\\Avata\\avatar_man.dump");
		
		// ��Ű���� �� �޽��� �ҷ��� MeshList[1]�� �����ϰ�,
		file_path = "C:\\Data\\Project\\ETRI\\2015\\data\\Clothes\\";
		_chdir(file_path.c_str());
		::load_mesh_from_obj_file(&theScene(), "[126][Woman][Tops][Light_Blue_T-Shirt][Edited].obj");
		
		// ���� ���� Ȱ��ȭ�� �ƹ�Ÿ �𵨷� �����ϰ�,
		m_pAvatar = FindMesh(theScene().MeshList, "avatar_woman");

		// ���� ���� ��Ų �޽��� Ȱ��ȭ�� ��Ų �޽��� �����ϰ�,
		m_pAvatarSkinMesh = FindMesh(m_pAvatar->SubMeshList, "avatar_woman_skin");
		
		// ���� ���� �� �޽��� Ȱ��ȭ�� �� �޽��� �����ϰ�,
		m_pAvatarBoneMesh = FindMesh(m_pAvatar->SubMeshList, "avatar_woman_Root");

		// ���� ���� ġ�� �޽��� �����ϰ�,
		m_pAvatarSkirtMesh = FindMesh(m_pAvatar->SubMeshList, "avatar_woman_skirt");
		
		// ��Ű���� �� �޽��� �����Ѵ�.
		m_pAvatarClothesMesh = FindMesh(theScene().MeshList, "[126][Woman][Tops][Light_Blue_T-Shirt][Edited]");
		m_pAvatarClothesMesh->Renderer.RenderType = RENDER_WIRE;

		// ���� ���� ������ ���θ� false�� �����Ѵ�.
		//theScene().MeshList[1]->HideMesh(true);

		// Ȱ��ȭ�� ��Ų �޽��� ������ ���θ� false�� �����Ѵ�.
		//m_pAvatarSkinMesh->HideMesh(true);

		// Ȱ��ȭ�� ��ĿƮ �޽��� ������ ���θ� false�� �����Ѵ�.
		m_pAvatarSkirtMesh->HideMesh(true);

		// Ȱ��ȭ�� �� �޽��� ������ ���θ� false�� �����Ѵ�.
		//m_pAvatarBoneMesh->HideMesh(true);

		// ���ε��� ���� ���� ���� ����� �����Ѵ�.
		Etri2015CreateBindRefSwpPlnLS();
		Etri2015CreateBindRefSwpPlnRS();

		theMainFrm().UpdateWorkSpaceDlg();
	}
	else
	{
		m_pClothesSkinDlg->OnInitDialog();
		m_pClothesSkinDlg->ShowWindow(SW_SHOW);
	}
}

void GaiaDoc::Etri2015CreateBindRefSwpPlnLS() // mj : Sweep plane for Left Shoulder
{
	// �� �޽��� ������ �޽� ����Ʈ�� �����ϰ�,
	std::vector<GMesh *> MeshList;

	// 4 ���� �� �޽��� ã��, �޽� ����Ʈ�� �߰��ϰ�,
	MeshList.push_back(FindMesh(m_pAvatar->SubMeshList, m_pAvatar->Name + "_Spine2"));
	MeshList.push_back(FindMesh(m_pAvatar->SubMeshList, m_pAvatar->Name + "_Spine3"));
	MeshList.push_back(FindMesh(m_pAvatar->SubMeshList, m_pAvatar->Name + "_LCollarBone"));
	MeshList.push_back(FindMesh(m_pAvatar->SubMeshList, m_pAvatar->Name + "_LShoulder"));
	MeshList.push_back(FindMesh(m_pAvatar->SubMeshList, m_pAvatar->Name + "_LShoulderTwist"));

	// ���� ���� ����� ������ �Ǻ� �޽��� �����Ѵ�.
	GMesh *pLSPivot = create_mesh_from_sphere("LS_Pivot", GTransf(), 1.0, 10, 10);
	
	// �Ǻ� �޽��� ��鿡 �߰��� ��, ��ġ�� �����Ѵ�.
	theScene().AddMesh(pLSPivot);
	GVector3 dt(MeshList[3]->MC.t[0], MeshList[0]->MC.t[1], MeshList[0]->MC.t[2]);	
	pLSPivot->Edit(GTransf(), dt);
	
	// ���� ����� ���ε��� ���� ���� ���� ����� �����Ѵ�.
	GBindRefPln *pRefPln = new GBindRefPln("LS_BindRefPln", GTransf(), pLSPivot, MeshList, m_pAvatar->Name);
	pRefPln->CreateMesh();
	theScene().AddMesh(pRefPln);
	m_pBindRefPlnLeftShoulder = pRefPln;
}

void GaiaDoc::Etri2015CreateBindRefSwpPlnRS() // mj : Sweep plane for Right Shoulder
{
	// �� �޽��� ������ �޽� ����Ʈ�� �����ϰ�,
	std::vector<GMesh *> MeshList;

	// 4 ���� �� �޽��� ã��, �޽� ����Ʈ�� �߰��ϰ�,
	MeshList.push_back(FindMesh(m_pAvatar->SubMeshList, m_pAvatar->Name + "_Spine2"));
	MeshList.push_back(FindMesh(m_pAvatar->SubMeshList, m_pAvatar->Name + "_Spine3"));
	MeshList.push_back(FindMesh(m_pAvatar->SubMeshList, m_pAvatar->Name + "_RCollarBone"));
	MeshList.push_back(FindMesh(m_pAvatar->SubMeshList, m_pAvatar->Name + "_RShoulder"));
	MeshList.push_back(FindMesh(m_pAvatar->SubMeshList, m_pAvatar->Name + "_RShoulderTwist"));

	// ���� ���� ����� ������ �Ǻ� �޽��� �����Ѵ�.
	GMesh *pRSPivot = create_mesh_from_sphere("RS_Pivot", GTransf(), 1.0, 10, 10);
	pRSPivot->HideMesh(false);

	// �Ǻ� �޽��� ��鿡 �߰��� ��, ��ġ�� �����Ѵ�.
	theScene().AddMesh(pRSPivot);
	GVector3 dt(MeshList[3]->MC.t[0], MeshList[0]->MC.t[1], MeshList[0]->MC.t[2]);	
	pRSPivot->Edit(GTransf(), dt);

	// ������ ����� ���ε��� ���� ���� ���� ����� �����Ѵ�.
	GBindRefPln *pRefPln = new GBindRefPln("RS_BindRefPln", GTransf(), pRSPivot, MeshList, m_pAvatar->Name);
	pRefPln->CreateMesh();
	pRefPln->HideMesh(false);
	theScene().AddMesh(pRefPln);
	m_pBindRefPlnRightShoulder = pRefPln;
}

void GaiaDoc::OnImplantGuide()
{
	// ��ȭ���ڰ� �������� �ʾҴٸ�,
	if (m_pDentalCadDlg == NULL)
	{
		// ����� ��Ź����
		OnNewDocument();
		theScene().FileName = "Guide.gff";

		// ��ȭ���ڸ� �����ϰ� ����� ��,
		m_pDentalCadDlg = new GDentalCadDlg();
		m_pDentalCadDlg->Create(IDD_DENTAL_CAD_DLG);
		m_pDentalCadDlg->OnInitDialog();
		m_pDentalCadDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pDentalCadDlg->OnInitDialog();
		m_pDentalCadDlg->ShowWindow(SW_SHOW);
	}
}

void GaiaDoc::OnDentalModel1()
{
	OnNewDocument();

	theScene().FileName = "test.gff";
	std::string path = "C:\\Data\\Project\\�����\\ġ�Ƶ�����-���ø�\\";
	_chdir(path.c_str());

	std::string fname1 = "tooth-";
	for (int i = 11; i < 19; ++i)
	{
		std::string fname = fname1 + cast_str(i) + ".obj";
		::load_mesh_from_obj_file(&theScene(), fname.c_str());
	}

	for (int i = 21; i < 29; ++i)
	{
		std::string fname = fname1 + cast_str(i) + ".obj";
		::load_mesh_from_obj_file(&theScene(), fname.c_str());
	}

	for (int i = 31; i < 39; ++i)
	{
		std::string fname = fname1 + cast_str(i) + ".obj";
		::load_mesh_from_obj_file(&theScene(), fname.c_str());
	}

	for (int i = 41; i < 49; ++i)
	{
		std::string fname = fname1 + cast_str(i) + ".obj";
		::load_mesh_from_obj_file(&theScene(), fname.c_str());
	}

	theMainFrm().UpdateWorkSpaceDlg();
}

void GaiaDoc::OnDentalModel2()
{
	OnNewDocument();

	theScene().FileName = "Tooth.gff";

	std::string fname1 = "C:\\Data\\Project\\�����\\ġ�Ƶ�����-�𵨸�2\\tooth_";
	for (int i = 11; i < 19; ++i)
	{
		std::string fname = fname1 + cast_str(i) + "_smooth.obj";
		::load_mesh_from_obj_file(&theScene(), fname.c_str());
	}

	for (int i = 21; i < 29; ++i)
	{
		std::string fname = fname1 + cast_str(i) + "_smooth.obj";
		::load_mesh_from_obj_file(&theScene(), fname.c_str());
	}

	for (int i = 31; i < 39; ++i)
	{
		std::string fname = fname1 + cast_str(i) + "_smooth.obj";
		::load_mesh_from_obj_file(&theScene(), fname.c_str());
	}

	for (int i = 41; i < 49; ++i)
	{
		std::string fname = fname1 + cast_str(i) + "_smooth.obj";
		::load_mesh_from_obj_file(&theScene(), fname.c_str());
	}

	theMainFrm().UpdateWorkSpaceDlg();
}

