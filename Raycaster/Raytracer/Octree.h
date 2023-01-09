#pragma once

#include "Color.h"
#include <array>

enum class OCTREE_CONTENT { EMPTY, FILLED, SPARSE };

template <class T>
struct Octree
{
	Octree()
	{
		object = nullptr;
	}

	inline ~Octree()
	{
		if (contains == OCTREE_CONTENT::SPARSE)
		{
			delete[] tree;
		}
		else if (contains == OCTREE_CONTENT::FILLED)
		{
			delete object;
		}
	}

	//Octree()
	//{
	//	tree[0][0][0] = nullptr;
	//	tree[1][0][0] = nullptr;
	//	tree[0][1][0] = nullptr;
	//	tree[0][0][1] = nullptr;
	//	tree[1][1][0] = nullptr;
	//	tree[1][0][1] = nullptr;
	//	tree[0][1][0] = nullptr;
	//	tree[0][1][1] = nullptr;
	//}

	OCTREE_CONTENT contains = OCTREE_CONTENT::EMPTY;

	//Only one can exist at a time
	union {
		Octree<T>* tree;
		T* object;
	};

};