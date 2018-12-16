#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <string>

using Registers = std::array<int, 4>;

struct Instruction
{
	Instruction(int op, int A, int B, int C) : opCode(op), a(A), b(B), c(C) {}
	int opCode;
	int a, b, c;
};

const std::string instructionNames[] = { "addr", "addi","mulr", "muli", "banr", "bani", "borr", "bori", "setr", "seti", "gtir", "gtri", "gtrr", "eqir", "eqri", "eqrr" };
void RunInstruction(const Instruction& i, Registers& r)
{
	switch (i.opCode)
	{
		case 0:  r[i.c] = r[i.a] + r[i.b]; return;
		case 1:  r[i.c] = r[i.a] + i.b; return;
		case 2:  r[i.c] = r[i.a] * r[i.b]; return;
		case 3:  r[i.c] = r[i.a] * i.b; return;
		case 4:  r[i.c] = r[i.a] & r[i.b]; return;
		case 5:  r[i.c] = r[i.a] & i.b; return;
		case 6:  r[i.c] = r[i.a] | r[i.b]; return;
		case 7:  r[i.c] = r[i.a] | i.b; return;
		case 8:  r[i.c] = r[i.a]; return;
		case 9:  r[i.c] = i.a; return;
		case 10: r[i.c] = i.a > r[i.b] ? 1 : 0; return;
		case 11: r[i.c] = r[i.a] > i.b ? 1 : 0; return;
		case 12: r[i.c] = r[i.a] > r[i.b] ? 1 : 0; return;
		case 13: r[i.c] = i.a == r[i.b] ? 1 : 0; return;
		case 14: r[i.c] = r[i.a] == i.b ? 1 : 0; return;
		case 15: r[i.c] = r[i.a] == r[i.b] ? 1 : 0; return;
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage dec16.exe []" << std::endl;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	std::string line;
	int count = 0;
	std::array<int, 4> initialRegisters, targetRegisters, work;

	std::array<std::array<bool, 16>, 16> opCodeTable;
	for (int i = 0; i < 16; ++i)
		opCodeTable[i].fill(true);

	while (true)
	{
		int a, b, c, d;
		if (!std::getline(in, line) || line[0] != 'B') break;
		sscanf_s(line.c_str(), "Before: [%d, %d, %d, %d]", &a, &b, &c, &d);
		initialRegisters = { a, b, c, d };
		std::getline(in, line);
		sscanf_s(line.c_str(), "%d %d %d %d", &a, &b, &c, &d);
		int opCode = a;
		Instruction inst(0, b, c, d);
		std::getline(in, line);
		sscanf_s(line.c_str(), "After:  [%d, %d, %d, %d]", &a, &b, &c, &d);
		targetRegisters = { a, b, c, d };
		std::getline(in, line); // Get empty line

		int sameEffects = 0;
		for (; inst.opCode < 16; ++inst.opCode)
		{
			work = initialRegisters;
			RunInstruction(inst, work);
			bool same = true;
			if (std::mismatch(work.cbegin(), work.cend(), targetRegisters.begin()).first != work.cend()) // C++17
				opCodeTable[opCode][inst.opCode] = false;
			else
				++sameEffects;
		}
		if (sameEffects >= 3)
			++count;
	}

	std::cout << "Part 1: " << count << std::endl;
	std::array<int,16> opCodeConversionTable;
	opCodeConversionTable.fill(-1);

	int toConvert = 16;
	while(toConvert > 0)
	{
		// Check if there is an opcode with unique conversion
		for (int i = 0; toConvert > 0 && i < 16; ++i)
			if (std::count(opCodeTable[i].cbegin(), opCodeTable[i].cend(), true) == 1)
			{
				int index = std::find(opCodeTable[i].cbegin(), opCodeTable[i].cend(), true) - opCodeTable[i].cbegin();
				opCodeConversionTable[i] = index;
				for (int j = 0; j < 16; ++j)
					opCodeTable[j][index] = false;
				--toConvert;
			}
		// Check if there is a conversion with a single opcode
		for (int i = 0; toConvert > 0 && i < 16; ++i)
		{
			int count = 0, first = -1;
			for (int j = 0; j < 16; ++j)
				if (opCodeTable[j][i])
				{
					if (++count == 1)
						first = j;
					else break;
				}
			if (count == 1)
			{
				opCodeTable[first].fill(false);
				opCodeConversionTable[first] = i;
				--toConvert;
			}
		}
	}
	for (int i = 0; i < 16; ++i)
		std::cout << "OpCode " << i << " is " << instructionNames[opCodeConversionTable[i]] << std::endl;
	
	std::getline(in, line); // Read empty line before program
	work.fill(0);
	while (std::getline(in, line))
	{
		int a, b, c, d;
		sscanf_s(line.c_str(), "%d %d %d %d", &a, &b, &c, &d);
		Instruction i(opCodeConversionTable[a], b, c, d);
		RunInstruction(i, work);
	}

	std::cout << "Part 2: " << work[0] << std::endl;

	return 0;
}
