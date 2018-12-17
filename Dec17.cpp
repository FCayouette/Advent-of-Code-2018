#include <iostream>
#include <fstream>
#include <string>
#include <numeric>
#include <vector>
#include <algorithm>

struct Line
{
	Line(int x1, int x2, int y1, int y2) : minX(x1), maxX(x2), minY(y1), maxY(y2) {}
	int minX, minY, maxX, maxY;
};

using Survey = std::vector <std::string>;
struct Ground
{
	Ground(Survey&& s) : survey(s) {}
	Survey survey;
	bool fillFrom(int fromX, int fromY);
	std::pair<size_t, size_t> CheckWater() const
	{
		int count1 = 0, count2 = 0;
		std::for_each(survey.cbegin(), survey.cend(), [&count1, &count2](const std::string&s) {
			std::for_each(s.cbegin(), s.cend(), [&count1, &count2](char c) {if (c == '|') ++count1; else if (c == '~') ++count2; }); });
		return std::make_pair(count1, count2);
	}
};

bool Ground::fillFrom(int fromX, int fromY) // Returns true if we need to check for row fill
{
	if ((size_t)fromY < survey.size())
	{
		if (survey[fromY][fromX] == '#' || survey[fromY][fromX] == '~') // Water coming from above needs to check for fill
			return true;
		if ((size_t)fromY == survey.size() - 1) // Bottom row special case
		{
			survey[fromY][fromX] = '|';
			return false;
		}
		if (survey[fromY][fromX] == '|') // We have fallen on an already processed row
			return false;
		if (survey[fromY][fromX] == '.') // Keep on falling
		{
			survey[fromY][fromX] = '|';
			if (!fillFrom(fromX, fromY + 1))
				return false; // Row below current position didn't fill, we can safely go back up
		}

		// We need to check whether the row is bounded from both sides
		int minX = fromX - 1;
		int maxX = fromX + 1;
		while (survey[fromY][minX] == '.' && (survey[fromY + 1][minX] == '~' || survey[fromY + 1][minX] == '#'))
			--minX;
		if (survey[fromY][minX] == '.' && fillFrom(minX, fromY + 1))
		{
			survey[fromY][fromX] = '.';
			return fillFrom(fromX, fromY); // Redo row as we have filled from left
		}
		bool leftBounded;
		if ((leftBounded = !(survey[fromY][minX] == '|' || survey[fromY][minX] == '.')))
			++minX; // We hit # to the left, (else we are unbounded)

		while (survey[fromY][maxX] == '.' && (survey[fromY + 1][maxX] == '~' || survey[fromY + 1][maxX] == '#'))
			++maxX;
		if (survey[fromY][maxX] == '.' && fillFrom(maxX, fromY + 1))
		{
			survey[fromY][fromX] = '.';
			return fillFrom(fromX, fromY); // Redo row as we have filled from right
		}
		bool rightBounded;
		if ((rightBounded = !(survey[fromY][maxX] == '|' || survey[fromY][maxX] == '.')))
			--maxX; // We hit # to the right, (else we are unbounded)
		
		char fillCharacter = rightBounded && leftBounded ? '~' : '|';
		for (int x = minX; x <= maxX; ++x)
			survey[fromY][x] = fillCharacter;

		return rightBounded && leftBounded;
	}
	return false;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Dec17.exe []" << std::endl;
		return -1;
	}

	int minX = 500;
	int maxX = 500;
	int minY = std::numeric_limits<int>::max();
	int maxY = 0;
	std::ifstream in(argv[1], std::ios::in);
	std::vector<Line> lines;
	std::string str;
	while (std::getline(in, str))
	{
		int x1, x2, y1, y2;
		if (str[0] == 'x')
		{
			sscanf_s(str.c_str(), "x=%d, y=%d..%d", &x1, &y1, &y2);
			lines.emplace_back(x1, x1, y1, y2);
			minX = std::min(x1, minX);
			maxX = std::max(x1, maxX);
			minY = std::min(y1, minY);
			maxY = std::max(y2, maxY);
		}
		else
		{
			sscanf_s(str.c_str(), "y=%d, x=%d..%d", &y1, &x1, &x2);
			lines.emplace_back(x1, x2, y1, y1);
			minX = std::min(x1, minX);
			maxX = std::max(x2, maxX);
			minY = std::min(y1, minY);
			maxY = std::max(y1, maxY);
		}
	}
	--minX;
	++maxX;
	// Initialize blank survey
	str.resize(maxX - minX+1);
	std::for_each(str.begin(), str.end(), [](char& c) {c = '.'; });
	Survey survey(maxY - minY + 1, str);

	for (const Line& l : lines) // Put clay in the survey based on read lines
		for (int y = l.minY - minY; y <= l.maxY - minY; ++y)
			for (int x = l.minX - minX; x <= l.maxX - minX; ++x)
				survey[y][x] = '#';

	Ground ground(std::move(survey));
	ground.fillFrom(500 - minX, 0);
	auto counts = ground.CheckWater();

	std::cout << "Part 1: " << counts.first + counts.second << std::endl << "Part 2: " << counts.second << std::endl;

	return 0;
}