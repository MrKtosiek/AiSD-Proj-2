#include <iostream>
#include "Vector.h"
#include "Queue.h"
#include "HashMap.h"
#include "PriorityQueue.h"
#include "Position.h"
#include "City.h"
#include "Map.h"

using namespace std;


///////////////////////////////////////////////////////////////////
// MAP READING

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
	for (size_t i = 0; i < map.size.x; i++)
	{
		map.tiles[i] = new char[map.size.y + 1];
		cin.getline(map.tiles[i], map.size.y + 1);
	}
}

void ReadCity(Map& map, Vector<City>& cities, HashMap<size_t>& citiesDictionary, Position startPos)
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
	citiesDictionary.Put(city.name, cities.GetLength() - 1);
}

void ReadCities(Map& map, Vector<City>& cities, HashMap<size_t>& citiesDictionary)
{
	for (size_t x = 0; x < map.size.x; x++)
	{
		for (size_t y = 0; y < map.size.y; y++)
		{
			if (IsAlphanumeric(map.tiles[x][y]))
			{
				// skip if this isn't the first letter
				if (y > 0 && IsAlphanumeric(map.tiles[x][y - 1]))
					continue;

				ReadCity(map, cities, citiesDictionary, { x,y });
			}
		}
	}
}


///////////////////////////////////////////////////////////////////
// GRAPH CREATION

size_t FindCityByPos(const Vector<City>& cities, const Position& pos)
{
	for (size_t i = 0; i < cities.GetLength(); i++)
	{
		if (pos == cities[i].pos)
		{
			return i;
		}
	}
	return UINT_MAX;
}


void CheckConnections(Map& map, Vector<City>& cities, City& city)
{
	struct Tile
	{
		Position pos;
		int dist;
	};

	// if the city doesn't have any neighboring road or city tiles, don't start the search
	bool canSkip = true;
	for (size_t i = 0; i < 4; i++)
	{
		Position neighbor = city.pos.GetNeighbor4Way(i);
		if (map.Contains(neighbor) && (map[neighbor] == '#' || map[neighbor] == '*'))
		{
			canSkip = false;
			break;
		}
	}
	if (canSkip) return;


	bool** visited = new bool* [map.size.x];
	bool** inQueue = new bool* [map.size.x];
	for (size_t i = 0; i < map.size.x; i++)
	{
		visited[i] = new bool[map.size.y]();
		inQueue[i] = new bool[map.size.y]();
	}


	// floodfill/BFS from the current city to all neighboring cities
	Queue<Tile> toCheck;
	toCheck.Enqueue({ city.pos, 0 });
	while (toCheck.GetLength() > 0)
	{
		Tile curTile;
		toCheck.Dequeue(curTile);
		for (size_t i = 0; i < 4; i++) // check all 4 neighbors
		{
			Position neighbor = curTile.pos.GetNeighbor4Way(i);
			if (map.Contains(neighbor) && !visited[neighbor.x][neighbor.y])
			{
				if (map[neighbor] == '#' && !inQueue[neighbor.x][neighbor.y])
				{
					// add the neighbor to the queue
					toCheck.Enqueue({ neighbor, curTile.dist + 1 });
					inQueue[neighbor.x][neighbor.y] = true;
				}
				else if (map[neighbor] == '*')
				{
					// add a connection
					size_t c = FindCityByPos(cities, neighbor);
					city.connections.Append({ c, curTile.dist + 1 });
					visited[neighbor.x][neighbor.y] = true;
				}
			}
		}
		visited[curTile.pos.x][curTile.pos.y] = true;
	}

	for (size_t i = 0; i < map.size.x; i++)
	{
		delete[] visited[i];
		delete[] inQueue[i];
	}
	delete[] visited;
	delete[] inQueue;
}

void CreateGraph(Map& map, Vector<City>& cities)
{
	for (size_t i = 0; i < cities.GetLength(); i++)
	{
		CheckConnections(map, cities, cities[i]);
	}
}


void ReadAirlines(Vector<City>& cities, HashMap<size_t>& citiesDictionary)
{
	int airlineCount;
	cin >> airlineCount;

	for (int i = 0; i < airlineCount; i++)
	{
		const size_t bufferSize = 256;
		char buffer[bufferSize];
		String source;
		String target;
		int length = 0;

		cin >> ws;
		cin.getline(buffer, bufferSize);

		int c = 0;
		while (buffer[c] != ' ')
		{
			source.Append(buffer[c]);
			c++;
		}
		c++;
		while (buffer[c] != ' ')
		{
			target.Append(buffer[c]);
			c++;
		}
		c++;
		while (buffer[c] != '\0')
		{
			length *= 10;
			length += buffer[c] - '0';
			c++;
		}


		size_t city = citiesDictionary.Get(source);

		cities[city].connections.Append({ citiesDictionary.Get(target), length });
	}
}


///////////////////////////////////////////////////////////////////
// PATHFINDING

// returns the distance to the target and saves all the traversed cities in 'path'
int FindPath(Vector<City>& cities, size_t source, size_t target, Vector<size_t>& path)
{
	PriorityQueue<size_t> unvisited;
	Vector<size_t> dist;
	Vector<size_t> prev;

	const size_t invalidIndex = UINT_MAX;

	// fill all the necessary containers
	for (size_t i = 0; i < cities.GetLength(); i++)
	{
		if (i != source)
			dist.Append(INT_MAX);
		else
			dist.Append(0);
		prev.Append(invalidIndex);
		unvisited.Add(i, dist[i]);
	}

	// Dijkstra's algorithm
	while (unvisited.GetLength() > 0)
	{
		size_t city = unvisited.ExtractMin();

		if (city == target)
			break;
		
		for (size_t i = 0; i < cities[city].connections.GetLength(); i++)
		{
			size_t neighborIndex = cities[city].connections[i].targetIndex;
			size_t alt = dist[city] + cities[city].connections[i].length;
			if (alt < dist[neighborIndex])
			{
				dist[neighborIndex] = alt;
				prev[neighborIndex] = city;
				unvisited.Add(neighborIndex, alt);
			}
		}
	}

	// retrieve the path
	size_t curIndex = target;
	if (prev[curIndex] != invalidIndex || curIndex == source)
	{
		while (curIndex != invalidIndex)
		{
			path.Append(curIndex);
			curIndex = prev[curIndex];
		}
	}

	return dist[target];
}

void WritePath(Vector<City>& cities, Vector<size_t>& path, int dist, bool writeCities)
{
	cout << dist;

	// the path includes the source and target, so there are no intermediate cities if the path is shorter than 3
	if (writeCities && path.GetLength() >= 3)
	{
		for (size_t i = path.GetLength() - 2; i >= 1; i--)
		{
			cout << ' ' << cities[path[i]].name;
		}
	}
	cout << '\n';
}

void ReadQueries(Vector<City>& cities, HashMap<size_t>& citiesDictionary)
{
	int queryCount;
	cin >> queryCount;

	for (int i = 0; i < queryCount; i++)
	{
		String source;
		String target;
		bool writeCities;

		cin >> source;
		cin >> target;
		cin >> writeCities;

		Vector<size_t> path;
		int dist = FindPath(cities, citiesDictionary.Get(source), citiesDictionary.Get(target), path);

		WritePath(cities, path, dist, writeCities);
	}
}


int main()
{
	iostream::sync_with_stdio(false);
	cin.tie(nullptr);

	Map map;
	HashMap<size_t> citiesDictionary;
	Vector<City> cities;

	// input
	ReadMap(map);
	ReadCities(map, cities, citiesDictionary);
	CreateGraph(map, cities);
	ReadAirlines(cities, citiesDictionary);

	// output
	ReadQueries(cities, citiesDictionary);

	// remove all elements from the dictionary to prevent a memory leak
	for (size_t i = 0; i < cities.GetLength(); i++)
		citiesDictionary.Remove(cities[i].name);

	return 0;
}
