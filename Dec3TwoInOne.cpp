#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

struct Fabric
{
	int num, minX, minY, maxX, maxY;
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
	int maxX = 0, maxY = 0, maxNum = 0;
	while (std::getline(in, line))
	{
		Fabric f;
		sscanf_s(line.c_str(), "#%d @ %d,%d: %dx%d", &f.num, &f.minX, &f.minY, &f.maxX, &f.maxY);

		f.maxX += f.minX;
		f.maxY += f.minY;
		maxX = std::max(maxX, f.maxX);
		maxY = std::max(maxY, f.maxY);
		maxNum = std::max(maxNum, f.num);
		fabrics.push_back(f);
	}

	// Two in one processing
	std::vector<std::pair<int, int>> usage((maxX + 1) * (maxY + 1), std::make_pair(0, 0));
	std::vector<bool> alone(maxNum + 1, true);
	int count = 0;
	for (const auto& f : fabrics)
	{
		for (int x = f.minX; x < f.maxX; ++x)
		{
			int temp = x * maxX + x;
			for (int y = f.minY; y < f.maxY; ++y)
			{
				auto& p = usage[temp + y];
				if (++p.first == 2)
					++count;
				if (p.second != 0)
					alone[p.second] = alone[f.num] = false;
				p.second = f.num;
			}
		}
	}
	std::cout << "Part 1: " << count << " Part 2: " << std::find(alone.cbegin()+1, alone.cend(), true) - alone.cbegin() << std::endl;
	
	
	return -1;
}
