// PEditBlendSrfDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PEditBlendSrfDlg.h"


// PEditBlendSrfDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(PEditBlendSrfDlg, CDialog)

PEditBlendSrfDlg::PEditBlendSrfDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PEditBlendSrfDlg::IDD, pParent)
	, m_SNumU(0)
	, m_PatchType(0)
	, m_BlendOffset(0)
	, m_BlendOffsetE0(0)
	, m_BlendOffsetE1(0)
	, m_BlendOffsetE2(0)
{

}

PEditBlendSrfDlg::~PEditBlendSrfDlg()
{
}

void PEditBlendSrfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SNUM_U, m_SNumU);
	DDX_Radio(pDX, IDC_PATCH_TYPE_PN, m_PatchType);
	DDX_Control(pDX, IDC_BLEND_OFFSET, m_cBlendOffset);
	DDX_Control(pDX, IDC_BLEND_OFFSET_E0, m_cBlendOffsetE0);
	DDX_Control(pDX, IDC_BLEND_OFFSET_E1, m_cBlendOffsetE1);
	DDX_Control(pDX, IDC_BLEND_OFFSET_E2, m_cBlendOffsetE2);
	DDX_Slider(pDX, IDC_BLEND_OFFSET, m_BlendOffset);
	DDX_Slider(pDX, IDC_BLEND_OFFSET_E0, m_BlendOffsetE0);
	DDX_Slider(pDX, IDC_BLEND_OFFSET_E1, m_BlendOffsetE1);
	DDX_Slider(pDX, IDC_BLEND_OFFSET_E2, m_BlendOffsetE2);
	DDX_Control(pDX, IDC_SNUM_U_SPIN, m_cSNumU);
}


BEGIN_MESSAGE_MAP(PEditBlendSrfDlg, CDialog)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_EN_KILLFOCUS(IDC_SNUM_U, &PEditBlendSrfDlg::OnEnKillfocusSnumU)
	ON_BN_CLICKED(IDC_PATCH_TYPE_PN, &PEditBlendSrfDlg::OnBnClickedPatchTypePn)
	ON_BN_CLICKED(IDC_PATCH_TYPE_EXT_PN, &PEditBlendSrfDlg::OnBnClickedPatchTypeExtPn)
	ON_BN_CLICKED(IDC_PATCH_TYPE_FLAT, &PEditBlendSrfDlg::OnBnClickedPatchTypeFlat)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// PEditBlendSrfDlg �޽��� ó�����Դϴ�.

BOOL PEditBlendSrfDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	static bool bFirst = true;

	// ó�� ȣ��� �����,
	if (bFirst)
	{
		m_cSNumU.SetRange(2, 10000);
		m_cBlendOffset.SetRange(0, 100);
		m_cBlendOffsetE0.SetRange(0, 100);
		m_cBlendOffsetE1.SetRange(0, 100);
		m_cBlendOffsetE2.SetRange(0, 100);
		bFirst = false;
		return TRUE;
	}

	if (pEditor == NULL || pDoc == NULL)
		return TRUE;

	// ���� ����Ʈ ���� �����ϰ�,
	m_SNumU = 0;
	m_BlendOffset = 0;
	m_BlendOffsetE0 = 0;
	m_BlendOffsetE1 = 0;
	m_BlendOffsetE2 = 0;
	m_PatchType = 0;

	// ���� ������ ���� ���õ� ���� ��ü�� �ִٸ�,
	if (!pEditor->EditObjList.empty())
	{
		// ����Ʈ�� ù��° ��ü�� ���� �޽��� ������ �Ӽ��� ���,
		GMesh *pMesh = pEditor->EditObjList[0]->GetRefMesh();
		GRenderer attrib = pMesh->Renderer;

		// ������ �ɼ��� �缳���ϰ�,
		m_SNumU = attrib.NumSampU;
		
		// ���� �޽��� ���� �޽����,
		if (pMesh->SemanticType == MESH_CTRL)
		{
			// ���� �޽��� ���� �޽��� ã��,
			GBlendMesh *pBlendMesh = (GBlendMesh *)(FindMesh(pEditor->pScene->MeshList, pMesh->LinkedMeshNameList[0]));
			if (pBlendMesh != NULL)
			{
				int fidx = -1;
				GFace *pFace = NULL;

				// GFace�� ���õ� �����,
				if (pEditor->EditObjList[0]->GetClassName() == "GFace")
				{
					pFace = (GFace *)(pEditor->EditObjList[0]);
					fidx = pFace->Idx;
				}
				else	// // GFace�� ���õ� ��찡 �ƴ϶��,
				{
					pFace = &pMesh->Faces[0];
					fidx = 0;
				}

				// ���õ� �ﰢ���� ���� �������� �о�ͼ�,
				int pos0 = (int)(pBlendMesh->BlendOffset[fidx][0] * 100);
				int pos1 = (int)(pBlendMesh->BlendOffset[fidx][1] * 100);
				int pos2 = (int)(pBlendMesh->BlendOffset[fidx][2] * 100);

				// ���� ������ ���� �缳�� �ϰ�,
				m_BlendOffset = 0;
				m_BlendOffsetE0 = pos0;
				m_BlendOffsetE1 = pos1;
				m_BlendOffsetE2 = pos2;

				// ��ġ Ÿ���� �缳�� �Ѵ�.
				if (pBlendMesh->PatchType[fidx] & PATCH_TYPE_BZR_TRI)
					m_PatchType = 0;
				else if (pBlendMesh->PatchType[fidx] & PATCH_TYPE_BZR_TRI_EXT)
					m_PatchType = 1;
				else if (pBlendMesh->PatchType[fidx] & PATCH_TYPE_BZR_TRI_PLN)
					m_PatchType = 2;
			}
			else
				pMesh->LinkedMeshNameList.clear();
		}
	}

	UpdateData(FALSE);	

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void PEditBlendSrfDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData(true);
	if (pEditor->EditObjList.empty())
		return;

	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();

	pSltMesh->Renderer.NumSampU = m_SNumU;
	pSltMesh->ModifiedFlag = MODIFIED_SAMPLE_NUM;
	pSltMesh->UpdateMesh();
	
	pDoc->UpdateAllViews(NULL);

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void PEditBlendSrfDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pEditor->EditObjList.empty())
		return;

	// ��Ʈ���� ���� ������ �����ϰ�,
	UpdateData(TRUE);

	// ���� ��ü�� ���� �޽��� ���ϰ�,
	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();

	// ���� �޽��� ���� �޽��� ��쿡 ���Ͽ�,
	if (pSltMesh->SemanticType == MESH_CTRL)
	{
		// ���� �޽����� ������ ���� �޽��� ���ϰ�,
		GBlendMesh *pBlendMesh = (GBlendMesh *)(FindMesh(pEditor->pScene->MeshList, pSltMesh->LinkedMeshNameList[0]));

		if (pBlendMesh != NULL)
		{
			// ���� ��ü�� Ŭ���� �̸��� �����Ͽ�,
			std::string ClsName = pEditor->EditObjList[0]->GetClassName();

			if (pScrollBar->GetDlgCtrlID() == IDC_BLEND_OFFSET)
			{
				m_BlendOffsetE0 = m_BlendOffsetE1 = m_BlendOffsetE2 = m_BlendOffset;
				UpdateData(FALSE);
			}

			// �ﰢ���� ���õ� �����,
			if (ClsName == "GFace")
			{
				// ���õ� �ﰢ���� ���� �������� �����Ѵ�.
				GFace *pFace = (GFace *)(pEditor->EditObjList[0]);
				pBlendMesh->BlendOffset[pFace->Idx][0] = m_BlendOffsetE0 / 100.0;
				pBlendMesh->BlendOffset[pFace->Idx][1] = m_BlendOffsetE1 / 100.0;
				pBlendMesh->BlendOffset[pFace->Idx][2] = m_BlendOffsetE2 / 100.0;				
			}
			// ���� �޽� ��ü�� ���õ� �����,
			else
			{
				// ��� �ﰢ���� ���� �������� �����ϰ� �����Ѵ�.
				int NumFace = pSltMesh->GetNumFace();
				for (int i = 0; i < NumFace; ++i)
				{
					pBlendMesh->BlendOffset[i][0] = m_BlendOffsetE0 / 100.0;
					pBlendMesh->BlendOffset[i][1] = m_BlendOffsetE1 / 100.0;
					pBlendMesh->BlendOffset[i][2] = m_BlendOffsetE2 / 100.0;
				}
			}

			// ���� �޽��� ������ ���� �޽��� �����Ѵ�.
			pBlendMesh->CreateMesh();
		}
		else
			pSltMesh->LinkedMeshNameList.clear();		
	}

	pDoc->UpdateAllViews(NULL);	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void PEditBlendSrfDlg::OnEnKillfocusSnumU()
{
	UpdateData(true);
	if (pEditor->EditObjList.empty())
		return;

	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh(); 
	pSltMesh->Renderer.NumSampU = m_SNumU;
	pSltMesh->ModifiedFlag = MODIFIED_SAMPLE_NUM;
	pSltMesh->UpdateMesh();
	
	pDoc->UpdateAllViews(NULL);
}

void PEditBlendSrfDlg::OnBnClickedPatchTypePn()
{
	if (pEditor->EditObjList.empty())
		return;

	// ���� �޽��� ���Ͽ�,
	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();

	// ���� �޽��� ���� �޽��� ��쿡 ���Ͽ�,
	if (pSltMesh->SemanticType == MESH_CTRL)
	{
		// ���� �޽����� ������ ���� �޽��� ���ϰ�,
		GBlendMesh *pBlendMesh = (GBlendMesh *)(FindMesh(pEditor->pScene->MeshList, pSltMesh->LinkedMeshNameList[0]));

		if (pBlendMesh != NULL)
		{
			// ���� ��ü�� Ŭ���� �̸��� �����Ͽ�,
			std::string ClsName = pEditor->EditObjList[0]->GetClassName();

			// �ﰢ���� ���õ� �����,
			if (ClsName == "GFace")
			{
				// ���õ� �ﰢ���� ��ġ Ÿ���� �����Ѵ�.
				GFace *pFace = (GFace *)(pEditor->EditObjList[0]);
				if (!(pBlendMesh->PatchType[pFace->Idx] & PATCH_TYPE_BZR_TRI))
					pBlendMesh->PatchType[pFace->Idx] = (PATCH_TYPE_MODIFIED | PATCH_TYPE_BZR_TRI);
			}
			// ���� �޽� ��ü�� ���õ� �����,
			else
			{
				// ��� �ﰢ���� ��ġ Ÿ���� �����Ѵ�.
				int NumPatch = (int)pBlendMesh->PatchList.size();
				for (int i = 0; i < NumPatch; ++i)
				{
					if (!(pBlendMesh->PatchType[i] & PATCH_TYPE_BZR_TRI))
						pBlendMesh->PatchType[i] = (PATCH_TYPE_MODIFIED | PATCH_TYPE_BZR_TRI);
				}
			}

			// ���� �޽��� ������ ���� �޽��� �����Ѵ�.
			pBlendMesh->CreateMesh();
		}
		else
			pSltMesh->LinkedMeshNameList.clear();		
	}

	pDoc->UpdateAllViews(NULL);
}

void PEditBlendSrfDlg::OnBnClickedPatchTypeExtPn()
{
	if (pEditor->EditObjList.empty())
		return;

	// ���� �޽��� ���Ͽ�,
	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();

	// ���� �޽��� ��쿡 ���Ͽ�,
	if (pSltMesh->SemanticType == MESH_CTRL)
	{
		// ���� �޽����� ������ ���� �޽��� ���ϰ�,
		GBlendMesh *pBlendMesh = (GBlendMesh *)(FindMesh(pEditor->pScene->MeshList, pSltMesh->LinkedMeshNameList[0]));

		if (pBlendMesh != NULL)
		{
			// ���� ��ü�� Ŭ���� �̸��� �����Ͽ�,
			std::string ClsName = pEditor->EditObjList[0]->GetClassName();

			// �ﰢ���� ���õ� �����,
			if (ClsName == "GFace")
			{
				// ���õ� �ﰢ���� ��ġ Ÿ���� �����Ѵ�.
				GFace *pFace = (GFace *)(pEditor->EditObjList[0]);
				if (!(pBlendMesh->PatchType[pFace->Idx] & PATCH_TYPE_BZR_TRI_EXT))
					pBlendMesh->PatchType[pFace->Idx] = (PATCH_TYPE_MODIFIED | PATCH_TYPE_BZR_TRI_EXT);
			}
			// ���� �޽� ��ü�� ���õ� �����,
			else
			{
				// ��� �ﰢ���� ��ġ Ÿ���� �����Ѵ�.
				int NumPatch = (int)pBlendMesh->PatchList.size();
				for (int i = 0; i < NumPatch; ++i)
				{
					if (!(pBlendMesh->PatchType[i] & PATCH_TYPE_BZR_TRI_EXT))
						pBlendMesh->PatchType[i] = (PATCH_TYPE_MODIFIED | PATCH_TYPE_BZR_TRI_EXT);
				}
			}

			// ���� �޽��� ������ ���� �޽��� �����Ѵ�.
			pBlendMesh->CreateMesh();

		}
		else
			pSltMesh->LinkedMeshNameList.clear();
	}

	pDoc->UpdateAllViews(NULL);
}

void PEditBlendSrfDlg::OnBnClickedPatchTypeFlat()
{
	if (pEditor->EditObjList.empty())
		return;

	// ���� �޽��� ���Ͽ�,
	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();

	// ���� �޽��� ��쿡 ���Ͽ�,
	if (pSltMesh->SemanticType == MESH_CTRL)
	{
		// ���� �޽����� ������ ���� �޽��� ���ϰ�,
		GBlendMesh *pBlendMesh = (GBlendMesh *)(FindMesh(pEditor->pScene->MeshList, pSltMesh->LinkedMeshNameList[0]));

		if (pBlendMesh != NULL)
		{
			// ���� ��ü�� Ŭ���� �̸��� �����Ͽ�,
			std::string ClsName = pEditor->EditObjList[0]->GetClassName();

			// �ﰢ���� ���õ� �����,
			if (ClsName == "GFace")
			{
				// ���õ� �ﰢ���� ��ġ Ÿ���� �����Ѵ�.
				GFace *pFace = (GFace *)(pEditor->EditObjList[0]);
				if (!(pBlendMesh->PatchType[pFace->Idx] & PATCH_TYPE_BZR_TRI_PLN))
					pBlendMesh->PatchType[pFace->Idx] = (PATCH_TYPE_MODIFIED | PATCH_TYPE_BZR_TRI_PLN);
			}
			// ���� �޽� ��ü�� ���õ� �����,
			else
			{
				// ��� �ﰢ���� ��ġ Ÿ���� �����Ѵ�.
				int NumPatch = (int)pBlendMesh->PatchList.size();
				for (int i = 0; i < NumPatch; ++i)
				{
					if (!(pBlendMesh->PatchType[i] & PATCH_TYPE_BZR_TRI_PLN))
						pBlendMesh->PatchType[i] = (PATCH_TYPE_MODIFIED | PATCH_TYPE_BZR_TRI_PLN);
				}
			}

			// ���� �޽��� ������ ���� �޽��� �����Ѵ�.
			pBlendMesh->CreateMesh();
		}
		else
			pSltMesh->LinkedMeshNameList.clear();
	}

	pDoc->UpdateAllViews(NULL);
}

HBRUSH PEditBlendSrfDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
