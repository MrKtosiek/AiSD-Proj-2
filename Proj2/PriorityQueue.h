#pragma once
#include "Vector.h"

template <typename T>
class PriorityQueue
{
	struct PriorityNode
	{
		T data;
		size_t priority;
	};

private:
	Vector<PriorityNode> nodes;

	static size_t Parent(const size_t& i)
	{
		return (i - 1) / 2;
	}
	static size_t LeftChild(const size_t& i)
	{
		return 2 * i + 1;
	}
	static size_t RightChild(const size_t& i)
	{
		return 2 * i + 2;
	}

public:
	size_t GetLength() const
	{
		return nodes.GetLength();
	}

	void Add(const T& data, const size_t& priority)
	{
		nodes.Append({ data, priority });

		size_t curNode = nodes.GetLength() - 1;
		while (curNode > 0)
		{
			size_t parent = Parent(curNode);
			if (nodes[curNode].priority < nodes[parent].priority)
			{
				PriorityNode tmp = nodes[curNode];
				nodes[curNode] = nodes[parent];
				nodes[parent] = tmp;

				curNode = parent;
			}
			else
			{
				break;
			}
		}
	}
	T& ExtractMin()
	{
		/*T result = 0;
		size_t queueIndexToRemove = 0;
		int lowest = INT_MAX;
		for (size_t i = 0; i < nodes.GetLength(); i++)
		{
			if (nodes[i].priority < lowest)
			{
				lowest = nodes[i].priority;
				result = nodes[i].data;
				queueIndexToRemove = i;
			}
		}
		nodes.RemoveAt(queueIndexToRemove);
		return result;*/



		nodes.RemoveAt(0);
		

		size_t curNode = 0;
		while (LeftChild(curNode) < nodes.GetLength())
		{
			size_t childToSwap = curNode;
			size_t leftChild = LeftChild(curNode);
			size_t rightChild = RightChild(curNode);

			if (nodes[curNode].priority > nodes[leftChild].priority)
				childToSwap = leftChild;

			if (rightChild < nodes.GetLength() && nodes[rightChild].priority < nodes[childToSwap].priority)
				childToSwap = rightChild;

			if (childToSwap == curNode)
				break;

			PriorityNode tmp = nodes[curNode];
			nodes[curNode] = nodes[childToSwap];
			nodes[childToSwap] = tmp;

			curNode = childToSwap;
		}

		return nodes[GetLength()].data;
	}
};
