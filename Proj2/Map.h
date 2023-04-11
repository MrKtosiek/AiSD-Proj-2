#pragma once
#include "Position.h"

struct Map
{
	Position size = {};
	char** tiles = nullptr;

	~Map()
	{
		for (size_t i = 0; i < size.x; i++)
			delete[] tiles[i];

		delete[] tiles;
	}


	bool Contains(const Position& position) const
	{
		return
			position.x < size.x &&
			position.y < size.y;
	}

	char& operator[](const Position& pos) const
	{
		return tiles[pos.x][pos.y];
	}
};