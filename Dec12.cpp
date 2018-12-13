#include <iostream>
#include <fstream>
#include <deque>
#include <vector>
#include <string>
#include <algorithm>

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Dec12.exe [filename]" << std::endl;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	std::deque<bool> currentState = { false, false, false, false, false };
	std::string line;
	// Read starting positions
	std::getline(in, line);
	line = line.substr(line.find(':')+2); // Ignore statements before data
	for (char c : line)
		currentState.push_back(c == '#');
	for (int i = 0; i < 5; ++i)
		currentState.push_back(false);
	
	// Read patterns
	std::vector<bool> patterns(32, false);
	while (std::getline(in, line))
	{
		if (line.size() < 10)
			continue;
		
		int patternIndex = 0;
		for (int i = 0; i < 5; ++i)
			patternIndex += ((line[i] == '#' ? 1 : 0) << (4-i)); 
		patterns[patternIndex] = (line[9] == '#');
	}
	
	// Start iterating on inital state 
	std::deque<bool> nextState(currentState.size());
	int firstIndex = -5;
	long long previousResult = 0;
	long long previousDiff = 0;
	bool found = false;
	for (int iteration = 0; true; ++iteration)
	{
		// Process each spot
		char pattern = 0;
		for (int i = 0; i < 5; ++i)
			pattern = pattern * 2 + (currentState[i] ? 1 : 0);
		for (size_t i = 5; i < currentState.size(); ++i)
		{
			nextState[i - 3] = patterns[pattern];
			pattern = (pattern * 2 + (currentState[i] ? 1 : 0)) & 0x1f;
		}

		// Make sure we grow or shrink to always have 5 empty pots at each end and not more
		int fillCount = 0;
		for (int i = 0; i < 5; ++i)
			if (nextState[i])
			{
				fillCount = 5 - i;
				break;
			}
		if (fillCount > 0)
		{
			firstIndex -= fillCount;
			while (fillCount-- > 0)
				nextState.push_front(false);
		}
		else while (!nextState[5])
			{
				++firstIndex;
				nextState.pop_front();
			}

		fillCount = 0;
		
		for (size_t i = 0; i < 5; ++i)
			if (nextState[nextState.size() - i - 1])
			{
				fillCount = 5 - i;
				break;
			}
		if (fillCount > 0)
			for (int i = 0; i < fillCount; ++i)
				nextState.push_back(false);
		else
			while (!nextState[nextState.size() - 6])
				nextState.pop_back();

		// Prepare next iteration;
		currentState.resize(nextState.size());
		std::swap(currentState, nextState);

		// Results processing
		{
			long long currentValue = firstIndex;
			long long totalValue = 0;
			for (bool b : currentState)
			{
				if (b) totalValue += currentValue;
				++currentValue;
			}

			long long diff = totalValue - previousResult;
			if (iteration == 19)
			{
				std::cout << "Part 1: " << totalValue << std::endl;
				if (found)
					break;
			}

			if (previousDiff == diff && !found)
			{
				// We have settled into a pattern, extrapolate to 50 billionth iteration
				long long iter50BResult = (50000000000ll - iteration-1) * diff + totalValue;
				std::cout << "Part 2: " << iter50BResult << std::endl;
				if (iteration >= 19)
					break;
				found = true;
			}
			previousResult = totalValue;
			previousDiff = diff;
		}
	}
	
	return 0;
}