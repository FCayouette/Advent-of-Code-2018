#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: dec2.exe [filename]" << std::endl;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	std::string input;
	std::vector<std::string> boxes;
	int twos = 0;
	int threes = 0;

	std::vector<int> histogram(26, 0);
	while (in >> input)
	{
		boxes.push_back(input);
		for (char c : input)
			++histogram[c - 'a'];
		
		if (std::find(histogram.cbegin(), histogram.cend(), 2) != histogram.cend())
			++twos;
		if (std::find(histogram.cbegin(), histogram.cend(), 3) != histogram.cend())
			++threes;
		std::fill(histogram.begin(), histogram.end(), 0);
	}
	
	for (auto iter1 = boxes.cbegin(); iter1 != boxes.cend(); ++iter1)
		for (auto iter2 = iter1 + 1; iter2 != boxes.cend(); ++iter2)
		{
			int diffs = 0;
			for (size_t i = 0; i < iter1->size(); ++i)
				if ((*iter1)[i] != (*iter2)[i])
					if (++diffs > 1)
						break;
				
			if (diffs == 1)
			{
				std::cout << "Checksum: " << twos * threes << std::endl;
				for (size_t i = 0; i < iter1->size(); ++i)
					if ((*iter1)[i] == (*iter2)[i])
						std::cout << (*iter1)[i];
				std::cout << std::endl;
				return 0;
			}
		}
	return -1;
}
