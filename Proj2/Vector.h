#pragma once

template <typename T>
class Vector
{
private:
	size_t length = 0;
	size_t capacity = 1;
	T* arr = nullptr;

public:
	Vector()
	{
		arr = new T[capacity];
	}
	Vector(const Vector<T>& orig)
	{
		length = orig.length;
		capacity = orig.capacity;
		arr = new T[capacity];
		for (size_t i = 0; i < length; i++)
			arr[i] = orig.arr[i];
	}
	~Vector()
	{
		if (arr != nullptr)
			delete[] arr;
	}

	size_t GetLength() const
	{
		return length;
	}

	void Append(const T& element)
	{
		length++;
		if (length >= capacity)
		{
			capacity *= 2;
			T* tmp = new T[capacity];
			
			for (size_t i = 0; i < length; i++)
			{
				tmp[i] = arr[i];
			}

			delete[] arr;
			arr = tmp;
		}

		arr[length - 1] = element;
	}
	void RemoveAt(const size_t index)
	{
		if (index < length - 1)
		{
			T tmp = arr[index];
			arr[index] = arr[length - 1];
			arr[length - 1] = tmp;
		}
		length--;
	}

	bool Contains(const T& element) const
	{
		for (size_t i = 0; i < length; i++)
		{
			if (arr[i] == element)
				return true;
		}
		return false;
	}
	size_t IndexOf(const T& element) const
	{
		for (size_t i = 0; i < length; i++)
		{
			if (arr[i] == element)
				return i;
		}
		return -1;
	}

	T& operator[](size_t index) const
	{
		return arr[index];
	}
	Vector<T>& operator=(const Vector<T>& other)
	{
		Vector<T> tmp(other);
		length = tmp.length;
		capacity = tmp.capacity;

		T* a = tmp.arr;
		tmp.arr = arr;
		arr = a;

		return *this;
	}
};