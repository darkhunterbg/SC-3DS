#include "Pak.h"
#include "../Debug.h"
#include <cstdio>

void PakLoader::LoadEntries(FILE* f, std::vector<PakFileEntry>& entries)
{
	PakHeader header;

	GAME_ASSERT(fread(&header, sizeof(header), 1, f), "Failed to read pak header!");

	if (header.entries == 0) return;

	entries.reserve(entries.size() + header.entries);

	void* ptr = entries.data() + entries.size();

	entries.resize(entries.capacity());

	GAME_ASSERT(fread(ptr, sizeof(PakFileEntry), header.entries, f) == header.entries, "Failed to read %i pack entries!", header.entries);
}
