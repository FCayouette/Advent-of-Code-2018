#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <list>
#include <future>

void React(std::string& p)
{
	std::list<char> work(p.cbegin(), p.cend());
	auto second = work.begin();
	auto first = second++;

	while (second != work.end())
	{
		if ((*first ^ *second) == 0x20) // if one character is upper case, the othe lower case and both are the same letter
		{
			second = work.erase(first, ++second); // erase both characters
			if (second == work.begin())
				first = second++; // increase second as we are already at string's start
			else
				--(first = second); // first goes back one character to check newly adjacent characters
		}
		else
			first = second++;
	}
	p.resize(work.size());
	std::copy(work.cbegin(), work.cend(), p.begin());
}

int workTask(std::string s, char c)
{
	s.resize(std::remove_if(s.begin(), s.end(), [c](char a) {return a == c || a == c + ('A' - 'a'); }) - s.begin());
	React(s);
	return s.size();
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage dec5.exe [filename]" << std::endl;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	std::string polymer;
	if (!(in >> polymer))
		return -1;

	React(polymer);
	std::cout << "Part 1: " << polymer.size() << std::endl;
	
	// Optimization: Reuse already reduced polymer as removing characters will not impact what were already adjacent pairs (what was removed will be removed again)
	// Performance: Each character to remove from the polymer and subsequent React is done in a separate task as they independent from each other.
	std::array<std::future<int>, 26> work;
	for (int i = 0; i < 26; ++i)
		work[i] = std::async(workTask, polymer, (char)('a' + i));
	
	int min = polymer.size();
	std::for_each(work.data(), work.data() + 26, [&min](std::future<int>& w) { min = std::min(min, w.get()); });

	std::cout << "Part 2: " << min << std::endl;
	return 0;
}