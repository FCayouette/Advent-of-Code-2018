#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>

struct Fighter;
using FighterList = std::vector<Fighter>;
using Arena = std::vector<std::string>;
FighterList::iterator FindFighterAt(FighterList& l, int x, int y);

int diffX[] = { -1, 0, 0, 1 };
int diffY[] = { 0, -1, 1, 0 };

// Required structures for fill algorithm
struct Point
{
	Point(int px = 0, int py = 0) : x(px), y(py) {}
	int x, y;
	bool operator<(const Point& p) const { return x < p.x || (x == p.x && y < p.y); }
	bool operator==(const Point& p) const { return x == p.x && y == p.y; }
};

struct FillInfo
{
	FillInfo() : from(-1, -1), distance(std::numeric_limits<int>::max()) {}
	FillInfo(const Point& p, int d) : from(p), distance(d) {}
	Point from;
	int distance;
};

using FillData = std::vector<std::vector<FillInfo>>;

struct Fighter
{
	Fighter(int px = 0, int py = 0, bool isAnElf = true) : x(px), y(py), health(200), combatPower(3), opponent(isAnElf ? 'G' : 'E'), elf(isAnElf), dead(false) {}
	int x, y;
	int health;
	int combatPower;
	char opponent;
	bool elf, dead;

	bool operator<(const Fighter& f) const
	{
		return x < f.x || (x == f.x && y < f.y);
	}

	bool HasOpponentInRange(const Arena& arena) const
	{
		return arena[x - 1][y] == opponent || arena[x + 1][y] == opponent || arena[x][y - 1] == opponent || arena[x][y + 1] == opponent;
	}

	FighterList::iterator FindBestAttackTarget(const Arena& arena, FighterList& list)
	{
		int minHealth = 2000;
		FighterList::iterator iter = list.end();
		if (arena[x - 1][y] == opponent) { iter = FindFighterAt(list, x - 1, y); minHealth = iter->health; }
		if (arena[x][y - 1] == opponent) { auto tmp = FindFighterAt(list, x, y - 1); if (tmp->health < minHealth) { minHealth = tmp->health; iter = tmp; } }
		if (arena[x][y + 1] == opponent) { auto tmp = FindFighterAt(list, x, y + 1); if (tmp->health < minHealth) { minHealth = tmp->health; iter = tmp; } }
		if (arena[x + 1][y] == opponent) { auto tmp = FindFighterAt(list, x + 1, y); if (tmp->health < minHealth) { return tmp; } }
		return iter;
	}

	bool Move(const Arena& arena, const FighterList& fighters, FillData& fill)
	{
		for (auto& f : fill)
			std::for_each(f.begin(), f.end(), [](FillInfo& fi) {fi.distance = std::numeric_limits<int>::max(); });
		std::set<Point> step;
		for (const Fighter& f : fighters)
			if (!f.dead && f.elf != elf) // for each opponent
				for (int i = 0; i < 4; ++i)
				{
					int nx = f.x + diffX[i], ny = f.y + diffY[i];
					if (arena[nx][ny] == '.')
					{
						fill[nx][ny] = FillInfo(Point(nx, ny), 0);
						step.insert(Point(nx, ny));
					}
				}

		int move = -1;
		while (!step.empty())
		{
			std::set<Point> nextIter;
			for (const Point& p : step)
				for (int i = 3; i >= 0; --i)
				{
					int nx = p.x + diffX[i], ny = p.y + diffY[i];
					if (arena[nx][ny]=='.')
						if (fill[nx][ny].distance > fill[p.x][p.y].distance + 1 ||
							(fill[nx][ny].distance == fill[p.x][p.y].distance + 1 && fill[p.x][p.y].from < fill[nx][ny].from))
						{
							fill[nx][ny] = FillInfo(fill[p.x][p.y].from, fill[p.x][p.y].distance + 1);
							nextIter.insert(Point(nx, ny));
							if (nx == x && ny == y)
								move = 3 - i;
						}
				}
			if (move != -1)
			{
				x += diffX[move];
				y += diffY[move];
				return true;
			}
			std::swap(step, nextIter);
		}
		return false;
	}
};

FighterList::iterator FindFighterAt(FighterList& l, int x, int y)
{
	return std::find_if(l.begin(), l.end(), [x, y](const Fighter& f) { return x == f.x && y == f.y; });
}

bool NoRemainingOpponents(const FighterList& list)
{
	int e = 0, g = 0;
	for (const auto& f : list)
		if (!f.dead)
		{
			if (f.elf) ++e;
			else ++g;
		}
	return e == 0 || g == 0;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Dec15.exe []" << std::endl;;
		return -1;
	}
	
	std::ifstream in(argv[1], std::ios::in);

	std::vector<std::string> arenaBackup;
	std::string line;
	FighterList fightersBackup;
	int elves = 0;
	while (std::getline(in, line))
	{
		// Find goblins and elves
		size_t index = 0;
		while ((index = line.find_first_of("EG", index)) != std::string::npos)
		{
			if (line[index] == 'E') ++elves;
			fightersBackup.emplace_back(arenaBackup.size(), index, line[index] == 'E');
			++index;
		}
		arenaBackup.push_back(line);
	}
	FillData fill = FillData(arenaBackup.size(), std::vector<FillInfo>(arenaBackup[0].length()));

	int elvePower = 3;
	FighterList fighters;
	bool stableState = false;
	do
	{
		std::vector<std::string> arena = arenaBackup;
		fighters.clear();

		fighters = fightersBackup;

		for (auto& f : fighters)
			if (f.elf)
				f.combatPower = elvePower;

		int rounds = 0;
		bool fightGoingOn = true;
		while (fightGoingOn)
		{
			std::sort(fighters.begin(), fighters.end());
			bool stable = true;
			for (Fighter& f : fighters)
			{
				if (NoRemainingOpponents(fighters))
				{
					fightGoingOn = false;
					break;
				}
				if (f.dead)
					continue;
				// Check if it should move
				if (!f.HasOpponentInRange(arena) && !stableState)
				{
					arena[f.x][f.y] = '.';
					if (f.Move(arena, fighters, fill))
						stableState = stable = false;
					arena[f.x][f.y] = f.elf ? 'E' : 'G';
				}

				// Combat, determine best target if any
				FighterList::iterator target = f.FindBestAttackTarget(arena, fighters);
				if (target != fighters.end())
				{
					target->health -= f.combatPower;
					if (target->health < 1)
					{
						target->dead = true;
						stableState = stable = false;
						arena[target->x][target->y] = '.';
						if (target->elf && target->combatPower != 3)
						{
							fightGoingOn = false;
							break;
						}
					}
				}
			}
			stableState = stable;
			// Remove casualties from Fighters list
			fighters.erase(std::remove_if(fighters.begin(), fighters.end(), [](Fighter& f) { return f.dead; }), fighters.end());
			if (fightGoingOn)
				++rounds;
		}

		if (elvePower == 3)
		{
			//Score computation
			int totalHealth = 0;
			for (const auto& f : fighters)
				totalHealth += f.health;
			std::cout << "Part 1: " << totalHealth * rounds << std::endl;
		}
		else if (std::count_if(fighters.cbegin(), fighters.cend(), [](const Fighter& f) {return f.elf; }) == elves)
		{
			int totalHealth = 0;
			for (const auto& f : fighters)
				totalHealth += f.health;
			
			std::cout << "Part 2: " << totalHealth * rounds << std::endl;
		}
		++elvePower;
	} while (std::count_if(fighters.cbegin(), fighters.cend(), [](const Fighter& f) {return f.elf; }) != elves);

	return 0;
}