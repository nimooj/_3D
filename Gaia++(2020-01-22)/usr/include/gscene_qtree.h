#ifndef _GQUADTREE_H
#define _GQUADTREE_H

extern const int BITVECTOR[];
extern const double D_EPSILON_2;

class GQTree;

/*!
*	\class GQTreeNode
*	\brief ���� Ʈ���� ��带 ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu) 
*	\date 10 Feb. 2014	
*/
class GQTreeNode
{
public:
	// ������ �� �Ҹ���
	GQTreeNode(GVector2 *pBB, GQTree *pTree, bool init = false);       
	~GQTreeNode();

	// ��� �Լ�
	void CreateSubRegion(int currentDepth);
	void AddFaceToSubRegion(int FaceIdx);
	void AddFaceToSubRegion(int FaceIdx, int RegionIdx);

	void FindTriangleIntersections(GVector2 *pFace, int FaceIdx);
	void FindIntersectionsWithX(GVector2 edge, GVector2 *pFace, int FaceIdx, int k);
	void FindIntersectionsWithY(GVector2 edge, GVector2 *pFace, int FaceIdx, int k);

	std::vector<int> &FindFaces(GVector2 p);

public:
	// ������ ���
	/*! \brief �簢���� �ٿ�� �ڽ� */	
	GVector2 BndBox[2];

	/*! \brief �ٿ�� �ڽ��� �߽� */	
	GVector2 Center;

	/*! \brief Ʈ���� ���� ������ */	
	GQTree *pTree;

	/*! \brief �ڽ� ��忡 ���� ������ */	
	GQTreeNode *pSubRegion[4]; 

	/*! \brief ���� ������ ���Ե� �ﰢ���� �ε��� */	
	std::vector<int> FaceIdxs;

	/*! \brief �ﰢ���� ���Ե� ������ ǥ�� */	
	char BitRegions;	
};   

/*!
*	\class GQTree
*	\brief 2���� ������ ȿ������ Ž���� ���� ���� Ʈ�� Ŭ����
*
*	\author ������(shyun@dongguk.edu) 
*	\date 10 Feb. 2014	
*/
class GQTree
{
public:
	// ������ �� �Ҹ���
	GQTree();
	GQTree(GMesh *_pMesh, int _Depth, int _MaxNumFace);
	~GQTree();

	// ��� �Լ�
	std::vector<int> *FindFaces(GVector2 p);
	int FindFace(GVector2 p, double *alpha, double *beta);

public:
	// ������ ���
	/*! \brief ���� Ʈ���� ��� �޽��� ���� ������ */	
	GMesh *pMesh;

	/*! \brief ��Ʈ ��忡 ���� ������ */	
	GQTreeNode *pRoot;

	/*! \brief ��忡 ���Ե� �ִ� �ﰢ���� ���� */	
	int MaxNumFace;

	/*! \brief Ʈ���� �ִ� ���� */	
	int Depth;	
};

#endif // _GQUADTREE_H