#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>

struct Point4D
{
	Point4D(int X = 0, int Y = 0, int Z = 0, int W = 0) : x(X), y(Y), z(Z), w(W) {}

	bool SameConstellation(const Point4D& p) const { return std::abs(x - p.x) + std::abs(y - p.y) + std::abs(z - p.z) + std::abs(w - p.w) <= 3; }
	int x, y, z, w;
};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage Dec25.exe [starsFilename]" << std::endl;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	std::string line;
	std::list<std::vector<Point4D>> constellations;

	while (std::getline(in, line))
	{
		Point4D p;
		sscanf_s(line.c_str(), "%d,%d,%d,%d", &p.x, &p.y, &p.z, &p.w);
		auto belongsTo = constellations.end();
		for (auto iterC = constellations.begin(); iterC != constellations.end();)
		{
			bool shouldIncrease = true;
			for (const Point4D other : *iterC)
				if (p.SameConstellation(other))
				{
					if (belongsTo == constellations.end())
						belongsTo = iterC;
					else
					{
						belongsTo->insert(belongsTo->end(), iterC->begin(), iterC->end());
						iterC = constellations.erase(iterC);
						shouldIncrease = false;
					}
					break;
				}
			if (shouldIncrease)
				++iterC;
		}
		if (belongsTo == constellations.end())
			constellations.insert(belongsTo, std::vector<Point4D>(1, p));
		else
			belongsTo->push_back(p);
	}

	std::cout << "Part 1: " << constellations.size() << std::endl;

	return 0;
}