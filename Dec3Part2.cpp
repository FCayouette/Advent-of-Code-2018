#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

struct Fabric
{
	bool Intersect(const Fabric& f) const
	{
		if (num == f.num || minX > f.maxX || maxX < f.minX || minY > f.maxY || maxY < f.minY)
			return false;
		return true;
	}

	int num, minX, minY, maxX, maxY;
	bool alone;
};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Dec3.exe [filename]" << std::endl;;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	std::string line;
	std::vector<Fabric> fabrics;
	while (std::getline(in, line))
	{
		Fabric f;
		sscanf_s(line.c_str(), "#%d @ %d,%d: %dx%d", &f.num, &f.minX, &f.minY, &f.maxX, &f.maxY);

		f.maxX += f.minX;
		f.maxY += f.minY;
		f.alone = true;
		fabrics.push_back(f);
	}

	for (auto iter1 : fabrics)
		if (iter1.alone)
		{
			for (auto iter2 : fabrics)
				if (iter1.Intersect(iter2)) // Intersect disregards intersection with self
				{
					iter1.alone = iter2.alone = false;
					break;
				}

			if (iter1.alone)
			{
				std::cout << "Looking for " << iter1.num << std::endl;
				return 0;
			}
		}
	
	return -1;
}
