#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

template <typename T>
constexpr void GrowToEncompass(T& min, T& max, T val)
{
	min = std::min(min, val);
	max = std::max(max, val);
}

struct Point{
	int x;
	int y;
	int id;
};

struct Dist
{
	Dist() : minDist(0x40000000), who(-1) {}
	Dist(int d, int w) : minDist(d), who(w) {}
	int minDist;
	int who;
};

void CheckVoronoiPoint(const Dist& from, Dist& to)
{
	int dist = from.minDist + 1;
	if (dist < to.minDist)
	{
		to.minDist = dist;
		to.who = from.who;
	}
	else if (dist == to.minDist && from.who != to.who)
		to.who = -1;
}


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage dec6.exe [filename]" << std::endl;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	std::string line;
	std::vector<Point> points;
	int minX = 100000, minY = 100000;
	int maxX = 0, maxY = 0;
	int id = 0;
	while (std::getline(in, line))
	{
		Point p;
		sscanf_s(line.c_str(), "%d, %d", &p.x, &p.y);
		GrowToEncompass(minX, maxX, p.x);
		GrowToEncompass(minY, maxY, p.y);
		p.id = id++;
		points.push_back(p);
	}

	// Bound work space, anything outside will not contribute to the solution
	std::for_each(points.begin(), points.end(), [minX, minY](Point& p) { p.x -= minX; p.y -= minY; });
	maxX -= minX;
	maxY -= minY;
	++maxX;
	++maxY;

	// Voronoi computation on the workspace
	std::vector<std::vector<Dist>> voronoi(maxX, std::vector<Dist>(maxY));
	// Initialization phase
	std::for_each(points.cbegin(), points.cend(), [&voronoi](const Point& p) {voronoi[p.x][p.y] = { 0, p.id }; });
	
	// First pass
	for (int i = 0; i < maxX; ++i)
		for (int j = 0; j < maxY; ++j)
		{
			Dist& d = voronoi[i][j];
			if (i > 0)
				CheckVoronoiPoint(voronoi[i - 1][j], d);
			if (j > 0)
				CheckVoronoiPoint(voronoi[i][j - 1], d);
		}
	
	// Second pass, area counting and infinite tagging
	std::vector<std::pair<int, bool>> areaInfo(points.size(), std::make_pair(0, false));
	for (int i = maxX-1; i >= 0; --i)
		for (int j = maxY-1; j >= 0; --j)
		{
			Dist& d = voronoi[i][j];
			if (i < maxX - 1)
				CheckVoronoiPoint(voronoi[i + 1][j], d);
			if (j < maxY - 1)
				CheckVoronoiPoint(voronoi[i][j + 1], d);

			if (d.who != -1 && !areaInfo[d.who].second)
			{
				if (i == 0 || i == maxX - 1 || j == 0 || j == maxY - 1)
					areaInfo[d.who].second = true;
				else
					++areaInfo[d.who].first;
			}
		}

	int result = 0;
	std::for_each(areaInfo.cbegin(), areaInfo.cend(), [&result](const std::pair<int, bool>& data) { if (!data.second) result = std::max(result, data.first); });
	std::cout << "Max non-infinite area " << result << std::endl;

	// Part 2
	int safeArea = 0;
	for (int i = 0; i <= maxX; ++i)
		for (int j = 0; j <= maxY; ++j)
		{
			int pixel = 0;
			for (const auto& p : points)
			{
				pixel += std::abs(p.x - i) + std::abs(p.y - j);
				if (pixel >=10000)
					break;
			}
			if (pixel < 10000)
				++safeArea;
		}
		
	std::cout << "Safe area size " << safeArea << std::endl;

	return 0;
}