#ifndef _GQUADTREE_H
#define _GQUADTREE_H

extern const int BITVECTOR[];
extern const double D_EPSILON_2;

class GQTree;

/*!
*	\class GQTreeNode
*	\brief 쿼드 트리의 노드를 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu) 
*	\date 10 Feb. 2014	
*/
class GQTreeNode
{
public:
	// 생성자 및 소멸자
	GQTreeNode(GVector2 *pBB, GQTree *pTree, bool init = false);       
	~GQTreeNode();

	// 멤버 함수
	void CreateSubRegion(int currentDepth);
	void AddFaceToSubRegion(int FaceIdx);
	void AddFaceToSubRegion(int FaceIdx, int RegionIdx);

	void FindTriangleIntersections(GVector2 *pFace, int FaceIdx);
	void FindIntersectionsWithX(GVector2 edge, GVector2 *pFace, int FaceIdx, int k);
	void FindIntersectionsWithY(GVector2 edge, GVector2 *pFace, int FaceIdx, int k);

	std::vector<int> &FindFaces(GVector2 p);

public:
	// 데이터 멤버
	/*! \brief 사각형의 바운딩 박스 */	
	GVector2 BndBox[2];

	/*! \brief 바운딩 박스의 중심 */	
	GVector2 Center;

	/*! \brief 트리에 대한 포인터 */	
	GQTree *pTree;

	/*! \brief 자식 노드에 대한 포인터 */	
	GQTreeNode *pSubRegion[4]; 

	/*! \brief 현재 영역에 포함된 삼각형의 인덱스 */	
	std::vector<int> FaceIdxs;

	/*! \brief 삼각형이 포함된 영역을 표시 */	
	char BitRegions;	
};   

/*!
*	\class GQTree
*	\brief 2차원 공간의 효율적인 탐색을 위한 쿼드 트리 클래스
*
*	\author 윤승현(shyun@dongguk.edu) 
*	\date 10 Feb. 2014	
*/
class GQTree
{
public:
	// 생성자 및 소멸자
	GQTree();
	GQTree(GMesh *_pMesh, int _Depth, int _MaxNumFace);
	~GQTree();

	// 멤버 함수
	std::vector<int> *FindFaces(GVector2 p);
	int FindFace(GVector2 p, double *alpha, double *beta);

public:
	// 데이터 멤버
	/*! \brief 쿼드 트리의 대상 메쉬에 대한 포인터 */	
	GMesh *pMesh;

	/*! \brief 루트 노드에 대한 포인터 */	
	GQTreeNode *pRoot;

	/*! \brief 노드에 포함될 최대 삼각형의 개수 */	
	int MaxNumFace;

	/*! \brief 트리의 최대 깊이 */	
	int Depth;	
};

#endif // _GQUADTREE_H