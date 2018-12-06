#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>

struct Record
{
	bool operator <(const Record& r) const
	{
		return time < r.time;
	}

	std::string time;
	int minute, number;	
	char action;
};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Dec4.exe [filename]" << std::endl;;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	std::string line;
	std::vector< Record > records;
	int maxGuard = 0;
	while (std::getline(in, line))
	{
		Record r;
		int index = line.find(']');
		r.time = std::move(line.substr(1, index - 1)); //Time string without []
		r.minute = atoi(r.time.substr(r.time.find(':') + 1).c_str());

		r.action = line[index + 2]; // G, f or w. 
		if (r.action == 'G')
		{
			sscanf_s(line.c_str() + index + 2, "Guard #%d", &r.number);
			maxGuard = std::max(r.number, maxGuard);
		}
		records.push_back(std::move(r));
	}

	std::sort(records.begin(), records.end());

	std::vector<int> sleepTime(maxGuard + 1, 0);
	std::vector<int> sleepHistogram(60 * (maxGuard + 1), 0); // Flatened vector of histogram to facilite algorithmic search for part 2
	
	int num = -1;
	int start;
	for (auto& r : records)
	{
		if (r.action == 'G') // Guard starts it's shift
			num = r.number;
		else if (r.action == 'f') // Guard falls asleep
			start = r.minute;
		else // Guard wakes up
		{
			sleepTime[num] += r.minute - start;
			auto guardHistogramBegin = sleepHistogram.begin() + num * 60;
			std::for_each(guardHistogramBegin + start, guardHistogramBegin + r.minute, [](int i) { return i + 1; });
		}
	}

	int guardNumber = std::max_element(sleepTime.cbegin(), sleepTime.cend()) - sleepTime.cbegin(); 
	auto iter = sleepHistogram.cbegin() + guardNumber * 60;
	int maxMinute = std::max_element(iter, iter + 60) - iter;
	std::cout << "Part 1: Guard " << guardNumber << " minute " << maxMinute << " => " << guardNumber * maxMinute << std::endl;

	auto result = std::max_element(sleepHistogram.cbegin(), sleepHistogram.cend()) - sleepHistogram.cbegin();
	std::cout << "Part 2: Guard " << result / 60 << " minute " << result%60 << " => " << (result /60) * (result %60) << std::endl;

	return 0;
}
