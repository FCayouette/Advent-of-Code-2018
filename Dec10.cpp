#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>

template <typename T>
constexpr void GrowToEncompass(T& min, T& max, const T val)
{
	min = std::min(min, val);
	max = std::max(max, val);
}

struct Point
{
	int posX, posY, velX, velY;
	void Iterate() { posX += velX; posY += velY; }
};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Dec10.exe [filename]" << std::endl;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	
	int minY = std::numeric_limits<int>::max();
	int maxY = std::numeric_limits<int>::min();

	std::string line;
	std::vector<Point> points;
	while (std::getline(in, line))
	{
		Point p;
		std::string sub = line.substr(line.find('<') + 1);
		p.posX = atoi(sub.substr(0, sub.find(',')).c_str());
		p.posY = atoi(sub.substr(sub.find(',') + 1, sub.find('>')).c_str());
		sub = sub.substr(sub.find('<') + 1);
		p.velX = atoi(sub.substr(0, sub.find(',')).c_str());
		p.velY = atoi(sub.substr(sub.find(',') + 1, sub.find('>')).c_str());
		
		GrowToEncompass(minY, maxY, p.posY);
		
		points.push_back(p);
	}

	int rangeY = maxY - minY;
	int iterations = 0;
	while (true)
	{
		minY = std::numeric_limits<int>::max();
		maxY = std::numeric_limits<int>::min();
		std::for_each(points.begin(), points.end(), [&](Point& p) { p.Iterate(); GrowToEncompass(minY, maxY, p.posY); });

		if (rangeY > (maxY - minY))
		{
			rangeY = (maxY - minY);
			++iterations;
		}
		else
			break;
	}

	int minX = minY = std::numeric_limits<int>::max();
	int maxX = maxY = std::numeric_limits<int>::min();
	std::for_each(points.begin(), points.end(), 
		[&minX, &minY, &maxX, &maxY](Point& p) {
		p.posX -= p.velX; 
		p.posY -= p.velY; 
		GrowToEncompass(minX, maxX, p.posX); 
		GrowToEncompass(minY, maxY, p.posY); 
	});
	
	int rangeX = maxX - minX + 1;
	rangeY = maxY - minY + 1;

	std::vector<bool> map(rangeX*rangeY, 0);
	for (const auto& p : points)
		map[rangeX*(p.posY-minY) + p.posX-minX] = true;

	for (int y = 0; y < rangeY; ++y)
	{
		for (int x = 0; x < rangeX; ++x)
			std::cout << (map[rangeX*y + x] ? '#' : ' ');
		std::cout << std::endl;
	}
	
	std::cout << std::endl << iterations << std::endl;
	
	return 0;
}
