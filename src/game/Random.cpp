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


