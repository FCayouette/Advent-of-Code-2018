#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using Result = std::pair<int, int>;

Result ProcessTreeNode(std::ifstream& in)
{
	int childs, metadatas;
	in >> childs >> metadatas;
	if (childs == 0)
	{
		int sum = 0;
		for (int i = 0; i < metadatas; ++i)
		{
			int tmp;
			if (!(in >> tmp))
				throw - 1;
			sum += tmp;
		}
		return std::make_pair(sum, sum);
	}

	int sum = 0, indexedSum = 0;
	std::vector<int> nodes(childs, 0);
	std::for_each(nodes.begin(), nodes.end(), [&in, &sum](int& secondPart) { Result r = ProcessTreeNode(in); sum += r.first; secondPart = r.second; });
	
	for (int i = 0; i < metadatas; ++i)
	{
		int index;
		in >> index;
		sum += index;
		--index; // Indices are 1 based
		if (index >= 0 && index < childs)
			indexedSum += nodes[index];
	}
	return std::make_pair(sum, indexedSum);
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage dec8.exe [filename]" << std::endl;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	
	Result t = ProcessTreeNode(in);
	std::cout << "Part 1 result: " << t.first << std::endl << "Part 2 result: " << t.second << std::endl;
	
	return 0;
}
