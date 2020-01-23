#include "stdafx.h"

GHeap::GHeap()
{
	keys = NULL;
}

GHeap::GHeap(std::vector<double> *_keys) 
{ 
	initialize(_keys); 
}

GHeap::~GHeap() 
{
}

void GHeap::initialize(std::vector<double> *_keys) 
{ 
	keys = _keys;
	resize(keys->size()); 
	std::fill(flags.begin(), flags.end(), false);
}

void GHeap::resize(int size) 
{ 
	flags.resize(size); 
}

bool GHeap::is_in_heap(int idx) const 
{ 
	return flags[idx];
}

void GHeap::remove_from_heap(int idx) 
{ 
	flags[idx] = false;
}

void GHeap::push(int idx)
{ 
	heap.push(GHeapNode(idx, (*keys)[idx])); 
	flags[idx] = true; 
}

bool GHeap::empty() 
{ 
	return (top() == -1);
}

int GHeap::size() const 
{ 
	return heap.size();
}

int GHeap::get_candidate() 
{
	if (!heap.empty()) 
	{
		int ret = top(); 
		pop();
		return ret;
	} 
	else
		return -1;
}

void GHeap::pop() 
{ 
	while (!heap.empty() && !is_in_heap(heap.top().idx))
		heap.pop();

	if (!heap.empty()) 
	{ 
		flags[heap.top().idx] = false; 
		heap.pop();		
	}
} 

int GHeap::top() 
{
	while (!heap.empty() && !is_in_heap(heap.top().idx))
		heap.pop();

	if (!heap.empty())
		return heap.top().idx;
	else
		return -1;
} 