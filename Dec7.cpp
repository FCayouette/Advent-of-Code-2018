#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <limits>

struct Edge
{
	Edge(char s, char e) :start(s), end(e) {}
	char start, end;
};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage dec7.exe [filename]" << std::endl;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	std::string line;
	std::vector<Edge> graph;
	char maxC = 0;
	while (std::getline(in, line))
	{
		char start = line[5];
		char end = line[36];
		graph.emplace_back(start, end);
		maxC = std::max(start, maxC);
		maxC = std::max(end, maxC);
	}
	++maxC;
	
	// Part 1
	bool done = false;
	std::vector<bool> completed(maxC-'A', false);
	do
	{
		done = true;
		for (char c = 'A'; c < maxC; ++c) // attempt processing character c
		{
			if (completed[c - 'A'])
				continue;

			bool free = true;
			for (auto i : graph)
				if (!completed[i.end-'A'] && i.end == c && !completed[i.start - 'A'])
				{
					free = false;
					break;
				}
			if (free)
			{
				std::cout << c;
				completed[c - 'A'] = true;
				done = false;
				break;
			}
		}
	} while (!done);
	std::cout << std::endl;
	// Completed part 1


	std::vector<bool> started(maxC - 'A', false);
	std::vector<int> completionTime(maxC - 'A', std::numeric_limits<int>::max());
	std::vector<int> workers(5, 0);
	int step = 0;
	do
	{
		auto worker = std::find_if(workers.begin(), workers.end(), [step](int s) {return s <= step; });
		if (worker == workers.end())
		{
			// Worker contention. Find next free worker and move current step to that time point
			worker = std::min_element(workers.begin(), workers.end());
			step = *worker;
		}
			

		bool workStarted = false;
		for (char c = 'A'; c < maxC; ++c) // attempt processing c
		{
			if (started[c - 'A']) // Dont process already started jobs
				continue;

			bool free = true;
			for (auto i : graph)
				if (i.end == c && completionTime[i.start - 'A'] > step)
				{
					free = false;
					break;
				}
				
			if (free)
			{
				started[c - 'A'] = true;
				completionTime[c - 'A'] = step + c-'A'+61;
				done = false;
				workStarted = true;
				*worker = step + c - 'A' + 1;
				break;
			}
		}
		if (!workStarted) //If work not started, we must wait for the next job to complete
		{
			int nextCompletion = std::numeric_limits<int>::max();
			std::for_each(completionTime.cbegin(), completionTime.cend(), [&nextCompletion, step](int i) { if (i > step && i < nextCompletion) nextCompletion = i; });
			step = nextCompletion;
		}
	} while (std::find(started.cbegin(), started.cend(), false) != started.cend());

	std::cout << "Completion time: " << *std::max_element(completionTime.cbegin(), completionTime.cend()) << std::endl;

	return 0;
}
