#ifndef GHEAP_H
#define GHEAP_H

#include <queue>
#include <functional>

/*!
*	\class GHeapNode
*	\brief 힙 자료구조의 노드를 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 19 December 2015
*/
class GHeapNode 
{
public:
	// 생성자 및 소멸자
	GHeapNode(int _idx, double _key): idx(_idx), key(_key) {}
	~GHeapNode() {}

	// 대소 연산자
	bool operator >(const GHeapNode &rhs) const {return (key > rhs.key);}
	bool operator >=(const GHeapNode &rhs) const {return (key >= rhs.key);}
	bool operator <(const GHeapNode &rhs) const {return (key < rhs.key);}
	bool operator <=(const GHeapNode &rhs) const {return (key <= rhs.key);}

public:
	// 데이터 멤버
	/*! \brief 정점의 인덱스 */	
	int   idx;

	/*! \brief 기준점에서 정점까지의 거리 */	
	double  key;
};

/*!
*	\class GHeap
*	\brief 힙 자료구조를 표현하는 클래스
*
*	\author 윤승현(shyun@dongguk.edu)
*	\date 19 December 2015
*/
class GHeap
{
	typedef std::priority_queue<GHeapNode, std::vector<GHeapNode>, std::greater<GHeapNode>> Heap_t;
public:
	// 생성자 및 소멸자
	GHeap();
	GHeap(std::vector<double> *_keys);
	~GHeap();

	// 멤버 함수
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
	// 데이터 멤버
	/*! \brief 힙 자료구조 */
	Heap_t heap;

	/*! \brief 추가 정보를 위한 플래그 */
	std::vector<bool> flags;

	/*! \brief 키에 대한 포인터 */
	std::vector<double> *keys;
};

#endif // GHEAP_H
