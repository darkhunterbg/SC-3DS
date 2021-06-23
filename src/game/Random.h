#pragma once
#include <stdint.h>

class Random {

private:
	uint32_t seed = 0;

public:
	Random();
	Random(uint32_t seed);
	inline int Next()
	{
		// https://stackoverflow.com/questions/26237419/faster-than-rand
		int i = seed;
		seed = (214013 * seed + 2531011);
		seed = (seed >> 16) & 0x7FFF;
		return i;
	}
	inline int Next(int max) {
		return Next() % max;
	}
	inline int Next(int min, int max) {
		int size = max - min;
		return min + (Next(size));
	}

};