#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>

#define InvalidTime 100000000;

struct CaveData
{
	CaveData() { visitTimeWithTool[0] = visitTimeWithTool[1] = visitTimeWithTool[2] = InvalidTime; }
	void SetData(int el) { erosionLevel = el; type =(char)(el % 3); }
	int erosionLevel;
	int visitTimeWithTool[3];
	char type;
};

enum Tool: char
{
	neither = 0,
	torch,
	climbingRope,
};

struct Point
{
	constexpr Point(int X = 0, int Y = 0) : x(X), y(Y) {}
	constexpr bool operator <  (const Point& p) const { return x < p.x || (x == p.x && y < p.y); }
	constexpr Point operator + (const Point& p) const { return Point(x + p.x, y + p.y); }
	int x, y;
};

constexpr Point directions[] = { Point(1, 0), Point(0, 1), Point(-1, 0), Point(0,-1) };

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Dec22.exe [caveFilename]" << std::endl;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	std::string line;
	std::getline(in, line);
	int depth, targetX, targetY;
	sscanf_s(line.c_str(), "depth: %d", &depth);
	std::getline(in, line);
	sscanf_s(line.c_str(), "target: %d,%d", &targetX, &targetY);

	const int caveSize = 3*std::max(targetX, targetY)/2;
	std::vector<std::vector<CaveData>> cave(caveSize, std::vector<CaveData>(caveSize));

	for (int x = 0; x < caveSize; ++x)
		for (int y = 0; y < caveSize; ++y)
		{
			int level = 0;
			if (x == 0)
				level = y * 48271;
			else if (y == 0)
				level = x * 16807;
			else if (x != targetX || y != targetY)
				level = cave[x][y - 1].erosionLevel * cave[x - 1][y].erosionLevel;
			cave[x][y].SetData((level + depth) % 20183);
		}
	
	int total = 0;
	for (int x = 0; x <= targetX; ++x)
		for (int y = 0; y <= targetY; ++y)
			total += cave[x][y].type;
	std::cout << "Part 1 " << total << std::endl;

	cave[0][0].visitTimeWithTool[torch] = 0;
	cave[0][0].visitTimeWithTool[climbingRope] = 7;

	std::set<Point> changingPoints;
	changingPoints.insert(Point(0, 0));
	const Point target(targetX, targetY);
	bool hitTarget = false;
	const int& targetTime = cave[targetX][targetY].visitTimeWithTool[torch];
	while (true)
	{
		std::set<Point> nextIter;
		for (const Point& p : changingPoints)
			for (int i = 0; i < 4; ++i)
			{
				Point next = p + directions[i];
				if (std::min(next.x, next.y) >= 0 && std::max(next.x, next.y) < caveSize)
				{
					int delta = std::abs(next.x - targetX) + std::abs(next.y - targetY);
					bool shouldInsert = false;
					for (char tool = 0; tool < 3; ++tool)
						if (cave[next.x][next.y].type != tool && cave[p.x][p.y].type != tool)
						{
							int time = std::min(cave[p.x][p.y].visitTimeWithTool[0] + (tool == 0 ? 1 : 8), 
									   std::min(cave[p.x][p.y].visitTimeWithTool[1] + (tool == 1 ? 1 : 8),
												cave[p.x][p.y].visitTimeWithTool[2] + (tool == 2 ? 1 : 8)));
							if (time < cave[next.x][next.y].visitTimeWithTool[tool] && time < targetTime - delta)
							{
								shouldInsert = true;
								cave[next.x][next.y].visitTimeWithTool[tool] = time;
							}
						}
					if (shouldInsert) // We only insert points that could contribute to improve the solution
						nextIter.insert(next);
				}
			}
		if (nextIter.empty())
			break;

		std::swap(changingPoints, nextIter);
	}
	std::cout << "Part 2 " << cave[targetX][targetY].visitTimeWithTool[torch] << std::endl;
	return 0;
}