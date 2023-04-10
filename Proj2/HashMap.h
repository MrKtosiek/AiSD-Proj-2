#pragma once
#include "String.h"

template <typename V>
class HashMap
{
	struct KeyValuePair
	{
		String key;
		V val;
		KeyValuePair* next;
		KeyValuePair(const String& key, const V& val, KeyValuePair* next)
			:key(key), val(val), next(next) {}
	};
private:
	static const size_t capacity = 100000;
	KeyValuePair** chains = nullptr;

	static size_t StringHash(const String& str)
	{
		size_t hash = 0;
		size_t p = 1;
		for (size_t i = 0; i < str.GetLength(); i++)
		{
			hash += str[i] * p;
			p *= 31;
		}
		return hash % capacity;
	}

public:
	HashMap()
	{
		chains = new KeyValuePair*[capacity];
	}
	~HashMap()
	{
		if (chains != nullptr)
			delete[] chains;
	}

	void Put(const String& key, const V& value)
	{
		size_t hash = StringHash(key);
		chains[hash] = new KeyValuePair(key, value, chains[hash]);
	}

	V& Get(const String& key) const
	{
		size_t hash = StringHash(key);
		for (KeyValuePair* p = chains[hash]; p != nullptr; p = p->next)
		{
			if (p->key == key)
			{
				return p->val;
			}
		}
		return chains[0]->val;
	}

	void Remove(const String& key)
	{
		size_t hash = StringHash(key);
		KeyValuePair* prev = nullptr;
		for (KeyValuePair* p = chains[hash]; p != nullptr; p = p->next)
		{
			if (p->key == key)
			{
				// remove the pair from the chain
				if (p == chains[hash])
					chains[hash] = p->next;
				else
					prev->next = p->next;

				delete p;
				return;
			}
			prev = p;
		}
	}
};
