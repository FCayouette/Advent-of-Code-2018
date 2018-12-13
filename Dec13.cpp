#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>

using Tracks = std::vector<std::string>;

enum TurnSequence
{
	left = 0,
	straight,
	right,
	MaxSequence
};

const int directionX[] = { 0, 1, 0, -1 };
const int directionY[] = { -1, 0, 1, 0 };
const char directionChars[] = { '^', '>', 'v', '<' };

struct Cart
{
	Cart() : atNext(left), crashed(false) {}
	Cart(int x, int y, char dir) : posX(x), posY(y), atNext(left), crashed(false)
	{
		if (dir == '^')      { direction = 0; trackCharacter = '|'; }
		else if (dir == '>') { direction = 1; trackCharacter = '-'; }
		else if (dir == 'v') { direction = 2; trackCharacter = '|'; }
		else /*dir == '<'*/  { direction = 3; trackCharacter = '-'; }
		ComputeDirection();
	}

	int posX, posY;
	int dirX, dirY, direction;
	TurnSequence atNext;
	char trackCharacter, directionCharacter;
	bool crashed;

	void ComputeDirection()
	{
		dirX = directionX[direction];
		dirY = directionY[direction];
		directionCharacter = directionChars[direction];
	}

	void TurnLeft()
	{
		direction = (direction + 3) % 4;
		ComputeDirection();
	}

	void TurnRight()
	{
		direction = (direction + 1) % 4;
		ComputeDirection();
	}

	bool operator<(const Cart& c) const
	{
		return posY < c.posY || (posY == c.posY && posX < c.posX);
	}
	
	bool Move( Tracks& tracks)
	{
		if (tracks[posY][posX] == 'X')
			// We have crashed
			return crashed = true;
		
		char nextPos = tracks[posY + dirY][posX + dirX];
				
		tracks[posY][posX] = trackCharacter;
		posX += dirX;
		posY += dirY;
		trackCharacter = nextPos;
		if (nextPos == '<' || nextPos == '^' || nextPos == '>' || nextPos == 'v' || nextPos =='X')
		{
			tracks[posY][posX] = 'X';
			crashed = true;
			return true;
		}

		// No processing required for straights
		
		// Intersections
		if (nextPos == '+')
		{
			if (atNext == left)
				TurnLeft();
			else if (atNext == right)
				TurnRight();
			
			atNext = (TurnSequence)((atNext + 1) % MaxSequence);
		}
		// Bends
		else if (nextPos == '/')
		{
			if (dirX == 0)
				TurnRight();
			else
				TurnLeft();
		}
		else if (nextPos == '\\')
		{
			if (dirX == 0)
				TurnLeft();
			else
				TurnRight();
		}
		tracks[posY][posX] = directionCharacter;
		return false;
	}
};

bool ValidTrackCharacter(char c)
{
	return c == '-' || c == '|' || c == '+' || c == '\\' || c == '/';
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Dec12 [filename]" << std::endl;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	std::string line;
	std::vector<Cart> carts;
	Tracks tracks;
	int y = 0;
	// Read tracks
	while (std::getline(in, line))
	{
		tracks.push_back(line);
		// Find carts on the line
		
		int index = 0;
		while ((index = line.find_first_of("<>^v", index)) != std::string::npos)
		{
			carts.emplace_back(index, y, line[index]);
			++index;
		}
		++y;
	}

	bool firstCrash = false;
	int iteration = 0;
	while (carts.size()>1)
	{ 
		// Make sure we process in the correct order
		std::sort(carts.begin(), carts.end());
		bool hasCrash = false;
		for (Cart& c : carts)
			if (c.Move(tracks))
			{
				if (!firstCrash)
				{
					firstCrash = true;
					std::cout << "First crash at " << c.posX << ',' << c.posY << std::endl;
				}
				hasCrash = true;
			}

		if (hasCrash)
		{
			std::for_each(carts.begin(), carts.end(), [&tracks](const Cart&c)
				{if (c.crashed && ValidTrackCharacter(c.trackCharacter))
					tracks[c.posY][c.posX] = c.trackCharacter;
				});
			carts.erase(std::remove_if(carts.begin(), carts.end(), [](const Cart& c) {return c.crashed; }), carts.end());
		}
		++iteration;
	}
	if (!carts.empty())
		std::cout << "Last cart at " << carts.begin()->posX << ',' << carts.begin()->posY << std::endl;
	std::cout << "Took " << iteration << " iterations" << std::endl;

	return 0;
}