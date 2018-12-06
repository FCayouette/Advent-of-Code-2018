#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <algorithm>

template <class InputIterator1, class InputIterator2>
bool set_intersects(InputIterator1 first1, InputIterator1 last1,
					InputIterator2 first2, InputIterator2 last2)
{
	while (first1 != last1 && first2 != last2)
	{
		if (*first1 < *first2) ++first1;
		else if (*first2 < *first1) ++first2;
		else return true;
	}
	return false;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: dec1.exe [filename]" << std::endl;
		return 0;
	}

	std::ifstream in(argv[1], std::ios::in);
	int readValue = 0;
	std::vector<int> values;
	int total = 0;
	
	std::set<int> frequencies{ 0 };
	bool found = false;
	while (in >> readValue)
	{
		values.push_back(readValue);
		frequencies.insert(total);
		total += readValue;
		if (!found && frequencies.find(total) != frequencies.cend())
		{
			std::cout << "First repeat: " << total << std::endl;
			found = true;
		}
	}
	std::cout << "Total: " << total << std::endl; // Solution part 1
	
	// Second solution part 2 (if not found earlier)
	if (values.empty())
		return -1;
	if (found)
		return 0;

	int iteration = 0;
	std::vector<int> other(frequencies.begin(), frequencies.end());
	while (!found)
	{
		++iteration;
		std::for_each(other.begin(), other.end(), [total](int i) { return i + total; });
		found = set_intersects(frequencies.begin(), frequencies.end(), other.begin(), other.end());
	}

	total *= iteration;
	for (int i : values)
	{
		total += i;
		if (frequencies.find(total) != frequencies.cend())
		{
			std::cout << "First Repeat " << total << std::endl;
			return 0;
		}
	}
}