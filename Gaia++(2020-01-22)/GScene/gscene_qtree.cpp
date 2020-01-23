#include "stdafx.h"

const int BITVECTOR[] = {0x1, 0x2, 0x4, 0x8};
const double D_EPSILON_2 = 1e-6;

GQTreeNode::GQTreeNode(GVector2 *pBB, GQTree *_pTree, bool init)
{
	pTree = _pTree;
	Center = (pBB[0] + pBB[1]) / 2.0;
	BndBox[0] = pBB[0];
	BndBox[1] = pBB[1];
	pSubRegion[0] = NULL;
	pSubRegion[1] = NULL;
	pSubRegion[2] = NULL;
	pSubRegion[3] = NULL;
	BitRegions = 0;

	if (init)
	{
		int NumFace = (int)(pTree->pMesh->Faces.size());
		for (int i = 0; i < NumFace; ++i)
			FaceIdxs.push_back(i);
	}
}

GQTreeNode::~GQTreeNode()
{
	for (int i = 0; i < 4; i++)
		delete pSubRegion[i]; 
}

std::vector<int> &GQTreeNode::FindFaces(GVector2 p)
{
	int region = 2;

	if (p[1] < Center[1])
		region = 0;

	if (p[0] > Center[0])
		region++;

	if (!pSubRegion[region])
		return FaceIdxs;

	return pSubRegion[region]->FindFaces(p);
}

void GQTreeNode::CreateSubRegion(int currentDepth)
{
	GVector2 boundingBox[4][2];

	// 1. region south west
	boundingBox[0][0][0] = BndBox[0][0];
	boundingBox[0][0][1] = BndBox[0][1];
	boundingBox[0][1][0] = Center[0];
	boundingBox[0][1][1] = Center[1];

	// 2. region south east
	boundingBox[1][0][0] = Center[0];
	boundingBox[1][0][1] = BndBox[0][1];
	boundingBox[1][1][0] = BndBox[1][0];
	boundingBox[1][1][1] = Center[1];

	// 3. region north west
	boundingBox[2][0][0] = BndBox[0][0];
	boundingBox[2][0][1] = Center[1];
	boundingBox[2][1][0] = Center[0];
	boundingBox[2][1][1] = BndBox[1][1];

	// 4. region north east
	boundingBox[3][0][0] = Center[0];
	boundingBox[3][0][1] = Center[1];
	boundingBox[3][1][0] = BndBox[1][0];
	boundingBox[3][1][1] = BndBox[1][1];   

	for (int i = 0; i < 4; i++)
		pSubRegion[i] = new GQTreeNode(boundingBox[i], pTree); 

	// find at least one region for each triangle 
	int NumFace = (int)FaceIdxs.size();
	for (int i = 0; i < NumFace; ++i)
		AddFaceToSubRegion(FaceIdxs[i]);
	
	FaceIdxs.clear();

	for (int i = 0; i < 4; i++)
	{
		// check conditions for region subdivision
		if ((int)pSubRegion[i]->FaceIdxs.size() > pTree->MaxNumFace && currentDepth < pTree->Depth)
			pSubRegion[i]->CreateSubRegion(currentDepth + 1);
	}
}

void GQTreeNode::AddFaceToSubRegion(int FaceIdx)
{
	BitRegions = 0;

	int vidx0 = pTree->pMesh->Faces[FaceIdx].vIdx[0];
	int vidx1 = pTree->pMesh->Faces[FaceIdx].vIdx[1];
	int vidx2 = pTree->pMesh->Faces[FaceIdx].vIdx[2];

	GVector2 face[3];
	face[0].Set(pTree->pMesh->Verts[vidx0].P[0], pTree->pMesh->Verts[vidx0].P[1]);
	face[1].Set(pTree->pMesh->Verts[vidx1].P[0], pTree->pMesh->Verts[vidx1].P[1]);
	face[2].Set(pTree->pMesh->Verts[vidx2].P[0], pTree->pMesh->Verts[vidx2].P[1]);
	
	// 영역의 중심이 삼각형에 포함된 경우(즉, 삼각형이 모든 영역에 포함됨)
	if (pt_in_triangle(Center, face[0], face[1], face[2]))
	{
		AddFaceToSubRegion(FaceIdx, 0);
		AddFaceToSubRegion(FaceIdx, 1);
		AddFaceToSubRegion(FaceIdx, 2);
		AddFaceToSubRegion(FaceIdx, 3);
		return;
	}

	FindTriangleIntersections(face, FaceIdx);

	if (!BitRegions)
	{
		// we didn't find any intersection so we add this triangle to a point's region		
		int region = 2;
		if (face[0][1] < Center[1])
			region = 0;

		if (face[0][0] > Center[0])
			region++;

		pSubRegion[region]->FaceIdxs.push_back(FaceIdx);		
	}
}

void GQTreeNode::AddFaceToSubRegion(int FaceIdx, int RegionIdx)
{
	if (!(BitRegions & BITVECTOR[RegionIdx]))
	{         
		pSubRegion[RegionIdx]->FaceIdxs.push_back(FaceIdx);
		BitRegions |= BITVECTOR[RegionIdx];
	}
}


void GQTreeNode::FindTriangleIntersections(GVector2 *pFace, int FaceIdx)
{
	int k = 2;	   
	GVector2 edge;
	// Iterate through all triangle laterals and find bounding box intersections
	for (int i = 0; i < 3; k = i++)
	{         
		edge = pFace[i] - pFace[k];     

		if (edge[0])
			FindIntersectionsWithX(edge, pFace, FaceIdx, k);
		
		if (edge[1]) 
			FindIntersectionsWithY(edge, pFace, FaceIdx, k);
	}
}

void GQTreeNode::FindIntersectionsWithX(GVector2 edge, GVector2 *pFace, int FaceIdx, int k)
{
	// find intersection with plane x = Center[0]
	double t = (Center[0] - pFace[k][0]) / (edge[0]);
	if (t < (1 + D_EPSILON_2) && t > -D_EPSILON_2)
	{
		// we have an intersection
		double yComponent = pFace[k][1] + t * (edge[1]);

		if (yComponent < Center[1])
		{
			if (yComponent >= BndBox[0][1])
			{					
				AddFaceToSubRegion(FaceIdx, 0);
				AddFaceToSubRegion(FaceIdx, 1);
			}
		}
		else if (yComponent <= BndBox[1][1])
		{
			AddFaceToSubRegion(FaceIdx, 2);
			AddFaceToSubRegion(FaceIdx, 3);
		}
	}

	// find intersection with plane x = BndBox[0][0]
	t = (BndBox[0][0] - pFace[k][0]) / (edge[0]);
	if (t < (1 + D_EPSILON_2) && t > -D_EPSILON_2)
	{
		// we have an intersection
		double yComponent = pFace[k][1] + t * (edge[1]);

		if (yComponent <= Center[1] && yComponent >= BndBox[0][1])
		{
			AddFaceToSubRegion(FaceIdx, 0);
		}
		else if (yComponent >= Center[1] && yComponent <= BndBox[1][1])
		{
			AddFaceToSubRegion(FaceIdx, 2);			
		}
	}

	// find intersection with plane x = BndBox[1][0]
	t = (BndBox[1][0] - pFace[k][0]) / (edge[0]);
	if (t < (1 + D_EPSILON_2) && t > -D_EPSILON_2)
	{
		// we have an intersection
		double yComponent = pFace[k][1] + t * (edge[1]);

		if (yComponent <= Center[1] && yComponent >= BndBox[0][1])
		{
			AddFaceToSubRegion(FaceIdx, 1);
		}
		else if (yComponent >= Center[1] && yComponent <= BndBox[1][1])
		{
			AddFaceToSubRegion(FaceIdx, 3);
		}
	}
}

void GQTreeNode::FindIntersectionsWithY(GVector2 edge, GVector2 *pFace, int FaceIdx, int k)
{
	// find intersection with plane y = Center[1]
	double t = (Center[1] - pFace[k][1]) / (edge[1]);
	if (t < (1 + D_EPSILON_2) && t > -D_EPSILON_2)
	{
		// we have an intersection
		double xComponent = pFace[k][0] + t * (edge[0]);

		if (xComponent > Center[0])
		{
			if (xComponent <= BndBox[1][0])
			{
				AddFaceToSubRegion(FaceIdx, 1);
				AddFaceToSubRegion(FaceIdx, 3);
			}
		}
		else if (xComponent >= BndBox[0][0])
		{
			AddFaceToSubRegion(FaceIdx, 0);
			AddFaceToSubRegion(FaceIdx, 2);
		}
	}

	// find intersection with plane y = BndBox[0][1]
	t = (BndBox[0][1] - pFace[k][1]) / (edge[1]);
	if (t < (1 + D_EPSILON_2) && t > -D_EPSILON_2)
	{
		// we have an intersection
		double xComponent = pFace[k][0] + t * (edge[0]);

		if (xComponent <= Center[0] && xComponent >= BndBox[0][0])
		{
			AddFaceToSubRegion(FaceIdx, 0);
		}
		else if (xComponent >= Center[0] && xComponent <= BndBox[1][0])
		{
			AddFaceToSubRegion(FaceIdx, 1);
		}
	}

	// find intersection with plane y = BndBox[1][1]
	t = (BndBox[1][1] - pFace[k][1]) / (edge[1]);
	if (t < (1 + D_EPSILON_2) && t > -D_EPSILON_2)
	{
		// we have an intersection
		double xComponent = pFace[k][0] + t * (edge[0]);

		if (xComponent <= Center[0] && xComponent >= BndBox[0][0])
		{
			AddFaceToSubRegion(FaceIdx, 2);
		}
		else if (xComponent >= Center[0] && xComponent <= BndBox[1][0])
		{
			AddFaceToSubRegion(FaceIdx, 3);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
GQTree::GQTree()
{
	pRoot = NULL;
	pMesh = NULL;
	MaxNumFace = 0;
	Depth = 0;
}

GQTree::GQTree(GMesh *_pMesh, int _Depth, int _MaxNumFace)
{
	pMesh = _pMesh;
	Depth = _Depth;
	MaxNumFace = _MaxNumFace;

	GVector2 BndBox[2];
	BndBox[0].Set(pMesh->BBox.MinPt[0], pMesh->BBox.MinPt[1]);
	BndBox[1].Set(pMesh->BBox.MaxPt[0], pMesh->BBox.MaxPt[1]);

	pRoot = new GQTreeNode(BndBox, this, true);
	if ((int)pRoot->FaceIdxs.size() > MaxNumFace)
		pRoot->CreateSubRegion(1);
}

GQTree::~GQTree()
{
	if (pRoot != NULL)
		delete pRoot;
	pRoot = NULL;
}

std::vector<int> *GQTree::FindFaces(GVector2 p)
{
	GVector2 p1 = pRoot->BndBox[0];
	GVector2 p2 = pRoot->BndBox[1];

	if (p[0] >= p1[0] && p[0] <= p2[0] && p[1] >= p1[1] && p[1] <= p2[1])
		return &(pRoot->FindFaces(p));
	else
		return NULL;
}

int GQTree::FindFace(GVector2 p, double *alpha, double *beta) 
{
	std::vector<int> *pFaces = FindFaces(p);
	if (pFaces == NULL)
		return -1;

	double u = p[0];
	double v = p[1];
	*alpha = 0.0;
	*beta = 0.0;
	
	int NumFace = (int)pFaces->size();
	for (int i = 0; i < NumFace; ++i)
	{
		int fidx = pFaces->at(i);
		double x1 = pMesh->Verts[pMesh->Faces[fidx].vIdx[0]].P[0];
		double y1 = pMesh->Verts[pMesh->Faces[fidx].vIdx[0]].P[1];
		double x2 = pMesh->Verts[pMesh->Faces[fidx].vIdx[1]].P[0];
		double y2 = pMesh->Verts[pMesh->Faces[fidx].vIdx[1]].P[1];
		double x3 = pMesh->Verts[pMesh->Faces[fidx].vIdx[2]].P[0];
		double y3 = pMesh->Verts[pMesh->Faces[fidx].vIdx[2]].P[1];
		double a = x2 - x1;
		double b = x3 - x1;
		double c = y2 - y1;
		double d = y3 - y1;
		double e = u - x1;
		double f = v - y1;

		double _alpha = (d * e - b * f) / (a * d - b * c);
		double _beta = (-c * e + a * f) / (a * d - b * c);

		if (_alpha >= 0.0 && _alpha <= 1.0 && _beta >= 0.0 && _beta <= 1.0 && (_alpha + _beta) <= 1.0)
		{
			*alpha = _alpha;
			*beta = _beta;
			return fidx;
		}
	}
	return -1;
}



