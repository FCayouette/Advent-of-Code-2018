#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <set>

using Registers = std::array<int, 6>;
const std::string instructionNames[] = { "addr", "addi","mulr", "muli", "banr", "bani", "borr", "bori", "setr", "seti", "gtir", "gtri", "gtrr", "eqir", "eqri", "eqrr" };

struct Instruction
{
	Instruction(const std::string& operation, int A, int B, int C) : a(A), b(B), c(C)
	{
		opCode = std::find(&(instructionNames[0]), &(instructionNames[16]), operation) - &(instructionNames[0]);
	}
	int opCode;
	int a, b, c;
};

void RunInstruction(const Instruction& i, Registers& r)
{
	switch (i.opCode)
	{
	case 0:  r[i.c] = r[i.a] + r[i.b];			return;
	case 1:  r[i.c] = r[i.a] + i.b;				return;
	case 2:  r[i.c] = r[i.a] * r[i.b];			return;
	case 3:  r[i.c] = r[i.a] * i.b;				return;
	case 4:  r[i.c] = r[i.a] & r[i.b];			return;
	case 5:  r[i.c] = r[i.a] & i.b;				return;
	case 6:  r[i.c] = r[i.a] | r[i.b];			return;
	case 7:  r[i.c] = r[i.a] | i.b;				return;
	case 8:  r[i.c] = r[i.a];					return;
	case 9:  r[i.c] = i.a;						return;
	case 10: r[i.c] = i.a > r[i.b] ? 1 : 0;		return;
	case 11: r[i.c] = r[i.a] > i.b ? 1 : 0;		return;
	case 12: r[i.c] = r[i.a] > r[i.b] ? 1 : 0;	return;
	case 13: r[i.c] = i.a == r[i.b] ? 1 : 0;	return;
	case 14: r[i.c] = r[i.a] == i.b ? 1 : 0;	return;
	case 15: r[i.c] = r[i.a] == r[i.b] ? 1 : 0; return;
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Dec21.exe [filename]" << std::endl;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	std::string line;
	std::getline(in, line);
	int ipRegister = -1;
	sscanf_s(line.c_str(), "#ip %d", &ipRegister);

	std::vector<Instruction> program;
	int a, b, c;
	int instructionToWatch =-1, registerToWatch = -1;
	while (in >> line >> a >> b >> c)
	{
		program.emplace_back(line, a, b, c);
		// Check if instruction compares register 0
		if (program.back().opCode == 15) // eqrr
		{
			instructionToWatch = program.size() - 1;
			registerToWatch = (a == 0 ? b : a);
		}
	}

	Registers registers;
	registers.fill(0);
	std::set<int> potentialValues;
	int lastPossible = 0;

	while (registers[ipRegister] >= 0 && registers[ipRegister] < (int)program.size())
	{
		if (registers[ipRegister] == instructionToWatch)
		{
			if (potentialValues.find(registers[registerToWatch]) == potentialValues.cend())
			{
				if (potentialValues.empty())
					std::cout << "Part 1: " << registers[registerToWatch] << std::endl;
				potentialValues.insert(registers[registerToWatch]);
				lastPossible = registers[registerToWatch];
			}
			else break;
		}
			
		RunInstruction(program[registers[ipRegister]], registers);
		++registers[ipRegister]; // Increase instruction pointer
	}
	
	std::cout << "Part 2: " << lastPossible << std::endl;
}