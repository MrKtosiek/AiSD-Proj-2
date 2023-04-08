#pragma once
#include <iostream>

using namespace std;

template <typename T>
class Queue
{
	class Node
	{
	private:
		T data;
		Node* prev = nullptr;
		Node* next = nullptr;

		Node(const T& data, Node* prev, Node* next) : data(data), prev(prev), next(next) {}

		template <typename T>
		friend class Queue;
	};

private:
	size_t length = 0;
	Node* first = nullptr;
	Node* last = nullptr;

public:
	Queue()
	{
	}
	Queue(const Queue<T>& orig)
	{
		for (Node* cur = orig.first; cur != nullptr; cur = cur->next)
		{
			Enqueue(cur->data);
		}
	}
	~Queue()
	{
		Node* curNode = first;
		while (curNode != nullptr)
		{
			Node* tmp = curNode;
			curNode = curNode->next;
			delete tmp;
		}
	}

	size_t GetLength() const
	{
		return length;
	}
	void Enqueue(const T& data)
	{
		if (length == 0)
		{
			// create the first Node
			Node* newNode = new Node(data, nullptr, nullptr);
			first = newNode;
			last = newNode;
		}
		else
		{
			// add a new Node
			Node* newNode = new Node(data, last, nullptr);
			last->next = newNode;
			last = newNode;
		}

		length++;
	}
	void Dequeue(T& result)
	{
		if (first != nullptr)
		{
			Node* tmp = first;
			result = first->data;

			if (first->next == nullptr)
			{
				first = nullptr;
				last = nullptr;
			}
			else
			{
				first = first->next;
				first->prev = nullptr;
			}

			delete tmp;
			length--;
		}
	}

	Queue<T>& operator=(const Queue<T>& orig)
	{
		Queue<T> tmp(orig);
		length = tmp.length;

		Node* t = tmp.first;
		tmp.first = first;
		first = t;

		t = tmp.last;
		tmp.last = last;
		last = t;

		return *this;
	}
};
