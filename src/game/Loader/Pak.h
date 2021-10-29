#pragma once

#include <stdint.h>
#include <vector>
#include <cstdio>

#pragma pack(push,1)

struct PakHeader {
	uint32_t entries;
};

struct PakFileEntry {
	char name[64];
	uint32_t offset;
	uint32_t size;
};

#pragma pack(pop)


class PakLoader {
private:
	PakLoader() = delete;
	~PakLoader() = delete;
public:
	static void LoadEntries(FILE* f, std::vector<PakFileEntry>& entries);
};