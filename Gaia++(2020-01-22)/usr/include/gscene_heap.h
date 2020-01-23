#ifndef GHEAP_H
#define GHEAP_H

#include <queue>
#include <functional>

/*!
*	\class GHeapNode
*	\brief �� �ڷᱸ���� ��带 ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 19 December 2015
*/
class GHeapNode 
{
public:
	// ������ �� �Ҹ���
	GHeapNode(int _idx, double _key): idx(_idx), key(_key) {}
	~GHeapNode() {}

	// ��� ������
	bool operator >(const GHeapNode &rhs) const {return (key > rhs.key);}
	bool operator >=(const GHeapNode &rhs) const {return (key >= rhs.key);}
	bool operator <(const GHeapNode &rhs) const {return (key < rhs.key);}
	bool operator <=(const GHeapNode &rhs) const {return (key <= rhs.key);}

public:
	// ������ ���
	/*! \brief ������ �ε��� */	
	int   idx;

	/*! \brief ���������� ���������� �Ÿ� */	
	double  key;
};

/*!
*	\class GHeap
*	\brief �� �ڷᱸ���� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 19 December 2015
*/
class GHeap
{
	typedef std::priority_queue<GHeapNode, std::vector<GHeapNode>, std::greater<GHeapNode>> Heap_t;
public:
	// ������ �� �Ҹ���
	GHeap();
	GHeap(std::vector<double> *_keys);
	~GHeap();

	// ��� �Լ�
	void initialize(std::vector<double> *_keys);
	void push(int idx);
	void pop();
	int top();
	void resize(int size);
	bool is_in_heap(int idx) const;
	void remove_from_heap(int idx);
	bool empty();
	int size() const;
	int get_candidate();
	
public:
	// ������ ���
	/*! \brief �� �ڷᱸ�� */
	Heap_t heap;

	/*! \brief �߰� ������ ���� �÷��� */
	std::vector<bool> flags;

	/*! \brief Ű�� ���� ������ */
	std::vector<double> *keys;
};

#endif // GHEAP_H
