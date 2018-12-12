#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Dec11.exe [serialnumber]" << std::endl;
		return -1;
	}
	int serialNumber = atoi(argv[1]);

	std::vector<char> grid(300 * 300); // If you have to run in debug use char grid[300*300] instead to avoid unnecessary bound checks
	
	// Compute power level for each coordinate
	for (int x = 0; x < 300; ++x)
	{
		int rackId = x + 11;
		for (int y = 0; y < 300; ++y)
		{
			int powerLevel = rackId * (y+1) + serialNumber;
			powerLevel *= rackId;
			powerLevel = (powerLevel / 100) % 10 - 5;
			grid[x*300 + y] = (char)powerLevel;
		}
	}

	// Part 1, check all posible 3x3 squares
	// To avoid doing redundant additions, we store the results of all additions of three contiguous values of the previous 2 rows
	int maxPower = std::numeric_limits<int>::min();
	int maxX = 0, maxY = 0;
	
	std::vector<int> runningScore(298 * 3); // If you have to run in debug use int runningScore[3*298] instead to avoid unnecessary bound checks
	for (int x = 0; x < 2; ++x)
	{
		int runningCount = grid[x * 300] + grid[x * 300 + 1];
		for (int y = 0; y < 298; ++y)
		{
			runningCount += grid[x * 300 + y + 2];
			runningScore[x*298+y] = runningCount;
			runningCount -= grid[x * 300 + y];
		}
	}
	for (int x = 2; x < 300; ++x)
	{
		int row = x % 3;
		int runningCount = grid[x * 300] + grid[x * 300 + 1];
		for (int y = 0; y < 298; ++y)
		{
			runningCount += grid[x * 300 + y + 2];
			runningScore[row * 298 + y] = runningCount;
			runningCount -= grid[x * 300 + y];
			int total = runningScore[y] + runningScore[y + 298] + runningScore[y + 2*298];
			if (total > maxPower)
			{
				maxX = x - 1;
				maxY = y + 1;
				maxPower = total;
			}
		}
	}
	std::cout << "Part 1: " << maxX << ',' << maxY << "  Max power: " << maxPower << std::endl;

	// Find the minimal size of a square that could beat the maximum result. (a coordinate maximum value is 4)
	int minS = 2;
	while (minS*minS * 4 < maxPower)
		++minS;
	if (minS == 3)
		++minS; // We already checked all 3x3 possibilities.

	int maxS = 3; // From part 1
	
	for (int x = 0; x < 300 - minS; ++x) // Avoid processing coordinates that will not yield a valid minimal size square in the 300x300 grid
		for (int y = 0; y < 300 - minS; ++y)
		{
			bool recheck = false;
			int power = 0;
			// Compute minimal size square at this coordinate
			for (int i = 0; i < minS; ++i)
				for (int j = 0; j < minS; ++j)
					power += grid[(x + i)*300 + y + j];
			if(power > maxPower)
			{
				maxPower = power;
				maxX = x+1;
				maxY = y+1;
				maxS = minS;
				recheck = true;
			}
			// Now grow size until maximum allowed by index by adding new coordinates encompassed by the square
			int maxXY = std::max(x, y);
			for (int s = minS; s + maxXY < 300; ++s)
			{
				for (int i = 0; i < s; ++i)
				{
					power += grid[(x + i)*300 + y + s];
					power += grid[(x + s)*300 + y + i];
				}
				power += grid[(x + s)*300 + y + s];

				if (power > maxPower)
				{
					maxPower = power;
					maxX = x+1;
					maxY = y+1;
					maxS = s+1;
					recheck = true;
				}
			}

			if (recheck) // If better result is found, make sure we only look for squares that has the required minimal size
				while ((minS + 1) * (minS + 1) * 4 < maxPower || minS == 3)
					++minS;
		}

	std::cout << "Part 2: " << maxX << ',' << maxY << ',' << maxS << "  Max power: " << maxPower << std::endl;
	return 0;
}