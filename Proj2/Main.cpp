#include <iostream>
#include "Vector.h"
#include "Position.h"
#include "City.h"
#include "Map.h"

using namespace std;


bool IsAlphanumeric(const char& c)
{
	return
		(c >= '0' && c <= '9') ||
		(c >= 'A' && c <= 'Z') ||
		(c >= 'a' && c <= 'z');
}


void ReadMap(Map& map)
{
	cin >> map.size.y;
	cin >> map.size.x;
	cin >> ws;

	map.tiles = new char*[map.size.x];
	for (int i = 0; i < map.size.x; i++)
	{
		map.tiles[i] = new char[map.size.y + 1];
		cin.getline(map.tiles[i], map.size.y + 1);
	}
}

void ReadCity(Map& map, Vector<City>& cities, Position startPos)
{
	City city;

	// read city name
	char* nameString = map.tiles[startPos.x] + startPos.y;
	for (int i = 0; IsAlphanumeric(nameString[i]); i++)
		city.name.Append(nameString[i]);

	// check city position
	bool posFound = false;
	for (int i = 0; i < 8; i++) // check around the first letter of the name
	{
		Position neighbor = startPos.GetNeighbor8Way(i);
		if (map.Contains(neighbor))
		{
			if (map.tiles[neighbor.x][neighbor.y] == '*')
			{
				city.pos = neighbor;
				posFound = true;
				break;
			}
		}
	}
	if (!posFound)
	{
		Position endPos = { startPos.x, startPos.y + city.name.GetLength() - 1 };
		for (int i = 0; i < 8; i++) // check around the first letter of the name
		{
			Position neighbor = endPos.GetNeighbor8Way(i);
			if (map.Contains(neighbor))
			{
				if (map.tiles[neighbor.x][neighbor.y] == '*')
				{
					city.pos = neighbor;
					break;
				}
			}
		}
	}

	cities.Append(city);
}

void ReadCities(Map& map, Vector<City>& cities)
{
	for (int x = 0; x < map.size.x; x++)
	{
		for (int y = 0; y < map.size.y; y++)
		{
			if (IsAlphanumeric(map.tiles[x][y]))
			{
				// skip if this isn't the first letter
				if (y > 0 && IsAlphanumeric(map.tiles[x][y - 1]))
					continue;

				ReadCity(map, cities, { x,y });
			}
		}
	}
}


City* FindCityByPos(const Vector<City>& cities, const Position& pos)
{
	for (size_t i = 0; i < cities.GetLength(); i++)
	{
		if (pos == cities[i].pos)
		{
			return &(cities[i]);
		}
	}
	return nullptr;
}


void CheckConnections(Map& map, Vector<City>& cities, City& city)
{
	struct Tile
	{
		Position pos;
		int dist;
	};
	Vector<Position> visited;
	Vector<Tile> toCheck;
	toCheck.Append({ city.pos, 0 });
	while (toCheck.GetLength() > 0)
	{
		for (size_t i = 0; i < 4; i++)
		{
			Position neighbor = toCheck[0].pos.GetNeighbor4Way(i);
			if (map.Contains(neighbor) && !visited.Contains(neighbor))
			{
				if (map[neighbor] == '#')
				{
					toCheck.Append({ neighbor, toCheck[0].dist + 1 });
				}
				else if (map[neighbor] == '*')
				{
					City* c = FindCityByPos(cities, neighbor);
					city.connections.Append({ c, toCheck[0].dist + 1 });
					visited.Append(neighbor);
				}
			}
		}
		visited.Append(toCheck[0].pos);
		toCheck.RemoveAt(0);
	}
}

void CreateGraph(Map& map, Vector<City>& cities)
{
	for (size_t i = 0; i < cities.GetLength(); i++)
	{
		CheckConnections(map, cities, cities[i]);
	}
}


int main()
{
	Map map;
	Vector<City> cities;

	ReadMap(map);

	ReadCities(map, cities);

	CreateGraph(map, cities);

	for (int i = 0; i < cities.GetLength(); i++)
	{
		cout << cities[i].name << " (" << cities[i].pos.x << "," << cities[i].pos.y << ")" << endl;
		for (int j = 0; j < cities[i].connections.GetLength(); j++)
		{
			cout << " - " << cities[i].connections[j].target->name << " " << cities[i].connections[j].length << endl;
		}
	}

	return 0;
}
