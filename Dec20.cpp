#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <set>
#include <array>

template <typename T>
constexpr void GrowToEncompass(T& min, T& max, T val)
{
	min = std::min(min, val);
	max = std::max(max, val);
}

struct Point
{
	constexpr Point(int X = 0, int Y = 0) : x(X), y(Y) {}
	constexpr bool operator <  (const Point& p) const { return x < p.x || (x == p.x && y < p.y); }
	constexpr Point operator + (const Point& p) const { return Point(x + p.x, y + p.y); }
	constexpr Point& operator+=(const Point& p) { x += p.x; y += p.y; return *this; }
	int x, y;
};

constexpr char coords[] = { 'N', 'E', 'S', 'W' };
constexpr Point directions[] = { Point(1, 0), Point(0, 1), Point(-1, 0), Point(0,-1) };

struct Bounds
{
	constexpr Bounds() : minX(0), minY(0), maxX(0), maxY(0) {}
	constexpr Bounds(const Point& p) : minX(p.x), minY(p.y), maxX(p.x), maxY(p.y) {}
	constexpr const Bounds& operator+=(const Bounds& b) { minX = std::min(minX, b.minX); 
														  minY = std::min(minY, b.minY); 
														  maxX = std::max(maxX, b.maxX); 
														  maxY = std::max(maxY, b.maxY); 
														  return *this; }
	constexpr const Bounds& operator+=(const Point& p) { GrowToEncompass(minX, maxX, p.x); GrowToEncompass(minY, maxY, p.y); return *this; }
	constexpr void Translate(const Point& p) { minX += p.x; maxX += p.x; minY += p.y; maxY += p.y; }
	int minX, minY, maxX, maxY;
};

struct Maze
{
	std::vector<std::vector<std::array<bool, 4>>> maze;
	const std::string regex;
	int maxValue, maxDistance;
	Point center;

	Maze(const std::string& str) : regex(str)
	{
		size_t i = 1;
		Point origin(0, 0);
		Bounds bounds(origin);
		FindBounds(i, bounds, origin);
		maxValue = (bounds.maxX - bounds.minX + 1) * (bounds.maxY - bounds.minY + 1);
		
		center.x = -bounds.minX;
		center.y = -bounds.minY;
		maze = std::vector<std::vector<std::array<bool, 4>>>(bounds.maxX - bounds.minX + 1, std::vector<std::array<bool, 4>>(bounds.maxY - bounds.minY + 1, { false, false, false, false }));
	
		i = 1;
		origin = center;
		FindConnections(i, origin);
	}

	void FindBounds(size_t& index, Bounds& bound, Point& point) // Linear time algorithm
	{
		switch (regex[index])
		{
		case 'N':
		case 'E':
		case 'S':
		case 'W':
		{
			char nextChar = regex[index];
			do {
				int dir = std::find(&(coords[0]), &(coords[4]), nextChar) - &(coords[0]);
				point += directions[dir];
				bound += point;
				nextChar = regex[++index];
			} while (nextChar == 'N' || nextChar == 'E' || nextChar == 'S' || nextChar == 'W');
			
			FindBounds(index, bound, point);
		} // Intentional fallthrough
		case '|':
		case ')':
		case '$':
			return;
		case '(':
		{
			std::set<Point> endPoints;
			do {
				Point workPoint = point;
				Bounds tmp(workPoint);
				++index;
				FindBounds(index, tmp, workPoint);
				bound += tmp;
				endPoints.insert(workPoint);
			} while (regex[index] == '|');

			Point newStart(0, 0);
			Bounds tmp(newStart);
			++index;
			FindBounds(index, tmp, newStart);
			for (const Point& p : endPoints)
			{
				Bounds work = tmp;
				work.Translate(p);
				bound += work;
			}
		}
		}
	}

	void FindConnections(size_t& index, Point& point)
	{
		switch (regex[index])
		{
		case 'N':
		case 'E':
		case 'S':
		case 'W':
		{
			char nextChar = regex[index];
			do
			{
				int dir = std::find(&(coords[0]), &(coords[4]), nextChar) - &(coords[0]);
				maze[point.x][point.y][dir] = true;
				point += directions[dir];
				maze[point.x][point.y][(dir + 2) % 4] = true;
				nextChar = regex[++index];
			} while (nextChar == 'N' || nextChar == 'E' || nextChar == 'S' || nextChar == 'W');
			FindConnections(index, point);
		}// Intentional fallthrough
		case '|':
		case ')':
		case '$':
			return;
		case '(':
		{
			std::set<Point> endPoints; // Must be a set to avoid duplicating work
			do 
			{
				Point workPoint = point;
				++index;
				FindConnections(index, workPoint);
				endPoints.insert(workPoint);
			} while (regex[index] == '|');

			int workIndex = ++index;
			for (auto& p : endPoints)
			{
				index = workIndex;
				Point tmp = p;
				FindConnections(index, tmp);
			}
		}
		}
	}

	std::pair<int, int> TraverseRooms(int countAboveThreshold) // Iterative fill algorithm
	{
		int iteration = 0;
		int count = 0;
		std::set<Point> fill;
		fill.insert(center);
		std::set<Point> lastIter = fill;
		while (true)
		{
			std::set<Point> step;
			for (const Point& p : lastIter) // Work only from points filled on the last iteration
				for (int i = 0; i < 4; ++i)
					if (maze[p.x][p.y][i])
					{
						Point newLoc = p + directions[i];
						if (fill.find(newLoc) == fill.cend())
						{
							fill.insert(newLoc);
							step.insert(newLoc);
							if (iteration >= countAboveThreshold)
								++count;
						}
					}

			if (step.empty()) // We have filled every room
				break;
			std::swap(step, lastIter);
			++iteration;
		}
		return std::make_pair(iteration, count);
	}
};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage Dec20.exe [regexPatternFilename]" << std::endl;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	std::string regex;
	std::getline(in, regex);

	Maze maze(regex);
	auto results = maze.TraverseRooms(999);
	std::cout << "Part 1: " << results.first << std::endl;
	std::cout << "Part 2: " << results.second << std::endl;
	return 0;
}