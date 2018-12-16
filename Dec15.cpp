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

// Required for fill algorithm
struct Point
{
	Point(int px = 0, int py = 0) : x(px), y(py) {}
	int x, y;
	bool operator<(const Point& p) const { return x < p.x || (x == p.x && y < p.y); }
	bool operator==(const Point& p) const { return x == p.x && y == p.y; }
};

struct Fighter
{
	Fighter(int px = 0, int py = 0, bool isAnElf = true) : x(px), y(py), health(200), combatPower(3), opponent(isAnElf ? 'G' :'E'), elf(isAnElf), dead(false) {}
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

int diffX[] = { -1, 0, 0, 1 };
int diffY[] = { 0, -1, 1, 0 };

bool Move(const Arena& arena, Fighter& f, FighterList& list)
{
	std::set<Point> fill;
	fill.emplace(f.x, f.y);
	std::set<Point> targets;
	while (true)
	{
		std::set<Point> step;
		for (const Point& p : fill)
			for (int i = 0; i < 4; ++i)
			{
				Point temp(p.x + diffX[i], p.y+diffY[i]);
				
				char loc = arena[temp.x][temp.y];
				if (loc == f.opponent)
					targets.insert(p);
				else if (loc == '.' && fill.find(temp) == fill.cend())
					step.insert(temp);
			}
		
		if (!targets.empty())
			break;
		if (step.empty())
			return false; // There are no valid targets
		fill.insert(step.cbegin(), step.cend());
	}
	
	// Target to move to is the first in the set
	fill.clear();
	fill.insert(*targets.begin());
	
	// Work backward toward fighter
	while (true)
	{
		std::set<Point> step;
		int moveDirection = 4;
		for (const Point& p : fill)
		{
			if (p.x + 1 == f.x && p.y == f.y)
			{
				--f.x;
				return true;
			}
			if (arena[p.x + 1][p.y] == '.') { Point tmp(p.x + 1, p.y); if (fill.find(tmp) == fill.end()) step.insert(tmp); }

			if (p.x == f.x && p.y + 1 == f.y && moveDirection > 0)
				moveDirection = 0;
			if (arena[p.x][p.y + 1] == '.') { Point tmp(p.x, p.y + 1); if (fill.find(tmp) == fill.end()) step.insert(tmp); }
			
			if (p.x == f.x && p.y - 1 == f.y && moveDirection > 1)
				moveDirection = 1;
			if (arena[p.x][p.y - 1] == '.') { Point tmp(p.x, p.y - 1); if (fill.find(tmp) == fill.end()) step.insert(tmp); }

			if (p.x - 1 == f.x && p.y == f.y && moveDirection > 2)
				moveDirection = 2;
			if (arena[p.x - 1][p.y] == '.') { Point tmp(p.x - 1, p.y); if (fill.find(tmp) == fill.end()) step.insert(tmp); }
		}

		if (moveDirection < 4)
		{
			if (moveDirection == 0)
				--f.y;
			else if (moveDirection == 1)
				++f.y;
			else
				++f.x;
			return true;
		}

		if (step.empty()) //We do not have a valid path
			return false;
		fill.insert(step.begin(), step.end());
	}	
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
		std::cout << line << std::endl;
	}

	int elvePower = 3;
	FighterList fighters;
	bool stableState = false;
	do
	{
		std::cout << "Combat Power " << elvePower << std::endl;
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
					if (Move(arena, f, fighters))
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
			std::cout << "Score: " << totalHealth * rounds << ' ' << totalHealth << ' ' << rounds << std::endl;
		}
		else if (std::count_if(fighters.cbegin(), fighters.cend(), [](const Fighter& f) {return f.elf; }) == elves)
		{
			int totalHealth = 0;
			for (const auto& f : fighters)
				totalHealth += f.health;
			std::cout << "Score: " << totalHealth * rounds << ' ' << totalHealth << ' ' << rounds << ' ' << elvePower << std::endl;
		}
		++elvePower;
	} while (std::count_if(fighters.cbegin(), fighters.cend(), [](const Fighter& f) {return f.elf; }) != elves);

	return 0;
}
