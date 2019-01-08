#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

template <typename T>
constexpr void GrowToEncompass(T& min, T& max, T val)
{
	min = std::min(min, val);
	max = std::max(max, val);
}

struct Nanobot
{
	Nanobot(int X = 0, int Y = 0, int Z = 0, int r = 1) : x(X), y(Y), z(Z), range(r) {}
	bool InRange(const Nanobot& nb) const { return std::abs(nb.x - x) + std::abs(nb.y - y) + std::abs(nb.z - z) <= range; }
	int x, y, z, range;
};

struct Voxel
{
	Voxel(int x, int X, int y, int Y, int z, int Z) : minX(x), maxX(X), minY(y), maxY(Y), minZ(z), maxZ(Z), score(0), scale(X - x + 1)
		, distance((std::abs(X + x) + std::abs(Y + y) + std::abs(Z + z)) / 2) {} // Manhattan distance from origin to the Voxel's center

	void CheckBot(const Nanobot& b)
	{	// Checks if bot range touches the voxel
		if (b.range - std::max(minX - b.x, 0) - std::max(b.x - maxX, 0) - std::max(minY - b.y, 0) - std::max(b.y - maxY, 0)
			- std::max(minZ - b.z, 0) - std::max(b.z - maxZ, 0) >= 0)
			++score;
	}

	bool operator<(const Voxel& v) const { return score < v.score || (score == v.score && (distance > v.distance || (distance == v.distance && scale > v.scale))); }

	int minX, minY, minZ, maxX, maxY, maxZ; // Range values are inclusive
	int score, scale, distance;
};


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Dec23.exe [NanobotsFilename]" << std::endl;
		return -1;
	}

	std::ifstream in(argv[1], std::ios::in);
	std::vector<Nanobot> bots;

	int maxRange = -1;
	int maxRangeBot = -1;

	int _a, _b, _c, _r;

	int minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
	std::string line;
	while (std::getline(in, line))
	{
		sscanf_s(line.c_str(), "pos=<%d,%d,%d>, r=%d", &_a, &_b, &_c, &_r);
		if (_r > maxRange)
		{
			maxRangeBot = bots.size();
			maxRange = _r;
		}
		bots.emplace_back(_a, _b, _c, _r);
		GrowToEncompass(minX, maxX, _a);
		GrowToEncompass(minY, maxY, _b);
		GrowToEncompass(minZ, maxZ, _c);
	}

	std::cout << "Part 1: " << std::count_if(bots.cbegin(), bots.cend(), [b = bots[maxRangeBot]](const Nanobot& n) { return b.InRange(n); }) << std::endl;
	
	int maxDimension = std::max(maxX - minX, std::max(maxY - minY, maxZ - minZ));
	int sampling = 1;
	while (maxDimension >= 2)
	{
		sampling *= 2;
		maxDimension /= 2;
	}

	// Create initial voxels
	std::vector<Voxel> voxels;
	for (int x = minX; x < maxX; x += sampling)
		for (int y = minY; y < maxY; y += sampling)
			for (int z = minZ; z < maxZ; z += sampling)
			{
				Voxel v(x, x + sampling - 1, y, y + sampling - 1, z, z + sampling - 1);
				for (const Nanobot& bot : bots)
					v.CheckBot(bot);
				voxels.push_back(v);
			}
	std::make_heap(voxels.begin(), voxels.end());

	// Split best voxel until best voxel is only a single point
	while (voxels.cbegin()->scale > 1)
	{
		Voxel bestVoxel = *voxels.begin();
		std::pop_heap(voxels.begin(), voxels.end());
		voxels.pop_back();
		// Split the old voxel
		int newScale = bestVoxel.scale / 2;
		for (int x = bestVoxel.minX; x < bestVoxel.maxX; x += newScale)
			for (int y = bestVoxel.minY; y < bestVoxel.maxY; y += newScale)
				for (int z = bestVoxel.minZ; z < bestVoxel.maxZ; z += newScale)
				{
					Voxel v(x, x + newScale - 1, y, y + newScale - 1, z, z + newScale - 1);
					for (const Nanobot& bot : bots)
						v.CheckBot(bot);
					voxels.push_back(v);
					std::push_heap(voxels.begin(), voxels.end());
				}
	}
	
	std::cout << "Part 2: " << voxels.cbegin()->distance << std::endl;
	return 0;
}