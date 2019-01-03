#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>

using Registers = std::array<int, 6>;
const std::string instructionNames[] = { "addr", "addi","mulr", "muli", "banr", "bani", "borr", "bori", "setr", "seti", "gtir", "gtri", "gtrr", "eqir", "eqri", "eqrr" };

struct Instruction
{
	Instruction(const std::string& operation, int A, int B, int C) : opCode(std::find(&(instructionNames[0]), &(instructionNames[16]), operation) - &(instructionNames[0])), a(A), b(B), c(C) {}
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
		std::cout << "Usage: Dec19.exe [filename]" << std::endl;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	std::string line;
	std::getline(in, line);
	int ipRegister = -1;
	sscanf_s(line.c_str(), "#ip %d", &ipRegister);

	std::vector<Instruction> program;
	int a, b, c;
	while (in >> line >> a >> b >> c)
		program.emplace_back(line, a, b, c);

	Registers registers;
	registers.fill(0);

	while (registers[ipRegister] >= 0 && registers[ipRegister] < (int)program.size())
	{
		RunInstruction(program[registers[ipRegister]], registers);
		++registers[ipRegister]; // Increase instruction pointer
	}
	
	std::cout << "Part 1: " << registers[0] << std::endl;
	
	// Part 2. Might not be generic but it worked with the program copied as comments below
	registers.fill(0);
	registers[0] = 1;
	while(registers[ipRegister] != 2) // Wait until it computes the number we have to extract the factors of
	{
		RunInstruction(program[registers[ipRegister]], registers);
		++registers[ipRegister];
	}
	
	long long total = 1 + registers[4];
	int i = 2;
	for (; i*i < registers[4]; ++i)
		if (registers[4] % i == 0)
			total += i + registers[4]/i;
	if (i*i == registers[4] && registers[4] % i == 0)
		total += i;

	std::cout << "Part 2: " << total << std::endl;
}

/* Input for Day 19
#ip 1
addi 1 16 1
seti 1 2 5
seti 1 2 2
mulr 5 2 3
eqrr 3 4 3
addr 3 1 1
addi 1 1 1
addr 5 0 0
addi 2 1 2
gtrr 2 4 3
addr 1 3 1
seti 2 8 1
addi 5 1 5
gtrr 5 4 3
addr 3 1 1
seti 1 1 1
mulr 1 1 1
addi 4 2 4
mulr 4 4 4
mulr 1 4 4
muli 4 11 4
addi 3 3 3
mulr 3 1 3
addi 3 4 3
addr 4 3 4
addr 1 0 1
seti 0 0 1
setr 1 5 3
mulr 3 1 3
addr 1 3 3
mulr 1 3 3
muli 3 14 3
mulr 3 1 3
addr 4 3 4
seti 0 0 0
seti 0 1 1
*/
