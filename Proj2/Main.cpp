#include <iostream>
#include "Vector.h"
#include "Queue.h"
#include "HashMap.h"
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
	for (int x = 0; x < map.size.x; x++)
	{
		for (int y = 0; y < map.size.y; y++)
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


size_t FindCityByPos(const Vector<City>& cities, const Position& pos)
{
	for (size_t i = 0; i < cities.GetLength(); i++)
	{
		if (pos == cities[i].pos)
		{
			return i;
		}
	}
	return -1;
}


void CheckConnections(Map& map, Vector<City>& cities, City& city)
{
	struct Tile
	{
		Position pos;
		int dist;
	};

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
	bool** toVisit = new bool* [map.size.x];
	for (int i = 0; i < map.size.x; i++)
	{
		visited[i] = new bool[map.size.y]();
		toVisit[i] = new bool[map.size.y]();
	}

	Queue<Tile> toCheck;
	toCheck.Enqueue({ city.pos, 0 });
	while (toCheck.GetLength() > 0)
	{
		Tile curTile;
		toCheck.Dequeue(curTile);
		for (size_t i = 0; i < 4; i++)
		{
			Position neighbor = curTile.pos.GetNeighbor4Way(i);
			if (map.Contains(neighbor) && !visited[neighbor.x][neighbor.y])
			{
				if (map[neighbor] == '#' && !toVisit[neighbor.x][neighbor.y])
				{
					toCheck.Enqueue({ neighbor, curTile.dist + 1 });
					toVisit[neighbor.x][neighbor.y] = true;
				}
				else if (map[neighbor] == '*')
				{
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
		delete[] toVisit[i];
	}
	delete[] visited;
	delete[] toVisit;
}

void CreateGraph(Map& map, Vector<City>& cities)
{
	//if (map.size.x == 600 && map.size.y == 2048)
	//{
	//	cout << cities.GetLength() << '\n';
	//	cout << map.tiles[0] << '\n';
	//	exit(0);
	//}


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
		String source;
		String target;
		int length;

		cin >> source;
		cin >> target;
		cin >> length;

		size_t city = citiesDictionary.Get(source);

		if (city != -1)
			cities[city].connections.Append({ citiesDictionary.Get(target), length });
	}
}


///////////////////////////////////////////////////////////////////
// PATHFINDING

size_t PopPriorityIndex(Vector<size_t>& queue, const Vector<size_t>& distances)
{
	size_t index = 0;
	size_t queueIndexToRemove = 0;
	int lowest = INT_MAX;
	for (size_t i = 0; i < queue.GetLength(); i++)
	{
		if (distances[queue[i]] < lowest)
		{
			lowest = distances[queue[i]];
			index = queue[i];
			queueIndexToRemove = i;
		}
	}
	queue.RemoveAt(queueIndexToRemove);
	return index;
}

int FindPath(Vector<City>& cities, size_t source, size_t target, Vector<int>& path)
{
	Vector<size_t> queue;
	Vector<size_t> dist;
	Vector<size_t> prev;
	for (size_t i = 0; i < cities.GetLength(); i++)
	{
		queue.Append(i);
		dist.Append(INT_MAX);
		prev.Append(-1);
	}
	dist[source] = 0;

	while (queue.GetLength() > 0)
	{
		size_t city = PopPriorityIndex(queue, dist);

		if (city == target)
			break;

		for (size_t i = 0; i < cities[city].connections.GetLength(); i++)
		{
			size_t neighborIndex = cities[city].connections[i].targetIndex;
			int alt = dist[city] + cities[city].connections[i].length;
			if (alt < dist[neighborIndex])
			{
				dist[neighborIndex] = alt;
				prev[neighborIndex] = city;
			}
		}
	}

	int curIndex = target;
	if (prev[curIndex] != -1 || curIndex == source)
	{
		while (curIndex != -1)
		{
			path.Append(curIndex);
			curIndex = prev[curIndex];
		}
	}

	return dist[target];
}

void WritePath(Vector<City>& cities, Vector<int>& path, int dist, bool writeCities)
{
	cout << dist;

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

		Vector<int> path;
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

	ReadMap(map);

	ReadCities(map, cities, citiesDictionary);

	CreateGraph(map, cities);

	//if (map.size.x == 600 && map.size.y == 2048)
	//{
	//	exit(0);
	//}

	ReadAirlines(cities, citiesDictionary);

	//for (int i = 0; i < cities.GetLength(); i++)
	//{
	//	cout << cities[i].name << " (" << cities[i].pos.x << "," << cities[i].pos.y << ")" << endl;
	//	for (int j = 0; j < cities[i].connections.GetLength(); j++)
	//	{
	//		cout << " - " << cities[cities[i].connections[j].targetIndex].name << " " << cities[i].connections[j].length << endl;
	//	}
	//}

	ReadQueries(cities, citiesDictionary);

	return 0;
}
