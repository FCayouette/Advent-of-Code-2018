#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>

void InsertMarble(std::list<int>& circle, std::list<int>::iterator& currentMarble, int marble)
{
	for (int i = 0; i < 2; ++i)
		if (++currentMarble == circle.end())
			currentMarble = circle.begin();
	currentMarble = circle.insert(currentMarble, marble);
}

int RemoveMarble(std::list<int>& circle, std::list<int>::iterator& currentMarble)
{
	for (int i = 0; i < 7; ++i)
	{
		if (currentMarble == circle.begin())
			currentMarble = circle.end();
		--currentMarble;
	}
	int result = *currentMarble;
	currentMarble = circle.erase(currentMarble);
	if (currentMarble == circle.end())
		currentMarble = circle.begin();
	return result;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Dec9.exe [filename]" << std::endl;
		return -1;
	}
	
	std::ifstream in(argv[1], std::ios::in);
	
	std::string input;
	std::getline(in, input);
	int players, marbles;
	sscanf_s(input.c_str(), "%d players; last marble is worth %d", &players, &marbles);
	++marbles;

	std::vector<long long> scores(players, 0); //Using long long as part 2 creates integer overflow on 32 bits integers
	std::list<int> circle;

	circle.push_back(0);
	auto currentPosition = circle.begin();
	int currentPlayer = 0;

	for (int i = 1; i < marbles; ++i, currentPlayer = (currentPlayer + 1) % players)
	{
		if (i % 23 == 0) // Scoring
			scores[currentPlayer] += RemoveMarble(circle, currentPosition) + i;
		else
			InsertMarble(circle, currentPosition, i);
	}

	std::cout << "Part1 Max score: " << *std::max_element(scores.cbegin(), scores.cend()) << std::endl;

	int newMarbles = (marbles-1) * 100 + 1; // (Don't forget, we increased marbles by 1 earlier)
	for (int i = marbles; i < newMarbles; ++i, currentPlayer = (currentPlayer + 1) % players)
	{
		if (i % 23 == 0) // Scoring
			scores[currentPlayer] += RemoveMarble(circle, currentPosition) + i;
		else
			InsertMarble(circle, currentPosition, i);
	}

	std::cout << "Part2 Max score: " << *std::max_element(scores.cbegin(), scores.cend()) << std::endl;

	return 0;
}