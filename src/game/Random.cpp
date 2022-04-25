#include "Random.h"

#include <random>
#include <ctime>

#include "Platform.h"

Random::Random()
{
	srand((unsigned int)time(nullptr));
	seed = std::rand();
	seed = std::rand();
}

Random::Random(uint32_t seed):
	seed(seed)
{
}
int Random::Next()
{
	// https://stackoverflow.com/questions/26237419/faster-than-rand
	int i = seed;
	seed = (214013 * seed + 2531011);
	seed = (seed >> 16) & 0x7FFF;
	return i;
}


