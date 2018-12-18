#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <string>
#include <algorithm>
#include <set>

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: dec18.exe [filename]" << std::endl;
		return -1;
	}
    
	std::ifstream in(argv[1], std::ios::in);
	std::string line;
	std::vector<std::string> ground, work;
	while (std::getline(in, line))
		ground.emplace_back(std::move(line));

	work = ground; // Create a work copy
	
	std::set<int> transient;
	std::vector<int> steadyState;
	bool found = false;
	for (int iteration = 0; !found; ++iteration)
	{
		int trees = 0, lumberyards = 0;
		for (size_t x = 0; x < ground.size(); ++x)
			for (size_t y = 0; y < ground[x].size(); ++y)
			{
				int localTrees = 0, localLumberyards = 0;
				for (size_t i = (x==0 ? 0: x-1); i < std::min(x+2, ground.size()) ; ++i)
					for (size_t j = (y==0 ? 0:y-1); j < std::min(y + 2, ground[i].size()); ++j)
					{
						if (i == x && j == y)
							continue;
						if (ground[i][j] == '#') ++localLumberyards;
						else if (ground[i][j] == '|') ++localTrees;
					}
				if (ground[x][y] == '.')
				{
					if (localTrees >= 3) { work[x][y] = '|'; ++trees; }
					else work[x][y] = '.';
				}
				else if (ground[x][y] == '|')
				{
					if (localLumberyards >= 3) { work[x][y] = '#'; ++lumberyards; }
					else { work[x][y] = '|'; ++trees; }
				}
				else if (localLumberyards > 0 && localTrees > 0)
				{
					work[x][y] = '#';
					++lumberyards;
				}
				else work[x][y] = '.';
			}
		
		// Check for pattern
		int score = trees * lumberyards;
		if (transient.find(score) == transient.end())
		{
			transient.insert(score);
			steadyState.clear();
		}
		else
		{
			if (std::find(steadyState.cbegin(), steadyState.cend(), score) == steadyState.cend())
				steadyState.push_back(score);
			else
			{
				int index = (1000000000ll - iteration - 1) % steadyState.size();
				std::cout << "Part 2 resource value: " << steadyState[index] << std::endl;
				found = true;
			}
		}
		if (iteration == 9)
			std::cout << "Part 1 resource value: " << score << std::endl;
		// Prepare next iteration
		std::swap(ground, work);
	}
	return 0;
}
