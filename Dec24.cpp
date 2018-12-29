#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

struct Group
{
	Group(const std::string& s) : target(-1), targeted(false)
	{
		sscanf_s(s.c_str(), "%d units each with %d hit points", &units, &hitPoints);

		int index;
		if ((index = s.find('(')) != std::string::npos)
		{
			// Process weaknesses and immunities
			std::string work = s.substr(index + 1, s.find(')') - index - 1);

			// Weaknesses
			if ((index = s.find("weak to ")) != std::string::npos)
			{
				std::string weak = s.substr(index + 8);
				if ((index = weak.find_first_of(";)")) != std::string::npos)
				{
					weak = weak.substr(0, index);
				}
				index = 0;
				while (index != std::string::npos)
				{
					index = weak.find(',');
					weaknesses.push_back(ConvertToType(weak.substr(0, index)));
					if (index != std::string::npos)
						weak = weak.substr(index + 2);
				}
			}
			if ((index = s.find("immune to ")) != std::string::npos)
			{
				std::string imm = s.substr(index + 10);
				if ((index = imm.find_first_of(";)")) != std::string::npos)
				{
					imm = imm.substr(0, index);
				}
				index = 0;
				while (index != std::string::npos)
				{
					index = imm.find(',');
					immunities.push_back(ConvertToType(imm.substr(0, index)));
					if (index != std::string::npos)
						imm = imm.substr(index + 2);
				}
			}
		}

		index = s.find("does ");
		std::string secondPart = s.substr(index);
		char buffer[64];
		memset(buffer, 0, 64 * sizeof(char));
		sscanf_s(secondPart.c_str(), "does %d %s damage at initiative %d", &attack, buffer, 64, &initiative);

		attackType = ConvertToType(buffer);

		effectivePower = units * attack;
	}

	int units;
	int hitPoints;
	int attack;
	int attackType;
	int initiative;
	int effectivePower;
	int target;
	bool targeted;

	std::vector<int> weaknesses;
	std::vector<int> immunities;
	static std::vector<std::string> typeMap;

	bool operator<(const Group& g) const { return effectivePower > g.effectivePower || (effectivePower == g.effectivePower && initiative > g.initiative); }

	int DamageModifier(int type)
	{
		if (std::find(immunities.cbegin(), immunities.cend(), type) != immunities.end())
			return 0;
		if (std::find(weaknesses.cbegin(), weaknesses.cend(), type) != weaknesses.end())
			return 2;
		return 1;
	}

	void DealDamage(int efPower, int type)
	{
		int mod = DamageModifier(type);
		int totalDamage = efPower * mod;
		int unitLoss = totalDamage / hitPoints;
		units = std::max(0, units - unitLoss);
		effectivePower = units * attack;
	}

	int ConvertToType(const std::string& s)
	{
		auto iter = std::find(typeMap.begin(), typeMap.end(), s);
		if (iter != typeMap.end())
			return iter - typeMap.begin();
		typeMap.push_back(s);
		return typeMap.size() - 1;
	}
};

std::vector<std::string> Group::typeMap = std::vector<std::string>();

void DetermineTargets(std::vector<Group>& attackers, std::vector<Group>& defenders)
{
	for (Group& d : defenders)
		d.targeted = false;
	for (int a = 0; a < attackers.size(); ++a)
	{
		// Determine best target
		int index = -1, maxDamage = 0;
		attackers[a].target = -1; //invalid target at first
		for (int d = 0; d < defenders.size(); ++d)
		{
			if (!defenders[d].targeted)
			{
				int mod = defenders[d].DamageModifier(attackers[a].attackType);
				if (mod > maxDamage)
				{
					maxDamage = mod;
					index = d;
				}
			}
		}
		if (index != -1)
		{
			attackers[a].target = index;
			defenders[index].targeted = true;
		}
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Dec24.exe []" << std::endl;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	std::string line;
	std::getline(in, line); // Immune system
	std::vector<Group> immuneBackup;

	int highestInitiative = 0;
	while (true)
	{
		std::getline(in, line);
		if (line.empty())
			break;
		immuneBackup.emplace_back(line);
		if (immuneBackup.back().initiative > highestInitiative)
			highestInitiative = immuneBackup.back().initiative;
	}
	
	std::getline(in, line); // Get rid of Infection:
	
	std::vector<Group> infectionBackup;
	while (std::getline(in, line))
	{
		infectionBackup.emplace_back(line);
		if (infectionBackup.back().initiative > highestInitiative)
			highestInitiative = infectionBackup.back().initiative;
	}

	int boost = 0;
	bool stalemate;
	std::vector<Group> immuneSystem, infection;
	do
	{
		immuneSystem = immuneBackup;
		infection = infectionBackup;
		stalemate = false;
		for (Group& g : immuneSystem)
		{
			g.attack += boost;
			g.effectivePower = g.attack * g.units;
		}
		int count = 0;
		for (const Group& g : immuneSystem) count += g.units;
		for (const Group& g : infection) count += g.units;
		while (!immuneSystem.empty() && !infection.empty() && !stalemate)
		{
			std::sort(immuneSystem.begin(), immuneSystem.end());
			std::sort(infection.begin(), infection.end());
			
			DetermineTargets(immuneSystem, infection);
			DetermineTargets(infection, immuneSystem);

			for (int ini = highestInitiative; ini >= 0; --ini)
			{
				auto iter = std::find_if(immuneSystem.cbegin(), immuneSystem.cend(), [ini](const Group& g) { return g.initiative == ini; });
				if (iter != immuneSystem.cend())
				{
					if (iter->target != -1)
						infection[iter->target].DealDamage(iter->effectivePower, iter->attackType);
				}
				else
				{
					iter = std::find_if(infection.cbegin(), infection.cend(), [ini](const Group& g) { return g.initiative == ini; });
					if (iter != infection.cend() && iter->target != -1)
						immuneSystem[iter->target].DealDamage(iter->effectivePower, iter->attackType);
				}
			}

			// Remove dead groups
			immuneSystem.erase(std::remove_if(immuneSystem.begin(), immuneSystem.end(), [](const Group& g) {return g.units <= 0; }), immuneSystem.end());
			infection.erase(std::remove_if(infection.begin(), infection.end(), [](const Group& g) {return g.units <= 0; }), infection.end());

			// Stalemate detection
			int newCount = 0;
			for (const Group& g : immuneSystem) newCount += g.units;
			for (const Group& g : infection) newCount += g.units;
			if (newCount != count)
				count = newCount;
			else stalemate = true;
		}
		if (boost == 0)
		{
			int unitRemaining = 0;
			for (const Group& g : infection)
				unitRemaining += g.units;
			std::cout << "Part 1: " << unitRemaining << std::endl;
			
		}
		++boost;
	} while (immuneSystem.empty() || stalemate);
	
	int unitRemaining = 0;
	for (const Group& g : immuneSystem)
		unitRemaining += g.units;

	std::cout << "Part 2: " << unitRemaining << std::endl;

	return 0;
}