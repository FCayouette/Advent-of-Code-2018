#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

struct Fabric
{
	int minX, minY, maxX, maxY;
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
	int maxX = 0, maxY = 0;
	while (std::getline(in, line))
	{
		Fabric f;
		int num;
		sscanf_s(line.c_str(), "#%d @ %d,%d: %dx%d", &num, &f.minX, &f.minY, &f.maxX, &f.maxY);

		std::cout << num << ' ' << f.minX << ' ' << f.minY << ' ' << ' ' << f.maxX << ' ' << f.maxY << std::endl;
		f.maxX += f.minX;
		f.maxY += f.minY;
		maxX = std::max(maxX, f.maxX);
		maxY = std::max(maxY, f.maxY);

		fabrics.push_back(f);
	}

	std::vector<int> usage((maxX + 1) * (maxY + 1), 0);
	int count = 0;
	for (const auto&f : fabrics)
	{
		for (int x = f.minX; x < f.maxX; ++x)
		{
			auto iter = usage.begin() + (x + 1)*maxX;
			std::for_each(iter + f.minY, iter + f.maxY, [&count](int i) {if (i == 1) ++count; return i+1; });
		}
	}
	std::cout << count << std::endl;
	return 0;
}
