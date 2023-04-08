#pragma once

struct Position
{
	int x, y;

	enum Direction
	{
		UpperLeft,
		Up,
		UpperRight,
		Right,
		LowerRight,
		Down,
		LowerLeft,
		Left
	};

	// returns one of the 8 neighboring positions, numbered clockwise beginning with the upper-left corner
	Position GetNeighbor8Way(int i) const
	{
		if (i == 0) return { x - 1, y - 1 };
		if (i == 1) return { x - 1, y     };
		if (i == 2) return { x - 1, y + 1 };
		if (i == 3) return { x    , y + 1 };
		if (i == 4) return { x + 1, y + 1 };
		if (i == 5) return { x + 1, y     };
		if (i == 6) return { x + 1, y - 1 };
		if (i == 7) return { x    , y - 1 };

		return { -1,-1 };
	}

	// returns one of the 4 neighboring positions, numbered clockwise beginning with the upper one
	Position GetNeighbor4Way(int i) const
	{
		if (i == 0) return { x - 1, y     };
		if (i == 1) return { x    , y + 1 };
		if (i == 2) return { x + 1, y     };
		if (i == 3) return { x    , y - 1 };

		return { -1,-1 };
	}

	bool operator==(const Position& other) const
	{
		return x == other.x && y == other.y;
	}
};