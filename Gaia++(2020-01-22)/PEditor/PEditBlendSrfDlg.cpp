// PEditBlendSrfDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PEditBlendSrfDlg.h"


// PEditBlendSrfDlg 대화 상자입니다.

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


// PEditBlendSrfDlg 메시지 처리기입니다.

BOOL PEditBlendSrfDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	static bool bFirst = true;

	// 처음 호출된 경우라면,
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

	// 먼저 디폴트 값을 설정하고,
	m_SNumU = 0;
	m_BlendOffset = 0;
	m_BlendOffsetE0 = 0;
	m_BlendOffsetE1 = 0;
	m_BlendOffsetE2 = 0;
	m_PatchType = 0;

	// 편집 도구에 의해 선택된 편집 객체가 있다면,
	if (!pEditor->EditObjList.empty())
	{
		// 리스트의 첫번째 객체가 속한 메쉬와 렌더링 속성을 얻어,
		GMesh *pMesh = pEditor->EditObjList[0]->GetRefMesh();
		GRenderer attrib = pMesh->Renderer;

		// 렌더링 옵션을 재설정하고,
		m_SNumU = attrib.NumSampU;
		
		// 참조 메쉬가 제어 메쉬라면,
		if (pMesh->SemanticType == MESH_CTRL)
		{
			// 제어 메쉬의 블렌딩 메쉬를 찾고,
			GBlendMesh *pBlendMesh = (GBlendMesh *)(FindMesh(pEditor->pScene->MeshList, pMesh->LinkedMeshNameList[0]));
			if (pBlendMesh != NULL)
			{
				int fidx = -1;
				GFace *pFace = NULL;

				// GFace가 선택된 경우라면,
				if (pEditor->EditObjList[0]->GetClassName() == "GFace")
				{
					pFace = (GFace *)(pEditor->EditObjList[0]);
					fidx = pFace->Idx;
				}
				else	// // GFace가 선택된 경우가 아니라면,
				{
					pFace = &pMesh->Faces[0];
					fidx = 0;
				}

				// 선택된 삼각형의 블렌딩 오프셋을 읽어와서,
				int pos0 = (int)(pBlendMesh->BlendOffset[fidx][0] * 100);
				int pos1 = (int)(pBlendMesh->BlendOffset[fidx][1] * 100);
				int pos2 = (int)(pBlendMesh->BlendOffset[fidx][2] * 100);

				// 블렌딩 오프셋 값을 재설정 하고,
				m_BlendOffset = 0;
				m_BlendOffsetE0 = pos0;
				m_BlendOffsetE1 = pos1;
				m_BlendOffsetE2 = pos2;

				// 패치 타입을 재설정 한다.
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
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
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

	// 컨트롤의 값을 변수에 대입하고,
	UpdateData(TRUE);

	// 선택 객체의 참조 메쉬를 구하고,
	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();

	// 참조 메쉬가 제어 메쉬인 경우에 한하여,
	if (pSltMesh->SemanticType == MESH_CTRL)
	{
		// 제어 메쉬에서 생성된 블렌딩 메쉬를 구하고,
		GBlendMesh *pBlendMesh = (GBlendMesh *)(FindMesh(pEditor->pScene->MeshList, pSltMesh->LinkedMeshNameList[0]));

		if (pBlendMesh != NULL)
		{
			// 선택 객체의 클래스 이름을 조사하여,
			std::string ClsName = pEditor->EditObjList[0]->GetClassName();

			if (pScrollBar->GetDlgCtrlID() == IDC_BLEND_OFFSET)
			{
				m_BlendOffsetE0 = m_BlendOffsetE1 = m_BlendOffsetE2 = m_BlendOffset;
				UpdateData(FALSE);
			}

			// 삼각형이 선택된 경우라면,
			if (ClsName == "GFace")
			{
				// 선택된 삼각형의 블렌딩 오프셋을 설정한다.
				GFace *pFace = (GFace *)(pEditor->EditObjList[0]);
				pBlendMesh->BlendOffset[pFace->Idx][0] = m_BlendOffsetE0 / 100.0;
				pBlendMesh->BlendOffset[pFace->Idx][1] = m_BlendOffsetE1 / 100.0;
				pBlendMesh->BlendOffset[pFace->Idx][2] = m_BlendOffsetE2 / 100.0;				
			}
			// 제어 메쉬 자체가 선택된 경우라면,
			else
			{
				// 모든 삼각형의 블렌딩 오프셋을 동일하게 설정한다.
				int NumFace = pSltMesh->GetNumFace();
				for (int i = 0; i < NumFace; ++i)
				{
					pBlendMesh->BlendOffset[i][0] = m_BlendOffsetE0 / 100.0;
					pBlendMesh->BlendOffset[i][1] = m_BlendOffsetE1 / 100.0;
					pBlendMesh->BlendOffset[i][2] = m_BlendOffsetE2 / 100.0;
				}
			}

			// 제어 메쉬로 생성된 블렌딩 메쉬를 갱신한다.
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

	// 참조 메쉬를 구하여,
	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();

	// 참조 메쉬가 제어 메쉬인 경우에 한하여,
	if (pSltMesh->SemanticType == MESH_CTRL)
	{
		// 제어 메쉬에서 생성된 블렌딩 메쉬를 구하고,
		GBlendMesh *pBlendMesh = (GBlendMesh *)(FindMesh(pEditor->pScene->MeshList, pSltMesh->LinkedMeshNameList[0]));

		if (pBlendMesh != NULL)
		{
			// 선택 객체의 클래스 이름을 조사하여,
			std::string ClsName = pEditor->EditObjList[0]->GetClassName();

			// 삼각형이 선택된 경우라면,
			if (ClsName == "GFace")
			{
				// 선택된 삼각형의 패치 타입을 설정한다.
				GFace *pFace = (GFace *)(pEditor->EditObjList[0]);
				if (!(pBlendMesh->PatchType[pFace->Idx] & PATCH_TYPE_BZR_TRI))
					pBlendMesh->PatchType[pFace->Idx] = (PATCH_TYPE_MODIFIED | PATCH_TYPE_BZR_TRI);
			}
			// 참조 메쉬 자체가 선택된 경우라면,
			else
			{
				// 모든 삼각형의 패치 타입을 설정한다.
				int NumPatch = (int)pBlendMesh->PatchList.size();
				for (int i = 0; i < NumPatch; ++i)
				{
					if (!(pBlendMesh->PatchType[i] & PATCH_TYPE_BZR_TRI))
						pBlendMesh->PatchType[i] = (PATCH_TYPE_MODIFIED | PATCH_TYPE_BZR_TRI);
				}
			}

			// 제어 메쉬로 생성된 서브 메쉬를 갱신한다.
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

	// 참조 메쉬를 구하여,
	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();

	// 제어 메쉬인 경우에 한하여,
	if (pSltMesh->SemanticType == MESH_CTRL)
	{
		// 제어 메쉬에서 생성된 블렌딩 메쉬를 구하고,
		GBlendMesh *pBlendMesh = (GBlendMesh *)(FindMesh(pEditor->pScene->MeshList, pSltMesh->LinkedMeshNameList[0]));

		if (pBlendMesh != NULL)
		{
			// 선택 객체의 클래스 이름을 조사하여,
			std::string ClsName = pEditor->EditObjList[0]->GetClassName();

			// 삼각형이 선택된 경우라면,
			if (ClsName == "GFace")
			{
				// 선택된 삼각형의 패치 타입을 설정한다.
				GFace *pFace = (GFace *)(pEditor->EditObjList[0]);
				if (!(pBlendMesh->PatchType[pFace->Idx] & PATCH_TYPE_BZR_TRI_EXT))
					pBlendMesh->PatchType[pFace->Idx] = (PATCH_TYPE_MODIFIED | PATCH_TYPE_BZR_TRI_EXT);
			}
			// 참조 메쉬 자체가 선택된 경우라면,
			else
			{
				// 모든 삼각형의 패치 타입을 설정한다.
				int NumPatch = (int)pBlendMesh->PatchList.size();
				for (int i = 0; i < NumPatch; ++i)
				{
					if (!(pBlendMesh->PatchType[i] & PATCH_TYPE_BZR_TRI_EXT))
						pBlendMesh->PatchType[i] = (PATCH_TYPE_MODIFIED | PATCH_TYPE_BZR_TRI_EXT);
				}
			}

			// 제어 메쉬로 생성된 서브 메쉬를 갱신한다.
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

	// 참조 메쉬를 구하여,
	GMesh *pSltMesh = pEditor->EditObjList[0]->GetRefMesh();

	// 제어 메쉬인 경우에 한하여,
	if (pSltMesh->SemanticType == MESH_CTRL)
	{
		// 제어 메쉬에서 생성된 블렌딩 메쉬를 구하고,
		GBlendMesh *pBlendMesh = (GBlendMesh *)(FindMesh(pEditor->pScene->MeshList, pSltMesh->LinkedMeshNameList[0]));

		if (pBlendMesh != NULL)
		{
			// 선택 객체의 클래스 이름을 조사하여,
			std::string ClsName = pEditor->EditObjList[0]->GetClassName();

			// 삼각형이 선택된 경우라면,
			if (ClsName == "GFace")
			{
				// 선택된 삼각형의 패치 타입을 설정한다.
				GFace *pFace = (GFace *)(pEditor->EditObjList[0]);
				if (!(pBlendMesh->PatchType[pFace->Idx] & PATCH_TYPE_BZR_TRI_PLN))
					pBlendMesh->PatchType[pFace->Idx] = (PATCH_TYPE_MODIFIED | PATCH_TYPE_BZR_TRI_PLN);
			}
			// 참조 메쉬 자체가 선택된 경우라면,
			else
			{
				// 모든 삼각형의 패치 타입을 설정한다.
				int NumPatch = (int)pBlendMesh->PatchList.size();
				for (int i = 0; i < NumPatch; ++i)
				{
					if (!(pBlendMesh->PatchType[i] & PATCH_TYPE_BZR_TRI_PLN))
						pBlendMesh->PatchType[i] = (PATCH_TYPE_MODIFIED | PATCH_TYPE_BZR_TRI_PLN);
				}
			}

			// 제어 메쉬로 생성된 서브 메쉬를 갱신한다.
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
