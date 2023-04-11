#pragma once
#include "Position.h"
#include "String.h"
#include "Vector.h"

struct City
{
	String name;
	Position pos = {};

	struct Connection
	{
		size_t targetIndex;
		int length;
	};
	Vector<Connection> connections;
};