#include <iostream>
#include <fstream>
#include <list>
#include <deque>
#include <vector>

template <typename iter1, typename iter2>
bool Match(iter1 start, iter1 end, iter2 seq)
{
	while (start != end)
		if (*(start++) != *(seq++))
			return false;
	return true;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage Dec14.exe [puzzle input]" << std::endl;
		return -1;
	}

	size_t requiredSize = atoi(argv[1]);
	int patternToLookFor = requiredSize;
	
	std::vector<char> reversedPattern;
	while (patternToLookFor > 0)
	{
		reversedPattern.push_back(patternToLookFor % 10);
		patternToLookFor /= 10;
	}
	
	requiredSize += 10;

	std::vector<char> recipes = { 3, 7 }; // Avoid most vexing parse
		
	size_t firstElf = 0;
	size_t secondElf = 1;
	size_t currentSize = recipes.size();
	int iteration = 0;
	bool foundPattern = false;
	
	while (currentSize < requiredSize || !foundPattern)
	{
		++iteration;
		int tmp = recipes[firstElf] + recipes[secondElf];
		bool twoInserted = false;
		if (tmp > 9)
		{
			twoInserted = true;
			++currentSize;
			recipes.push_back(1);
			tmp -= 10;
		}
		++currentSize;
		recipes.push_back(tmp);
		
		firstElf = (firstElf + recipes[firstElf] + 1) % currentSize;
		secondElf = (secondElf + recipes[secondElf] + 1) % currentSize;

		if (!foundPattern && currentSize >= reversedPattern.size())
		{
			int index;
			if ((foundPattern = Match(reversedPattern.cbegin(), reversedPattern.cend(), recipes.crbegin())))
				index = currentSize - reversedPattern.size();
			else if (foundPattern = (twoInserted && currentSize + 1 > reversedPattern.size() && Match(reversedPattern.cbegin(), reversedPattern.cend(), recipes.crbegin() + 1)))
				index = currentSize - reversedPattern.size() - 1;

			if (foundPattern)
				std::cout << "Part 2: " << index << std::endl;
		}
	}

	std::cout << "Part 1: ";
	for (size_t index = requiredSize - 10, i = 0; i < 10; ++i, ++index)
		std::cout << (int)recipes[index];
	std::cout << std::endl;

	return 0;
}