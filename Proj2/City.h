#pragma once
#include "Position.h"
#include "String.h"
#include "Vector.h"

struct City
{
	String name;
	Position pos;

	struct Connection
	{
		City* target;
		int length;
	};
	Vector<Connection> connections;
};