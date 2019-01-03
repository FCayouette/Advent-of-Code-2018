#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <future>

int Reduce(const std::vector<char>& source, char ignore)
{
	std::vector<char> reduced;
	reduced.reserve(source.size());
	for (char c : source)
		if (c != ignore && c != ignore + ('A' - 'a'))
		{
			if (reduced.empty() || (reduced.back() ^ c) != 0x20)
				reduced.push_back(c);
			else
				reduced.pop_back();
		}

	return reduced.size();
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage dec5.exe [filename]" << std::endl;
		return -1;
	}
	
	std::ifstream in(argv[1], std::ios::in);

	std::vector<char> polymer;
	char c;
	while (in >> c)
	{
		if (polymer.empty() || (polymer.back() ^ c) != 0x20) // both are the same character but one lower case the other upper case
			polymer.push_back(c);
		else
			polymer.pop_back();
	}
	std::cout << "Part 1: " << polymer.size() << std::endl;

	// Optimization: Reuse already reduced polymer as removing characters will not impact what were already adjacent pairs (what was removed will be removed again)
	// Performance: Each character to remove from the polymer and subsequent Reduce is done in a separate task as they are independent from each other.
	std::array<std::future<int>, 26> work;
	for (int i = 0; i < 26; ++i)
		work[i] = std::async(Reduce, polymer, (char)('a' + i));

	int min = polymer.size();
	std::for_each(work.data(), work.data() + 26, [&min](std::future<int>& w) { min = std::min(min, w.get()); });
	
	std::cout << "Part 2: " << min << std::endl;

	return 0;
}