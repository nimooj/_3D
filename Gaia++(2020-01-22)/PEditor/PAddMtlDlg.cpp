// PAddMtlDlg.cpp : ���� �����Դϴ�.
//
#include "stdafx.h"
#include "PAddMtlDlg.h"


// PAddMtlDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(PAddMtlDlg, CDialog)

PAddMtlDlg::PAddMtlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PAddMtlDlg::IDD, pParent)
	, m_AmbientR(0)
	, m_AmbientG(0)
	, m_AmbientB(0)
	, m_DiffuseR(0)
	, m_DiffuseG(0)
	, m_DiffuseB(0)
	, m_SpecularR(0)
	, m_SpecularG(0)
	, m_SpecularB(0)
	, m_MtlName(_T(""))
	, m_TexFileName(_T(""))
{
	m_CurrMtlIdx = -1;
}

PAddMtlDlg::~PAddMtlDlg()
{
}

void PAddMtlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MTL_NAME, m_MtlName);
	DDX_Text(pDX, IDC_TEX_FILE_NAME, m_TexFileName);
	DDX_Slider(pDX, IDC_AMBIENT_RED, m_AmbientR);
	DDX_Slider(pDX, IDC_AMBIENT_GREEN, m_AmbientG);
	DDX_Slider(pDX, IDC_AMBIENT_BLUE, m_AmbientB);
	DDX_Slider(pDX, IDC_DIFFUSE_RED, m_DiffuseR);
	DDX_Slider(pDX, IDC_DIFFUSE_GREEN, m_DiffuseG);
	DDX_Slider(pDX, IDC_DIFFUSE_BLUE, m_DiffuseB);
	DDX_Slider(pDX, IDC_SPECULAR_RED, m_SpecularR);
	DDX_Slider(pDX, IDC_SPECULAR_GREEN, m_SpecularG);
	DDX_Slider(pDX, IDC_SPECULAR_BLUE, m_SpecularB);
	DDX_Control(pDX, IDC_AMBIENT_RED, m_cAmbientR);
	DDX_Control(pDX, IDC_AMBIENT_GREEN, m_cAmbientG);
	DDX_Control(pDX, IDC_AMBIENT_BLUE, m_cAmbientB);
	DDX_Control(pDX, IDC_DIFFUSE_RED, m_cDiffuseR);
	DDX_Control(pDX, IDC_DIFFUSE_GREEN, m_cDiffuseG);
	DDX_Control(pDX, IDC_DIFFUSE_BLUE, m_cDiffuseB);
	DDX_Control(pDX, IDC_SPECULAR_RED, m_cSpecularR);
	DDX_Control(pDX, IDC_SPECULAR_GREEN, m_cSpecularG);
	DDX_Control(pDX, IDC_SPECULAR_BLUE, m_cSpecularB);
}


BEGIN_MESSAGE_MAP(PAddMtlDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDOK, &PAddMtlDlg::OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_MTL_RANDOM, &PAddMtlDlg::OnBnClickedMtlRandom)
	ON_BN_CLICKED(IDC_OPEN_DIFFUSE_TEXTURE, &PAddMtlDlg::OnBnClickedOpenDiffuseTexture)
END_MESSAGE_MAP()


// PAddMtlDlg �޽��� ó�����Դϴ�.
BOOL PAddMtlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���� ���õ� ���� �޽��� ���ٸ� �����Ѵ�.
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh == NULL)
		return TRUE;

	// ��Ʈ���� ������ �����ϰ� �����Ѵ�.
	m_cAmbientR.SetRange(0, 255);
	m_cAmbientG.SetRange(0, 255);
	m_cAmbientB.SetRange(0, 255);
	m_cDiffuseR.SetRange(0, 255);
	m_cDiffuseG.SetRange(0, 255);
	m_cDiffuseB.SetRange(0, 255);
	m_cSpecularR.SetRange(0, 255);
	m_cSpecularG.SetRange(0, 255);
	m_cSpecularB.SetRange(0, 255);

	// ���ο� ������ �̸��� �����ϰ�,
	std::string MtlName = create_mtl_name("Mtl");
	int NumMtl = pMesh->GetNumMtl();
	for (int i = 0; i < NumMtl; ++i)
	{
		if (pMesh->MtlList[i].Name == MtlName)
		{
			MtlName = MtlName + "_";
			i = 0;
			continue;
		}				
	}
	m_MtlName = MtlName.c_str();
	UpdateData(FALSE);

	// ������ �ݻ����� �����ϰ�,
	m_cAmbientR.SetPos(m_AmbientR);
	m_cAmbientG.SetPos(m_AmbientG);
	m_cAmbientB.SetPos(m_AmbientB);

	m_cDiffuseR.SetPos(m_DiffuseR);
	m_cDiffuseG.SetPos(m_DiffuseG);
	m_cDiffuseB.SetPos(m_DiffuseB);

	m_cSpecularR.SetPos(m_SpecularR);
	m_cSpecularG.SetPos(m_SpecularG);
	m_cSpecularB.SetPos(m_SpecularB);
	UpdateData(FALSE);

	// ������ �����Ͽ� �߰��Ѵ�.
	GMaterial Mtl;
	Mtl.Name = m_MtlName;
	Mtl.SetAmbient(m_AmbientR / 255.0, m_AmbientG / 255.0, m_AmbientB / 255.0);
	Mtl.SetDiffuse(m_DiffuseR / 255.0, m_DiffuseG / 255.0, m_DiffuseB / 255.0);
	Mtl.SetSpecular(m_SpecularR / 255.0, m_SpecularG / 255.0, m_SpecularB / 255.0);
	pMesh->AddMtl(Mtl);

	// ���� �߰��� ������ �ε����� ���Ͽ�,
	m_CurrMtlIdx = pMesh->GetMtlIdx(MtlName);

	// �ﰢ�� ������ ���õ� �����,
	if (pEditor->GetClsNameEditObj() == "GFace")
	{
		// ���õ� �ﰢ���� ������ �����Ѵ�.
		int NumFace = pEditor->GetNumEditObj();
		for (int i = 0; i < NumFace; ++i)
		{
			GFace *f = (GFace *)(pEditor->EditObjList[i]);
			f->MtlIdx = m_CurrMtlIdx;
		}
	}
	// �޽��� ���õ� �����,
	else
	{
		// ��� �ﰢ���� ���� ������ �����Ѵ�.
		int NumFace = pMesh->GetNumFace();
		for (int i = 0; i < NumFace; ++i)
			pMesh->Faces[i].MtlIdx = m_CurrMtlIdx;

		// ������ ǥ���Ǵ� ��� ��쿡 ���Ͽ� ������ �����Ѵ�.
		pMesh->Renderer.CrvMtlIdx = m_CurrMtlIdx;
	}

	// �޽��� ������ ����Ǿ����� ����ϰ� �޽��� �����Ѵ�.
	pMesh->ModifiedFlag = MODIFIED_MTL;
	pMesh->UpdateMesh();
	pDoc->UpdateAllViews(NULL);	

	return TRUE; 
}

void PAddMtlDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData(TRUE);

	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh != NULL && m_CurrMtlIdx != -1)
	{
		pMesh->MtlList[m_CurrMtlIdx].SetAmbient(m_AmbientR / 255.0, m_AmbientG / 255.0, m_AmbientB / 255.0);
		pMesh->MtlList[m_CurrMtlIdx].SetDiffuse(m_DiffuseR / 255.0, m_DiffuseG / 255.0, m_DiffuseB / 255.0);
		pMesh->MtlList[m_CurrMtlIdx].SetSpecular(m_SpecularR / 255.0, m_SpecularG / 255.0, m_SpecularB / 255.0);
		pDoc->UpdateAllViews(NULL);
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void PAddMtlDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh != NULL && m_CurrMtlIdx != -1)
		pMesh->MtlList[m_CurrMtlIdx].Name = (CStringA)(m_MtlName);

	pEditor->EditMeshDlg.OnInitDialog();
	OnOK();
}

HBRUSH PAddMtlDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{ 
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	unsigned char *BC = pEditor->pScene->UIBkgColor;
	unsigned char *TC = pEditor->pScene->UItxtColor;
	pDC->SetBkColor(RGB(BC[0], BC[1], BC[2]));
	pDC->SetTextColor(RGB(TC[0], TC[1], TC[2]));
	pDC->SetBkMode(OPAQUE);
	static CBrush BkgBrush(RGB(BC[0], BC[1], BC[2]));
	hbr = (HBRUSH)BkgBrush;

	return hbr;
}

void PAddMtlDlg::OnBnClickedMtlRandom()
{
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh != NULL && m_CurrMtlIdx != -1)
	{
		srand(time(NULL));
		int diffuse[3];		
		diffuse[0] = rand() % 256;
		diffuse[1] = rand() % 256;
		diffuse[2] = rand() % 256;

		m_AmbientR = 50;			
		m_AmbientG = 50;
		m_AmbientB = 50;
		m_cAmbientR.SetPos(m_AmbientR);
		m_cAmbientG.SetPos(m_AmbientG);
		m_cAmbientB.SetPos(m_AmbientB);

		m_DiffuseR = diffuse[0];
		m_DiffuseG = diffuse[1];
		m_DiffuseB = diffuse[2];
		m_cDiffuseR.SetPos(m_DiffuseR);
		m_cDiffuseG.SetPos(m_DiffuseG);
		m_cDiffuseB.SetPos(m_DiffuseB);
		
		m_SpecularR = 178;
		m_SpecularG = 178;
		m_SpecularB = 178;
		m_cSpecularR.SetPos(m_SpecularR);
		m_cSpecularG.SetPos(m_SpecularG);
		m_cSpecularB.SetPos(m_SpecularB);

		pMesh->MtlList[m_CurrMtlIdx].SetAmbient(m_AmbientR / 255.0, m_AmbientG / 255.0, m_AmbientB / 255.0);
		pMesh->MtlList[m_CurrMtlIdx].SetDiffuse(m_DiffuseR / 255.0, m_DiffuseG / 255.0, m_DiffuseB / 255.0);
		pMesh->MtlList[m_CurrMtlIdx].SetSpecular(m_SpecularR / 255.0, m_SpecularG / 255.0, m_SpecularB / 255.0);
		pDoc->UpdateAllViews(NULL);
	}
}

void PAddMtlDlg::OnBnClickedOpenDiffuseTexture()
{
	GMesh *pMesh = pEditor->GetEditMesh();
	if (pMesh != NULL && m_CurrMtlIdx != -1)
	{
		// ���� ���� ��ȭ���ڸ� ����ϰ�,
		LPCTSTR filter = _T("BMP file(*.bmp)|*.bmp|TGA file(*.tga)|*.tga|JPG file(*.jpg)|*.jpg||");
		CFileDialog FileDlg(true, NULL, NULL, OFN_HIDEREADONLY, filter);
		if(FileDlg.DoModal() != IDOK) 
			return;

		// ����ڰ� ������ ������ ��θ�, ���� �̸� �� ���� �ε����� ���ϰ�,
		CString FilePath = FileDlg.GetPathName();	// ���� ��� �� �̸����� ����
		CString FileName = FileDlg.GetFileName();	// ���� �̸��� ����
		
		// ���� ������ ��θ� ���ͼ�, �۾� ���丮�� �����Ѵ�.
		std::string file_path = (CStringA)(FilePath);
		//file_path = get_file_path(file_path);
 		//_chdir(file_path.c_str());

		pMesh->MtlList[m_CurrMtlIdx].LoadTexture(file_path.c_str());
		pDoc->UpdateAllViews(NULL);
	}	
}
